#include "StdAfx.h"
#include "morphograph.h"
#include "classstorage.h"

#define abs(x) ((x)>=0?(x):-(x))
#define PEAK 2.5





CStringX CurveDescr::GenerateString()
{
	m_strDescr.RemoveAll();
	
	POSITION pos = GetHeadPosition();
	double s =0.;
	double size =0.;
	TCHAR ch; 
	while(pos)
	{
		
		CURVDESCR desc = GetNext(pos);
		ch = GetLetter(desc.value);
		size += desc.size/1000.;
		while(s<=size)
		{
			m_strDescr+=ch;
			s+=.05;
		}
		
	}
	while(m_strDescr.GetCount()<20) m_strDescr+=ch;
	ASSERT(m_strDescr.GetCount()==20);
	return m_strDescr;
}
static CString SimpledString(CString& str)
{
	CString rs;
	TCHAR* s = str.GetBuffer();
	TCHAR prev = *s;	
	rs+=prev;
	s++;
	while(*s)
	{
		if(*s == prev) 
		{
			s++;
			continue;
		}
		prev=*s;
		rs+=*s;
		s++;
	}
	return rs;
}
CMorphoGraph::CMorphoGraph(void)
{

}

CMorphoGraph::~CMorphoGraph(void)
{
}
void CMorphoGraph::GetDescriptor(CurveDescr* cd)
{
	POSITION pos = m_desc.GetHeadPosition();
	while(pos)
	{
		CURVDESCR desc = m_desc.GetNext(pos);
		cd->AddTail(desc);
	}
	cd->m_strDescr = m_desc.m_strDescr;
	cd->m_strClass =  m_desc.m_strClass;
	cd->m_nClass =  m_desc.m_nClass;	
}
void CMorphoGraph::Init(double* graphic, int n,int x, int y, int sence)
{
	m_curv.Create(x,y); 
	m_curv.Attach(graphic, n);

	m_curv.GetDescriptor(m_desc);
	
}

void CMorphoGraph::WriteToTextFile(CString& strDes, CString& path)
{
	CFile file;
	file.Open(path, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate); 
	CString s = strDes + "\r\n";
	char* c = s.GetBuffer();
	file.SeekToEnd();
	try{
	file.Write(c, s.GetLength());
	}catch(CException* e)
	{
		TCHAR s[1024];
		e->GetErrorMessage((LPTSTR)&s,1024);
		s[1023] ='\n';
		OutputDebugString(s);
	}	
}


void CCurvatureBin::sub(int* result, int* pts1, int* pts2, int n)
{
	for(int i=0; i<n; i++, result++, pts1++, pts2++)
	{
		*result = *pts1 - *pts2;
	}
}

CCurvatureBin& CCurvatureBin::operator=(const CCurvatureBin& cb)
{
	//delete[] m_image;
	delete[] m_points;
	delete[] m_plus;
	delete[] m_minus;

	m_se = cb.m_se;
	Create(cb.m_cx, cb.m_cy*2);
	m_points = new int[cb.m_cx];
	memcpy(m_points, cb.m_points, sizeof(int)*m_cx);

	m_plus = new int[cb.m_cx];
	memcpy(m_plus, cb.m_plus, sizeof(int)*m_cx);

	m_minus = new int[cb.m_cx];
	memcpy(m_minus, cb.m_minus, sizeof(int)*m_cx);

	m_weightY = cb.m_weightY, m_weightX= cb.m_weightX;
	m_min = cb.m_min, m_max = cb.m_max;
	m_N = cb.m_N;
	return *this;
}
void CCurvatureBin::shift()
{
	int m = 101;
	int* points = m_points;
	int div;
	for(int i=0;i<m_cx;i++,points++)
	{
		if(m>*points && *points>=0)
		{
			m = *points;
			div =i;
			if(m ==0) break;
		}			
	}

	if(div!=0)
	{	
		int* pts =new int[m_cx];
		memcpy(pts, m_points+div, (m_cx-div)*sizeof(int));
		memcpy(pts+ m_cx- div, m_points, div*sizeof(int));
		delete[] m_points;
		m_points = pts;
	}
}

