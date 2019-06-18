// EditTestDBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "EditTestDBDlg.h"
#include "TestUIUtils.h"
#include "\vt5\comps\misc\TestMan\test_defs.h"
#include "ActionEditTestDB.h"

#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )

//#include "\vt5\comps\general\data\misc_types.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XItemInfoEx
CEditTestDBDlg::XItemInfoEx::XItemInfoEx()
{
	lparent_pos = lpos = 0;
}

CEditTestDBDlg::XItemInfoEx::~XItemInfoEx()
{
}


// EditTestDBDlg dialog					 

IMPLEMENT_DYNAMIC(CEditTestDBDlg, CDialog)
CEditTestDBDlg::CEditTestDBDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEditTestDBDlg::IDD, pParent)
{
	m_selected_test.lparent_pos = m_selected_test.lpos = 0;

	// get test namager
	IUnknown *punk_testman = _GetOtherComponent(GetAppUnknown(), IID_ITestManager); 
	m_sptrTM = punk_testman;
	if( punk_testman )
		punk_testman->Release(); punk_testman = 0;

	m_bChangeConds = m_bChangeTestSequence = m_bChangeTestSequenceFinal = 
		m_bprevent_ok_cancel = m_bChangeAddInfo = m_bChangeExecConds = FALSE;

	// load bitmaps for context menu
	HBITMAP hbitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ADD_TEST));
	if( hbitmap )
		m_mapIdToBitmap.SetAt( ID_ADD_TEST, hbitmap );

	hbitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DELETE));
	if( hbitmap )
		m_mapIdToBitmap.SetAt( ID_DELETE, hbitmap );

	hbitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPY));
	if( hbitmap )
		m_mapIdToBitmap.SetAt( ID_COPY, hbitmap );

	hbitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PASTE));
	if( hbitmap )
		m_mapIdToBitmap.SetAt( ID_PASTE, hbitmap );

	hbitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RENAME));
	if( hbitmap )
		m_mapIdToBitmap.SetAt( ID__RENAME, hbitmap );

	m_ilTreeIcons.Create( IDB_TESTS, 16, 1, (COLORREF)0xFFFFFF );
}

CEditTestDBDlg::~CEditTestDBDlg()
{
	POSITION pos = m_mapIdToBitmap.GetStartPosition();
	int nkey = 0;
	HBITMAP hb_value = 0;
	while( pos )
	{
		m_mapIdToBitmap.GetNextAssoc( pos, nkey, hb_value);        
		if( hb_value )
			::DeleteObject( hb_value );
	}
	m_mapIdToBitmap.RemoveAll( );
}

void CEditTestDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TestsTree);
	DDX_Control(pDX, IDC_EDIT_START_CONDITIONS, m_EditConds);
	DDX_Control(pDX, IDC_EDIT_TEST_SEQUENCE, m_EditTestSequence);
	DDX_Control(pDX, IDC_EDIT_FINAL_TEST_SEQUENCE, m_EditFinalTestSequence);
	DDX_Control(pDX, IDC_EDIT_INFO, m_EditAddInfo);
	DDX_Control(pDX, IDC_EDIT_EXEC_COND, m_EditExecConds);
}


BEGIN_MESSAGE_MAP(CEditTestDBDlg, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, OnNMRclickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE1, OnTvnDeleteitemTree1)

	ON_EN_CHANGE(IDC_EDIT_START_CONDITIONS, OnEnChangeEditStartConditions)
	ON_EN_CHANGE(IDC_EDIT_TEST_SEQUENCE, OnEnChangeEditTestSequence)
	ON_EN_CHANGE(IDC_EDIT_FINAL_TEST_SEQUENCE, OnEnChangeEditFinalTestSequence)
	ON_EN_CHANGE(IDC_EDIT_INFO, OnEnChangeEditInfo)

	ON_EN_MAXTEXT(IDC_EDIT_START_CONDITIONS, OnEnMaxTextEditStartConditions)
	ON_EN_MAXTEXT(IDC_EDIT_TEST_SEQUENCE, OnEnMaxTextEditTestSequence)
	ON_EN_MAXTEXT(IDC_EDIT_FINAL_TEST_SEQUENCE, OnEnMaxTextEditFinalTestSequence)
	ON_EN_MAXTEXT(IDC_EDIT_INFO, OnEnMaxTextEditInfo)

	ON_BN_CLICKED(IDC_BUTTON_START_CONDITIONS, OnBnClickedButtonStartConditions)
	ON_BN_CLICKED(IDC_BUTTON_TEST_SEQUENCE, OnBnClickedButtonTestSequence)
	ON_BN_CLICKED(IDC_BUTTON_FINAL_TEST_SEQUENCE, OnBnClickedButtonFinalTestSequence)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_TEST_SEQUENCE, OnBnClickedButtonEditTestSequence)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE, OnBnClickedButtonEditFinalTestSequence)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, OnTvnEndlabeleditTree1)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, OnTvnBeginlabeleditTree1)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, OnTvnKeydownTree1)
	ON_EN_CHANGE(IDC_EDIT_INFO, OnEnChangeEditInfo)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_ENV, OnBnClickedButtonSaveEnv)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_TREE1, OnTvnGetInfoTipTree1)
	ON_BN_CLICKED(IDC_BUTTON_EXEC_COND, OnBnClickedButtonExecCond)
	ON_EN_CHANGE(IDC_EDIT_EXEC_COND, OnEnChangeEditExecCond)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SIZING()
END_MESSAGE_MAP()


