// Dodge1PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "Dodge1PropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND APIENTRY CreateToolTip(HWND hWndParent); 
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool = 0); 
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr = NULL, UINT nIDTool = 0, LPCTSTR szText = NULL); 
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool = 0, LPCTSTR lpszText = NULL);
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText);
void APIENTRY EnableToolTip(HWND hTip, BOOL activate);

IMPLEMENT_DYNCREATE(CDodge1PropPage, CDialog)

// {F88994AC-0466-4a38-B158-C4BB4D7B74E1}
GUARD_IMPLEMENT_OLECREATE(CDodge1PropPage, szEditorDodgePropPageProgID,  
0xf88994ac, 0x466, 0x4a38, 0xb1, 0x58, 0xc4, 0xbb, 0x4d, 0x7b, 0x74, 0xe1);


/////////////////////////////////////////////////////////////////////////////
// CDodge1PropPage dialog


CDodge1PropPage::CDodge1PropPage(CWnd* pParent /*=NULL*/) :
	CEditorPropBase(CDodge1PropPage::IDD)
{
	//{{AFX_DATA_INIT(CDodge1PropPage)
		// NOTE: the ClassWizard will add member initialization here
	m_nExposure = 0;
	//}}AFX_DATA_INIT

	m_sName = c_szCDodge1PropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_DODGE1 );
}


void CDodge1PropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDodge1PropPage)
	DDX_Control(pDX, IDC_SLIDER_EXPOSURE, m_sliderExposure);
	DDX_Control(pDX, IDC_VTWIDTHCTRL1, m_pbWidth[0]);
	DDX_Control(pDX, IDC_VTWIDTHCTRL2, m_pbWidth[1]);
	DDX_Control(pDX, IDC_VTWIDTHCTRL3, m_pbWidth[2]);
	DDX_Text(pDX, IDC_EDIT_EXPOSURE, m_nExposure);
	DDV_MinMaxUInt(pDX, m_nExposure, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDodge1PropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CDodge1PropPage)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_EXPOSURE, OnChangeEditExposure)
	ON_BN_CLICKED(IDC_RADIO_SHADOWS, OnTypeChange)
	ON_BN_CLICKED(IDC_RADIO_MIDTONES, OnTypeChange)
	ON_BN_CLICKED(IDC_RADIO_HIGHLIGHTS, OnTypeChange)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_EDIT_EXPOSURE, OnKillFocus)
	//}}AFX_MSG_MAP
	//---------  For tolltip -------------
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDodge1PropPage message handlers

BEGIN_EVENTSINK_MAP(CDodge1PropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CDodge1PropPage)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL1, 1 /* OnChange */, OnChangeVtwidthctrl1, VTS_NONE)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL2, 1 /* OnChange */, OnChangeVtwidthctrl2, VTS_NONE)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL3, 1 /* OnChange */, OnChangeVtwidthctrl3, VTS_NONE)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL1, -600 /* OnClick */, OnOnClickVtwidthctrl1, VTS_NONE)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL2, -600 /* OnClick */, OnOnClickVtwidthctrl2, VTS_NONE)
	ON_EVENT(CDodge1PropPage, IDC_VTWIDTHCTRL3, -600 /* OnClick */, OnOnClickVtwidthctrl3, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDodge1PropPage::OnChangeVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	OnWidth(0);
}

void CDodge1PropPage::OnChangeVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	OnWidth(1);
}

void CDodge1PropPage::OnChangeVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	OnWidth(2);
}

void CDodge1PropPage::OnOnClickVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 0;
	_ManageWidthButtons();
	Validation();
}

void CDodge1PropPage::OnOnClickVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 1;
	_ManageWidthButtons();
	Validation();
}

void CDodge1PropPage::OnOnClickVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 2;
	_ManageWidthButtons();
	Validation();
}

