// ConvertImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "ConvertImageDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CConvertImageDlg dialog


CConvertImageDlg::CConvertImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertImageDlg::IDD, pParent), m_imgCC()
{
	//{{AFX_DATA_INIT(CConvertImageDlg)
	m_stFileName = _T("");
	//}}AFX_DATA_INIT
	m_strCurSystem = "";
}

CConvertImageDlg::~CConvertImageDlg()
{
	m_imgCC.DeleteImageList();
}

void CConvertImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertImageDlg)
	DDX_Control(pDX, IDC_STATIC_CUR_ICON, m_stCurIcon);
	DDX_Control(pDX, IDC_COMBOBOXEX_CONVERTTO, m_cbConvertTo);
	DDX_Control(pDX, IDC_STATIC_CUR_SYSTEM, m_stCurSystem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvertImageDlg, CDialog)
	//{{AFX_MSG_MAP(CConvertImageDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertImageDlg message handlers

BOOL CConvertImageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int nIconX = 15;
	int nIconY = 15;

	LONG nStyle = ::GetWindowLong(m_stCurIcon.GetSafeHwnd(), GWL_STYLE);
	nStyle |= SS_ICON|SS_CENTER;
	::SetWindowLong(m_stCurIcon.GetSafeHwnd(), GWL_STYLE, nStyle);
	m_imgCC.Create(nIconX, nIconY, ILC_COLOR, 0, 1);
	m_imgCC.SetBkColor(::GetSysColor(COLOR_WINDOW));
	m_cbConvertTo.SetImageList(&m_imgCC);
	m_stCurSystem.SetWindowText(m_strCurSystem);
	IUnknown* punk;
	punk = _GetOtherComponent(GetAppUnknown(), IID_IColorConvertorManager);
	ICompManagerPtr sptrCM(punk);
	punk->Release();
	ASSERT(sptrCM != 0);
	int numCvtrs;
	sptrCM->GetCount(&numCvtrs);
	BSTR bstr;
	sptrIColorConvertor4 sptrCC;
	COMBOBOXEXITEM cbItem;
	int idx = 0;
	for(int i = 0; i < numCvtrs; i++)
	{
		sptrCM->GetComponentUnknownByIdx(i, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);
		HICON hIcon = 0;
		DWORD dwFlags = 0;
		sptrCC->GetConvertorFlags(&dwFlags);
		if (strName != m_strCurSystem && ((dwFlags & ccfLabCompatible) == ccfLabCompatible))
		{
			sptrCC->GetConvertorIcon( &hIcon );
			m_imgCC.Add(hIcon);
			cbItem.mask = CBEIF_TEXT|CBEIF_DI_SETITEM|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
			cbItem.pszText = (LPTSTR)(LPCTSTR)strName;
			cbItem.iImage = idx;
			cbItem.iSelectedImage = idx;
			cbItem.iItem = idx;
			idx++;
			m_cbConvertTo.InsertItem(&cbItem);	
		}
		else if(strName == m_strCurSystem)
		{
			sptrCC->GetConvertorIcon( &hIcon );
			m_stCurIcon.SetIcon(hIcon);
		}
	}

	
	m_cbConvertTo.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConvertImageDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_cbConvertTo.GetLBText(m_cbConvertTo.GetComboBoxCtrl( )->GetCurSel(), m_strCurSystem);	
	CDialog::OnOK();
}

void CConvertImageDlg::OnHelp() 
{
	HelpDisplay( "ConvertImageDlg" );	
}
