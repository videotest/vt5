// SelPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "selection.h"
#include "SelPropPage.h"
#include "selectactions.h"
#include "DDXVEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char szMagick[] = _T("Selection");
const char szMagickInt[] = _T("MagickInt");
const char szDynamicBrightness[] = _T("DynamicBrightness");

CValueLimits g_BrightLim(0,100);
CValueLimits g_SmoothLim(0,300);

IMPLEMENT_DYNCREATE(CSelPropPage, CDialog)
IMPLEMENT_DYNCREATE(CFixedSelRectPropPage, CDialog);

// {0D742D74-36DF-4162-B1B0-31375FF6E1ED}
GUARD_IMPLEMENT_OLECREATE(CSelPropPage, szSelectionPropPageProgID, 
0xd742d74, 0x36df, 0x4162, 0xb1, 0xb0, 0x31, 0x37, 0x5f, 0xf6, 0xe1, 0xed);

// {107999F4-E078-44d5-83F1-F5A87C9C81CD}
GUARD_IMPLEMENT_OLECREATE(CFixedSelRectPropPage, szFixedSelPropPageProgID, 
0x107999f4, 0xe078, 0x44d5, 0x83, 0xf1, 0xf5, 0xa8, 0x7c, 0x9c, 0x81, 0xcd);


BEGIN_INTERFACE_MAP(CSelPropPage, CPropertyPageBase)
	INTERFACE_PART(CSelPropPage, IID_IMsgListener,	MsgList	)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPropPage dialog


CSelPropPage::CSelPropPage(CWnd* pParent /*=NULL*/)
	: CPropertyPageBase(CSelPropPage::IDD)
{
	//{{AFX_DATA_INIT(CSelPropPage)
	m_bDynamic = FALSE;
	m_nBright = 0;
	m_nSmooth = 0;
	//}}AFX_DATA_INIT
	m_bBrightValid = true;
	m_bSmoothValid = true;
	m_bAttached = false;

	m_sName = c_szCSelPropPage;
	m_sUserName.LoadString(IDS_PROPPAGE_SELECTION);

}


void CSelPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPropPage)
	DDX_Control(pDX, IDC_SPIN_SMOOTH, m_spinSmooth);
	DDX_Control(pDX, IDC_SPIN_MAGICK, m_spinMagick);
	DDX_Control(pDX, IDC_PB_MODE_PLUS, m_pbModePlus);
	DDX_Control(pDX, IDC_PB_MODE_MINUS, m_pbModeMinus);
	DDX_Control(pDX, IDC_PB_TYPE_1, m_pbType1);
	DDX_Control(pDX, IDC_PB_TYPE_2, m_pbType2);
	DDX_Check(pDX, IDC_DYNAMIC_BRIGHTNESS, m_bDynamic);
	DDX_Text_Ex(pDX, IDC_EDIT_MAGICK, m_nBright, &m_bBrightValid);
//	DDV_MinMaxLong(pDX, m_nBright, 0, 100);
	DDX_Text_Ex(pDX, IDC_EDIT_SMOOTH, m_nSmooth, &m_bSmoothValid);
//	DDV_MinMaxLong(pDX, m_nSmooth, 0, 300);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CSelPropPage)
	ON_BN_CLICKED(IDC_DYNAMIC_BRIGHTNESS, OnDynamicBrightness)
	ON_EN_CHANGE(IDC_EDIT_MAGICK, OnChangeEditMagick)
	ON_EN_CHANGE(IDC_EDIT_SMOOTH, OnChangeEditSmooth)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_EDIT_MAGICK, OnKillfocusMagic)
	ON_EN_KILLFOCUS(IDC_EDIT_SMOOTH, OnKillfocusSmooth)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPropPage message handlers

BOOL CSelPropPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DoLoadSettings();

	m_spinMagick.SetRange(0, 100);
	m_spinSmooth.SetRange(0, 300);

	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(0, TTS_ALWAYSTIP);
	m_ToolTip.SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
	m_ToolTip.SendMessage( TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX);
	m_ToolTip.SendMessage( TTM_SETDELAYTIME,   TTDT_INITIAL, 0);
	m_ToolTip.SendMessage( TTM_SETDELAYTIME,   TTDT_RESHOW,  0);
