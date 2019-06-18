#include "stdafx.h"
#include "resource.h"
#include "pfaction.h"
#include "PrintFieldAXCtl.h"
#include "EnterValuesDlg.h"
#include "\vt5\ifaces\dbase.h"
#include "\vt5\awin\misc_utils.h"
#include "\vt5\awin\misc_map.h"

#include "..\\..\\common2\\misc_calibr.h"

request_impl::request_impl()
{
	first_group = 0;
	current_id = 1000;
	m_max_extent.cx = 20;
	m_max_extent.cy = 26;

	m_size.cx = m_size.cy = 0;
	m_bUseReportPosition = false;
}

request_impl::~request_impl()
{
	group_data *p = first_group;
	while( p )
	{
		p = p->next;

		control_data	*pc = first_group->first_control;
		while( pc )
		{
			pc = pc->next;
			delete first_group->first_control;
			first_group->first_control = pc;
		}
		delete first_group;
		first_group = p;
	}
}

bool request_impl::check_data()
{
	INamedDataPtr	ptrNamedData( GetAppUnknown() );
	ptrNamedData->SetupSection( _bstr_t( "\\Values" ) );
	long	count;
	ptrNamedData->GetEntriesCount( &count );

	bool	bAllFound = true;
	for( group_data *pg = first_group; pg; pg = pg->next )		
		for( control_data *pc = pg->first_control; pc; pc = pc->next )
		{
			bool	bFound = false;
			for( long i = 0; i < count; i++ )
			{
				BSTR	bstr;
				ptrNamedData->GetEntryName( i, &bstr );
				_bstr_t	bstr_entry = bstr;
				::SysFreeString( bstr );

				char	sz[100];
//				::strencode( sz, pc->caption, ":\n\r\\", sizeof( sz ) );
				::strcpy( sz, pc->caption );

				if( !strcmp( sz, bstr_entry ) )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				bAllFound = false;
				break;
			}
		}
	return bAllFound;
}


bool request_impl::add_control( print_field_params *pparams, BSTR caption )
{
	control_data	*p = new control_data;
	::ZeroMemory( p, sizeof(control_data) );

	memcpy( &p->params, pparams, sizeof( p->params ) );
	CString	str_db;
	str_db.LoadString( IDS_DATABASE );

	p->id = current_id++;
	p->id_spin = current_id++;
	p->id_static = current_id++;
	p->caption = caption;
	p->data_format = ::GetValueInt( GetAppUnknown(), "\\RequestForPrintData", "Format", 2 );

	//find group
	group_data	*pg = first_group;
	while( pg )
	{
		if( (char*)pg->caption == str_db )
			break;
		pg = pg->next;
	}

	if( !pg )
	{
		pg = new group_data;
		pg->caption = str_db;
		pg->controls_count = 0;
		pg->first_control = 0;
		pg->next = first_group;
		first_group = pg;
	}
	//find position for insert
	p->next = pg->first_control;
	pg->first_control = p;
	pg->controls_count++;


	return true;
}

bool request_impl::add_control( print_field_params *pparams, BSTR caption, BSTR groupCaption  )
{
	control_data	*p = new control_data;
	::ZeroMemory( p, sizeof(control_data) );

	memcpy( &p->params, pparams, sizeof( p->params ) );
	CString	str_db = groupCaption;

	p->id = current_id++;
	p->id_spin = current_id++;
	p->id_static = current_id++;
	p->caption = caption;
	p->data_format = 	::GetValueInt( GetAppUnknown(), "\\RequestForPrintData", "Format", 2 );

	//find group
	group_data	*pg = first_group;
	while( pg )
	{
		if( (char*)pg->caption == str_db )
			break;
		pg = pg->next;
	}

	if( !pg )
	{
		pg = new group_data;
		pg->caption = str_db;
		pg->controls_count = 0;
		pg->first_control = 0;
		pg->next = first_group;
		first_group = pg;
	}
	//find position for insert
	p->next = pg->first_control;
	pg->first_control = p;
	pg->controls_count++;


	return true;
}


