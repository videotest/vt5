// ReportPage.cpp : implementation file
//

#include "stdafx.h"
#include "axeditor.h"
#include "ReportPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "printconst.h"
/////////////////////////////////////////////////////////////////////////////
// CReportPage property page


// {28212154-F40A-4cbc-8003-B0EFD111E56C}
GUARD_IMPLEMENT_OLECREATE(CReportPage, "axeditor.ReportPage", 
0x28212154, 0xf40a, 0x4cbc, 0x80, 0x3, 0xb0, 0xef, 0xd1, 0x11, 0xe5, 0x6c);


IMPLEMENT_DYNCREATE(CReportPage, COlePropertyPage)

CReportPage::CReportPage() : 
	COlePropertyPage( CReportPage::IDD, IDS_REPORT_PAGE_TITLE )
{
	//{{AFX_DATA_INIT(CReportPage)
	m_fHeight = 0.0;
	m_fLeft = 0.0;
	m_fWidth = 0.0;
	m_fTop = 0.0;
	m_bHorzScale = FALSE;
	m_bVertScale = FALSE;
	m_fDPI = 0.0;
	m_fZoom = 0.0;
	//}}AFX_DATA_INIT
}

CReportPage::~CReportPage()
{
}

void CReportPage::DoDataExchange(CDataExchange* pDX)
{
	COlePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportPage)
	DDX_Control(pDX, IDC_ZOOM_SPIN, m_spinZoom);
	DDX_Control(pDX, IDC_DPI_SPIN, m_spinDPI);
	DDX_Control(pDX, IDC_WIDTH_SPIN, m_spinWidth);
	DDX_Control(pDX, IDC_TOP_SPIN, m_spinTop);
	DDX_Control(pDX, IDC_LEFT_SPIN, m_spinLeft);
	DDX_Control(pDX, IDC_HEIGHT_SPIN, m_spinHeight);
	DDX_Text(pDX, IDC_HEIGHT, m_fHeight);
	DDV_MinMaxDouble(pDX, m_fHeight, 1., 1000.);
	DDX_Text(pDX, IDC_LEFT, m_fLeft);
	DDV_MinMaxDouble(pDX, m_fLeft, 0., 1000.);
	DDX_Text(pDX, IDC_WIDTH, m_fWidth);
	DDV_MinMaxDouble(pDX, m_fWidth, 1., 1000.);
	DDX_Text(pDX, IDC_TOP, m_fTop);
	DDV_MinMaxDouble(pDX, m_fTop, 0., 1000.);
	DDX_Check(pDX, IDC_HORZ_SCALE, m_bHorzScale);
	DDX_Check(pDX, IDC_VERT_SCALE, m_bVertScale);
	DDX_Text(pDX, IDC_DPI, m_fDPI);
	DDV_MinMaxDouble(pDX, m_fDPI, 0., 10000.);
	DDX_Text(pDX, IDC_ZOOM, m_fZoom);
	DDV_MinMaxDouble(pDX, m_fZoom, 0., 10000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CReportPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CReportPage, COlePropertyPage)
	INTERFACE_PART(CReportPage, IID_IPropertyPage, MyPropertyPage)
	INTERFACE_PART(CReportPage, IID_IPropertyPage2, MyPropertyPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CReportPage, MyPropertyPage)



