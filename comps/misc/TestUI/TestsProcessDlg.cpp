// TestsProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "TestsProcessDlg.h"
#include "TestingResults.h"
#include "TestUIUtils.h"

#include <commctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL DoIfAnyTestAvailable(ITestManager *pTestManager, long lParentPos)
{
	long lPos = 0;
	pTestManager->GetFirstTestPos(lParentPos, &lPos);
	while (lPos)
	{
		long lPosSaved = lPos;
		IUnknownPtr punkTest;
		pTestManager->GetNextTest(lParentPos, &lPos, &punkTest);
		ITestRunningDataPtr sptrTRD = punkTest;
		if (sptrTRD == 0)
			continue;
		long bCheck = 0;
		sptrTRD->GetTestChecked(&bCheck);
		if (bCheck) return TRUE;
		BOOL bAnyChld = DoIfAnyTestAvailable(pTestManager, lPosSaved);
		if (bAnyChld) return TRUE;
	}
	return FALSE;
}

static BOOL IfAnyTestAvailable()
{
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestManagerPtr sptrTM(punkTM);
	return DoIfAnyTestAvailable(sptrTM, NULL);
}

// CTestsProcessDlg dialog

IMPLEMENT_DYNCREATE(CTestsProcessDlg, CDialog)

CTestsProcessDlg::CTestsProcessDlg() : CDialog(CTestsProcessDlg::IDD, NULL)
{
	m_sName = _T("TestingPanel");
	m_sUserName.LoadString(IDS_TESTING_PANEL);
	m_bRun = false;
	_OleLockApp( this );
	Register();
}

CTestsProcessDlg::~CTestsProcessDlg()
{
	UnRegister();
	_OleUnlockApp( this );
}

void CTestsProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEST_PROCESS, m_TestList);
}

BEGIN_INTERFACE_MAP(CTestsProcessDlg, CDialog)
	INTERFACE_PART(CTestsProcessDlg, IID_IWindow, Wnd)
	INTERFACE_PART(CTestsProcessDlg, IID_IDockableWindow, Dock)
	INTERFACE_PART(CTestsProcessDlg, IID_INamedObject2, Name)
	INTERFACE_PART(CTestsProcessDlg, IID_IHelpInfo, Help)
	INTERFACE_PART(CTestsProcessDlg, IID_IEventListener, EvList)
END_INTERFACE_MAP()

// {3AC99CF3-50B9-420a-9EE1-C3D64B679327}
GUARD_IMPLEMENT_OLECREATE(CTestsProcessDlg, "TestUI.TestProcessDlg",
0x3ac99cf3, 0x50b9, 0x420a, 0x9e, 0xe1, 0xc3, 0xd6, 0x4b, 0x67, 0x93, 0x27);

CWnd *CTestsProcessDlg::GetWindow()
{
	return this;
}

bool CTestsProcessDlg::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	Create(IDD, pparent);
	CString s;
	s.LoadString(IDS_TESTING_PANEL);
	SetWindowText(s);
	CRect rect = NORECT;
	GetClientRect(&rect);
	m_size = CSize(rect.Width(), rect.Height());
	m_bFixed = true;
	_trace_file( "test_sys.log", "[~] Test process window is created" );
	return true;
}

BEGIN_MESSAGE_MAP(CTestsProcessDlg, CDialog)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TEST_PROCESS, OnNMCustomdrawListTestProcess)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_RESULTS, OnBnClickedButtonViewResults)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnBnClickedButtonPause)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_LIST_TEST_PROCESS, OnLvnGetInfoTipListTestProcess)
END_MESSAGE_MAP()


// CTestsProcessDlg message handlers

void CTestsProcessDlg::PostNcDestroy()
{
	delete this;
	__super::PostNcDestroy();
}