// CEditTestDBDlg message handlers
BOOL CEditTestDBDlg::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog::OnInitDialog();

	m_gr1.subclass( ::GetDlgItem( m_hWnd, IDC_STATIC_GR1 ) );
	m_gr2.subclass( ::GetDlgItem( m_hWnd, IDC_STATIC_GR2 ) ); 

	if( m_ilTreeIcons.GetSafeHandle() )
		m_TestsTree.SetImageList( &m_ilTreeIcons, TVSIL_NORMAL );

	// Set the bitmap of the button to be the system check mark bitmap.
	HBITMAP hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(OBM_BUTTON_START_CONDITIONS) );
	((CButton*)GetDlgItem(IDC_BUTTON_START_CONDITIONS))->SetBitmap(hbm);
	((CButton*)GetDlgItem(IDC_BUTTON_EXEC_COND))->SetBitmap(hbm);
	hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TEST_SEQUENCE));
	((CButton*)GetDlgItem(IDC_BUTTON_TEST_SEQUENCE))->SetBitmap(hbm);
	hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TEST_SEQUENCE));
	((CButton*)GetDlgItem(IDC_BUTTON_FINAL_TEST_SEQUENCE))->SetBitmap(hbm);
	hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_EDIT_TEST_SEQUENCE));
	((CButton*)GetDlgItem(IDC_BUTTON_EDIT_TEST_SEQUENCE))->SetBitmap(hbm);
	hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_EDIT_TEST_SEQUENCE));
	((CButton*)GetDlgItem(IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE))->SetBitmap(hbm);

	m_EditConds.SetLimitText( 0 );
	m_EditTestSequence.SetLimitText( 0 );
	m_EditFinalTestSequence.SetLimitText( 0 );
	m_EditAddInfo.SetLimitText( 0 );
	m_EditExecConds.SetLimitText( 0 );

	InitTestsWindow();

	m_TestsTree.Expand( m_TestsTree.GetChildItem( TVI_ROOT ), TVE_EXPAND );
	UpdateSelectedTest( );
	m_TestsTree.UpdateWindow();

	init_resize();

	::GetWindowRect( m_hWnd, &m_rect_wnd );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditTestDBDlg::InitTestsWindow()
{
	if (m_sptrTM != 0)
		DoInitTestsWindow(TVI_ROOT, m_sptrTM, 0);				  
}

void CEditTestDBDlg::DoInitTestsWindow(HTREEITEM hti, ITestManager *pTestManager, long lParentPos)
{
	long lPos = 0;
	pTestManager->GetFirstTestPos(lParentPos, &lPos);
	while (lPos)
	{
		IUnknownPtr punkTest;
		long lPosSaved = lPos;
		pTestManager->GetNextTest(lParentPos, &lPos, &punkTest);
		ITestItemPtr sptrTI =  punkTest;
		if (sptrTI == 0)
			continue;

		// 1. Item exist?
		HTREEITEM hti_exist = FindTestItemInTree( hti, lParentPos, lPosSaved );
		if( hti_exist )
			continue;

		// 2. Get test name
		BSTR bstrPath = NULL;
		sptrTI->GetPath(&bstrPath);
		CString sName = _TestNameFromPath(CString(bstrPath));
		::SysFreeString(bstrPath);

		// 3. Add item
		HTREEITEM htiNew = AddTreeItem( hti, sName, lParentPos, lPosSaved );
		if( !htiNew )
			continue;

		// 4. Add children
		DoInitTestsWindow(htiNew, pTestManager, lPosSaved);

		// 5. Set Icon
		UpdateTreeItemIcon(htiNew);
	}

	// [vanek] SBT: 811 - 11.03.2004
	m_TestsTree.SortChildren( hti );
}

void CEditTestDBDlg::OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	::GetCursorPos( &pt );

	CMenu *psub_menu = 0;
	CMenu menu;

	menu.LoadMenu( IDR_MENU_EDIT_TEST_DB );
	psub_menu = menu.GetSubMenu(0);
	if( !psub_menu )
		return;

	::SetMenuItemBitmaps( psub_menu->GetSafeHmenu(), ID_ADD_TEST, MF_BYCOMMAND, m_mapIdToBitmap[ID_ADD_TEST], 
		m_mapIdToBitmap[ID_ADD_TEST]);
	::SetMenuItemBitmaps( psub_menu->GetSafeHmenu(), ID_DELETE, MF_BYCOMMAND, m_mapIdToBitmap[ID_DELETE], 
		m_mapIdToBitmap[ID_DELETE]);
	::SetMenuItemBitmaps( psub_menu->GetSafeHmenu(), ID_COPY, MF_BYCOMMAND, m_mapIdToBitmap[ID_COPY], 
		m_mapIdToBitmap[ID_COPY]);
	::SetMenuItemBitmaps( psub_menu->GetSafeHmenu(), ID_PASTE, MF_BYCOMMAND, m_mapIdToBitmap[ID_PASTE], 
		m_mapIdToBitmap[ID_PASTE]);
	::SetMenuItemBitmaps( psub_menu->GetSafeHmenu(), ID__RENAME, MF_BYCOMMAND, m_mapIdToBitmap[ID__RENAME], 
		m_mapIdToBitmap[ID__RENAME]);

	// [vanek] SBT: 835 - disable menu item 27.02.2004
	if( !::IsClipboardFormatAvailable( ::RegisterClipboardFormat(_T(szTestClipboardFormat)) ) )
		::EnableMenuItem( psub_menu->GetSafeHmenu(), ID_PASTE, MF_BYCOMMAND | MF_GRAYED );

	int ncmd = psub_menu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, this, 0 );
	m_TestsTree.ScreenToClient( &pt );
	HTREEITEM hti = m_TestsTree.HitTest( pt );
	switch( ncmd )
	{
	case ID_ADD_TEST:
		OnAddNewTest( hti );
		break;

	case ID_DELETE:
		OnDeleteTest( hti );
		break;

	case ID__RENAME:
		OnRenameTest( hti );
		break;

	case ID_COPY:
		OnCopyTest( hti );
		break;

	case ID_PASTE:
		OnPasteTest( hti );
		break;
	}

	*pResult = 0;
}

HTREEITEM	CEditTestDBDlg::AddTreeItem( HTREEITEM hti_parent, CString sName, long lparent_pos, long lpos)
{
	HTREEITEM htiNew = 0;
	if( !hti_parent )
		return htiNew;

	XItemInfoEx *pitem_info_ex = 0;
	pitem_info_ex = new XItemInfoEx;
	pitem_info_ex->lpos = lpos;
	pitem_info_ex->lparent_pos = lparent_pos;

	htiNew = m_TestsTree.InsertItem( TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT, sName, 0, 0, 0, 0, (LPARAM)pitem_info_ex, hti_parent, TVI_LAST);
	if( !htiNew )
	{
		delete pitem_info_ex; 
		pitem_info_ex = 0;
	}
	else
	{
		// update icons
		UpdateTreeItemIcon( htiNew );
		UpdateTreeItemIcon( hti_parent );
	}

	return htiNew;	
}

BOOL	CEditTestDBDlg::DeleteTreeItem( HTREEITEM hti )
{
	if( !hti )
		return FALSE;

	HTREEITEM hti_parent  = m_TestsTree.GetParentItem( hti );
	if( !m_TestsTree.DeleteItem( hti ) )	// lParam free on fit notification!
		return FALSE;

	UpdateTreeItemIcon( hti_parent );  
	return TRUE;
}

