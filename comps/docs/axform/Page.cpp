// Page.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "Page.h"



/////////////////////////////////////////////////////////////////////////////
// CPage property page

IMPLEMENT_DYNCREATE(CPage, CPropertyPage)

CPage::CPage() : CPropertyPage(CPage::IDD, IDS_PAGE_TITLE)
{
	//{{AFX_DATA_INIT(CPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = "Page";
	m_site.m_bFormInited = true;

}

CPage::~CPage()
{
}

void CPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPage)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_CAPTIONCHANGE, OnCaptionChange)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_HELP, OnHelpHitTest)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CPage, CPropertyPage)
	INTERFACE_PART(CPage, IID_INamedObject2, Name)
END_INTERFACE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CPage message handlers

BOOL CPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();


//	SetWindowLong(  GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE )|WS_EX_CONTEXTHELP );
	
	// TODO: Add extra initialization here
	//SetWindowText( m_site.GetCaption() );
	
	CRect	rectSite;
	GetDlgItem( IDC_CONTROLS )->GetWindowRect( &rectSite );
	GetDlgItem( IDC_CONTROLS )->ShowWindow( SW_HIDE );

	ScreenToClient( rectSite );

	CSize	sizeOld = rectSite.Size();
	CSize	sizeNew = m_site.GetSize();

	rectSite.right = rectSite.left+sizeNew.cx;
	rectSite.bottom = rectSite.top+sizeNew.cy;

	CPoint	pointOffset( sizeNew.cx-sizeOld.cx, sizeNew.cy-sizeOld.cy );

	CRect	rectDialog;
	GetWindowRect( &rectDialog );
	rectDialog.right+= pointOffset.x;
	rectDialog.bottom+= pointOffset.y;

	MoveWindow( rectDialog );

	HWND	hwnd = ::GetWindow( GetSafeHwnd(), GW_CHILD );

	while( hwnd ) 
	{
		CRect	rect;
		::GetWindowRect( hwnd, &rect );
		ScreenToClient( rect );
		rect.OffsetRect( pointOffset );
		::MoveWindow( hwnd, rect.left, rect.top, rect.Width(), rect.Height(), 0 );

		hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
	}


	m_site.SetEdgeType( 2 );
	m_site.DoCreate( WS_CHILD|WS_VISIBLE, rectSite, this, 101 );

	//m_psp.pszTitle = m_sName;
	//SetWindowText(m_sName);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPage::SetForm( IUnknown *punkForm )
{
	m_site.SetForm( punkForm );
	m_sName = ::GetObjectName( punkForm );
}

void CPage::SetDlgTitle(CString strTitle)
{
	m_psp.pszTitle = strTitle;
}

LRESULT CPage::OnCaptionChange(WPARAM, LPARAM)
{
	m_psp.pszTitle = m_site.GetCaption();
	if( IsWindow( GetSafeHwnd() ) )
	{
		CPropertySheet	*psheet = (CPropertySheet*)GetParent();
		CTabCtrl	*ptab = psheet->GetTabControl();

		int	nItemCount = ptab->GetItemCount();

		for( int n = 0; n < nItemCount; n++ )
		{
			if( ptab->GetItemState( n, TCIS_BUTTONPRESSED ) )
			{
				TCITEM	tcItem;
				char	sz[255];

				strcpy( sz, m_site.GetCaption() ) ;

				ZeroMemory( &tcItem, sizeof( tcItem ) );
				tcItem.pszText = sz;
				tcItem.cchTextMax = 255;
				tcItem.mask = TCIF_TEXT;
				ptab->SetItem( n, &tcItem );
			}
		}
	}

	
	SetWindowText( m_site.GetCaption() );
	return 0;
}


BOOL CPage::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return OnHelpHitTest( 0, 0 ) != -2;
}

LRESULT CPage::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	return m_site.SendMessage( WM_HELPHITTEST );
}

BOOL CPage::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CPropertyPage::PreCreateWindow(cs) )return false;
//	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return true;
}

void CPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
/*	
	if( bShow )
		m_site.FireOnShow();
	else
		m_site.FireOnHide();
*/
}
