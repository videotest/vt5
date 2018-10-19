#if !defined(AFX_ACTIVEXSITE_H__CC00FE11_A4D0_49D2_871F_1A96A40DBC2D__INCLUDED_)
#define AFX_ACTIVEXSITE_H__CC00FE11_A4D0_49D2_871F_1A96A40DBC2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActiveXSite.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActiveXSite window
#include "ActionHelper.h"
#include "ActionHelper2.h"

class CAxWnd : public CWnd
{
public:
	void		SetControlObjectUnknown(IUnknown	*punk)	{m_ptrDataUnknown = punk;}
	IUnknown	*GetControlObjectUnknown()				{return m_ptrDataUnknown;}

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
public:
	CString	m_strControlProgID;
	CString	m_strControlName;
protected:
	IUnknownPtr	m_ptrDataUnknown;
};


#include "ScriptDebugImpl.h"
//The site specific tasks are
//1. Route preview info to the active view 
//2. Termiante action when user control interface/Clean-up
//3. Finalize action then user press OK or Apply
//4. Terminate action when user press Cancel
#define WM_INITIALIZE_FORM	(WM_USER+780)
#define WM_FIRE_ON_CREATE_FORM	(WM_USER+781)

class CActiveXSite : public CWnd,
					public CScriptDebugImpl,
					public CWindowImpl,
					public COleEventSourceImpl,
					public CNamedObjectImpl,
					public CPreviewSiteImpl
{
	enum PreviewActionState
	{
		Undefined,
		Running,
		Complete,
		Terminated,
	};
	ENABLE_MULTYINTERFACE();

	DECLARE_OLETYPELIB(CActiveXSite)
	DECLARE_DYNCREATE(CActiveXSite)
	GUARD_DECLARE_OLECREATE(CActiveXSite)

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_EVENT_MAP()

	ROUTE_OLE_VIRTUALS();

// Construction
public:
	CActiveXSite();

// Attributes
public:
	CSize	GetSize();
	bool m_bLockFireEvent;

	void SetSize(CSize size);

	CString GetCaption();

// Operations
public:
	void SetForm( IUnknown *punkForm );
	void SetEdgeType(UINT nType)		{m_nEdgeType = nType;};
	void SetHelpDisplayed( bool bSet )	{m_bHelpDisplayed = bSet;}

	bool IsHelpDisplayed()		const	{return m_bHelpDisplayed;}
	bool AllowClose() const				{return !m_bContinueModal;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveXSite)
	public:
	virtual void OnFinalRelease();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CActiveXSite();

	// Generated message map functions
	CString m_strTitle;
protected:
	//{{AFX_MSG(CActiveXSite)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnFireInitialize( WPARAM, LPARAM );
	afx_msg LRESULT OnFireCreateForm( WPARAM, LPARAM );
	
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CActiveXSite)
	afx_msg void BeginFilterGroup();
	afx_msg void EndFilterGroup();

	afx_msg void OnTitleChanged();
	afx_msg void ApplyPreview();
	afx_msg void CancelPreview();
	afx_msg void AddHeadActionArgument(LPDISPATCH pdispObject);
	afx_msg void InitializePreview();
	afx_msg void UpdateSystemSettings();
	afx_msg void ContiueModal();
	afx_msg void debug_break();
	afx_msg LPDISPATCH GetFormScript();
	afx_msg void EnableButtons(long dwState);
	afx_msg void ShowButtons(long dwState);
	afx_msg void TerminateCurrentAction();

	afx_msg void Preview2_Init();
	afx_msg void Preview2_DeInit();
	afx_msg void Preview2_SetStagesCount(long nCount);
	afx_msg long Preview2_GetStagesCount();
	afx_msg void Preview2_LoadState(long index);
	afx_msg void Preview2_SaveState(long index);
	afx_msg void Preview2_Invalidate(long index);
	afx_msg void Preview2_Cancel();
	afx_msg void Preview2_Apply();

	//}}AFX_DISPATCH
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	BEGIN_INTERFACE_PART(Site, IActiveScriptSite)
        com_call GetLCID( LCID *plcid );
		com_call GetItemInfo( LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti );
		com_call GetDocVersionString( BSTR *pszVersion );
		com_call RequestItems( void );
		com_call RequestTypeLibs( void );
		com_call OnScriptTerminate( const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo );
		com_call OnStateChange( SCRIPTSTATE ssScriptState );
		com_call OnScriptError( IActiveScriptError *pscripterror );
		com_call OnEnterScript( void );
		com_call OnLeaveScript( void );
    END_INTERFACE_PART(Site)

	BEGIN_INTERFACE_PART(SiteWnd, IActiveScriptSiteWindow)
		com_call GetWindow( HWND *phwnd );
		com_call EnableModeless( BOOL fEnable );
    END_INTERFACE_PART(SiteWnd)

	BEGIN_INTERFACE_PART(AxSite, IAXSite)
		com_call ExecuteAction( BSTR bstrActionName, BSTR bstrActionParam, DWORD dwFlags );
    END_INTERFACE_PART(AxSite)

	BEGIN_INTERFACE_PART(Help, IHelpInfo2)
		com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags );
		com_call GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix );
	END_INTERFACE_PART(Help)


	enum {
	//{{AFX_DISP_ID(CActiveXSite)
	dispidTitle = 1L,
	dispidApplyPreview = 2L,
	dispidCancelPreview = 3L,
	dispidAddHeadActionArgument = 4L,
	dispidInitializePreview = 5L,
	dispidUpdateSystemSettings = 6L,
	dispidContiueModal = 7L,
	dispidDebug = 8L,
	dispidGetFormScript = 9L,
	eventidOnOK = 1L,
	eventidOnCancel = 2L,
	eventidOnLoadState = 3L,
	eventidOnSaveState = 4L,
	eventidOnInitialize = 5L,
	eventidOnDestroy = 6L,
	eventidOnNext = 7L,
	eventidOnPrev = 8L,
	eventidOnChangePreview = 9L,
	eventidOnShow = 10L,
	eventidOnCreate = 11L,
	eventidOnHide = 12L,
	eventidOnProcess = 13L,
	//}}AFX_DISP_ID
	};