bool request_impl::add_control( IUnknown *punk )
{
	IActiveXCtrlPtr	ptrAX( punk );
	if( ptrAX == 0 )return false;

	BSTR	prog_id;
	ptrAX->GetProgID( &prog_id );

	IDispatchPtr	ptrDispatch;
	ptrDispatch.CreateInstance( prog_id );
	::SysFreeString( prog_id );
	IPrintFieldPtr	ptrPrintField = ptrDispatch;
	if( ptrPrintField == 0 )return false;

	::RestoreContainerFromDataObject( ptrAX, ptrDispatch );

	control_data	*p = new control_data;
	::ZeroMemory( p, sizeof(control_data) );
	ptrPrintField->GetParams( &p->params );

	BSTR	bstr = 0;
	_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidCaption, VT_BSTR, &bstr );

	p->caption = bstr;
	p->data_format = ::GetValueInt( GetAppUnknown(), "\\RequestForPrintData", "Format", 2 );
	if( bstr )	::SysFreeString( bstr );	bstr = 0;

	_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidGroup, VT_BSTR, &bstr );

	_bstr_t	group = bstr;
	if( bstr )	::SysFreeString( bstr );	bstr = 0;

	p->id = current_id++;
	p->id_spin = current_id++;
	p->id_static = current_id++;

	ptrAX->GetRect( &p->rect_position );

	//find group
	group_data	*pg = first_group;
	while( pg )
	{
		if( pg->caption == group )
			break;
		pg = pg->next;
	}

	if( !pg )
	{
		pg = new group_data;
		pg->caption = group;
		pg->controls_count = 0;
		pg->first_control = 0;
		pg->next = first_group;
		first_group = pg;
	}

	//find position for insert
	p->next = pg->first_control;
	pg->first_control = p;
	pg->controls_count++;


	return true;
}

const int dx_group = 10, dy_group = 20,
			dx_group_frame = 10,
			dy_group_frame = 10,
			dx_between_controls = 20,
			dy_between_controls = 6,
			dx_static_add = 5,
			dx_edit = 120,
			dx_num_edit = 70;

CSize	request_impl::total_size( int cols , int cx_min /*= 0*/ )	// [vanek]: учет минимального размер?форм?
{
	CSize	size( cx_min, 0 ); // [vanek]: учет минимального размер?форм?
	for( group_data *pg = first_group; pg; pg = pg->next )
	{
		int	c = min( pg->controls_count, cols );
		size.cx = max( size.cx, 
				c*(m_max_extent.cx+dx_edit+dx_static_add)+
				(c-1)*dx_between_controls+
				2*(dx_group_frame+dx_group) );

		c = pg->controls_count/cols+1;
		size.cy += c*m_max_extent.cy+
			(c-1)*dy_between_controls+
			2*(dy_group_frame+dy_group);
	}
	return size;
}

double request_impl::GetDisplayPixelPMM()
{
    static double fPixelPerMM = 0;
	if( fPixelPerMM == 0 )
	{
		DisplayCalibration dc;
		fPixelPerMM = dc.GetPixelPerMM();
	}

	return fPixelPerMM;
}

