// VTProgressBarCtrl.cpp : Implementation of the CVTProgressBarCtrl ActiveX Control class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTProgressBarCtrl.h"
#include "VTProgressBarPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTProgressBarCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CVTProgressBarCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CVTProgressBarCtrl, COleControl)
	DISP_PROPERTY_EX_ID(CVTProgressBarCtrl, "Min", dispidMin, GetMin, SetMin, VT_I4)
	DISP_PROPERTY_EX_ID(CVTProgressBarCtrl, "Max", dispidMax, GetMax, SetMax, VT_I4)
	DISP_PROPERTY_EX_ID(CVTProgressBarCtrl, "Pos", dispidPos, GetPos, SetPos, VT_I4)
	DISP_PROPERTY_EX_ID(CVTProgressBarCtrl, "Smooth", dispidSmooth, GetSmooth, SetSmooth, VT_BOOL)
	//DISP_PROPERTY_EX_ID(CVTProgressBarCtrl, "ShowPercent", dispidShowPercent, GetShowPercent, SetShowPercent, VT_BOOL)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CVTProgressBarCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTProgressBarCtrl, 1)
	PROPPAGEID(CVTProgressBarPropPage::guid)
END_PROPPAGEIDS(CVTProgressBarCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTProgressBarCtrl, "VTPROGRESSBAR.VTProgressBarCtrl.1",
	0xed1cbcf6, 0x7824, 0x4294, 0x92, 0xd1, 0x23, 0xe, 0x9b, 0x26, 0xde, 0x2c)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTProgressBarCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DVTProgressBar =
		{ 0xD0DB664C, 0x3B19, 0x404B, { 0x8B, 0x54, 0x8A, 0xCA, 0x7A, 0x50, 0xBC, 0xC0 } };
const IID BASED_CODE IID_DVTProgressBarEvents =
		{ 0x799BA6E4, 0xC589, 0x47EF, { 0xB8, 0x96, 0xA1, 0x4, 0x73, 0x1B, 0x4A, 0xD9 } };



// Control type information

static const DWORD BASED_CODE _dwVTProgressBarOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTProgressBarCtrl, IDS_VTPROGRESSBAR, _dwVTProgressBarOleMisc)



// CVTProgressBarCtrl::CVTProgressBarCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTProgressBarCtrl

BOOL CVTProgressBarCtrl::CVTProgressBarCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VTPROGRESSBAR,
			IDB_VTPROGRESSBAR,
			afxRegApartmentThreading,
			_dwVTProgressBarOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CVTProgressBarCtrl::CVTProgressBarCtrl - Constructor

CVTProgressBarCtrl::CVTProgressBarCtrl()
{
	InitializeIIDs(&IID_DVTProgressBar, &IID_DVTProgressBarEvents);
	// TODO: Initialize your control's instance data here.
	m_lMin = m_lMax = m_lPos = 0;
	m_bShowPercent = m_bSmooth = FALSE;
	InitCurrState( );
}



// CVTProgressBarCtrl::~CVTProgressBarCtrl - Destructor

CVTProgressBarCtrl::~CVTProgressBarCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

void	CVTProgressBarCtrl::InitCurrState( )
{	// Инициализация элемента в соответствии с текущими настройками
    if( GetSafeHwnd( ) )
	{
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->SetRange32( m_lMin, m_lMax );
		pProgress->SetPos( m_lPos );
		SetSmooth( m_bSmooth );
	}
}

CString CVTProgressBarCtrl::get_output_text( )
{
	CString strPercent( _T("") );
	if( m_lMax > m_lMin )
	{
		float fp = 100.0f;
		fp *= (float)(GetPos() - m_lMin); 
		fp /= (float)(m_lMax - m_lMin);
	
		strPercent.Format(_T("%3.0f %%"), fp);
	}        

	return strPercent;
}


// CVTProgressBarCtrl::OnDraw - Drawing function

void CVTProgressBarCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	/*CDC memDC;
	CBitmap bmp;
	CBitmap *oldBitmap;
	
	memDC.CreateCompatibleDC( pdc );
	bmp.CreateCompatibleBitmap(pdc,rcBounds.Width(),rcBounds.Height());
	oldBitmap=memDC.SelectObject(&bmp);
*/
    DoSuperclassPaint( pdc, rcBounds);

	CRect rcClient;
	GetClientRect( &rcClient );

	if( m_bShowPercent )
	{
		CString strPercent( _T("") );
		strPercent = get_output_text( );
		pdc->SetBkMode( TRANSPARENT );	
		pdc->SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
		pdc->DrawText( strPercent, &rcClient, DT_VCENTER |  DT_CENTER | DT_SINGLELINE );
	}

//	pdc->BitBlt(rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height(),&memDC,0,0,SRCCOPY);
//	memDC.SelectObject(oldBitmap);
}



