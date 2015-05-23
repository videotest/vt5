#include "StdAfx.h"
#include "vlistctrl.h"


CVListCtrl::CVListCtrl(void)
{
	// initialisation list-view
	INITCOMMONCONTROLSEX icex = {0};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	ResetSelection( );

	m_hfHeaderFont = 0;
	m_crHeaderText = (COLORREF) -1;

	m_lHeightClientArea = 0;
	m_lWidthClientArea = 0;
	m_bDrawGridLines = FALSE;
	m_bEnableMultiSelection = FALSE;
	m_bEnableDragDrop = FALSE;

	m_nHScrollState = m_nVScrollState = -1;

	m_bHScrollbarRealHide = m_bVScrollbarRealHide = FALSE;
	m_bLockForeignHSB = m_bLockForeignVSB = FALSE;
	m_bUpdateForeignHSB = m_bUpdateForeignVSB = TRUE;

	m_bHideScrollbarsOnVisibleAllRows = FALSE;
}

CVListCtrl::~CVListCtrl(void)
{

}

LPARAM	CVListCtrl::get_lparam( int iItem )
{
	if( !m_hwnd )
		return 0;
	// получение структуры LPARAM
	HDITEM stHdItem = {0};
	stHdItem.mask = HDI_LPARAM;
	if( !Header_GetItem( ListView_GetHeader( m_hwnd ), iItem, &stHdItem ) )
		return 0;
	return stHdItem.lParam;
}

BOOL	CVListCtrl::need_paint_cell( const RECT *pCellRect )
{
	if( !pCellRect || !m_hwnd )
		return FALSE;

	if( pCellRect->left == pCellRect->right )
		// ширина ячейки = 0
		return FALSE; 

	if( ( pCellRect->right < 0 ) || ( pCellRect->left > m_lWidthClientArea ) ||
		( pCellRect->bottom < 0 ) || ( pCellRect->top > m_lHeightClientArea ) )
		return FALSE;
	else
		return TRUE;
}

BOOL		CVListCtrl::set_foreign_scrollbar_info( int iBar )
{	// iBar: SB_HORZ, SB_VERT

	if( !handle( ) )
		return FALSE;

	HWND hwndSB = 0;
	hwndSB = GetScrollbar( iBar );
	if( !hwndSB )
		return FALSE;

	if( (iBar == SB_HORZ) && (/*m_bLockForeignHSB ||*/ !m_bUpdateForeignHSB) ) 
		return FALSE;
    
    if( (iBar == SB_VERT) && (/*m_bLockForeignVSB ||*/ !m_bUpdateForeignVSB)  ) 
		return FALSE;

	SCROLLINFO si = {0};
	si.cbSize = sizeof( SCROLLINFO );
	
	si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
	
	if( !::GetScrollInfo( handle( ), iBar, &si ) )
		return FALSE;

    {
		if( iBar == SB_VERT )
		{
			RECT rtItem = {0};
			if( ListView_GetItemRect( handle(), ListView_GetTopIndex( handle() ), &rtItem, LVIR_BOUNDS ) )
				si.nPage = (UINT)max( 0, ((double)m_lHeightClientArea - rtItem.top) / max(1,(rtItem.bottom - rtItem.top)) );

			if( (m_vert_sb.nMin == si.nMin) && (m_vert_sb.nMax == si.nMax) && 
				(m_vert_sb.nPage == si.nPage) && ( m_vert_sb.nPos == si.nPos ) )
				return FALSE;
		
			m_vert_sb = si;
		}
		else if( iBar == SB_HORZ )
		{
			si.nPage = max( 0, m_lWidthClientArea );
	
			if( (m_horz_sb.nMin == si.nMin) && (m_horz_sb.nMax == si.nMax) &&
				( m_horz_sb.nPage == si.nPage ) && ( m_horz_sb.nPos == si.nPos ) )
				return FALSE;

			m_horz_sb = si;
		}
		si.fMask |= SIF_PAGE;

		bool	bDisable = ( (si.nMax - int(si.nPage)) < 0 ) || ( si.nMax == int(si.nPage) && si.nMax == 0 );
        if( bDisable )
		{
			si.fMask += SIF_DISABLENOSCROLL;
			//::SetScrollInfo( hwndSB, SB_CTL, &si, TRUE );           
			::SendMessage( hwndSB, SBM_SETSCROLLINFO, true, (LPARAM)&si );
			::EnableWindow( hwndSB, FALSE );		
		}
		else
		{
			::EnableWindow( hwndSB, TRUE );
			::SendMessage( hwndSB, SBM_SETSCROLLINFO, true, (LPARAM)&si );
		}

		// for test
		//char trace_str[ MAX_PATH ];
		//sprintf( trace_str, "set_foreign_scrollbar_info: min=%i, max =%i, page = %i, pos = %i, mask = %i, SBcode = %i\r\n", si.nMin, si.nMax, si.nPage, si.nPos, si.fMask, iBar );
		//trace( trace_str );
		//
	}
	
	return TRUE;
}

void	CVListCtrl::redirect_scrollbar( int iBar, int iCode, unsigned uPos, xDirection dir /*= XD_Forward*/ )
{
	{
     	switch( dir )
		{
		case XD_Forward:
			{
				HWND hwndSB = GetScrollbar( iBar );
				
				if( hwndSB && ( (iBar == SB_VERT) || (iBar == SB_HORZ) ) )
				{
					SCROLLINFO si_foreign = {0};
					si_foreign.cbSize = sizeof( SCROLLINFO );
					si_foreign.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
					::GetScrollInfo( hwndSB, SB_CTL, &si_foreign );
					switch( iCode )
					{
					case SB_LINEDOWN:
					case SB_PAGEDOWN:
					case SB_BOTTOM:
						if( (si_foreign.nPos + si_foreign.nPage) > (UINT)si_foreign.nMax )
								return;
					}
                }  

				LRESULT iRetVal = 0;
				switch( iBar )
				{
				case SB_HORZ:
					iRetVal = __super::on_hscroll( iCode, uPos, 0 );
					break;
				case SB_VERT:
					iRetVal = __super::on_vscroll( iCode, uPos, 0 );
					break;
				}

				if( !hwndSB )
					return;
				
				int fnBar = SB_CTL,
					nPosForeign = ::GetScrollPos( hwndSB, fnBar ),
                    nPos = ::GetScrollPos( handle( ), iBar );

				if( nPos != nPosForeign )
				{
					nPosForeign = nPos;
					::SetScrollPos( hwndSB, fnBar, nPosForeign, TRUE );
				}
			}
			break;
			

		case XD_Backward:
			{
				HWND hwndSB = GetScrollbar( iBar );
				if( !hwndSB )
				{
					switch( iBar )
					{
					case SB_HORZ:
						__super::on_hscroll( iCode, uPos, 0 );
						break;
					case SB_VERT:
						__super::on_vscroll( iCode, uPos, 0 );
						break;
					}

					return;
				}

				int		iX = 0, iY = 0;
				int		iNewPositionForeignSB = 0,
						iPositionForeignSB = 0, 
						iPositionOwnSB = 0;
				
				SCROLLINFO si_foreign = {0};
				si_foreign.cbSize = sizeof( SCROLLINFO );
				si_foreign.fMask = SIF_ALL;
				if( !::GetScrollInfo( hwndSB, SB_CTL, &si_foreign ) )
					return;

				SCROLLINFO si_own = {0};
				si_own.cbSize = sizeof( SCROLLINFO );
				si_own.fMask = SIF_ALL;
				if( !::GetScrollInfo( handle(), iBar, &si_own ) )
					return;

				iPositionOwnSB = si_own.nPos;
                if( (iCode/256)==SB_THUMBTRACK || (iCode & 0xFF)==SB_THUMBTRACK || 
					(iCode/256==SB_THUMBPOSITION) || (iCode & 0xFF)==SB_THUMBPOSITION )
				    iPositionForeignSB = si_foreign.nTrackPos;
				else if( iCode == SB_LINEDOWN )
					iPositionForeignSB = si_foreign.nPos + 1;
				else if( iCode == SB_LINEUP )
					iPositionForeignSB = si_foreign.nPos - 1;
				else if( iCode == SB_PAGEDOWN || iCode == SB_PAGEUP )
				{
                    switch ( iCode )
					{
					case SB_PAGEDOWN:
						iPositionForeignSB  = si_foreign.nPos + (int)si_foreign.nPage;
						break;
					case SB_PAGEUP:
						iPositionForeignSB  = si_foreign.nPos - (int)si_foreign.nPage;
						break;    
					}
				}
				else if( iCode == SB_RIGHT )
				{
					iPositionForeignSB = si_foreign.nMax - (int)si_foreign.nPage + 1;
				}
				else if( iCode == SB_ENDSCROLL )
				{
					switch( iBar )
					{
					case SB_HORZ:
						__super::on_hscroll( iCode, uPos, 0 );
						break;
					case SB_VERT:
						__super::on_vscroll( iCode, uPos, 0 );
						break;
					}

					return;
				}

				if( iPositionForeignSB > (si_foreign.nMax - (int)si_foreign.nPage + 1) )
					iPositionForeignSB =  si_foreign.nMax - (int)si_foreign.nPage + 1;
				
				if( iBar == SB_HORZ )
					iX = iPositionForeignSB - si_foreign.nPos;
				else if( iBar == SB_VERT )
				{
                    if( 1 < GetRowCount( ) )
					{
						RECT rt;
						if( ListView_GetItemRect( handle( ), 0, &rt, LVIR_LABEL ) )
							iY = (iPositionForeignSB - si_foreign.nPos)* (rt.bottom - rt.top); 
					}
				}

				/*BOOL	bOldLockForeignHSB = m_bLockForeignHSB,
						bOldLockForeignVSB = m_bLockForeignVSB;

				m_bLockForeignHSB = (iX != 0);
				m_bLockForeignVSB = (iY != 0);
                if( m_bLockForeignHSB || m_bLockForeignVSB )*/
				if( (iX != 0) || (iY != 0) )
					ListView_Scroll( handle( ), iX, iY );	// scroll list
				                
				/*m_bLockForeignHSB = bOldLockForeignHSB;
				m_bLockForeignVSB = bOldLockForeignVSB;

				set_foreign_scrollbar_info( iBar );*/
			}
			break;
        }
	}
}


