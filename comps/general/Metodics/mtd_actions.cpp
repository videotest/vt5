#include "StdAfx.h"
#include "resource.h"
#include "mtd_utils.h"
#include "mtd_actions.h"
#include "InputMtdName.h"
#include "InputMsg.h"
#include "ScriptNotifyInt.h"
#include "stdlib.h"
#include "MethodStep.h"
#include "PropBag.h"
#include "\vt5\awin\misc_map.h"
#include "MethodManState.h"



////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdList
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdList, CCmdTargetEx);

// {3CC390B0-DB37-4a5d-9C33-E8FD4A1A442C}
GUARD_IMPLEMENT_OLECREATE( CActionMtdList, "Metodics.MtdList", 
						  0x3cc390b0, 0xdb37, 0x4a5d, 0x9c, 0x33, 0xe8, 0xfd, 0x4a, 0x1a, 0x44, 0x2c);

// {FADA4C04-DFDB-45f0-8352-28CC3111278D}
static const GUID guid_mtd_list = 
{ 0xfada4c04, 0xdfdb, 0x45f0, { 0x83, 0x52, 0x28, 0xcc, 0x31, 0x11, 0x27, 0x8d } };

ACTION_INFO_FULL( CActionMtdList, IDS_ACTION_MTD_LIST, -1, -1, guid_mtd_list );
ACTION_TARGET( CActionMtdList, szTargetApplication );

//////////////////////////////////////////////////////////////////////
ACTION_UI( CActionMtdList, CMtdChooser )

//////////////////////////////////////////////////////////////////////
//CActionMtdList implementation


bool CActionMtdList::Invoke()
{
	return true;
}

//extended UI
bool CActionMtdList::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptrMan = ptr;
	if( sptrMan == 0 )
		return false;

	BOOL bOK = false;

	sptrMan->IsRecording( &bOK );

	if( bOK )
		return false;

	bOK = false;

	sptrMan->IsRunning( &bOK );

	if( bOK )
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
//
//	Class CMtdChooser
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CMtdChooser, CCmdTargetEx )

BEGIN_INTERFACE_MAP( CMtdChooser, CCmdTargetEx )
	INTERFACE_PART( CMtdChooser, IID_IUIComboBox, Combo )
	INTERFACE_PART(CMtdChooser, IID_IEventListener,	EvList)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN( CMtdChooser, Combo )

CMtdChooser::CMtdChooser()
{		
	m_hwnd_combo = 0;
	m_punkExcludeIt=0;
	m_lCurSel = 0;
	m_bVirtualFreeMode = true;
	Register( 0 );

	// register as MethodMan event listener
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	if( ptr_mtd_man ) Register( ptr_mtd_man );
}

CMtdChooser::~CMtdChooser()
{		
	UnRegister( 0 );

	// register as MethodMan event listener
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	if( ptr_mtd_man ) UnRegister( ptr_mtd_man );
}

//////////////////////////////////////////////////////////////////////

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

HRESULT CMtdChooser::XCombo::OnInitList( HWND hWnd )
{
	METHOD_PROLOGUE_EX(CMtdChooser, Combo)

		pThis->m_hwnd_combo = hWnd;

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false ); 

	// —оздавать ли искуственную Free Mode или ее роль играет одна из методик (неудал€ема€)
	pThis->m_bVirtualFreeMode = 0 != ::GetValueInt( GetAppUnknown(), "\\Methodics", "VirtualFreeMode", 1 );

	if( (ptr!=0) && hWnd )
	{
		if(pThis->m_bVirtualFreeMode)
		{
			CString strFree;
			strFree.LoadString( IDS_FREE_MODE );
			::SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)( (LPCTSTR)strFree ) );
		}

		IMethodManPtr sptrMan = ptr;

		if( sptrMan )
		{
			// get active method
			TPOS lpos_active_mtd = 0;
			sptrMan->GetActiveMethodPos( &lpos_active_mtd );

			_list_map_t<TPOS, CString, cmp_cstring> method_map;

			TPOS lPos = 0;
			sptrMan->GetFirstMethodPos( &lPos );

			while( lPos )
			{
				TPOS lPos0 = lPos;

				IUnknownPtr ptrMethod = 0;
				sptrMan->GetNextMethod( &lPos, &ptrMethod );
				IMethodPtr sptrMtd = ptrMethod;
				if(sptrMtd==0) continue;

				_bstr_t bstrName;
				sptrMtd->GetName( bstrName.GetAddress() );

				CString strName = bstrName.GetBSTR();
				CString strNameForSort = "";

				INamedPropBagPtr sptr_propbag_mtd = sptrMtd;
				if( sptr_propbag_mtd != 0 )
				{
					_variant_t var;
					sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropSortPrefix)), var.GetAddress() );
					var.ChangeType(VT_BSTR);
					if( var.vt == VT_BSTR )
						strNameForSort = var.bstrVal;
				}
				if(strNameForSort=="") strNameForSort = "method.";
				strNameForSort = strNameForSort + strName;

				method_map.set(lPos0, strNameForSort);
			}

			TPOS lMapPos = method_map.tail();

			int n = pThis->m_bVirtualFreeMode ? 1 : 0;
			pThis->m_lCurSel = 0;
			while( lMapPos )
			{
				TPOS lPos = method_map.get(lMapPos);
				lMapPos = method_map.prev(lMapPos);

				if(lPos==lpos_active_mtd) pThis->m_lCurSel = n;
				n++;

				IUnknown *punkMethod = 0;
				sptrMan->GetNextMethod( &lPos, &punkMethod );

				IMethodPtr sptrMtd = punkMethod;
				if( punkMethod ) 
					punkMethod->Release(); punkMethod = 0;

				_bstr_t bstrName;
				if( sptrMtd )
					sptrMtd->GetName( bstrName.GetAddress() );

				if(sptrMtd!=pThis->m_punkExcludeIt)
					::SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)( (char *)bstrName ) );
			}
		}
		::SendMessage( hWnd, CB_SETCURSEL,  pThis->m_lCurSel , 0 );

		{
			int nIndex = ::SendMessage( pThis->m_hwnd_combo, CB_GETCURSEL, 0, 0 );
			int nLen = ::SendMessage( pThis->m_hwnd_combo, CB_GETLBTEXTLEN, nIndex, 0 );
			nLen = max(10,nLen);

			CString strText;
			::SendMessage( pThis->m_hwnd_combo, CB_GETLBTEXT, nIndex, (LPARAM)( strText.GetBufferSetLength( nLen ) ) );
			strText.ReleaseBuffer();

			IScriptSitePtr sptr_site = ::GetAppUnknown();

			if( sptr_site )
			{
				_bstr_t bstr_event( "MethodMan_MtdActivate" );

				_variant_t var = strText;
				sptr_site->Invoke( bstr_event, &var, 1, 0, fwAppScript | fwFormScript );
			}
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CMtdChooser::XCombo::OnSelChange()
{
	METHOD_PROLOGUE_EX(CMtdChooser, Combo)

	int nIndex = ::SendMessage( pThis->m_hwnd_combo, CB_GETCURSEL, 0, 0 );
	int nLen = ::SendMessage( pThis->m_hwnd_combo, CB_GETLBTEXTLEN, nIndex, 0 );

	pThis->m_lCurSel = nIndex;

	CString strText;
	::SendMessage( pThis->m_hwnd_combo, CB_GETLBTEXT, nIndex, (LPARAM)( strText.GetBufferSetLength( nLen ) ) );
	strText.ReleaseBuffer();

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 

	IScriptSitePtr sptr_site = ::GetAppUnknown();

	if( sptr_site )
	{
		_bstr_t bstr_event( "MethodMan_MtdActivate" );
		
		_variant_t var = strText;
		sptr_site->Invoke( bstr_event, &var, 1, 0, fwAppScript | fwFormScript );
	}

	if( (!nIndex) && pThis->m_bVirtualFreeMode )
	{
		IMethodManPtr sptrMan = ptr;

		if( sptrMan )
		{
			sptrMan->SetActiveMethodPos(0);
			return S_OK;
		}
	}


	if( (ptr!=0) && !strText.IsEmpty() )
	{
		IMethodManPtr sptrMan = ptr;

		if( sptrMan )
		{
			TPOS lPos = 0;
			sptrMan->GetFirstMethodPos( &lPos );

			while( lPos )
			{
				IUnknown *punkMethod = 0;
				TPOS lPos1 = lPos;
				sptrMan->GetNextMethod( &lPos, &punkMethod );

				IMethodPtr sptrMtd = punkMethod;
				if( punkMethod ) 
					punkMethod->Release(); punkMethod = 0;

				_bstr_t bstrName;
				if( sptrMtd )
					sptrMtd->GetName( bstrName.GetAddress() );

				if( strText == (char *)bstrName )
				{
					sptrMan->SetActiveMethodPos( lPos1 );
					break;
				}
			}
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CMtdChooser::XCombo::OnDetachWindow()
{
	METHOD_PROLOGUE_EX(CMtdChooser, Combo)
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CMtdChooser::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventNewSettings ) )
	{
		::SendMessage( m_hwnd_combo, CB_RESETCONTENT, 0, 0 );

		IUIComboBoxPtr sptrCombo = GetControllingUnknown();
		sptrCombo->OnInitList( m_hwnd_combo );

	}
	if( !strcmp( pszEvent, szEventChangeMethod ) )
	{
		long	lHint= *(long*)pdata;

		if(lHint==cncAdd || lHint==cncRemove || lHint==cncReset)
		{
			::SendMessage( m_hwnd_combo, CB_RESETCONTENT, 0, 0 );

			IUIComboBoxPtr sptrCombo = GetControllingUnknown();
			if(cncRemove==lHint) m_punkExcludeIt = punkHit;
			sptrCombo->OnInitList( m_hwnd_combo );
			m_punkExcludeIt = 0;
		}

		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false);
		IMethodManPtr sptr_mtd_man(ptr_mtd_man);
		if(sptr_mtd_man!=0 && lHint != cncDeactivate/*SBT1436*/)
		{
			// get active method
			TPOS lpos_active_mtd = 0;
			sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd );

			IUnknownPtr ptr_active_mtd = 0;
			sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &ptr_active_mtd);

			IMethodPtr sptr_work_mtd(ptr_active_mtd);

			if(sptr_work_mtd!=0)
			{
				_bstr_t bstrt_name;
				if( S_OK == sptr_work_mtd->GetName( bstrt_name.GetAddress() ) )
				{
					//LRESULT num = SendMessage( m_hwnd_combo, CB_FINDSTRING, -1, (LPARAM)( (char *)bstrt_name ) );

					for(int nIndex = 0; true; nIndex++)
					{
						int nLen = ::SendMessage( m_hwnd_combo, CB_GETLBTEXTLEN, nIndex, 0 );
						if(nLen==CB_ERR) break;

						CString strText;
						::SendMessage( m_hwnd_combo, CB_GETLBTEXT, nIndex, (LPARAM)( strText.GetBufferSetLength( nLen ) ) );
						strText.ReleaseBuffer();

						if(_bstr_t(strText) == bstrt_name)
						{
							m_lCurSel = nIndex;
							::SendMessage( m_hwnd_combo, CB_SETCURSEL,  m_lCurSel , 0 );
						}
					}
				}
			}
			else
			{ // нет активной методики - подсветить Free Mode (если Free Mode запрещена - то хоть кого-нибудь)
				m_lCurSel = 0;
				::SendMessage( m_hwnd_combo, CB_SETCURSEL,  m_lCurSel , 0 );
			}
		}
		if (lHint == cncActivate)
		{	
			int nIndex = ::SendMessage( m_hwnd_combo, CB_GETCURSEL, 0, 0 );
			int nLen = ::SendMessage( m_hwnd_combo, CB_GETLBTEXTLEN, nIndex, 0 );

			CString strText;
			::SendMessage( m_hwnd_combo, CB_GETLBTEXT, nIndex, (LPARAM)( strText.GetBufferSetLength( nLen ) ) );
			strText.ReleaseBuffer();

			IScriptSitePtr sptr_site = ::GetAppUnknown();

			if( sptr_site )
			{
				_bstr_t bstr_event( "MethodMan_MtdActivate" );
				
				_variant_t var = strText;
				sptr_site->Invoke( bstr_event, &var, 1, 0, fwAppScript | fwFormScript );
	}
}

	}
}

