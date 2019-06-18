// VTSlider.cpp : Implementation of the CVTSlider ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTSlider.h"
#include "VTSliderPpg.h"

#include <msstkppg.h>

#include "FramesPropPage.h"

#include "utils.h"
#include "BaseColumn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#ifndef FOR_HOME_WORK
BEGIN_INTERFACE_MAP(CVTSlider, COleControl)
	INTERFACE_PART(CVTSlider, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CVTSlider, IID_IVtActiveXCtrl2, ActiveXCtrl)
	INTERFACE_PART(CVTSlider, IID_IVtActiveXCtrl3, ActiveXCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTSlider, ActiveXCtrl)
#endif


IMPLEMENT_DYNCREATE(CVTSlider, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTSlider, COleControl)
	//{{AFX_MSG_MAP(CVTSlider)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTSlider, COleControl)
	//{{AFX_DISPATCH_MAP(CVTSlider)
	DISP_PROPERTY_NOTIFY(CVTSlider, "Min", m_min, OnMinChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CVTSlider, "Max", m_max, OnMaxChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CVTSlider, "Cycled", m_bCycled, OnCycledChanged, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "ShowTick", GetShowTick, SetShowTick, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "TickFreq", GetTickFreq, SetTickFreq, VT_I2)
	DISP_PROPERTY_EX(CVTSlider, "PosLo", GetPosLo, SetPosLo, VT_I4)
	DISP_PROPERTY_EX(CVTSlider, "PosHi", GetPosHi, SetPosHi, VT_I4)
	DISP_PROPERTY_EX(CVTSlider, "ShowNums", GetShowNums, SetShowNums, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "AutoUpdate", GetAutoUpdate, SetAutoUpdate, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "LowKeyValue", GetLoKeyValue, SetLoKeyValue, VT_BSTR)
	DISP_PROPERTY_EX(CVTSlider, "HighKeyValue", GetHiKeyValue, SetHiKeyValue, VT_BSTR)
	DISP_PROPERTY_EX(CVTSlider, "Name", GetName, SetName, VT_BSTR)
	DISP_PROPERTY_EX(CVTSlider, "InnerRaised", GetInnerRaised, SetInnerRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "InnerSunken", GetInnerSunken, SetInnerSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "OuterRaised", GetOuterRaised, SetOuterRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "OuterSunken", GetOuterSunken, SetOuterSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "SingleSlide", GetSingleSlide, SetSingleSlide, VT_BOOL)
	DISP_FUNCTION(CVTSlider, "SetBothLimits", SetBothLimits, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CVTSlider, "StoreToData", StoreToData, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTSlider, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTSlider, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTSlider, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTSlider, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_ENABLED()	
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTSlider, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTSlider, COleControl)
	//{{AFX_EVENT_MAP(CVTSlider)
	EVENT_CUSTOM("PosChanged", FirePosChanged, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTSlider, 3)
	PROPPAGEID(CVTSliderPropPage::guid)
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTSlider)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTSlider, "VTCONTROLS.VTSlider.1",
	0x232343f7, 0x5d35, 0x475e, 0x8f, 0xf7, 0xf2, 0x4e, 0x91, 0x18, 0xab, 0x8)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTSlider, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTSlider =
		{ 0xbf93dfce, 0xd919, 0x4ab3, { 0xac, 0x2e, 0xaa, 0x7e, 0x75, 0x6, 0x9d, 0x38 } };