BOOL	CVListCtrl::modify_windows_style( DWORD dwRemove, int nStyleOffset, DWORD dwAdd, UINT nFlags )
{
	if( !handle( ) )
		return FALSE;

	DWORD dwStyle = ::GetWindowLong( handle( ), nStyleOffset);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return FALSE;

	::SetWindowLong( handle( ), nStyleOffset, dwNewStyle);
	if (nFlags != 0)
	{
		::SetWindowPos( handle( ), NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return TRUE;
}

BOOL	CVListCtrl::modify_listview_style_ex( DWORD dwRemove, DWORD dwAdd, UINT nFlags )
{
	if( !handle( ) )
		return FALSE;

	DWORD dwLVStyleEx = 0, dwLVNewStyleEx = 0;
	dwLVStyleEx = ListView_GetExtendedListViewStyle( handle( ) );
	dwLVNewStyleEx = (dwLVStyleEx & ~dwRemove) | dwAdd;
  	if( dwLVNewStyleEx == dwLVStyleEx )
		return FALSE;

	ListView_SetExtendedListViewStyle( handle( ), dwLVNewStyleEx );
	if (nFlags != 0)
	{
		::SetWindowPos( handle( ), NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return TRUE;
}

void	CVListCtrl::update_foreign_scrollbars()
{
	// update foreign scroll bars
	int iBar = SB_HORZ, 
		iX = 0,
		iY = 0;

	HWND hwndSB = 0;
	int nOwnPos = 0;
	SCROLLINFO si_foreign = {0};

	if( !m_bLockForeignHSB )
	{
		if( set_foreign_scrollbar_info( iBar ) || !m_bUpdateForeignHSB )
		{
			si_foreign.cbSize = sizeof( SCROLLINFO );
			si_foreign.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
			
			hwndSB = GetScrollbar( iBar );
			if( hwndSB && ::GetScrollInfo( hwndSB, SB_CTL, &si_foreign ) )
			{
				nOwnPos = ::GetScrollPos( handle(), iBar );
							
				//if( ( (int)si_foreign.nPage < si_foreign.nMax ) && (si_foreign.nMax != 0) &&
				if( (si_foreign.nMax != 0) &&
					( nOwnPos > (si_foreign.nMax - (int)si_foreign.nPage + 1) ) )
				{
					iX = (si_foreign.nMax - (int)si_foreign.nPage + 1) - nOwnPos;         	
					
					// for test
					//char trace_str[ MAX_PATH ];
					//sprintf( trace_str, "update_foreign_scrollbars: iX = %i, si_foreign.nMax = %i, si_foreign.nPage = %i, nOwnPos = %i, SBcode = %i\r\n", iX, si_foreign.nMax, si_foreign.nPage, nOwnPos, iBar );
					//trace( trace_str );
					//
				}
			}
		}
	}

	iBar = SB_VERT;
	if( !m_bLockForeignVSB )
	{
		if( set_foreign_scrollbar_info( iBar ) || !m_bUpdateForeignVSB )
		{
			hwndSB = GetScrollbar( iBar );
			::ZeroMemory( &si_foreign, sizeof(SCROLLINFO) );
			si_foreign.cbSize = sizeof( SCROLLINFO );
			si_foreign.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;

			if( hwndSB && ::GetScrollInfo( hwndSB, SB_CTL, &si_foreign ) )
			{
				nOwnPos = ::GetScrollPos( handle(), iBar );
				
				//if(  ( (int)si_foreign.nPage < si_foreign.nMax ) && (si_foreign.nMax != 0) &&
				if( (si_foreign.nMax != 0) &&
					( nOwnPos > ( si_foreign.nMax - (int)si_foreign.nPage + 1) ) )
				{
					RECT rt;
					if( (0 < GetRowCount()) && ListView_GetItemRect( handle( ), 0, &rt, LVIR_LABEL ) )
						iY = ( (si_foreign.nMax - (int)si_foreign.nPage + 1) - nOwnPos ) * (rt.bottom - rt.top); 
				}  
			}
		}
	}

	BOOL	bOldLockForeignHSB = m_bLockForeignHSB,
			bOldLockForeignVSB = m_bLockForeignVSB;

	m_bLockForeignHSB = (iX != 0) && !m_bLockForeignHSB;
	m_bLockForeignVSB = (iY != 0) && !m_bLockForeignVSB;
	if( m_bLockForeignHSB || m_bLockForeignVSB ){
		//ListView_Scroll( handle( ), iX, iY );
		//if(iX)
		//	if(HWND hHorSB = GetScrollbar(SB_HORZ))
		//	{
		//		SCROLLINFO si = {sizeof SCROLLINFO,SIF_POS};
		//		GetScrollInfo(hwndSB,SB_CTL,&si);
		//		SetScrollInfo(hwndSB,SB_CTL,&si,TRUE);
		//	}

		if(iX)
			if(HWND hHorSB = GetScrollbar(SB_HORZ))
			{
				SCROLLINFO si = {sizeof SCROLLINFO,SIF_POS};
				GetScrollInfo(hwndSB,SB_CTL,&si);
				SetScrollInfo(hwndSB,SB_CTL,&si,TRUE);
				{
					si.nPos += iX;
					::SendMessage( handle(), WM_HSCROLL, (WPARAM)MAKELONG( SB_THUMBPOSITION, si.nPos ), (LPARAM)hwndSB );
				}
			}
	}
		
	m_bLockForeignHSB = bOldLockForeignHSB;
	m_bLockForeignVSB = bOldLockForeignVSB;
	
	return;
}

BOOL	CVListCtrl::CalcNewCellPos( int *piRow, int *piColumn, long lVKey )
{	// вычисление новой позиции ячейки
	BOOL	bProcessed = FALSE;

	if( !handle( ) )
		return FALSE;

	switch( lVKey )
	{
	case VK_LEFT:
		*piColumn  = GetNextVisibleColumn( *piColumn, DIR_LEFT );
		bProcessed = TRUE;
		break;
	case VK_RIGHT:
		*piColumn  = GetNextVisibleColumn( *piColumn, DIR_RIGHT );
		bProcessed = TRUE;
		break;
	case VK_UP:
		if( *piRow > 0 )
			(*piRow) --;
		bProcessed = TRUE;
		break;
	case VK_DOWN:
		if( ( *piRow < GetRowCount( ) - 1 ) && ( *piRow >= 0 ) )
			(*piRow) ++;
		bProcessed = TRUE;
		break;
	case VK_END:
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			*piColumn = GetNextVisibleColumn( GetColumnCount( ) - 1, DIR_LEFT, TRUE );
		else
			*piRow = GetRowCount( ) - 1;

		bProcessed = TRUE;
		break;
	case VK_HOME:
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			*piColumn = GetNextVisibleColumn( 1, DIR_RIGHT, TRUE );
		else
        	*piRow = 0;
		bProcessed = TRUE;
		break;
	default:
		bProcessed = FALSE;
	}

	return bProcessed;
}

bool CVListCtrl::create( DWORD style, const RECT &rect, const _char *pszTitle /*= 0*/, HWND parent /*= 0*/, HMENU hmenu /*= 0*/, const _char *pszClass /*= 0*/ )
{
	return create_ex( style, rect, pszTitle, parent, hmenu );
}

bool CVListCtrl::create_ex( DWORD style, const RECT &rect, const _char *pszTitle /*= 0*/, HWND parent /*= 0*/, HMENU hmenu /*= 0*/, const _char *pszClass /*= 0*/, DWORD ex_style /*= 0*/ )
{
	//style |= ( WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_ALIGNLEFT | LVS_REPORT | LVS_OWNERDATA );
	style |= ( WS_CHILDWINDOW | WS_CLIPSIBLINGS | LVS_ALIGNLEFT | LVS_REPORT | LVS_OWNERDATA );
	style &= ~WS_CLIPCHILDREN;	// удаление стиля WS_CLIPCHILDREN, т.к. данный стиль приводит к ситуации, когда заголовок не перерисовывается

	ex_style |= ( WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY );
	return win_impl::create_ex( style, rect, pszTitle, parent, hmenu, WC_LISTVIEW, ex_style );
}

BOOL	CVListCtrl::ModifyStyle( DWORD dwRemove, DWORD dwAdd, UINT nFlags /*= 0*/ )
{
	return modify_windows_style( dwRemove, GWL_STYLE, dwAdd, nFlags );
}

BOOL	CVListCtrl::ModifyStyleEx( DWORD dwRemove, DWORD dwAdd, BOOL bWorksWithLVExStyles /*= TRUE*/, UINT nFlags /*= 0*/ )
{
	if( bWorksWithLVExStyles )
		return modify_listview_style_ex( dwRemove, dwAdd, nFlags );
	else
		return modify_windows_style( dwRemove, GWL_EXSTYLE, dwAdd, nFlags );
}

BOOL	CVListCtrl::SetBackColor( COLORREF clrBk )
{
	if( !handle( ) )
		return FALSE;

	return ListView_SetBkColor( handle( ), clrBk );
}

BOOL	CVListCtrl::GetBackColor( COLORREF *pclrBk )
{
	if( !handle( ) || !pclrBk )
		return FALSE;

	*pclrBk = ListView_GetBkColor( handle( ) );
	return TRUE;
}

bool CVListCtrl::subclass( HWND hwnd, bool f_nccreate /*= false*/ )
{
	bool bRes = __super::subclass( hwnd, f_nccreate );
	EnableDragDrop( m_bEnableDragDrop );

	DWORD dwRemove = 0, dwAdd = 0;
    if( m_bEnableMultiSelection )
		dwRemove = LVS_SINGLESEL;
	else
		dwAdd = LVS_SINGLESEL;
	ModifyStyle( dwRemove, dwAdd );

	if( bRes && handle( ) )
	{
		RECT rtClient = {0};
		if( ::GetClientRect( handle( ), &rtClient ) )
		{
			m_lHeightClientArea = (rtClient.bottom - rtClient.top);
			m_lWidthClientArea = (rtClient.right - rtClient.left);
		}
	}

    return bRes;
}

LRESULT	CVListCtrl::handle_message(UINT m, WPARAM w, LPARAM l)
{
	LRESULT lRes = 1;

	switch( m )
	{
	case WM_WINDOWPOSCHANGED:
		lRes = OnWindowPosChanged( (LPWINDOWPOS) l );
		break;

	case WM_NCCALCSIZE:
		lRes = OnNcCalcSize( w, l );
		break;

	case LVM_INSERTCOLUMN:
		lRes = OnInsertColumn( w, (LPLVCOLUMN) l);		
		break;

	case LVM_DELETECOLUMN:
		lRes = OnDeleteColumn( w );
		break;

	case LVM_INSERTITEM:
		lRes = OnInsertRow( (LPLVITEM) l );
		break;

	case LVM_DELETEITEM:
		lRes = OnDeleteRow( w );
		break;

	case LVM_DELETEALLITEMS:
		lRes = OnDeleteAllRows( );
		break;

	case 0x020A:  // WM_MOUSEWHEEL	
		{
			POINT pt = {GET_X_LPARAM(l),GET_Y_LPARAM(l)};
			lRes = OnMouseWheel( LOWORD(w), HIWORD(w), pt);
		}
		break;

	default:
		lRes = __super::handle_message( m, w, l );
	}
	
	return lRes;
}

LRESULT CVListCtrl::on_create( CREATESTRUCT *pcs )
{
	LRESULT lRes = __super::on_create(pcs);
	EnableDragDrop( m_bEnableDragDrop );
	
	DWORD dwRemove = 0, dwAdd = 0;
	if( m_bEnableMultiSelection )
		dwRemove = LVS_SINGLESEL;
	else
		dwAdd = LVS_SINGLESEL;
	ModifyStyle( dwRemove, dwAdd );

	return lRes;
}

LRESULT CVListCtrl::on_hscroll( unsigned code, unsigned pos, HWND hwndScroll )
{	
	xDirection dir = ( hwndScroll && (hwndScroll != handle( )) ) ? XD_Backward : XD_Forward;
    redirect_scrollbar( SB_HORZ, code, pos, dir );
    return 0;					   
}

LRESULT CVListCtrl::on_vscroll(unsigned code, unsigned pos, HWND hwndScroll)
{
	xDirection dir = ( hwndScroll && (hwndScroll != handle( )) ) ? XD_Backward : XD_Forward;
	redirect_scrollbar( SB_VERT, code, pos, dir );
	return 0;
}

LRESULT	CVListCtrl::OnWindowPosChanged(LPWINDOWPOS lpwp)
{
	LRESULT lRes = __super::handle_message(WM_WINDOWPOSCHANGED, 0, (LPARAM)lpwp);
    update_foreign_scrollbars( );
    return lRes;
}

long	CVListCtrl::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
	//int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;
	int nRowsScrolled = zDelta / 120;

	HWND hwndSB = 0, hwndReceiver = 0;
	hwndSB = GetScrollbar( SB_VERT );
	hwndReceiver = hwndSB ? ::GetParent( hwndSB ) : handle();

	if (nRowsScrolled > 0)
	{
		for (int i = 0; i < nRowsScrolled; i++)
			::PostMessage( hwndReceiver, WM_VSCROLL, SB_LINEUP, (LPARAM ) hwndSB );		
	}
    else
	{
        for (int i = 0; i > nRowsScrolled; i--)
			::PostMessage( hwndReceiver, WM_VSCROLL, SB_LINEDOWN, (LPARAM) hwndSB );
	}

	return 0;
}	   

long	CVListCtrl::OnNcCalcSize( WPARAM w, LPARAM l )
{
	if( !m_nHScrollState )
		ModifyStyle( WS_HSCROLL , 0 );
		
	if( !m_nVScrollState )
		ModifyStyle( WS_VSCROLL, 0 );
	
	return __super::handle_message( WM_NCCALCSIZE , w, l);
}

LRESULT	CVListCtrl::OnInsertColumn(int iCol, const LPLVCOLUMN pcol)
{
	LRESULT lRes = __super::handle_message( LVM_INSERTCOLUMN, iCol, (LPARAM) pcol );
	if( lRes == -1 )
		return lRes;

	// формирование поумолчанию дополнительных параметров столбца
	ListColumnAdditionInfo *pLCAInfo = 0;
	pLCAInfo = new ListColumnAdditionInfo;
	if( GetSysLogFont( &pLCAInfo->lfFont, ANSI_VAR_FONT ) )
	{
		pLCAInfo->crBkColor = GetSysColor( COLOR_WINDOW );
		pLCAInfo->crForeColor = GetSysColor( COLOR_WINDOWTEXT );
		pLCAInfo->fmt = DT_LEFT;
		//pLCAInfo->text_copy( "" );
		pLCAInfo->iVisible = ( lRes == 0 ) ? FALSE : TRUE;
		pLCAInfo->uiMask = LSTINF_BKCOLOR | LSTINF_FORECOLOR | LSTINF_FONT | LSTINF_FORMAT | LSTINF_VISIBLE;
        HDITEM hdC = {0};
		hdC.mask = HDI_LPARAM;
		hdC.lParam = (LPARAM) pLCAInfo;
		if( !Header_SetItem( ListView_GetHeader(m_hwnd), lRes, &hdC ) )
		{
			delete pLCAInfo;	pLCAInfo = 0;
			ListView_DeleteColumn( m_hwnd, lRes ); // удаление столбца
			return -1;	
		}
	}
	return lRes;
}

LRESULT	CVListCtrl::OnDeleteColumn(int iCol)
{
	HDITEM hdC = {0};
    // получение указателя на дополнительные
	hdC.mask = HDI_LPARAM;
	if( Header_GetItem( ListView_GetHeader( handle( ) ), iCol, &hdC ) )
		if( hdC.lParam )
			delete (ListColumnAdditionInfo*) hdC.lParam;
	return __super::handle_message( LVM_DELETECOLUMN, iCol, 0 );
}

LRESULT	CVListCtrl::OnInsertRow(const LPLVITEM prow)
{
	return __super::handle_message( LVM_INSERTITEM, 0, (LPARAM) prow );
}

long	CVListCtrl::OnDeleteRow( int iRow )
{
	return __super::handle_message( LVM_DELETEITEM, iRow, 0 );
}

long	CVListCtrl::OnDeleteAllRows( )
{
	return __super::handle_message( LVM_DELETEALLITEMS, 0 , 0 );
}
	
BOOL	CVListCtrl::GetSysLogFont( LOGFONT *plfFont, int iParam )
{
	if( !plfFont )
		return FALSE;

	HFONT oldFont = 0, hfont = 0;

	ZeroMemory( plfFont, sizeof( *plfFont ) );
	hfont = (HFONT) GetStockObject( iParam );
	
	if( hfont == NULL )
		return FALSE;

	HDC hdc = GetDC( m_hwnd );

	oldFont = (HFONT) SelectObject( hdc, hfont );

	TEXTMETRIC tm = {0};

	GetTextMetrics( hdc, &tm );
	
	char facename[50];
	
	plfFont->lfHeight = -MulDiv( (MulDiv( ( tm.tmHeight - tm.tmInternalLeading ), 72, GetDeviceCaps(hdc, LOGPIXELSY) )), GetDeviceCaps(hdc, LOGPIXELSY), 72 );
	GetTextFace( hdc, sizeof( facename ) / sizeof( char ), facename);
	strcpy( plfFont->lfFaceName, facename);
	plfFont->lfItalic = tm.tmItalic;
	plfFont->lfUnderline = tm.tmUnderlined;
	plfFont->lfPitchAndFamily = tm.tmPitchAndFamily;
	SelectObject( hdc, oldFont );
	ReleaseDC( m_hwnd, hdc );
	return TRUE;
}

void	CVListCtrl::GetSelection( int *piRowBegin, int *piRowEnd, int *piColumnBegin, int *piColumnEnd )
{
	if( piRowBegin )
		*piRowBegin = m_stSelection.iRowBegin;
	if( piRowEnd )
		*piRowEnd = m_stSelection.iRowEnd;
	if( piColumnBegin )
		*piColumnBegin = m_stSelection.iColumnBegin;
	if( piColumnEnd )
		*piColumnEnd = m_stSelection.iColumnEnd;
}

void	CVListCtrl::SetSelection( int iRowBegin, int iRowEnd, int iColumnBegin, int iColumnEnd )
{
	/*m_stSelection.iRowBegin = ( iRowBegin > (-1) ) ? iRowBegin : -1;
	m_stSelection.iRowEnd = ( iRowEnd > (-1) ) ? iRowEnd : -1;
	m_stSelection.iColumnBegin = ( iColumnBegin > (-1) ) ? iColumnBegin : -1;
	m_stSelection.iColumnEnd = ( iColumnEnd > (-1) ) ? iColumnEnd : -1;*/
	m_stSelection.iRowBegin = ( iRowBegin > xSelectionInfo::xsNoSelected ) ? iRowBegin : xSelectionInfo::xsNoSelected;
	m_stSelection.iRowEnd = ( iRowEnd > xSelectionInfo::xsNoSelected ) ? iRowEnd : xSelectionInfo::xsNoSelected;
	m_stSelection.iColumnBegin = ( iColumnBegin > xSelectionInfo::xsNoSelected ) ? iColumnBegin : xSelectionInfo::xsNoSelected;
	m_stSelection.iColumnEnd = ( iColumnEnd > xSelectionInfo::xsNoSelected ) ? iColumnEnd : xSelectionInfo::xsNoSelected;
}

BOOL	CVListCtrl::IsSelected( int iRow, int iColumn )
{
	/*if( ( iRow < 0 ) && ( iColumn <= 0 ) )
		return FALSE; 

	m_stSelection.arrange( );

	if(	  ( ( iRow <= m_stSelection.iRowEnd )  && ( iColumn <= m_stSelection.iColumnEnd ) && 
			( iRow >= m_stSelection.iRowBegin ) && (iColumn >= m_stSelection.iColumnBegin ) ) ||
			( ( m_stSelection.iRowBegin == m_stSelection.iRowEnd ) && ( m_stSelection.iRowEnd == -1) && ( iColumn <= m_stSelection.iColumnEnd ) && ( iColumn >= m_stSelection.iColumnBegin ) ) ||
			( ( m_stSelection.iColumnBegin == m_stSelection.iColumnEnd ) && ( m_stSelection.iColumnBegin == -1 ) && ( iRow <= m_stSelection.iRowEnd ) && ( iRow >= m_stSelection.iRowBegin ) )
		)
		return TRUE;
	else 
		return FALSE;*/

	return m_stSelection.InSelection( iRow, iColumn );
}

BOOL	CVListCtrl::IsFocused( int iRow, int iColumn )
{
	if( ( iRow == m_stFocused.iRow ) && ( iColumn == m_stFocused.iColumn ) )
		return TRUE;
	else
		return FALSE;
}

void	CVListCtrl::SetFocused( int iRow, int iColumn )
{
	if( m_bEnableMultiSelection )
	{	// multi selection

	}
	else
	{	// single selection
		xSelectionInfo stOldSel = m_stSelection;
		m_stFocused.iRow = iRow;
		m_stFocused.iColumn = iColumn;
		m_stSelection.iColumnBegin = m_stSelection.iColumnEnd = iColumn;
		m_stSelection.iRowBegin = m_stSelection.iRowEnd = iRow;
		EnsureVisibleCell( m_stFocused.iRow, m_stFocused.iColumn );
		if( !handle() )
			return;
		if( stOldSel != m_stSelection )
		{
			stOldSel.Redraw( handle( ) );
			m_stSelection.Redraw( handle( ) );
		}
	}
}

void	CVListCtrl::GetFocused( int *piRow, int *piColumn )
{
	if( piRow )
		*piRow = m_stFocused.iRow;
    
	if( piColumn )
		*piColumn = m_stFocused.iColumn;
}

void	CVListCtrl::EnsureVisibleFocused( )
{
	if( ExistFocused( ) )
		EnsureVisibleCell( m_stFocused.iRow, m_stFocused.iColumn );
}

BOOL	CVListCtrl::ExistFocused( )
{
	//if( ( m_stFocused.iRow > -1 ) && ( m_stFocused.iColumn > 0) )
	if( ( m_stFocused.iRow > xFocusedInfo::xsNoSelected ) && ( m_stFocused.iColumn > 0) )
		return TRUE;
	else
		return FALSE;
}

BOOL	CVListCtrl::GetFocusedCellProp( ListCellInfo *pLCInfo )
{
	int iRow = 0,
		iColumn = 0;
	GetFocused( &iRow, &iColumn );
	return GetCellProp( iRow, iColumn, pLCInfo );
}

BOOL	CVListCtrl::SetFocusedCellProp( const ListCellInfo *pLCInfo )
{
	int iRow = 0,
		iColumn = 0;
	GetFocused( &iRow, &iColumn );
	return SetCellProp( iRow, iColumn, pLCInfo );
}

void	CVListCtrl::ResetSelection( )
{
	/*m_stSelection.iColumnBegin = -1;
	m_stSelection.iColumnEnd = -1;
	m_stSelection.iRowBegin = -1;
	m_stSelection.iRowEnd = -1;*/

	m_stSelection.init( );
}

BOOL	CVListCtrl::ExistSelections( )
{
	//return ( -4 == (m_stSelection.iColumnBegin + m_stSelection.iColumnEnd + m_stSelection.iRowBegin + m_stSelection.iRowEnd ) ) ? FALSE : TRUE;
	return m_stSelection.IsExist( );
}

int	CVListCtrl::GetColumnCount( )
{
	return  m_hwnd ? Header_GetItemCount( ListView_GetHeader( m_hwnd ) ) : -1;
}

BOOL	CVListCtrl::SetColumnCount( int iCount )
{
	//if( !m_hwnd && iCount <= 0 )
	
	if( !m_hwnd || (iCount < 0) )
		return FALSE;
	
	int iCurrColumnCount = 0,
		iNeedAdd = 0;

	if( ( iCurrColumnCount = GetColumnCount( ) ) == -1 )
		return FALSE;


	iNeedAdd = iCount - iCurrColumnCount;
	if( iNeedAdd > 0 )
	{	// adding iNeedAdd columns
		LVCOLUMN lvC = {0};
		lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvC.fmt = LVCFMT_CENTER;
        		
		for( int iIndex = iCurrColumnCount; iIndex < iCount; iIndex ++ )
		{
			lvC.cx = iIndex ? 100 : 0;
			lvC.pszText = "";
			if( -1 == ListView_InsertColumn( m_hwnd, iIndex, &lvC ) )
				return FALSE;			
		}
	}
	else
	{	// deleting iNeedAdd columns
		for( int iIndex = (iCurrColumnCount - 1); iIndex >= iCount; iIndex -- )
		{
			if( !ListView_DeleteColumn( m_hwnd, iIndex ) )
				return FALSE;
		}
	}

	return TRUE;
}

BOOL	CVListCtrl::SetColumnProp( int iColumn, const ListColumnInfo *pstLCInfo )
{
	if( !m_hwnd || !pstLCInfo )
		return FALSE;

	ListColumnAdditionInfo *pstLAInfoCurr = 0;
	// получение структуры в LPARAM
	HDITEM stHdItem = {0};
	stHdItem.mask = HDI_LPARAM;
	if( !Header_GetItem( ListView_GetHeader( m_hwnd ), iColumn, &stHdItem ) )
		return FALSE;
	pstLAInfoCurr = (ListColumnAdditionInfo *) stHdItem.lParam;
	if( !pstLAInfoCurr )
		return FALSE;
    
	//EXCHANGE_COLUMN_ADDITIONAL_INFO_BY_MASK( pstLCInfo->uiMask, pstLCInfo, pstLAInfoCurr );
	//pstLAInfoCurr->uiMask = pstLCInfo->uiMask;
	pstLAInfoCurr->SetInfoByMask( pstLCInfo );

	// установка специфических параметров
	stHdItem.mask |= pstLCInfo->uiMask & 0xFFF; // 0xFFF - set listview flags only
	if( ( pstLCInfo->uiMask & LSTINF_TEXT ) == LSTINF_TEXT )
	{
		stHdItem.mask |= (HDI_TEXT /*| HDI_FORMAT*/);
		stHdItem.pszText = pstLCInfo->sText;
		stHdItem.cchTextMax = 0;
		if( pstLCInfo->sText )
			stHdItem.cchTextMax = strlen( pstLCInfo->sText );
		//stHdItem.fmt = /*pstLCInfo->fmt |*/ HDF_STRING; // закоментированно, т.к. pstLCInfo->fmt применяется к тексту в ячейках данного столбца, п не к тексту в заголовке
	}
	if( ( pstLCInfo->uiMask & LSTINF_WIDTH ) == LSTINF_WIDTH )
	{
		stHdItem.cxy = pstLAInfoCurr->iVisible ? pstLCInfo->iWidth : 0;
		stHdItem.mask |= HDI_WIDTH;
	}
	if( ( pstLCInfo->uiMask & LSTINF_ORDER ) == LSTINF_ORDER )
		stHdItem.iOrder = pstLCInfo->iOrder;

	return Header_SetItem( ListView_GetHeader( m_hwnd ), iColumn, &stHdItem );
}

BOOL	CVListCtrl::GetColumnProp( int iColumn, ListColumnInfo *pstLCInfo )
{
	if( !m_hwnd || !pstLCInfo )
		return FALSE;

	// получение структуры LPARAM
	HDITEM stHdItem = {0};
	stHdItem.mask = (pstLCInfo->uiMask | HDI_LPARAM) & 0xFFF; // 0xFFF - set listview flags only
	if( !Header_GetItem( ListView_GetHeader( m_hwnd ), iColumn, &stHdItem ) )
		return FALSE;
	// получение значений общих параметров
	ListColumnAdditionInfo *pstLAInfo = 0;
    pstLAInfo = ((ListColumnAdditionInfo *) stHdItem.lParam);
	if( !pstLAInfo )
		return FALSE;
	//EXCHANGE_COLUMN_ADDITIONAL_INFO_BY_MASK( pstLCInfo->uiMask, pstLAInfo, pstLCInfo );
	//pstLCInfo->GetInfoByMask( pstLAInfo );
	pstLAInfo->GetInfoByMask( pstLCInfo );

	// получение значений специфических параметров
	//if( ( pstLCInfo->uiMask & LSTINF_TEXT ) == LSTINF_TEXT )
	//	pstLCInfo->text_copy( stHdItem.pszText );		текст берем из	структуры (lParam)
	if( ( pstLCInfo->uiMask & LSTINF_ORDER ) == LSTINF_ORDER )
		pstLCInfo->iOrder = stHdItem.iOrder;
    return  TRUE;
}

int		CVListCtrl::GetNextVisibleColumn( int iCurrColumn, int iHDirection, BOOL CurrIncl /*= FALSE*/ )
{
	if( !m_hwnd )
		return xSelectionInfo::xsNoSelected;//-1;

	LVCOLUMN stCol = {0};
	stCol.mask = LVCF_ORDER;
	if( !ListView_GetColumn( m_hwnd, iCurrColumn, &stCol ) )
		return -1;
	
	int iColCount = 0;
	iColCount = GetColumnCount( );

	for( int iOrder = stCol.iOrder; ( iOrder >= 0 ) && ( iOrder < iColCount ); iOrder = iHDirection == DIR_RIGHT ? iOrder + 1 : iOrder - 1 )
	{
		if( ( iOrder == stCol.iOrder ) && !CurrIncl )
			continue;
		int iCol = Header_OrderToIndex( ListView_GetHeader( m_hwnd ), iOrder );
		ListColumnInfo stColInfo;
		stColInfo.init( );
		stColInfo.uiMask = LSTINF_VISIBLE;
		if( !GetColumnProp( iCol, &stColInfo ) )
			return xSelectionInfo::xsNoSelected;//-1;
		if( stColInfo.iVisible )
			return iCol;
	}

	return iCurrColumn;
}

BOOL	CVListCtrl::SetRowCount( int iCount, DWORD dwOption /*= 0*/ )
{
	if( !m_hwnd || ( iCount < 0 ))
		return FALSE;
	
	long style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	if( ( style & LVS_OWNERDATA ) == LVS_OWNERDATA )
		ListView_SetItemCountEx( m_hwnd, iCount, dwOption );
	ResetSelection( );
	return TRUE;
}

int	CVListCtrl::GetRowCount( )
{
	return	m_hwnd ? ListView_GetItemCount( m_hwnd ) : -1;
}

BOOL	CVListCtrl::IsVisibleAllRow( )
{
    if( !handle( ) )
		return FALSE;

	/*RECT rtClient = {0};
	if( !::GetClientRect( handle( ), &rtClient ) )
		return FALSE;

	DWORD dwDim = ListView_ApproximateViewRect( handle( ), -1, -1, GetRowCount( ) );*/
	return ( ListView_GetCountPerPage( handle() ) >= GetRowCount( ) );
}

void	CVListCtrl::RedrawWindow( )
{
	RECT rect = {0};

	if( m_hwnd && __super::get_client_rect( &rect ) )
		::InvalidateRect( m_hwnd, &rect, FALSE );

	return;
}

BOOL	CVListCtrl::EnsureVisibleCell( int iRow, int iColumn )
{	// Returns TRUE if window have scrolled, or FALSE otherwise
	if( (iRow < 0 ) || ( iColumn < 1 ) )
		return FALSE;

    if( !handle( ) )
		return FALSE;

	if( !ListView_EnsureVisible( m_hwnd, iRow, FALSE ) )
		return FALSE;

	RECT CellRect = {0};
	if( ! ListView_GetSubItemRect( m_hwnd , iRow, iColumn,LVIR_BOUNDS, &CellRect ) )
		return FALSE;

	if( ( CellRect.left < 0 ) || ( ( CellRect.right - CellRect.left ) >= m_lWidthClientArea ) )
		return ListView_Scroll( m_hwnd, CellRect.left, 0 );
	else if( CellRect.right > m_lWidthClientArea )
		return ListView_Scroll( m_hwnd, CellRect.right - m_lWidthClientArea, 0 );
	
    return FALSE;
}

void	CVListCtrl::EnableShowScrollBars( int iBar, BOOL bEnabled )
{
	if( iBar == SB_HORZ )
		m_nHScrollState = bEnabled;
	else if( iBar == SB_VERT )
		m_nVScrollState = bEnabled;
	else if( iBar == SB_BOTH )
	{
		m_nHScrollState = bEnabled;
		m_nVScrollState = bEnabled;
	}
    	
}

HWND	CVListCtrl::GetScrollbar( int iBar )
{	// iBar: SB_HORZ, SB_VERT
	return 0;
}

BOOL	CVListCtrl::IsScrollBarCtrl( HWND hwndSB )
{
	if( !hwndSB ) 
		return FALSE;

	TCHAR _stClassName[MAX_PATH];
	GetClassName( hwndSB, _stClassName, sizeof( _stClassName ) );
	return (!_tcscmp( _stClassName, WC_SCROLLBAR ) );
}

void	CVListCtrl::DisableUpdateForeignScrollBar( int iBar, BOOL bDisable /*= TRUE*/ )
{
	switch( iBar )
	{
	case SB_VERT:
		m_bUpdateForeignVSB = !bDisable;
		break;
	case SB_HORZ:
		m_bUpdateForeignHSB = !bDisable;
		break;
	case SB_BOTH:
		m_bUpdateForeignVSB = m_bUpdateForeignHSB = !bDisable;
		break;
	}
}

BOOL	CVListCtrl::GetCellProp( int iRow, int iColumn, ListCellInfo * pLCLInfo )
{
    if( !pLCLInfo )
		return FALSE;

	ListColumnInfo *pLCLMInfo = 0;
    pLCLMInfo = new ListColumnInfo;
	pLCLMInfo->uiMask = pLCLInfo->uiMask; // маска
	if( !GetColumnProp( iColumn, pLCLMInfo ) )
	{
		delete pLCLMInfo;
		pLCLMInfo = 0;
		return FALSE;
	}

	if( ( pLCLMInfo->uiMask & LSTINF_TEXT ) == LSTINF_TEXT )
	{
		pLCLInfo->uiMask &= ~LSTINF_TEXT;
		pLCLInfo->sText = 0;
		pLCLInfo->iszText = 0;
	}
//	EXCHANGE_GENERAL_INFO_BY_MASK( pLCLMInfo->uiMask, pLCLMInfo, pLCLInfo );
	//pLCLInfo->ListGeneralInfo::GetInfoByMask( pLCLMInfo );
	pLCLMInfo->ListGeneralInfo::GetInfoByMask( pLCLInfo );

	if( ( pLCLMInfo->uiMask & LSTINF_TEXT ) == LSTINF_TEXT  )
		pLCLInfo->uiMask |= LSTINF_TEXT;

	if( pLCLMInfo )
	{
		delete pLCLMInfo;
		pLCLMInfo = 0;
	}

	return TRUE;	
}

BOOL	CVListCtrl::SetCellProp( int iRow, int iColumn, const ListCellInfo * pLCInfo )
{
	return FALSE;
}

// установка параметров 
void	CVListCtrl::EnableDragDrop( BOOL bEnable /* = TRUE*/ )
{								  
	m_bEnableDragDrop = bEnable;

	if( handle( ) )
	{
		DWORD dwAdd = 0, dwRemove = 0;
		if( m_bEnableDragDrop )
	        dwAdd = LVS_EX_HEADERDRAGDROP;
		else
	        dwRemove = LVS_EX_HEADERDRAGDROP;

		ModifyStyleEx( dwRemove, dwAdd );
	} 
}

void	CVListCtrl::EnableDrawGridLines( BOOL bEnabled /*= TRUE */ )
{
	m_bDrawGridLines = bEnabled;
	RedrawWindow( );
}

long	CVListCtrl::on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
{
	switch( pnmhdr->code )
	{
	case NM_CUSTOMDRAW:
		return OnCustomDraw( (NMLVCUSTOMDRAW*)pnmhdr, plProcessed );
	case	LVN_ODCACHEHINT:
		return OnODCacheInt( ((NMLVCACHEHINT*)pnmhdr)->iFrom, ((NMLVCACHEHINT*)pnmhdr)->iTo );
	default:
		return __super::on_notify_reflect( pnmhdr, plProcessed );
	}
}

long	CVListCtrl::OnCustomDraw( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	switch( pnmCustomDraw->nmcd.dwDrawStage )	
	{
	case CDDS_PREPAINT:
		return OnPrePaint( pnmCustomDraw, plProcessed );
	case CDDS_ITEMPREPAINT:
		return OnPrePaintRow( pnmCustomDraw, plProcessed );
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		return OnPrePaintCell( pnmCustomDraw, plProcessed );
	case CDDS_POSTPAINT:
		return OnPostPaint( pnmCustomDraw, plProcessed );
	case CDDS_ITEMPOSTPAINT:
		return OnPostPaintRow( pnmCustomDraw, plProcessed );
	case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:
		return OnPostPaintCell( pnmCustomDraw, plProcessed );
	default :
		*plProcessed = false;
		return 0;
	}
}

long	CVListCtrl::OnPrePaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

long	CVListCtrl::OnPrePaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

long	CVListCtrl::OnPrePaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	if( pnmCustomDraw->iSubItem == 0 )
		return CDRF_SKIPDEFAULT;									

	RECT	rtCellRect = {0};
	if( !ListView_GetSubItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec,pnmCustomDraw->iSubItem, LVIR_BOUNDS, &rtCellRect ) )
		return CDRF_DODEFAULT;
	
	if( !need_paint_cell( &rtCellRect ) )
		return CDRF_SKIPDEFAULT;
	
	if( !*plProcessed )
	{
		ListCellInfo stCellInfo;
		ZeroMemory( &stCellInfo, sizeof( stCellInfo ));
		stCellInfo.uiMask = LSTINF_BKCOLOR;
		if( !GetCellProp( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem, &stCellInfo ) )
			return CDRF_DODEFAULT;

		pnmCustomDraw->clrTextBk = IsSelected( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem ) ? ::GetSysColor( IsFocused( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem ) ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION ) : stCellInfo.crBkColor;
	}
	else if( IsSelected( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem ) )
		pnmCustomDraw->clrTextBk = ::GetSysColor( IsFocused( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem ) ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION );

	*plProcessed = TRUE;
    	
	return CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT;
}

long	CVListCtrl::OnPostPaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_SKIPDEFAULT;
}

