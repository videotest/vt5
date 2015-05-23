// ViewAXCGHPropPage.cpp: implementation of the CViewAXCGHPropPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewAXStatUIPropPage.h"
#include "propbag.h"
#include "statui_int.h"
#include "statistics.h"
#include "iface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CViewAXStatUIPropPage, COlePropertyPage)

BEGIN_MESSAGE_MAP(CViewAXStatUIPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CViewAXStatUIPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {2E6BED74-74D7-4a19-8EDC-77187B18AAFE}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXStatUIPropPage, "VIEWAX.ViewAXStatUIPropPage.1",
0x2e6bed74, 0x74d7, 0x4a19, 0x8e, 0xdc, 0x77, 0x18, 0x7b, 0x18, 0xaa, 0xfe);


BOOL CViewAXStatUIPropPage::CViewAXStatUIPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage( bRegister, AfxGetInstanceHandle(), IDS_VIEWAX_PPG );
}

CViewAXStatUIPropPage::CViewAXStatUIPropPage() :
	COlePropertyPage( IDD, IDS_VIEWAX_STATUI_PPG_CAPTION )
{
	//{{AFX_DATA_INIT(CViewAXStatUIPropPage)
	//}}AFX_DATA_INIT
}

void CViewAXStatUIPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CViewAXStatUIPropPage)
	//}}AFX_DATA_MAP

	IViewAXPropBagPtr sptrBag = _get_viewAX_control();

	if( !pDX->m_bSaveAndValidate && sptrBag != 0 )
	{

		_variant_t varPanes;

		sptrBag->GetProperty( _bstr_t( PROPBAG_PANES ), &varPanes );

		if( varPanes.vt == VT_EMPTY )
		{
			sptrBag->SetProperty( _bstr_t( PROPBAG_PANES ), _variant_t( long( vtChartView )) );
			((CButton *)GetDlgItem( IDC_RATIO_CHART ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_RATIO_LEGEND ))->SetCheck( 0 );
			((CButton *)GetDlgItem( IDC_RATIO_TABLE ))->SetCheck( 0 );
		}
		else
		{
			((CButton *)GetDlgItem( IDC_RATIO_CHART ))->SetCheck( varPanes.lVal & vtChartView );
			((CButton *)GetDlgItem( IDC_RATIO_LEGEND ))->SetCheck( varPanes.lVal & vtLegendView );
			((CButton *)GetDlgItem( IDC_RATIO_TABLE ))->SetCheck( varPanes.lVal & vtTableView );
		}
	}
	else if( sptrBag != 0 )
	{
		DWORD dwPane = 0;
		
		if( ((CButton *)GetDlgItem( IDC_RATIO_CHART ))->GetCheck() )
			dwPane |= vtChartView;
		else if( ((CButton *)GetDlgItem( IDC_RATIO_LEGEND ))->GetCheck() )
			dwPane |= vtLegendView;
		else if( ((CButton *)GetDlgItem( IDC_RATIO_TABLE ))->GetCheck() )
			dwPane |= vtTableView;

		sptrBag->SetProperty( _bstr_t( PROPBAG_PANES ),   _variant_t( long( dwPane ) ) );
	}

	DDP_PostProcessing(pDX);
}

sptrIViewCtrl CViewAXStatUIPropPage::_get_viewAX_control()
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

