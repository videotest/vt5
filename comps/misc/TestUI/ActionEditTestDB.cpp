#include "StdAfx.h"
#include "TestUI.h"
#include "actionedittestdb.h"
#include "core5.h"
#include "Utils.h"
#include "EditTestDBDlg.h"
#include "\vt5\comps\general\data\misc_types.h"
#include "\vt5\awin\misc_map.h"
#include "inputpath.h"

#define OBJECT		"Object"
#define APPLICATION	"Application"
#define DOCUMENT	"Document"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL	CheckTestBranch( ITestManager *ptest_man, LPOS lpos_test_check, LPOS lpos_start_parent /*= 0*/ )
{
	if( !ptest_man )
		return FALSE;
    
	LPOS lpos_test = 0;
	ptest_man->GetFirstTestPos( lpos_start_parent, &lpos_test );
	while( lpos_test )
	{
		IUnknownPtr punkTest;
		LPOS lpos_saved = lpos_test;
		ptest_man->GetNextTest( lpos_start_parent, &lpos_test, &punkTest );

		if( lpos_saved == lpos_test_check )
		{
            ITestRunningDataPtr sptrTRD =  punkTest;
			if (sptrTRD != 0)       
				sptrTRD->SetTestChecked( 1 );                        
			return TRUE;
		}
		
		if( CheckTestBranch( ptest_man, lpos_test_check, lpos_saved ) )
		{
			ITestRunningDataPtr sptrTRD =  punkTest;
			if (sptrTRD != 0)       
				sptrTRD->SetTestChecked( 1 );                        
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL	HasSavedChild( ITestManager *ptest_man, LPOS lpos_parent )
{
    if( !ptest_man || !lpos_parent )
		return FALSE;

	LPOS lpos_test = 0;
	ptest_man->GetFirstTestPos( lpos_parent, &lpos_test );
	while( lpos_test )
	{
		IUnknownPtr punkTest;
		LPOS lpos_saved = lpos_test;
		ptest_man->GetNextTest( lpos_parent, &lpos_test, &punkTest );

		ITestRunningDataPtr sptrTRD = punkTest;
		long lsaved_state = 0;
		if (sptrTRD != 0)       
		{
			sptrTRD->GetTestSavedState( &lsaved_state );                        
			if( lsaved_state )
				return TRUE;
		}	
		
		if( HasSavedChild( ptest_man, lpos_saved ) )
			return TRUE;
	}

    return FALSE;
}

BOOL	ExecuteBranchTests( ITestManager *ptest_man, LPOS lpos_test, long lexecution_parts )
{
	if( !ptest_man || !lpos_test )
		return FALSE;

	CHourglass wait;

    // uncheck all
	DoOperationWithChildTests( ptest_man, toUnCheck, 0);
	
	// check test's branch
	CheckTestBranch( ptest_man, lpos_test );


    if( lexecution_parts == 1 )
	{
		// save state
		CString strState( _T("") );
		strState = ::GetValueString( ::GetAppUnknown(), "", "\\General\\CurrentState", "" );
		::ExecuteAction( "ToolsSaveState", CString( "\"" ) + strState + "\""  );
    }

	// run
	ITestProcessPtr sptrTestProcess = ptest_man;
	if( sptrTestProcess != 0 )
	{
		SetValue( GetAppUnknown(), "\\TestMan", "ExecutionParts", lexecution_parts );
		sptrTestProcess->RunTestSimple( );		
	}

	// uncheck all
	DoOperationWithChildTests( ptest_man, toUnCheck, 0);

	if( lexecution_parts == 2 )
	{
		// load state
		CString strState( _T("") );
		strState = ::GetValueString( ::GetAppUnknown(), "", "\\General\\CurrentState", "" );
		::ExecuteAction( "ToolsLoadState", CString( "\"" ) + strState + "\""  );

		// [vanek] SBT:1071 - 07.07.2004
		IApplicationPtr sptrApp(GetAppUnknown());
		if (sptrApp)
			sptrApp->IdleUpdate();
	}
	
	return TRUE;
}

BOOL	DoOperationWithChildTests( ITestManager *ptest_man, DWORD dwoperations, LPOS lpos_parent )
{
    if( !ptest_man )
		return FALSE;;

	_bstr_t bstr_path_parent;
	ITestItemPtr sptr_parent_test; 
	if( dwoperations & toUpdatePath )
	{	// initialize info for parent test
        IUnknownPtr sptr_unk_test;
		LPOS lpos = lpos_parent;
		ptest_man->GetNextTest( 0, &lpos, &sptr_unk_test );
		sptr_parent_test = sptr_unk_test;
		if( sptr_parent_test == 0 )
			return FALSE;

		sptr_parent_test->GetPath( bstr_path_parent.GetAddress() );
	}
    
	LPOS lpos_test = 0;
	ptest_man->GetFirstTestPos( lpos_parent, &lpos_test );
	while( lpos_test )
	{
		IUnknownPtr punkTest;
		LPOS lpos_saved = lpos_test;
		ptest_man->GetNextTest( lpos_parent, &lpos_test, &punkTest );
		ITestItemPtr sptrTI =  punkTest;
		if (sptrTI == 0)
			continue;

		if( dwoperations & 
			( toCheck|  toUnCheck | toResetSavedState | toResetTestRunTime | toResetRunTime | toResetSaveTime | toResetLastError ) )
		{
			ITestRunningDataPtr sptrTRD = sptrTI;
			if (sptrTRD == 0)
				continue;
                
			if( dwoperations & toCheck )
				sptrTRD->SetTestChecked( 1 );
		    if( dwoperations & toUnCheck )
		         sptrTRD->SetTestChecked( 0 );
            if( dwoperations & toResetSavedState )
				sptrTRD->SetTestSavedState( 0 );
			if( dwoperations & toResetTestRunTime )
                sptrTRD->SetTestRunTime( 0 );
			if( dwoperations & toResetRunTime )
                sptrTRD->SetRunTime( 0 );                        
			if( dwoperations & toResetSaveTime )
                sptrTRD->SetSaveTime( 0 );                        
			if( dwoperations & toResetLastError )
                sptrTRD->SetTestErrorState( 0 );                        
		}


        if( dwoperations & toStoreContent )
			sptrTI->StoreContent( );
		if( dwoperations &  toLoadContent )
			sptrTI->LoadContent( );
		if( dwoperations & toUpdatePath )
		{
			_bstr_t bstrPathOld;
			sptrTI->GetPath( bstrPathOld.GetAddress() );
			CString str_path = (char*)bstrPathOld;

			int npos = str_path.ReverseFind( '\\' );
			if( npos )
			{
				CString str_name = str_path.Right( str_path.GetLength() - npos - 1 );
				sptrTI->SetPath( bstr_path_parent + _bstr_t( "\\" ) + _bstr_t( str_name ) );
			}
		}
		if( dwoperations & toParseCond )
			sptrTI->ParseCond( );
		
        DoOperationWithChildTests( ptest_man, dwoperations, lpos_saved );
	}
    
	return TRUE;    
}


_bstr_t GetNewScript(_bstr_t bstrScript, IMacroHelper *pMacroHelper)
{
	_bstr_t bstrNewScript;
	const wchar_t *pwcLine = bstrScript;
	while (pwcLine)
	{
		const wchar_t *pwcNextLine = wcspbrk(pwcLine, L"\n\r");
		INT_PTR nLen = pwcNextLine?pwcNextLine-pwcLine:wcslen(pwcLine);
		std::auto_ptr<wchar_t> CurLine(new wchar_t[nLen+1]);
		wchar_t *pwcLine1 = CurLine.get();
		wcsncpy(pwcLine1, pwcLine, nLen);
		pwcLine1[nLen] = 0;
		bool bAdded = false;
		if (wcsncmp(pwcLine1, L"MacroHelper.SetEventsWindow2", wcslen(L"MacroHelper.SetEventsWindow2")) == 0)
		{
			wchar_t szBuff[256],szBuff1[256];
			int x,y,cx,cy;
			long lHwnd;
			int n = swscanf(pwcLine1, L"MacroHelper.SetEventsWindow2 \"%[^\"]\",%d,%d,%d,%d,%d", szBuff, &x, &y, &cx, &cy, &lHwnd);
			if (n == 6)
			{
				CEditedRectangleInfo ri;
				ri.rcOld.left = ri.rcNew.left = x;
				ri.rcOld.top = ri.rcNew.top = y;
				ri.rcOld.right = ri.rcNew.right = x + cx;
				ri.rcOld.bottom = ri.rcNew.bottom = y + cy;
				ri.hwndSaved = (HWND)lHwnd;
				ri.bstrWndClass = ::SysAllocString(szBuff);
				if (pMacroHelper->FindEditedRectangle(&ri)==S_OK)
				{
					swprintf(szBuff1, L"MacroHelper.SetEventsWindow2 \"%s\",%d,%d,%d,%d,%d",
						szBuff, ri.rcNew.left, ri.rcNew.top, ri.rcNew.right-ri.rcNew.left,
						ri.rcNew.bottom-ri.rcNew.top, lHwnd);
					bstrNewScript += szBuff1;
					bAdded = true;
				}
			}
		}
		if (!bAdded)
			bstrNewScript += pwcLine1;
		if (pwcNextLine)
		{
			bstrNewScript += NEW_LINE;
			INT_PTR n = wcsspn(pwcNextLine, L"\n\r");
			pwcLine = pwcNextLine+n;
		}
		else
			pwcLine = 0;
	}
	return bstrNewScript;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionEditTestDB
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionEditTestDB, CCmdTargetEx);

// {0FFB0E9C-0A79-4860-BB23-F70AC498F0F7}
GUARD_IMPLEMENT_OLECREATE(CActionEditTestDB, _T("TestUI.ActionEditTestDB"), 
0xffb0e9c, 0xa79, 0x4860, 0xbb, 0x23, 0xf7, 0xa, 0xc4, 0x98, 0xf0, 0xf7);

// {F32AC8C9-2A2B-49bb-9037-5E8CC44157EC}
static const GUID guidActionEditTestDB = 
{ 0xf32ac8c9, 0x2a2b, 0x49bb, { 0x90, 0x37, 0x5e, 0x8c, 0xc4, 0x41, 0x57, 0xec } };

ACTION_INFO_FULL(CActionEditTestDB, ID_ACTION_EDITTESTDB, -1, -1, guidActionEditTestDB);
ACTION_TARGET(CActionEditTestDB, szTargetApplication);
ACTION_ARG_LIST(CActionEditTestDB)
END_ACTION_ARG_LIST();


CActionEditTestDB::CActionEditTestDB(void)
{
	m_lworking_test = -1;
}

CActionEditTestDB::~CActionEditTestDB(void)
{
}

bool CActionEditTestDB::Invoke()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IMacroManagerPtr sptrMacroManager;
	IUnknown *punkMacroMan = 0;
	punkMacroMan = _GetOtherComponent( GetAppUnknown(), IID_IMacroManager );
	sptrMacroManager = punkMacroMan;
    if(  punkMacroMan  )
		punkMacroMan->Release(); punkMacroMan = 0;
	
	if( sptrMacroManager == 0 )
		return false;

	BOOL brec = FALSE;
	sptrMacroManager->IsRecording( &brec );
	if( brec )
		return false;

	IUnknownPtr punkMH(_GetOtherComponent(GetAppUnknown(),IID_IMacroHelper),FALSE);
	IMacroHelperPtr sptrMacroHelper = punkMH;
	if (sptrMacroHelper == 0)
		return false;

	ITestManagerPtr sptrTestMan;
	IUnknown *punk_testman = _GetOtherComponent(GetAppUnknown(), IID_ITestManager); 
	sptrTestMan = punk_testman;
	if( punk_testman )
		punk_testman->Release(); punk_testman = 0;

	if( sptrTestMan == 0 )
		return false;

	// [vanek]: enter in edit mode
	ITestProcessPtr sptr_test_process = sptrTestMan;
	if( sptr_test_process == 0 )
		return false;

    sptr_test_process->SetEditMode( true );
	
	// read last selected test
	LPOS lpos_parent = 0, lpos_test = 0;
	lpos_parent = GetValueInt64( GetAppUnknown(), "\\EditTestDB", "LastParentPos", 0);
	lpos_test = GetValueInt64( GetAppUnknown(), "\\EditTestDB", "LastTestPos", 0);
    
	HWND	hwnd_main = 0;
	IApplicationPtr	ptrA(GetAppUnknown());
	ptrA->GetMainWindowHandle(&hwnd_main);
	CEditTestDBDlg dlg(CWnd::FromHandle(hwnd_main));
	// set selected test
	dlg.SetSelectedTestItem( lpos_parent, lpos_test );
	INT_PTR nres = 0;
    while( nres = dlg.DoModal() ) // show window
	{
		if( nres == IDCLOSE || nres == IDOK || nres == IDCANCEL )
			break;	// exit

		dlg.GetSelectedTestItem( &lpos_parent, &lpos_test);
		if( nres == IDC_BUTTON_TEST_SEQUENCE || nres == IDC_BUTTON_FINAL_TEST_SEQUENCE )
			_record_test_sequence( sptrTestMan, nres == IDC_BUTTON_FINAL_TEST_SEQUENCE, sptrMacroManager, hwnd_main, lpos_parent, lpos_test );

		if( nres == IDC_BUTTON_START_CONDITIONS || nres == IDC_BUTTON_EXEC_COND )
		{
			CString str_cond( _T("") );
			_input_condition( sptrTestMan, hwnd_main, lpos_parent, lpos_test, nres == IDC_BUTTON_START_CONDITIONS ? xctStart : xctExec );
		}
		if( nres == IDC_BUTTON_EDIT_TEST_SEQUENCE || nres == IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE )
			_edit_test_sequence( sptrTestMan, nres == IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE,
				sptrMacroManager, sptrMacroHelper, hwnd_main, lpos_parent, lpos_test );
	}

	// [vanek] : run final test for working test - 23.03.2004
	if( m_lworking_test != -1 )
	{
		ExecuteBranchTests( sptrTestMan, m_lworking_test, 2 );
		m_lworking_test = -1;
	}

    // save last selected test
	dlg.GetSelectedTestItem( &lpos_parent, &lpos_test);
	SetValue( GetAppUnknown(), "\\EditTestDB", "LastParentPos", (LONG_PTR)lpos_parent);
	SetValue( GetAppUnknown(), "\\EditTestDB", "LastTestPos", (LONG_PTR)lpos_test);

	// [vanek]: exit in edit mode
    sptr_test_process->SetEditMode( false );
    	
 	return true;
}

class _CManageEnvironment
{
	IUnknownPtr m_sptr_app_data_saved;
public:
	_CManageEnvironment()
	{
		if (GetValueInt(::GetAppUnknown(), "\\EditTestDB", "SaveStateOnRecord", 1) == 1)
		{
			CString	strLastState = ::GetValueString(::GetAppUnknown(), "\\General", "CurrentState", "default.state");
			if( !strLastState.IsEmpty() )
				::ExecuteAction( "ToolsSaveState", CString("\"")+strLastState+CString("\""), aefNoShowInterfaceAnyway);
		}

		//[vanek] BT2000:3751 - 15.06.2004
		if ( 1 == GetValueInt(::GetAppUnknown(), "\\EditTestDB", "SaveAppDataOnRecord", 1) )
		{
			CLSID	clsidData = {0};
        	if( S_OK == ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) )
			{		
				if( S_OK == ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_sptr_app_data_saved ))
					CopyNamedData( m_sptr_app_data_saved, GetAppUnknown() );
			}
		}
	}
	~_CManageEnvironment()
	{
		if (GetValueInt(::GetAppUnknown(), "\\EditTestDB", "SaveStateOnRecord", 1) == 1)
		{
			CString	strLastState = ::GetValueString(::GetAppUnknown(), "\\General", "CurrentState", "default.state");
			if( !strLastState.IsEmpty() )
			{
				::ExecuteAction("ToolsLoadState", CString("\"")+strLastState+CString("\""), aefNoShowInterfaceAnyway);
				IApplicationPtr sptrApp(GetAppUnknown());
				if (sptrApp)
					sptrApp->IdleUpdate();
			}
		}

		//[vanek] BT2000:3751 - 15.06.2004
		if ( 1 == GetValueInt(::GetAppUnknown(), "\\EditTestDB", "SaveAppDataOnRecord", 1) && m_sptr_app_data_saved != 0)
		{
			IApplicationPtr	ptrA( GetAppUnknown() );
            CopyNamedData( ptrA, m_sptr_app_data_saved);
            m_sptr_app_data_saved = 0;

			// update settings
			::FireEvent( ptrA, szEventNewSettings, 0, 0, 0, 0 );
            LPOS	lposTempl = 0;

			ptrA->GetFirstDocTemplPosition( &lposTempl );
        	while( lposTempl )
			{
				LPOS	lposDoc = 0;
				ptrA->GetFirstDocPosition( lposTempl, &lposDoc );

        		while( lposDoc )
				{
					IUnknown	*punk = 0;
					ptrA->GetNextDoc( lposTempl, &lposDoc, &punk );

					::FireEvent( punk, szEventNewSettings, 0, 0, 0, 0 );

					if( punk )
						punk->Release();
				}
				ptrA->GetNextDocTempl( &lposTempl, 0, 0 );
			}
			
			ptrA->ProcessMessage();
		}
	}
};


