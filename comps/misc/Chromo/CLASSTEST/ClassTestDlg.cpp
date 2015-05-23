// ClassTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClassTest.h"
#include "ClassTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClassTestDlg dialog

CClassTestDlg::CClassTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClassTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClassTestDlg)
	m_sScriptPath = _GetProfileString(_T("Paths"), _T("ScriptPathName"));
	m_sTechDir = _GetProfileString(_T("Paths"), _T("TeachDir"));
	m_sTestDir = _GetProfileString(_T("Paths"), _T("TestDir"));
	m_sCompareDir = _GetProfileString(_T("Paths"), _T("CompareDir"));
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClassTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassTestDlg)
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Text(pDX, IDC_E_COMPARE_DIR, m_sCompareDir);
	DDX_Text(pDX, IDC_E_TEACH_DIR, m_sTechDir);
	DDX_Text(pDX, IDC_E_TEST_DIR, m_sTestDir);
	DDX_Text(pDX, IDC_E_SCRIPT_PATH, m_sScriptPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClassTestDlg, CDialog)
	//{{AFX_MSG_MAP(CClassTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SCRIPT, OnScript)
	ON_BN_CLICKED(IDC_BROWSE_COMPARE_DIR, OnBrowseCompareDir)
	ON_BN_CLICKED(IDC_BROWSE_TEST_DIR, OnBrowseTestDir)
	ON_BN_CLICKED(IDC_TEACH_BROWSE, OnTeachBrowse)
	ON_BN_CLICKED(IDC_BROWSE_SCRIPT, OnBrowseScript)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTestDlg message handlers

BOOL CClassTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetDlgItemText(IDC_STATIC_CONFIGURATION, _GetApp()->m_sProfile);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClassTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClassTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CClassTestDlg::OnScript() 
{
	UpdateData();
	if (m_sScriptPath.IsEmpty())
	{
		m_sScriptPath = _AskForPath(FALSE, NULL, _T(".script"), _T("Scripts(*.script)|*.script|All files(*.*)|*.*||"));
		_SetProfileString(_T("Paths"), _T("ScriptPathName"), m_sScriptPath);
	}
	if (m_sTechDir.IsEmpty())
	{
		m_sTechDir = _AskForPath(TRUE, NULL);
		_SetProfileString(_T("Paths"), _T("TeachDir"), m_sTechDir);
	}
	if (m_sTestDir.IsEmpty())
	{
		m_sTestDir = _AskForPath(TRUE, NULL);
		_SetProfileString(_T("Paths"), _T("TestDir"), m_sTestDir);
	}
	if (m_sCompareDir.IsEmpty())
	{
		m_sCompareDir = _AskForPath(TRUE< NULL);
		_SetProfileString(_T("Paths"), _T("CompareDir"), m_sCompareDir);
	}
	UpdateData(FALSE);
	try
	{
		m_Log.ResetContent();
		m_Log.AddString(_T("generating script..."));
		CStdioFile file(m_sScriptPath,CFile::modeCreate|CFile::modeWrite|CFile::typeText);
		file.WriteString(_T("ActionManager.EmptyClassifier\n"));
		_MakeScriptAddLearn(file,m_sTechDir,&m_Log);
		file.WriteString(_T("ActionManager.TeachClassifier\n"));
		_MakeScriptTest(file,m_sTestDir,m_sCompareDir,&m_Log);
		CString s;
		s.Format(_T("%s generated"), m_sScriptPath);
		m_Log.AddString(s);
	}
	catch(CException *pe)
	{
		pe->ReportError();
		pe->Delete();
	}
}

void CClassTestDlg::OnBrowseScript() 
{
	UpdateData();
	m_sScriptPath = _AskForPath(FALSE, m_sScriptPath, _T(".script"), _T("Scripts(*.script)|*.script|All files(*.*)|*.*||"));
	UpdateData(FALSE);
	_SetProfileString(_T("Paths"), _T("ScriptPathName"), m_sScriptPath);
}

void CClassTestDlg::OnTeachBrowse() 
{
	UpdateData();
	m_sTechDir = _AskForPath(TRUE, m_sTechDir);
	UpdateData(FALSE);
	_SetProfileString(_T("Paths"), _T("TeachDir"), m_sTechDir);
}

void CClassTestDlg::OnBrowseTestDir() 
{
	UpdateData();
	m_sTestDir = _AskForPath(TRUE, m_sTestDir);
	UpdateData(FALSE);
	_SetProfileString(_T("Paths"), _T("TestDir"), m_sTestDir);
}

void CClassTestDlg::OnBrowseCompareDir() 
{
	UpdateData();
	m_sCompareDir = _AskForPath(TRUE, m_sCompareDir);
	UpdateData(FALSE);
	_SetProfileString(_T("Paths"), _T("CompareDir"), m_sCompareDir);
}

bool CClassTestDlg::DoLoadFile()
{
	CString s = AfxGetApp()->GetProfileString(_T("Settings"), _T("ProjectFile"), NULL);
	static char BASED_CODE szFilter[] = _T(".ini files(*.ini)|*.ini|All files(*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("ini"), 	s, OFN_NOREADONLYRETURN|OFN_PATHMUSTEXIST, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFile = dlg.GetPathName();
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("ProjectFile"), sFile);
		SetDlgItemText(IDC_STATIC_CONFIGURATION, sFile);
		_GetApp()->m_sProfile = sFile;
		return true;
	}
	return false;
}


void CClassTestDlg::OnLoad() 
{
	if (DoLoadFile())
	{
		UpdateData();
		m_sScriptPath = _GetProfileString(_T("Paths"), _T("ScriptPathName"));
		m_sTechDir = _GetProfileString(_T("Paths"), _T("TeachDir"));
		m_sTestDir = _GetProfileString(_T("Paths"), _T("TestDir"));
		m_sCompareDir = _GetProfileString(_T("Paths"), _T("CompareDir"));
		UpdateData(FALSE);
	}
}

void CClassTestDlg::OnSave() 
{
	if (DoLoadFile())
	{
		UpdateData();
		_SetProfileString(_T("Paths"), _T("ScriptPathName"), m_sScriptPath);
		_SetProfileString(_T("Paths"), _T("TeachDir"), m_sTechDir);
		_SetProfileString(_T("Paths"), _T("TestDir"), m_sTestDir);
		_SetProfileString(_T("Paths"), _T("CompareDir"), m_sCompareDir);
	}
}