//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdRunStop
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdRunStop, CCmdTargetEx);

// {147F817F-A7A5-4fd9-ADC1-577495192307}
GUARD_IMPLEMENT_OLECREATE( CActionMtdRunStop, "Metodics.MtdRunStop", 
						  0x147f817f, 0xa7a5, 0x4fd9, 0xad, 0xc1, 0x57, 0x74, 0x95, 0x19, 0x23, 0x7);

// {7FB8E618-F7FE-4fcc-AB82-0EF4F13BF83D}
static const GUID guid_mtd_run_stop = 
{ 0x7fb8e618, 0xf7fe, 0x4fcc, { 0xab, 0x82, 0xe, 0xf4, 0xf1, 0x3b, 0xf8, 0x3d } };


ACTION_INFO_FULL( CActionMtdRunStop, IDS_ACTION_MTD_RUN_STOP, -1, -1, guid_mtd_run_stop );
ACTION_TARGET( CActionMtdRunStop, szTargetApplication );


//////////////////////////////////////////////////////////////////////
//CActionMtdRunStop implementation


bool CActionMtdRunStop::Invoke()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptrMan = ptr;
	if( sptrMan == 0 )
		return false;

	BOOL bOK = false;
	sptrMan->IsRunning( &bOK );

	if( !bOK )
		sptrMan->Run();
	else
		sptrMan->Stop();

	return true;
}

//extended UI
bool CActionMtdRunStop::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptrMan = ptr;
	if( sptrMan == 0 )
		return false;

	BOOL bOK = false;

	sptrMan->IsRecording( &bOK );

	if( bOK )
		return false;

	TPOS lPos = 0;
	sptrMan->GetActiveMethodPos( &lPos );

	if( !lPos )
		return false;

	// SBT 1447
	IUnknownPtr ptr_active_mtd = 0;
	if( S_OK != sptrMan->GetNextMethod( &lPos, &ptr_active_mtd ) )
		return false;

	INamedPropBagPtr sptr_propbag_mtd = ptr_active_mtd;
	if( sptr_propbag_mtd == 0 )
		return false;

	variant_t vcan_run;
	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanRun)), vcan_run.GetAddress() );
	if( vcan_run.vt == VT_I4 && !vcan_run.lVal )
		return false; // если выставлено CanRun=0 - то нефиг ("—вободный режим" или еще что)

	return true;
}

//extended UI
bool CActionMtdRunStop::IsChecked()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptrMan = ptr;
	if( sptrMan == 0 )
		return false;

	BOOL bOK = false;
	sptrMan->IsRunning( &bOK );

	return bOK != 0;
}

