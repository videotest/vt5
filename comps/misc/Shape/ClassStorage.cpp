#include "StdAfx.h"

#include "morphograph.h"
#include "classstorage.h"
#include <math.h>
#include "ParzenWindows.h"




// {F75316A0-E41C-42f6-B724-2F0636E18084}
static const GUID FileID = 
{ 0xf75316a0, 0xe41c, 0x42f6, { 0xb7, 0x24, 0x2f, 0x6, 0x36, 0xe1, 0x80, 0x84 } };



void OutputDescr(double* d, int n)
{
	CString s, t;
	double* temp =d;
	for(int i=0;i<n;i++,temp++)
	{
		t.Format("%.2f ", *temp);
		s+=t;
		t.Empty();
	}
	s+="\n";
	OutputDebugString(s);
}

#define MAXENERGY 8.

double curvEnergy(double* curv, int n)
{
	double diff = 0.; 
	double ce =0.;
	diff = curv[0]-curv[n-1];
	ce+=diff*diff;
	for(int i=1;i<n;i++)
	{
		diff= curv[i]-curv[i-1];
		ce+=diff*diff;
	}

	double maxCurv = MAXENERGY*MAXENERGY*n;
	return ce/maxCurv;	
}
#undef MAXENERGY 
double bendingEnergy(double* curv, int n)
{

	double be =0.;
	double d =n/8.;
	d=d*d;
	for(int i=1;i<n;i++)
	{
		be+=curv[i]*curv[i];
	}
	return be/n/d * 10E6;
}

double curvMaxima(double* curv, int n)
{
	double d = *curv; 
	for(int i=1;i<n;i++)
	{
		if(d<curv[i]) d=curv[i];
	}
	return d/8.*n;
}
double curvMiniima(double* curv, int n)
{
	double d = *curv; 
	for(int i=1;i<n;i++)
	{
		if(d>curv[i]) d=curv[i];
	}
	return d/8.*n;;
}

double peakRatio(double* curv, int n)
{
	double d=0.;
	for(int i=1;i<n;i++)
	{
		if(curv[i]>1.) d++;
	}
	return d/n;
}

CClassStorage::CClassStorage(void)
{
	m_bNewBase = false;
}

CClassStorage::~CClassStorage(void)
{
	while(!m_classes.IsEmpty())
	{
	 ShapeClass* sc = m_classes.GetTail();
	 delete sc;
	 m_classes.RemoveTail();
	}
}


void CClassStorage::Write(CString& path)
{
	m_file.Open(path, CFile::modeWrite | CFile::modeCreate );
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);
	ar.Flush();
	ar.Close();
	m_file.Close();
}

BOOL CClassStorage::Init(CString& path)
{
	m_dblTshld = .2;
	BOOL result =1;
	if(!m_file.Open(path, CFile::modeRead))
	{
		m_file.Open(path, CFile::modeRead | CFile::modeCreate );
		m_file.Close();
		m_bNewBase = true;
		return 1;
	}
	try
	{
	CArchive ar(&m_file, CArchive::load);
	Serialize(ar);
	}
	catch(CWrongFileException* e)
	{
		e->Delete();
		result =0;
		CString s;
		s.Format("File \"%s\" is not valid as classifier knowledge base!", path);
		AfxMessageBox(s, MB_OK | MB_ICONERROR, 0);
    }
	catch(CNewFileException* e )
	{
		e->Delete();
		m_bNewBase = true;
		result =1;
	}
	catch(...)
	{
		m_bNewBase = true;
	}
	m_file.Close();
	return result;
}

ShapeClass* CClassStorage::find(int n)
{
	POSITION pos = m_classes.GetHeadPosition();
	while(pos)
	{
		ShapeClass* sc = m_classes.GetNext(pos);
		if(sc->GetClassNum()==n)
			return sc;
	}
	return 0;
}
ShapeClass* CClassStorage::find(CString& s)
{
	POSITION pos = m_classes.GetHeadPosition();
	while(pos)
	{
		ShapeClass* sc = m_classes.GetNext(pos);
		if(sc->GetClassName()==s)
			return sc;
	}
	return 0;
}
void CClassStorage::Recalculate()
{


	POSITION pos = m_classes.GetHeadPosition();
	while(pos)
	{
		ShapeClass* sc = m_classes.GetNext(pos);
		sc->Clusterize();
	}
}

