// ActiveScript.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "ActiveScript.h"
#include "CommandManager.h"
#include "scriptdocint.h"

//some common defines
const TCHAR* szItemName=_T("shell");


/////////////////////////////////////////////////////////////////////////////
// CActiveScript

CActiveScript	g_script;

IMPLEMENT_DYNCREATE(CActiveScript, CCmdTargetEx)

CActiveScript::CActiveScript()
{
	EnableAggregation();

	m_pIActiveScript=0;
	m_pIActiveScriptParse=0;	
}

CActiveScript::~CActiveScript()
{
	DeInit();
}


BEGIN_MESSAGE_MAP(CActiveScript, CCmdTargetEx)
	//{{AFX_MSG_MAP(CActiveScript)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CActiveScript, CCmdTargetEx)
    INTERFACE_PART(CActiveScript, IID_IActiveScriptSite, Site)
    INTERFACE_PART(CActiveScript, IID_IActiveScriptSiteWindow, SiteWnd)	
	INTERFACE_PART(CActiveScript, IID_IScriptEngine, ScriptEngine)	
	INTERFACE_AGGREGATE(CActiveScript, m_punkScriptDebug)	
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CActiveScript, Site);
IMPLEMENT_UNKNOWN(CActiveScript, SiteWnd);
IMPLEMENT_UNKNOWN(CActiveScript, ScriptEngine);


/////////////////////////////////////////////////////////////////////////////
// CActiveScript message handlers

//execute the script from application named data 
void CActiveScript::ExecuteAppScript( const char *szNamedDataEntry )
{
//get script script from the NamedData
	_bstr_t	bstr = ::GetValueString( theApp.GetControllingUnknown(), SECTION_ACTIVESCRIPT, szNamedDataEntry );

	::ExecuteScript( bstr, szNamedDataEntry );
}

IActiveScript* CActiveScript::GetActiveScript()
{
	if( m_pIActiveScript )	m_pIActiveScript->AddRef();
	return m_pIActiveScript;
}


//purpose : initialize script engeene 
void CActiveScript::Init()
{
	_try(CActiveScript, Init)
	{
		if( CreateScriptEngine() !=S_OK )
		{
			AfxMessageBox( IDS_CANTSTARTENGEENE );
			DeInit();
		}

	}
	_catch_report;
}

//purpose : execute AppTerminate macros ( not implemented yet ),
//deinitialize script engeene and 
void CActiveScript::DeInit()
{
	//Active Scripting debug support

	ClearScripts( );

	if (m_pIActiveScriptParse)
		m_pIActiveScriptParse->Release();
	if (m_pIActiveScript)
	{
		m_pIActiveScript->Close();
		m_pIActiveScript->Release();
    }

	m_pIActiveScriptParse = 0;
	m_pIActiveScript = 0;

	deinit_debuger();
}

//interface implementation 
STDMETHODIMP CActiveScript::XSite::GetLCID(LCID *plcid)
{
	return E_NOTIMPL;     // Use system settings
}

STDMETHODIMP CActiveScript::XSite::GetItemInfo
(
  LPCOLESTR   pstrName,
  DWORD       dwReturnMask,
  IUnknown**  ppunkItemOut,
  ITypeInfo** pptinfoOut
)
{
	//HRESULT hr;
	METHOD_PROLOGUE(CActiveScript, Site)

	if( pptinfoOut )
		*pptinfoOut = 0;
	if( ppunkItemOut )
		*ppunkItemOut = 0;

	CString	strItemName = pstrName;

	//TRACE( _T("GetItemInfo called (%s)\n"), (const char *)strItemName );

	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		if (!pptinfoOut)
			return E_INVALIDARG;
		*pptinfoOut = NULL;

		IUnknownPtr sptrUnk( pThis->GetScriptItemUnk( strItemName ), false );

		if( CheckInterface( sptrUnk, IID_IProvideClassInfo ) )
		{
			IProvideClassInfoPtr sptrPCI( sptrUnk );
			sptrPCI->GetClassInfo( pptinfoOut );
		}
	}

	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		if (!ppunkItemOut)
			return E_INVALIDARG;

		*ppunkItemOut = pThis->GetScriptItemUnk( strItemName );
    }

	return S_OK;
}


STDMETHODIMP CActiveScript::XSite::GetDocVersionString(BSTR *pbstrVersion)
{
	return E_NOTIMPL;   // UNDONE: Implement this method
}

STDMETHODIMP CActiveScript::XSite::RequestItems(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CActiveScript::XSite::RequestTypeLibs(void)
{
	METHOD_PROLOGUE(CActiveScript,Site);
	return E_NOTIMPL;
}

STDMETHODIMP CActiveScript::XSite::OnScriptTerminate(const VARIANT   *pvarResult,const EXCEPINFO *pexcepinfo)
{
	// UNDONE: Put up error dlg here
	return S_OK;
}


//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveScript::XSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	// Don't care about notification
	return S_OK;
}


