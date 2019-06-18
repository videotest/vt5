#include "StdAfx.h"

#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "float.h"
#include "binimageint.h"
#include "units_const.h"
#include "misc_utils.h"
#include "\vt5\awin\profiler.h"
//#include "class_utils.h"
#include "propbag.h"
#include "params.h"
#include "classifyshape.h"
#include <stack>
#include "cccontour.h"
#include "MorphoGraph.h"
#include "classstorage.h"
#include "params.h"
#include "ParzenWindows.h"
#include "shapemeasuregroup.h"

//#define MINDETAIL 1.0 //погрешность аппроксимации, мкм

IMPLEMENT_DYNCREATE(CClassifyShape, CCmdTargetEx);

// {1AFBE299-0FBB-458e-840C-AE3CFEED4691}
GUARD_IMPLEMENT_OLECREATE(CClassifyShape, "Shape.ClassifyShape", 
0x1afbe299, 0xfbb, 0x458e, 0x84, 0xc, 0xae, 0x3c, 0xfe, 0xed, 0x46, 0x91);

// {E5738952-AD3E-4688-A49C-4DEA0362DF46}
static const GUID guidClassifyShape = 
{ 0xe5738952, 0xad3e, 0x4688, { 0xa4, 0x9c, 0x4d, 0xea, 0x3, 0x62, 0xdf, 0x46 } };

ACTION_INFO_FULL(CClassifyShape,IDS_ACTION_CLASSIFY_SHAPE, -1, -1, guidClassifyShape); 

ACTION_TARGET(CClassifyShape, szTargetAnydoc);

ACTION_ARG_LIST(CClassifyShape)
	ARG_INT("learn",0)
	ARG_OBJECT(_T("ObjectsIn"))
	RES_OBJECT( _T("ObjectsOut"))
END_ACTION_ARG_LIST()

//int g_learn;




CClassifyShape::CClassifyShape(void)
{
}

CClassifyShape::~CClassifyShape(void)
{
}

inline const char *short_filename( const char *sz_path )
{
	const char *sz1 = strrchr( sz_path, '\\' );
	return 0==sz1 ? sz_path : sz1+1;
}

inline const char *def_filename( char *sz_path, const char *sz_section, const char *sz_file, int cb )
{
	GetModuleFileName( 0, sz_path, cb );
	strcpy( strrchr( sz_path, '\\' )+1, sz_section );
	strcat( sz_path, "\\" );
	strcpy( sz_path, GetValueString( GetAppUnknown(), "\\Paths", sz_section, sz_path ) );
	strcat( sz_path, sz_file );

	return sz_path;

}



inline const char *short_classifiername(const char *szClassifierName)
{
	if(szClassifierName && *szClassifierName) return short_filename(szClassifierName);
	// (если имя не пустое - не 0 и не "")

	static char sz_ini[MAX_PATH];
	def_filename( sz_ini, "Classes", "classes.ini", sizeof(sz_ini) );
	strcpy( sz_ini, (const char*)::GetValueString( GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ) );
	return short_filename(sz_ini);
}
inline void set_object_class( ICalcObject *pcalc, long lclass, char *szClassifierName=0 )
{
	if( !pcalc )return;

	if(szClassifierName==0 || *szClassifierName==0)
	{ // если пишем в активный классификатор - на всякий случай сдублируем значение в параметр - для старых скриптов
		pcalc->SetValue( KEY_CLASS, (double)lclass );
	}

	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_variant_t	var;
		bag->SetProperty(_bstr_t(short_classifiername(szClassifierName)), _variant_t(lclass));
	}
}
inline long get_object_class( ICalcObject *pcalc, const char *szClassifierName=0 )
{
	if( !pcalc )return -1;
	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_variant_t	var;
		bag->GetProperty(_bstr_t(short_classifiername(szClassifierName)), &var);
		if(var.vt == VT_I4) return var.lVal;
	}

	// если не вернули значение - используем старую процедуру
	double	dblClass;
	if( pcalc->GetValue( KEY_CLASS, &dblClass ) != S_OK )return -1;
	return (long)dblClass;
}