void CEditTestDBDlg::UpdateTreeItemIcon( HTREEITEM hti )
{
	if( !hti || hti == TVI_FIRST || hti == TVI_LAST || hti == TVI_ROOT )
		return;

	int nImageIdx = -1;
	nImageIdx = m_TestsTree.ItemHasChildren( hti ) ? 1 : 0;
	m_TestsTree.SetItemImage( hti, nImageIdx, nImageIdx );

}

void CEditTestDBDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	SynchronizeTestItemWithCtrls( &pNMTreeView->itemOld );   	
	SynchronizeCtrlsWithTestItem( &pNMTreeView->itemNew );
	*pResult = 0;
}

void CEditTestDBDlg::OnTvnDeleteitemTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	// free lParam
	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)pNMTreeView->itemOld.lParam;
	if( pitem_info_ex )
	{
		delete pitem_info_ex;
		pNMTreeView->itemOld.lParam = 0;
	}

	*pResult = 0;
}

void	CEditTestDBDlg::SynchronizeCtrlsWithTestItem( TVITEM *pst_tvitem )
{
	if( !pst_tvitem )
		return;

	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)pst_tvitem->lParam;
	if (m_sptrTM == 0 || !pitem_info_ex )
		return;

	IUnknownPtr sptrTest;
	long lPos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lPos, &sptrTest);
	ITestItemPtr sptrTI = sptrTest;
	if( sptrTI == 0 )
		return;

	// [vanek] : get executing conditions - 24.08.2004
    BSTR bstrCond = NULL;
	long lCondPos = 0;
	CString sConds( _T("") );
	sptrTI->GetFirstExecCondPos(&lCondPos);
	while (lCondPos)
	{
		sptrTI->GetNextExecCond(&lCondPos, &bstrCond);
		CString sCond(bstrCond);
		::SysFreeString(bstrCond);
		bstrCond = NULL;
		sConds += sCond;
		if( lCondPos )
			sConds += _T( "\r\n" );
	}		
	m_EditExecConds.SetWindowText( sConds );
	m_bChangeExecConds = FALSE;

	// get test conditions
	bstrCond = NULL;
	lCondPos = 0;
	sConds.Empty( );
	sptrTI->GetFirstCondPos(&lCondPos);
	while (lCondPos)
	{
		sptrTI->GetNextCond(&lCondPos, &bstrCond);
		CString sCond(bstrCond);
		::SysFreeString(bstrCond);
		bstrCond = NULL;
		sConds += sCond;
		if( lCondPos )
			sConds += _T( "\r\n" );
	}		
	m_EditConds.SetWindowText( sConds );
	//_SetEditText( &m_EditConds, sConds );
	m_bChangeConds = FALSE;

	// get test script
	BSTR bstrScript = 0;
	sptrTI->GetScript(&bstrScript);
	m_EditTestSequence.SetWindowText( CString(bstrScript) );
	if( bstrScript )
		::SysFreeString(bstrScript); bstrScript	= 0;
	m_bChangeTestSequence = FALSE;

	// get test script 2
	BSTR bstrFinalScript = 0;
	sptrTI->GetScriptFinal(&bstrFinalScript);
	m_EditFinalTestSequence.SetWindowText( CString(bstrFinalScript) );
	if( bstrFinalScript )
		::SysFreeString( bstrFinalScript ); bstrFinalScript = 0;
	m_bChangeTestSequenceFinal = FALSE;

	// [vanek] - 06.08.2004
	// get environment's load flags
	{
		DWORD dwload_flags = 0;
		ITestEnvironmentPtr sptr_te = sptrTI;
		if( sptr_te != 0 )
			sptr_te->GetLoadFlags( &dwload_flags);
        
		CheckDlgButton( IDC_CHECK_LOAD_STATE, (UINT)(dwload_flags & tefState));
		CheckDlgButton( IDC_CHECK_LOAD_SDC, (UINT)(dwload_flags & tefSDC));
	}

	// [vanek] get saved test's info - 09.07.2004
	_update_test_info_ctrls( sptrTI );

	// get test add info
	BSTR bstrAddInfo = 0;
	sptrTI->GetAdditionalInfo( &bstrAddInfo );
	m_EditAddInfo.SetWindowText( CString(bstrAddInfo) );
	if( bstrAddInfo )
		::SysFreeString( bstrAddInfo ); bstrAddInfo = 0;
	m_bChangeAddInfo = FALSE;

	//[max] get combination count
	long lCount = 0, lCountTests = 0, lCountTestsR = 0;
	m_sptrTM->GetCombinationCont( pitem_info_ex->lpos, &lCount);
	m_sptrTM->GetTestCont( 0, pitem_info_ex->lpos, &lCountTests );
	m_sptrTM->GetTestContR( 0, pitem_info_ex->lpos, &lCountTestsR );

	CString str, str2, str3, str4;
	str.LoadString( IDS_COMB );
	str3.LoadString( IDS_COMB_TEST );
	str4.LoadString( IDS_COMB_TEST_REAL );

	str2.Format( " %s %ld.\n %s %ld.\n %s %ld.", str, lCount, str3, lCountTests, str4, lCountTestsR );
	::SetWindowText( ::GetDlgItem( m_hWnd, IDC_STATIC_CMB ), str2 );
}