void request_impl::layout( CWnd *pwnd )
{
	if( m_bUseReportPosition )
	{
		layout_by_report_pos( pwnd );
		return;
	}

	{
		CClientDC	dc( pwnd );
		dc.SelectObject( pwnd->GetFont() );

		for( group_data *pg = first_group; pg; pg = pg->next )
			for( control_data *pc = pg->first_control; pc; pc = pc->next )
			{
				CSize	size = dc.GetTextExtent( pc->caption, pc->caption.length() );

				RECT rcc = { 0 };
				dc.DrawText( (char *)pc->caption, &rcc, DT_CALCRECT );

				size.cx = rcc.right - rcc.left;
				size.cy = rcc.bottom - rcc.top;

				m_max_extent.cx = max( size.cx, m_max_extent.cx );
				m_max_extent.cy = max( size.cy, m_max_extent.cy );
			}
	}

	//find optimal row count
	long	lmin = 1;
	double	fmin = 100000;
	CSize	size;

	// [vanek]: учет минимального размер?форм?
	CRect rcClient;
	rcClient.SetRectEmpty();
	if( pwnd )
		pwnd->GetClientRect( &rcClient );
	//

	for( int i = 1; i < 4; i++ )
	{
		CSize	s = total_size( i, rcClient.Width() );	// [vanek]: учет минимального размер?форм?
		double	f = 1.5-double(s.cx)/s.cy;	// фактор форм? стремимся ?соотношени?3/2
		if( f < 0 )f=-f;

		if( f < fmin )
		{
			lmin = i;
			fmin = f;
			size = s;
		}
	}
	
	int	cols_count = lmin;
	m_size.cx = size.cx;

	// [vanek]: учет минимального размер?форм?
	// расчет недостающе?длин?контрола - разность межд?реальной ?вычисленно?ширинами столбц?(столбц?вс?одинаковой ширины!)
    int ndx_add = 0; 
	if( cols_count )
		 ndx_add = max( 0, ( size.cx - ( cols_count * (m_max_extent.cx+dx_edit+dx_static_add) +
						(cols_count-1)*dx_between_controls + 2 * (dx_group_frame + dx_group) ) ) / cols_count );
	//

	long	current_y = 0;

	for( group_data *pg = first_group; pg; pg = pg->next )
	{
		pg->rect.top = current_y+dy_group_frame;
		pg->rect.left = dx_group_frame;
		pg->rect.right = size.cx-dy_group_frame;

		current_y+=dy_group+dy_group_frame;

		long	current_x = pg->rect.left+dx_group;
		long	current_col = 0;
		for( control_data *pc = pg->first_control; pc; pc = pc->next )
		{
			if( current_col == cols_count )
			{
				current_x = pg->rect.left+dx_group;
				current_col = 0;
				current_y+=dy_between_controls+m_max_extent.cy;
			}

			bool	bTextEdit = (pc->params.flags & PFF_TYPE_MASKS)==PFF_TYPE_TEXT;

			if( pc->params.flags & PFF_TEXT_STYLE_MULTYLINE )
			{
				if( current_col != 0 )
					current_y+=dy_between_controls+m_max_extent.cy;

				pc->rect_static.top = current_y;
				pc->rect_static.bottom = current_y+m_max_extent.cy;
				pc->rect_static.left = pg->rect.left+dx_group;
				pc->rect_static.right = pg->rect.right-dx_group;

			
				pc->rect_edit = pc->rect_static;
				pc->rect_edit.top = pc->rect_static.bottom+dx_static_add;
				pc->rect_edit.bottom = pc->rect_edit.top+m_max_extent.cy*2;

				current_x = pg->rect.left+dx_group;
				current_col = 0;
				current_y=pc->rect_edit.bottom;

				if( pc->next ) 
					current_y+=dy_between_controls;
				else
					current_y-=m_max_extent.cy;
				continue;

			}
			pc->rect_static.top = current_y;
			pc->rect_static.bottom = current_y+m_max_extent.cy;
			pc->rect_static.left = current_x;
			pc->rect_static.right = current_x+m_max_extent.cx;

			pc->rect_edit = pc->rect_static;
			pc->rect_edit.left = pc->rect_static.right+dx_static_add;

			// [vanek]: учет минимального размер?форм?
			//pc->rect_edit.right = pc->rect_edit.left+(bTextEdit?dx_edit:dx_num_edit);
			pc->rect_edit.right = min( pc->rect_edit.left+(bTextEdit?dx_edit:dx_num_edit) 
				+ ndx_add, pg->rect.right-dx_group );	// стараемся не выходить за крайню?правую границ?группы (для последнего столбц?
	
			current_x = pc->rect_edit.left+dx_edit+dx_between_controls;
			current_col++;
		}
		current_y+=dy_group_frame+m_max_extent.cy;

		pg->rect.bottom = current_y;
	}

	current_y+=dy_group_frame;
	m_size.cy = current_y;
}

long compare_rect(CRect rc1, CRect rc2)
{
	return 10000 * rc2.top + rc2.left - 10000 * rc1.top - rc1.left;			// vanek - sort from up to down and left to right
}

