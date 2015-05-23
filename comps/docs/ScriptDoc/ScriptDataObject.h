#if !defined(AFX_SCRIPTDATAOBJECT_H__39346387_2480_11D3_A5D9_0000B493FF1B__INCLUDED_)
#define AFX_SCRIPTDATAOBJECT_H__39346387_2480_11D3_A5D9_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptDataObject.h : header file
//

#include "CCrystalTextBuffer.h"
#include "scriptdocint.h"
#include "DataBase.h"
/////////////////////////////////////////////////////////////////////////////
// CScriptDataObject command target

class CScriptDataObject;

// [vanek] : генераци€ идентификатора дл€ скриптов
inline DWORD	generateScriptActionID()
{
	static DWORD dwID = 0;
	return ++ dwID;
}

class CScriptActionInfo : public CCmdTargetEx,
						public CNumeredObjectImpl
{
	DECLARE_INTERFACE_MAP();

	ENABLE_MULTYINTERFACE();

	
	enum StateAnswerType
	{
		Nothing,
		DockBar,
		Action,
		ToolBar,
		Function,
	};
public:
	CScriptActionInfo();
	virtual ~CScriptActionInfo();

	void Init( CScriptDataObject* );
	void DeInit();
	void CheckScriptInfo();

protected:
	BEGIN_INTERFACE_PART(Info, IActionInfo3)
		com_call GetArgsCount( int *piParamCount );
		com_call GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue );
		com_call GetOutArgsCount( int *piParamCount );
		com_call GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind );
		com_call IsFunction( BOOL *pboolAction );
	//user interface
		com_call GetCommandInfo( 
			//general information
			BSTR	*pbstrActionName,
			BSTR	*pbstrActionUserName,
			BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
			BSTR	*pbstrGroupName	);
		com_call GetTBInfo(
			//toolbar information
			BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
			IUnknown **ppunkBitmapHelper );
		com_call GetHotKey( DWORD *pdwKey, BYTE *pbVirt );
			//menu information
		com_call GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode );
		com_call GetMenuInfo(
			BSTR	*pbstrMenus,		//"general", "image doc", ... if action avaible in several menus, it should 
			DWORD	*pdwMenuFlag );		//not used, must be zero. Will be used later, possible, 
										//for special sub-menu (such as "Recent files")
		com_call ConstructAction( IUnknown **ppunk );
		com_call GetTargetName( BSTR *pbstr );
		com_call GetLocalID( int *pnID );
		com_call SetLocalID( int nID );

		com_call IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs );
		com_call GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber );
		com_call GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber );
		com_call GetUIUnknown( IUnknown **ppunkinterface );	
		com_call SetActionString( BSTR	bstr );

        //  [vanek] : IActionInfo3 - 13.12.2004
		com_call SetBitmapHelper( IUnknown *punkBitmapHelper );
	END_INTERFACE_PART(Info)

	BEGIN_INTERFACE_PART(Help, IHelpInfo2)
		com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags );
		com_call GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix );
	END_INTERFACE_PART(Help)
public:
	void ReloadString();
	DWORD GetActionState();
protected:

	CString	m_strUserName;
	CString	m_strHelpString;
	CString	m_strActionHelpString;
	CString	m_strGroupName;
	
	_bstr_t	m_bstrStateAction;
	_bstr_t	m_bstrStateDockBar;
	_bstr_t	m_bstrStateToolBar;
	_bstr_t	m_bstrFunction;
	
	StateAnswerType	m_StateAnswer;

	bool	m_bStringsLoaded;
	CScriptDataObject	*m_pDataObject;
	bool				m_bInit;		//initialized flag
	int					m_nLocalID;

	IActionManagerPtr	m_ptrAM;
	ICompManagerPtr		m_ptrCM;		//ActionManager

	// [vanek] : support IActionInfo3 interface - 13.12.2004
	IUnknownPtr	m_spunkBmpHelper;
	DWORD		m_dwID;	// возвращаетс€ из GetRuntimeInformaton в качестве dwCmdCode, это нужно, чтобы можно было
						// отличить два разных скрипта по сочетанию dwCmdCode, dwDllCode, а т.к.  dwDllCode равно 0
};

class CScriptAction: public CCmdTargetEx,
						public CRootedObjectImpl
{
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
public:
	CScriptAction( CScriptDataObject* );
protected:
	BEGIN_INTERFACE_PART(Action, IAction)
		com_call AttachTarget( IUnknown *punkTarget );
		com_call SetArgument( BSTR bstrArgName, VARIANT *pvarVal );
		com_call SetResult( BSTR bstrArgName, VARIANT *pvarVal );
		com_call Invoke();
		com_call GetActionState( DWORD *pdwState );
		com_call StoreParameters( BSTR *pbstr );
		com_call GetActionInfo(IUnknown** ppunkAI);
		com_call GetTarget( IUnknown **ppunkTarget );
	END_INTERFACE_PART(Action)
protected:
	_bstr_t	m_bstrScriptText;
	_bstr_t m_str_script_name;
	CScriptActionInfo	*m_pinfo;
};

class CScriptDataObject : public CDataObjectBase
{
	PROVIDE_GUID_INFO()

	DECLARE_DYNCREATE(CScriptDataObject)
	GUARD_DECLARE_OLECREATE(CScriptDataObject)

	CScriptDataObject();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDataObject)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScriptDataObject();

	// Generated message map functions
	//{{AFX_MSG(CScriptDataObject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CScriptDataObject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ScriptDataObj, IScriptDataObject)
		com_call InitNew();
		com_call ReadFile( BSTR bstr );
		com_call WriteFile( BSTR bstr );
		com_call GetText( BSTR *pbstr );
		com_call GetFullText( BSTR *pbstr );
		com_call GetLinesCount( long *pnCount );
		com_call GetLine( long index, BSTR *pbstrText );
		com_call InsertString( BSTR bstr, long index );
		com_call DeleteString( long index );
		com_call GetPrivateDataPtr( DWORD *pdwDataPtr );
		com_call RegisterScriptAction();
	END_INTERFACE_PART(ScriptDataObj)
protected:
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual bool GetTextParams( CString &strParams );
	virtual bool SetTextParams( const char *pszParams );

	virtual bool IsModified();
	virtual void SetModifiedFlag( bool bSet = true );

	virtual void OnChangeName();
	virtual void OnScriptChange();

	virtual bool CanBeBaseObject();
public:
	static char	*m_szType;
	CCrystalTextBuffer	m_buffer;
	CScriptActionInfo	m_info;		//action info - for ActionManager

	CString	m_strOldName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTDATAOBJECT_H__39346387_2480_11D3_A5D9_0000B493FF1B__INCLUDED_)