//	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_MAGICK), _T("0-100"), NULL, 0);
//	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_SMOOTH), _T("0-300"), NULL, 0);
	m_ToolTip.Activate(TRUE);

	if (!m_bAttached)
	{
		IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
		IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
		if (sptrMainFrame != 0)
			sptrMainFrame->AttachMsgListener(GetControllingUnknown());
		m_bAttached = true;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_EVENTSINK_MAP(CSelPropPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CSelPropPage)
	ON_EVENT(CSelPropPage, IDC_PB_MODE_PLUS, -600 /* Click */, OnClickPbModePlus, VTS_NONE)
	ON_EVENT(CSelPropPage, IDC_PB_MODE_MINUS, -600 /* Click */, OnClickPbModeMinus, VTS_NONE)
	ON_EVENT(CSelPropPage, IDC_PB_TYPE_1, -600 /* Click */, OnClickPbType1, VTS_NONE)
	ON_EVENT(CSelPropPage, IDC_PB_TYPE_2, -600 /* Click */, OnClickPbType2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSelPropPage::OnClickPbModePlus() 
{
	if(m_pbModePlus.IsPressed() == TRUE)
	{
		CActionSelectBase::s_defMode = CActionSelectBase::fdmAdd;
		m_pbModeMinus.ResetPressedState();
	}
	else
		CActionSelectBase::s_defMode = CActionSelectBase::fdmNew;
	SetValue( GetAppUnknown(), "\\Selection", "DefMode", (long)(CActionSelectBase::s_defMode) );
}

void CSelPropPage::OnClickPbModeMinus() 
{
	if(m_pbModeMinus.IsPressed() == TRUE)
	{
		CActionSelectBase::s_defMode = CActionSelectBase::fdmRemove;
		m_pbModePlus.ResetPressedState();
	}
	else
		CActionSelectBase::s_defMode = CActionSelectBase::fdmNew;
	SetValue( GetAppUnknown(), "\\Selection", "DefMode", (long)(CActionSelectBase::s_defMode) );
}

void CSelPropPage::_ManageTypeButtons()
{
	if(m_pbType1.m_hWnd == 0 || m_pbType2.m_hWnd == 0)
		return;

	m_pbType1.ResetPressedState();
	m_pbType2.ResetPressedState();
	if( GetValueInt( GetAppUnknown(), "\\Selection", "FinalizeByButtonUp", true ) )
		m_pbType1.SetPressedState();
	else
		m_pbType2.SetPressedState();
}

void CSelPropPage::OnClickPbType1() 
{
	SetValue( GetAppUnknown(), "\\Selection", "FinalizeByButtonUp", (long)true );
	_ManageTypeButtons();
}

void CSelPropPage::OnClickPbType2() 
{
	SetValue( GetAppUnknown(), "\\Selection", "FinalizeByButtonUp", (long)false );
	_ManageTypeButtons();
}

void CSelPropPage::OnDynamicBrightness() 
{
	if(m_pbModePlus.m_hWnd)
	{
		UpdateData();
		::SetValue(::GetAppUnknown(), szMagick, szDynamicBrightness, _variant_t((long)(m_bDynamic ? 1 : 0)));
		UpdateAction();
	}
}

void CSelPropPage::OnChangeEditMagick() 
{
	if(m_pbModePlus.m_hWnd)
	{
		UpdateData();
		SetValue(GetAppUnknown(), szMagick, szMagickInt,_variant_t(g_BrightLim.Validate(m_nBright)));
		UpdateAction();
		::InvalidateRect(::GetDlgItem(m_hWnd, IDC_EDIT_MAGICK), NULL, TRUE);
		bool bValid = m_bBrightValid&&g_BrightLim.Check(m_nBright);
		CString sToolTip = g_BrightLim.FormatTipString();
		SetToolTip(m_ToolTip, GetDlgItem(IDC_EDIT_MAGICK), !bValid, sToolTip/*_T("0-100")*/);
	}
}

void CSelPropPage::OnKillfocusMagic()
{
	UpdateData();
	m_nBright = m_bBrightValid?g_BrightLim.Validate(m_nBright):g_BrightLim.Default();
	UpdateData(FALSE);
	m_bBrightValid = true;
	::InvalidateRect(::GetDlgItem(m_hWnd, IDC_EDIT_MAGICK), NULL, TRUE);
	SetToolTip(m_ToolTip, GetDlgItem(IDC_EDIT_MAGICK), false, g_BrightLim.FormatTipString());
}

void CSelPropPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;

	{
	CWnd* pWnd = 0;
	pWnd = GetDlgItem(IDC_STATIC_MAGIC);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC_BD);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_EDIT_MAGICK);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_SPIN_MAGICK);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_DYNAMIC_BRIGHTNESS);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	if(CActionSelectBase::s_bMagick)
	{
		CString str;
		str.Format("%d", GetValueInt(GetAppUnknown(), szMagick, szMagickInt, 0));
		pWnd = GetDlgItem(IDC_EDIT_MAGICK);
		if(pWnd)pWnd->SetWindowText(str);
		pWnd = GetDlgItem(IDC_DYNAMIC_BRIGHTNESS);
		if(pWnd)((CButton*)pWnd)->SetCheck(::GetValueInt(::GetAppUnknown(), szMagick, szDynamicBrightness, 0) == 0 ? FALSE : TRUE);
	}
	}

	{
	CWnd* pWnd = 0;
	pWnd = GetDlgItem(IDC_STATIC_SMOOTH);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand || CActionSelectBase::s_bMagick ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_EDIT_SMOOTH);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand || CActionSelectBase::s_bMagick? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_SPIN_SMOOTH);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand || CActionSelectBase::s_bMagick? SW_SHOW : SW_HIDE);
	if(CActionSelectBase::s_bFreehand || CActionSelectBase::s_bMagick)
	{
		CString str;
		str.Format("%d", GetValueInt(GetAppUnknown(), szMagick, "SmoothStrength", 0));
		pWnd = GetDlgItem(IDC_EDIT_SMOOTH);
		if(pWnd)pWnd->SetWindowText(str);
	}
	}

	{
	CWnd* pWnd = 0;
	pWnd = GetDlgItem(IDC_STATIC_TYPE);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC_TYPE2);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_PB_TYPE_1);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand ? SW_SHOW : SW_HIDE);
	pWnd = GetDlgItem(IDC_PB_TYPE_2);
	if(pWnd)pWnd->ShowWindow(CActionSelectBase::s_bFreehand ? SW_SHOW : SW_HIDE);
	}


	CActionSelectBase::s_defMode = 
		(CActionSelectBase::FrameDefMode)
		(GetValueInt( GetAppUnknown(), "\\Selection", "DefMode", CActionSelectBase::s_defMode ));
	m_pbModePlus.ResetPressedState();
	m_pbModeMinus.ResetPressedState();
	if(CActionSelectBase::s_defMode == CActionSelectBase::fdmAdd)
	m_pbModePlus.SetPressedState();
	else if(CActionSelectBase::s_defMode == CActionSelectBase::fdmRemove)
	m_pbModeMinus.SetPressedState();

	_ManageTypeButtons();
}

