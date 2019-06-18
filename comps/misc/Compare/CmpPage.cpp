// CmpPage.cpp : implementation file
//

#include "stdafx.h"
#include "compare.h"
#include "CmpPage.h"

#include "\vt5\common2\misc_classes.h"
#include "\vt5\awin\win_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmpPage property page

IMPLEMENT_DYNCREATE(CCmpPage, CDialog)

// {41FC85B7-21E0-443c-B497-B3E7EBF48573}
GUARD_IMPLEMENT_OLECREATE(CCmpPage, "Compare.CmpPage", 
0x41fc85b7, 0x21e0, 0x443c, 0xb4, 0x97, 0xb3, 0xe7, 0xeb, 0xf4, 0x85, 0x73);


CCmpPage::CCmpPage() : CPropertyPageBase(CCmpPage::IDD)
{
	//{{AFX_DATA_INIT(CCmpPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nOverlayMode = 0;
	m_sName = c_szCCmpPage;
	m_sUserName.LoadString( IDS_PROPPAGE_CMPPAGE );
}

CCmpPage::~CCmpPage()
{
}

void CCmpPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmpPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmpPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CCmpPage)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmpPage message handlers

BEGIN_EVENTSINK_MAP(CCmpPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CCmpPage)
	ON_EVENT(CCmpPage, IDC_ADD, -600 /* Click */, OnClickAdd, VTS_NONE)
	ON_EVENT(CCmpPage, IDC_DEFAULT, -600 /* Click */, OnClickDefault, VTS_NONE)
	ON_EVENT(CCmpPage, IDC_OVERLAY, -600 /* Click */, OnClickOverlay, VTS_NONE)
	ON_EVENT(CCmpPage, IDC_SUB, -600 /* Click */, OnClickSub, VTS_NONE)
	ON_EVENT(CCmpPage, IDOK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CCmpPage, IDC_ANGLE, -600 /* Click */, OnAngle, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CCmpPage::OnClickAdd() 
{
	m_nOverlayMode = 0;
	DoStoreSettings();
	_UpdateControls();

	ReturnFocus();//BT5152
}

void CCmpPage::OnClickOverlay() 
{
	m_nOverlayMode = 1;
	DoStoreSettings();
	_UpdateControls();
	ReturnFocus();//BT5152
}

void CCmpPage::OnClickSub() 
{
	m_nOverlayMode = 2;
	DoStoreSettings();
	_UpdateControls();
	ReturnFocus();//BT5152
}

void CCmpPage::OnClickDefault() 
{
	::SetValue( GetAppUnknown(), "\\Compare", "Angle", (long)0 );
	::SetValue( GetAppUnknown(), "\\Compare", "Width", (long)100 );
	::SetValue( GetAppUnknown(), "\\Compare", "Height", (long)100 );
	::SetValue( GetAppUnknown(), "\\Compare", "XPos", (long)0 );
	::SetValue( GetAppUnknown(), "\\Compare", "YPos", (long)0 );	
	UpdateAction();
	_UpdateControls();
	ReturnFocus();
}


void CCmpPage::OnClickOk() 
{
	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkAM = 0;
	ptrApp->GetActionManager( &punkAM );
	IActionManagerPtr	ptrAM( punkAM );
	punkAM->Release();
	IUnknown *punk = 0;
	ptrAM->GetRunningInteractiveAction( &punk );
	if( !punk )return;
	IActionPtr	ptrA = punk;
	punk->Release();

	if( ptrA == 0 )return;

	IUnknown *punkTarget = 0;
	ptrA->GetTarget( &punkTarget );

	if( !punkTarget )return;
	IWindowPtr	ptrWindow( punkTarget );
	punkTarget->Release();

	if( ptrWindow == 0 )return;

	HWND	hwnd = 0;
	ptrWindow->GetHandle( (HANDLE*)&hwnd );
	if( hwnd == 0 )return;

	::PostMessage( hwnd, WM_CHAR, VK_RETURN, 0 );
}

void CCmpPage::_UpdateControls()
{
	// защита от вызова до полной инициализации
	if( !IsInitialized() )
		return;

	long	lShow = ::GetValueInt( GetAppUnknown(), "Compare", "Compare", 0l )?SW_SHOW:SW_HIDE;	

	//if( GetDlgItem( IDC_OVERLAY_FRAME )->IsWindowVisible() != (lShow == SW_SHOW ) )
	{
		GetDlgItem( IDC_ADD )->ShowWindow( lShow );
		GetDlgItem( IDC_OVERLAY )->ShowWindow( lShow );
		GetDlgItem( IDC_SUB )->ShowWindow( lShow );
		GetDlgItem( IDC_OVERLAY_FRAME )->ShowWindow( lShow );
	}

	if( lShow )
	{
		((CButton*)GetDlgItem( IDC_ADD ))->SetCheck( m_nOverlayMode == 0 );
		((CButton*)GetDlgItem( IDC_OVERLAY ))->SetCheck( m_nOverlayMode == 1 );
		((CButton*)GetDlgItem( IDC_SUB ))->SetCheck( m_nOverlayMode == 2 );
	}

	{
		//angle and angle step information
		double	a, as;
		a = ::GetValueDouble( GetAppUnknown(), "\\Compare", "Angle", 0 );
		as = ::GetValueDouble( GetAppUnknown(), "\\Compare", "AngleStep", 1 );

		if( !GetValueInt( GetAppUnknown(), "\\Compare", "Clockwise", true ) && a != 0 )
			a = -(360-a);

		CString	str;
		str.Format( IDS_ANGLE_FORMAT, a, as );
		GetDlgItem( IDC_CURRENT_ANGLE )->SetWindowText( str );
	}

	//ReturnFocus();
}

void CCmpPage::DoStoreSettings()
{
	::SetValue( GetAppUnknown(), "\\Compare", "OverlayMode", (long)m_nOverlayMode );
	UpdateAction();
}

void CCmpPage::DoLoadSettings()
{
	m_nOverlayMode = ::GetValueInt( GetAppUnknown(), "\\Compare", "OverlayMode", (long)m_nOverlayMode );
	_UpdateControls();
}

BOOL CCmpPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	
	DoLoadSettings();

	SetTimer( 777, 100, 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCmpPage::ReturnFocus()
{
	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkAM = 0;
	ptrApp->GetActionManager( &punkAM );
	IActionManagerPtr	ptrAM( punkAM );
	punkAM->Release();
	IUnknown *punk = 0;
	ptrAM->GetRunningInteractiveAction( &punk );
	if( !punk )return;
	IActionPtr	ptrA = punk;
	punk->Release();

	if( ptrA == 0 )return;

	IUnknown *punkTarget = 0;
	ptrA->GetTarget( &punkTarget );

	if( !punkTarget )return;
	IWindowPtr	ptrWindow( punkTarget );
	punkTarget->Release();

	if( ptrWindow == 0 )return;

	HWND	hwnd = 0;
	ptrWindow->GetHandle( (HANDLE*)&hwnd );
	if( hwnd == 0 )return;

	if( ::GetFocus() != hwnd )
		::SetFocus( hwnd );
}

BOOL CCmpPage::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	/*if( message == WM_TIMER )
		ReturnFocus();*/
	return CWnd::OnWndMsg( message, wParam, lParam, pResult );
}


void CCmpPage::OnDestroy() 
{
	CPropertyPageBase::OnDestroy();
	KillTimer( 777 );
}

void CCmpPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPageBase::OnShowWindow(bShow, nStatus);

	KillTimer( 777 );
	if( bShow )	SetTimer( 777, 500, 0 );
}

