// PropPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "PropPageDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyPageEx

CPropertyPageEx_::CPropertyPageEx_(UINT ui) : CPropertyPage( ui )
{

}

/////////////////////////////////////////////////////////////////////////////
void CPropertyPageEx_::OnInit()
{
	UpdateData( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
bool CPropertyPageEx_::OnPresedOK()
{
	if( !UpdateData( TRUE ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CPropertyPageEx_::OnPresedCancel()
{
	return true;
}


/////////////////////////////////////////////////////////////////////////////
int CPropertyPageEx_::ShowModal()
{
	CPropPageDlg dlg;
	dlg.SetPropPage( this );

	return dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageDlg dialog


CPropPageDlg::CPropPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropPageDlg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CPropPageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageDlg, CDialog)
	//{{AFX_MSG_MAP(CPropPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageDlg message handlers

BOOL CPropPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* pWndOK = GetDlgItem( IDOK );
	if( !pWndOK )
		return TRUE;

	CWnd* pWndCancel = GetDlgItem( IDCANCEL );
	if( !pWndCancel )
		return TRUE;

	if( !m_pChild )
		return TRUE;

	if( !m_pChild->Create( m_pChild->GetPageID(), this ) )
		return TRUE;

	HWND hwndChild = m_pChild->GetSafeHwnd();
	if( !hwndChild )
		return TRUE;

	//some changes in wnd style
	LONG lChildStyle = ::GetWindowLong( hwndChild, GWL_STYLE );
	lChildStyle &= ~WS_DISABLED;
	lChildStyle &= ~WS_CAPTION;	
	lChildStyle &= ~WS_BORDER;		
	::SetWindowLong( hwndChild, GWL_STYLE, lChildStyle );	


	CSize sizePage( 200, 200 );

	{
		CRect rc;
		m_pChild->GetClientRect( &rc );
		sizePage.cx = rc.Width();
		sizePage.cy = rc.Height();
	}


	//move page and ok & cancel button

	CRect rcDlg;
	GetClientRect( &rcDlg );

	CRect rcOK;
	pWndOK->GetWindowRect( &rcOK );

	CRect rcCancel;
	pWndCancel->GetWindowRect( &rcCancel );
	
	int nDeltaWidth		= 0;
	int nDeltaHeight	= 0;

	CRect rcWndThis;
	CRect rcThis = rcDlg;
	{		
		GetWindowRect( &rcWndThis );
		
		nDeltaWidth		= rcWndThis.Width()		- rcDlg.Width();
		nDeltaHeight	= rcWndThis.Height()	- rcDlg.Height();
	}

	rcThis.right	= rcThis.left + max( sizePage.cx, rcOK.Width() + rcCancel.Width() + 50 ) + nDeltaWidth;
	rcThis.bottom	= rcThis.top  + sizePage.cy + rcOK.Height() + 20 + nDeltaHeight;
	MoveWindow( &rcThis );
	CenterWindow( GetMainFrameWnd() );
	

	GetClientRect( &rcThis );

	m_pChild->MoveWindow( &CRect( 0, 0, sizePage.cx, sizePage.cy ) );

	m_pChild->ShowWindow( SW_SHOWNA );

	CRect rcCancelNew, rcOKNew;

	rcCancelNew.right	= rcThis.Width() - 10;
	rcCancelNew.left	= rcCancelNew.right - rcCancel.Width();

	rcCancelNew.bottom	= rcThis.Height() - 10;
	rcCancelNew.top		= rcCancelNew.bottom - rcCancel.Height();

	rcOKNew = rcCancelNew;

	rcOKNew.right	= rcCancelNew.left - 20;
	rcOKNew.left	= rcOKNew.right - rcOK.Width();


	pWndCancel->MoveWindow( &rcCancelNew );
	pWndOK->MoveWindow( &rcOKNew );


	CString strTitle;
	m_pChild->GetWindowText( strTitle );

	SetWindowText( strTitle );

	m_pChild->OnInit();

	return TRUE;
}

void CPropPageDlg::OnOK() 
{
	if( m_pChild )
	{
		if( !m_pChild->OnPresedOK() )
			return;
	}
	CDialog::OnOK();
}

void CPropPageDlg::OnCancel() 
{	
	if( m_pChild )
	{
		if( !m_pChild->OnPresedCancel() )
			return;
	}
	CDialog::OnCancel();
}
