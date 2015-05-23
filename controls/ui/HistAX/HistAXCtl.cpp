// HistAXCtl.cpp : Implementation of the CHistAXCtrl ActiveX Control class.

#include "stdafx.h"
#include "HistAX.h"
#include "HistAXCtl.h"
#include "HistAXPpg.h"
#include "FramesPropPage.h"
#include "misc_utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CHistAXCtrl, COleControl)


BEGIN_INTERFACE_MAP(CHistAXCtrl, COleControl)
	INTERFACE_PART(CHistAXCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CHistAXCtrl, IID_IEventListener, EvList)
END_INTERFACE_MAP()


bool g_bAlwaysCalcHist = false;
/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CHistAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CHistAXCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CHistAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CHistAXCtrl)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "InnerRaised", m_innerRaised, OnInnerRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "OuterRaised", m_outerRaised, OnOuterRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "InnerSunken", m_innerSunken, OnInnerSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "OuterSunken", m_outerSunken, OnOuterSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "PaintPanes", m_paintPanes, OnPaintPanesChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "LowThreshold", m_lowThreshold, OnLowThresholdChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "HighThreshold", m_highThreshold, OnHighThresholdChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CHistAXCtrl, "YZoom", m_yZoom, OnYZoomChanged, VT_R8)
	DISP_FUNCTION(CHistAXCtrl, "GetCnvName", GetCnvName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHistAXCtrl, "GetPanesCount", GetPanesCount, VT_I2, VTS_NONE)
	DISP_FUNCTION(CHistAXCtrl, "SetPane", SetPane, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CHistAXCtrl, "GetPaneName", GetPaneName, VT_BSTR, VTS_I2)
	DISP_FUNCTION(CHistAXCtrl, "GetPaneColor", GetPaneColor, VT_COLOR, VTS_I2)
	DISP_FUNCTION(CHistAXCtrl, "AddPane", AddPane, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CHistAXCtrl, "RemovePane", RemovePane, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CHistAXCtrl, "ZoomIn", ZoomIn, VT_EMPTY, VTS_R8)
	DISP_FUNCTION(CHistAXCtrl, "ZoomOut", ZoomOut, VT_EMPTY, VTS_R8)
	DISP_FUNCTION(CHistAXCtrl, "Lock", Lock, VT_BOOL, VTS_BOOL)
	DISP_FUNCTION(CHistAXCtrl, "SetTarget", SetTarget, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CHistAXCtrl, "SetNumPhases", SetNumPhases, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CHistAXCtrl, "SetPhaseLowThreshold", SetPhaseLowThreshold, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CHistAXCtrl, "SetPhaseHighThreshold", SetPhaseHighThreshold, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CHistAXCtrl, "SetPhaseColor", SetPhaseColor, VT_EMPTY, VTS_I4 VTS_COLOR)
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FORECOLOR()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CHistAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CHistAXCtrl)
	EVENT_CUSTOM("OnNewCalc", FireOnNewCalc, VTS_NONE)
	EVENT_CUSTOM("OnPhaseClick", FireOnPhaseClick, VTS_I4)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CHistAXCtrl, 3)
	PROPPAGEID(CHistAXPropPage::guid)
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_CColorPropPage)
END_PROPPAGEIDS(CHistAXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
// {65BD787C-FDC7-4f83-A788-13E6266835E9}
GUARD_IMPLEMENT_OLECREATE_CTRL(CHistAXCtrl, "HISTAX.HistAXCtrl.1",
	0x65bd787c, 0xfdc7, 0x4f83, 0xa7, 0x88, 0x13, 0xe6, 0x26, 0x68, 0x35, 0xe9);


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CHistAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DHistAX =
		{ 0x6fdde56, 0xf981, 0x11d3, { 0xa0, 0xc1, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };
const IID BASED_CODE IID_DHistAXEvents =
		{ 0x6fdde57, 0xf981, 0x11d3, { 0xa0, 0xc1, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwHistAXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

	IMPLEMENT_OLECTLTYPE(CHistAXCtrl, IDS_HISTAX, _dwHistAXOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl IVtActiveXCtrl implement
IMPLEMENT_UNKNOWN(CHistAXCtrl, ActiveXCtrl);

HRESULT CHistAXCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CHistAXCtrl, ActiveXCtrl)

	pThis->m_sptrSite = punkSite;
	pThis->m_sptrApp = punkVtApp;	

	g_bAlwaysCalcHist = true;
	pThis->_ManageHistState();
	g_bAlwaysCalcHist = false;

	return S_OK;
}
  

HRESULT CHistAXCtrl::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CHistAXCtrl, ActiveXCtrl)	
	*pbstrName = NULL;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::CHistAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CHistAXCtrl

BOOL CHistAXCtrl::CHistAXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.
	return UpdateRegistryCtrl(bRegister, AfxGetInstanceHandle(), IDS_HISTAX, IDB_HISTAX,
							  afxRegApartmentThreading, _dwHistAXOleMisc,
							  _tlid, _wVerMajor, _wVerMinor);
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::CHistAXCtrl - Constructor

CHistAXCtrl::CHistAXCtrl()
{
	InitializeIIDs(&IID_DHistAX, &IID_DHistAXEvents);

	// TODO: Initialize your control's instance data here.
	m_sptrSite = 0;
	m_sptrApp = 0;

	m_innerRaised = TRUE;
	m_innerSunken = FALSE;
	m_outerRaised = FALSE;
	m_outerSunken = TRUE;

	m_bDefaultFill = true;

	m_strCnvName = "";
	m_pbstrPanesNames = 0;
	m_pdwPanesColors = 0;
	m_pdwGradientColors = 0;
	m_nPanes = 0;
	m_ppnHistData = 0;
	m_pnMaxHistData = 0;
	m_rcLastDraw = NORECT;

	m_ppnBrightData = 0;
	m_pnMaxBrightData = 0;
	ppnHistBrightR =0;
	ppnHistBrightG =0;
	ppnHistBrightB =0;

	m_paintPanes = 7;
	m_nPanes	 = 0;

	m_lowThreshold = 1;
	m_highThreshold = 2;

	m_lCachedLowTh = 0;
	m_lCachedHighTh = 0;

	m_yZoom = 1;
	m_bLock = FALSE;

	m_ptrTargetImage = 0;
	m_ptrProvideHistColors = 0;

	m_nPhases=0;
	m_PhaseLow.alloc(m_nPhases);
	m_PhaseHigh.alloc(m_nPhases);
	m_PhaseColor.alloc(m_nPhases);
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::~CHistAXCtrl - Destructor

CHistAXCtrl::~CHistAXCtrl()
{
	// TODO: Cleanup your control's instance data here.
	_KillCurCnvInfo();
	_KillHistogramm();
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::OnDraw - Drawing function

void CHistAXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, &CBrush(::GetSysColor(COLOR_BTNFACE)));

	CRect	rc = rcBounds;
	rc.InflateRect( -1, -1 );
	//draw edge
	UINT	edgeType = 0;
	if(m_innerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_innerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_outerRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_outerSunken)
		edgeType |= BDR_SUNKENOUTER;
	if(edgeType != 0)
	{
		pdc->DrawEdge(rc, edgeType, BF_RECT);
	}

	rc.InflateRect( -1, -1 );
	
	//if( GetEnabled() )
	//if(this->m_hWnd && IsWindowVisible())
	{
		DrawNormalHist( pdc, rc );
	}


}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::DoPropExchange - Persistence support

void CHistAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	//ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl::OnResetState - Reset control to default state

void CHistAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	m_innerRaised = TRUE;
	m_innerSunken = FALSE;
	m_outerRaised = FALSE;
	m_outerSunken = TRUE;

	SetForeColor(DWORD(RGB(255, 255, 0)));
	SetBackColor(GetSysColor(COLOR_BACKGROUND));
	
	//m_paintPanes = 0;

	m_rcLastDraw = NORECT;

	m_lowThreshold = 1;
	m_highThreshold = 2;

	m_lCachedLowTh = 0;
	m_lCachedHighTh = 0;

	m_yZoom = 1;
	
	//DefaultFill();
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl message handlers

void CHistAXCtrl::OnInnerRaisedChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CHistAXCtrl::OnOuterRaisedChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CHistAXCtrl::OnInnerSunkenChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CHistAXCtrl::OnOuterSunkenChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CHistAXCtrl::Serialize(CArchive& ar) 
{
	
	if (ar.IsStoring())
	{	// storing code
		ar<<(BYTE)m_innerRaised;
		ar<<(BYTE)m_innerSunken;
		ar<<(BYTE)m_outerRaised;
		ar<<(BYTE)m_outerSunken;
		//ar<<(BYTE)m_paintPanes;
		//ar<<m_lowThreshold;
		//ar<<m_highThreshold;
		//ar<<(double)m_yZoom;

	}
	else
	{	// loading code
		BYTE byte;
		ar>>byte;
		m_innerRaised = (BOOL)byte;
		ar>>byte;
		m_innerSunken = (BOOL)byte;
		ar>>byte;
		m_outerRaised = (BOOL)byte;
		ar>>byte;
		m_outerSunken = (BOOL)byte;
		//ar>>byte;
		//m_paintPanes = (BOOL)byte;
		//ar>>m_lowThreshold;
		//ar>>m_highThreshold;
		//ar>>(double)m_yZoom;
	}

	SerializeStockProps(ar);
	InvalidateHist();
}

void CHistAXCtrl::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( (!strcmp( pszEvent, szAppActivateView )) || 
		(!strcmp( pszEvent, szEventActivateObject )) ||
		(!strcmp( pszEvent, szEventChangePreview))		
		)
	{
		//if(m_ptrTargetImage == 0)
		m_ptrTargetImage = 0;
		m_ptrProvideHistColors = 0;
//		if (!strcmp( pszEvent, szEventChangePreview))
//		g_bAlwaysCalcHist = true;
		_ManageHistState(punkFrom);
//		if (!strcmp( pszEvent, szEventChangePreview))
//		g_bAlwaysCalcHist = false;
	}


	if( !strcmp(pszEvent, szAppActivateView) )
	{	
		IUnknown* punkDoc = NULL;
		punkDoc = GetActiveDocument();
		if( punkDoc )
		{
			RegisterDoc( punkDoc );
			punkDoc->Release();
		}
	}
	else if( !strcmp(pszEvent, "AfterClose"/*szEventBeforeClose*/) )
	{
		if( ::GetObjectKey(m_ptrActiveDoc) == ::GetObjectKey(punkFrom) )
			UnRegisterDoc();
	}
	if(!strcmp(pszEvent, szEventChangeObject))	
	{		
		if( GetObjectKind( punkFrom ) == szTypeImage );
		{
			m_ptrTargetImage = 0;
			m_ptrProvideHistColors = 0;
			_ManageHistState(punkFrom);
		}
	}



}



void CHistAXCtrl::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	UnRegisterDoc( );
	UnRegister();
	COleControl::OnFinalRelease();
}

int CHistAXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetForeColor(DWORD(RGB(255, 255, 0)));
	SetBackColor(GetSysColor(COLOR_BACKGROUND));

	// TODO: Add your specialized creation code here
	Register();


	IUnknown* punkActiveDoc = NULL;
	punkActiveDoc = GetActiveDocument();
	if( punkActiveDoc  )
	{
		RegisterDoc( punkActiveDoc );
		punkActiveDoc->Release();
	}

	
	return 0;
}

void CHistAXCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	CRect rc;
	GetClientRect(&rc);
	int w = max(rc.Width(),1);

	int j = int ( double(point.x-rc.left)*HistMaxXBound/w + 0.5 );

	for(int nPhase=0; nPhase<m_nPhases; nPhase++)
	{
		if( j < m_PhaseHigh.ptr()[nPhase] && j >= m_PhaseLow.ptr()[nPhase] )
		{
			FireOnPhaseClick(nPhase);
			break;
		}
	}
	
	return;
}

void CHistAXCtrl::DrawNormalHist(CDC* pdc, CRect rcDraw)
{

	//m_bDefaultFill = m_sptrApp == 0;

	pdc->FillRect(rcDraw, &CBrush(TranslateColor(GetBackColor())));

	
	if(m_paintPanes==8)
	{
		if(m_ppnBrightData == 0 || m_pnMaxBrightData == 0) return;
		m_bDefaultFill=false;
	}
	else
	if(m_ppnHistData == 0 || m_pnMaxHistData == 0) return;

	pdc->SetBkMode(TRANSPARENT);

	bool bUseGradient = false;

	//rcDraw.DeflateRect(1,1,1,1);
	pdc->IntersectClipRect(rcDraw);

	if(m_nPanes <= 0) return;

	
	if(m_bDefaultFill)
	{
		SetTextColor(pdc->GetSafeHdc(), _GetDifferenceColor((DWORD)TranslateColor(GetBackColor())));
		CString strDefault;
		strDefault.LoadString(IDS_DEFAULT_HIST_CAPTION);
		pdc->TextOut(3, 3, strDefault);
		rcDraw.DeflateRect(0, 20, 0, 0);
	}
	
	short nMask = 1;
	int nCountDrawPanes = 0;
	int i = 0;
	for(i = 0; i < m_nPanes; i++)
	{
		if((m_paintPanes & nMask) > 0)
			nCountDrawPanes++;
		nMask = nMask << 1;
	}

	if(m_paintPanes==8)
		nCountDrawPanes=1;

	if((nCountDrawPanes == 0) && (!m_bDefaultFill)) return;

	if(nCountDrawPanes == 1 || m_bDefaultFill)
		bUseGradient = true;
	
	nMask = 1;

	COLORREF	colors[HistMaxXBound], color;
	memset( colors, sizeof( colors ), 255 );
	color = 0;



	IProvideHistColorsPtr	ptrH;
	
	if( m_ptrTargetImage != 0 )
	{

		IUnknown	*punkCC = 0;
		m_ptrTargetImage->GetColorConvertor( &punkCC );
		ptrH = punkCC;
		if( punkCC )punkCC->Release();
	}


	for(i = 0; i < m_nPanes; i++)
	{
		if(((m_paintPanes & nMask) > 0) || m_bDefaultFill || m_paintPanes==8)
		{
			if( m_ptrProvideHistColors != 0 )
			{
				if(m_paintPanes==8)
					//CColorConvertorGRAY::XHist::GetHistColors;
					//m_ptrProvideHistColors->GetHistColors( 0, colors, &color );
				//m_ptrProvideHistColors->
				{
					color = RGB( 0, 0, 0 );
	
					for( int i = 0; i < 256; i++ )
						colors[i] = RGB( i, i, i );
				}
				else
				m_ptrProvideHistColors->GetHistColors( i, colors, &color );
			}
			//CPen	pen( PS_SOLID, 1, m_pdwPanesColors[i]);
			CPen	pen( PS_SOLID, 1, color );
			CPen* penOld = pdc->SelectObject( &pen );

			DWORD dwGradient;
			if(m_paintPanes==8)
				dwGradient =  m_pdwGradientColors[0];
			else
				dwGradient =  m_pdwGradientColors[i];
			CPoint	ptold, pt;
			bool	bOldInit = false;

			for(int j = 0; j < HistMaxXBound; j++)
			{
				int	iColor = int( j*256./HistMaxXBound+.5 );
				COLORREF cr = colors[iColor];


				/*COLORREF cr;
				if (m_nPanes == 1)
				{
					cr = RGB(iColor, iColor, iColor);
				}
				else
				{
					switch(i)
					{
					case 0:
						cr = RGB(iColor, (BYTE)(dwGradient >> 8), (BYTE)(dwGradient));
						break;
					case 1:
						cr = RGB((BYTE)(dwGradient >> 16), iColor, (BYTE)(dwGradient));
						break;
					case 2:
						cr = RGB((BYTE)(dwGradient >> 16), (BYTE)(dwGradient >> 8), iColor);
						break;
					default:
						cr = RGB(iColor, iColor, iColor);
					}
				}*/

				CRect	rc;
				rc.left = int( rcDraw.left+(double)rcDraw.Width()*j/(HistMaxXBound)+.5);
				rc.right = int( rcDraw.left+(double)rcDraw.Width()*(j+1)/(HistMaxXBound)+.5);
				rc.bottom = rcDraw.bottom;
				if(m_paintPanes==8)
					rc.top = int( rcDraw.bottom-1 - double(m_ppnBrightData[0][j])/m_pnMaxBrightData[0]*(rcDraw.Height()-1)*m_yZoom+.5);
				else
					rc.top = int( rcDraw.bottom-1 - double(m_ppnHistData[i][j])/m_pnMaxHistData[i]*(rcDraw.Height()-1)*m_yZoom+.5);
				

				for(int nPhase=0; nPhase<m_nPhases; nPhase++)
				{
					if( j < m_PhaseHigh.ptr()[nPhase] && j >= m_PhaseLow.ptr()[nPhase] )
						cr = TranslateColor( m_PhaseColor.ptr()[nPhase] );
				}

				if( m_highThreshold > m_lowThreshold )
					if( j < m_highThreshold && j >= m_lowThreshold )
						cr = TranslateColor( GetForeColor() );
				if( m_highThreshold < m_lowThreshold )
					if( j <= m_highThreshold || j > m_lowThreshold )
						cr = TranslateColor( GetForeColor() );
									
				if(bUseGradient) pdc->FillRect(rc, &CBrush(cr));
				
				pt = CPoint( rc.CenterPoint().x, rc.top );

				if( bOldInit )
				{
					pdc->MoveTo( ptold );
					pdc->LineTo( pt );
				}
				ptold = pt;
				bOldInit = true;

			}
			pdc->SelectObject(penOld);
		}
		nMask = nMask << 1;
	}

	
	

	m_rcLastDraw = rcDraw;
}