void request_impl::layout_by_report_pos( CWnd *pwnd )
{	
	CClientDC	dc( pwnd );
	dc.SelectObject( pwnd->GetFont() );

	{
		for( group_data *pg = first_group; pg; pg = pg->next )
			for( control_data *pc = pg->first_control; pc; pc = pc->next )
			{
				CSize	size = dc.GetTextExtent( pc->caption, pc->caption.length() );

				RECT rcc = { 0 };
				dc.DrawText( (char *)pc->caption, &rcc, DT_CALCRECT );

				size.cx = rcc.right - rcc.left;
				size.cy = rcc.bottom - rcc.top;

				m_max_extent.cx = max( size.cx, m_max_extent.cx );
				m_max_extent.cy = max( size.cy, m_max_extent.cy );
			}
	}

	long lMaxDlgWidth = ::GetValueInt( GetAppUnknown(), "\\RequestForPrintData", "PrintFieldDlgWidth", 800 );
	if( lMaxDlgWidth < 200 )
	{
		lMaxDlgWidth = 200;
		::SetValue( GetAppUnknown(), "\\RequestForPrintData", "PrintFieldDlgWidth", lMaxDlgWidth );
	}

	int nmax_group_y = dx_group;
	int nmax_group_x = dx_group;

	for( group_data *pg = first_group; pg; pg = pg->next )
	{
		CRect rc_group;
		rc_group.left = dx_group; 
		rc_group.top = nmax_group_y;

		_list_map_t<control_data*, CRect, compare_rect> map_ctrls;

		//add to sort map
		{
			for( control_data *pc = pg->first_control; pc; pc = pc->next )		
				map_ctrls.set( pc, pc->rect_position );

			//set line index (ny_dlg)
			int ny = 0;
			_rect rc_first_row;
			if( map_ctrls.head() )
				rc_first_row = map_ctrls.get( map_ctrls.head() )->rect_position;

			for( long lpos=map_ctrls.head(); lpos; lpos=map_ctrls.next(lpos) )
			{
				control_data* pc = map_ctrls.get( lpos );
				
				// [vanek] - 21.11.2003 : относи?контро??строке i, если абсолютное смещение ег?верхне?границ?относительно
				// вернег?кр? первог?контрола i-ой строки состав?ет не боле?3 миллиметро?

				_rect rc_intersect;
				_rect rc_move = pc->rect_position;
				rc_move.top += 3;
				rc_move.bottom += 3;
				rc_move.left = rc_first_row.left;
				rc_move.right = rc_first_row.right;
				
				if( ::IntersectRect( &rc_intersect, &rc_move, &rc_first_row ) && ( 3 >= abs((double)(rc_first_row.top - pc->rect_position.top) / GetDisplayPixelPMM( ) + 0.5) ) )
				{
					pc->ny_dlg = ny;
				}
				else
				{
					rc_first_row = pc->rect_position;
					ny++;
					pc->ny_dlg = ny;
				}
			}

		}

		//determine max group text extension
		CSize max_extent = CSize( 0, 0 );
		for( long lpos=map_ctrls.head(); lpos; lpos=map_ctrls.next(lpos) )
		{
			control_data* pc = map_ctrls.get( lpos );
			CSize	size = dc.GetTextExtent( pc->caption, pc->caption.length() );
			max_extent.cx = max( size.cx, max_extent.cx );
			max_extent.cy = max( size.cy, max_extent.cy );
		}

		long lcount_multi = 0;
		long lprev_multi = -1;

		// vanek
		// finding minimum left and top of controls coordinates
		long lmin_left = 0, lmin_top = 0;
		for( lpos=map_ctrls.head(); lpos; lpos=map_ctrls.next(lpos) )
		{
            control_data* pc = map_ctrls.get( lpos );
			if( lpos == map_ctrls.head() )
			{
				lmin_left = pc->rect_position.left;
				lmin_top = pc->rect_position.top;
			}
			else
			{
				lmin_left = min( lmin_left, pc->rect_position.left );
				lmin_top = min( lmin_top, pc->rect_position.top );
			}
		}

		// calculating offsets of controls coordinates
		long xstart = dx_group + dx_between_controls;
		long ystart = nmax_group_y + dy_group;
		
		int nxdelta = xstart - lmin_left;
		int nydelta = ystart - lmin_top;

		// offseting controls positions
		for( long lpos_move = map_ctrls.head() ; lpos_move; lpos_move=map_ctrls.next(lpos_move) )
		{
			control_data* pc_move = map_ctrls.get( lpos_move );
			pc_move->rect_position.OffsetRect( nxdelta, nydelta );
		}
		//


		for( lpos=map_ctrls.head(); lpos; lpos=map_ctrls.next(lpos) )
		{
			control_data* pc = map_ctrls.get( lpos );

			pc->rect_position.top = rc_group.top;
			pc->rect_position.top += 3 * dy_between_controls + pc->ny_dlg * ( m_max_extent.cy + dy_between_controls );

			if( lcount_multi )
			{
				long ladd_lines = lcount_multi;
				if( lprev_multi == pc->ny_dlg ) //the same line
					ladd_lines--;
				pc->rect_position.top += ladd_lines * 2*m_max_extent.cy;
			}
			
			pc->rect_position.bottom = pc->rect_position.top + m_max_extent.cy;

			CRect rc_ctrl = pc->rect_position;

			if( pc->params.flags & PFF_TEXT_STYLE_MULTYLINE )
				rc_ctrl.bottom += 2*m_max_extent.cy;
			
			CRect rc_static = rc_ctrl, rc_edit = rc_ctrl;
			if( pc->params.flags & PFF_TEXT_STYLE_MULTYLINE )
			{
				rc_static.bottom = rc_static.top + max_extent.cy + 2;
				rc_edit.top = rc_static.bottom + 2;				
				lprev_multi = pc->ny_dlg;
				lcount_multi++;
			}
			else
			{
				rc_static.right = rc_static.left + max_extent.cx + 2;
				rc_edit.left = rc_static.right + 2;
			}

			pc->rect_static	= rc_static;
			pc->rect_edit	= rc_edit;			

			nmax_group_y = max( nmax_group_y, rc_ctrl.bottom );
			nmax_group_x = max( nmax_group_x, rc_ctrl.right );
		}

		//add space to group
		nmax_group_y += 2 * dy_between_controls;

		rc_group.right	= nmax_group_x + 2 * dx_group; 
		rc_group.bottom	= nmax_group_y;

		//set group rect
		pg->rect = rc_group;
		
		//next group
		nmax_group_y += dy_group;

		//max form size
		m_size.cx = max( m_size.cx, rc_group.right );
		m_size.cy = max( m_size.cy, rc_group.bottom );
	}

	m_size.cx += dx_group;
	m_size.cy += dy_group;
}


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionRequestValuesForDB, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRequestValuesForReport, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionStoreValuesForReport, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRequestValuesForReportSD, CCmdTargetEx);

