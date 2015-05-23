// BinaryMinimumEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BinaryMinimum.h"
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
//		BinaryMinimumEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CBinaryMinimumEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"BinaryMinimum",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CBinaryMinimumEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	int i=0,j=0;
	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	double PixTotal = size.cx*size.cy;
	_ptr_t2<double> smartP1(256);
	double *pP1 = smartP1.ptr;
	_ptr_t2<double> smartMju1(256);
	double *pMju1 = smartMju1.ptr;
	_ptr_t2<double> smartMju2(256);
	double *pMju2 = smartMju2.ptr;

	double pSigma1=0;
	double pSigma2=0;
	double Jcur=0,Jmin=1000000000;
	color T=0;

	double sum1=0,sum2=0,sum3=0,sum4=0,sum5=0;
	for(i=0; i<256; i++)
	{
		
		sum1+=pAprHist[i];
		pP1[i] = sum1;

		if(pP1[i])
		{
			sum2+=pAprHist[i]*i;
			pMju1[i] = sum2/pP1[i];
		}
		else
			pMju1[i]=0;

		if(PixTotal-pP1[i])
		{
			sum3+=pAprHist[255-i]*(255-i);
			pMju2[i] = sum3/(PixTotal-pP1[i]);
		}
		else
			pMju2[i]=0;

	}

	for(i=0; i<256; i++)
	{
		sum4=0;
		if(pP1[i])
		{
			for(j=0; j<=i; j++)
				sum4+=pAprHist[j]*(j-pMju1[i])*(j-pMju1[i]);
			pSigma1=sum4/pP1[i];
		}
		else
			pSigma1=0;

		sum5=0;
		if(PixTotal-pP1[i])
		{
			for(j=i+1; j<256; j++)
				sum5+=pAprHist[j]*(j-pMju2[i])*(j-pMju2[i]);
			pSigma2=sum5/(PixTotal-pP1[i]);
		}
		else
			pSigma2=0;

		if(pSigma1 && pSigma2 && pP1[i] && (PixTotal-pP1[i]))
			Jcur = 1+2*( pP1[i]*log10(pSigma1)+(PixTotal-pP1[i])*log10(pSigma2) ) - 2*( pP1[i]*log10(pP1[i])+(PixTotal-pP1[i])*log10(PixTotal-pP1[i]) );
		else
			Jcur = 100000000;

		if(Jcur<Jmin)
		{
			Jmin = Jcur;
			T =i;
		}
	}

	return (T<<8);
}

bool CBinaryMinimumEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