int CHistAXCtrl::_GetActivePane()
{
	int nActivePane = -1;

	short nMask = 1;
	int nActive = -1;
	int nCountDrawPanes = 0;
	for(int i = 0; i < m_nPanes; i++)
	{
		if((m_paintPanes & nMask) > 0)
		{
			nCountDrawPanes++;
			nActive = i;
		}
		nMask = nMask << 1;
	}

	if(nCountDrawPanes == 1)
		return nActive;

	return nActivePane;
}


void CHistAXCtrl::InvalidateHist(long nLow, long nHigh)
{
	int nActivePane = _GetActivePane();
	if((nLow == 0 && nHigh == 0) || (nActivePane < 0))
	{
		if(GetSafeHwnd())
			InvalidateControl();
		return;
	}

	if (m_ppnHistData == 0) return;

	bool	bInit = false;

	long	nMin, nMax;

	nLow = max( 0, min( nLow, HistMaxXBound-1 ) );
	nHigh = max( 0, min( nHigh, HistMaxXBound-1 ) );

	if( nLow > nHigh )
	{
		InvalidateHist( 0, nHigh );
		InvalidateHist( nLow, HistMaxXBound );
		return;
	}

	for( int i = nLow; i < nHigh; i++ )
	{
		long	lVal;
		if(m_paintPanes==8)
			lVal = m_ppnBrightData[0][i];
		else
			lVal = m_ppnHistData[nActivePane][i];

		if( lVal )
		{
			if( !bInit )
				nMax = nMin = lVal;
			else
			{
				nMin = min( lVal, nMin );
				nMax = max( lVal, nMax );
			}
			bInit = true;
		}
	}

	if( !bInit )
		return;

	CRect	rc = NORECT;
	
	rc.left = int( m_rcLastDraw.left+(double)m_rcLastDraw.Width()*nLow/(HistMaxXBound)+.5);
	rc.right = int( m_rcLastDraw.left+(double)m_rcLastDraw.Width()*(nHigh+1)/(HistMaxXBound)+.5);
	rc.bottom = m_rcLastDraw.bottom;
	if(m_paintPanes==8)
		rc.top = int( m_rcLastDraw.bottom-(double)((m_rcLastDraw.Height())*nMax/m_pnMaxBrightData[0])*m_yZoom+.5);
	else
		rc.top = int( m_rcLastDraw.bottom-(double)((m_rcLastDraw.Height())*nMax/m_pnMaxHistData[nActivePane])*m_yZoom+.5);

	if( GetSafeHwnd() )
		InvalidateRect( rc );
}

