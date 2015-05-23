// objpages.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "objpages.h"
#include "PushButton.h"

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixedObjectPage dialog
IMPLEMENT_DYNCREATE(CFixedEllipsePage, CDialog);
IMPLEMENT_DYNCREATE(CFixedRectPage, CDialog);
IMPLEMENT_DYNCREATE(CMagickObjectPage, CDialog);
IMPLEMENT_DYNCREATE(CEditObjectPage, CDialog);
IMPLEMENT_DYNCREATE(CObjectGeneralPage, CDialog);
IMPLEMENT_DYNCREATE(CObjectFreePage, CObjectGeneralPage);

// {4E151FD2-A0B6-11d3-A6A3-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CFixedEllipsePage, szObjectsFixedEllipsePropPageProgID,
0x4e151fd2, 0xa0b6, 0x11d3, 0xa6, 0xa3, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {4E151FD4-A0B6-11d3-A6A3-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CFixedRectPage, szObjectsFixedRectPropPageProgID,
0x4e151fd4, 0xa0b6, 0x11d3, 0xa6, 0xa3, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {AB0204F3-A96A-11d3-A6BB-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMagickObjectPage, szObjectsMagickObjectPropPageProgID,
0xab0204f3, 0xa96a, 0x11d3, 0xa6, 0xbb, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {BE875032-64CD-41d1-898C-57E236DDA9ED}
GUARD_IMPLEMENT_OLECREATE(CEditObjectPage, szObjectsEditObjectPropPageProgID,
0xbe875032, 0x64cd, 0x41d1, 0x89, 0x8c, 0x57, 0xe2, 0x36, 0xdd, 0xa9, 0xed);
// {23C40DBA-713C-4acb-84F5-F493B862564A}
GUARD_IMPLEMENT_OLECREATE(CObjectGeneralPage, szObjectsGeneralObjectPropPageProgID,
0x23c40dba, 0x713c, 0x4acb, 0x84, 0xf5, 0xf4, 0x93, 0xb8, 0x62, 0x56, 0x4a);
// {A167FF9B-1A13-437e-AE6F-08A919ED7E37}
GUARD_IMPLEMENT_OLECREATE(CObjectFreePage, szObjectsFreeObjectPropPageProgID,
0xa167ff9b, 0x1a13, 0x437e, 0xae, 0x6f, 0x8, 0xa9, 0x19, 0xed, 0x7e, 0x37);

//For ToolTip
HWND APIENTRY CreateToolTip(HWND hWndParent); 
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool = 0); 
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr = NULL, UINT nIDTool = 0, LPCTSTR szText = NULL); 
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool = 0, LPCTSTR lpszText = NULL);
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText);
void APIENTRY EnableToolTip(HWND hTip, BOOL activate);

////////////////////////////////////////////////////////////////////////////////////////////////
CFixedEllipsePage::CFixedEllipsePage():CFixedObjectPage( IDD_ELLIPSE_FIXED )
{
	m_sName = c_szCFixedEllipsePage;
	m_sUserName.LoadString( IDS_FIX_ELPS_DLG );
}

const char *CFixedEllipsePage::GetWidthEntry()const
{	return _T("EllipseCX"); }
const char *CFixedEllipsePage::GetHeightEntry()const
{	return _T("EllipseCY"); }
////////////////////////////////////////////////////////////////////////////////////////////////
CFixedRectPage::CFixedRectPage():CFixedObjectPage( IDD_RECT_FIXED )
{
	m_sName = c_szCFixedRectPage;
	m_sUserName.LoadString( IDS_FIX_RECT_DLG );
}
const char *CFixedRectPage::GetWidthEntry()const
{	return _T("RectCX"); }
const char *CFixedRectPage::GetHeightEntry()const
{	return _T("RectCY"); }


////////////////////////////////////////////////////////////////////////////////////////////////
const char *CFixedObjectPage::GetSection()const
{	return _T("\\ObjectEditor");}

