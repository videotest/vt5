// data_context_ctrl.cpp : implementation file
//

#include "stdafx.h"
#include "DataUI.h"
#include "data_context_ctrl.h"

#include "dataui_consts.h"
#include "scriptnotifyint.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// data_context_ctrl

IMPLEMENT_DYNCREATE(data_context_ctrl, CCmdTarget)

data_context_ctrl::data_context_ctrl()
{
	m_sName = m_sUserName = DATA_CONTEXT_CTRL_NAME;

	EnableAutomation();
	EnableAggregation();	
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	Register( ::GetAppUnknown() );	
	
	AfxOleLockApp();

	m_mEnable = true;
}

data_context_ctrl::~data_context_ctrl()
{
	UnRegister( ::GetAppUnknown() );

	ASSERT( m_ptr_active_doc == 0 );
	ASSERT( m_ptr_active_view == 0 );

	AfxOleUnlockApp();
}


void data_context_ctrl::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(data_context_ctrl, CCmdTarget)
	//{{AFX_MSG_MAP(data_context_ctrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(data_context_ctrl, CCmdTarget)
	//{{AFX_DISPATCH_MAP(data_context_ctrl)
	DISP_FUNCTION(data_context_ctrl, "GetItemCount", GetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(data_context_ctrl, "GetItemString", GetItemString, VT_BSTR, VTS_I4)
	DISP_FUNCTION(data_context_ctrl, "GetItemData", GetItemData, VT_I4, VTS_I4)
	DISP_FUNCTION(data_context_ctrl, "DeleteItem", DeleteItem, VT_BOOL, VTS_I4)
	DISP_FUNCTION(data_context_ctrl, "AddItem", AddItem, VT_I4, VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION(data_context_ctrl, "GetCurSel", GetCurSel, VT_I4, VTS_NONE)
	DISP_FUNCTION(data_context_ctrl, "SetCurSel", SetCurSel, VT_BOOL, VTS_I4)
	DISP_FUNCTION(data_context_ctrl, "ResetContent", ResetContent, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(data_context_ctrl, "SetEnable", SetEnable, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Idata_context_ctrl to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {0A93A964-CCFE-4E2D-8E6A-45FDB93E18A6}
static const IID IID_Idata_context_ctrl =
{ 0xa93a964, 0xccfe, 0x4e2d, { 0x8e, 0x6a, 0x45, 0xfd, 0xb9, 0x3e, 0x18, 0xa6 } };

BEGIN_INTERFACE_MAP(data_context_ctrl, CCmdTarget)
	INTERFACE_PART(data_context_ctrl, IID_Idata_context_ctrl, Dispatch)
	INTERFACE_PART(data_context_ctrl, IID_IEventListener, EvList)	
	INTERFACE_PART(data_context_ctrl, IID_INamedObject2, Name)
	INTERFACE_PART(data_context_ctrl, IID_IDataContextCtrl, DataContextCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(data_context_ctrl, DataContextCtrl);

// {D50FBA6E-4709-4F1C-866E-0597E6D7621A}
GUARD_IMPLEMENT_OLECREATE(data_context_ctrl, DATA_CONTEXT_CTRL_PROGID, 
0xd50fba6e, 0x4709, 0x4f1c, 0x86, 0x6e, 0x5, 0x97, 0xe6, 0xd7, 0x62, 0x1a)

/////////////////////////////////////////////////////////////////////////////
void data_context_ctrl::register_doc( IUnknown* punk_doc )
{
	unregister_doc( );

	m_ptr_active_doc = punk_doc;
	
	if( m_ptr_active_doc )
		Register( m_ptr_active_doc );
}

/////////////////////////////////////////////////////////////////////////////
void data_context_ctrl::unregister_doc( )
{
	if( m_ptr_active_doc )
		UnRegister( m_ptr_active_doc );
	
	m_ptr_active_doc = 0;
}

/////////////////////////////////////////////////////////////////////////////
void data_context_ctrl::register_view( IUnknown* punk_view )
{
	unregister_view( );

	m_ptr_active_view = punk_view;
	
	if( m_ptr_active_view )
		Register( m_ptr_active_view );
}

/////////////////////////////////////////////////////////////////////////////
void data_context_ctrl::unregister_view( )
{
	if( m_ptr_active_view )
		UnRegister( m_ptr_active_view );
	
	m_ptr_active_view = 0;
}

/////////////////////////////////////////////////////////////////////////////
void data_context_ctrl::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szAppActivateView ) )
	{	
		if( punkFrom && punkHit )
		{
			VERIFY( CheckInterface( punkFrom, IID_IView ) );
			VERIFY( CheckInterface( punkHit, IID_IDocument ) );
			_on_activate_view( punkHit, punkFrom );
		}
	}
	else if( !strcmp( pszEvent, "AfterClose" ) )
	{
		VERIFY( CheckInterface( punkFrom, IID_IDocument ) );
		_on_close_document( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		VERIFY( CheckInterface( punkFrom, IID_IView ) );
		_on_destroy_view( punkFrom );
	}
	else if( !strcmp( pszEvent, szEventActiveContextChange ) )
	{		
		if( cbSize == sizeof( NotifyCodes ) )
		{			
			if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( m_ptr_active_view ) )
			{
				NotifyCodes* pnc = (NotifyCodes*)(pdata);
				_on_data_context_change( punkFrom, punkHit, *pnc );
			}
		}
	}
	else if( !strcmp( pszEvent, szEventActivateObject ) )
	{
		//if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( m_ptr_active_view ) )
		{
			//set_active_object_to_controls( get_active_object_name_from_context() );			
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
//notifications
bool data_context_ctrl::_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view )
{
	if( !CheckInterface( punk_doc, IID_IDocument ) )
		return false;

	if( !CheckInterface( punk_view, IID_IView ) )
		return false;

	if( ::GetObjectKey( punk_doc ) == GetObjectKey( m_ptr_active_doc ) &&
		::GetObjectKey( punk_view ) == GetObjectKey( m_ptr_active_view ) )
		return true;

	register_doc( punk_doc );
	register_view( punk_view );

	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss )
	{
		_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
		bstr_func += "_";
		bstr_func += ON_ACTIVATE_VIEW;

		ptr_ss->Invoke( bstr_func, 0, 0, 0, fwAppScript );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::_on_close_document( IUnknown* punk_doc )
{
	if( !CheckInterface( punk_doc, IID_IDocument ) )
		return false;

	if( ::GetObjectKey( m_ptr_active_doc ) == ::GetObjectKey( punk_doc ) )
		unregister_doc( );

	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss )
	{
		_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
		bstr_func += "_";
		bstr_func += ON_CLOSE_DOC;

		ptr_ss->Invoke( bstr_func, 0, 0, 0, fwAppScript );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::_on_destroy_view( IUnknown* punk_view )
{
	if( !CheckInterface( punk_view, IID_IView ) )
		return false;

	if( ::GetObjectKey( m_ptr_active_view ) == ::GetObjectKey( punk_view ) )
		unregister_view( );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::_on_data_context_change( IUnknown* punk_view, IUnknown* punkObj, NotifyCodes nc )
{
	if( !punk_view || !punkObj )
		return false;

	if( ::GetObjectKey( punk_view ) != ::GetObjectKey( m_ptr_active_view ) )
		return false;

	if( nc == ncAddObject )
	{
		IDispatchPtr	ptr_disp = punkObj;
		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if( ptr_disp != 0 && ptr_ss != 0 )
		{
			_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
			bstr_func += "_";
			bstr_func += ON_ADD_OBJECT;

			_variant_t arg = (IDispatch*)ptr_disp;
			ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );
		}

		/*
		CString str_object_name = ::GetObjectName( punkObj );		
		if( !str_object_name.IsEmpty() )
		{
			for( int i=0;i<m_ar_combo.GetSize();i++ )
			{
				HWND hwnd = m_ar_combo[i];
				int idx = get_item_idx_by_name( hwnd, (LPCSTR)str_object_name );
				if( idx == -1 )
				{
					::SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)str_object_name );
				}
				else
				{
					ASSERT( false );
				}
			}
		}

		set_active_object_to_controls( get_active_object_name_from_context() );
		*/

	}
	else if( nc == ncRemoveObject )
	{
		IDispatchPtr	ptr_disp = punkObj;
		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if( ptr_disp != 0 && ptr_ss != 0 )
		{
			_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
			bstr_func += "_";
			bstr_func += ON_DELETE_OBJECT;

			_variant_t arg = (IDispatch*)ptr_disp;
			ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );
		}

		/*
		CString str_object_name = ::GetObjectName( punkObj );		
		if( !str_object_name.IsEmpty() )
		{
			for( int i=0;i<m_ar_combo.GetSize();i++ )
			{
				HWND hwnd = m_ar_combo[i];
				int idx = get_item_idx_by_name( hwnd, (LPCSTR)str_object_name );
				if( idx != -1 )
				{
					::SendMessage( hwnd, CB_DELETESTRING, idx, 0L );
				}
				else
				{
					ASSERT( false );
				}
			}
		}

		set_active_object_to_controls( get_active_object_name_from_context() );
		*/
	}

	return true;
}

/*
bool data_context_ctrl::fill_controls()
{
	for( int i=0;i<m_ar_combo.GetSize();i++ )
		fill_control( m_ar_combo[i] );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::fill_control( HWND hwnd )
{
	IDataContext3Ptr ptr_dc( m_ptr_active_view );

	if( !hwnd || ptr_dc == 0 )
		return false;	

	//delete all items
	::SendMessage( hwnd, CB_RESETCONTENT, 0L, 0L );

	CString str_name;

	long lpos = 0;
	_bstr_t bstr_type = get_object_type();
	ptr_dc->GetFirstObjectPos( bstr_type, &lpos );
	while( lpos )
	{
		IUnknown* punk = 0;
		ptr_dc->GetNextObject( bstr_type, &lpos, &punk );
		str_name = ::GetObjectName( punk );
		punk->Release();

		::SendMessage( hwnd, CB_ADDSTRING, 0L, (LPARAM) (LPCTSTR)str_name );
	}	

	set_active_object_to_controls( get_active_object_name_from_context() );

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::set_active_object_to_controls( const char* psz_object_name )
{
	if( !strcmp( m_str_active_object, psz_object_name ) )
		return true;

	m_str_active_object = psz_object_name;

	for( int i=0;i<m_ar_combo.GetSize();i++ )
		set_active_object_to_control( m_ar_combo[i], psz_object_name );


	{
		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if( ptr_ss )
		{
			_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
			bstr_func += "_";
			bstr_func += ON_ACTIVE_OBJECT_CHANGE;

			_variant_t arg = _bstr_t( (LPCSTR)m_str_active_object );
			
			ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );
		}
	}


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::set_active_object_to_control( HWND hwnd, const char* psz_object_name )
{
	if( !hwnd || !psz_object_name )
		return false;

	::SendMessage( hwnd, CB_SELECTSTRING, 0L, (LPARAM)psz_object_name );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
const char*	data_context_ctrl::get_object_type()
{
	if( !m_str_object_type.IsEmpty() )
		return m_str_object_type;


	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss )
	{
		_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
		bstr_func += "_";
		bstr_func += GET_OBJECT_TYPE;

		_variant_t res;
		if( S_OK == ptr_ss->Invoke( bstr_func, 0, 0, &res, fwAppScript ) )
		{
			
		}
	}

	return (LPCSTR)m_str_object_type;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool data_context_ctrl::is_object_match( IUnknown* punk_obj )
{
	if( !punk_obj )
		return false;

	IDispatchPtr ptr_disp( punk_obj );
	if( ptr_disp == 0 )
		return true;

	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss )
	{
		_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
		bstr_func += "_";
		bstr_func += ON_ACTIVE_OBJECT_CHANGE;

		_variant_t arg = _bstr_t( (LPCSTR)m_str_active_object );
		
		ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );
	}


	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
CString	data_context_ctrl::get_active_object_name_from_context()
{
	if( m_ptr_active_view == 0 ) return "";

	IDataContext3Ptr ptr_dc( m_ptr_active_view );
	if( ptr_dc == 0 )	return "";

	CString str_name;

	_bstr_t bstr_type = get_object_type();
	long lpos = 0;
	ptr_dc->GetFirstSelectedPos( bstr_type, &lpos );
	if( !lpos )//try determine base object key
	{
		INamedDataPtr ptr_nd( m_ptr_active_doc );
		if( ptr_nd )
		{
			IViewPtr ptr_view( m_ptr_active_view );
			if( ptr_view )
			{
				ptr_view->GetFirstVisibleObjectPosition( &lpos );
				while( lpos )
				{
					IUnknown* punk_obj = 0;
					ptr_view->GetNextVisibleObject( &punk_obj, &lpos );
					INamedDataObject2Ptr ptr_obj( punk_obj );
					punk_obj->Release();	punk_obj = 0;

					GUID guid_base = INVALID_KEY;
					ptr_obj->GetBaseKey( &guid_base );

					
					GUID guid_group = guid_base;
					long lpos_group = 0;					
					ptr_nd->GetBaseGroupObjectFirstPos( &guid_group, &lpos_group );
					while( lpos_group )
					{
						IUnknown* punk_obj = 0;
						ptr_nd->GetBaseGroupNextObject( &guid_group, &lpos_group, &punk_obj );
						if( !punk_obj )	continue;						

						IUnknownPtr ptr_obj = punk_obj;
						punk_obj->Release();	punk_obj = 0;						

						if( ::GetObjectKind( ptr_obj ) != get_object_type() )
							continue;

						if( !is_object_match( ptr_obj ) )
							continue;

						return ::GetObjectName( ptr_obj );
					}
				}
			}
		}
	}

	IUnknown* punk = 0;
	ptr_dc->GetNextSelected( bstr_type, &lpos, &punk );
	str_name = ::GetObjectName( punk );

	if( !is_object_match( punk ) )
	{
		if( punk )
			punk->Release();	punk = 0;
		
		return "";
	}

	if( punk )
		punk->Release();	punk = 0;

	return str_name;
}

////////////////////////////////////////////////////////////////////////////////////////////
int data_context_ctrl::get_item_idx_by_name( HWND hwnd, const char* psz_item )
{
	if( !hwnd || !psz_item )	return -1;

	int ncount = ::SendMessage( hwnd, CB_GETCOUNT, 0, 0 );
	for( int i=0;i<ncount;i++ )
	{
		char sz_buf[1024];	sz_buf[0] = 0;
		::SendMessage( hwnd, CB_GETLBTEXT, i, (LPARAM)sz_buf );
		if( !strcmp( sz_buf, (LPCSTR)psz_item ) )
			return i;				
	}

	return -1;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	IDataContextCtrl interface part
//
//
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT data_context_ctrl::XDataContextCtrl::AddComboBox( HWND hwnd_combo )
{
	_try_nested(data_context_ctrl, DataContextCtrl, AddComboBox )
	{	
		pThis->m_ar_combo.Add( hwnd_combo );
		{
			IScriptSitePtr	ptr_ss = ::GetAppUnknown();
			if( ptr_ss )
			{
				_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
				bstr_func += "_";
				bstr_func += ON_COMBO_CREATE;

				_variant_t var_arg = (long)hwnd_combo;

				ptr_ss->Invoke( bstr_func, &var_arg, 1, 0, fwAppScript );
			}

		}
		//pThis->fill_control( hwnd_combo );
		return S_OK;
	}
	
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT data_context_ctrl::XDataContextCtrl::RemoveComboBox( HWND hwnd_combo )
{
	_try_nested(data_context_ctrl, DataContextCtrl, RemoveComboBox )
	{	
		for( int i=0;i<pThis->m_ar_combo.GetSize();i++ )
		{
			if( pThis->m_ar_combo[i] = hwnd_combo )
			{
				pThis->m_ar_combo.RemoveAt( i );
				return S_OK;
			}
		}
		return S_FALSE;
	}
	
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT data_context_ctrl::XDataContextCtrl::OnSelChange( HWND hwnd_combo )
{
	_try_nested(data_context_ctrl, DataContextCtrl, OnSelChange )
	{	
		if( !hwnd_combo )
			return S_FALSE;


		int nidx = ::SendMessage( hwnd_combo, CB_GETCURSEL, 0, 0 );
		if( nidx == -1 ) 
			return S_FALSE;

		//at first sync
		//pThis->SetCurSel( nidx );


		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if( ptr_ss == 0 )
			return S_FALSE;

		_bstr_t bstr_func = DATA_CONTEXT_CTRL_SECTION;
		bstr_func += "_";
		bstr_func += ON_SELECTION_CHANGE;

		_variant_t arg = long( nidx );
		
		ptr_ss->Invoke( bstr_func, &arg, 1, 0, fwAppScript );

		return S_OK;
	}
	
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT data_context_ctrl::XDataContextCtrl::IsAvailable( BOOL* pbavailable )
{
	_try_nested(data_context_ctrl, DataContextCtrl, IsAvailable )
	{	
		if( !pbavailable ) 
			return E_INVALIDARG;

		*pbavailable = pThis->m_mEnable;
		return S_OK;
	}
	
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
IDataContextCtrlPtr	GetDataContextCtrl()
{
	IUnknown	*punk = NULL;
	punk = _GetOtherComponent( GetAppUnknown(), IID_IDataContextCtrl );
	if( !punk )
		return 0;

	IDataContextCtrlPtr ptr_dcc = punk;
	punk->Release();	punk = 0;

	return ptr_dcc;
}

////////////////////////////////////////////////////////////////////////////////////////////
long data_context_ctrl::GetItemCount() 
{
	if( !m_ar_combo.GetSize() )
		return 0;
	
	return ::SendMessage( m_ar_combo[0], CB_GETCOUNT, 0, 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////
BSTR data_context_ctrl::GetItemString(long idx) 
{
	CString strResult;	

	if( m_ar_combo.GetSize() != 0 )
	{
		char sz_buf[256];	sz_buf[0] = 0;
		::SendMessage( m_ar_combo[0], CB_GETLBTEXT, idx, (LPARAM)sz_buf );
		strResult = sz_buf;
	}	

	return strResult.AllocSysString();
}

////////////////////////////////////////////////////////////////////////////////////////////
long data_context_ctrl::GetItemData(long idx) 
{
	if( m_ar_combo.GetSize() != 0 )
		return ::SendMessage( m_ar_combo[0], CB_GETITEMDATA, idx, 0 );	

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL data_context_ctrl::DeleteItem(long idx) 
{
	if( !m_ar_combo.GetSize() )
		return FALSE;

	::SendMessage( m_ar_combo[0], CB_DELETESTRING, idx, 0 );	

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
long data_context_ctrl::AddItem(LPCTSTR str_item, long data, long idx) 
{
	if( !m_ar_combo.GetSize() )
		return -1;

	int lpos = ::SendMessage( m_ar_combo[0], CB_INSERTSTRING, idx, (LPARAM)str_item );
	::SendMessage( m_ar_combo[0], CB_SETITEMDATA, lpos, data );	

	return lpos;
}

////////////////////////////////////////////////////////////////////////////////////////////
long data_context_ctrl::GetCurSel() 
{
	if( !m_ar_combo.GetSize() )
		return -1;

	return ::SendMessage( m_ar_combo[0], CB_GETCURSEL, 0, 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL data_context_ctrl::SetCurSel(long idx) 
{
	if( !m_ar_combo.GetSize() )
		return -1;

	return ( -1 != ::SendMessage( m_ar_combo[0], CB_SETCURSEL, idx, 0 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL data_context_ctrl::ResetContent() 
{
	if( !m_ar_combo.GetSize() )
		return FALSE;

	::SendMessage( m_ar_combo[0], CB_RESETCONTENT, 0, 0 );
	
	return TRUE;

}

void data_context_ctrl::SetEnable(BOOL benable) 
{
	if( !m_ar_combo.GetSize() )
		return;

	//::EnableWindow( m_ar_combo[0], benable );

	m_mEnable = benable;

	IApplicationPtr ptr_app( ::GetAppUnknown() );

	if( ptr_app )	ptr_app->IdleUpdate();
}

