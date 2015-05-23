#include "stdafx.h"
#include "image_bar.h"
#include "resource.h"

#include "script_int.h"
#include <activscp.h>
#include "afxpriv.h"
#include "MenuRegistrator.h"

#include "scriptnotifyint.h"
#include "PropBag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//shell data values
#define IMAGEBAR_SECTION		"\\ImageBar"
#define IMAGEBAR_NAME			"ImageBar"
#define IMAGEBAR_WIDTH			"ItemWidth"
#define IMAGEBAR_HEIGHT			"ItemHeight"
#define IMAGEBAR_SYSCOLORS      "UseSystemColors"
#define IMAGEBAR_BACKCOLOR		"BackColor"
#define IMAGEBAR_HIGHLIGHTCOLOR	"HighlightColor"
#define IMAGEBAR_DRAWTEXT	"DrawText"
#define IMAGEBAR_DRAWICON		"DrawIcon"
#define IMAGEBAR_ENABLEDOCK	"EnableDock"
#define IMAGEBAR_ICONPROPNAME	"IconPropName"

/////////////////////////////////////////////////////////////////////////////
// vanek  27.08.2003
int	isdigitc( int c )
{
	return (c >= '0') && (c <= '9');
}

/////////////////////////////////////////////////////////////////////////////
bool parse_name( const char* psz_name, CString& str_name, long& lcount ) 
{
	if( !psz_name )	return false;

	char sz_buf[256];	sz_buf[0] = 0;
	if( strlen( psz_name ) >= sizeof(sz_buf) )	return false;

	strcpy( sz_buf, psz_name );

	int nlen = strlen( sz_buf );
	char* psz_digit = sz_buf + nlen;
	for( int i=nlen-1;i>=0;i-- )
	{
		if( isdigitc( *(sz_buf + i) ) )
			psz_digit--;
		else
			break;
	}

	//default
	lcount = -1;
	str_name = sz_buf;
	
	if( psz_digit && strlen( psz_digit ) )
	{
		lcount = atoi(psz_digit);
		*psz_digit = 0;
		str_name = sz_buf;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// vanek  26.08.2003
int		compare_image_info( const void *arg1, const void *arg2 )	// for qsort function
{
	image_info	*p1 = *(image_info**) arg1,
				*p2 = *(image_info**) arg2;
	
	if( !p1 || !p2)		return 0;

	if( (p1->ptr_image == 0 && p1->sptr_storage == 0) || (p2->ptr_image == 0 && p2->sptr_storage == 0) )
		return 0;
	
	int ncmp = stricmp( p1->m_str_short_name, p2->m_str_short_name );
	if( !ncmp )
		ncmp = p1->m_nnumber - p2->m_nnumber;
	if( !ncmp )
		ncmp = p1->nstorage_record - p2->nstorage_record;

	return ncmp;
}

/////////////////////////////////////////////////////////////////////////////
CString _GetObjectName( image_info* pii )
{
	if( !pii || ( (pii->ptr_image == 0) && (pii->sptr_storage == 0) ) )
		return "";

	if( pii->ptr_image )
		return ::GetObjectName( pii->ptr_image );
	
	// vanek 01.08.2003
	if( pii->sptr_storage )
	{
		long lOldCurrRecord = 0;

		pii->sptr_storage->GetCurrentRecord( &lOldCurrRecord );
		
		if( S_OK != pii->sptr_storage->SetCurrentRecord( pii->nstorage_record, asAbortObjectCountCalculation ) )
			return "";

        BSTR bstr_obj_name = 0;
		if( S_OK != pii->sptr_storage->GetObjectName( _bstr_t(szTypeImage), &bstr_obj_name ) )
			return "";

		CString obj_name( bstr_obj_name );

		if( bstr_obj_name )
		{
			::SysFreeString( bstr_obj_name );
			bstr_obj_name = 0;
		}


		pii->sptr_storage->SetCurrentRecord( lOldCurrRecord, 0 );

		//return ::GetObjectName( pii->sptr_storage );

		return obj_name;
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////
// vanek 01.08.2003
bool _IsNameAviable( CStringArray *pFilter, CString *pstr_name )
{
	if( !pFilter || !pstr_name )
		return false;

	bool bAviable = true;
	if( pFilter->GetSize() )
	{
		bAviable = false;
		const char* psz_str = (const char*)(*pstr_name);
		for( int i=0;i<pFilter->GetSize();i++ )
		{
			char* psz_find = (char*)strstr( psz_str, (*pFilter)[i] );
			if( psz_find && psz_find == psz_str )
			{
				bAviable = true;
				break;
			}
		}
	}

	return bAviable;
}

/////////////////////////////////////////////////////////////////////////////
// image_bar
IMPLEMENT_DYNCREATE(image_bar, CWnd)
image_bar::image_bar()
{
	EnableAutomation();
	EnableAggregation();

	_OleLockApp( this );

	m_sName = IMAGEBAR_NAME;
	m_sUserName.LoadString( IDS_IMAGE_BAR );

	m_str_icon_prop_name = _T("");

	m_bDrawIcon = true;
	m_bDrawText	= true;
	m_bUseSysColors		= FALSE;

	//calc item offset
	m_rect_offset		= NORECT;	
	_ReadParams();

	//active item
	m_nactive_item		= -1;	

	//register on app
	register_on_controller( 0 );

	m_bchange_context	= true;
	m_str_name_filter	= "";

	m_block_sync		= false;
}

/////////////////////////////////////////////////////////////////////////////
image_bar::~image_bar()
{
	_OleUnlockApp( this );

	unregister_on_controller( 0 );
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnFinalRelease()
{
	//CWnd::OnFinalRelease();

	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;	
}

BEGIN_MESSAGE_MAP(image_bar, CWnd)
	ON_WM_NCDESTROY()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// Note: we add support for IID_Iimage_bar to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {BBFE74A9-1C9A-4AC6-8794-2D4D7DF0254C}
static const IID IID_Iimage_bar =
{ 0xBBFE74A9, 0x1C9A, 0x4AC6, { 0x87, 0x94, 0x2D, 0x4D, 0x7D, 0xF0, 0x25, 0x4C } };

BEGIN_INTERFACE_MAP(image_bar, CWnd)
	INTERFACE_PART(image_bar, IID_Iimage_bar,		Dispatch	)
	INTERFACE_PART(image_bar, IID_IWindow,			Wnd			)
	INTERFACE_PART(image_bar, IID_IDockableWindow,	Dock		)
	INTERFACE_PART(image_bar, IID_IRootedObject,	Rooted		)
	INTERFACE_PART(image_bar, IID_IEventListener,	EvList		)
	INTERFACE_PART(image_bar, IID_INamedObject2,	Name		)
	INTERFACE_PART(image_bar, IID_IMsgListener,		MsgList		)
	INTERFACE_PART(image_bar, IID_IHelpInfo,		Help		)
END_INTERFACE_MAP()

// {2D98D0CD-3FA2-4D08-84CE-9E0733F5AD87}
GUARD_IMPLEMENT_OLECREATE(image_bar, "DataUI.ImageBar", 0x2d98d0cd, 0x3fa2, 0x4d08, 0x84, 0xce, 0x9e, 0x7, 0x33, 0xf5, 0xad, 0x87)

BEGIN_DISPATCH_MAP(image_bar, CWnd)
	//{{AFX_DISPATCH_MAP(data_context_ctrl)
	DISP_PROPERTY_EX(image_bar, "ChangeContext", GetChangeContext, SetChangeContext, VT_BOOL)
	DISP_FUNCTION(image_bar, "Refresh", Refresh, VT_EMPTY, VTS_NONE)	
	DISP_PROPERTY_EX(image_bar, "NameFilter", GetNameFilter, SetNameFilter, VT_BSTR)
	DISP_FUNCTION(image_bar, "GetObjectInfoByIndex", GetObjectInfoByIndex, VT_BSTR, VTS_I4 VTS_PVARIANT VTS_PVARIANT)	
	DISP_PROPERTY_EX(image_bar, "ActiveItem", GetActiveItem, SetActiveItem, VT_I4)
	DISP_FUNCTION(image_bar, "Invalidate", Invalidate, VT_EMPTY, VTS_NONE)	
	DISP_FUNCTION(image_bar, "GetItemCount", GetItemCount, VT_I4, VTS_NONE)
	DISP_PROPERTY_EX(image_bar, "LockSync", GetLockSync, SetLockSync, VT_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
int image_bar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;


	//get render man
	m_ptr_thumb_man = ::GetAppUnknown();

	//load icons
	m_imagelist.Create( MAKEINTRESOURCE( IDB_IMAGEBAR ), 16, 0, RGB(255,255,255) );

	CString	strScrollClass = GetValueString( GetAppUnknown(), "\\Interface", "ScrollClass", "SCROLLBAR" );
	
	
	m_phrzScoll = new CScrollBar;
	m_pvrtScoll = new CScrollBar;

	m_phrzScoll->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_HORZ, CRect( 0, 0, 100, 100 ), this, 100 );
	m_pvrtScoll->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_VERT, CRect( 0, 0, 100, 100 ), this, 104 );


	//correct offset
	{
		m_rect_offset.left		= 10;
		m_rect_offset.right		= 10;
		m_rect_offset.top		= 10;
		m_rect_offset.bottom	= 10;

		m_rect_offset_without_text = m_rect_offset;

		CClientDC dc(0);
		LOGFONT lf = {0};
		{
			CFont* pfont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
			if( pfont->GetLogFont( &lf ) )
				m_font.CreateFontIndirect( &lf );
		}
		CFont* pfont_old = dc.SelectObject( &m_font );		

		TEXTMETRIC tm = {0};
		dc.GetTextMetrics( &tm );
		dc.SelectObject( pfont_old );

		m_rect_offset.bottom = min( tm.tmHeight, 16 /*ico*/) + 15;

		m_rect_offset_with_text = m_rect_offset;
	}
	m_rect_offset = (m_bDrawText || m_bDrawIcon) ? m_rect_offset_with_text : m_rect_offset_without_text;

	_ReadParams();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::register_on_controller( IUnknown* punk_ctrl )
{
	Register( punk_ctrl );	
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::unregister_on_controller( IUnknown* punk_ctrl )
{
	UnRegister( punk_ctrl );
}


// image_bar message handlers
/////////////////////////////////////////////////////////////////////////////
CWnd* image_bar::GetWindow()
{
	return this;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	context_listener::on_notify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if( !strcmp( pszEvent, szEventChangeObject ) )	
	{
		if( GetObjectKind( punkFrom ) == szTypeImage );
			Invalidate();
	}
	else if( !strcmp( pszEvent, szEventActivateObject ) && 
		GetObjectKind( punkHit ) == szTypeImage )
	{
		sync_activity( );
	}else if( !strcmp( pszEvent, "AppInit" ) )
	{
		//add to app && add named item to script engine
		ICompManagerPtr ptr_cm = GetAppUnknown();
		if( ptr_cm )
		{
			ptr_cm->AddComponent( GetControllingUnknown(), 0 );
			IApplicationPtr	ptr_app( GetAppUnknown() );
			if( ptr_app )
			{
				IUnknown* punk_se = 0;
				ptr_app->GetActiveScript( &punk_se );
				if( punk_se )
				{
					IScriptEnginePtr ptr_se( punk_se );
					punk_se->Release();	punk_se = 0;
					if( ptr_se )
					{
						IUnknown* punk_as = 0;
						ptr_se->GetActiveScript( &punk_as );
						if( punk_as )
						{
							IActiveScriptPtr ptr_as( punk_as );
							punk_as->Release();
							if( ptr_as )
								ptr_as->AddNamedItem( _bstr_t(IMAGEBAR_NAME), SCRIPTITEM_ISVISIBLE );
						}
					}
				}
			}
		}
	}
	else if(!strcmp(pszEvent, szEventNewSettings))
    {
		_ReadParams();
		Invalidate();
    }
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view )
{
	if( !context_listener::_on_activate_view( punk_doc, punk_view ) )
		return false;

	// A.M. fix BT4787 - do not renew image bar when it is not visible.
	BOOL bVisible = m_hWnd!=0&&IsWindowVisible();
	if (bVisible)
	{
	sync_info( );
	sync_activity( );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::_on_close_document( IUnknown* punk_doc )
{
	if( !context_listener::_on_close_document( punk_doc ) )
		return false;
	
	destroy_info( );
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::_on_destroy_view( IUnknown* punk_view )
{
	if( !context_listener::_on_destroy_view( punk_view ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::_on_data_context_change( IUnknown* punk_view, IUnknown* punk_obj, NotifyCodes nc )
{
	// A.M. fix BT4787 - do not renew image bar when it is not visible.
	BOOL bVisible = m_hWnd!=0&&IsWindowVisible();
	// vanek - 28.08.2003
	if( nc == ncRescan && bVisible)
	{
		sync_info( );
		return true;
	}
	//

	if( !context_listener::_on_data_context_change( punk_view, punk_obj, nc ) )
		return false;

	if (!bVisible)
		return true;

	if( nc == ncAddObject || nc == ncRemoveObject && (!stricmp( ::GetObjectKind( punk_obj ), szTypeImage ) || !stricmp( ::GetObjectKind( punk_obj ), szTypeStorageObject ) ) )
		sync_info( );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::sync_info( )
{
	if( m_block_sync )
		return true;

	_bstr_t bstr_image = szTypeImage;
	// vanek 01.08.2003
	_bstr_t bstrt_storage = szTypeStorageObject;

	destroy_info( false );
	// vanek 01.08.2003
	m_map_image_names_in_doc.clear( );

	CStringArray arr;
	int nfilter_len = m_str_name_filter.GetLength();
	if( nfilter_len )
	{
		char sz_buf[255];	sz_buf[0] = 0;
		if( nfilter_len >= sizeof(sz_buf) )
			return false;

		strcpy( sz_buf, (const char*)m_str_name_filter );

		char* psz_token = strtok( sz_buf, ";" );
		while( psz_token )
		{
			arr.Add( psz_token );
			psz_token = strtok( 0, ";" );
		}
	}

	IDataContext3Ptr ptr_dc = m_ptr_active_view;
	if( ptr_dc != 0  )
	{
		//fill list
		LONG_PTR lpos = 0;		
		ptr_dc->GetFirstObjectPos( bstr_image, &lpos );
		while( lpos )
		{
			IUnknown* punk = 0;
			ptr_dc->GetNextObject( bstr_image, &lpos, &punk );

			image_info* pi = new image_info;
			pi->ptr_image = punk;
			punk->Release();	punk = 0;

			// [vanek] 01.08.2003
			CString str_name = _GetObjectName( pi );
			bool bAviable = false;
			bAviable = _IsNameAviable( &arr, &str_name );
			if( !bAviable )
			{
				delete pi;	pi = 0;
				continue;
			}

			m_list_image.add_tail( pi );			
			// vanek 01.08.2003
			m_map_image_names_in_doc.set( 0, _GetObjectName( pi ) );
		}

		// vanek 01.08.2003
		lpos = 0;		
		ptr_dc->GetFirstObjectPos( bstrt_storage, &lpos );
		while( lpos )
		{
			IUnknown* punk = 0;
			ptr_dc->GetNextObject( bstrt_storage, &lpos, &punk );
            IStorageObject *ptr_storage = 0;
			punk->QueryInterface( IID_IStorageObject, (void**) &ptr_storage );
			punk->Release();	punk = 0;
			if( !ptr_storage )
				continue;
				
			long lRecordCount = 0;
			ptr_storage->GetRecordCount( &lRecordCount );

			// vanek 28.08.2003
			long lExportRecord = -1;
			ptr_storage->GetExportRecord( &lExportRecord );

			long lOldCurrRecord = 0;
			ptr_storage->GetCurrentRecord( &lOldCurrRecord );

				
			for( long lrecord = 1; lrecord <= lRecordCount; lrecord ++ )
			{
				// vanek 28.08.2003
				if( lExportRecord == lrecord  )
					continue;	// skip record
				
				if( S_OK != ptr_storage->SetCurrentRecord( lrecord, asAbortObjectCountCalculation ) )
					continue;

				BSTR bstr_name = 0;
                if( S_OK != ptr_storage->GetObjectName( bstr_image, &bstr_name ) )
					continue;

				if( _IsNameAviable( &arr, &CString(bstr_name) ) /*&& !m_map_image_names_in_doc.find( CString(bstr_name) ) vanek 28.08.2003*/ )
				{	
                 	image_info *pi = new image_info;
					pi->sptr_storage = ptr_storage;
					pi->nstorage_record = lrecord;

					// [vanek]: get image's index - 09.04.2004
					if( !m_str_icon_prop_name.IsEmpty( ) )
					{
						IStorageObjectMiscPtr sptr_storage_misc = ptr_storage;
						if( sptr_storage_misc != 0 )
						{
							IUnknown *punk_propbag = 0;
							sptr_storage_misc->GetObjectPropBag( bstr_image, &punk_propbag );
							INamedPropBagPtr sptr_prop_bag = punk_propbag;
							if( punk_propbag )
								punk_propbag->Release( ); punk_propbag = 0;

							_variant_t var;
							if( S_OK == sptr_prop_bag->GetProperty( _bstr_t( m_str_icon_prop_name ), var.GetAddress( ) ) )
								pi->m_licon_index = var.vt == VT_EMPTY ? -1 : var;
						}
					}

                    m_list_image.add_tail( pi );
				}

				if( bstr_name )
				{
					::SysFreeString( bstr_name );
					bstr_name = 0;
				}
			}
		
			ptr_storage->SetCurrentRecord( lOldCurrRecord, 0 );

			ptr_storage->Release( );
			ptr_storage = 0;
		}
	}

	//sort items
	// vanek - 26.08.2003
    {
		long lCount = m_list_image.count();
		image_info **ppinfo = 0;
		ppinfo = new image_info*[lCount];
		{
			long lIndex = 0;
			for (TPOS lpos = m_list_image.head(); lpos && (lIndex < lCount);
				lpos = m_list_image.next(lpos), lIndex++)
			{
				image_info *pinfo = m_list_image.get(lpos);
				ppinfo[lIndex] = pinfo;
				parse_name(_GetObjectName(pinfo), pinfo->m_str_short_name, (long&)pinfo->m_nnumber);
			}
		}

		qsort((void *)ppinfo, (size_t)lCount, sizeof(image_info*), compare_image_info);
		{
			long  lIndex = 0;
			for (TPOS lpos = m_list_image.head(); lpos && (lIndex < lCount); lpos = m_list_image.next(lpos), lIndex++)
				m_list_image.set(ppinfo[lIndex], lpos);
		}

		if( ppinfo )
		{
			delete[] ppinfo;
			ppinfo = 0;
		}
	}
	/*for( long lpos1=m_list_image.head(); lpos1; lpos1=m_list_image.next(lpos1) )
	{
		if( lpos1 == m_list_image.tail() )
			continue;

		image_info* pinfo1 = m_list_image.get( lpos1 );

		for( long lpos2=m_list_image.next(lpos1); lpos2; lpos2=m_list_image.next(lpos2) )
		{
			image_info* pinfo2 = m_list_image.get( lpos2 );

			if( compare_image_info( pinfo1, pinfo2 ) > 0 )
			{
				m_list_image.set( pinfo2, lpos1  );
				m_list_image.set( pinfo1, lpos2  );				

				pinfo2 = m_list_image.get( lpos2 );
				pinfo1 = m_list_image.get( lpos1 );
			}
		}
	}  */

	set_client_size( );

	Invalidate();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::sync_activity( )
{
	IDataContext3Ptr ptr_dc = m_ptr_active_view;
	if( ptr_dc == 0 )	return false;
	_bstr_t bstr_image = szTypeImage;

	//calc active item		
	long nactive_item = -1;
	GUID guid_sel = INVALID_KEY;
	{
		IUnknown* punk = ::GetActiveParentObjectFromContext( ptr_dc, szTypeImage );
		if( punk )
		{
			guid_sel = ::GetObjectKey( punk );
			punk->Release();	punk = 0;
		}
	}

	long lidx = 0;
	for( TPOS lpos=m_list_image.head(); lpos; lpos=m_list_image.next(lpos) )
	{
		image_info* pi = m_list_image.get( lpos );
		if( pi->ptr_image != 0 && ::GetObjectKey( pi->ptr_image ) == guid_sel )
		{
			nactive_item = lidx;
			break;
		}
		lidx++;
	}

	if( nactive_item != m_nactive_item )
	{
		m_nactive_item = nactive_item;
		Invalidate();
	}

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::destroy_info( bool bset_client_size )
{
	m_list_image.clear();
	
	if( bset_client_size )
		set_client_size();
    
	// [vanek] SBT:912 - 12.04.2004
	Invalidate();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
long image_bar::get_item_from_point( CPoint pt_client )
{
	CRect rc_item = get_item_rect( 0, true );
	if( !rc_item.Width() )
		return -1;

	//calc appropriate rect
	int nitem = (pt_client.x - rc_item.left) / rc_item.Width();
	rc_item = get_item_rect( nitem, true );
	if( rc_item.PtInRect( pt_client ) )
		return nitem;

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
long image_bar::get_item_count()
{
	return m_list_image.count();
}

/////////////////////////////////////////////////////////////////////////////
CRect image_bar::get_item_rect( long nitem, bool bfull )
{
	CRect rc;
	int nwidth	= m_rect_offset.left + m_nitem_width + m_rect_offset.right;
	int nheight	= m_rect_offset.top + m_nitem_height + m_rect_offset.bottom;

	rc.left		= nitem * nwidth;
	rc.right	= rc.left + nwidth;	
	rc.top		= 0;
	rc.bottom	= nheight;

	if( !bfull )
		rc.DeflateRect( &m_rect_offset );

	rc += m_ptBarOffset;

	return rc;
}

/////////////////////////////////////////////////////////////////////////////
image_info* image_bar::get_image_info_by_idx( long lidx )
{
	long litem_idx = 0;
	for( TPOS lpos=m_list_image.head(); lpos; lpos=m_list_image.next(lpos) )	
	{
		if( lidx == litem_idx)
			return m_list_image.get(lpos);

		litem_idx++;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CRect image_bar::convert_from_client_to_wnd( CRect rc )
{
	CRect rc_wnd = rc;
	CPoint pt_scroll = get_scroll_pos();
	rc_wnd.left		-= pt_scroll.x;
	rc_wnd.right	-= pt_scroll.x;
	rc_wnd.top		-= pt_scroll.y;
	rc_wnd.bottom	-= pt_scroll.y;

	return rc_wnd;
}

/////////////////////////////////////////////////////////////////////////////
CRect image_bar::convert_from_wnd_to_client( CRect rc )
{
	CRect rc_client = rc;
	CPoint pt_scroll = get_scroll_pos();
	rc_client.left		+= pt_scroll.x;
	rc_client.right		+= pt_scroll.x;
	rc_client.top		+= pt_scroll.y;
	rc_client.bottom	+= pt_scroll.y;

	return rc_client;
}

/////////////////////////////////////////////////////////////////////////////
CPoint image_bar::convert_from_client_to_wnd( CPoint pt )
{
	CPoint pt_wnd = pt;
	CPoint pt_scroll = get_scroll_pos();
	pt_wnd.x	-= pt_scroll.x;
	pt_wnd.y	-= pt_scroll.y;

	return pt_wnd;
}

/////////////////////////////////////////////////////////////////////////////
CPoint image_bar::convert_from_wnd_to_client( CPoint pt )
{
	CPoint pt_client = pt;
	CPoint pt_scroll = get_scroll_pos();
	pt_client.x	+= pt_scroll.x;
	pt_client.y	+= pt_scroll.y;

	return pt_client;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::set_client_size( )
{
	CSize size	= get_client_size( );
	RECT rcClient = {0};
	GetClientRect( &rcClient );

	SCROLLINFO si = {0};
	si.cbSize = sizeof( SCROLLINFO );
	si.fMask = SIF_RANGE | SIF_PAGE;
	//si.nPage = rc.bottom;	
	
	si.nMax = size.cx + rcClient.right % get_item_rect( 0, true ).Width() - 1;		
	si.nPage = rcClient.right - rcClient.left;

	SCROLLINFO si_old = {0};
	si_old.cbSize = sizeof( SCROLLINFO );
	si_old.fMask = SIF_RANGE | SIF_PAGE;
	if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si_old );
    
	if( si_old.nMax != si.nMax || si_old.nPage != si.nPage )
		if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_SETSCROLLINFO, true, (LPARAM)&si );
	
	si.nMax = size.cy;	
	si.nPage = rcClient.bottom;
	si_old.fMask = SIF_RANGE | SIF_PAGE;
	if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si_old );

	if( si_old.nMax != si.nMax || si_old.nPage != si.nPage )
		if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_SETSCROLLINFO, true, (LPARAM)&si );


	show_scrollbars();

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
CSize image_bar::get_client_size( )
{
	CSize size;
	CRect rc = NORECT;

	long nitem_count = get_item_count();
	if( nitem_count )
		rc = get_item_rect( nitem_count - 1, true );

	return CSize( rc.right, rc.bottom );	
}

/////////////////////////////////////////////////////////////////////////////
CPoint image_bar::get_scroll_pos()
{
	CPoint pt( 0, 0 );

	SCROLLINFO si = {0};
	si.cbSize = sizeof( SCROLLINFO );
	si.fMask = SIF_POS;
	//get horz scroll
	if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si );
	pt.x = si.nPos;
	//get vert scroll
	if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si );
	pt.y = si.nPos;

	return pt;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::set_scroll_pos( CPoint pt )
{
	SCROLLINFO si = {0};
	si.cbSize = sizeof( SCROLLINFO );
	si.fMask = SIF_POS;
	//set horz scroll
	si.nPos = pt.x;

	SCROLLINFO si_old = {0};
	si_old.cbSize = sizeof( SCROLLINFO );
	si_old.fMask = SIF_POS;
	//get horz scroll
	if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si_old );

	if( si_old.nPos != si.nPos )
		if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_SETSCROLLINFO, true, (LPARAM)&si );

	//set vert scroll
	si.nPos = pt.y;
	if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&si_old );

	if( si_old.nPos != si.nPos )
		if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_SETSCROLLINFO, true, (LPARAM)&si );
		
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::show_scrollbars()
{
	CSize size	= get_client_size( );
	CPoint pt	= get_scroll_pos( );

	CRect rc = NORECT;
	GetClientRect( &rc );
	
	/*
	ShowScrollBar( SB_HORZ, size.cx > rc.Width() || pt.x );
	ShowScrollBar( SB_VERT, size.cy > rc.Height() || pt.y );
	*/

	int _cx = ::GetSystemMetrics( SM_CXVSCROLL );
	int _cy = ::GetSystemMetrics( SM_CYHSCROLL );
	bool bShowH = false;
	bool bShowV = false;
	if(size.cx > rc.Width() || pt.x) bShowH = true;
	if(size.cy > rc.Height() || pt.y) bShowV = true;
	if(bShowH) rc.bottom -= _cy;
	if(bShowV) rc.right -= _cx;
	if(size.cx > rc.Width() || pt.x) bShowH = true;
	if(size.cy > rc.Height() || pt.y) bShowV = true;

	m_phrzScoll->ShowWindow( bShowH ? SW_SHOW : SW_HIDE );
	m_pvrtScoll->ShowWindow( bShowV ? SW_SHOW : SW_HIDE );
}


/////////////////////////////////////////////////////////////////////////////
void image_bar::OnPaint()
{
	CRect rc_update;
	GetUpdateRect( &rc_update );

	CPaintDC	dcPaint(this);
	CDC			dcMemory;

	dcMemory.CreateCompatibleDC( &dcPaint );

	int nImageWidth		= rc_update.Width( )  + 1;
	int nImageHeight	= rc_update.Height( )  + 1;

	if( nImageWidth < 1 || nImageHeight < 1 )
		return;	

	BITMAPINFOHEADER bmih = {0};	

	bmih.biBitCount = 24;	
	bmih.biWidth	= nImageWidth;
	bmih.biHeight	= nImageHeight;
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcPaint, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection );

	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

	dcMemory.SetMapMode( MM_TEXT );	

	dcMemory.SetViewportOrg( -rc_update.left, -rc_update.top );

	{
		//fill backgrund
		CRect rc_fill = rc_update;
		
		dcMemory.FillRect( &rc_fill, &CBrush( GetBackColor() ) );
		//process drawing
		do_draw( &dcMemory, rc_update, &bmih, pdibBits );
	}

	dcMemory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dcPaint, rc_update.left, rc_update.top, nImageWidth, nImageHeight,
		0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );

	dcMemory.SelectObject( poldBmp );

	if( psrcBitmap )
		psrcBitmap->DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::do_draw( CDC* pdc, CRect rc_update, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	if( !pdc || !pbmih || !pdibBits )	return false;

	item_draw_info idi;
	long lidx = 0;
	for( TPOS lpos=m_list_image.head(); lpos; lpos=m_list_image.next(lpos) )
	{
		CRect rc_item = convert_from_client_to_wnd( get_item_rect( lidx, true ) );
		
		CRect rc_tmp;
		if( rc_tmp.IntersectRect( &rc_update, &rc_item ) )
		{
			idi.lidx			= lidx;
			idi.pi				= m_list_image.get( lpos );
			idi.rc_wnd_full		= rc_item;
			idi.rc_wnd_image	= convert_from_client_to_wnd( get_item_rect( lidx, false ) );

			draw_item( &idi, pdc, rc_update, pbmih, pdibBits );
		}
		lidx++;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool image_bar::draw_item(	item_draw_info* pid, CDC* pdc, 
							CRect rc_update, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	if( !pid || !pid->pi || !pdc || !pbmih || !pdibBits )	return false;

	CString str_caption = "";
	// vanek 01.08.2003
	if( (pid->pi->ptr_image != 0) || (pid->pi->sptr_storage != 0) )
		str_caption = _GetObjectName( pid->pi );

	CRect rc_full	= pid->rc_wnd_full;
	CRect rc_image	= pid->rc_wnd_image;
	
	//set dc attrubutes
	CFont* pfont_old		= pdc->SelectObject( &m_font );		
	int nold_mode			= pdc->SetBkMode( TRANSPARENT );
	COLORREF clr_old_text	= pdc->SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );

	if( pid->lidx == m_nactive_item )
	{
		pdc->FillRect( &rc_full, &CBrush( GetHighlightColor() ) );
		//pdc->FillRect( &rc_image, &CBrush( ::GetSysColor(COLOR_BTNFACE) ) );
	}

	//draw border
	CRect rc_border	= rc_full;//rc_image;
	//rc_border.InflateRect( 3, 3, 3, 3 );
	rc_border.DeflateRect( 1, 1, 1, 1 );
	pdc->DrawEdge( &rc_border, EDGE_RAISED, BF_RECT );
	
	// [vanek]: draw icon and text if need - 09.04.2004
	if(m_bDrawText || m_bDrawIcon)
	{
		CRect rc_text	= NORECT;
		CRect rc_ico	= NORECT;
		CRect rc_calc_text = NORECT;    
		
		if( m_bDrawText )
			//calc text rect
			pdc->DrawText( str_caption, &rc_calc_text, DT_CALCRECT );
		

		int nfooter_top = rc_image.bottom + m_nfooter_offset;
		int nfooter_height = max( rc_calc_text.Height(), 16 );

		bool bicon_draw = false;
		if( m_bDrawIcon )
		{   //calc ico rect
			long licon_index = get_icon_index( pid->pi );
			bicon_draw = licon_index >= 0 && licon_index < m_imagelist.GetImageCount();
			if( bicon_draw )
			{
				rc_ico.left		= rc_image.left;
				rc_ico.right	= rc_ico.left + 16;
				rc_ico.top		= nfooter_top + nfooter_height / 2 - 8; //by center
				rc_ico.bottom	= rc_ico.top + 16;

				m_imagelist.Draw( pdc, get_icon_index( pid->pi ), rc_ico.TopLeft(), DI_NORMAL );
			}				
		}

		if( !rc_calc_text.IsRectEmpty() )
		{
			rc_text.left	= (bicon_draw ? rc_ico.right : rc_image.left) + 5;	
			rc_text.right	= rc_image.right - 5;
			rc_text.top		= nfooter_top + nfooter_height / 2 - rc_calc_text.Height() / 2; //by center
			rc_text.bottom	= rc_text.top + rc_calc_text.Height();

			pdc->DrawText( str_caption, &rc_text, (bicon_draw ? DT_LEFT : DT_CENTER)|DT_END_ELLIPSIS );
		}
	}

	// vanek 01.08.2003
	BYTE * pbi = NULL;
	CRect rc_tmp = rc_image;
	if( pid->pi->ptr_image != 0 )
	{
		if( m_ptr_thumb_man )
		{
			IUnknown* punk = 0;
			m_ptr_thumb_man->GetRenderObject( pid->pi->ptr_image, &punk );
			if( punk )
			{
				IRenderObjectPtr ptr_render = punk;
				punk->Release();	punk = 0;
				if( ptr_render )				
				{					
					short nSupported = 0;
					ptr_render->GenerateThumbnail( pid->pi->ptr_image, 24, RF_BACKGROUND, GetBackColor(),
													CSize(rc_tmp.Width(), rc_tmp.Height()), 
													&nSupported, &pbi );
				}
			}
		}
	}
	else if( pid->pi->sptr_storage != 0 )
	{
		long lOldCurrRecord = 0;
		pid->pi->sptr_storage->GetCurrentRecord( &lOldCurrRecord );

		if( S_OK == pid->pi->sptr_storage->SetCurrentRecord( pid->pi->nstorage_record, asAbortObjectCountCalculation) )
		{

			IStorageObjectMisc *pstg_misc = 0;
			pid->pi->sptr_storage->QueryInterface( IID_IStorageObjectMisc, (void**) &pstg_misc );
			if( pstg_misc )
			{
			    pstg_misc->GetRecordDib( &pbi, 0 );
		        pstg_misc->Release();
				pstg_misc = 0;
			}
		}

		pid->pi->sptr_storage->SetCurrentRecord( lOldCurrRecord, 0 );
	}

	if (pbi)
	{
		BITMAPINFOHEADER* pbih = (BITMAPINFOHEADER*)pbi;

		CSize size_out = 
			::ThumbnailGetRatioSize(	CSize( rc_tmp.Width(), rc_tmp.Height() ),
										CSize( pbih->biWidth, pbih->biHeight) );

		/*
		if( pid->lidx == m_nactive_item )
			rc_tmp.OffsetRect( 1, 1 );
			*/

		CRect rc_image_border = NORECT;						
		rc_image_border.left	= rc_tmp.left + (rc_tmp.Width() - size_out.cx) / 2;
		rc_image_border.top		= rc_tmp.top  + (rc_tmp.Height() - size_out.cy) / 2;
		rc_image_border.right	= rc_image_border.left + size_out.cx;
		rc_image_border.bottom	= rc_image_border.top  + size_out.cy;
		rc_image_border.InflateRect( 1, 1, 1, 1 );

		pdc->DrawEdge( &rc_image_border, BDR_SUNKENOUTER, BF_RECT );

		::ThumbnailDraw( pbi, pdc, rc_tmp, pid->pi->ptr_image );
		::ThumbnailFree( pbi );	pbi = 0;
	}


	//restore dc attrubutes
	pdc->SelectObject( pfont_old );	
	pdc->SetBkMode( nold_mode );
	pdc->SetTextColor( clr_old_text );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CPoint	pt_scroll = get_scroll_pos( );	

	CRect	rc_client;
	GetClientRect( &rc_client );

	// Get the minimum and maximum scroll-bar positions.
   INT nminpos = 0, nmaxpos = 0;
   if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETRANGE, (WPARAM)&nminpos, (LPARAM)&nmaxpos);

	bool bprocess = true;
	if( nSBCode == SB_PAGELEFT )
	{
		SCROLLINFO   info = {0};
		info.fMask = SIF_ALL;
		if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		if (pt_scroll.x > nminpos)
			pt_scroll.x = max( nminpos, pt_scroll.x - (int)( info.nPage - rc_client.right % get_item_rect( 0, true ).Width()) );
	}
	else if( nSBCode == SB_PAGERIGHT )
	{
		// Get the page size. 
		SCROLLINFO   info = {0};
		info.fMask = SIF_ALL;
		if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		if (pt_scroll.x < nmaxpos)
			pt_scroll.x = min( nmaxpos, pt_scroll.x + (int)( info.nPage - rc_client.right % get_item_rect( 0, true ).Width() ) );
	}
	else if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION ) 
	{
		int nItemWidth = get_item_rect( 0, true ).Width();
		SCROLLINFO   info = {0};
		info.fMask = SIF_TRACKPOS;
		if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		pt_scroll.x = ( ( info.nTrackPos + nItemWidth / 2 ) / nItemWidth ) * nItemWidth;
	}
	else if( nSBCode == SB_LINELEFT )
	{
		if (pt_scroll.x > nminpos)
			pt_scroll.x -= get_item_rect( 0, true ).Width();
	}
	else if( nSBCode == SB_LINERIGHT )
	{
		if (pt_scroll.x < nmaxpos)
			pt_scroll.x += get_item_rect( 0, true ).Width();
	}
	else
		bprocess = false;

	if( bprocess )
	{	
	/*	CSize	size_client = get_client_size( );

		if( pt_scroll.x < 0 )
			pt_scroll.x = 0;
		if( pt_scroll.x > size_client.cx )
			pt_scroll.x = size_client.cx - 1;*/

		set_scroll_pos( pt_scroll );

		Invalidate( );
	}

	__super::OnHScroll( nSBCode, nPos, pScrollBar );
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CPoint	pt_scroll = get_scroll_pos( );	

	CRect	rc_client;
	GetClientRect( &rc_client );

	int nminpos = 0, nmaxpos = 0;
	if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETRANGE, (WPARAM)&nminpos, (LPARAM)&nmaxpos);
	
	bool bprocess = true;
	if( nSBCode == SB_PAGEUP )
	{
		SCROLLINFO   info = {0};
		info.fMask = SIF_ALL;
		if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		if (pt_scroll.y > nminpos)
			pt_scroll.y = max( nminpos, pt_scroll.y - rc_client.Height() );
	}
	else if( nSBCode == SB_PAGEDOWN )
	{
		// Get the page size. 
		SCROLLINFO   info = {0};
		info.fMask = SIF_ALL;
		if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		if (pt_scroll.y < nmaxpos)
			pt_scroll.y = min( nmaxpos, pt_scroll.y + rc_client.Height() );
	}
	else if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION ) 
	{
		SCROLLINFO   info = {0};
		info.fMask = SIF_TRACKPOS;
		if(m_pvrtScoll) m_pvrtScoll->SendMessage( SBM_GETSCROLLINFO, true, (LPARAM)&info );
		pt_scroll.y = info.nTrackPos;
	}
	else if( nSBCode == SB_LINEUP )
	{
		if (pt_scroll.y > nminpos)
			pt_scroll.y --;
	}
	else if( nSBCode == SB_LINEDOWN )
	{
		if (pt_scroll.y < nmaxpos)
			pt_scroll.y ++;
	}
	else
		bprocess = false;

	if( bprocess )
	{	
		set_scroll_pos( pt_scroll );
    	Invalidate( );
	}

	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
BOOL image_bar::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	set_client_size();
	show_scrollbars();

	CSize size	= get_client_size( );
	CPoint pt	= get_scroll_pos( );

 	CRect rc = NORECT;
	GetClientRect( &rc );

	int _cx = ::GetSystemMetrics( SM_CXVSCROLL );
	int _cy = ::GetSystemMetrics( SM_CYHSCROLL );

	bool bShowH = false;
	bool bShowV = false;
	if(size.cx > rc.Width() || pt.x) bShowH = true;
	if(size.cy > rc.Height() || pt.y) bShowV = true;
	if(bShowH) rc.bottom -= _cy;
	if(bShowV) rc.right -= _cx;
	if(size.cx > rc.Width() || pt.x) bShowH = true;
	if(size.cy > rc.Height() || pt.y) bShowV = true;

	RECT rcHorz = { 0, cy - _cy, cx, cy };
	RECT rcVert = { cx - _cx, 0, cx,  cy - (bShowH?_cx:0) };

	m_phrzScoll->MoveWindow( &rcHorz );
	m_pvrtScoll->MoveWindow( &rcVert );

	Invalidate(); // vanek
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnDestroy()
{
	m_ptr_thumb_man = 0;

	m_imagelist.DeleteImageList();

	delete m_phrzScoll;
	delete m_pvrtScoll;

	__super::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnNcDestroy()
{
	__super::OnNcDestroy();	
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nactive_item = get_item_from_point( convert_from_wnd_to_client( point ) );	
	if( nactive_item != m_nactive_item && nactive_item >= 0 && nactive_item < get_item_count() )
	{
		CRect rc_old = get_item_rect( m_nactive_item, true );
		CRect rc_new = get_item_rect( nactive_item, true );

		m_nactive_item = nactive_item;

		InvalidateRect( &convert_from_client_to_wnd( rc_old ) );
		InvalidateRect( &convert_from_client_to_wnd( rc_new ) );

		image_info* pinfo = get_image_info_by_idx( m_nactive_item );
		// vanek 01.08.2003
		if( pinfo && (pinfo->ptr_image != 0 || pinfo->sptr_storage != 0) )
		{
			// vanek 01.08.2003
			if( m_bchange_context && (pinfo->ptr_image != 0) )
			{
				IDataContext3Ptr ptr_dc = m_ptr_active_view;
				if( ptr_dc )
				{
					m_block_sync = true;
					ptr_dc->SetActiveObject( _bstr_t(szTypeImage), pinfo->ptr_image, aofActivateDepended );
					::AjustViewForObject( ptr_dc, pinfo->ptr_image );
					m_block_sync = false;
				}
			}

			//fire script notify
			IDispatchPtr	ptr_disp = (pinfo->ptr_image != 0) ? pinfo->ptr_image : pinfo->sptr_storage;
			IScriptSitePtr	ptr_ss = ::GetAppUnknown();
			if( ptr_disp != 0 && ptr_ss != 0 )
			{
				_bstr_t bstr_func = IMAGEBAR_NAME;
				bstr_func += "_";
				bstr_func += "OnActivateObject";

				//_variant_t arg = (IDispatch*)ptr_disp;
				// vanek 01.08.2003
				//_variant_t arg = _GetObjectName( pinfo );
				_variant_t arg = nactive_item;	// object index
				ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );
			}
		}		

		// Проскроллить окно, если кликнули частично видимый объект
		{
			CRect rc_item = get_item_rect( m_nactive_item, true );
			rc_item = convert_from_client_to_wnd(rc_item);
			CRect	rc_client;
			CPoint	pt_scroll = get_scroll_pos( );
			GetClientRect( &rc_client );
			// Get the minimum and maximum scroll-bar positions.
			int nminpos = 0, nmaxpos = 0;
			if(m_phrzScoll) m_phrzScoll->SendMessage( SBM_GETRANGE, (WPARAM)&nminpos, (LPARAM)&nmaxpos);
			if(rc_item.left<rc_client.left)
			{
				if (pt_scroll.x > nminpos)
					pt_scroll.x -= get_item_rect( 0, true ).Width();
				set_scroll_pos( pt_scroll );
				Invalidate();
			}
			else if(rc_item.right>rc_client.right)
			{
				if (pt_scroll.x < nmaxpos)
					pt_scroll.x += get_item_rect( 0, true ).Width();
				set_scroll_pos( pt_scroll );
				Invalidate();
			}
		}
	}
	
	__super::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT image_bar::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	::HelpDisplayTopic( IMAGEBAR_NAME, 0, IMAGEBAR_NAME, "$GLOBAL_main" );
	return (DWORD)-2;
}

//Dispatch
/////////////////////////////////////////////////////////////////////////////
void image_bar::Refresh()
{
	//[vanek] - 09.04.2004
	_ReadParams( );

	sync_info( );
	sync_activity( );
}

/////////////////////////////////////////////////////////////////////////////
BOOL image_bar::GetChangeContext()
{
	return ( m_bchange_context == true );
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::SetChangeContext(BOOL bNewValue)
{
	m_bchange_context = ( bNewValue == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BSTR image_bar::GetNameFilter()
{
	return m_str_name_filter.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::SetNameFilter( LPCTSTR strNewValue )
{
	m_str_name_filter = strNewValue;
	sync_info( );
	sync_activity( );

	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
BSTR image_bar::GetObjectInfoByIndex( long lIdx, VARIANT FAR* varStorageName, VARIANT FAR* varRecord )
{
	// vanek  07.08.2003
	CString sImageName("");

	::VariantClear( varStorageName );
	::VariantClear( varRecord );
	*varRecord = _variant_t( (long) -1 );

	if( ( lIdx < 0 ) || ( lIdx >= m_list_image.count() ) )
		return sImageName.AllocSysString( );

	long lListIdx = 0; TPOS lPos = 0;
	for( lPos = m_list_image.head(); lPos; lPos = m_list_image.next( lPos )  )
	{
		if( lListIdx == lIdx )
			break;
		else
		   lListIdx ++;
	}

	if( !lPos )
		return sImageName.AllocSysString( );

	image_info *pi = 0;
	pi = m_list_image.get( lPos );
	if( !pi )
		return sImageName.AllocSysString( );

	if( pi->sptr_storage != 0 )
	{
		BSTR bstrStorageName = 0;
		pi->sptr_storage->get_Name( &bstrStorageName );
		if( bstrStorageName )
		{
			VariantCopy( varStorageName, &_variant_t( bstrStorageName ) );
			::SysFreeString( bstrStorageName );
			bstrStorageName = 0;
		}
		*varRecord = _variant_t( (long)( pi->nstorage_record ) );
	}
		
	sImageName = _GetObjectName( pi );
	return sImageName.AllocSysString( );
}

/////////////////////////////////////////////////////////////////////////////
// vanek  07.08.2003
long image_bar::GetActiveItem()
{
	return m_nactive_item;
}

/////////////////////////////////////////////////////////////////////////////
// vanek  07.08.2003
void image_bar::SetActiveItem(long lIdx)
{
	if( ( lIdx >= 0 ) || ( lIdx < m_list_image.count() ) )
		m_nactive_item = lIdx;
}

/////////////////////////////////////////////////////////////////////////////
// vanek  07.08.2003
void image_bar::Invalidate()
{
	__super::Invalidate( );
}  

/////////////////////////////////////////////////////////////////////////////
// vanek  20.08.2003
long image_bar::GetItemCount()
{				
	return get_item_count( );
}

// vanek 15.10.2003
/////////////////////////////////////////////////////////////////////////////
BOOL image_bar::GetLockSync()
{
	return m_block_sync;
}

/////////////////////////////////////////////////////////////////////////////
void image_bar::SetLockSync(BOOL bNewValue)
{
	m_block_sync = bNewValue == TRUE;
}


/////////////////////////////////////////////////////////////////////////////
void image_bar::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint pt_client = point;	
	ScreenToClient( &pt_client );

	OnLButtonDown( 0, pt_client );

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )			return;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == 0 )	return;
	
	CMenuRegistrator rcm;	
	_bstr_t	bstrMenuName = rcm.GetMenu( szImageBarNameMenu, 0 );
	ptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );	

}

void image_bar::_ReadParams()
{
	m_bUseSysColors = ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_SYSCOLORS, 0);
	m_clBackColor	= ::GetValueColor( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_BACKCOLOR, ::GetSysColor(COLOR_BTNFACE) );
	m_clHighlightColor	= ::GetValueColor( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_HIGHLIGHTCOLOR, ::GetSysColor(COLOR_APPWORKSPACE) );
	m_bDrawText		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_DRAWTEXT, 1 ) != 0;
	m_bDrawIcon		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_DRAWICON, 1 ) != 0;

	//get item width, height
	m_nitem_width		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_WIDTH, 50 );
	m_nitem_height		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_HEIGHT, 50 );

	// [vanek]: get propperty's name of icon's icon
	m_str_icon_prop_name = ::GetValueString( ::GetAppUnknown(), IMAGEBAR_SECTION, IMAGEBAR_ICONPROPNAME, "ImageState" );

	m_rect_offset = (m_bDrawText || m_bDrawIcon) ? m_rect_offset_with_text : m_rect_offset_without_text;

	CRect rc(10,10,10,10);
	rc.left		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "FieldLeft", 10 );
	rc.right		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "FieldRight", 10 );
	rc.top		= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "FieldTop", 10 );
	rc.bottom	= ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "FieldBottom", 10 );

	m_nfooter_offset = ::GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "FooterOffset", 5 ); // получим отступ текста от картинки
	rc.bottom += m_nfooter_offset-5; // если отступ больше 5 - увеличить и нижнее поле, дабы текст влез

	// от отступа по умолчанию в 10 пикселей перейдем к заданному
	m_rect_offset.left += rc.left - 10;
	m_rect_offset.right += rc.right - 10;
	m_rect_offset.top += rc.top - 10;
	m_rect_offset.bottom += rc.bottom - 10;

	m_ptBarOffset.x = GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "BarOffsetX", 0 );
	m_ptBarOffset.y = GetValueInt( ::GetAppUnknown(), IMAGEBAR_SECTION, "BarOffsetY", 0 );
}

LRESULT image_bar::OnNcHitTest(CPoint point) 
{
	CPoint pt1(point);
	ScreenToClient(&pt1);
	CRect rcClient;
	GetClientRect(&rcClient);
	if(rcClient.PtInRect(pt1))
	{
		int n = get_item_from_point(convert_from_wnd_to_client(pt1));
		if(n<0 || n>=get_item_count() )
		{
			return HTTRANSPARENT;
		}
	}
	return __super::OnNcHitTest(point);
}

long	image_bar::get_icon_index( image_info* pii )
{
	if( !pii )
		return -1;

	// [vanek] SBT:929 - 12.04.2004
	try
	{
		if( pii->ptr_image != 0 )
		{
			INamedPropBagPtr sptr_prop_bag = pii->ptr_image;
			_variant_t var;
			if( sptr_prop_bag != 0 )
				sptr_prop_bag->GetProperty( _bstr_t( m_str_icon_prop_name ),&var );
			return var.vt == VT_EMPTY ? -1 : (long)(var);
		}
		else if( pii->sptr_storage !=0 )
			return	pii->m_licon_index;
	}
	catch( ... )
	{
	}
	
	return -1;
}
