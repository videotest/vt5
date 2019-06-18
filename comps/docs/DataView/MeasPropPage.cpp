// MeasPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "dataview.h"
#include "MeasPropPage.h"

#include "PropPage.h"
#include "\vt5\common2\misc_calibr.h"
#include "units_const.h"



#import "stdole2.tlb" rename_namespace("OcxSpace") exclude("GUID", "DISPPARAMS", "EXCEPINFO", \
	"OLE_XPOS_PIXELS", "OLE_YPOS_PIXELS", "OLE_XSIZE_PIXELS", "OLE_YSIZE_PIXELS", "OLE_XPOS_HIMETRIC", \
	"OLE_YPOS_HIMETRIC", "OLE_XSIZE_HIMETRIC", "OLE_YSIZE_HIMETRIC", "OLE_XPOS_CONTAINER", \
	"OLE_YPOS_CONTAINER", "OLE_XSIZE_CONTAINER", "OLE_YSIZE_CONTAINER", "OLE_HANDLE", "OLE_OPTEXCLUSIVE", \
	"OLE_CANCELBOOL", "OLE_ENABLEDEFAULTBOOL", "FONTSIZE", "OLE_COLOR", \
	"IUnknown", "IDispatch", "IEnumVARIANT", "IFont", "IPicture")

//ViewAX button referense
#ifdef _DEBUG
#import "..\\..\\..\\vt5\\debug\\comps\\vtcontrols.ocx" rename_namespace("OcxSpace") 
#else
#import "..\\..\\..\\vt5\\release\\comps\\vtcontrols.ocx" rename_namespace("OcxSpace")
#endif //_DEBUG

using namespace OcxSpace; 


/////////////////////////////////////////////////////////////////////////////
// CMeasPropPage dialog


/////////////////////////////////////////////////////////////////////////////
// CZoomNavigatorPage

IMPLEMENT_DYNCREATE(CMeasPropPage, CDialog);

// {31F2E3A1-D4B9-4047-B891-C0B3F0527EF4}
GUARD_IMPLEMENT_OLECREATE(CMeasPropPage, szMeasurementPropPageProgID, 
0x31f2e3a1, 0xd4b9, 0x4047, 0xb8, 0x91, 0xc0, 0xb3, 0xf0, 0x52, 0x7e, 0xf4);


BEGIN_INTERFACE_MAP(CMeasPropPage, CPropertyPageBase)
	INTERFACE_PART(CMeasPropPage, IID_IEventListener, EvList)
END_INTERFACE_MAP()


CMeasPropPage::CMeasPropPage()
	: CPropertyPageBase(CMeasPropPage::IDD)
{

	//{{AFX_DATA_INIT(CMeasPropPage)
	m_strName = _T("");
	m_strUnti = _T("");
	//}}AFX_DATA_INIT

	m_sName = c_szCMeasPropPage;
	m_sUserName.LoadString( IDS_MEAS_SHIFT_PAGE );
}


void CMeasPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeasPropPage)
	DDX_Text(pDX, IDC_CALIBR_NAME, m_strName);
	DDX_Text(pDX, IDC_CALIBR_UNIT, m_strUnti);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMeasPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CMeasPropPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CMeasPropPage message handlers

