// ColorPickerPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ColorPickerPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PropPage.h"


/////////////////////////////////////////////////////////////////////////////
// CColorWnd
CColorWnd::CColorWnd()
{
	m_color = CLR_NONE;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_hIconCantPickColor = AfxGetApp()->LoadIcon( IDI_CANT_PICK_COLOR );
}

/////////////////////////////////////////////////////////////////////////////
CColorWnd::~CColorWnd()
{
	if( m_hIconCantPickColor )
		::DestroyIcon( m_hIconCantPickColor );
}

/////////////////////////////////////////////////////////////////////////////
void CColorWnd::SetColor( COLORREF color )
{
	m_color = color;
	if( !GetSafeHwnd() )
		return;

	Invalidate();
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CColorWnd, CWnd)
	//{{AFX_MSG_MAP(CColorWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CColorWnd::OnPaint() 
{
	CPaintDC dc(this);
	CRect rcWnd;
	GetClientRect( &rcWnd );

	CDC dcImage;
	dcImage.CreateCompatibleDC( &dc );

	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( &dc, rcWnd.Width() + 1, rcWnd.Height() + 1 );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

	dcImage.FillRect( &CRect(0,0,rcWnd.Width() + 1, rcWnd.Height() + 1), &CBrush( ::GetSysColor(COLOR_BTNFACE) ) );

	if( m_color == CLR_NONE && m_hIconCantPickColor )
	{
		CPoint pt( 0, 0 );
		::DrawIcon( dcImage.m_hDC, pt.x, pt.y, m_hIconCantPickColor );		
	}
	else
	{
		dcImage.FillRect( &CRect(0,0,rcWnd.Width() + 1, rcWnd.Height() + 1), &CBrush( m_color ) );
	}

	

	dc.BitBlt( rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom,
					&dcImage, 0, 0, SRCCOPY );


	dcImage.SelectObject( pOldBitmap );
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorWnd::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CColorPickerPropPage dialog

IMPLEMENT_DYNCREATE(CColorPickerPropPage, CDialog);

// {40667D30-AE63-495f-84E2-F8C314AB70F3}
GUARD_IMPLEMENT_OLECREATE(CColorPickerPropPage, szEditorColorPickerPropPageProgID, 
0x40667d30, 0xae63, 0x495f, 0x84, 0xe2, 0xf8, 0xc3, 0x14, 0xab, 0x70, 0xf3);


BEGIN_INTERFACE_MAP(CColorPickerPropPage, CPropertyPageBase)	
	INTERFACE_PART(CColorPickerPropPage, IID_IColorPickerPropPage, ColorPickerPropPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorPickerPropPage, ColorPickerPropPage);


HRESULT CColorPickerPropPage::XColorPickerPropPage::ChangeColor( DWORD dwRGB, color H, color S, color L )
{
	_try_nested(CColorPickerPropPage, ColorPickerPropPage, ChangeColor)
	{	
		if( !pThis->GetSafeHwnd() )
			return S_FALSE;

		CString strR, strG, strB, strH, strS, strL;
		pThis->m_wndColor.SetColor( dwRGB );
		if( dwRGB != CLR_NONE )
		{
			strR.Format( "%d", GetRValue(dwRGB) );
			strG.Format( "%d", GetGValue(dwRGB) );
			strB.Format( "%d", GetBValue(dwRGB) );

			strH.Format( "%d", H );
			strS.Format( "%d", S );
			strL.Format( "%d", L );
		}

		pThis->GetDlgItem( IDC_EDIT_R )->SetWindowText( strR );
		pThis->GetDlgItem( IDC_EDIT_G )->SetWindowText( strG );
		pThis->GetDlgItem( IDC_EDIT_B )->SetWindowText( strB );

		pThis->GetDlgItem( IDC_EDIT_H )->SetWindowText( strH );
		pThis->GetDlgItem( IDC_EDIT_S )->SetWindowText( strS );
		pThis->GetDlgItem( IDC_EDIT_L )->SetWindowText( strL );


		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
CColorPickerPropPage::CColorPickerPropPage()
	: CPropertyPageBase(CColorPickerPropPage::IDD)	
{
	//{{AFX_DATA_INIT(CColorPickerPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	_OleLockApp( this );
	m_sName = c_szCColorPickerPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_COLOR_PICKER );	
	
}

/////////////////////////////////////////////////////////////////////////////
// CColorPickerPropPage message handlers
BOOL CColorPickerPropPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pWnd = GetDlgItem( IDC_COLOR_WND );
	if( pWnd )
	{
		CRect rc;
		pWnd->GetWindowRect( &rc );
		ScreenToClient( &rc);
		m_wndColor.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, 10023, NULL );
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
CColorPickerPropPage::~CColorPickerPropPage()
{
	_OleUnlockApp( this );
}

/////////////////////////////////////////////////////////////////////////////
void CColorPickerPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPickerPropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPickerPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CColorPickerPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

