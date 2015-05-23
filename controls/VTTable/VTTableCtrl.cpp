// VTTableCtrl.cpp : Implementation of the CVTTableCtrl ActiveX Control class.

#include "stdafx.h"
#include "VTTable.h"
#include "VTTableCtrl.h"
#include "VTTablePropPage.h"
#include <comutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTTableCtrl, COleControl)

BEGIN_INTERFACE_MAP(CVTTableCtrl, COleControl)
	INTERFACE_PART(CVTTableCtrl, IID_IAXCtrlDataSite, AXCtrlDataSite)  // vanek - 27.08.2003
END_INTERFACE_MAP()

IMPLEMENT_AX_DATA_SITE(CVTTableCtrl)	// vanek - 27.08.2003

// Message map
BEGIN_MESSAGE_MAP(CVTTableCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// Dispatch map
BEGIN_DISPATCH_MAP(CVTTableCtrl, COleControl)
	DISP_FUNCTION_ID(CVTTableCtrl, "PasteFromClipboard", dispidPasteFromClipboard, PasteFromClipboard, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CVTTableCtrl, "CopyToClipboard", dispidCopyToClipboard, CopyToClipboard, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CVTTableCtrl, "RootSection", dispidRootSection, GetRootSection, SetRootSection, VT_BSTR)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CVTTableCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
// [vanek] SBT711: 22.12.2003
/*BEGIN_PROPPAGEIDS(CVTTableCtrl, 1)
	PROPPAGEID(CVTTablePropPage::guid)
END_PROPPAGEIDS(CVTTableCtrl)*/

static CLSID pages[1];

LPCLSID CVTTableCtrl::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;

	if( ::GetValueInt( ::GetAppUnknown(), "\\VTTable", "UseMainPane", 1 ) != 0 )
		pages[cPropPages++] = CVTTablePropPage::guid;

	return pages;
}



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTTableCtrl, "VTTABLE.VTTableCtrl.1",
	0xc63275f8, 0x6165, 0x4432, 0x90, 0x22, 0xc7, 0xcc, 0x5f, 0x49, 0xf2, 0xa6)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTTableCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DVTTable =
		{ 0xA4CCD95C, 0x7C07, 0x4B20, { 0xA2, 0x4A, 0xFE, 0xBA, 0x65, 0x92, 0x7A, 0x1D } };
const IID BASED_CODE IID_DVTTableEvents =
		{ 0x8321B3DD, 0x9A35, 0x4185, { 0xBC, 0xD, 0x8D, 0x21, 0x2D, 0x6C, 0xC1, 0x8F } };



// Control type information

static const DWORD BASED_CODE _dwVTTableOleMisc =
	//OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTTableCtrl, IDS_VTTABLE, _dwVTTableOleMisc)



// CVTTableCtrl::CVTTableCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTTableCtrl

BOOL CVTTableCtrl::CVTTableCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTTABLE,
			IDB_VTTABLE,
			afxRegApartmentThreading,
			_dwVTTableOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CVTTableCtrl::CVTTableCtrl - Constructor

CVTTableCtrl::CVTTableCtrl()
{
	InitializeIIDs(&IID_DVTTable, &IID_DVTTableEvents);
	// TODO: Initialize your control's instance data here.
	::SetRectEmpty( &m_RequestRect);
	m_sRootSection.Empty( );
	m_sTemplateInRtf.Empty( );
}



// CVTTableCtrl::~CVTTableCtrl - Destructor

CVTTableCtrl::~CVTTableCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CVTTableCtrl::OnDraw - Drawing function

void CVTTableCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    _draw_content( pdc, rcBounds );
}



// CVTTableCtrl::DoPropExchange - Persistence support

void CVTTableCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CVTTableCtrl::OnResetState - Reset control to default state

void CVTTableCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

// CVTTableCtrl message handlers
BOOL CVTTableCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	return COleControl::PreCreateWindow(cs);
}

int CVTTableCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create RichEdit
	RECT crt = {0};
	lpCreateStruct->style = (WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | 0x81C4 );
	//lpCreateStruct->style = WS_CHILD|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_AUTOVSCROLL;
	m_ctrlRichEdit.Create( lpCreateStruct->style /*| ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN*/, crt, IsWindow( GetSafeHwnd( ) ) ? this : 0, 1);
	m_ctrlRichEdit.SetEventMask( ENM_REQUESTRESIZE );
	_update_content( );
    return 0;
}

