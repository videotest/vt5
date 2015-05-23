// Line.cpp : Implementation of the CVTLine ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "Line.h"
#include "LinePpg.h"
#include "shapeppg.h"
#include <msstkppg.h>

#include "jdraw\\DashLine.h"

#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTLine, COleControl)


BEGIN_INTERFACE_MAP(CVTLine, COleControl)
	INTERFACE_PART(CVTLine, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTLine, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTLine, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVTLine)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTLine, COleControl)
	//{{AFX_MSG_MAP(CVTLine)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTLine, COleControl)
	//{{AFX_DISPATCH_MAP(CVTLine)
	DISP_PROPERTY_EX(CVTLine, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTLine, "TransparentBack", GetTransparentBack, SetTransparentBack, VT_BOOL)
	DISP_PROPERTY_EX(CVTLine, "LineColor", GetLineColor, SetLineColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTLine, "LineStyle", GetLineStyle, SetLineStyle, VT_I2)
	DISP_PROPERTY_EX(CVTLine, "LineWidth", GetLineWidth, SetLineWidth, VT_R8)
	DISP_PROPERTY_EX(CVTLine, "RoundedLine", GetRoundedLine, SetRoundedLine, VT_BOOL)
	DISP_PROPERTY_EX(CVTLine, "StartArrow", GetStartArrow, SetStartArrow, VT_I2)
	DISP_PROPERTY_EX(CVTLine, "EndArrow", GetEndArrow, SetEndArrow, VT_I2)
	DISP_PROPERTY_EX(CVTLine, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CVTLine, "HorizontalFlip", GetHorizontalFlip, SetHorizontalFlip, VT_I2)
	DISP_PROPERTY_EX(CVTLine, "VerticalFlip", GetVerticalFlip, SetVerticalFlip, VT_I2)
	DISP_PROPERTY_EX(CVTLine, "Orientation", GetOrientation, SetOrientation, VT_I2)
	DISP_FUNCTION(CVTLine, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTLine, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTLine, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTLine, COleControl)
	//{{AFX_EVENT_MAP(CVTLine)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTLine, 2)
	PROPPAGEID(CVTShapePropPage::guid)
	PROPPAGEID(CVTLinePropPage::guid)	
//	PROPPAGEID(CLSID_StockColorPage)	
END_PROPPAGEIDS(CVTLine)
*/
static CLSID pages[2];

LPCLSID CVTLine::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTShapePropPage::guid;
	pages[cPropPages++]=CVTLinePropPage::guid;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTLine, "VTDESIGN.VTDLineCtrl.1",
	0xbc127b0b, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTLine, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