const IID BASED_CODE IID_DVTSliderEvents =
		{ 0xedfd94e7, 0x7b71, 0x4251, { 0x98, 0x8, 0x5c, 0x3b, 0xb3, 0xa5, 0xcc, 0x51 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTSliderOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTSlider, IDS_VTSLIDER, _dwVTSliderOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::CVTSliderFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTSlider

BOOL CVTSlider::CVTSliderFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTSLIDER,
			IDB_VTSLIDER,
			afxRegApartmentThreading,
			_dwVTSliderOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::CVTSlider - Constructor
static const FONTDESC _fontdesc =
  { sizeof(FONTDESC), OLESTR("MS Sans Serif"), FONTSIZE( 8 ), FW_THIN, 
     ANSI_CHARSET, FALSE, FALSE, FALSE };


CVTSlider::CVTSlider()
{
	InitializeIIDs(&IID_DVTSlider, &IID_DVTSliderEvents);
	m_bShowNums = FALSE;
	m_bShowTick = TRUE;
	m_nTickFreq = 10;
	m_nNumsCount = 10;
	m_nLo = 25;
	m_nHi = 75;
	m_min = 0;
	m_max = 100;
	m_bActive = FALSE;
	m_bCycled = FALSE;
	m_track = trackNone;

	m_bAutoUpdate	= FALSE;
	m_strName		= "";

	m_strKeyValueLo		= "";
	m_strKeyValueHi		= "";

	m_bInnerRaised = m_bInnerSunken =
		m_bOuterRaised = m_bOuterSunken = FALSE;


	m_iStartedDelta = 0;

	SetInitialSize( 300, 40 );

	m_bOddSliderFirst = true;

	m_bSingleSlide = FALSE;

	m_bSliderCross = FALSE;

	m_bUserChangeValue = false;

	m_bUseSystemFont = TRUE;

}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::~CVTSlider - Destructor

CVTSlider::~CVTSlider()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::OnDraw - Drawing function

CRect CVTSlider::CalcRect( const CRect rc, const Rects kind, int ipos )
{
	int	iSliderHeight;
	int	iLinePosY;
	int	iTickHeight;
	int	iNumHeight;

	{
		CDC	dc;
		dc.CreateDC( "Display", 0, 0, 0 );

		CFont font;
		if( ::GetFontFromDispatch( this, font, GetUseSystemFont() ) )
		{		
			CFont* pOldFont = (CFont*)dc.SelectObject( &font );
		
			CRect	rc;
			dc.DrawText( CString("1"), rc, DT_CALCRECT );

			iNumHeight = rc.Height();

			dc.SelectObject(pOldFont);			
		}

		dc.DeleteDC();
	}

	iTickHeight = 3; 

	if( !m_bShowTick && m_bShowNums )
		iTickHeight = 0;
	if( m_bShowTick && !m_bShowNums )
		iNumHeight = 0;
	if( !m_bShowTick && !m_bShowNums )
	{
		iTickHeight = 0;
		iNumHeight = 0;
	}

	iSliderHeight = rc.Height()-iTickHeight-iNumHeight;
	iLinePosY = iSliderHeight/3;
	

	int	iSliderWidth = int(iSliderHeight/3*2.7);
	int	imin = rc.left+iSliderWidth/2;
	int	imax = rc.right-iSliderWidth/2;
	

	CRect	rcResult;

	switch( kind )
	{
	case rectLine:
		rcResult.left = rc.left+iSliderWidth/2;
		rcResult.top = rc.bottom-iSliderHeight/3-2;
		rcResult.bottom = rc.bottom-iSliderHeight/3+2;
		rcResult.right = rc.right-iSliderWidth/2;
		break;
	case rectSlider:
	{
		int	pos = imin+int((double)(imax-imin)/(m_max-m_min)*((ipos?m_nHi:m_nLo)-m_min)+.5);

		rcResult.left = pos-iSliderWidth/2;
		rcResult.right = pos+iSliderWidth/2;
		rcResult.top = rc.bottom-iSliderHeight;
		rcResult.bottom = rc.bottom;
		
		break;
	}
	case rectNum:
	{
		int	pos;

		if( ipos <= m_nNumsCount )
		{
			pos = imin+int((double)(imax-imin)/(m_max-m_min)*(ipos)*m_nTickFreq+.5);
			//pos = imin+int((double)(imax-imin)/(m_max-m_min)*(ipos)*m_nTickFreq+.5);
		}
		else
			pos = imax;

		rcResult.top = rc.top;
		rcResult.bottom = rc.top+iNumHeight;
		rcResult.left = pos-iSliderWidth/2;
		rcResult.right = pos+iSliderWidth/2;
		break;
	}
	case rectTick:
	{
		int	pos;

		if( ipos <= m_nNumsCount )
		{
			pos = imin+int((double)(imax-imin)/(m_max-m_min)*(ipos)*m_nTickFreq+.5);
			//pos = imin+int((double)(imax-imin)/(m_max-m_min)*(ipos)*m_nTickFreq+.5);
		}
		else
			pos = imax;

		rcResult.left = pos;
		rcResult.right = pos;

		rcResult.top = rc.top+iNumHeight;
		rcResult.bottom = rc.top+iNumHeight+iTickHeight;
		
		rcResult.top = max( rcResult.CenterPoint().y-3, rcResult.top );
		rcResult.bottom= min( rcResult.CenterPoint().y+3, rcResult.bottom );

		break;
	}				 
	}

	return rcResult;
}

void CVTSlider::DrawLine( CDC *pdc, CRect rc )
{
	bool bDisabled = GetSafeHwnd() && !IsWindowEnabled();

	pdc->DrawEdge( rc, BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT );
	CPen* pOldPen = (CPen*)pdc->SelectStockObject( BLACK_PEN );

	CRect	rc1, rc2;

	rc1 = CalcRect( m_rectLast, rectSlider, 0 );
	rc2 = CalcRect( m_rectLast, rectSlider, 1 );

	if( m_bCycled )
	{
		CPen	penFr( PS_SOLID, 0, RGB( 255, 0, 0 ) );
		CPen	penFg( PS_SOLID, 0, bDisabled ? ::GetSysColor( COLOR_GRAYTEXT ) : RGB( 0, 0, 0 ) );

		pdc->SelectObject( &penFr );

		int	y1 = rc.CenterPoint().y-1;
		int	y2 = rc.CenterPoint().y-3;
		int	x1 = rc1.CenterPoint().x;
		int	x2 = rc2.CenterPoint().x;


		if( x1 <= x2 )
		{
			pdc->MoveTo( x1, y1 );
			pdc->LineTo( x2, y1 );
			pdc->MoveTo( x1, y2 );
			pdc->LineTo( x2, y2 );
		}
		else
		{
			pdc->MoveTo( rc.left, y1 );
			pdc->LineTo( x2, y1 );
			pdc->MoveTo( x1, y1 );
			pdc->LineTo( rc.right, y1 );
			pdc->MoveTo( rc.left, y2 );
			pdc->LineTo( x2, y2 );
			pdc->MoveTo( x1, y2 );
			pdc->LineTo( rc.right, y2 );
		}

		pdc->SelectStockObject( BLACK_PEN );
	}

	pdc->SelectObject( pOldPen );

	
}

void CVTSlider::DrawSlider( CDC *pdc, CRect rc, BOOL bActive )
{
	bool bDisabled = GetSafeHwnd() &&!IsWindowEnabled();

	POINT	pts[3];
	int	xc = rc.CenterPoint().x;
	int	yc = (rc.bottom*2+rc.top)/3;
	int	x1 = rc.left;
	int	y1 = rc.bottom;
	int	x2 = rc.right;
	int	y2 = rc.bottom;
	int	x3 = xc;
	int	y3 = rc.top;

	CBrush	brLight( !bDisabled ? ::GetSysColor( COLOR_3DHIGHLIGHT ) : ::GetSysColor( COLOR_GRAYTEXT )  );
	CBrush	brDark( ::GetSysColor( COLOR_3DSHADOW ) );
	CBrush	brBody( ::GetSysColor( COLOR_3DFACE ) );

	CPen* pOldPen = (CPen*)pdc->SelectStockObject( NULL_PEN );

	pts[0].x = x1;
	pts[0].y = y1;
	pts[1].x = x2;
	pts[1].y = y2;
	pts[2].x = xc;
	pts[2].y = yc;

	CBrush* pOldBrush = pdc->SelectObject( &brDark );
	pdc->Polygon( pts, 3 );

	pts[1].x = x3;
	pts[1].y = y3;

	pdc->SelectObject( &brLight );
	pdc->Polygon( pts, 3 );

	pts[0].x = x2;
	pts[0].y = y2;

	pdc->SelectObject( &brBody );
	pdc->Polygon( pts, 3 );

	if( GetEnabled() )
	{
		CPen pen;
		pen.CreatePen( PS_SOLID, 1, ::GetSysColor( COLOR_GRAYTEXT ) );		
		if( bDisabled )
		{
			pdc->SelectObject( &pen );
		}
		else
		{
			if( bActive )
				pdc->SelectStockObject( WHITE_PEN );
			else
				pdc->SelectStockObject( BLACK_PEN );
		}
	
		pdc->SelectStockObject( NULL_BRUSH );

		pts[0].x = x1;
		pts[0].y = y1;
		pts[1].x = x2;
		pts[1].y = y2;
		pts[2].x = x3;
		pts[2].y = y3;


		pdc->Polygon( pts, 3 );

		pdc->SelectObject( pOldPen );
	}
	
	pdc->SelectObject( pOldBrush );
	pdc->SelectObject( pOldPen );
}

void CVTSlider::DrawNum( CDC *pdc, CRect rc, int iNo )
{
	bool bDisabled = GetSafeHwnd() &&!IsWindowEnabled();

	CFont font;
	if( ::GetFontFromDispatch( this, font, GetUseSystemFont() ) )
	{

		CFont* pOldFont = (CFont*)pdc->SelectObject( &font );//SelectStockFont( &dc );

		COLORREF clrOld = pdc->SetTextColor( bDisabled ? ::GetSysColor( COLOR_GRAYTEXT ) : RGB( 0, 0, 0 ) );		

		CString	s;
		s.Format( "%d", iNo );
		pdc->DrawText( s, rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP );

		pdc->SelectObject( pOldFont );
		
		pdc->SetTextColor( clrOld );
	}
}

void CVTSlider::DrawTick( CDC *pdc, CRect rc )
{
	bool bDisabled = GetSafeHwnd() &&!IsWindowEnabled();

	CPen* pOldPen = NULL;
	CPen penDis( PS_SOLID, 1, ::GetSysColor( COLOR_GRAYTEXT ) );

	if( bDisabled )	
		pOldPen = (CPen*)pdc->SelectObject( &penDis );	
	else
		pOldPen = (CPen*)pdc->SelectStockObject( BLACK_PEN );

	pdc->MoveTo( rc.CenterPoint().x, rc.top );
	pdc->LineTo( rc.CenterPoint().x, rc.bottom );

	pdc->SelectObject( pOldPen );
}






void CVTSlider::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{



	// TODO: Replace the following code with your own drawing code.

	//m_nLo = m_nHi;

	CRect rc = rcBounds;
	//GetClientRect( &rc );
	
	CBrush br( ::GetSysColor( COLOR_3DFACE ) );
	pdc->FillRect( rc/*Invalid*/, &br );

	UINT	edgeType = 0;
	if(m_bInnerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_bInnerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_bOuterRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_bOuterSunken)
		edgeType |= BDR_SUNKENOUTER;
	if(edgeType != 0)
	{
		pdc->DrawEdge(CRect(0,0,rc.Width(),rc.Height()), edgeType, BF_RECT);

		rc.left		+= VT_BORDER_WIDTH;
		rc.top		+= VT_BORDER_WIDTH;
		rc.right	-= VT_BORDER_WIDTH;
		rc.bottom	-= VT_BORDER_WIDTH;

	}
	
	//To avoid erasing border

	pdc->IntersectClipRect( rc );
	//pdc->SetBoundsRect( rcInvalid, DCB_ACCUMULATE|DCB_ENABLE );

 

	pdc->SetBkMode( TRANSPARENT );

	CRect	rcAll = rc/*Bounds*/;

	rcAll.InflateRect( -2, -2 );

	bool bDisabled = GetSafeHwnd() &&!IsWindowEnabled();

	if( m_bActive && !bDisabled )
		pdc->DrawFocusRect( rcAll );

	rcAll.InflateRect( -2, -2 );
	m_rectLast = rcAll;

	DrawLine( pdc, CalcRect( rcAll, rectLine ) );

	if( m_bOddSliderFirst )
	{
		if( !m_bSingleSlide )
			DrawSlider( pdc, CalcRect( rcAll, rectSlider, 1 ), m_track==track2||m_track==trackBoth );

		DrawSlider( pdc, CalcRect( rcAll, rectSlider, 0 ), m_track==track1||m_track==trackBoth );
	}
	else
	{		
		DrawSlider( pdc, CalcRect( rcAll, rectSlider, 0 ), m_track==track1||m_track==trackBoth );

		if( !m_bSingleSlide )
			DrawSlider( pdc, CalcRect( rcAll, rectSlider, 1 ), m_track==track2||m_track==trackBoth );
	}
		
	int i=0;
	for( ; i <= m_nNumsCount; i++ )
	{
		if( m_bShowNums )
			DrawNum( pdc, CalcRect( rcAll, rectNum, i ), i*m_nTickFreq + m_min);
		if( m_bShowTick )
			DrawTick( pdc, CalcRect( rcAll, rectTick, i ) );
	}

	if( m_bShowNums )	//draw last tick
		DrawNum( pdc, CalcRect( rcAll, rectNum, i ), m_max );
	if( m_bShowTick )	//draw last tick
		DrawTick( pdc, CalcRect( rcAll, rectTick, i ) );
}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::DoPropExchange - Persistence support

void CVTSlider::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider::OnResetState - Reset control to default state

void CVTSlider::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	m_bShowNums = FALSE;
	m_bShowTick = TRUE;
	m_nTickFreq = 10;
	m_nNumsCount = 10;
	m_nLo = 25;
	m_nHi = 75;
	m_min = 0;
	m_max = 100;
	m_bActive = FALSE;

	m_track = trackNone;

	m_bAutoUpdate	= FALSE;
	m_strName		= "";
	
	m_strKeyValueLo	= "";
	m_strKeyValueHi		= "";
	
	m_bInnerRaised = m_bInnerSunken =
		m_bOuterRaised = m_bOuterSunken = FALSE;

	m_font.ReleaseFont();
	m_font.InitializeFont(&_fontdesc);	

	m_bUserChangeValue = false;

}


/////////////////////////////////////////////////////////////////////////////
// CVTSlider message handlers

void CVTSlider::OnMinChanged() 
{
	SetTickFreq( GetTickFreq() );

	SetPosLo( GetPosLo() );
	SetPosHi( GetPosHi() );

	SetModifiedFlag();
	Refresh();
}

void CVTSlider::OnMaxChanged() 
{
	SetTickFreq( GetTickFreq() );

	SetPosLo( GetPosLo() );
	SetPosHi( GetPosHi() );

	SetModifiedFlag();
	Refresh();
}

BOOL CVTSlider::GetShowTick() 
{
	return m_bShowTick;
}

void CVTSlider::SetShowTick(BOOL bNewValue) 
{
	m_bShowTick = bNewValue;

	SetModifiedFlag();
	Refresh();
}


short CVTSlider::GetTickFreq() 
{
	return m_nTickFreq;
}

void CVTSlider::SetTickFreq(short nNewValue) 
{
	m_nTickFreq = nNewValue;
	m_nNumsCount = (m_max-m_min)/m_nTickFreq;

	SetModifiedFlag();
	Refresh();
}

void CVTSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	bool bDisabled = GetSafeHwnd() &&!IsWindowEnabled();
	if( bDisabled )
		return;

	if( !GetEnabled() )
		return;

	m_bUserChangeValue = true;

	CRect	rc1 = CalcRect( m_rectLast, rectSlider, 0 );
	CRect	rc2 = CalcRect( m_rectLast, rectSlider, 1 );

	if( rc1 == rc2 )
		m_bSliderCross = TRUE;
	else
		m_bSliderCross = FALSE;

	if( m_bOddSliderFirst )
	{
		if( rc1.PtInRect( point ) )
		{
			m_bOddSliderFirst = true;
			m_track = track1;
			m_iOffset = point.x-rc1.CenterPoint().x;
			InvalidateSliderRect( 0 );
		}
		if( rc2.PtInRect( point ) )
		{
			m_bOddSliderFirst = false;
			m_track = track2;
			m_iOffset = point.x-rc2.CenterPoint().x;
			InvalidateSliderRect( 1 );
		}
	}
	else
	{
		if( rc2.PtInRect( point ) )
		{
			m_bOddSliderFirst = false;
			m_track = track2;
			m_iOffset = point.x-rc2.CenterPoint().x;
			InvalidateSliderRect( 1 );
		}
		if( rc1.PtInRect( point ) )
		{
			m_bOddSliderFirst = true;
			m_track = track1;
			m_iOffset = point.x-rc1.CenterPoint().x;
			InvalidateSliderRect( 0 );
		}
	}

	CRect	rc3 = rc1;

	if( m_nLo < m_nHi )
	{
		rc3.left = rc1.right;
		rc3.right = rc2.left;
	}
	else
	{
		rc3.left = rc2.right;
		rc3.right = rc1.left;
	}


	if( rc3.PtInRect( point ) )
	{
		m_track = trackBoth;
		m_iOffset = point.x-rc1.CenterPoint().x;
		m_iStartedDelta = m_nHi-m_nLo;
		InvalidateSliderRect( 0 );
		InvalidateSliderRect( 1 );
	}

	if(  point.x < rc1.left && m_nLo < m_nHi  )
		SetPosLo( m_nLo-m_nTickFreq );
	if(  point.x > rc1.right && m_nLo > m_nHi  )
		SetPosLo( m_nLo+m_nTickFreq );

	
	if( m_bSingleSlide )
	{
		if( point.x > rc1.right && m_nLo < m_nHi )
			SetPosLo( m_nLo+m_nTickFreq );
		if( point.x < rc1.left && m_nLo >= m_nHi )
			SetPosLo( m_nLo-m_nTickFreq );
	}
	

	if( point.x > rc2.right && m_nLo < m_nHi )
		SetPosHi( m_nHi+m_nTickFreq );
	if( point.x < rc2.left && m_nLo > m_nHi )
		SetPosHi( m_nHi-m_nTickFreq );

	if( m_track != trackNone )
	{
		SetCapture();
		//OutputDebugString("\n-----------------------------------Set Capture");
	}

	m_bUserChangeValue = false;

	COleControl::OnLButtonDown(nFlags, point);
}

void CVTSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	if( GetCapture() == this )
		ReleaseCapture();
	

	if( m_track == trackNone )
		return;

	InvalidateSliderRect( 0 );
	InvalidateSliderRect( 1 );

	
	//OutputDebugString("\n-----------------------------------RELEASE Capture");

	m_track = trackNone;
	
	COleControl::OnLButtonUp(nFlags, point);
}

void CVTSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_track == trackNone )
		return;

	m_bUserChangeValue = true;

	int	ipos = point.x - m_iOffset;
	int	ival = CalcValueFromPos( ipos );


	long nOld_Hi, nOld_Lo;
	
	nOld_Hi = m_nHi;
	nOld_Lo = m_nLo;

	if( !m_bSliderCross )
	{
	if( m_track == track1 )
	{
		SetPosLo( ival );
	}
	if( m_track == track2 )
	{
		SetPosHi( ival );
	}
	}

	if( m_bSliderCross )
	{
		if( /*m_track == track1 && nOld_Lo == m_nLo*/ival>m_nHi )
		{
			m_track = track2;
			SetPosHi( ival );
			m_nLo=nOld_Lo;
		}
		if(/* m_track == track2 && nOld_Hi == m_nHi*/ ival<m_nLo)
		{
			m_track = track1;
			SetPosLo( ival );
			m_nHi=nOld_Hi;
		}

		m_bSliderCross = FALSE;
	}


	if( m_track == trackBoth )
	{
		if( !m_bCycled )
		{
			SetPosLo( ival );			
			SetPosHi( ival+m_iStartedDelta );
		}
		else
		{
			int	posLow = ival;
			int	posHigh = ival+m_iStartedDelta;

			if( posLow < m_min )
				posLow += m_max - m_min;
			if( posHigh > m_max )
				posHigh -= m_max - m_min;

			SetBothLimits( posLow, posHigh );
		}
	}

	UpdateWindow();

	m_bUserChangeValue = false;


	COleControl::OnMouseMove(nFlags, point);
}


