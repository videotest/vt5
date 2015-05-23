#include "StdAfx.h"


#include "Params.h"
#include "shapemeasuregroup.h"
#include "PropBag.h"
#include "cccontour.h"
#include "morphograph.h"
#include "units_const.h"
#include "classstorage.h"
#include "math.h"
#include "sceleton.h"
#include "classifyshape.h"

#define MIN_POINTS_IN_THRES 0.03
#define MAX_POINTS_IN_THRES 1.- MIN_POINTS_IN_THRES
#define DIST 1.5

IMPLEMENT_DYNCREATE(CShapeMeasureGroup, CMeasParamGroupBase)

// {D70047B4-F385-4aa6-82BE-BE522261CA61}
static const GUID clsidShapeMeasureGroup = 
{ 0xd70047b4, 0xf385, 0x4aa6, { 0x82, 0xbe, 0xbe, 0x52, 0x22, 0x61, 0xca, 0x61 } };

// {986C2766-382B-4010-8D89-2B65E2F77ED2}
IMPLEMENT_GROUP_OLECREATE(CShapeMeasureGroup, "Shape.ShapeMeasureGroup", 
						  0x986c2766, 0x382b, 0x4010, 0x8d, 0x89, 0x2b, 0x65, 0xe2, 0xf7, 0x7e, 0xd2);


double polyEdge(IUnknown* object)
{
	CPolygonalApprox pa;
	pa.CreatePolygone(object);
	double e = pa.GetAverageEdge();

	return e;
}

double sceleton(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;


	CImageWrp image;
	IUnknown* unk;
	mo->GetImage(&unk);
	if(unk==0) return 0.0;

	image.Attach(unk, false);

	CSize size(image.GetWidth(),image.GetHeight()); 
	byte* img = new byte[size.cx*size.cy];
	for(int y =0;y<size.cy;y++)
	{
		byte* row = image.GetRowMask(y);
		memcpy(img + size.cx*y, row, size.cx);
	}
	CSceleton sc;
	Contour* c = image.GetContour(0);
	CPoint p = image.GetOffset();
	double caliber =0;
	GUID g;
	ICalibrPtr clb(object);
	clb->GetCalibration(&caliber, &g);
	ICalcObject2Ptr co(object);
	double s, pm;

	co->GetValue(KEY_PERIMETER, &pm);
	co->GetValue(KEY_AREA, &s);
	double tt = MINDETAIL/caliber; 
	int n = sc.AttachDataSKIZ(c, img, size, p, tt);   

	/*byte *t = img;
	for(int i=0;i<size.cx*size.cy;i++, t++)
	{
		*t = *t ? 255:0; 
	}
	for(y =0;y<size.cy;y++)
	{
		byte* row = image.GetRowMask(y);
		memcpy(row, img + size.cx*y, size.cx);
	}*/
	delete []img;
	return (double)n/(double)c->nContourSize;
}

double moments_ratio(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;


	CImageWrp image;
	IUnknown* unk;
	mo->GetImage(&unk);
	if(unk==0) return 0.0;

	image.Attach(unk, false);

	Contour* c = image.GetContour(0);//внешний(главный) контур объекта
	if(c->nContourSize<30) 
	{
		return 0.0;
	}
	if(c==0) return 0.0;

	ContourPoint pt1={0}, pt2={0};
	double dist=0.;
	for(int i =0;i<c->nContourSize; i++ )
	{
		for(int j =0;j<c->nContourSize; j++ )
		{
			double dx = c->ppoints[i].x-c->ppoints[j].x;
			double dy = c->ppoints[i].y-c->ppoints[j].y;
			double d = sqrt(dx*dx + dy*dy);
			if(d>dist)
			{
				dist = d;
				pt1 = c->ppoints[i];
				pt2 = c->ppoints[j];
			}
		}
	}
		

	POINT offset = image.GetOffset();

	pt1.x -= offset.x;
	pt1.y -= offset.y;

	pt2.x -= offset.x;
	pt2.y -= offset.y;

	double X = pt2.x - pt1.x;
	double Y = pt2.y - pt1.y;

	double l = sqrt(X*X+Y*Y);//length of horde


	
	int cx = image.GetWidth();
	int cy = image.GetHeight();
	double m1=0., m2=0.;

	for(i=0;i<cy; i++ )
	{
		byte* row = image.GetRowMask(i);
		double dy1 = pt1.y - i;
		double dy2 = pt2.y - i;

		for(int j =0;j<cx;j++,row++)
		{
			if(*row)
			{
				double dx = pt1.x - j;
				m1+=::abs(dx*X + dy1*Y)/l;

				dx = pt2.x - j;
				m2+=::abs(dx*X + dy2*Y)/l;
			}
		}
	}
	if(m1>m2) return m2/m1;

	return m1/m2;
}