void CSelPropPage::OnChangeEditSmooth() 
{
	if(m_pbModePlus.m_hWnd)
	{
		UpdateData();
		SetValue(GetAppUnknown(), szMagick, "SmoothStrength", _variant_t(g_SmoothLim.Validate(m_nSmooth)));
		UpdateAction();
		::InvalidateRect(::GetDlgItem(m_hWnd, IDC_EDIT_SMOOTH), NULL, TRUE);
		bool bValid = m_bSmoothValid&&g_SmoothLim.Check(m_nSmooth);
		CString sToolTip = g_SmoothLim.FormatTipString();
		SetToolTip(m_ToolTip, GetDlgItem(IDC_EDIT_SMOOTH), !bValid, sToolTip);
	}
}

void CSelPropPage::OnKillfocusSmooth()
{
	UpdateData();
	m_nSmooth = m_bSmoothValid?g_SmoothLim.Validate(m_nSmooth):g_SmoothLim.Default();
	UpdateData(FALSE);
	m_bSmoothValid = true;
	::InvalidateRect(::GetDlgItem(m_hWnd, IDC_EDIT_SMOOTH), NULL, TRUE);
	SetToolTip(m_ToolTip, GetDlgItem(IDC_EDIT_SMOOTH), false, g_SmoothLim.FormatTipString());
}



HBRUSH CSelPropPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   // Call the base class implementation first! Otherwise, it may
   // undo what we're trying to accomplish here.
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // Are we painting the IDC_MYSTATIC control? We can use
   // CWnd::GetDlgCtrlID() to perform the most efficient test.
   int id = pWnd->GetDlgCtrlID();
   if (id == IDC_EDIT_MAGICK || id == IDC_EDIT_SMOOTH)
   {
      // Set the background mode for text to transparent 
      // so background will show thru.
      pDC->SetBkMode(TRANSPARENT);
      // Return handle to our CBrush object
	  if (m_brush.m_hObject == NULL)
		  m_brush.CreateSolidBrush(COLOR_INVALID_VALUE);
	  if (id == IDC_EDIT_MAGICK && (!m_bBrightValid || !g_BrightLim.Check(m_nBright)) ||
		  id == IDC_EDIT_SMOOTH && (!m_bSmoothValid || !g_SmoothLim.Check(m_nSmooth)) )
	      hbr = m_brush;
   }

   return hbr;
}

LRESULT CSelPropPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

