#if !defined(AFX_VTCHECKBOXCTL_H__A8095779_B5EA_4B1C_B33A_CCF841F99F57__INCLUDED_)
#define AFX_VTCHECKBOXCTL_H__A8095779_B5EA_4B1C_B33A_CCF841F99F57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTCheckBoxCtl.h : Declaration of the CVTCheckBoxCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl : See VTCheckBoxCtl.cpp for implementation.

class CVTCheckBoxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTCheckBoxCtrl)

// Constructor
public:
	CVTCheckBoxCtrl();
	virtual void OnGetControlInfo(LPCONTROLINFO pControlInfo);
	virtual void OnMnemonic(LPMSG pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTCheckBoxCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	virtual DWORD GetControlFlags();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTCheckBoxCtrl();

	DECLARE_OLECREATE_EX(CVTCheckBoxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTCheckBoxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTCheckBoxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTCheckBoxCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTCheckBoxCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);				
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTCheckBoxCtrl)
	afx_msg LPFONTDISP GetTextFont();
	afx_msg void SetTextFont(LPFONTDISP newValue);
	afx_msg BSTR GetCaption();
	afx_msg void SetCaption(LPCTSTR lpszNewValue);
	afx_msg OLE_COLOR GetUncheckTextColor();
	afx_msg void SetUncheckTextColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetCheckTextColor();
	afx_msg void SetCheckTextColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetBackgroundColor();
	afx_msg void SetBackgroundColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg short GetChecked();
	afx_msg void SetChecked(short bNewValue);
	afx_msg BOOL GetInnerRaised();
	afx_msg void SetInnerRaised(BOOL bNewValue);
	afx_msg BOOL GetInnerSunken();
	afx_msg void SetInnerSunken(BOOL bNewValue);
	afx_msg BOOL GetOuterRaised();
	afx_msg void SetOuterRaised(BOOL bNewValue);
	afx_msg BOOL GetOuterSunken();
	afx_msg void SetOuterSunken(BOOL bNewValue);
	afx_msg BOOL GetNewDrawMode();
	afx_msg void SetNewDrawMode(BOOL bNewValue);	
	afx_msg BOOL GetAppearance3D();
	afx_msg void SetAppearance3D(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTCheckBoxCtrl)
	void FireOnCheck(short nCheck)
		{FireEvent(eventidOnCheck,EVENT_PARAM(VTS_I2), nCheck);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTCheckBoxCtrl)
	dispidTextFont = 1L,
	dispidCaption = 2L,
	dispidUncheckTextColor = 3L,
	dispidCheckTextColor = 4L,
	dispidBackgroundColor = 5L,
	dispidDisabled = 6L,
	dispidVisible = 7L,
	dispidChecked = 8L,
	dispidInnerRaised = 9L,
	dispidInnerSunken = 10L,
	dispidOuterRaised = 11L,
	dispidOuterSunken = 12L,
	dispidNewDrawMode = 13L,
	dispidAppearance3D = 14L,
	dispidSetFocus = 15L,	
	dispidUseSystemFont = 16L,
	eventidOnCheck = 1L,
	//}}AFX_DISP_ID
	};

protected:
	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;

	OLE_COLOR	m_textCheckColor;		
	OLE_COLOR	m_textUncheckColor;
	OLE_COLOR	m_backgroundColor;	

	CString		m_strCaption;

	short		m_nChecked;

	BOOL		m_bInnerRaised;
	BOOL		m_bInnerSunken;
	BOOL		m_bOuterRaised;
	BOOL		m_bOuterSunken;

	BOOL		m_bDisabled;	

	bool CanProcessInput()	{ return (!m_bDisabled && GetVisible() ); }

	void UserChangeValue( short bNewValue );

	void InitDefaults();
	
	void InvalidateAndSetModifiedFlag();

	bool CreateFont( CFont* pFont, HDC hDC );

	CImageList	m_ImageList;

	CRect GetBorderRect( CRect rcClient );
	CRect GetCheckRect( CRect rcClient );
	CRect GetTextRect( CRect rcClient, CDC* pdc = 0 );

	bool	m_bPressed;

	BOOL	m_bNewDrawMode;
	BOOL	m_bAppearance3D;

	bool	IsPointInTarget( CPoint point );
	
	
	//Acceleration
	HACCEL	m_hAccel;
	char	m_cAccelKey;
	
	
	BOOL 	FindAccelKey();
	bool IsAccelChar( char ch );

	BOOL	m_bUseSystemFont;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCHECKBOXCTL_H__A8095779_B5EA_4B1C_B33A_CCF841F99F57__INCLUDED)
