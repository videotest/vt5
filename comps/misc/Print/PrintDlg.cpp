// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg

IMPLEMENT_DYNAMIC(CPrintDlg, CPrintDialog)

CPrintDlg::CPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd):
			CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
	//{{AFX_DATA_INIT(CPrintDlg)
	m_bUseBaseTemplate = 0;		//Use base template
	m_nSelection = 1;			//0-All Data Object 1 - Active View
	m_bStore = FALSE;			//Save new template or not
	m_nStoreTarget = 2;		//0-To active doc, 1-to spec. doc,2-to file	
	m_nExistTemplate = -1;		//Index of exist template
	m_nBasedOnTemplate = -1;	//Index of based on template
	//}}AFX_DATA_INIT
				
	m_pd.lpPrintTemplateName = (LPTSTR) MAKEINTRESOURCE( PRINTDLGORD ); 
	m_pd.lpSetupTemplateName = (LPTSTR) MAKEINTRESOURCE( PRNSETUPDLGORD ); 	
	m_pd.Flags |= PD_ENABLESETUPTEMPLATE | PD_ENABLEPRINTTEMPLATE;
	m_pd.hInstance = AfxGetInstanceHandle();
	m_pd.Flags &= ~PD_RETURNDC;	
	//AfxGetApp()->GetPrinterDeviceDefaults(&m_pd);
}


void CPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDlg)		
	DDX_Radio(pDX, IDC_SOURCE_TEMPLATE, m_bUseBaseTemplate);
	DDX_Radio(pDX, IDC_ALL_DATA_OBJECT, m_nSelection);
	DDX_Check(pDX, IDC_STORE, m_bStore);
	DDX_Radio(pDX, IDC_STORE_TARGET, m_nStoreTarget);
	DDX_CBIndex(pDX, IDC_BASED_ON_TEMPLATE, m_nBasedOnTemplate);
	DDX_CBIndex(pDX, IDC_EXIST_TEMPLATE, m_nExistTemplate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintDlg, CPrintDialog)
	//{{AFX_MSG_MAP(CPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CPrintDlg message handlers

void CPrintDlg::OnOK() 
{
	UpdateData( TRUE );	
	//m_bUseBaseTemplate
	CPrintDialog::OnOK();
}
