// Text.cpp : Implementation of the CVTText ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "Text.h"
#include "TextPpg.h"
#include <msstkppg.h>
#include <math.h>
#include "..\\..\\ifaces\\data5.h"
#define _USEGDIPLUS
#if defined(_USEGDIPLUS)
#include <gdiplus.h>
using namespace Gdiplus;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTText, COleControl)

BEGIN_INTERFACE_MAP(CVTText, COleControl)
	INTERFACE_PART(CVTText, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTText, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CVTText, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTText, PrintCtrl)
IMPLEMENT_UNKNOWN(CVTText, ActiveXCtrl)
IMPLEMENT_AX_DATA_SITE(CVTText)



/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTText, COleControl)
	//{{AFX_MSG_MAP(CVTText)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTText, COleControl)
	//{{AFX_DISPATCH_MAP(CVTText)
	DISP_PROPERTY_EX(CVTText, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTText, "TransparentBack", GetTransparentBack, SetTransparentBack, VT_BOOL)
	DISP_PROPERTY_EX(CVTText, "FontColor", GetFontColor, SetFontColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTText, "TextPosition", GetTextPosition, SetTextPosition, VT_I2)	
	DISP_PROPERTY_EX(CVTText, "Text", GetText, SetText, VT_BSTR)
	DISP_PROPERTY_EX(CVTText, "TextFont", GetTextFont, SetTextFont, VT_FONT)	
	DISP_PROPERTY_EX(CVTText, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CVTText, "Degree", GetDegree, SetDegree, VT_I2)	
	DISP_PROPERTY_EX(CVTText, "HorzAlign", GetHorzAlign, SetHorzAlign, VT_I2)	
	DISP_PROPERTY_EX(CVTText, "VertAlign", GetVertAlign, SetVertAlign, VT_I2)	
	DISP_PROPERTY_EX(CVTText, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTText, "Key", GetKey, SetKey, VT_BSTR)
	DISP_FUNCTION(CVTText, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTText, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTText, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTText, COleControl)
	//{{AFX_EVENT_MAP(CVTText)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTText, 2)
	PROPPAGEID(CVTTextPropPage::guid)
//	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTText)
*/
static CLSID pages[2];

LPCLSID CVTText::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTTextPropPage::guid;
	pages[cPropPages++]=CLSID_StockFontPage;
	m_PropertyAXStatus=true;

	return pages;
}
/*
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	

	*/



/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTText, "VTDESIGN.VTDTextCtrl.1",
	0xbc127b17, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTText, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTDText =
		{ 0xbc127b15, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDTextEvents =
		{ 0xbc127b16, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDTextOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTText, IDS_VTDTEXT, _dwVTDTextOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTText::CVTTextFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTText

BOOL CVTText::CVTTextFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTDTEXT,
			IDB_VTDTEXT,
			afxRegApartmentThreading,
			_dwVTDTextOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


#if defined(_USEGDIPLUS)
static int s_nVTTexts = 0;
static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR           gdiplusToken;
#endif
/////////////////////////////////////////////////////////////////////////////
// CVTText::CVTText - Constructor

CVTText::CVTText() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DVTDText, &IID_DVTDTextEvents);
	InitDefaults();
	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
#if defined(_USEGDIPLUS)
	if (s_nVTTexts++ == 0)
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CVTText::~CVTText - Destructor

CVTText::~CVTText()
{
#if defined(_USEGDIPLUS)
	if (--s_nVTTexts == 0)
		GdiplusShutdown(gdiplusToken);
#endif
	// TODO: Cleanup your control's instance data here.
}




/////////////////////////////////////////////////////////////////////////////
// CVTText::DoPropExchange - Persistence support

void CVTText::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

	PX_Font(pPX, _T("TextFont"), m_fontHolder, &m_fontDesc);

}


/////////////////////////////////////////////////////////////////////////////
// CVTText::OnResetState - Reset control to default state

void CVTText::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTText::AboutBox - Display an "About" box to the user