void CTestsProcessDlg::OnNotify(const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if (strcmp(pszEvent,szEventTestsChangeState)==0)
	{
		BOOL bAnyAvail = IfAnyTestAvailable();
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_STOP), bAnyAvail);
	}
	else if (strcmp(pszEvent,szEventBeginTestProcess)==0)
	{
		_trace_file_clear( "testing.log" );

		_trace_file( "testing.log", "[~] test process is started" );
		_trace_file( "test_sys.log", "[~] test process is started" );

		ClearDialog();
		m_bRun = true;
		SetRunStopButton();
	}
	else if (strcmp(pszEvent,szEventEndTestProcess)==0)
	{
		_trace_file( "testing.log", "[~] test process is finished" );
		_trace_file( "test_sys.log", "[~] test process is finished" );

		m_bRun = false;
		SetRunStopButton();
		IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
		ITestProcessPtr sptrTP(punkTM);
		if (sptrTP != 0)
		{
			long lRunningState;
			sptrTP->GetRunningState(&lRunningState);
			if (lRunningState && m_arrTests.GetSize()>0)
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), TRUE);
			else
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), FALSE);
		}
	}
	else
	{
		bool bBefore = strcmp(pszEvent,szEventBeforeTestExecute)==0;
		bool bAfter = strcmp(pszEvent,szEventAfterTestExecute)==0;
		if (bBefore || bAfter)
		{
			DWORD *pdw = (DWORD*)pdata;
			int iCond = pdw ? HIBYTE(*pdw) : 0;
			ITestItemPtr sptrTI(punkHit);
			CString sTestName = _TestFullName(sptrTI,iCond);

			_bstr_t bstr_info;
			sptrTI->GetAdditionalInfo( bstr_info.GetAddress() );
			CString str_add_info = (char *)bstr_info;
			
			if( bBefore && sptrTI != 0 )
			{
				_trace_file( "testing.log", "[+] test is started: %s, additional info: %s", sTestName, str_add_info );
				_trace_file( "test_sys.log", "[+] test is started: %s, additional info: %s", sTestName, str_add_info );
			}
/*
			else if( bAfter )
			{
				_trace_file( "testing.log", "[+] test is finished: %s", sTestName );
				_trace_file( "test_sys.log", "[+] test is finished: %s", sTestName );
			}
*/

			if (bBefore)
				AddOrSetStatus(sTestName, str_add_info,iCond,false,false);
			else
			{
				if (LOBYTE(*pdw)==0)
				{
					AddOrSetStatus(sTestName,str_add_info, iCond,true,false); // failure
					_trace_file( "testing.log", "[+] test is finished: %s (FAILED)", sTestName );
				}
				else
				{
					AddOrSetStatus(sTestName,str_add_info, iCond,true,true); // success
					_trace_file( "testing.log", "[+] test is finished: %s (SUCCESSED)", sTestName );
				}

				int iSuccess = 0, iFailed = 0;
				for (int i = 0; i < m_arrTests.GetSize(); i++)
				{
					if (m_arrTests.ElementAt(i).m_bSuccess)
						iSuccess++;
					else
						iFailed++;
				}
				CString s;
				s.Format(IDS_STATISTICS, iSuccess, iFailed);
				SetDlgItemText(IDC_STATISTICS, s);
			}

			{
				IApplicationPtr	ptrA( GetAppUnknown() );
				ptrA->ProcessMessage();
			}
		}
	}
}

void CTestsProcessDlg::ClearDialog()
{
	m_TestList.DeleteAllItems();
	m_arrTests.RemoveAll();
	SetDlgItemText(IDC_STATISTICS, 0);
}

void CTestsProcessDlg::SetRunStopButton()
{
	CBitmap bmp;
	bmp.LoadBitmap(m_bRun?IDB_BITMAP_STOP:IDB_BITMAP_RUN);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP))->SetBitmap(bmp);
	m_bmpStopRun.DeleteObject();
	m_bmpStopRun.Attach(bmp.Detach());

	((CButton*)GetDlgItem(IDC_BUTTON_PAUSE))->EnableWindow( m_bRun );
}

void CTestsProcessDlg::AddOrSetStatus(LPCTSTR lpstrName, LPCTSTR lpstr_info, int nCond, bool bPassed, bool bSuccess)
{
	CTestInfoRec *p = bPassed?m_arrTests.FindRec(lpstrName,nCond):NULL; // if bPassed==falsethen called first time
	if (p)
	{
		p->m_bPassed = bPassed;
		p->m_bSuccess = bSuccess;
	}
	else
	{
		int nItem = m_arrTests.GetSize();
		m_arrTests.Add(CTestInfoRec(lpstrName,lpstr_info,nCond,bPassed,bSuccess,0));
		m_TestList.InsertItem(nItem, lpstrName);
		
		m_TestList.EnsureVisible(nItem,false);
	}
	m_TestList.Invalidate();
}