CStringX CurveDescr::GenerateStringEx(int* points, int nSize, double R)
{
	int n = (nSize+10)/20;
	m_strDescr.RemoveAll();
	m_strDescr.SetSize(20);
	double* d =m_strDescr.GetBuffer();
	int* pts = points;
	double t =0;
	for(int i=0, k=0;i<nSize;i++,k++,pts++)
	{
		if(k<n && i<nSize-1)
		{
			t+=*pts;
		}
		else
		{
			*d =t/R/k;
			k=0;
			
			t=*pts;
			d++;
		}
	}
	return m_strDescr;
}

void CCurvatureBin::GetDescriptor(CurveDescr& cd)
{
	double R = 8./m_N/m_weightY;
	/*cd.GenerateStringEx(m_points, m_cx, R);
	return;*/
	cd.R = R;
	cd.RemoveAll();
	CCurvatureBin cb;  
	m_se =50;  

		cb = *this;	
		/*to_mat();*/
		//
		/*cb.pad(m_se);*/
		Close(cb, m_se);
		Open(cb, m_se);
		  




		int* pts = new int[m_cx]; 
		//to_mat();
		sub(pts, m_points, cb.m_points, m_cx);

		//to_mat(pts,m_cx);
		int* old = pts;
		//провер€ем наличие пика в начале контура
		int n=0;
		while(*pts)
		{
			n++;
			if((*pts ^ *(pts+1))<0) break;
			pts++;
		}
		if(n)
		{
			CURVDESCR cvd = {0};
			cvd.size =n;
			cvd.center = n/2;
			cvd.begin = 0;
			cvd.end = n;
			cd.AddTail(cvd);
		}
		//все прочие
		int m =n;
		for(int i=m;i<m_cx;i++, pts++)
		{
			n=0;
			while(*pts && i<m_cx)
			{
				n++;
				if((*pts ^ *(pts+1))<0) break;
				pts++;
				i++;
			}
			if(n)
			{
				CURVDESCR cvd = {0};
				cvd.size =n;
				cvd.center = n/2+i;
				cvd.begin = i-n;
				cvd.end = i;
				cd.AddTail(cvd);
			}
		}

		POSITION pos = cd.GetHeadPosition();
		CURVDESCR prev ={0};
		while(pos)
		{
			POSITION p = pos;
			CURVDESCR& descr = cd.GetNext(pos);
			descr.CalcType(m_points, R);
			if(descr.begin-prev.end > 9)
			{
				CURVDESCR d ={0};
				d.begin = prev.end +1;
				d.end = descr.begin-1;
				d.CalcType(m_points,R);
				cd.InsertBefore(p,d);
			}			
			prev = descr;
		}
		cd.SetSignificance(0.045);
		cd.Compact(R);
		CStringX s = cd.GenerateString();
		delete[] old;
		

//__asm nop
}


POSITION CurveDescr::find_useless(double sf)
{
	POSITION pos = GetHeadPosition();
	
	while(pos)
	{
		CURVDESCR cd = GetAt(pos);
		if(cd.size/1000. < sf) break;
		GetNext(pos);
	}
	return pos;
}
void CURVDESCR::SetType()
{
	if(abs(value)<PEAK) 
	{
		if(value<0.) type = _Curve;
		else type = Curve;
	}
	else
	{
		if(value<0.) type = _Peak;
		else type = Peak;
	}
}

//int CurveDescr::operator-(CurveDescr& desc)
//{
//	CStringX strTemp, strStr;
//	int n1 = m_strDescr.GetCount();
//	int n2 = desc.m_strDescr.GetCount();
//	int n;
//	if(n1>n2)
//	{
//		strTemp = m_strDescr + m_strDescr;
//		strStr = desc.m_strDescr;
//		n = n1;
//	}
//	else
//	{
//		strTemp = desc.m_strDescr + desc.m_strDescr;
//		strStr = desc.m_strDescr;
//		n =n2;
//	}
//
//	double* strL = strTemp.GetBuffer();
//	double* strS = strStr.GetBuffer();
//
//	int match =0;
//	for(int i=0;i<n;i++, strL++)
//	{
//		int m = 0;
//		for(int j=0;j<n;j++)
//		{
//			if(strS[j]==strL[j]) m++;
//		}
//		if(m>match) match = m;
//	}
//	return match;
//}

