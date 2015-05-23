#include "StdAfx.h"
#include "parzenwindows.h"
#include "morphograph.h"
#include "math.h"

ParzenWindows::ParzenWindows(void)
{
	m_nN =100;
	calc_gauss();
	m_h =.5;
}


ParzenWindows::~ParzenWindows(void)
{
}

void ParzenWindows::calc_gauss()
{
	double p = 1./sqrt(2.*PI);
	double s = 0.0;
	m_gauss.RemoveAll();
	for(double i=0.;;i++)
	{
		double g = p*exp(-i*i/2.);  
		m_gauss.AddTail(g);
		s+=g;
		if(g<.001) break;
	}
	/*POSITION pos = m_gauss.GetHeadPosition(); 
	while(pos) 
	{
		POSITION p = pos;
		double g = m_gauss.GetNext(pos);
		g/=s;
		m_gauss.SetAt(p, g);
	}*/
}

double ParzenWindows::EstimateDescriptor(const double d, int key)
{
	int n = m_params.GetSize();

	ParamEst* pe;	
	for(int i=0;i<n;i++)
	{
		pe = &m_params.GetAt(i);
		if(pe->Key()== key) break;
		pe=0;
	}
	if(pe==0) return 0.;

	n = pe->GetSize();
	int k = (d - pe->Min())/pe->Weight();
	if(k<0) return 0.;
	if(k>=n) return 0.;
	return pe->GetAt(k).value;
}

//void ParzenWindows::to_mat(ParamEst& pe, int n)
//{
//
//	PRZPT* prz1 = &pe.GetAt(0);
//	CString s ="",temp;
//	double weight = pe.Weight();
//	double dMin = pe.Min();
//	for(int i =0;i<n;i++)
//	{
//		temp.Format("%.6f %.6f\n", (*prz1).value, dMin+weight*i);
//		s+=temp; 
//		prz1++;
//	}
//
//	CFile file;
//	file.Open("E:\\Programs\\Mathlab\\work\\shape.mat", CFile::modeCreate | CFile::modeWrite);
//	char* t =s.GetBuffer();
//	file.Write(t,s.GetLength());
//	file.Close();
//
//
//}


void ParzenWindows::AttachDescriptors(const CArray<CStringX>& descr)
{
	m_nN = descr.GetSize();
	ASSERT(m_nN);
	CStringX sx = descr.GetAt(0);
	m_nD = sx.GlobalParameters().GetSize();

	calc_gauss();

	m_params.SetSize(m_nD);
	int ext = m_nN/20+1;
	for(int i=0;i<m_nD; i++)
	{
		m_params.GetAt(i).SetSize(m_nN + ext*2/**m_gauss.GetCount()*/);
	}

	for(int j=0; j<m_nD; j++)
	{
		CStringX s = descr.GetAt(0);
		ParamEst& pe = m_params.GetAt(j);
		pe.Min() = s.GlobalParameters().GetAt(j).value;
		pe.Key() = s.GlobalParameters().GetAt(j).key;
		double dMax = pe.Min();
		for(i=1; i<m_nN; i++)
		{
			CStringX sx = descr.GetAt(i);
			double d = sx.GlobalParameters().GetAt(j).value;
			if(d>dMax) dMax = d;
			if(d<pe.Min()) pe.Min() = d;
		}

		int N = m_nN+2*ext/**m_gauss.GetCount()*/; 
		pe.Weight() = (dMax-pe.Min())/(double)m_nN;
		dMax = dMax + /*m_gauss.GetCount()**/ext*pe.Weight();
		pe.Min() = pe.Min() - /*m_gauss.GetCount()**/ext*pe.Weight();

		for(i=0; i<m_nN; i++) 
		{
			CStringX sx = descr.GetAt(i);
			double d = sx.GlobalParameters().GetAt(j).value;
			int n = (d-pe.Min())/pe.Weight();
			PRZPT* prz  = &(pe.GetAt(n));
			POSITION pos = m_gauss.GetHeadPosition();
			for(int j =0;pos ;j++)
			{
				double g = m_gauss.GetNext(pos);
				if(j+n>=N || n-j<0) continue; 
				prz[j].value+= g;
				if(j) prz[-j].value += g;
			}

		}
		
		PRZPT* prz1  = &(m_params.GetAt(j).GetAt(0));
		for(i=0;i<N;i++,prz1++)
		{
			prz1->value/=(dMax-pe.Min());
		}
		/*to_mat(m_params.GetAt(j),N);*/
		
	}


}