/*
//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdExportMethod
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdExportMethod, CCmdTargetEx);

// {6143BF59-5C57-4f7c-BAFA-68CB93EBC388}
GUARD_IMPLEMENT_OLECREATE( CActionMtdExportMethod, "Metodics.ExportMethod",
						  0x6143bf59, 0x5c57, 0x4f7c, 0xba, 0xfa, 0x68, 0xcb, 0x93, 0xeb, 0xc3, 0x88);

// {75CA3E2C-27AF-4024-AC6D-A76A406B95AF}
static const GUID guidActionMtdExportMethod = 
{ 0x75ca3e2c, 0x27af, 0x4024, { 0xac, 0x6d, 0xa7, 0x6a, 0x40, 0x6b, 0x95, 0xaf } };


ACTION_INFO_FULL( CActionMtdExportMethod, IDS_ACTION_MTD_EXPORT_METHOD, -1, -1, guidActionMtdExportMethod );
ACTION_TARGET( CActionMtdExportMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdExportMethod )
	ARG_STRING( "MethodName", "" )	
	ARG_STRING( "SavePath", "" )
	ARG_INT( "ShowInterfaceAnyway", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdExportMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdExportMethod::IsAvaible()
{
	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	IMethodManPtr sptr_mtd_man = punk_mtd_man;
	if( punk_mtd_man )
		punk_mtd_man->Release(); punk_mtd_man = 0;

	return sptr_mtd_man != 0;
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdExportMethod::Invoke()
{
	CString str_path( _T("") ),
		str_mtd_name( _T("") );
	BOOL	bforce_show = TRUE;
	str_path = GetArgumentString( "SavePath" );
	str_mtd_name = GetArgumentString( "MethodName" );
	bforce_show = GetArgumentInt( "ShowInterfaceAnyway" ) != 0;

	IMethodPtr sptr_work_mtd;
	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	IMethodManPtr sptr_mtd_man = punk_mtd_man;
	if( punk_mtd_man )
		punk_mtd_man->Release(); punk_mtd_man = 0;

	if( sptr_mtd_man == 0 )
		return false;

	if( str_mtd_name.IsEmpty() )
	{
		LPOS lpos_active_mtd = 0;
		if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
			return false;			

		IUnknown *punk_active_mtd = 0;
		if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
			return false;

		sptr_work_mtd = punk_active_mtd;
		if( punk_active_mtd )
			punk_active_mtd->Release(); punk_active_mtd = 0;
	}
	else
	{	
        LPOS lpos_mtd = 0;
		sptr_mtd_man->GetFirstMethodPos( &lpos_mtd );
		while( lpos_mtd )
		{
            IUnknown  *punk_mtd = 0;
			if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_mtd, &punk_mtd ) )
				break;
            
			IMethodPtr sptr_mtd = punk_mtd;
			if( punk_mtd )
				punk_mtd->Release(); punk_mtd = 0;

			if( sptr_mtd == 0 )
				break;

			_bstr_t bstrt_name;
			if( S_OK != sptr_mtd->GetName( bstrt_name.GetAddress() ) )
				break;

			if( str_mtd_name.Compare( bstrt_name ) )
			{
                sptr_work_mtd = sptr_mtd;
				break;
			}
		}        
	}

	if( sptr_work_mtd == 0 )
		return false;

	if( bforce_show || str_path.IsEmpty() || str_mtd_name.IsEmpty() )
	{
		if( str_path.IsEmpty() )
			str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	

		_bstr_t bstrt_name;
		sptr_work_mtd->GetName( bstrt_name.GetAddress() );
		if( !bstrt_name.length() )
			return false;

		str_mtd_name = (TCHAR*)bstrt_name;
		
        CString strFilter( _T("") );
		strFilter.LoadString( IDS_FILTER_MTD );

		IDispatchPtr sptrDisp;
		sptrDisp.CreateInstance( "Shell.App", 0, CLSCTX_INPROC_HANDLER );

		_variant_t varFlName = _bstr_t( str_path + str_mtd_name + _T(".method" ));
		_variant_t varFilter = _bstr_t( strFilter );

		VARIANT varArgs[2] = { 0 };
		varArgs[1] = varFlName;
		varArgs[0] = varFilter;

		VARIANT varRes={VT_EMPTY}; 
		varRes.bstrVal = 0;
		
		// clear
		str_path.Empty();
		str_mtd_name.Empty();

		// get
		_invoke_func( sptrDisp, _bstr_t( "DoSaveFileDialog" ), varArgs, 2, &varRes );

		if( varRes.bstrVal == 0 || _bstr_t( varRes.bstrVal ).length() == 0)
			return false;
		else
			str_path = varRes.bstrVal;
        
		sptrDisp = 0;

		int nIndex = str_path.ReverseFind( '\\' );
		str_mtd_name = str_path.Right( str_path.GetLength() - nIndex - 1 );
		str_path.Delete( nIndex + 1, str_path.GetLength() - nIndex - 1 );    

		nIndex = -1;
		nIndex = str_mtd_name.ReverseFind( '.' );
		if( nIndex != -1 )
            str_mtd_name.Delete( nIndex, str_mtd_name.GetLength() - nIndex );

		if( str_path.IsEmpty() || str_mtd_name.IsEmpty() )
			return false;
	}

	// save method
	if( S_OK != sptr_work_mtd->StoreFile(_bstr_t(str_path + str_mtd_name + _T(".method"))) )
		return false;

	if( str_path == ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", (const char*)"" ) )
		sptr_mtd_man->Reload();

    return true;
}


//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdImportMethod
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdImportMethod, CCmdTargetEx);

// {B616115F-A475-4aed-B73B-CE18A1B748C4}
GUARD_IMPLEMENT_OLECREATE( CActionMtdImportMethod, "Metodics.ImportMethod",
0xb616115f, 0xa475, 0x4aed, 0xb7, 0x3b, 0xce, 0x18, 0xa1, 0xb7, 0x48, 0xc4);

// {96282082-1FFE-45d0-934B-C8C210CE4CB8}
static const GUID guidActionMtdImportMethod = 
{ 0x96282082, 0x1ffe, 0x45d0, { 0x93, 0x4b, 0xc8, 0xc2, 0x10, 0xce, 0x4c, 0xb8 } };


ACTION_INFO_FULL( CActionMtdImportMethod, IDS_ACTION_MTD_IMPORT_METHOD, -1, -1, guidActionMtdImportMethod );
ACTION_TARGET( CActionMtdImportMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdImportMethod )
	ARG_STRING( "ImportPathName", "" )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdImportMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdImportMethod::IsAvaible()
{
	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	IMethodManPtr sptr_mtd_man = punk_mtd_man;
	if( punk_mtd_man )
		punk_mtd_man->Release(); punk_mtd_man = 0;

	return sptr_mtd_man != 0;
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdImportMethod::Invoke()
{
	CString str_path_name( _T("") );
	str_path_name = GetArgumentString( "ImportPathName" );
	
	IMethodPtr sptr_work_mtd;
	IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
	IMethodManPtr sptr_mtd_man = punk_mtd_man;
	if( punk_mtd_man )
		punk_mtd_man->Release(); punk_mtd_man = 0;

	if( sptr_mtd_man == 0 )
		return false;

	if( str_path_name.IsEmpty() )
	{
		str_path_name = ::GetValueString( GetAppUnknown(), "\\Paths", "CurrentPath",	(const char*)"" );	

		CString strFilter( _T("") );
		strFilter.LoadString( IDS_FILTER_MTD );

		IDispatchPtr sptrDisp;
		sptrDisp.CreateInstance( "Shell.App", 0, CLSCTX_INPROC_HANDLER );

		_variant_t varFlName;// = _bstr_t( str_path_name );
		_variant_t varFilter = _bstr_t( strFilter );

		VARIANT varArgs[2] = { 0 };
		varArgs[1] = varFlName;
		varArgs[0] = varFilter;

		VARIANT varRes={VT_EMPTY}; 
		varRes.bstrVal = 0;
		
		// get
		_invoke_func( sptrDisp, _bstr_t( "DoOpenFileDialog" ), varArgs, 2, &varRes );

		if( varRes.bstrVal == 0 || _bstr_t( varRes.bstrVal ).length() == 0)
			return false;
		else
			str_path_name = varRes.bstrVal;
        
		sptrDisp = 0;

		if( str_path_name.IsEmpty() )
			return false;
	}

	// load method

    return true;
}*/