double brightness_moment(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;

	CBrightnessMoment bm(object);
	return bm.GetDisperse();
}
double points_in_ed(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;

	CBrightnessMoment bm(object);
	return bm.points_in_ed();
}

double hordes_ratio(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;

	CBrightnessMoment bm(object);
	return bm.hordes_ratio();
}

double eigendisperse(IUnknown* object)
{
	IMeasureObject2Ptr mo;
	mo = object;
	if(mo==0) return 0.0;

	CBrightnessMoment bm(object);
	return bm.GetEigenValuesRatio();
}

double distance_to_sl(POINTF sl, POINTF pt)
{
	double op = sl.x*pt.x + sl.y*pt.y;
	return sqrt((pt.x*pt.x+pt.y*pt.y)-op*op);

}


CBrightnessMoment::CBrightnessMoment(IUnknown* object)
{
	m_Unk = object;
	m_Unk->AddRef();

	IMeasureObject2Ptr mo;
	mo = object;

	IUnknown* unk;
	mo->GetImage(&unk);
	if(unk==0) return;

	m_object.Attach(unk, false);
	
	m_cx = m_object.GetWidth();
	m_cy = m_object.GetHeight();

	m_ptsThr.reserve(m_cy*m_cy);

	m_gray = new byte[m_cx*m_cy];
	ZeroMemory(m_gray, m_cx*m_cy);
	offset = CPoint(0,0);
	m_bVirtual = m_object.IsVirtual();
	if(m_bVirtual)
	{
		INamedDataObject2Ptr ndImage = m_object;
		IUnknown* pUnk;
		ndImage->GetParent(&pUnk);
		if(pUnk)
		{
			m_iwParent.Attach(pUnk, false);
			offset = m_object.GetOffset();
		}
	}
	else 
	{
		m_iwParent = m_object;
	}
	
	int nClr = m_iwParent.GetColorsCount();
	color** rows = new color*[nClr]; 

	for(int i=0;i<m_cy;i++)
	{
		for(int k=0;k<nClr;k++)
		{
			rows[k] = m_iwParent.GetRow(i + offset.y, k);
		}
		byte* temp = m_gray + i*m_cx;
		byte* b = m_object.GetRowMask(i);
		for(int j=0;j<m_cx;j++,temp++, b++)
		{
			if(*b==0) continue;
			double d = 0.;
			for(k=0;k<nClr;k++)
			{
				byte c = ColorAsByte(rows[k][j+offset.x]); 
				d += c*c;
			}
			*temp = sqrt(d/nClr);
		}
	}
	m_grayO =0;


}
CBrightnessMoment::~CBrightnessMoment()
{
	if(m_Unk) m_Unk->Release();
	delete[] m_gray;
	delete[] m_grayO;
}
double CBrightnessMoment::equivalent_diam()
{
	generate_threshold();
	gravity_center();
	double n = m_ptsThr.size();
	return sqrt(4*n/PI);
}

double CBrightnessMoment::points_in_ed()
{
	double d = equivalent_diam();
	vector<POINT>::const_iterator p = m_ptsThr.begin();
	double N=0;
	while(p!=m_ptsThr.end())
	{
		double dx = p->x - m_ptGC.x;
		double dy = p->y - m_ptGC.y;
		double dd = sqrt(dx*dx + dy*dy);
		if(sqrt(dx*dx + dy*dy)<d) N++;
		p++;
	}
	return N/m_n;
}

