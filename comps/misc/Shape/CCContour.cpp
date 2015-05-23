#include "StdAfx.h"
//#include "image5.h"
#include "cccontour.h"
#include "math.h"

inline int in_product(POINT pt1, POINT pt2)
{
	return pt1.x*pt2.x + pt1.y*pt2.y;
}
inline int cr_product(POINT pt1, POINT pt2)
{
	return pt1.x*pt2.y - pt1.y*pt2.x;
}
inline POINT operator -(POINT p1, POINT p2)
{
	POINT p = {p1.x - p2.x, p1.y - p2.y};
	return p;
}
void CChCodedContour::expand_contour(CArray<double>& out)
{
	int T = out.GetSize();
	out.SetSize(T*3); 
	double* ppoints = out.GetData();
	double* temp = new double[T];
	for(int i=0;i<T;i++)
	{
		temp[i] =ppoints[i]+ 8.;
	}
	memcpy(ppoints + T, temp, T*sizeof(double));
	
	for(int i=0;i<T;i++)
	{
		temp[i] += 8.;
	}
	memcpy(ppoints + T*2, temp, T*sizeof(double));
	delete temp;
}

CChCodedContour::CChCodedContour(IUnknown* object, int nExt/* = 10*/)
{

	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return;


	CImageWrp image;
	IUnknown* unk;
	mo->GetImage(&unk);
	if(unk==0) return;

	image.Attach(unk, false);

	if(!image.IsCountored())
	{
		m_nContourSize = 0;
		return;
	}
	Contour* c = image.GetContour(0);//внешний(главный) контур объекта
	if(c->nContourSize<30) 
	{
		m_nContourSize = 0;
		return;
	}
	if(c==0) return;
	m_nContourSize = c->nContourSize;
	
	nExt = nExt>c->nContourSize? c->nContourSize:nExt;
	/*c = expand_contour(c, nExt);*/
	m_points.SetSize(c->nContourSize);
	//Calculating not normalized chain codes
	m_points[0].s = 0;

	/*int minSum =0, maxSum = 0, sum =0;*/
	POINT ptPrev = {c->ppoints[c->nContourSize-1].x, c->ppoints[c->nContourSize-1].y};
	POINT ptCurr = {c->ppoints[0].x, c->ppoints[0].y};
	POINT ptNext = {c->ppoints[1].x, c->ppoints[1].y};
	
	POINT ptP,ptC; 

	for(int i=1;i < c->nContourSize-1; i++)
	{
		ptP = ptCurr-ptPrev;
		ptC = ptNext - ptCurr;

		m_points[i-1].chainCode = get_angle_code(ptP, ptC);

		ptPrev = ptCurr;
		ptCurr = ptNext;
		ptNext.x = c->ppoints[i+1].x;
		ptNext.y = c->ppoints[i+1].y;
	
	}

	ptPrev = ptCurr;
	ptCurr = ptNext;
	ptNext.x = c->ppoints[0].x;
	ptNext.y = c->ppoints[0].y;

	ptP = ptCurr-ptPrev;
	ptC = ptNext - ptCurr;
	m_points[c->nContourSize-1].chainCode = get_angle_code(ptP, ptC);

	
	m_points[0].chainCode  = get_chain_code(c->ppoints[0], c->ppoints[1]);


	for(int i=1;i < c->nContourSize; i++)
	{
		
		m_points[i].chainCode += m_points[i-1].chainCode;
	}
	
	
	resolve_path();

	gauss(m_graphic ,5);


}
void CChCodedContour::resolve_path()
{
	int n = m_points.GetSize();
	m_points[0].s =0;
	for(int i=1;i < n; i++)
	{
		int c;
		m_points[i].s= m_points[i-1].s;
		if((c = ((int)m_points[i].chainCode)%2) == 0)
		m_points[i].s+=2;
		else m_points[i].s+=3;
	}
}

void CChCodedContour::filter(CArray<double>& arr, int k, int n)
{
	int m = arr.GetSize();
	ASSERT(m%3==0);
	int T = m/3;//период
	double* begin = arr.GetData();
	double* out = new double[m];
//arr содержит 3 периода, поэтому не заботимс€ о пределах свертки
	while(k)
	{
		k--;
		double sum = 0.;
		double* cntr = begin +T -n/2;
		for(int i =0; i<n; i++, cntr++)
		{
			sum+= *cntr;
		}
		cntr = begin+T-n/2;
		for(int i = 0; i< T; i++)
		{
			out[i] = sum/n; 
			sum+= cntr[i+n]-cntr[i];	
		}

		memcpy(begin, out, sizeof(double)*T);
		double* temp = out;
		for(int i=0;i<T;i++)
		{
			temp[i]+=8.;
		}
		memcpy(begin+T, temp,  sizeof(double)*T);
		for(int i=0;i<T;i++)
		{
			temp[i]+=8.;
		}
		memcpy(begin+T+T, temp,  sizeof(double)*T);
		ZeroMemory(out, sizeof(double)*m);
	}
	delete[] out;
}

