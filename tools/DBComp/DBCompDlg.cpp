// DBCompDlg.cpp : implementation file
//


#include "stdafx.h"
#include "DBComp.h"
#include "DBCompDlg.h"

#include <afxdao.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBCompDlg dialog

CDBCompDlg::CDBCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBCompDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBCompDlg)
	m_strDBPath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDBCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBCompDlg)
	DDX_Control(pDX, IDC_STATIC1, m_Status);
	DDX_Control(pDX, IDC_EDIT1, m_wndDBPath);
	DDX_Control(pDX, IDC_START_COMPACTING, m_btnSize);
	DDX_Text(pDX, IDC_EDIT1, m_strDBPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDBCompDlg, CDialog)
	//{{AFX_MSG_MAP(CDBCompDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FILE_SELECT, OnFileSelect)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit)
	ON_BN_CLICKED(IDC_START_COMPACTING, OnStartCompacting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBCompDlg message handlers

BOOL CDBCompDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CWinApp *pApp = AfxGetApp();
	m_strDBPath = pApp->GetProfileString( "General", "Last", "" );
	m_wndDBPath.SetWindowText( m_strDBPath );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDBCompDlg::OnPaint() 
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
HCURSOR CDBCompDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDBCompDlg::OnFileSelect() 
{
	CFileDialog mDlg( true, ".mbd" ,"",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Database (*.mdb)|*.mdb|All files (*.*)|*.*||");

	if( mDlg.DoModal() == IDOK )
	{
		m_strDBPath = mDlg.GetPathName();
		m_wndDBPath.SetWindowText( m_strDBPath );
	}
	
}

void CDBCompDlg::OnChangeEdit() 
{
	m_wndDBPath.GetWindowText( m_strDBPath );
	BOOL bEnable = !m_strDBPath.IsEmpty();
	m_btnSize.EnableWindow( bEnable );
	m_Status.SetWindowText( "" );
}

void CDBCompDlg::OnStartCompacting() 
{
	m_Status.SetWindowText( "" );
	m_wndDBPath.GetWindowText( m_strDBPath );
	CString tmp = "C:\\temp.mdb";

	COwnDaoWorkspace cd;

	cd.CompactDatabase(m_strDBPath, tmp,
		dbLangCyrillic, dbVersion40, "");

	CopyFile( tmp, m_strDBPath, false );
	DeleteFile( tmp );

	tmp.LoadString( IDS_READY );
	m_Status.SetWindowText( tmp );
}

BOOL CDBCompDlg::DestroyWindow() 
{
	CWinApp *pApp = AfxGetApp();
	m_wndDBPath.GetWindowText( m_strDBPath );
	pApp->WriteProfileString( "General", "Last", m_strDBPath );
	
	return CDialog::DestroyWindow();
}

/****************************** Maxim [15.07.2002] - Creating DAO 3.6 ******************************/
#include "DAOIMPL.H"
extern _AFX_DAO_STATE* AFX_CDECL AfxGetDaoState();

#include "dbdaoid.h"



void PASCAL COwnDaoWorkspace::CompactDatabase(LPCTSTR lpszSrcName,
	LPCTSTR lpszDestName, LPCTSTR lpszLocale, int nOptions,
	LPCTSTR lpszPassword)
{
	_AFX_DAO_STATE* pDaoState = AfxGetDaoState();
	if (pDaoState->m_pDAODBEngine == NULL)
		InitializeEngine();

	CDaoWorkspace::CompactDatabase( lpszSrcName, lpszDestName, lpszLocale, nOptions, lpszPassword );
}

void AFX_CDECL COwnDaoWorkspace::InitializeEngine()
{
	_AFX_DAO_STATE* pDaoState = AfxGetDaoState();

	// Attempt to initialize OLE component objects
	//  (use default IMalloc allocator)
	DAO_CHECK_ERROR(::CoInitialize(NULL),
		AFX_DAO_ERROR_ENGINE_INITIALIZATION);
	pDaoState->m_bOleInitialized = TRUE;

	// Hook AfxDaoTerm to CWinApp, otherwise explicit AfxDaoTerm call req'd
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL)
		pApp->m_lpfnDaoTerm = AfxDaoTerm;

	BYTE bUseDao = 36;

	CLSID clsidEngine;
	COleVariant varKey;
	GUID guidEngine;

	// Set the interface type
#ifdef _UNICODE
			guidEngine = IID_IDAODBEngineW;
#else
			guidEngine = IID_IDAODBEngine;
#endif

	// DAO 3.5 and 3.6 runtime key
	varKey = _T("mbmabptebkjcdlgtjmskjwtsdhjbmkmwtrak");

	switch (bUseDao)
	{
		case 35:
			// Use DAO 3.5
			clsidEngine = CLSID35_CDAODBEngine;
			break;

		case 30:
			// Use DAO 3.0
			clsidEngine = CLSID30_CDAODBEngine;

			// DAO 3.0 runtime key
			varKey = _T("mjgcqcejfchcijecpdhckcdjqigdejfccjri");

			// Set the interface type
#ifdef _UNICODE
			guidEngine = IID30_IDAODBEngineW;
#else
			guidEngine = IID30_IDAODBEngine;
#endif
			break;

		case 36:
			// Use DAO 3.6
			clsidEngine = CLSID_CDAODBEngine;
			break;

		default:
			ASSERT(FALSE);
	}

	LPCLASSFACTORY2 pCF2;
	DAO_CHECK_ERROR(::CoGetClassObject(clsidEngine,
		CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory2, (LPVOID*)&pCF2),
		AFX_DAO_ERROR_ENGINE_INITIALIZATION);

	TRY
	{
		DAO_CHECK_ERROR(pCF2->CreateInstanceLic( NULL, NULL, guidEngine,
			V_BSTR(&varKey), (LPVOID*)&pDaoState->m_pDAODBEngine),
			AFX_DAO_ERROR_ENGINE_INITIALIZATION);
	}
	CATCH_ALL(e)
	{
		pCF2->Release();
		THROW_LAST();
	}
	END_CATCH_ALL

	pCF2->Release();
}

/****************************************************************************************************/
