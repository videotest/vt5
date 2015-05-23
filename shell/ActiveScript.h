#if !defined(AFX_ACTIVESCRIPT_H__C0B1BAB3_F95E_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_ACTIVESCRIPT_H__C0B1BAB3_F95E_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Activscp.h"
#include "ScriptDebugImpl.h"
#include "scriptdocint.h"
// ActiveScript.h : header file

#define SECTION_ACTIVESCRIPT	_T("ActiveScript") 
#define ENTRY_AUTOEXEC			_T("AppStart")
#define ENTRY_AUTOEXIT			_T("AppTerminate")

//



/////////////////////////////////////////////////////////////////////////////
// CActiveScript command target

class CActiveScript : public CCmdTargetEx,
					  public CScriptDebugImpl
{
	DECLARE_DYNCREATE(CActiveScript)
	ENABLE_MULTYINTERFACE();

public:
	CActiveScript();           // protected constructor used by dynamic creation
	virtual ~CActiveScript();
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveScript)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CActiveScript)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()
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


	BEGIN_INTERFACE_PART(Debug, IScriptDebugger)
		com_call SetBreakOnStart();
		com_call ConnectDebugger();
		com_call SetScriptText(BSTR bstrScriptText, DWORD* pdwContext);
		com_call Reset();
		com_call StartDebugger();
    END_INTERFACE_PART(Debug)

	BEGIN_INTERFACE_PART(ScriptEngine, IScriptEngine)
		com_call GetActiveScript( IUnknown** pi_as );
		com_call GetActiveScriptParse( IUnknown** pi_asp );	
    END_INTERFACE_PART(ScriptEngine)

	

	IActiveScript        *m_pIActiveScript;
	IActiveScriptParse   *m_pIActiveScriptParse;

	HRESULT CreateScriptEngine();
	void Reset();

public:
	void Init();
	void DeInit();
	void LoadScripts();
	void ClearScripts();	// [vanek] SBT:888 - 26.05.2004

	void ExecuteAppScript( const char *szNamedDataEntry );	//execute the script 
													//from application named data 
	IActiveScript*	GetActiveScript();
protected:
	//return iunknown by name
	IUnknown *GetScriptItemUnk( const char *sz );
public://script namespace
	CCompManager	m_compsScriptItems;

	CString		m_strAppStartScript;
	CString		m_strAppTerminateScript;
	CTypedPtrList<CPtrList, IUnknown*>	m_Scripts;
};


extern CActiveScript	g_script;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVESCRIPT_H__C0B1BAB3_F95E_11D2_A596_0000B493FF1B__INCLUDED_)