//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdCreateMethod 
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdCreateMethod, CCmdTargetEx);

// {4BC03965-DDB6-4f7c-9C5C-641EAC4ACD25}
GUARD_IMPLEMENT_OLECREATE( CActionMtdCreateMethod, "Metodics.CreateMethod",
0x4bc03965, 0xddb6, 0x4f7c, 0x9c, 0x5c, 0x64, 0x1e, 0xac, 0x4a, 0xcd, 0x25);

// {1EBB3E3E-AB47-4d21-8337-5B8A2BBD8962}
static const GUID guidActionMtdCreateMethod = 
{ 0x1ebb3e3e, 0xab47, 0x4d21, { 0x83, 0x37, 0x5b, 0x8a, 0x2b, 0xbd, 0x89, 0x62 } };

ACTION_INFO_FULL( CActionMtdCreateMethod, IDS_ACTION_MTD_CREATE_METHOD, -1, -1, guidActionMtdCreateMethod );
ACTION_TARGET( CActionMtdCreateMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdCreateMethod )
	ARG_STRING( "MethodName", "" )
	ARG_INT( "Activate", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdCreateMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdCreateMethod ::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;
	
	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

    return true;	
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdCreateMethod::Invoke()
{
	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );

	BOOL	bactivate = TRUE;
	bactivate = ( 0 != GetArgumentInt( "Activate" ) );

	CString str_path( _T("") );
	str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
	if( str_path.IsEmpty() )
		return false;

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;

	if( str_name.IsEmpty() )
	{
        CInputMtdNameDlg dlg;
		dlg.m_str_caption.LoadString( IDS_CREATE_NEW_MTD_STR );
		dlg.m_bcheck_unique = TRUE;
		dlg.m_ballow_overwrite = TRUE;

		HWND	hwnd_main = 0;
		IApplicationPtr	ptrA(GetAppUnknown());
		ptrA->GetMainWindowHandle(&hwnd_main);

		if( IDCANCEL == dlg.do_modal( hwnd_main ) )
			return false;
 
		str_name = dlg.m_str_method_name;

		if(dlg.m_balready_exists)
		{ // методика уже существует - грохнуть ее перед созданием новой
			// [vanek] SBT:1194 - 26.10.2004
			::ExecuteScript( _bstr_t("MtdDeleteMethod \"") + _bstr_t(dlg.m_str_method_name) + "\", 0" );
		}
	}

	//create new method
	IUnknown *punk_method = 0;
	CLSID clsid;
	if( ::CLSIDFromProgID( _bstr_t( _T("Metodics.Method") ), &clsid ) != S_OK)
	{
		ASSERT( false );
		return false;
	}

	HRESULT hr = CoCreateInstance(	clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk_method );	
	if(hr != S_OK)
	{
		ASSERT( false );
		return false;
	}

	IMethodPtr sptr_new_mtd = punk_method;
	if( sptr_new_mtd == 0 )
		return false;

	IMethodDataPtr sptr_mtd_data(sptr_new_mtd);
	if(sptr_mtd_data==0) return false;

	if( punk_method )
		punk_method->Release(); punk_method = 0;

	// создали метод с состо€нием по умолчанию.
	// теперь попробуем вместо этой хрени прочитать шаблон метода
	{
		CString	strPathDef = ::MakeAppPathName( "Methods" )+"\\";
		CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", 
			(const char*)strPathDef );
		CString strFile = ::GetValueString( GetAppUnknown(), "\\Methodics", "MethodTemplate",
			strPath + "vt5.method_t" );

		sptr_new_mtd->LoadFile( _bstr_t(strFile) );
	}

	_bstr_t bstrt_pathname;
	bstrt_pathname = str_path + str_name + _T(".method");

	// set name
	if( S_OK != sptr_new_mtd->SetName( _bstr_t( str_name ) ) )
		return false;

	// store active method to new location
	if( S_OK != sptr_new_mtd->StoreFile( bstrt_pathname ) )
		return false;
	
	// add new method
	TPOS lpos_new_mtd = 0;
	if( S_OK != sptr_mtd_man->AddMethod( sptr_new_mtd, 0, &lpos_new_mtd) )
		return false;

	// activate if need
	if( bactivate )
		sptr_mtd_man->SetActiveMethodPos( lpos_new_mtd );

	// notify script
	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	if( sptrScriptSite != 0 )
	{
		_variant_t vartArg( str_name );
		sptrScriptSite->Invoke( _bstr_t( "Metodics_OnNewMethodCreated" ), &vartArg, 1, 0, fwFormScript | fwAppScript );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdSaveMethod 
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionMtdSaveMethod , CCmdTargetEx);

// {B616115F-A475-4aed-B73B-CE18A1B748C4}
GUARD_IMPLEMENT_OLECREATE( CActionMtdSaveMethod, "Metodics.SaveMethod",
0xb616115f, 0xa475, 0x4aed, 0xb7, 0x3b, 0xce, 0x18, 0xa1, 0xb7, 0x48, 0xc4);

// {96282082-1FFE-45d0-934B-C8C210CE4CB8}
static const GUID guidActionMtdSaveMethod = 
{ 0x96282082, 0x1ffe, 0x45d0, { 0x93, 0x4b, 0xc8, 0xc2, 0x10, 0xce, 0x4c, 0xb8 } };


ACTION_INFO_FULL( CActionMtdSaveMethod , IDS_ACTION_MTD_SAVE_METHOD, -1, -1, guidActionMtdSaveMethod );
ACTION_TARGET( CActionMtdSaveMethod , szTargetApplication );

ACTION_ARG_LIST( CActionMtdSaveMethod )
	ARG_STRING( "MethodName", "" )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdSaveMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdSaveMethod ::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;

	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );
	
	// search method by name
	IUnknown *punk_mtd = 0;
    if( !get_method_by_name( _bstr_t(str_name), 0, &punk_mtd, 0) )
		return false;

	// [vanek] : на доступность акции также вли€ет состо€ние флага модификации акции - 28.10.2004
	IMethodDataPtr sptr_mtd_data = punk_mtd;
	if( punk_mtd )
		punk_mtd->Release(); punk_mtd = 0;

	if(sptr_mtd_data == 0)
		return false;

	// 20.06.2006 build 107
	// BT 5282 - разрешаем сохран€ть r/o.
	//INamedPropBagPtr sptr_propbag_mtd = sptr_mtd_data;
	//if( sptr_propbag_mtd != 0 )
	//{
	//	variant_t var;
	//	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), var.GetAddress() );
	//	if( var.vt == VT_I4 && var.lVal==0 )
	//		return false; // дл€ r/o методик - запретить Save
	//}


    //BOOL bis_modified = FALSE;
	//sptr_mtd_data->GetModifiedFlag( &bis_modified );    	
	//return bis_modified != FALSE;
	return true; // 26.07.2005 build 88: последн€€ концепци€ методик - save доступен всегда, чтобы можно было обновить shell.data методики.
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdSaveMethod::Invoke()
{
	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );

	CString str_path( _T("") );
	str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
	if( str_path.IsEmpty() )
		return false;
    
	IMethodPtr sptr_work_mtd;
	IUnknown *punk_mtd = 0;
	if( !get_method_by_name( _bstr_t(str_name), 0, &punk_mtd ) )
		return false;

	sptr_work_mtd = punk_mtd;
	if( punk_mtd )
		punk_mtd->Release(); punk_mtd = 0;

	if( sptr_work_mtd == 0 )
		return false;

	IMethodDataPtr sptr_mtd_data(sptr_work_mtd);
	if(sptr_mtd_data==0) return false;

	_bstr_t bstrt_name;
	if( S_OK != sptr_work_mtd->GetName( bstrt_name.GetAddress() ) )
		return false;
    
	str_path += (TCHAR*)bstrt_name;
	str_path += _T(".method");

	// обновим состо€ние сплиттера в активном шаге, прежде чем переключатьс€
	//sptr_mtd_data->UpdateActiveStep(); // 06.10.2005 build 91 - убрано, теперь апдейтим только при завершении записи


    return S_OK == sptr_work_mtd->StoreFile( _bstr_t(str_path) );
}