void CVTText::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDTEXT);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTText message handlers

DWORD CVTText::GetControlFlags() 
{			
	return COleControl::GetControlFlags();// | windowlessActivate;
}								  

BOOL CVTText::PreCreateWindow(CREATESTRUCT& cs) 
{	
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

BOOL CVTText::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;

	CWnd*  pWndParent;       // handle of our parent window
	POINT  pt;

	pWndParent = GetParent();
	pt.x       = 0;
	pt.y       = 0;

	MapWindowPoints( pWndParent, &pt, 1 );
	OffsetWindowOrgEx(pDC->m_hDC, pt.x, pt.y, &pt );

	::SendMessage( pWndParent->m_hWnd, WM_ERASEBKGND,
			  (WPARAM)pDC->m_hDC, 0);	

	SetWindowOrgEx( pDC->m_hDC, pt.x, pt.y, NULL );

	return 1;
      
}



void CVTText::InitDefaults()
{
	m_BackColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentBack	= TRUE;	
	m_FontColor			= 0x80000000 + COLOR_WINDOWTEXT;
	
	m_TextPositionType	= ptTop;	

	m_strText			= "Text";

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

	m_nDegree	= 0;
	m_horzAlign = ahLeft;
	m_vertAlign = avTop;

}

void CVTText::Serialize(CArchive& ar)
{
	long nVersion = 3;


	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;		
		ar<<(SHORT)m_TextPositionType;		
		ar<<m_BackColor;
		ar<<m_bTransparentBack;
		ar<<m_FontColor;
		//ar.Write( &m_fontDesc, sizeof(FONTDESC) );
		ar<<m_strText;
		ar<<m_nDegree;
		ar<<(SHORT)m_horzAlign;
		ar<<(SHORT)m_vertAlign;
		ar<<m_strKeyValue;

	}
	else
	{			
		
		ar>>nVersion;
		SHORT nVal;		
		ar>>nVal; m_TextPositionType	= (PositionType)nVal;		
		ar>>m_BackColor;
		ar>>m_bTransparentBack;
		ar>>m_FontColor;
		ar>>m_strText;

		if( nVersion >= 2 )
		{
			ar>>m_nDegree;
			ar>>nVal; m_horzAlign = (AlignHorizontal)nVal;		
			ar>>nVal; m_vertAlign = (AlignVertical)nVal;		
		}

		if( nVersion >= 3 )
		{
			ar>>m_strKeyValue;
		}

		InvalidateCtrl( this, m_pInPlaceSiteWndless );

	}

	COleControl::Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTText::GetDisabled()
{
	if( !GetSafeHwnd() )
		return false;

	return !IsWindowEnabled();
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTText::SetDisabled(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( !bNewValue );
	Refresh();
}

BSTR CVTText::GetKey() 
{
	CString strResult = m_strKeyValue;
	return strResult.AllocSysString();
}

void CVTText::SetKey(LPCTSTR lpszNewValue) 
{
	m_strKeyValue = lpszNewValue;

	SetModifiedFlag();
}


//Restore Section & Entry from KeyValue
BOOL GetSectionEntry(const CString strKeyValue, CString& strSection, CString& strEntry)
{

	if( strKeyValue == "" ) return FALSE;

	if( strKeyValue[0] != '\\' ) return FALSE;


	CString _strKeyValue = strKeyValue;
//	_strKeyValue.Delete( 0 );
	

	int nLength = _strKeyValue.GetLength();
	int nBackSlashPosition = _strKeyValue.Find( "\\", 1 );

	// verify that accept ("?\?")
	if( nLength < 3 ) return FALSE;

	//verify not ("\?")
	if( nBackSlashPosition  < 1) return FALSE;

	strSection	= _strKeyValue.Left( nBackSlashPosition );
	strEntry	= _strKeyValue.Right( nLength - nBackSlashPosition - 1 );

	if( strSection == "" ) return FALSE;
	if( strEntry == "" ) return FALSE;

		
	
	return TRUE;
}


HRESULT CVTText::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CVTText, ActiveXCtrl)

	pThis->m_ptrSite = punkSite;
	pThis->m_ptrApp = punkVtApp;

	CString strSection;
	CString strEntry;

	if( !::GetSectionEntry( pThis->m_strKeyValue, strSection, strEntry ) )
		return S_FALSE;


	_bstr_t	bstrSection( strSection );
	_bstr_t	bstrEntry( strEntry );

	INamedDataPtr	sptrData( pThis->m_ptrApp );
	if( sptrData == 0 )
		return S_FALSE;

	_variant_t var;	
	sptrData->SetupSection( bstrSection );
	sptrData->GetValue( bstrEntry, &var );

	if( var.vt != VT_BSTR )
		return S_FALSE;	

	pThis->m_strText = var.bstrVal;	

	return S_OK;
}
  