long	CVListCtrl::OnPostPaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	return CDRF_SKIPDEFAULT;
}

long	CVListCtrl::OnPostPaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed )
{
	*plProcessed = true;
	
	RECT	rtCellRect = {0};
	if( !ListView_GetSubItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec,pnmCustomDraw->iSubItem, LVIR_BOUNDS, &rtCellRect ) )
		return CDRF_DODEFAULT;

	ListCellInfo stCellInfo;
	ZeroMemory( &stCellInfo, sizeof( stCellInfo ));
	stCellInfo.uiMask = LSTINF_FORECOLOR | LSTINF_FONT | LSTINF_FORMAT | LSTINF_TEXT;
	if( !GetCellProp( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem, &stCellInfo ) )
		return CDRF_DODEFAULT;
	
	if( m_bDrawGridLines )
	{
		HPEN hPen = ::CreatePen( PS_SOLID, 0, ::GetSysColor( COLOR_BTNSHADOW ) );
		hPen = (HPEN) ::SelectObject( pnmCustomDraw->nmcd.hdc, hPen );
		MoveToEx( pnmCustomDraw->nmcd.hdc, rtCellRect.right - 1, rtCellRect.top, (LPPOINT) NULL );
		LineTo( pnmCustomDraw->nmcd.hdc, rtCellRect.right - 1, rtCellRect.bottom );
		MoveToEx( pnmCustomDraw->nmcd.hdc, rtCellRect.left - 1, rtCellRect.bottom - 1, (LPPOINT) NULL );
		LineTo( pnmCustomDraw->nmcd.hdc, rtCellRect.right, rtCellRect.bottom - 1 );
		DeleteObject( ::SelectObject( pnmCustomDraw->nmcd.hdc, hPen ) );
	}

	RECT	rtIconRetc = {0};
	if( !ListView_GetSubItemRect( m_hwnd, pnmCustomDraw->nmcd.dwItemSpec,pnmCustomDraw->iSubItem, LVIR_ICON, &rtIconRetc ) )
		return CDRF_DODEFAULT;

	// смещение точки по OX
	//rtCellRect.left = rtIconRetc.right + 1;
	rtCellRect.left = rtIconRetc.right + 2;
	COLORREF crTextColor = IsSelected( pnmCustomDraw->nmcd.dwItemSpec, pnmCustomDraw->iSubItem ) ? GetSysColor( COLOR_CAPTIONTEXT ) : stCellInfo.crForeColor;
	crTextColor = SetTextColor( pnmCustomDraw->nmcd.hdc, crTextColor);
	HFONT hfOldFont = (HFONT) SelectObject( pnmCustomDraw->nmcd.hdc, CreateFontIndirect( &stCellInfo.lfFont ) );
	
	
	// отрисовка текста
	rtCellRect.right -= 2; // сдвиг правой границы 
	if( stCellInfo.sText )
		::DrawText( pnmCustomDraw->nmcd.hdc, stCellInfo.sText, strlen( stCellInfo.sText ), &rtCellRect, 
		stCellInfo.fmt | DT_WORD_ELLIPSIS );
			
	DeleteObject( SelectObject( pnmCustomDraw->nmcd.hdc, hfOldFont ) );
	SetTextColor( pnmCustomDraw->nmcd.hdc, crTextColor );

	return CDRF_SKIPDEFAULT;
}


