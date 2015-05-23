// ThresholdFilterBase.cpp: implementation of the CThresholdFilterBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThresholdFilterBase.h"
#include "misc_utils.h"

#include "core5.h"

#define MaxColor color(-1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThresholdFilterBase::CThresholdFilterBase()
{
	m_nWorkPane = -1;
}

CThresholdFilterBase::~CThresholdFilterBase()
{

}

HRESULT CThresholdFilterBase::DoInvoke()
{
	// [vanek]: чтение номера паны, с которой надо работать
    m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 
	return	__super::DoInvoke(); 
}

bool CThresholdFilterBase::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	if( !TestImage( ptrSrcImage ) )
		return false;

	IBinaryImagePtr	ptrBin = GetDataResult();
			
	if( ptrSrcImage == NULL || ptrBin==NULL )
		return false;

	SIZE size;
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	ptrBin->CreateNew(size.cx,size.cy);
	POINT point;
	ptrSrcImage->GetOffset(&point);
	ptrBin->SetOffset(point,true);

	_ptr_t2<long> smartHist(MaxColor+1);
	long *pHist = smartHist.ptr;
	_ptr_t2<long> smartAprHist(256);
	long *pAprHist = smartAprHist.ptr;

	CreateHistogram(ptrSrcImage,GetWorkPane( ),pHist,pAprHist);
	m_Edge=SelectEdge(ptrSrcImage,pHist,pAprHist);
	byte *pmask=0;

	try
	{
		StartNotification( size.cy, 1 );

		for( long y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc;
			ptrSrcImage->GetRow(y , GetWorkPane( ), &pSrc);
			byte* pDst;

			ptrBin->GetRow(&pDst,y,0);

			for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
			{
				if( *pmask==255  )
				{
					if(*pSrc>=m_Edge)
						*pDst=0xFF;
					else
						*pDst = 0;
				}
				else
					*pDst = 0;
			}			

			Notify( y );
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

bool CThresholdFilterBase::CreateHistogram(IImage3* pImgSrc, int nPane, long* pHist ,long *pAprHist)
{
	if(!pImgSrc) return false;
	if(!pHist) return false;

	SIZE size;
	pImgSrc->GetSize((int*)&size.cx,(int*)&size.cy);
	
	ZeroMemory(pHist,(MaxColor+1)*sizeof(long));
	ZeroMemory(pAprHist,256*sizeof(long));

	for( long y=0;y<size.cy;y++ )
	{
		color* pSrc;
		pImgSrc->GetRow(y, nPane ,&pSrc);
		byte* pmask;
		pImgSrc->GetRowMask(y,&pmask); 
		for( long x = 0; x < size.cx; x++, pmask++, pSrc++)
		{
			if(*pmask==255)
			{
				pHist[*pSrc]++;
				pAprHist[((*pSrc)>>8)]++;
			}
		}			 

	}
	return true;
}

color CThresholdFilterBase::SelectEdge(IImage3 *pSrc, long *pHist , long *pAprHist)
{
	return 0;
}

bool	CThresholdFilterBase::TestImage( IImage3 *pimage )
{
	if( !pimage )
		return false;
    
	int nPaneCount = ::GetImagePaneCount( pimage );

	if( (nPaneCount > 1) && IsWorksWithGreyscaleOnly( ) )
	{
		HWND hwnd;
		IApplicationPtr	ptrA( App::application() );
		ptrA->GetMainWindowHandle(&hwnd);
		char sz[256]; sz[0]=0;
		int loaded = ::LoadString( App::handle(), IDS_IMAGE_HAVE_TO_BE_GREYSCALE, sz, 254 );
		MessageBox(hwnd,sz,"VT5 error",MB_OK);
		ptrA->Release();
		return false;
	}    

	return true;
}