void CHistAXCtrl::DefaultFill()
{
	m_bLock = FALSE;

	_KillCurCnvInfo();
	_KillHistogramm();

	m_ppnHistData = new long[1][HistMaxXBound];
	m_pnMaxHistData = new long[1];
	
	m_ppnBrightData = new long[1][HistMaxXBound];
	m_pnMaxBrightData = new long[1];
	ppnHistBrightR = new float[1];
	ppnHistBrightG = new float[1];
	ppnHistBrightB = new float[1];

	ppnHistBrightR[0] =1;
	ppnHistBrightG[0] =1;
	ppnHistBrightB[0] =1;
	m_pnMaxBrightData[0] = 1;

	m_pnMaxHistData[0] = 1;

	m_pdwPanesColors = new DWORD[1];
	m_pdwGradientColors = new DWORD[1];

	m_pbstrPanesNames = new BSTR[1];

	m_pdwPanesColors[0] = RGB(255, 255, 255);
	m_pdwGradientColors[0] = RGB(0, 0, 0);

	CString strDefPane;
	strDefPane.LoadString(IDS_DEFAULT_HIST_CAPTION);
	m_pbstrPanesNames[0] = strDefPane.AllocSysString();

	long nValue = 0;
	for(int i = 0; i < HistMaxXBound; i++)
	{
		//if((i % 10) == 0)
		//	nValue = (3000*rand())/RAND_MAX;
		if(m_paintPanes==8)
		{
			m_ppnBrightData[0][i] = nValue;
			m_pnMaxBrightData[0] = max(m_pnMaxBrightData[0], nValue);
		}
		else
		{
			m_ppnHistData[0][i] = nValue;
			m_pnMaxHistData[0] = max(m_pnMaxHistData[0], nValue);
		}
	}

	m_nPanes = 1;

	m_yZoom = 1;

	InvalidateControl();//Refresh();
}

