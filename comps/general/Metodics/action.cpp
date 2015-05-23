#include "StdAfx.h"
#include "action.h"
#include "ParamDialog.h"
#include "method_int.h"

namespace MetodicsTabSpace
{
namespace Actions
{

//////////////////////////////////////////////////////////////////////
//// CActionAddPane //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddPane, CCmdTargetEx);

// {64F7C8D5-232B-42b8-A815-FB041DF5CE01}
GUARD_IMPLEMENT_OLECREATE( CActionAddPane, "Metodics.AddMtdPane",
0x64f7c8d5, 0x232b, 0x42b8, 0xa8, 0x15, 0xfb, 0x4, 0x1d, 0xf5, 0xce, 0x1);

// {EC79D4E9-E3D9-4e27-A013-7BB346C0CF00}
static const GUID guidActionAddPane = 
{ 0xec79d4e9, 0xe3d9, 0x4e27, { 0xa0, 0x13, 0x7b, 0xb3, 0x46, 0xc0, 0xcf, 0x0 } };

ACTION_INFO_FULL( CActionAddPane, IDS_ACTION_ADD_PANE, -1, -1, guidActionAddPane );
ACTION_TARGET( CActionAddPane, szTargetApplication );

ACTION_ARG_LIST( CActionAddPane )
	ARG_STRING("PaneName", "" )
END_ACTION_ARG_LIST()


bool CActionAddPane::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();

		CString strName = GetArgumentString( "PaneName" );
		if( !strName.IsEmpty() )
			sptrTab->AddPane( _bstr_t( strName ) );
	}
	
	return true;
}

bool CActionAddPane::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	punk->Release();

	return true;
}

bool CActionAddPane::ExecuteSettings( CWnd *pwndParent )
{
	CString strName = GetArgumentString( "PaneName" );

	if( strName.IsEmpty() )
	{
		CAddPaneParamDlg dlg;
		if( dlg.DoModal() == IDOK )
			SetArgument( "PaneName", _variant_t( _bstr_t( dlg.m_strName ) ) );
	}

	return true;
} 
//////////////////////////////////////////////////////////////////////
//// CActionRemoveMtdPane ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionRemoveMtdPane, CCmdTargetEx);

// {F13F8692-2BE1-4845-8FBB-99BE43E091ED}
GUARD_IMPLEMENT_OLECREATE( CActionRemoveMtdPane, "Metodics.RemoveMtdPane",
0xf13f8692, 0x2be1, 0x4845, 0x8f, 0xbb, 0x99, 0xbe, 0x43, 0xe0, 0x91, 0xed);

// {45F54F87-560C-46c6-8961-68DC139B5A79}
static const GUID guidActionRemovePane = 
{ 0x45f54f87, 0x560c, 0x46c6, { 0x89, 0x61, 0x68, 0xdc, 0x13, 0x9b, 0x5a, 0x79 } };


ACTION_INFO_FULL( CActionRemoveMtdPane, IDS_ACTION_REMOVE_PANE, -1, -1, guidActionRemovePane );
ACTION_TARGET( CActionRemoveMtdPane, szTargetApplication );

bool CActionRemoveMtdPane::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();
		
		long lPane = -1;
		sptrTab->GetActivePane( &lPane );

		sptrTab->Remove( lPane );
	}
	
	return true;
}

bool CActionRemoveMtdPane::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	IMethodTabPtr sptrTab = punk;
	punk->Release();

	long lCount = 0;
	sptrTab->GetPaneCount( &lCount );

	if( !lCount )
		return false;

	long lPane = -1;
	sptrTab->GetActivePane( &lPane );

	if( lPane == -1 )
		return false;


	return true;
}			 
//////////////////////////////////////////////////////////////////////
//// CActionMtdLoadContent ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdLoadContent, CCmdTargetEx);

// {FA16A179-9BAE-49b4-91C3-2F42A71223FB}
GUARD_IMPLEMENT_OLECREATE( CActionMtdLoadContent, "Metodics.LoadMtdContent",
0xfa16a179, 0x9bae, 0x49b4, 0x91, 0xc3, 0x2f, 0x42, 0xa7, 0x12, 0x23, 0xfb);

// {B9A0CD4C-D2F5-4d0b-A8E8-E9AA038929AE}
static const GUID guidActionMtdLoadContent = 
{ 0xb9a0cd4c, 0xd2f5, 0x4d0b, { 0xa8, 0xe8, 0xe9, 0xaa, 0x3, 0x89, 0x29, 0xae } };

ACTION_INFO_FULL( CActionMtdLoadContent, IDS_ACTION_MTD_LOAD_CONTENT, -1, -1, guidActionMtdLoadContent );
ACTION_TARGET( CActionMtdLoadContent, szTargetApplication );

