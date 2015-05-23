// PageSetup.cpp : implementation file
//

#include "stdafx.h"
#include "Print.h"
#include "PageSetup.h"
#include <Winspool.h>
#include <COMDEF.H>
#include "PrinterSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_PAGE	3

PageSize page[] = 
{
	{"A4",					1,		210.0,		297.0},
	{"A3",					2,		297.0,		420.0},
	{"User defined",		3,		210.0,		297.0}
}	  
;
/////////////////////////////////////////////////////////////////////////////
// CPageSetup dialog


CPageSetup::CPageSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CPageSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageSetup)
	m_fBottom = 0.0;
	m_nOrientation = -1;
	m_nPaperName = -1;
	m_fHeight = 0.0;
	m_fLeft = 0.0;
	m_fTop = 0.0;
	m_fWidth = 0.0;
	m_fRight = 0.0;		
	m_bUseSimpleMode = FALSE;	
	m_nBuildMode = -1;
	//}}AFX_DATA_INIT

	m_bInitDlg = FALSE;
}


void CPageSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSetup)
	DDX_Control(pDX, IDC_PAPER_SIZE, m_paperCtrl);	
	DDX_Text(pDX, IDC_BOTTOM, m_fBottom);	
	DDX_Radio(pDX, IDC_ORIENTATION, m_nOrientation);
	DDX_CBIndex(pDX, IDC_PAPER_SIZE, m_nPaperName);
	DDX_Text(pDX, IDC_HEIGHT, m_fHeight);
	DDX_Text(pDX, IDC_LEFT, m_fLeft);	
	DDX_Text(pDX, IDC_TOP, m_fTop);	
	DDX_Text(pDX, IDC_WIDTH, m_fWidth);
	DDX_Text(pDX, IDC_RIGHT, m_fRight);	
	DDX_Check(pDX, IDC_SIMPLE_MODE, m_bUseSimpleMode);	
	//}}AFX_DATA_MAP
	
	//DDX_CBIndex(pDX, IDC_BUILD_MODE, m_nBuildMode);

}

/*
DDV_MinMaxDouble(pDX, m_fLeft, (double)m_rcPrnMargins.left, (double)m_fWidth / 2 );
DDV_MinMaxDouble(pDX, m_fTop, (double)m_rcPrnMargins.top, (double)m_fHeight / 2 );
DDV_MinMaxDouble(pDX, m_fRight, (double)m_rcPrnMargins.right, (double)m_fWidth / 2 );
DDV_MinMaxDouble(pDX, m_fBottom, (double)m_rcPrnMargins.bottom, (double)m_fHeight / 2 );
*/

/////////////////////////////////////////////////////////////////////////////
void CPageSetup::OnOK() 
{
	UpdateData( TRUE );

	if( m_fWidth <= 0 )
	{
		GetDlgItem(IDC_WIDTH)->SetFocus();
		return;
	}

	if( m_fHeight <= 0 )
	{
		GetDlgItem(IDC_HEIGHT)->SetFocus();
		return;
	}

	if( m_fLeft < m_rcPrnMargins.left || m_fLeft >= m_fWidth )
	{
		GetDlgItem(IDC_LEFT)->SetFocus();
		return;
	}

	if( m_fRight < m_rcPrnMargins.right || m_fRight >= m_fWidth )
	{
		GetDlgItem(IDC_RIGHT)->SetFocus();
		return;
	}

	if( m_fTop < m_rcPrnMargins.top || m_fTop >= m_fHeight )
	{
		GetDlgItem(IDC_TOP)->SetFocus();
		return;
	}

	if( m_fBottom < m_rcPrnMargins.bottom || m_fBottom >= m_fHeight )
	{
		GetDlgItem(IDC_BOTTOM)->SetFocus();
		return;
	}

	if( m_fLeft + m_fRight > m_fWidth )
	{
		GetDlgItem(IDC_LEFT)->SetFocus();
		return;
	}


	if( m_fTop + m_fBottom > m_fHeight )
	{
		GetDlgItem(IDC_TOP)->SetFocus();
		return;
	}

	CDialog::OnOK();
}


