
// PeaksEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PeaksEdge.h"
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
//		PeaksEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CPeaksEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Peaks",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CPeaksEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	long j = 0,i=0;
	_ptr_t2<long> smart64Hist(64);
	long *p64Hist = smart64Hist.ptr;
	ZeroMemory(p64Hist,sizeof(long)*64);

	for(j=0; j<256; j++)
		p64Hist[(int)(j/4)]+=pAprHist[j];
/* Find the first peak */
	j=0;
	for (i=0; i<64; i++)
	  if (p64Hist[i] > p64Hist[j]) j = i;

/* Find the second peak */
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

bool CPeaksEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
