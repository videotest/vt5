#include "stdafx.h"
#include "resource.h"
#include "methodpanel.h"
#include "MenuRegistrator.h"
#include "PropBag.h"
#include "mtd_utils.h"

#define TITLEHEIGHT	36
#define BARHEIGHT	28
//#define CXBUTTON	24
//#define CYBUTTON	CXBUTTON
#define CXBUTTON	22
#define CYBUTTON	21

// method panel's controls
#define IDC_METHOD_TITLE		10000
#define IDC_METHOD_BAR			10001
#define IDC_METHOD_BODY			10002

// buttonbar's controls
#define IDC_MENU_BTN			10003
#define IDC_NEW_METHOD_BTN		10004
#define IDC_COPY_METHOD_BTN		10005
#define IDC_SAVE_METHOD_BTN		10006
#define IDC_REC_METHOD_BTN		10007
#define IDC_PLAY_METHOD_BTN		10008
#define IDC_DEL_FUNC_BTN		10009
#define IDC_UNDO_METHOD_BTN		10010
#define IDC_REDO_METHOD_BTN		10011


/////////////////////////////////////////////////////////////////////////////////////////
//CMethodPanel
IMPLEMENT_DYNCREATE(CMethodPanel , CWnd);

//////////////////////////////////////////////////////////////////
CMethodPanel::CMethodPanel(void) 
{
	_OleLockApp( this );

	m_dwDockSide = AFX_IDW_DOCKBAR_LEFT;
	m_size = CSize(250, 100);
													
	m_sName = "MethodPanel";
	m_sUserName.LoadString( IDS_METHOD_PANEL );

	EnableAutomation();

	// register 
	IUnknown *punk_mtd_man = 0;
	m_list_func_ctrl.get_mtd_man( &punk_mtd_man );
	if( punk_mtd_man )
	{
		Register( punk_mtd_man );
		punk_mtd_man->Release();
		punk_mtd_man = 0;
	}
}

//////////////////////////////////////////////////////////////////
CMethodPanel::~CMethodPanel(void)
{
	// unregister 
	IUnknown *punk_mtd_man = 0;
	m_list_func_ctrl.get_mtd_man( &punk_mtd_man );
	if( punk_mtd_man )
	{
		UnRegister( punk_mtd_man );
		punk_mtd_man->Release();
		punk_mtd_man = 0;
	}
	
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CMethodPanel, CWnd)
	ON_WM_SIZE( )
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
    ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CMethodPanel, CWnd)
	INTERFACE_PART(CMethodPanel, IID_IWindow,	Wnd				)
	INTERFACE_PART(CMethodPanel, IID_IDockableWindow, Dock		)
	INTERFACE_PART(CMethodPanel, IID_IRootedObject,	Rooted		)
	INTERFACE_PART(CMethodPanel, IID_IEventListener,	EvList	)
	INTERFACE_PART(CMethodPanel, IID_INamedObject2,	Name		)
	INTERFACE_PART(CMethodPanel, IID_IMsgListener,	MsgList		)
	INTERFACE_PART(CMethodPanel, IID_IHelpInfo,		Help		)
END_INTERFACE_MAP()

// {BC8FE138-2565-4ef1-894D-D075A6DC10BC}
GUARD_IMPLEMENT_OLECREATE( CMethodPanel, "Metodics.MethodPanel", 
0xbc8fe138, 0x2565, 0x4ef1, 0x89, 0x4d, 0xd0, 0x75, 0xa6, 0xdc, 0x10, 0xbc);

//////////////////////////////////////////////////////////////////
CWnd *CMethodPanel::GetWindow()
{
	return this;
}  