DWORD CHistAXCtrl::_GetDifferenceColor(DWORD dwColor)
{
	DWORD dwNewColor = dwColor;
	DWORD dwOrigColor = dwColor;

	dwNewColor = ~dwNewColor;
	dwNewColor &= 0x00ffffff;
	if(abs(GetRValue(dwNewColor) - GetRValue(dwOrigColor)) < 10 && 
	   abs(GetGValue(dwNewColor) - GetGValue(dwOrigColor)) < 10 && 
	   abs(GetBValue(dwNewColor) - GetBValue(dwOrigColor)) < 10)
			dwNewColor = RGB(0, 0, 0);
	return dwNewColor;
}

void CHistAXCtrl::_ManageHistState(IUnknown* punkActiveView)
{
	m_bDefaultFill = true;
	if(m_sptrApp != 0)
	{

		IImagePtr sptrImage;
		
		if(m_ptrTargetImage == 0)
		{
			IUnknown* punkDoc = 0;		
			m_sptrApp->GetActiveDocument(&punkDoc);
	
			IDocumentSitePtr sptrDoc(punkDoc);
			if(punkDoc)
				punkDoc->Release();
			if(sptrDoc != 0)
			{				
				
				IUnknown* punkView = 0;
				sptrDoc->GetActiveView(&punkView);
				IImageView2Ptr sptrView(punkView);

				IDataContext2Ptr ptrDC(punkView);

				if(punkView)
					punkView->Release();

				if(sptrView == 0)
						sptrView = punkActiveView;
				if(sptrView != 0)
				{
					IUnknown* punkImage = 0;
					IUnknown* punkImageSel = 0;
					sptrView->GetActiveImage2(&punkImage, &punkImageSel, TRUE);
					if(punkImageSel)
					{
						sptrImage = punkImageSel;
						punkImageSel->Release();
					}
					else
						sptrImage = punkImage;
					if(punkImage)
						punkImage->Release();
				}				

				if( sptrImage == 0 && ptrDC != 0 )
				{
					IUnknown* punk = ::GetActiveObjectFromContext( ptrDC, szTypeImage );
					sptrImage = punk;
					if( punk )
						punk->Release();
				}
			}
		}
		else
			sptrImage = m_ptrTargetImage;

		if(sptrImage != 0)
		{
			IUnknown* punkCC = 0;
			sptrImage->GetColorConvertor(&punkCC);
			IColorConvertor4Ptr sptrCC(punkCC);
			m_ptrProvideHistColors = punkCC;
			if(punkCC)
				punkCC->Release();
			if(sptrCC != 0)
			{
				_KillCurCnvInfo();
	
				BSTR bstrName = 0;
				sptrCC->GetCnvName(&bstrName);
				m_strCnvName = bstrName;
				::SysFreeString(bstrName);
			
				m_nPanes = ::GetImagePaneCount( sptrImage );
				
				if(m_nPanes > 0)
				{
					m_pbstrPanesNames = new BSTR[m_nPanes];
					m_pdwPanesColors = new DWORD[m_nPanes];
					m_pdwGradientColors = new DWORD[m_nPanes];
					for(int i = 0; i < m_nPanes; i++)
					{
						BSTR bstrName = 0;
						sptrCC->GetPaneName(i, &bstrName);
						m_pbstrPanesNames[i] = bstrName;
						sptrCC->GetPaneColor(i, &m_pdwPanesColors[i]); 
						sptrCC->GetCorrespondPanesColorValues(i, &m_pdwGradientColors[i]); 
					}

					_CalcHistogramm(sptrImage);
				}
			}
			m_bDefaultFill = false;
		}
		else
		{
			//it's not image view
			_KillHistogramm();
		}


	}
	if(m_bDefaultFill)
		DefaultFill();
	else
		InvalidateControl();//Refresh();
}

