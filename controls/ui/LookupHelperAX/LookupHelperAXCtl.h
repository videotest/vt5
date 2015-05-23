#if !defined(AFX_LOOKUPHELPERAXCTL_H__168C941B_BE54_4894_A23D_80D014D0B9E4__INCLUDED_)
#define AFX_LOOKUPHELPERAXCTL_H__168C941B_BE54_4894_A23D_80D014D0B9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LookupHelperAXCtl.h : Declaration of the CLookupHelperAXCtrl ActiveX Control class.

class CCurveBox;

/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl : See LookupHelperAXCtl.cpp for implementation.

class CLookupHelperAXCtrl : public COleControl,
    		       			public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CLookupHelperAXCtrl)

// Constructor
public:
	CLookupHelperAXCtrl();

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)

	DECLARE_INTERFACE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLookupHelperAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	virtual void Serialize(CArchive& ar);

// Implementation
protected:
	~CLookupHelperAXCtrl();

	GUARD_DECLARE_OLECREATE_CTRL(CLookupHelperAXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CLookupHelperAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CLookupHelperAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CLookupHelperAXCtrl)		// Type name and misc status

// Message maps
	afx_msg LRESULT OnCurveChange(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CLookupHelperAXCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CLookupHelperAXCtrl)
	short m_nBorder;
	afx_msg void OnBorderChanged();
	BOOL m_bShowGrid;
	afx_msg void OnShowGridChanged();
	short m_nMode;
	afx_msg void OnModeChanged();
	short m_nCurveNum;
	afx_msg void OnCurveNumChanged();
	short m_nBrightBrightness;
	afx_msg void OnBrightBrightnessChanged();
	short m_nBrightGamma;
	afx_msg void OnBrightGammaChanged();
	short m_nBrightContrast;
	afx_msg void OnBrightContrastChanged();
	short m_nRedBrightness;
	afx_msg void OnRedBrightnessChanged();
	short m_nGreenBrightness;
	afx_msg void OnGreenBrightnessChanged();
	short m_nBlueBrightness;
	afx_msg void OnBlueBrightnessChanged();
	short m_nRedGamma;
	afx_msg void OnRedGammaChanged();
	short m_nGreenGamma;
	afx_msg void OnGreenGammaChanged();
	short m_nBlueGamma;
	afx_msg void OnBlueGammaChanged();
	short m_nRedContrast;
	afx_msg void OnRedContrastChanged();
	short m_nGreenContrast;
	afx_msg void OnGreenContrastChanged();
	short m_nBlueContrast;
	afx_msg void OnBlueContrastChanged();
	BOOL m_bActionMode;
	afx_msg void OnActionModeChanged();
	afx_msg void ResetState();
	afx_msg void Trace(LPCTSTR szOutString);
	afx_msg void Cancel();
	afx_msg void Apply();
	afx_msg void LoadState(LPCTSTR szSection);
	afx_msg void SaveState(LPCTSTR szSection);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CLookupHelperAXCtrl)
	void FireUpdateExternalControls()
		{FireEvent(eventidUpdateExternalControls,EVENT_PARAM(VTS_NONE));}
	void FireOnAction()
		{FireEvent(eventidOnAction,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CLookupHelperAXCtrl)
	dispidBorder = 1L,
	dispidShowGrid = 2L,
	dispidMode = 3L,
	dispidCurveNum = 4L,
	dispidBrightBrightness = 5L,
	dispidBrightGamma = 6L,
	dispidBrightContrast = 7L,
	dispidRedBrightness = 8L,
	dispidGreenBrightness = 9L,
	dispidBlueBrightness = 10L,
	dispidRedGamma = 11L,
	dispidGreenGamma = 12L,
	dispidBlueGamma = 13L,
	dispidRedContrast = 14L,
	dispidGreenContrast = 15L,
	dispidBlueContrast = 16L,
	dispidActionMode = 17L,
	dispidResetState = 18L,
	dispidTrace = 19L,
	dispidCancel = 20L,
	dispidApply = 21L,
	dispidLoadState = 22L,
	dispidSaveState = 23L,
	eventidUpdateExternalControls = 1L,
	eventidOnAction = 2L,
	//}}AFX_DISP_ID
	};

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	void SetTarget(IUnknown* punkActiveView = 0);
	void _SetDefaultLookupTable(IUnknown* punkDoc);
	void _InvalidateActiveView();

private:
	IAXSitePtr		m_sptrSite;
	IApplicationPtr	m_sptrApp;
	byte*			m_pLookup;
	long			m_nLookupSize;
	bool			m_bRegistered;
	CCurveBox*		m_pCurve;
	CCurveBox*		m_pCurvePreview;
	INamedDataPtr	m_sptrND;

	bool			m_bCancel;
	byte*			m_pbyteLookupBackUp;
	long			m_nSizeBackUp;

	byte*			m_pbyteLookupStateBackUp;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOOKUPHELPERAXCTL_H__168C941B_BE54_4894_A23D_80D014D0B9E4__INCLUDED)
