#if !defined(AFX_OPTICMEASUREMAN_H__0B6ADE84_821A_4B60_99FD_023FEA189AA4__INCLUDED_)
#define AFX_OPTICMEASUREMAN_H__0B6ADE84_821A_4B60_99FD_023FEA189AA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpticMeasureMan.h : header file
//




class COptDensSet
{
	CMap<double,double,double,double&> m_mapData;
	//static double s_dMaxDiff;
	double m_fMedium;
public:
	COptDensSet() {m_fMedium = 0.;}
	void Add(double d) 
	{
		double dd = 0;
		if(m_mapData.Lookup(d, dd) == FALSE)
			m_mapData.SetAt(d, d);

		_CalcMedium();
	};
	bool RemoveVal(double dValToRemove)
	{
		double dd = 0;
		if(m_mapData.Lookup(dValToRemove, dd) == TRUE)
		{
			m_mapData.RemoveKey(dValToRemove);
			_CalcMedium();
			return true;
		}
		return false;
	};
	double GetMedium(){return m_fMedium;};

protected:
	void _CalcMedium();
};

#include <map>
typedef std::map<double,COptDensSet*> MAPA;


class CExampleOptDens : public CObject
{
public:
	// Adds one sample and sets nominal if its not set.
	void AddSample(double dCalc, double dNominal = 0.);
	long GetSize(){return (long)m_mapCalcOptDens.size();};

	MAPA m_mapCalcOptDens;

};


/////////////////////////////////////////////////////////////////////////////
// COpticMeasureMan command target
class COpticMeasureMan : public CCmdTarget,
						 public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(COpticMeasureMan)

	COpticMeasureMan();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpticMeasureMan)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~COpticMeasureMan();

	// Generated message map functions
	//{{AFX_MSG(COpticMeasureMan)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


	BEGIN_INTERFACE_PART(OpticMan, IOpticMeasureMan)
		//com_call DoMeasOptic(long x, long y, long nBrightness, double* pfIntOptDens);
		com_call GetMeasOpticStuff(long* pnMethod, BYTE* pnAvgBack, IUnknown** ppunkBack, IUnknown** ppunkDark);
		com_call CalibrateOpticalDensity(double* pfOptDens);
	END_INTERFACE_PART(OpticMan)

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(COpticMeasureMan)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(COpticMeasureMan)
	short m_nMethod;
	afx_msg void OnMethodChanged();
	afx_msg void SetDarkField(LPDISPATCH pImage);
	afx_msg void SetBackground(LPDISPATCH pImage);
	afx_msg void Free();
	afx_msg double AddExample(double fDensity, LPDISPATCH dispImage);
	afx_msg bool RemoveSampleCalc(double dNominal, double dCalcToRemove);
	afx_msg long CalkAvgBack(LPDISPATCH dispImage);
	afx_msg double Interpolate(double fVal);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


	double LinearInterpolate(double Val);
	double SquareInterpolate(double Val);
	void OnEndInputExamples();
	double CalcExampleOpticalDensity(long iNum, CImageWrp img, double dNominal, bool bChangeNominal = false);
	double GetDarkFieldPoint(long x, long y);
	double GetBackgroundPoint(long x, long y);
	void Deinit();
	//void DoMeasOptic(double &fIntOptDens, long x, long y, long nBrightness);
	void Init();
	void CalibrateOpticalDensity(double &fOptDens);


	long				m_nAvrBack;
	CImageWrp			m_pimgDarkField;
	CImageWrp			m_pimgBackground;
private:
	CExampleOptDens		m_ExampleOptDens;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTICMEASUREMAN_H__0B6ADE84_821A_4B60_99FD_023FEA189AA4__INCLUDED_)