BOOL CMeasPropPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();	
	
	DoLoadSettings();
	Register();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CMeasPropPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CMeasPropPage)
	ON_EVENT(CMeasPropPage, IDC_HORZ, -600 /* Click */, OnClickHorz, VTS_NONE)
	ON_EVENT(CMeasPropPage, IDC_VERT, -600 /* Click */, OnClickVert, VTS_NONE)
	ON_EVENT(CMeasPropPage, IDC_DIAGONAL, -600 /* Click */, OnClickDiagonal, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMeasPropPage::OnClickHorz() 
{
	 OnBtnClick( 0 );
}

void CMeasPropPage::OnClickVert() 
{
	 OnBtnClick( 1 );	
}

void CMeasPropPage::DoLoadSettings()
{
	if( !IsInitialized() || !GetSafeHwnd() )
		return;
	long dir = ::GetValueInt( ::GetAppUnknown(), "ManualMeasure", "ManualMeasShiftMode", 0L );

	_DPushButtonPtr ptrHorz;
	if( GetDlgItem( IDC_HORZ ) )
		ptrHorz = GetDlgItem( IDC_HORZ )->GetControlUnknown();

	_DPushButtonPtr ptrVert;
	if( GetDlgItem( IDC_VERT ) )
		ptrVert = GetDlgItem( IDC_VERT )->GetControlUnknown();

	_DPushButtonPtr ptrDiag;
	if( GetDlgItem( IDC_DIAGONAL ) )
		ptrDiag = GetDlgItem( IDC_DIAGONAL )->GetControlUnknown();

	if( ptrHorz == 0 || ptrVert == 0 || ptrDiag == 0 )return;

	if( dir == 0 )
	{
		if( !ptrHorz->IsPressed())ptrHorz->SetPressedState();
		if( ptrVert->IsPressed())ptrVert->ResetPressedState();
		if( ptrDiag->IsPressed())ptrDiag->ResetPressedState();
	}
	else if( dir == 1 )
	{
		if( ptrHorz->IsPressed())ptrHorz->ResetPressedState();
		if( !ptrVert->IsPressed())ptrVert->SetPressedState();
		if( ptrDiag->IsPressed())ptrDiag->ResetPressedState();
	}
	else 
	{
		if( ptrHorz->IsPressed())ptrHorz->ResetPressedState();
		if( ptrVert->IsPressed())ptrVert->ResetPressedState();
		if( !ptrDiag->IsPressed())ptrDiag->SetPressedState();
	}

	IUnknown	*punkDoc = 0;
	IApplication	*papp = GetAppUnknown();
	papp->GetActiveDocument( &punkDoc );

	if( punkDoc )
	{
		IUnknown	*punk = GetActiveObjectFromDocument( punkDoc, szTypeObjectList );
		GUID	guid;
		::GetCalibration( punk, 0, &guid );

		CString strExtCalibr;
		strExtCalibr.LoadString( IDS_EXT_CALIBRATION );
		char	sz[255];
		strcpy( sz, (LPCSTR)strExtCalibr );
		::GetCalibrationFromTable( guid, 0, sz, 0 );
		punkDoc->Release();
		if( punk )punk->Release();
		m_strName =  sz;
	}
	m_strUnti = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	UpdateData( false );

}

void CMeasPropPage::OnBtnClick( long dir )
{
	_DPushButtonPtr ptrHorz;
	if( GetDlgItem( IDC_HORZ ) )
		ptrHorz = GetDlgItem( IDC_HORZ )->GetControlUnknown();

	_DPushButtonPtr ptrVert;
	if( GetDlgItem( IDC_VERT ) )
		ptrVert = GetDlgItem( IDC_VERT )->GetControlUnknown();

	_DPushButtonPtr ptrDiag;
	if( GetDlgItem( IDC_DIAGONAL ) )
		ptrDiag = GetDlgItem( IDC_DIAGONAL )->GetControlUnknown();

	if( dir == 0  )
	{
		ptrHorz->SetPressedState();
		ptrVert->ResetPressedState();
		ptrDiag->ResetPressedState();
	}
	else if( dir == 1  )
	{
		ptrHorz->ResetPressedState();
		ptrVert->SetPressedState();
		ptrDiag->ResetPressedState();
	}
	else if( dir == 2  )
	{
		ptrHorz->ResetPressedState();
		ptrVert->ResetPressedState();
		ptrDiag->SetPressedState();
	}

	::SetValue( ::GetAppUnknown(), "ManualMeasure", "ManualMeasShiftMode", dir );	
}

void CMeasPropPage::OnClickDiagonal() 
{
	OnBtnClick( 2 );
}

void CMeasPropPage::OnDestroy() 
{
	CDialog::OnDestroy();
	UnRegister();
}

void CMeasPropPage::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventNewSettings ) )
		DoLoadSettings();
}