// CVTProgressBarCtrl::DoPropExchange - Persistence support

void CVTProgressBarCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CVTProgressBarCtrl::OnResetState - Reset control to default state

void CVTProgressBarCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CVTProgressBarCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CVTProgressBarCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("msctls_progress32"); 
	cs.style |= WS_CHILD| WS_VISIBLE |WS_BORDER;
	if( m_bSmooth )
		cs.style |= PBS_SMOOTH;
	else
		cs.style &= ~PBS_SMOOTH;
	return COleControl::PreCreateWindow(cs);
}



// CVTProgressBarCtrl::IsSubclassedControl - This is a subclassed control

BOOL CVTProgressBarCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CVTProgressBarCtrl::OnOcmCommand - Handle command messages

LRESULT CVTProgressBarCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}



// CVTProgressBarCtrl message handlers

LONG CVTProgressBarCtrl::GetMin(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( GetSafeHwnd( ) )
	{
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->GetRange( (int&)m_lMin , (int&)m_lMax );
	}

	return m_lMin;
}

void CVTProgressBarCtrl::SetMin(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( newVal > m_lMax )
		newVal = m_lMax;

    if( GetSafeHwnd( ) )
	{
		m_lMin = newVal;
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->SetRange32( m_lMin, m_lMax ); 
		SetModifiedFlag();
	}

	
}

LONG CVTProgressBarCtrl::GetMax(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if( GetSafeHwnd( ) )
	{
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->GetRange( (int&)m_lMin , (int&)m_lMax );
	}
	return m_lMax;
}

void CVTProgressBarCtrl::SetMax(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( newVal < m_lMin )
		newVal = m_lMin;

	if( GetSafeHwnd( ) )
	{
		m_lMax = newVal;
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->SetRange32( m_lMin, m_lMax );
		SetModifiedFlag();
	}

}

LONG CVTProgressBarCtrl::GetPos(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( !GetSafeHwnd( ) )
		return 0;
	
	CProgressCtrl *pProgress = (CProgressCtrl *) this;
	return m_lPos = pProgress->GetPos( );
}

void CVTProgressBarCtrl::SetPos(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( GetSafeHwnd( ) )
	{
		/*if( m_bShowPercent )
		{
			CString strPercent( _T("") );
			strPercent = get_output_text( );
			CDC *pdc = GetDC();
			if( pdc )
			{
				CRect rcInvalidate(0,0,0,0), rcClient(0,0,0,0);
				GetClientRect( &rcClient );
				pdc->DrawText( strPercent, &rcInvalidate, DT_CALCRECT );
				rcInvalidate.right = (rcClient.left + rcClient.Width() / 2) + rcInvalidate.Width();
				rcInvalidate.left = rcClient.left + rcClient.Width() / 2;
				rcInvalidate.bottom = (rcClient.top + rcClient.Height() / 2) + rcInvalidate.Height();
				rcInvalidate.top = rcClient.top + rcClient.Height() / 2;
				InvalidateRect( &rcInvalidate, TRUE );
            	ReleaseDC( pdc );
			}
		}*/

	 	CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->SetPos( newVal );
		m_lPos = pProgress->GetPos( );
		SetModifiedFlag();
	}	
}

BOOL CVTProgressBarCtrl::GetSmooth(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( GetSafeHwnd( ) )
	{
	 	CProgressCtrl *pProgress = (CProgressCtrl *) this;
		DWORD dwStyle = pProgress->GetStyle();
		m_bSmooth = dwStyle & PBS_SMOOTH;
	}
    return m_bSmooth;
}

void CVTProgressBarCtrl::SetSmooth(BOOL bNewValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_bSmooth = bNewValue;
	SetModifiedFlag();
	RecreateControlWindow();
}

/*BOOL CVTProgressBarCtrl::GetShowPercent()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_bShowPercent;
}


void CVTProgressBarCtrl::SetShowPercent(BOOL bNewValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bShowPercent = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}*/

void CVTProgressBarCtrl::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int iVer = 2; 
		ar << iVer;
		ar << m_lMin;
		ar << m_lMax;
		ar << m_lPos;
		ar << m_bSmooth;
		//ar << m_bShowPercent;
	}
	else
	{	// loading code
        int iVer = 0;
		ar >> iVer;
		ar >> m_lMin;
		ar >> m_lMax;
		ar >> m_lPos;
		
		if( iVer >= 2 )
		{
			ar >> m_bSmooth;
			//ar >> m_bShowPercent;
		}

		InitCurrState( );
	}

	COleControl::Serialize( ar );
}

int CVTProgressBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( GetSafeHwnd( ) )
	{
		CProgressCtrl *pProgress = (CProgressCtrl *) this;
		pProgress->SetRange32( m_lMin, m_lMax );
		pProgress->SetPos( m_lPos );
	}

	return 0;
}