extern const IID BASED_CODE IID_DVTDLine =
		{ 0xbc127b09, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDLineEvents =
		{ 0xbc127b0a, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDLineOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTLine, IDS_VTDLINE, _dwVTDLineOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTLine::CVTLineFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTLine

BOOL CVTLine::CVTLineFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTDLINE,
			IDB_VTDLINE,
			afxRegApartmentThreading,
			_dwVTDLineOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTLine::CVTLine - Constructor

CVTLine::CVTLine()
{
	InitializeIIDs(&IID_DVTDLine, &IID_DVTDLineEvents);

	InitDefaults();	
	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTLine::~CVTLine - Destructor

CVTLine::~CVTLine()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CVTLine::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTLine, PrintCtrl)			

		int nCur = (int)pThis->m_ftFlipHorizontal;
		nCur++;

		if( nCur < 0 )
			nCur++;

		if( nCur > 2 )
			nCur = 0;

		pThis->m_ftFlipHorizontal = (FlipType)nCur;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTLine::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTLine, PrintCtrl)

		int nCur = (int)pThis->m_ftFlipVertical;
		nCur++;

		if( nCur < 0 )
			nCur++;

		if( nCur > 2 )
			nCur = 0;

		pThis->m_ftFlipVertical = (FlipType)nCur;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTLine::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTLine, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTLine::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTLine, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTLine::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTLine, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle:: Drawing function
void CVTLine::DrawContext( HDC hDC, SIZE size )
{
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);


	ArrowType oldStartArrow = m_StartArrow;
	ArrowType oldEndArrow = m_EndArrow;		
	

	//Now draw!


	CRect rc = CRect( 0, 0, size.cx, size.cy );
	
	//rc.left = rc.right;

	CRect rcBounds = rc;


	//BackGround
	if( !m_bTransparentBack )
	{
		CBrush brush( TranslateColor(m_BackColor) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rc, &brush );
		pdc->SelectObject( pOldBrush );
	}

	int nPenWidth = m_nLineWidth;

	rc = CRect(nPenWidth/2, nPenWidth/2, size.cx - nPenWidth/2, size.cy - nPenWidth/2 );
	rcBounds = rc;

	unsigned arrPattern[8];
	int nCount = CDashLine::GetPattern( arrPattern, ( m_bRoundedLine==TRUE ? true : false ),
				nPenWidth/*m_nLineWidth*/, m_LineStyle );

	CDashLine dashline(*pdc, arrPattern, nCount);
	dashline.SetPattern( arrPattern, nCount );

	pdc->BeginPath();

	CRect rect = rcBounds;

	if( m_nOrientation == 3 || m_nOrientation == 7 )
	{		
		
		int nTemp;
		nTemp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = nTemp;		

		if( m_nOrientation == 7 )
		{
			m_StartArrow = m_EndArrow;
			m_EndArrow = oldStartArrow;
		}
	}
	else
	if( m_nOrientation == 2 || m_nOrientation == 6 /*Horizontal */ )
	{
		int nTemp;
		nTemp = rect.top;
		int nHeight = rect.Height() / 2;
		rect.top = nTemp + nHeight;
		rect.bottom = nTemp + nHeight;

		if( m_nOrientation == 6 )
		{
			m_StartArrow = m_EndArrow;
			m_EndArrow = oldStartArrow;
		}

	}
	else
	if( m_nOrientation == 1 || m_nOrientation == 5 )
	{
		//Do nothing
		if( m_nOrientation == 5 )
		{
			m_StartArrow = m_EndArrow;
			m_EndArrow = oldStartArrow;
		}

	}
	else
	if( m_nOrientation == 0 || m_nOrientation == 4 /*Vertical*/ )
	{
		int nTemp;
		nTemp = rect.left;
		int nWidth = rect.Width() / 2;
		rect.left = nTemp + nWidth;
		rect.right = nTemp + nWidth;

		if( m_nOrientation == 4 )
		{
			m_StartArrow = m_EndArrow;
			m_EndArrow = oldStartArrow;
		}

	}


	int m_lArrowSize, m_rArrowSize;

	ArrowType m_rArrow = m_StartArrow;
	ArrowType m_lArrow = m_EndArrow;

	m_lArrowSize = m_rArrowSize = 5;

	if( nPenWidth/*m_nLineWidth*/  * 2 > m_lArrowSize) 
		m_lArrowSize = m_rArrowSize = nPenWidth/*m_nLineWidth*/ * 2;

    CRect saveRect=rect;
	int arrowlength=2*m_lArrowSize;  //arrowlength is the start arrow length
	int arrowlength1=2*m_rArrowSize; //arrowlength1 is the end arrow length
	int dx=0,dy=0,dx1=0,dy1=0;
	long r2=(rect.top-rect.bottom)*(rect.top-rect.bottom)+(rect.left-rect.right)*(rect.left-rect.right);
	int l=(int)sqrt((double)r2);

	

    if (l)
	{
		dy=(arrowlength*(rect.top-rect.bottom))/l;
		dx=(arrowlength*(rect.left-rect.right))/l;
		dy1=(arrowlength1*(rect.top-rect.bottom))/l;
		dx1=(arrowlength1*(rect.left-rect.right))/l;
	}
	if (rect.top > rect.bottom)
	{
		rect.top -= nPenWidth/*m_nLineWidth*/ / 2;
		if (m_rArrow!=none )rect.top -= abs(dy1);
		rect.bottom += (nPenWidth/*m_nLineWidth*/  + 1) / 2;
		if (m_lArrow!=none)rect.bottom += abs(dy);
	}
	else
	{
		rect.top += (nPenWidth/*m_nLineWidth*/ + 1) / 2;
		if (m_rArrow!=none )rect.top += abs(dy1);
		rect.bottom -= nPenWidth/*m_nLineWidth*/ / 2;
		if (m_lArrow!=none)rect.bottom -=abs(dy);
	}

	if (rect.left > rect.right)
	{
		rect.left -= nPenWidth/*m_nLineWidth*/ / 2;
		if (m_rArrow!=none ) rect.left -=abs(dx1);
		rect.right += (nPenWidth/*m_nLineWidth*/ + 1) / 2;
		if (m_lArrow!=none)rect.right +=abs(dx);
	}
	else
	{
		rect.left += (nPenWidth/*m_nLineWidth*/ + 1) / 2;
		if (m_rArrow!=none) rect.left +=abs(dx1);
		rect.right -= nPenWidth/*m_nLineWidth*/ / 2;
		if (m_lArrow!=none) rect.right -=abs(dx);
	}


	if( m_nOrientation == 2 || m_nOrientation == 6 /*Horizontal*/ )
	{
		rect.top = rect.bottom;
	}
	else
	if( m_nOrientation == 0 || m_nOrientation == 4/*Vertical*/ )
	{
		rect.left = rect.right;
	}



	/*
	if( m_ftFlipVertical == ftNormal )
		rect.left = rect.right;
		*/


	


	dashline.MoveTo(rect.TopLeft());
	dashline.LineTo(rect.BottomRight());


	pdc->EndPath();

	LOGBRUSH lbrush;
	lbrush.lbStyle = BS_SOLID;
	lbrush.lbColor = TranslateColor(m_LineColor);
	CPen Pen(PS_GEOMETRIC | PS_SOLID | 
				(m_bRoundedLine ? 
				(PS_JOIN_ROUND | PS_ENDCAP_ROUND):
				(PS_JOIN_MITER | PS_ENDCAP_FLAT)	)
				, nPenWidth/*m_nLineWidth*/, &lbrush);
	CPen *pOldPen = pdc->SelectObject(&Pen);

	pdc->StrokePath();
		

	CBrush brush( TranslateColor(m_LineColor) );

	CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
	


	//draw left arrow
	DrawArrow(m_lArrow,saveRect.BottomRight(), rect.BottomRight(), 
		m_lArrowSize, pdc,dx,dy,arrowlength, &brush);
	//draw right arrow
    DrawArrow(m_rArrow,saveRect.TopLeft(), rect.TopLeft(), 
		m_rArrowSize, pdc,dx1,dy1,arrowlength1,&brush);			

	

	
	pdc->SelectObject(pOldPen);
	pdc->SelectObject(pOldBrush);	


	m_StartArrow = oldStartArrow;
	m_EndArrow = oldEndArrow;		



}
/////////////////////////////////////////////////////////////////////////////
// CVTLine:: Drawing function

