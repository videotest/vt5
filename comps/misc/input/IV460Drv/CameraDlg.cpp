// CameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IV460Drv.h"
#include "CameraDlg.h"
#include "iv4hlp.h"
#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraDlg dialog


CCameraDlg::CCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCameraDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CCameraDlg::UpdateCamera()
{
	unsigned nCamera = GetCheckedRadioButton(IDC_BW_RS170, IDC_USER_CAMERA)-IDC_BW_RS170;
	m_pDrv.SetInt(0, NULL, _T("Camera"), nCamera, ID2_IGNOREMETHODIC);
	m_pDrv->ReinitPreview();
	if (m_pDrv)
		m_pDrv->UpdateSizes();
	BOOL bEnable = nCamera == 4;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FILE_NAME), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BROWSE), bEnable);
}


BEGIN_MESSAGE_MAP(CCameraDlg, CDialog)
	//{{AFX_MSG_MAP(CCameraDlg)
	ON_BN_CLICKED(IDC_BW_CCIR, OnBwCcir)
	ON_EN_CHANGE(IDC_E_FILE_NAME, OnChangeEFileName)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BW_RS170, OnBwRs170)
	ON_BN_CLICKED(IDC_USER_CAMERA, OnUserCamera)
	ON_BN_CLICKED(IDC_RGB_CCIR, OnRgbCcir)
	ON_BN_CLICKED(IDC_RGB_RS170, OnRgbRs170)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraDlg message handlers

BOOL CCameraDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString sFileName = m_pDrv.GetString(0, NULL, _T("CameraIniFile"), _T("rs170rgb"), ID2_STORE|ID2_IGNOREMETHODIC);
	unsigned nCamera = m_pDrv.GetInt(0, NULL, _T("Camera"), 0, ID2_STORE|ID2_IGNOREMETHODIC);
	CheckRadioButton(IDC_BW_RS170, IDC_USER_CAMERA,nCamera+IDC_BW_RS170);
	SetDlgItemText(IDC_E_FILE_NAME, sFileName);
	/*char nCamera;
	int r = IV4InquireCurrentBoardConfig(&nCamera);
	dprintf("IV4InquireCurrentBoardConfig in CCameraDlg::OnInitDialog : %d\n", r);
	if (r != IV4_OK || nCamera == -1)
		nCamera = 0;
	CheckRadioButton(IDC_BW_RS170, IDC_USER_CAMERA,nCamera+IDC_BW_RS170);
	char szPath[_MAX_PATH];
	IV4CameraConfig camcfg;
	r = IV4GetBoardConfigEx(4, &camcfg, szPath);
	dprintf("IV4GetBoardConfigEx in CCameraDlg::OnInitDialog : %d\n", r);
	if (r != IV4_OK)
		szPath[0] = 0;
	SetDlgItemText(IDC_E_FILE_NAME, szPath);*/
	BOOL bEnable = nCamera == 4;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FILE_NAME), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BROWSE), bEnable);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCameraDlg::OnBwCcir() 
{
	UpdateCamera();
}

void CCameraDlg::OnBwRs170() 
{
	UpdateCamera();
}

void CCameraDlg::OnUserCamera() 
{
	UpdateCamera();
}

void CCameraDlg::OnRgbCcir() 
{
	UpdateCamera();
}

void CCameraDlg::OnRgbRs170() 
{
	UpdateCamera();
}

void CCameraDlg::OnChangeEFileName() 
{
	CString sIniPath;
	GetDlgItemText(IDC_E_FILE_NAME, sIniPath);
	if (!sIniPath.IsEmpty())
	{
		m_pDrv.SetString(0, NULL, _T("CameraIniFile"), sIniPath, ID2_IGNOREMETHODIC);
		/*int r = IV4SetBoardConfigEx(4, NULL, sIniPath.GetBuffer(256));
		dprintf("IV4SetBoardConfigEx in CCameraDlg::OnChangeEFileName : %d\n", r);*/
	}
	if (m_pDrv)
		m_pDrv->UpdateSizes();
}

void CCameraDlg::OnBrowse() 
{
	CString sIniPath;
	GetDlgItemText(IDC_E_FILE_NAME, sIniPath);
	CFileDialog dlg(TRUE, _T("ini"), sIniPath, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("ini files(*.ini)|*.ini||"), this, _FILE_OPEN_SIZE_ );  // common/defs.h
	
	int r = dlg.DoModal();
	if (r == IDOK)
	{
		CString s = dlg.GetPathName();
		SetDlgItemText(IDC_E_FILE_NAME, s);
	}
}

void CCameraDlg::OnCancel() 
{
	m_pDrv->CancelChanges();
	/*CString sFileName = g_StdProf.GetProfileString(NULL, _T("CameraIniFile"), _T("rs170rgb"), true, true);
	unsigned nCamera = g_StdProf.GetProfileInt(NULL, _T("Camera"), 0, true, true);
	IV4HlpInitializeCamera(nCamera, sFileName);*/
	CDialog::OnCancel();
}

void CCameraDlg::OnOK() 
{
	m_pDrv->ApplyChanges();
	/*unsigned nCamera = GetCheckedRadioButton(IDC_BW_RS170, IDC_USER_CAMERA)-IDC_BW_RS170;
	nCamera = min(nCamera,4);
	CString sIniPath;
	GetDlgItemText(IDC_E_FILE_NAME, sIniPath);
	g_StdProf.WriteProfileInt(NULL, _T("Camera"), nCamera, true);
	g_StdProf.WriteProfileString(NULL, _T("CameraIniFile"), sIniPath, true);*/
	CDialog::OnOK();
}