double CBrightnessMoment::hordes_ratio()
{
	generate_threshold();
	gravity_center();
	double fPtsInThr = (double)m_n/(double)m_cx/(double)m_cy;
	if(fPtsInThr<MIN_POINTS_IN_THRES) return 1.;
	

	vector<POINT>::const_iterator p = m_ptsThr.begin();
	int hordes[12] ={0};
	while(p!=m_ptsThr.end())
	{
		POINTF pt = {p->x -m_ptGC.x,p->y -m_ptGC.y};
		double tg = pt.y/pt.x;
		if(tg>0.)//1st or 3rd quadrant 
		{
			if(tg>1.)
			{
				if(tg>tg3_8)
				{
					if(distance_to_sl(pt90, pt)<=DIST)
					{
						hordes[6]++;
					}
					else if(distance_to_sl(pt75, pt)<=DIST)
					{
						hordes[5]++;
					}
				}
				else
				{
					if(distance_to_sl(pt60, pt)<=DIST)
					{
						hordes[4]++;
					}
					else if(distance_to_sl(pt45, pt)<=DIST)
					{
						hordes[3]++;
					}
				}

			}
			else
			{
				if(tg>tg1_8)
				{
					if(distance_to_sl(pt45, pt)<=DIST)
					{
						hordes[3]++;
					}
					else if(distance_to_sl(pt30, pt)<=DIST)
					{
						hordes[2]++;
					}
				}
				else
				{
					if(distance_to_sl(pt15, pt)<=DIST)
					{
						hordes[1]++;
					}
					else if(distance_to_sl(pt0, pt)<=DIST)
					{
						hordes[0]++;
					}

				}
			}
		}
		else//2nd or 4th quadrant
		{
			if(tg<-1.)
			{
				if(tg<-tg3_8)
				{
					if(distance_to_sl(pt90, pt)<=DIST)
					{
						hordes[6]++;
					}
					else if(distance_to_sl(pt105, pt)<=DIST)
					{
						hordes[7]++;
					}
				}
				else
				{
					if(distance_to_sl(pt120, pt)<=DIST)
					{
						hordes[8]++;
					}
					else if(distance_to_sl(pt135, pt)<=DIST)
					{
						hordes[9]++;
					}
				}
			}
			else
			{
				if(tg<-tg1_8)
				{
					if(distance_to_sl(pt135, pt)<=DIST)
					{
						hordes[9]++;
					}
					else if(distance_to_sl(pt150, pt)<=DIST)
					{
						hordes[10]++;
					}
				}
				else
				{
					if(distance_to_sl(pt165, pt)<=DIST)
					{
						hordes[11]++;
					}
					else if(distance_to_sl(pt0, pt)<=DIST)
					{
						hordes[0]++;
					}

				}

			}
		}	
	p++;
	}
	int nMin=100000, nMax=0;
	for(int i=0;i<12;i++)
	{
		if(hordes[i]>nMax) 
		{
			nMax=hordes[i];
			
		}
		if(hordes[i]<nMin) nMin=hordes[i];
	}
	return (double)nMin/nMax;
};

