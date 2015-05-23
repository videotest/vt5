// CreateATdlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuditTrail.h"
#include "CreateATdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCreateATdlg dialog


CCreateATdlg::CCreateATdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateATdlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateATdlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreateATdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateATdlg)
	DDX_Control(pDX, IDC_LIST_IMAGES, m_listImages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateATdlg, CDialog)
	//{{AFX_MSG_MAP(CCreateATdlg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DESELECT, OnButtonDeselect)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateATdlg message handlers

BOOL CCreateATdlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect	rcList = NORECT;
	m_listImages.GetClientRect(rcList);
	m_listImages.InsertColumn(1, "", LVCFMT_LEFT, rcList.Width());
	m_listImages.SetExtendedStyle(LVS_EX_CHECKBOXES);

	int nSize = m_arrImages.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		m_listImages.InsertItem(i, m_arrImages[i]);
		m_listImages.SetCheck(i);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateATdlg::OnOK() 
{
	// TODO: Add extra validation here
	int nSize = m_listImages.GetItemCount();	
	for(int i = nSize; i >= 0; i--)
	{
		if(m_listImages.GetCheck(i) == FALSE)
		{
			m_arrImages.RemoveAt(i);
		}
	}
	
	CDialog::OnOK();
}

void CCreateATdlg::OnButtonSelect() 
{
	for(int i = 0; i < m_listImages.GetItemCount(); i++)
		m_listImages.SetCheck(i, TRUE);
}

void CCreateATdlg::OnButtonDeselect() 
{
	for(int i = 0; i < m_listImages.GetItemCount(); i++)
		m_listImages.SetCheck(i, FALSE);
}

void CCreateATdlg::OnHelp() 
{
	HelpDisplay( "CreateATdlg" );	
}