ACTION_ARG_LIST( CActionMtdLoadContent )
	ARG_STRING("FileName", "" )
END_ACTION_ARG_LIST()


bool CActionMtdLoadContent::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();

		CString strName = GetArgumentString( "FileName" );
		if( !strName.IsEmpty() )
			sptrTab->LoadContent( _bstr_t( strName ) );
	}
	
	return true;
}

bool CActionMtdLoadContent::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	punk->Release();

	return true;
}

bool CActionMtdLoadContent::ExecuteSettings( CWnd *pwndParent )
{
	CString strName = GetArgumentString( "FileName" );

	if( strName.IsEmpty() )
	{
		CString	strLast = ::GetValueString( ::GetAppUnknown(), "\\General", "CurMtdTabContent", "" );
		
		CString strFilter;
		strFilter.LoadString( IDS_FILTER_STR );

		if( strLast.IsEmpty() )
			strLast = ::GetValueString( ::GetAppUnknown(), "\\Path", "States", "" ) + "*.smtd";

		CFileDialog	dlg( TRUE, ".smtd", strLast,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, 0, _FILE_OPEN_SIZE_ );

		if( dlg.DoModal() == IDOK )
			SetArgument( "FileName", _variant_t( _bstr_t( dlg.GetPathName() ) ) );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//// CActionMtdStoreContent //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdStoreContent, CCmdTargetEx);

// {7115B91E-33AB-48b5-A48F-5B422B0A8DD5}
GUARD_IMPLEMENT_OLECREATE( CActionMtdStoreContent, "Metodics.StoreMtdContent",
0x7115b91e, 0x33ab, 0x48b5, 0xa4, 0x8f, 0x5b, 0x42, 0x2b, 0xa, 0x8d, 0xd5);

// {2408FD73-40FF-4384-B262-7A19A252446D}
static const GUID guidActionMtdStoreContent = 
{ 0x2408fd73, 0x40ff, 0x4384, { 0xb2, 0x62, 0x7a, 0x19, 0xa2, 0x52, 0x44, 0x6d } };


ACTION_INFO_FULL( CActionMtdStoreContent, IDS_ACTION_MTD_STORE_CONTENT, -1, -1, guidActionMtdStoreContent );
ACTION_TARGET( CActionMtdStoreContent, szTargetApplication );

ACTION_ARG_LIST( CActionMtdStoreContent )
	ARG_STRING("FileName", "" )
END_ACTION_ARG_LIST()


bool CActionMtdStoreContent::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();

		CString strName = GetArgumentString( "FileName" );
		if( !strName.IsEmpty() )
		{
			HRESULT hr = sptrTab->StoreContent( _bstr_t( strName ) );
			if( hr == S_OK )
				::SetValue( ::GetAppUnknown(), "\\General", "CurMtdTabContent", strName );
		}
	}
	
	return true;
}

bool CActionMtdStoreContent::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	punk->Release();

	return true;
}

bool CActionMtdStoreContent::ExecuteSettings( CWnd *pwndParent )
{
	CString strName = GetArgumentString( "FileName" );

	if( strName.IsEmpty() )
	{
		CString	strLast = ::GetValueString( ::GetAppUnknown(), "\\General", "CurMtdTabContent", "" );
		
		CString strFilter;
		strFilter.LoadString( IDS_FILTER_STR );

		if( strLast.IsEmpty() )
			strLast = ::GetValueString( ::GetAppUnknown(), "\\Path", "States", "" ) + "*.smtd";

		CFileDialog	dlg( FALSE, ".smtd", strLast,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, 0, _FILE_OPEN_SIZE_ );

		if( dlg.DoModal() == IDOK )
			SetArgument( "FileName", _variant_t( _bstr_t( dlg.GetPathName() ) ) );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//// CActionRemoveItem //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionRemoveItem, CCmdTargetEx);

// {E979661D-F787-4ee7-858B-693E3276A071}
GUARD_IMPLEMENT_OLECREATE( CActionRemoveItem, "Metodics.RemoveItem",
0xe979661d, 0xf787, 0x4ee7, 0x85, 0x8b, 0x69, 0x3e, 0x32, 0x76, 0xa0, 0x71);

// {951A74BC-0B2A-4d20-A686-A3E14DF8E45A}
static const GUID guidActionMtdRemoveItem = 
{ 0x951a74bc, 0xb2a, 0x4d20, { 0xa6, 0x86, 0xa3, 0xe1, 0x4d, 0xf8, 0xe4, 0x5a } };

ACTION_INFO_FULL( CActionRemoveItem, IDS_ACTION_MTD_REMOVE_ITEM, -1, -1, guidActionMtdRemoveItem );
ACTION_TARGET( CActionRemoveItem, szTargetApplication );

bool CActionRemoveItem::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();

		POINT ptContext = {0};

		if( sptrTab->GetContextPoint( &ptContext ) != S_OK )
			return false;

		if( sptrTab->PanePaneItemByPoint( ptContext ) != S_OK )
			return false;
/*
		long lPaneIndex = -1;
		if( sptrTab->GetPaneIndexbyPoint( ptContext, &lPaneIndex ) != S_OK )
			return false;

		long lItemIndex = -1;
		if( sptrTab->GetItemIndexbyPoint( ptContext, &lItemIndex ) != S_OK )
			return false;

		if( sptrTab->RemovePaneItem( lPaneIndex, lItemIndex ) != S_OK )
			return false;
		
		if( sptrTab->SetActivePane( lPaneIndex ) != S_OK )
			return false;
*/		 

	}
	
	return true;
}

