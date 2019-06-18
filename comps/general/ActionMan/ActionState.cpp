// ActionState.cpp : implementation file
//

#include "stdafx.h"
#include "actionman.h"
#include "ActionState.h"
#include "undoint.h"
#include "ScriptNotifyInt.h"
#include "ActionManager.h"

/////////////////////////////////////////////////////////////////////////////
// CActionState

IMPLEMENT_DYNCREATE(CActionState, CCmdTargetEx)

// {080118ED-98CC-47b6-B1A2-7D585C698FAA}
static const GUID guid_events = 
{ 0x80118ed, 0x98cc, 0x47b6, { 0xb1, 0xa2, 0x7d, 0x58, 0x5c, 0x69, 0x8f, 0xaa } };
// {C416B1E1-27D9-49A9-995C-D2986871C344}
static const IID IID_IActionState =
{ 0xc416b1e1, 0x27d9, 0x49a9, { 0x99, 0x5c, 0xd2, 0x98, 0x68, 0x71, 0xc3, 0x44 } };

CActionState	*g_pActionState = 0;

CActionState::CActionState()
{
	m_piidEvents = &guid_events;
	m_piidPrimary = &IID_IActionState;


	EnableAutomation();
	EnableConnections();
	
	m_sName = "ActionState";

	g_pActionState = this;
	
	_OleLockApp( this );
}

CActionState::~CActionState()
{
	g_pActionState = 0; 
	_OleUnlockApp( this );
}