//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdCopyMethod 
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdCopyMethod, CCmdTargetEx);

// {0D3A1B52-7D6C-4a74-964B-43019EC8C23E}
GUARD_IMPLEMENT_OLECREATE( CActionMtdCopyMethod, "Metodics.CopyMethod",
0xd3a1b52, 0x7d6c, 0x4a74, 0x96, 0x4b, 0x43, 0x1, 0x9e, 0xc8, 0xc2, 0x3e);

// {45F9DAE0-DDB9-4b88-B62A-F8B04D59EF2F}
static const GUID guidActionMtdCopyMethod = 
{ 0x45f9dae0, 0xddb9, 0x4b88, { 0xb6, 0x2a, 0xf8, 0xb0, 0x4d, 0x59, 0xef, 0x2f } };

ACTION_INFO_FULL( CActionMtdCopyMethod, IDS_ACTION_MTD_COPY_METHOD, -1, -1, guidActionMtdCopyMethod );
ACTION_TARGET( CActionMtdCopyMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdCopyMethod )
	ARG_STRING( "MethodName", "" )
	ARG_INT( "Activate", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdCopyMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdCopyMethod ::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;

	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;			
	
	if( !lpos_active_mtd )
		return false;

	IUnknown *punk_mtd = 0;
	TPOS lpos = lpos_active_mtd;
	if( S_OK != sptr_mtd_man->GetNextMethod( &lpos, &punk_mtd ) )
		return false;

	INamedPropBagPtr sptr_propbag_mtd = punk_mtd;
	if( punk_mtd )
		punk_mtd->Release(); punk_mtd = 0;

	if( sptr_propbag_mtd == 0 )
		return false;

	variant_t vcan_copy;
	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanCopy)), vcan_copy.GetAddress() );
	if( vcan_copy.vt != VT_I4 )
		return true;

	return (vcan_copy.lVal != 0);	
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdCopyMethod::Invoke()
{
	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );

	BOOL	bactivate = TRUE;
	bactivate = ( 0 != GetArgumentInt( "Activate" ) );

	CString str_path( _T("") );
	str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
	if( str_path.IsEmpty() )
		return false;

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;

	// get active method
	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;			

	if( !lpos_active_mtd )
		return false;

	IUnknown *punk_active_mtd = 0;
	IMethodPtr sptr_active_mtd;
	if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
		return false;

	sptr_active_mtd = punk_active_mtd;
	if( punk_active_mtd )
		punk_active_mtd->Release(); punk_active_mtd = 0;

	if( sptr_active_mtd == 0 )
		return false;

	INamedPropBagPtr sptr_propbag_mtd = sptr_active_mtd;
	if( sptr_propbag_mtd == 0 )
		return false;

	// проверка свойста CanCopy находитс€ в CActionMtdCopyMethod ::IsAvaible

	// check name of method's copy
	if( str_name.IsEmpty() )
	{
        CInputMtdNameDlg dlg;
		dlg.m_str_caption.LoadString( IDS_COPY_ACTIVE_MTD_STR );
		dlg.m_bcheck_unique = TRUE;

		HWND	hwnd_main = 0;
		IApplicationPtr	ptrA(GetAppUnknown());
		ptrA->GetMainWindowHandle(&hwnd_main);

		if( IDCANCEL == dlg.do_modal( hwnd_main ) )
			return false;
 
		str_name = dlg.m_str_method_name;
	}

	//create new method
	IUnknown *punk_method = 0;
	CLSID clsid;
	if( ::CLSIDFromProgID( _bstr_t( _T("Metodics.Method") ), &clsid ) != S_OK)
	{
		ASSERT( false );
		return false;
	}

	HRESULT hr = CoCreateInstance(	clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk_method );	
	if(hr != S_OK)
	{
		ASSERT( false );
		return false;
	}

	IMethodPtr sptr_new_mtd = punk_method;
	if( sptr_new_mtd == 0 )
		return false;

	IMethodDataPtr sptr_mtd_data(sptr_new_mtd);
	if(sptr_mtd_data==0) return false;

	if( punk_method )
		punk_method->Release(); punk_method = 0;

	_bstr_t bstrt_pathname;
	bstrt_pathname = str_path + str_name + _T(".method");

	// store active method to new location
	{
		IMethodDataPtr sptr_active_mtd_data(sptr_active_mtd);

		BOOL bModified=FALSE;
		if(sptr_active_mtd_data!=0)
			sptr_active_mtd_data->GetModifiedFlag(&bModified);

		// —охраним свойства CanEdit и CanDelete - в новой их надо выставить в 1
		variant_t vcan_edit;
		sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), vcan_edit.GetAddress() );
		variant_t vcan_delete;
		sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanDelete)), vcan_delete.GetAddress() );
		_variant_t vSortPrefix;
		sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropSortPrefix)), vSortPrefix.GetAddress() );
		
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropCanEdit)), _variant_t(1L) );
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropCanDelete)), _variant_t(1L) );
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropSortPrefix)), _variant_t() );
		
		HRESULT hRes = sptr_active_mtd->StoreFile( bstrt_pathname );
		// ¬осстановим CanEdit и CanDelete
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropCanEdit)), vcan_edit );
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropCanDelete)), vcan_delete );
		sptr_propbag_mtd->SetProperty( _bstr_t(_T(szMtdPropSortPrefix)), vSortPrefix );
		if(sptr_active_mtd_data!=0)
			sptr_active_mtd_data->SetModifiedFlag(bModified);

		if( S_OK != hRes )	return false;
	}
	
	if( S_OK != sptr_new_mtd->SetName( _bstr_t( str_name ) ) )
		return false;

	// load new method from new location
	if( S_OK != sptr_new_mtd->LoadFile( bstrt_pathname ) )
		return false;

	// add new method
	TPOS lpos_new_mtd = 0;
    if( S_OK != sptr_mtd_man->AddMethod( sptr_new_mtd, 0, &lpos_new_mtd ) )
		return false;

	// activate if need
	if( bactivate )
		sptr_mtd_man->SetActiveMethodPos( lpos_new_mtd );       

	// notify script
	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	if( sptrScriptSite != 0 )
	{
		_variant_t vartArg( str_name );
		sptrScriptSite->Invoke( _bstr_t( "Metodics_OnMethodCopied" ), &vartArg, 1, 0, fwFormScript | fwAppScript );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdDeleteMethod 
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdDeleteMethod, CCmdTargetEx);

// {4F2578DF-CB8A-4191-A035-F15CC8A081BA}
GUARD_IMPLEMENT_OLECREATE( CActionMtdDeleteMethod, "Metodics.DeleteMethod",
0x4f2578df, 0xcb8a, 0x4191, 0xa0, 0x35, 0xf1, 0x5c, 0xc8, 0xa0, 0x81, 0xba);

// {790F0E30-CB11-4b37-91B6-4A264E738B6E}
static const GUID guidActionMtdDeleteMethod = 
{ 0x790f0e30, 0xcb11, 0x4b37, { 0x91, 0xb6, 0x4a, 0x26, 0x4e, 0x73, 0x8b, 0x6e } };

