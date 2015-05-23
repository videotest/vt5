// MacroRecorder.cpp : implementation file
//

#include "stdafx.h"
#include "macros.h"
#include "MacroRecorder.h"
#include "macrodialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacroRecorder

IMPLEMENT_DYNCREATE(CMacroRecorder, CCmdTarget)

CMacroRecorder	*CMacroRecorder::s_pMacroRecorderInstance = 0;

CMacroRecorder::CMacroRecorder()
{
	EnableAutomation();
	
	s_pMacroRecorderInstance = this;
	m_bRecording = false;
	
	_OleLockApp( this );
}

CMacroRecorder::~CMacroRecorder()
{
	s_pMacroRecorderInstance = 0;
	
	_OleUnlockApp( this );
}


void CMacroRecorder::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMacroRecorder, CCmdTarget)
	//{{AFX_MSG_MAP(CMacroRecorder)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMacroRecorder, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CMacroRecorder)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMacroRecorder to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {8E8E8346-6B33-11D3-A652-0090275995FE}
static const IID IID_IMacroRecorder =
{ 0x8e8e8346, 0x6b33, 0x11d3, { 0xa6, 0x52, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

BEGIN_INTERFACE_MAP(CMacroRecorder, CCmdTarget)
	INTERFACE_PART(CMacroRecorder, IID_IMacroRecorder, Dispatch)
	INTERFACE_PART(CMacroRecorder, IID_IRootedObject, Rooted)
	INTERFACE_PART(CMacroRecorder, IID_IEventListener, EvList)
END_INTERFACE_MAP()

// {16B459F1-6B6E-11d3-A652-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMacroRecorder, "macros.MacroRecorder", 
0x16b459f1, 0x6b6e, 0x11d3, 0xa6, 0x52, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

/////////////////////////////////////////////////////////////////////////////
// CMacroRecorder message handlers
void CMacroRecorder::DeInit()
{
	UnRegister();
}

void CMacroRecorder::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !GetRecording() )
		return;
	if( !strcmp( pszEvent, szEventBeforeActionExecute ) )
	{
		CString	strParam, strAction;

		BSTR	bstrParams = 0, bstrActionName = 0;
		IActionPtr	ptrA( punkHit );
		IUnknown	*punkAI = 0;

		ptrA->GetActionInfo( &punkAI );

		ASSERT(punkAI);
		IActionInfoPtr	ptrAI( punkAI );
		punkAI->Release();

		ptrA->StoreParameters( &bstrParams );
		ptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0 );

		if( bstrParams )strParam = bstrParams;
		if( bstrActionName )strAction = bstrActionName;

		::SysFreeString( bstrParams );
		::SysFreeString( bstrActionName );

		m_commands.Add( strAction+" "+strParam );
	}
}

void CMacroRecorder::OnChangeParent()
{
	Register();
}

void CMacroRecorder::Record()
{
	ASSERT( !m_bRecording );

	m_bRecording = true;
}

bool CMacroRecorder::Finalize( IUnknown *punkDD )
{
	ASSERT( m_bRecording );
	CString	strMacroText;

	bool	bFirstLine = true;
							//skip last command
	for( int i = 0; i < m_commands.GetSize()-1; i++ )
	{
		if( !bFirstLine )
			strMacroText+="\r\n";
		strMacroText+=m_commands[i];

		bFirstLine = false;
	}

	Cancel();

	IUnknown *punkObject = ::CreateTypedObject( _T("Script") );

	if( !punkObject )
		return false;

	_bstr_t	bstrText = strMacroText;

	sptrITextObject	sptrT( punkObject );
	sptrT->SetText( bstrText );

	CEnterNameDlg	dlg;

	if( dlg.DoModal() != IDOK )
		return false;

	::SetObjectName( punkObject, dlg.m_strName );
	::SetValue( punkDD, 0, 0, _variant_t( punkObject ) );
	punkObject->Release();

	
	ASSERT( !m_bRecording );

	return true;
}

void CMacroRecorder::Cancel()
{
	m_bRecording = false;
	m_commands.RemoveAll();
}
