#include "stdafx.h"
#include "resource.h"
#include "filter_objectlist.h"
#include "misc_utils.h"
#include "clonedata.h"


/////////////////////////////////////////////////////////////////////////////
CVirtDoc::CVirtDoc()
{
	EnableAggregation();

	m_punk_data		= 0;	
	m_punk_context	= 0;
}

/////////////////////////////////////////////////////////////////////////////
CVirtDoc::~CVirtDoc()
{
	deinit();
}

/////////////////////////////////////////////////////////////////////////////
bool CVirtDoc::init()
{
	deinit();	

	CLSID	clsid_data = {0}, clsid_context = {0};
	_bstr_t	bstr_data( szNamedDataProgID ), bstr_context( szContextProgID );

	::CLSIDFromProgID( bstr_data, &clsid_data );
	::CLSIDFromProgID( bstr_context, &clsid_context );

	::CoCreateInstance( clsid_data, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_punk_data );
	::CoCreateInstance( clsid_context, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_punk_context );

	IDataContextPtr	ptr( GetControllingUnknown() );
	if( ptr == 0 )	return false;
	ptr->AttachData( GetControllingUnknown() );    

	return m_punk_data && m_punk_context;
}

/////////////////////////////////////////////////////////////////////////////
void CVirtDoc::deinit()
{
	{
		IDataContextPtr	ptr( GetControllingUnknown() );
		if( ptr )
			ptr->AttachData( 0 );    
	}

	if( m_punk_data )
		m_punk_data->Release();	m_punk_data = 0;

	if( m_punk_context )
		m_punk_context->Release();	m_punk_context = 0;
}


BEGIN_INTERFACE_MAP(CVirtDoc, CCmdTarget)
	INTERFACE_PART(CVirtDoc, IID_INumeredObject, Num)
	INTERFACE_AGGREGATE(CVirtDoc, m_punk_data)
	INTERFACE_AGGREGATE(CVirtDoc, m_punk_context)
END_INTERFACE_MAP()



IMPLEMENT_DYNCREATE(CFilterObjectList, CCmdTargetEx);

// {6DA73482-7A64-41cb-B2AF-AEFC2B8EECAA}
GUARD_IMPLEMENT_OLECREATE(CFilterObjectList, "Objects.FilterObjectList", 
0x6da73482, 0x7a64, 0x41cb, 0xb2, 0xaf, 0xae, 0xfc, 0x2b, 0x8e, 0xec, 0xaa);

// {2DBE964C-41E6-47b7-B7EF-F897BA2265DD}
static const GUID guidFilterObjectList = 
{ 0x2dbe964c, 0x41e6, 0x47b7, { 0xb7, 0xef, 0xf8, 0x97, 0xba, 0x22, 0x65, 0xdd } };

ACTION_INFO_FULL(CFilterObjectList, IDS_ACTION_FILTER_OBJECTLIST, 0, 0, guidFilterObjectList);
ACTION_TARGET(CFilterObjectList, szTargetAnydoc);

ACTION_ARG_LIST(CFilterObjectList)
	ARG_STRING( _T("Action"), "" )
	ARG_STRING( _T("Params"), "" )
	ARG_INT( _T("LeaveVirtual"), 0 )
	ARG_INT( _T("ActiveObjectOnly"), 0 )
	ARG_OBJECT( _T("ObjectList") )
	RES_OBJECT( _T("ObjectListNew") )
END_ACTION_ARG_LIST();


/////////////////////////////////////////////////////////////////////////////
CFilterObjectList::CFilterObjectList()
{
	m_pvirt_doc = 0;
}

/////////////////////////////////////////////////////////////////////////////
CFilterObjectList::~CFilterObjectList()
{
	if( m_pvirt_doc )
	{
		IUnknownPtr ptr = m_pvirt_doc->GetControllingUnknown();
		m_pvirt_doc->deinit();
		ptr->Release();	m_pvirt_doc = 0;
	}
}

#include "\vt5\AWIN\profiler.h"