void CClassStorage::Decalculate()
{
	POSITION pos = m_classes.GetHeadPosition();
	while(pos)
	{
		ShapeClass* sc = m_classes.GetNext(pos);
		sc->DeClusterize();
	}
}

void CClassStorage::AddDescriptor(CurveDescr& descr)
{
	int nClass = descr.GetClass();
	ShapeClass* sc = find(nClass);
 	if(sc ==0)
		sc = AddClass(nClass, descr.GetClassName());
	sc->SetSubclassThreshold(m_dblTshld);
	sc->AddDescriptor(descr.GetString(), false);
}

int CClassStorage::ClarifyClass(CurveDescr& descr)
{
	CStringX s = descr.GetString();

	//TODO: output energy

	double d = -1000.;
	ShapeClass* bingo =0;
	double temp;
	POSITION pos = m_classes.GetHeadPosition();
	while(pos)
	{
		ShapeClass* sc = m_classes.GetNext(pos);
		temp= sc->FindNearest/*Exact*/(s);
		if(temp>d) 
		{
			d=temp;
			bingo = sc;
			/*if(temp==0.) break;*/
		}
	}
	
 	if(bingo==0 || d==0.) return -1;  
	return bingo->GetClassNum();
}



ShapeClass* CClassStorage::AddClass(int num, CString& className)
{
	ShapeClass* sc = find(num);
	if(sc!=0 ) return sc;

	sc =new ShapeClass(num, className);
	m_classes.AddTail(sc);
	return sc;
}
void CClassStorage::RemoveClass(CString& className, int num/* =-1*/)
{

		POSITION pos = m_classes.GetHeadPosition();
		while(pos)
		{
			POSITION sp =pos;
			ShapeClass* sc = m_classes.GetNext(pos);
			if(num!=-1)
			{
				if(sc->GetClassNum() == num)
				{
					m_classes.RemoveAt(sp);
					delete sc;
					return;
				}
			}
			else
			{
				if(sc->GetClassName() == className)
				{
					m_classes.RemoveAt(sp);
					delete sc;
					return;
				}
			}
		}


}
void CClassStorage::Serialize(CArchive& ar)
{
	long version =1.;

	if(ar.IsLoading())
	{
		GUID g;
		UINT n = ar.Read(&g, sizeof(g));
		if(n==0)
		{
			CNewFileException* nfe = new CNewFileException();
			throw nfe;
		}
		if(n<sizeof(g) || g!=FileID) 
		{
			CWrongFileException* wfe = new CWrongFileException();
			throw wfe ;
		}
		ar>>version;
		ar>>m_strName;
		ar>>m_dblTshld;
	}
	else
	{
		ar.Write(&FileID, sizeof(FileID));
		ar<<version;
		ar<<m_strName;
		ar<<m_dblTshld;	
	}
	SerializeClasses(ar);
	m_arrParams.Serialize(ar);

}
void CClassStorage::SerializeClasses(CArchive& ar)
{
	long version =1;

	if(ar.IsLoading())
	{

		int count = 0;
		ar>>version;
		ar>>count;

		while(count)
		{
			ShapeClass* sc = new ShapeClass;
			sc->Serialize(ar);
			m_classes.AddTail(sc);
			count--;
		}
	}
	else
	{
		int count =m_classes.GetCount();
		ar<<version;
		ar<<count;

		POSITION pos = m_classes.GetHeadPosition();
		while(pos)
		{
			ShapeClass* sc =m_classes.GetNext(pos);
			sc->Serialize(ar);
		}
	}

}

bool CClassStorage::CheckParameter(long key)
{
	int n= m_arrParams.GetSize();
	if(n==0) return true; //новый классификатор
	for(int i=0;i<n;i++)
	{
		if(m_arrParams[i]==key) return true;
	}
	return false;
}

void ShapeClass::K_Mean()
{
	
}

