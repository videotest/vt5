#if !defined(AFX_CONTEXTAXCTL_H__224F6C55_DDB8_44F0_9021_636A3BB9B2BE__INCLUDED_)
#define AFX_CONTEXTAXCTL_H__224F6C55_DDB8_44F0_9021_636A3BB9B2BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContextAXCtl.h : Declaration of the CContextAXCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl : See ContextAXCtl.cpp for implementation.
#include "ContextWrp.h"
#include "PushButton.h"
#include "Factory.h"


class CContextAXCtrl :	public COleControl,
						public CEventListenerImpl,
						public CMsgListenerImpl
{
	DECLARE_DYNCREATE(CContextAXCtrl)
	ENABLE_MULTYINTERFACE();


// Constructor
public:
	CContextAXCtrl();

	void SetViewType(BOOL bShow = TRUE);

	void SetNumeric();
	void RemoveNumeric();
	void Expand(bool bExpand = true);
	bool CalcLayout();
	bool CreateButtons();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnFontChanged();
	virtual void OnForeColorChanged();
	virtual void OnBackColorChanged();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CContextAXCtrl();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	void OnNotifyViewType( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	void OnNotifyViewObject( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	// override from msg listner
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );

	void AttachToView(IView * pView = 0);


	void RemoveTempFiles();

	GUARD_DECLARE_OLECREATE_CTRL(CContextAXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CContextAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CContextAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CContextAXCtrl)		// Type name and misc status

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite(IUnknown * punkVtApp, IUnknown * punkSite);
		com_call GetName(BSTR * pbstrName);	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)

	BEGIN_INTERFACE_PART(Viewer, IMenuInitializer)
		com_call OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU * phOutMenu);
	END_INTERFACE_PART(Viewer);

	BEGIN_INTERFACE_PART(ContextView, IContextView)
		com_call GetView(IUnknown ** ppunkView);
		com_call SetView(IUnknown * punkView);
		com_call SetReadOnlyMode(BOOL bMode);
		com_call GetReadOnlyMode(BOOL * pbMode);
	END_INTERFACE_PART(ContextView);


	DECLARE_INTERFACE_MAP()

// Message maps
	//{{AFX_MSG(CContextAXCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClickButtonType();
	afx_msg void OnClickButtonObj();
	afx_msg void OnClickButtonNumberOn();
	afx_msg void OnClickButtonNumberOff();
	afx_msg void OnClickButtonExpand();
	afx_msg void OnClickButtonCollapse();
	afx_msg void OnClickButtonAbc();
	afx_msg void OnClickButtonTime();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CContextAXCtrl)
	BOOL m_bEnableTitleTips;
	afx_msg void OnEnableTitleTipsChanged();
	BOOL m_bEditable;
	afx_msg void OnEditableChanged();
	short m_nBorder;
	afx_msg void OnBorderChanged();
	short m_nIconSize;
	afx_msg void OnIconSizeChanged();
	short m_nImageOffset;
	afx_msg void OnImageOffsetChanged();
	short m_nIndent;
	afx_msg void OnIndentChanged();
	short m_nMargin;
	afx_msg void OnMarginChanged();
	short m_nNumberOffset;
	afx_msg void OnNumberOffsetChanged();
	BOOL m_bShowButtons;
	afx_msg void OnShowButtonsChanged();
	BOOL m_bTracking;
	afx_msg void OnTrackingChanged();
	BOOL m_bViewType;
	afx_msg void OnViewTypeChanged();
	afx_msg LPDISPATCH GetView();
	afx_msg void SetView(LPDISPATCH newValue);
	afx_msg BOOL GetReadOnly();
	afx_msg void SetReadOnly(BOOL bNewValue);
	afx_msg LPDISPATCH GetSelectedObject();
	afx_msg void SetSelectedObject(LPDISPATCH newValue);
	afx_msg OLE_COLOR GetTitleTipsColor();
	afx_msg void SetTitleTipsColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetTitleTipsBackColor();
	afx_msg void SetTitleTipsBackColor(OLE_COLOR nNewValue);
	afx_msg void Register(BOOL bRegister);
	afx_msg LPDISPATCH GetButtonType();
	afx_msg LPDISPATCH GetButtonObj();
	afx_msg LPDISPATCH GetButtonAbc();
	afx_msg LPDISPATCH GetButtonTime();
	afx_msg LPDISPATCH GetButtonNumOn();
	afx_msg LPDISPATCH GetButtonNumOff();
	afx_msg LPDISPATCH GetButtonExpand();
	afx_msg LPDISPATCH GetButtonCollapse();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CContextAXCtrl)
	void FireBeforeViewChange(LPDISPATCH pView)
		{FireEvent(eventidBeforeViewChange,EVENT_PARAM(VTS_DISPATCH), pView);}
	void FireAfterViewChange(LPDISPATCH pView)
		{FireEvent(eventidAfterViewChange,EVENT_PARAM(VTS_DISPATCH), pView);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CContextAXCtrl)
	dispidView = 12L,
	dispidReadOnly = 13L,
	dispidEnableTitleTips = 1L,
	dispidEditable = 2L,
	dispidBorder = 3L,
	dispidIconSize = 4L,
	dispidImageOffset = 5L,
	dispidIndent = 6L,
	dispidMargin = 7L,
	dispidNumberOffset = 8L,
	dispidShowButtons = 9L,
	dispidTracking = 10L,
	dispidViewType = 11L,
	dispidSelectedObject = 14L,
	dispidTitleTipsColor = 15L,
	dispidTitleTipsBackColor = 16L,
	dispidRegister = 17L,
	dispidGetButtonType = 18L,
	dispidGetButtonObj = 19L,
	dispidGetButtonAbc = 20L,
	dispidGetButtonTime = 21L,
	dispidGetButtonNumOn = 22L,
	dispidGetButtonNumOff = 23L,
	dispidGetButtonExpand = 24L,
	dispidGetButtonCollapse = 25L,
	eventidBeforeViewChange = 1L,
	eventidAfterViewChange = 2L,
	//}}AFX_DISP_ID
	};



protected:
	GuidKey	m_lLastViewKey; // last used view
	GuidKey	m_lLastDocKey;	// last used doc
	BOOL	m_bReadOnlyMode;// flag read only mode
	
	CContextWrpType		m_ViewType; // wrapper of type
	CContextWrpObject	m_ViewObj;	// wrapper of object
	IViewPtr			m_sptrView;	// ptr to interface of current active view
	bool	m_bAttachedToView; // flag control is attached to view


	// buttons
	CPushButton	m_btnType;
	CPushButton	m_btnObj;
	CPushButton	m_btnAbc;
	CPushButton	m_btnTime;
	CPushButton	m_btnNumOn;
	CPushButton	m_btnNumOff;
	CPushButton	m_btnExpand;
	CPushButton	m_btnCollapse;

	int		m_nBarHeight;	// "toolbar"  height
	CSize	m_ButtonSize;	// button size

	CStringArray	m_arrTempFiles;	// array of temporary files used for buttons
	HWND			m_hFocusWnd;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTAXCTL_H__224F6C55_DDB8_44F0_9021_636A3BB9B2BE__INCLUDED)