void CClassifyShape::read_class_file()
{
	CString classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("ClassFile"),"");
	if(classfile.IsEmpty()) return;

	int r,g,b;
	TCHAR buffer[128];
	TCHAR sz[5]; 
	::GetPrivateProfileString( "Classes", "ClassCount", "0", buffer, sizeof( TCHAR )*128, classfile );
	int classCount = _ttoi(buffer);
	if(!classCount) return;
	m_strClassNames.RemoveAll();
	
	int i=0;
	_itot(i,sz,10);

	::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
	
	while(*buffer)
	{
		sscanf( buffer, "(%d,%d,%d),%[^;]s", &r, &g, &b, buffer );
		TCHAR*  name = new TCHAR[ _tcslen(buffer)+1];
		_tcscpy(name,buffer);
		m_strClassNames.Add(CString(name));

		i++;
		_itot(i,sz,10);
		::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
		delete []name;
	}

}
CString CShapeDescriptor::ToString()
{
	CString s = "Descriptor:\n";

	CString temp;
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		POLYNODE pn = GetNext(pos);
		temp.Format("%f\n", pn.curvature);
		s+=temp;
	}
	return s;
}
bool CClassifyShape::InvokeFilter()
{


	CString strClassFile =	::GetValueString( GetAppUnknown(), "\\Classes", "ClassFile", "");
	if(strClassFile.IsEmpty()) return false;	

	read_class_file();

	IUnknown* unk =  GetDataArgument("ObjectsIn");
	int nOperation = GetArgumentInt("learn");
	bool bLearn = (nOperation==1);

	CClassStorage cs;
	CString path = "e:\\vt5\\shape.knb";
	cs.Init(path);
	cs.Recalculate();
	if(nOperation ==2)
	{
		/*cs.Decalculate();*/
		cs.Recalculate();
		cs.Write(path);
		return true;
		
	}


	INamedDataObject2Ptr ndo(unk);
	/*if(unk) unk->Release();*/
	if(ndo==0) return false;

	long pos;
	ndo->GetFirstChildPosition(&pos);


	
	/*ndo->GetActiveChild(&pos);*/
	if(pos==0) return false;
	while(pos)
	{
		IUnknown *punkChild;
		ndo->GetNextChild( &pos, &punkChild );
		

		/*CPolygonalApprox pga;
		pga.CreatePolygone(punkChild);

		punkChild->Release();

		pga.CalculateDescriptor();
		pga.Print();
		pga.ViewContour();
		static int count;
		count++;
		CString sss;
		sss.Format("------ %i\n",count);
		OutputDebugStr(sss);*/ 
		CChCodedContour ccc(punkChild);
		if(!ccc.GetContourSize()) 
		{
			punkChild->Release();
			continue;
		}
		CMorphoGraph mg;
		CArray<double>& arr = ccc.GetArray();

		mg.Init(arr.GetData(), arr.GetSize(), 1000, 200, 10);

		ICalcObject2Ptr cobj(punkChild);

		CurveDescr cd;
		GDescr gd;
		GPARAM gp;

		
		gp.key = KEY_CIRCLE;
		HRESULT hr = cobj->GetValue(gp.key, &gp.value);
		ASSERT(SUCCEEDED(hr));
		if(!SUCCEEDED(hr))
		{
			punkChild->Release();
			AfxMessageBox("!");
			return true;
		}
		gd.Add(gp);

		gp.key = KEY_ELLIPSE;
		hr = cobj->GetValue(gp.key, &gp.value);
		ASSERT(SUCCEEDED(hr));
				if(!SUCCEEDED(hr))
		{
			punkChild->Release();
			AfxMessageBox("!");
			return true;
		}
		gd.Add(gp);	
		
		gp.key = KEY_ROUNDISH;
		hr = cobj->GetValue(gp.key, &gp.value);
		ASSERT(SUCCEEDED(hr));
		if(!SUCCEEDED(hr))
		{
			punkChild->Release();
			AfxMessageBox("!");
			return true;
		}
		gd.Add(gp);

		gp.key = KEY_NOUN;
		hr = cobj->GetValue(gp.key, &gp.value);
		ASSERT(SUCCEEDED(hr));
		if(!SUCCEEDED(hr))
		{
			punkChild->Release();
			AfxMessageBox("!");
			return true;
		}
		gd.Add(gp);

		/*gp.key = 131076;
		hr = cobj->GetValue(gp.key, &gp.value);
		ASSERT(SUCCEEDED(hr));
		gd.Add(gp);*/

		gp.key = KEY_MOMENTS_RATIO;
		gp.value =moments_ratio(punkChild);
		gd.Add(gp);

		mg.GetDescriptor(&cd);
		cd.SetGlobalParameters(gd);
		

		cd.CalculateAddons();
		if(bLearn)
		{
			int n = get_object_class(cobj, strClassFile);
			cd.SetClass(n);
			if(n>-1)
			{
				cd.SetClassName(m_strClassNames[n]);
				cs.AddDescriptor(cd);	
			}
		}
		else if(nOperation==0)
		{

			int n = cs.ClarifyClass(cd);
			set_object_class(cobj, n, strClassFile.GetBuffer());
		}
		punkChild->Release();
	}
	if(bLearn)
	{
		cs.Write(path);
		
	}


	return true;
}