CString ShapeClass::GetClassName()
{
	return m_strName;
}
int ShapeClass::GetClassNum()
{
	return m_nClass;
}
void ShapeClass::Serialize(CArchive& ar)
{
	long version =1;
	int nSubClasses = m_subClasses.GetCount();
	int nDescriptors = m_descrs.GetCount();
	if(ar.IsLoading())
	{
		ar>>version;
		ar>>m_strName;
		ar>>m_nClass;
		ar>>m_nDscLength;
		ar>>nSubClasses;
		ar>>m_dblTshld;
		ar>>nDescriptors;
		m_weights.Serialize(ar);
			

		while(nSubClasses)
		{
			ShapeClass* sc = new ShapeClass;
			sc->Serialize(ar);
			nSubClasses--;
			m_subClasses.Add(sc);
		}
		while(nDescriptors)
		{
			CStringX sx;
			sx.Serialize(ar);
			m_descrs.Add(sx);
			nDescriptors--;
		}
	}
	else
	{
		CString ctr ="\n" + m_strName;
		ar<<version;
		ar<<m_strName;
		ar<<m_nClass;
		ar<<m_nDscLength;
		ar<<nSubClasses;
		ar<<m_dblTshld;
		ar<<nDescriptors;
	
		m_weights.Serialize(ar);
	

		if(nSubClasses)
		{
			int n = m_subClasses.GetCount();
			for(int i=0;i<n;i++)
			{
				ShapeClass* sc = m_subClasses.GetAt(i);
				sc->Serialize(ar);
			}
		}
		if(nDescriptors)
		{
			CString s;
			s.Format(" %i\n",nDescriptors);
			ctr+=s;
			OutputDebugString(ctr);
			for(int i=0;i<nDescriptors;i++)
			{
				m_descrs.GetAt(i).Serialize(ar);
			}
		}
	}
	
}

ShapeClass::ShapeClass(int num, CString& className)
:m_nClass(num)
{
	m_strName = className;
	m_nDscLength =0;
	m_dt =0;
	m_bWeights = false;
	m_pw =0;
}
ShapeClass::~ShapeClass()
{
	int n = m_subClasses.GetCount();
	for(int i=0;i<n;i++)
	{
		ShapeClass* shc = m_subClasses.GetAt(i);
		delete shc;
	}
	m_subClasses.RemoveAll();
	delete[] m_dt;
	delete m_pw;
}
double ShapeClass::FindNearestExact(CStringX& str)
{
	if (!m_bWeights) CalcWeights();
	int n = m_descrs.GetCount();
	double dMin = 1000.;
	for(int i=0;i<n;i++)
	{
		CStringX s = m_descrs[i];
		double d = distWW(s,str);
		if(d<dMin) dMin =d;
	}

	return dMin;
}

double ShapeClass::FindNearest(CStringX& str)
{

	double ret = 1.0;
	int s = str.m_gDescr.GetSize();
	for(int i=0;i<s;i++)
	{
		GPARAM gp = str.m_gDescr.GetAt(i);
		ret*= this->m_pw->EstimateDescriptor(gp.value, gp.key);
		if(ret ==0.0) break;
	}
	return ret;
	double dblMin=1000000.;
	ShapeClass* scMin =0;
	CStringX gauge;
	if(!m_subClasses.IsEmpty())
	{

		int n = m_subClasses.GetCount();
		for(int i=0;i<n;i++)
		{
			ShapeClass* sc = m_subClasses.GetAt(i);
			/*int n = sc->GetDescriptorLength();*/
			/*sc->RecalculateGauge();*/
			sc->GetGauge(gauge);
			double d = distWW(gauge, str);
			if(d< dblMin)
			{
				dblMin = d;
				scMin = sc;
			}
			/*delete[] g;*/				
		}
	}
	/*if(scMin)
	{
		double dblMinEx =scMin->FindNearestExact(str);
		if(dblMinEx<dblMin) return dblMin = dblMinEx;
	}*/

	return dblMin;
}
bool  ShapeClass::AddDescriptor(CStringX& str, bool bSubclassAllowed/* = false*/)
{
	/*if(bSubclassAllowed)
	{

		if(!m_subClasses.IsEmpty())
		{
			int n = m_subClasses.GetCount();
			for(int i=0;i<n;i++)
			{
				ShapeClass* sc = m_subClasses.GetAt(i);
				int n = sc->GetDescriptorLength();
				CStringX g;
				sc->GetGauge(g);
				if(dist(g, str)<=m_dblTshld)
				{
					if(sc->AddDescriptor(str, false))
						sc->RecalculateGauge();
					
					return true;
				}
				delete[] g;				
			}
		}
			CString s = CString("sub_") + this->m_strName;
			ShapeClass* sc = new ShapeClass(m_nClass, s);
			m_subClasses.Add(sc);
			sc->AddDescriptor(str, false);
			sc->RecalculateGauge();	

	}
	else*/
	{//TODO: is this check needed?
		/*int n = m_descrs.GetSize();
		for(int i=0;i<n;i++)
		{
			if(str == m_descrs[i]) return false;
		}*/
		if(!m_nDscLength) m_nDscLength = str.GetCount();
		m_descrs.Add(str);
	}
	return true;
}