bool CActionRemoveItem::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	punk->Release();

	return true;
}

//////////////////////////////////////////////////////////////////////
//// CActionOpenPane //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionOpenPane, CCmdTargetEx);

// {3596EE0D-B73A-44e6-A615-EFDE400501CB}
GUARD_IMPLEMENT_OLECREATE( CActionOpenPane, "Metodics.OpenMtdPane",
0x3596ee0d, 0xb73a, 0x44e6, 0xa6, 0x15, 0xef, 0xde, 0x40, 0x5, 0x1, 0xcb);

// {2DA416C9-D684-46d2-93E4-F324F943142C}
static const GUID guidActionOpenPane = 
{ 0x2da416c9, 0xd684, 0x46d2, { 0x93, 0xe4, 0xf3, 0x24, 0xf9, 0x43, 0x14, 0x2c } };

ACTION_INFO_FULL( CActionOpenPane, IDS_ACTION_OPEN_MTD_PANE, -1, -1, guidActionOpenPane );
ACTION_TARGET( CActionOpenPane, szTargetApplication );

ACTION_ARG_LIST( CActionOpenPane )
	ARG_STRING("PanePath", "" )
	ARG_INT("Open", 1 )
END_ACTION_ARG_LIST()

bool CActionOpenPane::Invoke()
{
	CString str_path	= GetArgumentString( "PanePath" );
	bool bopen			= GetArgumentInt( "Open" ) != 0;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptr_tab = punk;
		punk->Release();

		if( sptr_tab )
			sptr_tab->OpenPane( _bstr_t( str_path ), bopen );
	}


	return true;
}

bool CActionOpenPane::IsAvaible()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
//// CActionRemoveItem //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddItem, CCmdTargetEx);
// {2E377243-2A19-46a5-BE78-5F47173DD433}
GUARD_IMPLEMENT_OLECREATE( CActionAddItem, "Metodics.AddItem",
0x2e377243, 0x2a19, 0x46a5, 0xbe, 0x78, 0x5f, 0x47, 0x17, 0x3d, 0xd4, 0x33);

// {58E83298-7288-4403-B9C7-89E557D4E8AF}
static const GUID guidActionMtdAddItem = 
{ 0x58e83298, 0x7288, 0x4403, { 0xb9, 0xc7, 0x89, 0xe5, 0x57, 0xd4, 0xe8, 0xaf } };

ACTION_INFO_FULL( CActionAddItem, IDS_ACTION_MTD_ADD_ITEM, -1, -1, guidActionMtdAddItem );
ACTION_TARGET( CActionAddItem, szTargetApplication );

bool CActionAddItem::Invoke()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab); 

	if( punk )
	{
		IMethodTabPtr sptrTab = punk;
		punk->Release();

		POINT ptContext = {0};

		if( sptrTab->GetContextPoint( &ptContext ) != S_OK )
			return false;

		if( sptrTab->PanePaneItemByPoint( ptContext ) != S_OK )
			return false;
/*
		long lPaneIndex = -1;
		if( sptrTab->GetPaneIndexbyPoint( ptContext, &lPaneIndex ) != S_OK )
			return false;

		long lItemIndex = -1;
		if( sptrTab->GetItemIndexbyPoint( ptContext, &lItemIndex ) != S_OK )
			return false;

		if( sptrTab->RemovePaneItem( lPaneIndex, lItemIndex ) != S_OK )
			return false;
		
		if( sptrTab->SetActivePane( lPaneIndex ) != S_OK )
			return false;
*/		 

	}
	
	return true;
}

bool CActionAddItem::IsAvaible()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_IMethodTab ); 

	if( !punk )
		return false;

	punk->Release();

	return true;
}

}
}