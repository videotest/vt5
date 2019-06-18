// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Register.h"
#include "RegisterDlg.h"
#include "DirDlg.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void WriteLogLine( const char *lpszFormat, ... );

static void AddWithCheck(CString & str, CStringArray & sa)
{
	if (str.IsEmpty())
		return;

	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (sa[i] == str)
			return;
	}
	sa.Add(str);
}

bool RegisterFile(LPCTSTR szFile);
bool UnregisterFile(LPCTSTR szFile);


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

CRegisterDlg::CRegisterDlg(CRegisterVersion * pData, CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
{

	m_pData = pData;

	//{{AFX_DATA_INIT(CRegisterDlg)
	m_strAppName = _T("");
	m_strPath = _T("");
	m_strSuffix = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Control(pDX, IDC_OUTPUT, m_Output);
	DDX_Control(pDX, IDC_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_FILES, m_listCtrl);
	DDX_Text(pDX, IDC_PROGNAME, m_strAppName);
	DDX_Text(pDX, IDC_PATH, m_strPath);
	DDX_Text(pDX, IDC_SUFFIX, m_strSuffix);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REGISTER, OnRegister)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_UNREGISTER, OnUnregister)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_FIND_APP, OnFindApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers

BOOL CRegisterDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	ASSERT(m_pData);

	m_strAppName = m_pData->strProgramName;
	m_strPath = m_pData->strPath;
	m_strSuffix = m_pData->strSuffix;

	m_listCtrl.ResetContent();
	for (int i = 0; i < m_pData->arrFiles.GetSize(); i++)
		m_listCtrl.InsertString(i, m_pData->arrFiles[i]);

	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegisterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegisterDlg::OnPaint() 
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
HCURSOR CRegisterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


static char szFilter[] = "All Files (*.*)|*.*||";

void CRegisterDlg::OnAdd() 
{
	CFileDialog fd(true, 0, 0, OFN_ALLOWMULTISELECT, szFilter, this, _FILE_OPEN_SIZE_ );
	if (fd.DoModal() != IDOK)
		return;

	CStringArray sa;
	POSITION pos = fd.GetStartPosition();
	while (pos)
		sa.Add(fd.GetNextPathName(pos));

	for (int i = 0; i < sa.GetSize(); i++)
		AddWithCheck(sa[i], m_pData->arrFiles);

	m_listCtrl.ResetContent();
	for (i = 0; i < m_pData->arrFiles.GetSize(); i++)
		m_listCtrl.InsertString(i, m_pData->arrFiles[i]);
}

void CRegisterDlg::OnRemove() 
{
	int nIndex = m_listCtrl.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CString str;
	m_listCtrl.GetText(nIndex, str);
	m_listCtrl.DeleteString(nIndex);

	if (str == m_pData->arrFiles[nIndex])
		m_pData->arrFiles.RemoveAt(nIndex);
	else
	{
		for (int i = 0; i < m_pData->arrFiles.GetSize(); i++)
		{
			if (m_pData->arrFiles[i] == str)
			{
				m_pData->arrFiles.RemoveAt(i);
				break;
			}
		}
	}
}

void CRegisterDlg::OnRegister() 
{
	UpdateData(true);
	m_pData->strProgramName = m_strAppName;
	m_pData->strPath = m_strPath;
	m_pData->strSuffix = m_strSuffix;

	RegisterFiles(true);
}

LRESULT CRegisterDlg::OnFindApp(WPARAM, LPARAM)
{

	BringWindowToTop();
	SetForegroundWindow();


	int nCmdShow = SW_RESTORE;
	WINDOWPLACEMENT wpl;
	if (GetWindowPlacement(&wpl))
	{
		switch (wpl.showCmd)
		{
		case SW_HIDE:
		case SW_MINIMIZE:
		case SW_SHOWMINIMIZED:
		case SW_SHOWMINNOACTIVE:
		case SW_RESTORE:
		case SW_SHOW:
		case SW_SHOWNORMAL:
		case SW_SHOWNOACTIVATE:
		case SW_SHOWNA:
			nCmdShow = SW_RESTORE;
			break;
			
		case SW_SHOWMAXIMIZED:
			nCmdShow = SW_SHOWMAXIMIZED;
			break;

		}
	}
	ShowWindow(nCmdShow);
	SetActiveWindow();
	return TRUE;
}



void CRegisterDlg::OnBrowse() 
{
	CString strPath(m_strPath);
	CEdit * pEdit = (CEdit*)GetDlgItem(IDC_PATH);
	if (pEdit)
		pEdit->GetWindowText(strPath);
		
	CDirDialog	dlg;
	dlg.m_InitDir = strPath;
	dlg.m_Title = "Choose directory";
	if (dlg.DoBrowse())
	{
		m_strPath = dlg.m_Path;
		if (pEdit)
			pEdit->SetWindowText(m_strPath);
	}
}

char	g_szTaskFileName[MAX_PATH] = "";