void ShapeClass::SetSubclassThreshold(double thr)
{
	m_dblTshld = thr;
}

void ShapeClass::RecalculateGauge()
{

	int n = m_descrs.GetCount();
	if(n==0) return;
	m_Gauge.m_gDescr =m_descrs.GetAt(0).m_gDescr;
	
	for(int i=1;i<n;i++)
	{
		CStringX s = m_descrs.GetAt(i);
		m_Gauge.m_gDescr += s.m_gDescr;
	}
	m_Gauge.m_gDescr/=n;
	/*delete []m_dblGauge;
	m_dblGauge =0;
	int n = m_descrs.GetCount();

	if(n>0)
	{
		int m = m_descrs[0].GetCount();
		double* temp = m_descrs[0].GetBuffer();
		m_dblGauge = new double[m];

		if(n==1)
		{
			for(int j=0;j<m;j++)
			{
				m_dblGauge[j] = (double)temp[j];
			}
			return;
		}

		double* dsts = new double[m];

		ZeroMemory(m_dblGauge, sizeof(double)*m);

		for(int i=0;i<m;i++)
		{
			dsts[i] = (double)temp[i];
		}

		double d=0.;
		for(i=1;i<n;i++)
		{
			d = dist(dsts,m_descrs[i]);
			OutputDescr(dsts, m);
			OutputDescr(m_descrs[i].GetBuffer(), m);

			for(int j=0;j<m;j++)
			{
				m_dblGauge[j] = (m_dblGauge[j]*(i-1)+ dsts[j])/((i-1)+1);
			}
			memcpy(dsts, m_dblGauge, m*sizeof(double));
		}
		delete[] dsts;
	}*/
	
}

int ShapeClass::GetDescriptorLength()
{
	return m_nDscLength;
}

//void ShapeClass::GetGauge(double* gauge)
//{
//	if(m_dblGauge) RecalculateGauge();
//	if(m_dblGauge)
//	{
//		memcpy(gauge, m_dblGauge, m_nDscLength*sizeof(double));
//	}
//	
//}
double ShapeClass::abs(double d)
{
	if(d>=0.) return d;
	else return -d;
}
double ShapeClass::distWW(CStringX& s1, CStringX& s2)
{
	double result =0.;
	int n = s1.m_gDescr.GetCount();
	
	for(int i=0;i<n;i++)
	{
		double d = s1.m_gDescr[i].value - s2.m_gDescr[i].value;
		result+=m_weights[i].value*d*d;
	}
	return sqrt(result);
}

double ShapeClass::dist(CStringX& s1, CStringX& s2)
{
	ASSERT(s1.GetCount()==s2.GetCount());
	int n = m_nDscLength = s1.GetCount();
	double* t = s1.GetBuffer();
	double* temp = new double[n];
	for(int i=0;i<n;i++)
	{
		temp[i]=t[i];
	}
	double ret = dist(temp, s2);
	memcpy(t, temp, sizeof(double)*n); 
	delete[] temp;
	return ret;
}

double ShapeClass::dist(double* s1, CStringX& s2)
{
	//return subtraction(s1, s2);
	return correlation(s1, s2);
}

double ShapeClass::subtraction(double* s1, CStringX& s2)
{
	int n =s2.GetCount();
	double* str = new double[n*2];
	double* temp = new double[n];

	memcpy(str,s1, sizeof(double)*n);
	memcpy(str+n,s1, sizeof(double)*n);
	double* strDel = str;

	double* strL = s2.GetBuffer();
	double match =160.;

	for(int i=0; i<n; i++, str++)
	{
		double m = 0.;
		for(int j=0;j<n;j++)
		{
			m+=abs(str[j]-strL[j]);
			int k = i+j>=n?i+j-n:i+j;
			ASSERT(k<n);
			temp[k]=(double)strL[j];//saving elements of shifted string,
									//after all, the best suited string will placed in s1  
		}
		if(m<match) 
		{
			match = m;
			memcpy(s1,temp, sizeof(double)*n); 
		}
	}
	delete []strDel;
	delete []temp;

	return match;
}

