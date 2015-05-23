// TemplateDeleteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "TemplateDeleteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDeleteDlg dialog


CTemplateDeleteDlg::CTemplateDeleteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateDeleteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateDeleteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTemplateDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDeleteDlg)
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_ctrlTemplate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateDeleteDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateDeleteDlg)
	ON_BN_CLICKED(IDC_DELETE_BTN, OnDeleteBtn)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDeleteDlg message handlers


BOOL CTemplateDeleteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlTemplate.ResetContent();

	//Fill file based report
	CString strDir = GetReportDir();
	CString strMask = strDir + "*.rpt";

	CFileFind ff;

	BOOL bFound = ff.FindFile( strMask );

	while( bFound )
	{
		bFound = ff.FindNextFile();				
		m_ctrlTemplate.AddString( ff.GetFileTitle() );			
	}

	m_ctrlTemplate.SetSel( 0);

	
	return TRUE;
}

void CTemplateDeleteDlg::OnDeleteBtn() 
{
	CString strFileName;
	int nIndex = m_ctrlTemplate.GetCurSel( );
	if( nIndex < 0 )
		return;

	CString strSelection;	
	m_ctrlTemplate.GetText( nIndex, strSelection );


	CString strPath = GetReportDir();

	strFileName = strPath + strSelection + CString( ".rpt" );

	CFileFind ff;

	if( !ff.FindFile( strFileName ) )
		return;


	if( ::DeleteFile( (LPCSTR)strFileName ) )	
	{
		m_ctrlTemplate.DeleteString( nIndex );
	}
	
}


void CTemplateDeleteDlg::OnHelp() 
{
	HelpDisplay( "TemplateDeleteDlg" );
}
