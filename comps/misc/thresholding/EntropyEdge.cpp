// EntropyEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EntropyEdge.h"
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
//		EntropyEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CEntropyEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Entropy",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CEntropyEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	_ptr_t2<double> smartP(256);
	double *pP=smartP.ptr;
	int i,t;
	__int64 PixTotal=0;

	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	PixTotal=size.cx*size.cy;
	for(i=0; i<256; i++)
	{
		pP[i]=(double)(pAprHist[i])/(double)(PixTotal);
	}
	double min=10000000;
		color minT=0;
	double Cur=0;
	double Sb=0,Sw=0;
	double sum=0;

	for(t=1; t<256; t++)
	{
		Sb=0;
		Sw=0;
		sum=0;
		for(i=0; i<t; i++)
			sum+=pP[i];
		if(sum && sum!=pP[t] && pP[t])
			Sb=log10(sum)+(E(pP[t])+E(sum-pP[t]))/sum;
		else
			Sb=1000000;
	
		sum=0;
		for(i=t; i<256; i++)
			sum+=pP[i];
		if(sum && sum!=pP[t] && pP[t])
			Sw=log10(sum)+(E(pP[t])+E(sum-pP[t]))/sum;
		else
			Sw=1000000;

		Cur = Sb + Sw;
			
		if(Cur<min)
		{
			min=Cur;
			minT=t;
		}

	}

	return (minT<<8);
}

bool CEntropyEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

double CEntropyEdgeFilter::E(double X)
{
	return -X*log10(X);
}