CFixedObjectPage::CFixedObjectPage( UINT nID )	: CPropertyPageBase( nID )
{
	//{{AFX_DATA_INIT(CFixedObjectPage)
	//}}AFX_DATA_INIT
	m_nWidth = 100;
	m_nHeight = 100;
	m_bUseCalibration = 0;
	_OleLockApp( this );
}

CFixedObjectPage::~CFixedObjectPage()
{
	_OleUnlockApp( this );
}


void CFixedObjectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixedObjectPage)
	DDX_Control(pDX, IDC_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_HEIGH, m_editHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixedObjectPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CFixedObjectPage)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_HEIGH, OnChangeSizes)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeSizes)
	ON_BN_CLICKED(IDC_CALIBR, OnChangeSizes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedObjectPage message handlers

void CFixedObjectPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPropertyPageBase::OnVScroll(nSBCode, nPos, pScrollBar);

	_UpdateValues();

	UpdateAction();
}

void CFixedObjectPage::OnChangeSizes() 
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

BOOL CFixedObjectPage::OnInitDialog() 
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

void CFixedObjectPage::_UpdateEditors()
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

void CFixedObjectPage::_UpdateValues()
{
	::SetValue( ::GetAppUnknown(), GetSection(), GetWidthEntry(), m_nWidth );
	::SetValue( ::GetAppUnknown(), GetSection(), GetHeightEntry(), m_nHeight );
	::SetValue( ::GetAppUnknown(), GetSection(), GetCalibrEntry(), (long)m_bUseCalibration );
	
}

/////////////////////////////////////////////////////////////////////////////
// CMagickObjectPage property page


CMagickObjectPage::CMagickObjectPage() : CPropertyPageBase(CMagickObjectPage::IDD)
{
	//{{AFX_DATA_INIT(CMagickObjectPage)
	m_nBriInt = 0;
	m_bDynamicBrightness = FALSE;
	//}}AFX_DATA_INIT
	_OleLockApp( this );

	m_sName = c_szCMagickObjectPage;
	m_sUserName.LoadString( IDS_MAGICK );
}

CMagickObjectPage::~CMagickObjectPage()
{
	_OleUnlockApp( this );
}

void CMagickObjectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMagickObjectPage)
	DDX_Text(pDX, IDC_BRI_INT, m_nBriInt);
	DDX_Check(pDX, IDC_DYNAMIC_BRIGHTNESS, m_bDynamicBrightness);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMagickObjectPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CMagickObjectPage)
	ON_EN_CHANGE(IDC_BRI_INT, OnChangeBriInt)
	ON_BN_CLICKED(IDC_DYNAMIC_BRIGHTNESS, OnDynamicBrightness)
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_BRI_INT, OnKillFocusBriInt)
	//}}AFX_MSG_MAP
	//---------  For tolltip -------------
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMagickObjectPage message handlers

