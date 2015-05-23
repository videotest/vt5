// WindowPage.cpp : implementation file
//

#include "stdafx.h"
#include "axeditor.h"
#include "WindowPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowPage dialog

// {95332A8E-0132-4fca-91C8-B93BF75A0351}
GUARD_IMPLEMENT_OLECREATE(CWindowPage, "axeditor.WindowPage", 
0x95332a8e, 0x132, 0x4fca, 0x91, 0xc8, 0xb9, 0x3b, 0xf7, 0x5a, 0x3, 0x51);

IMPLEMENT_DYNCREATE(CWindowPage, COlePropertyPage);


CWindowPage::CWindowPage()
	: COlePropertyPage( CWindowPage::IDD, IDS_PAGE_TITLE )
{
	//{{AFX_DATA_INIT(CWindowPage)
	m_bDisabled = FALSE;
	m_bGroup = FALSE;
	m_nLeft = 0;
	m_strName = _T("");
	m_bTabstop = FALSE;
	m_nTop = 0;
	m_bVisible = FALSE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nID = 0;
	//}}AFX_DATA_INIT
	m_bActive = false;
	m_bCanEntry = true;
}

void CWindowPage::DoDataExchange(CDataExchange* pDX)
{
	COlePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindowPage)
	DDX_Control(pDX, IDC_HEIGHT_SPIN, m_spinHeight);
	DDX_Control(pDX, IDC_WIDTH_SPIN, m_spinWidth);
	DDX_Control(pDX, IDC_TOP_SPIN, m_spinTop);
	DDX_Control(pDX, IDC_LEFT_SPIN, m_spinLeft);
	DDX_Check(pDX, IDC_DISABLED, m_bDisabled);
	DDX_Check(pDX, IDC_GROUP, m_bGroup);
	DDX_Text(pDX, IDC_LEFT, m_nLeft);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Check(pDX, IDC_TABSTOP, m_bTabstop);
	DDX_Text(pDX, IDC_TOP, m_nTop);
	DDX_Check(pDX, IDC_VISIBLE, m_bVisible);
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_ID, m_nID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWindowPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CWindowPage)
	ON_WM_CREATE()
	ON_EN_SETFOCUS(IDC_NAME, OnSetfocusName)
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CWindowPage, COlePropertyPage)
	INTERFACE_PART(CWindowPage, IID_IPropertyPage, MyPropertyPage)
	INTERFACE_PART(CWindowPage, IID_IPropertyPage2, MyPropertyPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CWindowPage, MyPropertyPage)