void CVTSlider::InvalidateSliderRect( int iSlider )
{
	if( !GetSafeHwnd() )
		return;

	CRect	rc = CalcRect( m_rectLast, rectSlider, iSlider );
	rc.InflateRect( 2, 3 );

	InvalidateRect( rc );

	if( m_bCycled )
	{
		rc = CalcRect( m_rectLast, rectLine );
		rc.InflateRect( 2, 3 );
		InvalidateRect( rc );
	}
}

long CVTSlider::GetPosLo() 
{
	return m_nLo;
}

void CVTSlider::SetPosLo(long nNewValue) 
{
	
	
	long	nNewPos = nNewValue;

	if( m_bSingleSlide )
		m_nHi = m_max;

	nNewPos = min( max( m_min, nNewValue ), m_max );

	if( !m_bCycled )
		nNewPos = min( m_nHi, nNewPos );

	//if( m_bSingleSlide )
	//	m_nHi = m_max;

	if( m_nLo == nNewPos )return;


	InvalidateSliderRect( 0 );
	m_nLo = nNewPos;
	InvalidateSliderRect( 0 );

	SetModifiedFlag();

	FirePosChanged();
	//if( m_bAutoUpdate )
	//	WriteToShellData();	

}

long CVTSlider::GetPosHi() 
{
	return m_nHi;
}