/////////////////////////////////////////////////////////////////////////////
bool CFilterObjectList::InvokeFilter()
{
	IUnknownPtr ptr_src_ol = GetDataArgument();
	IUnknownPtr	ptr_target_ol = GetDataResult();

	TIME_TEST( "CFilterObjectList::InvokeFilter" );

	//Empty Object List
	{
		INamedDataObject2Ptr ptr_ndo( ptr_target_ol );
		if( ptr_ndo )
		{
			long lpos = 0;
			ptr_ndo->GetFirstChildPosition( &lpos );
			while( lpos )
			{
				IUnknown* punk = 0;
				ptr_ndo->GetNextChild( &lpos, &punk );

				INamedDataObject2Ptr ptr_ndo_child( punk );
				punk->Release();	punk = 0;

				if( ptr_ndo_child )
					ptr_ndo_child->SetParent( 0, 0 );
			}
		}
	}


	if( ptr_src_ol == 0 || ptr_target_ol == 0 )	return false;

	if( !create_compatible_ol( ptr_src_ol, ptr_target_ol ) )	return false;

	m_pvirt_doc = new CVirtDoc;
	m_pvirt_doc->init();

	//process ol
	{
		//create 2 images, from ol & empty
		IUnknownPtr ptr_image_src = extract_image_from_list( ptr_src_ol );
		IUnknownPtr ptr_image_target( ::CreateTypedObject( szTypeImage ), false );

		if( ptr_image_src == 0 || ptr_image_target == 0 )	return false;

		::SetValue( m_pvirt_doc->GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptr_image_src ) );
		::SetValue( m_pvirt_doc->GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptr_image_target ) );

		if( ptr_image_src == 0 || ptr_image_target == 0 )
			return false;

		//filter images
		if( !filter_images( ptr_image_src, ptr_image_target ) )
			return false;

		//put image to list
		if( !place_image_to_list( ptr_image_target, ptr_target_ol ) )
			return false;

	}

	StartNotification( 1 );
	FinishNotification();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CFilterObjectList::extract_image_from_list( IUnknown* punk_ol )
{
	TIME_TEST( "CFilterObjectList::extract_image_from_list" );
	//create image
	IUnknown* punk_image = ::CreateTypedObject( szTypeImage );
	if( punk_image == 0 )	return 0;

	IImage3Ptr ptr_image = punk_image;
	punk_image->Release();	punk_image = 0;

	INamedDataObject2Ptr ptr_list = punk_ol;
	if( ptr_list == 0 )		return 0;


	long lpos_active = 0;
	if( 1L == GetArgumentInt("ActiveObjectOnly" ) )
	{
		ptr_list->GetActiveChild( &lpos_active );
		if( !lpos_active )
			return false;
	}

	

	//determine size && CC
	int cx = 0;
	int cy = 0;	
	int npane_count = 0;
	_bstr_t bstr_cc;

	long lpos = 0;
	ptr_list->GetFirstChildPosition( &lpos );
	while( lpos )
	{		
		IUnknown* punk_child = 0;
		ptr_list->GetNextChild( &lpos, &punk_child );
		
		if( !punk_child )			continue;

		IMeasureObjectPtr ptr_meas_obj = punk_child;
		punk_child->Release(); punk_child = 0;

		if( ptr_meas_obj == 0 )		continue;
	
		IUnknown *punk_obj_image = 0;
		ptr_meas_obj->GetImage( &punk_obj_image );

		IImage3Ptr ptr_child_image = punk_obj_image;
		punk_obj_image->Release(); punk_obj_image = 0;

		if( ptr_child_image == 0 )	continue;

		//color converter
		if( !bstr_cc.length() )
		{
			IUnknownPtr ptr;
			ptr_child_image->GetColorConvertor( &ptr );
			if( ptr )
			{
				IColorConvertor2Ptr ptr_cc = ptr;
				if( ptr_cc )
				{
					BSTR bstr = 0;
					ptr_cc->GetCnvName( &bstr );
					bstr_cc = bstr;
					::SysFreeString( bstr );	bstr = 0;
				}
			}
			npane_count = ::GetImagePaneCount( ptr_child_image );
		}

		POINT pt = {0};
		int nx = 0, ny = 0;

		ptr_child_image->GetOffset( &pt );
		ptr_child_image->GetSize( &nx, &ny  );

		if( pt.x + nx > cx )
			cx = pt.x + nx;

		if( pt.y + ny > cy )
			cy = pt.y + ny;
	}

	if( !cx || !cy || !bstr_cc.length() || !npane_count )
		return false;

	ptr_image->CreateImage( cx, cy, bstr_cc, npane_count );
	ptr_image->InitRowMasks();

	//set row mask to 0
	for( int y=0;y<cy;y++ )
	{		
		BYTE* pmask = 0;
		ptr_image->GetRowMask( y, &pmask );
		::ZeroMemory( pmask, cx );
	}


	//copy objectlist images
	lpos = 0;
	ptr_list->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		long lpos_save = lpos;

		IUnknownPtr ptr_child = 0;
		ptr_list->GetNextChild( &lpos, &ptr_child );		
		if( ptr_child == 0 )			continue;

		IMeasureObjectPtr ptr_meas_obj = ptr_child;
		if( ptr_meas_obj == 0 )		continue;

		if( lpos_active && lpos_save != lpos_active )	continue;
	
		IUnknownPtr ptr_obj_image = 0;
		ptr_meas_obj->GetImage( &ptr_obj_image );
		IImage3Ptr ptr_child_image = ptr_obj_image;
		if( ptr_child_image == 0 )	continue;

		CRect rc_src;
		CPoint pt_target;

		POINT pt = {0};
		int nx = 0, ny = 0;

		ptr_child_image->GetOffset( &pt );
		ptr_child_image->GetSize( &nx, &ny  );

		rc_src = CRect( 0, 0, nx-1, ny-1 );
		pt_target = pt;

		_copy_image( ptr_child_image, ptr_image, &rc_src, &pt_target );

		//set row mask to FF where image exist
		for( int y=0;y<ny;y++ )
		{	
			//src mask
			BYTE* psrc_mask = 0;
			ptr_child_image->GetRowMask( y, &psrc_mask );
			
			//BIG image mask
			BYTE* ptarget_mask = 0;
			ptr_image->GetRowMask( y + pt.y, &ptarget_mask );

			for( int x=0;x<nx;x++ )
				if(psrc_mask[x]>=0x80)
					ptarget_mask[x + pt.x] = psrc_mask[x];
		}
	}

	return ptr_image;	
}