long	CVListCtrl::OnODCacheInt( int nRowFrom, int	nRowTo )
{
	return 0;
}

LRESULT	CVListCtrl::on_keydown(long nVirtKey)
{
    if( !handle() )
		return 1;

	BOOL	bProcessed = FALSE;
    xSelectionInfo	stOldSel = m_stSelection;
	xFocusedInfo	stOldFocused = m_stFocused;

	/*if( !ExistSelections( ) && ( GetRowCount( ) > 0 ) && ( GetColumnCount( ) > 1 ) )
	{	// если ничего не выбрано
		m_stSelection.iColumnBegin = m_stSelection.iColumnEnd = 1;
		m_stSelection.iRowBegin = m_stSelection.iRowEnd = 0;
	}*/

	if( !( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) || !m_bEnableMultiSelection )
	{
		// set focused
		bProcessed = CalcNewCellPos( &m_stFocused.iRow, &m_stFocused.iColumn, nVirtKey );
		if( !ExistFocused( ) )
			SetFocused( 0, 1);
		if( bProcessed )
		{	// если нажатие на клавишу обработано
			m_stSelection.iColumnBegin = m_stSelection.iColumnEnd = m_stFocused.iColumn;
			m_stSelection.iRowBegin = m_stSelection.iRowEnd = m_stFocused.iRow;
			EnsureVisibleCell( m_stFocused.iRow, m_stFocused.iColumn );
		}
	}
	else
	{
		if( bProcessed = CalcNewCellPos( &m_stSelection.iRowEnd, &m_stSelection.iColumnEnd, nVirtKey ) )
			EnsureVisibleCell( m_stSelection.iRowEnd, m_stSelection.iColumnEnd );
	}


	if( stOldSel != m_stSelection )
	{
		stOldSel.Redraw( m_hwnd );
		m_stSelection.Redraw( m_hwnd );
	}

	if( stOldFocused != m_stFocused )	
		OnFocusedChanged( m_stFocused.iRow, m_stFocused.iColumn );	// fire event

	return /*bProcessed ? 1 : __super::on_keydown( nVirtKey )*/ 1;
}

