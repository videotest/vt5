#include "stdafx.h"
#include "funcmethodlistctrl.h"
#include "resource.h"

#define CX_ICON	16
#define CY_ICON	CX_ICON

///////////////////////////////////////////////////////////////////////
// CFuncMethodListCtrl

///////////////////////////////////////////////////////////////////////
CFuncMethodListCtrl::CFuncMethodListCtrl( )
{
	// initialisation list-view
	INITCOMMONCONTROLSEX icex = {0};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);    	

	m_litem_hot = -1;
	m_hrgn_old = 0;

	m_block_items_operation = FALSE;
	m_block_steps_operation = FALSE;
	m_block_ui = FALSE;

	// drag'n'drop
	m_bdragging = FALSE;
	m_nstart_drag_item = -1;
	m_ndrag_item = -1;
	m_hdrag_cursor = 0;
	m_hnodrag_cursor = 0;
	m_hbefore_drag_cursor = 0;
	m_hdrag_cursor = ::LoadCursor( AfxGetResourceHandle(), MAKEINTRESOURCE( IDC_DRAGGING ) );
    m_hnodrag_cursor = ::LoadCursor( AfxGetResourceHandle(), MAKEINTRESOURCE( IDC_NODRAGGING ) );

	m_lpos_active_mtd = 0;

	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	if( punk_mtd_man )
	{	
		m_sptr_mtd_man = punk_mtd_man;
		punk_mtd_man->Release( ); 
		punk_mtd_man = 0;
	}


}

///////////////////////////////////////////////////////////////////////
CFuncMethodListCtrl::~CFuncMethodListCtrl( )
{
	m_lpos_active_mtd = 0;

	if( m_hrgn_old )
		::DeleteObject( m_hrgn_old ), m_hrgn_old = 0;

	if( m_hdrag_cursor )
		::DestroyCursor( m_hdrag_cursor ), m_hdrag_cursor = 0;

	if( m_hnodrag_cursor )
		::DestroyCursor( m_hnodrag_cursor ), m_hnodrag_cursor = 0;
}

BOOL CFuncMethodListCtrl::DisableUI(BOOL bDisable)
{ // разрешить/запретить UI - вместо EnableWindow (чтобы работала прокрутка, SBT1354)
	BOOL bOld = m_block_ui;
	m_block_ui = bDisable;
	return bOld;
}

///////////////////////////////////////////////////////////////////////
void CFuncMethodListCtrl::set_def_styles( void )
{
	if( !handle() )
		return;

	DWORD dwstyle = 0;
	dwstyle = ::GetWindowLong( handle(), GWL_STYLE );
	dwstyle &= ~LVS_TYPEMASK;
	dwstyle |= LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS | LVS_REPORT | LVS_SINGLESEL | LVS_ALIGNTOP;
	::SetWindowLong( handle(), GWL_STYLE, dwstyle );

	ListView_SetExtendedListViewStyle( handle(), LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT  );
}