void	CEditTestDBDlg::SynchronizeTestItemWithCtrls( TVITEM *pst_tvitem )
{
	if( !pst_tvitem )
		return;

	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)pst_tvitem->lParam;
	if( m_sptrTM == 0 || !pitem_info_ex )
		return;

	IUnknownPtr sptrTest;
	long lPos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lPos, &sptrTest);
	ITestItemPtr sptrTI = sptrTest;
	if( sptrTI == 0 )
		return;

	// [vanek] : set executing conditions - 24.08.2004
	if( m_bChangeExecConds )
	{
        sptrTI->DeleteAllExecCond();
		int nlines = m_EditExecConds.GetLineCount();
		for( int nline = 0; nline < nlines; nline ++)
		{
			CString str_cond( _T("") );
			int nline_length = m_EditExecConds.LineLength( m_EditExecConds.LineIndex( nline ) );
			if( !nline_length )
				continue;

			m_EditExecConds.GetLine( nline, str_cond.GetBufferSetLength( nline_length), nline_length );
			str_cond.ReleaseBuffer();
			str_cond.TrimLeft();
			str_cond.TrimRight();
			if( str_cond.IsEmpty() )
				continue;

			BSTR bstr_cond = str_cond.AllocSysString();
			sptrTI->AddExecCond( bstr_cond );                
			if( bstr_cond )
				::SysFreeString( bstr_cond ); bstr_cond = 0;
		}

		sptrTI->ParseExecCond( );
	}

	// set conditions
	if( m_bChangeConds )
	{
		sptrTI->DeleteAllCond();
		int nlines = m_EditConds.GetLineCount();
		for( int nline = 0; nline < nlines; nline ++)
		{
			CString str_cond( _T("") );
			int nline_length = m_EditConds.LineLength( m_EditConds.LineIndex( nline ) );
			if( !nline_length )
				continue;

			m_EditConds.GetLine( nline, str_cond.GetBufferSetLength( nline_length), nline_length );
			str_cond.ReleaseBuffer();
			str_cond.TrimLeft();
			str_cond.TrimRight();
			if( str_cond.IsEmpty() )
				continue;

			BSTR bstr_cond = str_cond.AllocSysString();
			sptrTI->AddCond( bstr_cond );                
			if( bstr_cond )
				::SysFreeString( bstr_cond ); bstr_cond = 0;
		}

		sptrTI->ParseCond( );
	}

	// set test script
	if( m_bChangeTestSequence )
	{
		CString str_test( _T("") );
		m_EditTestSequence.GetWindowText( str_test );
		BSTR bstr_script = str_test.AllocSysString( );
		sptrTI->SetScript( bstr_script );    
		if( bstr_script )
			::SysFreeString( bstr_script ); bstr_script = 0;
	}

	// set final test script 
	if( m_bChangeTestSequenceFinal )
	{
		CString str_test_final( _T("") );
		m_EditFinalTestSequence.GetWindowText( str_test_final );
		BSTR bstr_script = str_test_final.AllocSysString( );
		sptrTI->SetScriptFinal( bstr_script );    
		if( bstr_script )
			::SysFreeString( bstr_script ); bstr_script = 0;
	}

	// [vanek] SBT: 867 - 23.03.2004
	// set environment's load flags
	BOOL bChangeLoadFlags = FALSE;
	{
		ITestEnvironmentPtr sptr_te = sptrTI;
		if( sptr_te != 0 )
		{
			DWORD dwold_load_flags = 0;
            sptr_te->GetLoadFlags( &dwold_load_flags );

            DWORD dwnew_load_flags = dwold_load_flags;
			dwnew_load_flags = (IsDlgButtonChecked(IDC_CHECK_LOAD_STATE) ? (dwnew_load_flags | tefState) : (dwnew_load_flags & ~tefState) );
            dwnew_load_flags = (IsDlgButtonChecked(IDC_CHECK_LOAD_SDC) ? (dwnew_load_flags | tefSDC) : (dwnew_load_flags & ~tefSDC) );
			if( bChangeLoadFlags = ( dwnew_load_flags != dwold_load_flags ) )
				sptr_te->SetLoadFlags( dwnew_load_flags );
		}
	}


	// set add info 
	if( m_bChangeAddInfo )
	{
		CString str_add_info( _T("") );
		m_EditAddInfo.GetWindowText( str_add_info );
		BSTR bstr_add_info = str_add_info.AllocSysString( );
		sptrTI->SetAdditionalInfo( bstr_add_info );    
		if( bstr_add_info )
			::SysFreeString( bstr_add_info ); bstr_add_info = 0;
	}

	// reset saved state and store content
	BOOL bchange_body = m_bChangeConds || m_bChangeTestSequence || m_bChangeTestSequenceFinal || 
		bChangeLoadFlags || m_bChangeExecConds;
	if( bchange_body || m_bChangeAddInfo )
	{
		// [vanek] SBT: 866 - 23.03.2004
		if( bchange_body  )
		{
			// reset saved state
			ITestRunningDataPtr sptrTestRunning = sptrTI;
			if( sptrTestRunning != 0 )
			{
				sptrTestRunning->SetTestSavedState( 0 );
				sptrTestRunning->SetSaveTime( 0 );                        
			}

			if( m_TestsTree.ItemHasChildren( pst_tvitem->hItem ) && 
				HasSavedChild( m_sptrTM, pitem_info_ex->lpos ) )	// [vanek] SBT:891 - 01.04.2004
			{
				CString str_message( _T("") ),
					str_title( _T("") );
				str_message.Format( IDS_TEST_MODIFIED, _TestName( sptrTI ) );
				str_title.LoadString( IDS_ATTENTION );

				if( IDYES == MessageBox( str_message, str_title, MB_YESNO | MB_ICONEXCLAMATION ) )
					DoOperationWithChildTests( m_sptrTM, toResetSavedState | toResetSaveTime, pitem_info_ex->lpos );
			}
		}


		// store
		sptrTI->StoreContent();
	}

	m_bChangeConds = m_bChangeTestSequence = m_bChangeTestSequenceFinal = m_bChangeAddInfo = FALSE;
}

BOOL	CEditTestDBDlg::GenerateUniqueTestName( HTREEITEM hti_parent, CString sbase_name, CString *psUniqueTestName )
{
	if( !psUniqueTestName )
		return FALSE;

	if( sbase_name.IsEmpty() )
		sbase_name.LoadString( AfxGetInstanceHandle(), IDS_NEW_TEST_NAME );

	HTREEITEM hti_child = m_TestsTree.GetChildItem( hti_parent );
	int nmax_number = -1;
	while( hti_child )
	{
		CString schild_name = m_TestsTree.GetItemText( hti_child );
		schild_name.TrimLeft();
		if( 0 == schild_name.Find( sbase_name ) )
		{
			schild_name.Delete( 0, sbase_name.GetLength() );
			int number = _tstoi( schild_name );
			if( number > nmax_number )
				nmax_number = number;
		}

		hti_child = m_TestsTree.GetNextItem( hti_child, TVGN_NEXT );
	}					

	nmax_number ++;
	if( nmax_number > 0)
		psUniqueTestName->Format( "%s%d", sbase_name, nmax_number );
	else
		*psUniqueTestName = sbase_name;

	return TRUE;
}

