// AutoContrast.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoContrast.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		AutoContrast
//
//
/////////////////////////////////////////////////////////////////////////////
#define szStringValues1 "string_values1"
#define szStringValues2 "string_values2"

_ainfo_base::arg	CAutoContrastInfo::s_pargs[] = 
{	
	{szStringValues1,	szArgumentTypeString, "", true, false },
	{szStringValues2,	szArgumentTypeString, "", true, false },
	{"Img",				szArgumentTypeImage, 0, true, true },
	{"Result",			szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CAutoContrastFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;

	//create compatible image
	ICompatibleObjectPtr ptrCO( ptrDestImage );
	CImagePaneCache paneCache( ptrSrcImage );
	
	if( ptrCO == NULL )
		return false;

	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
	{			
		return false;
	}		

	//get image information
	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	SIZE size;
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	bstr_t bstrArgs1(GetArgString( szStringValues1 ));
	int nStrLength1 = bstrArgs1.length();
	_ptr_t2<char> smartParams1(nStrLength1);
	char* pstrParams1 = smartParams1.ptr;
	pstrParams1=(char*)bstrArgs1;

	bstr_t bstrArgs2(GetArgString( szStringValues2 ));
	int nStrLength2 = bstrArgs2.length();
	_ptr_t2<char> smartParams2(nStrLength2);
	char* pstrParams2 = smartParams2.ptr;
	pstrParams2=(char*)bstrArgs2;

	int nPane=0;

	StartNotification(size.cy,nPaneCount);

	_ptr_t2<int>	ptrColorVals1(nPaneCount);
	FillMemory(ptrColorVals1,sizeof(color)*nPaneCount,0);
	_ptr_t2<int>	ptrColorVals2(nPaneCount);
	FillMemory(ptrColorVals2,sizeof(color)*nPaneCount,MaxColor);

	ParseColor( ptrColorVals1, GetArgString( szStringValues1 ), nPaneCount );
	ParseColor( ptrColorVals2, GetArgString( szStringValues2 ), nPaneCount );

	for(nPane=0;nPane<nPaneCount;nPane++)
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		AutoContrast(ptrColorVals1[nPane],ptrColorVals2[nPane],nPane,size,/*ptrSrcImage*/ &paneCache, ptrDestImage);
		NextNotificationStage();
	}

	FinishNotification();
	return true;
}

bool CAutoContrastFilter::ParseColor( int *pParams, char *strParams, int nPaneCount)
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

// bool CAutoContrastFilter::AutoContrast(int nValue1,int nValue2, int nPane, SIZE size,IImage2 *pSource, IImage2 *pResult)
bool CAutoContrastFilter::AutoContrast(int nValue1,int nValue2, int nPane, SIZE size,CImagePaneCache* pImageSrcPC,
									   IImage2 *pResult)
{
	// [vanek] 12.12.2003
	/*if( !pSource || !pResult )
		return false;*/

	if( !pImageSrcPC || !pResult )
		return false;

	IUnknown	*punk_src_image = 0;

	pImageSrcPC->GetImage( &punk_src_image );
	    
	IImage2Ptr	sptr_source( punk_src_image );

	if( punk_src_image )
		punk_src_image->Release( ); punk_src_image = 0;

	if( sptr_source == 0 )
		return false;


	byte *pmask;
	double dKoef= (double)MaxColor/(nValue2-nValue1);

	//bool	bEnabled = IsPaneEnable( nPane );
	bool	bEnabled = pImageSrcPC->IsPaneEnable( nPane ) && IsPaneEnable( nPane );

	for( long y=0;y<size.cy;y++ )
	{
		//pSource->GetRowMask( y, &pmask );
		sptr_source->GetRowMask( y, &pmask );
		color *pSrc,*pDst;
		//pSource->GetRow( y, nPane , &pSrc);
		sptr_source->GetRow( y, nPane , &pSrc);
		pResult->GetRow( y, nPane , &pDst);
		for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
		{
			if( *pmask )
			{
				if( bEnabled )
					*pDst=(color)max( 0, min( MaxColor, ((*pSrc-nValue1)*dKoef) ) );
				else
					*pDst=*pSrc;
			}
		}
		Notify( y );
	}

	return true;
}
