#if !defined(AFX_HISTAXCTL_H__06FDDE65_F981_11D3_A0C1_0000B493A187__INCLUDED_)
#define AFX_HISTAXCTL_H__06FDDE65_F981_11D3_A0C1_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// HistAXCtl.h : Declaration of the CHistAXCtrl ActiveX Control class.
#include "histint.h"
#include "\vt5\awin\misc_ptr.h"

#define HistMaxXBound	256

/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl : See HistAXCtl.cpp for implementation.

class CHistAXCtrl : public COleControl,
    		       	public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CHistAXCtrl)

// Constructor
public:
	CHistAXCtrl();

	void DrawNormalHist(CDC* pdc, CRect rc);
	void InvalidateHist(long nLow = 0, long nHigh = 0);
	void DefaultFill();



	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)

	DECLARE_INTERFACE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CHistAXCtrl();

	GUARD_DECLARE_OLECREATE_CTRL(CHistAXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CHistAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CHistAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CHistAXCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CHistAXCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CHistAXCtrl)
	BOOL m_innerRaised;
	afx_msg void OnInnerRaisedChanged();
	BOOL m_outerRaised;
	afx_msg void OnOuterRaisedChanged();
	BOOL m_innerSunken;
	afx_msg void OnInnerSunkenChanged();
	BOOL m_outerSunken;
	afx_msg void OnOuterSunkenChanged();
	short m_paintPanes;
	afx_msg void OnPaintPanesChanged();
	long m_lowThreshold;
	afx_msg void OnLowThresholdChanged();
	long m_highThreshold;
	afx_msg void OnHighThresholdChanged();
	double m_yZoom;
	afx_msg void OnYZoomChanged();
	afx_msg BSTR GetCnvName();
	afx_msg short GetPanesCount();
	afx_msg void SetPane(short nPane);
	afx_msg BSTR GetPaneName(short nPane);
	afx_msg OLE_COLOR GetPaneColor(short nPane);
	afx_msg void AddPane(short nPane);
	afx_msg void RemovePane(short nPane);
	afx_msg void ZoomIn(double nStep);
	afx_msg void ZoomOut(double nStep);
	afx_msg BOOL Lock(BOOL bLock);
	afx_msg void SetTarget(LPDISPATCH pdispTarget);
	afx_msg void SetNumPhases(long nPhases);
	afx_msg void SetPhaseLowThreshold(long nPhase, long fThreshold);
	afx_msg void SetPhaseHighThreshold(long nPhase, long fThreshold);
	afx_msg void SetPhaseColor(long nPhase, OLE_COLOR clPhase);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CHistAXCtrl)
	void FireOnNewCalc()
		{FireEvent(eventidOnNewCalc,EVENT_PARAM(VTS_NONE));}
	void FireOnPhaseClick(long nPhase)
		{FireEvent(eventidOnPhaseClick,EVENT_PARAM(VTS_I4), nPhase);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CHistAXCtrl)
	dispidInnerRaised = 1L,
	dispidOuterRaised = 2L,
	dispidInnerSunken = 3L,
	dispidOuterSunken = 4L,
	dispidPaintPanes = 5L,
	dispidLowThreshold = 6L,
	dispidHighThreshold = 7L,
	dispidYZoom = 8L,
	dispidGetCnvName = 9L,
	dispidGetPanesCount = 10L,
	dispidSetPane = 11L,
	dispidGetPaneName = 12L,
	dispidGetPaneColor = 13L,
	dispidAddPane = 14L,
	dispidRemovePane = 15L,
	dispidZoomIn = 16L,
	dispidZoomOut = 17L,
	dispidLock = 18L,
	dispidSetTarget = 19L,
	dispidSetNumPhases = 20L,
	dispidSetPhaseLowThreshold = 21L,
	dispidSetPhaseHighThreshold = 22L,
	dispidSetPhaseColor = 22L,
	eventidOnNewCalc = 1L,
	eventidOnPhaseClick = 2L,
	//}}AFX_DISP_ID
	};

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	DWORD _GetDifferenceColor(DWORD dwColor);
	void _ManageHistState(IUnknown* punkActiveView = 0);
	void _KillCurCnvInfo();
	void _KillHistogramm();
	void _CalcHistogramm(IUnknown* punkImage);
	int _GetActivePane();

private:
	IAXSitePtr		m_sptrSite;
	IApplicationPtr	m_sptrApp;
	bool			m_bDefaultFill;
	CString			m_strCnvName;
	BSTR*			m_pbstrPanesNames;
	DWORD*			m_pdwPanesColors;
	DWORD*			m_pdwGradientColors;
	BOOL*			m_pbPaintPanes;
	int				m_nPanes;
	long			(*m_ppnHistData)[HistMaxXBound];
	long*			m_pnMaxHistData;
	CRect			m_rcLastDraw;
	long			m_lCachedLowTh;
	long			m_lCachedHighTh;
	BOOL			m_bLock;
	IImagePtr				m_ptrTargetImage;
	IProvideHistColorsPtr	m_ptrProvideHistColors;

	long			(*m_ppnBrightData)[HistMaxXBound];
	long*			m_pnMaxBrightData;
	float *ppnHistBrightR;
	float *ppnHistBrightG;
	float *ppnHistBrightB;

	long m_nPhases;
	_ptr_t<long> m_PhaseLow;
	_ptr_t<long> m_PhaseHigh;
	_ptr_t<OLE_COLOR> m_PhaseColor;

	void RegisterDoc( IUnknown* punkDoc );
	void UnRegisterDoc( );

	IUnknownPtr m_ptrActiveDoc;

	IUnknown* GetActiveDocument();
	IUnknown* GetActiveView();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTAXCTL_H__06FDDE65_F981_11D3_A0C1_0000B493A187__INCLUDED)