void CChCodedContour::gauss(CArray<double>& out,
							int k,//кол-во сверток
							int n //"размер" свертки
							)
{
	ASSERT(m_points[0].s != m_points[m_points.GetSize()-1].s);


	int nn = m_points.GetSize();

	for(int i=0;i<nn-1;i++)
	{
		double ch =m_points[i].chainCode;
	
		out.Add(ch);
		if(m_points[i+1].s - m_points[i].s == 3)
		{			
			out.Add(ch);
			ch = m_points[i+1].chainCode;
			out.Add(ch);			
		}
		else
		{

			ch = (m_points[i+1].chainCode + m_points[i].chainCode)/2;
			out.Add(ch);
		}
	}
	if(((int)m_points[nn-1].chainCode)%2 ==0) 
	{
		out.Add((m_points[nn-1].chainCode + m_points[0].chainCode)/2);
	}
	else
	{
		out.Add(m_points[nn-1].chainCode);
		out.Add(m_points[0].chainCode + 8.);
	}
	int T = out.GetSize();
	expand_contour(out);
	if(n ==0)
	{
		n = out.GetSize()/100;
	}

	filter(out, k, n);
	out.SetSize(T+1);
	diff(out);
	out.SetSize(T);
	
	/*{
		CString s ="",temp;
		for(int i=0;i < out.GetSize(); i++)
		{
			temp.Format("%f\n", out.GetAt(i));
			s+=temp;
		}

		CFile file;
		file.Open("E:\\Programs\\Mathlab\\work\\shape.mat", CFile::modeCreate | CFile::modeWrite);
		char* t =s.GetBuffer();
		file.Write(t,s.GetLength());
		file.Close();
	}*/
}
void CChCodedContour::diff(CArray<double>& out)
{
	int T = out.GetSize();
	double* dif = new double[T];
	double* fn = out.GetData();
	for(int i = 0;i<T;i++)
	{
		dif[i] = fn[i+1]-fn[i];
		
	}

	memcpy(fn, dif, T*sizeof(double));
	delete[] dif;
}
void CChCodedContour::conv(CArray<double>* in1, CArray<double>* in2, CArray<double>* out)
{
	int n1 = in1->GetSize();
	int n2 = in2->GetSize();
	int n;
	out->SetSize(n=n1+n2-1);

	//CArray<double> cnv;
	//cnv.SetSize(n2);
	//for(int i =0;i<n2;i++)
	//{
	//	cnv[i] = in2->GetAt(n2-i-1);
	//}

	double* pIn1 = in1->GetData();
	double* pIn2 = in2->GetData();

	double* pOut = out->GetData();
	
	for(int i=0;i<n;i++,pOut++)
	{
		double s =0;
		int m = min(i+1, n2);
		int j = max(0,i-n1);

		for(j;j<m;j++)
		{
			s += pIn1[i-j]*pIn2[n2-j-1];
		}

		(*pOut) = s;
	}
}


int CChCodedContour::get_chain_code(ContourPoint p1, ContourPoint p2)
{
	if(p2.x>p1.x)
	{
		if(p2.y > p1.y) return 1;
		else if(p2.y < p1.y) return 7;
		else return 0;
	}
	else if(p2.x<p1.x)
	{
		if(p2.y > p1.y ) return 3;
		else if(p2.y < p1.y) return 5;
		else return 4;
	}
	else
	{
		if(p2.y > p1.y ) return 2;
		else if(p2.y < p1.y) return 6;
		else return -1;
	}
}
int CChCodedContour::get_angle_code(POINT ptPrev, POINT ptCurr)
{	
	int ret =5;
	int temp;
	if((in_product(ptPrev, ptCurr))==0)//перпендикул€рные векторы
	{
		if(cr_product(ptPrev, ptCurr)>0)
			return 2;
		else
			return -2;
	}
	if(in_product(ptPrev, ptCurr)<0) //-тупой угол
	{
		if((temp = cr_product(ptPrev, ptCurr))==0)
		{
			return 4;
		}
		if(temp >0) return 3;
		return -3;
	}
	if(in_product(ptPrev, ptCurr)>0) //-острый угол
	{
		if((temp = cr_product(ptPrev, ptCurr))>0) return 1;
		else if(temp ==0 ) return 0;
		else return -1;
	}
	return 0;
}
CChCodedContour::~CChCodedContour(void)
{
}

CntrPoint& ContourArray::GetAt(int i)
{
	int index = i;
	if(i<0)
	{
		index = m_nSize+i;

	}
	else if(i>= m_nSize)
	{
		index = i-m_nSize;
	}
	return __super::GetAt(index);
}

CntrPoint& ContourArray::operator[](int i)
{
	
	return GetAt(i);
}

