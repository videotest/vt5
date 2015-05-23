// Rectangle.cpp : Implementation of the CVTRectangle ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "Rectangle.h"
#include "RectanglePpg.h"
#include "shapeppg.h"
#include <msstkppg.h>

#include "jdraw\\DashLine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTRectangle, COleControl)


BEGIN_INTERFACE_MAP(CVTRectangle, COleControl)
	INTERFACE_PART(CVTRectangle, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTRectangle, IID_IAXCtrlDataSite, AXCtrlDataSite)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTRectangle, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVTRectangle)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTRectangle, COleControl)
	//{{AFX_MSG_MAP(CVTRectangle)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTRectangle, COleControl)
	//{{AFX_DISPATCH_MAP(CVTRectangle)
	DISP_PROPERTY_EX(CVTRectangle, "FillColor", GetFillColor, SetFillColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTRectangle, "TransparentFill", GetTransparentFill, SetTransparentFill, VT_BOOL)
	DISP_PROPERTY_EX(CVTRectangle, "LineColor", GetLineColor, SetLineColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTRectangle, "LineStyle", GetLineStyle, SetLineStyle, VT_I2)
	DISP_PROPERTY_EX(CVTRectangle, "LineWidth", GetLineWidth, SetLineWidth, VT_R8)
	DISP_PROPERTY_EX(CVTRectangle, "RoundedLine", GetRoundedLine, SetRoundedLine, VT_BOOL)
	DISP_PROPERTY_EX(CVTRectangle, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTRectangle, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTRectangle, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTRectangle, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTRectangle, COleControl)
	//{{AFX_EVENT_MAP(CVTRectangle)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

//added by akm 22_11_k5

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTRectangle, 1)
	//PROPPAGEID(CVTRectanglePropPage::guid)
	PROPPAGEID(CVTShapePropPage::guid)
//	PROPPAGEID(CLSID_StockColorPage)	
END_PROPPAGEIDS(CVTRectangle)
*/
static CLSID pages[1];

LPCLSID CVTRectangle::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTShapePropPage::guid;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTRectangle, "VTDESIGN.VTDRectangleCtrl.1",
	0xbc127b0f, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTRectangle, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs
extern const IID BASED_CODE IID_DVTDRectangle =
		{ 0xbc127b0d, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDRectangleEvents =
		{ 0xbc127b0e, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDRectangleOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTRectangle, IDS_VTDRECTANGLE, _dwVTDRectangleOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::CVTRectangleFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTRectangle

BOOL CVTRectangle::CVTRectangleFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTDRECTANGLE,
			IDB_VTDRECTANGLE,
			afxRegApartmentThreading,
			_dwVTDRectangleOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::CVTRectangle - Constructor

CVTRectangle::CVTRectangle()
{
	InitializeIIDs(&IID_DVTDRectangle, &IID_DVTDRectangleEvents);	
	InitDefaults();	
	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::~CVTRectangle - Destructor

CVTRectangle::~CVTRectangle()
{
}

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::DoPropExchange - Persistence support

void CVTRectangle::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::OnResetState - Reset control to default state

void CVTRectangle::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle::AboutBox - Display an "About" box to the user

void CVTRectangle::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDRECTANGLE);
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle InitDefaults
void CVTRectangle::InitDefaults()
{
	m_FillColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentFill	= TRUE;	

	m_LineColor			= 0x80000000 + COLOR_WINDOWTEXT;	
	m_LineStyle			= DL_SOLID;

	m_nLineWidth		= 50;

	m_bRoundedLine		= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle message handlers

DWORD CVTRectangle::GetControlFlags() 
{	
	return COleControl::GetControlFlags();// | windowlessActivate;
}

BOOL CVTRectangle::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

BOOL CVTRectangle::OnEraseBkgnd(CDC* pDC) 
{
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CVTRectangle::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTRectangle, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTRectangle::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTRectangle, PrintCtrl)
	return S_OK;
}

HRESULT CVTRectangle::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTRectangle, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTRectangle::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTRectangle, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTRectangle::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTRectangle, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle:: Drawing function
void CVTRectangle::DrawContext( HDC hDC, SIZE size )
{
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);

	//Now draw!

	CRect rc = CRect( 0, 0, size.cx, size.cy );
  

	//Filling
	if( !m_bTransparentFill )
	{
		CBrush brush( TranslateColor(m_FillColor) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rc, &brush );
		pdc->SelectObject( pOldBrush );
	}


	int nPenWidth = m_nLineWidth;
	rc = CRect(nPenWidth/2, nPenWidth/2, size.cx -nPenWidth/2, size.cy -nPenWidth/2 );

		   
	unsigned arrPattern[8];
	int nCount = CDashLine::GetPattern( arrPattern, ( m_bRoundedLine==TRUE ? true : false ),
				nPenWidth/*m_nLineWidth*/, m_LineStyle );

	CDashLine Line(*pdc, arrPattern, nCount);
	Line.SetPattern( arrPattern, nCount );
	Line.Rectangle( rc, TranslateColor( m_LineColor ), nPenWidth/*m_nLineWidth*/, m_bRoundedLine, m_LineStyle );	


}


void CVTRectangle::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rcArea = rcBounds;
	rcArea.DeflateRect( 0, 0, 1, 1 );
	SIZE sizeHM;
	sizeHM.cx = FROM_DOUBLE_TO_VTPIXEL( rcArea.Width() ) / 10;
	sizeHM.cy = FROM_DOUBLE_TO_VTPIXEL( rcArea.Height() ) / 10;

	int nOldMapMode = pdc->SetMapMode( MM_ANISOTROPIC );
	pdc->SetWindowExt( sizeHM.cx, -sizeHM.cy );
	pdc->SetViewportExt( rcArea.Width(), rcArea.Height() );	
	pdc->SetViewportOrg( rcArea.left, rcArea.Height() + rcArea.top );


	int nOldLineWidth = m_nLineWidth;
	m_nLineWidth /= 10;

	DrawContext( pdc->GetSafeHdc(), sizeHM );

	m_nLineWidth = nOldLineWidth;

	pdc->SetMapMode( nOldMapMode );

}