HTREEITEM	CEditTestDBDlg::FindTestItemInTree( HTREEITEM hti_parent, long lpos_parent_test, long lpos_test )
{
	if( !lpos_test )
		return 0;

	HTREEITEM hti = m_TestsTree.GetChildItem( hti_parent );
	while( hti )
	{
		XItemInfoEx *pitem_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti );
		if( pitem_info_ex->lparent_pos == lpos_parent_test && pitem_info_ex->lpos == lpos_test )
			break;

		HTREEITEM hti_find = FindTestItemInTree( hti, lpos_parent_test, lpos_test );
		if( !hti_find )
			hti = m_TestsTree.GetNextItem( hti, TVGN_NEXT );
		else
		{
			hti = hti_find;
			break;
		}
	}

	return 	hti;
}

HTREEITEM	CEditTestDBDlg::FindItemByName( HTREEITEM hti_parent, CString str_item_name, BOOL bsearch_in_subitems )
{
	if( !hti_parent )
		return 0;

	HTREEITEM hti = 0;
	hti = m_TestsTree.GetChildItem( hti_parent );
	while( hti )
	{
		CString str_text_item( _T("") );
		str_text_item = m_TestsTree.GetItemText( hti );
		if( str_text_item == str_item_name )
			break;

		if( bsearch_in_subitems )
		{
			HTREEITEM hti_found = 0;
			hti_found = FindItemByName( hti, str_item_name, bsearch_in_subitems );
			if( hti_found )  
				return hti_found;
		}

		hti = m_TestsTree.GetNextItem( hti, TVGN_NEXT );
	}    

	return hti;
}


void	CEditTestDBDlg::SetSelectedTestItem( long lpos_parent, long lpos )
{
	m_selected_test.lparent_pos = lpos_parent;
	m_selected_test.lpos = lpos;
}

void	CEditTestDBDlg::GetSelectedTestItem( long *plpos_parent, long *plpos )
{
	if( !plpos_parent || !plpos )
		return;

	*plpos_parent = m_selected_test.lparent_pos;
	*plpos = m_selected_test.lpos;
}

void	CEditTestDBDlg::UpdateSelectedTest()
{
	HTREEITEM hti_select = 0;
	hti_select = FindTestItemInTree( TVI_ROOT, m_selected_test.lparent_pos, m_selected_test.lpos );
	if( !hti_select )
		hti_select = m_TestsTree.GetFirstVisibleItem();

	m_TestsTree.SelectItem( hti_select );    
}

void		CEditTestDBDlg::_update_test_info_ctrls( ITestItem *pti )
{
	if( !pti )
		return;

	_bstr_t bstr_path( _T("") );
	pti->GetPath( bstr_path.GetAddress() );
	CString str_state_path( _T("") );
	_bstr_t bstr_lang( _T("") );
	m_sptrTM->GetLang( bstr_lang.GetAddress() );
	if( bstr_lang.length() )
		str_state_path = (LPTSTR)(bstr_path + _T("\\"szTestSavedStateName"_") + bstr_lang + _T(".state"));

	CString str_fi( _T("") );
	if( !str_state_path.IsEmpty() ) 
	{
		HANDLE	hfile = ::CreateFile( str_state_path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
			FILE_ATTRIBUTE_NORMAL, 0 );

		if( hfile != INVALID_HANDLE_VALUE )
		{
			// get file creation info
			FILETIME	filetime, ft;
			::GetFileTime( hfile, 0, 0, &filetime );
			::CloseHandle( hfile );

			::FileTimeToLocalFileTime( &filetime, &ft );
			SYSTEMTIME	systime;
			::FileTimeToSystemTime( &ft, &systime );                

			str_fi.Format( "%.2d.%.2d.%d %.2d:%.2d", systime.wDay, systime.wMonth, systime.wYear, systime.wHour, systime.wMinute );            			
		}
	}

	::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_CHECK_LOAD_STATE ), !str_fi.IsEmpty() );        
	::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_CHECK_LOAD_SDC ), !str_fi.IsEmpty() );        
	
	CString str_no( _T("") );
	str_no.LoadString( IDS_NO_STRING );
	
	ITestEnvironmentPtr sptr_te( pti );
	_bstr_t bstrKbdLayout;
	if( sptr_te != 0 )
	{
		sptr_te->GetKbdLayoutName( bstrKbdLayout.GetAddress( ) );
		if( bstrKbdLayout.length( ) )
		{
			DWORD  dw_klid = 0;
			dw_klid = strtoul( bstrKbdLayout, 0, 16 );
			int nbuff_length = 0;
			nbuff_length = ::GetLocaleInfo( (LCID) MAKELCID( LOWORD(dw_klid),SORT_DEFAULT), LOCALE_SISO639LANGNAME , 0, 0);
			TCHAR *ptcbuff_lang = 0;
			ptcbuff_lang = new TCHAR[ nbuff_length ];
			::ZeroMemory( ptcbuff_lang, sizeof(TCHAR) *nbuff_length  );
			::GetLocaleInfo( (LCID)MAKELCID( LOWORD(dw_klid),SORT_DEFAULT), LOCALE_SISO639LANGNAME , ptcbuff_lang, nbuff_length);                
			bstrKbdLayout = ptcbuff_lang;
			if( ptcbuff_lang )
				delete[] ptcbuff_lang; ptcbuff_lang = 0;
		}
	}

	CString str_saved_state( _T("") );
	str_saved_state.Format( IDS_ENV_INFO_STRING, (!str_fi.IsEmpty() ? str_fi : str_no), (bstrKbdLayout.length( ) ? (TCHAR *)(bstrKbdLayout) : str_no) ); 
	::SetWindowText( ::GetDlgItem( m_hWnd, IDC_STATIC_ENV_INFO ), str_saved_state );
}

void CEditTestDBDlg::OnEnChangeEditStartConditions()
{
	m_bChangeConds = TRUE;
}

void CEditTestDBDlg::OnEnChangeEditTestSequence()
{
	m_bChangeTestSequence = TRUE;
}

void CEditTestDBDlg::OnEnChangeEditFinalTestSequence()
{
	m_bChangeTestSequenceFinal = TRUE;
}

void CEditTestDBDlg::OnEnChangeEditInfo()
{
	m_bChangeAddInfo = TRUE;   
}

void CEditTestDBDlg::OnEnMaxTextEditStartConditions()
{
	m_EditConds.MessageBox( "The text insertion has been truncated!", ":(", MB_OK | MB_ICONERROR );
}

void CEditTestDBDlg::OnEnMaxTextEditTestSequence()
{
	m_EditTestSequence.MessageBox( "The text insertion has been truncated!", ":(", MB_OK | MB_ICONERROR );
}

void CEditTestDBDlg::OnEnMaxTextEditFinalTestSequence()
{
	m_EditFinalTestSequence.MessageBox( "The text insertion has been truncated!", ":(", MB_OK | MB_ICONERROR );
}