void CActionState::OnFinalRelease()
{
	m_ptrActionManager = 0;

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CActionState, CCmdTargetEx)
	//{{AFX_MSG_MAP(CActionState)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CActionState, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CActionState)
	DISP_FUNCTION(CActionState, "TerminateCurrentAction", TerminateCurrentAction, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActionState, "GetCurrentAction", GetCurrentAction, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CActionState, "GetActionState", GetActionState, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CActionState, "SetActionState", SetActionState, VT_EMPTY, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CActionState, "ResetActionState", ResetActionState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CActionState, "ResetAllActionStates", ResetAllActionStates, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActionState, "IsActionAvaible", IsActionAvaible, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CActionState, "RunInteractiveWait", RunInteractiveWait, VT_I4, VTS_BSTR )
	DISP_FUNCTION(CActionState, "BeginGroupUndo", BeginGroupUndo, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CActionState, "EndGroupUndo", EndGroupUndo, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActionState, "ClearUndoList", ClearUndoList, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActionState, "RunInteractive", RunInteractive, VT_I4, VTS_BSTR )
	DISP_FUNCTION(CActionState, "ClearRedoList", ClearRedoList, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActionState, "FireNotify", FireNotify, VT_BOOL, VTS_BSTR VTS_I4 VTS_VARIANT )
	DISP_PROPERTY_EX(CActionState, "EnableUndo", GetEnableUndo, SetEnableUndo, VT_BOOL)
	DISP_FUNCTION(CActionState, "TerminateLongOperation", TerminateLongOperation, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CActionState, "RunInteractiveEx", RunInteractiveEx, VT_I4, VTS_BSTR VTS_BSTR )
	DISP_FUNCTION(CActionState, "RunInteractiveWaitEx", RunInteractiveWaitEx, VT_I4, VTS_BSTR VTS_BSTR )
	DISP_FUNCTION(CActionState, "BeginGroupUndo2", BeginGroupUndo2, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CActionState, "DeleteLastUndo", DeleteLastUndo, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActionState, "EndGroupUndo2", EndGroupUndo2, VT_EMPTY, VTS_BSTR VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IActionState to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CActionState, CCmdTargetEx)
	INTERFACE_PART(CActionState, IID_IActionState, Dispatch)
	INTERFACE_PART(CActionState, IID_INamedObject2, Name)
	INTERFACE_PART(CActionState, IID_IRootedObject, Rooted)
	INTERFACE_PART(CActionState, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(CActionState, IID_IProvideClassInfo, ProvideClassInfo)
END_INTERFACE_MAP()

BEGIN_EVENT_MAP(CActionState, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(CActionState)
	EVENT_CUSTOM("OnActionComplete", FireOnActionComplete, VTS_BSTR  VTS_I4)
	EVENT_CUSTOM("OnActionRunning", FireOnActionRunning, VTS_BSTR)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

// {538D4938-AD2A-4D4B-BA2F-37477DA9FA3F}
GUARD_IMPLEMENT_OLECREATE(CActionState, "ActionMan.ActionState", 
0x538d4938, 0xad2a, 0x4d4b, 0xba, 0x2f, 0x37, 0x47, 0x7d, 0xa9, 0xfa, 0x3f)

extern GUID IID_ITypeLibID;

IMPLEMENT_OLETYPELIB(CActionState, IID_ITypeLibID, 1, 0)



/////////////////////////////////////////////////////////////////////////////
// CActionState message handlers

void CActionState::TerminateCurrentAction() 
{
	m_ptrActionManager->TerminateInteractiveAction();
}

BSTR CActionState::GetCurrentAction() 
{
	IUnknown	*punkActionInfo = 0;
	m_ptrActionManager->GetRunningInteractiveActionInfo( &punkActionInfo );

	if( !punkActionInfo )return 0;

	IActionInfoPtr	ptrAI( punkActionInfo );
	punkActionInfo->Release();

	BSTR	bstrActionName;
	ptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0 );

	return bstrActionName;
}

long CActionState::GetActionState(LPCTSTR szAction) 
{
	DWORD	dwFlags = 0;
	_bstr_t	bstrAction( szAction );

	if( m_ptrActionManager->GetActionFlags( bstrAction, &dwFlags )!= S_OK )
		return 0;

	return dwFlags;
}

void CActionState::OnChangeParent()
{
	IUnknown	*punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	m_ptrActionManager = punkAM;
	if( punkAM )punkAM->Release();
}

void CActionState::SetActionState(LPCTSTR szAction, long dwNewState) 
{
	_bstr_t	bstrAction( szAction );
	m_ptrActionManager->SetActionFlags( bstrAction, dwNewState );
}

void CActionState::ResetActionState(LPCTSTR szAction) 
{
	_bstr_t	bstrAction( szAction );
	m_ptrActionManager->ResetActionFlags( bstrAction );
}

void CActionState::ResetAllActionStates() 
{
	m_ptrActionManager->ResetAllActionsFlags();
}

BOOL CActionState::IsActionAvaible(LPCTSTR szAction) 
{
	IUnknown	*punkAI = 0;
	_bstr_t		bstrAction = szAction;
	m_ptrActionManager->GetActionInfo( bstrAction, &punkAI );

	bool	bPresent = false;

	if( punkAI )
	{
		bPresent = true;
		punkAI->Release();
	}

	return bPresent;
}

long CActionState::RunInteractiveWait(LPCTSTR szAction) 
{
	IDispatchPtr	ptrDisp( m_ptrActionManager );
	_bstr_t	bstrA( szAction );
	DISPID	dispID = -1;
	BSTR	bstrAction = bstrA;
	if( ptrDisp->GetIDsOfNames( IID_IActionManager, &bstrAction, 1, 0, &dispID ) )
		return 0;

	_variant_t	var( 0l);

	BOOL	bScriptRunning = m_ptrActionManager->IsScriptRunning() == S_OK;
	m_ptrActionManager->SetScriptRunningFlag( true );
	ptrDisp->Invoke( dispID, IID_IUnknown, 0, 0, 0, &var, 0, 0 );
	m_ptrActionManager->SetScriptRunningFlag( bScriptRunning  );

	return (long)var;
}

long CActionState::BeginGroupUndo(LPCTSTR szGroupName) 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return 1;
	IUndoListPtr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return 2;

	ptr_list->BeginGroup( _bstr_t( szGroupName ) );

	return 0;
}