/*
//[ag]2. olecreate debug

//[ag]3. guidinfo debug

*/
//[ag]4. olecreate release

// {323B81B8-75E4-4f4e-AE7F-38B4503118BB}
GUARD_IMPLEMENT_OLECREATE(CActionRequestValuesForDB, "PrintField.RequestValuesForDB",
0x323b81b8, 0x75e4, 0x4f4e, 0xae, 0x7f, 0x38, 0xb4, 0x50, 0x31, 0x18, 0xbb);
// {66FF8703-002A-41de-B746-51A4A51C1AE4}
GUARD_IMPLEMENT_OLECREATE(CActionRequestValuesForReport, "PrintField.RequestValuesForReport",
0x66ff8703, 0x2a, 0x41de, 0xb7, 0x46, 0x51, 0xa4, 0xa5, 0x1c, 0x1a, 0xe4);
// {1DF4A1D9-C1AF-4d7a-A503-1CF38DFD0578}
GUARD_IMPLEMENT_OLECREATE(CActionStoreValuesForReport, "PrintField.CActionRequestValuesForReport", 
0x1df4a1d9, 0xc1af, 0x4d7a, 0xa5, 0x3, 0x1c, 0xf3, 0x8d, 0xfd, 0x5, 0x78);
// {80D5BAAC-0021-49c8-8B05-EF8FEE3988FF}
GUARD_IMPLEMENT_OLECREATE(CActionRequestValuesForReportSD, "PrintField.RequestValuesForReportSD",
0x80d5baac, 0x21, 0x49c8, 0x8b, 0x5, 0xef, 0x8f, 0xee, 0x39, 0x88, 0xff);

//[ag]5. guidinfo release

// {DDBC7321-61AB-499b-9FBB-CC3382B24B83}
static const GUID guidRequestValuesForDB =
{ 0xddbc7321, 0x61ab, 0x499b, { 0x9f, 0xbb, 0xcc, 0x33, 0x82, 0xb2, 0x4b, 0x83 } };
// {B7893703-29D7-4dad-A2DE-0C2F2FA01D1D}
static const GUID guidRequestValuesForReport =
{ 0xb7893703, 0x29d7, 0x4dad, { 0xa2, 0xde, 0xc, 0x2f, 0x2f, 0xa0, 0x1d, 0x1d } };
// {08922B92-7CA1-4f76-8F54-12EF3B7D83C9}
static const GUID guidStoreValuesForReport = 
{ 0x8922b92, 0x7ca1, 0x4f76, { 0x8f, 0x54, 0x12, 0xef, 0x3b, 0x7d, 0x83, 0xc9 } };
// {E86B1214-7A09-4189-9778-54849A09B5D1}
static const GUID guidRequestValuesForReporSD = 
{ 0xe86b1214, 0x7a09, 0x4189, { 0x97, 0x78, 0x54, 0x84, 0x9a, 0x9, 0xb5, 0xd1 } };

//[ag]6. action info

ACTION_INFO_FULL(CActionRequestValuesForDB, IDS_REQUEST_FOR_DB, -1, -1, guidRequestValuesForDB);
ACTION_INFO_FULL(CActionRequestValuesForReport, IDS_REQUEST_FOR_PRINT, -1, -1, guidRequestValuesForReport);
ACTION_INFO_FULL(CActionStoreValuesForReport, IDS_STORE_FOR_PRINT, -1, -1, guidStoreValuesForReport);
ACTION_INFO_FULL(CActionRequestValuesForReportSD, IDS_REQUEST_FOR_PRINTSD, -1, -1, guidRequestValuesForReporSD);

//[ag]7. targets

ACTION_TARGET(CActionRequestValuesForDB, "anydoc");
ACTION_TARGET(CActionRequestValuesForReport, szTargetApplication);
ACTION_TARGET(CActionStoreValuesForReport, szTargetApplication);
ACTION_TARGET(CActionRequestValuesForReportSD, szTargetApplication );

//[ag]8. arguments
ACTION_ARG_LIST(CActionRequestValuesForDB)
	ARG_INT(_T("ShowIfDataExist"), 1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionRequestValuesForReport)
	ARG_INT(_T("ShowIfDataExist"), 1)
	ARG_INT(_T("UseReportPosition"), 0)
	ARG_STRING(_T("DlgTitle"), "")	
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionStoreValuesForReport)
	ARG_INT(_T("ShowIfDataExist"), 1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionRequestValuesForReportSD)
	ARG_INT(_T("ShowIfDataExist"), 1)
END_ACTION_ARG_LIST()

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionRequestValuesForDB implementation
CActionRequestValuesForDB::CActionRequestValuesForDB()
{
}

CActionRequestValuesForDB::~CActionRequestValuesForDB()
{
}

