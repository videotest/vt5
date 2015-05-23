// DoubleEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DoubleEdge.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "clone.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		DoubleEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CDoubleEdgeInfo::s_pargs[] = 
{
	{"Tmin",		szArgumentTypeInt, "30000", true, false },
	{"Tmax",		szArgumentTypeInt, "50000", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


bool CDoubleEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////


bool CDoubleEdgeFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	if(nPaneCount>1)
		return false;

	IBinaryImagePtr	ptrBin = GetDataResult();
			
	if( ptrSrcImage == NULL || ptrBin==NULL )
		return false;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	ptrBin->CreateNew(size.cx,size.cy);
	POINT point;
	ptrSrcImage->GetOffset(&point);
	ptrBin->SetOffset(point,true);

	_ptr_t2<CConnectedArea> smartAreas(size.cx*size.cy);
	CConnectedArea *areas=smartAreas.ptr;

	_ptr_t2<color> smartImageA1(size.cx*size.cy);
	color* pImageA1=smartImageA1.ptr;
	_ptr_t2<long> smartMap(size.cx*size.cy);
	long *pMap=smartMap.ptr;
	_ptr_t2<POINT> smartPoints(size.cx*size.cy);
	POINT *pPoints=smartPoints.ptr;

	//	m_Edge=SelectEdge(ptrSrcImage,pHist,pAprHist);

	double *curP=0,*curQ=0;

	color Tmin = (color)GetArgLong( "Tmin" );
	color Tmax = (color)GetArgLong( "Tmax" );
	double NotifyCount = 0;
	byte *pmask=0;

	try
	{
		StartNotification( size.cy, 3 );
		long x,y;
		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc;
			ptrSrcImage->GetRow(y ,0 , &pSrc);
			byte* pDst;
			ptrBin->GetRow(&pDst,y,0);
			color* pSrcA1;
			pSrcA1 = pImageA1 + y*size.cx;

			for( x = 0; x < size.cx; x++, pmask++, pSrc++, pSrcA1++ , pDst++)
			{
				if(*pSrc>Tmin)
					*pSrcA1 = 0xFF;
				else
					*pSrcA1 = 0;

				if(*pSrc>Tmax)
					*pDst = 0xFF;
				else
					*pDst = 0;
			}
			Notify(y);
		}
		NextNotificationStage();

// =========== Create Connectead Areas ===========================			
		FillMemory(pMap,size.cx*size.cy*sizeof(long),-1);
		CreateConnectedArea2(0,pMap,ptrBin,pImageA1,pPoints,areas,0);
		
		long *pMapCur;
		pMapCur=pMap;
		for( y=0;y<size.cy;y++ )
		{
			byte* pDst;
			ptrBin->GetRow(&pDst,y,0);
			
			for( x = 0; x < size.cx; x++, pMapCur++, pDst++)
			{
				if(areas[*pMapCur].m_Deleted==true)
					*pDst = 0;
			}
			Notify(y);
		}


		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

/*
color CDoubleEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	long j = 0,i=0;
	_ptr_t<long> smart64Hist(64);
	long *p64Hist = smart64Hist.ptr;
	ZeroMemory(p64Hist,sizeof(long)*64);

	for(j=0; j<256; j++)
		p64Hist[(int)(j/4)]+=pAprHist[j];
// Find the first peak 
	for (i=0; i<64; i++)
	  if (p64Hist[i] > p64Hist[j]) j = i;

// Find the second peak 
	long k = 0;
	for (i=1; i<64; i++)
	  if (p64Hist[i-1]<=p64Hist[i] && p64Hist[i+1]<=p64Hist[i])
	    if ((k-j)*(k-j)*p64Hist[k] < (i-j)*(i-j)*p64Hist[i])
			k = i;

	color T = (color) j;
	if (j<k)
	{
	  for (i=j; i<k; i++)
		  if (p64Hist[i] < p64Hist[T])
			  T = (color)i;
	}
	else
	{
	  for (i=k; i<j; i++)
	    if (p64Hist[i] < p64Hist[T])
			T = (color)i;
	}

	return (T<<10);
}
*/

