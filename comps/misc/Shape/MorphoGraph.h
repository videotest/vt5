#pragma once
#include "resource.h"

enum CURVTYPE
{
	NA , Curve, _Curve, Peak, _Peak
};
struct CURVDESCR
{
	int size;
	int center;
	double value;
	double r;
	int begin;
	int end;
	CURVTYPE type;
	void CalcType(int* points, double R);
	void SetType();
	
};
struct GPARAM
{
	long key;
	double value;
};

class GDescr:
	public CArray<GPARAM>
{
public:
	void Serialize(CArchive& ar);
	GDescr& operator=(const GDescr&);
	GDescr& operator=(double d);
	GDescr& operator+=(GDescr&);
	GDescr& operator/=(double d);
	GPARAM  Find(long key);
};

class CStringX:public CArray<double>
{
private:
	GDescr m_gDescr;
	int m_nClassPos;
public:
	CStringX(){m_nClassPos =-1;};
	~CStringX();
	CStringX(const CStringX&);
	CStringX& operator+=(double tc);
	double* GetBuffer(){return GetData();};
	CStringX& operator=(const CStringX&);
	/*CStringX& operator+(const CStringX&);*/
	bool operator==( CStringX&);
	void Serialize(CArchive& ar);
	GDescr& GlobalParameters();
	friend class ShapeClass;
};

class CurveDescr:
	public CList<CURVDESCR>
{
	double R;
	POSITION find_useless(double sf);
	CStringX m_strDescr; 
	CString m_strClass;
	int m_nClass;
	GDescr m_gDesc;
public:
	CurveDescr(){m_strClass = "Unknown", m_nClass =-1;};
	CurveDescr(CString desc){};
	CStringX GenerateString();
	CStringX GenerateStringEx(int*,int, double);
	void Compact(double R);
	void SetSignificance(double sf);
	CURVDESCR& GetNextCyclic(POSITION& pos);
	CURVDESCR& GetPrevCyclic(POSITION& pos);
	CString ToString();
	CStringX& GetString(){return m_strDescr;};
	int operator-(CurveDescr&);
	void SetClassName(const CString& strName){m_strClass = strName;};
	CString GetClassName(){return m_strClass;};
	int GetClass(){return m_nClass;};
	void SetClass(int n){m_nClass =n;};
	void SetGlobalParameters(GDescr&);
	void CalculateAddons();
	friend class CCurvatureBin;
	friend class CMorphoGraph;
};

class CCurvatureBin
{
	int m_cx, m_cy, m_N;
	/*byte *m_image;*/
	//byte* m_zeroLine;
	int* m_points, *m_plus, *m_minus;
	int m_se;
	double m_weightY, m_weightX;
	double m_min, m_max;
	void pad(int delta);
	void unpad(int delta);
	void sub(int* result, int* pts1, int* pts2, int n);
	CCurvatureBin& operator=(const CCurvatureBin& cb);
	void shift();
	void combine();
	void get_real(double* r);
	

public:
	/*void to_mat();
	void to_mat(int* pts, int n);
	void to_mat(double* pts, int n);*/
	CCurvatureBin(CCurvatureBin& cb);	
	CCurvatureBin(void);
	~CCurvatureBin(void);
	void Create(UINT cx, UINT cy);
	void Attach(double* points, UINT n);
	void GetSizes(UINT* x, UINT* y);
	//inline byte* operator[](int i){return m_image + i*m_cy;};
	void GetDescriptor(CurveDescr& cd);

	//Морфологические операции; n-произв. элемент 
	void Erode(CCurvatureBin& cb, int n);
	void Dilate(CCurvatureBin& cb, int n);
	void Open(CCurvatureBin& cb, int n);
	void Close(CCurvatureBin& cb, int n);
};

class CMorphoGraph
{
private:
	double* m_ppoints;
	UINT m_size;
	CurveDescr m_desc;
	CCurvatureBin m_curv;
public:
	CMorphoGraph(void);
	~CMorphoGraph(void);
	void Init(	double* graphic,//график кривизны
				int n,//размер graphic
				int x,//желаемые размеры бинарного изображения
				int y, 
				int sence //чувствительность- относительный размер произв. элемента в тысячных долях
				);

	void GetDescriptor(CurveDescr* desc);
	void Learn(CString& strDes);
	CString GetClass(CString& strDes);
	void WriteToTextFile(CString& strDes, CString& path);

protected:
	void FindPeaks();
	void GenerateDescriptor(CurveDescr* desc);

};

static TCHAR GetLetter(double value)
{	
	static const double A =0.5;
	static const double B =1.; 
	static const double C =1.5;
	static const double D =2.5;
	if(value<-D) return -4;	
	if(value<-C && value>=-D) return -3;	
	if(value<-B && value>=-C) return -2;	
	if(value<-A && value>=-B) return -1;	
	if(value>=-A && value<=A) return 0;
	if(value>A && value<=B) return 1;
	if(value>B && value<=C) return 2;
	if(value>C && value<=D) return 3; 
	if(value>D) return 4;


	return '?';
};




static CString SimpledString(const CString& str);





