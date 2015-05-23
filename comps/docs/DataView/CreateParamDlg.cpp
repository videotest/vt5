// CreateParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dataview.h"
#include "CreateParamDlg.h"

#include "ClassBase.h"


/////////////////////////////////////////////////////////////////////////////
// CCreateParamDlg dialog




CCreateParamDlg::CCreateParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateParamDlg)
	m_nNumber = etLinear;
	m_strName = _T("");
	//}}AFX_DATA_INIT

	m_dwType = 0;
	m_arrParam.RemoveAll();
}

CCreateParamDlg::~CCreateParamDlg()
{
}

void CCreateParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateParamDlg)
	DDX_Control(pDX, IDC_COMBO, m_Combo);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_nNumber);
	DDV_MinMaxInt(pDX, m_nNumber, 0, 99);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateParamDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateParamDlg)
	ON_BN_CLICKED(IDC_RADIO_ANGLE, OnRadioAngle)
	ON_BN_CLICKED(IDC_RADIO_COUNT, OnRadioCount)
	ON_BN_CLICKED(IDC_RADIO_LINEAR, OnRadioLinear)
	ON_BN_CLICKED(IDC_RADIO_UNDEFINED, OnRadioUndefined)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_EN_CHANGE(IDC_EDIT_NUMBER, OnChangeEditNumber)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateParamDlg message handlers

BOOL CCreateParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UINT uResId = -1;
	if (m_dwType == etLinear)
		uResId = IDC_RADIO_LINEAR;
	else if (m_dwType == etAngle)
		uResId = IDC_RADIO_ANGLE;
	else if (m_dwType == etCounter)
		uResId = IDC_RADIO_COUNT;
	else if (m_dwType == etUndefined)
		uResId = IDC_RADIO_UNDEFINED;

	CButton * pButton = 0;
	if (uResId != -1)
		pButton = (CButton*)GetDlgItem(uResId);

	if (pButton && ::IsWindow(pButton->GetSafeHwnd()))
		pButton->SetCheck(1);

	UpdateName();
	EnableApply(true);
	return TRUE;
}

void CCreateParamDlg::OnOK() 
{
	bool bEnable = true;
	CButton * pButton = (CButton*)GetDlgItem(IDC_APPLY);
	if (pButton || ::IsWindow(pButton->GetSafeHwnd()))
	{
		bEnable = pButton->IsWindowEnabled() ? true : false;
	}

	CDialog::OnOK();


	if (bEnable)
	{
		if (m_nNumber <= 0)
			return;

		TParam param;
		param.dwType = m_dwType;
		param.nCount = m_nNumber;
		param.strName = m_strName;
		m_arrParam.Add(param);
	}
}

void CCreateParamDlg::OnRadioAngle() 
{
	CButton * pButton = (CButton*)GetDlgItem(IDC_RADIO_ANGLE);

	if (!pButton || !::IsWindow(pButton->GetSafeHwnd()))
		return;
	if (pButton->GetCheck())
		m_dwType = etAngle;

	UpdateName();
	EnableApply(true);

}

void CCreateParamDlg::OnRadioCount() 
{
	CButton * pButton = (CButton*)GetDlgItem(IDC_RADIO_COUNT);

	if (!pButton || !::IsWindow(pButton->GetSafeHwnd()))
		return;

	if (pButton->GetCheck())
		m_dwType = etCounter;

	UpdateName();
	EnableApply(true);
}

void CCreateParamDlg::OnRadioLinear() 
{
	CButton * pButton = (CButton*)GetDlgItem(IDC_RADIO_LINEAR);

	if (!pButton || !::IsWindow(pButton->GetSafeHwnd()))
		return;

	if (pButton->GetCheck())
		m_dwType = etLinear;
	
	UpdateName();
	EnableApply(true);
}

void CCreateParamDlg::OnRadioUndefined() 
{
	CButton * pButton = (CButton*)GetDlgItem(IDC_RADIO_UNDEFINED);

	if (!pButton || !::IsWindow(pButton->GetSafeHwnd()))
		return;

	if (pButton->GetCheck())
		m_dwType = etUndefined;
	UpdateName();
	EnableApply(true);
}

void CCreateParamDlg::UpdateName()
{
	if (!GetManualParam(m_dwType, m_strName))
		return;

	CEdit * pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NAME);
	if (pEdit && ::IsWindow(pEdit->GetSafeHwnd()))
	{
		pEdit->SetWindowText(m_strName);
	}
}

struct ParamDefs
{	char	*pszName;	ParamType	type;};

ParamDefs	g_defs[] = 
{
	{"Linear", etLinear},
	{"Angle", etAngle},
	{"Counter", etCounter},
	{0, etUndefined}
};

bool CCreateParamDlg::GetManualParam(DWORD dwType, CString & strName)
{
	for (int i = 0; g_defs[i].pszName; i++)
	{
		if( dwType == (DWORD)g_defs[i].type )
		{
			strName = g_defs[i].pszName;
			return true;
		}
	}
	return false;
}


void CCreateParamDlg::OnApply() 
{
	if (!::IsWindow(GetSafeHwnd()))
	{
		EnableApply(false);
		return;
	}

	if (!UpdateData(TRUE))
	{
		EnableApply(false);
		return;
	}

	if (m_nNumber <= 0)
	{
		EnableApply(true);
		return;
	}
	
	TParam param;
	param.dwType = m_dwType;
	param.nCount = m_nNumber;
	param.strName = m_strName;
	m_arrParam.Add(param);


	EnableApply(false);
}

void CCreateParamDlg::EnableApply(bool bEnable)
{
	CButton * pButton = (CButton*)GetDlgItem(IDC_APPLY);
	if (!pButton || !::IsWindow(pButton->GetSafeHwnd()))
		return;

	pButton->EnableWindow(bEnable);
}

void CCreateParamDlg::OnChangeEditNumber() 
{
	EnableApply(true);
}

void CCreateParamDlg::OnChangeEditName() 
{
	CString str;

	CEdit * pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NAME);
	if (pEdit && ::IsWindow(pEdit->GetSafeHwnd()))
	{
		pEdit->GetWindowText(str);
		if (str != m_strName)
		{
			EnableApply(true);
			m_strName = str;
		}
	}
}