void CVTSlider::SetPosHi(long nNewValue) 
{
	long	nNewPos = nNewValue;
	
	if( !m_bSingleSlide )
		nNewPos = min( max( m_min, nNewValue ), m_max );
	else
		nNewPos = m_max;

	if( !m_bCycled )
		nNewPos = max( m_nLo, nNewPos );

	if( m_nHi == nNewPos )return;

	if( m_bSingleSlide )
		m_nHi = nNewPos;

	if( !m_bSingleSlide )
	{
		InvalidateSliderRect( 1 );		
		m_nHi = nNewPos;
		InvalidateSliderRect( 1 );

		SetModifiedFlag();
		
		FirePosChanged();
		//if( m_bAutoUpdate )
		//	WriteToShellData();	
	}
		

}

int	CVTSlider::CalcValueFromPos( int iposx )
{
	CRect	rc = CalcRect( m_rectLast, rectLine );

	return int( double(iposx-rc.left)/(rc.Width())*(m_max-m_min) +.5 )+m_min;
}

void CVTSlider::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	
	m_bActive = TRUE;
	Refresh();
	
}

void CVTSlider::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);
	
	m_bActive = FALSE;
	Refresh();
	
}

BOOL CVTSlider::OnGetViewRect(DWORD dwAspect, LPRECTL pRect) 
{

	return COleControl::OnGetViewRect(dwAspect, pRect);
}

