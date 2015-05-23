// PreviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pixera.h"
#include "PreviewDialog.h"
#include "Settings.h"
#include "StdProfile.h"
#include "PixSDK.h"
//sergey 24/01/06

#include "MethCombo.h"
#include "MethNameDlg.h"
//end

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog dialog

const int Marg = 15;
const int ImgX = 640;
const int ImgY = 480;


CPreviewDialog::CPreviewDialog(IUnknown *punk, int nDevice, bool bPreview, CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPreviewDialog::IDD, pParent, NULL), m_Image(punk, nDevice)
{
	//{{AFX_DATA_INIT(CPreviewDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_sptrDrv = punk;
	m_nDev = nDevice;
	m_pDialog = NULL;
	m_bDelayedInitCtrllSite = true;
	m_nAdditionalInit = 0;
	m_bDisableAllCtrls = false;
	m_bPreview = bPreview;
	//sergey 24/01/06
	
	g_Methodics.SetId(IDC_LIST11);
	CMethNameDlg::s_idd = IDD_DIALOG_SETTING_NAME11;
	CMethNameDlg::s_idEdit = IDC_EDIT11;
	m_nIdNewMeth = -1;
	m_nIdDeleteMeth = -1;
	if (CStdProfileManager::m_pMgr->GetMethodic().IsEmpty())
	{
		CString sMeth = CStdProfileManager::m_pMgr->GetProfileString(_T("Methodics"), _T("Default"), NULL, false, true);
		CStdProfileManager::m_pMgr->InitMethodic(sMeth);
	}

	m_bChancedProf=0;
	m_bbuld=0;
	m_bNotB=0;
	
	//end
}

CPreviewDialog::~CPreviewDialog()
{
	delete m_pDialog;
}


void CPreviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewDialog)
	//DDX_Control(pDX, IDC_STATIC_PERIOD, m_StaticPeriod);
	//}}AFX_DATA_MAP
	if (m_bPreview) DDX_Control(pDX, IDC_IMAGE, m_Image);
}


BEGIN_MESSAGE_MAP(CPreviewDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LEVEL, OnLevel)
	ON_BN_CLICKED(IDC_COLOR_BALANCE, OnColorBalance)
	ON_BN_CLICKED(IDC_BLACK_BALANCE, OnBlackBalance)
	ON_BN_CLICKED(IDC_IMAGE_PAGE, OnImagePage)
	//sergey 11/01/06
	ON_BN_CLICKED(IDC_SETTING1, OnBnClickedSetting)
	ON_BN_CLICKED(IDC_SETTING11, OnBnClickedSetting11)
	ON_BN_CLICKED (IDC_BUTTON_NEW_SETTINGS11,OnNew)
	//end
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP+1, OnDelayedRepaint)
	//sergey 23/01/06
	ON_MESSAGE(WM_APP+2, OnDelayedRepaint1)
	//end
	//ON_BN_CLICKED(IDC_SETTING1, OnBnClickedSetting)
	ON_STN_CLICKED(IDC_IMAGE, OnStnClickedImage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

BOOL CPreviewDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	// begin preview	
	m_sptrPrv->BeginPreview(m_nDev, this);
	m_bInitOk = CAM_IsDriverLoaded()?true:false;
	// Init image
	CRect rc;
	//sergey 13/01/06
	//::EnableWindow(::GetDlgItem(m_hWnd, IDC_SETTING1), TRUE);
	//end
	if (m_bPreview)
	{
		m_Image.InitSizes(m_bInitOk);
		::SetWindowPos(::GetDlgItem(m_hWnd, IDC_IMAGE), 0, Marg, Marg, m_Image.m_szPrv.cx, m_Image.m_szPrv.cy,
			SWP_NOZORDER|SWP_NOMOVE);
		
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_IMAGE), &rc);
		ScreenToClient(rc);
		m_rcImage = CRect(Marg, Marg, m_Image.m_szPrv.cx, m_Image.m_szPrv.cy);
		SetWindowPos(0, 0, 0, rc.right+Marg, rc.bottom+Marg, SWP_NOZORDER|SWP_NOMOVE);
		
	}
	else
	{
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_IMAGE), &rc);
		ScreenToClient(rc);
		rc = CRect(0,0,rc.left,rc.bottom);
		AdjustWindowRect(rc, ::GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
		SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_NOMOVE);
		m_rcImage = CRect(0,0,0,0);
	}
	m_HistoDlg.Create(IDD_HISTO,this);
	SetWindowLong(m_HistoDlg.m_hWnd, GWL_ID, IDC_HISTO);
	AddDialog(NULL, 0);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1, true))
		::ShowWindow(::GetDlgItem(m_hWnd, ID_HELP), SW_HIDE);

	InitControlsState();
	//Sleep(200);
	// PIXERA GLUCK : auto white balance
	if(m_bInitOk)
	{
//		if (CAM_WB_GetMode() == kAutoWB || CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
//			SetTimer(1, 1000, NULL);
		
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SHUTTER_AUTO), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_EXP_TIME), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_EXP_TIME), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_TIME), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_SENSITIVITY), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_SENSITIVITY), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_SENSITIVITY), FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//sergey 27.12.05

