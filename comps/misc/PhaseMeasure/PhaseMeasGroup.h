#pragma once


class CPhaseMeasGroup : 
	public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CPhaseMeasGroup)
	DECLARE_GROUP_OLECREATE(CPhaseMeasGroup);
public:
	CPhaseMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CPhaseMeasGroup();
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();
	virtual bool OnInitCalculation();

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	

protected:
	// [vanek] 18.12.2003: наклонные хорды
	bool m_bEnableNonVerticalChord;
	int get_min_distance( int nx1, int ny1, int *py2, int ny2size, int *px_min = 0 );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