BOOL CVTSlider::GetShowNums() 
{
	return m_bShowNums;
}

void CVTSlider::SetShowNums(BOOL bNewValue) 
{
	m_bShowNums = bNewValue;

	SetModifiedFlag();
	Refresh();
}


static LONG nVersion = 5;	

void CVTSlider::Serialize( CArchive &ar )
{
	COleControl::Serialize( ar );

	SetPosHi( m_nHi );

	SerializeStockProps(ar);

	if( ar.IsLoading() )
	{
		ar>>nVersion;
		ar>>m_bShowNums>>m_bShowTick>>m_nTickFreq>>m_nNumsCount>>(long&)m_track>>m_nLo>>m_nHi>>m_min>>m_max>>m_bCycled;
		if( nVersion >= 2 )
		{
			ar >> m_bAutoUpdate;
			ar >> m_strName;
			ar >> m_strKeyValueLo;
			ar >> m_strKeyValueHi;
			ar >> m_bInnerRaised;
			ar >> m_bInnerSunken;
			ar >> m_bOuterRaised;
			ar >> m_bOuterSunken;

		}
		if( nVersion >= 3 )
			ar >> m_bSingleSlide;

		if( nVersion >= 5 )
			ar >> m_bUseSystemFont;

	}
	else
	{
		ar << nVersion;
		ar<<m_bShowNums<<m_bShowTick<<m_nTickFreq<<m_nNumsCount<<(long)m_track<<m_nLo<<m_nHi<<m_min<<m_max<<m_bCycled;
		ar << m_bAutoUpdate;
		ar << m_strName;
		ar << m_strKeyValueLo;
		ar << m_strKeyValueHi;
		ar << m_bInnerRaised;
		ar << m_bInnerSunken;
		ar << m_bOuterRaised;
		ar << m_bOuterSunken;
		ar << m_bSingleSlide;

		ar << m_bUseSystemFont;
	}

	if( nVersion >= 4 )
		SerializeStockProps(ar);

	SetPosHi( m_nHi );

	InvalidateControl();

	

}

