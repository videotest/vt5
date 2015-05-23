// MeanEV.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeanEVFilter.h"
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
//		MeanEV
//
//
/////////////////////////////////////////////////////////////////////////////
#define szParam "nParam"
#define szMaskSize "mask_size"

_ainfo_base::arg	CMeanEVInfo::s_pargs[] = 
{	
	{szMaskSize,	szArgumentTypeInt, "20", true, false },
	{szParam,	szArgumentTypeInt, "10000", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CMeanEVFilter::InvokeFilter()
{

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
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

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( ptrSrcImage );
	//int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	int nPaneCount = paneCache.GetPanesCount( );
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	long nMaskSize = (long) (GetArgLong(szMaskSize)/2);
	long nParam = GetArgLong(szParam);
	
	MeanEV(nParam,nMaskSize,nPaneCount,size, /*ptrSrcImage*/ &paneCache, ptrDestImage);

	return true;
}

bool CMeanEVFilter::MeanEV(long nParam, long nMaskSize, int nPaneCount, SIZE size,
						   /*IImage2* pSource*/ CImagePaneCache *pImageSrcPC, IImage2* pResult)
{
	// [vanek] 12.12.2003: CImagePaneCache
	if( !pImageSrcPC || !pResult )
		return false;

	IUnknown *punk_src_image = 0;
	pImageSrcPC->GetImage( &punk_src_image );
	IImage2Ptr sptr_src( punk_src_image );
    if( punk_src_image )
		punk_src_image->Release( ); punk_src_image = 0;

	if( sptr_src == 0 )
        return false;

	byte *pmask=0;
//	_ptr_t<long> smartHist(MaxColor+1);
//	long *pHist = smartHist.ptr;
	long* pHist=0;
	_ptr_t2<long> smartAprHist(256);
	long *pAprHist = smartAprHist.ptr;

//	_ptr_t<long> smartpredRowsHist(MaxColor+1);
//	m_predRowsHist = smartpredRowsHist.ptr;
	
	_ptr_t2<color*> smartRows(size.cy);
	color** pRows = smartRows.ptr;

	_ptr_t2<color*> smartMaskRowsL(2*nMaskSize+1);
	pMaskRowsL = smartMaskRowsL.ptr;
	_ptr_t2<color*> smartMaskRowsR(2*nMaskSize+1);
	pMaskRowsR = smartMaskRowsR.ptr;
		
	int y;
	StartNotification(size.cy,nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++)
	{
		if( !pImageSrcPC->IsPaneEnable( nPane ) ) 
		{			
			continue;
		}

		for(y=0; y<size.cy; y++)
			sptr_src->GetRow(y,nPane,&pRows[y]);

//		bool bEnabled =IsPaneEnable(nPane);
		long nSum,nCount,from,to;
		
		for( y=0;y<size.cy;y++ )
		{
			sptr_src->GetRowMask( y, &pmask );
			color *pDst=0,*pSrc=0;
			pResult->GetRow( y, nPane , &pDst);
			pSrc=pRows[y];
			InitHist(y,size,nPane,nMaskSize,nParam,pRows,pHist,pAprHist);
		
			for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
			{
				if( *pmask>=128 )
				{
					nSum=0;
					nCount=0;
					from=max(0,(*pSrc-nParam)>>8);
					to=min(255,(*pSrc+nParam)>>8);
		//			if(to!=from)
					for(int k=from; k<=to; k++)
					{
						nSum+=k*pAprHist[k];
						nCount+=pAprHist[k];
					}
		/*				else
						{
							nSum+=(*pSrc)*pHist[*pSrc];
							nCount+=pHist[*pSrc];

						}
/*						
						int i = *pSrc - nParam;
						if (i<0) i = 0;
						int n = ( i + 255 ) & 0xFF00;
						for ( ; i < n; i++ )
						{
							if(pHist[i])
							{
								nSum += i * pHist[i];
								nCount += pHist[i];
							}
						} 

						n = *pSrc + nParam;
						if (n>0x10000) n = 0x10000;
						i = n == 0x10000 ? 0xFF00 : n & 0xFF00;
						for ( ; i < n; i++ )
						{
							if(pHist[i])
							{
								nSum += i * pHist[i];
								nCount += pHist[i];
							}
						}

*/
					*pDst=((color)(nSum/nCount))<<8;
				}

				MoveHist(y,x,size,nPane,nMaskSize,nParam,pRows,pHist,pAprHist);
			}
			Notify( y );
		}
		NextNotificationStage();
	}
	FinishNotification();

	return true;
}

bool CMeanEVFilter::InitHist(int y, SIZE size, int nPane, int nMaskSize, int nParam, color **pRows, long *pHist ,long *pAprHist)
{
	ZeroMemory(pMaskRowsL,sizeof(color*)*(2*nMaskSize+1));
	ZeroMemory(pMaskRowsR,sizeof(color*)*(2*nMaskSize+1));
	ZeroMemory(pAprHist,sizeof(long)*256);
	int to=min(size.cy-1,y+nMaskSize);
	int k=0;
	int to1=min(nMaskSize,size.cx-1);
	color* pCurColor=0;
	for( long j=max(0,y-nMaskSize); j<=to; j++)
	{
		pCurColor=pRows[j];
		for(long x=0; x<=to1; x++,pCurColor++)
		{
			long index2 = (*pCurColor)>>8;
			pAprHist[index2]++;
		}

		pMaskRowsL[k] = pRows[j];
		pMaskRowsR[k] = pCurColor;
		k++;
	}
	return true;
}

bool CMeanEVFilter::MoveHist(int y, int x, SIZE size, int nPane, int nMaskSize, int nParam, color **pRows, long *pHist ,long *pAprHist)
{
	long j=0;
	if(x+nMaskSize<size.cx)   // Add right
	{ 
		for(j=0; j<2*nMaskSize+1; j++)
		{ 
			if(pMaskRowsR[j]!=0)
			{
				pAprHist[*(pMaskRowsR[j])>>8]++;
				pMaskRowsR[j]++;
			}
		} 
	}
	if(x-nMaskSize>=0)   // Remove left
	{
		for(j=0; j<2*nMaskSize+1; j++)
		{ 
			if(pMaskRowsL[j]!=0)
			{
				pAprHist[*(pMaskRowsL[j])>>8]--;
				pMaskRowsL[j]++;
			}
		} 
	}
	return true;
}
