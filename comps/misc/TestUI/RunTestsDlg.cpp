// RunTestsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "RunTestsDlg.h"
#include "TestUIUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct CTestInfo
{
	CItemInfo* m_pItemInfo;
	LPOS m_lPosSaved;
	long m_lError;
};

class CTestsList : public CTypedPtrList<CPtrList,CTestInfo*>
{
public:
	~CTestsList()
	{
		while (!IsEmpty())
			delete RemoveHead();
	}
	void Add(CTestInfo *pNew)
	{
		CString sName = pNew->m_pItemInfo->GetSubItem(1);
		bool bInserted = false;
		POSITION pos = GetHeadPosition();
		while (pos)
		{
			POSITION pos1 = pos;
			CTestInfo *pTest = GetNext(pos);
			CString sNameT = pTest->m_pItemInfo->GetSubItem(1);
			if (sName.Compare(sNameT) < 0)
			{
				InsertBefore(pos1, pNew);
				bInserted = true;
				break;
			}
		}
		if (!bInserted)
			AddTail(pNew);
	}
};

static CTestInfo* MakeItemInfo(ITestManager *pTestManager, IUnknown *punkTest, LPOS lPosSaved)
{
	ITestItemPtr sptrTI = punkTest;
	ITestRunningDataPtr sptrTRD = punkTest;
	if (sptrTI == 0 || sptrTRD == 0)
		return NULL;
	CItemInfo* lp = new CItemInfo();
	// Set item icon
	LPOS lChildrenPos = 0;
	pTestManager->GetFirstTestPos(lPosSaved, &lChildrenPos);
	lp->SetImage(lChildrenPos==0?2:1);
	// Obtain and set test values name:
	//// Name
	CString sName = _TestName(sptrTI);
	///// Date
	COleDateTime dtRun,dtSave;
	if (FAILED(sptrTRD->GetRunTime(&dtRun.m_dt)))
		dtRun.SetStatus(COleDateTime::null);
	if (FAILED(sptrTRD->GetSaveTime(&dtSave.m_dt)))
		dtSave.SetStatus(COleDateTime::null);
	///// Is checked, run and error?
	long bCheck = 0, bSaved = 0, lError = 0;
	sptrTRD->GetTestChecked(&bCheck);
	sptrTRD->GetTestSavedState(&bSaved);
	sptrTRD->GetTestErrorState(&lError);
	//// Commentary?
	_bstr_t bstrAddInfo;
	sptrTI->GetAdditionalInfo(bstrAddInfo.GetAddress());
	COLORREF clrText = lError==0?-1:RGB(255,0,0);
	lp->AddSubItemText(_T(""), clrText, bSaved?3:4);
	lp->AddSubItemText(sName, clrText, lChildrenPos==0?2:1);
	lp->AddSubItemText(dtSave.Format(), clrText);
	lp->AddSubItemText(dtRun.Format(), clrText);
	lp->AddSubItemText((LPCTSTR)bstrAddInfo, clrText);
	lp->SetCheck(bCheck);
	CTestInfo *pti = new CTestInfo;
	pti->m_pItemInfo = lp;
	pti->m_lPosSaved = lPosSaved;
	pti->m_lError = lError;
	return pti;
}

// CRunTestsDlg dialog

IMPLEMENT_DYNAMIC(CRunTestsDlg, CDialog)
CRunTestsDlg::CRunTestsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRunTestsDlg::IDD, pParent)
{
}

CRunTestsDlg::~CRunTestsDlg()
{
}

void CRunTestsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TESTS, m_TestsList);
}


BEGIN_MESSAGE_MAP(CRunTestsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_TEST, OnBnClickedButtonSaveTest)
	ON_BN_CLICKED(IDC_BUTTON_TEST_SYSTEM, OnBnClickedButtonTestSystem)
END_MESSAGE_MAP()


void CRunTestsDlg::InitializeList()
{
	m_TestsList.CreateImageList(IDB_FOLDERS_RUNTESTS);
	m_TestsList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_TestsList.SetTreeSubItem(2);
	m_TestsList.SetFlags(LVS_EX_EXPAND_CHECK);
	int n = 0;
	InsertColumn(n++, 0);
	InsertColumn(n++, 16);
	InsertColumn(n++, 200, IDS_TEST_NAME);
	InsertColumn(n++, 150, IDS_TEST_SAVING_DATE);
	InsertColumn(n++, 150, IDS_TEST_LAST_RUN_DATE);
	InsertColumn(n++, 200, IDS_COMMENTARY);
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestManagerPtr sptrTM(punkTM);
	if (sptrTM != 0)
		DoAddListItem(sptrTM, 0, NULL);
	m_TestsList.SetSite(this);
	m_TestsList.SetExtendedStyle(LVS_EX_HEADERDRAGDROP|m_TestsList.GetExtendedStyle());
	OnCheckStateChanged(NULL);
}