//+++++++++++++++CPolygonalApprox

void CPolygonalApprox::ViewContour()
{
	IUnknown* unk;
	mo->GetImage(&unk);
	IImage4Ptr image = unk;
	if(unk) unk->Release();
	Contour* c = ContourCreate();


	POSITION pos = m_sd.GetHeadPosition();
	while(pos) 
	{
	
		POLYNODE pn1 = m_sd.GetNext(pos);
		ContourAddPoint(c, pn1.x, pn1.y);
	}
	image->AddContour(c);
	return;
	POLYNODE pn1 = m_sd.GetNext(pos);
	POLYNODE pn2 = m_sd.GetNext(pos);
	while(pos)
	{
		
		double k;
		if(pn2.x-pn1.x==0)
		{
			int sign = (pn2.y-pn1.y)>0? 1:-1;
			for(int i=0;i!=pn2.y-pn1.y;i=i+sign)
			{
				ContourAddPoint(c, pn2.x, pn1.y+i);
			}
		}
		else
		{
			k= (pn2.y-pn1.y)/double(pn2.x-pn1.x);
			int sign = (pn2.x-pn1.x)>0? 1:-1;

			for(int i=0;i!=pn2.x-pn1.x;i=i+sign)
			{
				ContourAddPoint(c, pn1.x+i,pn1.y +i*k);
			}
		}
		pn1 =pn2;
		pn2 = m_sd.GetNext(pos);
	}
	image->AddContour(c);
}