void CurveDescr::SetSignificance(double sf)
{
	if(GetCount()==1) return;
	POSITION pos = find_useless(sf);
	while(pos)
	{
		CURVDESCR descr = GetAt(pos);
		if(descr.size/1000.< sf)
		{
			POSITION next = pos;
			POSITION prev = pos;

			GetNextCyclic(next);
			CURVDESCR& dnext = GetAt(next);

			GetPrevCyclic(prev);
			CURVDESCR& dprev = GetAt(prev);

			if(abs(descr.value-dprev.value) < abs(descr.value-dnext.value) ) 
			{
				
				dprev.value = (dprev.value*dprev.size + descr.value*descr.size)/(descr.size + dprev.size);
				dprev.SetType();
				dprev.end = descr.end;
				dprev.size = dprev.end - dprev.begin;
				if(dprev.size<0) dprev.size+=1000;
				
				RemoveAt(pos);

			}
			else 
			{
				
				dnext.value = (dnext.value*dnext.size + descr.value*descr.size)/(descr.size + dnext.size);
				dnext.SetType();
				dnext.begin = descr.begin;
				dnext.size = dnext.end - dnext.begin;
				if(dnext.size<0) dnext.size+=1000;

				RemoveAt(pos);
			}
		}
		pos = find_useless(sf);
	}
}

CURVDESCR& CurveDescr::GetNextCyclic(POSITION& pos)
{
	CURVDESCR& cd = GetNext(pos);
	if(pos ==0) pos = GetHeadPosition();
	return cd;
}

CURVDESCR& CurveDescr::GetPrevCyclic(POSITION& pos)
{
	CURVDESCR& cd = GetPrev(pos);
	if(pos ==0) 
		pos = GetTailPosition();
	return cd;
}
//void CurveDescr::SetGlobalParameters(GDESCR g)
//{
//	m_gDesc = g;
//	m_strDescr.GlobalParameters()=g;
//}

CString CurveDescr::ToString()
{
	CString s, t;
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		TCHAR* cType;
		CURVDESCR descr = GetNext(pos);
		switch(descr.type)
		{
		case _Peak:
			cType = "P-";
			break;
		case Peak:
			cType = "P+";
			break;
		case Curve:
			cType = "C+";
			break;
		case _Curve:
			cType = "C-";
			break;
		}
		t.Format("%s%.2f,%.2f ", cType, ((double)descr.size)/1000, descr.value);
		s+=t;
	}
	s+="\n";

	return s;
}
void CurveDescr::SetGlobalParameters(GDescr& gd)
{
	m_gDesc = gd;
	m_strDescr.GlobalParameters() = gd;
}
void CurveDescr::CalculateAddons()
{
	double* buff = m_strDescr.GetBuffer();
	int n = m_strDescr.GetCount();

	GPARAM gp;

	//Energy
	gp.key =800;
	gp.value = curvEnergy(buff, n);
	m_gDesc.Add(gp);
	m_strDescr.GlobalParameters() = m_gDesc;

	gp.key =801;
	gp.value = curvMaxima(buff, n);
	m_gDesc.Add(gp);
	m_strDescr.GlobalParameters() = m_gDesc;



}

void CurveDescr::Compact(double R)
{
	if(GetCount()<=1) return;
	POSITION pos = GetHeadPosition();
	POSITION prevPos = GetTailPosition();	
	CURVDESCR prev = GetTail();
	while(pos)
	{
		POSITION oldPos = pos;
		CURVDESCR& cd = GetNext(pos);
		if(prev.type<Peak && cd.type < Peak && abs(cd.value-prev.value)<.3)   
		{
			cd.value = (prev.value*prev.size + cd.value*cd.size)/(prev.size + cd.size);
			cd.begin = prev.begin;
			cd.size = cd.end-cd.begin;
			if(cd.size<0) 
				cd.size+=1000;
			if(cd.value<0.) cd.type = _Curve;
			else cd.type = Curve;
			RemoveAt(prevPos);

			pos = oldPos;
			GetPrevCyclic(oldPos);
			if(oldPos==pos) break;
			prev = GetAt(oldPos);
			prevPos = oldPos;
			continue;
		}
		prev = cd;
		prevPos = oldPos;
	}
}

