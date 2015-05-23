#if !defined(AFX_VTSLIDER_H__C8303D28_4616_4BBB_AAB2_852112D6E293__INCLUDED_)
#define AFX_VTSLIDER_H__C8303D28_4616_4BBB_AAB2_852112D6E293__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTSlider.h : Declaration of the CVTSlider ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTSlider : See VTSlider.cpp for implementation.

class CVTSlider : public COleControl
{
	DECLARE_DYNCREATE(CVTSlider)

#ifndef FOR_HOME_WORK
	DECLARE_INTERFACE_MAP();
#endif

// Constructor
public:
	CVTSlider();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTSlider)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL OnGetViewRect(DWORD dwAspect, LPRECTL pRect);
	//}}AFX_VIRTUAL

protected:

#ifndef FOR_HOME_WORK

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl3)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
		com_call SetDoc( IUnknown *punkDoc );
		com_call SetApp( IUnknown *punkVtApp );
		com_call OnOK( );
		com_call OnApply( );
		com_call OnCancel( );
	END_INTERFACE_PART(ActiveXCtrl)


	IUnknownPtr	m_ptrSite;
	IUnknownPtr	m_ptrApp;
#endif

protected:
	BOOL WriteToShellData();
	BOOL RestoreFromShellData();

// Implementation
protected:
	~CVTSlider();

	DECLARE_OLECREATE_EX(CVTSlider)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTSlider)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTSlider)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTSlider)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTSlider)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTSlider)
	long m_min;
	afx_msg void OnMinChanged();
	long m_max;
	afx_msg void OnMaxChanged();
	BOOL m_bCycled;
	afx_msg void OnCycledChanged();
	afx_msg BOOL GetShowTick();
	afx_msg void SetShowTick(BOOL bNewValue);
	afx_msg short GetTickFreq();
	afx_msg void SetTickFreq(short nNewValue);
	afx_msg long GetPosLo();
	afx_msg void SetPosLo(long nNewValue);
	afx_msg long GetPosHi();
	afx_msg void SetPosHi(long nNewValue);
	afx_msg BOOL GetShowNums();
	afx_msg void SetShowNums(BOOL bNewValue);
	afx_msg BOOL GetAutoUpdate();
	afx_msg void SetAutoUpdate(BOOL bNewValue);
	afx_msg BSTR GetLoKeyValue();
	afx_msg void SetLoKeyValue(LPCTSTR lpszNewValue);
	afx_msg BSTR GetHiKeyValue();
	afx_msg void SetHiKeyValue(LPCTSTR lpszNewValue);
	afx_msg BSTR GetName();
	afx_msg void SetName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetInnerRaised();
	afx_msg void SetInnerRaised(BOOL bNewValue);
	afx_msg BOOL GetInnerSunken();
	afx_msg void SetInnerSunken(BOOL bNewValue);
	afx_msg BOOL GetOuterRaised();
	afx_msg void SetOuterRaised(BOOL bNewValue);
	afx_msg BOOL GetOuterSunken();
	afx_msg void SetOuterSunken(BOOL bNewValue);
	afx_msg BOOL GetSingleSlide();
	afx_msg void SetSingleSlide(BOOL bNewValue);
	afx_msg void SetBothLimits(long nLow, long nHigh);
	afx_msg void StoreToData();
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTSlider)
	void FirePosChanged()
		{
			if( m_bUserChangeValue )
				FireEvent(eventidPosChanged,EVENT_PARAM(VTS_NONE));
		}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTSlider)
	dispidMin = 1L,
	dispidMax = 2L,
	dispidShowTick = 4L,
	dispidTickFreq = 5L,
	dispidPosLo = 6L,
	dispidPosHi = 7L,
	dispidShowNums = 8L,
	dispidCycled = 3L,
	dispidAutoUpdate = 9L,
	dispidLowKeyValue = 10L,
	dispidHighKeyValue = 11L,
	dispidName = 12L,
	dispidInnerRaised = 13L,
	dispidInnerSunken = 14L,
	dispidOuterRaised = 15L,
	dispidOuterSunken = 16L,
	dispidSingleSlide = 17L,
	dispidSetBothLimits = 18L,
	dispidStoreToData = 19L,
	dispidDisabled = 20L,
	dispidVisible = 21L,
	dispidSetFocus = 22L,
	dispidUseSystemFont = 23L,
	eventidPosChanged = 1L,
	//}}AFX_DISP_ID
	};
protected:
	enum Rects{	rectLine, rectSlider, rectNum, rectTick	};
	enum TrackStage	{trackNone, track1, track2, trackBoth};

	BOOL m_bShowNums, m_bShowTick;
	int	m_nTickFreq, m_nNumsCount;
	TrackStage	m_track;
	CRect	m_rectLast;
	long	m_nLo, m_nHi;
	int		m_iOffset;
	int		m_iStartedDelta;
	BOOL	m_bActive;
	bool	m_bOddSliderFirst;

	BOOL	m_bAutoUpdate;
	CString	m_strName;
	CString	m_strKeyValueLo;
	CString	m_strKeyValueHi;

	BOOL	m_bInnerRaised;
	BOOL	m_bInnerSunken;
	BOOL	m_bOuterRaised;
	BOOL	m_bOuterSunken;

	BOOL	m_bSingleSlide;
	BOOL	m_bUseSystemFont;
	

	void DrawLine( CDC *pdc, CRect rc );
	void DrawSlider( CDC *pdc, CRect rc, BOOL bActive );
	void DrawNum( CDC *pdc, CRect rc, int iNo );
	void DrawTick( CDC *pdc, CRect rc );
	CRect CalcRect( const CRect rc, const Rects kind, int ipos = 0 );

	void InvalidateSliderRect( int iSlider );

	int	CalcValueFromPos( int iposx );

	BOOL m_bSliderCross;

	virtual void Serialize( CArchive &ar );

	bool m_bUserChangeValue;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIDER_H__C8303D28_4616_4BBB_AAB2_852112D6E293__INCLUDED)
