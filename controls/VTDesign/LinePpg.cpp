// LinePpg.cpp : Implementation of the CVTLinePropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "LinePpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTLinePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTLinePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTLinePropPage)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTLinePropPage, "VTDESIGN.VTDLinePropPage.1",
	0xbc127b0c, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTLinePropPage::CVTLinePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTLinePropPage

BOOL CVTLinePropPage::CVTLinePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDLINE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTLinePropPage::CVTLinePropPage - Constructor

CVTLinePropPage::CVTLinePropPage() :
	COlePropertyPage(IDD, IDS_VTDLINE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTLinePropPage)
	m_nOrientation = 0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTLinePropPage::DoDataExchange - Moves data between page and properties

void CVTLinePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTLinePropPage)
	DDP_Text(pDX, IDC_ORIENTATION, m_nOrientation, _T("Orientation") );
	DDX_Text(pDX, IDC_ORIENTATION, m_nOrientation);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CVTLinePropPage message handlers

void CVTLinePropPage::OnLeft() 
{	
	CWnd* pWnd = GetDlgItem(IDC_ORIENTATION);
	if( !pWnd || !pWnd->GetSafeHwnd() )
		return;


	if( m_nOrientation - 1 >= 0 )
		m_nOrientation--;
	else
		m_nOrientation = 7;

	
	CString str;
	str.Format( "%d", m_nOrientation );

	pWnd->SetWindowText( (LPCSTR)str );	

	UpdateData( TRUE );

	CWnd* pWndDT = GetDlgItem( IDC_DRAW_TO );
	if( pWndDT )
	{
		CRect rc;
		pWndDT->GetWindowRect( &rc );
		ScreenToClient( &rc );
		InvalidateRect( &rc );
		UpdateWindow();
	}

}

/////////////////////////////////////////////////////////////////////////////
void CVTLinePropPage::OnRight() 
{
	
	CWnd* pWnd = GetDlgItem(IDC_ORIENTATION);
	if( !pWnd || !pWnd->GetSafeHwnd() )
		return;	


	if( m_nOrientation + 1 <= 7 )
		m_nOrientation++;
	else
		m_nOrientation = 0;

	
	CString str;
	str.Format( "%d", m_nOrientation );

	pWnd->SetWindowText( (LPCSTR)str );
	
	UpdateData( TRUE );

	CWnd* pWndDT = GetDlgItem( IDC_DRAW_TO );
	if( pWndDT )
	{
		CRect rc;
		pWndDT->GetWindowRect( &rc );
		ScreenToClient( &rc );
		InvalidateRect( &rc );
		UpdateWindow();
	}
}

void CVTLinePropPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CWnd* pWndDT = GetDlgItem( IDC_DRAW_TO );
	if( !pWndDT || !pWndDT->GetSafeHwnd() )
		return;

	CRect rc;
	pWndDT->GetWindowRect( &rc );
	ScreenToClient( &rc );

	CPen pen( PS_SOLID, 1, RGB(0,0,0) );

	CPoint pt = rc.CenterPoint();

	CPoint pt2 = pt;

	if( m_nOrientation == 0 )
		pt2.y = rc.top;
	else
	if( m_nOrientation == 1 )
		pt2 = CPoint( rc.right, rc.top );
	else
	if( m_nOrientation == 3 )
		pt2 = CPoint( rc.right, rc.bottom );
	else
	if( m_nOrientation == 4 )
		pt2.y = rc.bottom;
	else
	if( m_nOrientation == 5 )
		pt2 = CPoint( rc.left, rc.bottom );
	else
	if( m_nOrientation == 6 )
		pt2.x = rc.left;
	else
	if( m_nOrientation == 7 )
		pt2 = CPoint( rc.left, rc.top);
	else
		pt2.x = rc.right;
	//if( m_nOrientation == 2 )
		



	CPen* pOldPen = dc.SelectObject( &pen );

	dc.MoveTo( pt.x, pt.y );
	dc.LineTo( pt2.x, pt2.y );


	dc.SelectObject( pOldPen );
}
