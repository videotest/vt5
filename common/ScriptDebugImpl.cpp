// ScriptDebugImpl.cpp: implementation of the CScriptDebugImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "common.h"
#include "ScriptDebugImpl.h"


#ifndef NOT_COMPATIBLE_VT5
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScriptDebugImpl::CScriptDebugImpl()
{
	m_punkScriptDebug = 0;
}

//////////////////////////////////////////////////////////////////////
CScriptDebugImpl::~CScriptDebugImpl()
{
	deinit_debuger();
//	ULONG ul=m_punkScriptDebug->Release();
//	_ASSERTE(ul==0);
}

//////////////////////////////////////////////////////////////////////
bool CScriptDebugImpl::init_debugger( const char* ps_app_name )
{
	deinit_debuger();

	bool bEnableDebuger = ( ::GetValueInt( ::GetAppUnknown(), DEBUGGER_SECTION, DEBUGGER_ENABLE, 0L ) == 1L );
	
	if( !bEnableDebuger )
		return false;

	bool bCreateDebugApp = false;
	CDebugHost *p = new CDebugHost;
	{
		IDebugHostPtr ptrDH( p->GetControllingUnknown() );
		if( ptrDH )		
			bCreateDebugApp = ( ptrDH->Init( _bstr_t( ps_app_name ) ) == S_OK );
		
	}

	if( bCreateDebugApp ) 
	{
		p->m_pOuterUnknown = GetCmdTarget()->GetControllingUnknown();
		m_punkScriptDebug = (IUnknown*)&p->m_xInnerUnknown;
	}
	else
	{
		delete p;
	}

	return bCreateDebugApp;
}

/////////////////////////////////////////////////////////////////////////////
bool CScriptDebugImpl::before_parse_text(	const char* psz_script, 
										const char* psz_file_name, 
										const char* psz_title, 
										IActiveScript *pIActiveScript,
										DWORD* pdw_script_context
										)
{
	if( !psz_script || !pIActiveScript )
		return false;

	IDebugHostPtr ptrDH( GetCmdTarget()->GetControllingUnknown() );
	if( ptrDH == 0 )
		return false;

	HRESULT hr = ptrDH->OnRunScript( 
						_bstr_t(psz_script), 
						_bstr_t(psz_file_name), 
						_bstr_t(psz_title),
						pIActiveScript, 
						pdw_script_context );

	return ( hr == S_OK );
}

/////////////////////////////////////////////////////////////////////////////
void CScriptDebugImpl::deinit_debuger()
{
	if( m_punkScriptDebug ){
		ULONG ul=m_punkScriptDebug->Release();
		_ASSERTE(ul==0);
		m_punkScriptDebug=0;
	}
}
#endif//NOT_COMPATIBLE_VT5

/////////////////////////////////////////////////////////////////////////////
//
//
//	class CDebugHost
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDebugHost, CCmdTarget)


/////////////////////////////////////////////////////////////////////////////
CDebugHost::CDebugHost()
{	
	EnableAggregation();

	m_pdm				= 0;
	m_pDebugApp			= 0;
	m_dwAppCookie		= 0;	
}

/////////////////////////////////////////////////////////////////////////////
CDebugHost::~CDebugHost()
{
	_de_init();
}

/////////////////////////////////////////////////////////////////////////////
// CLSID for ProcessDebugManager
// {78A51822-51F4-11D0-8F20-00805F2CD064}
static const GUID __CLSID_ProcessDebugManager =
{ 0x78a51822, 0x51f4, 0x11d0, { 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64 } };

static const GUID __IID_IActiveScriptSiteDebug = 
{ 0x51973C11, 0xcb0c, 0x11d0, { 0xb5, 0xc9, 0x0, 0xa0, 0x24, 0x4a, 0x0e, 0x7a } };

static const GUID __IID_IProcessDebugManager = 
{ 0x51973C2f, 0xcb0c, 0x11d0, { 0xb5, 0xc9, 0x0, 0xa0, 0x24, 0x4a, 0x0e, 0x7a } };


BEGIN_INTERFACE_MAP(CDebugHost, CCmdTarget)
	INTERFACE_PART(CDebugHost, __IID_IActiveScriptSiteDebug, ActiveScriptSiteDebug)
    INTERFACE_PART(CDebugHost, IID_IDebugDocumentHost, DebugDocumentHost)
    INTERFACE_PART(CDebugHost, IID_IDebugHost, DebugHost)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDebugHost, ActiveScriptSiteDebug);
IMPLEMENT_UNKNOWN(CDebugHost, DebugDocumentHost);
IMPLEMENT_UNKNOWN(CDebugHost, DebugHost);