LRESULT	CVListCtrl::on_lbuttondown(const _point &point)
{

	if( !handle() )
		return FALSE;

	LVHITTESTINFO lvH = {0};
	xSelectionInfo	stOldSel = m_stSelection;
	xFocusedInfo	stOldFocused = m_stFocused;



	
	lvH.pt.x =  point.x; 
	lvH.pt.y = point.y;
	lvH.flags = LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON;
	if( ListView_SubItemHitTest( m_hwnd, &lvH ) == -1 )
	{
		//ResetSelection( );
		//m_stFocused.reset( );
	}
	else //if( ( lvH.iItem != -1 ) && ( lvH.iSubItem != -1 ) ) // (-1) в данном случае это не xSelectioninfo::xsFullRow, а это значение, воторое возвращает контрол при непопадании ни в один Item или SubItem
	{
		if( IsFocused( lvH.iItem, lvH.iSubItem ) )
			OnFocusedLButtonDown( );
		else
		{
			m_stFocused.iRow = lvH.iItem;
			m_stFocused.iColumn = lvH.iSubItem;
		}

		m_stSelection.iRowBegin = m_stSelection.iRowEnd = lvH.iItem;
		m_stSelection.iColumnBegin = m_stSelection.iColumnEnd = lvH.iSubItem;
			
		EnsureVisibleCell( m_stFocused.iRow, m_stFocused.iColumn );
		if( stOldSel != m_stSelection )
		{
			stOldSel.Redraw( m_hwnd );
			m_stSelection.Redraw( m_hwnd );
		}
	}

	if( stOldFocused != m_stFocused )	
		OnFocusedChanged( m_stFocused.iRow, m_stFocused.iColumn );	// fire event

	return __super::on_lbuttondown( point );
}

