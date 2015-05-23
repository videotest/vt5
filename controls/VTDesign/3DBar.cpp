// 3DBar.cpp : Implementation of the CVT3DBar ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "3DBar.h"
#include "3DBarPpg.h"
#include <msstkppg.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVT3DBar, COleControl)


BEGIN_INTERFACE_MAP(CVT3DBar, COleControl)
	INTERFACE_PART(CVT3DBar, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVT3DBar, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVT3DBar, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVT3DBar)



/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVT3DBar, COleControl)
	//{{AFX_MSG_MAP(CVT3DBar)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVT3DBar, COleControl)
	//{{AFX_DISPATCH_MAP(CVT3DBar)
	DISP_PROPERTY_EX(CVT3DBar, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVT3DBar, "TransparentBack", GetTransparentBack, SetTransparentBack, VT_BOOL)
	DISP_PROPERTY_EX(CVT3DBar, "FontColor", GetFontColor, SetFontColor, VT_COLOR)
	DISP_PROPERTY_EX(CVT3DBar, "TextPosition", GetTextPosition, SetTextPosition, VT_I2)
	DISP_PROPERTY_EX(CVT3DBar, "TextAlign", GetTextAlign, SetTextAlign, VT_I2)
	DISP_PROPERTY_EX(CVT3DBar, "Text", GetText, SetText, VT_BSTR)
	DISP_PROPERTY_EX(CVT3DBar, "TextFont", GetTextFont, SetTextFont, VT_FONT)
	DISP_PROPERTY_EX(CVT3DBar, "BorderType", GetBorderType, SetBorderType, VT_I2)
	DISP_PROPERTY_EX(CVT3DBar, "LineWidth", GetLineWidth, SetLineWidth, VT_R8)
	DISP_PROPERTY_EX(CVT3DBar, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVT3DBar, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVT3DBar, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVT3DBar, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVT3DBar, COleControl)
	//{{AFX_EVENT_MAP(CVT3DBar)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVT3DBar, 2)
	PROPPAGEID(CVT3DBarPropPage::guid)
	//PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVT3DBar)





/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVT3DBar, "VTDESIGN.VTD3DBarCtrl.1",
	0xbc127b1f, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVT3DBar, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTD3DBar =
		{ 0xbc127b1d, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTD3DBarEvents =
		{ 0xbc127b1e, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTD3DBarOleMisc =
	//OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVT3DBar, IDS_VTD3DBAR, _dwVTD3DBarOleMisc)




/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::CVT3DBarFactory::UpdateRegistry -
// Adds or removes system registry entries for CVT3DBar

BOOL CVT3DBar::CVT3DBarFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTD3DBAR,
			IDB_VTD3DBAR,
			afxRegApartmentThreading,
			_dwVTD3DBarOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::CVT3DBar - Constructor


CVT3DBar::CVT3DBar() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DVTD3DBar, &IID_DVTD3DBarEvents);

	InitDefaults();

	m_PropertyAXStatus=false;

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::~CVT3DBar - Destructor

CVT3DBar::~CVT3DBar()
{
	// TODO: Cleanup your control's instance data here.
}


void CVT3DBar::InitDefaults()
{
	m_BackColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentBack	= TRUE;	
	m_FontColor			= 0x80000000 + COLOR_WINDOWTEXT;

	m_BorderType		= btEdgeRaised;
	m_TextPositionType	= ptTop;
	m_TextAlignType		= atLeft;

	m_strText			= "";

	m_nLineWidth		= 50;

	m_fontDesc.cbSizeofstruct = sizeof( FONTDESC );    
    m_fontDesc.lpstrName = OLESTR("Arial");

    m_fontDesc.cySize.Lo = 80000;
	m_fontDesc.cySize.Hi = 0;
	
    m_fontDesc.sWeight = FW_THIN;
    m_fontDesc.sCharset = DEFAULT_CHARSET;
    m_fontDesc.fItalic	= FALSE;
    m_fontDesc.fUnderline = FALSE;
    m_fontDesc.fStrikethrough = FALSE;

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	
	
	m_fFontRatio = 1.0;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVT3DBar::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVT3DBar, PrintCtrl)			
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVT3DBar::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVT3DBar, PrintCtrl)		
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVT3DBar::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{	
	METHOD_PROLOGUE_EX(CVT3DBar, PrintCtrl)
	int nOldLineWidth = pThis->m_nLineWidth;
	pThis->m_nLineWidth /= 10;
	pThis->m_fFontRatio = 1.0;
	pThis->DrawContext( hDC, sizeVTPixel );
	pThis->m_nLineWidth = nOldLineWidth;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVT3DBar::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVT3DBar, PrintCtrl)	
	return S_OK;
}