void CVTLine::OnDraw(
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

void CVTLine::OnDrawMetafile(CDC* pDC, const CRect& rcBounds) 
{
	COleControl::OnDrawMetafile(pDC, rcBounds);
}

			   
void CVTLine::DrawArrow(ArrowType arrowType, 
						  POINT  p1, POINT p2, int arrowSize ,CDC *pDC,
						  int dx, int dy, int arrowlength, CBrush* pbrush)
{
		POINT points[4];
	    CRgn rgn;
	    switch (arrowType){
		case none:
			break;
		case pureArrow:
            //actually a triagle
			points[0]=p1;
			points[1]=points[2]=p2;
			points[1].x+=arrowSize*dy/arrowlength;
			points[1].y-=arrowSize*dx/arrowlength;
			points[2].x-=arrowSize*dy/arrowlength;
			points[2].y+=arrowSize*dx/arrowlength;
			rgn.CreatePolygonRgn(points, 3, ALTERNATE);
			//if(m_bBrush)
				pDC->FillRgn(&rgn,pbrush);
			pDC->Polygon(points,3);
			break;
		case openArrow:
			points[0]=p1;
			points[1]=points[2]=points[3]=p2;
			points[1].x+=arrowSize*dy/arrowlength;
			points[1].y-=arrowSize*dx/arrowlength;
			points[2].x-=arrowSize*dy/arrowlength;
			points[2].y+=arrowSize*dx/arrowlength;
            pDC->MoveTo(points[1]);
			pDC->LineTo(points[0]);
			pDC->LineTo(points[2]);
            pDC->MoveTo(points[0]);
			pDC->LineTo(points[3]);
			break;
		case stealthArrow:
			points[0]=p1;
			points[1]=points[2]=points[3]=p2;
			points[1].x-=(p1.x-p2.x);
			points[1].y-=(p1.y-p2.y);
			points[3]=points[1];
			points[1].x+=arrowSize*dy/arrowlength;
			points[1].y-=arrowSize*dx/arrowlength;
			points[3].x-=arrowSize*dy/arrowlength;
			points[3].y+=arrowSize*dx/arrowlength;
			rgn.CreatePolygonRgn(points, 4, ALTERNATE);
			//if(m_bBrush)
	            pDC->FillRgn(&rgn,pbrush);
			pDC->Polygon(points,4);
			break;
		case diamondArrow:
			points[0]=p1;
			points[2]=p2;
			points[1].x=points[3].x=(p1.x+p2.x)/2;
			points[1].y=points[3].y=(p1.y+p2.y)/2;
			//points[2].x-=(p1.x-p2.x);
			//points[2].y-=(p1.y-p2.y);
			points[1].x+=(long)(.5*arrowSize*dy/arrowlength);
			points[1].y-=(long)(.5*arrowSize*dx/arrowlength);
			points[3].x-=(long)(.5*arrowSize*dy/arrowlength);
			points[3].y+=(long)(.5*arrowSize*dx/arrowlength);
			rgn.CreatePolygonRgn(points, 4, ALTERNATE);
			//if(m_bBrush)
	            pDC->FillRgn(&rgn,pbrush);
			pDC->Polygon(points,4);
			break;
		case ovalArrow:
			POINT p;
			p.x=(p1.x+p2.x)/2;
			p.y=(p1.y+p2.y)/2;
      long r=(long)sqrt((double)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)))/2+1;
			CRect circleRect;
			circleRect.left=p.x-r;
			circleRect.right=p.x+r;
			circleRect.bottom=p.y+r;
			circleRect.top=p.y-r;
			pDC->Ellipse(&circleRect);
			break;
		}
}

