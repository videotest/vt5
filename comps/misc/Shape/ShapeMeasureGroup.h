#pragma once
#include <vector>
#include <math.h>
using namespace std;

const POINTF pt0 = {1., 0.};
const POINTF pt15 = {cos(PI/12.),sin(PI/12.)};
const POINTF pt30 = {cos(PI/6.),sin(PI/6.)};
const POINTF pt45 = {cos(PI/4.),sin(PI/4.)};
const POINTF pt60 = {cos(PI/3.),sin(PI/3.)};
const POINTF pt75 = {cos(PI/12.*5.),sin(PI/12.*5.)};
const POINTF pt90 = {cos(PI/2.),sin(PI/2.)};
const POINTF pt105 = {cos(PI/12.*7.),sin(PI/12.*7.)};
const POINTF pt120 = {cos(PI/3.*2.),sin(PI/3.*2.)};
const POINTF pt135 = {cos(PI/4.*3),sin(PI/4.*3)};
const POINTF pt150 = {cos(PI/6.*5.),sin(PI/6.*5.)};
const POINTF pt165 = {cos(PI/12.*11.),sin(PI/12.*11.)};
const double tg1_8 = tan(PI/8.);
const double tg3_8 = tan(PI/8.*3);


class CShapeMeasureGroup:
			public CMeasParamGroupBase

{

	DECLARE_DYNCREATE(CShapeMeasureGroup)
	DECLARE_GROUP_OLECREATE(CShapeMeasureGroup);


	ENABLE_MULTYINTERFACE();
public:

	CShapeMeasureGroup(void);
	virtual ~CShapeMeasureGroup(void);

	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeffToUnits );
	virtual bool LoadCreateParam();
	void GetClassID( CLSID* pClassID );
	virtual bool OnInitCalculation();
};


double moments_ratio(IUnknown* object);
double brightness_moment(IUnknown* object);
double points_in_ed(IUnknown* object);
double hordes_ratio(IUnknown* object);
double distance_to_sl(POINTF sl);
double sceleton(IUnknown* object);
double polyEdge(IUnknown* object);

class CBrightnessMoment
{
	IUnknown* m_Unk;
	CImageWrp m_iwParent;
	CImageWrp m_object;
	int m_cx,m_cy;
	byte* m_gray;
	double m_I;
	byte* m_grayO;
	int m_n;
	bool m_bVirtual;
	POINT m_ptGC;
	CPoint offset;
	vector<POINT> m_ptsThr;
	void gravity_center();
	void generate_threshold();
	void open();
	void erode();
	void dilate();
	double equivalent_diam();
	double m_EqDiam;
public:
	CBrightnessMoment(IUnknown* object);
	~CBrightnessMoment();

	double GetDisperse();
	double GetEigenValuesRatio();
	double GetMoment();
	double points_in_ed();
	double hordes_ratio();

};

typedef double covariance_matrix [2][2];

class Matrix2x2
{
		covariance_matrix m_cm;
public:
	Matrix2x2()
	{
		ZeroMemory(m_cm[0], sizeof(double)*4);
	};
	Matrix2x2(const Matrix2x2& m2x2)
	{
		m_cm[0][0]=m2x2.m_cm[0][0];
		m_cm[1][0]=m2x2.m_cm[1][0];
		m_cm[0][1]=m2x2.m_cm[0][1];
		m_cm[1][1]=m2x2.m_cm[1][1];		
	};
	Matrix2x2(POINT v1, POINT v2)
	{
		m_cm[0][0]=v1.x*v2.x;
		m_cm[1][0]=v1.y*v2.x;
		m_cm[0][1]=v1.x*v2.y;
		m_cm[1][1]=v1.y*v2.y;	
	};

	Matrix2x2& operator+(const Matrix2x2& m)
	{
		m_cm[0][0]+= m.m_cm[0][0];
		m_cm[1][0]+=m.m_cm[1][0];
		m_cm[0][1]+=m.m_cm[0][1];
		m_cm[1][1]+=m.m_cm[1][1];
		return *this;
	};
	Matrix2x2& operator-(const Matrix2x2& m)
	{
		m_cm[0][0]-= m.m_cm[0][0];
		m_cm[1][0]-=m.m_cm[1][0];
		m_cm[0][1]-=m.m_cm[0][1];
		m_cm[1][1]-=m.m_cm[1][1];
		return *this;
	};
	Matrix2x2& operator=(const Matrix2x2& m)
	{
		m_cm[0][0]= m.m_cm[0][0];
		m_cm[1][0]=m.m_cm[1][0];
		m_cm[0][1]=m.m_cm[0][1];
		m_cm[1][1]=m.m_cm[1][1];
		return *this;
	};
	Matrix2x2& operator/=(int k)
	{
		m_cm[0][0]/= k;
		m_cm[1][0]/= k;
		m_cm[0][1]/= k;
		m_cm[1][1]/= k;
		return *this;
	};
	double GetAt(int row, int col)
	{
		return m_cm[row][col];
	};

	double ratio()
	{
		CString s;
		double r = -m_cm[1][0]*m_cm[0][1]/m_cm[1][1]/m_cm[1][1];
		r = r<0. ? -r:r;
		if(r>1.) return r = 1./r; 
		s.Format("%f\n", r);
		OutputDebugString(s);
		return r;
	}

};