bool CPolygonalApprox::CalculateDescriptor()
{
	//TODO: вычислить eps
	/*m_eps =10.; */   
	
	if(m_sd.GetCount()<3) return false;
	POSITION postail = m_sd.GetTailPosition();
	POSITION pos = m_sd.GetHeadPosition();
	POLYNODE* pPrev, *pNext, *pCurr;
	
	pPrev = &(m_sd.GetTail());
	pCurr =  &(m_sd.GetNext(pos));
	pNext = &(m_sd.GetNext(pos));
	CalculatePolynode(*pPrev, pCurr, *pNext, m_eps);

	while(pos)
	{
		pPrev = pCurr;
		pCurr = pNext;
		pNext = &(m_sd.GetNext(pos));
		CalculatePolynode(*pPrev, pCurr, *pNext, m_eps);
	}
	pPrev = pCurr;
	pCurr = pNext;
	pNext = &(m_sd.GetHead());
	CalculatePolynode(*pPrev, pCurr, *pNext, m_eps);
	return true;
}
POINT CPolygonalApprox::Center()
{
	int X=0,Y=0;
	for(int i=0;i<m_pc->nContourSize;i++)
	{
		X+=m_pc->ppoints[i].x;
		Y+=m_pc->ppoints[i].y;
	}
	POINT p = {X/m_pc->nContourSize, Y/m_pc->nContourSize};
	return p;
}
void CPolygonalApprox::NormalizeContour()
{
	int n = MaxDistPoint();
	ContourPoint* pp = new ContourPoint[m_pc->nAllocatedSize];
	memcpy( pp, m_pc->ppoints+n, (m_pc->nContourSize-n)*sizeof( ContourPoint ));
	memcpy(pp+m_pc->nContourSize-n, m_pc->ppoints, n*sizeof( ContourPoint ));
	delete []m_pc->ppoints;
	m_pc->ppoints = pp;
	m_pointsDist.SetSize(m_pc->nContourSize);
	m_pointsDist[0] =0.;
	double sqrt2 = sqrt(2.);
	for(int i=1;i<m_pc->nContourSize;i++)
	{
		double ds = ((m_pc->ppoints[i].x==m_pc->ppoints[i-1].x) || (m_pc->ppoints[i].y==m_pc->ppoints[i-1].y)) ? 1. : sqrt2;
		m_pointsDist[i] = m_pointsDist[i-1] + ds; 
	}
}
int CPolygonalApprox::MaxDistPoint()
{
	POINT cp = Center();
	double maxDist =-1.;
	int n;
	for(int i=0;i<m_pc->nContourSize;i++)
	{
		int dx = cp.x-m_pc->ppoints[i].x;
		int dy = cp.y-m_pc->ppoints[i].y;
		double dist = sqrt(double(dx*dx+dy*dy));
		if(dist>maxDist) 
		{
			maxDist = dist;
			n =i;
		}
	}
	return n;
}
double CPolygonalApprox::GetContourSize()
{
	if(m_pc==0) return 0.;
	return (double)m_pc->nContourSize/**m_caliber*/;
}

double CPolygonalApprox::GetAverageEdge()
{
	int n = m_sd.GetCount();
	if(n==0) return 1./*(double)m_pc->nContourSize*/;

	double dl =0.0;
	POLYNODE pn1, pn2;
	pn1 = m_sd.GetTail();

	POSITION pos = m_sd.GetHeadPosition();
	while(pos)
	{
		pn2 = m_sd.GetNext(pos);
		double dx = pn2.x - pn1.x;
		double dy = pn2.y - pn1.y;
		dl += sqrt(dx*dx + dy*dy);
		pn1 = pn2;
	}

	return dl*m_caliber*1000.;	
}