/////////////////////////////////////////////////////////////////////////////
// CVTLine::DoPropExchange - Persistence support

void CVTLine::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

}


/////////////////////////////////////////////////////////////////////////////
// CVTLine::OnResetState - Reset control to default state

void CVTLine::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();	
}


/////////////////////////////////////////////////////////////////////////////
// CVTLine::AboutBox - Display an "About" box to the user

void CVTLine::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDLINE);
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVTLine InitDefaults
void CVTLine::InitDefaults()
{
	m_BackColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentBack	= TRUE;	

	m_LineColor			= 0x80000000 + COLOR_WINDOWTEXT;	
	m_LineStyle			= DL_SOLID;

	m_nLineWidth		= 50;

	m_bRoundedLine		= FALSE;

	m_StartArrow		= none;
	m_EndArrow			= none;

	m_ftFlipHorizontal	= ftNormal;
	m_ftFlipVertical	= ftPositive;

	m_nOrientation		= 2;

}


/////////////////////////////////////////////////////////////////////////////
DWORD CVTLine::GetControlFlags() 
{		
	return COleControl::GetControlFlags();// | windowlessActivate;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTLine::PreCreateWindow(CREATESTRUCT& cs) 
{		
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTLine::OnEraseBkgnd(CDC* pDC) 
{		
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CVTLine  Serialization
/////////////////////////////////////////////////////////////////////////////
void CVTLine::Serialize(CArchive& ar)
{
	long nVersion = 4;

	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;
		ar<<m_BackColor;
		ar<<m_bTransparentBack;
		ar<<m_LineColor;
		ar<<(unsigned)m_LineStyle;
		ar<<m_nLineWidth;
		ar<<m_bRoundedLine;
		ar<<(unsigned)m_StartArrow;
		ar<<(unsigned)m_EndArrow;
		ar<<(int)m_ftFlipHorizontal; 
		ar<<(int)m_ftFlipVertical;
		ar<<(short)m_nOrientation;
	}
	else
	{			
		ar>>nVersion;
		ar>>m_BackColor;
		ar>>m_bTransparentBack;
		ar>>m_LineColor;
		
		unsigned uTemp;
		ar>>uTemp;
		m_LineStyle = (DashStyle)uTemp;
		
		ar>>m_nLineWidth;
		ar>>m_bRoundedLine;

		ar>>uTemp;
		m_StartArrow = (ArrowType)uTemp;

		ar>>uTemp;
		m_EndArrow = (ArrowType)uTemp;

		if( nVersion >= 2 )
		{
			int nTemp;
			ar>>nTemp; m_ftFlipHorizontal = (FlipType)nTemp;
			ar>>nTemp; m_ftFlipVertical   = (FlipType)nTemp;
		}

		if( nVersion >= 4 )
			ar>>(short)m_nOrientation;
	}

	COleControl::Serialize( ar );
}


/////////////////////////////////////////////////////////////////////////////
// CVTLine  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTLine::GetBackColor() 
{
	return m_BackColor;
}

void CVTLine::SetBackColor(OLE_COLOR nNewValue) 
{	
	m_BackColor = nNewValue;
	BoundPropertyChanged( 1 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTLine::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVTLine::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTLine::GetLineColor() 
{
	return m_LineColor;
}

void CVTLine::SetLineColor(OLE_COLOR nNewValue) 
{
	m_LineColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetLineStyle() 
{
	return (short)m_LineStyle;
}

void CVTLine::SetLineStyle(short nNewValue) 
{
	m_LineStyle = (DashStyle)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
double CVTLine::GetLineWidth() 
{
	return FROM_VTPIXEL_TO_DOUBLE(m_nLineWidth);
}

void CVTLine::SetLineWidth(double fNewValue) 
{
	m_nLineWidth = FROM_DOUBLE_TO_VTPIXEL( fNewValue );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTLine::GetRoundedLine() 
{
	return m_bRoundedLine;	
}

void CVTLine::SetRoundedLine(BOOL bNewValue) 
{
	m_bRoundedLine = bNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetStartArrow() 
{
	return (short)m_StartArrow;
}

void CVTLine::SetStartArrow(short nNewValue) 
{
	m_StartArrow = (ArrowType)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetEndArrow() 
{
	return (short)m_EndArrow;
}

void CVTLine::SetEndArrow(short nNewValue) 
{
	m_EndArrow = (ArrowType)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTLine::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////
void CVTLine::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetHorizontalFlip() 
{
	return (short)m_ftFlipHorizontal;
}

/////////////////////////////////////////////////////////////////////////////
void CVTLine::SetHorizontalFlip(short nNewValue) 
{
	m_ftFlipHorizontal = (FlipType)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetVerticalFlip() 
{
	return (short)m_ftFlipVertical;
}

/////////////////////////////////////////////////////////////////////////////
void CVTLine::SetVerticalFlip(short nNewValue) 
{
	m_ftFlipVertical = (FlipType)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}


/////////////////////////////////////////////////////////////////////////////
short CVTLine::GetOrientation() 
{
	return m_nOrientation;
}

/////////////////////////////////////////////////////////////////////////////
void CVTLine::SetOrientation(short nNewValue) 
{	
	m_nOrientation = nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

//added by akm 22_11_k5
BOOL CVTLine::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTLine::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}