BEGIN_MESSAGE_MAP(CPageSetup, CDialog)
	//{{AFX_MSG_MAP(CPageSetup)
	ON_CBN_SELCHANGE(IDC_PAPER_SIZE, OnSelchangePaperSize)
	ON_BN_CLICKED(IDC_ORIENTATION, OnPortrait)
	ON_BN_CLICKED(IDC_RADIO2, OnLandscape)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_WIDTH, OnChangeWidth)
	ON_EN_CHANGE(IDC_HEIGHT, OnChangeHeight)
	ON_EN_CHANGE(IDC_BOTTOM, OnChangeBottom)
	ON_EN_CHANGE(IDC_RIGHT, OnChangeRight)
	ON_EN_CHANGE(IDC_TOP, OnChangeTop)
	ON_EN_CHANGE(IDC_LEFT, OnChangeLeft)	
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSetup message handlers

BOOL CPageSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();

	for( int i=0;i<MAX_PAGE;i++)
	{
		int nIndex = m_paperCtrl.AddString( page[i].szPageName );
		m_paperCtrl.SetItemData( nIndex, page[i].data );
	}

	//m_nPaperName
	UpdateData( FALSE );
	OnSelchangePaperSize();

	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_LEFT ))->SetRange( 0, 10000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_TOP ))->SetRange( 0, 10000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_RIGHT ))->SetRange( 0, 10000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_BOTTOM ))->SetRange( 0, 10000 );

	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH ))->SetRange( 0, 100000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_HEIGHT ))->SetRange( 0, 100000 );

	m_bInitDlg = TRUE;


	CRect rcMargins = g_prnSetup.GetPrinterMargins( 0 );
	m_rcPrnMargins = rcMargins;

	m_nPrnLeft		= rcMargins.left;
	m_nPrnTop		= rcMargins.top;
	m_nPrnRight		= rcMargins.right;
	m_nPrnBottom	= rcMargins.bottom;

	CString str;
	str.Format( "%d", m_nPrnTop );
	GetDlgItem(IDC_PRN_TOP)->SetWindowText( str );
	str.Format( "%d", m_nPrnBottom );
	GetDlgItem(IDC_PRN_BOTTOM)->SetWindowText( str );
	str.Format( "%d", m_nPrnLeft );
	GetDlgItem(IDC_PRN_LEFT)->SetWindowText( str );
	str.Format( "%d", m_nPrnRight );
	GetDlgItem(IDC_PRN_RIGHT)->SetWindowText( str );	

	return TRUE;	            
}

void CPageSetup::SetData( double fWidth, double fHeight,
					double fLeftField, double fTopField, 
					double fRightField, double fBottomField,
					int nOrientation
					)
{
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;

	m_fLeft		= fLeftField;
	m_fTop		= fTopField;
	m_fRight	= fRightField;
	m_fBottom	= fBottomField;

	m_nOrientation = nOrientation;

	int nIndex = -1;

	for(int i=0;i<MAX_PAGE;i++)
	{
		if( nIndex == -1 )
		{
			if( m_nOrientation == 0 )//Portrait
			{
				if( m_fWidth == page[i].fWidth && m_fHeight == page[i].fHeight )
					nIndex = i;
			}
			else //Landscape
			{
				if( m_fWidth == page[i].fHeight && m_fHeight == page[i].fWidth )
					nIndex = i;

			}
		}
	}

	if( nIndex == -1 ) // Not found
		nIndex = MAX_PAGE-1;// User defined

	m_nPaperName = nIndex;

	//UpdateData( FALSE );
}


void CPageSetup::OnSelchangePaperSize() 
{
	UpdateData( TRUE );
	

	int nIndex = m_paperCtrl.GetCurSel();	


	if( nIndex == MAX_PAGE - 1 ) //User defined
	{
		GetDlgItem( IDC_HEIGHT )->EnableWindow( TRUE );
		GetDlgItem( IDC_WIDTH )->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem( IDC_HEIGHT )->EnableWindow( FALSE );
		GetDlgItem( IDC_WIDTH )->EnableWindow( FALSE );
	}

	if( nIndex >= 0 && nIndex < MAX_PAGE ) 
	{
		if( m_nOrientation == 0 )//Portrait
		{
			m_fWidth  = page[nIndex].fWidth;
			m_fHeight = page[nIndex].fHeight;
		}
		else
		{
			m_fWidth  = page[nIndex].fHeight;
			m_fHeight = page[nIndex].fWidth;
		}
	}

	UpdateData( FALSE );

	RepaintPage();
}



void CPageSetup::OnPortrait() 
{
	UpdateData( TRUE );

	double fTemp = m_fWidth;
	m_fWidth = m_fHeight;
	m_fHeight = fTemp;

	UpdateData( FALSE );
	
	RepaintPage();
}

