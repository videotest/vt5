#pragma once
class CurveDescr;
class ShapeClass;
#include "morphograph.h"
/*typedef CArray<double> DblStr;*/ 

class GDescr;


class CClassStorage
{
private:
	CString m_strName;
	double m_dblTshld;

	double distance(CurveDescr& descr1, CurveDescr& descr2);
	CFile m_file;
	CList<ShapeClass*> m_classes;
	ShapeClass* find(int n);
	ShapeClass* find(CString& s);
	void SerializeClasses(CArchive& ar);
	CArray<long> m_arrParams; 
	bool m_bNewBase;
public:
	CClassStorage(void);
	~CClassStorage(void);
	void Write(CString& path);
	BOOL Init(CString& path);
	void AddDescriptor(CurveDescr& descr);
	ShapeClass* AddClass(int num, CString& className);
	void RemoveClass(CString& className, int num =-1);
	void Recalculate();
	void Decalculate();
	int ClarifyClass(CurveDescr& descr);
	bool CheckParameter(long key);
	int GetParamsCount(){return m_arrParams.GetSize();}
	long GetParameterAt(int i){return m_arrParams.GetAt(i);};
	void AddParameter(long key){m_arrParams.Add(key);};
	bool IsNew(){return m_bNewBase;};
protected:
	void Serialize(CArchive& ar);
};

class ParzenWindows;
class ShapeClass
{
	CArray<ShapeClass*> m_subClasses;
	CStringX m_Gauge;
	CString m_strName;
	ParzenWindows* m_pw; 
	CArray<CStringX> m_descrs;
	double* m_dt;
	GDescr m_weights, m_mean;
	double m_dblTshld;
	int m_nClass;
	int m_nDscLength;
	double correlation(double* s1, CStringX& s2);
	double subtraction(double* s1, CStringX& s2);
	POINT find_min();
	inline double abs(double);
	int m_nDTSize;
	bool m_bWeights;
protected:
	void K_Mean();

	// dist - method for calculating distance between syntaxic descriptors
	double dist(double* s1, CStringX& s2);
	double dist(CStringX& s1, CStringX& s2);
	double distWW(CStringX& s1, CStringX& s2);
	void CalcDistTableWithWeights();
	double disp();
	CStringX combine_to_subclass(CStringX& s1, CStringX& s2);
	
public:
	ShapeClass(){	m_nDscLength =0;m_dt=0;	m_bWeights = false;m_pw=0;};
	ShapeClass(int num, CString& className);
	~ShapeClass();
	void Clusterize();
	void DeClusterize();
	bool AddDescriptor(CStringX& str, bool bSubclassAllowed);
	void SetSubclassThreshold(double thr );
	/*void GetGauge(double* gauge);*/
	void GetGauge(CStringX& gauge);
	double FindNearest(CStringX& str);
	double FindNearestExact(CStringX& str);
	CString GetClassName();
	int GetClassNum();
	void Serialize(CArchive& ar);
	void RecalculateGauge();
	int GetDescriptorLength();
	void CalcWeights();
	int AddDescriptor(CStringX& str);
	void SetClassPos(int pos);
	int GetClassPos(){return m_descrs[0].m_nClassPos;};
	void AssignWeights(GDescr& weights);

};

double curvEnergy(double* curv, int n);
double peakRatio(double* curv, int n);
double bendingEnergy(double* curv, int n);
double curvMaxima(double* curv, int n);
double curvMiniima(double* curv, int n);

class CWrongFileException:
	public CException
{};

class CNewFileException:
	public CException
{};
