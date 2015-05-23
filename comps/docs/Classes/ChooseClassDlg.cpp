// ChooseClassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Classes.h"
#include "ChooseClassDlg.h"
#include "NameConsts.h"


/////////////////////////////////////////////////////////////////////////////
// CChooseClassDlg dialog


CChooseClassDlg::CChooseClassDlg(CObjectListWrp & wrp, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseClassDlg::IDD, pParent),
	  m_listClasses(wrp)
{
	m_strDialogTitle.LoadString(IDS_CHOOSECLASSTOSELECT);
	//{{AFX_DATA_INIT(CChooseClassDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChooseClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseClassDlg)
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseClassDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseClassDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseClassDlg message handlers

BOOL CChooseClassDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_strDialogTitle);
	FillCombo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseClassDlg::OnOK() 
{
	if (!m_Combo.GetCount())
		m_strClassName = _T("");
	
	int nSel = m_Combo.GetCurSel();
	m_Combo.GetLBText(nSel, m_strClassName);

	CDialog::OnOK();
}

void CChooseClassDlg::SetClassName(LPCTSTR szSelClassName)
{
	CString strName(szSelClassName);

	if (!(m_Combo.GetSafeHwnd() && ::IsWindow(m_Combo.GetSafeHwnd())))
	{
		m_strClassName = strName;
		return;
	}
	
//	m_strClassName = _T("");
	int nCount = m_Combo.GetCount();
	if (!nCount) // yet not filled
	{
		m_strClassName = strName;
		return;
	}

	int nsel = m_Combo.SelectString(-1, strName);
	if (nsel == CB_ERR)
		return;

	m_strClassName = strName;
}

void CChooseClassDlg::FillCombo()
{
	if (!(m_Combo.GetSafeHwnd() && ::IsWindow(m_Combo.GetSafeHwnd())))
		return;

	if (m_Combo.GetCount())
		m_Combo.ResetContent();

	if (!m_listClasses.CheckState())
		return;

	// enumerate all objects in list and fill listbox
	POSITION pos = m_listClasses.GetFirstObjectPosition();
	while (pos)
	{
		IUnknown *punk = m_listClasses.GetNextObject(pos);
		if (!punk)
			continue;
		INamedObject2Ptr sptrObj(punk);
		punk->Release();

		BSTR bstrName = 0;
		if (SUCCEEDED(sptrObj->GetName(&bstrName)))
		{
			CString strName = bstrName;
			::SysFreeString(bstrName);
			m_Combo.AddString(strName);
		}
	}

	int nsel = m_Combo.SelectString(-1, m_strClassName);
	if (nsel == CB_ERR)
	{
		m_strClassName = _T("");

		if (m_Combo.GetCount())
			m_Combo.SetCurSel(0);
	}
}

void CChooseClassDlg::OnHelp() 
{
	HelpDisplay( "ChooseClassDlg" );		
}