void CURVDESCR::CalcType(int* points, double R)
{
	double total =0.;
	double nmax = abs(points[begin]);
	for(int i =begin;i<end;i++)
	{
		total+=points[i];
		if(nmax<abs(points[i])) 
		{
			nmax = points[i];
			center = i;
		}
	}
	
	size = end - begin;
	value = total/size;
	value /=R;

	r = nmax!=0.? total/size/nmax:0.;
	if(abs(value)<PEAK) 
	{
		if(value<0.) type = _Curve;
		else type = Curve;
	}
	else
	{
		if(value<0.) type = _Peak;
		else type = Peak;
	}
	

}
void CCurvatureBin::get_real(double* r)
{
	int* pts = m_points;
	for(int i=0;i<m_cx;i++,r++, pts++)
	{
		*r = m_weightY * (*pts);
	}
}
CCurvatureBin::CCurvatureBin(CCurvatureBin& cb)
{
	//	m_image =0;
	m_points =0;
	m_minus=0;
	m_plus=0;
	*this = cb;
}

CCurvatureBin::CCurvatureBin(void)
{
	//	m_image =0;
	m_points =0;
	m_minus=0;
	m_plus=0;
	m_se = 20;
}

CCurvatureBin::~CCurvatureBin(void)
{
	delete[] m_points;
	delete[] m_minus;
	delete[] m_plus;
}

void CCurvatureBin::Create(UINT cx, UINT cy)
{

	m_cx = cx, m_cy = (cy+1)/2;

	m_se = m_cx/50;
}
void CCurvatureBin::Attach(double* pts, UINT n)
{

	m_max = m_min = *pts;
	double* points = pts; 
		double total =0.;
	for(UINT i=1;i<n;i++,points++)
	{
		if(m_max<*points) m_max = *points;
		else if(m_min>*points) m_min = *points;
		total+=*points;
	}
	points = pts;

	if(m_min<0.)
	{
		m_weightY = max(m_max/(double)m_cy,-m_min/(double)m_cy);
	}
	else m_weightY = m_max/(double)m_cy;


	m_weightX = ((double)(m_N = n))/(double)m_cx;
	double avg =m_max*n;

	ASSERT(m_points==0);
	m_points = new int[m_cx];
	m_plus = new int[m_cx];
	m_minus = new int[m_cx];
	ZeroMemory(m_plus, sizeof(int)*m_cx);
	ZeroMemory(m_minus, sizeof(int)*m_cx);

	double tick =0.;
	double prevValue = *points;
	for(int x = 0; x< m_cx; x++)
	{
		double value =0.;
		int count =0;
		double nextTick =(x+1)* m_weightX;
		while(tick <= nextTick)
		{
			value+=(*points);
			points++;
			tick++;
			count++;
		}
		if(count ==0) m_points[x] = prevValue;
		else
		{
			m_points[x] = value/(double)count/m_weightY;
			prevValue = m_points[x];
		}

	}
	shift();
	for(int x = 0; x< m_cx; x++)
	{
		if(m_points[x]>0) m_plus[x]=m_points[x];
		else m_minus[x]=m_points[x];
	}
}

//void CCurvatureBin::to_mat(double* pts, int n)
//{
//	CString s ="",temp;
//
//	for(int i=0;i < n; i++)
//	{
//		temp.Format("%f\n", pts[i]);
//		s+=temp;
//	}
//
//	CFile file;
//	file.Open("E:\\Programs\\Mathlab\\work\\shape.mat", CFile::modeCreate | CFile::modeWrite);
//	char* t =s.GetBuffer();
//	file.Write(t,s.GetLength());
//	file.Close();
//}

//void CCurvatureBin::to_mat(int* pts, int n)
//{
//	CString s ="",temp;
//
//	for(int i=0;i < n; i++)
//	{
//		temp.Format("%i\n", pts[i]);
//		s+=temp;
//	}
//
//	CFile file;
//	file.Open("E:\\Programs\\Mathlab\\work\\shape.mat", CFile::modeCreate | CFile::modeWrite);
//	char* t =s.GetBuffer();
//	file.Write(t,s.GetLength());
//	file.Close();
//}

void CCurvatureBin::pad(int delta)
{
	int* temp = new int[m_cx + delta*2];
	memcpy( temp, m_points+m_cx-delta, delta*sizeof(int));
	memcpy(temp+delta, m_points, m_cx*sizeof(int));
	m_cx+=delta*2;
	memcpy(temp-delta+m_cx, m_points, delta*sizeof(int));
	delete[] m_points;
	m_points = temp;

}
void CCurvatureBin::unpad(int delta)
{
	m_cx-=delta*2;
	int* temp = new int[m_cx];
	memcpy( temp, m_points+delta, m_cx*sizeof(int));
	delete[] m_points;
	m_points = temp;
}