BSTR CHistAXCtrl::GetCnvName() 
{
	CString strResult;
	// TODO: Add your dispatch handler code here
	strResult = m_strCnvName;
	return strResult.AllocSysString();
}

void CHistAXCtrl::_KillCurCnvInfo()
{
	if(m_nPanes)
	{
		for(int i = 0; i < m_nPanes; i++)
		{
			::SysFreeString(m_pbstrPanesNames[i]);
		}
	}
	if(m_pbstrPanesNames)
		delete m_pbstrPanesNames;

	if(m_pdwPanesColors)
		delete m_pdwPanesColors;

	if(m_pdwGradientColors)
		delete m_pdwGradientColors;
	
	m_strCnvName = "";
	m_pbstrPanesNames = 0;
	m_pdwPanesColors = 0;
	m_pdwGradientColors = 0;
	m_nPanes = 0;
	//m_paintPanes = 7;
}

void CHistAXCtrl::_CalcHistogramm(IUnknown* punkImage)
{
	if(m_bLock == TRUE) return;

	if(!g_bAlwaysCalcHist)
		if(!(this->m_hWnd && IsWindowVisible())) return;

	_KillHistogramm();
	CImageWrp image(punkImage);
	
	int nRows = image.GetHeight();
	int nCols = image.GetWidth();

	//if(m_paintPanes==8)
	{
		// variables for calc €ркости Y = 0.257R + 0.504G + 0.098B
		m_ppnBrightData = new long[1][HistMaxXBound];
		m_pnMaxBrightData = new long[1];
		ppnHistBrightR = new float[nCols];
		ppnHistBrightG = new float[nCols];
		ppnHistBrightB = new float[nCols];
		for(int i = 0; i < nCols; i++)
		{
			ppnHistBrightR[i] = 1;
			ppnHistBrightG[i] = 1;
			ppnHistBrightB[i] = 1;
		}
		ZeroMemory(m_ppnBrightData[0], sizeof(long)*HistMaxXBound);
		m_pnMaxBrightData[0] = 1;
	}
	m_ppnHistData = new long[m_nPanes][HistMaxXBound];
	m_pnMaxHistData = new long[m_nPanes];
	
	for(int i = 0; i < m_nPanes; i++)
	{
		ZeroMemory(m_ppnHistData[i], sizeof(long)*HistMaxXBound);
		m_pnMaxHistData[i] = 1;
	}

	/*if(m_nPanes==4)
	{
		new_nPanes=3;
	}
	else
		new_nPanes=m_nPanes;*/
	for(int nRow = 0; nRow < nRows; nRow++)
	{
		for(int nPane = 0; nPane < m_nPanes; nPane++)
		{
			color* pRow = image.GetRow( nRows-nRow-1, nPane);
			byte * pRowMask = image.GetRowMask( nRows-nRow-1);
			for( int nCol = 0; nCol < nCols; nCol++ )
			{
				if (pRowMask[nCol])
				{
					BYTE Color = ColorAsByte(pRow[nCol]);
					//if(m_paintPanes==8)
					{
						if(nPane==0)
							ppnHistBrightR[nCol]=(float)Color;
						else
							if(nPane==1)
								ppnHistBrightG[nCol]=(float)Color;
							else
								if(nPane==2)
									ppnHistBrightB[nCol]=(float)Color;
					}
					m_ppnHistData[nPane][Color]++;
					m_pnMaxHistData[nPane] = max(m_pnMaxHistData[nPane], m_ppnHistData[nPane][Color]);
				}
			}
		}
		//if(m_paintPanes==8)
		{
			for( int nCol = 0; nCol < nCols; nCol++ )
			{
				BYTE Color = (BYTE)(1.164144*(ppnHistBrightR[nCol]*0.257+
						ppnHistBrightG[nCol]*0.504+ppnHistBrightB[nCol]*0.098));
				m_ppnBrightData[0][Color]++;
				m_pnMaxBrightData[0] = max(m_pnMaxBrightData[0], m_ppnBrightData[0][Color]);
			}
		}
	}
	

	m_bDefaultFill = false;

	FireOnNewCalc();
}

