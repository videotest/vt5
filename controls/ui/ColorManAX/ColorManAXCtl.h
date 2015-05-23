#if !defined(AFX_COLORMANAXCTL_H__5DB73BC3_F590_11D3_A0BC_0000B493A187__INCLUDED_)
#define AFX_COLORMANAXCTL_H__5DB73BC3_F590_11D3_A0BC_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ColorManAXCtl.h : Declaration of the CColorManAXCtrl ActiveX Control class.


struct CCInfo
{
	BYTE		nPanes;
	BSTR*		pszPanesCaptions;
	DWORD*		pdwFlags;
	DWORD*		pdwPanesColors;
};

struct IntervalUndo
{
	BYTE		nModel;
	WORD*		pnLoRanges;
	WORD*		pnHiRanges;
};

struct IntervalsSet
{
	WORD**		ppnLoRanges;
	WORD**		ppnHiRanges;
	CTypedPtrList<CPtrList, IntervalUndo*> listCCIntervalsUndo;
};


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl : See ColorManAXCtl.cpp for implementation.

class CColorManAXCtrl : public COleControl,
    		       		public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CColorManAXCtrl)

// Constructor
public:
	CColorManAXCtrl();

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorManAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	virtual void OnFinalRelease();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CColorManAXCtrl();

	GUARD_DECLARE_OLECREATE_CTRL(CColorManAXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CColorManAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CColorManAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CColorManAXCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CColorManAXCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnCloseCB();
	afx_msg void OnRadioChange(UINT nID);
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CColorManAXCtrl)
	BOOL m_innerSunken;
	afx_msg void OnInnerSunkenChanged();
	BOOL m_outerRaised;
	afx_msg void OnOuterRaisedChanged();
	BOOL m_outerSunken;
	afx_msg void OnOuterSunkenChanged();
	BOOL m_innerRaised;
	afx_msg void OnInnerRaisedChanged();
	long m_hiRange;
	afx_msg void OnHiRangeChanged();
	long m_loRange;
	afx_msg void OnLoRangeChanged();
	BOOL m_addInterval;
	afx_msg void OnAddIntervalChanged();
	afx_msg BOOL IsPaneCycled(short nPane);
	afx_msg void ResetIntervalsSet(short nIS);
	afx_msg void UndoChangeIntervals();
	afx_msg BOOL IsUndoAvail();
	afx_msg void AddIntervalsSet();
	afx_msg void RemoveIntervalsSet(short nIntervalsSet);
	afx_msg void SetActiveIntervalsSet(short nIntervalsSet);
	afx_msg long GetLoRangeOfPane(short nClass, short nPane);
	afx_msg long GetHiRangeOfPane(short nClass, short nPane);
	afx_msg long GetPanesCount();
	afx_msg void Fake(LPCTSTR szComment);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CColorManAXCtrl)
	void FireOnPaneChange(short nPane)
		{FireEvent(eventidOnPaneChange,EVENT_PARAM(VTS_I2), nPane);}
	void FireOnChangeHiRange(long nValue)
		{FireEvent(eventidOnChangeHiRange,EVENT_PARAM(VTS_I4), nValue);}
	void FireOnChangeLoRange(long nValue)
		{FireEvent(eventidOnChangeLoRange,EVENT_PARAM(VTS_I4), nValue);}
	void FireOnManageColorInterval()
		{FireEvent(eventidOnManageColorInterval,EVENT_PARAM(VTS_NONE));}
	void FireOnChangeUndoState()
		{FireEvent(eventidOnChangeUndoState,EVENT_PARAM(VTS_NONE));}
	void FireOnColorModelChange()
		{FireEvent(eventidOnColorModelChange,EVENT_PARAM(VTS_NONE));}
	void FireOnChangeWorkingImage(LPDISPATCH pdispNewImage)
		{FireEvent(eventidOnChangeWorkingImage,EVENT_PARAM(VTS_DISPATCH), pdispNewImage);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CColorManAXCtrl)
	dispidInnerSunken = 1L,
	dispidOuterRaised = 2L,
	dispidOuterSunken = 3L,
	dispidInnerRaised = 4L,
	dispidHiRange = 5L,
	dispidLoRange = 6L,
	dispidAddInterval = 7L,
	dispidIsPaneCycled = 8L,
	dispidResetIntervalsSet = 9L,
	dispidUndoChangeIntervals = 10L,
	dispidIsUndoAvail = 11L,
	dispidAddIntervalsSet = 12L,
	dispidRemoveIntervalsSet = 13L,
	dispidSetActiveIntervalsSet = 14L,
	dispidGetLoRangeOfPane = 15L,
	dispidGetHiRangeOfPane = 16L,
	dispidGetPanesCount = 17L,
	dispidFake = 18L,
	eventidOnPaneChange = 1L,
	eventidOnChangeHiRange = 2L,
	eventidOnChangeLoRange = 3L,
	eventidOnManageColorInterval = 4L,
	eventidOnChangeUndoState = 5L,
	eventidOnColorModelChange = 6L,
	eventidOnChangeWorkingImage = 7L,
	//}}AFX_DISP_ID
	};

	DECLARE_INTERFACE_MAP();

protected:
	void _InitCCcombo();
	void _ManageEnabled();
	void _Layout();
	void _LoadState();
	void _StoreState();
	int _GetActiveRadio();
	void _KillCCArray();
	void _SetupInterval(int nModel);
	void _SetPaneInterval(int nPane, int nModel = -1, bool bSetText = true);
	void _AdjustLimits(WORD wordVal, WORD &iLow, WORD &iHigh, bool bCycled = false);
	void _ManageColorInterval(IUnknown* punkProvideColors);
	void _CopyToUndo(int nModel);
	bool _IsUndoAvail();
	void _SetEditors(WORD nLo, WORD nHi);
	int _AddIntervalsSet();
	int _GetIntervalsSetsCount();
	void _DeleteIntervalsSet(int nIS);
	void _ChangeCurLo(WORD nLo);
	void _ChangeCurHi(WORD nHi);

	void _FireChangeWorkingImage();

	int _GetCnvNumberInCB(IUnknown* punkImage);

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

private:
	IAXSitePtr		m_sptrSite;
	IApplicationPtr	m_sptrApp;

	CComboBoxEx*	m_pcbColorModel;
	CImageList		m_imgCC;

	CEdit*			m_pEdit1;
	CEdit*			m_pEdit2;

	CSpinButtonCtrl* m_pSpin1;
	CSpinButtonCtrl* m_pSpin2;

	CButton*		m_pRadio1;
	CButton*		m_pRadio2;
	CButton*		m_pRadio3;

	CStatic*		m_pStaticWorkImage;

	CTypedPtrArray<CPtrArray, IntervalsSet*>	m_arrIntervals;
	CTypedPtrArray<CPtrArray, CCInfo*>			m_arrCCInfo;

	

	long			m_nLastCBSel;
	long			m_nLastRadioSel;

	long			m_nCurIntervalsSet;

	IImagePtr		m_sptrWorkingImage;

	CTypedPtrArray<CPtrArray, IImagePtr>	m_arrConvertedImages;

	long			m_nInfiniteImageIdx; //at end remove all imgs except this one
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORMANAXCTL_H__5DB73BC3_F590_11D3_A0BC_0000B493A187__INCLUDED)