void CCurvatureBin::GetSizes(UINT* x, UINT* y)
{
	*x = m_cx;
	*y = m_cy;
}
void CCurvatureBin::combine()
{
	delete[] m_points;
	m_points = new int[m_cx];
	int* pts = m_points, *p = m_plus, *p_ = m_minus;
	for(int i=0;i<m_cx; i++, p_++, p++, pts++)
	{
		*pts = *p_ + *p; 
	}
}

//ћорфологические операции
void CCurvatureBin::Erode(CCurvatureBin& cb, int n)
{
	ASSERT(n<cb.m_cx);
	int* out_plus = new int[cb.m_cx];
	int* out_minus = new int[cb.m_cx];
	memset(out_plus, 100, cb.m_cx*sizeof(int));
	memset(out_minus, -100, cb.m_cx*sizeof(int));

	int* points = cb.m_points;

	int t,m,k;
	for(int i=0; i<cb.m_cx; i++,points++,out_plus++,out_minus++)
	{

		m = max(-i,-n);
		k = min(cb.m_cx-i, n+1);
		t = *points>0 ? *points:0;
		for(int j=m;j<k;j++)
		{
			if(t<out_plus[j]) out_plus[j]= t;
		}
		t = *points<0 ? *points:0;
		for(j=m;j<k;j++)
		{
			if(t>out_minus[j]) out_minus[j]= t;
		}
	}
	delete[] cb.m_plus;
	cb.m_plus = (out_plus-cb.m_cx);
	delete[] cb.m_minus;
	cb.m_minus = (out_minus-cb.m_cx);
}
void CCurvatureBin::Dilate(CCurvatureBin& cb, int n)
{
	ASSERT(n<cb.m_cx);

	int* out_plus = new int[cb.m_cx];
	int* out_minus = new int[cb.m_cx];
	memset(out_plus, 0, cb.m_cx*sizeof(int));
	memset(out_minus, 0, cb.m_cx*sizeof(int));


	int* points = cb.m_points;
	int t,m,k;
	for(int i=0; i<cb.m_cx; i++,points++,out_plus++,out_minus++)
	{
		m = max(-i,-n);
		k = min(cb.m_cx-i, n+1);
		t = *points>0 ? *points:0;
		for(int j=m;j<k;j++)
		{
			if(t>out_plus[j]) out_plus[j]= t;
		}
		t = *points<0 ? *points:0;
		for(int j=m;j<k;j++)
		{
			if(t<out_minus[j]) out_minus[j]= t;
		}
	}

	delete[] cb.m_plus;
	cb.m_plus = (out_plus-cb.m_cx);
	delete[] cb.m_minus;
	cb.m_minus = (out_minus-cb.m_cx);


}
void CCurvatureBin::Open(CCurvatureBin& cb, INT n)
{
	Erode(cb, n);
	Dilate(cb, n);
	cb.combine();
}
void CCurvatureBin::Close(CCurvatureBin& cb, INT n)
{
	Dilate(cb, n);
	Erode(cb, n);
	cb.combine();
}

//CKnlgBase::CKnlgBase()
//{
//
//
//}
//CKnlgBase::~CKnlgBase()
//{
//
//}
//void CKnlgBase::AddDescriptor(CString& strDesc)
//{
//	char temp[4]={0};
//	int n = strDesc.GetLength();
//	char* buff =  strDesc.GetBuffer();
//	CString str = buff;
//	str =  buff[n-1]+str;
//	str = buff[n-2]+str;
//	str += buff[0];
//	str +=buff [1];
//	buff = str.GetBuffer();
//	while(n)
//	{	
//		strncpy(temp,buff,3); 
//		tplt.Set(temp);
//		buff++;
//		n--;
//	}
//}
//
//void CKnlgBase::WriteToFile(CString& strFileName)
//{
//	CFile file;
//	file.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate);
//	CArchive ar(&file, CArchive::Mode::store);
//	Serialize(ar);
//}
//void CKnlgBase::ReadFile(CString& strFileName)
//{
//	CFile file;
//	if(!file.Open(strFileName, CFile::modeRead )) return;
//	CArchive ar(&file, CArchive::Mode::load);
//	Serialize(ar);
//}
//
//void CKnlgBase::Serialize(CArchive& ar)
//{
//	long version =1;
//	if(ar.IsLoading())
//	{
//		ar.Read(&version, sizeof(long));
//		ar.Read(tplt.words, STEPS);
//	}
//	else
//	{
//		ar.Write(&version, sizeof(long));
//		ar.Write(tplt.words, STEPS);
//	}
//}
//int CKnlgBase::Find(CString& strDesc)
//{
//	char temp[4]={0};
//	int n = strDesc.GetLength();
//	char* buff = strDesc.GetBuffer();
//	CString str = buff; 
//	str = buff[n-1] + str;
//	str = buff[n-2] + str;
//	str = str + buff[0];
//	str = str + buff[1];
//	buff = str.GetBuffer();
//	int rank =0;
//	while(n)
//	{	
//		strncpy(temp,buff,3); 
//		if(tplt.Get(temp)) rank++;;
//		buff++;
//		n--;
//	}
//	return rank;
//}
//
//byte Triplet::Get(char* s)
//{
//	byte* h = words + ((*s)-'a')*81;
//	s++;
//	h = h+((*s)-'a')*9;
//	s++;
//	h = h+((*s)-'a');
//	return *h;
//}
//
//void  Triplet::Set(char* s)
//{
//	byte* h = words + ((*s)-'a')*81;
//	s++;
//	h = h+((*s)-'a')*9;
//	s++;
//	h = h+((*s)-'a');
//	*h = 1;
//}