void CHistAXCtrl::_KillHistogramm()
{
	m_bDefaultFill = true;
	if(m_ppnHistData)
	{
		delete []m_ppnHistData;
		m_ppnHistData = 0;
	}
	if(m_pnMaxHistData)
	{
		delete m_pnMaxHistData;
		m_pnMaxHistData = 0;
	}
	if(m_ppnBrightData)
	{
		delete []m_ppnBrightData;
		m_ppnBrightData = 0;
	}
	if(m_pnMaxBrightData)
	{
		delete m_pnMaxBrightData;
		m_pnMaxBrightData = 0;
	}
	if(ppnHistBrightR)
	{
		delete ppnHistBrightR;
		ppnHistBrightR=0;
	}
	if(ppnHistBrightG)
	{
		delete ppnHistBrightG;
		ppnHistBrightG=0;
	}
	if(ppnHistBrightB)
	{
		delete ppnHistBrightB;
		ppnHistBrightB=0;
	}
}

void CHistAXCtrl::OnPaintPanesChanged() 
{
	// TODO: Add notification handler code
	Refresh();
	SetModifiedFlag();
}

void CHistAXCtrl::OnLowThresholdChanged() 
{
	// TODO: Add notification handler code
	if(_GetActivePane() < 0) return;
	InvalidateHist( m_lCachedLowTh, m_lCachedHighTh );
	
 	m_lCachedLowTh = m_lowThreshold;
	m_lCachedHighTh = m_highThreshold;

	InvalidateHist( m_lCachedLowTh, m_lCachedHighTh );

	SetModifiedFlag();
}

void CHistAXCtrl::OnHighThresholdChanged() 
{
	// TODO: Add notification handler code
	if(_GetActivePane() < 0) return;
	InvalidateHist( m_lCachedLowTh, m_lCachedHighTh );
	
 	m_lCachedLowTh = m_lowThreshold;
	m_lCachedHighTh = m_highThreshold;

	InvalidateHist( m_lCachedLowTh, m_lCachedHighTh );

	SetModifiedFlag();
}

short CHistAXCtrl::GetPanesCount() 
{
	// TODO: Add your dispatch handler code here
	return m_nPanes;
}

void CHistAXCtrl::SetPane(short nPane) 
{
	// TODO: Add your dispatch handler code here
	//paul 18.04.2003
	//if(nPane < m_nPanes && nPane >= 0)
	{
		m_paintPanes = 1;
		while(nPane > 0)
		{
			m_paintPanes = m_paintPanes << 1;
			nPane--;
		}
	}
	Refresh();
}

BSTR CHistAXCtrl::GetPaneName(short nPane) 
{
	if(nPane < m_nPanes && nPane >= 0)
	{
		CString strResult;
		// TODO: Add your dispatch handler code here
		strResult = m_pbstrPanesNames[nPane];
		return strResult.AllocSysString();
	}
	return 0;
}

OLE_COLOR CHistAXCtrl::GetPaneColor(short nPane) 
{
	// TODO: Add your dispatch handler code here
	if(nPane < m_nPanes && nPane >= 0)
		return m_pdwPanesColors[nPane];
	return RGB(0,0,0);
}

void CHistAXCtrl::AddPane(short nPane) 
{
	// TODO: Add your dispatch handler code here
	//paul 18.04.2003
	//if(nPane < m_nPanes && nPane >= 0)
	{
		short nPanes = 1;
		while(nPane > 0)
		{
			nPanes = nPanes << 1;
			nPane--;
		}

		m_paintPanes |= nPanes;
	}
	Refresh();
}

void CHistAXCtrl::RemovePane(short nPane) 
{
	// TODO: Add your dispatch handler code here
	if(nPane < m_nPanes && nPane >= 0)
	{
		short nPanes = 1;
		while(nPane > 0)
		{
			nPanes = nPanes << 1;
			nPane--;
		}
		m_paintPanes |= nPanes;
		m_paintPanes ^= nPanes;
	}
	Refresh();
}