HRESULT CVTText::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CVTText, ActiveXCtrl)	
	*pbstrName = pThis->m_strName.AllocSysString();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CVTText::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTText, PrintCtrl)			
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTText::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTText, PrintCtrl)		
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTText::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTText, PrintCtrl)
	pThis->m_fFontRatio = 1.0;
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTText::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTText, PrintCtrl)	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CVTText::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTText, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

#if defined(_USEGDIPLUS)
static RectF CalculateTextRect(Graphics &graph, wchar_t *pText, Gdiplus::Font &fontText, PointF OrigPoint,
	StringFormat &strFmt, Matrix &myMatrix)
{
	RectF rectBounds;
	graph.MeasureString(pText, -1, &fontText, OrigPoint, &strFmt, &rectBounds);
	SizeF size(rectBounds.Width, rectBounds.Height);
	PointF apts[4];
	apts[0] = PointF(0.,0.);
	apts[1] = PointF((REAL)rectBounds.Width,0.);
	apts[2] = PointF((REAL)rectBounds.Width,(REAL)rectBounds.Height);
	apts[3] = PointF(0.,(REAL)rectBounds.Height);
	myMatrix.TransformPoints(apts,4);
	REAL MinX = min(min(apts[0].X,apts[1].X),min(apts[2].X,apts[3].X));
	REAL MaxX = max(max(apts[0].X,apts[1].X),max(apts[2].X,apts[3].X));
	REAL MinY = min(min(apts[0].Y,apts[1].Y),min(apts[2].Y,apts[3].Y));
	REAL MaxY = max(max(apts[0].Y,apts[1].Y),max(apts[2].Y,apts[3].Y));
	return RectF(MinX,MinY,MaxX-MinX,MaxY-MinY);
}
#endif