BOOL	CActionEditTestDB::_record_test_sequence( ITestManager *ptest_man, BOOL bfinal_sequence, IMacroManager *pmacro_man, HWND hwnd_main, LPOS lpos_parent, LPOS lpos_test )
{
	if( !pmacro_man || !hwnd_main || !lpos_test )
		return FALSE;

	IUnknownPtr spunkTest;
	LPOS lpos_saved = lpos_test;
	ptest_man->GetNextTest( lpos_parent, &lpos_test, &spunkTest);
    lpos_test = lpos_saved;	 // restore tets's position
	ITestItemPtr sptrTI =  spunkTest;
	if( sptrTI == 0 )
		return FALSE;

	CString str_title( _T("") ), str_message( _T("") );

	// [vanek] : теперь всегда инициируется механизм запуска тестовых последовательностей (см. ниже) - 11.08.2004
	//_CManageEnvironment ManageEnv;
    //if( bfinal_sequence || lpos_parent )
	{
        str_title.LoadString( IDS_RUN_TEST_SEQUENCE_TITLE );
		
		// [vanek] : всегда перед записью тестовой последовательности запускаем начальные тестовые 
		// последовательности родительских тестов - инициируется механизм сохранения/восстановления окружения 
		// тестов перед/после запуска теста -> не требуется дополнительное сохранение/восстановления shelldata, 
		// state и т.д. - 11.08.2004
        /* str_message.LoadString( IDS_ON_RUN_TESTS_BRANCH );
		int nres = IDNO;
		if( m_lworking_test == lpos_test )
			nres = bfinal_sequence ? IDYES : IDNO;	// если запись финального теста, то нужно перед записью 
													// воспроизвести финальную часть текущего теста -> IDYES
		else
			nres = ::MessageBox( hwnd_main, str_message, str_title, MB_YESNOCANCEL | MB_ICONEXCLAMATION );
		
		if( nres == IDCANCEL ) 
			return TRUE;
		else if( nres == IDYES )*/ 
		{
			// [vanek] : run final test for working test - 23.03.2004
			if( m_lworking_test != -1 && m_lworking_test != lpos_test )
			{
				ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
				m_lworking_test = -1;
			}

			// execute start parent's test sequence
			if( m_lworking_test == -1 )
			{
				m_lworking_test = lpos_test;
				ExecuteBranchTests( ptest_man, m_lworking_test, 1 );			    
			}

			// [vanek] - execute current final test sequence
			if( bfinal_sequence )
			{
                _bstr_t bstrt_final_script;
				sptrTI->GetScriptFinal( bstrt_final_script.GetAddress() );
				pmacro_man->SetMacroScript( bstrt_final_script.GetBSTR( ) );
				pmacro_man->ExecuteScript( );
			}
            
			CString str_message2( _T("") );
			str_message.LoadString( IDS_ON_END_RUN_TESTS_BRANCH );
			str_message2.LoadString( IDS_RECORD_STARTING );
			str_message += _T("\n") + str_message2;
			::MessageBox( hwnd_main, str_message, str_title, MB_OK | MB_ICONEXCLAMATION );       
		}
	}

	// start record macro
	pmacro_man->StartRecord();

  	// waiting stop recording
	BOOL brecording = TRUE;
	do
	{
		IApplicationPtr	sptr_app( GetAppUnknown( ) );		
		if( sptr_app == 0 )
		{
            pmacro_man->StopRecord();				
			break;
		}
        		
		sptr_app->ProcessMessage();
		sptr_app = 0;
		pmacro_man->IsRecording( &brecording );
	}
	while( brecording );
	
	// add new macro to test item
	
	// get test script
	CString str_script( _T("") );
	BSTR bstrScript = 0;
	if( bfinal_sequence )
		sptrTI->GetScriptFinal( &bstrScript );
	else
		sptrTI->GetScript( &bstrScript );

	str_script = bstrScript;
	if( bstrScript )
		::SysFreeString(bstrScript); bstrScript	= 0;

    // get new script
	CString str_new_script( _T("") );
	pmacro_man->GetMacroScript( &bstrScript );
    str_new_script = bstrScript;
	if( bstrScript )
		::SysFreeString(bstrScript); bstrScript	= 0;

	if( bfinal_sequence )
	{	// clear current final test sequence
		_bstr_t bstrt_empty;
		sptrTI->SetScriptFinal( bstrt_empty.GetBSTR() ); 
	}
	

	// [vanek] SBT: 820 - 27.02.2004
	// run final test's sequences (if need) before setting final script
	if( m_lworking_test == lpos_test )
	{
		int nres = IDNO;
		if( bfinal_sequence )
			nres = IDYES;
		else
		{
			str_title.LoadString( IDS_RUN_TEST_SEQUENCE_TITLE );
			str_message.LoadString( IDS_ON_END_RUN_FINAL_TESTS_BRANCH );
	        nres = ::MessageBox( hwnd_main, str_message, str_title, MB_YESNO | MB_ICONEXCLAMATION );
		}

		if( nres == IDYES )
		{
			// execute final test's sequence
			ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
			m_lworking_test = -1;

			str_message.LoadString( IDS_ON_END_RUN_TESTS_BRANCH );   
			::MessageBox( hwnd_main, str_message, str_title, MB_OK | MB_ICONEXCLAMATION );	
		}

	}

    
	// add new script to test's script
	if( str_new_script.IsEmpty() )
		return FALSE;
	
	if( !str_script.IsEmpty() )
		str_script += _T( NEW_LINE );

	str_script += str_new_script;
	BSTR bstr_script = str_script.AllocSysString( );
	if( bfinal_sequence )
		sptrTI->SetScriptFinal( bstr_script );
	else
		sptrTI->SetScript( bstr_script );

	if( bstr_script )
		::SysFreeString( bstr_script ); bstr_script = 0;

	// store
	sptrTI->StoreContent();

	return TRUE;
}