HRESULT CVT3DBar::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVT3DBar, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}





/////////////////////////////////////////////////////////////////////////////
void CVT3DBar::DrawContext( HDC hDC, SIZE size/* in himetrics*/ )	
{
	
	CRect rcBounds(0, 0, size.cx , size.cy  );
	
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);

	int nOldBkMode = pdc->SetBkMode( TRANSPARENT );

	//BackGround
	if( !m_bTransparentBack )
	{
		CBrush brush( TranslateColor(m_BackColor) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rcBounds, &brush );
		pdc->SelectObject( pOldBrush );
	}

	//Draw text

	int nTextWidth, nTextHeight;
	nTextWidth = nTextHeight = 0;

	CRect rcEdge = rcBounds;
	int nEdgeD = m_nLineWidth ; //1 mm
	rcEdge.DeflateRect(nEdgeD,nEdgeD,nEdgeD,nEdgeD);

	if( !m_strText.IsEmpty() )
	{
		COLORREF clrOldText = pdc->SetTextColor( TranslateColor(m_FontColor) );
		
		IFontPtr fontPtr( m_fontHolder.m_pFont );

		if( fontPtr )
		{
			BSTR	bstrName;
			CY		sizeCY;
			BOOL	bBold;
			BOOL	bItalic;
			BOOL	bUnderline;
			BOOL	bStrikethrough;
			SHORT	nWeight;
			SHORT	nCharset;

			fontPtr->get_Name( &bstrName );
			fontPtr->get_Size( &sizeCY );
			fontPtr->get_Bold( &bBold );
			fontPtr->get_Italic( &bItalic );
			fontPtr->get_Underline( &bUnderline );
			fontPtr->get_Strikethrough( &bStrikethrough );
			fontPtr->get_Weight( &nWeight );
			fontPtr->get_Charset( &nCharset );                

			CString strFontName = bstrName;
			::SysFreeString( bstrName );

			int nAngle;

			if( m_TextPositionType == ptTop || m_TextPositionType == ptBottom )
				nAngle = 0;
			if( m_TextPositionType == ptLeft )
				nAngle = -900;

			if( m_TextPositionType == ptRight )
				nAngle = 900;
			
			double fAspect = 1.0 / 72.0 * 25.2 * 10.0 * m_fFontRatio;
			CFont font;

			font.CreateFont( 
				(int)(((double)sizeCY.Lo)/(1000.0) * fAspect), 0, 
				nAngle, 0, 
				nWeight, (BYTE)bItalic, 
				(BYTE)bUnderline, (BYTE)bStrikethrough, 
				(BYTE)DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,DEFAULT_PITCH,						
				strFontName 
				);


			CFont* pOldFont = (CFont*)pdc->SelectObject( &font );

			CRect rcCalc(0,0,0,0);
			VERIFY( pdc->DrawText( m_strText, &rcCalc , DT_CALCRECT ) );			
			rcCalc.NormalizeRect();

			if( m_TextPositionType == ptTop || m_TextPositionType == ptBottom )
			{
				nTextWidth  = rcCalc.Width();
				nTextHeight = rcCalc.Height();
			}
			else
			{
				nTextHeight = rcCalc.Width();
				nTextWidth  = rcCalc.Height();
			}

			int nX, nY;
			int nBoundsWidth  = rcBounds.Width();
			int nBoundsHeight = rcBounds.Height();


			if( m_TextPositionType == ptTop )
			{
				rcEdge.bottom -= nTextHeight;
				nY = nBoundsHeight - nEdgeD;

				if( m_TextAlignType == atLeft ) nX = 5*nEdgeD;

				if( m_TextAlignType == atCenter ) nX = (nBoundsWidth - nTextWidth-5*nEdgeD)/2;									
				
				if( m_TextAlignType == atRight ) nX = nBoundsWidth - nTextWidth - 10*nEdgeD;

			}

			if( m_TextPositionType == ptBottom )
			{
				rcEdge.top += nTextHeight;
				nY = nTextHeight + nEdgeD;				

				if( m_TextAlignType == atLeft )  nX = 5*nEdgeD;

				if( m_TextAlignType == atCenter ) nX = (nBoundsWidth - nTextWidth-5*nEdgeD)/2;
					
				if( m_TextAlignType == atRight ) nX = nBoundsWidth - nTextWidth - 10*nEdgeD;
			}

			if( m_TextPositionType == ptLeft )
			{	
				nX = nEdgeD;
				rcEdge.left += nTextWidth;
				if( m_TextAlignType == atLeft ) nY = 5*nEdgeD;
				
				if( m_TextAlignType == atCenter ) nY = (nBoundsHeight - nTextHeight - 5*nEdgeD)/2;

				if( m_TextAlignType == atRight ) nY = nBoundsHeight - nTextHeight - 10*nEdgeD;
			}

			if( m_TextPositionType == ptRight )
			{
				nX = nBoundsWidth-nEdgeD;
				rcEdge.right -= nTextWidth;
				if( m_TextAlignType == atLeft ) nY = nBoundsHeight - 5*nEdgeD;

				if( m_TextAlignType == atCenter ) nY = (nBoundsHeight + nTextHeight + 5*nEdgeD)/2;

				if( m_TextAlignType == atRight ) nY = nTextHeight + 10*nEdgeD;

			}
			
			
			CRect rcOut;


			if( m_TextPositionType == ptTop || m_TextPositionType == ptBottom )			
				rcOut = CRect( nX, nY, nX + nTextWidth, nY - nTextHeight );

			if( m_TextPositionType == ptLeft )
				rcOut = CRect( nX, nY , nX + nTextWidth, nY + nTextHeight );

			if( m_TextPositionType == ptRight )
				rcOut = CRect( nX, nY, nX - nTextWidth, nY - nTextHeight );
			
			if( rcOut.left < rcBounds.left )	rcOut.left = rcBounds.left;
			if( rcOut.right > rcBounds.right )	rcOut.right = rcBounds.right;
			if( rcOut.top > rcBounds.bottom )	rcOut.top = rcBounds.bottom;
			if( rcOut.bottom < rcBounds.top )	rcOut.bottom = rcBounds.top;
			
			CRgn rgnClip;
			pdc->BeginPath();
			pdc->MoveTo( 0, 0 );
			pdc->LineTo( size.cx, 0 );
			pdc->LineTo( size.cx, size.cy );
			pdc->LineTo( 0, size.cy );
            pdc->LineTo( 0, 0 );
			pdc->EndPath( );
			rgnClip.CreateFromPath( pdc );
			pdc->SelectClipRgn( &rgnClip, RGN_AND );

			VERIFY( pdc->DrawText(m_strText, &rcOut, DT_LEFT ) );

			pdc->SelectClipRgn( &rgnClip, RGN_DIFF );
			pdc->SelectClipRgn( 0 );
			pdc->SelectObject( pOldFont );
		}

		pdc->SetTextColor( clrOldText );
	}

	
	if( m_BorderType != btEdgeNone )
	{


		COLORREF clrBaseColor = ::GetSysColor( COLOR_BTNFACE );

		BYTE red, green, blue;

		red		= GetRValue( clrBaseColor );
		green	= GetRValue( clrBaseColor );
		blue	= GetRValue( clrBaseColor );

		int nValue = min( min((255 - red)/2, (255 - green)/2 ), (255 - blue)/2 );		
		int nValue1 = min( min((red)/2, (green)/2 ), (blue)/2 );		

		COLORREF clM1 = RGB( 0,0,0 );
		COLORREF clM2 = RGB( nValue1, nValue1, nValue1 );
		COLORREF clM3 = clrBaseColor;
		COLORREF clM4 = RGB( 255 - nValue, 255 - nValue, 255 - nValue );

		//********************1**************************@
		//*###################2#########################$@
		//*#     										$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*#											$@
		//*$$$$$$$$$$$$$$$$$$$$4$$$$$$$$$$$$$$$$$$$$$$$$$@
		//@@@@@@@@@@@@@@@@@@@@@3@@@@@@@@@@@@@@@@@@@@@@@@@@


		int nPenWidth = m_nLineWidth;
		
		CBrush brush1(clM1);
		CBrush brush2(clM2);
		CBrush brush3(clM3);
		CBrush brush4(clM4);
		
		//
		int nClientWidth  = rcEdge.Width();
		int nClientHeight = rcEdge.Height();

		CBrush* pBrush1, *pBrush2, *pBrush3, *pBrush4;

		if( m_BorderType == btEdgeRaised )
		{
			pBrush1 = &brush3;	pBrush2 = &brush4;	pBrush3 = &brush1;	pBrush4 = &brush2;
		}			

		if( m_BorderType == btEdgeSunken )
		{
			pBrush1 = &brush2;	pBrush2 = &brush1;	pBrush3 = &brush4;	pBrush4 = &brush3;
		}

		if( m_BorderType == btEdgeEtched )
		{
			pBrush1 = &brush2;	pBrush2 = &brush4;	pBrush3 = &brush4;	pBrush4 = &brush2;
		}

		if( m_BorderType == btEdgeBump )
		{
			pBrush1 = &brush3;	pBrush2 = &brush1;	pBrush3 = &brush1;	pBrush4 = &brush4;
		}

		CPoint ptAdd( rcEdge.left, rcEdge.top );

		//At first vert then horz line
		//*********	
		CRect rc1(0,nPenWidth,nPenWidth,nClientHeight-nPenWidth);
		CRect rc2(0,nClientHeight,nClientWidth - nPenWidth,nClientHeight-nPenWidth);

		rc1 += ptAdd;
		rc2 += ptAdd;

		pdc->FillRect( &rc1, pBrush1 );
		pdc->FillRect( &rc2, pBrush1 );
		
		//###############
		CRect rc3(nPenWidth,2*nPenWidth,2*nPenWidth,nClientHeight-2*nPenWidth);
		CRect rc4(nPenWidth,nClientHeight-nPenWidth,nClientWidth - 2*nPenWidth,nClientHeight-2*nPenWidth);
		rc3 += ptAdd;
		rc4 += ptAdd;
		pdc->FillRect( &rc3, pBrush2 );
		pdc->FillRect( &rc4, pBrush2 );
		
		//@@@@@@@@@
		CRect rc5(0, nPenWidth, nClientWidth , 0 );
		CRect rc6(nClientWidth- nPenWidth, 0, nClientWidth, nClientHeight );
		rc5 += ptAdd;
		rc6 += ptAdd;
		pdc->FillRect( &rc5, pBrush3 );
		pdc->FillRect( &rc6, pBrush3 );

		//$$$$$$$$$
		CRect rc7(nPenWidth, 2*nPenWidth, nClientWidth-nPenWidth , nPenWidth );
		CRect rc8(nClientWidth- 2*nPenWidth, nPenWidth, nClientWidth-nPenWidth, nClientHeight-nPenWidth );
		rc7 += ptAdd;
		rc8 += ptAdd;

		pdc->FillRect( &rc7, pBrush4 );
		pdc->FillRect( &rc8, pBrush4 );
	}

	pdc->SetBkMode( nOldBkMode );


}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::OnDraw - Drawing function
void CVT3DBar::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{

	m_fFontRatio = DISPLAY_FONT_RATIO;

	CRect rcArea = rcBounds;
	rcArea.right += 2;

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

	double fOldFontRatio = m_fFontRatio;
	m_fFontRatio /= 10.0;

	DrawContext( pdc->GetSafeHdc(), sizeHM );
	
	m_nLineWidth = nOldLineWidth;
	m_fFontRatio = fOldFontRatio;

	pdc->SetMapMode( nOldMapMode );
	
}


