// MeasPages.cpp : implementation file
//

#include "stdafx.h"
#include "measmanual.h"
#include "MeasPages.h"
#include "\vt5\common2\misc_calibr.h"

#import "stdole2.tlb" rename_namespace("OcxSpace") exclude("GUID", "DISPPARAMS", "EXCEPINFO", \
	"OLE_XPOS_PIXELS", "OLE_YPOS_PIXELS", "OLE_XSIZE_PIXELS", "OLE_YSIZE_PIXELS", "OLE_XPOS_HIMETRIC", \
	"OLE_YPOS_HIMETRIC", "OLE_XSIZE_HIMETRIC", "OLE_YSIZE_HIMETRIC", "OLE_XPOS_CONTAINER", \
	"OLE_YPOS_CONTAINER", "OLE_XSIZE_CONTAINER", "OLE_YSIZE_CONTAINER", "OLE_HANDLE", "OLE_OPTEXCLUSIVE", \
	"OLE_CANCELBOOL", "OLE_ENABLEDEFAULTBOOL", "FONTSIZE", "OLE_COLOR", \
	"IUnknown", "IDispatch", "IEnumVARIANT", "IFont", "IPicture")

//ViewAX button referense
#import <vtcontrols.ocx> rename_namespace("OcxSpace") 
using namespace OcxSpace; 

/////////////////////////////////////////////////////////////////////////////
// CMeasPage dialog
IMPLEMENT_DYNCREATE(CMeasPage, CDialog);

// {4988ADCD-0438-43d2-94B7-E2434C1F8D53}
GUARD_IMPLEMENT_OLECREATE(CMeasPage, "MeasManual.MeasPage",
0x4988adcd, 0x438, 0x43d2, 0x94, 0xb7, 0xe2, 0x43, 0x4c, 0x1f, 0x8d, 0x53);

BEGIN_INTERFACE_MAP(CMeasPage, CPropertyPageBase)
	INTERFACE_PART(CMeasPage, IID_IEventListener, EvList)
END_INTERFACE_MAP()


CMeasPage::CMeasPage(UINT nID) : CPropertyPageBase(nID)
{
	m_sName = c_szCMeasPage;
	m_sUserName.LoadString(IDS_MEASPAGE);
	//{{AFX_DATA_INIT(CMeasPage)
	m_strParamName = _T("");
	m_strValue = _T("");
	m_strUnit = _T("");
	m_strCalibr = _T("");
	//}}AFX_DATA_INIT
	_OleLockApp( this );
}


CMeasPage::~CMeasPage()
{
	_OleUnlockApp( this );
}


void CMeasPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeasPage)
	DDX_Text(pDX, IDC_PARAM_NAME,	m_strParamName);
	DDX_Text(pDX, IDC_VALUE,		m_strValue);
	DDX_Text(pDX, IDC_MEAS_UNIT,	m_strUnit);
	DDX_Text(pDX, IDC_CALIBR,		m_strCalibr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMeasPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CMeasPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasPage message handlers

BOOL CMeasPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();

	//Register();
	
	if (IsInitialized())
	{
		DoLoadSettings();
		UpdateData(false);
	}
	return TRUE;
}

bool CMeasPage::OnSetActive()
{
	if (IsInitialized())
		DoLoadSettings();

	return true;
}

void CMeasPage::DoLoadSettings()
{
//	double fX = 1., 
//	double fY = 1., 
//	long lUnit = 0;
//	::GetCalibrValues(&fX, &fY, &lUnit);
	if( !GetSafeHwnd() )	return;

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

	//::GetCalibrationUnit( 
	m_strValue	= ::GetValueString(GetAppUnknown(false), szManualMeasure, szCurrentValue, "");
	m_strParamName = ::GetValueString(GetAppUnknown(false), szManualMeasure, szCurrentParam, "?");
	m_strUnit = ::GetValueString(GetAppUnknown(false), szManualMeasure, szCurrentUnitName, "?");
	m_strCalibr = ::GetValueString(GetAppUnknown(false), szManualMeasure, szCurrentCalibrName, "?");

	if( GetSafeHwnd() )
		UpdateData(false);
}

void CMeasPage::DoStoreSettings()
{
	if( GetSafeHwnd() )
	{
		if (!UpdateData())
			return;
	}

//	::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentValue, m_strValue);
//	::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentParam, m_strParamName);
}


BEGIN_EVENTSINK_MAP(CMeasPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CMeasPage)
	ON_EVENT(CMeasPage, IDC_HORZ, -600 /* Click */, OnClickHorz, VTS_NONE)
	ON_EVENT(CMeasPage, IDC_VERT, -600 /* Click */, OnClickVert, VTS_NONE)
	ON_EVENT(CMeasPage, IDC_DIAGONAL, -600 /* Click */, OnClickDiagonal, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMeasPage::OnClickHorz() 
{
	OnBtnClick( 0 );
}

void CMeasPage::OnClickVert() 
{
	OnBtnClick( 1 );
}


void CMeasPage::OnBtnClick( long dir )
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

void CMeasPage::OnClickDiagonal() 
{
	OnBtnClick( 2 );
	
}

void CMeasPage::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventNewSettings ) )
		DoLoadSettings();
}

void CMeasPage::OnDestroy() 
{
	CPropertyPageBase::OnDestroy();
	//UnRegister();	
}
