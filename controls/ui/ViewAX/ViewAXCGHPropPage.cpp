// ViewAXCGHPropPage.cpp: implementation of the CViewAXCGHPropPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewAXCGHPropPage.h"
#include "propbag.h"
#include "cgh_int.h"
#include "iface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CViewAXCGHPropPage, COlePropertyPage)

BEGIN_MESSAGE_MAP(CViewAXCGHPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CViewAXCGHPropPage)
	ON_BN_CLICKED(IDC_RATIO_ACTIVE, OnRatio1)
	ON_BN_CLICKED(IDC_RATIO_SELECTED, OnRatio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {F938184E-0564-4802-B17F-6392CEB82A21}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXCGHPropPage, "VIEWAX.ViewAXCGHPropPage.1",
0xf938184e, 0x564, 0x4802, 0xb1, 0x7f, 0x63, 0x92, 0xce, 0xb8, 0x2a, 0x21);

BOOL CViewAXCGHPropPage::CViewAXCGHPropPageFactory::UpdateRegistry(BOOL bRegister)
{
#if defined(NOGUARD)
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VIEWAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
#else
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_VIEWAX_PPG);
#endif
}

CViewAXCGHPropPage::CViewAXCGHPropPage() :
	COlePropertyPage(IDD, IDS_VIEWAX_CGH_PPG_CAPTION )
{
	//{{AFX_DATA_INIT(CViewAXCGHPropPage)
	m_bActive   = 1;
	m_bSelected = 0;
	m_bNegative = 0;
	m_bMono     = 0;
	//}}AFX_DATA_INIT
	m_bFilled = false;
}

void CViewAXCGHPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CViewAXCGHPropPage)
	DDX_Check(pDX, IDC_RATIO_ACTIVE, m_bActive );
	DDX_Check(pDX, IDC_RATIO_SELECTED, m_bSelected );
	DDX_Check(pDX, IDC_CHECK1, m_bNegative );
	DDX_Check(pDX, IDC_CHECK2, m_bMono );
	//}}AFX_DATA_MAP

	if( !m_bFilled )
	{
		m_bFilled = true;
		CComboBox *pWnd = (CComboBox *)GetDlgItem( IDC_COMBO1 );

		if( pWnd )
		{
			CString str;

			str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW0, CGH_VIEW0_DEF );
			pWnd->AddString( str );

			str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW1, CGH_VIEW1_DEF );
			pWnd->AddString( str );

			str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW2, CGH_VIEW2_DEF );
			pWnd->AddString( str );

			str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW3, CGH_VIEW3_DEF );
			pWnd->AddString( str );

			str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW4, CGH_VIEW4_DEF );
			pWnd->AddString( str );
		}
	}


	IViewAXPropBagPtr sptrBag = _get_viewAX_control();

	if( !pDX->m_bSaveAndValidate && sptrBag != 0 )
	{
		_variant_t varPane, varNegative, varMono;
		sptrBag->GetProperty( _bstr_t( szPaneNum ), &varPane );
		sptrBag->GetProperty( _bstr_t( szMonochrome ), &varNegative );
		sptrBag->GetProperty( _bstr_t( szNegative ), &varMono );

		if( varPane.vt == VT_EMPTY )
		{
			((CButton *)GetDlgItem( IDC_RATIO_ACTIVE ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_RATIO_SELECTED ))->SetCheck( 0 );
		}
		else
		{
			CString str;
		
			switch( varPane.lVal )
			{
			case 0:
				str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW0, CGH_VIEW0_DEF );
				break;
			case 1:
				str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW1, CGH_VIEW1_DEF );
				break;
			case 2:
				str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW2, CGH_VIEW2_DEF );
				break;
			case 3:
				str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW3, CGH_VIEW3_DEF );
				break;
			case 4:
				str = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_VIEW4, CGH_VIEW4_DEF );
				break;
			}
		
			( (CComboBox *)GetDlgItem( IDC_COMBO1 ) )->SelectString( 0, str );
			((CButton *)GetDlgItem( IDC_RATIO_ACTIVE ))->SetCheck( 0 );
			((CButton *)GetDlgItem( IDC_RATIO_SELECTED ))->SetCheck( 1 );

		}
		if( varNegative.vt != VT_EMPTY )
			( (CButton *)GetDlgItem( IDC_CHECK1 ) )->SetCheck( varNegative.lVal != 0 );
		if( varMono.vt != VT_EMPTY )
			( (CButton *)GetDlgItem( IDC_CHECK2 ) )->SetCheck( varMono.lVal != 0 );

		GetDlgItem( IDC_COMBO1 )->EnableWindow( ((CButton *)GetDlgItem( IDC_RATIO_ACTIVE ))->GetCheck() == 0 );

	}
	else if( sptrBag != 0 )
	{
		int nPane = ( (CComboBox *)GetDlgItem( IDC_COMBO1 ) )->GetCurSel();

		if( ((CButton *)GetDlgItem( IDC_RATIO_ACTIVE ))->GetCheck() != 0 )
			nPane = -1;

		int nNeg  = ( (CButton *)GetDlgItem( IDC_CHECK1 ) )->GetCheck() != 0;
		int nMono = ( (CButton *)GetDlgItem( IDC_CHECK2 ) )->GetCheck() != 0;

		sptrBag->SetProperty( _bstr_t( szPaneNum ),    _variant_t( long( nPane ) ) );
		sptrBag->SetProperty( _bstr_t( szMonochrome ), _variant_t( long( nNeg  ) ) );
		sptrBag->SetProperty( _bstr_t( szNegative ),   _variant_t( long( nMono ) ) );
	}

	DDP_PostProcessing(pDX);
}

sptrIViewCtrl CViewAXCGHPropPage::_get_viewAX_control()
{
	ULONG nControls = 0;
	LPDISPATCH FAR* ppDisp = GetObjectArray(&nControls);
	
	if( ppDisp[0] == 0 )
		return 0;

	sptrIViewCtrl sptrV( ppDisp[0] );
	if( sptrV == 0 )
		return 0;

	return sptrV;
}

void CViewAXCGHPropPage::OnRatio1() 
{
	GetDlgItem( IDC_COMBO1 )->EnableWindow( 0 );
}

void CViewAXCGHPropPage::OnRatio2() 
{
	GetDlgItem( IDC_COMBO1 )->EnableWindow( 1 );
}