ACTION_INFO_FULL( CActionMtdDeleteMethod, IDS_ACTION_MTD_DELETE_METHOD, -1, -1, guidActionMtdDeleteMethod );
ACTION_TARGET( CActionMtdDeleteMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdDeleteMethod )
	ARG_STRING( "MethodName", "" )
	ARG_INT( "ShowWarning", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdDeleteMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdDeleteMethod ::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;
	
	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );
	
	// search method by name
	IUnknown *punk_mtd = 0;
    if( !get_method_by_name( _bstr_t(str_name), sptr_mtd_man, &punk_mtd, 0) )
		return false;
	
	INamedPropBagPtr sptr_propbag_mtd = punk_mtd;
	if( punk_mtd )
		punk_mtd->Release(); punk_mtd = 0;

	if( sptr_propbag_mtd == 0 )
		return false;

	variant_t vcan_delete;
	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanDelete)), vcan_delete.GetAddress() );
	if( vcan_delete.vt != VT_I4 )
    return true;	

	return (vcan_delete.lVal != 0);	
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdDeleteMethod::Invoke()
{
	CString str_name( _T("") );
	str_name = GetArgumentString( "MethodName" );
    
	CString str_path( _T("") );
	str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
	if( str_path.IsEmpty() )
		return false;

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;
	if( sptr_mtd_man == 0 )
		return false;

	// search method by name
	IUnknown *punk_mtd = 0;
	TPOS lpos_mtd_del = 0;
    if( !get_method_by_name( _bstr_t(str_name), sptr_mtd_man, &punk_mtd, &lpos_mtd_del) )
			return false;		

	IMethodPtr sptr_mtd = punk_mtd;
		if( punk_mtd )
			punk_mtd->Release(); punk_mtd = 0; 

	if( sptr_mtd == 0 || !lpos_mtd_del )
		return false;

	if( str_name.IsEmpty() )
	{	// [vanek] SBT:994 get method's name - 19.05.2004
		_bstr_t bstrt_name;
		if( S_OK != sptr_mtd->GetName( bstrt_name.GetAddress() ) )
			return false;
		
		str_name = (TCHAR*)(bstrt_name);
	}

	// проверка свойста CanDelete находитс€ в CActionMtdDeleteMethod ::IsAvaible
	
	if( 0 != GetArgumentInt( "ShowWarning" ) )
	{
		CString str_caption( _T("") );
		str_caption.LoadString( IDS_ATTENTION );

		CString str_message( _T("") );
		str_message.Format( IDS_METHOD_DELETE_STR, str_name );
		HWND	hwnd_main = 0;
		IApplicationPtr	ptrA(GetAppUnknown());
		ptrA->GetMainWindowHandle(&hwnd_main);
		if( IDNO == ::MessageBox( hwnd_main, str_message, str_caption, MB_YESNO | MB_ICONQUESTION ) )
			return false;
	}

	// notify script
	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	if( sptrScriptSite != 0 )
	{
		_variant_t vartArg( str_name );
		sptrScriptSite->Invoke( _bstr_t( "Metodics_OnMethodDelete" ), &vartArg, 1, 0, fwFormScript | fwAppScript );
	}

	IMethodDataPtr sptrMtdData = sptr_mtd;
	if(sptrMtdData!=0) 
		sptrMtdData->SetModifiedFlag( FALSE ); // сбросим флаг модифицированности - все равно убивать...

	//delete method
	return ( S_OK == sptr_mtd_man->DeleteMethod( lpos_mtd_del ) );
}

//////////////////////////////////////////////////////////////////////
/*
IMPLEMENT_DYNCREATE( CActionMtdDeleteStep, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionMtdDeleteStep, "Metodics.DeleteStep",
0xd0c105af, 0x4976, 0x4a09, 0xba, 0x17, 0xfe, 0x40, 0x24, 0x64, 0xce, 0x17);

static const GUID guidActionMtdDeleteStep = 
{ 0x9830377a, 0x5c8b, 0x4c2f, { 0xa3, 0x47, 0xc8, 0x19, 0x28, 0xe0, 0x90, 0xb1 } };

ACTION_INFO_FULL( CActionMtdDeleteStep, IDS_ACTION_MTD_DELETE_STEP, -1, -1, guidActionMtdDeleteStep );
ACTION_TARGET( CActionMtdDeleteStep, szTargetApplication );

ACTION_ARG_LIST( CActionMtdDeleteStep )
END_ACTION_ARG_LIST()

bool CActionMtdDeleteStep::IsAvaible()
{
    return true;	
} 

bool CActionMtdDeleteStep::Invoke()
{
	return true;
}
*/

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdUndo, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionMtdUndo, "Metodics.MethodUndo",
0xa513ec96, 0xa570, 0x4a0b, 0x80, 0x4d, 0x8f, 0x50, 0x2d, 0xcc, 0xb2, 0x19);

static const GUID guidActionMtdUndo = 
{ 0x222988ff, 0xfca2, 0x454a, { 0x88, 0x9a, 0x4c, 0xbb, 0xc, 0xf9, 0x68, 0xd5 } };

ACTION_INFO_FULL( CActionMtdUndo, IDS_ACTION_MTD_UNDO, -1, -1, guidActionMtdUndo );
ACTION_TARGET( CActionMtdUndo, szTargetApplication );

ACTION_ARG_LIST( CActionMtdUndo )
END_ACTION_ARG_LIST()

bool CActionMtdUndo::IsAvaible()
{
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0)
		return false;

	TPOS lMethodPos = 0;
	sptrM->GetActiveMethodPos(&lMethodPos);
	if(!lMethodPos) return false;

	IUnknownPtr ptrMethod;
	sptrM->GetNextMethod(&lMethodPos,&ptrMethod);
	IMethodChangesPtr sptrMethodChanges(ptrMethod);
	if(sptrMethodChanges==0) return false;

	long nCount=0;
	if( FAILED(sptrMethodChanges->GetUndoAvailable(&nCount)) ) return false;

	return nCount>0;
} 

bool CActionMtdUndo::Invoke()
{
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0) return false;

	TPOS lMethodPos = 0;
	sptrM->GetActiveMethodPos(&lMethodPos);
	if(!lMethodPos) return false;

	IUnknownPtr ptrMethod;
	sptrM->GetNextMethod(&lMethodPos,&ptrMethod);
	IMethodChangesPtr sptrMethodChanges(ptrMethod);
	if(sptrMethodChanges==0) return false;

	if( FAILED(sptrMethodChanges->Undo()) ) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdRedo, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionMtdRedo, "Metodics.MethodRedo",
0xe17b09dc, 0x8b77, 0x491d, 0xb7, 0x47, 0x5d, 0xd0, 0xd7, 0xe5, 0xed, 0x93);

static const GUID guidActionMtdRedo = 
{ 0xd990c374, 0xc7b6, 0x460c, { 0x99, 0xd9, 0x98, 0xe0, 0xae, 0xcc, 0xed, 0x7f } };

ACTION_INFO_FULL( CActionMtdRedo, IDS_ACTION_MTD_REDO, -1, -1, guidActionMtdRedo );
ACTION_TARGET( CActionMtdRedo, szTargetApplication );

ACTION_ARG_LIST( CActionMtdRedo )
END_ACTION_ARG_LIST()

bool CActionMtdRedo::IsAvaible()
{
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0) return false;

	TPOS lMethodPos = 0;
	sptrM->GetActiveMethodPos(&lMethodPos);
	if(!lMethodPos) return false;

	IUnknownPtr ptrMethod;
	sptrM->GetNextMethod(&lMethodPos,&ptrMethod);
	IMethodChangesPtr sptrMethodChanges(ptrMethod);
	if(sptrMethodChanges==0) return false;

	long nCount=0;
	if( FAILED(sptrMethodChanges->GetRedoAvailable(&nCount)) ) return false;

	return nCount>0;
} 