class _CManageManualModeTI
{
public:
	ITestItemPtr m_sptrTI;
	_CManageManualModeTI(ITestItem *p)
	{
		m_sptrTI = p;
		m_sptrTI->SetManualCorrectionMode(TRUE);
	}
	~_CManageManualModeTI()
	{
		m_sptrTI->SetManualCorrectionMode(FALSE);
	};
};

BOOL	CActionEditTestDB::_edit_test_sequence( ITestManager *ptest_man, BOOL bfinal_sequence,
	IMacroManager *pmacro_man, IMacroHelper *pMacroHelper, HWND hwnd_main, LPOS lpos_parent, LPOS lpos_test )
{
	if( !pmacro_man || !hwnd_main || !lpos_test )
		return FALSE;

	IUnknownPtr spunkTest;
	LPOS lpos_saved = lpos_test;
	ptest_man->GetNextTest( lpos_parent, &lpos_test, &spunkTest);
    lpos_test = lpos_saved;	 // restore tets's position
	ITestItemPtr sptrTI =  spunkTest;
	if( sptrTI == 0 )
		return FALSE;


	CString str_title( _T("") ), str_message( _T("") );

	// [vanek] : см. _record_test_sequence - 11.08.2004
	// _CManageEnvironment ManageEnv;
	// if (bfinal_sequence || lpos_parent)
	{
		if( m_lworking_test != -1 && m_lworking_test != lpos_test )
		{
			ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
			m_lworking_test = -1;
		}
		// execute start parent's test sequence
		if (bfinal_sequence)
		{
			// For final sequence execute all branch in common mode and execute final
			// script in manual correction mode.
			if( m_lworking_test == -1 )
			{
				m_lworking_test = lpos_test;
				ExecuteBranchTests(ptest_man, m_lworking_test, 1 );			    
			}
		}
		else
		{
			// For common sequence execute final script in usual mode and correct
			// common script
			if( m_lworking_test == -1 )
			{
				m_lworking_test = lpos_test;
				_CManageManualModeTI ManageManualModeTI(sptrTI);
				_bstr_t bstrScript;
				sptrTI->GetScript(bstrScript.GetAddress());
				pMacroHelper->ClearListOfExecuted();
				ExecuteBranchTests( ptest_man, m_lworking_test, 1 );
				_bstr_t bstrNewScript = GetNewScript(bstrScript, pMacroHelper);
				sptrTI->SetScript(bstrNewScript);
				pMacroHelper->ClearListOfExecuted();
			}
		}
	}

	// run final test's sequences and generate final script
	if( m_lworking_test == lpos_test )
	{
		if (bfinal_sequence)
		{
			_CManageManualModeTI ManageManualModeTI(sptrTI);
			pMacroHelper->ClearListOfExecuted();
			ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
			_bstr_t bstr_final_script;
			sptrTI->GetScriptFinal(bstr_final_script.GetAddress());
			_bstr_t bstr_final_script_new = GetNewScript(bstr_final_script, pMacroHelper);
			sptrTI->SetScriptFinal(bstr_final_script_new);
		}
		else
			ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
		m_lworking_test = -1;
	}
	// store
	sptrTI->StoreContent();
	return TRUE;
}



