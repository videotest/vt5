#if !defined(AFX_ScriptView_H__528AB686_0C3D_11D3_A5B7_0000B493FF1B__INCLUDED_)
#define AFX_ScriptView_H__528AB686_0C3D_11D3_A5B7_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptView window
#include "CCrystalEditView.h"
#include "scriptdocint.h"

class CScriptView : public CCrystalEditView,
				public CWindow2Impl,
				public CViewImpl,
				public CRootedObjectImpl,
				public CEventListenerImpl,
				public CNamedObjectImpl,
				public CPrintImpl,
				public CHelpInfoImpl
{
	DECLARE_DYNCREATE(CScriptView);
	GUARD_DECLARE_OLECREATE(CScriptView);
	PROVIDE_GUID_INFO();
// Construction
public:
	CScriptView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptView)
	public:
	virtual void OnFinalRelease();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptView();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

	
	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
//component architecture support
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	virtual void OnInitialUpdate();

	virtual CWnd *GetWindow(){return this;}
	virtual void OnActivateObject( IUnknown *punkDataObject );
	virtual DWORD GetMatchType( const char *szType );
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	virtual bool DoTranslateAccelerator( MSG *pMsg );

	int _GetMaxWidth();
	int _GetNumLinesPerHeight(int nBeginLine, int& nHeight);


	//Active Script Debug IDE support
	BEGIN_INTERFACE_PART(AppDebuger, IApplicationDebugger)
		com_call QueryAlive(void); 
		com_call CreateInstanceAtDebugger(REFCLSID rclsid, IUnknown *pUnkOuter, DWORD dwClsContext, REFIID riid, IUnknown **ppvObject);
		com_call onDebugOutput(LPCOLESTR pstr); 
		com_call onHandleBreakPoint(IRemoteDebugApplicationThread *prpt, BREAKREASON br, IActiveScriptErrorDebug *pError);
		com_call onClose(void);
		com_call onDebuggerEvent(REFIID riid, IUnknown *punk); 
	END_INTERFACE_PART(AppDebuger)

	BEGIN_INTERFACE_PART(AppDebugerUI, IApplicationDebuggerUI)
		com_call BringDocumentToTop(IDebugDocumentText* pddt);
		com_call BringDocumentContextToTop(IDebugDocumentContext* pddc); 
	END_INTERFACE_PART(AppDebugerUI)

	BEGIN_INTERFACE_PART(DebugerIDE, IProvideDebuggerIDE)
		com_call GetApplicationDebugger(IApplicationDebugger** ppad);
		com_call ToggleBreakpoint();
		com_call SetBreakpointsToEngine(DWORD dwContext);
	END_INTERFACE_PART(DebugerIDE)


	CMap<DWORD, DWORD, bool, bool>	m_mapBreakpoins;
	DWORD			m_dwContext;

	void SetBreakpoint(DWORD dwOffset);

protected:    
	virtual CCrystalTextBuffer *LocateTextBuffer();
	virtual DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);

	virtual CScrollBar	*GetScrollBarCtrl( int iSBCode ) const
	{	return CViewImpl::GetScrollBarCtrl( iSBCode );	}
	
	LOGFONT	m_lf;

	sptrIScriptDataObject	m_sptrD;
	IUnknownPtr				m_sptrForm;

	void			CreatePosPane();
	void			DestroyPosPane();
	void			UpdatePosPane();

	virtual void OnSetCaretPos( CPoint pt );


	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );



};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ScriptView_H__528AB686_0C3D_11D3_A5B7_0000B493FF1B__INCLUDED_)