bool CActionMtdRedo::Invoke()
{
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0) return false;

	TPOS lMethodPos = 0;
	sptrM->GetActiveMethodPos(&lMethodPos);
	if(!lMethodPos) return false;

	IUnknownPtr ptrMethod;
	sptrM->GetNextMethod(&lMethodPos,&ptrMethod);
	IMethodChangesPtr sptrMethodChanges(ptrMethod);
	if(sptrMethodChanges==0) return false;

	if( FAILED(sptrMethodChanges->Redo()) ) return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdExportScript, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionMtdExportScript, "Metodics.MethodExportScript",
0xa0b1cbab, 0xbc85, 0x47d5, 0x9b, 0xa, 0x64, 0x4b, 0xec, 0x79, 0x98, 0xe3);

static const GUID guidActionMtdExportScript = 
{ 0xa845a1f9, 0x270a, 0x46ca, { 0xb9, 0xa6, 0xd3, 0x5a, 0x7a, 0x90, 0xed, 0xf } };

ACTION_INFO_FULL( CActionMtdExportScript, IDS_ACTION_MTD_EXPORTSCRIPT, -1, -1, guidActionMtdExportScript );
ACTION_TARGET( CActionMtdExportScript, szTargetApplication );

ACTION_ARG_LIST( CActionMtdExportScript )
	ARG_STRING( "FileName", "" )
END_ACTION_ARG_LIST()

bool CActionMtdExportScript::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;			
	
	return (lpos_active_mtd != 0);
} 

bool CActionMtdExportScript::Invoke()
{
	CString str_name( _T("") );
	str_name = GetArgumentString( "FileName" );

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	if( str_name.IsEmpty() ) return false;

	// get active method
	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;			

	if( !lpos_active_mtd )
		return false;

	IUnknown *punk_active_mtd = 0;
	IMethodPtr sptr_active_mtd;
	if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
		return false;

	sptr_active_mtd = punk_active_mtd;
	if( punk_active_mtd )
		punk_active_mtd->Release(); punk_active_mtd = 0;

	if( sptr_active_mtd == 0 )
		return false;

	BSTR bstr;
	if ( S_OK != sptr_active_mtd->GetFullScript(&bstr) )
		return false;
	CString strScript(bstr);
	SysFreeString(bstr);

    FILE *f = fopen(str_name, "w");
	if(0==f) return false;

	fwrite(strScript.GetBuffer(), 1, strScript.GetLength(), f);

	fclose(f);

	return true;
}

bool CActionMtdExportScript::ExecuteSettings( CWnd *pwndParent )
{
	CString strName = GetArgumentString( "FileName" );

	if( strName.IsEmpty() )
	{
		//CString	strLast = ::GetValueString( ::GetAppUnknown(), "\\General", "CurMtdTabContent", "" );
		
		CString strFilter;
		strFilter.LoadString( IDS_FILTER_SCRIPT_STR );

		//if( strLast.IsEmpty() )
		//	strLast = ::GetValueString( ::GetAppUnknown(), "\\Path", "States", "" ) + "*.smtd";

		CFileDialog	dlg( FALSE, ".script", 0,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, 0, _FILE_OPEN_SIZE_ );

		if( dlg.DoModal() == IDOK )
			SetArgument( "FileName", _variant_t( _bstr_t( dlg.GetPathName() ) ) );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdInsertMsgStep
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdInsertMsgStep, CCmdTargetEx);

// {BAF1EAB9-1617-4aa7-A6BD-EE61A0ABB4D2}
GUARD_IMPLEMENT_OLECREATE( CActionMtdInsertMsgStep, "Metodics.InsertMsgStep", 
0xbaf1eab9, 0x1617, 0x4aa7, 0xa6, 0xbd, 0xee, 0x61, 0xa0, 0xab, 0xb4, 0xd2);


// {20DEA717-466C-496d-B1BC-98D018BF9998}
static const GUID guidActionInsertMsgStep = 
{ 0x20dea717, 0x466c, 0x496d, { 0xb1, 0xbc, 0x98, 0xd0, 0x18, 0xbf, 0x99, 0x98 } };

ACTION_INFO_FULL( CActionMtdInsertMsgStep, IDS_ACTION_MTD_INSERT_MSG_STEP, -1, -1, guidActionInsertMsgStep );
ACTION_TARGET( CActionMtdInsertMsgStep, szTargetApplication );

ACTION_ARG_LIST( CActionMtdInsertMsgStep )
	ARG_STRING( "Message", "" )
    ARG_INT( "ShowResult", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdInsertMsgStep implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdInsertMsgStep ::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	BOOL brunning = FALSE;
	sptr_mtd_man->IsRunning( &brunning );
	if( brunning )
		return false;

	CMethodManState st(sptr_mtd_man);

	if(st.m_bError) return false;
	if(st.m_sptrMethod==0) return false; // Ќет методики - нах
	if(st.m_pStep==0) return false; // Ќет шага - нах

	INamedPropBagPtr sptr_propbag_mtd = st.m_sptrMethod;
	if( sptr_propbag_mtd == 0 )
		return false;

	variant_t vcan_edit;
	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), vcan_edit.GetAddress() );
	if( vcan_edit.vt != VT_I4 )
		return true;

	return (vcan_edit.lVal != 0);	
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdInsertMsgStep::Invoke()
{
	CString str_msg( _T("") );
	str_msg = GetArgumentString( "Message" );

	BOOL bshow_result = FALSE;
	bshow_result = GetArgumentInt( "ShowResult" ) != 0;

	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	BOOL brunning = FALSE;
	sptr_mtd_man->IsRunning( &brunning );
	if( brunning )
		return false;

	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;	

	if( !lpos_active_mtd )
		return false;

	IUnknown *punk_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
		return false;

	IMethodDataPtr sptr_active_mtd_data = punk_active_mtd;
	if( punk_active_mtd )
		punk_active_mtd->Release(); punk_active_mtd = 0;        

	if( sptr_active_mtd_data == 0 )
		return false;

	// [vanek] SBT:1192 - 28.10.2004
	IMethodChangesPtr sptr_mtd_ch =	sptr_active_mtd_data ;
	if( sptr_mtd_ch == 0 )
		return FALSE;

	if( str_msg.IsEmpty() )
	{
        CInputMsgDlg dlg;
		dlg.m_str_caption.LoadString( IDS_INPUT_MESSAGE_STR );

		HWND	hwnd_main = 0;
		IApplicationPtr	ptrA(GetAppUnknown());
		ptrA->GetMainWindowHandle(&hwnd_main);

		if( IDCANCEL == dlg.do_modal( hwnd_main ) )
			return false;
 
		str_msg = dlg.m_str_msg;
		str_msg.Remove(10); // убрать все LF - оставить только CR

		// 31.01.2006 build 98

		// заменим все CR на "+CHR(13)" - дл€ vbScript.
		int j = str_msg.Find(13); // найдем CR
		while(j>=0)
		{

			str_msg.Delete(j,1);
			str_msg.Insert(j,"\"+CHR(13)+\"");
			j = str_msg.Find(13,j);
		}
	}

	// forming script;
	CString str_name( _T("Message") );

	// [vanek] : заголовок грузим из технологовских ресурсов (строка "Message" ) - 28.10.2004
	CString str_script( _T("") );
	str_script.Format( "\nMsgStepTitleStr = Docs.LoadString(\"%s\")\nMsgBox \"%s\", vbInformation, MsgStepTitleStr", str_name, str_msg );
	
	HRESULT hr = S_FALSE;
	
    BOOL brec = FALSE;
	sptr_mtd_man->IsRecording( &brec );
	if( brec )
	{	// [vanek] : идет запись - имитируем запуск акции с MetodicsTab, т.к. это дает возможность MethodMan(monster)
		// корректно обработать добавление нового шага (создать кеш и т.д.)
        ::FireEvent( ::GetAppUnknown(), szEventBeforeRecordMethodStep, 0, 0, (LPVOID)((LPCTSTR)str_script), str_script.GetLength() );
		_bstr_t bstrRename = _bstr_t("MethodMan.RenameCurrentStep \"") + str_name + "\"";
		::ExecuteScript( bstrRename );
		CMethodManState st(sptr_mtd_man);
		if( st.m_pStep )
		{
			st.m_pStep->m_bstrActionName = str_script;
			st.m_pStep->m_bstrForcedName = str_name;
			st.m_pStep->m_dwFlags |= msfStateless;
		}
		::FireEvent( ::GetAppUnknown(), szEventAfterRecordMethodStep, 0, 0,  (LPVOID)((LPCTSTR)str_name), str_name.GetLength() );
	}
	else
	{
		// create new method's step
		CMethodStep step_data;
		step_data.m_bstrActionName = str_script;
		step_data.m_bstrForcedName = str_name;
		step_data.m_dwFlags |= msfStateless;

		// add new step after active
		TPOS lpos_step = 0;
		sptr_active_mtd_data->GetActiveStepPos( &lpos_step );    
		
		// [vanek] : поддержка undo/redo методики - 28.10.2004
		long lindex = -1;
		if( S_OK != sptr_active_mtd_data->GetStepIndexByPos( lpos_step, &lindex ) )
			return false;
		lindex++; // следом за активным

		hr = sptr_mtd_ch->AddStep( lindex, &step_data );
	}

	if( bshow_result )
		::ExecuteScript( _bstr_t( str_script ) );

	return hr == S_OK;
}