double ShapeClass::correlation(double* s1, CStringX& s2)
{
	int n =s2.GetCount();
	double* str = new double[n*2];
	double* temp = new double[n];
	double* strL = s2.GetBuffer();
	double* strDel = str;

	memcpy(str,s1, sizeof(double)*n);
	memcpy(str+n,s1, sizeof(double)*n);


	double auto1 =.0, auto2=.0;//автокоррел€ции
	for(int i=0;i<n;i++,str++,strL++)
	{
		auto1+=(*strL)*(*strL);
		auto2+=(*str)*(*str);
	}
	double norm = sqrt(auto1*auto2);
	str = strDel;
	strL = s2.GetBuffer();

	double match =-1.e20;
	int k;
	for(i=0; i<n; i++, str++)
	{
		double m = 0.;
		for(int j=0;j<n;j++)
		{
			m+=str[j]*strL[j];
		}
		if(m>match) 
		{
			k=i;
			match = m;
		}
	}
	
	memcpy(s1, strDel+k, sizeof(double)*(n-k));
	memcpy(s1+(n-k), strDel, sizeof(double)*k);
	match= (match/norm);
	delete []strDel;
	delete []temp;

	return match;
}

void ShapeClass::CalcWeights()
{

	int n = m_descrs.GetCount();//кол-во собранных дескрипторов объектов
	int m = m_descrs[0].m_gDescr.GetCount();//кол-во глобальных параметров

	m_nDTSize =n;
	delete[] m_dt;
	m_dt = new double[m_nDTSize*m_nDTSize];
	ZeroMemory(m_dt, m_nDTSize*m_nDTSize*sizeof(double));

	m_weights.SetSize(m);
	m_mean.SetSize(m);
	for(int i=0;i<n;i++)
	{
		CStringX desc = m_descrs.GetAt(i);
		m_mean+=desc.m_gDescr;
	}
	m_mean/=n;

	double* qS = new double[m];;
	ZeroMemory(qS, sizeof(double)*m);
	for(i=0;i<n;i++)
	{
		CStringX desc = m_descrs.GetAt(i);
		for(int j=0;j<m;j++)
		{
			double d = m_mean[j].value - desc.m_gDescr[j].value;
			qS[j] += d*d;
		}		
	}

	double d;
	for(i=0;i<m;i++)
	{
		d = m_weights[i].value = 1./qS[i]*n;
		/*if(m_weights[i].value>10000.) m_weights[i].value =10000.;*/
	}
	delete[] qS;
}
POINT ShapeClass::find_min()
{
	double dblMax = m_dt[m_nDTSize];

	POINT pt={1,0};
	for(int i=0;i<m_nDTSize;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(i==j) continue;
			double d = m_dt[i*m_nDTSize+j];

			if(dblMax>d)
			{
				pt.x = i;
				pt.y = j;
				dblMax = d;
			}
		
		}	
	}

	return pt;
}
void ShapeClass::CalcDistTableWithWeights()
{
	m_nDTSize = m_descrs.GetCount();
	int m = m_weights.GetCount();
	if(m==0) 
	{
		CalcWeights();
		m = m_weights.GetCount();
	}
	if(m==0) return;

	delete[] m_dt;
	m_dt = new double[m_nDTSize*m_nDTSize];
	ZeroMemory(m_dt, m_nDTSize*m_nDTSize*sizeof(double));

	for(int i=0;i<m_nDTSize;i++)
	{
		CStringX& desc = m_descrs.GetAt(i);
		for(int j=0;j<i;j++)
		{
			CStringX& desc2 = m_descrs.GetAt(j);
			double* d = m_dt+i*m_nDTSize+j;
			for(int k=0;k<m;k++)
			{
				double D = desc2.m_gDescr[k].value - desc.m_gDescr[k].value;
				*d += D*D*m_weights[k].value;
			}
			*d = sqrt(*d);
		}
	}
}