BOOL CMagickObjectPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	
	m_nBriInt = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "MagickInt", m_nBriInt );
	m_bDynamicBrightness = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "DynamicBrightness", m_bDynamicBrightness );

	UpdateData( false );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->SetRange( 0, 255 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->SetPos( m_nBriInt );
	
	hTip = NULL;
	static char* buf[]={"Edit Box"};
	HWND hDlg=this->m_hWnd;
	hTip = CreateToolTip(hDlg);
	BOOL res = AddTool(hTip,GetDlgItem(IDC_BRI_INT)->m_hWnd,NULL,0,buf[0]);
    EnableToolTip(hTip,FALSE);

	bEditTip=false;

	::SendMessage(GetDlgItem(IDC_BRI_INT)->m_hWnd,EM_LIMITTEXT,3,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CMagickObjectPage::OnChangeBriInt() 
{
	if( !IsInitialized() )
		return;
	//if( !UpdateData() )
	//	return;
	CString str;
	GetDlgItem(IDC_BRI_INT)->GetWindowText(str);
	m_nBriInt=atoi(str);
	int sp_min, sp_max;
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetRange(sp_min, sp_max);
	if(m_nBriInt>sp_max || m_nBriInt<sp_min || str=="")
	{
		bEditTip=true;
	}
	else
	{
		bEditTip=false;
		UpdateData(TRUE);
		if(((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetPos() != m_nBriInt)
			((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->SetPos(m_nBriInt);
		::SetValue( GetAppUnknown(), "ObjectEditor", "MagickInt", (long)((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetPos());
	}
	
	UpdateAction();

	if(bEditTip)
	{
		CString s;
		s.Format("Min=%d, max=%d", sp_min, sp_max);
		UpdateTipText(hTip,GetDlgItem(IDC_BRI_INT)->m_hWnd,0,s);
		//BT5275
		/*CRect r;
		GetDlgItem(IDC_BRI_INT)->GetWindowRect(r);
		::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
	}
	GetDlgItem(IDC_BRI_INT)->Invalidate();
	GetDlgItem(IDC_BRI_INT)->UpdateWindow();
	//::SetValue( GetAppUnknown(), "ObjectEditor", "MagickInt", (long)m_nBriInt );
	//UpdateAction();
}

HBRUSH CMagickObjectPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, uCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_BRI_INT)
	{
		CString str;
		GetDlgItem(IDC_BRI_INT)->GetWindowText(str);
		int sp_min, sp_max;
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetRange(sp_min, sp_max);
		if(m_nBriInt>sp_max || m_nBriInt<sp_min || str=="")
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

BOOL CMagickObjectPage::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText=" ";

	_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));

	*pResult = 0;
	return TRUE;
}

void CMagickObjectPage::OnDynamicBrightness() 
{
	CButton* pButton = (CButton*)GetDlgItem( IDC_DYNAMIC_BRIGHTNESS );
	if( !pButton )
	{
		ASSERT( FALSE );
		return;
	}

	if( pButton->GetCheck() )
		::SetValue( GetAppUnknown(), "ObjectEditor", "DynamicBrightness", (long)1 );
	else
		::SetValue( GetAppUnknown(), "ObjectEditor", "DynamicBrightness", (long)0 );

}

void CMagickObjectPage::OnKillFocusBriInt() 
{
	CString str;
	GetDlgItem(IDC_BRI_INT)->GetWindowText(str);
	//m_nBriInt=atoi(str);
	int sp_min, sp_max;
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetRange(sp_min, sp_max);
	if(m_nBriInt>sp_max  )
		m_nBriInt=sp_max;
	else
		if(m_nBriInt<sp_min || str=="")
			m_nBriInt=sp_min;
	str.Format("%d",m_nBriInt);
	GetDlgItem(IDC_BRI_INT)->SetWindowText(str);
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->SetPos(m_nBriInt);
	::SetValue( GetAppUnknown(), "ObjectEditor", "MagickInt", (long)((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 ))->GetPos());
}
/////////////////////////////////////////////////////////////////////////////
// CEditObjectPage dialog

UINT	nMergeControlIDS[] = 
{IDC_WIDTH4, IDC_WIDTH3, IDC_WIDTH2, IDC_WIDTH1};
UINT	nSeparateControlIDS[] = 
{IDC_WIDTH1_BACK, IDC_WIDTH2_BACK, IDC_WIDTH3_BACK, IDC_WIDTH4_BACK};
int		nWidths[] = 
{3, 10, 20, 50};

CEditObjectPage::CEditObjectPage()
	: CPropertyPageBase( CEditObjectPage::IDD )
{	//{{AFX_DATA_INIT(CEditObjectPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCEditObjectPage;
	m_sUserName.LoadString( IDS_EDIT_OBJECT_DLG );
}


void CEditObjectPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditObjectPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditObjectPage, CDialog)
	//{{AFX_MSG_MAP(CEditObjectPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditObjectPage message handlers

BEGIN_EVENTSINK_MAP(CEditObjectPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CEditObjectPage)
	ON_EVENT(CEditObjectPage, IDC_WIDTH1_BACK, -600 /* Click */, OnClickWidth1Back, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH2_BACK, -600 /* Click */, OnClickWidth2Back, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH3_BACK, -600 /* Click */, OnClickWidth3Back, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH4_BACK, -600 /* Click */, OnClickWidth4Back, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH1, -600 /* Click */, OnClickWidth1, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH2, -600 /* Click */, OnClickWidth2, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH3, -600 /* Click */, OnClickWidth3, VTS_NONE)
	ON_EVENT(CEditObjectPage, IDC_WIDTH4, -600 /* Click */, OnClickWidth4, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CEditObjectPage::OnClickWidth1Back() 
{
	_SetBackWidth( nWidths[0] );
}

void CEditObjectPage::OnClickWidth2Back() 
{
	_SetBackWidth( nWidths[1] );
}

void CEditObjectPage::OnClickWidth3Back() 
{
	_SetBackWidth( nWidths[2] );
}

void CEditObjectPage::OnClickWidth4Back() 
{
	_SetBackWidth( nWidths[3] );
}

void CEditObjectPage::OnClickWidth1() 
{
	_SetMergeWidth( nWidths[3] );
}

void CEditObjectPage::OnClickWidth2() 
{
	_SetMergeWidth( nWidths[2] );
}

void CEditObjectPage::OnClickWidth3() 
{
	_SetMergeWidth( nWidths[1] );
}

void CEditObjectPage::OnClickWidth4() 
{
	_SetMergeWidth( nWidths[0] );
	
}

BOOL CEditObjectPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	_SetMergeWidth( GetValueInt( GetAppUnknown(), "ObjectEditor", "MergePenWidth", 3 ) );	
	_SetBackWidth( GetValueInt( GetAppUnknown(), "ObjectEditor", "SeparatePenWidth", 3 ) );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditObjectPage::_SetMergeWidth( int nWidth )
{
	for( int n = 0; n < 4; n++ )
	{
		if( nWidths[n] == nWidth )
			break;
	}
	if( n == 4 )n = 0;

	for( int nn = 0; nn < 4; nn++ )
		((CButton*)GetDlgItem( nMergeControlIDS[nn] ) )->SetCheck( n==nn );
	SetValue( GetAppUnknown(), "ObjectEditor", "MergePenWidth", (long)nWidths[n] );	
	UpdateAction();
}

void CEditObjectPage::_SetBackWidth( int nWidth )
{
	for( int n = 0; n < 4; n++ )
	{
		if( nWidths[n] == nWidth )
			break;
	}
	if( n == 4 )n = 0;

	for( int nn = 0; nn < 4; nn++ )
		((CButton*)GetDlgItem( nSeparateControlIDS[nn] ) )->SetCheck( n==nn );
	SetValue( GetAppUnknown(), "ObjectEditor", "SeparatePenWidth", (long)nWidths[n] );	
	UpdateAction();
}
/////////////////////////////////////////////////////////////////////////////
// CObjectGeneralPage dialog


CObjectFreePage::CObjectFreePage():CObjectGeneralPage()
{
	m_bShowType = 1;
	m_sName = c_szCObjectFreePage;
	m_sUserName.LoadString( IDS_FREE_OBJECT );
}


CObjectGeneralPage::CObjectGeneralPage()
		: CPropertyPageBase( IDD_OBJECTS_GENERAL )
{
	//{{AFX_DATA_INIT(CObjectGeneralPage)
	m_nMinSize = 0;
	//}}AFX_DATA_INIT
	m_bShowType = 0;
	m_sName = c_szCObjectGeneralPage;
	m_sUserName.LoadString( IDS_GENERAL );
}


void CObjectGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectGeneralPage)
	DDX_Text(pDX, IDC_MIN, m_nMinSize);
	DDV_MinMaxInt(pDX, m_nMinSize, 0, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectGeneralPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CObjectGeneralPage)
	ON_EN_CHANGE(IDC_MIN, OnChangeMin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectGeneralPage message handlers

BOOL CObjectGeneralPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();

	m_nFreeMode = (FreeMode)::GetValueInt( GetAppUnknown(), "ObjectEditor", "FreeMode", fmClick );
	m_nDefineMode = (DefineMode)::GetValueInt( GetAppUnknown(), "ObjectEditor", "DefineMode", dmNormal );
	m_nMinSize = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "MinSize", 5 );

	((CSpinButtonCtrl*)GetDlgItem( IDC_MIN_SPIN ))->SetRange( 0, 255 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_MIN_SPIN ))->SetPos( m_nMinSize );

	_Update();

	if( !m_bShowType )
	{
		GetDlgItem( IDC_CLICK )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_DBLCLICK )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_STATIC_TYPE )->ShowWindow( SW_HIDE );
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CObjectGeneralPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CObjectGeneralPage)
	ON_EVENT(CObjectGeneralPage, IDC_CLICK, -600 /* Click */, OnClickClick, VTS_NONE)
	ON_EVENT(CObjectGeneralPage, IDC_DBLCLICK, -600 /* Click */, OnClickDblclick, VTS_NONE)
	ON_EVENT(CObjectGeneralPage, IDC_MOD_EADD, -600 /* Click */, OnClickModEadd, VTS_NONE)
	ON_EVENT(CObjectGeneralPage, IDC_MODE_SEP, -600 /* Click */, OnClickModeSep, VTS_NONE)
	ON_EVENT(CObjectGeneralPage, IDC_MODE_SUB, -600 /* Click */, OnClickModeSub, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CObjectGeneralPage::OnClickClick() 
{
	m_nFreeMode = fmClick;
	_Update();
}

void CObjectGeneralPage::OnClickDblclick() 
{
	m_nFreeMode = fmDblClick;
	_Update();
}

void CObjectGeneralPage::OnClickModEadd() 
{
	if( m_nDefineMode == dmAdd )m_nDefineMode = dmNormal;
	else m_nDefineMode = dmAdd;
	_Update();
}

void CObjectGeneralPage::OnClickModeSep() 
{
	if( m_nDefineMode == dmSep )m_nDefineMode = dmNormal;
	else m_nDefineMode = dmSep;
	_Update();
}

void CObjectGeneralPage::OnClickModeSub() 
{
	if( m_nDefineMode == dmSub )m_nDefineMode = dmNormal;
	else m_nDefineMode = dmSub;
	_Update();
}

void CObjectGeneralPage::_Update()
{
	if( !IsInitialized() )
		return;
	::SetValue( ::GetAppUnknown(), "ObjectEditor", "FreeMode", (long)m_nFreeMode );
	::SetValue( ::GetAppUnknown(), "ObjectEditor", "DefineMode", (long)m_nDefineMode );
	::SetValue( GetAppUnknown(), "ObjectEditor", "MinSize", (long)m_nMinSize );

	((CButton*)GetDlgItem( IDC_CLICK ) )->SetCheck( m_nFreeMode == fmClick );
	((CButton*)GetDlgItem( IDC_DBLCLICK ) )->SetCheck( m_nFreeMode == fmDblClick );
	
	((CButton*)GetDlgItem( IDC_MOD_EADD ) )->SetCheck( m_nDefineMode == dmAdd );
	((CButton*)GetDlgItem( IDC_MODE_SUB ) )->SetCheck( m_nDefineMode == dmSub );
	((CButton*)GetDlgItem( IDC_MODE_SEP ) )->SetCheck( m_nDefineMode == dmSep );

	UpdateAction();
}

void CObjectGeneralPage::OnChangeMin() 
{
	UpdateData();
	_Update();	
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