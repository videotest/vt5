#include "stdafx.h"
#include "MacroMan.h"
#include "macro_utils.h"
#include "misc_utils.h"

#include "script_int.h"
#include <activscp.h>
#include "core5.h"
#include "action5.h"

#include "misc_utils.h"
#include "misc_dbg.h"

#include "nameconsts.h"

bool g_ballow_exit_now = false;

static void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

CMacroManager *CMacroManager::s_pMacroManager = 0;
HHOOK	hook = 0;

// msghook
static LRESULT CALLBACK msghook(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0)
	{ // pass it on 
		CallNextHookEx(hook, nCode, wParam, lParam);
		return 0;
	}

	LPMSG lpmsg = (LPMSG)lParam;
	if(  wParam == PM_REMOVE && lpmsg && CMacroManager::s_pMacroManager )  
		CMacroManager::s_pMacroManager->ProcessMessageHandler( lpmsg );

	return CallNextHookEx(hook, nCode, wParam, lParam);
}

// timer procedure
void CALLBACK MacroManagerTimerProc(HWND hWnd, UINT nMsg, UINT_PTR idEvent, DWORD dwTime)
{
	::KillTimer( 0, idEvent );

	if( CMacroManager::s_pMacroManager )
		CMacroManager::s_pMacroManager->OnTimer( );
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CMacroManager
//
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////
CMacroManager::CMacroManager(void) : IDispatchImpl< IMacroManagerDisp >( "MacroRecorder.tlb" )
{
	if( !s_pMacroManager  )
		s_pMacroManager = this;


	m_bstrName = m_bstrUserName = MACROMAN_NAME;

	m_brecording = m_bexecuting = m_bstart_record = FALSE;
	m_str_script = _T("");

	m_nold_mousespeed = -1;

	m_nlast_btndown = -1;
	m_bwait_end_dblclick = false;

	m_bsnapshot_running = false;

	if( !hook )
		hook = SetWindowsHookEx( WH_GETMESSAGE, (HOOKPROC)msghook, 0, ::GetCurrentThreadId() );

	// register
	Register( GetAppUnknown(), true, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////
CMacroManager::~CMacroManager(void)
{
	m_str_script.Empty( );

	if( hook  )
	{
		UnhookWindowsHookEx( hook );
		hook  = 0;
	}

	if( s_pMacroManager  )
		s_pMacroManager = 0;

	// unregister
	Register( GetAppUnknown(), false, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////
IUnknown* CMacroManager::DoGetInterface(const IID &iid )
{
	if( iid == __uuidof( IMacroManager ) )
		return (IMacroManager *)(this);
	else if( iid == __uuidof( IDispatch ) )
		return (IDispatch *)(this);
	else if( iid == __uuidof( INamedObject2 ) )
		return (INamedObject2 *)(this);
	else if( iid == __uuidof( IEventListener ) )
		return (IEventListener *)(this);

	return ComObjectBase::DoGetInterface( iid );
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CMacroManager::OnTimer( )
{
	if( m_bsnapshot_running )
		return;

	IApplicationPtr	sptrApp( GetAppUnknown() );
	IUnknown	*punkAM = 0;
	sptrApp->GetActionManager( &punkAM );
	sptrApp = 0;

	IActionManagerPtr	sptrAM( punkAM );
	if( punkAM )
	{
		punkAM->Release();
		punkAM = 0;
	}
	if( sptrAM )
	{
		DWORD dwFlags = 0;
		sptrAM->GetCurrentExecuteFlags( &dwFlags );
		if( dwFlags & aefInteractiveRunning )
			return;
	}

	::ExecuteScript( _bstr_t( _T("Snapshot 2") ), "CMacroManager::OnTimer" );
}

void CMacroManager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	bool bbefore_exec = false;
	if( (bbefore_exec = !strcmp( pszEvent, szEventBeforeActionExecute )) || 
		!strcmp(pszEvent, szEventAfterActionExecute) )
	{
		_bstr_t bstr_name;
		IActionPtr	ptrA( punkHit );
		IUnknown	*punkAI = 0;

		ptrA->GetActionInfo( &punkAI );
		IActionInfoPtr	ptrAI( punkAI );
		if( punkAI )
			punkAI->Release(); punkAI = 0;

		ptrAI->GetCommandInfo( bstr_name.GetAddress(), 0, 0, 0 );
        
		if( bstr_name == _bstr_t( _T("Snapshot") ) )
			m_bsnapshot_running = bbefore_exec;            
	}
}

void CMacroManager::_init_for_rec(void)
{
    m_str_script.Empty( );
	m_nlast_btndown = -1;
	m_bwait_end_dblclick = false;
	m_dwkeys_states = 0;        
}

// IMacroManager Interface Implementation
///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::ProcessMessageHandler( /*[in]*/ MSG *pMsg )
{
	// [vanek]: use Ctrl+Break - 09.04.2004
	if( g_ballow_exit_now || (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) &&
		(pMsg->wParam == VK_CANCEL) && (::GetAsyncKeyState( VK_CONTROL ) & 0x8000) )
	{
		if( m_brecording  )
			// stop record
			StopRecord( );
		else
		{	
			// stop play
			IMacroHelper *pmh = 0;
			m_helper.GetMacroHelper( &pmh );
			if( pmh  )
			{
				pmh->KillAllEvents();
				pmh->Release( );
				pmh = 0;
			}

			bool bfull_terminate = true;
			// [vanek] BT2000: 3932 terminate testing  - 18.08.2004
			if( g_ballow_exit_now || (::GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) )
				 bfull_terminate = false;

			::FireEvent( GetAppUnknown(), szEventTerminateTestProcess, Unknown(), 0, &bfull_terminate, sizeof( bfull_terminate ) );
		}
		g_ballow_exit_now = false;

		return S_OK;
	}            			


	if( m_brecording )
	{
		bool bskip_message = m_bwait_end_dblclick;

		switch( pMsg->message )
		{		
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
			if( m_dwkeys_states & VK_LBUTTON )
				bskip_message |= true;
			else
				m_dwkeys_states |= VK_LBUTTON;
			break;

		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
			m_dwkeys_states &= ~VK_LBUTTON;
			break;

		case WM_RBUTTONDOWN:		
		case WM_NCRBUTTONDOWN:
			if( m_dwkeys_states & VK_RBUTTON )
				bskip_message |= true;
			else
				m_dwkeys_states |= VK_RBUTTON;
			break;

		case WM_RBUTTONUP:				
		case WM_NCRBUTTONUP:
			m_dwkeys_states &= ~VK_RBUTTON;
			break;
		}

		if( !bskip_message )
		{
			BSTR bstr_script = 0;

            IMacroHelper *pmh = 0;
			m_helper.GetMacroHelper( &pmh );
			if( pmh  )
			{
				pmh->ConvertMessageToMacro( pMsg, m_bstart_record, &bstr_script );
				pmh->Release( );
				pmh = 0;
			}

			if( SysStringLen( bstr_script ) > 0 )
			{

				// [vanek] BT2000:4010 - 04.10.2004
				if( !m_str_script.IsEmpty() )
				{
					switch( pMsg->message )
					{		
					case WM_LBUTTONDOWN:
					case WM_RBUTTONDOWN:
					case WM_NCLBUTTONDOWN:
					case WM_NCRBUTTONDOWN:
						m_nlast_btndown = m_str_script.GetLength( );
						break;

					case WM_NCLBUTTONDBLCLK:
					case WM_LBUTTONDBLCLK:
					case WM_NCRBUTTONDBLCLK:
					case WM_RBUTTONDBLCLK:
						if( m_nlast_btndown != -1 )
						{
							// correction script
							m_str_script.Delete( m_nlast_btndown, m_str_script.GetLength() - m_nlast_btndown );
							m_nlast_btndown = -1;
							m_bwait_end_dblclick = true;
						}
						break;
					}

					m_str_script += _T( NEW_LINE );	
				}
				else
					m_nlast_btndown = -1;

				m_str_script += bstr_script;			
			} 

			if( bstr_script )
				::SysFreeString( bstr_script ); bstr_script = 0;

			if( m_bstart_record )
				m_bstart_record = FALSE;

			// [vanek] BT2000:4010 - 04.10.2004
			if( m_nold_mousespeed != -1)
			{
				switch( pMsg->message )
				{
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_NCLBUTTONDOWN:
				case WM_NCRBUTTONDOWN:
					::SystemParametersInfo( SPI_SETMOUSESPEED, 0, (void *)(4), 0 );
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_NCLBUTTONUP:
				case WM_NCRBUTTONUP:
					::SystemParametersInfo( SPI_SETMOUSESPEED, 0, (void *)(m_nold_mousespeed), 0 );
					break;
				}
			} 			
		}
	}

	// [vanek] BT2000:4010 check end of double click - 04.10.2004
	switch( pMsg->message )
	{		
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
		m_bwait_end_dblclick = false;
		break;
	}

	if( (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) && pMsg->wParam == VK_SCROLL && !(pMsg->lParam & 0x40000000) )
		::SetTimer( 0, 0, 1, MacroManagerTimerProc );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::IsRecording( /*[out]*/ BOOL *pbRecording )
{
	if( !pbRecording )
		return E_INVALIDARG;

	*pbRecording = m_brecording;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::IsExecuting( /*[out]*/ BOOL *pbExecuting )
{
	if( !pbExecuting )
		return E_INVALIDARG;

	*pbExecuting = m_bexecuting;
	return S_OK;
}

// IMacroManagerDisp Interface Implementation
///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::StartRecord( )
{
	if( m_brecording )
		return S_OK;

	_init_for_rec( );
	return ResumeRecord( );
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::ResumeRecord( )
{
	if( m_brecording )
		return S_OK;

	// [vanek] : save current mouse speed - 22.07.2004
	::SystemParametersInfo( SPI_GETMOUSESPEED, 0, &m_nold_mousespeed, 0 );

	m_brecording = TRUE;
	m_bstart_record = TRUE;	// set to FALSE in ProcessMessageHandler
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::StopRecord( )
{
	if( m_brecording )
	{
		CHourglass wait;

		m_brecording = FALSE;

		// [vanek] : restore saved mouse speed - 22.07.2004
		if( m_nold_mousespeed != -1)
		{
			::SystemParametersInfo( SPI_SETMOUSESPEED, 0, (void *)(m_nold_mousespeed), 0 );
			m_nold_mousespeed = -1;						  
		}

		if( !m_str_script.IsEmpty() )
			m_str_script += _T( "\r\nMacroHelper.WaitMacroEnd" );
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::SetMacroScript( /*[in]*/ BSTR bstrScript )
{
	if( !bstrScript )
		return E_INVALIDARG;

	m_str_script = bstrScript;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::GetMacroScript( /*[out]*/ BSTR *pbstrScript )
{
	if( !pbstrScript )
		return E_INVALIDARG;

	*pbstrScript = m_str_script.AllocSysString( );
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroManager::ExecuteScript( )
{
	if( m_brecording || m_str_script.IsEmpty( ) )
		return S_OK;

	CLockerFlag locker( &m_bexecuting );
	HRESULT hr = ::ExecuteScript( m_str_script.AllocSysString(), "CMacroManager::ExecuteScript" ) ? S_OK : E_FAIL;
	return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CCachedMacroHelper
//
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////
CCachedMacroHelper::CCachedMacroHelper(void)
{
	m_phelper = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CCachedMacroHelper::~CCachedMacroHelper(void)
{
	if( m_phelper )
		m_phelper->Release( ); m_phelper  = 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCachedMacroHelper::GetMacroHelper( IMacroHelper **ppmh )
{
	if( !ppmh )
		return FALSE;

	if( !m_phelper )
	{
		IUnknown *punkHelper = 0;
		punkHelper  = _GetOtherComponent( GetAppUnknown(), IID_IMacroHelper );
		if(  punkHelper  )
		{
			punkHelper->QueryInterface( IID_IMacroHelper, (void**)&m_phelper );
			punkHelper->Release(); punkHelper = 0;
		}
	}

	*ppmh = 0;
	if(	m_phelper )
	{
		*ppmh = m_phelper;
		(*ppmh)->AddRef( );
	}

	return (*ppmh) ? TRUE : FALSE;
}