void CEditTestDBDlg::OnEnMaxTextEditInfo()
{
	m_EditAddInfo.MessageBox( "The text insertion has been truncated!", ":(", MB_OK | MB_ICONERROR );
}

BOOL CEditTestDBDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( LOWORD( wParam ) )
	{
	case IDOK:
	case IDCANCEL:
	case IDCLOSE:
	case IDC_BUTTON_TEST_SEQUENCE:
	case IDC_BUTTON_FINAL_TEST_SEQUENCE:
	case IDC_BUTTON_EDIT_TEST_SEQUENCE:
	case IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE:
	case IDC_BUTTON_START_CONDITIONS:
	case IDC_BUTTON_EXEC_COND:
		{
			if( (wParam == IDOK || wParam == IDCANCEL) && m_bprevent_ok_cancel )
			{
				m_TestsTree.SetFocus();
				return TRUE; // prevent
			}

			// update selected item data
			HTREEITEM hItem = m_TestsTree.GetSelectedItem();
			TVITEM st_tvitem = {0};
			st_tvitem.mask = TVIF_HANDLE | TVIF_PARAM;
			st_tvitem.hItem =  hItem;
			m_TestsTree.GetItem( &st_tvitem );
			SynchronizeTestItemWithCtrls( &st_tvitem );

			// save selected
			if( st_tvitem.lParam )
				m_selected_test = *((XItemInfoEx*)st_tvitem.lParam);

		}
		break;
	}

	return CDialog::OnCommand(wParam, lParam);
}




void	CEditTestDBDlg::OnAddNewTest( HTREEITEM hti_parent )
{
	if( !hti_parent || m_sptrTM == 0 )
		return;

	// get parent test item
	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti_parent );
	if( !pitem_info_ex )
		return;

	long lpos_saved = pitem_info_ex->lpos;
	IUnknownPtr sptr_parent;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lpos_saved, &sptr_parent);
	ITestItemPtr sptr_test_parent = sptr_parent;
	if( sptr_test_parent == 0 )
		return;

	// create new test item
	IUnknown *punk = 0;
	if( S_OK != ::CoCreateInstance( clsidTestItem, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
		return;

	ITestItemPtr sptr_test_item( punk );
	if( punk )
		punk->Release();punk = 0;
	if( sptr_test_item == 0 )
		return;

	// set test's path
	CString stest_name( _T("") ), 
		stest_path( _T("") );

	if( !GenerateUniqueTestName( hti_parent, 0, &stest_name ) )
		return;

	BSTR bstr_path = 0;
	sptr_test_parent->GetPath( &bstr_path );
	stest_path = CString(bstr_path) + _T("\\") + stest_name;	
	if( bstr_path )
		::SysFreeString( bstr_path );

	bstr_path = stest_path.AllocSysString( );
	sptr_test_item->SetPath( bstr_path );   
	if( bstr_path )
		::SysFreeString( bstr_path );


	// add new test
	long lpos_newtest = 0;
	m_sptrTM->AddTest( pitem_info_ex->lpos, sptr_test_item, &lpos_newtest );
	if( !lpos_newtest )
		return;

	sptr_test_item->AddRef();
	sptr_test_item->Release();

	// store to disk
	sptr_test_item->StoreContent( );

	// add tree item
	HTREEITEM hti_new = AddTreeItem( hti_parent, stest_name, pitem_info_ex->lpos, lpos_newtest );

	// [vanek] SBT: 811 - 11.03.2004
	m_TestsTree.SortChildren( hti_parent );

	// set selection
	m_TestsTree.Select( hti_new, TVGN_CARET );
}

void	CEditTestDBDlg::OnDeleteTest( HTREEITEM hti )
{
	if( !hti || m_sptrTM == 0 )
		return;

	CString str_prompt( _T("") ), str_title( _T("") );
	str_title.LoadString( AfxGetInstanceHandle(), IDS_DELETE_TEST );
	str_prompt.LoadString( AfxGetInstanceHandle(), 
		m_TestsTree.ItemHasChildren( hti ) ? IDS_ON_DELETE_GROUP : IDS_ON_DELETE_TEST ); 
	if( IDNO == MessageBox( str_prompt, str_title, MB_YESNO | MB_ICONEXCLAMATION ) )
		return;

	// get next select tree item
	HTREEITEM hti_next_sel = m_TestsTree.GetNextSiblingItem( hti );
	if( !hti_next_sel )
		hti_next_sel = m_TestsTree.GetPrevSiblingItem( hti );
	if( !hti_next_sel )
		hti_next_sel = m_TestsTree.GetParentItem( hti );

	// get test item
	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti );
	if( !pitem_info_ex )	 
		return;

	IUnknownPtr sptrTest;
	long lPos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lPos, &sptrTest);
	ITestItemPtr sptrTI = sptrTest;
	if( sptrTI == 0 )
		return;

	// destroy content of test item 
	sptrTI->KillThemselvesFromDisk();

	// delete test item
	m_sptrTM->DeleteTest( pitem_info_ex->lpos );

	// delete tree item
	DeleteTreeItem( hti );

	// set selection
	m_TestsTree.Select( hti_next_sel, TVGN_CARET );
}

void	CEditTestDBDlg::OnRenameTest( HTREEITEM hti_rename )
{
	m_TestsTree.EditLabel( hti_rename );
}