bool CMethodPanel::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	if (!Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE/* | WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/, rc, pparent, nID))
		return false;

	
	create_controls( );
	calc_layout( );

	// refresh controls
	m_list_func_ctrl.refresh_data( );

	IUnknownPtr sptr_active_mtd = 0;
	m_list_func_ctrl.get_mtd( &sptr_active_mtd );

	IMethodPtr sptr_mtd = sptr_active_mtd;
	m_title.m_str_method_name = _T("");
	if( sptr_mtd != 0 )
	{
		_bstr_t bstrt_name;
		sptr_mtd->GetName( bstrt_name.GetAddress() );
		m_title.m_str_method_name = (TCHAR *)bstrt_name;
	}			
	if( m_title.GetSafeHwnd() )
		m_title.Invalidate();

	update_btns_states( );

	return true;
}

//////////////////////////////////////////////////////////////////
void CMethodPanel::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CString str_tooltip( _T("") );

	BOOL	bneed_update_btns_states = FALSE;

	if( !strcmp(pszEvent, szEventMethodRecord) )	
	{
		// set pushed button
		m_buttons_bar.m_btn_rec_method.SetPressedState( TRUE );

		// set tooltip
		str_tooltip.LoadString( IDS_STOP_REC_STR );
		m_buttons_bar.m_btn_rec_method.SetToolTipText( str_tooltip );

		// [vanek] SBT:1004 - 21.05.2004
		//::EnableWindow( m_list_func_ctrl.handle(), FALSE );
		// aam: SBT 1354 // 14.07.2005 build 87
		m_list_func_ctrl.DisableUI(TRUE);

		bneed_update_btns_states = TRUE;
	}
	else if( !strcmp(pszEvent, szEventMethodRun) )	
	{				
		// set pushed button
		m_buttons_bar.m_btn_play_method.SetPressedState( TRUE );
		
		// set tooltip
		str_tooltip.LoadString( IDS_STOP_PLAY_STR );
		m_buttons_bar.m_btn_play_method.SetToolTipText( str_tooltip );	
	
		// [vanek] SBT:1004 - 21.05.2004 
		//::EnableWindow( m_list_func_ctrl.handle(), FALSE );
		// aam: SBT 1354 // 14.07.2005 build 87
		m_list_func_ctrl.DisableUI(TRUE);

		bneed_update_btns_states = TRUE;
	}
	else if( !strcmp(pszEvent, szEventMethodStop) )	
	{
		// set unpushed buttons
		m_buttons_bar.m_btn_rec_method.SetPressedState( FALSE );
		m_buttons_bar.m_btn_play_method.SetPressedState( FALSE );

		// set tooltips
		str_tooltip.LoadString( IDS_START_REC_STR );
		m_buttons_bar.m_btn_rec_method.SetToolTipText( str_tooltip );
        str_tooltip.LoadString( IDS_START_PLAY_STR );
		m_buttons_bar.m_btn_play_method.SetToolTipText( str_tooltip );

		// [vanek] SBT:1004 - 21.05.2004
		//::EnableWindow( m_list_func_ctrl.handle(), TRUE );
		// aam: SBT 1354 // 14.07.2005 build 87
		m_list_func_ctrl.DisableUI(FALSE);


		bneed_update_btns_states = TRUE;
	}
	else if( !strcmp(pszEvent, szEventChangeMethod) )	
	{
		long lcode = 0;
		lcode = *(long *)(pdata);

		IMethodPtr sptr_mtd = punkHit;
        
		switch( lcode )
		{
		case cncActivate:
			{
				m_title.m_str_method_name = _T("");
				if( sptr_mtd != 0 )
				{
					_bstr_t bstrt_name;
					sptr_mtd->GetName( bstrt_name.GetAddress() );
					m_title.m_str_method_name = (TCHAR *)bstrt_name;
				}			
				if( m_title.GetSafeHwnd() )	m_title.Invalidate();
				m_list_func_ctrl.refresh_data( );
				bneed_update_btns_states = TRUE;
			}
            break;

		case cncDeactivate:
			{	
				//// check modified - перенесено в MethodMan.SetActiveMethodPos (SBT 1327)
				//IMethodDataPtr sptr_mtd_data = sptr_mtd;
				//if( sptr_mtd_data != 0 && sptr_mtd != 0 )
				//{
    //                BOOL	bmodified = FALSE;
    //                sptr_mtd_data->GetModifiedFlag( &bmodified );
				//	if( bmodified )
				//	{
    //                    CString str_title( _T("") ),
				//				str_msg( _T("") );

				//		str_title.LoadString( IDS_CHANGE_ACTIVE_METHOD_STR );
				//		_bstr_t bstrt_name;
				//		sptr_mtd->GetName( bstrt_name.GetAddress() );
				//		str_msg.Format( IDS_METHOD_MODIFIED_STR, (TCHAR*)(bstrt_name) );

				//		_bstr_t bstrt_pathname;
				//		bstrt_pathname = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
				//		if( bstrt_pathname.length( ) )
				//		{
				//			bstrt_pathname += bstrt_name + _T(".method");

				//			if( IDYES == ::MessageBox( GetSafeHwnd(), str_msg, str_title, MB_YESNO | MB_ICONQUESTION ) )
				//			{
				//				// обновим состояние сплиттера в активном шаге, прежде чем переключаться
				//				sptr_mtd_data->UpdateActiveStep();
				//				sptr_mtd->StoreFile( bstrt_pathname );
				//			}
				//			else
				//				sptr_mtd->LoadFile( bstrt_pathname );
				//		}
				//	}
				//}       

				// reset
				m_title.m_str_method_name = _T("");
				if( m_title.GetSafeHwnd() )		m_title.Invalidate();
                m_list_func_ctrl.clear( );
				m_list_func_ctrl.reset_active_mtd( );
			}

			break;
		}        		
	}
	else if( !strcmp(pszEvent, szEventChangeMethodStep) )	
	{
		tagMethodStepNotification msn = {0};

		IUnknownPtr sptr_active_mtd = 0;
		m_list_func_ctrl.get_mtd( &sptr_active_mtd );

		// response changing for steps of active method
		if( (punkFrom == sptr_active_mtd) && (cbSize == sizeof(msn)) )        		   
		{
			msn = *(tagMethodStepNotification *)(pdata);

			switch( msn.lHint )
			{
			case cncAdd:
				{
					IMethodDataPtr sptr_mtd_data = sptr_active_mtd;
					if( sptr_mtd_data != 0 )
					{
						long lpos_next_step = msn.lPos;
						sptr_mtd_data->GetNextStep( &lpos_next_step, 0 );
						if( lpos_next_step )
							m_list_func_ctrl.insert_before_step( lpos_next_step, msn.lPos );
						else // add to end
							m_list_func_ctrl.insert_step( msn.lPos, (int)(m_list_func_ctrl.get_items_count()) );
					}
				}
				// [vanek] SBT:999 - 21.05.2004
				bneed_update_btns_states = TRUE;
				break;

			case cncRemove:
				m_list_func_ctrl.delete_step( msn.lPos );
				// [vanek] SBT:999 - 21.05.2004
				bneed_update_btns_states = TRUE;
				break;

			case cncChange:
				m_list_func_ctrl.set_redraw( FALSE );
				m_list_func_ctrl.update_check_state( msn.lPos );
				m_list_func_ctrl.set_redraw( TRUE );
				
				m_list_func_ctrl.redraw_step( msn.lPos );
                break;

			case cncReset:
				m_list_func_ctrl.refresh_data( );
				break;

			case cncActivate:
				{
					m_list_func_ctrl.set_active_step_item( msn.lPos );
					// [vanek] : ensure visible active step - 13.10.2004
					if( msn.lPos )
						m_list_func_ctrl.ensure_visible( m_list_func_ctrl.get_selected_item(), false );
                    bneed_update_btns_states = TRUE;
				}
				break;

			case cncDeactivate:
				break;
			}

//			if( msn.lHint == cncAdd || msn.lHint == cncRemove || msn.lHint == cncChange )
//				check_loop( );
		}
	}


	if( bneed_update_btns_states )
		update_btns_states( );

    return;
}

//////////////////////////////////////////////////////////////////
LRESULT CMethodPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_NOTIFY )
	{
        MSG msg = {0};
		msg.message	= WM_NOTIFY;
		msg.hwnd	= GetSafeHwnd();
		msg.wParam	= wParam;
		msg.lParam	= lParam;
		::GetCursorPos( &msg.pt );				

		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), wParam );
		long	lProcessed = false;
		long	lret_reflect = ::SendMessage( hwnd, WM_NOTYFYREFLECT, (WPARAM)&lProcessed, (LPARAM)&msg );
		if( lProcessed ) 
			return lret_reflect;                        		
	}

	// [vanek] : обновляем кнопки, состояние которых зависит от состояния соответствующих акций - 28.10.2004
	if( message == WM_IDLEUPDATECMDUI )
		update_btns_states( true );
	
	return CWnd::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMethodPanel::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL bret_val = FALSE;
    
    switch( LOWORD( wParam ) )
	{
	case IDC_NEW_METHOD_BTN:
		bret_val = on_create_new_mtd( );
		break;
		
	case IDC_COPY_METHOD_BTN:
		bret_val = on_copy_mtd( );
		break;
		
	case IDC_SAVE_METHOD_BTN:
		bret_val = on_save_mtd( );
		break;
		
	case IDC_REC_METHOD_BTN:
		bret_val = on_rec_mtd( );
		break;
		
	case IDC_PLAY_METHOD_BTN:
		bret_val = on_play_mtd( );
		break;

	case IDC_DEL_FUNC_BTN:	
		bret_val = on_del_active_func( );
		break;

	case IDC_MENU_BTN:
		bret_val = on_menu_btn( );
		break;

	case IDC_UNDO_METHOD_BTN:
		bret_val = on_undo_btn( );
		break;

	case IDC_REDO_METHOD_BTN:
		bret_val = on_redo_btn( );
		break;
	}

	return bret_val ? bret_val : __super::OnCommand( wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMethodPanel::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if (!pmsg)
		return FALSE;

	*plResult = CWnd::WalkPreTranslateTree( m_hWnd, pmsg );
	return (*plResult);
}

/////////////////////////////////////////////////////////////////////////////
void CMethodPanel::PostNcDestroy()
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMethodPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    return CWnd::Create(AfxRegisterWndClass(/*CS_DBLCLKS | */CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_3DFACE + 1), 0), 
						m_sName, dwStyle | WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS , rect,
						pParentWnd, nID, NULL);

}