BOOL	CActionEditTestDB::_input_condition( ITestManager *ptest_man, HWND hwnd_main, LPOS lpos_parent, LPOS lpos_test, XConditionType cond_type )
{
    if( !ptest_man || !hwnd_main || !lpos_test )
		return FALSE;

	CString str_message( _T("") ), str_title( _T("") );
	int nres_run_tests_branch = 0;
	if( !lpos_parent )	 // [vanek] SBT: 814 - 24.02.2004
		nres_run_tests_branch = IDYES;
	else
	{
		str_title.LoadString( IDS_RUN_TEST_SEQUENCE_TITLE );
		str_message.LoadString( IDS_ON_RUN_TESTS_BRANCH );
		nres_run_tests_branch = ::MessageBox( hwnd_main, str_message, str_title, MB_YESNOCANCEL | MB_ICONEXCLAMATION );
	}

	if( nres_run_tests_branch == IDCANCEL ) 
		return TRUE;
	else if( cond_type == xctStart && nres_run_tests_branch == IDYES )
	{
		str_message.LoadString( IDS_RUN_CURR_START_TEST );
		int nres = 0;
		nres = ::MessageBox( hwnd_main, str_message, str_title, MB_YESNO | MB_ICONEXCLAMATION );

		// [vanek] : run final test for working test - 23.03.2004
		if( m_lworking_test != -1 && m_lworking_test != (nres == IDYES ? lpos_test : lpos_parent) )
		{
			ExecuteBranchTests( ptest_man, m_lworking_test, 2 );
			m_lworking_test = -1;
		}

		// execute start test sequence
		if( m_lworking_test == -1 )
		{
			m_lworking_test = nres == IDYES ? lpos_test : lpos_parent;
			ExecuteBranchTests( ptest_man, m_lworking_test, 1);		    
		}
	}

	if( nres_run_tests_branch == IDYES )
	{
		str_title.LoadString( IDS_RUN_TEST_SEQUENCE_TITLE );
		str_message.LoadString( IDS_ON_END_RUN_TESTS_BRANCH );   
		::MessageBox( hwnd_main, str_message, str_title, MB_OK | MB_ICONEXCLAMATION );       
	}

    if( !ExecuteAction( "DataShowContents", 0, 0 ) )
		return FALSE;
	
	// read result of action
	int		nDataSource = ::GetValueInt( ::GetAppUnknown(), "\\DataShowContents", "DataSource", 0);
	CString str_full_path = ::GetValueString( ::GetAppUnknown(), "\\DataShowContents", "SelectedPath", "" ),
			str_path( _T("") );


	// get object
	IUnknown *punk = 0;
	CString str_object( _T("") );
	CString str_object_name( _T("") );
    if( nDataSource == 0 )
	{	// application
		punk = GetAppUnknown( ); // without addref
		if( punk )
			punk->AddRef();
		
		str_path = str_full_path;
		str_object = _T(APPLICATION);
	}
	else if( nDataSource == 1 )
	{	// active document
        IApplicationPtr	ptrApp = GetAppUnknown();
		ptrApp->GetActiveDocument( &punk );
		      
		// [vanek] - support object's named data in active document 23.03.2004
		int nopen_bracket = str_full_path.Find( '[' ),
			nclose_bracket = str_full_path.Find( ']' );
        if( nopen_bracket != -1 && nopen_bracket < nclose_bracket )
		{   // object
				str_object_name = str_full_path.Mid( nopen_bracket + 1, nclose_bracket - (nopen_bracket + 1) );
				str_path = str_full_path.Mid( nclose_bracket + 1 );
			IUnknown *punk_doc = punk;
			punk = 0;
            punk  = ::GetObjectByName( punk_doc, str_object_name, 0 );
			if( punk_doc )  
				punk_doc->Release( ); punk_doc = 0;

			str_object = _T(OBJECT);
		}
		else
		{	// document
			str_path = str_full_path;
			str_object = _T(DOCUMENT);
		}

	}

	INamedDataPtr	sptrNamedData = punk;
	if( punk )
		punk->Release(); punk = 0;

	_variant_t var;
	CString str_type( _T("") );
	// [vanek] SBT: 893 - 02.04.2004
	if( str_path.IsEmpty( ) && (str_object == _T(OBJECT)) )
	{
		CInputPathDlg dlg;
		dlg.m_str_object_name = str_object_name;
		if( IDOK == dlg.DoModal() )
		{
			str_type = dlg.m_str_type;
			str_path = dlg.m_str_path;
            str_path.TrimLeft();
			if( !str_path.IsEmpty() && str_path[0] != '\\' )
				str_path = '\\' + str_path;
            
			str_full_path += str_path;
		}
	}
	else 
	{	// get value
		if( sptrNamedData == 0 )
			return FALSE;

		sptrNamedData->SetupSection( 0 );

		BSTR bstrSection = str_path.AllocSysString();
		sptrNamedData->GetValue( bstrSection, &var );
		if( bstrSection )
			::SysFreeString( bstrSection ); bstrSection = 0;

		if( var.vt == VT_EMPTY )
			return FALSE;

		// get type
		str_type = VarEnumToString( &var );		
	}

	// convert to string
	_VarChangeType( var, VT_BSTR );
	
	// forming string of result
	CString str_cond( _T("") );
	try
	{
		if( !str_object.IsEmpty() && !str_full_path.IsEmpty() && !str_type.IsEmpty() && var.vt != VT_UNKNOWN &&
			var.vt != VT_DISPATCH  )
			str_cond.Format( "%s:%s:%s={%s}", str_object, str_full_path, str_type, CString( var ) );
	}
	catch(...)
	{
	}

	if( !str_cond.IsEmpty() )
	{
		IUnknownPtr punkTest;
		ptest_man->GetNextTest( lpos_parent, &lpos_test, &punkTest);
		ITestItemPtr sptrTI = punkTest;
		if( sptrTI != 0 )
		{
			// add condition
			BSTR bstr_cond = str_cond.AllocSysString();
			if( cond_type == xctStart)
				sptrTI->AddCond( bstr_cond );                
			else if( cond_type == xctExec )
				sptrTI->AddExecCond( bstr_cond );

			if( bstr_cond )
				::SysFreeString( bstr_cond ); bstr_cond = 0;

			// store
			sptrTI->StoreContent();
		}
	}
	
	return TRUE;
}