// MeanKNV.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "MeanKNV.h"
#include "ImagePane.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		MeanKNV
//
//
/////////////////////////////////////////////////////////////////////////////
#define szParam "nParam"
#define szMaskSize "nMaskSize"

_ainfo_base::arg	CMeanKNVInfo::s_pargs[] = 
{	
	{szMaskSize,szArgumentTypeInt, "20", true, false },
	{szParam,	szArgumentTypeInt, "100", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CMeanKNVFilter::InvokeFilter()
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

	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	long nParam = GetArgLong(szParam);
	long nMaskSize = (long) (GetArgLong(szMaskSize)/2);
	
	MeanKNV(nParam,nMaskSize,nPaneCount,size,ptrSrcImage,ptrDestImage);

	return true;
}

bool CMeanKNVFilter::MeanKNV(long nParam, long nMaskSize, int nPaneCount, SIZE size,IImage2* pSource,IImage2* pResult)
{
		if( !pSource || !pResult )
		return false;

	byte *pmask=0;
//	_ptr_t<long> smartHist(MaxColor+1);
//	long *pHist = smartHist.ptr;
	long *pHist=0;
	_ptr_t2<long> smartAprHist(256);
	long *pAprHist = smartAprHist.ptr;

	_ptr_t2<color*> smartRows(size.cy);
	color** pRows = smartRows.ptr;

	_ptr_t2<color*> smartMaskRowsL(2*nMaskSize+1);
	pMaskRowsL = smartMaskRowsL.ptr;
	_ptr_t2<color*> smartMaskRowsR(2*nMaskSize+1);
	pMaskRowsR = smartMaskRowsR.ptr;
		
	int y=0;
	CImagePaneCache paneCache( pSource );
	StartNotification(size.cy,nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++)
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}

		for(y=0; y<size.cy; y++)
			pSource->GetRow(y,nPane,&pRows[y]);

		long bAprPoint=0 ;
		long nSum=0;
		long nCount=0;
		long il=0,ir=0;


		for( y=0;y<size.cy;y++ )
		{
			pSource->GetRowMask( y, &pmask );
			color *pDst=0,*pSrc=0;
			pResult->GetRow( y, nPane , &pDst);
			pSrc=pRows[y];
			InitHist(y,size,nPane,nMaskSize,nParam,pRows,pHist,pAprHist);
		
			for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
			{
				if( *pmask==255 )
				{
					bAprPoint = (*pSrc)>>8;
//					__int64 nSum = (*pSrc)*pHist[*pSrc];
						
///					long nCount=pHist[*pSrc];
						
					//long il=max(*pSrc-1,0);
					//long ir=min(*pSrc+1,MaxColor);

//					if (pAprHist[bAprPoint]<=nParam)
					{
						nCount = pAprHist [bAprPoint];
						nSum = pAprHist[bAprPoint]*(bAprPoint);
						il = bAprPoint;
						ir = bAprPoint;
						while (nCount < nParam)
						{
							il--;
							if (il>=0 && pAprHist[il])
							{
								nCount += pAprHist[il];
								nSum += pAprHist[il]*il;
							} 
							ir++;
							//ir = bAprPoint + 1;
							if (ir<256 && pAprHist[ir])
							{
								nCount += pAprHist[ir];
								nSum += pAprHist[ir]*ir;
							} 
							if (il<0&&ir>255)
							 break;
						}
						//il=il<<8;
						//ir=ir<<8; 
/*						// Уточнение
						while (nCount > nParam)
						{
							il++;
							if (il>=0)
							{
								nCount -= pHist[il];
								nSum -= pHist[il]*il;
							} 
							ir--;
							//ir = bAprPoint + 1;
							if (ir<MaxColor)
							{
								nCount -= pHist[ir];
								nSum -= pHist[ir]*ir;
							} 
							if (il==ir)
							 break;
						}
*/						
					}
/*
						while (nCount < nParam)
						  {
							 if (il >= 0 && pHist[il])
							 {
							   long lCur = pHist[il];
							   nCount += lCur;
							   nSum += il*lCur; 
							   il--;
							 }      
							 if (ir < MaxColor && pHist[ir])
							 {
							   long lCur = pHist[ir];
							   nCount += lCur;
							   nSum += ir*lCur; 
							   ir++;
							 }
							 if (il<0&&ir>=MaxColor)
								 break;
						  }
*/					*pDst=((color)(nSum/nCount))<<8;

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

bool CMeanKNVFilter::InitHist(int y,SIZE size,int nPane,int nMaskSize,int nParam,color** pRows,long* pHist,long *pAprHist)
{
	ZeroMemory(pMaskRowsL,sizeof(color*)*(2*nMaskSize+1));
	ZeroMemory(pMaskRowsR,sizeof(color*)*(2*nMaskSize+1));
	ZeroMemory(pAprHist,sizeof(long)*256);
	int to=min(size.cy-1,y+nMaskSize);
	int k=0;
	int to1=min(nMaskSize,size.cx-1);
	for( long j=max(0,y-nMaskSize); j<=to; j++)
	{
		color* pCurColor=pRows[j];
		for(long x=0; x<=to1; x++,pCurColor++)
		{
//			long index1 = *pCurColor;
			long index2 = (*pCurColor)>>8;
//			pHist[index1]++;
			pAprHist[index2]++;
//			m_predRowsHist[index1]++;
		}

		pMaskRowsL[k] = pRows[j];
		pMaskRowsR[k] = pCurColor;
		k++;
	}
	return true;
}

bool CMeanKNVFilter::MoveHist(int y, int x, SIZE size, int nPane, int nMaskSize, int nParam, color **pRows, long *pHist ,long *pAprHist)
{
	long j=0;
	if(x+nMaskSize<size.cx)   // Add right
	{ 
		for(j=0; j<2*nMaskSize+1; j++)
		{ 
			if(pMaskRowsR[j]!=0)
			{
//				pHist[*(pMaskRowsR[j])]++;
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
//				pHist[*(pMaskRowsL[j])]--;
				pAprHist[*(pMaskRowsL[j])>>8]--;
				pMaskRowsL[j]++;
			}
		} 
	}
	
	return true;
}