/////////////////////////////////////////////////////////////////////////////
int CMethodPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

    IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->AttachMsgListener(GetControllingUnknown());

	return 0;        
}

/////////////////////////////////////////////////////////////////////////////
void CMethodPanel::OnDestroy()
{
    IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->DetachMsgListener(GetControllingUnknown());

	__super::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void CMethodPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize( nType, cx, cy );
	calc_layout( );
}

void CMethodPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE )
	{
		int i = 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMethodPanel::create_controls(void)
{
	bool bflatstyle = 0 == ::GetValueInt( GetAppUnknown(), "\\General", "ButtonStyle", 0 );
	m_title.m_bsemiflat = m_buttons_bar.m_bsemiflat = bflatstyle;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP;
    
	// create title
	CRect rc_title( 0, 0, 5, TITLEHEIGHT);
    m_title.Create( 0, 0, dwStyle, rc_title, this, IDC_METHOD_TITLE );		

	// create buttonsbar
	CRect rc_bar( 0, 0, 5, BARHEIGHT );
	
	if( m_buttons_bar.Create( 0, 0, dwStyle, rc_bar, this, IDC_METHOD_BAR ) )
	{	// enable/disable buttons - start state
		m_buttons_bar.m_btn_new_method.EnableButton( TRUE );
		//m_buttons_bar.m_btn_copy_method.EnableButton( FALSE );
		m_buttons_bar.m_btn_save_method.EnableButton( FALSE );
		m_buttons_bar.m_btn_rec_method.EnableButton( FALSE );
		m_buttons_bar.m_btn_play_method.EnableButton( FALSE );        
		m_buttons_bar.m_btn_delete_func.EnableButton( FALSE );

		m_buttons_bar.m_btn_undo_method.EnableButton( FALSE );
		m_buttons_bar.m_btn_redo_method.EnableButton( FALSE );
	}

	// create list
	CRect rc_list( 0, 0, 10, 10);
	m_list_func_ctrl.create_ex( dwStyle | (bflatstyle ? 0 : WS_BORDER) , rc_list, 0, this->GetSafeHwnd(), (HMENU)(IDC_METHOD_BODY), m_list_func_ctrl.window_class(), bflatstyle ? WS_EX_STATICEDGE : (WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME) );  
	m_list_func_ctrl.refresh_data( );
}

/////////////////////////////////////////////////////////////////////////////
void CMethodPanel::calc_layout( )
{
    CRect rc_free;
	GetClientRect( rc_free );    

	if( m_title.GetSafeHwnd( ) )
	{
		CRect rc_title;
		rc_title.left = rc_free.left;
		rc_title.top = rc_free.top;
		rc_title.right = rc_free.right;
		rc_title.bottom = rc_free.top + TITLEHEIGHT;
		m_title.MoveWindow( rc_title, TRUE );

		::SubtractRect( &rc_free, &rc_free, &rc_title );
	}

	if( m_buttons_bar.GetSafeHwnd( ) )
	{
		CRect rc_bar = rc_free;
		rc_bar.bottom = rc_bar.top + BARHEIGHT;
		m_buttons_bar.MoveWindow( rc_bar, TRUE );

		::SubtractRect( &rc_free, &rc_free, &rc_bar );
	}

	if( m_list_func_ctrl.handle() )
		m_list_func_ctrl.move_window( rc_free, true);
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_create_new_mtd(void)
{
	/*IMethodManPtr sptr_mtd_man;
	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	if( punk_mtd_man )
	{	
		sptr_mtd_man = punk_mtd_man;
		punk_mtd_man->Release( ); 
		punk_mtd_man = 0;
	}

	if( sptr_mtd_man != 0 )
	{	

		long lpos_mtd = 0;
		sptr_mtd_man->GetFirstMethodPos( &lpos_mtd );
		if( lpos_mtd )
		{
			sptr_mtd_man->SetActiveMethodPos( lpos_mtd );
			m_list_func_ctrl.refresh_data( );
		}  


	}*/

	::ExecuteAction( "MtdCreateMethod", "\"\", 1", 0 );
	

	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_copy_mtd(void)
{
	::ExecuteAction( "MtdCopyMethod", "\"\", 1", 0 );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_save_mtd(void)
{
	::ExecuteAction( "MtdSaveMethod", "\"\"", 0 );	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_rec_mtd(void)
{
	IUnknown *punk_mtd_man = 0;
	
	m_list_func_ctrl.get_mtd_man( &punk_mtd_man );
	IMethodManPtr sptr_mtd_man = punk_mtd_man; 
	if( punk_mtd_man )
		punk_mtd_man->Release( ); punk_mtd_man = 0;

	if( sptr_mtd_man == 0 )
		return FALSE;

	if( m_buttons_bar.m_btn_rec_method.IsPressed( ) )
		sptr_mtd_man->Stop( );
	else
		sptr_mtd_man->Record( );        

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_play_mtd(void)
{
	IUnknown *punk_mtd_man = 0;
	
	m_list_func_ctrl.get_mtd_man( &punk_mtd_man );
	IMethodManPtr sptr_mtd_man = punk_mtd_man; 
	if( punk_mtd_man )
		punk_mtd_man->Release( ); punk_mtd_man = 0;

	if( sptr_mtd_man == 0 )
		return FALSE;

	if( m_buttons_bar.m_btn_play_method.IsPressed( ) )
		sptr_mtd_man->Stop( );
	else
		// [vanek] BT2000: 4003, 4004 - 17.09.2004
		sptr_mtd_man->RunFromActive( );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodPanel::on_del_active_func(void)
{
    IUnknown *punk_mtd = 0;   
	IMethodDataPtr sptr_mtd;
	if( !m_list_func_ctrl.get_mtd( &punk_mtd ) )
		return FALSE;
	
	sptr_mtd = punk_mtd;
	if( punk_mtd )
		punk_mtd->Release(); punk_mtd = 0;

	if( sptr_mtd == 0 )
		return FALSE;

    long lpos_step = 0;
	sptr_mtd->GetActiveStepPos( &lpos_step );
	if( !lpos_step )
		return FALSE;
    
    //sptr_mtd->DeleteStep( lpos_step );
	long lindex = -1;
	if( S_OK != sptr_mtd->GetStepIndexByPos( lpos_step, &lindex ) )
		return FALSE;

	IMethodChangesPtr sptr_mtd_ch =	sptr_mtd;
	if( sptr_mtd_ch == 0 )
		return FALSE;

	CString str_caption( _T("") ), str_message( _T("") );
	str_caption.LoadString( IDS_ATTENTION );
	str_message.LoadString( IDS_METHOD_DELETE_STEP_STR );
	HWND	hwnd_main = 0;
	IApplicationPtr	ptrA(GetAppUnknown());
	ptrA->GetMainWindowHandle(&hwnd_main);
	if( IDNO == ::MessageBox( hwnd_main, str_message, str_caption, MB_YESNO | MB_ICONQUESTION ) )
		return false;
    		
	return S_OK == sptr_mtd_ch->DeleteStep( lindex );
}

BOOL	CMethodPanel::on_menu_btn(void)
{
	IUnknown *punk = 0;
	punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )
		return FALSE;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();
	if( sptrCM == NULL )
		return FALSE;

	RECT rc_menu_btn = {0};
	m_buttons_bar.m_btn_menu.GetWindowRect( &rc_menu_btn );
    POINT point = { rc_menu_btn.right, (int)((double)(rc_menu_btn.top + rc_menu_btn.bottom) / 2. + 0.5) };
	
	CMenuRegistrator rcm;	
	_bstr_t	bstrMenuName = rcm.GetMenu( szMethodPanelMenu, 0 );
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
		    
	return TRUE;
}

BOOL	CMethodPanel::on_undo_btn(void)
{
	::ExecuteAction( "MethodUndo", 0, 0 );	
	return TRUE;
}

BOOL	CMethodPanel::on_redo_btn(void)
{
	::ExecuteAction( "MethodRedo", 0, 0 );	
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void	CMethodPanel::update_btns_states( bool bupdate_actions_btns /*= false*/ )
{
	if( bupdate_actions_btns  )
	{
		// [vanek] : проверка доступности undo/redo методики
		DWORD dwflags = 0;

		// undo button
		dwflags = get_action_flags( _bstr_t(_T("MethodUndo")) );
		m_buttons_bar.m_btn_undo_method.EnableButton( dwflags & afEnabled );

		// redo button
		dwflags = 0;
		dwflags = get_action_flags( _bstr_t(_T("MethodRedo")) );
		m_buttons_bar.m_btn_redo_method.EnableButton( dwflags & afEnabled );

		// save butoon
		dwflags = 0;
		dwflags = get_action_flags( _bstr_t(_T("MtdSaveMethod")) );
		m_buttons_bar.m_btn_save_method.EnableButton( dwflags & afEnabled );

		// run/stop button
		dwflags = 0;
		dwflags = get_action_flags( _bstr_t(_T("MtdRunStop")) );
		m_buttons_bar.m_btn_play_method.EnableButton( dwflags & afEnabled );

	}
	else
	{
		BOOL	brecording = FALSE,
				bplaying = FALSE,
				//bcan_del_mtd = FALSE,
				bcan_copy_mtd = FALSE,
				bcan_edit_mtd = FALSE;

		IUnknownPtr	sptr_unk_active_mtd;
		long		lstep_count = 0,
			lpos_active_step = 0;

		// [vanek] SBT:1160 - 05.10.2004
		DWORD		dwactive_step_state = 0;

		IMethodManPtr sptr_mtd_man;
		IUnknownPtr	sptr_unk_mtd_man = 0;
		m_list_func_ctrl.get_mtd_man( &sptr_unk_mtd_man );
		sptr_mtd_man = sptr_unk_mtd_man;
		if( sptr_mtd_man == 0 )
		{
			ASSERT( false );
			return;
		}

		sptr_mtd_man->IsRunning( &bplaying );	
		sptr_mtd_man->IsRecording( &brecording );
		m_list_func_ctrl.get_mtd( &sptr_unk_active_mtd );

		// [vanek] : поддержка свойств методики: CanDelete, CanCopy и CanEdit - 27.10.2004
		INamedPropBagPtr sptr_propbag_mtd = sptr_unk_active_mtd;
		if( sptr_propbag_mtd != 0 )
		{
			variant_t var;
			sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanCopy)), var.GetAddress() );
			bcan_copy_mtd = var.vt != VT_I4 ? TRUE : var.lVal != 0;

			/* не будем читать, т.к. нет контролов, состояние которых зависело бы от этого значения
			sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanDelete)), var.GetAddress() );
			bcan_del_mtd = var.vt != VT_I4 ? TRUE : var.lVal != 0;*/

			sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), var.GetAddress() );
			bcan_edit_mtd = var.vt != VT_I4 ? TRUE : var.lVal != 0;			
		}


		lstep_count = m_list_func_ctrl.get_items_count( );
		lpos_active_step = m_list_func_ctrl.update_active_step( );
		if( lpos_active_step )
			dwactive_step_state = m_list_func_ctrl.get_step_state( lpos_active_step );

		// [vanek] : поддержка неудаляемых шагов методики - 26.10.2004
		CMethodStep step_data;
		step_data.m_bSkipData = true;
		step_data.m_dwFlags = 0;
		m_list_func_ctrl.get_step_data( &step_data, lpos_active_step );

		// [vanek] : эти кнопки по просьбе технологов убили - 28.10.2004
		m_buttons_bar.m_btn_new_method.EnableButton( !(brecording || bplaying) );
		//m_buttons_bar.m_btn_copy_method.EnableButton( !(brecording || bplaying || sptr_unk_active_mtd == 0 || 
		//	!bcan_copy_mtd) );

		// [vanek] : эти кнопки завязаны на соответствующие акции -> берем стейт из этих акций (см. выше) - 28.10.2004
		//m_buttons_bar.m_btn_save_method.EnableButton( !(brecording || bplaying || sptr_unk_active_mtd == 0) );
		//m_buttons_bar.m_btn_play_method.EnableButton( !(brecording || sptr_unk_active_mtd == 0 || lstep_count == 0) );

		m_buttons_bar.m_btn_rec_method.EnableButton( !(bplaying || sptr_unk_active_mtd == 0 || !bcan_edit_mtd) );
		m_buttons_bar.m_btn_delete_func.EnableButton( !(brecording || bplaying || sptr_unk_active_mtd == 0 ||
			lstep_count == 0 || lpos_active_step == 0 || (step_data.m_dwFlags & msfUndead) || 
			(dwactive_step_state & (essFirstInLoop | essLastInLoop)) || !bcan_edit_mtd) );   

		// [vanek] : есть методика - кнопка активна, все другие случаи будут обрабатываться конкретными командами (акциями) 
		// в контекстном меню методики - 26.10.2004
		m_buttons_bar.m_btn_menu.EnableButton( !(/*brecording || bplaying ||*/ sptr_unk_active_mtd == 0) );
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//CMethodTitleWnd 

/////////////////////////////////////////////////////////////////////////////
CMethodTitleWnd ::CMethodTitleWnd (void) : m_bsemiflat( false )
{
	m_hicon = 0;
	m_hicon = (HICON)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_METHOD_ICON), IMAGE_ICON, 32, 32, 0);
}

/////////////////////////////////////////////////////////////////////////////
CMethodTitleWnd ::~CMethodTitleWnd (void)
{
	if( m_hicon )
		::DestroyIcon( m_hicon ); m_hicon = 0;
}

BEGIN_MESSAGE_MAP(CMethodTitleWnd, CWnd)
	ON_WM_PAINT( )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//long CMethodTitleWnd ::on_paint()
void CMethodTitleWnd::OnPaint()
{
    //PAINTSTRUCT ps = {0};
	//HDC hdc = ::BeginPaint( handle(), &ps );
	CPaintDC dc(this);

	RECT rc_client = {0};
	//::GetClientRect( handle(), &rc_client);
	GetClientRect( &rc_client );



	// draw frame	
	if( m_bsemiflat )
	{
		CBrush brush( ::GetSysColor( COLOR_3DFACE ) );
		dc.FillRect( &rc_client, &brush );
		dc.DrawEdge( &rc_client, BDR_RAISEDINNER , BF_RECT | BF_TOPLEFT );		
		dc.DrawEdge( &rc_client, BDR_RAISEDINNER , BF_RECT | BF_BOTTOMRIGHT );		
	}
	else
		::DrawFrameControl( dc, &rc_client, DFC_BUTTON, DFCS_BUTTONPUSH );
	
	
	RECT rc_draw = rc_client; 
	::InflateRect( &rc_draw, -2, -2 );

	// draw big icon
	if( m_hicon )
	{
        ICONINFO st_ii = {0};
		if( ::GetIconInfo( m_hicon, &st_ii ) )
		{
			if( ::DrawIcon( dc, rc_draw.left, (int)(rc_draw.top + (rc_draw.bottom - rc_draw.top )/ 2. - st_ii.yHotspot + 0.5), m_hicon ) )
				rc_draw.left += (int)(st_ii.yHotspot * 2. + 0.5);

			::DeleteObject( st_ii.hbmColor ), st_ii.hbmColor = 0;
			::DeleteObject( st_ii.hbmMask ), st_ii.hbmMask = 0;
		}
	}


	// draw text
	if( !m_str_method_name.IsEmpty() )
	{
		HFONT hOldFont = (HFONT)::SelectObject( dc, (HFONT)::GetStockObject( DEFAULT_GUI_FONT ) );

		int	nold_bk = ::SetBkMode( dc, TRANSPARENT );
		::DrawText( dc, m_str_method_name, -1, &rc_draw, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP | DT_WORD_ELLIPSIS );
		
		::SelectObject( dc, hOldFont );
		::SetBkMode( dc, nold_bk );

	}

	//::EndPaint( handle(), &ps );
	//return 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CMethodBar

/////////////////////////////////////////////////////////////////////////////
CMethodBar::CMethodBar(void) : 	m_btn_menu( eftCircle ),
								m_btn_new_method( eftRectangle ),
								//m_btn_copy_method( eftRectangle ),
								m_btn_save_method( eftRectangle ),
								m_btn_rec_method( eftRectangle ),
								m_btn_play_method( eftRectangle ),
								m_btn_delete_func( eftRectangle ),
								m_btn_undo_method( eftRectangle ),
								m_btn_redo_method( eftRectangle ),
								m_bsemiflat( false )
{
	// vanek : drawing disabled icons - 27.02.2005
	CFigureButton::m_ImgDrawer.SetEnableDraw( 0 != GetValueInt( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsEnable, 0 ) );
	CFigureButton::m_ImgDrawer.SetAlpha( (float)(GetValueDouble( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsAlpha, 0.5 )) );
}

/////////////////////////////////////////////////////////////////////////////
CMethodBar::~CMethodBar(void)
{
}

BEGIN_MESSAGE_MAP(CMethodBar, CWnd)
    ON_WM_CREATE( )	
	ON_WM_PAINT( )
	ON_WM_SIZE( )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
int CMethodBar::OnCreate(LPCREATESTRUCT pcs)
{
	//long lret = __super::on_create( pcs );
	long lret = __super::OnCreate( pcs );
	if( lret != -1 )
	{
		create_buttons( );
		calc_layout( );
	}

    return lret;		
}

/////////////////////////////////////////////////////////////////////////////
void CMethodBar::OnPaint()
{
    CPaintDC dc( this );

	CRect rc_client;
	GetClientRect( rc_client );
	if( m_bsemiflat )
	{
		CBrush brush( ::GetSysColor( COLOR_3DFACE ) );
		dc.FillRect( &rc_client, &brush );
		dc.DrawEdge( &rc_client, BDR_RAISEDINNER , BF_RECT | BF_TOPLEFT );		
		dc.DrawEdge( &rc_client, BDR_RAISEDINNER , BF_RECT | BF_BOTTOMRIGHT );		
	}
	else
		::DrawFrameControl( dc, &rc_client, DFC_BUTTON, DFCS_BUTTONPUSH );

	
}	 

/////////////////////////////////////////////////////////////////////////////
void CMethodBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize( nType, cx, cy );
	calc_layout( );
}

BOOL CMethodBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CWnd *pwnd_parent = 0;
	pwnd_parent = GetParent();
	if( pwnd_parent )
		return pwnd_parent->SendMessage( WM_COMMAND, wParam, lParam );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodBar::create_buttons(void)
{
	if( !GetSafeHwnd() )
		return FALSE;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP;

	RECT rc_bar = {0};
	::GetClientRect( GetSafeHwnd(), &rc_bar );
	::InflateRect( &rc_bar, -2, -2 );

	int nbar_height = rc_bar.bottom - rc_bar.top;
    
	// create buttons
    RECT rc_btn = {0};
	rc_btn.left = 0;
    rc_btn.right = CXBUTTON;
	rc_btn.top = 0;
	rc_btn.bottom = CYBUTTON;

	// [vanek] SBT:983 - 20.05.2004
	CString str_tooltip( _T("") );

	// new method button
	m_btn_new_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_NEW_METHOD_BTN );
	m_btn_new_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_NEW_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	str_tooltip.LoadString( IDS_CREATE_NEW_STR );
	m_btn_new_method.SetToolTipText( str_tooltip );

	// copy method button
	//m_btn_copy_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_COPY_METHOD_BTN );
	//m_btn_copy_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_COPY_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	//str_tooltip.LoadString( IDS_COPY_STR );
	//m_btn_copy_method.SetToolTipText( str_tooltip );

	// save method button
	m_btn_save_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_SAVE_METHOD_BTN );
	m_btn_save_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_SAVE_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	str_tooltip.LoadString( IDS_SAVE_STR );
	m_btn_save_method.SetToolTipText( str_tooltip );

	// rec method button
	m_btn_rec_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_REC_METHOD_BTN );
	m_btn_rec_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REC_OFF_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	m_btn_rec_method.SetPushedBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REC_ON_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	m_btn_rec_method.SetThreeState( TRUE );
	str_tooltip.LoadString( IDS_START_REC_STR );
	m_btn_rec_method.SetToolTipText( str_tooltip );

	// play method button
	m_btn_play_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_PLAY_METHOD_BTN );
	m_btn_play_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PLAY_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	m_btn_play_method.SetThreeState( TRUE );
	str_tooltip.LoadString( IDS_START_PLAY_STR );
	m_btn_play_method.SetToolTipText( str_tooltip );

	// delete function button
	m_btn_delete_func.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_DEL_FUNC_BTN );
	m_btn_delete_func.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_DEL_FUNC), IMAGE_BITMAP, 0, 0, 0) );
	str_tooltip.LoadString( IDS_DEL_ACTIVE_FUNC_STR );
	m_btn_delete_func.SetToolTipText( str_tooltip );

	// undo method button
	m_btn_undo_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_UNDO_METHOD_BTN );
	m_btn_undo_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UNDO_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	str_tooltip.LoadString( IDS_UNDO_STR );
	m_btn_undo_method.SetToolTipText( str_tooltip );

	// redo method button
	m_btn_redo_method.Create( 0, dwStyle | BS_BITMAP, rc_btn, this, IDC_REDO_METHOD_BTN );
	m_btn_redo_method.SetBitmap( (HBITMAP)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REDO_METHOD), IMAGE_BITMAP, 0, 0, 0) );
	str_tooltip.LoadString( IDS_REDO_STR );
	m_btn_redo_method.SetToolTipText( str_tooltip );


	// menu button
	m_btn_menu.Create( 0, dwStyle | BS_ICON | BS_FLAT, rc_btn, this, IDC_MENU_BTN );
	m_btn_menu.SetIcon( (HICON)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_MENU_ICON), IMAGE_ICON, 16, 16, 0) );
	str_tooltip.LoadString( IDS_OPEN_MENU_STR );
	m_btn_menu.SetToolTipText( str_tooltip );


	// SemiFlat 
	m_btn_new_method.SetSemiFlat( TRUE );
    m_btn_rec_method.SetSemiFlat( TRUE );
	m_btn_play_method.SetSemiFlat( TRUE );
	m_btn_save_method.SetSemiFlat( TRUE );
	m_btn_delete_func.SetSemiFlat( TRUE );
	m_btn_undo_method.SetSemiFlat( TRUE );
	m_btn_redo_method.SetSemiFlat( TRUE );        
	m_btn_menu.SetSemiFlat( TRUE );

	if( m_bsemiflat )
	{	// Flat
		m_btn_new_method.SetFlatButton( TRUE );
		m_btn_rec_method.SetFlatButton( TRUE );
		m_btn_play_method.SetFlatButton( TRUE );
		m_btn_save_method.SetFlatButton( TRUE );
		m_btn_delete_func.SetFlatButton( TRUE );
		m_btn_undo_method.SetFlatButton( TRUE );
		m_btn_redo_method.SetFlatButton( TRUE );        
		m_btn_menu.SetFlatButton( TRUE );

		m_btn_new_method.SetDrawBorder( TRUE );
		m_btn_rec_method.SetDrawBorder( TRUE );
		m_btn_play_method.SetDrawBorder( TRUE );
		m_btn_save_method.SetDrawBorder( TRUE );
		m_btn_delete_func.SetDrawBorder( TRUE );
		m_btn_undo_method.SetDrawBorder( TRUE );
		m_btn_redo_method.SetDrawBorder( TRUE );        
		m_btn_menu.SetDrawBorder( TRUE );
	}
	else
	{	
        m_btn_new_method.SetFlatButton( FALSE );
        m_btn_rec_method.SetFlatButton( FALSE );
		m_btn_play_method.SetFlatButton( FALSE );
		m_btn_save_method.SetFlatButton( FALSE );
		m_btn_delete_func.SetFlatButton( FALSE );
		m_btn_undo_method.SetFlatButton( FALSE );
		m_btn_redo_method.SetFlatButton( FALSE );        
		m_btn_menu.SetFlatButton( FALSE );

		m_btn_new_method.SetDrawBorder( FALSE );
		m_btn_rec_method.SetDrawBorder( FALSE );
		m_btn_play_method.SetDrawBorder( FALSE );
		m_btn_save_method.SetDrawBorder( FALSE );
		m_btn_delete_func.SetDrawBorder( FALSE );
		m_btn_undo_method.SetDrawBorder( FALSE );
		m_btn_redo_method.SetDrawBorder( FALSE );        
		m_btn_menu.SetDrawBorder( FALSE );
	}

	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL	CMethodBar::calc_layout(void)
{
	if( !GetSafeHwnd() )
		return FALSE;

	RECT rc_bar = {0};
	::GetClientRect( GetSafeHwnd(), &rc_bar );
	::InflateRect( &rc_bar, -2, -2 );

	int nbar_height = rc_bar.bottom - rc_bar.top;

	CRect rc_btn;
    rc_btn.left = rc_bar.left;
    rc_btn.right = rc_btn.left + CXBUTTON;
	rc_btn.top = (int)(rc_bar.top + nbar_height / 2. - CYBUTTON / 2. + 0.5);
	rc_btn.bottom = rc_btn.top + CYBUTTON;

	if( m_btn_new_method.GetSafeHwnd() )
	{
		m_btn_new_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_new_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right;// + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	//if( m_btn_copy_method.GetSafeHwnd() )
	//{
	//	m_btn_copy_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
	//	m_btn_copy_method.MoveWindow( rc_btn, TRUE );
	//}
	//rc_btn.left = rc_btn.right + 1;
    //rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_rec_method.GetSafeHwnd() )
	{
        m_btn_rec_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_rec_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_play_method.GetSafeHwnd() )
	{
		m_btn_play_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_play_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_save_method.GetSafeHwnd() )
	{
		m_btn_save_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_save_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_delete_func.GetSafeHwnd() )
	{
		m_btn_delete_func.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_delete_func.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_undo_method.GetSafeHwnd() )
	{
		m_btn_undo_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_undo_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	if( m_btn_redo_method.GetSafeHwnd() )
	{
		m_btn_redo_method.ShowWindow( rc_btn.right > rc_bar.right ? SW_HIDE : SW_SHOW );
		m_btn_redo_method.MoveWindow( rc_btn, TRUE );
	}
	rc_btn.left = rc_btn.right; // + 1;
    rc_btn.right = rc_btn.left + CXBUTTON;

	CRect rc_menu_btn;
	rc_menu_btn = rc_btn;
    rc_menu_btn.right = rc_bar.right;
	rc_menu_btn.left = rc_menu_btn.right - CXBUTTON;
	if( m_btn_menu.GetSafeHwnd() )
	{
		if( rc_menu_btn.left <= rc_btn.left )
			m_btn_menu.ShowWindow( SW_HIDE );
		else
			m_btn_menu.ShowWindow( SW_SHOW );

		m_btn_menu.MoveWindow( rc_menu_btn, TRUE );        
	}

    return TRUE;
}