void CCmpPage::OnSetFocus(CWnd* pOldWnd) 
{
	CPropertyPageBase::OnSetFocus(pOldWnd);
	//ReturnFocus();
}

class angle_dlg : public dlg_impl
{
public:
	angle_dlg() : dlg_impl( IDD_ANGLE )
	{
		m_angle = 0;
		m_clockwise = true;
	}

	virtual long on_initdialog()
	{
		m_clockwise = ::GetValueInt( GetAppUnknown(), "\\Compare", "Clockwise", m_clockwise )!=0;
		if( !m_clockwise && m_angle != 0 )
			m_angle = 360-m_angle;

		char	sz[20];
		sprintf( sz, "%g", m_angle );
		::SetDlgItemText( handle(), IDC_ANGLE, sz );

		CheckDlgButton( handle(), IDC_CLOCKWISE, m_clockwise );
		CheckDlgButton( handle(), IDC_ANTICLOCKWISE, !m_clockwise );

		return dlg_impl::on_initdialog();
	}
	virtual void on_ok()
	{
		char	sz[20];
		::GetDlgItemText( handle(), IDC_ANGLE, sz, sizeof( sz ) );
		if( ::sscanf( sz, "%lf", &m_angle ) != 1 )
		{
			AfxMessageBox( IDS_WRONG_NUMBER );
			return;
		}

		if( !m_clockwise && m_angle != 0 )
			m_angle = 360-m_angle;

		::SetValue( GetAppUnknown(), "\\Compare", "Clockwise", (long)m_clockwise );

		dlg_impl::on_ok();
	}
	virtual long on_command( uint cmd )
	{
		if( cmd == IDC_CLOCKWISE )m_clockwise = true;
		if( cmd == IDC_ANTICLOCKWISE )m_clockwise = false;

		return dlg_impl::on_command( cmd );
	}

	double	m_angle;
	bool	m_clockwise;
};

void CCmpPage::OnAngle() 
{
	angle_dlg	dlg;
	dlg.m_angle = ::GetValueDouble( GetAppUnknown(), "\\Compare", "Angle", (long)0 );
	if( dlg.do_modal( GetSafeHwnd() ) == IDOK )
	{
		::SetValue( GetAppUnknown(), "\\Compare", "Angle", dlg.m_angle );
		_UpdateControls();
		UpdateAction();
	}

	ReturnFocus();
}

const char *CCmpPage::GetHelpTopic()
{
	long	lCompare = ::GetValueInt( GetAppUnknown(), "Compare", "Compare", 0l );	
	if( lCompare )return "Compare";
	else return "FreeTransform";
}