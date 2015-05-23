// TalbeWiz.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "TalbeWiz.h"
#include "Wizards.h"


/////////////////////////////////////////////////////////////////////////////
// CNewTablePage property page

IMPLEMENT_DYNCREATE(CNewTablePage, CPropertyPage)

CNewTablePage::CNewTablePage() : CPropertyPage(CNewTablePage::IDD)
{
	//{{AFX_DATA_INIT(CNewTablePage)
	m_strTableName = _T("");
	//}}AFX_DATA_INIT

	m_strTableName.LoadString( IDS_DEF_TABLE_NAME );
}

CNewTablePage::~CNewTablePage()
{
}

void CNewTablePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTablePage)
	DDX_Text(pDX, IDC_TABLE_NAME, m_strTableName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewTablePage, CPropertyPage)
	//{{AFX_MSG_MAP(CNewTablePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTablePage message handlers
/////////////////////////////////////////////////////////////////////////////
// CDefineFieldCreationPage dialog

IMPLEMENT_DYNCREATE(CDefineFieldCreationPage, CPropertyPage)

CDefineFieldCreationPage::CDefineFieldCreationPage() : CPropertyPage(CDefineFieldCreationPage::IDD)
{
	//{{AFX_DATA_INIT(CDefineFieldCreationPage)
	m_nDefineType = 0;
	//}}AFX_DATA_INIT
}


void CDefineFieldCreationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineFieldCreationPage)
	DDX_Radio(pDX, IDC_DEFINE_FIELD_TYPE, m_nDefineType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefineFieldCreationPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDefineFieldCreationPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineFieldCreationPage message handlers
/////////////////////////////////////////////////////////////////////////////
// CAdvFieldConstrPage property page

IMPLEMENT_DYNCREATE(CAdvFieldConstrPage, CPropertyPage)

CAdvFieldConstrPage::CAdvFieldConstrPage() : CPropertyPage(CAdvFieldConstrPage::IDD)
{
	//{{AFX_DATA_INIT(CAdvFieldConstrPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvFieldConstrPage::~CAdvFieldConstrPage()
{
}

void CAdvFieldConstrPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvFieldConstrPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CAdvFieldConstrPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CRect rcGrid;
	GetClientRect( &rcGrid );

	/*
	CWnd* pWnd = GetDlgItem(IDC_GRID_PLACE);
	if( pWnd )
	{
		pWnd->GetWindowRect( &rcGrid );
		ScreenToClient( &rcGrid );
	}
	*/

	VERIFY( m_grid.Create( 
							rcGrid, this, 10025, 
							WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER
							) );

	CString strTableName( "table" );
	CWnd* pParent = GetParent();
	if( pParent && pParent->IsKindOf( RUNTIME_CLASS(CWizardSheet)) ) 
	{
		CNewTablePage* pPage = (CNewTablePage*)
			((CWizardSheet*)pParent)->FindPage( RUNTIME_CLASS(CNewTablePage), 0 );
		if( pPage )
			strTableName = pPage->m_strTableName;
	}

	m_grid.Init();
	m_grid.Clear();
	m_grid.CreateKeyRow( strTableName );	
	
	return TRUE;
}




BEGIN_MESSAGE_MAP(CAdvFieldConstrPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvFieldConstrPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvFieldConstrPage message handlers
/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldConstrPage property page

IMPLEMENT_DYNCREATE(CSimpleFieldConstrPage, CPropertyPage)

CSimpleFieldConstrPage::CSimpleFieldConstrPage() : CPropertyPage(CSimpleFieldConstrPage::IDD)
{
	//{{AFX_DATA_INIT(CSimpleFieldConstrPage)
	m_nImage = 2;
	m_nNumber = 0;
	m_nDateTime = 0;
	m_nString = 2;
	m_nOLE = 0;
	//}}AFX_DATA_INIT
}

CSimpleFieldConstrPage::~CSimpleFieldConstrPage()
{
}

void CSimpleFieldConstrPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimpleFieldConstrPage)
	DDX_Text(pDX, IDC_IMAGE, m_nImage);
	DDV_MinMaxInt(pDX, m_nImage, 0, 100);
	DDX_Text(pDX, IDC_NUMBER, m_nNumber);
	DDV_MinMaxInt(pDX, m_nNumber, 0, 100);
	DDX_Text(pDX, IDC_DATE_TIME, m_nDateTime);
	DDV_MinMaxInt(pDX, m_nDateTime, 0, 100);
	DDX_Text(pDX, IDC_STRING, m_nString);
	DDV_MinMaxInt(pDX, m_nString, 0, 100);
	DDX_Text(pDX, IDC_OLE, m_nOLE);
	DDV_MinMaxInt(pDX, m_nOLE, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimpleFieldConstrPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSimpleFieldConstrPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldConstrPage message handlers

BOOL CAdvFieldConstrPage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG()
}

BOOL CDefineFieldCreationPage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG()
}

BOOL CNewTablePage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG()
}

BOOL CSimpleFieldConstrPage::OnSetActive() 
{
	PROCESS_SET_ACTIVE_MSG()
}

LRESULT CAdvFieldConstrPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CAdvFieldConstrPage::OnWizardNext() 
{
	UpdateData( TRUE );

	CString strError;
	if( !m_grid.VerifyStruct( true, strError ) )
	{
		AfxMessageBox( strError, MB_ICONSTOP );
		return -1;
	}
	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CAdvFieldConstrPage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}

LRESULT CDefineFieldCreationPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CDefineFieldCreationPage::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CDefineFieldCreationPage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}

LRESULT CNewTablePage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CNewTablePage::OnWizardNext() 
{
	UpdateData( TRUE );

	CWnd* pWiz = GetParent();
	if( pWiz && pWiz->IsKindOf( RUNTIME_CLASS(CWizardSheet)) )
	{
		TABLEINFO_ARR arrTable;
		if( ::GetDBFieldInfo( (CWizardSheet*)pWiz, arrTable, 0 ) )
		{
			bool bValidName = false;
			CString strError;
			bValidName = IsValidTableName( m_strTableName, arrTable, strError );
			for( int i=0;i<arrTable.GetSize();i++ )
				delete arrTable[i];
			
			arrTable.RemoveAll();

			if( !bValidName )
			{
				AfxMessageBox( strError, MB_ICONSTOP );
				GetDlgItem(IDC_TABLE_NAME)->SetFocus();
				return -1;				
			}
		}
	}	
	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CNewTablePage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}

LRESULT CSimpleFieldConstrPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CSimpleFieldConstrPage::OnWizardNext() 
{
	if( !UpdateData( TRUE ) )
		return -1;

	if( m_nImage < 1 && m_nNumber < 1 && m_nDateTime < 1 && m_nString < 1 )
	{
		AfxMessageBox( IDS_WARNING_ZERO_FIELD_COUNT, MB_ICONSTOP );
		return -1;
	}
	
	PROCESS_WIZARD_NEXT_MSG()
}

BOOL CSimpleFieldConstrPage::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_FINISH_MSG()
}


BOOL CSimpleFieldConstrPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( GetDlgItem( IDC_SPIN1 ) )
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->SetRange( 0, 100 );

	if( GetDlgItem( IDC_SPIN2 ) )
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN2 ))->SetRange( 0, 100 );

	if( GetDlgItem( IDC_SPIN3 ) )
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN3 ))->SetRange( 0, 100 );

	if( GetDlgItem( IDC_SPIN4 ) )
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 ))->SetRange( 0, 100 );

	if( GetDlgItem( IDC_SPIN5 ) )
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 ))->SetRange( 0, 100 );
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
