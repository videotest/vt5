#include "stdafx.h"
#include <Mmsystem.h>
//#include <winable.h>
#include "MacroHelper.h"
#include "misc_classes.h"
#include "misc_utils.h"
#include "\vt5\com_base\com_defs.h"
#include "core5.h"
#include "action5.h"
#include "macro_utils.h"
#include "\vt5\awin\misc_dbg.h" 

#pragma comment(lib, "Winmm.lib")

#define MACROHELPER_NAME_SECT	"\\" MACROHELPER_NAME

extern bool g_ballow_exit_now;

DWORD WINAPI MacroHelperThreadProc( LPVOID lpParameter )
{
	CMacroHelper *pmacro_helper = (CMacroHelper *)(lpParameter);
	if( !pmacro_helper )
		return 0;

	while( !pmacro_helper->m_bstop_thread )
	{
		Sleep( 5 ); // отдохнем чуть-чуть( даем возможность принудительно остановить выполнение текущей последовательности - KillAllEvents )
		
		if( !pmacro_helper->handle( ) )
			continue;
					
		if( WAIT_OBJECT_0 == ::WaitForSingleObject( pmacro_helper->m_hdo_synth, 1 ) )
		{
            BOOL bprotected = FALSE;
			bprotected = (WAIT_OBJECT_0 == ::WaitForSingleObject( pmacro_helper->m_hprotect_mode, 0 ));
			if( !bprotected )
			{
				if( pmacro_helper->m_lposted_count < 500 )
				{
					// все OK - можно постить
					if( ::PostMessage( pmacro_helper->handle( ), (WM_USER + 777), 0, 0 ) )
						::InterlockedIncrement( &pmacro_helper->m_lposted_count );
				}
				else
				{
					// переходим в "защищенный" режим
					::SetEvent( pmacro_helper->m_hprotect_mode );
					
					// сброс счетчика сообщений
					::InterlockedExchangePointer( (void **)(&pmacro_helper->m_lposted_count), (void *)(0) );	                    
				}
			}
			else	
				// "защищенный" режим - синтезируем следующиее событие
				pmacro_helper->synth_events( true );
		}		
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////
CMacroHelper::CMacroHelper(void) : IDispatchImpl< IMacroHelperDisp >( "MacroRecorder.tlb" )
{
	m_bstrName = m_bstrUserName = MACROHELPER_NAME;

	m_bfiring_event = m_bneed_save_conditions = FALSE;
	m_stcurr_cond.init( );

	m_dwLastProcessedMessageTime = 0;
	m_bskipped_previous_msg = FALSE;
	m_dwFirstSkippedMessageDelay = 0;

	m_ltimeout = GetValueInt( GetAppUnknown( ), MACROHELPER_NAME_SECT, "AttemptsTimeout", 1000 );
	m_fdelay_factor = GetValueDouble( GetAppUnknown( ), MACROHELPER_NAME_SECT, "DelayFactor", 1.0 );
	m_lmin_slowing_delay = GetValueInt( GetAppUnknown( ), MACROHELPER_NAME_SECT, "MinSlowingDelay", 20 );
	m_bplay_sound = GetValueInt( GetAppUnknown( ), MACROHELPER_NAME_SECT, "PlaySound", 1 ) == 1;

	m_ballow_terminate = false;

	init_values_for_synth( );

	m_bDoNotAddToScript = FALSE;
	m_bManualCorr = FALSE;

	::InitializeCriticalSection( &m_crs_events );

	m_hthread = 0;
	m_dwthread_id = 0;

	m_hdo_synth = 0;
	m_hdo_synth = ::CreateEvent( NULL, TRUE, FALSE, NULL ); 

	m_hprotect_mode = 0;
    m_hprotect_mode = ::CreateEvent( NULL, /*FALSE*/ TRUE, FALSE, NULL ); 

	m_bstop_thread = FALSE;

	m_lposted_count = 0;

	// create window
	RECT rc_window = {0};
	create( 0, rc_window );

	// create thread
	if( !m_hthread )
		m_hthread = ::CreateThread( 0, 0, MacroHelperThreadProc, this, 0, &m_dwthread_id );
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
CMacroHelper::~CMacroHelper(void)
{
	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.clear( );
	::LeaveCriticalSection( &m_crs_events );

	// waiting end of thread
    if( m_hthread )
	{
		InterlockedExchangePointer( (void **)(&m_bstop_thread), (void *)(TRUE) );	
		::SetEvent( m_hdo_synth );
		::WaitForSingleObject( m_hthread, INFINITE );
		::CloseHandle( m_hthread );		
		InterlockedExchangePointer( (void **)(&m_bstop_thread), (void *)(FALSE) );	

		m_hthread = 0;
		m_dwthread_id = 0;
	}
	::CloseHandle( m_hdo_synth );
	m_hdo_synth = 0;

	// destroy window
	if( handle() )
		::DestroyWindow( handle() );

	::DeleteCriticalSection( &m_crs_events );
}


///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::init_values_for_synth( )
{
	m_dwStartFiring = (DWORD)(-1);
	m_bkill_all_events = FALSE;    
	m_lScriptRunningAfterStart = -1;
	m_bslowing_delay = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
IUnknown *CMacroHelper::DoGetInterface( const IID &iid )
{
	if( iid == IID_IDispatch )
		return (IDispatch*)(this);
	else if( iid == __uuidof( INamedObject2 ) )
		return (INamedObject2 *)(this);
	else if( iid == __uuidof( IMacroHelper ) )
		return (IMacroHelper *)(this);

	return ComObjectBase::DoGetInterface( iid );
}


// IMacroHelperDisp Implementation

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::SetEventsWindow( /*[in]*/ BSTR bstrWndClass )
{
	m_bDoNotAddToScript = TRUE;
	BOOL b = SetEventsWindow2(bstrWndClass, 0, 0, 0, 0, 0);
	m_bDoNotAddToScript = FALSE;
	return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::MouseMove( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;
	pmouse_synth->SetMessage( WM_MOUSEMOVE );

	// [vanek] : притормаживаем - 25.03.2004
	if( m_bslowing_delay )
		dwDelay = max( dwDelay, (DWORD)(m_lmin_slowing_delay) );

	pmouse_synth->SetDelay( dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) );
	::LeaveCriticalSection( &m_crs_events );

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::LButtonDown( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	// [vanek] BT2000:4010 не надо имиитировать нажатие кнопок мыши на клавиатуре! - 04.10.2004
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;

	pmouse_synth->SetMessage( WM_LBUTTONDOWN );
	pmouse_synth->SetDelay( /*0*/dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) );        
	::LeaveCriticalSection( &m_crs_events );

	m_bslowing_delay = TRUE;

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::LButtonUp( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	// [vanek] BT2000:4010 не надо имиитировать нажатие кнопок мыши на клавиатуре! - 04.10.2004
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;

	pmouse_synth->SetMessage( WM_LBUTTONUP );
	pmouse_synth->SetDelay( /*0*/dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) );        
	::LeaveCriticalSection( &m_crs_events );

	m_bslowing_delay = FALSE;

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::LButtonDblClk( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	m_bDoNotAddToScript = TRUE;
	
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;
	pmouse_synth->SetMessage( WM_LBUTTONDBLCLK );
	pmouse_synth->SetDelay( dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) );        
	::LeaveCriticalSection( &m_crs_events );

	m_bDoNotAddToScript = FALSE;

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::RButtonDown( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	// [vanek] BT2000:4010 не надо имиитировать нажатие кнопок мыши на клавиатуре! - 04.10.2004
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;

	pmouse_synth->SetMessage( WM_RBUTTONDOWN );
	pmouse_synth->SetDelay( /*0*/dwDelay );
	pmouse_synth->SetPoint( nX, nY );  

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) ); 
	::LeaveCriticalSection( &m_crs_events );

	m_bslowing_delay = TRUE;

	// начинаем генерирование сообщений
	start_synth( );
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::RButtonUp( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	// [vanek] BT2000:4010 не надо имиитировать нажатие кнопок мыши на клавиатуре! - 04.10.2004
	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;

	pmouse_synth->SetMessage( WM_RBUTTONUP );
	pmouse_synth->SetDelay( /*0*/dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) ); 
	::LeaveCriticalSection( &m_crs_events );

	m_bslowing_delay = FALSE;

	// начинаем генерирование сообщений
	start_synth( );
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::RButtonDblClk( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	CMouseSynthesizer *pmouse_synth = new CMouseSynthesizer;
	pmouse_synth->SetMessage( WM_RBUTTONDBLCLK );
	pmouse_synth->SetDelay( dwDelay );
	pmouse_synth->SetPoint( nX, nY );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pmouse_synth) );        
	::LeaveCriticalSection( &m_crs_events );

	m_bDoNotAddToScript = FALSE;

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::KeyDown( /*[in]*/ long lKeyCode, /*[in]*/ long lIsExtendedKey, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	CKeybdSynthesizer *pkbd_synth = new CKeybdSynthesizer;
	pkbd_synth->SetMessage( WM_KEYDOWN );
	pkbd_synth->SetDelay( dwDelay );
	pkbd_synth->SetKey( lKeyCode, lIsExtendedKey == 1 );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pkbd_synth) );
	::LeaveCriticalSection( &m_crs_events );

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::KeyUp( /*[in]*/ long lKeyCode, /*[in]*/ long lIsExtendedKey, /*[in]*/ DWORD dwDelay )
{
	dwDelay = (DWORD)(dwDelay * m_fdelay_factor + 0.5);

	CKeybdSynthesizer *pkbd_synth = new CKeybdSynthesizer;
	pkbd_synth->SetMessage( WM_KEYUP );
	pkbd_synth->SetDelay( dwDelay );
	pkbd_synth->SetKey( lKeyCode, lIsExtendedKey == 1 );

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pkbd_synth) );
	::LeaveCriticalSection( &m_crs_events );

	// начинаем генерирование сообщений
	start_synth( );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::WaitMacroEnd( )
{
	while( 1 )
	{
		bool bempty = false;

		::EnterCriticalSection( &m_crs_events );
		bempty = !m_list_pevents.count();
		::LeaveCriticalSection( &m_crs_events );

		if( bempty ) // нет сообщений в списке -> выходим
			break;

		IApplicationPtr	sptr_app( GetAppUnknown( ) );
		if( sptr_app == 0 )
			break;

		sptr_app->ProcessMessage();
	}


	if( m_lScriptRunningAfterStart != -1 )
	{	// восстанавливаем флаг "запуска из скрипта"
		IApplicationPtr sptr_app;
		sptr_app = GetAppUnknown();
		if( sptr_app != 0 )
		{
			IUnknownPtr	sptr_unk = 0;
			sptr_app->GetActionManager( &sptr_unk );
			IActionManagerPtr sptr_am = sptr_unk;
			if( sptr_am != 0 )
				sptr_am->SetScriptRunningFlag( m_lScriptRunningAfterStart == 1 );                
		}

		// сброс
		m_lScriptRunningAfterStart = -1;
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::SetScriptRunning( /*[in]*/ long lSet )
{
	IApplicationPtr sptr_app;
	sptr_app = GetAppUnknown();
	if( sptr_app == 0 )
		return E_FAIL;

	IUnknownPtr	sptr_unk = 0;
	sptr_app->GetActionManager( &sptr_unk );
	IActionManagerPtr sptr_am = sptr_unk;
	if( sptr_am == 0 )
		return E_FAIL;

	if( m_lScriptRunningAfterStart == -1 )    	
		// сохраняем текущее состояние -> восстанавливаем в конце WaitMacroEnd
		m_lScriptRunningAfterStart = sptr_am->IsScriptRunning( ) ? 1 : 0;

	sptr_am->SetScriptRunningFlag( lSet == 1 );
	return S_OK;
}

HRESULT CMacroHelper::SetEventsWindow2( /*[in]*/ BSTR bstrWndClass, /*[in]*/ int nX,
									   /*[in]*/ int nY, /*[in]*/ int nCX, /*[in]*/ int nCY, /*[in]*/ long lHwnd )
{
	if( !bstrWndClass )
		return E_INVALIDARG;

	CConditionsSynthesizer *pcond_synth = new CConditionsSynthesizer;
	WorkConditions st_wc;
	st_wc.init( );
	st_wc.strWndClass = bstrWndClass;
	st_wc.rcWnd = _rect(nX, nY, nX+nCX, nY+nCY);
	st_wc.hWndSaved = (HWND)lHwnd;
	st_wc.bGoodData = TRUE;

	if (m_bManualCorr && lHwnd)
	{
		while( 1 )
		{
			bool bempty = false;

			::EnterCriticalSection( &m_crs_events );
			bempty = !m_list_pevents.count();
			::LeaveCriticalSection( &m_crs_events );

			if( bempty )
				break;

			IApplicationPtr	sptr_app( GetAppUnknown( ) );
			if( sptr_app == 0 )
				break;
			sptr_app->ProcessMessage();
		}

		CEditedRectangleInfo RInfo;
		RInfo.rcOld = st_wc.rcWnd;
		RInfo.rcNew = st_wc.rcWnd;
		RInfo.hwndSaved = st_wc.hWndSaved;
		RInfo.bstrWndClass = st_wc.strWndClass.AllocSysString();
		if (FindEditedRectangle(&RInfo) == S_OK)
			memcpy(&st_wc.rcWnd,&RInfo.rcNew,sizeof(RECT));
		else
		{
			if (DoEditRectangle(RInfo))
			{
				AddChangedRect(RInfo);
				memcpy(&st_wc.rcWnd,&RInfo.rcNew,sizeof(RECT));
			}
		}
		::SysFreeString(RInfo.bstrWndClass);
	}

	pcond_synth->SetConditions( st_wc );
	pcond_synth->SetHelper( this );
	//	pcond_synth->SetScriptGenerator(pCmdScriptGenerator);

	::EnterCriticalSection( &m_crs_events );
	m_list_pevents.add_tail( (CEventSynthesizer*)(pcond_synth) );
	::LeaveCriticalSection( &m_crs_events );

	// запускаем генерирование сообщений
	start_synth( );

	return S_OK;
}


// IMacroHelper Implementation

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::ConvertMessageToMacro( /*[in]*/ MSG *pMsg, /*[in]*/ BOOL bStartRecord, /*[out]*/ BSTR *pbstrScript )
{
	if( !pMsg || !pbstrScript )
		return E_INVALIDARG;

	DWORD dwCurrTime = __GetTickCount( );
	CString str_script( _T("") );
	if( bStartRecord )
	{
		// если началась новая запись, надо обязательно на первом сообщении мыши запомнить(установить) текущее состояние
		m_bneed_save_conditions = TRUE;
		m_dwLastProcessedMessageTime = m_dwFirstSkippedMessageDelay = 0;
	}

	DWORD dwDelay = 0;
	if( dwCurrTime >= m_dwLastProcessedMessageTime )
		dwDelay = m_bskipped_previous_msg ? m_dwFirstSkippedMessageDelay : ( m_dwLastProcessedMessageTime > 0 ? dwCurrTime - m_dwLastProcessedMessageTime : 0 );
	else
	{
		dwDelay = 0;
		assert( 0 );
	}

	BOOL bSkippedMessage = FALSE;
	if( process_msg( pMsg, &str_script, dwDelay, &bSkippedMessage ) )
	{
		if( !bSkippedMessage )									// сообщение обработано
		{
			m_dwLastProcessedMessageTime = dwCurrTime;
			m_dwFirstSkippedMessageDelay = 0;
		}
		/*else if( !m_bskipped_previous_msg ) 
		// первое необработанное сообщение
		m_dwFirstSkippedMessageDelay = dwDelay;

		m_bskipped_previous_msg = bSkippedMessage;*/
	}

	*pbstrScript = str_script.AllocSysString( );
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMacroHelper::KillAllEvents( )
{
	m_bkill_all_events = TRUE; // process in synth_events() function
	return S_OK;
}

HRESULT CMacroHelper::GetManualCorrection(BOOL *pbManualCorr)
{
	if (pbManualCorr)
		*pbManualCorr = m_bManualCorr;
	return S_OK;
}

HRESULT CMacroHelper::SetManualCorrection(BOOL bManualCorr)
{
	m_bManualCorr = bManualCorr;
	return S_OK;
}

HRESULT CMacroHelper::ClearListOfExecuted()
{
	m_edited_rectangles.clear();
	return S_OK;
}

bool _same(RECT r1, RECT r2)
{
	return r1.left == r2.left && r1.right == r2.right && r1.top == r2.top && r1.bottom == r2.bottom;
}

HRESULT CMacroHelper::FindEditedRectangle(CEditedRectangleInfo *pInfo)
{
	TPOS lPos = m_edited_rectangles.head();
	while (lPos != 0)
	{
		CEditedRectangleInfo *p = (CEditedRectangleInfo *)m_edited_rectangles.get(lPos);
		if (_same(pInfo->rcOld,p->rcOld) &&	p->hwndSaved == pInfo->hwndSaved &&
			CompareClassNames(_bstr_t(pInfo->bstrWndClass),_bstr_t(p->bstrWndClass)))
		{
			pInfo->rcNew = p->rcNew;
			return S_OK;
		}
		lPos = m_edited_rectangles.next(lPos);
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::is_mouse_msg( UINT message )
{
	return (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) || 
		(message >= WM_NCMOUSEFIRST && message <= WM_NCMOUSELAST);	// [vanek] SBT: 790 - 27.02.2004
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::is_keybrd_msg( UINT message )
{
	return (message >= WM_KEYFIRST) && (message <= WM_KEYLAST);
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::process_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *pbSkipped )
{
	if( !pMsg || !pstrScript || !pbSkipped )
		return FALSE;

	if( is_mouse_msg( pMsg->message ) )
		return process_mouse_msg( pMsg, pstrScript, dwDelay, pbSkipped );
	else if( is_keybrd_msg( pMsg->message) )
		return process_keybrd_msg( pMsg, pstrScript, dwDelay, pbSkipped );
	else 
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::process_mouse_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *pbSkipped )
{
	if( !pMsg || !pstrScript || !pbSkipped )
		return FALSE;

	CString str_method( _T("") );
	// [vanek] SBT: 790 - обработка сообщений, приходящих в неклиенскую область окна 27.02.2004
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:	
	case WM_NCMOUSEMOVE:
		// учитываем только передвижение мыши c зажатыми клавишами и предполагаем, что обработка нажатия клавиш была 
		// проведена ранее
		// if( pMsg->wParam ) теперь учитываем все :)) 
		str_method = _T("MouseMove");
		break;

	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
		str_method = _T("LButtonDown");		
		break;

	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
		str_method = _T("LButtonUp");		
		break;

	case WM_LBUTTONDBLCLK:
	case WM_NCLBUTTONDBLCLK:
		str_method = _T("LButtonDblClk");		
		break;

	case WM_RBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
		str_method = _T("RButtonDown");		
		break;

	case WM_RBUTTONUP:
	case WM_NCRBUTTONUP:
		str_method = _T("RButtonUp");		
		break;

	case WM_RBUTTONDBLCLK:
	case WM_NCRBUTTONDBLCLK:
		str_method = _T("RButtonDblClk");		
		break;

	default:
		str_method = _T("");
	}

	if( !str_method.IsEmpty( ) )
	{
		_point pt(0,0);
		// [vanek] SBT: 803, 801
		pt = pMsg->pt;

		CString str_line( _T("") );
		BOOL bupdated = update_conditions( FALSE, pt.x, pt.y );
		if( m_bneed_save_conditions || bupdated )
		{
			if( m_bneed_save_conditions ) 
			{
				on_after_first_msg( pstrScript );
				m_bneed_save_conditions = FALSE;
			}

			str_line.Format( _T("%s.%s \"%s\",%d,%d,%d,%d,%d%s"), _T( MACROHELPER_NAME ), _T( "SetEventsWindow2" ),
				m_stcurr_cond.strWndClass, m_stcurr_cond.rcWnd.left, m_stcurr_cond.rcWnd.top,
				m_stcurr_cond.rcWnd.width(), m_stcurr_cond.rcWnd.height(), (int)m_stcurr_cond.hWndSaved,
				_T(NEW_LINE) );
			*pstrScript += str_line;
		}

		str_line.Format( _T("%s.%s %d, %d, %u"), _T( MACROHELPER_NAME ), str_method,
			pt.x-m_stcurr_cond.rcWnd.left, pt.y-m_stcurr_cond.rcWnd.top, dwDelay);
		*pstrScript += str_line;

		*pbSkipped = FALSE;
	}
	else
		*pbSkipped = TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::process_keybrd_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *pbSkipped )
{
	if( !pMsg || !pstrScript || !pbSkipped )
		return FALSE;

	CString str_line( _T("") ), str_method( _T("") );
	// [vanek] SBT: 818 - 24.02.2004
	int nextended_key = 0;

	switch( pMsg->message )
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		str_method = _T("KeyDown");                       
		nextended_key = pMsg->lParam & 0x1000000 ? 1 : 0;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		str_method = _T("KeyUp");
		nextended_key = pMsg->lParam & 0x1000000 ? 1 : 0;
		break;

	default:
		str_method = _T("");
	}

	if( !str_method.IsEmpty( ) )
	{
		BOOL bupdated = update_conditions( TRUE, 0, 0);
		if( m_bneed_save_conditions || bupdated )
		{
			if( m_bneed_save_conditions ) 
			{
				on_after_first_msg( pstrScript );
				m_bneed_save_conditions = FALSE;
			}

			str_line.Format( _T("%s.%s \"%s\",%d,%d,%d,%d,%d%s"), _T( MACROHELPER_NAME ), _T( "SetEventsWindow2" ),
				m_stcurr_cond.strWndClass, m_stcurr_cond.rcWnd.left, m_stcurr_cond.rcWnd.top,
				m_stcurr_cond.rcWnd.width(), m_stcurr_cond.rcWnd.height(), (int)m_stcurr_cond.hWndSaved,
				_T(NEW_LINE) );
			*pstrScript += str_line;

		}

		str_line.Format( _T("%s.%s %d, %d, %u"), _T( MACROHELPER_NAME ), str_method, pMsg->wParam, nextended_key, dwDelay );
		*pstrScript += str_line;

		*pbSkipped = FALSE;
	}
	else
		*pbSkipped = TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::update_conditions( BOOL bUseForegroundWindow, int nX, int nY )
{
	WorkConditions st_wc;
	if( bUseForegroundWindow )
	{	// сообщение от клавиатуры
		HWND hwnd = ::GetForegroundWindow( );
		st_wc.from_window( hwnd );
	}
	else
	{	// мышиное сообщение
		POINT pt = { nX, nY};
		st_wc.from_point( pt );
	}

	if( m_stcurr_cond != st_wc )	
	{
		m_stcurr_cond = st_wc;
		return TRUE;
	}
	else 
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool	CMacroHelper::have_events( )
{
	bool bhave = false;

	::EnterCriticalSection( &m_crs_events );
	bhave = (0 != m_list_pevents.count( ) );
	::LeaveCriticalSection( &m_crs_events );

	return bhave;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CMacroHelper::synth_events( bool bin_protected_mode /*= false*/ )
{
	::EnterCriticalSection( &m_crs_events );

	m_ballow_terminate = GetValueInt( GetAppUnknown( ), MACROHELPER_NAME_SECT, "AllowTerminate", 0 ) != 0;

	while( m_list_pevents.count() ) // пытаемся синтезировать все события...
	{
		// если надо завершить - стираем всё
		if( m_bkill_all_events )
			m_list_pevents.clear();

		TPOS lhead_pos = 0;
		lhead_pos = m_list_pevents.head( );
		if( !lhead_pos )
			break;	

		CEventSynthesizer *pevent_synth = 0;
		pevent_synth = m_list_pevents.get( lhead_pos );
		if( !pevent_synth )
			break;
			
        if( !is_synthesizing( ) )
		{	// start firing
			m_dwStartFiring = __GetTickCount();
			on_start_firing( );
		}

		if(  handle() && ( ::GetWindowThreadProcessId( handle(), 0 ) == ::GetCurrentThreadId() ) )
		{
			if( HIWORD(::GetQueueStatus( QS_MOUSE )) )	
				break;	// не генерируем события, пока не обработается текущий "мышиный" ввод;
						// только "мышиный", т.к. клавиши нужно отжимать вовремя, а то начинаются проблемы с 
						// кнопками по-умолчанию на формах и т.д.  [vanek] 06.08.2004
			
		}
		
		int nres = 0;
		nres = pevent_synth->CanFireEvent( &m_stcurr_cond, m_dwStartFiring, m_ltimeout );
		if( !nres )
			break;

		//[max]
		if( nres == 2 && m_ballow_terminate )
			g_ballow_exit_now = true;
		
        // firing...
		UINT uidelay = pevent_synth->GetDelay();
		pevent_synth->FireEvent( &m_stcurr_cond, m_bplay_sound );
		m_list_pevents.remove( lhead_pos );
		if( m_list_pevents.count() )
			m_dwStartFiring = (nres == 1) ? m_dwStartFiring + uidelay : __GetTickCount();

		if( bin_protected_mode )
			break;
	}


	// синтезируем и последовательность пуста - окончание синтезиррования событий
	if( is_synthesizing( ) && !m_list_pevents.count( ) )
	{
		stop_synth( );
		on_end_firing( );	
	}

	::LeaveCriticalSection( &m_crs_events );
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::SetCurrCond( WorkConditions st_wc )
{
	if( st_wc.bGoodData )
		m_stcurr_cond =	st_wc;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CMacroHelper::GetCurrCond( WorkConditions *pst_wc )
{
	if( !pst_wc )
		return FALSE;

	pst_wc->init();
	if( m_stcurr_cond.bGoodData )
	{
		*pst_wc = m_stcurr_cond;
		return TRUE;
	}
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::AddChangedRect(CEditedRectangleInfo &RInfo)
{
	CEditedRectangleInfo *p = new CEditedRectangleInfo;
	memset(p, 0, sizeof(*p));
	p->rcOld = RInfo.rcOld;
	p->rcNew = RInfo.rcNew;
	p->hwndSaved = RInfo.hwndSaved;
	p->bstrWndClass = _bstr_t(RInfo.bstrWndClass).copy();
	if (FindEditedRectangle(p) == S_OK)
	{
		::SysFreeString(p->bstrWndClass);
		delete p;
		return;
	}
	m_edited_rectangles.add_tail(p);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::on_after_first_msg( CString *pstr_script )
{
	if( !pstr_script )
		return;

	IApplicationPtr sptr_app;
	sptr_app = GetAppUnknown();
	if( sptr_app != 0 )
	{
		IUnknownPtr	sptr_unk = 0;
		sptr_app->GetActionManager( &sptr_unk );
		IActionManagerPtr sptr_am = sptr_unk;
		if( sptr_am != 0 )
		{
			int nfromscript = S_OK == sptr_am->IsScriptRunning( ) ? 1 : 0;
			CString str_line( _T("") );
			str_line.Format( _T("%s.%s %d%s"), _T( MACROHELPER_NAME ), _T( "SetScriptRunning" ), nfromscript, _T(NEW_LINE));	
			*pstr_script += str_line;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::on_start_firing(void)
{
#ifndef _DEBUG
	::BlockInput( TRUE );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::on_end_firing(void)
{
#ifndef _DEBUG
	::BlockInput( FALSE );
#endif

	// reset states for all keys
	for( int i = 1; i <= 254; i++ )
	{
		if( GetAsyncKeyState( i ) & 0x8000 )
		{		
			DWORD dwFlags = KEYEVENTF_KEYUP;  
			BYTE byteScanCode = 0;					
			switch( i )
			{
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_PRIOR:
			case VK_NEXT:
			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
			case VK_NUMLOCK:
			case VK_SNAPSHOT:
			case VK_LCONTROL:
			case VK_RCONTROL:
			case VK_LSHIFT:
			case VK_RSHIFT:
			case VK_LMENU:
			case VK_RMENU:
				dwFlags |= KEYEVENTF_EXTENDEDKEY;
				byteScanCode = ::MapVirtualKey( i, 0);
				break;
			}

			::keybd_event( (BYTE)(i), byteScanCode, dwFlags, 0 );
		}
	}

	init_values_for_synth( );
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::start_synth(void)
{
	if( m_hdo_synth )
		::SetEvent( m_hdo_synth );
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CMacroHelper::stop_synth(void)
{
	if( m_hdo_synth )
		::ResetEvent( m_hdo_synth );
}

// override
///////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMacroHelper::handle_message(UINT m, WPARAM w, LPARAM l)
{
	if( WAIT_OBJECT_0 == ::WaitForSingleObject( m_hprotect_mode, 0 ) )
	{	// "защищенный" режим
        
		// 1 - чистим очередь сообщений
		MSG msg = {0};
		while( ::PeekMessage( &msg, handle(), (WM_USER + 777), (WM_USER + 777), PM_REMOVE ) );

		// 2 - сброс счетчика сообщений
		::InterlockedExchangePointer( (void **)(&m_lposted_count), (void *)(0) );	                    

		// 3 - выход из защищенного режима 
		::ResetEvent( m_hprotect_mode );		

		if( m == (WM_USER + 777) )
			return 1;
	}

	switch( m )
	{
//		case (WM_USER + 666):	// тестовое сообщение
//			  return 3;

		case (WM_USER + 777):
			::InterlockedDecrement( &m_lposted_count );
			synth_events( );
			return 1;
	}

	return __super::handle_message( m, w, l );
}