//---------------------------------------------------------------------------
// Display the error
//---------------------------------------------------------------------------
STDMETHODIMP CActiveScript::XSite::OnScriptError(IActiveScriptError *pse)
{
	METHOD_PROLOGUE(CActiveScript,Site);
  
	CString   strError;
	CString	  strArrow;
	CString	  strDesc;
	CString	  strLine;
	
	EXCEPINFO ei;
	DWORD     dwSrcContext;
	ULONG     ulLine;
	LONG      ichError;
	BSTR      bstrLine = NULL;
	
	HRESULT   hr;

	pse->GetExceptionInfo(&ei);
	pse->GetSourcePosition(&dwSrcContext, &ulLine, &ichError);
	hr = pse->GetSourceLineText(&bstrLine);
	if (hr)
		hr = S_OK;  // Ignore this error, there may not be source available
  
	if (!hr)
    {
		if( ei.bstrSource )
			strError=ei.bstrSource;
		if( ei.bstrDescription )
			strDesc=ei.bstrDescription;
		if( bstrLine )
			strLine=bstrLine;

		if (ichError > 0 && ichError < 255)
		{
			strArrow=CString(_T('-'),ichError);
			strArrow.SetAt(ichError-1,_T('v'));
			
		}

		CString strErrorCopy=strError;
		strError.Format(_T("Source:'%s'\nFile:'%s'  Line:%d  Char:%ld\nError:%d  '%s'\n%s\n%s"),
						LPCTSTR(strErrorCopy),
						_T("File"),
						ulLine,
						ichError,
						(int)ei.wCode,
						LPCTSTR(strDesc),
						LPCTSTR(strArrow),
						LPCTSTR(strLine));
		
		AfxMessageBox(strError);
    }

	if (bstrLine)
		SysFreeString(bstrLine);

	return hr;
}
STDMETHODIMP CActiveScript::XSite::OnEnterScript(void)
{
  // No need to do anything
  return S_OK;
}

STDMETHODIMP CActiveScript::XSite::OnLeaveScript(void)
{
  // No need to do anything
  return S_OK;
}

//***************************************************************************
// IActiveSiteWnd Interface
//***************************************************************************

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveScript::XSiteWnd::GetWindow(HWND *phwndOut)
{
  if (!phwndOut)
    return E_INVALIDARG;

  METHOD_PROLOGUE(CActiveScript,SiteWnd);

  *phwndOut = AfxGetMainWnd()->GetSafeHwnd();
  return S_OK;
}


//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveScript::XSiteWnd::EnableModeless(BOOL fEnable)
{
	AfxGetApp()->EnableModeless( fEnable );
  return S_OK;
}
// #####  END  ACTIVEX SCRIPTING SUPPORT #####


// ##### BEGIN ACTIVEX SCRIPTING SUPPORT #####
//---------------------------------------------------------------------------



//
//---------------------------------------------------------------------------

HRESULT CActiveScript::CreateScriptEngine()
{
	//CString	s( szItemName );
	//BSTR	bstr = s.AllocSysString();
	
	init_debugger( DEBUGGER_APP_NAME );

	HRESULT hr;

	if (m_pIActiveScript)
		return S_FALSE;   // Already created it

	BSTR bstr;

	CLSID	clsid;
	CString s = _T("VBScript");
	bstr = s.AllocSysString();

	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return E_FAIL;

	::SysFreeString( bstr );

	// Create the ActiveX Scripting Engine
	hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IActiveScript, (void **)&m_pIActiveScript);

	if (hr)
	{
		return E_FAIL;
	}
  
	// Script Engine must support IActiveScriptParse for us to use it
	
	hr = m_pIActiveScript->QueryInterface(IID_IActiveScriptParse, (void **)&m_pIActiveScriptParse);
	if (hr)
	{
		return hr;
	}

	{
		IActiveScriptSite	*psite = 0;

		hr = GetControllingUnknown()->QueryInterface( IID_IActiveScriptSite, (void **)&psite );
		if (hr)
			return hr;

		hr = m_pIActiveScript->SetScriptSite( psite );

		psite->Release();
		if (hr)
			return hr;
	}

	//m_pIActiveScript->SetScriptState(SCRIPTSTATE_INITIALIZED);

	// InitNew the object:
	hr=m_pIActiveScriptParse->InitNew();

	if (hr)
		return hr;

	for( int i = 0; i < theApp.GetComponentCount(); i++ )
	{
		IUnknown *punk = theApp.GetComponent( i );

		if( !CheckInterface( punk, IID_INamedObject2 ) )
		{
			punk->Release();
			continue;
		}
//check, if item provide class info for himself
		DWORD	dwAddFlag = SCRIPTITEM_ISVISIBLE;

		if( CheckInterface( punk, IID_IProvideClassInfo ) )
			dwAddFlag |= SCRIPTITEM_ISSOURCE;


		BSTR	bstr = 0;

		INamedObject2Ptr	sp( punk );
		sp->GetName( &bstr );
		punk->Release();


		CString strName(bstr);

		if( strName == "ActionManager" || strName == "Application" )
			dwAddFlag|=SCRIPTITEM_GLOBALMEMBERS;

		m_pIActiveScript->AddNamedItem( bstr, dwAddFlag );

		::SysFreeString( bstr );
	}

	
	return hr;
}