void CHistAXCtrl::OnYZoomChanged() 
{
	// TODO: Add notification handler code
	m_yZoom = max(1, m_yZoom);
	Refresh();
	SetModifiedFlag();
}

void CHistAXCtrl::ZoomIn(double nStep) 
{
	// TODO: Add your dispatch handler code here
	//m_yZoom += nStep;
	m_yZoom *= (1+nStep);
	Refresh();
}

void CHistAXCtrl::ZoomOut(double nStep) 
{
	// TODO: Add your dispatch handler code here
	if(m_yZoom == 1) return;

	//m_yZoom -= nStep;
	m_yZoom /= (1+max(0,nStep));
	m_yZoom = max(1, m_yZoom);
	Refresh();
}

BOOL CHistAXCtrl::Lock(BOOL bLock) 
{
	// TODO: Add your dispatch handler code here
	BOOL bPrevState = m_bLock;
	m_bLock = bLock;
	return bPrevState;
}

void CHistAXCtrl::SetTarget(LPDISPATCH pdispTarget) 
{
	// A.M. BT4516.
	// ѕохоже, m_ptrTargetImage больше не нужен. ќн все равно устанавливаетс€ в 0 при активизации
	// окна программы. ≈сли он равен 0, то гистограмма правильно берет изображение дл€ себ€:
	// из текущей вьюхи превью (если оно есть) или изображение из документа. ≈сли оставить присваивание,
	// то надо в Histogram.form в Form_OnShow разруливать есть ли превью или нет.
	// ѕересчет гистограммы здесь нужен дл€ того, чтобы пересчитывать ее при показе формы
	// (метод вызываетс€ из Form_OnShow). 
//	m_ptrTargetImage = pdispTarget;

	//if(pdispTarget)
	//	pdispTarget->Release();

	bool bPrev = g_bAlwaysCalcHist;
	g_bAlwaysCalcHist = true;
	_ManageHistState();
	g_bAlwaysCalcHist = bPrev;
}

BOOL CHistAXCtrl::OnEraseBkgnd(CDC* pDC) 
{
	//if( GetSafeHwnd() )return true;
	return COleControl::OnEraseBkgnd(pDC);
}



/////////////////////////////////////////////////////////////////////////////
void CHistAXCtrl::RegisterDoc( IUnknown* punkDoc )
{
	if( ::GetObjectKey(m_ptrActiveDoc) == ::GetObjectKey(punkDoc) )
		return;

	if( m_ptrActiveDoc )
	{
		UnRegisterDoc( );		
	}

	m_ptrActiveDoc = punkDoc;
	Register( m_ptrActiveDoc );
}

/////////////////////////////////////////////////////////////////////////////
void CHistAXCtrl::UnRegisterDoc( )
{
	if( m_ptrActiveDoc )
	{
		UnRegister( m_ptrActiveDoc );
		m_ptrActiveDoc = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CHistAXCtrl::GetActiveDocument()
{
	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView == NULL )
		return NULL;

	IViewPtr ptrView(punkView);
	punkView->Release();
	if( ptrView == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	ptrView->GetDocument( &punkDoc );

	return punkDoc;		
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CHistAXCtrl::GetActiveView()
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	
	if( punkDoc == NULL )
		return NULL;
	

	sptrIDocumentSite sptrDS( punkDoc );
	punkDoc->Release();
	if( sptrDS == NULL )
		return NULL;

	IUnknown* punkView = NULL;
	sptrDS->GetActiveView( &punkView );

	return punkView;

}

/////////////////////////////////////////////////////////////////////////////
void CHistAXCtrl::SetNumPhases(long nPhases)
{
	m_nPhases=max(0, min(100,nPhases));
	m_PhaseLow.alloc(m_nPhases);
	m_PhaseHigh.alloc(m_nPhases);
	m_PhaseColor.alloc(m_nPhases);

	if(m_PhaseLow.ptr()==0 || m_PhaseHigh.ptr()==0 || m_PhaseColor.ptr()==0) m_nPhases=0; //обработка ошибки выделени€ пам€ти
};
void CHistAXCtrl::SetPhaseLowThreshold(long nPhase, long fThreshold)
{
	if (nPhase<0 || nPhase>=m_nPhases) return;
	m_PhaseLow.ptr()[nPhase] = fThreshold;
	if(GetSafeHwnd()) InvalidateControl();
};
void CHistAXCtrl::SetPhaseHighThreshold(long nPhase, long fThreshold)
{
	if (nPhase<0 || nPhase>=m_nPhases) return;
	m_PhaseHigh.ptr()[nPhase] = fThreshold;
	if(GetSafeHwnd()) InvalidateControl();
};
void CHistAXCtrl::SetPhaseColor(long nPhase, OLE_COLOR clPhase)
{
	if (nPhase<0 || nPhase>=m_nPhases) return;
	m_PhaseColor.ptr()[nPhase] = clPhase;
	if(GetSafeHwnd()) InvalidateControl();
};