double CBrightnessMoment::GetEigenValuesRatio()
{
	generate_threshold();
	gravity_center();

	Matrix2x2 m(m_ptGC,m_ptGC);
	Matrix2x2 Cx;
	int k = m_ptsThr.size();
	vector<POINT>::const_iterator p = m_ptsThr.begin();
	while(p!=m_ptsThr.end())
	{
		Matrix2x2 X(*p,*p);
		Cx = Cx + X;
		p++;
	}
	Cx/=k;
	Cx = Cx -m;
	return Cx.ratio();
}
double CBrightnessMoment::GetDisperse()
{
	GetMoment();
	double fPtsInThr = (double)m_n/(double)m_cx/(double)m_cy; 
	if(fPtsInThr<MIN_POINTS_IN_THRES) return 1.;

	double meanI = m_I/m_n;
	double meanI2 = meanI*meanI;
	double ose =0.; 
	double I;
	vector<POINT>::const_iterator p = m_ptsThr.begin();
	for(int i=0;i<m_n;i++,p++)
	{
		double x = m_ptGC.x - p->x;
		double y = m_ptGC.y - p->y;
		I= sqrt(x*x + y*y)*m_gray[m_cx*p->y + p->x];
		ose += (I- meanI)*(I- meanI);
	}
	ose /= m_n;
	ICalcObject2Ptr co(m_Unk);
	return ose/meanI/meanI;
}
double CBrightnessMoment::GetMoment()
{
	generate_threshold();
	gravity_center();
	vector<POINT>::const_iterator p = m_ptsThr.begin();
	double I=0.;
	for(int i=0;i<m_n;i++,p++)
	{
		double x = m_ptGC.x - p->x;
		double y = m_ptGC.y - p->y;
		I+=sqrt(x*x + y*y)*m_gray[m_cx*p->y + p->x];
	}
	m_I =I;
	return I;
}
void CBrightnessMoment::generate_threshold()
{
	
	byte* histo = new byte[256];
	ZeroMemory(histo, 256);
	int t=0;
	double d=0.;
	for(int i=0;i<m_cy;i++)
	{
		byte* row = m_gray + m_cx*i;
		byte* b = m_object.GetRowMask(i);
		for(int j=0;j<m_cx;j++,row++, b++)
		{
			if(*b==0) continue;
			t++;
			d+=*row;
			histo[*row]++;
		}
	}
	
	double left=0.,right =0., newLeft=-1.;

	byte n = 0, N =d/t;
	while(n!=N)
	{
		n = N;
		int nLeft =0, nRight =0;
		left =0.,right=0.;
		for(int i=0;i<n;i++)
		{
			nLeft+=histo[i];
			left+=histo[i]*i;
		}
		for(;i<256;i++)
		{
			nRight+=histo[i];
			right+=histo[i]*i;
		}
		N = (right/nRight + left/nLeft)/2;
	}

	delete []histo;
	m_ptsThr.clear();
	for(int i=0;i<m_cy;i++)
	{
		byte* row = m_gray + m_cx*i; 
		for(int j=0;j<m_cx;j++,row++)
		{
			if(*row>N+20) 
			{
				continue;
				
			}
			else
			{
				*row =0;
			}
		}
	}



	for(int i=0;i<m_cy;i++)
	{
		byte* row = m_gray + m_cx*i; 
		for(int j=0;j<m_cx;j++,row++)
		{
			if(*row) 
			{
				POINT p = {j,i};
				m_ptsThr.push_back(p);	
			}
		}
	}

	m_n = m_ptsThr.size();
}
void CBrightnessMoment::open()
{

}
void CBrightnessMoment::erode()
{
	delete[] m_grayO; 
	m_grayO = new byte[(m_cy+2)*(m_cx+2)];
	ZeroMemory(m_grayO, (m_cy+2)*(m_cx+2));
	for(int i=1;i<=m_cy;i++)
	{
		memcpy( m_grayO + (m_cx+2)*i + 1, m_gray + m_cx*i, m_cx);
	}


	byte *rowUp, *rowDown, *row;
	for(int i=1;i<=m_cy;i++)
	{
		row = &m_grayO[(m_cx+2)*i];
		rowUp = row - (m_cx+2);
        rowDown = row + (m_cx+2);
		row++,rowUp++,rowDown++;
		for(int j=0;j<m_cx;j++,row++,rowUp++,rowDown++)
		{
			if( !*(row-1) 
				|| !*(row+1) 
				|| !*rowUp 
				|| !*(rowUp-1)  
				|| !*(rowUp+1) 
				|| !*(rowDown+1)
				|| !*(rowDown-1)
				|| !*rowDown
				) continue;
			m_gray[(i-1)*m_cx + j] = 0;
		}
	}
	
	ZeroMemory(m_grayO, (m_cy+2)*(m_cx+2));
	for(int i=1;i<=m_cy;i++)
	{
		memcpy( m_grayO + (m_cx+2)*i + 1, m_gray + m_cx*(i-1), m_cx);
	}

}
void CBrightnessMoment::dilate()
{
	byte *rowUp, *rowDown, *row;
	for(int i=1;i<=m_cy;i++)
	{
		row = &m_grayO[(m_cx+2)*i];
		rowUp = row - (m_cx+2);
		rowDown = row + (m_cx+2);
		for(int j=0;j<m_cx;j++,row++,rowUp++,rowDown++)
		{
			*row = 0;
			if( *(row-1) 
				&& *(rowUp-1) 
				&& *(rowDown-1) 
				&& *(row+1) 
				&& *(rowDown+1) 
				&& *(rowUp+1) 
				&& *rowUp 
				&& *rowDown) continue;
			
		}
	}
}
void CBrightnessMoment::gravity_center()
{
	double Ix =0., Iy =0.;
	double M =0.;

	POINT* p = &m_ptsThr.front();
	int n = m_ptsThr.size();
	for(int i=0;i<n;i++,p++)
	{
		double m = 1./*m_gray[p->y*m_cx + p->x]*/;
		M+=m;
		Ix+=p->x*m;
		Iy+=p->y*m;
	}
	m_ptGC.x = Ix/M;
	m_ptGC.y = Iy/M;
}