void CPageSetup::OnLandscape() 
{
	UpdateData( TRUE );

	double fTemp = m_fWidth;
	m_fWidth = m_fHeight;
	m_fHeight = fTemp;

	UpdateData( FALSE );

	RepaintPage();

}

void CPageSetup::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	DrawPage( &dc );			
}

void CPageSetup::RepaintPage()
{
	CWnd* pWndStart = GetDlgItem( IDC_START );
	if( !(pWndStart && pWndStart->GetSafeHwnd()) )
		return;

	CWnd* pWndEnd = GetDlgItem( IDC_END );
	if( !(pWndEnd && pWndEnd->GetSafeHwnd()) )
		return;

	CRect rcWndStart, rcWndEnd;
	pWndStart->GetWindowRect( &rcWndStart );
	pWndEnd->GetWindowRect( &rcWndEnd );	
	
	CRect rcWnd = CRect( rcWndStart.left, rcWndStart.top, rcWndEnd.right, rcWndEnd.bottom );
	
	ScreenToClient( &rcWnd );

	InvalidateRect( rcWnd );

}

void CPageSetup::DrawPage( CDC* pDC )
{
	CWnd* pWndStart = GetDlgItem( IDC_START );
	if( !(pWndStart && pWndStart->GetSafeHwnd()) )
		return;

	CWnd* pWndEnd = GetDlgItem( IDC_END );
	if( !(pWndEnd && pWndEnd->GetSafeHwnd()) )
		return;
	
	CRect rcWndStart, rcWndEnd;
	pWndStart->GetWindowRect( &rcWndStart );
	pWndEnd->GetWindowRect( &rcWndEnd );	
	
	CRect rcWnd = CRect( rcWndStart.left, rcWndStart.top, rcWndEnd.right, rcWndEnd.bottom );
	
	ScreenToClient( &rcWnd );

	double fMax = max( m_fWidth, m_fHeight );
	fMax *= 1.2;

	int nMax = (int) fMax;

	int nMapMode = pDC->SetMapMode( MM_ANISOTROPIC );
	pDC->SetWindowExt( nMax, nMax );
	pDC->SetViewportExt( rcWnd.Width(), rcWnd.Height() );
	pDC->SetViewportOrg( 0, 0 );	

	CPoint ptStart;

	if( m_fWidth > m_fHeight ) 
	{
		ptStart.x = (int)(m_fWidth * 0.1); 
		ptStart.y = (int)((nMax - m_fHeight)/2);
	}
	else
	{
		ptStart.x = (int)((nMax - m_fWidth)/2);
		ptStart.y = (int)(m_fHeight * 0.1); 
	}

	CRect rcPage = CRect( ptStart.x, ptStart.y, 
							ptStart.x + (int)m_fWidth, ptStart.y + (int)m_fHeight
							);
		

	CRect rcField = rcPage;
	rcField.DeflateRect( (int)m_fLeft, (int)m_fTop, (int)m_fRight, (int)m_fBottom );

	CRect rcPrnMargins = rcPage;
	rcPrnMargins.DeflateRect( m_nPrnLeft, m_nPrnTop, m_nPrnRight, m_nPrnBottom );

	pDC->LPtoDP( &rcPage );
	pDC->LPtoDP( &rcField );
	pDC->LPtoDP( &rcPrnMargins );


	pDC->SetMapMode( nMapMode );


	CDC dcImage;
	dcImage.CreateCompatibleDC( pDC );

	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( pDC, rcWnd.Width(), rcWnd.Height() );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

	dcImage.FillRect( CRect(0,0,rcWnd.Width(),rcWnd.Height()), 
				&CBrush(::GetSysColor(COLOR_BTNFACE)) );

	rcPage += CPoint(2,2);
	dcImage.FillRect( rcPage, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
	//Inside
	rcPage -= CPoint(2,2);
	COLORREF color = RGB(255,255,255);
	dcImage.FillRect( rcPage, &CBrush(color) );		


	
	CPen penMargins( PS_SOLID, 1, RGB(255,0,0) );
	CPen* pOldPen = (CPen*)dcImage.SelectObject( &penMargins );	

	//left
	//left
	dcImage.MoveTo( rcPrnMargins.left, rcPage.top);
	dcImage.LineTo( rcPrnMargins.left, rcPage.bottom);

	//right
	dcImage.MoveTo( rcPrnMargins.right, rcPage.top);
	dcImage.LineTo( rcPrnMargins.right, rcPage.bottom);

	//top
	dcImage.MoveTo( rcPage.left , rcPrnMargins.top );
	dcImage.LineTo( rcPage.right, rcPrnMargins.top );

	//bottom
	dcImage.MoveTo( rcPage.left , rcPrnMargins.bottom );
	dcImage.LineTo( rcPage.right, rcPrnMargins.bottom );



	CPen pen( PS_SOLID, 1, RGB(127,127,127) );
	dcImage.SelectObject( &pen );


	//left
	dcImage.MoveTo( rcField.left, rcPage.top);
	dcImage.LineTo( rcField.left, rcPage.bottom);

	//right
	dcImage.MoveTo( rcField.right, rcPage.top);
	dcImage.LineTo( rcField.right, rcPage.bottom);

	//top
	dcImage.MoveTo( rcPage.left , rcField.top );
	dcImage.LineTo( rcPage.right, rcField.top );

	//bottom
	dcImage.MoveTo( rcPage.left , rcField.bottom );
	dcImage.LineTo( rcPage.right, rcField.bottom );


	dcImage.SelectObject( pOldPen );



	pDC->BitBlt( rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
					&dcImage, 0, 0, SRCCOPY );


	dcImage.SelectObject( pOldBitmap );

}

void CPageSetup::TryUpdateCtrl( UINT nID )
{
	if( !m_bInitDlg )
		return;

	CString strBuf;
	CWnd* pWnd = NULL;
	pWnd = GetDlgItem(nID);
	if( !(pWnd && pWnd->GetSafeHwnd()) )
		return;

	pWnd->GetWindowText( strBuf );

	try
	{
		_variant_t var( strBuf );
		_VarChangeType( var, VT_R8 );
		UpdateData( TRUE );
		RepaintPage();
	}
	catch(...)
	{
		
	}

}

void CPageSetup::OnChangeWidth() 
{
	TryUpdateCtrl( IDC_WIDTH );
}

void CPageSetup::OnChangeHeight() 
{
	TryUpdateCtrl( IDC_HEIGHT );	
}



void CPageSetup::OnChangeLeft() 
{
	if( !m_bInitDlg )
		return;

	TryUpdateCtrl( IDC_LEFT );
	/*

	if( m_fLeft < m_rcPrnMargins.left )
	{
		m_fLeft = m_rcPrnMargins.left;
		UpdateData( FALSE );
		RepaintPage();
	}
	else
	if( m_fLeft > m_fWidth - m_fRight )
	{
		m_fLeft = m_fWidth - m_fRight;
		UpdateData( FALSE );
		RepaintPage();
	}
	*/
}

void CPageSetup::OnChangeTop() 
{
	if( !m_bInitDlg )
		return;
	/*

	TryUpdateCtrl( IDC_TOP );	
	
	if( m_fTop < m_rcPrnMargins.top )
	{
		m_fTop = m_rcPrnMargins.top;
		UpdateData( FALSE );
		RepaintPage();
	}
	else
	if( m_fTop > m_fHeight - m_fBottom )
	{
		m_fTop = m_fHeight - m_fBottom;
		UpdateData( FALSE );
		RepaintPage();
	}
	*/

}

void CPageSetup::OnChangeRight() 
{
	if( !m_bInitDlg )
		return;

	TryUpdateCtrl( IDC_RIGHT );	

	/*
	
	if( m_fRight < m_rcPrnMargins.right )
	{
		m_fRight = m_rcPrnMargins.right;
		UpdateData( FALSE );
		RepaintPage();
	}
	else
	if( m_fRight > m_fLeft )
	{
		m_fRight = m_fLeft;
		UpdateData( FALSE );
		RepaintPage();
	}
	*/


}

void CPageSetup::OnChangeBottom() 
{	
	if( !m_bInitDlg )
		return;

	/*
	TryUpdateCtrl( IDC_BOTTOM );	

	if( m_fBottom < m_rcPrnMargins.bottom )
	{
		m_fBottom = m_rcPrnMargins.bottom;
		UpdateData( FALSE );
		RepaintPage();
	}
	else
	if( m_fBottom > m_fTop )
	{
		m_fBottom = m_fTop;
		UpdateData( FALSE );
		RepaintPage();
	}
	*/

}





void CPageSetup::OnHelp() 
{
	HelpDisplay( "PageSetup" );
}