bool CActionRequestValuesForDB::Invoke()
{
	bool bShow = GetArgumentInt( "ShowIfDataExist" ) != 0;

//get active document
	IDBaseDocumentPtr	ptrDBDoc;
	IApplication	*papp = GetAppUnknown();
	long	lpos_t, lpos_d;

	papp->GetFirstDocTemplPosition( &lpos_t );

	while( lpos_t )
	{
		papp->GetFirstDocPosition( lpos_t, &lpos_d );
		while( lpos_d )
		{
			IUnknown *punk = 0;
			papp->GetNextDoc( lpos_t, &lpos_d, &punk );

			ptrDBDoc = punk;
			
			punk->Release();

			if( ptrDBDoc != 0 )break;
		}
		papp->GetNextDocTempl( &lpos_t, 0, 0 );
		if( ptrDBDoc != 0 )break;
	}

	if( ptrDBDoc == 0 )
	{
		AfxMessageBox( IDS_NO_DBASE_DOCUMENT );
		return false;
	}
//enum fields
	
	IUnknown *punkQuery = 0;
	ptrDBDoc->GetActiveQuery( &punkQuery );

	if( !punkQuery )
	{
		AfxMessageBox( IDS_NO_ACTIVE_QUERY );
		return false;
	}

	punkQuery->Release();

	IDBaseStructPtr	ptrDBStruct( ptrDBDoc );
	IQueryObjectPtr	ptrQuery( punkQuery );

	int fields_count, field_no;
	ptrQuery->GetFieldsCount( &fields_count );

	bool	bOneFound = false;

	for( field_no = 0; field_no < fields_count; field_no++ )
	{
		BSTR	bstrTable, bstrField;
		BSTR	bstrCaption;

		ptrQuery->GetField( field_no, 0, &bstrTable, &bstrField );

		short	type;

		ptrDBStruct->GetFieldInfo( bstrTable, bstrField, &bstrCaption, &type, 0, 0, 0, 0, 0, 0 );

		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		if( type != ftDigit && type != ftString )
		{
			::SysFreeString( bstrCaption );
			continue;
		}
		
		print_field_params	params;
		ZeroMemory( &params, sizeof( params ) );

		params.flags = ((type == ftDigit)?PFF_TYPE_NUMERIC:PFF_TYPE_TEXT);
		bOneFound |= add_control( &params, bstrCaption );
		::SysFreeString( bstrCaption );
	}


	if( !bShow )
		if( check_data() )
			return true;

	if( !bOneFound )
	{
		AfxMessageBox( IDS_NO_DBASECONTROLS_FOUND );
		return false;
	}

	CEnterValuesDlg	dlg( this );
	return dlg.DoModal()==IDOK;
	
	return true;
}

//extended UI
bool CActionRequestValuesForDB::IsAvaible()
{
	return true;
}

bool CActionRequestValuesForDB::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionRequestValuesForReport implementation
CActionRequestValuesForReport::CActionRequestValuesForReport()
{
}

CActionRequestValuesForReport::~CActionRequestValuesForReport()
{
}

static void InvalidateAllChildren(HWND hwndParent)
{
	InvalidateRect(hwndParent, NULL, TRUE);
	HWND hwnd = ::GetWindow(hwndParent, GW_CHILD);
	while(hwnd)
	{
		InvalidateAllChildren(hwnd);
		hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
	}
}

