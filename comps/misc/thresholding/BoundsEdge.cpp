// BoundsEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoundsEdge.h"
#include "misc_utils.h"
#include <math.h>

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
//		BoundsEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CBoundsEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"BoundsEdge",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CBoundsEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	long Count = size.cx*size.cy/100;
	_ptr_t2<color> smartTemp(size.cx*size.cy);
	color *pTemp = smartTemp.ptr;
	_ptr_t2<long> smartTempHist(256);
	long *pTempHist = smartTempHist.ptr;

	ZeroMemory(pTemp,sizeof(color)*size.cx*size.cy);
	ZeroMemory(pTempHist,sizeof(long)*256);
	color *pDst = pTemp+size.cx;
	
	for( long y=1;y<size.cy-1;y++ )
	{
		color *pSrc3,*pSrc1,*pSrc2;
		pSource->GetRow( y-1, GetWorkPane() , &pSrc3);
		pSource->GetRow( y, GetWorkPane() , &pSrc1);
		pSource->GetRow( y+1, GetWorkPane() , &pSrc2);
		pDst++;
		for( long x = 1; x < size.cx-1; x++, pSrc1++, pDst++,pSrc2++,pSrc3++  )
		{
			long temp = -4*(*pSrc1)+*(pSrc1+1)+*(pSrc1-1)+*pSrc2+*pSrc3;
			*pDst = temp>=0 ? (color)temp : (color)-temp;
			pTempHist[(*pDst)>>8]++;
		}
		pDst++;
	}

	int i=0;
	long sum=0;
	Count = size.cx*size.cy*15/100;
	for(i=255; sum<Count; i--)
	{
		sum+=pTempHist[i];
	}
	color ColorMax = i<<8;
	long amount=0;
	long Sum=0;
	
	pDst = pTemp+size.cx;
	for( y=1;y<size.cy-1;y++ )
	{
		color* pSrc1;
		pSource->GetRow( y, GetWorkPane(), &pSrc1);
		pDst++;
		for( long x = 1; x < size.cx-1; x++, pSrc1++, pDst++)
		{
			if((*pDst)>ColorMax)
			{
				Sum+=*pSrc1;
				amount++;
			}
			if(amount>=Count)
				break;
			
		}
		pDst++;
	}

	color Edge = (color)(Sum / max( amount, 1 ) );
	return Edge;
}

bool CBoundsEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