//end
void CPreviewDialog::AddDialog(CBaseDialog *p, int idd)
{
	if (m_pDialog)
	{
		m_pDialog->DestroyWindow();
		delete m_pDialog;
		m_nIDHelp = IDD_PREVIEW;
	}
	m_pDialog = p;
	CRect rc,rc1;
	GetWindowRect(rc1);
	BOOL bHisto = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	if (p)
	{
		m_pDialog->Create(idd, this);
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_DLG_POS), &rc);
		ScreenToClient(rc);
		m_pDialog->SetWindowPos(&wndTop, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		m_pDialog->GetWindowRect(&rc);
		ScreenToClient(rc);
		::SetWindowPos(::GetDlgItem(m_hWnd, IDC_HISTO), 0, rc.left, rc.bottom+10, rc.Width(), 50, SWP_NOZORDER|SWP_NOSIZE|SWP_NOCOPYBITS);
		m_pDialog->ShowWindow(SW_SHOW);
		m_nIDHelp = idd;
	}
	else
	{
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_DLG_POS), &rc);
		ScreenToClient(rc);
		HWND hwnd1 = ::GetDlgItem(m_hWnd, IDC_HISTO);
		::SetWindowPos(::GetDlgItem(m_hWnd, IDC_HISTO), 0, rc.left, rc.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	}
	OnLayoutChanged();
}


void CPreviewDialog::OnLevel() 
{
	int n = IsDlgButtonChecked(IDC_LEVEL);
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, -1);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, -1);
		AddDialog(NULL, 0);
	}
	else
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, IDC_LEVEL);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, IDC_LEVEL);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, IDC_LEVEL);
		AddDialog(new CBaseDialog(IDD_LEVEL_ADJUSTMENT,this,this), IDD_LEVEL_ADJUSTMENT);
	}
}

void CPreviewDialog::OnColorBalance() 
{
	int n = IsDlgButtonChecked(IDC_COLOR_BALANCE);
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, -1);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, -1);
		AddDialog(NULL, 0);
	}
	else
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, IDC_COLOR_BALANCE);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, IDC_COLOR_BALANCE);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, IDC_COLOR_BALANCE);
		AddDialog(new CBaseDialog(IDD_COLOR_BALANCE,this,this), IDD_COLOR_BALANCE);
	}
}

void CPreviewDialog::OnBlackBalance() 
{
	int n = IsDlgButtonChecked(IDC_BLACK_BALANCE);
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, -1);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, -1);
		AddDialog(NULL, 0);
	}
	else
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, IDC_BLACK_BALANCE);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, IDC_BLACK_BALANCE);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, IDC_BLACK_BALANCE);
		AddDialog(new CBaseDialog(IDD_BLACK_BALANCE,this,this), IDD_BLACK_BALANCE);
	}
}

void CPreviewDialog::OnImagePage() 
{
	int n = IsDlgButtonChecked(IDC_IMAGE_PAGE);
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, -1);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, -1);
		AddDialog(NULL, 0);
	}
	else
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, IDC_IMAGE_PAGE);
		//CheckRadioButton(IDC_LEVEL, IDC_SETTING1, IDC_IMAGE_PAGE);//27/01/06
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, IDC_IMAGE_PAGE);
		AddDialog(new CBaseDialog(IDD_IMAGE,this,this), IDD_IMAGE);
	}
}

void CPreviewDialog::OnCancel() 
{
	//sergey 25.05.06
	//if (m_bInitOk && !m_bPreview) ::RestoreSettings();	
	//end
	CBaseDialog::OnCancel();
}

void CPreviewDialog::OnOK() 
{
	//sergey 05/02/06
	//CString sDefTitle;
	if(m_bChancedProf)
	{
	CDialog dlg1(IDD_DIALOG_SETTING_NAME12);	
	if(dlg1.DoModal()==IDOK)
	{
	      if (m_bInitOk && !m_bPreview)::SaveSettings();//sergey 23/01/06	
	if (m_bInitOk && m_bPreview)::SaveSettings();	
	}
	m_bChancedProf=0; 	
	
	}
	
	//end
	CBaseDialog::OnOK();
}


void CPreviewDialog::OnDestroy() 
{
	
	
	//sergey 05/02/06
	//if (m_bInitOk && m_bPreview)::SaveSettings();	
	//end
	m_sptrPrv->EndPreview(m_nDev, this);
	CBaseDialog::OnDestroy();
}

HRESULT CPreviewDialog::Invalidate()
{
	if (m_bPreview) m_Image.Invalidate();
	if (m_bHistogram && m_sptrDrv != 0)
	{
		LPBITMAPINFOHEADER lpbi;
		DWORD dwSize;
		m_sptrDrv->GetImage(m_nDev, (void**)&lpbi, &dwSize);
		m_HistoDlg.m_HistoBox.InitHisto(lpbi);
	}
	PostMessage(WM_APP+1);
	//sergey 23/01/06
	PostMessage(WM_APP+2);
	//end
	return S_OK;
}

