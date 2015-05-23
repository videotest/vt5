#include "stdafx.h"
#include "scriptdoc.h"
#include "scriptactions.h"
#include "core5.h"
#include "ScriptDataObject.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionStartDebugger, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionToggleBreakpoint, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBreakAtNextStatement, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionScriptChange, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowScript, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionScriptStart, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionScriptTerminate, CCmdTargetEx);


//[ag]4. olecreate

// {B65E40FB-9090-48b1-AF9E-90A1F596A278}
GUARD_IMPLEMENT_OLECREATE(CActionStartDebugger, "ScriptDoc.StartDebugger",
0xb65e40fb, 0x9090, 0x48b1, 0xaf, 0x9e, 0x90, 0xa1, 0xf5, 0x96, 0xa2, 0x78);
// {BA555151-BE6B-44b3-8001-D27B1E19C8BD}
GUARD_IMPLEMENT_OLECREATE(CActionToggleBreakpoint, "ScriptDoc.ToggleBreakpoint",
0xba555151, 0xbe6b, 0x44b3, 0x80, 0x1, 0xd2, 0x7b, 0x1e, 0x19, 0xc8, 0xbd);
// {E59A6C89-9FEA-44f5-9653-D1F60D9DF288}
GUARD_IMPLEMENT_OLECREATE(CActionBreakAtNextStatement, "ScriptDoc.BreakAtNextStatement",
0xe59a6c89, 0x9fea, 0x44f5, 0x96, 0x53, 0xd1, 0xf6, 0xd, 0x9d, 0xf2, 0x88);
// {0E333B2B-6F65-11d3-A659-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionScriptChange, "ScriptDoc.ScriptChange",
0xe333b2b, 0x6f65, 0x11d3, 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {E839C743-2A2C-11d3-A5E2-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionShowScript, "ScriptDoc.ShowScript",
0xe839c743, 0x2a2c, 0x11d3, 0xa5, 0xe2, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {2B862DF5-0DDD-11d3-A5BB-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionScriptStart, "ScriptDoc.ScriptStart", 
0x2b862df5, 0xddd, 0x11d3, 0xa5, 0xbb, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {2B862DF6-0DDD-11d3-A5BB-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionScriptTerminate, "ScriptDoc.ScriptTerminate", 
0x2b862df6, 0xddd, 0x11d3, 0xa5, 0xbb, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

//[ag]5. guidinfo

// {394FAE75-8173-4cde-B0AE-43489DD69A19}
static const GUID guidStartDebugger =
{ 0x394fae75, 0x8173, 0x4cde, { 0xb0, 0xae, 0x43, 0x48, 0x9d, 0xd6, 0x9a, 0x19 } };
// {2EF00F7F-1B70-4ebb-BC4E-85DA4A0EAB6A}
static const GUID guidToggleBreakpoint =
{ 0x2ef00f7f, 0x1b70, 0x4ebb, { 0xbc, 0x4e, 0x85, 0xda, 0x4a, 0xe, 0xab, 0x6a } };
// {B7B52A49-2747-4c2a-BA2A-672EAE9EEDF3}
static const GUID guidBreakAtNextStatement =
{ 0xb7b52a49, 0x2747, 0x4c2a, { 0xba, 0x2a, 0x67, 0x2e, 0xae, 0x9e, 0xed, 0xf3 } };
// {0E333B29-6F65-11d3-A659-0090275995FE}
static const GUID guidScriptChange =
{ 0xe333b29, 0x6f65, 0x11d3, { 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {0E333B25-6F65-11d3-A659-0090275995FE}
static const GUID guidScriptInsertText =
{ 0xe333b25, 0x6f65, 0x11d3, { 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {E839C741-2A2C-11d3-A5E2-0000B493FF1B}
static const GUID guidShowScript =
{ 0xe839c741, 0x2a2c, 0x11d3, { 0xa5, 0xe2, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {2B862DF7-0DDD-11d3-A5BB-0000B493FF1B}
static const GUID guidScriptStartInfo =
{ 0x2b862df7, 0xddd, 0x11d3, { 0xa5, 0xbb, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {2B862DF8-0DDD-11d3-A5BB-0000B493FF1B}
static const GUID guidScriptTerminateInfo = 
{ 0x2b862df8, 0xddd, 0x11d3, { 0xa5, 0xbb, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

//[ag]6. action info

ACTION_INFO_FULL(CActionStartDebugger, IDS_ACTION_SCRIPT_START_DEBUGGER, IDS_MENU_TOOLS, IDS_TB_TOOLS, guidStartDebugger);
ACTION_INFO_FULL(CActionToggleBreakpoint, IDS_ACTION_SCRIPT_TOGGLE_BREAK, IDS_MENU_TOOLS, IDS_TB_TOOLS, guidToggleBreakpoint);
ACTION_INFO_FULL(CActionBreakAtNextStatement, IDS_ACTION_SCRIPT_BREAK_AT_NEXT, IDS_MENU_TOOLS, IDS_TB_TOOLS, guidBreakAtNextStatement);
ACTION_INFO_FULL(CActionScriptStart, IDS_ACTION_SCRIPT_EXECUTE, IDS_MENU_TOOLS, IDS_TB_TOOLS, guidScriptTerminateInfo);
ACTION_INFO_FULL(CActionScriptTerminate, IDS_ACTION_SCRIPT_STOP, IDS_MENU_TOOLS, IDS_TB_TOOLS, guidScriptStartInfo);
ACTION_INFO_FULL(CActionScriptChange, IDS_ACTION_SCRIPTCHANGE, -1, -1, guidScriptChange);
ACTION_INFO_FULL(CActionShowScript, IDS_ACTION_SHOW_SCRIPT, IDS_MENU_VIEW, -1, guidShowScript);

//[ag]7. targets

ACTION_TARGET(CActionStartDebugger, szTargetAnydoc);
ACTION_TARGET(CActionToggleBreakpoint, szTargetViewSite);
ACTION_TARGET(CActionBreakAtNextStatement, szTargetAnydoc);
ACTION_TARGET(CActionScriptChange, szTargetAnydoc);
ACTION_TARGET(CActionShowScript, szTargetFrame);
ACTION_TARGET(CActionScriptStart, szTargetAnydoc);

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionStartDebugger implementation
CActionStartDebugger::CActionStartDebugger()
{
}

CActionStartDebugger::~CActionStartDebugger()
{
}

bool CActionStartDebugger::Invoke()
{
	return false;
	/*
	IApplicationPtr	sptrApp(GetAppUnknown());
	IUnknown* punkAS = 0;
	sptrApp->GetActiveScript(&punkAS);
	IScriptDebuggerPtr	sptrDebugger(punkAS);
	if(punkAS)
		punkAS->Release();

	if(sptrDebugger !=0 )
		sptrDebugger->StartDebugger();

	return true;
	*/
}

//////////////////////////////////////////////////////////////////////
//CActionToggleBreakpoint implementation
CActionToggleBreakpoint::CActionToggleBreakpoint()
{
}

CActionToggleBreakpoint::~CActionToggleBreakpoint()
{
}

bool CActionToggleBreakpoint::Invoke()
{

	sptrIProvideDebuggerIDE	sptrDebugIDE(m_punkTarget);
	if(sptrDebugIDE == 0)
	{
		ASSERT(FALSE);
		return false;
	}

	sptrDebugIDE->ToggleBreakpoint();

	return true;
}


bool CActionToggleBreakpoint::IsAvaible()
{
	sptrIProvideDebuggerIDE	sptrDebug(m_punkTarget);
	return sptrDebug != 0;
}


//////////////////////////////////////////////////////////////////////
//CActionBreakAtNextStatement implementation
CActionBreakAtNextStatement::CActionBreakAtNextStatement()
{
}

bool CActionBreakAtNextStatement::Invoke()
{
	//get data object
	CString	strScriptText = GetScriptText();
	_bstr_t	bstrScriptText = strScriptText;

	::ExecuteScript( bstrScriptText, 0, false, INVALID_KEY, esfBreakOnStart|esfDebugMode);

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionScriptChange implementation
IMPLEMENT_UNKNOWN(CActionScriptChange, SAct);

BEGIN_INTERFACE_MAP(CActionScriptChange, CActionBase)
	INTERFACE_PART(CActionScriptChange, IID_IScriptChangeAction, SAct)
END_INTERFACE_MAP();


HRESULT CActionScriptChange::XSAct::GetObjectKey( long *plKey )
{
	_try_nested(CActionScriptChange, SAct, GetObjectKey)
	{
		*plKey = -1;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CActionScriptChange::XSAct::GetTargetKey( long *plTargetKey )
{
	_try_nested(CActionScriptChange, SAct, GetTargetKey)
	{
		*plTargetKey = -1;
		return S_OK;
	}
	_catch_nested;
}

CActionScriptChange::CActionScriptChange()
{
}

bool CActionScriptChange::Invoke()
{
//Get active context
	sptrIDocumentSite	sptrS( m_punkTarget );
	IUnknown	*punkV = 0;
	sptrS->GetActiveView( &punkV );
	sptrIDataContext	sptrC( punkV );
	punkV->Release();
	if( sptrC == 0 )
		return 0;
//get the active object
	_bstr_t	bstrType = CScriptDataObject::m_szType;
	IUnknown	*punkO = 0;
	sptrC->GetActiveObject( bstrType, &punkO );
	
	m_lObjectKey = GetObjectKey( punkO );

	if( punkO )
		punkO->Release();

	return true;
}

CCrystalTextBuffer *CActionScriptChange::GetCurrentTextBuffer()
{
	sptrIScriptDataObject	sptrO( ::GetObjectByKey( m_punkTarget, m_lObjectKey ) );

	if( sptrO == 0 )
		return false;

	DWORD	dwData = 0;
	sptrO->GetPrivateDataPtr( &dwData );

	ASSERT( dwData );
	return (CCrystalTextBuffer *)dwData;
}

//undo/redo
bool CActionScriptChange::DoUndo()
{
	CCrystalTextBuffer *p = GetCurrentTextBuffer();
	if( !p )
		return false;

	CPoint	pt;
	p->Undo( pt );

	return true;
}

bool CActionScriptChange::DoRedo()
{
	CCrystalTextBuffer *p = GetCurrentTextBuffer();
	if( !p )
		return false;

	CPoint	pt;
	p->Redo( pt );

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionShowScript implementation
CString CActionShowScript::GetViewProgID()
{	return szScriptViewProgID;}


CPtrList	g_ptrListRunningDocuments;

//!!!todo : error information
bool CActionScriptStart::Invoke()
{
	//get data object
	CString	strScriptText = GetScriptText();
	_bstr_t	bstrScriptText = strScriptText;

	
	//POSITION posDoc = g_ptrListRunningDocuments.AddTail( punkObj );

	IUnknown	*punkView = 0;
	IDocumentSitePtr	ptrSite( m_punkTarget );
	ptrSite->GetActiveView( &punkView );

	//_ReportCounter( punkView );
	::ExecuteScript( bstrScriptText, 0, false, INVALID_KEY, esfDebugMode );
	//_ReportCounter( punkView );

	punkView->Release();

	//g_ptrListRunningDocuments.RemoveAt( posDoc );

	return true;
}

bool CActionScriptStart::IsAvaible()
{
	return GetScriptText().IsEmpty() == FALSE;
}

CString CActionScriptStart::GetScriptText()
{
	IUnknown *punkObj = ::GetActiveObjectFromDocument( m_punkTarget, szArgumentTypeScript );
	if( !punkObj )return "";

	m_lScriptKey = ::GetObjectKey( punkObj );
	m_strScriptName = ::GetObjectName( punkObj );

	sptrIScriptDataObject	sptrScript( punkObj );
	punkObj->Release();

	if( sptrScript == 0 )return "";

	BSTR bstr = 0;
	sptrScript->GetText( &bstr );

	CString	strScriptText = bstr;
	::SysFreeString( bstr );

	return strScriptText;
}


bool CActionScriptTerminate::Invoke()
{
	return false;
	/*
	sptrIApplication	sptrA( GetAppUnknown() );
	IUnknown	*punkAS = 0;

	IUnknown *punk = 0;

	sptrA->GetActiveScript( &punk );

	IActiveScriptPtr	sptrAS( punk );
	punk->Release();

	if( sptrAS == 0 )
		return false;


	call( sptrAS->SetScriptState( SCRIPTSTATE_DISCONNECTED ) );

	return true;
	*/
}

bool CActionScriptTerminate::IsAvaible()
{
	return false;
	/*
	sptrIApplication	sptrA( GetAppUnknown() );
	IUnknown	*punkAS = 0;

	IUnknown *punk = 0;

	sptrA->GetActiveScript( &punk );

	IActiveScriptPtr	sptrAS( punk );
	punk->Release();

	if( sptrAS == 0 )
		return false;

	SCRIPTSTATE ss;
	sptrAS->GetScriptState( &ss );

	return ss == SCRIPTSTATE_CONNECTED;
	*/
}