BOOL CDodge1PropPage::OnInitDialog() 
{
	MakeButtons(3);

	CDialog::OnInitDialog();

	_Init("Dodge", false);

	m_sliderExposure.SetRange(0, 100);
	long nType = ::GetValueInt(GetAppUnknown(), "Editor", "DodgeBurnType", 0);
	switch(nType)
	{
	case 1:
		((CButton*)GetDlgItem(IDC_RADIO_MIDTONES))->SetCheck(1);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_RADIO_HIGHLIGHTS))->SetCheck(1);
		break;
	default:
		((CButton*)GetDlgItem(IDC_RADIO_SHADOWS))->SetCheck(1);
		break;
	}
	
	long nExposure = ::GetValueInt(GetAppUnknown(), "Editor", "Exposure", 100);
	_SetEdit(nExposure);
	
	// TODO: Add extra initialization here

	hTip = NULL;
	static char* buf[]={"Edit Box"};
	HWND hDlg=this->m_hWnd;
	hTip = CreateToolTip(hDlg);
	BOOL res = AddTool(hTip,GetDlgItem(IDC_EDIT_EXPOSURE)->m_hWnd,NULL,0,buf[0]);
    EnableToolTip(hTip,FALSE);

	bEditTip=false;

	::SendMessage(GetDlgItem(IDC_EDIT_EXPOSURE)->m_hWnd,EM_LIMITTEXT,3,0);

	old_nExposure=nExposure;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDodge1PropPage::OnDestroy() 
{
	_DeInit();
	Validation();

	CDialog::OnDestroy();
}

void CDodge1PropPage::_SetEdit(int nVal)
{
	CString str;
	str.Format("%d", nVal);
	GetDlgItem(IDC_EDIT_EXPOSURE)->SetWindowText(str);
}