public:
	//{{AFX_EVENT(CActiveXSite)
	void FireOnOK()
	{
		if(!m_bFormInited) return; // пока не проинитили - нах кнопки
		if( !m_bContinueModal )
		{
			POSITION	pos = m_windows.GetHeadPosition();
			while( pos )
			{
				CAxWnd	*pwnd = m_windows.GetNext( pos );
				IVtActiveXCtrl3Ptr	sptrVTC( pwnd->GetControlUnknown() );				
				if( sptrVTC )sptrVTC->OnOK( );
			}

			FireEvent(eventidOnOK,EVENT_PARAM(VTS_NONE));
		}
		::FireEvent(GetAppUnknown(), szEventFormOnOk, GetControllingUnknown(), 0, NULL, 0);
	}
	void FireOnCancel()
	{
		if(!m_bFormInited)
		{
			m_bCancelSheduled = true;
			return; // пока не проинитили - нах кнопки
		}
		m_bCancelSheduled = false;
		if( !m_bContinueModal )
		{
			POSITION	pos = m_windows.GetHeadPosition();
			while( pos )
			{
				CAxWnd	*pwnd = m_windows.GetNext( pos );
				IVtActiveXCtrl3Ptr	sptrVTC( pwnd->GetControlUnknown() );				
				if( sptrVTC )sptrVTC->OnCancel( );
			}
			FireEvent(eventidOnCancel,EVENT_PARAM(VTS_NONE));
		}
		::FireEvent(GetAppUnknown(), szEventFormOnCancel, GetControllingUnknown(), 0, NULL, 0);
	}
	void FireOnLoadState(LPDISPATCH NamedData)
		{FireEvent(eventidOnLoadState,EVENT_PARAM(VTS_DISPATCH), NamedData);}
	void FireOnSaveState(LPDISPATCH NamedData)
		{FireEvent(eventidOnSaveState,EVENT_PARAM(VTS_DISPATCH), NamedData);}
	void FireOnInitialize()
		{
			::FireEvent(GetAppUnknown(), szEventFormOnInitialize, GetControllingUnknown(), 0, NULL, 0);
			FireEvent(eventidOnInitialize,EVENT_PARAM(VTS_NONE));
		}
	void FireOnDestroy()
		{
			FireEvent(eventidOnDestroy,EVENT_PARAM(VTS_NONE));
			::FireEvent(GetAppUnknown(), szEventFormOnDestroy, GetControllingUnknown(), 0, NULL, 0);
		}
	void FireOnNext()
		{FireEvent(eventidOnNext,EVENT_PARAM(VTS_NONE));}
	void FireOnPrev()
		{FireEvent(eventidOnPrev,EVENT_PARAM(VTS_NONE));}
	void FireOnChangePreview()
		{FireEvent(eventidOnChangePreview,EVENT_PARAM(VTS_NONE));}
	void FireOnShow()
		{FireEvent(eventidOnShow,EVENT_PARAM(VTS_NONE));}
	void FireOnCreate()
		{FireEvent(eventidOnCreate,EVENT_PARAM(VTS_NONE));}
	void FireOnHide()
		{FireEvent(eventidOnHide,EVENT_PARAM(VTS_NONE));}
	void FireOnProcess(long nStage)
		{FireEvent(eventidOnProcess,EVENT_PARAM(VTS_I4), nStage);}

	//}}AFX_EVENT

	virtual CWnd *GetWindow(){return this;}

	virtual void DoLeavePreviewMode();
	virtual void DoTerminatePreview();

	virtual bool CanPreviewObject( IUnknown *punkObject ){return true;}
	virtual CRect GetPreviewObjectRect( IUnknown *punkObject, int nPosition = -1 ){return NORECT;}
	virtual CWnd *GetTargetWindow(){return this;}
	

