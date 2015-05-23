// DBaseWiz.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBaseWiz.h"
#include "Wizards.h"


/////////////////////////////////////////////////////////////////////////////
// CNewDBTypePage property page

IMPLEMENT_DYNCREATE(CNewDBTypePage, CPropertyPage)

CNewDBTypePage::CNewDBTypePage() : CPropertyPage(CNewDBTypePage::IDD)
{
	//{{AFX_DATA_INIT(CNewDBTypePage)
	m_nDBaseType = 0;
	//}}AFX_DATA_INIT
}

CNewDBTypePage::~CNewDBTypePage()
{
}

void CNewDBTypePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewDBTypePage)
	DDX_Radio(pDX, IDC_DBASE_TYPE, m_nDBaseType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewDBTypePage, CPropertyPage)
	//{{AFX_MSG_MAP(CNewDBTypePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewDBTypePage message handlers
/////////////////////////////////////////////////////////////////////////////
// CNewDBAccessPage property page

IMPLEMENT_DYNCREATE(CNewDBAccessPage, CPropertyPage)

CNewDBAccessPage::CNewDBAccessPage() : CPropertyPage(CNewDBAccessPage::IDD)
{
	char *lpPath = new char[MAX_PATH];
	
	::GetCurrentDirectory( MAX_PATH, lpPath );
	CString Path = lpPath;
	delete []lpPath;

	//{{AFX_DATA_INIT(CNewDBAccessPage)
	m_strDBPath.LoadString( IDS_NEW_ACCESS_FILENAME );
	m_strDBPath = Path + "\\" + m_strDBPath;

	m_nTemplate = 1;
	m_strTemplatePath = _T("");
	m_bCreateNewTable = TRUE;
	//}}AFX_DATA_INIT
}

CNewDBAccessPage::~CNewDBAccessPage()
{
}

void CNewDBAccessPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewDBAccessPage)
	DDX_Text(pDX, IDC_DB_PATH, m_strDBPath);
	DDX_Radio(pDX, IDC_TEMPLATE, m_nTemplate);
	DDX_Text(pDX, IDC_TEMPLATE_LOCATION_PATH, m_strTemplatePath);
	DDX_Check(pDX, IDC_CREATE_NEW_TABLE, m_bCreateNewTable);
	//}}AFX_DATA_MAP
}

BOOL CNewDBAccessPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	((CButton*)GetDlgItem(IDC_LOAD_DB_LOCATION))->
					SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON_LOAD" ));	
	((CButton*)GetDlgItem(IDC_LOAD_TEMPLATE_LOCATION))->
					SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON_LOAD" ));			
	
	UpdateData( FALSE );

	SetAppearance();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewDBAccessPage::SetAppearance()
{
	UpdateData( TRUE );

	if( m_nTemplate == 0 )
	{
		GetDlgItem(IDC_TEMPLATE_LOCATION_PATH)->EnableWindow( TRUE );
		GetDlgItem(IDC_LOAD_TEMPLATE_LOCATION)->EnableWindow( TRUE );		
		GetDlgItem(IDC_CREATE_NEW_TABLE)->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem(IDC_TEMPLATE_LOCATION_PATH)->EnableWindow( FALSE );
		GetDlgItem(IDC_LOAD_TEMPLATE_LOCATION)->EnableWindow( FALSE );				
		m_bCreateNewTable = TRUE;
		GetDlgItem(IDC_CREATE_NEW_TABLE)->EnableWindow( FALSE );
	}	

	UpdateData( FALSE );

}

void CNewDBAccessPage::OnTemplateUseExistDB() 
{
	SetAppearance();
}

void CNewDBAccessPage::OnEmptyDatabase() 
{
	SetAppearance();	
}