HRESULT CReportPage::XMyPropertyPage::SetPageSite( LPPROPERTYPAGESITE lpp )
{
	_try_nested(CReportPage, MyPropertyPage, SetPageSite)
	{
		return pThis->m_xPropertyPage.SetPageSite( lpp );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::Activate( HWND hWnd, LPCRECT pcRect, BOOL b )
{
	_try_nested(CReportPage, MyPropertyPage, Activate)
	{
		pThis->m_bActive = true;
		return pThis->m_xPropertyPage.Activate( hWnd, pcRect, b );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::Deactivate()
{
	_try_nested(CReportPage, MyPropertyPage, Deactivate)
	{
		pThis->m_bActive = false;
		return pThis->m_xPropertyPage.Deactivate();
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::GetPageInfo( LPPROPPAGEINFO ppageInfo )
{
	_try_nested(CReportPage, MyPropertyPage, GetPageInfo)
	{
		return pThis->m_xPropertyPage.GetPageInfo( ppageInfo );
	}
	_catch_nested;
}
HRESULT CReportPage::XMyPropertyPage::SetObjects( ULONG nCount, LPUNKNOWN* ppUnk )
{
	_try_nested(CReportPage, MyPropertyPage, SetObjects)
	{
		if( !nCount )
			return E_FAIL;

		if( nCount == 1  )
			pThis->CheckOwnUnknown( ppUnk[nCount] );

		return pThis->m_xPropertyPage.SetObjects( nCount, ppUnk );
	}
	_catch_nested;
}
HRESULT CReportPage::XMyPropertyPage::Show( UINT nShow )
{
	_try_nested(CReportPage, MyPropertyPage, Show)
	{
		return pThis->m_xPropertyPage.Show( nShow );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::Move(LPCRECT prect )
{
	_try_nested(CReportPage, MyPropertyPage, Move)
	{
		return pThis->m_xPropertyPage.Move( prect );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::IsPageDirty()
{
	_try_nested(CReportPage, MyPropertyPage, IsPageDirty)
	{
		return pThis->m_xPropertyPage.IsPageDirty();
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::Apply()
{
	_try_nested(CReportPage, MyPropertyPage, Apply)
	{
		pThis->OnOK();
		return pThis->m_xPropertyPage.Apply();
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::Help(LPCOLESTR lpole)
{
	_try_nested(CReportPage, MyPropertyPage, Help)
	{
		return pThis->m_xPropertyPage.Help( lpole );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::TranslateAccelerator( LPMSG lpmsg )
{
	_try_nested(CReportPage, MyPropertyPage, TranslateAccelerator)
	{
		return pThis->m_xPropertyPage.TranslateAccelerator( lpmsg );
	}
	_catch_nested;
}

HRESULT CReportPage::XMyPropertyPage::EditProperty( DISPID dispid )
{
	_try_nested(CReportPage, MyPropertyPage, EditProperty)
	{
		return pThis->m_xPropertyPage.EditProperty( dispid );
	}
	_catch_nested;
}





bool CReportPage::CheckOwnUnknown( IUnknown *punk )
{
	if( !CheckInterface( punk, IID_IReportCtrl ) )
		return false;

	IReportCtrlPtr	sptrReport( punk );

	if( sptrReport == NULL ) return false;

	CRect rcCtrlPos;

	sptrReport->GetDesignPosition( &rcCtrlPos );

	m_fLeft		= (int)FROM_VTPIXEL_TO_DOUBLE(rcCtrlPos.left);
	m_fTop		= (int)FROM_VTPIXEL_TO_DOUBLE(rcCtrlPos.top);
	m_fWidth	= (int)FROM_VTPIXEL_TO_DOUBLE(rcCtrlPos.Width());
	m_fHeight	= (int)FROM_VTPIXEL_TO_DOUBLE(rcCtrlPos.Height());

	sptrReport->GetHorzScale( &m_bHorzScale );
	sptrReport->GetVertScale( &m_bVertScale );
	sptrReport->GetDPI( &m_fDPI );
	sptrReport->GetZoom( &m_fZoom );	
	sptrReport->GetCloneObject( &m_bCloneObject );	

	m_ptrControl = punk;	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CReportPage message handlers

void CReportPage::OnOK() 
{
	if( !UpdateData() )
		return;
	
	if( m_ptrControl != 0 )
	{
		IReportCtrlPtr	sptrReport( m_ptrControl );
		CRect rcCtrlPos;

		rcCtrlPos = CRect(
				FROM_DOUBLE_TO_VTPIXEL( m_fLeft ),
				FROM_DOUBLE_TO_VTPIXEL( m_fTop ),
				FROM_DOUBLE_TO_VTPIXEL( m_fLeft + m_fWidth ),
				FROM_DOUBLE_TO_VTPIXEL( m_fTop  + m_fHeight )
			);

		sptrReport->SetDesignPosition( rcCtrlPos );
		sptrReport->SetFlag( 1 );

		sptrReport->SetHorzScale( m_bHorzScale );
		sptrReport->SetVertScale( m_bVertScale );
		sptrReport->SetDPI( m_fDPI );
		sptrReport->SetZoom( m_fZoom );
		sptrReport->SetCloneObject( m_bCloneObject );	


	}
		/*IActiveXCtrlPtr	sptrAX( m_ptrControl );

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

		sptrAX->SetRect( rect );
		sptrAX->SetStyle( dwStyle );
	}
	*/
}

BOOL CReportPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	m_spinHeight.SetRange( 1, 1000 );
	m_spinWidth.SetRange( 1, 1000 );
	m_spinTop.SetRange( 0, 1000 );
	m_spinLeft.SetRange( 0, 1000 );
	

	m_spinZoom.SetBuddy( GetDlgItem(IDC_DPI) );
	m_spinDPI.SetBuddy( GetDlgItem(IDC_ZOOM) );

	m_spinZoom.SetRange( 0, 1000 );
	m_spinDPI.SetRange( 0, 1000 );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
