// Ellipse.cpp : Implementation of the CVTEllipse ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "Ellipse.h"
#include "EllipsePpg.h"
#include "shapeppg.h"
#include <msstkppg.h>

#include "jdraw\\DashLine.h"
#include "jdraw\\Bezier.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTEllipse, COleControl)


BEGIN_INTERFACE_MAP(CVTEllipse, COleControl)
	INTERFACE_PART(CVTEllipse, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTEllipse, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTEllipse, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVTEllipse)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTEllipse, COleControl)
	//{{AFX_MSG_MAP(CVTEllipse)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTEllipse, COleControl)
	//{{AFX_DISPATCH_MAP(CVTEllipse)
	DISP_PROPERTY_EX(CVTEllipse, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEllipse, "TransparentBack", GetTransparentBack, SetTransparentBack, VT_BOOL)
	DISP_PROPERTY_EX(CVTEllipse, "FillColor", GetFillColor, SetFillColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEllipse, "TransparentFill", GetTransparentFill, SetTransparentFill, VT_BOOL)
	DISP_PROPERTY_EX(CVTEllipse, "LineColor", GetLineColor, SetLineColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEllipse, "LineStyle", GetLineStyle, SetLineStyle, VT_I2)
	DISP_PROPERTY_EX(CVTEllipse, "LineWidth", GetLineWidth, SetLineWidth, VT_R8)
	DISP_PROPERTY_EX(CVTEllipse, "RoundedLine", GetRoundedLine, SetRoundedLine, VT_BOOL)
	DISP_PROPERTY_EX(CVTEllipse, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTEllipse, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTEllipse, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTEllipse, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTEllipse, COleControl)
	//{{AFX_EVENT_MAP(CVTEllipse)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

//added by akm 22_11_k5

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTEllipse, 1)
	//PROPPAGEID(CVTEllipsePropPage::guid)
	PROPPAGEID(CVTShapePropPage::guid)
//	PROPPAGEID(CLSID_StockColorPage)	
END_PROPPAGEIDS(CVTEllipse)
*/
static CLSID pages[1];

LPCLSID CVTEllipse::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTShapePropPage::guid;
	m_PropertyAXStatus=true;

	return pages;
}
/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTEllipse, "VTDESIGN.VTDEllipseCtrl.1",
	0xbc127b13, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTEllipse, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

