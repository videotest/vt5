// IterationEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IterationEdge.h"
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
//		IterationEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CIterationEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Iteration",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CIterationEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	color Tk=0;
	long i;
	__int64 sum1=0,sum2=0;
	for(i=0; i<256; i++)
		sum1+=i*pAprHist[i];
	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	Tk = (color)(sum1/(size.cx*size.cy))<<8;
	double newT=0,temp=0;
	long to=Tk>>8,to2=(Tk>>8)+1;
	while(fabs((double)Tk-temp)>0)
	{
		temp=Tk;
		sum1=0;
		sum2=0;
		for(i=0;i<to;i++)
		{
			sum1+=(i<<8)*pAprHist[i];
			sum2+=pAprHist[i];
		}
		for(i=(to<<8);i<Tk; i++)
		{
			sum1+=i*pHist[i];
			sum2+=pHist[i];
		}
		
		if(sum2)
			newT = (double)(sum1)/(double)(2*sum2);
		
		for(i=Tk;i<(to2<<8); i++)
		{
			sum1+=i*pHist[i];
			sum2+=pHist[i];
		}
		
		for(i=to2;i<256;i++)
		{
			sum1+=(i<<8)*pAprHist[i];
			sum2+=pAprHist[i];
		}

		if(sum2)
			newT += (double)(sum1)/(double)(2*sum2);

		Tk=(color)newT;

	}
	return (color)newT;
}

bool CIterationEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