/////////////////////////////////////////////////////////////////////////////
BOOL CVT3DBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;
	return COleControl::PreCreateWindow(cs);
	
}

/////////////////////////////////////////////////////////////////////////////
DWORD CVT3DBar::GetControlFlags() 
{	
	return COleControl::GetControlFlags();// | windowlessActivate;
}



/////////////////////////////////////////////////////////////////////////////
BOOL CVT3DBar::OnEraseBkgnd(CDC* pDC) 
{	
	// This is needed for transparency and the correct drawing...
	CWnd*  pWndParent = NULL; // handle of our parent window
	//POINT  pt;

	/*
	pWndParent = GetParent();
	pWndParent->Invalidate();
	pWndParent->UpdateWindow();
	*/

	//pWndParent = GetParent();
	//::SendMessage( pWndParent->m_hWnd, WM_ERASEBKGND, (WPARAM)pDC->m_hDC, 0 );


	/*
	CWnd* pWnd = pWndParent;
	
	HWND hWnd = pWnd->GetSafeHwnd();
	while( hWnd )
	{
		::SendMessage( hWnd, WM_ERASEBKGND,
			  (WPARAM)pDC->m_hDC, 0);
		if( pWnd )
		{
			pWnd = pWnd->GetParent();
			hWnd = pWnd->GetSafeHwnd();
		}
	}
	

	return true;
	*/

	/*
	pt.x       = 0;
	pt.y       = 0;

	MapWindowPoints(pWndParent, &pt, 1);
	OffsetWindowOrgEx(pDC->m_hDC, pt.x, pt.y, &pt);
	
	::SendMessage(pWndParent->m_hWnd, WM_ERASEBKGND,
			  (WPARAM)pDC->m_hDC, 0);

	//SetWindowOrgEx(pDC->m_hDC, pt.x, pt.y, NULL);	
	*/

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVT3DBar  Serialization
/////////////////////////////////////////////////////////////////////////////
void CVT3DBar::Serialize(CArchive& ar)
{
	long nVersion = 3;


	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;
		ar<<(SHORT)m_BorderType;
		ar<<(SHORT)m_TextPositionType;
		ar<<(SHORT)m_TextAlignType;
		ar<<m_BackColor;
		ar<<m_bTransparentBack;
		ar<<m_FontColor;
		//ar.Write( &m_fontDesc, sizeof(FONTDESC) );
		ar<<m_strText;		
		ar<<m_nLineWidth;

	}
	else
	{			
		
		ar>>nVersion;
		SHORT nVal;
		if( nVersion >= 2 )
		{
			ar>>nVal; m_BorderType			= (BorderType)nVal;
			ar>>nVal; m_TextPositionType	= (PositionType)nVal;
			ar>>nVal; m_TextAlignType		= (AlignType)nVal;
			ar>>m_BackColor;
			ar>>m_bTransparentBack;
			ar>>m_FontColor;
			//ar.Read( &m_fontDesc, sizeof(FONTDESC) );
			ar>>m_strText;
		}

		if( nVersion >= 3 )
		{
			ar>>m_nLineWidth;
		}


		InvalidateCtrl( this, m_pInPlaceSiteWndless );

	}

	COleControl::Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::DoPropExchange - Persistence support

void CVT3DBar::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);
	
	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

	PX_Font(pPX, _T("TextFont"), m_fontHolder, &m_fontDesc);
	
}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::OnResetState - Reset control to default state