void CActiveScript::LoadScripts()
{
	CString	strPath = ::MakeAppPathName( "Scripts" );
	strPath+= "\\";
	strPath = ::GetValueString( GetAppUnknown(), "Paths", "Scripts", strPath );
	CString	strFileFind =	::MakeFillFileName( strPath, _T("*.script") );

	CFileFind	ff;

	BOOL bFound = ff.FindFile( strFileFind );

	EnableActionRegistartion( true );

	while( bFound )
	{
		bFound = ff.FindNextFile();

		_bstr_t	bstrFileName( ff.GetFilePath() );

		IUnknown	*punk = ::CreateTypedObject( szArgumentTypeScript );
		if( !punk )
			continue;
		IScriptDataObjectPtr	ptrScript = punk;
		punk->Release();

		ptrScript->ReadFile( bstrFileName );


		char	szName[_MAX_PATH];
		::_splitpath( bstrFileName, 0, 0, szName, 0 );

		::SetObjectName( ptrScript, szName );

		m_Scripts.AddTail( ptrScript.Detach() );
	}
}

void CActiveScript::ClearScripts()
{
    POSITION	pos = m_Scripts.GetHeadPosition();
	while( pos )
	{
		IUnknown* punk = m_Scripts.GetNext( pos );
		punk->Release();
	}
	m_Scripts.RemoveAll();    
}

IUnknown *CActiveScript::GetScriptItemUnk( const char *sz )
{
	for( int i = 0; i < theApp.GetComponentCount(); i++ )
	{
		IUnknown *punk = theApp.GetComponent( i );

		if( !CheckInterface( punk, IID_INamedObject2 ) )
		{
			punk->Release();
			continue;
		}

		BSTR	bstr = 0;

		INamedObject2Ptr	sp( punk );
		sp->GetName( &bstr );
		
		CString		str( bstr );

		::SysFreeString( bstr );

		if( str == sz )
			return punk;
		punk->Release();
	}
	return 0;
}


//***************************************************************************
// IScriptDebugger Interface
//***************************************************************************
IMPLEMENT_UNKNOWN(CActiveScript, Debug);
HRESULT CActiveScript::XDebug::SetBreakOnStart()
{
	METHOD_PROLOGUE(CActiveScript, Debug);
	return E_NOTIMPL;
}

HRESULT CActiveScript::XDebug::ConnectDebugger()
{
	METHOD_PROLOGUE(CActiveScript, Debug);
	return E_NOTIMPL;
}

HRESULT CActiveScript::XDebug::SetScriptText(BSTR bstrScriptText, DWORD* pdwContext)
{
	METHOD_PROLOGUE(CActiveScript, Debug);
	return E_NOTIMPL;
}

HRESULT CActiveScript::XDebug::Reset()
{
	return E_NOTIMPL;
}


HRESULT CActiveScript::XDebug::StartDebugger()
{
	METHOD_PROLOGUE(CActiveScript, Debug);	
	return E_NOTIMPL;
}

//***************************************************************************
// IScriptEngine Interface
//***************************************************************************
HRESULT CActiveScript::XScriptEngine::GetActiveScript( IUnknown** ppi_as )
{
	METHOD_PROLOGUE(CActiveScript, ScriptEngine);
	if( !ppi_as )
		return E_POINTER;

	*ppi_as = (IUnknown*)pThis->m_pIActiveScript;
	if( pThis->m_pIActiveScript )
		pThis->m_pIActiveScript->AddRef();

	return E_NOTIMPL;
}

//***************************************************************************
HRESULT CActiveScript::XScriptEngine::GetActiveScriptParse( IUnknown** ppi_asp )
{
	METHOD_PROLOGUE(CActiveScript, ScriptEngine);
	if( !ppi_asp )
		return E_POINTER;

	*ppi_asp = (IUnknown*)pThis->m_pIActiveScriptParse;
	if( pThis->m_pIActiveScriptParse )
		pThis->m_pIActiveScriptParse->AddRef();

	return E_NOTIMPL;
}