/*	switch( message )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:				 
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{

			MSG MsgCopy = {0};
			MsgCopy.hwnd = m_hWnd;
			MsgCopy.message = message;
			MsgCopy.wParam = wParam;
			MsgCopy.lParam = lParam;
			MsgCopy.time = 0;
			MsgCopy.pt.x = ( (int)(short)LOWORD(lParam) );
			MsgCopy.pt.y = ( (int)(short)HIWORD(lParam) );
			m_ToolTip.RelayEvent( &MsgCopy );
		}
	}*/

	return __super::WindowProc(message, wParam, lParam);
}

void CSelPropPage::OnDestroy() 
{
	if (m_bAttached)
	{
		IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
		IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
		if (sptrMainFrame != 0)
			sptrMainFrame->DetachMsgListener(GetControllingUnknown());
		m_bAttached = false;
	}
	m_ToolTip.DestroyToolTipCtrl();

	__super::OnDestroy();
}

BOOL CSelPropPage::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if (!pmsg)return 0;

	switch( pmsg->message )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:				 
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			MSG MsgCopy = *pmsg;
			m_ToolTip.RelayEvent( &MsgCopy );
		}
	}

	return false;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// CFixedSelPropPage dialog
CFixedSelRectPropPage::CFixedSelRectPropPage():CFixedSelectPage( IDD_PROPPAGE_FIXED_SELECTION )
{
	m_sName = c_szCSelFixedRectPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_FIXED_SELECTION );
}
const char *CFixedSelRectPropPage::GetWidthEntry()const
{	return _T("RectCX"); }
const char *CFixedSelRectPropPage::GetHeightEntry()const
{	return _T("RectCY"); }

////////////////////////////////////////////////////////////////////////////////////////////////
const char *CFixedSelectPage::GetSection()const
{	return _T("\\SelectEditor");}

CFixedSelectPage::CFixedSelectPage( UINT nID )	: CPropertyPageBase( nID )
{
	//{{AFX_DATA_INIT(CFixedObjectPage)
	//}}AFX_DATA_INIT
	m_nWidth = 100;
	m_nHeight = 100;
	m_bUseCalibration = 0;
	_OleLockApp( this );
}

CFixedSelectPage::~CFixedSelectPage()
{
	_OleUnlockApp( this );
}


void CFixedSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixedSelectPage)
	DDX_Control(pDX, IDC_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_HEIGH, m_editHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixedSelectPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CFixedSelectPage)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_HEIGH, OnChangeSizes)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeSizes)
	ON_BN_CLICKED(IDC_CALIBR, OnChangeSizes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedSelectPage message handlers

void CFixedSelectPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPropertyPageBase::OnVScroll(nSBCode, nPos, pScrollBar);

	_UpdateValues();

	UpdateAction();
}

void CFixedSelectPage::OnChangeSizes() 
{
	if( !IsInitialized() )
		return;
	CString	strWidth, strHeight;
	m_editWidth.GetWindowText( strWidth );
	m_editHeight.GetWindowText( strHeight );

	long	w, h;

	if( ::sscanf( strHeight, "%d", &h ) == 1 )m_nHeight = h;
	if( ::sscanf( strWidth, "%d", &w ) == 1 )m_nWidth = w;
	m_bUseCalibration = IsDlgButtonChecked( IDC_CALIBR )!=0;

	_UpdateValues();

	UpdateAction();
}

BOOL CFixedSelectPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();

	m_nWidth = ::GetValueInt( ::GetAppUnknown(), GetSection(), GetWidthEntry(), m_nWidth );
	m_nHeight = ::GetValueInt( ::GetAppUnknown(), GetSection(), GetHeightEntry(), m_nHeight );
	m_bUseCalibration = ::GetValueInt( ::GetAppUnknown(), GetSection(), GetCalibrEntry(), m_bUseCalibration  )!=0;

	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_HEIGHT ))->SetRange( 1, 1000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH ))->SetRange( 1, 1000 );

	_UpdateEditors();
	_UpdateValues();
	UpdateAction();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedSelectPage::_UpdateEditors()
{
	CString	str1, str2;
	str1.Format( "%d", m_nWidth );
	str2.Format( "%d", m_nHeight );

	CheckDlgButton( IDC_CALIBR, m_bUseCalibration );

	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_HEIGHT ))->SetPos( m_nHeight );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH ))->SetPos( m_nWidth );

	m_editWidth.SetWindowText( str1 );
	m_editHeight.SetWindowText( str2 );

}

void CFixedSelectPage::_UpdateValues()
{
	::SetValue( ::GetAppUnknown(), GetSection(), GetWidthEntry(), m_nWidth );
	::SetValue( ::GetAppUnknown(), GetSection(), GetHeightEntry(), m_nHeight );
	::SetValue( ::GetAppUnknown(), GetSection(), GetCalibrEntry(), (long)m_bUseCalibration );
	
}