long CActionState::BeginGroupUndo2(LPCTSTR szGroupName, long dwStepsBack) 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return 1;
	IUndoList2Ptr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return 2;

	ptr_list->BeginGroup2( _bstr_t( szGroupName ), dwStepsBack );

	return 0;
}

void CActionState::EndGroupUndo(long lCancel) 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return;
	IUndoListPtr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return;

	ptr_list->EndGroup( lCancel );
}

void CActionState::EndGroupUndo2(LPCTSTR szGroupName, long lCancel) 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return;
	IUndoList2Ptr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return;

	ptr_list->EndGroup2( _bstr_t(szGroupName), lCancel );
}

void CActionState::ClearUndoList() 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return;
	IUndoListPtr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return;

	ptr_list->ClearUndoRedoList();
}

long CActionState::RunInteractive(LPCTSTR szAction ) 
{
	IDispatchPtr	ptrDisp( m_ptrActionManager );
	_bstr_t	bstrA( szAction );
	DISPID	dispID = -1;
	BSTR	bstrAction = bstrA;
	if( ptrDisp->GetIDsOfNames( IID_IActionManager, &bstrAction, 1, 0, &dispID ) )
		return 0;

	_variant_t	var( 0l);

	BOOL	bScriptRunning = m_ptrActionManager->IsScriptRunning() == S_OK;
	m_ptrActionManager->SetScriptRunningFlag( false );
	ptrDisp->Invoke( dispID, IID_IUnknown, 0, 0, 0, &var, 0, 0 );
	m_ptrActionManager->SetScriptRunningFlag( bScriptRunning  );

	return (long)var;

}

void CActionState::ClearRedoList() 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return;
	IUndoListPtr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return;

	ptr_list->ClearRedoList();
}

BOOL CActionState::FireNotify( LPCTSTR lpctNotifyDesc, long dwFlagLevel, VARIANT &varValue )
{
	BOOL bAction = false;
	if( dwFlagLevel & nfAction )
	{
		IUnknownPtr pUnk;
		m_ptrActionManager->GetRunningInteractiveAction( &pUnk );
		if( pUnk != 0 )
		{
			IScriptNotifyListnerPtr sptrListner = pUnk;
			if( sptrListner != 0 )
			{
				sptrListner->ScriptNotify( _bstr_t( lpctNotifyDesc ), varValue );
				bAction = true;
			}
		}
	}

	if( dwFlagLevel & nfDocument )
	{
		IApplicationPtr ptrApp = ::GetAppUnknown();

		if( dwFlagLevel & nfCurrentDocOnly )
		{
			IUnknownPtr pUnkDoc;
			ptrApp->GetActiveDocument( &pUnkDoc );
			INotifyControllerPtr ptrNotify = pUnkDoc;
			if( ptrNotify->FireEvent( _bstr_t( lpctNotifyDesc ), 0, 0, (LPVOID)&varValue, sizeof( varValue ) ) != S_OK )
				return false || bAction;

		}
		else
		{
			long lTemplPos = 0;
			ptrApp->GetFirstDocTemplPosition( &lTemplPos );
			BOOL bOK = false;

			while( lTemplPos )
			{
				ptrApp->GetNextDocTempl( &lTemplPos, 0, 0 );

				long lPos = 0;
				ptrApp->GetFirstDocPosition( lTemplPos, &lPos );
				while( lPos )
				{
					IUnknownPtr pUnkDoc;
					ptrApp->GetNextDoc( lTemplPos, &lPos , &pUnkDoc );
					if( pUnkDoc != 0 )
					{
						INotifyControllerPtr ptrNotify = pUnkDoc;
						if( ptrNotify )
						{
							if( ptrNotify->FireEvent( _bstr_t( lpctNotifyDesc ), 0, 0, (LPVOID)&varValue, sizeof( varValue ) ) == S_OK && !bOK )
								bOK = true;
						}
					}
				}
			}
			return bOK || bAction;
		}
	}

	return bAction;
}