HRESULT CPreviewDialog::OnChangeSize()
{
	return S_OK;
}


LRESULT CPreviewDialog::OnDelayedRepaint(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPeriod,dwFps;
	/*m_sptrPrv->GetPeriod(m_nDev,0,&dwPeriod);
	TCHAR szBuff[50];
	//sergey 21.12.05
	dwFps=1000./dwPeriod;
	//_ltot(dwPeriod, szBuff, 10);
	sprintf(szBuff,"%d кадр/сек.",dwFps);*/
	//end
	//m_StaticPeriod.SetWindowText(szBuff);
	if (m_pDialog) m_pDialog->OnDelayedNewImage();
	if (m_bHistogram && m_sptrDrv != 0)
		m_HistoDlg.ResetMinMax();
	//sergey 21.12.12.05
		//m_HistoDlg.ResetPeriod(dwFps);
		//end
	return 0;
}

//sergey 23/01/06
 LRESULT CPreviewDialog::OnDelayedRepaint1(WPARAM wParam, LPARAM lParam)
{
   static DWORD dwTime,dwNam;
	DWORD dwPeriod,dwFps,dwTimeTice;
	dwTimeTice=GetTickCount();
    dwTime++;
	
	//if((dwTime-dwTimeTice)==0 ||(dwTime-dwTimeTice)>120)
	
	m_sptrPrv->GetPeriod(m_nDev,0,&dwPeriod);
	TCHAR szBuff[50];
	//sergey 21.12.05
	dwFps=1000./dwPeriod;
	//_ltot(dwPeriod, szBuff, 10);
	sprintf(szBuff,"%d кадр/сек.",dwFps);
	//end
	//sergey 21.12.12.05
	if(dwTime==5)
	{
	dwTime=0;
		m_HistoDlg.ResetPeriod(dwFps);
		//end

	}
    
	return 0;
}
//end
void CPreviewDialog::OnLayoutChanged()
{
	CRect rc,rc1;
	GetWindowRect(rc1);
	BOOL bHisto = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	if (bHisto)
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_HISTO), &rc);
	else if (m_pDialog)
		m_pDialog->GetWindowRect(&rc);
	else
		::GetWindowRect(::GetDlgItem(m_hWnd, IDC_BLACK_BALANCE), &rc);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_HISTO), bHisto?SW_SHOW:SW_HIDE);
	ScreenToClient(rc);
	int nWidth = rc1.Width();
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_IMAGE), &rc1);
	ScreenToClient(rc1);
	m_rcImage = rc1;
	int nHeight = max(rc1.bottom+10, rc.bottom+10);
	rc = CRect(0,0,nWidth,nHeight);
	AdjustWindowRect(&rc, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
	SetWindowPos(0, 0, 0, nWidth, rc.Height(), SWP_NOZORDER|SWP_NOMOVE);
}

void CPreviewDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{

	if (m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoLButtonDown(nFlags, pt);
	}
	CBaseDialog::OnLButtonDown(nFlags, point);
}

void CPreviewDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoLButtonUp(nFlags, pt);
	}
	CBaseDialog::OnLButtonUp(nFlags, point);
}

void CPreviewDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) && m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoMouseMove(nFlags, pt);
	}
	CBaseDialog::OnMouseMove(nFlags, point);
}

BOOL CPreviewDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoSetCursor(0, pt);
		return TRUE;
	}
	
	return CBaseDialog::OnSetCursor(pWnd, nHitTest, message);
}


//sergey 11/01/06

void CPreviewDialog::OnBnClickedSetting()
{
	
	int n = IsDlgButtonChecked(IDC_SETTING1);//27/01/06
	
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		CheckRadioButton(IDC_LEVEL, IDC_SETTING1, -1);//27/01/06
		
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButton(IDC_LEVEL, IDC_SETTING1, IDC_SETTING1);//27/01/06
		
		AddDialog(new CBaseDialog(IDD_SETTING,this,this), IDD_SETTING);//27/01/06
		
	}
}

void CPreviewDialog::OnBnClickedSetting11()
{
	
	int n = IsDlgButtonChecked(IDC_SETTING11);
	if (n)
	{
		//CheckRadioButton(IDC_LEVEL, IDC_BLACK_BALANCE, -1);
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, -1);
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButton(IDC_LEVEL, IDC_SETTING11, IDC_SETTING11);
		AddDialog(new CBaseDialog(IDD_SETTING11,this,this), IDD_SETTING11);
	}
}
void CPreviewDialog::OnStnClickedImage()
{
	// TODO: Add your control notification handler code here
}

void CPreviewDialog::OnNew()
{
	int n = g_Methodics.NewMethodic();
}



HRESULT CPreviewDialog::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	
	
	if (nID == m_nIdNewMeth)
		{
			int n = g_Methodics.NewMethodic();
			return S_OK;
			
		}		
		
	return S_FALSE;
	
}



