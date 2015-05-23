#include "stdafx.h"
#include "sew_page.h"
#include "sew.h"


///////

CSewPropertyPage::CSewPropertyPage()
{
	char	sz[255];
	::LoadString( App::handle(), IDS_SEW_PAGE, sz, sizeof( sz ) );
	m_bstrCaption = sz;
	m_bstrHelpTopic = "Stitch";
}

CSewPropertyPage::~CSewPropertyPage()
{
	UnAdviseAll();
}

CWinImpl *CSewPropertyPage::GetTarget()
{
	return this;
}

IUnknown *CSewPropertyPage::DoGetInterface( const IID &iid )
{
	IUnknown	*punk = DoGetDlgInterface( iid );
	if( punk )return punk;
	else if( iid == IID_IPersist )return (IPersist*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT CSewPropertyPage::GetClassID( CLSID *pclsid )
{
	*pclsid = clsidSewPage;
	return S_OK;
}




bool CSewPropertyPage::DoInitDialog()
{
	//cleanup if it is not first call
	UnAdviseAll();
	m_ptrButtonCopy = 0;
	m_ptrButtonNew = 0;
	m_ptrButtonAdd = 0;
	m_ptrButtonSub = 0;

	Advise( IDC_MODE_COPY, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CSewPropertyPage::OnClickCopy);
	Advise( IDC_MODE_ADD, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CSewPropertyPage::OnClickAdd);
	Advise( IDC_MODE_SUB, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CSewPropertyPage::OnClickSub);
	Advise( IDC_NEXT_IMAGE, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CSewPropertyPage::OnClickNext);

	m_ptrButtonCopy = (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_MODE_COPY ), WM_GETCONTROL, 0, 0 );
	m_ptrButtonNew = (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_NEXT_IMAGE ), WM_GETCONTROL, 0, 0 );
	m_ptrButtonAdd = (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_MODE_ADD ), WM_GETCONTROL, 0, 0 );
	m_ptrButtonSub = (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_MODE_SUB ), WM_GETCONTROL, 0, 0 );

	UpdateControls();
	ReturnFocus();

	return true;
}

void CSewPropertyPage::DoCommand( UINT nCmd )
{
}


void CSewPropertyPage::DoChangeOverlayMode( UINT nMode )
{
	IInteractiveSewAction	*paction = GetAction();
	if( !paction )return;

	paction->SetMode( nMode );

	paction->Release();
	ReturnFocus();

}
IInteractiveSewAction	*CSewPropertyPage::GetAction()
{
	IUnknown *punk = GetCurrentInteractiveAction();
	if( !punk )return 0;
	IInteractiveSewAction	*paction = 0;
	punk->QueryInterface( IID_IInteractiveSewAction, (void**)&paction );
	punk->Release();

	return paction;
}

void CSewPropertyPage::UpdateControls()
{
	UINT nMode = 0;

	IInteractiveSewAction	*paction = GetAction();
	if( !paction )return;

	paction->GetMode( &nMode );
	
	paction->Release();


	if( m_ptrButtonCopy )
	if( nMode == '=' )m_ptrButtonCopy->SetPressedState();
	else m_ptrButtonCopy->ResetPressedState();
	if( m_ptrButtonSub )
	if( nMode == '-' )m_ptrButtonSub->SetPressedState();
	else m_ptrButtonSub->ResetPressedState();
	if( m_ptrButtonAdd )
	if( nMode == '+' )m_ptrButtonAdd->SetPressedState();
	else m_ptrButtonAdd->ResetPressedState();
}

void CSewPropertyPage::ReturnFocus()
{
	IUnknown *punk = ::GetCurrentInteractiveAction();
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

	::SetFocus( hwnd );
}

LRESULT CSewPropertyPage::DoNotify( UINT nID, NMHDR *phdr )			
{
	trace( "DoNotify %X\n", nID );
	return 0;
}

void CSewPropertyPage::OnClickCopy()
{
	DoChangeOverlayMode( '=' );
}
void CSewPropertyPage::OnClickAdd()
{
	DoChangeOverlayMode( '+' );
}
void CSewPropertyPage::OnClickSub()
{
	DoChangeOverlayMode( '-' );
}
void CSewPropertyPage::OnClickNext()
{
	IInteractiveSewAction	*paction = GetAction();
	if( !paction )return;

	paction->NextImage();
	
	paction->Release();

	ReturnFocus();
}