/////////////////////////////////////////////////////////////////////////////
// CVTRectangle  Serialization
/////////////////////////////////////////////////////////////////////////////
void CVTRectangle::Serialize(CArchive& ar)
{
	long nVersion = 1;

	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;
		ar<<m_FillColor;
		ar<<m_bTransparentFill;
		ar<<m_LineColor;
		ar<<(unsigned)m_LineStyle;
		ar<<m_nLineWidth;
		ar<<m_bRoundedLine;
	}
	else
	{			
		ar>>nVersion;
		ar>>m_FillColor;
		ar>>m_bTransparentFill;
		ar>>m_LineColor;
		
		unsigned uTemp;
		ar>>uTemp;
		m_LineStyle = (DashStyle)uTemp;
		
		ar>>m_nLineWidth;
		ar>>m_bRoundedLine;
	}

	COleControl::Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTRectangle::GetFillColor() 
{
	return m_FillColor;
}

void CVTRectangle::SetFillColor(OLE_COLOR nNewValue) 
{
	m_FillColor = nNewValue;
	BoundPropertyChanged( 1 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTRectangle::GetTransparentFill() 
{	
	return m_bTransparentFill;
}

void CVTRectangle::SetTransparentFill(BOOL bNewValue) 
{
	m_bTransparentFill = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTRectangle::GetLineColor() 
{
	return m_LineColor;
}

void CVTRectangle::SetLineColor(OLE_COLOR nNewValue) 
{	
	m_LineColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}
			  
/////////////////////////////////////////////////////////////////////////////
short CVTRectangle::GetLineStyle() 
{	
	return (short)m_LineStyle;
}

void CVTRectangle::SetLineStyle(short nNewValue) 
{
	m_LineStyle = (DashStyle)nNewValue;
}

/////////////////////////////////////////////////////////////////////////////
double CVTRectangle::GetLineWidth() 
{
	return FROM_VTPIXEL_TO_DOUBLE(m_nLineWidth);
}

void CVTRectangle::SetLineWidth(double fNewValue) 
{
	m_nLineWidth = FROM_DOUBLE_TO_VTPIXEL( fNewValue );

	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTRectangle::GetRoundedLine() 
{
	return m_bRoundedLine;
}

void CVTRectangle::SetRoundedLine(BOOL bNewValue) 
{
	m_bRoundedLine = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

BOOL CVTRectangle::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVTRectangle::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

//added by akm 22_11_k5
BOOL CVTRectangle::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTRectangle::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}