void CVTTableCtrl::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int iVer = 1;
		ar << iVer;
		ar << m_sTemplateInRtf;
		ar << m_sRootSection;
	}
	else
	{	// loading code
		int iVer = 0;
		ar >> iVer;
		m_sTemplateInRtf.Empty( );
		m_sRootSection.Empty( );
		ar >> m_sTemplateInRtf;
		ar >> m_sRootSection;
	
		_update_content( );
	}

	COleControl::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////////////					
BOOL	CVTTableCtrl::_update_content( )
{
	if( !m_ctrlRichEdit.GetSafeHwnd( ) )
		return FALSE;
	
	m_ctrlRichEdit.SetSel( 0, -1);
	m_ctrlRichEdit.Clear( );
	m_ctrlRichEdit.SetRTF( m_sTemplateInRtf );
	_find_and_replace_values( );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL	CVTTableCtrl::_draw_content( CDC* pdc, CRect rect )
{
	if( !pdc )
		return FALSE;

	pdc->FillRect( rect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)) );

	if( !m_ctrlRichEdit.GetSafeHwnd( ) )
		return FALSE;

	CDC *pctrl_dc = 0;
	pctrl_dc = m_ctrlRichEdit.GetDC();
	if( !pctrl_dc )
		return FALSE;

	int curr_dc_dpi_x = pdc->GetDeviceCaps( LOGPIXELSX ),
		curr_dc_dpi_y = pdc->GetDeviceCaps( LOGPIXELSY ),
		ctrl_dc_dpi_x = pctrl_dc->GetDeviceCaps( LOGPIXELSX ),
		ctrl_dc_dpi_y = pctrl_dc->GetDeviceCaps( LOGPIXELSY );

    
	CRect rc_request_in_curr_dpi;
	rc_request_in_curr_dpi.left = (long)( (double)(m_RequestRect.left * curr_dc_dpi_x) / ctrl_dc_dpi_x + 0.5);
	rc_request_in_curr_dpi.right = (long)( (double)(m_RequestRect.right * curr_dc_dpi_x) / ctrl_dc_dpi_x + 0.5);
	rc_request_in_curr_dpi.top = (long)( (double)(m_RequestRect.top * curr_dc_dpi_y) / ctrl_dc_dpi_y + 0.5);
	rc_request_in_curr_dpi.bottom = (long)( (double)(m_RequestRect.bottom * curr_dc_dpi_y) / ctrl_dc_dpi_y + 0.5);

	double fKh = 0., fKw = 0., fKres;
	fKh = (double)rect.Height( ) / (double)rc_request_in_curr_dpi.Height( ); 
	fKw = (double)rect.Width( ) / (double)rc_request_in_curr_dpi.Width( ); 
	if( fKh < fKw )
		fKres = fKh;
	else
		fKres = fKw;

	
	CRect rcTwips;
	rcTwips.left = 0;
	rcTwips.top = 0;
	rcTwips.right = ::MulDiv( rc_request_in_curr_dpi.Width( ), 1440,curr_dc_dpi_x);
	rcTwips.bottom = ::MulDiv( rc_request_in_curr_dpi.Height( ), 1440, curr_dc_dpi_y);
    
	int nOldMode = ::SetMapMode( pdc->m_hDC, MM_ANISOTROPIC );
	SIZE	szOldWindow = {0},
			szOldViewPort = {0};

	::SetWindowExtEx( pdc->m_hDC, rc_request_in_curr_dpi.Width(), rc_request_in_curr_dpi.Height( ), &szOldWindow );
	::SetViewportExtEx( pdc->m_hDC, (int)(fKres * rc_request_in_curr_dpi.Width( ) - 0.5) ,
		(int)(fKres * rc_request_in_curr_dpi.Height() - 0.5), &szOldViewPort );
	
	POINT	ptOrgOld = {0};
	::SetViewportOrgEx( pdc->m_hDC, rect.left, rect.top, &ptOrgOld );

	FORMATRANGE fr;
	fr.hdc = pdc->m_hDC;
	fr.hdcTarget = pdc->m_hAttribDC;
	fr.rc = rcTwips;
	fr.rcPage = CRect(0,0,0,0);
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;
	
	m_ctrlRichEdit.FormatRange( &fr);

 	::SetViewportExtEx( pdc->m_hDC, szOldViewPort.cx, szOldViewPort.cy, NULL );
	::SetWindowExtEx( pdc->m_hDC, szOldWindow.cx, szOldWindow.cy, NULL );
	::SetViewportOrgEx( pdc->m_hDC, ptOrgOld.x, ptOrgOld.y, 0 );
	::SetMapMode( pdc->m_hDC, nOldMode );
    
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL	CVTTableCtrl::_find_and_replace_values( )
{
	long	lBeginPos = 0, 
			lEndPos = -1;

	_find_tokenes( &lBeginPos, &lEndPos );
	while( lBeginPos != -1 )
	{
		if( (lEndPos - lBeginPos) > 1 ) 
		{
			
			m_ctrlRichEdit.SetSel( -1, 0 );
			m_ctrlRichEdit.SetSel( lBeginPos + 1, lEndPos );
			
			CString sText = m_ctrlRichEdit.GetSelText( );

			{	
				// get from shell.data
				_variant_t	var;
				var = ::GetValue( ::GetAppUnknown( ), m_sRootSection, sText, var );
				
				// replace sel
				sText.Empty();
				m_ctrlRichEdit.SetSel( -1, 0 );																	
				m_ctrlRichEdit.SetSel( lBeginPos, lEndPos + 1 );
				sText = m_ctrlRichEdit.GetSelText( );

				m_ctrlRichEdit.ReplaceSel( _bstr_t(var), TRUE );

				m_ctrlRichEdit.GetSel( lBeginPos, lEndPos );
			}
			 
		}

		lBeginPos = lEndPos;
		_find_tokenes( &lBeginPos, &lEndPos );
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void	CVTTableCtrl::_find_tokenes( long *plBeginTokenIdx, long *plEndTokenIdx )
{
	if( !plBeginTokenIdx || !plEndTokenIdx )
		return;

	FINDTEXTEX	stfind = { 0 };
	stfind.chrg.cpMin = *plBeginTokenIdx;
	stfind.chrg.cpMax = -1;
	stfind.lpstrText = _T("[");
	m_ctrlRichEdit.SetSel( -1, 0); 
	*plBeginTokenIdx = m_ctrlRichEdit.FindText( FR_DOWN, &stfind );
	
	if( *plBeginTokenIdx == -1 )
		return;

	stfind.chrg.cpMin = *plBeginTokenIdx;
	stfind.lpstrText = _T("]");
	m_ctrlRichEdit.SetSel( -1, 0);
	*plEndTokenIdx = m_ctrlRichEdit.FindText( FR_DOWN, &stfind );
    
	return;
}

void CVTTableCtrl::PasteFromClipboard(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_ctrlRichEdit.SetSel( 0, -1);
	m_ctrlRichEdit.Clear( );
	m_ctrlRichEdit.Paste( );
	m_sTemplateInRtf = m_ctrlRichEdit.GetRTF( );
	_find_and_replace_values( );
	
	InvalidateControl();

	// vanek - 27.08.2003
	FireCtrlPropChange( GetControllingUnknown() );
}

void CVTTableCtrl::CopyToClipboard(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CString temp = m_ctrlRichEdit.GetRTF( );
	m_ctrlRichEdit.SetRTF( m_sTemplateInRtf );
	m_ctrlRichEdit.SetSel( 0, -1);
	m_ctrlRichEdit.Cut( );
	m_ctrlRichEdit.SetRTF( temp );
}

BOOL CVTTableCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	REQRESIZE* prr = (REQRESIZE*)lParam;
    if( prr->nmhdr.code == EN_REQUESTRESIZE )
    {
		m_RequestRect = prr->rc;
        *pResult = NULL;
        return TRUE;
    }
	return COleControl::OnNotify(wParam, lParam, pResult);
}

BSTR CVTTableCtrl::GetRootSection(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_sRootSection.AllocSysString();
}

void CVTTableCtrl::SetRootSection(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( newVal )
	{
		m_sRootSection = newVal;
		m_ctrlRichEdit.SetSel( 0, -1);
		m_ctrlRichEdit.Clear( );
		m_ctrlRichEdit.SetRTF( m_sTemplateInRtf );
		_find_and_replace_values( );
        SetModifiedFlag();
		
		// vanek - 27.08.2003
		FireCtrlPropChange( GetControllingUnknown() );
	}
}