bool CActionRequestValuesForReport::Invoke()
{
	bool bShow				= GetArgumentInt( "ShowIfDataExist" ) != 0;
	m_bUseReportPosition	= GetArgumentInt( "UseReportPosition" ) != 0;
	CString str_title		= GetArgumentString( _T("DlgTitle") );
	

//get the current print report - from print or document
	IUnknown *(*pfnCreateReport)(IUnknown*) = 0;
	HMODULE	h = GetModuleHandle( "print.dll" );

	if( h )pfnCreateReport = (IUnknown*(*)(IUnknown*))::GetProcAddress( h, "CreateReport" );

	IApplicationPtr ptr_app( GetAppUnknown() );
	IUnknownPtr ptr_doc;
	if( ptr_app )
		ptr_app->GetActiveDocument( &ptr_doc );

	IUnknown	*punkReport = 0;
	if( pfnCreateReport )punkReport = pfnCreateReport( ptr_doc );
	if( !punkReport )punkReport = ::GetActiveObjectFromDocument( ptr_doc, szArgumenReportForm );

	if( !punkReport )
	{
		AfxMessageBox( IDS_CANT_LOAD_REPORT );
		return false;
	}

	INamedDataObject2Ptr	ptrNamed( punkReport );
	punkReport->Release();

//enum its object and scan for PrintField
	bool		bOneFound = false;
	long		lpos;
	CPtrArray	controls;
	
	ptrNamed->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown	*punkChild = 0;
		ptrNamed->GetNextChild( &lpos, &punkChild );
		controls.Add( punkChild );
	}

	//sort it
	int	i, j, opt;
	CPoint	point;

	for( i = 0; i < controls.GetSize()-1; i++ )
	{
		IActiveXCtrlPtr	ptrAX( (IUnknown*)controls[i] );
		opt = i;
		CRect	rect;
		ptrAX->GetRect( &rect );
		point = rect.CenterPoint();


		for( j = i+1; j < controls.GetSize(); j++ )
		{
			IActiveXCtrlPtr	ptrAX( (IUnknown*)controls[j] );
			ptrAX->GetRect( &rect );

			if( rect.CenterPoint().y > point.y || 
				(rect.CenterPoint().y == point.y && rect.CenterPoint().x > point.x ) )
			{
				point = rect.CenterPoint();
				opt = j;
			}
		}

		if( opt != i )
		{
			void *p = controls[i];
			controls[i] = controls[opt];
			controls[opt] = p;
		}
	}


	for( i = 0; i < controls.GetSize(); i++ )
	{
		IUnknown *punkChild = (IUnknown*)controls[i];
		IActiveXCtrlPtr	ptrAX( punkChild );
		punkChild->Release();

		if( ptrAX == 0 )continue;
		BSTR	bstr = 0;
		ptrAX->GetProgID( &bstr );

		_bstr_t	prog_id = bstr;
		if( bstr )	::SysFreeString( bstr );	bstr = 0;

		if( stricmp( prog_id, "PRINTFIELDAX.PrintFieldAXCtrl.1") )
			continue;

		bOneFound |= add_control( ptrAX );
	}

	if( !bOneFound )
	{
		SetValue(GetAppUnknown(), "\\RequestForPrint", "LastError", controls.GetSize()?1L:2L);
		if (GetValueInt(GetAppUnknown(), "\\RequestForPrint", "ShowMessageWhenNoFieldFound", TRUE))
			AfxMessageBox( IDS_NOFIELDAXFOUND );
		return false;
	}

	if( !bShow )
		if( check_data() )
			return true;

	CEnterValuesDlg	dlg( this );
	if( str_title.GetLength() )
		dlg.SetTitle( str_title );

	bool b = dlg.DoModal()==IDOK;
	if (b)
	{
		IDocumentSitePtr sptrDocSite(ptr_doc);
		if (sptrDocSite != 0)
		{
			long lPos = 0;
			sptrDocSite->GetFirstViewPosition(&lPos);
			while (lPos)
			{
				IUnknownPtr punkView;
				sptrDocSite->GetNextView(&punkView, &lPos);
				IWindowPtr sptrWndVw(punkView);
				if (sptrWndVw != 0)
				{
					HANDLE h;
					sptrWndVw->GetHandle(&h);
					InvalidateAllChildren((HWND)h);
				}
			}
		}
	}
	return b;
}

//extended UI
bool CActionRequestValuesForReport::IsAvaible()
{
	return true;
}

bool CActionRequestValuesForReport::IsChecked()
{
	return false;
}



//////////////////////////////////////////////////////////////////////
//CActionStoreValuesForReport implementation
CActionStoreValuesForReport::CActionStoreValuesForReport()
{
}

CActionStoreValuesForReport::~CActionStoreValuesForReport()
{
}