void CNewDBAccessPage::OnLoadDbLocation() 
{
	UpdateData( TRUE );

	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	
	strcpy(dlg.m_ofn.lpstrFile, m_strDBPath);
	dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strDBPath = dlg.GetPathName();		
		if( m_strDBPath.Find(".mdb") == -1 )
			m_strDBPath += ".mdb";
		UpdateData(FALSE);	
	}	
	
}

void CNewDBAccessPage::OnLoadTemplateLocation() 
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	
	strcpy(dlg.m_ofn.lpstrFile, m_strTemplatePath);
	dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strTemplatePath = dlg.GetPathName();
		UpdateData(FALSE);	
	}	
	
}



BEGIN_MESSAGE_MAP(CNewDBAccessPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNewDBAccessPage)
	ON_BN_CLICKED(IDC_TEMPLATE, OnTemplateUseExistDB)
	ON_BN_CLICKED(IDC_EMPTY_DATABASE, OnEmptyDatabase)
	ON_BN_CLICKED(IDC_LOAD_DB_LOCATION, OnLoadDbLocation)
	ON_BN_CLICKED(IDC_LOAD_TEMPLATE_LOCATION, OnLoadTemplateLocation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewDBAccessPage message handlers
/////////////////////////////////////////////////////////////////////////////
// CNewDBAdvPage property page

IMPLEMENT_DYNCREATE(CNewDBAdvPage, CPropertyPage)

CNewDBAdvPage::CNewDBAdvPage() : CPropertyPage(CNewDBAdvPage::IDD)
{
	//{{AFX_DATA_INIT(CNewDBAdvPage)
	m_strConnectionString = _T("");
	m_strPassword = _T("");
	m_strUserName = _T("");
	m_bAskForLogin = FALSE;
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CNewDBAdvPage::~CNewDBAdvPage()
{
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBAdvPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewDBAdvPage)
	DDX_Text(pDX, IDC_CONNECTION_STR, m_strConnectionString);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDX_Check(pDX, IDC_ASK_FOR_LOGIN, m_bAskForLogin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewDBAdvPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNewDBAdvPage)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CNewDBAdvPage::SetAppearance()
{
	if( m_strConnectionString.IsEmpty() )
		GetDlgItem(IDC_TEST)->EnableWindow( FALSE );
	else
		GetDlgItem(IDC_TEST)->EnableWindow( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CNewDBAdvPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetAppearance();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBAdvPage::OnConnect() 
{
	m_strConnectionString = "";

	SetAppearance();

	ATL::CDataSource datasource;
	
	if( S_OK != datasource.Open( AfxGetMainWnd()->GetSafeHwnd() ) )
	{
		SetAppearance();
		return;
	}

	
	BSTR bstr = 0;
	if( datasource.GetInitializationString( &bstr, true ) != S_OK )
	{
		SetAppearance();
		return;	
	}

	m_strConnectionString = bstr;

	if( bstr )
		::SysFreeString( bstr );

	UpdateData( FALSE );	

	SetAppearance();
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBAdvPage::OnTest() 
{
	if( m_strConnectionString.IsEmpty() )
		return;

	ADO::_ConnectionPtr connPtr;	

	UpdateData( TRUE );

	_bstr_t ConnectionString( m_strConnectionString );
	_bstr_t UserID( (LPCSTR)m_strUserName );
	_bstr_t Password( (LPCSTR)m_strPassword );	

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if( connPtr == 0 )
	{
		if( !SUCCEEDED( connPtr.CreateInstance( __uuidof(ADO::Connection) ) ) )		
		{
			AfxMessageBox( IDS_WARNING_CONNECTION_OBJECT_FAILED, MB_ICONSTOP | MB_OK );
			return;
		}
	}

	try
	{

		connPtr->Open( ConnectionString, UserID, Password, 0 );	
		connPtr->Close();

	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return;
	}


	AfxMessageBox( IDS_OPEN_CONNECTION_SUCCEDED );

}

/////////////////////////////////////////////////////////////////////////////
// CNewDBAdvPage message handlers

BOOL CNewDBAccessPage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG();
}

BOOL CNewDBAdvPage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG();
}

BOOL CNewDBTypePage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG();
}

LRESULT CNewDBAccessPage::OnWizardBack() 
{	
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CNewDBAccessPage::OnWizardNext() 
{	
	UpdateData( TRUE );

	if( m_strDBPath.IsEmpty() )
	{
		AfxMessageBox( IDS_WARNING_SET_ACCES_FILE, MB_ICONSTOP );
		GetDlgItem(IDC_DB_PATH)->SetFocus();
		return -1;
	}

	if( m_nTemplate == 0 )//use template
	{
		CFileFind ff;
		if( !ff.FindFile( m_strTemplatePath ) )
		{
			CString strMessage;
			strMessage.LoadString( IDS_WARNING_FILE_NOT_FOUND );
			CString strError;
			strError.Format( (LPCTSTR)strMessage, (LPCTSTR)m_strTemplatePath );
			AfxMessageBox( strError, MB_ICONSTOP );			

			GetDlgItem(IDC_TEMPLATE_LOCATION_PATH)->SetFocus();

			return -1;
		}
	}


	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CNewDBAccessPage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}

LRESULT CNewDBAdvPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CNewDBAdvPage::OnWizardNext() 
{
	UpdateData( TRUE );

	if( m_strConnectionString.IsEmpty() )
	{
		AfxMessageBox( IDS_WARNING_CHOOSE_SOURCE, MB_ICONSTOP );
		GetDlgItem(IDC_CONNECT)->SetFocus();
		return -1;
	}

	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CNewDBAdvPage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}

LRESULT CNewDBTypePage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CNewDBTypePage::OnWizardNext() 
{
	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CNewDBTypePage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}
/////////////////////////////////////////////////////////////////////////////
// CDBInfoCreate property page

IMPLEMENT_DYNCREATE(CDBInfoCreate, CPropertyPage)

CDBInfoCreate::CDBInfoCreate() : CPropertyPage(CDBInfoCreate::IDD)
{
	//{{AFX_DATA_INIT(CDBInfoCreate)
	m_strTaskList = _T("");
	//}}AFX_DATA_INIT
}

CDBInfoCreate::~CDBInfoCreate()
{
}

void CDBInfoCreate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBInfoCreate)
	DDX_Text(pDX, IDC_TASK_LIST, m_strTaskList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBInfoCreate, CPropertyPage)
	//{{AFX_MSG_MAP(CDBInfoCreate)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBInfoCreate message handlers
/////////////////////////////////////////////////////////////////////////////
// CDBCreateProcess property page

IMPLEMENT_DYNCREATE(CDBCreateProcess, CPropertyPage)

CDBCreateProcess::CDBCreateProcess() : CPropertyPage(CDBCreateProcess::IDD)
{
	//{{AFX_DATA_INIT(CDBCreateProcess)
	m_strProgress = _T("");
	//}}AFX_DATA_INIT
}

CDBCreateProcess::~CDBCreateProcess()
{
}

void CDBCreateProcess::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBCreateProcess)
	DDX_Text(pDX, IDC_PROCESS_LIST, m_strProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBCreateProcess, CPropertyPage)
	//{{AFX_MSG_MAP(CDBCreateProcess)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBCreateProcess message handlers

BOOL CDBCreateProcess::OnSetActive() 
{		
	PROCESS_SET_ACTIVE_MSG();
}

LRESULT CDBCreateProcess::OnWizardBack() 
{
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CDBCreateProcess::OnWizardNext() 
{
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CDBCreateProcess::OnWizardFinish() 
{
	PROCESS_WIZARD_FINISH_MSG()
}

BOOL CDBInfoCreate::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG();
}

LRESULT CDBInfoCreate::OnWizardBack() 
{
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CDBInfoCreate::OnWizardNext() 
{
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CDBInfoCreate::OnWizardFinish() 
{
	PROCESS_WIZARD_FINISH_MSG()
}