///////////////////////////////////////////////////////////////////////
BOOL CFuncMethodListCtrl::init(void)
{
	if( !handle() )
		return FALSE;

	set_def_styles( );

	ListView_SetBkColor( handle(), ::GetSysColor( COLOR_BTNFACE ) );
	ListView_SetTextBkColor( handle(), ::GetSysColor( COLOR_BTNFACE ) );
	LVCOLUMN st_column = {0};

	st_column.cx = 100;
	st_column.mask = LVCF_WIDTH;
	insert_column( 0, &st_column );

	if( !ListView_GetImageList( handle(), LVSIL_SMALL ) )
	{
		// Ensure that the common control DLL is loaded. 
		InitCommonControls(); 
		HIMAGELIST himlIcons = 0;

		// Create a masked image list large enough to hold the icons. 
		himlIcons = ImageList_LoadBitmap( AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PROPS), CX_ICON, 0, RGB(0,128,0) ); 
		if( himlIcons  )
			ListView_SetImageList( handle(), himlIcons, LVSIL_SMALL );
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
bool CFuncMethodListCtrl::subclass( HWND hwnd, bool f_nccreate /*= false*/ )
{
	bool bret = __super::subclass( hwnd, f_nccreate );
	if( bret )
		init( );

	return bret;    
}

///////////////////////////////////////////////////////////////////////
LRESULT	CFuncMethodListCtrl::handle_message(UINT m, WPARAM w, LPARAM l)
{
	LRESULT lret = __super::handle_message(m, w, l);

	if( m == WM_WINDOWPOSCHANGED )
	{
		WINDOWPOS *lpwpos = (WINDOWPOS *)l;
		if( !lpwpos || !(lpwpos->flags & SWP_NOSIZE) ) 
		{
			RECT rc_client = {0};					
			get_client_rect( &rc_client );
			if( (rc_client.right - rc_client.left) != ListView_GetColumnWidth( handle(), 0) )
			{
				ListView_SetColumnWidth( handle(), 0, (rc_client.right - rc_client.left - 2) );                       
				ListView_Update( handle(), 0 ); // for hiding horizontal scrollbar
			}
		}
	} 
	else if( m == WM_MOUSELEAVE )
	{
		reset_hot_step( );
	}
	
	return lret;
}

///////////////////////////////////////////////////////////////////////
int		CFuncMethodListCtrl::find_step(TPOS lpos_step, int nstart_from /*= -1*/)
{
	if( !handle() )
		return -1;
	
	LVFINDINFO st_find = {0};
	st_find.flags = LVFI_PARAM;
	st_find.lParam = (LPARAM)lpos_step;

	return ListView_FindItem( handle(), nstart_from, &st_find );
}

///////////////////////////////////////////////////////////////////////
int		CFuncMethodListCtrl::insert_step(TPOS lpos_step, int nitem)
{
	if( !handle() || m_block_items_operation )
		return -1;

	int nitem_found = -1;
	nitem_found = find_step( lpos_step );
	if( nitem_found != -1 )
		return nitem_found;

	LVITEM st_item = {0};
	st_item.mask = LVIF_PARAM | LVIF_IMAGE;
	st_item.iItem = nitem;
	st_item.lParam = (LPARAM)lpos_step; // store step's position to lParam
	st_item.iImage = I_IMAGECALLBACK;

	nitem = insert_item( &st_item );    
	update_check_state( nitem );
	
	return nitem;
}

///////////////////////////////////////////////////////////////////////
int		CFuncMethodListCtrl::insert_after_step(TPOS lpos_target, TPOS lpos_step)
{
	if( !handle() || m_block_items_operation )
		return -1;

	int nstep = 0;
	nstep = find_step( lpos_step );
	if( nstep != -1 ) 
		return nstep;

	int nitem_target = 0;
	nitem_target = find_step( lpos_target );

	return insert_step( lpos_step, max( nitem_target + 1, 0) );
}

///////////////////////////////////////////////////////////////////////
int		CFuncMethodListCtrl::insert_before_step(TPOS lpos_target, TPOS lpos_step)
{
	if( !handle() || m_block_items_operation )
		return -1;

	int nstep = 0;
	nstep = find_step( lpos_step );
	if( nstep != -1 ) 
		return nstep;

	int nitem_target = 0;
	nitem_target = find_step( lpos_target );

	return insert_step( lpos_step, max( nitem_target, 0) );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::delete_step(TPOS lpos_step)
{
	if( !handle() || !lpos_step || m_block_items_operation )
		return FALSE;

	int nitem = -1;
	nitem = find_step( lpos_step );
	if( nitem == -1 )
		return FALSE;

	delete_item( nitem );    
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::clear( )
{
	if( !handle() )
		return;

	delete_all_items( );
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::fill( )
{
	if( !handle() )
		return;

	// на всякий случай
	if( get_items_count() )
		clear( ); 

	if( m_sptr_active_mtd == 0 )
		return;

	TPOS	lpos_step = 0,
		lpos_prev_step = 0;

	// adding steps
	for( m_sptr_active_mtd->GetFirstStepPos( &lpos_step ); lpos_step;
		lpos_prev_step = lpos_step, m_sptr_active_mtd->GetNextStep( &lpos_step, 0) )
		insert_after_step( lpos_prev_step, lpos_step );
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::update_check_states( )
{
	if( !handle() || m_sptr_active_mtd == 0 )    
		return;

	int nitems_count = 0;
	nitems_count = (int)get_items_count();	

	for( int nitem = 0; nitem < nitems_count; nitem ++ )
		update_check_state( nitem );
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::update_check_state(TPOS lpos_step)
{
	update_check_state( find_step( lpos_step, -1 ) );
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::update_check_state( int nitem )
{
	if( !handle() || nitem == -1 )
		return;

    CMethodStep step_data;
	step_data.m_bSkipData = true;
	if( !get_step_data( &step_data, nitem ) )
		return;
	
    CLockerFlag locker( &m_block_steps_operation );
	// [vanek] для начала и конца цикла не отображаем checkbox - 10.06.2004
	// [vanek] : у неубиваемых тоже нет checkbox - 26.10.2004
	if( is_begin_loop_action( step_data.m_bstrActionName ) || is_end_loop_action( step_data.m_bstrActionName ) ||
		(step_data.m_dwFlags & msfUndead) )
	{
		ListView_SetItemState( handle(), nitem, INDEXTOSTATEIMAGEMASK(-1), LVIS_STATEIMAGEMASK );
	}
	else
	{
		ListView_SetCheckState( handle(), nitem, step_data.m_dwFlags & msfChecked ); 			
	}
}

///////////////////////////////////////////////////////////////////////
TPOS	CFuncMethodListCtrl::update_active_step(void)
{
	if( !handle() )
		return 0;

	if( m_sptr_active_mtd == 0 )    
		return 0;

	TPOS lpos_active_step = 0;
	m_sptr_active_mtd->GetActiveStepPos( &lpos_active_step );
	
	int nitem = 0;
	nitem = find_step( lpos_active_step );
	if( nitem != get_selected_item( ) )
	{
		set_selected_item( nitem );
		//ListView_RedrawItems( handle(), nitem, nitem );
	}
	
	return lpos_active_step;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::set_active_step(TPOS lpos_step)
{
	if( !handle() )    
		return;

	int nitem = 0;
	nitem = find_step( lpos_step );

	int	nitem_selected = -1;
	nitem_selected = get_selected_item( );
	if( nitem == nitem_selected )
		return;
	
	if( nitem != -1 )
		set_selected_item( nitem );
	else
	{	// deselecting item
        ListView_SetItemState( handle(), nitem_selected, 0, LVIS_SELECTED|LVIS_FOCUSED );	
		ListView_SetSelectionMark( handle(), -1 );        
	}
	
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::set_active_step_item(TPOS lpos_step)
{
	if( !handle() )
		return;
	
	CLockerFlag locker( &m_block_steps_operation );
	set_active_step( lpos_step );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::is_cached( int nitem_step )
{
	if( !m_lpos_active_mtd || m_sptr_mtd_man == 0 )
		return FALSE;

	TPOS lpos_step = 0;
	lpos_step = (TPOS)get_item_data( nitem_step );
	if( !lpos_step )
		return FALSE;

	BOOL bcached = FALSE;
	IUnknownPtr ptrMethod;
	TPOS lMethodPos2 = m_lpos_active_mtd;
	if( S_OK != m_sptr_mtd_man->GetNextMethod(&lMethodPos2, &ptrMethod) )
		return FALSE;
	if( S_OK != m_sptr_mtd_man->IsCached( ptrMethod, lpos_step, &bcached ) )
		return FALSE; 	

	return bcached;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::get_mtd_man( IUnknown **ppunk_mtd_man )
{
	if( !ppunk_mtd_man || m_sptr_mtd_man == 0)
		return FALSE;

    *ppunk_mtd_man = m_sptr_mtd_man;
	if( !(*ppunk_mtd_man) )
		return FALSE;
    
	(*ppunk_mtd_man)->AddRef( );
    return TRUE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::get_mtd( IUnknown **ppunk_mtd )
{
	if( !ppunk_mtd || m_sptr_active_mtd == 0 )
		return FALSE;

	*ppunk_mtd = m_sptr_active_mtd;
	if( !(*ppunk_mtd ) )
		return FALSE;

	(*ppunk_mtd)->AddRef( );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::redraw_step(TPOS lpos_step)
{
	if( !handle() )
		return;

	int nitem = -1;
	nitem = find_step( lpos_step );
    
	if( nitem == -1 )
		return;

	ListView_RedrawItems( handle(), nitem, nitem );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::refresh_data( )
{
	clear( );	
    reset_active_mtd( );

	if( m_sptr_mtd_man == 0)
		return FALSE;

	TPOS lpos_mtd = 0;
	if( S_OK != m_sptr_mtd_man->GetActiveMethodPos( &lpos_mtd ) )
		return FALSE;

	if( !lpos_mtd )
		return FALSE;

	IUnknown *punk_active_mtd = 0;
	TPOS lpos_mtd_saved = lpos_mtd;
	if( S_OK != m_sptr_mtd_man->GetNextMethod( &lpos_mtd, &punk_active_mtd ) )
		return FALSE;

	m_sptr_active_mtd = punk_active_mtd;
	if( punk_active_mtd )
		punk_active_mtd->Release();	punk_active_mtd = 0;

	if( m_sptr_active_mtd == 0 )
		return FALSE;

	m_lpos_active_mtd = lpos_mtd_saved;

	fill( );
	update_active_step( );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::reset_active_mtd(void)
{
    if( m_sptr_active_mtd != 0 )	
		m_sptr_active_mtd = 0;

	if( m_lpos_active_mtd )	
		m_lpos_active_mtd = 0;    
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::get_step_data( CMethodStep *pstep_data, TPOS lpos_step )
{
	if( !pstep_data || m_sptr_active_mtd == 0 )
		return FALSE;

	return (S_OK == m_sptr_active_mtd->GetNextStep( &lpos_step,  pstep_data ) );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::get_step_data(CMethodStep *pstep_data, int nitem_step, TPOS *plpos_step /*= 0*/)
{
	if( !pstep_data || m_sptr_active_mtd == 0 )
		return FALSE;

	TPOS lpos_step = 0;
	lpos_step = (TPOS)get_item_data( nitem_step );
	if( !lpos_step )
		return FALSE;

	if( plpos_step )
		*plpos_step = lpos_step;

	return get_step_data( pstep_data, lpos_step );  
}


///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::set_step_data( CMethodStep *pstep_data, int nitem_step, bool bdont_clear_cache /*= false*/ )
{
	if( m_block_steps_operation )
	{
		ASSERT(false);
		return FALSE;
	}

	if( !handle() || !pstep_data || m_sptr_active_mtd == 0 )
		return FALSE;

	IMethodChangesPtr sptr_mtd_ch = m_sptr_active_mtd;
	if( sptr_mtd_ch == 0 )
		return FALSE;

	return S_OK == sptr_mtd_ch->ChangeStep( nitem_step, pstep_data, bdont_clear_cache );
}

///////////////////////////////////////////////////////////////////////
int		CFuncMethodListCtrl::get_icon_step(TPOS lpos_step)
{
	if( m_sptr_active_mtd == 0 )
		return -1;

	CMethodStep step_data;
	step_data.m_bSkipData = true;
	if( S_OK != m_sptr_active_mtd->GetNextStep( &lpos_step, &step_data ) )
		return -1;
	
	if( is_begin_loop_action( step_data.m_bstrActionName ) ) 
		return 3;

	if( is_end_loop_action( step_data.m_bstrActionName ) )
		return 4;
	
	return (step_data.m_dwFlags & msfHasPropPage) ? ((step_data.m_dwFlags & msfShowPropPage)? 0 : 1) : 2; 
}

///////////////////////////////////////////////////////////////////////
DWORD		CFuncMethodListCtrl::get_step_state(TPOS lpos_step)
{
	if( !lpos_step || m_sptr_active_mtd == 0 )	
		return 0;

	TPOS lcurr_step_pos = lpos_step;
	CMethodStep step_data;
	step_data.m_bSkipData = true;
	do
	{
		TPOS lsaved_pos = lcurr_step_pos;
		m_sptr_active_mtd->GetNextStep( &lcurr_step_pos, &step_data );
		
		if( _bstr_t( step_data.m_bstrActionName ) == _bstr_t( szBeginMethodLoop ) )
			return lsaved_pos == lpos_step ? essFirstInLoop | essInLoop : essNotInLoop;
		else if( _bstr_t( step_data.m_bstrActionName ) == _bstr_t( szEndMethodLoop ) )
		    return lsaved_pos == lpos_step ? essLastInLoop | essInLoop : essInLoop;

	}while( lcurr_step_pos );

	return essNotInLoop;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::calc_loop_rect( RECT *ploop_rect )
{
	if( !handle() || !ploop_rect )
		return FALSE;

	::SetRectEmpty( ploop_rect );

	bool	bfirst_found = false,
		blast_found = false;

	long litem_count = 0;
	litem_count = get_items_count( );
	for( long litem_idx = 0; (litem_idx < litem_count) & ( !bfirst_found || !blast_found); litem_idx++ )
	{  
		DWORD dwstate = 0;
		dwstate = get_step_state( (TPOS)get_item_data( (int)(litem_idx) ) );
		if( !dwstate  || (dwstate & essNotInLoop) )
			continue;

		if( dwstate & (essFirstInLoop | essLastInLoop))
		{
			RECT rc_item = {0};
			get_item_rect( (int)(litem_idx), LVIR_LABEL, &rc_item );
			if( !bfirst_found ) 
			{
				if( bfirst_found = (dwstate & essFirstInLoop) == essFirstInLoop )
					rc_item.top = (int)(rc_item.top + (double)(rc_item.bottom - rc_item.top) / 2. + 0.5);
			}

			if( !blast_found ) 
			{
				if( blast_found = (dwstate & essLastInLoop) == essLastInLoop )
					rc_item.bottom = (int)(rc_item.bottom - (double)(rc_item.bottom - rc_item.top) / 2. + 0.5);
			}

			::UnionRect( ploop_rect, ploop_rect, &rc_item );
		}			
	}

	if( !bfirst_found || !blast_found )
		return FALSE;
	
	ploop_rect->right --;
	ploop_rect->bottom --;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
{
	switch( pnmhdr->code )
	{
	case NM_CUSTOMDRAW:
		return OnCustomDraw( (NMLVCUSTOMDRAW *)pnmhdr, plProcessed );

	case LVN_GETDISPINFO :
		OnDispInfo( (NMLVDISPINFO*)pnmhdr, plProcessed );
		break;

	case LVN_ITEMCHANGED:
		OnItemChanged( (NMLISTVIEW *)pnmhdr, plProcessed );
		break;

	case LVN_BEGINDRAG:
		OnBeginDrag( (NMLISTVIEW *)pnmhdr, plProcessed );
		break;

	default:
		return __super::on_notify_reflect( pnmhdr, plProcessed );
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::OnDispInfo( NMLVDISPINFO *plvdispinfo, long *plProcessed )
{
	if( plvdispinfo->item.mask & LVIF_IMAGE && plvdispinfo->item.lParam )    
	{
		plvdispinfo->item.iImage = get_icon_step( (TPOS)plvdispinfo->item.lParam );        
		*plProcessed = 1;			
	}
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::OnItemChanged( NMLISTVIEW *pnmlv, long *plProcessed )
{
	if( pnmlv->uChanged & LVIF_STATE )
	{
		int nold_check = ((pnmlv->uOldState & LVIS_STATEIMAGEMASK) >> 12),
			nnew_check = ((pnmlv->uNewState & LVIS_STATEIMAGEMASK) >> 12);

		if( !m_block_steps_operation && nold_check && nnew_check && (nnew_check != nold_check) )
		{	// change check state
			CMethodStep step_data;
			step_data.m_bSkipData = true;
			if( get_step_data( &step_data, pnmlv->iItem ) )
			{
				if( (step_data.m_dwFlags & msfChecked) != (nnew_check == 2) )
				{
					if(nnew_check == 2)
						step_data.m_dwFlags |= msfChecked;
					else
						step_data.m_dwFlags &= ~msfChecked;

					set_step_data( &step_data, pnmlv->iItem );
				}
				*plProcessed = 1;
			}
		}
        
		if( !(pnmlv->uOldState & LVIS_SELECTED) && (pnmlv->uNewState & LVIS_SELECTED) )
		{	// activate step
			
			if( !m_block_steps_operation && m_sptr_active_mtd != 0 && pnmlv->iItem >= 0 )
			{
				TPOS lpos_step = 0;
				lpos_step = (TPOS)get_item_data( pnmlv->iItem );
                if( lpos_step )
					m_sptr_active_mtd->SetActiveStepPos( lpos_step );
			}

		}

		if( !(pnmlv->uNewState & LVIS_SELECTED) && (pnmlv->uOldState & LVIS_SELECTED) )
		{	// deactivate step
			if( !m_block_steps_operation && m_sptr_active_mtd != 0 && pnmlv->iItem >= 0 )
			   m_sptr_active_mtd->SetActiveStepPos( 0, FALSE );
		}

	}
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::OnBeginDrag( NMLISTVIEW *pnmlv, long *plProcessed )
{
	if( !pnmlv )
		return;

	// [vanek] SBT:1004 - 21.05.2004
	if( m_sptr_mtd_man == 0 )
		return;
	
	BOOL bmanager_busy = FALSE;
    m_sptr_mtd_man->IsRunning( &bmanager_busy );
	if( bmanager_busy )
		return;	// prevent drag cos running

	m_sptr_mtd_man->IsRecording( &bmanager_busy );
	if( bmanager_busy )
		return;	// prevent drag cos recording

	INamedPropBagPtr sptr_propbag_mtd = m_sptr_active_mtd;
	if( sptr_propbag_mtd != 0 )
	{
		variant_t var;
		sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), var.GetAddress() );
		if( var.vt == VT_I4 && var.lVal == 0 )
			return; // [vanek] : prevent drag cos current method is not editable - 27.10.2004 
	}
	
	start_drag( pnmlv->iItem );
	*plProcessed = 1;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnCustomDraw( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	switch( pnmCustomDraw->nmcd.dwDrawStage )	
	{
	case CDDS_PREPAINT:
		return OnPrePaint( pnmCustomDraw, plProcessed );

	case CDDS_ITEMPREPAINT:
		return OnPrePaintRow( pnmCustomDraw, plProcessed );

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		return OnPrePaintCell( pnmCustomDraw, plProcessed );

	case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:
		return OnPostPaintCell( pnmCustomDraw, plProcessed );

	case CDDS_ITEMPOSTPAINT:
		return OnPostPaintRow( pnmCustomDraw, plProcessed );

	case CDDS_POSTPAINT:
		return OnPostPaint( pnmCustomDraw, plProcessed );

	default :
		*plProcessed = false;
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPrePaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPrePaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPrePaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	if( pnmCustomDraw->nmcd.uItemState & CDIS_FOCUS )
		pnmCustomDraw->nmcd.uItemState &= ~CDIS_FOCUS;   // prevent draw focused rect

	// store current clip region
	if( m_hrgn_old )
		::DeleteObject( m_hrgn_old ); m_hrgn_old = 0;

	DWORD dwstate_item = 0;
	dwstate_item = get_step_state((TPOS)pnmCustomDraw->nmcd.lItemlParam);

	if( ListView_GetItemState( handle(), pnmCustomDraw->nmcd.dwItemSpec, LVIS_SELECTED ) )
	{
		pnmCustomDraw->nmcd.uItemState &= ~CDIS_SELECTED;

		if( !(dwstate_item & (essFirstInLoop | essLastInLoop)) )
		{
			RECT	rtRect = {0};
			if( !ListView_GetItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec, &rtRect, LVIR_LABEL ) )
				return CDRF_DODEFAULT;

			::InflateRect( &rtRect, -2, -2);

			RECT rc_old_rgn_box = {0};
			::GetClipBox( pnmCustomDraw->nmcd.hdc, &rc_old_rgn_box );
			m_hrgn_old = ::CreateRectRgnIndirect( &rc_old_rgn_box );
			::GetClipRgn( pnmCustomDraw->nmcd.hdc, m_hrgn_old );

			::ExcludeClipRect( pnmCustomDraw->nmcd.hdc, rtRect.left, rtRect.top, rtRect.right, rtRect.bottom );
		}
	}

	if( dwstate_item  & essNotInLoop )  
		pnmCustomDraw->clrTextBk = ListView_GetTextBkColor( handle() );
	else
	{
		if( !m_hrgn_old && (dwstate_item & (essFirstInLoop | essLastInLoop)) )
		{
            RECT	rtRect = {0};
			if( !ListView_GetItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec, &rtRect, LVIR_LABEL ) )
				return CDRF_DODEFAULT;

            if( dwstate_item & essFirstInLoop  )
				rtRect.bottom = (int)(rtRect.bottom - (double)(rtRect.bottom - rtRect.top) / 2. + 0.5);
			else if( dwstate_item & essLastInLoop )
				rtRect.top = (int)(rtRect.top + (double)(rtRect.bottom - rtRect.top) / 2. + 0.5);
				

			RECT rc_old_rgn_box = {0};
			::GetClipBox( pnmCustomDraw->nmcd.hdc, &rc_old_rgn_box );
			m_hrgn_old = ::CreateRectRgnIndirect( &rc_old_rgn_box );
			::GetClipRgn( pnmCustomDraw->nmcd.hdc, m_hrgn_old );

			::ExcludeClipRect( pnmCustomDraw->nmcd.hdc, rtRect.left, rtRect.top, rtRect.right, rtRect.bottom );
		}

		pnmCustomDraw->clrTextBk = RGB( 197, 218, 244);
	}

	*plProcessed = TRUE;					    	
	return CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPostPaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
    
	RECT	rtRect = {0};
	if( !ListView_GetItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec, &rtRect, LVIR_LABEL ) )
		return CDRF_DODEFAULT;

	UINT uiselected = 0;
	uiselected = ListView_GetItemState( handle(), pnmCustomDraw->nmcd.dwItemSpec, LVIS_SELECTED ) && 
		!is_bound_loop( (int)pnmCustomDraw->nmcd.dwItemSpec );

	HRGN	hrgn_curr = 0,
		hrgn_adding = 0;

	if( m_hrgn_old )
	{	// restore old region
		::SelectClipRgn( pnmCustomDraw->nmcd.hdc, m_hrgn_old );
		::DeleteObject( m_hrgn_old );
		m_hrgn_old = 0;
	}

	// draw frame
	::InflateRect( &rtRect, -2, 0);
	if( pnmCustomDraw->nmcd.dwItemSpec == m_litem_hot ||  uiselected )
	{
		::InflateRect( &rtRect, 0, -2);

		HPEN hpen = ::CreatePen( PS_SOLID, 1, pnmCustomDraw->nmcd.dwItemSpec == m_litem_hot ? 0 : GetSysColor( COLOR_BTNSHADOW ));
		HPEN hold_pen = (HPEN)::SelectObject( pnmCustomDraw->nmcd.hdc, hpen );

		COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
		COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

		HBRUSH hold_brush = 0;
		HBRUSH hselected_brush = 0;
		if( uiselected )
		{
			hselected_brush = ::CreateSolidBrush( RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
				GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
				GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2)) );

			hold_brush = (HBRUSH)::SelectObject( pnmCustomDraw->nmcd.hdc, hselected_brush );
		}
		else
			hold_brush = (HBRUSH)::SelectObject( pnmCustomDraw->nmcd.hdc, GetStockObject( NULL_BRUSH ) );

		::Rectangle( pnmCustomDraw->nmcd.hdc, rtRect.left, rtRect.top, rtRect.right, rtRect.bottom );

		::SelectObject( pnmCustomDraw->nmcd.hdc, hold_pen);
		::SelectObject( pnmCustomDraw->nmcd.hdc, hold_brush );

		if( hselected_brush )
			::DeleteObject( hselected_brush ); hselected_brush  = 0;

		if( hpen )
			::DeleteObject( hpen ); hpen = 0;
	}

	// draw text
	::InflateRect( &rtRect, -2, -1 );
	CMethodStep step_data;
	step_data.m_bSkipData = true;
	if( get_step_data( &step_data, (int)pnmCustomDraw->nmcd.dwItemSpec ) )
	{
		// [vanek]: не выводим наименование для начала и конца цикла - 11.06.2004
		if( step_data.m_bstrUserName.length() && !is_begin_loop_action( step_data.m_bstrActionName ) && 
			!is_end_loop_action( step_data.m_bstrActionName ) )
		{
			UINT uiformat = DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER;

			int nold_bkmode = 0;
			nold_bkmode = ::SetBkMode( pnmCustomDraw->nmcd.hdc, TRANSPARENT );

			if( !is_cached( (int)pnmCustomDraw->nmcd.dwItemSpec ) )
			{
				COLORREF crOldTextColor = 0;
				//crOldTextColor  = ::SetTextColor( pnmCustomDraw->nmcd.hdc, ::GetSysColor( COLOR_GRAYTEXT ) );
				crOldTextColor  = ::SetTextColor( pnmCustomDraw->nmcd.hdc, RGB( 128, 128, 128) );
				::DrawText( pnmCustomDraw->nmcd.hdc, step_data.m_bstrUserName, 
					step_data.m_bstrUserName.length(), &rtRect, uiformat);

				::SetTextColor( pnmCustomDraw->nmcd.hdc, crOldTextColor );
			}
			else
				::DrawText( pnmCustomDraw->nmcd.hdc, step_data.m_bstrUserName, 
				step_data.m_bstrUserName.length(), &rtRect, uiformat);

			::SetBkMode( pnmCustomDraw->nmcd.hdc, nold_bkmode );
		}
	}      

	return CDRF_SKIPDEFAULT;
}

///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPostPaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_SKIPDEFAULT;
}


///////////////////////////////////////////////////////////////////////
long	CFuncMethodListCtrl::OnPostPaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;

	if( 0 >= get_items_count() )
	{	// indicate empty method
		RECT rc = {0};
		if( ::GetClientRect( handle(), &rc ) )
		{
			rc.left += 2;
			rc.right -= 2;

			CString str_message( _T("") );
			str_message.LoadString( AfxGetResourceHandle(), IDS_METHOD_EMPTY );

			HWND hwnd_header = 0;
			hwnd_header = ListView_GetHeader( handle() );
			if( hwnd_header && ::IsWindowVisible( hwnd_header ) )
			{
				RECT rc_header = {0};
				if( ::GetWindowRect( hwnd_header, &rc_header ) && !::IsRectEmpty( &rc_header ) )
				{
					::ScreenToClient( handle(), (LPPOINT)(&rc_header) );
					::ScreenToClient( handle(), (LPPOINT)(&rc_header) + 1 );
					rc.top = rc_header.bottom;
				}
			}

			TEXTMETRIC tm = {0};
			::GetTextMetrics( pnmCustomDraw->nmcd.hdc, & tm );
			rc.top += tm.tmHeight / 2;

			COLORREF cl_old = 0;
			cl_old = ::SetTextColor( pnmCustomDraw->nmcd.hdc, ::GetSysColor( COLOR_BTNTEXT ) );
			int nold_bkmode = ::SetBkMode( pnmCustomDraw->nmcd.hdc, TRANSPARENT );
			::DrawText( pnmCustomDraw->nmcd.hdc, str_message, str_message.GetLength(), &rc, DT_CENTER | /*DT_WORDBREAK |*/ DT_END_ELLIPSIS | DT_MODIFYSTRING );
			::SetTextColor( pnmCustomDraw->nmcd.hdc, cl_old );
			::SetBkMode( pnmCustomDraw->nmcd.hdc, nold_bkmode );
		}
	}
	else
	{
		// draw loop frame
		draw_loop_frame( pnmCustomDraw->nmcd.hdc );
	}

	return CDRF_SKIPDEFAULT;
}

//////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_rbuttondown(const _point &point)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	int nitem = 0;
	LVHITTESTINFO st_lvhittest = {0};
	st_lvhittest.pt = point;
	nitem = ListView_HitTest( handle( ), &st_lvhittest );
	if( nitem != -1 && (st_lvhittest.flags & LVHT_ONITEM) )
	{
		return 0; // prevent default
	}

	return __super::on_rbuttondown( point );   
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_lbuttondown(const _point &point)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	int nitem = 0;
	LVHITTESTINFO st_lvhittest = {0};
	st_lvhittest.pt = point;
	nitem = ListView_HitTest( handle( ), &st_lvhittest );
	if( nitem != -1 )
	{
		CMethodStep step_data;
		step_data.m_bSkipData = true;
		if( get_step_data( &step_data, nitem ) )
		{
			if( st_lvhittest.flags & LVHT_ONITEM && ( is_begin_loop_action( step_data.m_bstrActionName ) || 
				is_end_loop_action( step_data.m_bstrActionName ) ) )
					return 0; // prevent default (activating item)
			
			if( st_lvhittest.flags == LVHT_ONITEMICON )
			{
				if( (step_data.m_dwFlags & msfHasPropPage) )
				{
					step_data.m_dwFlags ^= msfShowPropPage;
					// [vanek] SBT:1051 - 05.07.2004
					if( set_step_data( &step_data, nitem, true ) )
					{
						// redraw step icon
						RECT rc_item_icon = {0};
						if( ListView_GetItemRect( handle(), nitem, &rc_item_icon, LVIR_ICON ) )
							::InvalidateRect( handle(), &rc_item_icon, FALSE );
					}
				}

				return 0; // prevent default (activating item) 
			}

			// [vanek] : у неубиваемых нет checkbox -> не обрабатываем нажатие - 02.11.2004
			if( st_lvhittest.flags == LVHT_ONITEMSTATEICON && (step_data.m_dwFlags & msfUndead) )
				return 0; // prevent
		}
	}

	return __super::on_lbuttondown( point );
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_lbuttonup(const _point &point)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	end_drag( );
	return __super::on_lbuttonup( point );
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_lbuttondblclk(const _point &point)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	int nitem = 0;
	LVHITTESTINFO st_lvhittest = {0};
	st_lvhittest.pt = point;
	nitem = ListView_HitTest( handle( ), &st_lvhittest );
	if( nitem != -1 && (st_lvhittest.flags & LVHT_ONITEM) )
	{
		return 0; // prevent default
	}

	return __super::on_lbuttondblclk( point );   
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_rbuttondblclk(const _point &point)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	int nitem = 0;
	LVHITTESTINFO st_lvhittest = {0};
	st_lvhittest.pt = point;
	nitem = ListView_HitTest( handle( ), &st_lvhittest );
	if( nitem != -1 && (st_lvhittest.flags & LVHT_ONITEM) )
	{
		return 0; // prevent default
	}

	return __super::on_rbuttondblclk( point );   
}

///////////////////////////////////////////////////////////////////////
void CFuncMethodListCtrl::update_hot_step( LPPOINT ppoint )
{
	POINT point = {0};
	if( ppoint )
		point = *ppoint;
	else
	{
		::GetCursorPos( &point );
		::ScreenToClient( handle(), &point );
	}

	LVHITTESTINFO st_lvhittest = {0};
	st_lvhittest.pt = point;

	long	lold_hot = m_litem_hot,
			lnew_hot = -1;
	lnew_hot = ListView_HitTest( handle( ), &st_lvhittest );

	// [vanek]: предотвращение наведения на начало и конец цикла - 10.06.2004
	if( is_bound_loop( lnew_hot ) )
		lnew_hot = -1;
		
	if( (lnew_hot == -1  || st_lvhittest.flags & LVHT_ONITEM) && lnew_hot != lold_hot )
	{
		m_litem_hot = lnew_hot;
		RECT rc_item_text = {0};
		//ListView_RedrawItems( handle(), m_litem_hot, m_litem_hot );
		if( ListView_GetItemRect( handle(), m_litem_hot, &rc_item_text, LVIR_LABEL ) )
			::InvalidateRect( handle(), &rc_item_text, FALSE );
        
		//ListView_RedrawItems( handle(), lold_hot, lold_hot );
		::SetRectEmpty( &rc_item_text );
		if( ListView_GetItemRect( handle(), lold_hot, &rc_item_text, LVIR_LABEL ) )
			::InvalidateRect( handle(), &rc_item_text, FALSE );
		
	}

}

///////////////////////////////////////////////////////////////////////
void CFuncMethodListCtrl::reset_hot_step( )
{
	if( m_litem_hot == -1 )
		return;

	long lold_hot = m_litem_hot ;
	m_litem_hot = -1;	  // reset hot
	//ListView_RedrawItems( handle(), lold_hot, lold_hot );    
	RECT rc_item_text = {0};
	if( ListView_GetItemRect( handle(), lold_hot, &rc_item_text, LVIR_LABEL ) )
		::InvalidateRect( handle(), &rc_item_text, FALSE );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::set_next_active_step( long lvkey )
{
	if( m_sptr_active_mtd == 0 )
		return FALSE;

    int nitem_sel = -1;
    nitem_sel = ListView_GetNextItem( handle(), -1, LVNI_ALL | LVNI_SELECTED );
	if( nitem_sel == -1 )
		return FALSE;

	
	TPOS lnext_pos = (TPOS)get_item_data(nitem_sel);
	CMethodStep step_data;
    step_data.m_bSkipData = true;
    if( lvkey == VK_DOWN )
	{		
		if( S_OK != m_sptr_active_mtd->GetNextStep( &lnext_pos, 0 ) )
			return FALSE;
		
		TPOS lpos = lnext_pos;
		do
		{
			lnext_pos = lpos;
			if( S_OK != m_sptr_active_mtd->GetNextStep( &lpos, &step_data ) )
				return FALSE;	                      
		}
		while( is_begin_loop_action( step_data.m_bstrActionName ) || 
			is_end_loop_action( step_data.m_bstrActionName ) );
	}
	else if( lvkey == VK_UP )
	{   
        if( S_OK != m_sptr_active_mtd->GetPrevStep( &lnext_pos, 0 ) )
			return FALSE;
		
				TPOS lpos = lnext_pos;
		do
		{
			lnext_pos = lpos;
			if( S_OK != m_sptr_active_mtd->GetPrevStep( &lpos, &step_data ) )
				return FALSE;	                      
		}
		while( is_begin_loop_action( step_data.m_bstrActionName ) || 
			is_end_loop_action( step_data.m_bstrActionName ) );
	}
	
	int nitem_newsel = -1;
    nitem_newsel = find_step( lnext_pos );
	if( nitem_newsel == nitem_sel ) 
		return FALSE;
	    
	set_selected_item( nitem_newsel );
    return TRUE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::start_drag( int nstep_item )
{
	if( nstep_item == -1 )
		return FALSE;

	//HWND hwndActive = ::GetActiveWindow();
	//if( hwndActive!=m_hwnd ) return FALSE; // SBT 1385 - никакого D'n'D, если окно не активно
	// убрал - на самом деле бага из-за MessageBox "Updating step ..."

	if( !m_bdragging )
	{
		// [vanek]: нет drag'n'drop для начала и конца цикла - 11.06.2004
		if( is_bound_loop( nstep_item ) )
				return FALSE;
		
		// [vanek]: начинаем MoveStep - 09.06.2004
		IMethodChangesPtr sptr_mtd_ch = m_sptr_active_mtd;
		if( sptr_mtd_ch == 0 )
			return FALSE;

		// ну, понеслось...
		sptr_mtd_ch->MoveStep( nstep_item, nstep_item, mmscMove );
		
		::SetCapture( handle() );
		::SetFocus( handle() );
		m_nstart_drag_item = m_ndrag_item = nstep_item;
		if( !m_hbefore_drag_cursor )
			m_hbefore_drag_cursor = ::SetCursor( m_hdrag_cursor );

		m_bdragging = TRUE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::end_drag( )
{
	if( m_bdragging )
	{
		IMethodChangesPtr sptr_mtd_ch = m_sptr_active_mtd;
		if( sptr_mtd_ch != 0)
		    sptr_mtd_ch->MoveStep( m_ndrag_item, m_ndrag_item, mmscFinishMove );
		
		m_bdragging = FALSE;
		::ReleaseCapture( );
		m_nstart_drag_item = m_ndrag_item = -1;
		if( m_hbefore_drag_cursor )
			::SetCursor( m_hbefore_drag_cursor );	m_hbefore_drag_cursor = 0;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
int CFuncMethodListCtrl::drag_drop_step( int nstep_item, int ndrop_to )
{
	if( nstep_item == -1 ||  m_sptr_active_mtd == 0 )
		return -1;

	IMethodChangesPtr sptr_mtd_ch = m_sptr_active_mtd;
	if( sptr_mtd_ch == 0)
		return -1;

	return S_OK == sptr_mtd_ch->MoveStep( nstep_item, ndrop_to, mmscContinueMove ) ? ndrop_to : -1;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::synch_lparams( int nitem_from, int nitem_to )
{
	if( m_sptr_active_mtd == 0 )
		return FALSE;

	if( nitem_from > nitem_to )
	{
		int ntemp = nitem_from;
		nitem_from = nitem_to;
		nitem_to = ntemp;
	}

	long lstep_count = 0;
	m_sptr_active_mtd->GetStepCount( &lstep_count );
	nitem_to = min( int(lstep_count), nitem_to );
	TPOS lpos_step = 0;
	int nitem_step = 0;
	for( nitem_step = 0, m_sptr_active_mtd->GetFirstStepPos( &lpos_step ) ; lpos_step && (nitem_step <= nitem_to); 
		m_sptr_active_mtd->GetNextStep( &lpos_step, 0), nitem_step ++ )
	{
		if( nitem_step >= nitem_from )
		{
			LV_ITEM	i = {0};
			i.iItem = nitem_step;
			i.lParam = (LPARAM)lpos_step;	
			i.mask = LVIF_PARAM;
			ListView_SetItem( handle(), &i );            
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::draw_loop_frame( HDC hdc )
{
	if( !hdc || !handle() )
		return FALSE;

	RECT rc_loop = {0};
	if( hdc && calc_loop_rect( &rc_loop ) )
	{
		HPEN	hpen_highlight = 0,
			hpen_shadow = 0,
			hpen_old = 0;

		hpen_highlight = ::CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT) );
		hpen_shadow = ::CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW) );

		// draw "-------------------------------"
		hpen_old = (HPEN)::SelectObject( hdc, hpen_shadow );
		::MoveToEx( hdc, rc_loop.left, rc_loop.top, 0 );
		::LineTo( hdc, rc_loop.right, rc_loop.top );

		// draw "|"
		::MoveToEx( hdc, rc_loop.left, rc_loop.top, 0 );
		::LineTo( hdc, rc_loop.left, rc_loop.bottom );

		// draw     "|"
		::SelectObject( hdc, hpen_highlight );
		::MoveToEx( hdc, rc_loop.right, rc_loop.top, 0 );
		::LineTo( hdc, rc_loop.right, rc_loop.bottom + 1 );

		// draw "________________________________"
		::MoveToEx( hdc, rc_loop.left, rc_loop.bottom, 0 );
		::LineTo( hdc, rc_loop.right + 1, rc_loop.bottom);        

		if( hpen_old )
			::SelectObject( hdc, hpen_old );

		if( hpen_highlight )
			::DeleteObject( hpen_highlight ); hpen_highlight = 0;

		if( hpen_shadow )
			::DeleteObject( hpen_shadow ); hpen_shadow = 0;
	}


	return TRUE;
}

///////////////////////////////////////////////////////////////////////
void	CFuncMethodListCtrl::update_loop( )
{
	RECT rc_loop = {0};
	if( calc_loop_rect( &rc_loop ) )
		::InvalidateRect( handle(), &rc_loop, TRUE );
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_mousemove(const _point &point)
{
	
	{
		TRACKMOUSEEVENT		csTME = {0};
		csTME.cbSize = sizeof( csTME );
		csTME.dwFlags = TME_LEAVE;
		csTME.hwndTrack = handle( );
		::_TrackMouseEvent( &csTME );

		update_hot_step( &(POINT)(point) );
	}
	
	if( m_bdragging )
	{
		int nredraw_from = -1,
			nredraw_to = -1; 

		HCURSOR hcur_to_set = 0;

		
		LVHITTESTINFO st_lvhittest = {0};
		st_lvhittest.pt = point;
		ListView_HitTest( handle(), &st_lvhittest );
        if( (st_lvhittest.flags & LVHT_ONITEM) && (st_lvhittest.iItem >= 0) )
		{
			hcur_to_set = m_hdrag_cursor;   
			if( m_ndrag_item != st_lvhittest.iItem )
			{
				int ndrop_to = st_lvhittest.iItem;		
				int ndrag_item_old = m_ndrag_item;
				long lpos_drag = 0;
				CMethodStep step_data_drag,
					step_data_dropto;

				step_data_drag.m_bSkipData =
					step_data_dropto.m_bSkipData = true;

				get_step_data( &step_data_drag, m_ndrag_item, 0);
				get_step_data( &step_data_dropto, ndrop_to, 0);

				{	// drag'n'drop

                    // turn off redraw
					set_redraw( FALSE );
                    
					m_ndrag_item = drag_drop_step( m_ndrag_item, ndrop_to );	

					// turn on redraw
					set_redraw( TRUE );
				}
			}
		}		
		else 
			hcur_to_set = m_hnodrag_cursor;
		
		// set cursor
        if( hcur_to_set  )
			::SetCursor( hcur_to_set );
		
		// redraw items
		if( nredraw_from != -1 && nredraw_to != -1 )
			ListView_RedrawItems( handle(), nredraw_from, nredraw_to );

		return 0;
	}
	
	return __super::on_mousemove( point );
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_destroy()
{
	return __super::on_destroy();
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_keydown(long nVirtKey)
{
	if(m_block_ui) return 0; // все юзерские действия запрещены

	switch( nVirtKey )
	{
	case VK_UP:
	case VK_DOWN:
		set_next_active_step( nVirtKey );
		return  0;
	}


	return __super::on_keydown( nVirtKey );
}

///////////////////////////////////////////////////////////////////////
LRESULT CFuncMethodListCtrl::on_killfocus(HWND hwndOld)
{
	// [vanek] SBT:1132 - 16.09.2004
	if( m_bdragging )
		end_drag( );
    
	return __super::on_killfocus( hwndOld );
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::is_begin_loop_action( BSTR bstrActionName )
{
	return bstrActionName ? _bstr_t( bstrActionName ) == _bstr_t( szBeginMethodLoop ) : FALSE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::is_end_loop_action( BSTR bstrActionName )
{
	return bstrActionName ? _bstr_t( bstrActionName ) == _bstr_t( szEndMethodLoop ) : FALSE;
}

///////////////////////////////////////////////////////////////////////
BOOL	CFuncMethodListCtrl::is_bound_loop( int nitem_step )
{
	if( m_sptr_active_mtd == 0 )
		return FALSE;

	CMethodStep step_data;
	step_data.m_bSkipData = true;
	if( !get_step_data( &step_data, nitem_step ) )
		return FALSE;

    return is_begin_loop_action( step_data.m_bstrActionName ) || is_end_loop_action( step_data.m_bstrActionName );
}