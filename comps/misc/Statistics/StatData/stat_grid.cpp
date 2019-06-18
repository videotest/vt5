#include "stdafx.h"
#include "stat_grid.h"
#include "stat_view.h"

#include "stat_utils.h"

#include "resource.h"
#include "params.h"

#include "\vt5\awin\profiler.h"
#include "MenuRegistrator.h"

//////////////////////////////////////////////////////////////////////
//
//	class CStatGrid
//
//////////////////////////////////////////////////////////////////////
CStatGrid::CStatGrid()
	: m_map_pcol(0)
{
	EnableDrawGridLines( true );
	EnableShowScrollBars( SB_BOTH, false );
	EnableDragDrop( true );

	m_pparent			= 0;

	m_hwnd_horz_scroll	= 0;
	m_hwnd_vert_scroll	= 0;

//	m_str_class_name	= "";
}

//////////////////////////////////////////////////////////////////////
CStatGrid::~CStatGrid()
{

}

//////////////////////////////////////////////////////////////////////
bool CStatGrid::buid( IUnknown* punk_tbl )
{
	TIME_TEST( "CStatGrid::buid()" );

	if( ::GetKey( m_ptr_table ) == ::GetKey( punk_tbl ) )	
		return true;

	if( m_ptr_table ){
		save_state( );
	}
	m_ptr_table = punk_tbl;
	m_map_pcol=&((CStatTableObject*)(IStatTable*)m_ptr_table)->m_map_col;

	fill_grid(false);

	return true;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::on_paint()
{
	//clear cache
	if( m_ptr_table )
		m_ptr_table->ClearCache( );
	
//	m_str_class_name = short_classifiername( 0 );

	return call_default();
}

namespace {
	bool less_col_info(const stat_col_info& e1, const stat_col_info& e2)
	{
		return e1.m_order<e2.m_order || 
			e1.m_order==e2.m_order && e1.m_lkey<e2.m_lkey;
	}
}

//////////////////////////////////////////////////////////////////////
bool CStatGrid::fill_grid( bool bload_from_shell_data )
{
	CreateAllParamDescrCache( m_map_meas_params, m_ptr_table);

	//clean grid
	SetRowCount( 0 );
	SetColumnCount( 0 );	

	//clear maps
	m_list_col.clear();
	if(m_ptr_table)
		m_map_pcol->clear();
	m_map_row.clear();	

	if( m_ptr_table == 0 )
		return true;

//	((CStatTableObject*)(IStatTable*)m_ptr_table)->UpdateColorsNames(short_classifiername(0));

	//cache columns
	long lpos_param = 0;
	m_ptr_table->GetFirstParamPos( &lpos_param );	
	while( lpos_param )
	{			
		stat_param* psp = 0;
		m_ptr_table->GetNextParam( &lpos_param, &psp );		

		stat_col_info col_info;
		col_info.m_lkey = psp->lkey;
		m_list_col.push_back( col_info );
	}

	//load state for this table
	load_state( bload_from_shell_data );

	//sort columns
	{
		std::sort(m_list_col.begin(), m_list_col.end(), less_col_info );

		//for( long lpos1=m_list_col.head(); lpos1; lpos1=m_list_col.next(lpos1) )
		//{
		//	stat_col_info* pinfo1 = m_list_col.get( lpos1 );
		//	for( long lpos2=lpos1; lpos2; lpos2=m_list_col.next(lpos2) )
		//	{
		//		stat_col_info* pinfo2 = m_list_col.get( lpos2 );
		//		if( pinfo1->m_order > pinfo2->m_order && 
		//			pinfo1->m_bvisible && pinfo2->m_bvisible )
		//		{
		//			m_list_col.set( pinfo2, lpos1  );
		//			m_list_col.set( pinfo1, lpos2  );				

		//			pinfo2 = m_list_col.get( lpos2 );
		//			pinfo1 = m_list_col.get( lpos1 );
		//		}
		//	}
		//}
	}
	
	//add invisible column
	SetColumnCount( 1 );
	//add columns
	int ncol_num = 1;
	for(StatColVector::iterator lpos=m_list_col.begin();
		lpos!=m_list_col.end(); ++lpos)
	{
		stat_col_info& pinfo = *lpos;
		if( !pinfo.m_bvisible )	continue;

		//set col prop
		char sz_caption[255];	sz_caption[0] = 0;
		//lookup measure unit
		_bstr_t bstr_name;
		_bstr_t bstr_unit;

		if( pinfo.m_lkey == KEY_CLASS )
		{
			::LoadString( app::handle(), IDS_CLASS_CAPTION, sz_caption, sizeof(sz_caption) );
		}
		else
		{
			map_meas_params::const_iterator it=m_map_meas_params.find( pinfo.m_lkey );
			if( it!=m_map_meas_params.end() )
			{
				ParameterDescriptor_ex* pmp = it->second;
				if( pmp->bstrUserName && *pmp->bstrUserName )
					bstr_name = pmp->bstrUserName;
				else if( pmp->bstrName && *pmp->bstrName)
					bstr_name = pmp->bstrName;
				if( pmp->bstrUnit && *pmp->bstrUnit){
					bstr_unit = pmp->bstrUnit;
					bstr_name += "," + bstr_unit;
				}
				strcpy(sz_caption,(char*)bstr_name);
			}else{
				continue;
			}
		}			
        		
		//set col count
		SetColumnCount( ncol_num + 1 );

		//cache col num
		(*m_map_pcol)[(long)ncol_num]=&pinfo;
//			set( pinfo, ncol_num );

		ListColumnInfo lci;
		lci.uiMask	= LSTINF_TEXT|LSTINF_WIDTH|LSTINF_FORMAT;
		lci.fmt		= DT_RIGHT;
		lci.iWidth	= pinfo.m_width;
		lci.sText	= sz_caption;
		lci.iszText	= strlen( sz_caption );		

		SetColumnProp( ncol_num, &lci );

		ncol_num++;
	}

	//cache row num
	long lrow_num = 0;
	long lpos_row = 0;
	m_ptr_table->GetFirstRowPos( &lpos_row );
	while( lpos_row )
	{
		m_map_row.set( lpos_row, lrow_num );
		stat_row* prow = 0;
		m_ptr_table->GetNextRow( &lpos_row, &prow );
		lrow_num++;
	}

	//set row count
	long lrow_count = 0;
	m_ptr_table->GetRowCount( &lrow_count );
	SetRowCount( lrow_count );

	save_state( );

	//Invalidate grid
	::InvalidateRect( handle(), 0, true );	
	::InvalidateRect( ListView_GetHeader( handle() ), 0, true );

	return true;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::on_contextmenu( const _point &point )
{
	POINT pt = point;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )	return 0;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == 0 )	return 0;
	
	CMenuRegistrator rcm;	
	ptrCM->ExecuteContextMenu2( rcm.GetMenu( szStatTabletViewNameMenu, 0 ), pt, 0 );	

	return 0;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::on_destroy()
{
	save_state( );
	return CVListCtrl::on_destroy();
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::on_rbuttondown( const _point &point )
{
	return __super::on_lbuttondown( point );
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::OnEndTrackHeader( LPNMHEADER lpmnhdr )
{
	long lres = __super::OnEndTrackHeader( lpmnhdr );
	save_state( );
	return lres;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::OnEndDragHeader( LPNMHEADER lpmnhdr )
{
	long lres = __super::OnEndDragHeader( lpmnhdr );
	save_state( );
	return lres;
}


//////////////////////////////////////////////////////////////////////
void CStatGrid::SetParent( CStatView* pparent )
{
	m_pparent = pparent;
}

//////////////////////////////////////////////////////////////////////
void CStatGrid::handle_init()
{
	CreateAllParamDescrCache( m_map_meas_params , m_ptr_table);
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::handle_message( UINT m, WPARAM w, LPARAM l )
{
	/*
	if( m == WM_KEYDOWN && w == VK_F2 )	
	{
		ListView_EditLabel( handle(), 1 );
		return 1L;
	}*/
	return __super::handle_message( m, w, l );
}

//////////////////////////////////////////////////////////////////////
HWND CStatGrid::GetScrollbar( int iBar )
{
	if( iBar == SB_HORZ )
	{
		if( !m_hwnd_horz_scroll && m_pparent )
		{
			IScrollZoomSitePtr ptr_szs( m_pparent->Unknown() );
			if( ptr_szs )
				ptr_szs->GetScrollBarCtrl( SB_HORZ, (HANDLE*)&m_hwnd_horz_scroll );
		}

		return m_hwnd_horz_scroll;
	}
	else if( iBar == SB_VERT )
	{
		if( !m_hwnd_vert_scroll && m_pparent )
		{
			IScrollZoomSitePtr ptr_szs( m_pparent->Unknown() );
			if( ptr_szs )
				ptr_szs->GetScrollBarCtrl( SB_VERT, (HANDLE*)&m_hwnd_vert_scroll );
		}

		return m_hwnd_vert_scroll;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
BOOL CStatGrid::GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo )
{
	if( m_ptr_table == 0 )	return false;

	CVListCtrl::GetCellProp( iRow, iColumn, pLCInfo );

	if( !( pLCInfo->uiMask & LSTINF_TEXT ) )	return true;

	if( !iColumn )	return true;
	
	CMapColInfo::const_iterator itCol = m_map_pcol->find( iColumn );
	long lpos_map_row = m_map_row.find( iRow );

	if( m_map_pcol->end()==itCol || !lpos_map_row )
	{
		_ASSERTE( false );
		return false;
	}

	
	stat_col_info* psci	= itCol->second;
	long lpos_row		= m_map_row.get( lpos_map_row );


	static char sz_buf[255]; sz_buf[0] = 0;
	//value
	stat_value* pvalue = 0;
	m_ptr_table->GetValueByKey( lpos_row, psci->m_lkey, &pvalue );

	{
		_bstr_t bstr_format = "%f";

		if( psci->m_lkey == KEY_CLASS && pvalue )
		{
			strcpy( sz_buf, ((CStatTableObject*)(IStatTable*)m_ptr_table)
				->get_class_name((long)pvalue->fvalue));
		}
		else
		{
			double funit = 1.;
			map_meas_params::const_iterator it=m_map_meas_params.find( psci->m_lkey );
			if( it!=m_map_meas_params.end() )
			{
				ParameterDescriptor_ex* pmp = it->second;
				if( pmp->bstrDefFormat )
					bstr_format = pmp->bstrDefFormat;
				funit = pmp->fCoeffToUnits;
			}

			if( pvalue && pvalue->bwas_defined )
				sprintf( sz_buf, (char*)bstr_format, pvalue->fvalue * funit );
			else
				strcpy( sz_buf, "-" );		
		}
	}

	pLCInfo->sText		= sz_buf;
	pLCInfo->iszText	= strlen( sz_buf );

	return true;
}

//Serailization
//////////////////////////////////////////////////////////////////////
bool CStatGrid::load_state( bool bload_from_shell_data )
{
	if( m_ptr_table == 0 )	return false;

	INamedDataPtr ptr_nd( m_ptr_table );
	if( ptr_nd == 0 )
	{
		INamedDataObject2Ptr ptr_ndo2( m_ptr_table );
		if( ptr_ndo2 )
			ptr_ndo2->InitAttachedData();
		ptr_nd = ptr_ndo2;
		bload_from_shell_data = true;
	}

	if( ptr_nd == 0 )
		return false;	

	for(StatColVector::iterator lpos=m_list_col.begin();
		lpos!=m_list_col.end(); ++lpos)
	{
		stat_col_info& pci = *lpos;
		long lkey = pci.m_lkey;

		char sz_section[255];		
		sprintf( sz_section, "%s\\%s%d", STAT_TABLE_GRID, ST_COL_KEY, lkey );
		ptr_nd->SetupSection( _bstr_t( sz_section ) );
		pci.load( ptr_nd );

		if( bload_from_shell_data )
		{
			map_meas_params::const_iterator it=m_map_meas_params.find( lkey );
			if( it!=m_map_meas_params.end() )
			{
				ParameterDescriptor_ex* pmp = it->second;
				if( pmp->bstrName )
				{
					bstr_t bstr_param_section = "\\measurement\\parameters\\";
					bstr_param_section += pmp->bstrName;
					long lenable = ::GetValueInt( ::GetAppUnknown(), bstr_param_section, "Enable", 0 );
					pci.m_bvisible = ( lenable != 0 );
				}
			}
		}
	}

	//load order
	_bstr_t bstr_value;
	if( bload_from_shell_data )
		bstr_value = ::GetValueString( ::GetAppUnknown(), SECTION_ORDER, ST_KEY_ORDER, "" );
	else
		bstr_value = ::GetValueString( ptr_nd, STAT_TABLE_GRID, ST_KEY_ORDER, "" );

	if( bstr_value.length() )
	{
		_ptr_t<char> ptr_buf;
		char* psz_buf = ptr_buf.alloc( bstr_value.length() + 1);
		if( psz_buf )
		{
			strcpy( psz_buf, (char*)bstr_value );

			long norder = 0;
			char* psz_tok = strtok( psz_buf, "," );
			while( psz_tok )
			{
				long lkey = atoi( psz_tok );
				//find info
				for(StatColVector::iterator lpos=m_list_col.begin();
					lpos!=m_list_col.end(); ++lpos)
				{
					stat_col_info& pci = *lpos;
					long lkey_test = pci.m_lkey;
					if( lkey == lkey_test )
					{
						pci.m_order = norder;
						break;
					}
				}
				psz_tok = strtok( 0, "," );
				norder++;
			}
			for(StatColVector::iterator lpos=m_list_col.begin();
				lpos!=m_list_col.end(); ++lpos)
			{
				stat_col_info& pci = *lpos;
				long lkey_test = pci.m_lkey;
			}
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
bool CStatGrid::save_state( long lflags )
{
	INamedDataPtr ptr_nd( m_ptr_table );
	if( ptr_nd == 0 )
	{
		INamedDataObject2Ptr ptr_ndo2( m_ptr_table );
		if( ptr_ndo2 )
			ptr_ndo2->InitAttachedData();
		ptr_nd = ptr_ndo2;
	}

	if( ptr_nd == 0 )
		return false;

	//get order & width
	_bstr_t bstr_order;
	int ncol_count = GetColumnCount();
	for( int ncol=1;ncol<ncol_count;ncol++ )
	{
		ListColumnInfo lci;
		lci.uiMask	= LSTINF_ORDER|LSTINF_WIDTH;
		GetColumnProp( ncol, &lci );
//		long lpos_map = m_map_col.find( ncol );
		CMapColInfo::const_iterator itCol = m_map_pcol->find( ncol );
		if( m_map_pcol->end()==itCol )
		{
			_ASSERTE(false);
			continue;
		}
		stat_col_info* psci = itCol->second;

		if( lflags & EXCHANGE_WIDTH )
			psci->m_width	= lci.iWidth;
		
		if( lflags & EXCHANGE_ORDER )
			psci->m_order	= lci.iOrder;
		if(1==ncol)
			bstr_order = bstr_t(psci->m_lkey);
		else 
			bstr_order += _bstr_t(L",")+_bstr_t(psci->m_lkey);
	}

	//save to data
	ptr_nd->SetupSection( _bstr_t( STAT_TABLE_GRID ) );

	for(StatColVector::const_iterator lpos=m_list_col.begin();
		lpos!=m_list_col.end(); ++lpos)
	{
		const stat_col_info& pci = *lpos;
		long lkey = pci.m_lkey;

		char sz_section[255];		
		sprintf( sz_section, "%s\\%s%d", STAT_TABLE_GRID, ST_COL_KEY, lkey );
		ptr_nd->SetupSection( _bstr_t( sz_section ) );
		pci.save( ptr_nd );
	}

	if( bstr_order.length() )
	{
		ptr_nd->SetupSection( _bstr_t( STAT_TABLE_GRID ) );
		ptr_nd->SetValue( _bstr_t(ST_KEY_ORDER), _variant_t(bstr_order));
	}


	return true;
}

//////////////////////////////////////////////////////////////////////
stat_col_info* CStatGrid::find_col_info_by_num( long ncol )
{
		CMapColInfo::const_iterator itCol = m_map_pcol->find( ncol );
		if( m_map_pcol->end()==itCol )
		{
			return 0;
		}
		return itCol->second;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::get_rows_count( )
{
	return GetRowCount() + 1;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::get_columns_count( )
{
	return GetColumnCount();
}

//////////////////////////////////////////////////////////////////////
_rect CStatGrid::get_cell_rect( int nrow, int ncol )
{
	_rect rc;

	if( !nrow )
	{
		HWND hwnd_header = ListView_GetHeader( handle() );
		Header_GetItemRect( hwnd_header, ncol, &rc );
		_rect rc_item;
		ListView_GetSubItemRect( handle(), 0, ncol, LVIR_BOUNDS, &rc_item );
		if( rc.bottom < rc_item.top )
			rc.bottom = rc_item.top;
	}
	else
	{
		ListView_GetSubItemRect( handle(), nrow - 1, ncol, LVIR_BOUNDS, &rc );

		if( !ncol )
			rc.left = rc.right = 0;		
	}
	return rc;
}

//////////////////////////////////////////////////////////////////////
_string CStatGrid::get_item_text( int nrow, int ncol )
{
	_string str_text = "";

	if( !nrow )
	{
		ListColumnInfo LCInfo;
		LCInfo.uiMask = LSTINF_TEXT;		
		GetColumnProp( ncol, &LCInfo );
		if( LCInfo.sText )
			str_text = LCInfo.sText;
	}
	else
	{
		ListCellInfo LCInfo;
		LCInfo.uiMask = LSTINF_TEXT;
		GetCellProp( nrow - 1, ncol, &LCInfo );
		if( LCInfo.sText )
			str_text = LCInfo.sText;
	}
	
	return str_text;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatGrid::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	if( !pnReturnWidth || !pbContinue || !pnNewUserPosX )	return E_POINTER;

	*pbContinue = false;
	
	int ncol_count = get_columns_count();
	int nwidth = 0;
	for( int i=nUserPosX; i<ncol_count;i++ )
	{
		_rect rc = get_cell_rect( 0, i );		
		if( nwidth + rc.width() <= nMaxWidth )
		{
			nwidth += rc.width();
			*pnReturnWidth = nwidth;
			*pnNewUserPosX = i+1;
			*pbContinue = ( i < ncol_count - 1 );
		}
		else
			break;
	}
//	*pnReturnWidth = std::numeric_limits<int>::max()/200;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatGrid::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	if( !pnReturnHeight || !pbContinue || !pnNewUserPosY )	return E_POINTER;

	*pbContinue = false;
	
	int nrow_count = get_rows_count();
	int nheight = 0;
	for( int i=nUserPosY; i<nrow_count;i++ )
	{
		_rect rc = get_cell_rect( i, 0 );
		if( nheight + rc.height() <= nMaxHeight )
		{
			nheight += rc.height();
			*pnReturnHeight = nheight;
			*pnNewUserPosY = i+1;
			*pbContinue = ( i < nrow_count - 1 );			
		}
		else
			break;
	}
//	*pnReturnHeight = std::numeric_limits<int>::max()/200;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatGrid::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	if( !hdc )	return S_FALSE;

	//clear cache first
	if( m_ptr_table )
		m_ptr_table->ClearCache();

	//m_str_class_name = short_classifiername( 0 );

	_rect rc_target( rectTarget.left, rectTarget.top, rectTarget.right, rectTarget.bottom );

	_rect rc_first_item = get_cell_rect( nUserPosY, nUserPosX );

	_rect rc_tmp;
	_rect rrectTarget=(const _rect&)rectTarget;

	//rrectTarget.left*=100;
	//rrectTarget.top*=100;
	//rrectTarget.right*=100;
	//rrectTarget.bottom*=100;

	//rrectTarget.left/=70;
	//rrectTarget.top/=70;
	//rrectTarget.right/=70;
	//rrectTarget.bottom/=70;

	//determine width	
	int nwidth = 0;
	int colLast=nUserPosX;
	for( ; nwidth<rrectTarget.width() && colLast<nUserPosX+nUserPosDX && colLast<get_columns_count(); ++colLast )
	{
		rc_tmp = get_cell_rect( 0, colLast+1);
		nwidth += rc_tmp.width();
	}
	
	//determine height
	int nheight = 0;
	int rowLast=nUserPosY;
	for(;rowLast<get_rows_count() && rowLast<nUserPosY+nUserPosDY; ++rowLast )
	{
		rc_tmp = get_cell_rect( rowLast, 1 );
		nheight += rc_tmp.height();
	}	
	
	
	//set mode
	int nold_map_mode = ::SetMapMode( hdc, MM_ANISOTROPIC );
	_size size_old_wnd_ext;
	//set extension
	::SetWindowExtEx( hdc, 100, 100, &size_old_wnd_ext );
	_size size_old_viewport_ext;
	::SetViewportExtEx( hdc, 100, 100, &size_old_viewport_ext );
	
	//set viewport
	_point pt_old_viewport_org;
	_point pt_viewport_org;

	pt_viewport_org.x = rc_target.left;
	pt_viewport_org.y = rc_target.top;
	::SetViewportOrgEx( hdc, pt_viewport_org.x, pt_viewport_org.y, &pt_old_viewport_org );

	//create brush
	HBRUSH brush_border = ::CreateSolidBrush( RGB(0,0,0) );

	//set bk mode
	int nmode = ::SetBkMode( hdc, TRANSPARENT );

	//set font
	HFONT hfont = (HFONT)::SendMessage( handle(), WM_GETFONT, 0, 0 );
	HFONT hfont_old = (HFONT)::SelectObject( hdc, hfont );

	HPEN hpen_solid	= ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
	HPEN hpen_dash	= ::CreatePen( PS_DASH, 1, RGB(0,0,0) );	

	COLORREF clr_old_text = ::SetTextColor( hdc, RGB(0,0,0) );	
	
	for( int ncol = nUserPosX; ncol < colLast; ncol++ )
	{
		for( int nrow = nUserPosY; nrow < rowLast; nrow++ )
		{
			_rect rc_cell = get_cell_rect( nrow, ncol );

			//set 0 offset on page
			rc_cell.left	-= rc_first_item.left;
			rc_cell.right	-= rc_first_item.left;
			rc_cell.top		-= rc_first_item.top;
			rc_cell.bottom	-= rc_first_item.top;			

			if( rc_cell.width() && rc_cell.height() )
			{					
				HPEN hpen_old = (HPEN)::SelectObject( hdc, hpen_solid );//!nrow ? hpen_solid : hpen_dash );

				_rect rc_border = rc_cell;
				::MoveToEx( hdc, rc_border.left, rc_border.top, 0 );
				::LineTo( hdc, rc_border.right, rc_border.top );
				::LineTo( hdc, rc_border.right, rc_border.bottom );
				::LineTo( hdc, rc_border.left, rc_border.bottom );
				::LineTo( hdc, rc_border.left, rc_border.top );

				::SelectObject( hdc, hpen_old );
				
				_rect rc_text = rc_cell;
				_string str_text = get_item_text( nrow, ncol );

				// [vanek] BT2000:4090 - 09.12.2004
				::InflateRect( &rc_text, -2, 0 );
				UINT ui_flags = DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER;
				if( !nrow )
					ui_flags |= DT_CENTER;
				else
					ui_flags |= DT_RIGHT;
						
        ::DrawText( hdc, (char*)str_text, str_text.length(), &rc_text, ui_flags );				
			}
		}
	}

	//restore dc
	 ::SetTextColor( hdc, clr_old_text );	

	::SetBkMode( hdc, nmode );
	::SelectObject( hdc, hfont_old );
	
	::SetMapMode( hdc, nold_map_mode );
	::SetWindowExtEx( hdc, size_old_wnd_ext.cx, size_old_wnd_ext.cy, 0 );
	::SetViewportExtEx( hdc, size_old_viewport_ext.cx, size_old_viewport_ext.cy, 0 );
	::SetViewportOrgEx( hdc, pt_old_viewport_org.x, pt_old_viewport_org.y, 0 );
	

	::DeleteObject( brush_border );	brush_border = 0;	 
	::DeleteObject( hpen_solid );	hpen_solid = 0;	 
	::DeleteObject( hpen_dash );	hpen_dash = 0;	 

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
long CStatGrid::OnODCacheInt( int nRowFrom, int nRowTo )
{
	return 1L;
}