protected:
	HRESULT		_CreateScriptEngine();
	HRESULT		_ExecuteFormScript();
	HRESULT		_LoadControls();

	int			_GetAppComponentCount();
	IUnknown*	_GetAppComponent( int nPos );
	int			_GetControlComponentsCount();
	IUnknown*	_GetControlComponent( int nPos );
	IUnknown*	_GetScriptItemUnk( const char *szItemName );

protected:
	IActiveScriptPtr			m_ptrActiveScript;
	IActiveScriptParsePtr		m_ptrActiveScriptParse;
	IOperationManagerPtr		m_ptrOperationManager;
	DWORD						m_dw_script_context;

	IUnknownPtr	m_ptrForm;
	CTypedPtrList<CPtrList, CAxWnd*>	m_windows;


	UINT				m_nIDEvent, m_nIDTimer;
	bool				m_bTerminated;
	CActionHelper		m_helper;

	CActionHelper2		m_helper2;

	UINT_PTR			m_nEdgeType;// 0 - вогнута€; 1 - выпукла€; 2 - никака€
	bool				m_bHelpDisplayed;
	bool				m_bNeedCallInitialize;
public:
	bool				m_bContinueModal, m_bClosingWindow;
	bool	m_bFormInited; // форма уже проиничена?
	bool	m_bCancelSheduled; // ¬о врем€ инита был PressCancel

protected:
	IDebugHostPtr		get_debugger();
//	bool				m_bform_cancel;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};




#define WM_CAPTIONCHANGE	(WM_USER+776)
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVEXSITE_H__CC00FE11_A4D0_49D2_871F_1A96A40DBC2D__INCLUDED_)