/////////////////////////////////////////////////////////////////////////////
void CVTText::DrawContextGDIPlus( HDC hDC, SIZE size/* in himetrics*/ )	
{
#if defined(_USEGDIPLUS)
	Rect rcBounds(0, 0, size.cx , size.cy );
	Graphics graphics(hDC);
	if( GetDeviceCaps( hDC, TECHNOLOGY ) == DT_RASPRINTER )
		graphics.SetPageUnit(UnitPixel);
	if( !m_bTransparentBack )
	{
		COLORREF clr(TranslateColor(m_BackColor));
		SolidBrush brush(Color(GetRValue(clr),GetGValue(clr),GetBValue(clr)));
		graphics.FillRectangle(&brush,rcBounds);
	}
	if( !m_strText.IsEmpty() )
	{
		COLORREF clrFont;
		if( GetDisabled() )
			clrFont = ::GetSysColor(COLOR_GRAYTEXT);
		else
			clrFont = TranslateColor(m_FontColor);
		Color ColorFont(GetRValue(clrFont),GetGValue(clrFont),GetBValue(clrFont));
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
			double fAspect = 1.0 / 72.0 * 25.2 * 10 * m_fFontRatio;
			double dSizeY = ((double)sizeCY.Lo)/(1000.0) * fAspect;
			INT nFontStyle = FontStyleRegular;
			if (bBold) nFontStyle |= FontStyleBold;
			if (bItalic) nFontStyle |= FontStyleItalic;
			if (bUnderline) nFontStyle |= FontStyleUnderline;
			if (bStrikethrough) nFontStyle |= FontStyleStrikeout;
			Gdiplus::Font fontText(_bstr_t(strFontName),(REAL)dSizeY,nFontStyle,UnitWorld);
			PointF OrigPoint((REAL)size.cx/2,(REAL)size.cy/2);
			_bstr_t bstrText(m_strText);
			StringFormat format;
			format.SetAlignment(StringAlignmentCenter);
			format.SetLineAlignment(StringAlignmentCenter);
			graphics.SetClip(Rect(0,0,size.cx,size.cy));
			Matrix myMatrix(1.f, 0.f, 0.f, -1.f, 0.f, 0.f);
			OrigPoint.Y = -OrigPoint.Y;
			myMatrix.RotateAt(m_nDegree, OrigPoint, MatrixOrderPrepend);
			RectF rcRot = CalculateTextRect(graphics, bstrText, fontText, OrigPoint,format,myMatrix);
			PointF Offset;
			if( m_horzAlign == ahLeft )
				Offset.X = -(OrigPoint.X-rcRot.Width/2);
			else if( m_horzAlign == ahRight )
				Offset.X = OrigPoint.X-rcRot.Width/2;
			else
				Offset.X = 0;
			if( m_vertAlign == avTop )
				Offset.Y = -(OrigPoint.Y+rcRot.Height/2);
			else if( m_vertAlign == avBottom )
				Offset.Y = (OrigPoint.Y+rcRot.Height/2);
			else
				Offset.Y = 0.;
			myMatrix.Translate(Offset.X, Offset.Y, MatrixOrderAppend);
			graphics.SetTransform(&myMatrix);
			SolidBrush brushFont(ColorFont);
			graphics.DrawString(bstrText,bstrText.length(),&fontText,OrigPoint,&format,&brushFont);
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
void CVTText::DrawContextGDI( HDC hDC, SIZE size/* in himetrics*/ )	
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
/*	else if( GetDeviceCaps( hDC, TECHNOLOGY ) == DT_RASDISPLAY )
	{
		CBrush brush( ::GetSysColor( COLOR_BTNFACE ) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rcBounds, &brush );
		pdc->SelectObject( pOldBrush );
	}*/

	//Draw text

	int nTextWidth, nTextHeight;
	nTextWidth = nTextHeight = 0;

	if( !m_strText.IsEmpty() )
	{
		COLORREF clrOldText = 0;
		
		if( GetDisabled() )
			pdc->SetTextColor( ::GetSysColor( COLOR_GRAYTEXT ) );
		else
			pdc->SetTextColor( TranslateColor(m_FontColor) );

		
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

			int nAngle = m_nDegree*10;

			double fAspect = 1.0 / 72.0 * 25.2 * 10 * m_fFontRatio;

			CFont font;
			
			//Our logical unit = 0.1 mm
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


			//CFont* pOldFont = (CFont*)pdc->SelectObject( &font );
			CFont* pOldFont = (CFont*)pdc->SelectObject( &font );

			CRect rcCalc(0,0,0,0);
			VERIFY( pdc->DrawText( m_strText, &rcCalc , DT_CALCRECT ) );			

			CRect rcOut;

			double fRectWidth  = (double)abs(rcCalc.Width());
			double fRectHeight = (double)abs(rcCalc.Height());
			
			int nRadius = (int)sqrt( fRectWidth * fRectWidth / 4.0 +
									 fRectHeight * fRectHeight / 4.0
									);

			double fAddAngle = atan( fRectWidth / fRectHeight );

			CPoint pt1, pt2, pt3, pt4;

			double fAngle = (double)m_nDegree / 180.0 * PI;

			double f1Ang = - PI/2.0 - fAddAngle + fAngle + PI/2;
			double f2Ang = - PI/2.0 + fAddAngle + fAngle + PI/2;
			double f3Ang = PI/2.0 - fAddAngle + fAngle + PI/2;
			double f4Ang = PI/2.0 + fAddAngle + fAngle + PI/2;

			pt1 = CPoint( (int)(sin( f1Ang ) * nRadius), (int)(cos( f1Ang ) * nRadius) );
			pt2 = CPoint( (int)(sin( f2Ang ) * nRadius), (int)(cos( f2Ang ) * nRadius) );

			pt3 = CPoint( (int)(sin( f3Ang ) * nRadius), (int)(cos( f3Ang ) * nRadius) );
			pt4 = CPoint( (int)(sin( f4Ang ) * nRadius), (int)(cos( f4Ang ) * nRadius) );
						

			CPoint ptCenter = CPoint(
									rcBounds.left + rcBounds.Width()  / 2,
									rcBounds.top  + rcBounds.Height() / 2
									);


			//CenterPoint
			pt1 += ptCenter;
			pt2 += ptCenter;
			pt3 += ptCenter;
			pt4 += ptCenter;

			int nMinX, nMaxX;
			int nMinY, nMaxY;

			nMinX = min( min( min( pt1.x, pt2.x ), pt3.x ), pt4.x );
			nMaxX = max( max( max( pt1.x, pt2.x ), pt3.x ), pt4.x );

			nMinY = min( min( min( pt1.y, pt2.y ), pt3.y ), pt4.y );
			nMaxY = max( max( max( pt1.y, pt2.y ), pt3.y ), pt4.y );

			CPoint ptAdd( 0, 0 );
			if( m_horzAlign == ahLeft )
			{
				ptAdd.x = rcBounds.left - nMinX;
			}
			else
			if( m_horzAlign == ahRight )
			{
				ptAdd.x = rcBounds.right - nMaxX;
			}


			if( m_vertAlign == avTop )
			{
				ptAdd.y = rcBounds.bottom - nMaxY;
			}
			else
			if( m_vertAlign == avBottom )
			{
				ptAdd.y = rcBounds.top - nMinY;				
			}

			//Allign points
			pt1 += ptAdd;
			pt2 += ptAdd;
			pt3 += ptAdd;
			pt4 += ptAdd;

			rcOut.left = pt1.x;
			rcOut.top  = pt1.y;
			
			rcOut.right  = rcOut.left + rcCalc.Width();
			rcOut.bottom = rcOut.top  + rcCalc.Height();
            

			{
				CRect rcFill = rcOut;
				//rcFill.NormalizeRect();
//				pdc->FillRect( &rcFill, &CBrush( RGB(255,0,0)) );
			}

/*			if( rcOut.left < 0 )		rcOut.left = 0;
			if( rcOut.right > size.cx )	rcOut.right = size.cx;

			if( rcOut.top > size.cy )	rcOut.top = size.cy;
			if( rcOut.bottom < 0 )		rcOut.bottom = 0;*/

			// [vanek] SBT: 726 - 13.01.2004
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
			
//			pdc->TextOut( pt1.x, pt1.y, m_strText );
			pdc->DrawText( m_strText, &rcOut, DT_NOCLIP);

			pdc->SelectClipRgn( &rgnClip, RGN_DIFF );

			pdc->SelectObject( pOldFont );
		}

		pdc->SetTextColor( clrOldText );
	}



	pdc->SetBkMode( nOldBkMode );
}

