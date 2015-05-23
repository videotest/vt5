// ColorBalance.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorBalance.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"


#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		ColorBalance
//
//
/////////////////////////////////////////////////////////////////////////////
#define szStringValues "string_valeus"

_ainfo_base::arg	CColorBalanceInfo::s_pargs[] = 
{	
	{szStringValues,	szArgumentTypeString, "", true, false },
	{"Img",				szArgumentTypeImage, 0, true, true },
	{"Result",			szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////
bool CColorBalanceFilter::ParseColor( int *pParams, char *strParams, int nPaneCount)
{
	char *cur_pos=strParams;
	char *stop_pos;
	int nParam;
	for(int i=0; i<nPaneCount; i++)
	{
		while(*cur_pos!=0)
			{
				nParam=strtol(cur_pos,&stop_pos,10);
				if(*cur_pos!=*stop_pos)
					break;
				cur_pos++;
			}
			if(*cur_pos==0)
				return false;
		pParams[i]=nParam;
		cur_pos=stop_pos;
	}
	return true;
}

bool CColorBalanceFilter::InvokeFilter()
{

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	CImagePaneCache paneCache( ptrSrcImage );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}

	int nPaneCount = paneCache.GetPanesCount( );
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	bstr_t bstrArgs(GetArgString( szStringValues ));
	int nStrLength = bstrArgs.length();
	_ptr_t2<char> smartParams(nStrLength);
	char* pstrParams = smartParams.ptr;
	pstrParams=(char*)bstrArgs;

	StartNotification(size.cy,nPaneCount);

	_ptr_t2<int>	ptrColorVals(nPaneCount);
	FillMemory(ptrColorVals,sizeof(color)*nPaneCount,0);
	
	ParseColor( ptrColorVals, GetArgString( szStringValues ), nPaneCount );
	
	for(int nPane=0;nPane<nPaneCount;nPane++)
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		ColorBalance(ptrColorVals[nPane],nPane,size,/*ptrSrcImage*/ &paneCache, ptrDestImage);
		NextNotificationStage();
	}

	FinishNotification();
	return true;
}

bool CColorBalanceFilter::ColorBalance(int nValue, int nPane, SIZE size,/*IImage2 *pSource*/
									   CImagePaneCache *pImageSrcPC, IImage2 *pResult)
{
	if( !pImageSrcPC || !pResult )
		return false;

	IUnknown *punk_image_src = 0;
	pImageSrcPC->GetImage( &punk_image_src );

	IImage2Ptr sptr_source( punk_image_src );
	if( punk_image_src )
		punk_image_src ->Release( ); punk_image_src = 0;

	if( sptr_source == 0 )
		return false;

	byte *pmask;

	bool bEnabled = pImageSrcPC->IsPaneEnable(nPane) && IsPaneEnable(nPane);

	for( long y=0;y<size.cy;y++ )
	{
		sptr_source->GetRowMask( y, &pmask );
		color *pSrc,*pDst;
		sptr_source->GetRow( y, nPane , &pSrc);
		pResult->GetRow( y, nPane , &pDst);
		for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
		{
			if( *pmask==255 )
			{
				if(bEnabled)
				{
					if(*pSrc+nValue>MaxColor)
						*pDst=MaxColor;
					else
					{
						if(*pSrc+nValue<0)
							*pDst=0;
						else
							*pDst=*pSrc+nValue;
						//*pDst = ~*pSrc;
					}
				}
				else
					*pDst=*pSrc;
			}
		}
		Notify( y );
	}

	return true;
}