BOOL CActionState::GetEnableUndo()
{
	BOOL benable = true;

	IApplication* pi_app = GetAppUnknown();
	if( pi_app )
	{
		IUnknown* punk_doc = 0;
		pi_app->GetActiveDocument( &punk_doc );
		if( punk_doc )
		{
			IUndoListPtr	ptr_list( punk_doc );
			punk_doc->Release();	punk_doc = 0;
			if( ptr_list )			
				ptr_list->GetEnableUndo( &benable );
		}		
	}

	return benable;
}

void CActionState::SetEnableUndo( BOOL bNewValue )
{
	IApplication* pi_app = GetAppUnknown();
	if( pi_app )
	{
		IUnknown* punk_doc = 0;
		pi_app->GetActiveDocument( &punk_doc );
		if( punk_doc )
		{
			IUndoListPtr	ptr_list( punk_doc );
			punk_doc->Release();	punk_doc = 0;
			if( ptr_list )			
				ptr_list->SetEnableUndo( bNewValue );
		}		
	}	
}

BOOL CActionState::TerminateLongOperation()
{
	if( !CActionManager::s_pActionManager )	return false;

	ILongOperationPtr ptr_long( CActionManager::s_pActionManager->m_ptr_running_action );
	if( ptr_long == 0 )	return false;
	ptr_long->Abort();
	return true;
	//m_ptrActionManager->
}

long CActionState::RunInteractiveEx(LPCTSTR szAction, LPCTSTR szActionParam ) 
{
	IDispatchPtr	ptrDisp( m_ptrActionManager );
	_bstr_t	bstrA( szAction );
	DISPID	dispID = -1;
	BSTR	bstrAction = bstrA;
	if( ptrDisp->GetIDsOfNames( IID_IActionManager, &bstrAction, 1, 0, &dispID ) )
		return 0;

	_variant_t	var( 0l);

	BOOL	bScriptRunning = m_ptrActionManager->IsScriptRunning() == S_OK;
	m_ptrActionManager->SetScriptRunningFlag( false );

	DISPPARAMS *pParams = CActionManager::PhraseArguments( szActionParam );

	ptrDisp->Invoke( dispID, IID_IUnknown, 0, 0, pParams, &var, 0, 0 );

	if( pParams )
		delete []pParams;

	m_ptrActionManager->SetScriptRunningFlag( bScriptRunning  );

	return (long)var;

}

long CActionState::RunInteractiveWaitEx(LPCTSTR szAction, LPCTSTR szActionParam ) 
{
	IDispatchPtr	ptrDisp( m_ptrActionManager );
	_bstr_t	bstrA( szAction );
	DISPID	dispID = -1;
	BSTR	bstrAction = bstrA;
	if( ptrDisp->GetIDsOfNames( IID_IActionManager, &bstrAction, 1, 0, &dispID ) )
		return 0;

	_variant_t	var( 0l);

	BOOL	bScriptRunning = m_ptrActionManager->IsScriptRunning() == S_OK;
	m_ptrActionManager->SetScriptRunningFlag( true );

	DISPPARAMS *pParams = CActionManager::PhraseArguments( szActionParam );

	ptrDisp->Invoke( dispID, IID_IUnknown, 0, 0, pParams, &var, 0, 0 );

	if( pParams )
		delete []pParams;

	m_ptrActionManager->SetScriptRunningFlag( bScriptRunning  );

	return (long)var;
}

void CActionState::DeleteLastUndo() 
{
	IApplication	*papp = GetAppUnknown();

	IUnknown	*punk_doc = 0;
	papp->GetActiveDocument( &punk_doc );

	if( !punk_doc )return;
	IUndoList2Ptr	ptr_list( punk_doc );
	punk_doc->Release();

	if( ptr_list == 0 )return;

	ptr_list->DeleteLastUndoAction();
}