HRESULT CWindowPage::XMyPropertyPage::SetPageSite( LPPROPERTYPAGESITE lpp )
{
	_try_nested(CWindowPage, MyPropertyPage, SetPageSite)
	{
		return pThis->m_xPropertyPage.SetPageSite( lpp );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::Activate( HWND hWnd, LPCRECT pcRect, BOOL b )
{
	_try_nested(CWindowPage, MyPropertyPage, Activate)
	{
		pThis->m_bActive = true;
		return pThis->m_xPropertyPage.Activate( hWnd, pcRect, b );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::Deactivate()
{
	_try_nested(CWindowPage, MyPropertyPage, Deactivate)
	{
		pThis->m_bActive = false;
		return pThis->m_xPropertyPage.Deactivate();
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::GetPageInfo( LPPROPPAGEINFO ppageInfo )
{
	_try_nested(CWindowPage, MyPropertyPage, GetPageInfo)
	{
		return pThis->m_xPropertyPage.GetPageInfo( ppageInfo );
	}
	_catch_nested;
}
HRESULT CWindowPage::XMyPropertyPage::SetObjects( ULONG nCount, LPUNKNOWN* ppUnk )
{
	_try_nested(CWindowPage, MyPropertyPage, SetObjects)
	{
		if( !nCount )
			return E_FAIL;

		if( nCount == 1  )
			pThis->CheckOwnUnknown( ppUnk[nCount] );

		return pThis->m_xPropertyPage.SetObjects( nCount, ppUnk );
	}
	_catch_nested;
}
HRESULT CWindowPage::XMyPropertyPage::Show( UINT nShow )
{
	_try_nested(CWindowPage, MyPropertyPage, Show)
	{
		return pThis->m_xPropertyPage.Show( nShow );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::Move(LPCRECT prect )
{
	_try_nested(CWindowPage, MyPropertyPage, Move)
	{
		return pThis->m_xPropertyPage.Move( prect );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::IsPageDirty()
{
	_try_nested(CWindowPage, MyPropertyPage, IsPageDirty)
	{
		return pThis->m_xPropertyPage.IsPageDirty();
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::Apply()
{
	_try_nested(CWindowPage, MyPropertyPage, Apply)
	{
		pThis->OnOK();
		return pThis->m_xPropertyPage.Apply();
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::Help(LPCOLESTR lpole)
{
	_try_nested(CWindowPage, MyPropertyPage, Help)
	{
		return pThis->m_xPropertyPage.Help( lpole );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::TranslateAccelerator( LPMSG lpmsg )
{
	_try_nested(CWindowPage, MyPropertyPage, TranslateAccelerator)
	{
		return pThis->m_xPropertyPage.TranslateAccelerator( lpmsg );
	}
	_catch_nested;
}

HRESULT CWindowPage::XMyPropertyPage::EditProperty( DISPID dispid )
{
	_try_nested(CWindowPage, MyPropertyPage, EditProperty)
	{
		return pThis->m_xPropertyPage.EditProperty( dispid );
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CWindowPage message handlers

int CWindowPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COlePropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	

	return 0;
}

bool CWindowPage::CheckOwnUnknown( IUnknown *punk )
{
	if( !CheckInterface( punk, IID_IActiveXCtrl ) )
		return false;

	IActiveXCtrlPtr	sptrAX( punk );

	DWORD	dwStyle;
	CRect	rect;
	BSTR	bstr;

	sptrAX->GetRect( &rect );
	sptrAX->GetStyle( &dwStyle );
	sptrAX->GetName( &bstr );
	sptrAX->GetControlID( (UINT*)&m_nID );
	m_strName = bstr;
	::SysFreeString( bstr );

	m_bDisabled = (dwStyle & WS_DISABLED) == WS_DISABLED;
	m_bGroup = (dwStyle & WS_GROUP) == WS_GROUP;
	m_bTabstop = (dwStyle & WS_TABSTOP) == WS_TABSTOP;
	m_bVisible = (dwStyle & WS_VISIBLE) == WS_VISIBLE;
	m_nLeft = rect.left;
	m_nTop = rect.top;
	m_nWidth = rect.Width();
	m_nHeight = rect.Height();

	m_ptrControl = punk;

	return true;
}

void _SetStyleBit( DWORD &dwStyle, DWORD dwMask, bool bSet )
{
	if( bSet )
		dwStyle |= dwMask;
	else
		if( (dwStyle & dwMask) == dwMask )
			dwStyle ^= dwMask;
}

void CWindowPage::OnOK()
{
	/*if( !m_bActive )
		return;*/
	if( !UpdateData() )
		return;

	if( m_ptrControl != 0 )
	{
		IActiveXCtrlPtr	sptrAX( m_ptrControl );

		DWORD	dwStyle;
		CRect	rect( m_nLeft, m_nTop, m_nLeft+m_nWidth, m_nTop+m_nHeight );

		sptrAX->GetStyle( &dwStyle );

		BSTR	bstr =m_strName.AllocSysString();
		sptrAX->SetName( bstr );
		::SysFreeString( bstr );

		_SetStyleBit( dwStyle, WS_DISABLED, m_bDisabled==TRUE );
		_SetStyleBit( dwStyle, WS_GROUP, m_bGroup==TRUE );
		_SetStyleBit( dwStyle, WS_TABSTOP, m_bTabstop==TRUE );
		_SetStyleBit( dwStyle, WS_VISIBLE, m_bVisible==TRUE );

		sptrAX->SetStyle( dwStyle );
		sptrAX->SetControlID( m_nID );

		//Auto size support
		{

			ULONG nCout;
			LPDISPATCH* lpDispatch = GetObjectArray( &nCout );
			if( lpDispatch )
			{
				if( lpDispatch[0] )
				{
					IOleInPlaceObject* pObject = NULL; 
					HRESULT hr = lpDispatch[0]->QueryInterface( IID_IOleInPlaceObject, 
						(void**)&pObject );
					if( hr == S_OK && pObject != NULL )
					{					
						HWND hWnd = NULL;
						pObject->GetWindow( &hWnd );

						if( IsWindow(hWnd) )
						{	   

							CRect cOldAxRect;
							sptrAX->GetRect( &cOldAxRect );
							if( cOldAxRect.Width() != m_nWidth ||
								cOldAxRect.Height() != m_nHeight )
							{
								::MoveWindow( hWnd, 0, 0, m_nWidth, m_nHeight, FALSE );
							}

						}

						pObject->Release();

					}
					
				}
			}

		}

		sptrAX->SetRect( rect );
		


	}
}

BOOL CWindowPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	m_spinHeight.SetRange( 1, 1000 );
	m_spinWidth.SetRange( 1, 1000 );
	m_spinTop.SetRange( 1, 1000 );
	m_spinLeft.SetRange( 1, 1000 );
	
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWindowPage::VerifyUniqueName( bool bSetFocus )
{
	if( !m_bCanEntry )
		return;

	CWnd* pWnd = NULL;
	pWnd = GetDlgItem(IDC_NAME);
	if( pWnd )
	{
		CString strName;
		pWnd->GetWindowText( strName );
		bool bNeedSetFocus = false;
		sptrIActiveXCtrl spActiveXCtrl(m_ptrControl);
		if( spActiveXCtrl )
		{
			
			BOOL bUniqueName = FALSE;
			spActiveXCtrl->IsUniqueName( _bstr_t(strName), &bUniqueName );
			if( !bUniqueName )
			{
				if( ::AfxMessageBox( IDS_NAME_EXIST_WARNING, MB_YESNO ) == IDYES )
				{
					BSTR bsrtNewName;
					spActiveXCtrl->GenerateUniqueName( _bstr_t(strName), &bsrtNewName );
					
					pWnd->SetWindowText( CString(bsrtNewName) );
					::SysFreeString( bsrtNewName );
				}
				bNeedSetFocus = true;
			}
			
		}

		if( bSetFocus && bNeedSetFocus)		
			if( GetFocus() != (CWnd*)this)
			{
				m_bCanEntry = false;
				pWnd->SetFocus();
				m_bCanEntry = true;								
			}
		
	}
}


void CWindowPage::OnSetfocusName() 
{
	//VerifyUniqueName();
}


void CWindowPage::OnKillfocusName() 
{
	VerifyUniqueName( true );
}