void CRunTestsDlg::InsertColumn(int n, int cx, int id)
{
	CString s;
	LV_COLUMN   lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = cx;
	if (id != -1)
	{
		s.LoadString(id);
		lvColumn.pszText = s.GetBuffer(-1);
		s.ReleaseBuffer();
	}
	else
		lvColumn.pszText = _T("");
	m_TestsList.InsertColumn(n,&lvColumn);
}

void CRunTestsDlg::DoAddListItem(ITestManager *pTestManager, long lParentPos,
	CSuperGridCtrl::CTreeItem *pBase)
{
	CTestsList List;
	LPOS lPos = 0;
	pTestManager->GetFirstTestPos(lParentPos, &lPos);
	while (lPos)
	{
		LPOS lPosSaved = lPos;
		IUnknownPtr punkTest;
		pTestManager->GetNextTest(lParentPos, &lPos, &punkTest);
		CTestInfo* pti = MakeItemInfo(pTestManager, punkTest, lPosSaved);
		if (!pti) continue;
		ITestRunningDataPtr sptrTRD = punkTest;
		List.Add(pti);
		m_mapItems.SetAt(pti->m_pItemInfo,(ITestRunningData*)sptrTRD);
	}
	POSITION pos1 = List.GetHeadPosition();
	while (pos1)
	{
		CTestInfo* pti = List.GetNext(pos1);
		CSuperGridCtrl::CTreeItem *pItem;
		if (pBase == NULL)
			pItem = m_TestsList.InsertRootItem(pti->m_pItemInfo);
		else
			pItem = m_TestsList.InsertItem(pBase, pti->m_pItemInfo);
		if (pti->m_lError)
		{
			for (CSuperGridCtrl::CTreeItem *p = m_TestsList.GetParentItem(pItem); p; p = m_TestsList.GetParentItem(p))
				m_TestsList.GetData(p)->SetSubItemImage(1,5);
		}
		DoAddListItem(pTestManager,pti->m_lPosSaved,pItem);
	}
}

void CRunTestsDlg::OnCheckStateChanged(CItemInfo *pInfo)
{
	BOOL bAnyCheck = FALSE;
	BOOL bAnyUnSavedCheck = FALSE;
	POSITION pos = m_mapItems.GetStartPosition();
	while (pos)
	{
		CItemInfo* pItemInfo;
		ITestRunningData* pTestRunningData;
		m_mapItems.GetNextAssoc(pos,pItemInfo,pTestRunningData);
		long lCheck = pItemInfo->GetCheck();
		if (lCheck)
		{
			bAnyCheck = TRUE;
			long bSaved;
			pTestRunningData->GetTestSavedState(&bSaved);
			if (bSaved == 0)
			{
				bAnyUnSavedCheck = TRUE;
				break;
			}
		}
		pTestRunningData->SetTestChecked(lCheck);
	}
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_TEST_SYSTEM), bAnyCheck&&!bAnyUnSavedCheck);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_SAVE_TEST), bAnyCheck);
}

void CRunTestsDlg::OnSelectionChanged(CItemInfo *pInfo)
{
}


void CRunTestsDlg::SaveAllItemsStates()
{
	POSITION pos = m_mapItems.GetStartPosition();
	while (pos)
	{
		CItemInfo* pItemInfo;
		ITestRunningData* pTestRunningData;
		m_mapItems.GetNextAssoc(pos,pItemInfo,pTestRunningData);
		long lCheck = pItemInfo->GetCheck();
		pTestRunningData->SetTestChecked(lCheck);
	}
	FireEvent(GetAppUnknown(), szEventTestsChangeState, NULL, NULL, NULL, 0);
}


// CRunTestsDlg message handlers

BOOL CRunTestsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitializeList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRunTestsDlg::OnBnClickedButtonSaveTest()
{
	SaveAllItemsStates();
	EndDialog(IDC_BUTTON_SAVE_TEST);
}

void CRunTestsDlg::OnBnClickedButtonTestSystem()
{
	SaveAllItemsStates();
	EndDialog(IDC_BUTTON_TEST_SYSTEM);
}