void CDodge1PropPage::OnChangeEditExposure() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDIT_EXPOSURE)->GetWindowText(str);
	old_nExposure=m_nExposure;
	m_nExposure=atoi(str);
	if(m_nExposure>m_sliderExposure.GetRangeMax() || m_nExposure<m_sliderExposure.GetRangeMin() || str=="")
	{
		bEditTip=true;
	}
	else
	{
		bEditTip=false;
	if(m_sliderExposure.GetPos() != m_nExposure)
		m_sliderExposure.SetPos(m_nExposure);
	}

	::SetValue(GetAppUnknown(), "Editor", "Exposure", _variant_t((long)m_sliderExposure.GetPos()));
	UpdateAction();	

	if(bEditTip)
	{
		CString s;
		s.Format("Min=%d, max=%d", m_sliderExposure.GetRangeMin(), m_sliderExposure.GetRangeMax());
		UpdateTipText(hTip,GetDlgItem(IDC_EDIT_EXPOSURE)->m_hWnd,0,s);
		//BT5275
		/*CRect r;
		GetDlgItem(IDC_EDIT_EXPOSURE)->GetWindowRect(r);
		::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
	}
	GetDlgItem(IDC_EDIT_EXPOSURE)->Invalidate();
	GetDlgItem(IDC_EDIT_EXPOSURE)->UpdateWindow();
}

void CDodge1PropPage::OnTypeChange() 
{
	// TODO: Add your control notification handler code here
	long nType = 0;
	if(((CButton*)GetDlgItem(IDC_RADIO_MIDTONES))->GetCheck() == 1)
		nType = 1;
	else if(((CButton*)GetDlgItem(IDC_RADIO_HIGHLIGHTS))->GetCheck() == 1)
		nType = 2;

	::SetValue(GetAppUnknown(), "Editor", "DodgeBurnType", _variant_t(nType));
	UpdateAction();	
	Validation();
}

void CDodge1PropPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(pScrollBar->GetSafeHwnd() == m_sliderExposure.GetSafeHwnd())
	{
		_SetEdit(m_sliderExposure.GetPos());
	}
	CPropertyPageBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

HBRUSH CDodge1PropPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, uCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_EXPOSURE)
	{
		CString str;
		GetDlgItem(IDC_EDIT_EXPOSURE)->GetWindowText(str);
		if(m_nExposure>m_sliderExposure.GetRangeMax()||m_nExposure<m_sliderExposure.GetRangeMin()||str=="")
		{
			EnableToolTip(hTip,TRUE);
			pDC->SetBkColor(RGB(255, 60, 60));
      		hbr = ::CreateSolidBrush(RGB(255, 60, 60));
		}
		else
		{
			EnableToolTip(hTip,FALSE);
			pDC->SetBkColor(RGB(255, 255, 255));
            hbr = ::CreateSolidBrush(RGB(255, 255, 255));
	   }
	}
	return hbr;
}

void CDodge1PropPage::OnKillFocus()
{
	CString str;
	GetDlgItem(IDC_EDIT_EXPOSURE)->GetWindowText(str);
	m_nExposure=atoi(str);
	if(m_nExposure>m_sliderExposure.GetRangeMax())
		m_nExposure=m_sliderExposure.GetRangeMax();
	else
		if(m_nExposure<m_sliderExposure.GetRangeMin())
			m_nExposure=m_sliderExposure.GetRangeMin();
	str.Format("%d",m_nExposure);
	GetDlgItem(IDC_EDIT_EXPOSURE)->SetWindowText(str);
}

BOOL CDodge1PropPage::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText=" ";

	_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));

	*pResult = 0;
	return TRUE;
}

//-------------------------------------------------------------
HWND APIENTRY CreateToolTip(HWND hWndParent)
{
    InitCommonControls();
    HWND hTip = CreateWindowEx(
        0,TOOLTIPS_CLASS,
        0,0,0,0,0,0,
        hWndParent,0,0,0);
    return hTip;
}

//-------------------------------------------------------------
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool)
{
    ZeroMemory(ti,sizeof(TOOLINFO));
    ti->cbSize = sizeof(TOOLINFO);
    if(!nIDTool)
    {
        ti->hwnd   = GetParent(hWnd);
        ti->uFlags = TTF_IDISHWND;
        ti->uId    = (UINT)hWnd;
    }
    else
    {
        ti->hwnd   = hWnd;
        ti->uFlags = 0;
        ti->uId    = nIDTool;
    }
}

//-------------------------------------------------------------
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr, UINT nIDTool, LPCTSTR szText)
{
    TOOLINFO ti;
    RECT     r = {0,0,0,0};

    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.hinst  = (HINSTANCE)GetModuleHandle(NULL);
    ti.uFlags |= TTF_SUBCLASS | TTF_TRANSPARENT;
    ti.lpszText = LPSTR( szText ? szText : LPSTR_TEXTCALLBACK );
    if(!(ti.uFlags & TTF_IDISHWND))
    {
        if(!pr)
        {
            pr = &r;
            GetClientRect(hWnd, pr);
        }
        memcpy(&ti.rect, pr, sizeof(RECT));
    }
    BOOL res = SendMessage(hTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    return res;
}

//-------------------------------------------------------------
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPCTSTR lpszText)
{
    TOOLINFO ti;
    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.lpszText = LPSTR( lpszText ? lpszText : LPSTR_TEXTCALLBACK );
    SendMessage(hTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
}

//-------------------------------------------------------------
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText)
{
    TOOLINFO ti;
    if(!szText)
        return;
    *szText = 0;
    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.lpszText = szText;
    SendMessage(hTip, TTM_GETTEXT, 0, (LPARAM)&ti);
}

//-------------------------------------------------------------
void APIENTRY EnableToolTip(HWND hTip, BOOL activate)
{
    SendMessage(hTip, TTM_ACTIVATE, activate, 0);
}

void CDodge1PropPage::Validation()
{
	if(old_nExposure!=m_nExposure)
	{
		if(m_nExposure>m_sliderExposure.GetRangeMax() || 
			m_nExposure<m_sliderExposure.GetRangeMin())
		{
			_SetEdit(old_nExposure);
		}
	}
}