void CVTText::DrawContext( HDC hDC, SIZE size/* in himetrics*/ )	
{
#if defined(_USEGDIPLUS)
	if (m_nDegree > 0 && m_strText.Find(_T('\n')) != -1)
		DrawContextGDIPlus(hDC, size);
	else
#endif
		DrawContextGDI(hDC, size);
}

/////////////////////////////////////////////////////////////////////////////
// CVTText::OnDraw - Drawing function
void CVTText::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{

	if( GetSafeHwnd() && m_bTransparentBack )
	{
		CRect rcUpdate = rcInvalid;

		CWnd* pParent = GetParent();
		if( pParent && pParent->GetSafeHwnd() )
		{			
			/*
			CRect rc;
			GetClientRect( &rc );
			ClientToScreen( &rc );
			pParent->ScreenToClient( &rc );
			pParent->InvalidateRect( &rc );
			//pParent->UpdateWindow();
			*/

			//rcInvalid
			//CRect rcParent;
		}
		
	}

	m_fFontRatio = DISPLAY_FONT_RATIO;

	CRect rcArea = rcBounds;
	rcArea.DeflateRect( 0, 0, 1, 1 );
	SIZE sizeHM;
	sizeHM.cx = FROM_DOUBLE_TO_VTPIXEL( rcArea.Width() ) / 10;
	sizeHM.cy = FROM_DOUBLE_TO_VTPIXEL( rcArea.Height() ) / 10;

	int nOldMapMode = pdc->SetMapMode( MM_ANISOTROPIC );
	pdc->SetWindowExt( sizeHM.cx, -sizeHM.cy );
	pdc->SetViewportExt( rcArea.Width(), rcArea.Height() );	
	pdc->SetViewportOrg( rcArea.left, rcArea.Height() + rcArea.top );


	double fOldFontRatio = m_fFontRatio;
	m_fFontRatio /= 10.0;

	DrawContext( pdc->GetSafeHdc(), sizeHM );

	m_fFontRatio = fOldFontRatio;

	pdc->SetMapMode( nOldMapMode );
}