void CVTSlider::SetBothLimits(long nLow, long nHigh) 
{
	nLow = max( min( nLow, m_max ), m_min );
	nHigh = max( min( nHigh, m_max ), m_min );

	if( !m_bCycled )
		nHigh = max( nLow, nHigh );

	if( nLow != m_nLo )
	{
		InvalidateSliderRect( 0 );
		m_nLo = nLow;
		InvalidateSliderRect( 0 );
	}

	if( nHigh != m_nHi )
	{
		InvalidateSliderRect( 1 );
		m_nHi = nHigh;
		InvalidateSliderRect( 1 );
	}

	SetModifiedFlag();

	FirePosChanged();

	//if( m_bAutoUpdate )
	//	WriteToShellData();	


}


void CVTSlider::OnCycledChanged() 
{
	CRect	rc = CalcRect( m_rectLast, rectLine );
	rc.InflateRect( 2, 3 );
	InvalidateRect( rc );

	SetModifiedFlag();
}

void CVTSlider::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTSLIDER);
	dlgAbout.DoModal();
}



BOOL CVTSlider::GetAutoUpdate() 
{
	return m_bAutoUpdate;
}

void CVTSlider::SetAutoUpdate(BOOL bNewValue) 
{
	m_bAutoUpdate = bNewValue;
	SetModifiedFlag();
}


BSTR CVTSlider::GetLoKeyValue() 
{
	CString strResult = m_strKeyValueLo;
	return strResult.AllocSysString();
}

void CVTSlider::SetLoKeyValue(LPCTSTR lpszNewValue) 
{
	m_strKeyValueLo = lpszNewValue;

	SetModifiedFlag();
}

BSTR CVTSlider::GetHiKeyValue() 
{
	CString strResult = m_strKeyValueHi;
	return strResult.AllocSysString();
}

void CVTSlider::SetHiKeyValue(LPCTSTR lpszNewValue) 
{	
	m_strKeyValueHi = lpszNewValue;
	SetModifiedFlag();
}

BSTR CVTSlider::GetName() 
{
	CString strResult = m_strName;
	return strResult.AllocSysString();
}