LRESULT	CVListCtrl::on_notify(uint idc, NMHDR *pmnhdr)
{
	LRESULT lRet = 0;

	switch( pmnhdr->code )
	{
	case HDN_DIVIDERDBLCLICKA:
	case HDN_DIVIDERDBLCLICKW:
		lRet = 0;
		break;		// disable double-clicked divider area of header

	case HDN_ITEMCLICKA:
	case HDN_ITEMCLICKW:
		switch( ( (LPNMHEADER) pmnhdr )->iButton )
		{
		case 0:
			lRet = OnLButtonClickHeader( (LPNMHEADER) pmnhdr );
			break;
		case 1:
			lRet = OnRButtonClickHeader( (LPNMHEADER) pmnhdr );
			break;
		case 2:
			lRet = OnMButtonClickHeader( (LPNMHEADER) pmnhdr );
			break;
		}
		break;

	case HDN_ITEMCHANGINGA:
	case HDN_ITEMCHANGINGW:
		lRet = OnItemChangingHeader( (LPNMHEADER) pmnhdr );
		break;

	case HDN_ITEMCHANGEDA:
	case HDN_ITEMCHANGEDW:
		lRet = OnItemChangedHeader( (LPNMHEADER) pmnhdr );
		break;

	case HDN_BEGINTRACKA:
	case HDN_BEGINTRACKW:
		lRet = OnBeginTrackHeader( (LPNMHEADER) pmnhdr );
		break;

	case HDN_ENDTRACKA:
	case HDN_ENDTRACKW:
		lRet = OnEndTrackHeader( (LPNMHEADER) pmnhdr );
		break;

	case HDN_BEGINDRAG:
		lRet = OnBeginDragHeader( (LPNMHEADER) pmnhdr );
		break;
	
	case HDN_ENDDRAG:
		lRet = OnEndDragHeader( (LPNMHEADER)  pmnhdr );
		break;

	case NM_CUSTOMDRAW:
		lRet = OnCustomDrawHeader( (NMCUSTOMDRAW*)pmnhdr );
		break;

	default:
		lRet =	__super::on_notify( idc, pmnhdr );
	}

	return lRet;
}