bool CPolygonalApprox::CreatePolygone(IUnknown* object)
{
	using namespace std;
	mo =object;
	if(mo==0) return false;
	//TODO eps

	CImageWrp image;
	IUnknown* unk;
	mo->GetImage(&unk);
	if(unk==0) return false;

	image.Attach(unk, false);

	Contour* c = image.GetContour(0);//внешний(главный) контур объекта
	m_pc = c;
	if(c==0) return false;
	
	NormalizeContour();
	int n = c->nContourSize/2;

	/*double caliber;*/
	GUID g;
	ICalibrPtr clb(object);
	clb->GetCalibration(&m_caliber, &g);
	
	m_eps = MINDETAIL/m_caliber;
	/*ICalcObject2Ptr co(object);
	double s, pm;

	co->GetValue(KEY_PERIMETER, &pm);
	co->GetValue(KEY_AREA, &s);
	m_eps = s/pm/caliber/3.;*/ 

	// c->nContourSize/100.+ .5;

	ContourPoint cptBegin = c->ppoints[0];
	ContourPoint cptNext = c->ppoints[n];

	stack<int> opened, closed;

	closed.push(0);
	opened.push(n);
	bool b2 = false;
	//proceccing first half of contour
	while(1)
	{
		double maxDist = 0.;
		int k = opened.top();
		int m = k==0 ? c->nContourSize:k; 
		int l = closed.top();
		double temp;
		int nmax = l;
		for(int i=l+1;i<m;i++)
		{
			temp = DistancePointToSL(c->ppoints[i], c->ppoints[l],c->ppoints[k]);
			if(temp > maxDist) 
			{
				maxDist = temp;
				nmax = i;
			}
		}
		if(maxDist < m_eps)
		{
			closed.push(opened.top());
			opened.pop();
		}
		else
		{
			opened.push(nmax);
		}
		if(opened.empty() )
		{
			if(closed.top()==n)
			{
				//proceccing another half of contour
				closed.pop();//исключаем искусственно выбранную точку n
				opened.push(0);
			}
			else break;
		}

	}

	//add all poligonal vertices(except last) to descriptor
	ASSERT(closed.top()==0);
	closed.pop();
	POLYNODE pn = {0};
	while(!closed.empty())
	{
		n = closed.top();
		closed.pop();
		
		pn.x = c->ppoints[n].x;
		pn.y = c->ppoints[n].y;
		pn.number = n;
		m_sd.AddHead(pn);
	}
	return true;
}

void CPolygonalApprox::Print()
{
	CString temp;
	CString out = "--------Descriptor---------\n\n";
	POSITION pos = m_sd.GetHeadPosition();
	POLYNODE pn;
	while(pos)
	{
		pn = m_sd.GetNext(pos);
		temp.Format("%f\t%f\n", pn.curvature, m_pointsDist[pn.number]);
		out+=temp;
	}
	OutputDebugStr(out);

}

double DistancePointToSL(ContourPoint pt,//point of interest 
						 ContourPoint pt1, ContourPoint pt2)//точки на прямой
{
	//a - вектор (pt2,pt1)
	//b - вектор (pt,pt1)
	//c - вектор нормали к a c точностью до множителя
	/*ASSERT(pt1.x != pt2.x || pt1.y != pt2.y);*/

	POINTF a ={pt2.x - pt1.x, pt2.y -pt1.y};
	POINTF b ={pt1.x - pt.x, pt1.y - pt.y};
	
	/*POINTF c;*/
	double db = sqrt(b.x*b.x+b.y*b.y);
	double cosA = abs(a.x*b.x+a.y*b.y)/sqrt(a.x*a.x+a.y*a.y)/db;

	double r = db*sqrt(1-cosA*cosA);
	return r;
	/*if(a.x != 0.) c.x = -a.y/a.x, c.y = 1;
	else c.x = 1, c.y = -a.x/a.y;

	return abs(c.x*b.x + c.y*b.y)/sqrt(c.x*c.x+c.y*c.y);*/

}
//<<<<<CPolygonalApprox

void CalculatePolynode(POLYNODE plnPrev, POLYNODE* plnCurrent, POLYNODE plnNext, double eps)
{
	//a - вектор (ptPrev,ptCurr)
	//b - вектор (ptCurr,ptNext)
	POINTF a = {plnCurrent->x - plnPrev.x, plnCurrent->y - plnPrev.y};
	POINTF b = {plnNext.x - plnCurrent->x, plnNext.y - plnCurrent->y};

	double cosA = abs(a.x*b.x + a.y*b.y)/sqrt((a.x*a.x + a.y*a.y)*(b.x*b.x + b.y*b.y));
	int grad =  acos(cosA)/2/3.1415*360;
	double sinA2 = sqrt((1-cosA)/2);

	double K = (1-sinA2)/eps/sinA2;//абс. значение - теперь надо вычислить направление
	
	int sign = (a.x*b.y-a.y*b.x)>0 ? 1 : -1;//векторное пр-е
	K = K*sign;
	
	plnCurrent->curvature = K;

//TODO: вычислить длину кривой?
}














