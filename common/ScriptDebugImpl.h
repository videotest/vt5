// ScriptDebugImpl.h: interface for the CScriptDebugImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTDEBUGIMPL_H__B0E4548E_9B83_4072_B65B_D0125AAF273F__INCLUDED_)
#define AFX_SCRIPTDEBUGIMPL_H__B0E4548E_9B83_4072_B65B_D0125AAF273F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef NOT_COMPATIBLE_VT5
#include "defs.h"
#endif//NOT_COMPATIBLE_VT5

#include "activdbg.h"
#include "\vt5\ifaces\script_int.h"


#ifndef NOT_COMPATIBLE_VT5
#include "common.h"

//////////////////////////////////////////////////////////////////////
//
// class CScriptDebugImpl
//
//////////////////////////////////////////////////////////////////////
class std_dll CScriptDebugImpl : public CImplBase
{
public:
	CScriptDebugImpl();
	~CScriptDebugImpl();

	bool				init_debugger( const char* psz_app_name );
	bool				before_parse_text(	const char* psz_script, 
											const char* psz_file_name, 
											const char* psz_title, 
											IActiveScript *pIActiveScript,
											DWORD* pdw_script_context );
	void				deinit_debuger();
	IUnknown*			m_punkScriptDebug;

};
#endif//NOT_COMPATIBLE_VT5

//////////////////////////////////////////////////////////////////////
//
// class script_debug_doc
//
//////////////////////////////////////////////////////////////////////
class script_debug_doc
{
	DWORD					m_dw_data;
public:
	ATL::CComPtr<IDebugDocumentHelper>	m_pDebugDocHelper;
	script_debug_doc():m_pDebugDocHelper(0), m_dw_data(0) {}
	~script_debug_doc(){
		if( m_pDebugDocHelper )
		{
			VERIFY( S_OK == m_pDebugDocHelper->Detach() );
		}
	}
	void SetData(DWORD dw){
		m_dw_data=dw;
	}
	DWORD GetData(void){
		return m_dw_data;
	}
};

//////////////////////////////////////////////////////////////////////
//
// class CDebugHost
//
//////////////////////////////////////////////////////////////////////
class CDebugHost :	public CCmdTarget
{
	DECLARE_DYNCREATE(CDebugHost)
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

public:
	CDebugHost();
	virtual ~CDebugHost();

	BEGIN_INTERFACE_PART(ActiveScriptSiteDebug, IActiveScriptSiteDebug)
		com_call GetDocumentContextFromPosition(DWORD, ULONG, ULONG, IDebugDocumentContext**);
		com_call GetApplication(IDebugApplication**);
		com_call GetRootApplicationNode(IDebugApplicationNode**);
		com_call OnScriptErrorDebug(IActiveScriptErrorDebug*, BOOL*, BOOL*);
	END_INTERFACE_PART(ActiveScriptSiteDebug)

	BEGIN_INTERFACE_PART(DebugDocumentHost, IDebugDocumentHost)
		com_call GetDeferredText(DWORD, WCHAR*, SOURCE_TEXT_ATTR*, ULONG*, ULONG); 
		com_call GetScriptTextAttributes(LPCOLESTR, ULONG, LPCOLESTR, DWORD, SOURCE_TEXT_ATTR*); 
		com_call OnCreateDocumentContext(IUnknown**); 
		com_call GetPathName(BSTR*, BOOL*); 
		com_call GetFileName(BSTR*); 
		com_call NotifyChanged(); 
	END_INTERFACE_PART(DebugDocumentHost)

	BEGIN_INTERFACE_PART(DebugHost, IDebugHost)
		com_call Init( BSTR bstr_app_name );
		com_call OnRunScript( BSTR bstrScript, BSTR bstrFileName, BSTR bstrTitle, IActiveScript *pIActiveScript, DWORD* pdw_context );
		com_call CloseScript( DWORD dw_context );
		com_call ShowDebugger( DWORD dw_context );
		com_call BreakIntoDebuger( );
	END_INTERFACE_PART(DebugHost)

	bool		_init( const char* psz_app_name );
	void		_de_init();


protected:
	IProcessDebugManager*	m_pdm;
	IDebugApplication*		m_pDebugApp;	

	DWORD					m_dwAppCookie;

	CString					m_strScript;
	CString					m_strFileName;
	CString					m_strTitle;

	CArray<script_debug_doc*, script_debug_doc*>	m_ar_debug_doc;
};

#endif // !defined(AFX_SCRIPTDEBUGIMPL_H__B0E4548E_9B83_4072_B65B_D0125AAF273F__INCLUDED_)