/////////////////////////////////////////////////////////////////////////////
bool CFilterObjectList::filter_images( IUnknown* punk_src, IUnknown* punk_target )
{
	TIME_TEST( "CFilterObjectList::filter_images" );

	if( !punk_src || !punk_target )	return false;

	CString str_action = GetArgumentString( "Action" );
	CString str_params = GetArgumentString( "Params" );

	//create action
	IActionPtr ptr_action = get_action( str_action );
	if( ptr_action == 0 )
	{
		CString str_format;
		str_format.Format( IDS_NO_ACTION, str_action );
		AfxMessageBox( str_format, MB_ICONERROR );
		return false;
	}

	IFilterActionPtr ptr_filter( ptr_action );
	if( ptr_filter == 0 )		return false;

	IUnknownPtr ptr_active_doc;
	{
		IApplicationPtr ptr_app = ::GetAppUnknown();
		ptr_app->GetActiveDocument( &ptr_active_doc );
	}

	ptr_action->AttachTarget( m_pvirt_doc->GetControllingUnknown()/*, ptr_active_doc*/ );	
	ptr_filter->LockDataChanging( true );	
	ptr_filter->InitArguments();

	//init filter argument
	long larg_pos = 0;
	ptr_filter->GetFirstArgumentPosition( &larg_pos );
	while( larg_pos )
	{
		BSTR		bstrArgType = 0;
		BSTR		bstrArgName = 0;
		BOOL		bOut = 0;
		BOOL		bRemove = false;
		ptr_filter->GetArgumentInfo( larg_pos, &bstrArgType, &bstrArgName, &bOut );

		CString str_arg_type = bstrArgType;
		CString str_arg_name = bstrArgName;
		
		::SysFreeString( bstrArgType );	bstrArgType = 0;
		::SysFreeString( bstrArgName );	bstrArgName = 0;

		if( bOut )
			ptr_filter->SetArgument( larg_pos, punk_target, false );
		else
			ptr_filter->SetArgument( larg_pos, punk_src, false );

		ptr_filter->GetNextArgument( 0, &larg_pos );
	}

	//init other argument
	IUnknownPtr ptr_ai;
	ptr_action->GetActionInfo( &ptr_ai );
	IActionInfoPtr ptr_ainfo = ptr_ai;
	if( ptr_ainfo == 0 )	return false;

	char sz_params[256];
	if( str_params.GetLength() >= sizeof(sz_params) )
		return false;

	strcpy( sz_params, (const char*)str_params );
	char* psz_token = strtok( sz_params, "," );

	int narg_count = 0;
	ptr_ainfo->GetArgsCount( &narg_count );
	for( int i=0;i<narg_count;i++ )
	{
		BSTR bstr_name = 0, bstr_kind = 0, bstr_def = 0;
		ptr_ainfo->GetArgInfo( i, &bstr_name, &bstr_kind, &bstr_def );
		bstr_t _bstr_name = "", _bstr_kind = "", _bstr_def = "";
		if( bstr_name )
		{
			_bstr_name = bstr_name;
			::SysFreeString( bstr_name );	bstr_name = 0;
		}
		if( bstr_kind )
		{
			_bstr_kind = bstr_kind;
			::SysFreeString( bstr_kind );	bstr_kind = 0;
		}
		if( bstr_def )
		{
			_bstr_def = bstr_def;
			::SysFreeString( bstr_def );	bstr_def = 0;
		}

		if( !( !strcmp( (char*)_bstr_kind, szArgumentTypeInt ) || 
			!strcmp( (char*)_bstr_kind, szArgumentTypeDouble ) || 
			!strcmp( (char*)_bstr_kind, szArgumentTypeString ) ) )
		{
			continue;
		}

		_variant_t var;
		CString str_value = "";
		if( psz_token )			 
		{
			str_value = psz_token;
			psz_token = strtok( 0, "," );
		}
		else if( _bstr_def.length() )
			str_value = (char*)_bstr_def;

		var = str_value;
		try
		{
			if( !strcmp( (char*)_bstr_kind, szArgumentTypeInt ) )
				var.ChangeType( VT_I4 );
			else if( !strcmp( (char*)_bstr_kind, szArgumentTypeDouble ) )
				var.ChangeType( VT_R8 );			
		}
		catch(...){}

		ptr_action->SetArgument( _bstr_name, &var );		
	}
	
	ILongOperationPtr ptr_lo( ptr_action );
	if( ptr_lo )
	{							   
		HWND hwnd = (HWND)INVALID_HANDLE_VALUE ;
		ptr_lo->AttachNotifyWindow( hwnd );
	}

	return ( S_OK == ptr_action->Invoke() );
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterObjectList::place_image_to_list( IUnknown* punk_image, IUnknown* punk_list )
{
	TIME_TEST( "CFilterObjectList::place_image_to_list" );

	IImage3Ptr ptr_image = punk_image;	
	INamedDataObject2Ptr ptr_list = punk_list;
	if( ptr_image == 0 || ptr_list == 0 )		return false;

	//copy objectlist images
	long lpos = 0;
	ptr_list->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknownPtr ptr_child = 0;
		ptr_list->GetNextChild( &lpos, &ptr_child );		
		if( ptr_child == 0 )			continue;

		IMeasureObjectPtr ptr_meas_obj = ptr_child;
		if( ptr_meas_obj == 0 )		continue;
	
		IUnknownPtr ptr_obj_image = 0;
		ptr_meas_obj->GetImage( &ptr_obj_image );
		IImage3Ptr ptr_child_image = ptr_obj_image;
		if( ptr_child_image == 0 )	continue;

		CRect rc_src;
		CPoint pt_target;

		POINT pt = {0};
		int nx = 0, ny = 0;

		ptr_child_image->GetOffset( &pt );
		ptr_child_image->GetSize( &nx, &ny  );

		rc_src = CRect( pt.x, pt.y, pt.x + nx-1, pt.y + ny-1 );
		pt_target = CPoint( 0, 0 );

		_copy_image( ptr_image, ptr_child_image, &rc_src, &pt_target );
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterObjectList::_copy_image( IImage3* pimg_src, IImage3* pimg_target, CRect* prc_src, CPoint* ppt_target )
{
	if( !pimg_src || !pimg_target || !prc_src || !ppt_target )
		return false;
	
	int cx = prc_src->Width() + 1, cy = prc_src->Height() + 1;

	int npanes = min( ::GetImagePaneCount( pimg_src ), ::GetImagePaneCount( pimg_target ) );

	for( int j = 0; j < cy; j++ )
	{
		for (int k = 0; k < npanes; k++)
		{
			color* psrc_color = 0;
			color* pdest_color = 0;

			int ysrc	= j + prc_src->top;
			int ytarget	= j + ppt_target->y;

			pimg_src->GetRow( ysrc, k, &psrc_color );
			pimg_target->GetRow( ytarget, k, &pdest_color );

			BYTE* psrc_mask = 0;
			pimg_src->GetRowMask( ysrc, &psrc_mask );

			if( !psrc_color || !pdest_color || !psrc_mask )
				continue;
					 
			for( int i = 0; i < cx; i++ )
			{
				int xsrc	= i + prc_src->left;
				int xtarget	= i + ppt_target->x;

				if( psrc_mask[xsrc]>=128 )
					pdest_color[xtarget] = psrc_color[xsrc];
			}
		}
	}
	return true;

}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CFilterObjectList::get_action( const char* psz_action )
{
	if( !psz_action )		return 0;
	//get action manager
	IApplicationPtr	ptr_app( GetAppUnknown() );
	IUnknown* punk_am = 0;
	ptr_app->GetActionManager( &punk_am );
	if( punk_am == 0 )		return 0;
	IActionManagerPtr ptr_am( punk_am );
	punk_am->Release();	punk_am = 0;
	if( ptr_am == 0 )		return 0;

    //get action info
	IUnknown* punk_info = 0;
	ptr_am->GetActionInfo( _bstr_t(psz_action), &punk_info );
	if( punk_info == 0 )	return 0;
	IActionInfoPtr	ptr_info( punk_info );
	punk_info->Release();	punk_info = 0;
	if( ptr_info == 0 )		return 0;

	//construct action
	IUnknownPtr ptr_action;
	ptr_info->ConstructAction( &ptr_action );
	return ptr_action;
}
	

/////////////////////////////////////////////////////////////////////////////
bool CFilterObjectList::create_compatible_ol( IUnknown* punk_src, IUnknown* punk_target )
{
	TIME_TEST( "CFilterObjectList::create_compatible_ol" );
	if( !punk_src || !punk_target )	return 0;

	int nleave_virtual = GetArgumentInt( "LeaveVirtual" );

	ICompatibleObjectPtr ptr_co( punk_target );
	if( ptr_co == 0 )	return 0;

	{
	TIME_TEST( "   ptr_co->CreateCompatibleObject" );
	ptr_co->CreateCompatibleObject( punk_src, 0, 0 );
	}

	long l1 = GetTickCount();
	long l2 = GetTickCount();

	//create new images
	INamedDataObject2Ptr ptr_list( ptr_co );
	if( ptr_list )
	{
		long lpos = 0;
		ptr_list->GetFirstChildPosition( &lpos );
		while( lpos )
		{
			IUnknown* punk_child = 0;
			ptr_list->GetNextChild( &lpos, &punk_child );
			
			if( !punk_child )			continue;

			IMeasureObjectPtr ptr_meas_obj = punk_child;
			punk_child->Release(); punk_child = 0;

			if( ptr_meas_obj == 0 )		continue;
		
			IUnknownPtr ptr_image = 0;
			ptr_meas_obj->GetImage( &ptr_image );
			if( ptr_image == 0 )		continue;

			IImage3Ptr ptr_src_image = ptr_image;
			if( ptr_src_image == 0 )	continue;

			IUnknownPtr ptr_new_image( ::CreateTypedObject( szTypeImage ), false );
			if( ptr_new_image == 0 )	continue;
			
			ImageCloneData icd = {0};
			icd.dwCopyFlags = ICD_COPY_MASK | ICD_COPY_CONTOURS | ICD_COPY_IMGDATA;

			ICompatibleObjectPtr ptr_comp_image = ptr_new_image;
			if( ptr_comp_image == 0 )	continue;

			ptr_comp_image->CreateCompatibleObject( ptr_image, &icd, sizeof(icd) );
			ptr_meas_obj->SetImage( ptr_comp_image );
			
			
			//Unvirtual image if need
			if( nleave_virtual == 1 )
				continue;

			DWORD dwFlags;
			ptr_src_image->GetImageFlags(&dwFlags);
			bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным

			if(!bIsVirtual) continue; // не с виртуальным возиться не надо

			INamedDataObject2Ptr ptr_ndo( ptr_comp_image );
			if( ptr_ndo )
			{
				IUnknownPtr ptr_parent;
				ptr_ndo->GetParent( &ptr_parent );
				if( ptr_parent )
					ptr_ndo->SetParent( 0, sdfCopyParentData );
			}
		}
	}

	return ptr_co;
}