//////////////////////////////////////////////////////////////////////
//
//	Class CActionMtdClearMethod 
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionMtdClearMethod, CCmdTargetEx);

// {830F5BDE-3076-4794-B5C6-41D4C1233D52}
GUARD_IMPLEMENT_OLECREATE( CActionMtdClearMethod, "Metodics.ClearMethod",
0x830f5bde, 0x3076, 0x4794, 0xb5, 0xc6, 0x41, 0xd4, 0xc1, 0x23, 0x3d, 0x52);

// {E36FA8E4-5481-4969-95A2-4F006C796E5E}
static const GUID guidActionMtdClearMethod = 
{ 0xe36fa8e4, 0x5481, 0x4969, { 0x95, 0xa2, 0x4f, 0x0, 0x6c, 0x79, 0x6e, 0x5e } };

ACTION_INFO_FULL( CActionMtdClearMethod, IDS_ACTION_MTD_CLEAR_METHOD, -1, -1, guidActionMtdClearMethod );
ACTION_TARGET( CActionMtdClearMethod, szTargetApplication );

ACTION_ARG_LIST( CActionMtdClearMethod )
	ARG_STRING( "MethodName", "" )
	ARG_INT( "ShowWarning", 1 )
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionMtdClearMethod implementation

//////////////////////////////////////////////////////////////////////
bool CActionMtdClearMethod::IsAvaible()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

	CString str_mtd_name( _T("") );
	str_mtd_name = GetArgumentString( "MethodName" );
	if( !str_mtd_name.IsEmpty() )
		return true;
	    
	TPOS lpos_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
		return false;	

	if( !lpos_active_mtd )
		return false;

	// [vanek] SBT:1193 - 26.10.2004
	IUnknown *punk_active_mtd = 0;
	if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
		return false;

	INamedPropBagPtr sptr_propbag_mtd = punk_active_mtd;
	if( punk_active_mtd )
		punk_active_mtd->Release(); punk_active_mtd = 0;

	if( sptr_propbag_mtd == 0 )
		return false;

	variant_t vcan_edit;
	sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropCanEdit)), vcan_edit.GetAddress() );
	if( vcan_edit.vt == VT_I4 && !vcan_edit.lVal )
		return false;

	IMethodDataPtr sptr_mtd_data = sptr_propbag_mtd;
	if( sptr_mtd_data == 0 )
		return false;

	long	lstep_count = 0;
	TPOS lstep_pos = 0;
    sptr_mtd_data->GetFirstStepPos( &lstep_pos );
	while( lstep_pos )
	{
		CMethodStep step_data;
		step_data.m_bSkipData = TRUE;
		TPOS lcurr_step_pos = lstep_pos;
		sptr_mtd_data->GetNextStep( &lstep_pos, &step_data );
		if( !(step_data.m_dwFlags & msfUndead) )
			lstep_count ++;
	}

    return (lstep_count > 0);	
} 

//////////////////////////////////////////////////////////////////////
bool CActionMtdClearMethod::Invoke()
{
	IUnknownPtr ptr( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	IMethodManPtr sptr_mtd_man = ptr;

	if( sptr_mtd_man == 0 )
		return false;

	BOOL bworking = FALSE;
	sptr_mtd_man->IsRunning( &bworking );
	if( bworking )
		return false;

	sptr_mtd_man->IsRecording( &bworking );
	if( bworking )
		return false;

	CString str_mtd_name( _T("") );
	str_mtd_name = GetArgumentString( "MethodName" );
	
	IMethodDataPtr sptr_mtd_data;
	if( str_mtd_name .IsEmpty() )
	{	// get active method
		TPOS lpos_active_mtd = 0;
		if( S_OK != sptr_mtd_man->GetActiveMethodPos( &lpos_active_mtd ) )
			return false;			

		IUnknown *punk_active_mtd = 0;
		if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_active_mtd, &punk_active_mtd ) )
			return false;

		sptr_mtd_data = punk_active_mtd;
		if( punk_active_mtd )
			punk_active_mtd->Release(); punk_active_mtd = 0;        

		IMethodPtr sptr_mtd = sptr_mtd_data;
		_bstr_t bstrt_name;
		if( sptr_mtd != 0 )
		{
			sptr_mtd->GetName( bstrt_name.GetAddress() );
            str_mtd_name = (TCHAR *)(bstrt_name);
		}
	}
	else
	{	// search method by name
		IUnknown *punk_mtd = 0;
		if( !get_method_by_name( _bstr_t(str_mtd_name ), sptr_mtd_man, &punk_mtd ) )
			return false;

		sptr_mtd_data = punk_mtd;
		if( punk_mtd )
			punk_mtd->Release(); punk_mtd = 0;
	}

	if( sptr_mtd_data == 0 )
		return false;

	IMethodChangesPtr sptr_mtd_changes = sptr_mtd_data;
	if( sptr_mtd_changes == 0 )
		return false;

	if( 0 != GetArgumentInt( "ShowWarning" ) )
	{
		CString str_caption( _T("") );
		str_caption.LoadString( IDS_ATTENTION );

		CString str_message( _T("") );
		str_message.Format( IDS_METHOD_CLEAR_STR, str_mtd_name );
		HWND	hwnd_main = 0;
		IApplicationPtr	ptrA(GetAppUnknown());
		ptrA->GetMainWindowHandle(&hwnd_main);
		if( IDNO == ::MessageBox( hwnd_main, str_message, str_caption, MB_YESNO | MB_ICONQUESTION ) )
			return false;
	}

	// начинаем группу undo
	sptr_mtd_changes->BeginGroupUndo( );

	TPOS lstep_pos = 0;
    sptr_mtd_data->GetFirstStepPos( &lstep_pos );
	while( lstep_pos )
	{
		CMethodStep step_data;
		step_data.m_bSkipData = TRUE;
		TPOS lcurr_step_pos = lstep_pos;
		sptr_mtd_data->GetNextStep( &lstep_pos, &step_data );
		if( !(step_data.m_dwFlags & msfUndead) )
		{
			long lindex = -1;
			if( S_OK == sptr_mtd_data->GetStepIndexByPos( lcurr_step_pos, &lindex ) )
				sptr_mtd_changes->DeleteStep( lindex );
		}
	}

	// завершаем группу undo
	sptr_mtd_changes->EndGroupUndo( );
    	    	
	return S_OK;
}