LRESULT	CVListCtrl::on_size(short cx, short cy, ulong fSizeType)
{
	m_lWidthClientArea = cx;
	m_lHeightClientArea = cy;
    
	// vanek - 24.09.2003
	long lres = __super::on_size( cx, cy, fSizeType );

	//set_foreign_scrollbar_info( SB_HORZ );
	//set_foreign_scrollbar_info( SB_VERT );

	return lres;
}

LRESULT	CVListCtrl::on_destroy()
{
	for( int iColumn = 0; iColumn < GetColumnCount( ); iColumn ++ )
	{
		ListColumnAdditionInfo *pstLParam = (ListColumnAdditionInfo *) get_lparam( iColumn );
		if( pstLParam )
		{
			delete pstLParam;
			pstLParam = 0;
		}
	}

	if( m_hfHeaderFont )
	{	// delete old font
		::DeleteObject( m_hfHeaderFont );	
		m_hfHeaderFont = 0;
	}

	return __super::on_destroy( );
}

/*BOOL	CVListCtrl::UpdateHeaderHeight( void )
{	// find max font and set it for header
	if( !handle() )
		return FALSE;

    HWND hwndHeader = 0;
	hwndHeader = ListView_GetHeader( handle() );
	if( !hwndHeader )
		return FALSE;

	HDC hdc = 0;
	hdc = ::GetDC( hwndHeader );
	if( !hdc )
		return FALSE;

	LOGFONT max_lf = {0};
	int nLogPixelsY = ::GetDeviceCaps( hdc, LOGPIXELSY ),
		nColumnCount = GetColumnCount( ),
		nPointSize = 0;
	
	ListColumnInfo stColumnInfo;
	stColumnInfo.init();
	stColumnInfo.uiMask = LSTINF_FONT;

	// find max font
	for( int nColumn = 1; nColumn <= nColumnCount; nColumn ++ )
	{
		if( !GetColumnProp( nColumn, &stColumnInfo ) )
			return FALSE;

		int nNewPointSize = -::MulDiv( stColumnInfo.lfFont.lfHeight, 72, nLogPixelsY );
		if( nNewPointSize > nPointSize )
		{
			max_lf = stColumnInfo.lfFont;
			nPointSize = nNewPointSize;
		}
	}
    
	if( !nPointSize )
		return FALSE;

	// create max font and set it for header
	HFONT hfNewFont = 0;
	hfNewFont = ::CreateFontIndirect( &max_lf );
	if( !hfNewFont )
		return FALSE;

	::SendMessage( hwndHeader, WM_SETFONT, (WPARAM) hfNewFont, (LPARAM) TRUE );
    
	if( m_hfHeaderFont )
	{	// delete old font
		::DeleteObject( m_hfHeaderFont );	
		m_hfHeaderFont = 0;
	}
	
	m_hfHeaderFont = hfNewFont;	// save new font
	return TRUE;
}		*/