void	CEditTestDBDlg::OnCopyTest( HTREEITEM hti )
{
	if( !hti || m_sptrTM == 0 )
		return;

	// update test's data
	TVITEM st_tvitem = {0};
	st_tvitem.mask = TVIF_HANDLE | TVIF_PARAM;
	st_tvitem.hItem =  hti;
	m_TestsTree.GetItem( &st_tvitem );
	SynchronizeTestItemWithCtrls( &st_tvitem );

	// get test item as serializable object
	XItemInfoEx *pitem_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti );
	if( !pitem_info_ex )	 
		return;

	IUnknownPtr sptrTest;
	long lpos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lpos, &sptrTest);
	ISerializableObjectPtr sptrSO = sptrTest;
	if( sptrSO == 0 )
		return;

	HWND	hWnd = 0;
	sptrIApplication	sptrA( GetAppUnknown() );
	sptrA->GetMainWindowHandle( &hWnd );

	// open clipboard
	if( !::OpenClipboard( hWnd ) )
	{
		//DislayWin32Error( GetLastError(), hWnd, 0 );
		return;
	}

	// clear clipboard
	if( !::EmptyClipboard() )
	{
		//DislayWin32Error( GetLastError(), hWnd, 0 );
		::CloseClipboard();
		return;
	}

	// create stream
	IStream*	pi_stream = 0;
	::CreateStreamOnHGlobal( 0, FALSE, &pi_stream );
	if( !pi_stream )
	{			
		//DislayWin32Error( GetLastError(), hWnd, 0 );
		::CloseClipboard();
		return;
	}

	// store test to stream
	sptrSO->Store( pi_stream, 0 );

	// store childs tests to stream
	if( S_OK != m_sptrTM->StoreTests( pi_stream, pitem_info_ex->lpos ) )
	{
		::CloseClipboard();
		return;
	}

	HGLOBAL h = 0;
	GetHGlobalFromStream( pi_stream, &h );
	if( ::SetClipboardData( ::RegisterClipboardFormat( _T(szTestClipboardFormat) ), h ) == 0 )
	{
		//DislayWin32Error( GetLastError(), hWnd, 0 );
		::CloseClipboard();
		return;
	}

	::CloseClipboard();
}

void	CEditTestDBDlg::OnPasteTest( HTREEITEM hti_paste )
{
	if( !hti_paste || m_sptrTM == 0 )
		return;

	HTREEITEM hti_parent = 0;
	hti_parent = hti_paste;

	UINT uitest_clipbrd_format = ::RegisterClipboardFormat( _T(szTestClipboardFormat) );
	if( !::IsClipboardFormatAvailable(uitest_clipbrd_format) )
		return;

	if( !hti_parent )
		return;

	// get parent test item
	XItemInfoEx *pparent_item_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti_parent );
	if( !pparent_item_info_ex )	 
		return;
	IUnknownPtr sptrParentTest;
	long lpos = pparent_item_info_ex->lpos;
	m_sptrTM->GetNextTest( pparent_item_info_ex->lparent_pos, &lpos, &sptrParentTest);
	lpos = 0;
	ITestItemPtr sptrParentTI = sptrParentTest;
	if( sptrParentTI == 0 )
		return;

	// create new test for loading
	IUnknown *punkTest = 0;
	if( ::CoCreateInstance( clsidTestItem, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punkTest ) != S_OK )
		return;

	if( !punkTest )
		return;

	ISerializableObjectPtr sptrSO = punkTest;
	punkTest->Release();
	if( sptrSO == 0 )
		return;

	ITestItemPtr sptrLoadedTI = sptrSO;
	if( sptrLoadedTI == 0 )
		return;

	// open clipboard
	HWND	hWnd = 0;
	sptrIApplication	sptrA( GetAppUnknown() );
	sptrA->GetMainWindowHandle( &hWnd );

	if( !::OpenClipboard( hWnd ) )
		return;

	HGLOBAL	hGlobal = ::GetClipboardData( uitest_clipbrd_format );
	if( !hGlobal )
	{
		::CloseClipboard( );
		return;
	}

	IStream* pi_stream = 0;
	::CreateStreamOnHGlobal( hGlobal, FALSE, &pi_stream );
	if( !pi_stream )
	{
		::CloseClipboard( );
		return;
	}

	// load test from stream
	sptrSO->Load( pi_stream, 0 );

	// check loaded test's name
	CString str_test_name( _T("") );
	str_test_name = _TestName( sptrLoadedTI );
	// [vanek] SBT:905 - 06.04.2004
	HTREEITEM hti = FindItemByName( hti_parent, str_test_name, FALSE );
	if( hti )
	{	// item with same name exist - generate new name for test
		if( !GenerateUniqueTestName( hti_parent, str_test_name, &str_test_name) )
		{
			::CloseClipboard( );
			return;
		}
	}

	// set path with checking name for loaded test
	_bstr_t bstrt_parent_path, bstrt_loaded_path;
	sptrParentTI->GetPath( bstrt_parent_path.GetAddress( ) );
	bstrt_loaded_path = bstrt_parent_path + _T("\\") + str_test_name;
	sptrLoadedTI->SetPath( bstrt_loaded_path );

	// add loaded test to current test
	long lpos_loaded_test = 0;
	m_sptrTM->AddTest( pparent_item_info_ex->lpos, sptrLoadedTI, &lpos_loaded_test );
	if( !lpos_loaded_test )
	{
		::CloseClipboard( );
		return;
	}

	// load childs tests
	if( S_OK != m_sptrTM->LoadTests( pi_stream, lpos_loaded_test )  )
	{
		::CloseClipboard( );
		return;
	}

	// close clipboard
	::CloseClipboard( );

	// reset loaded test and it's childs tests
	ITestRunningDataPtr sptrLoadedTRD = sptrLoadedTI;
	if (sptrLoadedTRD != 0)
	{
		sptrLoadedTRD->SetTestSavedState( 0 );
		sptrLoadedTRD->SetTestRunTime( 0 );
		sptrLoadedTRD->SetRunTime( 0 );
		sptrLoadedTRD->SetSaveTime( 0 );
	}
	DoOperationWithChildTests( m_sptrTM, 
		toResetSavedState|toResetRunTime|toResetTestRunTime|toResetSaveTime|toResetLastError, lpos_loaded_test );

	// store loaded test's content and content of child tests
	sptrLoadedTI->StoreContent( );
	DoOperationWithChildTests( m_sptrTM, toStoreContent, lpos_loaded_test );

	// [vanek] SBT:1100 parse loaded test's conditions and conditions of child tests - 19.08.2004
	sptrLoadedTI->ParseCond( );
	DoOperationWithChildTests( m_sptrTM, toParseCond, lpos_loaded_test );

	// update test's tree
	DoInitTestsWindow( hti_parent, m_sptrTM, pparent_item_info_ex->lpos );
	m_TestsTree.UpdateWindow( );

	// set selection to pasted test
	m_TestsTree.Select( FindTestItemInTree( hti_parent, pparent_item_info_ex->lpos, lpos_loaded_test), TVGN_CARET );
}

void CEditTestDBDlg::OnBnClickedButtonStartConditions()
{
	EndDialog( IDC_BUTTON_START_CONDITIONS );
}

void CEditTestDBDlg::OnBnClickedButtonTestSequence()
{
	EndDialog( IDC_BUTTON_TEST_SEQUENCE );
}

void CEditTestDBDlg::OnBnClickedButtonFinalTestSequence()
{
	EndDialog( IDC_BUTTON_FINAL_TEST_SEQUENCE );
}