void CVTSlider::SetName(LPCTSTR lpszNewValue) 
{
	m_strName = lpszNewValue;
	SetModifiedFlag();
}



BOOL CVTSlider::GetInnerRaised() 
{
	return m_bInnerRaised;
}

void CVTSlider::SetInnerRaised(BOOL bNewValue) 
{
	m_bInnerRaised = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTSlider::GetInnerSunken() 
{
	return m_bInnerSunken;
}

void CVTSlider::SetInnerSunken(BOOL bNewValue) 
{
	m_bInnerSunken = bNewValue;

	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTSlider::GetOuterRaised() 
{
	return m_bOuterRaised;
}

void CVTSlider::SetOuterRaised(BOOL bNewValue) 
{
	m_bOuterRaised = bNewValue;

	SetModifiedFlag();
	InvalidateControl();
}


BOOL CVTSlider::GetSingleSlide() 
{
	return m_bSingleSlide;
}

void CVTSlider::SetSingleSlide(BOOL bNewValue) 
{
	m_bSingleSlide = bNewValue;
	
	SetModifiedFlag();
	InvalidateControl( );
}


BOOL CVTSlider::GetOuterSunken() 
{
	return m_bOuterSunken;
}

void CVTSlider::SetOuterSunken(BOOL bNewValue) 
{	
	m_bOuterSunken = bNewValue;

	SetModifiedFlag();
	InvalidateControl();
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
//	
//		
//			
//		Shell support
//
//		For: Shell support
//		
//			
//
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#ifndef FOR_HOME_WORK
HRESULT CVTSlider::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)

	pThis->m_ptrSite = punkSite;
	pThis->m_ptrApp = punkVtApp;

	pThis->RestoreFromShellData();

	return S_OK;
}
  

HRESULT CVTSlider::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)	
	*pbstrName = pThis->m_strName.AllocSysString();
	return S_OK;
}

HRESULT CVTSlider::XActiveXCtrl::SetDoc( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)	
	return S_OK;
}

HRESULT CVTSlider::XActiveXCtrl::SetApp( IUnknown *punkVtApp )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)	
	return S_OK;
}


HRESULT CVTSlider::XActiveXCtrl::OnOK( )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)

	if( pThis->m_bAutoUpdate )
		pThis->StoreToData();

	return S_OK;
}

HRESULT CVTSlider::XActiveXCtrl::OnApply( )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)

	OnOK();

	return S_OK;
}

HRESULT CVTSlider::XActiveXCtrl::OnCancel( )
{
	METHOD_PROLOGUE_EX(CVTSlider, ActiveXCtrl)

	return S_OK;
}

#endif

BOOL CVTSlider::WriteToShellData()
{	
#ifndef FOR_HOME_WORK

	if( m_ptrApp == NULL ) return FALSE;
		

	CString strSection;
	CString strEntry;

	if( ::GetSectionEntry( m_strKeyValueLo, strSection, strEntry ) )
	{
		_SetValueInt( m_ptrApp, strSection, strEntry,  m_nLo );
	}
	

	strSection = strEntry = "";

	if( ::GetSectionEntry( m_strKeyValueHi, strSection, strEntry ) )
	{
		_SetValueInt( m_ptrApp, strSection, strEntry,  m_nHi );
	}


#endif
	return TRUE;
}

BOOL CVTSlider::RestoreFromShellData()
{
#ifndef FOR_HOME_WORK
	if( m_ptrApp == NULL ) return FALSE;


	CString strSection;
	CString strEntry;

	if( ::GetSectionEntry( m_strKeyValueLo, strSection, strEntry ) )
	{
		m_nLo = _GetValueInt( m_ptrApp, strSection, strEntry );

	}
		
	strSection = strEntry = "";

	if( ::GetSectionEntry( m_strKeyValueHi, strSection, strEntry ) )
	{
		m_nHi = _GetValueInt( m_ptrApp, strSection, strEntry );
	}

	InvalidateControl( );
#endif
	return TRUE;
}



void CVTSlider::StoreToData() 
{
	WriteToShellData();	
}


/////////////////////////////////////////////////////////////////////////////////////
// function for visible & enable control status & SetFocus
/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTSlider::GetDisabled()
{
	return !::_IsWindowEnable( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTSlider::SetDisabled(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( !bNewValue );
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTSlider::GetVisible()
{
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTSlider::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );



}

/////////////////////////////////////////////////////////////////////////////////////
void CVTSlider::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}


/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTSlider::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTSlider::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();	

	InvalidateControl( );
}