extern const IID BASED_CODE IID_DVTDEllipse =
		{ 0xbc127b11, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDEllipseEvents =
		{ 0xbc127b12, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDEllipseOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTEllipse, IDS_VTDELLIPSE, _dwVTDEllipseOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::CVTEllipseFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTEllipse

BOOL CVTEllipse::CVTEllipseFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTDELLIPSE,
			IDB_VTDELLIPSE,
			afxRegApartmentThreading,
			_dwVTDEllipseOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::CVTEllipse - Constructor

CVTEllipse::CVTEllipse()
{
	InitializeIIDs(&IID_DVTDEllipse, &IID_DVTDEllipseEvents);

	InitDefaults();

	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::~CVTEllipse - Destructor

CVTEllipse::~CVTEllipse()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::DoPropExchange - Persistence support

void CVTEllipse::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::OnResetState - Reset control to default state

void CVTEllipse::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::AboutBox - Display an "About" box to the user

void CVTEllipse::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDELLIPSE);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipse message handlers


void CVTEllipse::InitDefaults()
{
	m_BackColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentBack	= TRUE;	

	m_FillColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentFill	= TRUE;	

	m_LineColor			= 0x80000000 + COLOR_WINDOWTEXT;	
	m_LineStyle			= DL_SOLID;

	m_nLineWidth		= 50;

	m_bRoundedLine		= FALSE;

}


/////////////////////////////////////////////////////////////////////////////
DWORD CVTEllipse::GetControlFlags() 
{	
	return COleControl::GetControlFlags();// | windowlessActivate;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTEllipse::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTEllipse::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
HRESULT CVTEllipse::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTEllipse, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTEllipse::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTEllipse, PrintCtrl)
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTEllipse::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTEllipse, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTEllipse::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTEllipse, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTEllipse::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTEllipse, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
void CVTEllipse::DrawContext( HDC hDC, SIZE size )
{
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);

	//Now draw!

	CRect rc = CRect( 0, 0, size.cx, size.cy );


	//Background
	if( !m_bTransparentBack )
	{
		CBrush brush( TranslateColor(m_BackColor) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rc, &brush );
		pdc->SelectObject( pOldBrush );
	}	


	

	int nPenWidth = m_nLineWidth;
	//Work in mm/10
	rc = CRect(nPenWidth/2, nPenWidth/2, size.cx -nPenWidth/2, size.cy -nPenWidth/2 );


	int nPatternWidth = nPenWidth;//m_nLineWidth;

	if( nPatternWidth <=0 )
		nPatternWidth = 1;


	unsigned arrPattern[8];
	int nCount = CDashLine::GetPattern( arrPattern, ( m_bRoundedLine==TRUE ? true : false ),
				nPatternWidth, m_LineStyle );

	CDashLine Line(*pdc, arrPattern, nCount);
	Line.SetPattern( arrPattern, nCount );


	CPoint		EllipsePts[13];
	CRect		EllipseRect;
	CRgn		EllipseRgn;
	CRgn		EllipseFillRgn;

//RecreateEllipse
	CRect ellipseR = rc;
	//ellipseR.InflateRect(-m_nLineWidth/2, -m_nLineWidth/2, -m_nLineWidth/2-1,-m_nLineWidth/2-1);

	EllipseToBezier(ellipseR, EllipsePts);

	// Get bounding rect of ellipse
	// Note this does NOT include the pen width
	CPoint tl = EllipsePts[0];
	CPoint br = tl;
	for (int i = 1; i < 13; ++i)
	{
		if(EllipsePts[i].x < tl.x)
			tl.x = EllipsePts[i].x;
		else if (EllipsePts[i].x > br.x)
			br.x = EllipsePts[i].x;
		if(EllipsePts[i].y < tl.y)
			tl.y = EllipsePts[i].y;
		else if (EllipsePts[i].y > br.y)
			br.y = EllipsePts[i].y;
	}
	EllipseRect = CRect(tl, br);


	
//UpdateEllipseRgn
	// Get fill area of ellipse
	
	pdc->BeginPath();
	pdc->PolyBezier(EllipsePts, 13);
	pdc->EndPath();	
	EllipseFillRgn.CreateFromPath(pdc);
	

	
	// Add border width area of ellipse
	pdc->BeginPath();
	pdc->PolyBezier(EllipsePts, 13);
	// if you do not close the figure, 
	//	there will be a hole at the begin/end join
	pdc->CloseFigure();		
	pdc->EndPath();
	
	
	LOGBRUSH lbrush;
	lbrush.lbStyle = BS_SOLID;
	CPen Pen(PS_GEOMETRIC, nPenWidth/*m_nLineWidth*/, &lbrush);
	CPen* pOldPen = pdc->SelectObject(&Pen);
	pdc->WidenPath();

	
	EllipseRgn.CreateFromPath(pdc);
	pdc->SelectObject(pOldPen);

// Draw ellipse interior
	if( !m_bTransparentFill )
	{
		pdc->SelectClipRgn(&EllipseFillRgn);
		CBrush brush(TranslateColor(m_FillColor) );
		pdc->FillRect(&EllipseRect, &brush);		
		pdc->SelectClipRgn(0);
	}

// Draw dotted ellipse border
	pdc->BeginPath();
	Line.MoveTo(EllipsePts[0]);
	for (int i = 1; i < 12; i+=3)
		Line.BezierTo( EllipsePts+i );
	pdc->EndPath();	

	{
	// make pen and stroke path
	LOGBRUSH lbrush;
	lbrush.lbStyle = BS_SOLID;
	lbrush.lbColor = TranslateColor(m_LineColor);
	CPen Pen(PS_GEOMETRIC | PS_SOLID | 
				(m_bRoundedLine ? 
				(PS_JOIN_ROUND | PS_ENDCAP_ROUND):
				(PS_JOIN_MITER | PS_ENDCAP_FLAT)	)		, nPenWidth/*m_nLineWidth*/, &lbrush);
	CPen *pOldPen = pdc->SelectObject(&Pen);
	pdc->StrokePath();
	pdc->SelectObject(pOldPen);
	}


}

/////////////////////////////////////////////////////////////////////////////
// CVTEllipse::Drawing function
void CVTEllipse::OnDraw(
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
// CVTEllipse  Serialization
/////////////////////////////////////////////////////////////////////////////
void CVTEllipse::Serialize(CArchive& ar)
{
	long nVersion = 1;
	
	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;
		ar<<m_BackColor;
		ar<<m_bTransparentBack;
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
		ar>>m_BackColor;
		ar>>m_bTransparentBack;
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
// CVTEllipse  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTEllipse::GetBackColor() 
{
	return m_BackColor;
}

void CVTEllipse::SetBackColor(OLE_COLOR nNewValue) 
{
	m_BackColor = nNewValue;	
	BoundPropertyChanged( 1 );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTEllipse::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVTEllipse::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTEllipse::GetFillColor() 
{
	return m_FillColor;
}

void CVTEllipse::SetFillColor(OLE_COLOR nNewValue) 
{
	m_FillColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTEllipse::GetTransparentFill() 
{
	return m_bTransparentFill;
}

void CVTEllipse::SetTransparentFill(BOOL bNewValue) 
{
	m_bTransparentFill = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTEllipse::GetLineColor() 
{
	return m_LineColor;
}

void CVTEllipse::SetLineColor(OLE_COLOR nNewValue) 
{
	m_LineColor = nNewValue;
	BoundPropertyChanged( 5 );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVTEllipse::GetLineStyle() 
{
	return (short)m_LineStyle;
}

void CVTEllipse::SetLineStyle(short nNewValue) 
{	
	m_LineStyle = (DashStyle)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
double CVTEllipse::GetLineWidth() 
{
	return FROM_VTPIXEL_TO_DOUBLE(m_nLineWidth);
}

void CVTEllipse::SetLineWidth(double fNewValue) 
{
	m_nLineWidth = FROM_DOUBLE_TO_VTPIXEL( fNewValue );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTEllipse::GetRoundedLine() 
{
	return m_bRoundedLine;
}

void CVTEllipse::SetRoundedLine(BOOL bNewValue) 
{
	m_bRoundedLine = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

BOOL CVTEllipse::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVTEllipse::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

//added by akm 22_11_k5
BOOL CVTEllipse::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTEllipse::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}