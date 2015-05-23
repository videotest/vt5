#include "stdafx.h"
#include "shell.h"
#include "dialogs.h"

#include "mainfrm.h"

BEGIN_MESSAGE_MAP(CNewTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTypeDlg)
	ON_LBN_DBLCLK(AFX_IDC_LISTBOX, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CNewTypeDlg::OnInitDialog()
{
	CListBox* pListBox = (CListBox*)GetDlgItem( AFX_IDC_LISTBOX );
	ASSERT(pListBox != NULL);

	// fill with document templates in list
	pListBox->ResetContent();

	POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	// add all the CDocTemplates in the list by name
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CString strTypeName;
		if (pTemplate->GetDocString( strTypeName, CDocTemplate::fileNewName ) &&
		   !strTypeName.IsEmpty())
		{
			// add it to the listbox
			int nIndex = pListBox->AddString( strTypeName );
			if (nIndex == -1)
			{
				EndDialog(-1);
				return FALSE;
			}
			pListBox->SetItemDataPtr( nIndex, pTemplate );
		}
	}

	int nTemplates = pListBox->GetCount();
	if (nTemplates == 0)
	{
		TRACE0("Error: no document templates to select from!\n");
		EndDialog(-1); // abort
	}
	else if (nTemplates == 1)
	{
		// get the first/only item
		m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(0);
		ASSERT_VALID(m_pSelectedTemplate);
		ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
		EndDialog(IDOK);    // done
	}
	else
	{
		// set selection to the first one (NOT SORTED)
		pListBox->SetCurSel(0);
	}

	return CDialog::OnInitDialog();
}

void CNewTypeDlg::OnOK()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);
	ASSERT(pListBox != NULL);
	// if listbox has selection, set the selected template
	int nIndex;
	if ((nIndex = pListBox->GetCurSel()) == -1)
	{
		// no selection
		m_pSelectedTemplate = NULL;
	}
	else
	{
		m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(nIndex);
		ASSERT_VALID(m_pSelectedTemplate);
		ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
	}
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CChooseWindowDlg dialog


CChooseWindowDlg::CChooseWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseWindowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseWindowDlg)
	m_strWindowName = _T("");
	//}}AFX_DATA_INIT
}


void CChooseWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseWindowDlg)
	DDX_LBString(pDX, IDC_WINDOWS, m_strWindowName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseWindowDlg)
	ON_LBN_DBLCLK(IDC_WINDOWS, OnDblclkWindows)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseWindowDlg message handlers

BOOL CChooseWindowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CListBox	*plist = (CListBox	*)GetDlgItem( IDC_WINDOWS );
	CMainFrame	*pmain = (CMainFrame *)AfxGetMainWnd();

	POSITION	pos = pmain->GetFirstMDIChildPosition();	

	CWnd	*pwndMDIActive = pmain->_GetActiveFrame();

	while( pos )
	{
		CWnd	*pwnd = pmain->GetNextMDIChild( pos );
		CString	strTitle;

		pwnd->GetWindowText( strTitle );

		int	idx = plist->AddString( strTitle );

		if( pwnd == pwndMDIActive )
			plist->SetCurSel( idx );

	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseWindowDlg::OnOK() 
{
	if( !UpdateData() )
		return;

	CMainFrame	*pmain = NULL;
	pmain = (CMainFrame *)AfxGetMainWnd();
	if( pmain )
	{
		CWnd* pActiveWnd = NULL;
		pActiveWnd = pmain->GetWindowByTitle( m_strWindowName );
		if( pActiveWnd )
			pActiveWnd->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, 0L );
	}
	
	
	CDialog::OnOK();
}

void CChooseWindowDlg::OnDblclkWindows() 
{
	OnOK();	
}

void CChooseWindowDlg::OnHelp() 
{
	HelpDisplay( "ChooseWindowDlg" );
}