/////////////////////////////////////////////////////////////////////////////
bool CDebugHost::_init( const char* psz_app_name )
{
	_de_init();

	HRESULT hr = E_FAIL;

	hr = CoCreateInstance( __CLSID_ProcessDebugManager, 
			 NULL, 
			 CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
			 __IID_IProcessDebugManager,
			 (void**)&m_pdm);

	#ifndef NOT_COMPATIBLE_VT5
	::SetValue( ::GetAppUnknown(), DEBUGGER_SECTION, DEBUGGER_PRESENT, ( ( hr == S_OK ) ? "Yes" : "No" ) );
	#endif//NOT_COMPATIBLE_VT5

	if( hr != S_OK ) 
		return false;

	// Initialise the application
	hr = m_pdm->CreateApplication( &m_pDebugApp );	
	
	VERIFY( S_OK == hr );
	if( FAILED( hr ) )	return false;

	hr = m_pDebugApp->SetName( _bstr_t( psz_app_name ) );
	VERIFY( S_OK == hr );
	
	hr = m_pdm->AddApplication( m_pDebugApp, &m_dwAppCookie );
	VERIFY( S_OK == hr );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
void CDebugHost::_de_init()
{
	for( int i=0;i<m_ar_debug_doc.GetSize();i++ )
		delete m_ar_debug_doc[i];

	m_ar_debug_doc.RemoveAll();


	if( m_pDebugApp )
	{
		if( m_pdm )
			VERIFY( S_OK == m_pdm->RemoveApplication( m_dwAppCookie ) );

		VERIFY( S_OK == m_pDebugApp->Close() );

		m_pDebugApp->Release();
		m_pDebugApp = 0;
	}

	if( m_pdm )
	{
		m_pdm->Release();
		m_pdm = 0;
	}	
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDebugHost::XDebugHost::Init( BSTR bstr_app_name )
{	
	METHOD_PROLOGUE(CDebugHost, DebugHost)
	return pThis->_init( _bstr_t(bstr_app_name) ) ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDebugHost::XDebugHost::OnRunScript( BSTR bstrScript, BSTR bstrFileName, BSTR bstrTitle, IActiveScript *pIActiveScript, DWORD* pdw_context )
{	
	METHOD_PROLOGUE(CDebugHost, DebugHost)
	
	if( !bstrScript || !bstrFileName || !bstrTitle || !pIActiveScript || 
		!pThis->m_pdm || !pThis->m_pDebugApp )
		return S_FALSE;

	pThis->m_strScript		= bstrScript;
	pThis->m_strFileName	= bstrFileName;
	pThis->m_strTitle		= bstrTitle;

	HRESULT	hr = S_OK;

	ATL::CComPtr<IDebugDocumentHelper>	pi_DebugDocHelper;

	hr = pThis->m_pdm->CreateDebugDocumentHelper( NULL, &pi_DebugDocHelper );	

	if( !SUCCEEDED(hr) )
	{
		//AfxMessageBox("Failed to create debug document. Stage 1.", MB_ICONERROR );
		return S_FALSE;
	}

	script_debug_doc* pdoc = new script_debug_doc;
	pThis->m_ar_debug_doc.Add( pdoc );
	pdoc->m_pDebugDocHelper = pi_DebugDocHelper;	

	hr = pi_DebugDocHelper->Init( pThis->m_pDebugApp, bstrFileName, bstrTitle, TEXT_DOC_ATTR_READONLY );

	if( !SUCCEEDED(hr) )
	{
		//AfxMessageBox("Failed to create debug document. Stage 2.", MB_ICONERROR );
		return S_FALSE;		
	}

	hr = pi_DebugDocHelper->Attach(NULL);
	if (!SUCCEEDED(hr))
	{
		//AfxMessageBox("Failed to create debug document. Stage 3.", MB_ICONERROR );
		return S_FALSE;		
	}

	hr = pi_DebugDocHelper->SetDocumentAttr(TEXT_DOC_ATTR_READONLY);
	if (!SUCCEEDED(hr))
	{
		//AfxMessageBox("Failed to create debug document. Stage 4.", MB_ICONERROR );
		return S_FALSE;		
	}

	// this step is optional but it deosn't require much effort to perform	
	hr = pi_DebugDocHelper->SetDebugDocumentHost( &pThis->m_xDebugDocumentHost );
	if (!SUCCEEDED(hr))
	{
		//AfxMessageBox( "Failed to create debug document Stage 5.", MB_ICONERROR );
		return S_FALSE;		
	}

	hr = pi_DebugDocHelper->AddDBCSText( pThis->m_strScript );
	if( !SUCCEEDED(hr) )
	{
		//AfxMessageBox( "Problem adding text to the debug document. Stage 1.", MB_ICONERROR );
		return S_FALSE;
	}

	DWORD dw = 0;
	hr = pi_DebugDocHelper->DefineScriptBlock( 0, pThis->m_strScript.GetLength(), pIActiveScript, FALSE, &dw );
	if( !SUCCEEDED(hr) )
	{
		//AfxMessageBox( "Problem adding text to the debug document. Stage 2.", MB_ICONERROR );
		return S_FALSE;
	}

	pdoc->SetData(dw);
	if( pdw_context )
		*pdw_context = dw;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDebugHost::XDebugHost::CloseScript( DWORD dw_context )
{
	METHOD_PROLOGUE(CDebugHost, DebugHost)
   for( int i=0;i<pThis->m_ar_debug_doc.GetSize();i++ )
   {
	   if( pThis->m_ar_debug_doc[i]->GetData() == dw_context )
	   {
		   delete pThis->m_ar_debug_doc[i];
		   pThis->m_ar_debug_doc.RemoveAt( i );
		   return S_OK;
	   }
   }

   return S_FALSE;		
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDebugHost::XDebugHost::ShowDebugger( DWORD dw_context )
{
	METHOD_PROLOGUE(CDebugHost, DebugHost)

	IDebugDocumentHelper* pdoc = 0;

	for( int i=0;i<pThis->m_ar_debug_doc.GetSize();i++ )
	{
		if( dw_context == -1 )
		{
			pdoc = pThis->m_ar_debug_doc[i]->m_pDebugDocHelper;
			break;
		}

		if( pThis->m_ar_debug_doc[i]->GetData() == dw_context )
		{
			pdoc = pThis->m_ar_debug_doc[i]->m_pDebugDocHelper;
			break;
		}
	}
	
	if( !pdoc )
		return S_FALSE;

	return pdoc->BringDocumentToTop();
}

	
/////////////////////////////////////////////////////////////////////////////
HRESULT CDebugHost::XDebugHost::BreakIntoDebuger( )
{
	METHOD_PROLOGUE(CDebugHost, DebugHost)
	
	if( !pThis->m_pDebugApp )
		return S_FALSE;

	HRESULT hr = pThis->m_pDebugApp->CauseBreak();

	return hr;
}
/////////////////////////////////////////////////////////////////////////////
// IActiveScriptSiteDebug Implementation
// Used by the language engine to delegate IDebugCodeContext::GetSourceContext. 
STDMETHODIMP CDebugHost::XActiveScriptSiteDebug::GetDocumentContextFromPosition(
      DWORD dwSourceContext,// As provided to ParseScriptText 
                            // or AddScriptlet 
      ULONG uCharacterOffset,// character offset relative 
                             // to start of script block or scriptlet 
      ULONG uNumChars,// Number of characters in context 
                      // Returns the document context corresponding to this character-position range. 
      IDebugDocumentContext **ppsc)
{
	METHOD_PROLOGUE(CDebugHost, ActiveScriptSiteDebug)

   ULONG ulStartPos = 0;
   HRESULT hr;

   IDebugDocumentHelper* pdoc = 0;
   for( int i=0;i<pThis->m_ar_debug_doc.GetSize();i++ )
   {
	   if( pThis->m_ar_debug_doc[i]->GetData() == dwSourceContext )
	   {
		   pdoc = pThis->m_ar_debug_doc[i]->m_pDebugDocHelper;
		   break;
	   }
   }

   if (pdoc)
   {
      hr = pdoc->GetScriptBlockInfo(dwSourceContext, NULL, &ulStartPos, NULL);
      hr = pdoc->CreateDebugDocumentContext(ulStartPos + uCharacterOffset, uNumChars, ppsc);
   }
   else
   {
      hr = E_NOTIMPL;
   }

	return hr;
}

// Returns the debug application object associated with this script site. Provides 
// a means for a smart host to define what application object each script belongs to. 
// Script engines should attempt to call this method to get their containing application 
// and resort to IProcessDebugManager::GetDefaultApplication if this fails. 
STDMETHODIMP CDebugHost::XActiveScriptSiteDebug::GetApplication( 
      IDebugApplication **ppda)
{
	METHOD_PROLOGUE(CDebugHost, ActiveScriptSiteDebug)

   if (!ppda)
   {
      return E_INVALIDARG;
   }

   // bugbug - should addref to this ?
   if (pThis->m_pDebugApp)
   {
      ULONG ul = pThis->m_pDebugApp->AddRef();
   }

   *ppda = pThis->m_pDebugApp;

	return S_OK;
}

// Gets the application node under which script documents should be added 
// can return NULL if script documents should be top-level. 
STDMETHODIMP CDebugHost::XActiveScriptSiteDebug::GetRootApplicationNode( 
      IDebugApplicationNode **ppdanRoot)
{
	METHOD_PROLOGUE(CDebugHost, ActiveScriptSiteDebug)

   if (!ppdanRoot)
   {
      return E_INVALIDARG;
   }

   IDebugDocumentHelper* pi_doc = 0;

   if( pThis->m_ar_debug_doc.GetSize() )
	   pi_doc = pThis->m_ar_debug_doc[0]->m_pDebugDocHelper;

   if( pi_doc )
      return pi_doc->GetDebugApplicationNode(ppdanRoot);

   return E_NOTIMPL;
}

// Allows a smart host to control the handling of runtime errors 
STDMETHODIMP CDebugHost::XActiveScriptSiteDebug::OnScriptErrorDebug( 
      // the runtime error that occurred 
      IActiveScriptErrorDebug *pErrorDebug, 
      // whether to pass the error to the debugger to do JIT debugging 
      BOOL*pfEnterDebugger, 
      // whether to call IActiveScriptSite::OnScriptError() when the user 
      // decides to continue without debugging 
      BOOL *pfCallOnScriptErrorWhenContinuing)
{
	METHOD_PROLOGUE(CDebugHost, ActiveScriptSiteDebug)

   if (pfEnterDebugger)
   {
      *pfEnterDebugger = TRUE;
   }
   if (pfCallOnScriptErrorWhenContinuing)
   {
      *pfCallOnScriptErrorWhenContinuing = TRUE;
   }
   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDebugDocumentHost Implementation

// Return a particular range of characters in the original host document, 
// added using AddDeferredText. 
// 
// It is acceptable for a host to return E_NOTIMPL for this method, 
// as long as the host doesn't call AddDeferredText. 
// 
// (Note that this is text from the _original_ document. The host 
// does not need to be responsible for keeping track of edits, etc.) 
STDMETHODIMP CDebugHost::XDebugDocumentHost::GetDeferredText( 
      DWORD dwTextStartCookie, 
      // Specifies a character text buffer. NULL means do not return characters. 
      WCHAR *pcharText, 
      // Specifies a character attribute buffer. NULL means do not return attributes. 
      SOURCE_TEXT_ATTR *pstaTextAttr, 
      // Indicates the actual number of characters/attributes returned. Must be set to zero 
      // before the call. 
      ULONG *pcNumChars, 
      // Specifies the number maximum number of character desired. 
      ULONG cMaxChars) 
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   return E_NOTIMPL;
}

// Return the text attributes for an arbitrary block of document text. 
// It is acceptable for hosts to return E_NOTIMPL, in which case the 
// default attributes are used. 
STDMETHODIMP CDebugHost::XDebugDocumentHost::GetScriptTextAttributes( 
      // The script block text. This string need not be null terminated. 
      LPCOLESTR pstrCode, 
      // The number of characters in the script block text. 
      ULONG uNumCodeChars, 
      // See IActiveScriptParse::ParseScriptText for a description of this argument. 
      LPCOLESTR pstrDelimiter, 
      // See IActiveScriptParse::ParseScriptText for a description of this argument. 
      DWORD dwFlags, 
      // Buffer to contain the returned attributes. 
      SOURCE_TEXT_ATTR *pattr) 
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   return E_NOTIMPL;
}

// Notify the host that a new document context is being created 
// and allow the host to optionally return a controlling unknown 
// for the new context. 
// 
// This allows the host to add new functionality to the helper-provided 
// document contexts. It is acceptable for the host to return E_NOTIMPL 
// or a null outer unknown for this method, in which case the context is 
// used "as is". 
STDMETHODIMP CDebugHost::XDebugDocumentHost::OnCreateDocumentContext( 
      IUnknown** ppunkOuter) 
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   return E_NOTIMPL;
}

// Return the full path (including file name) to the document's source file. 
//*pfIsOriginalPath is TRUE if the path refers to the original file for the document. 
//*pfIsOriginalPath is FALSE if the path refers to a newly created temporary file 
//Returns E_FAIL if no source file can be created/determined. 
STDMETHODIMP CDebugHost::XDebugDocumentHost::GetPathName( 
      BSTR *pbstrLongName, 
      BOOL *pfIsOriginalFile)
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   *pbstrLongName = pThis->m_strFileName.AllocSysString();
   *pfIsOriginalFile = TRUE;

   return S_OK;
}

// Return just the name of the document, with no path information. 
// (Used for "Save As...") 
STDMETHODIMP CDebugHost::XDebugDocumentHost::GetFileName( 
      BSTR *pbstrShortName) 
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   // bugbug - shorten name !
   *pbstrShortName = pThis->m_strFileName.AllocSysString();

   return S_OK;
}

// Notify the host that the document's source file has been saved and 
// that its contents should be refreshed. 
STDMETHODIMP CDebugHost::XDebugDocumentHost::NotifyChanged()
{
	METHOD_PROLOGUE(CDebugHost, DebugDocumentHost)

   return S_OK;
}