BOOL CTestsProcessDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CString sName,sState;
	sName.LoadString(IDS_TEST_NAME);
	m_TestList.InsertColumn(0, sName, LVCFMT_LEFT, 400, 1);
	sState.LoadString(IDS_TESTING_STATE);
	m_TestList.InsertColumn(1, sState, LVCFMT_LEFT, 100, 2);
	SetRunStopButton();
	HBITMAP hbm = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_VIEW_RESULTS));
	((CButton*)GetDlgItem(IDC_BUTTON_VIEW_RESULTS))->SetBitmap(hbm);

	HBITMAP hbm2 = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_PAUSE));
	((CButton*)GetDlgItem(IDC_BUTTON_PAUSE))->SetBitmap(hbm2);

	BOOL bAnyAvail = IfAnyTestAvailable();
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_STOP), bAnyAvail);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), FALSE);

	ListView_SetExtendedListViewStyle( m_TestList.GetSafeHwnd(), LVS_EX_INFOTIP );

	_trace_file( "test_sys.log", "[~] Test process window is init" );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestsProcessDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_LIST_TEST_PROCESS) 
	{
		int nVisItem1 = m_TestList.GetTopIndex();
		int nLastVisItem = nVisItem1 + m_TestList.GetCountPerPage();
		CDC *pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
		COLORREF crBackground = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHT:COLOR_WINDOW);
		COLORREF crText = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT);
		pdc->SetBkColor(crBackground);
		pdc->SetTextColor(crText);
		pdc->FillSolidRect(&lpDrawItemStruct->rcItem, crBackground);
		int iItem = lpDrawItemStruct->itemID;
		CRect rcItem,rcSubItem1;
		if (m_TestList.GetItemRect(iItem, rcItem, LVIR_BOUNDS))
		{
			CString s = m_TestList.GetItemText(iItem, 0);
			pdc->DrawText(s, rcItem, 0);
		}
		if (m_TestList.GetSubItemRect(iItem,1,LVIR_BOUNDS,rcSubItem1))
		{
			rcSubItem1.InflateRect(-1,-1,-1,-1);
			CTestInfoRec *p = &m_arrTests.ElementAt(iItem);
			if (!p->m_bPassed)
			{
				CBrush br(RGB(0,0,0));
				pdc->FrameRect(rcSubItem1, &br);
			}
			else
				pdc->FillSolidRect(rcSubItem1, p->m_bSuccess?RGB(0,255,0):RGB(255,0,0));
		}
	}
	else
		__super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}



void CTestsProcessDlg::OnBnClickedButtonStop()
{
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestProcessPtr sptrTP(punkTM);
	if (sptrTP == 0) return;
	if (m_bRun)
	{
		sptrTP->StopTesting();
		m_bRun = false;
		SetRunStopButton();
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), TRUE );
	}
	else
	{
		long lRunningState;
		sptrTP->GetRunningState(&lRunningState);
		if (lRunningState == 0)
			sptrTP->RunTestSave();
		else
			sptrTP->RunTestCompare();
	}
}

void CTestsProcessDlg::OnBnClickedButtonViewResults()
{
	CTestingResults dlg;
	dlg.DoModal();
}

void CTestsProcessDlg::OnBnClickedButtonPause()
{
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestProcessPtr sptrTP(punkTM);
	if (sptrTP == 0) return;
	
	static bool bPaused = false; 

	if ( m_bRun )
	{
		bPaused = !bPaused;

		long lRunningState;
		sptrTP->GetRunningState(&lRunningState);
		if (lRunningState && m_arrTests.GetSize()>0)
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), TRUE);
		else
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_VIEW_RESULTS), FALSE);

		sptrTP->PauseTesting( bPaused );
	}
}

void CTestsProcessDlg::OnLvnGetInfoTipListTestProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	
	CTestInfoRec *p = &m_arrTests.ElementAt( pGetInfoTip->iItem );

	if( p )
		_tcsncpy( pGetInfoTip->pszText, p->m_str_info, pGetInfoTip->cchTextMax );

	*pResult = 0;
}