void ShapeClass::Clusterize()
{
	OutputDebugString(m_strName);
	OutputDebugString("\n");
	delete m_pw;
	m_pw = new ParzenWindows();
	m_pw->AttachDescriptors(m_descrs);
	return; 

	if(m_dt==0) 
	{
		CalcWeights();
		CalcDistTableWithWeights();
	}
	int nClassNum = sqrt(double(m_nDTSize));

	while(m_descrs.GetCount() > nClassNum)
	{
		POINT pt = find_min();//пара наиболее близких элементов
		CStringX s1 = m_descrs.GetAt(pt.x);
		CStringX s2 = m_descrs.GetAt(pt.y);
		s1 = combine_to_subclass(s1, s2);
		m_descrs.SetAt(pt.x, s1);
		m_descrs.RemoveAt(pt.y);
		CalcDistTableWithWeights();
	}

  for(int i=0;i<m_subClasses.GetCount();i++)
  {
	  ShapeClass* sc = m_subClasses.GetAt(i); 
	  if(sc->GetClassPos()==-1)
	  {
		m_subClasses.RemoveAt(i);
		i--;
		delete sc;
	  }
	  //TODO AssignWeights or CalcWeights
	  else /*sc->AssignWeights(m_weightR, m_weightC, m_weightE, m_weightD)*/
		  sc->CalcWeights();
  }

}
void ShapeClass::DeClusterize()
{
	for(int i=0;i<m_subClasses.GetCount();i++)
	{
		ShapeClass* sc = m_subClasses.GetAt(i); 
		sc->SetClassPos(-1);
		int n = sc->m_descrs.GetCount();
		for(int j=0;j<n;j++)
		{
			CStringX s = sc->m_descrs.GetAt(j);
			AddDescriptor(s, false);
		}
		sc->m_descrs.RemoveAll();
		delete sc;
	}
	m_subClasses.RemoveAll();
}

void ShapeClass::AssignWeights(GDescr& weights)
{
	m_weights = weights;
}
void ShapeClass::GetGauge(CStringX& gauge)
{
	int n = m_descrs.GetCount();
	int m = m_descrs[0].m_gDescr.GetCount();

	gauge.m_gDescr.SetSize(m);
	gauge.m_gDescr =0.;

	for(int i=0;i<n;i++)
	{
		CStringX s = m_descrs.GetAt(i);
		gauge.m_gDescr+=s.m_gDescr;
	}
	gauge.m_gDescr /=n; 
	gauge.m_nClassPos = m_descrs[0].m_nClassPos;
}
int ShapeClass::AddDescriptor(CStringX& str)
{
	ASSERT(str.m_nClassPos != -1);
	m_descrs.Add(str);
	return str.m_nClassPos;
}

CStringX ShapeClass::combine_to_subclass(CStringX& s1, CStringX& s2)
{
	ShapeClass* sc =0;
	int pos;
	int nClss =  m_subClasses.GetCount();
	if(s1.m_nClassPos == -1 && s2.m_nClassPos == -1)
	{
		sc = new ShapeClass(m_nClass, m_strName);
		pos = m_subClasses.Add(sc);		
		s1.m_nClassPos = s2.m_nClassPos = pos;
		sc->AddDescriptor(s1);
		sc->AddDescriptor(s2);
	}
	else if(s1.m_nClassPos == -1)
	{
		ASSERT(s2.m_nClassPos<nClss);
		sc =  m_subClasses.GetAt(s2.m_nClassPos);
		pos = s1.m_nClassPos = s2.m_nClassPos;
		sc->AddDescriptor(s1);
	}
	else if(s2.m_nClassPos == -1)
	{
		ASSERT(s1.m_nClassPos<nClss);
		sc =  m_subClasses.GetAt(s1.m_nClassPos);
		pos = s2.m_nClassPos = s1.m_nClassPos;
		sc->AddDescriptor(s2);
	}
	else
	{
		ASSERT(s1.m_nClassPos<nClss);
		ASSERT(s2.m_nClassPos<nClss);
		sc =  m_subClasses.GetAt(s1.m_nClassPos);
		ShapeClass* sc2 = m_subClasses.GetAt(s2.m_nClassPos);

		int n = sc2->m_descrs.GetCount();
		for(int i=0;i<n;i++)
		{
			CStringX& s = sc2->m_descrs.GetAt(i);
			//pos = s.m_nClassPos = s1.m_nClassPos;
			sc->AddDescriptor(s);
		}
		sc->SetClassPos(s1.m_nClassPos);
		sc2->SetClassPos(-1);
	}
	CStringX strX;
	sc->RecalculateGauge();
	sc->GetGauge(strX);
	
	/*strX.m_nClassPos =pos;*/
	return strX;
}
void ShapeClass::SetClassPos(int pos)
{
	int n = m_descrs.GetCount();
	for(int i=0;i<n;i++)
	{
		CStringX& s = m_descrs.GetAt(i);
		s.m_nClassPos = pos;
	}
}
