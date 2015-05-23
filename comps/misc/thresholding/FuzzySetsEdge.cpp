// FuzzySetsEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FuzzySetsEdge.h"
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
//		FuzzySetsEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CFuzzySetsEdgeInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"FuzzySets",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


color CFuzzySetsEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	_ptr_t2<double> smartM0(256);
	m_M0 = smartM0.ptr;
	_ptr_t2<double> smartM1(256);
	m_M1 = smartM1.ptr;
	int i,t;
	pSource->GetSize((int*)&m_size.cx,(int*)&m_size.cy);
	m_pAprHist = pAprHist;
	for(i=0;i<256 && pAprHist[i]==0;i++);
	for(t=255;t>0 && pAprHist[t]==0;t--);
//	m_C=t-i;
	m_C=255;
	__int64 sum1=0,sum2=0;
	color T;
	for(i=0; i<256; i++)
	{
		sum1+=i*pAprHist[i];
		sum2+=pAprHist[i];
		m_M0[i] = sum2 != 0 ? (double)sum1/sum2 : 0;
	}
	sum1=0;
	sum2=0;
	for(i=256-1; i>=0; i--)
	{
		sum1+=i*pAprHist[i];
		sum2+=pAprHist[i];
		m_M1[i] = sum2 != 0 ? (double)sum1/sum2 : 0;
	}

	double MinF=F(0);
	T=0;
	double MaxF=MinF;
	for(t=1; t<256; t++)
	{
		double temp = F(t);
		if(temp<MinF)
		{
			MinF=temp;
			T=t;
		}
		if(temp>MaxF)
			MaxF=temp;
	}
	double Delta = MinF + (MaxF-MinF)*0.05;
	color Lmax = T+Delta < 255 ? (color)(T + Delta) : 254;
	color Lmin = T-Delta>1 ? (color)(T - Delta) : 1;
	long h_min = 1000000000;
	for(i=Lmin; i<=Lmax; i++)
	{
		long sum = pAprHist[i-1]+pAprHist[i]+pAprHist[i+1];
		if(sum<h_min)
		{
			h_min = sum;
			T = i;
		}
	}

	return (T<<8);
}

double CFuzzySetsEdgeFilter::F(color t)
{
	if(false)
	{
		double sum=0;
		for (int g=0; g<256; g++)
		{
			double temp=m(g,t);
			sum+=(temp*(1-temp)) * (temp-(1-temp));
		}
		return sqrt(sum);
	}
	else
	{
		double sum = 0;
		for (int g=0; g<256; g++)
			sum+=Hf(m(g,t))*m_pAprHist[g];
		return sum/(m_size.cx*m_size.cy);
	}
	return 0;
	
}

double CFuzzySetsEdgeFilter::m(long g, long t)
{
	double m;
	if(g<t)
		m=1/(1 + fabs(g-m_M0[t])/m_C);
	else
		m=1/(1 + fabs(g-m_M1[t])/m_C);
	return m;
}

double CFuzzySetsEdgeFilter::Hf(double i)
{
	if(i<1 && i>0)
		return -i*log10(i)-(1-i)*log10(1-i);
	return 0;
}


bool CFuzzySetsEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