void CRegisterDlg::RegisterFiles(bool bRegister)
{
	CWaitCursor wait;

	ASSERT (m_pData);
	ASSERT (!m_pData->strPath.IsEmpty());
	ASSERT (!m_pData->strProgramName.IsEmpty());
	ASSERT (!m_pData->strSuffix.IsEmpty());

	CRegisterApp * pApp = (CRegisterApp *)AfxGetApp();
	if (pApp)
		pApp->SaveRegisterData(*m_pData);

	CString strPath = m_pData->strPath;
	if (strPath.GetLength() && strPath[strPath.GetLength() - 1] != '\\')
		strPath += "\\";
	strPath += "comps";

	// Create list of files needed to register
	CStringList list;

	if( strlen( g_szTaskFileName ) )
	{
	try
	{
		char	sz[_MAX_PATH];
		::GetModuleFileName( 0, sz, _MAX_PATH );
		::GetLongPathName( sz, sz, _MAX_PATH );

		char	*p = strrchr( sz, '\\' );
		ASSERT( p );

		strcpy( p, "\\" );
		strcat( p, g_szTaskFileName );

		CStdioFile	file( sz, CFile::modeRead|CFile::typeText );
		CString	str;
		
		while( file.ReadString( str ) )
		{
			if( str.GetLength() && str[0] != ';' )
				list.AddTail( str );
		}
	}
	catch(CException *pe)
	{
		pe->ReportError();
		pe->Delete();
	}
	}

	else
	{
		CFileFind finder;
		BOOL bWorking = finder.FindFile(strPath + _T("\\*.dll"));

		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory() || finder.IsDots())
				continue;

			list.AddTail(finder.GetFilePath());
		}

		bWorking = finder.FindFile(strPath + _T("\\*.ocx"));
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory() || finder.IsDots())
				continue;

			list.AddTail(finder.GetFilePath());
		}
		if (!list.GetCount())
			return;
	}

	// set app name	
	GuardSetAppName(m_pData->strProgramName);
	// set app suffix
	GuardSetSuffix(m_pData->strSuffix);
	
	// set valiables
	CString strEnv;
	// set env var for this app
	BOOL rc = GetEnvironmentVariable(_T("PATH"), strEnv.GetBuffer(8*1024), 8*1024);
	strEnv.ReleaseBuffer();

	if (strEnv.GetLength() && strEnv[strEnv.GetLength() - 1] != _T(';'))
		strEnv += _T(';');

	CString strRootPath = strPath + ";";
	if (strEnv.Find(strRootPath) == -1)
		strEnv += strRootPath;

	// set path
	SetEnvironmentVariable(_T("PATH"), strEnv); 

	// load preloaded files 
	for (int i = 0; i < m_pData->arrFiles.GetSize(); i++)
		LoadLibrary(m_pData->arrFiles[i]);

	CString str_fail;

	CString strOut;
	strOut.Format("%s \"%s\"", bRegister ? "Registering" : "Unregistering", m_pData->strProgramName);
	m_Output.AddString(strOut);

	// for all file in list
	for (POSITION pos = list.GetHeadPosition(); pos != NULL; )
	{
		CString str(list.GetNext(pos));
		bool bRet = bRegister ? RegisterFile(str) : UnregisterFile(str);
		if( !bRet )
		{
			str_fail += str;
			str_fail += "\r";
			str_fail += "\n";
		}
		str += bRet ? " - successful" : " - failure.";
		m_Output.SetCurSel( m_Output.AddString(str) );
		m_Output.UpdateWindow();

		WriteLogLine( str );

		MSG		msg;
		while( PeekMessage( &msg, GetSafeHwnd(), 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_PAINT || msg.message == WM_NCPAINT )
			{
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
		}
	}

	if( !str_fail.IsEmpty() )
	{
		CString str_error = "Register fail:\r\n";
		str_error += str_fail;
		AfxMessageBox( str_error, MB_ICONERROR );
	}
}

bool RegisterFile(LPCTSTR szFile)
{
	// load it 
	HMODULE hModule = LoadLibrary(szFile);

	if( !hModule )
	{
		HRESULT hr = ::GetLastError();
		CString str_error;

		LPVOID lpMsgBuf = 0;
		if( FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL ))
		{
		
			str_error = (LPCTSTR)lpMsgBuf;
			LocalFree( lpMsgBuf );
		}

		WriteLogLine( "Error loading dll '%s'.Error:%s (%x)", szFile, (LPCSTR)str_error, hr );
	}
	
	

	if (!hModule)
		return false;

	typedef HRESULT (STDAPICALLTYPE *PFNRS)();
	// and call DllRegisterServer
	PFNRS fn = (PFNRS)GetProcAddress(hModule, _T("DllRegisterServer"));
	if (!fn)
	{
		::FreeLibrary( hModule );
		return false;
	}

	bool bres = SUCCEEDED(fn());
	::FreeLibrary( hModule );

	return bres;
}

bool UnregisterFile(LPCTSTR szFile)
{
	// load it 
	HMODULE hModule = LoadLibrary(szFile);
	if (!hModule)
		return false;

	typedef HRESULT (STDAPICALLTYPE *PFNRS)();
	// and call DllRegisterServer
	PFNRS fn = (PFNRS)GetProcAddress(hModule, _T("DllUnregisterServer"));
	if (!fn)
	{
		::FreeLibrary( hModule );
		return false;
	}

	bool bres = SUCCEEDED(fn());
	::FreeLibrary( hModule );

	return bres;
}

void CRegisterDlg::OnUnregister() 
{
	UpdateData(true);
	m_pData->strProgramName = m_strAppName;
	m_pData->strPath = m_strPath;
	m_pData->strSuffix = m_strSuffix;

	RegisterFiles(false);
}

void CRegisterDlg::OnExit() 
{
	CDialog::OnOK();
}