void CEditTestDBDlg::OnBnClickedButtonEditTestSequence()
{
	EndDialog( IDC_BUTTON_EDIT_TEST_SEQUENCE );
}

void CEditTestDBDlg::OnBnClickedButtonEditFinalTestSequence()
{
	EndDialog( IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE );
}

void CEditTestDBDlg::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	*pResult = 0;
	if( pTVDispInfo->item.pszText )
	{
		// find item with same text
		HTREEITEM hti_parent = m_TestsTree.GetParentItem( pTVDispInfo->item.hItem );
		HTREEITEM hti_child = FindItemByName( hti_parent, pTVDispInfo->item.pszText, FALSE);
		if( hti_child )
		{	// reset edit
			*pResult = 0;
			m_bprevent_ok_cancel = FALSE;
			return;
		}

		// get test item
		XItemInfoEx *pitem_info_ex = (XItemInfoEx *)pTVDispInfo->item.lParam;
		if( pitem_info_ex )	 
		{
			IUnknownPtr sptrTest;
			long lpos = pitem_info_ex->lpos;
			m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lpos, &sptrTest);
			ITestItemPtr sptrTI = sptrTest;
			if( sptrTI != 0 )
			{
				// rename test
				_bstr_t bstrt_test_name( pTVDispInfo->item.pszText );
				if( S_OK == sptrTI->Rename( bstrt_test_name ) )
				{
					// update child's paths
					DoOperationWithChildTests( m_sptrTM, toUpdatePath, pitem_info_ex->lpos );

					// [vanek] SBT: 811 - 11.03.2004
					m_TestsTree.SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText );
					m_TestsTree.SortChildren( hti_parent );
					*pResult = 1;
				}
				else
				{
					CString str_title( _T("") ), str_message( _T("") );
					str_title.LoadString( AfxGetInstanceHandle(), IDS_RENAME_TEST_TITLE );
					str_message.LoadString( AfxGetInstanceHandle(), IDS_ON_RENAME_TEST_ERROR );
					::MessageBox( GetSafeHwnd(), str_message, str_title, MB_OK | MB_ICONERROR );
					*pResult = 0;
				}
			}
		}
	}

	m_bprevent_ok_cancel = FALSE;
	m_TestsTree.SetFocus();
}
void CEditTestDBDlg::OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	m_bprevent_ok_cancel = TRUE;
	*pResult = 0;
}

void CEditTestDBDlg::OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);

	if( pTVKeyDown->wVKey == VK_F2 )
		OnRenameTest( m_TestsTree.GetSelectedItem() );
	else if( pTVKeyDown->wVKey == VK_DELETE )
		OnDeleteTest( m_TestsTree.GetSelectedItem() );
	else if( IsCTRLpressed() ) 
	{
		if( pTVKeyDown->wVKey == VK_INSERT || pTVKeyDown->wVKey == 'C' )
			OnCopyTest( m_TestsTree.GetSelectedItem() );
		else if( pTVKeyDown->wVKey == 'V' )
			OnPasteTest( m_TestsTree.GetSelectedItem() );
		else if( pTVKeyDown->wVKey == 'N' )
			OnAddNewTest( m_TestsTree.GetSelectedItem() );
	}
	else if( pTVKeyDown->wVKey == VK_INSERT  && IsSHIFTpressed() )
		OnPasteTest( m_TestsTree.GetSelectedItem() );

	*pResult = 0;
}

void CEditTestDBDlg::OnBnClickedButtonSaveEnv()
{
	if( m_sptrTM == 0 )
		return;
    
	HTREEITEM hti = 0;
	hti = m_TestsTree.GetSelectedItem( );
	if( !hti )
		return;

	XItemInfoEx *pitem_info_ex = 0;
	pitem_info_ex = (XItemInfoEx *)m_TestsTree.GetItemData( hti );
	if( !pitem_info_ex )	 
		return;

    IUnknownPtr sptr_unk_test;
	long lpos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lpos, &sptr_unk_test);

	ITestEnvironmentPtr sptr_te;
	sptr_te = sptr_unk_test;
	if( sptr_te == 0 )
		return;
    
	sptr_te->SaveShellState( );
	sptr_te->SaveSDC( );
	sptr_te->SaveKbdLayout( );

	ITestItemPtr sptr_ti = sptr_te;
	_update_test_info_ctrls( sptr_ti );
}

void CEditTestDBDlg::OnTvnGetInfoTipTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
	
	*pResult = 0;

	// [vanek] : отображение дополнительной информации в tooltip, если не нажат CTRL - 20.07.2004
	if(::GetAsyncKeyState( VK_CONTROL ) & 0x8000)
		return; 

	if( !pGetInfoTip->hItem )
		return;
	
	XItemInfoEx *pitem_info_ex = 0;
	pitem_info_ex = (XItemInfoEx *)pGetInfoTip->lParam;
	if (m_sptrTM == 0 || !pitem_info_ex )
		return;
	
	IUnknownPtr sptrTest;
	long lPos = pitem_info_ex->lpos;
	m_sptrTM->GetNextTest( pitem_info_ex->lparent_pos, &lPos, &sptrTest);
	ITestItemPtr sptrTI = sptrTest;
	if( sptrTI == 0 )
		return;

    _bstr_t bstrAddInfo;
	sptrTI->GetAdditionalInfo( bstrAddInfo.GetAddress( ) );
	if( bstrAddInfo.length( ) )
		_tcsncpy( pGetInfoTip->pszText, bstrAddInfo, min( pGetInfoTip->cchTextMax, (int)(bstrAddInfo.length( ) + 1) ) );
	
}

void CEditTestDBDlg::OnBnClickedButtonExecCond()
{
	EndDialog( IDC_BUTTON_EXEC_COND );
}

void CEditTestDBDlg::OnEnChangeEditExecCond()
{
	m_bChangeExecConds = TRUE;	
}

void CEditTestDBDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	resize_control( cx, cy );
}

void CEditTestDBDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}

void CEditTestDBDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);

	if( (pRect->right - pRect->left ) < ( m_rect_wnd.right - m_rect_wnd.left ) )
		pRect->right = pRect->left + ( m_rect_wnd.right - m_rect_wnd.left );

	if( (pRect->bottom - pRect->top ) < ( m_rect_wnd.bottom - m_rect_wnd.top ) )
		pRect->bottom = pRect->top + ( m_rect_wnd.bottom - m_rect_wnd.top );

	// TODO: Add your message handler code here
}