void CVT3DBar::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();	
}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBar::AboutBox - Display an "About" box to the user

void CVT3DBar::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTD3DBAR);
	dlgAbout.DoModal();
}



/////////////////////////////////////////////////////////////////////////////
// CVT3DBar  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVT3DBar::GetBackColor() 
{
	return m_BackColor;
}

void CVT3DBar::SetBackColor(OLE_COLOR nNewValue) 
{
	m_BackColor = nNewValue;
	BoundPropertyChanged( 1 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVT3DBar::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVT3DBar::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;	
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVT3DBar::GetFontColor() 
{
	return m_FontColor;
}

void CVT3DBar::SetFontColor(OLE_COLOR nNewValue) 
{
	m_FontColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();		
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVT3DBar::GetTextPosition() 
{	
	return (short)m_TextPositionType;
}

void CVT3DBar::SetTextPosition(short nNewValue) 
{
	m_TextPositionType = (PositionType)nNewValue;	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
short CVT3DBar::GetTextAlign() 
{	
	return m_TextAlignType;
}

void CVT3DBar::SetTextAlign(short nNewValue) 
{
	m_TextAlignType = (AlignType)nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
BSTR CVT3DBar::GetText() 
{
	CString strResult = m_strText;	
	return strResult.AllocSysString();
}

void CVT3DBar::SetText(LPCTSTR lpszNewValue) 
{
	m_strText = lpszNewValue;	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}
						   
/////////////////////////////////////////////////////////////////////////////
LPFONTDISP CVT3DBar::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

void CVT3DBar::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVT3DBar::GetBorderType() 
{
	return (short)m_BorderType;
}

void CVT3DBar::SetBorderType(short nNewValue) 
{
	m_BorderType = (BorderType)nNewValue;
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

double CVT3DBar::GetLineWidth() 
{
	return FROM_VTPIXEL_TO_DOUBLE(m_nLineWidth);
}

void CVT3DBar::SetLineWidth(double newValue) 
{
	m_nLineWidth = FROM_DOUBLE_TO_VTPIXEL( newValue );
	SetModifiedFlag();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
}

LRESULT CVT3DBar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_strText, (int)wParam);
		if ((int)wParam > m_strText.GetLength())
		wParam = m_strText.GetLength();
		return TRUE;    
	}

	if( message == WM_SETTEXT )
	{
		if (lParam == NULL)
		{
			// NULL lParam means set caption to nothing
			m_strText.Empty();
		}
		else
		{
			// non-NULL sets caption to that specified by lParam
			lstrcpy(m_strText.GetBufferSetLength(lstrlen((LPCTSTR)lParam)),
				(LPCTSTR)lParam);
		}
		
		InvalidateCtrl( this, m_pInPlaceSiteWndless );
		SetModifiedFlag();    
		return TRUE;
	}
	
	return COleControl::DefWindowProc(message, wParam, lParam);
}

BOOL CVT3DBar::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVT3DBar::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}


LRESULT CVT3DBar::OnNcHitTest(CPoint point)
{
	// [vanek]: пропускаем сообщения дальше,чтобы они доходили до других контролов
	return HTTRANSPARENT;
}

BOOL CVT3DBar::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVT3DBar::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}