CShapeMeasureGroup::CShapeMeasureGroup(void)
{
	_OleLockApp( this );
	m_sName = "Shape";
}

CShapeMeasureGroup::~CShapeMeasureGroup(void)
{
	_OleUnlockApp( this );
}


bool CShapeMeasureGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	ICalcObject2Ptr co(punkCalcObject);

	CChCodedContour ccc(punkCalcObject);
	if(!ccc.GetContourSize()) return false;

	CMorphoGraph mg;
	CArray<double>& arr = ccc.GetArray();
	if(arr.GetSize()==0) return false;
	mg.Init(arr.GetData(), arr.GetSize(), 1000, 200, 10);

	CurveDescr cd;
	mg.GetDescriptor(&cd);
	CStringX s = cd.GetString();
	double* d = s.GetBuffer();
	int n = s.GetSize();

	POSITION pos = m_listParamDescr.GetHeadPosition();
	double value;
	while(pos)
	{
		ParameterDescriptor* pd = (ParameterDescriptor*)m_listParamDescr.GetNext(pos);
		if(pd->lEnabled==0) continue;
		switch(pd->lKey)
		{
		case KEY_CURVATURE_ENERGY:
			value = curvEnergy(d, n);
			break;
		case KEY_CURVATURE_MAX:
			value = curvMaxima(arr.GetData(), arr.GetSize());
			break;
		case KEY_MOMENTS_RATIO:
			value =moments_ratio(punkCalcObject);
			break;
		case KEY_BENDING_ENERGY:
			value = bendingEnergy(arr.GetData(), arr.GetSize());
			break;
		case KEY_BRIGHTNESS_MOMENTS_DISP:
			value = brightness_moment(punkCalcObject);
			break;
		case KEY_SKELETON_PERIMETER_RATIO:
			value = sceleton(punkCalcObject);
			break;
		case KEY_HORDES_RATIO:
			value = hordes_ratio(punkCalcObject);
			break;
		default:
			return true;
		}
	co->SetValue(pd->lKey, value);
	}
	return true;


}
bool CShapeMeasureGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeffToUnits )
{
	bstrType = GetValueString(GetAppUnknown(), "Units\\Relative", "Name", "-");
	fCoeffToUnits =1.;
	return true;
}

bool CShapeMeasureGroup::LoadCreateParam()
{
	DefineParameter(KEY_CURVATURE_ENERGY,			"Curvature Energy",				"%0.3f");
	DefineParameter(KEY_CURVATURE_MAX,				"Curvature Max",				"%0.3f");
	DefineParameter(KEY_MOMENTS_RATIO,				"Moments ratio",				"%0.3f");
	DefineParameter(KEY_BENDING_ENERGY,				"Bending Energy",				"%0.3f");
	DefineParameter(KEY_BRIGHTNESS_MOMENTS_DISP,	"Brightness moments",			"%0.3f");
	DefineParameter(KEY_SKELETON_PERIMETER_RATIO,	"Skeleton_Perimeter Ratio",		"%0.3f");
	DefineParameter(KEY_HORDES_RATIO,				"Hordes Ratio",				"%0.3f");

	return true;
}


void CShapeMeasureGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}

bool CShapeMeasureGroup::OnInitCalculation()
{
	return 	__super::OnInitCalculation();
}