/////////////////////////////////////////////////////////////////////////////
// CVTText  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTText::GetBackColor() 
{
	return m_BackColor;
}

void CVTText::SetBackColor(OLE_COLOR nNewValue) 
{
	m_BackColor = nNewValue;
	BoundPropertyChanged( 1 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	
	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTText::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVTText::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTText::GetFontColor() 
{
	return m_FontColor;
}

void CVTText::SetFontColor(OLE_COLOR nNewValue) 
{
	m_FontColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVTText::GetTextPosition() 
{	
	return (short)m_TextPositionType;
}

void CVTText::SetTextPosition(short nNewValue) 
{
	m_TextPositionType = (PositionType)nNewValue;	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
short CVTText::GetDegree()
{
	return m_nDegree;
}

/////////////////////////////////////////////////////////////////////////////
void CVTText::SetDegree(short nNewValue)
{
	m_nDegree = nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
short CVTText::GetHorzAlign()
{
	return (short)m_horzAlign;
}

/////////////////////////////////////////////////////////////////////////////
void CVTText::SetHorzAlign(short nNewValue)
{
	m_horzAlign = (AlignHorizontal)nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
short CVTText::GetVertAlign()
{
	return (short)m_vertAlign;
}

/////////////////////////////////////////////////////////////////////////////
void CVTText::SetVertAlign(short nNewValue)
{
	m_vertAlign = (AlignVertical)nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}


/////////////////////////////////////////////////////////////////////////////
BSTR CVTText::GetText() 
{
	CString strResult = m_strText;	
	return strResult.AllocSysString();
}

void CVTText::SetText(LPCTSTR lpszNewValue) 
{
	m_strText = lpszNewValue;	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}
						   
/////////////////////////////////////////////////////////////////////////////
LPFONTDISP CVTText::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

void CVTText::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.InitializeFont( NULL/*&m_fontDesc*/, newValue);

	IFontPtr fontPtr( newValue );
	if( fontPtr )
	{
		CY		sizeCY;
		fontPtr->get_Size( &sizeCY );
		BSTR	bstrName = 0;
		fontPtr->get_Name( &bstrName );
		CString str = bstrName;
		::SysFreeString( bstrName );
	}


	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}


LRESULT CVTText::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_strText, wParam);
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

BOOL CVTText::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVTText::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

//added by akm 22_11_k5
BOOL CVTText::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTText::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}
