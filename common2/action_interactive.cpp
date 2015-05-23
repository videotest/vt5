#include "stdafx.h"
#include "misc_utils.h"
#include "action_interactive.h"
#include "core5.h"
#include "com_main.h"
#include "nameconsts.h"
#include "ScriptNotifyInt.h"

CInteractiveAction::CInteractiveAction()
{
	m_state = Inactive;
	m_hwnd = 0;
	m_hcurDefault = 0;
}
CInteractiveAction::~CInteractiveAction()
{
	if( m_hcurDefault )::DestroyCursor( m_hcurDefault );
}

IUnknown *CInteractiveAction::DoGetInterface( const IID &iid )
{
	if( iid == IID_IInteractiveAction )return (IInteractiveAction*)this;
	else if( iid == IID_IMsgListener )return (IMsgListener*)this;
	else if( iid == IID_IDrawObject )return (IDrawObject*)this;
	else if( iid == IID_IEventListener )return (IEventListener*)this;
	return CAction::DoGetInterface( iid );
}
HRESULT CInteractiveAction::BeginInteractiveMode()
{
	if( m_state != Inactive )
		return E_INVALIDARG;

	if( !Initialize() )
	{
		Finalize();
		return E_INVALIDARG;
	}

	if( m_state != Ready ) m_state = Active; // AAM 19.03.2003 - приводим в соответствие с common

	return S_OK;
}
HRESULT CInteractiveAction::TerminateInteractive()
{
	if( m_state == Active )
	{
		if( !Finalize() )
			return E_INVALIDARG;
		if( m_bTracking ) // A.M. SBT2285. All interactive actions.
		{
			::ReleaseCapture();
			m_bTracking = false;
		}
	}

	m_state = Terminated;

	return S_OK;
}

bool CInteractiveAction::Initialize()
{
	IWindow2Ptr	ptrWindow( m_ptrTarget );
	if( ptrWindow == 0 )return false;

	m_state = Active;

	ptrWindow->AttachDrawing( Unknown() );
	ptrWindow->AttachMsgListener( Unknown() );

	InitMouse( m_ptrTarget );

	ptrWindow->GetHandle( ( HANDLE*)&m_hwnd );

	CEventListenerImpl::Register( ::GetAppUnknown(), true, 0 );

	return true;
}

bool CInteractiveAction::Finalize()
{
	m_state = Ready;

	IWindow2Ptr	ptrWindow( m_ptrTarget );
	if( ptrWindow == 0 )return false;

	ptrWindow->DetachMsgListener( Unknown() );
	ptrWindow->DetachDrawing( Unknown() );

	DeInitMouse();

	CEventListenerImpl::Register( ::GetAppUnknown(), false, 0 );

	// A.M. BT 4047. If non-interacive action, called from ActionHelper, was called during work of the
	// interactive action, user has finalized interactive action before before completion of
	// non-interactive action (i.e. inside CActiveXSite::OnTimer), ActionState_OnActionComplete
	// will be fired only after completion of the non-interactive action.
	// In some circumstances it's needed to terminate non-interactive action immediately.
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS != 0)
	{
		_bstr_t bstrName;
		if (m_ptrAI != 0 )
			m_ptrAI->GetCommandInfo( bstrName.GetAddress(), 0, 0, 0 );
		_bstr_t bstrEvent("ActionState_OnFinalizeInteractive");
		_variant_t vars[2];
		vars[0] = _variant_t((long)0);
		vars[1] = _variant_t(bstrName);
		sptrSS->Invoke(bstrEvent, vars, 2, 0, fwFormScript);
		sptrSS->Invoke(bstrEvent, vars, 2, 0, fwAppScript);
	}

	return true;
}


HRESULT CInteractiveAction::IsComplete()
{
	if(m_state == Ready)return S_OK;
	if(m_state == Terminated)return ERROR_CANCELLED;
	return S_FALSE;
}

LRESULT	CInteractiveAction::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	lResult = CMessageFilterImpl::DoMessage( nMsg, wParam, lParam );
	if( lResult )return lResult;
	lResult = DoMouseMessage( nMsg, wParam, lParam );
	
	return lResult;
}

void CInteractiveAction::LeaveMode()
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	IUnknown	*punkAM = 0; 
	ptrA->GetActionManager( &punkAM );
	IActionManagerPtr	ptrAM( punkAM );

	if( !punkAM )
		return;
	punkAM->Release();

	ptrAM->TerminateInteractiveAction();
}

bool CInteractiveAction::DoChar( int nChar, bool bKeyDown )
{
	if( CMouseImpl::DoChar( nChar, bKeyDown ) )
		return true;

	if( nChar == VK_ESCAPE )
	{
		LeaveMode();
		return true;
	}

	return false;
}

bool CInteractiveAction::DoSetCursor()
{
	if( !m_hcurDefault )
		return false;
	::SetCursor( m_hcurDefault );
	return true;
}

void CInteractiveAction::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventActivateObject ) )
	{
		if( ::GetKey( punkFrom ) == ::GetKey( m_ptrTarget ) )
		{			
			DoActivateObjects( punkHit );
		}
	}

}