BOOL	CVListCtrl::Header_SetFont( LOGFONT *plf )
{
	if( !plf )
		return FALSE;

	if( !handle() )
		return FALSE;

    HWND hwndHeader = 0;
	hwndHeader = ListView_GetHeader( handle() );
	if( !hwndHeader )
		return FALSE;

    // create font and set it for header
	HFONT hfNewFont = 0;
	hfNewFont = ::CreateFontIndirect( plf );
	if( !hfNewFont )
		return FALSE;

	::SendMessage( hwndHeader, WM_SETFONT, (WPARAM) hfNewFont, (LPARAM) TRUE );
    
	if( m_hfHeaderFont )
	{	// delete old font
		::DeleteObject( m_hfHeaderFont );	
		m_hfHeaderFont = 0;
	}
	
	m_hfHeaderFont = hfNewFont;	// save new font
	return TRUE;    
}

BOOL	CVListCtrl::Header_SetTextColor( COLORREF *pcr )
{
    if( !pcr )
		FALSE;
	
	m_crHeaderText = *pcr;
	HWND hwndHeader = 0;
	hwndHeader = ListView_GetHeader( handle() );
	::InvalidateRect( hwndHeader, 0, TRUE);
	return TRUE;
}

LRESULT	CVListCtrl::OnLButtonClickHeader(LPNMHEADER lpmnhdr)
{
	// выбор столбца
	//SetSelection( -1, -1, lpmnhdr->iItem, lpmnhdr->iItem );
	//SetFocused( ListView_GetTopIndex( m_hwnd ), lpmnhdr->iItem );
	//Redraw( );
	//return 1;
	return __super::call_default();
}

LRESULT	CVListCtrl::OnMButtonClickHeader(LPNMHEADER lpmnhdr)
{
	//return 1;
	return __super::call_default();
}

LRESULT	CVListCtrl::OnRButtonClickHeader(LPNMHEADER lpmnhdr)
{
	//return 1;
	return __super::call_default();
}

LRESULT	CVListCtrl::OnItemChangingHeader(LPNMHEADER lpmnhdr)
{
   return __super::call_default();
}

LRESULT	CVListCtrl::OnItemChangedHeader(LPNMHEADER lpmnhdr)
{
	if( ( ( lpmnhdr->pitem->mask & HDI_WIDTH ) == HDI_WIDTH ) )
	{	// если ширина изменилась
		// получение значений общих параметров
		ListColumnAdditionInfo *pstLAInfo = 0;
		pstLAInfo = (ListColumnAdditionInfo *) get_lparam( lpmnhdr->iItem );
		if( pstLAInfo )
		{
			pstLAInfo->iWidth = lpmnhdr->pitem->cxy;
			pstLAInfo->uiMask |= LSTINF_WIDTH;
		}
	}
	
	// vanek - 24.09.2003
	LRESULT lres = __super::call_default();
	if( ( ( lpmnhdr->pitem->mask & HDI_WIDTH ) == HDI_WIDTH ) )	
		set_foreign_scrollbar_info( SB_HORZ );
	else if( ( ( lpmnhdr->pitem->mask & HDI_WIDTH ) == HDI_HEIGHT ) )
		set_foreign_scrollbar_info( SB_VERT );
	return lres;
}

LRESULT	CVListCtrl::OnBeginTrackHeader(LPNMHEADER lpmnhdr)
{
	// получение значений общих параметров
	ListColumnAdditionInfo *pstLAInfo = 0;
	pstLAInfo = (ListColumnAdditionInfo *) get_lparam( lpmnhdr->iItem );
	LRESULT lRes = __super::call_default();
	return ( pstLAInfo ? !pstLAInfo->iVisible : lRes );
}

LRESULT	CVListCtrl::OnEndTrackHeader(LPNMHEADER lpmnhdr)
{
	return __super::call_default();
	//return FALSE;
}

LRESULT	CVListCtrl::OnBeginDragHeader(LPNMHEADER lpmnhdr)
{
	return __super::call_default();
	//return FALSE;
}

LRESULT	CVListCtrl::OnEndDragHeader(LPNMHEADER lpmnhdr)
{
	return __super::call_default();
	//return FALSE;
}


long	CVListCtrl::OnCustomDrawHeader( NMCUSTOMDRAW *pnmCustomDraw)
{
    switch( pnmCustomDraw->dwDrawStage )	
	{
	case CDDS_PREPAINT:
		return OnPrePaintHeader( pnmCustomDraw );
	case CDDS_ITEMPREPAINT:
		return OnPrePaintItemHeader( pnmCustomDraw );
	case CDDS_POSTPAINT:
		return OnPostPaintHeader( pnmCustomDraw );
	case CDDS_ITEMPOSTPAINT:
		return OnPostPaintItemHeader( pnmCustomDraw );
	default :
		return 0;
	}    
}

long	CVListCtrl::OnPrePaintHeader( NMCUSTOMDRAW *pnmCustomDraw )
{
	if( !pnmCustomDraw || ( m_crHeaderText == COLORREF(-1) ) )
		return CDRF_DODEFAULT;

	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;    
}

long	CVListCtrl::OnPostPaintHeader( NMCUSTOMDRAW *pnmCustomDraw )
{
	return CDRF_NOTIFYITEMDRAW;
}


long	CVListCtrl::OnPrePaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw )
{
	::SetTextColor( pnmCustomDraw->hdc, m_crHeaderText );
	return CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT;
}


long	CVListCtrl::OnPostPaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw )
{
 	return CDRF_SKIPDEFAULT;
}


////////// Firing events
void CVListCtrl::OnFocusedChanged( int iRow, int iColumn )
{
	return;
}

void CVListCtrl::OnFocusedLButtonDown( )
{
	return;
}