bool CActionStoreValuesForReport::Invoke()
{
	bool bShow = GetArgumentInt( "ShowIfDataExist" ) != 0;

//get the current print report - from print or document
	IUnknown *(*pfnCreateReport)(IUnknown*) = 0;
	HMODULE	h = GetModuleHandle( "print.dll" );

	if( h )pfnCreateReport = (IUnknown*(*)(IUnknown*))::GetProcAddress( h, "CreateReport" );

	IApplicationPtr ptr_app( GetAppUnknown() );
	IUnknownPtr ptr_doc;
	if( ptr_app )
		ptr_app->GetActiveDocument( &ptr_doc );

	IUnknown	*punkReport = 0;
	if( pfnCreateReport )punkReport = pfnCreateReport( ptr_doc );
	if( !punkReport )punkReport = ::GetActiveObjectFromDocument( ptr_doc, szArgumenReportForm );

	if( !punkReport )
	{
		AfxMessageBox( IDS_CANT_LOAD_REPORT );
		return false;
	}

	INamedDataObject2Ptr	ptrNamed( punkReport );
	punkReport->Release();

//enum its object and scan for PrintField
	bool		bOneFound = false;
	long		lpos;
	CPtrArray	controls;
	
	ptrNamed->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown	*punkChild = 0;
		ptrNamed->GetNextChild( &lpos, &punkChild );
		controls.Add( punkChild );
	}

	//sort it
	int	i, j, opt;
	CPoint	point;

	for( i = 0; i < controls.GetSize()-1; i++ )
	{
		IActiveXCtrlPtr	ptrAX( (IUnknown*)controls[i] );
		opt = i;
		CRect	rect;
		ptrAX->GetRect( &rect );
		point = rect.CenterPoint();


		for( j = i+1; j < controls.GetSize(); j++ )
		{
			IActiveXCtrlPtr	ptrAX( (IUnknown*)controls[j] );
			ptrAX->GetRect( &rect );

			if( rect.CenterPoint().y > point.y || 
				(rect.CenterPoint().y == point.y && rect.CenterPoint().x > point.x ) )
			{
				point = rect.CenterPoint();
				opt = j;
			}
		}

		if( opt != i )
		{
			void *p = controls[i];
			controls[i] = controls[opt];
			controls[opt] = p;
		}
	}


	for( i = 0; i < controls.GetSize(); i++ )
	{
		IUnknown *punkChild = (IUnknown*)controls[i];
		IActiveXCtrlPtr	ptrAX( punkChild );
		punkChild->Release();

		if( ptrAX == 0 )continue;
		BSTR	bstr = 0;
		ptrAX->GetProgID( &bstr );

		_bstr_t	prog_id = bstr;
		if( bstr )	::SysFreeString( bstr );	bstr = 0;		

		if( stricmp( prog_id, "PRINTFIELDAX.PrintFieldAXCtrl.1") )
			continue;

		bOneFound |= add_control( ptrAX );
	}

	if( !bOneFound )
	{
		SetValue(GetAppUnknown(), "\\RequestForPrint", "LastError", controls.GetSize()?1L:2L);
		if (GetValueInt(GetAppUnknown(), "\\RequestForPrint", "ShowMessageWhenNoFieldFound", TRUE))
			AfxMessageBox( IDS_NOFIELDAXFOUND );
		return false;
	}

	if( !bShow )
		if( check_data() )
			return true;

	CEnterValuesDlg	dlg( this );
	dlg.Create(IDD_ENTER_VALUES);
	//dlg.EndDialog(IDOK);
	//dlg.DoOK();

	//return dlg.DoModal()==IDOK;
	return true;
}

//extended UI
bool CActionStoreValuesForReport::IsAvaible()
{
	return true;
}

bool CActionStoreValuesForReport::IsChecked()
{
	return false;
}

/**********/
//////////////////////////////////////////////////////////////////////
//CActionRequestValuesForReport implementation
CActionRequestValuesForReportSD::CActionRequestValuesForReportSD()
{
}

CActionRequestValuesForReportSD::~CActionRequestValuesForReportSD()
{
}

bool CActionRequestValuesForReportSD::Invoke()
{
	bool bShow = GetArgumentInt( "ShowIfDataExist" ) != 0;

//enum fields
	bool	bOneFound = false;

	INamedDataPtr sptrND = ::GetAppUnknown();
	sptrND->SetupSection( _bstr_t( "\\RequestForPrintData" ) );


	_variant_t varTitle;
	sptrND->GetValue( _bstr_t( "Title" ), &varTitle );

	_bstr_t bstrTitle = varTitle;

	long fields_count = 0;
// 	sptrND->GetEntriesCount( &fields_count );

	_variant_t varCount = 0L;
	sptrND->GetValue( _bstr_t( "Count" ), &varCount );
	
	fields_count = varCount;


	for( long field_no = 0; field_no < fields_count; field_no++ )
	{
		print_field_params	params;
		ZeroMemory( &params, sizeof( params ) );

		CString strName;
		CString strType;
		CString strGroup;

		strName.Format( "%s%d", "Name", field_no );
		strType.Format( "%s%d", "Type", field_no );
		strGroup.Format( "%s%d", "Group", field_no );

		_variant_t varCaption;
		sptrND->GetValue( _bstr_t( strName ), &varCaption );

		_variant_t varType;
		sptrND->GetValue( _bstr_t( strType ), &varType );
		params.flags = (long)varType;

		_variant_t varGrCaption;
		sptrND->GetValue( _bstr_t( strGroup ), &varGrCaption );

		bOneFound |= add_control( &params, _bstr_t( varCaption ), _bstr_t( varGrCaption ) );
	}


	if( !bShow )
		if( check_data() )
			return true;

	if( !bOneFound )
	{
		AfxMessageBox( IDS_NOFIELDAXFOUND2 );
		return false;
	}


	CEnterValuesDlg	dlg( this );

	dlg.SetTitle( (char*)bstrTitle );
	return dlg.DoModal()==IDOK;
	
	return true;
}

//extended UI
bool CActionRequestValuesForReportSD::IsAvaible()
{
	return true;
}

bool CActionRequestValuesForReportSD::IsChecked()
{
	return false;
}