CStringX& CStringX::operator=(const CStringX& s)
{
	RemoveAll();
	Copy(s);
	m_gDescr = s.m_gDescr;
	m_nClassPos = s.m_nClassPos;
	return *this;
}

CStringX::CStringX(const CStringX& s)
{
	RemoveAll();
	Copy(s);
	m_gDescr = s.m_gDescr;
	m_nClassPos =s.m_nClassPos;
}
CStringX& CStringX::operator+=(double tc)
{
	Add(tc);
	return *this;
}
//CStringX& CStringX::operator+(const CStringX& s)
//{
//	Append(s);
//	return *this;
//}
bool CStringX::operator==(CStringX& s)
{
	int n = GetCount();
	if(n != s.GetCount()) return false;
	double* me = GetData();
	double* ss = s.GetData();
	for(; n; n--,ss++,me++)
	{
		if(*ss != *me) return false;
	}

	return true;
}
CStringX::~CStringX()
{
	RemoveAll();
}
GDescr& CStringX::GlobalParameters()
{
	return m_gDescr;
}

void CStringX::Serialize(CArchive& ar)
{
	long version =1;
	int n = GetCount();
	if(ar.IsLoading())
	{
		ar>>version;
		ar>>n;
		SetSize(n);
		for(int i=0;i<n;i++)
		{
			ar>>GetAt(i);
		}

	}
	else
	{
		ar<<version;
		ar<<n;
		for(int i=0;i<n;i++)
		{
			ar<<GetAt(i);
		}
	}
	m_gDescr.Serialize(ar);
}
GPARAM  GDescr::Find(long key)
{
	int n = GetSize();
	GPARAM gp ={0};
	for(int i=0;i<n;i++)
	{
		gp = GetAt(i);
		if(gp.key ==  key)
			return gp;
	}
	return gp;
}


void GDescr::Serialize(CArchive& ar)
{
	int n;
	if(ar.IsLoading())
	{
		ar>>n;
		for(int i=0;i<n;i++)
		{
			GPARAM gp;
			ar.Read(&gp, sizeof(GPARAM));
			Add(gp);
		}
	}
	else
	{
		n = GetCount();
		ar<<n;
		for(int i=0;i<n;i++)
		{
			GPARAM gp = GetAt(i);
			ar.Write(&gp, sizeof(gp));
		}
	}
}
 
GDescr& GDescr::operator=(const GDescr& gd)
{
	RemoveAll();
	Copy(gd);
	return *this;
}

GDescr& GDescr::operator+=(GDescr& gd)
{
	int n = gd.GetCount();
	ASSERT(GetCount()==n);
	for(int i =0;i<n;i++)
	{
		GetAt(i).value+=gd.GetAt(i).value;
	}
	return *this;
}

GDescr& GDescr::operator=(double d)
{
	int n = GetCount();
	for(int i=0;i<n;i++)
	{
		GetAt(i).value=d;
	}
	return *this;
}

GDescr& GDescr::operator/=(double d)
{
	int n = GetCount();
	for(int i=0;i<n;i++)
	{
		GetAt(i).value/=d;
	}
	return *this;
}