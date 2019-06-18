// ShowIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "axeditor.h"
#include "ShowIDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowIDDlg dialog


CShowIDDlg::CShowIDDlg(CAxActionBase *pa, CWnd* pParent)
	: CDialog(CShowIDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowIDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pa = pa;
}


void CShowIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowIDDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowIDDlg, CDialog)
	//{{AFX_MSG_MAP(CShowIDDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowIDDlg message handlers

int CShowIDDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CShowIDDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSize	size;
	m_pa->m_form->GetSize( &size );

	CRect	rectDlg;
	GetWindowRect( &rectDlg );

	CRect	rectControls;
	GetDlgItem( IDC_CONTROLS_PLACE )->GetWindowRect( &rectControls );
	GetDlgItem( IDC_CONTROLS_PLACE )->ShowWindow( SW_HIDE );

	int	cx = rectDlg.right-rectControls.right;
	int	cy = rectDlg.bottom-rectControls.bottom;

	rectControls.right = rectControls.left+size.cx;
	rectControls.bottom = rectControls.top+size.cy;

	rectDlg.right = rectControls.right+cx;
	rectDlg.bottom = rectControls.bottom+cy;

	ScreenToClient( &rectControls );

	MoveWindow( rectDlg );

	long	lpos = 0;

	INamedDataObject2Ptr	ptrN( m_pa->m_form );

	ptrN->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		IUnknown	*punkChild = 0;
		ptrN->GetNextChild( &lpos, &punkChild );
		IActiveXCtrlPtr	ptrAX( punkChild );
		punkChild->Release();
		
		if( ptrAX == 0 )
			continue;

		CRect	rect;
		UINT	nID;

		ptrAX->GetRect( &rect );
		ptrAX->GetControlID( &nID );

		CString	strID;
		strID.Format( "%d", (int)nID );

		rect+=rectControls.TopLeft();

		CStatic	*p = new CStatic;
		p->Create( strID, WS_CHILD|WS_VISIBLE|WS_BORDER, rect, this );
		p->SetFont( GetFont() );
	}

	CRect	rectClose;
	GetDlgItem( IDOK )->GetWindowRect( &rectClose );

	CSize	sizeClose = rectClose.Size();

	rectClose.top = rectDlg.bottom-cx/2-sizeClose.cy/2;
	rectClose.bottom = rectClose.top+sizeClose.cy;
	rectClose.left = rectDlg.CenterPoint().x-sizeClose.cx/2;
	rectClose.right = rectClose.left+sizeClose.cx;

	ScreenToClient( &rectClose );

	GetDlgItem( IDOK )->MoveWindow( rectClose );
	
	return TRUE;
}

void CShowIDDlg::OnHelp() 
{
	HelpDisplay( "ShowIDDlg" );	
}
