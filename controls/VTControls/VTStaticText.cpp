// VTStaticText.cpp : Implementation of the CVTStaticText ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTStaticText.h"
#include "VTStaticTextPpg.h"
#include "FramesPropPage.h"

#include <msstkppg.h>
#include "utils.h"
#include "BaseColumn.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTStaticText, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTStaticText, COleControl)
	//{{AFX_MSG_MAP(CVTStaticText)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTStaticText, COleControl)
	//{{AFX_DISPATCH_MAP(CVTStaticText)
	DISP_PROPERTY_EX(CVTStaticText, "TextColor", GetTextColor, SetTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTStaticText, "TextBackColor", GetTextBackColor, SetTextBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTStaticText, "TransparentColor", GetTransparentColor, SetTransparentColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignLeft", GetTextAlignLeft, SetTextAlignLeft, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignRight", GetTextAlignRight, SetTextAlignRight, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignTop", GetTextAlignTop, SetTextAlignTop, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignBottom", GetTextAlignBottom, SetTextAlignBottom, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "PictAlignLeft", GetPictAlignLeft, SetPictAlignLeft, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "PictAlignRight", GetPictAlignRight, SetPictAlignRight, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "PictAlignTop", GetPictAlignTop, SetPictAlignTop, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "PictAlignBottom", GetPictAlignBottom, SetPictAlignBottom, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Text2PictAlignLeft", GetText2PictAlignLeft, SetText2PictAlignLeft, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Text2PictAlignRight", GetText2PictAlignRight, SetText2PictAlignRight, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Text2PictAlignTop", GetText2PictAlignTop, SetText2PictAlignTop, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Text2PictAlignBottom", GetText2PictAlignBottom, SetText2PictAlignBottom, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Text", GetText, SetText, VT_BSTR)
	DISP_PROPERTY_EX(CVTStaticText, "PictureFileName", GetPictureFileName, SetPictureFileName, VT_BSTR)
	DISP_PROPERTY_EX(CVTStaticText, "InnerRaised", GetInnerRaised, SetInnerRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "InnerSunken", GetInnerSunken, SetInnerSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "OuterRaised", GetOuterRaised, SetOuterRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "OuterSunken", GetOuterSunken, SetOuterSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "CenterImage", GetCenterImage, SetCenterImage, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TransparencyText", GetTransparencyText, SetTransparencyText, VT_BOOL)	
	DISP_FUNCTION(CVTStaticText, "ReadPicture", ReadPicture, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CVTStaticText, "SetAutoSize", SetAutoSize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTStaticText, "ReloadBitmap", ReloadBitmap, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_EX(CVTStaticText, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTStaticText, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTStaticText, "TransparentChangeToColor", GetTransparentChangeToColor, SetTransparentChangeToColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTStaticText, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignCenter", GetTextAlignCenter, SetTextAlignCenter, VT_BOOL)
	DISP_PROPERTY_EX(CVTStaticText, "TextAlignMiddle", GetTextAlignMiddle, SetTextAlignMiddle, VT_BOOL)
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_BACKCOLOR()
	
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTStaticText, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTStaticText, COleControl)
	//{{AFX_EVENT_MAP(CVTStaticText)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTStaticText, 5)
	PROPPAGEID(CVTStaticTextPropPage::guid)
	PROPPAGEID(CVTStaticTextPropPageExt::guid)	
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTStaticText)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTStaticText, "VTCONTROLS.VTStaticText.1",
	0xd962dece, 0xfa07, 0x47dc, 0xa9, 0x1, 0x73, 0xde, 0xe4, 0x4f, 0xf0, 0x21)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTStaticText, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTStaticText =
		{ 0xb8e8aca1, 0xf32d, 0x4d62, { 0x8e, 0xe0, 0x28, 0xaf, 0x26, 0x84, 0xca, 0x5d } };
const IID BASED_CODE IID_DVTStaticTextEvents =
		{ 0x9886e2c0, 0x3ca3, 0x4bce, { 0x97, 0x12, 0x5f, 0x88, 0x4e, 0xa1, 0xee, 0xf8 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTStaticTextOleMisc =	
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTStaticText, IDS_VTSTATICTEXT, _dwVTStaticTextOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::CVTStaticTextFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTStaticText

BOOL CVTStaticText::CVTStaticTextFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTSTATICTEXT,
			IDB_VTSTATICTEXT,
			afxRegApartmentThreading,
			_dwVTStaticTextOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::CVTStaticText - Constructor
static const FONTDESC _fontdesc =
  { sizeof(FONTDESC), OLESTR("MS Sans Serif"), FONTSIZE( 8 ), FW_THIN, 
     ANSI_CHARSET, FALSE, FALSE, FALSE };

CVTStaticText::CVTStaticText()
{
	InitializeIIDs(&IID_DVTStaticText, &IID_DVTStaticTextEvents);
	InitDefaults();
	
	SetInitialSize( 300, 300 );

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::~CVTStaticText - Destructor

CVTStaticText::~CVTStaticText()
{
}

	 
/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::OnDraw - Drawing function


void CVTStaticText::OnDrawMetafile(CDC* pDC, const CRect& rcBounds)
{
	COleControl::OnDrawMetafile(pDC,rcBounds);
}

BOOL CVTStaticText::IsSubclassedControl()
{
	return FALSE;
}

void CVTStaticText::OnDraw(
			CDC* pDC, const CRect& rcBounds, const CRect& rcInvalid)
{

	CRect _rc = rcBounds;
	if( GetSafeHwnd() )
		GetClientRect( &_rc );
	//if (m_hWnd == NULL)
	//	CreateWindowForSubclassedControl();

	pDC->SetBkMode(TRANSPARENT);
	
	CRect rc = _rc;
	

	//Erase background
	OLE_COLOR clrBackColor = m_clrTransparentChangeToColor;
	CBrush brushBack;
	brushBack.CreateSolidBrush( TranslateColor( clrBackColor ) );	
	pDC->FillRect(rcBounds, &brushBack);

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
		pDC->DrawEdge(CRect(0,0,rc.Width(),rc.Height()), edgeType, BF_RECT);

		rc.left		+= VT_BORDER_WIDTH;
		rc.top		+= VT_BORDER_WIDTH;
		rc.right	-= VT_BORDER_WIDTH;
		rc.bottom	-= VT_BORDER_WIDTH;

	}
	
	//To avoid erasing border
	//pDC->IntersectClipRect( rc );
/*	CRgn rgn;
	rgn.CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	pDC->SelectClipRgn( &rgn );
	*/

	//Draw Image
	DrawImage( pDC, rc );

	//Draw text;
	DrawText( pDC, rc );
}


static int nDCpicXPos = 0;
static int nDCpicYPos = 0;

static int nDCpicWidth = 0;
static int nDCpicHeight = 0;


void CVTStaticText::DrawImage( CDC* pDC, CRect& rc)
{	
	if( m_ImageList.m_hImageList == NULL ) return;
	if( m_nImageIndex < 0 ) return;

	if( (m_nImageWidth < 1) || (m_nImageHeight < 1) ) return;

	int nLeft, nTop;
	int nWidth, nHeight;

	////////////////////////////
	//	Horizontal align
	////////////////////////////
	if( !m_bPictAlignLeft && !m_bPictAlignRight ){
		nLeft	= rc.left;
		nWidth	= m_nImageWidth;
	}

	if( m_bPictAlignLeft && m_bPictAlignRight ){
		nLeft	= rc.left;
		nWidth	= rc.Width( );
	}
	
	if( !m_bPictAlignLeft && m_bPictAlignRight ){
		nLeft	= rc.left + rc.Width( ) - m_nImageWidth;
		nWidth	= m_nImageWidth;
	}

	if( m_bPictAlignLeft && !m_bPictAlignRight ){
		nLeft	= rc.left;
		nWidth	= m_nImageWidth;
	}


	////////////////////////////
	//	Vertical align
	////////////////////////////
	if( !m_bPictAlignTop && !m_bPictAlignBottom ){
		nTop	= rc.top;
		nHeight	= m_nImageHeight;
	}

	if( m_bPictAlignTop && m_bPictAlignBottom ){
		nTop	= rc.top;
		nHeight	= rc.Height( );
	}

	if( !m_bPictAlignTop && m_bPictAlignBottom ){
		nTop	= rc.top + rc.Height() - m_nImageHeight;
		nHeight	= m_nImageHeight;
	}

	if( m_bPictAlignTop && !m_bPictAlignBottom ){
		nTop	= rc.top;
		nHeight	= m_nImageHeight;
	}


	if( m_bCentrImage )
	{
		nWidth	= m_nImageWidth;
		nHeight	= m_nImageHeight;

	   	nLeft	= rc.left + rc.Width( ) / 2 - nWidth / 2;
		nTop	= rc.top  + rc.Height( ) / 2 - nHeight / 2;

	}


	nDCpicXPos		= nLeft;
	nDCpicYPos		= nTop;

	nDCpicWidth		= nWidth;
	nDCpicHeight	= nHeight;
	

	m_ImageList.Draw(   pDC, 						
						m_nImageIndex,
						CPoint(nLeft,nTop),
						ILD_NORMAL
						);


	return;
	//CreateTansparencyImage(pDC);

	
	if( m_nImageIndex < 0 ) return;

	CDC dcImage;
	dcImage.CreateCompatibleDC(pDC);
	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( pDC, m_nImageWidth, m_nImageHeight );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );


	m_ImageList.Draw(   &dcImage, 						
						m_nImageIndex,
						CPoint(0,0),
						ILD_NORMAL
						);

/*	
	CClientDC dc(0);
	dc.BitBlt(0, 0, m_nImageWidth, m_nImageHeight, &dcCompatible, 0, 0, SRCCOPY);
*/	
	
	//////////////////////////////////////////////

	COLORREF clrTransparent = TranslateColor( m_clrTransparent );
	COLORREF clrBackColor = TranslateColor( GetBackColor() );


	COLORREF crOldBack = pDC->SetBkColor( RGB(255,255,255) );
	COLORREF crOldText = pDC->SetTextColor(RGB(0,0,0));


	CDC dcTrans;

	// Create two memory dcs for the image and the mask
	//dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// Select the image into the appropriate dc
	//CBitmap* pOldBitmapImage = dcImage.SelectObject(&Bitmap);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	int nWidthBmp = m_nImageWidth;
	int nHeightBmp = m_nImageHeight;
	bitmapTrans.CreateBitmap(nWidthBmp, nHeightBmp, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor( clrTransparent );
	dcTrans.BitBlt(0, 0, nWidthBmp, nHeightBmp, &dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	pDC->StretchBlt( nLeft, nTop, nWidth, nHeight, &dcImage, 0, 0, 
		m_nImageWidth, m_nImageHeight, SRCINVERT);
	pDC->StretchBlt( nLeft, nTop, nWidth, nHeight, &dcTrans, 0, 0, 
		m_nImageWidth, m_nImageHeight, SRCAND);
	pDC->StretchBlt( nLeft, nTop, nWidth, nHeight, &dcImage, 0, 0, 
		m_nImageWidth, m_nImageHeight, SRCINVERT);

	/*
	pDC->BitBlt(0, 0, nWidthBmp, nHeightBmp, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(0, 0, nWidthBmp, nHeightBmp, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(0, 0, nWidthBmp, nHeightBmp, &dcImage, 0, 0, SRCINVERT);
	*/

	// Restore settings
	dcImage.SelectObject( pOldBitmap );
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);

	//////////////////////////////////////////////


}

void CVTStaticText::DrawText( CDC* pDC, CRect& rc)
{

	CFont font;
	if( !::GetFontFromDispatch( this, font , GetUseSystemFont()) )
		return;

	CFont* pOldFont = (CFont*)pDC->SelectObject( &font );//SelectStockFont( &dc );

	CString strTextOut = m_strCaption;

	CRect rcCalc = CRect(0,0,0,0);
	{
		//CDC dc;//(this);			
		//dc.CreateCompatibleDC( pDC );
		//pDCSelectStockFont( &dc );
		pDC->DrawText( strTextOut, &rcCalc , DT_CALCRECT );
	}

	int nLeft, nTop;
	int nWidth, nHeight;

	int nTextWidth = rcCalc.Width();
	int nTextHeight = rcCalc.Height();


	////////////////////////////
	//	Horizontal align
	////////////////////////////
	if( !m_bTextAlignLeft && !m_bTextAlignRight ){
		nLeft	= rc.left;
		nWidth	= nTextWidth;
	}

	if( m_bTextAlignLeft && m_bTextAlignRight ){
		nLeft	= rc.left;
		nWidth	= rc.Width( );
	}
	
	if( !m_bTextAlignLeft && m_bTextAlignRight ){
		nLeft	= rc.left + rc.Width( ) - nTextWidth;
		nWidth	= nTextWidth;
	}

	if( m_bTextAlignLeft && !m_bTextAlignRight ){
		nLeft	= rc.left;
		nWidth	= nTextWidth;
	}

	if( m_bTextAlignCenter ){
		nLeft	= rc.left + rc.Width() / 2  - nTextWidth / 2;
		nWidth	= nTextWidth;			  
	}

	////////////////////////////
	//	Vertical align
	////////////////////////////
	if( !m_bTextAlignTop && !m_bTextAlignBottom ){
		nTop	= rc.top;
		nHeight	= nTextHeight;
	}

	if( m_bTextAlignTop && m_bTextAlignBottom ){
		nTop	= rc.top;
		nHeight	= rc.Height( );
	}

	if( !m_bTextAlignTop && m_bTextAlignBottom ){
		nTop	= rc.top + rc.Height() - nTextHeight;
		nHeight	= nTextHeight;
	}

	if( m_bTextAlignTop && !m_bTextAlignBottom ){
		nTop	= rc.top;
		nHeight	= nTextHeight;
	}
	
	if( m_bTextAlignMiddle ){
		nTop	= rc.top + rc.Height() / 2 - nTextHeight / 2;
		nHeight	= nTextHeight;
	}

	////////////////////////////
	//	Perform	align by picture
	////////////////////////////
	if( m_ImageList.m_hImageList != NULL ){
		if( m_nImageIndex >= 0 ){
			if( (m_nImageWidth > 0) && (m_nImageHeight > 0) ){ // Picture OK try align by picture

				////////////////////////////
				//	Horizontal align
				////////////////////////////
				if( m_bText2PictAlignLeft && m_bText2PictAlignRight){
					nLeft	= nDCpicXPos;
					nWidth	= nDCpicWidth;
					
				}

				if( m_bText2PictAlignLeft && !m_bText2PictAlignRight){						
					nWidth	= nTextWidth;
					nLeft	= nDCpicXPos - nWidth;
				}

				if( !m_bText2PictAlignLeft && m_bText2PictAlignRight){
					nWidth	= nTextWidth;
					nLeft	= nDCpicXPos + nDCpicWidth;

				}

				////////////////////////////
				//	Vertical align
				////////////////////////////
				if( m_bText2PictAlignTop && m_bText2PictAlignBottom){
					nTop	= nDCpicYPos;
					nHeight	= nDCpicHeight;
				}

				if( m_bText2PictAlignTop && !m_bText2PictAlignBottom){						
					nHeight	= nTextHeight;
					nTop	= nDCpicYPos - nHeight;					
				}

				if( !m_bText2PictAlignTop && m_bText2PictAlignBottom){
					nHeight	= nTextHeight;
					nTop	= nDCpicYPos + nDCpicHeight;

				}
			}
		}
	}

	CRect rectText = CRect( nLeft, nTop, nLeft + nWidth, nTop + nHeight );
	

	COLORREF colorOld = pDC->GetTextColor();
					   
	
	if( GetDisabled() )
		pDC->SetTextColor( ::GetSysColor( COLOR_GRAYTEXT ) );
	else
		pDC->SetTextColor( TranslateColor(m_clrText) );		
	
	
	
	int nOldBkMode;
	
	if( m_bTransparencyText )
	{
		nOldBkMode = pDC->SetBkMode( TRANSPARENT );			
		pDC->DrawText( strTextOut, &rectText , DT_END_ELLIPSIS );
		
	}
	else
	{
		COLORREF colorBack = pDC->SetBkColor( TranslateColor(m_clrBackText) );				
		nOldBkMode = pDC->SetBkMode( OPAQUE );			
		pDC->DrawText( strTextOut, &rectText , DT_END_ELLIPSIS );
		pDC->SetBkColor( TranslateColor( colorBack ) );
	}

	
	pDC->SetBkMode( nOldBkMode );

	pDC->SelectObject(pOldFont);
	
	pDC->SetTextColor( colorOld );
}


DWORD CVTStaticText::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	return dwFlags/* | windowlessActivate*/;
}




/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::DoPropExchange - Persistence support


LONG nVersion = 11;	

void CVTStaticText::DoPropExchange(CPropExchange* pPX)
{
	
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);	

	PX_Long(pPX, "Version", nVersion );
	
	/*
	PX_Font(pPX, "Font", InternalGetFont(), &_fontdesc );

	
	OLE_COLOR clrOLE;

	if( pPX->IsLoading() )
	{
		if( nVersion >= 2)
		{						
			PX_Font(pPX, "Font", InternalGetFont(), &_fontdesc );			
		}
		if( nVersion >= 5)
		{	
			PX_Color(pPX, "TextColor", clrOLE , (OLE_COLOR)::GetSysColor(COLOR_WINDOWTEXT) );		
			m_clrText = TranslateColor( clrOLE );

			PX_Color(pPX, "TextBackColor", clrOLE, (OLE_COLOR)::GetSysColor(COLOR_BTNFACE) );		
			m_clrBackText = TranslateColor( clrOLE );

			PX_Color(pPX, "TransparentColor", clrOLE, (OLE_COLOR)::GetSysColor(COLOR_BTNFACE) );		
			m_clrTransparent = TranslateColor( clrOLE );

			PX_Bool(pPX, "TextAlignLeft", m_bTextAlignLeft, TRUE);
			PX_Bool(pPX, "TextAlignRight", m_bTextAlignRight, FALSE);
			PX_Bool(pPX, "TextAlignTop", m_bTextAlignTop, TRUE);
			PX_Bool(pPX, "TextAlignBottom", m_bTextAlignBottom, FALSE);

			PX_Bool(pPX, "PictAlignLeft", m_bPictAlignLeft, TRUE);
			PX_Bool(pPX, "PictAlignRight", m_bPictAlignRight, FALSE);
			PX_Bool(pPX, "PictAlignTop", m_bPictAlignTop, TRUE);			
			PX_Bool(pPX, "PictAlignBottom", m_bPictAlignBottom, FALSE);

			PX_Bool(pPX, "Text2PictAlignLeft", m_bText2PictAlignLeft, FALSE);
			PX_Bool(pPX, "Text2PictAlignRight", m_bText2PictAlignRight, FALSE);
			PX_Bool(pPX, "Text2PictAlignTop", m_bText2PictAlignTop, FALSE);
			PX_Bool(pPX, "Text2PictAlignBottom", m_bText2PictAlignBottom, FALSE);

			PX_String(pPX, "Text", m_strCaption, "Text" );
			PX_String(pPX, "PictureFileName", m_strFileName, "" );
			
			short n;
			PX_Short(pPX, "m_nImageIndex", n, -1);
			m_nImageIndex = (int)n;
			PX_Short(pPX, "m_nImageWidth", n, 0);
			m_nImageWidth = (int)n;
			PX_Short(pPX, "m_nImageHeight", n, 0);
			m_nImageHeight = (int)n;
	
		}
			   
	}else
	{
		PX_Font(pPX, "Font", InternalGetFont(), &_fontdesc );		
		
		PX_Color(pPX, "TextColor", (OLE_COLOR) m_clrText );		
		PX_Color(pPX, "TextBackColor", (OLE_COLOR) m_clrBackText );		
		PX_Color(pPX, "TransparentColor", (OLE_COLOR) m_clrTransparent );		

		PX_Bool(pPX, "TextAlignLeft", m_bTextAlignLeft);
		PX_Bool(pPX, "TextAlignRight", m_bTextAlignRight);
		PX_Bool(pPX, "TextAlignTop", m_bTextAlignTop);
		PX_Bool(pPX, "TextAlignBottom", m_bTextAlignBottom);

		PX_Bool(pPX, "PictAlignLeft", m_bPictAlignLeft);
		PX_Bool(pPX, "PictAlignRight", m_bPictAlignRight);
		PX_Bool(pPX, "PictAlignTop", m_bPictAlignTop);			
		PX_Bool(pPX, "PictAlignBottom", m_bPictAlignBottom);

		PX_Bool(pPX, "Text2PictAlignLeft", m_bText2PictAlignLeft);
		PX_Bool(pPX, "Text2PictAlignRight", m_bText2PictAlignRight);
		PX_Bool(pPX, "Text2PictAlignTop", m_bText2PictAlignTop);
		PX_Bool(pPX, "Text2PictAlignBottom", m_bText2PictAlignBottom);

		PX_String(pPX, "Text", m_strCaption);
		PX_String(pPX, "PictureFileName", m_strFileName);		
		
		short n = m_nImageIndex;
		PX_Short(pPX, "m_nImageIndex", n );
		 n = m_nImageWidth;
		PX_Short(pPX, "m_nImageWidth", n);
		n = m_nImageHeight;
		PX_Short(pPX, "m_nImageHeight", n);	


	}	
	*/

}


void CVTStaticText::Serialize(CArchive& ar)
{	
	try
	{

	//SerializeExtent(ar);
    SerializeStockProps(ar	);

	if( ar.IsLoading() )
	{
		LONG nLoadVersion;
		ar >> nLoadVersion;

		if( nLoadVersion >= 5 )
		{
			ar >> m_clrText;
			ar >> m_clrBackText;
			ar >> m_clrTransparent;

			ar >> m_bTextAlignLeft;
			ar >> m_bTextAlignRight;
			ar >> m_bTextAlignTop;
			ar >> m_bTextAlignBottom;

			ar >> m_bPictAlignLeft;
			ar >> m_bPictAlignRight;
			ar >> m_bPictAlignTop;
			ar >> m_bPictAlignBottom;
			
			ar >> m_bText2PictAlignLeft;
			ar >> m_bText2PictAlignRight;
			ar >> m_bText2PictAlignTop;
			ar >> m_bText2PictAlignBottom;

			ar >> m_strCaption;
			ar >> m_strFileName;

			ar >> m_bInnerRaised;
			ar >> m_bInnerSunken;
			ar >> m_bOuterRaised;
			ar >> m_bOuterSunken;

			ar >> m_nImageIndex;
			ar >> m_nImageWidth;
			ar >> m_nImageHeight;

	
			m_ImageList.DeleteImageList( );
			m_ImageList.Read( &ar );

			// A.M. fix. BT 3793. Sometimes ImageList_Read damages image list which was read previously
			// so such image list destroys bitmaps from another image list in destructor.
			if( m_nImageWidth * m_nImageHeight )
			{
				CImageList ImageList;
				ImageList.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR16|ILC_MASK, 2, 0);
				int nImageCount = m_ImageList.GetImageCount();
				if (nImageCount > 0)
				{
					for (int i = 0; i < nImageCount; i++)
					{
						HICON hi = m_ImageList.ExtractIcon(i);
						ImageList.Add(hi);
					}
				}
				m_ImageList.DeleteImageList();
				m_ImageList.Attach(ImageList.Detach());
			}

		}

		if( nLoadVersion >= 6 )
		{
			ar >> m_bCentrImage;
		}

		if( nLoadVersion >= 7 )
			ar >> m_bTransparencyText;

		if( nLoadVersion >= 9 )
			ar >> m_clrTransparentChangeToColor;

		if( nLoadVersion >= 10 )
			ar >> m_bUseSystemFont;


		if( nLoadVersion >= 11 )
		{
			ar >> m_bTextAlignCenter;
			ar >> m_bTextAlignMiddle;
		}

	}
	else
	{
		ar << nVersion;
		ar << m_clrText;
		ar << m_clrBackText;
		ar << m_clrTransparent;
		ar << m_bTextAlignLeft;
		ar << m_bTextAlignRight;
		ar << m_bTextAlignTop;
		ar << m_bTextAlignBottom;

		ar << m_bPictAlignLeft;
		ar << m_bPictAlignRight;
		ar << m_bPictAlignTop;
		ar << m_bPictAlignBottom;
		
		ar << m_bText2PictAlignLeft;
		ar << m_bText2PictAlignRight;
		ar << m_bText2PictAlignTop;
		ar << m_bText2PictAlignBottom;

		ar << m_strCaption;
		ar << m_strFileName;

		ar << m_bInnerRaised;
		ar << m_bInnerSunken;
		ar << m_bOuterRaised;
		ar << m_bOuterSunken;

		ar << m_nImageIndex;
		ar << m_nImageWidth;
		ar << m_nImageHeight;
				
		m_ImageList.Write( &ar );

		ar << m_bCentrImage;
		ar << m_bTransparencyText;

		ar << m_clrTransparentChangeToColor;

		ar << m_bUseSystemFont;

		ar << m_bTextAlignCenter;
		ar << m_bTextAlignMiddle;
	}

	InvalidateControl();


	}
	catch(CException* pExc)
	{
		pExc->ReportError( );
		pExc->Delete( );
		OnResetState();
	}
	
}



/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::OnResetState - Reset control to default state

void CVTStaticText::OnResetState()
{
	COleControl::OnResetState();
	ResetStockProps();

	m_font.ReleaseFont();
	m_font.InitializeFont(&_fontdesc);	

 	InitDefaults();
	
}

void CVTStaticText::InitDefaults()
{
	m_clrText				= 0x80000000 + COLOR_WINDOWTEXT;
	m_clrBackText			= 0x80000000 + COLOR_BTNFACE;
	m_clrTransparent		= 0x80000000 + COLOR_BTNFACE;
	
	m_clrTransparentChangeToColor = 
						0x80000000 + COLOR_BTNFACE;


	SetBackColor( 0x80000000 + COLOR_BTNFACE );
	
	m_bTextAlignLeft		= TRUE;
	m_bTextAlignRight		= FALSE;
	m_bTextAlignTop			= TRUE;
	m_bTextAlignBottom		= FALSE;
	m_bTextAlignCenter		= FALSE;
	m_bTextAlignMiddle		= FALSE;

	m_bPictAlignLeft		= TRUE;
	m_bPictAlignRight		= FALSE;
	m_bPictAlignTop			= TRUE;
	m_bPictAlignBottom		= FALSE;

	
	m_bText2PictAlignLeft	= FALSE;
	m_bText2PictAlignRight	= FALSE;
	m_bText2PictAlignTop	= FALSE;
	m_bText2PictAlignBottom	= FALSE;

	m_strCaption			= "Text";
	m_strFileName			= "";	

	m_bInnerRaised = m_bInnerSunken = 
		m_bOuterRaised = m_bOuterSunken = FALSE;


	m_bCentrImage			= TRUE;

	m_bTransparencyText		= TRUE;

	CreateDefaultImageList();

	m_bUseSystemFont = TRUE;

}

void CVTStaticText::CreateDefaultImageList()
{
	m_nImageIndex			= -1;
	m_nImageWidth = m_nImageHeight = 0;

	m_ImageList.DeleteImageList();
	VERIFY( m_ImageList.Create( 1, 1, ILC_COLOR16|ILC_MASK, 1, 0 ) );
}



/////////////////////////////////////////////////////////////////////////////
// CVTStaticText::AboutBox - Display an "About" box to the user

void CVTStaticText::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTSTATICTEXT);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticText message handlers

void CVTStaticText::OnFontChanged() 
{	
	COleControl::OnFontChanged();
}

OLE_COLOR CVTStaticText::GetTextColor() 
{
	return m_clrText;
}

void CVTStaticText::SetTextColor(OLE_COLOR nNewValue) 
{
	m_clrText = nNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

OLE_COLOR CVTStaticText::GetTextBackColor() 
{
	return m_clrBackText;
}

void CVTStaticText::SetTextBackColor(OLE_COLOR nNewValue) 
{
	m_clrBackText = nNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

OLE_COLOR CVTStaticText::GetTransparentColor() 
{
	return m_clrTransparent;
}

void CVTStaticText::SetTransparentColor(OLE_COLOR nNewValue) 
{
	m_clrTransparent = nNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetTextAlignLeft() 
{
	return m_bTextAlignLeft;
}

void CVTStaticText::SetTextAlignLeft(BOOL bNewValue) 
{
	m_bTextAlignLeft =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetTextAlignRight() 
{
	return m_bTextAlignRight;
}

void CVTStaticText::SetTextAlignRight(BOOL bNewValue) 
{
	m_bTextAlignRight =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetTextAlignTop() 
{
	return m_bTextAlignTop;
}

void CVTStaticText::SetTextAlignTop(BOOL bNewValue) 
{
	m_bTextAlignTop =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetTextAlignBottom() 
{
	return m_bTextAlignBottom;
}

void CVTStaticText::SetTextAlignBottom(BOOL bNewValue) 
{
	m_bTextAlignBottom =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}


BOOL CVTStaticText::GetTextAlignCenter() 
{
	return m_bTextAlignCenter;
}

void CVTStaticText::SetTextAlignCenter(BOOL bNewValue) 
{
	m_bTextAlignCenter =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetTextAlignMiddle() 
{
	return m_bTextAlignMiddle;
}

void CVTStaticText::SetTextAlignMiddle(BOOL bNewValue) 
{
	m_bTextAlignMiddle =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetPictAlignLeft() 
{
	return m_bPictAlignLeft;
}

void CVTStaticText::SetPictAlignLeft(BOOL bNewValue) 
{
	m_bPictAlignLeft =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetPictAlignRight() 
{
	return m_bPictAlignRight;
}

void CVTStaticText::SetPictAlignRight(BOOL bNewValue) 
{
	m_bPictAlignRight =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetPictAlignTop() 
{
	return m_bPictAlignTop;
}

void CVTStaticText::SetPictAlignTop(BOOL bNewValue) 
{
	m_bPictAlignTop =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetPictAlignBottom() 
{
	return m_bPictAlignBottom;
}

void CVTStaticText::SetPictAlignBottom(BOOL bNewValue) 
{
	m_bPictAlignBottom =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetText2PictAlignLeft() 
{
	return m_bText2PictAlignLeft;
}

void CVTStaticText::SetText2PictAlignLeft(BOOL bNewValue) 
{
	m_bText2PictAlignLeft =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetText2PictAlignRight() 
{
	return m_bText2PictAlignRight;
}

void CVTStaticText::SetText2PictAlignRight(BOOL bNewValue) 
{
	m_bText2PictAlignRight =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetText2PictAlignTop() 
{
	return m_bText2PictAlignTop;
}

void CVTStaticText::SetText2PictAlignTop(BOOL bNewValue) 
{
	m_bText2PictAlignTop =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetText2PictAlignBottom() 
{
	return m_bText2PictAlignBottom;
}

void CVTStaticText::SetText2PictAlignBottom(BOOL bNewValue) 
{
	m_bText2PictAlignBottom =  bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BSTR CVTStaticText::GetText() 
{
	CString strResult = m_strCaption;
	return strResult.AllocSysString();
}

void CVTStaticText::SetText(LPCTSTR lpszNewValue) 
{
	m_strCaption = lpszNewValue;
	SetModifiedFlag();
	InvalidateControl();
}


BSTR CVTStaticText::GetPictureFileName() 
{
	CString strResult = m_strFileName;
	return strResult.AllocSysString();
}


BOOL CVTStaticText::ReadPictureFromFile()
{
	HBITMAP hBitmap = NULL;
	try
	{	    
		CreateDefaultImageList();		

		if( m_strFileName.IsEmpty() )
			return FALSE;

		hBitmap = NULL;
		hBitmap = (HBITMAP)LoadImage( NULL, m_strFileName, IMAGE_BITMAP, 0, 0,
					 /*LR_CREATEDIBSECTION |*/ LR_DEFAULTSIZE | LR_LOADFROMFILE );

		if( hBitmap )
		{
			CBitmap bmp;
			bmp.Attach (hBitmap);
			BITMAP bitmap;
			bmp.GetBitmap (&bitmap);

			m_nImageWidth = bitmap.bmWidth;
			m_nImageHeight = bitmap.bmHeight;

			m_ImageList.DeleteImageList();

			if( !m_ImageList.Create( m_nImageWidth, m_nImageHeight, ILC_COLOR16|ILC_MASK, 2, 0 ) )
			{
				if( hBitmap ) DeleteObject( hBitmap );

				m_strFileName.Empty( );
				CreateDefaultImageList();
				return FALSE;
			}			

			m_nImageIndex = m_ImageList.Add( &bmp, TranslateColor(m_clrTransparent) );

			DeleteObject( hBitmap );
		}
	}
	catch(CException* pExc)
	{
		pExc->ReportError();
		pExc->Delete();

		if( hBitmap ) DeleteObject( hBitmap );

		m_strFileName.Empty();
		CreateDefaultImageList();		

		return FALSE;

	}

	return TRUE;
}

void CVTStaticText::SetPictureFileName(LPCTSTR lpszNewValue) 
{
	m_strFileName = lpszNewValue;	
	ReadPictureFromFile();

	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::ReadPicture(LPCTSTR pstrFileName) 
{
	SetPictureFileName( pstrFileName );
	return TRUE;
}

void CVTStaticText::OnBackColorChanged() 
{	
	COleControl::OnBackColorChanged();
}

BOOL CVTStaticText::GetInnerRaised() 
{
	return m_bInnerRaised;
}

void CVTStaticText::SetInnerRaised(BOOL bNewValue) 
{
	m_bInnerRaised	= bNewValue;

	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetInnerSunken() 
{
	return m_bInnerSunken;
}

void CVTStaticText::SetInnerSunken(BOOL bNewValue) 
{
	m_bInnerSunken = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetOuterRaised() 
{
	return m_bOuterRaised;
}

void CVTStaticText::SetOuterRaised(BOOL bNewValue) 
{
	m_bOuterRaised = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetOuterSunken() 
{
	return m_bOuterSunken;
}

void CVTStaticText::SetOuterSunken(BOOL bNewValue) 
{
	m_bOuterSunken = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::GetCenterImage() 
{  
	return m_bCentrImage;
}

void CVTStaticText::SetCenterImage(BOOL bNewValue) 
{
	m_bCentrImage = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

BOOL CVTStaticText::SetAutoSize() 
{

	int nAddSize = 0;

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
		nAddSize = 2 * VT_BORDER_WIDTH;
	}
	
	int nControlHeight, nControlWidth;
	
	int nImageWidth		= m_nImageWidth;
	int nImageHeight	= m_nImageHeight;


	int nTextWidth;
	int nTextHeight;

	{
		CClientDC dc(this);

		CFont* pOldFont = SelectStockFont( &dc );

		CRect rcCalc(0,0,0,0);
		dc.DrawText( m_strCaption, &rcCalc , DT_CALCRECT );
		
		nTextWidth = rcCalc.Width();
		nTextHeight = rcCalc.Height();
		

	}

	//////////////////////////
	// At first max
	nControlWidth	= max( nImageWidth, nTextWidth ) + nAddSize;
	nControlHeight	= max( nImageHeight, nTextHeight ) + nAddSize;	
	//////////////////////////
	

	if( m_bText2PictAlignLeft || m_bText2PictAlignRight )
	{
		nControlWidth	= nImageWidth + nTextWidth + nAddSize;
	}

	if( m_bText2PictAlignTop || m_bText2PictAlignBottom )
	{
		nControlHeight	= nImageHeight + nTextHeight + nAddSize;
	}


	SetControlSize( nControlWidth, nControlHeight );
	InvalidateControl( );

	return TRUE;
}

BOOL CVTStaticText::ReloadBitmap() 
{
	ReadPictureFromFile();
	return TRUE;
}

BOOL CVTStaticText::GetTransparencyText() 
{
	return m_bTransparencyText;
}
	
void CVTStaticText::SetTransparencyText(BOOL bNewValue) 
{	
	m_bTransparencyText = bNewValue;	
	InvalidateControl();
	SetModifiedFlag();
}

LRESULT CVTStaticText::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_strCaption, wParam);
		if (wParam > m_strCaption.GetLength())
		wParam = m_strCaption.GetLength();
		return TRUE;    
	}

	if( message == WM_SETTEXT )
	{
		if (lParam == NULL)
		{
			// NULL lParam means set caption to nothing
			m_strCaption.Empty();
		}
		else
		{
			// non-NULL sets caption to that specified by lParam
			lstrcpy(m_strCaption.GetBufferSetLength(lstrlen((LPCTSTR)lParam)),
				(LPCTSTR)lParam);
		}
		
		InvalidateControl();
		SetModifiedFlag();    
		return TRUE;
	}

	return COleControl::DefWindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////////
// function for visible & enable control status & SetFocus
/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTStaticText::GetDisabled()
{
	return !::_IsWindowEnable( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTStaticText::SetDisabled(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( !bNewValue );
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTStaticText::GetVisible()
{
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTStaticText::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	
	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
	
	/*
	UINT uiStyle = ::GetWindowLong( GetSafeHwnd(), GWL_STYLE );
	if( !bNewValue )
		uiStyle &= ~WS_VISIBLE;
	else
		uiStyle |= WS_VISIBLE;

	::SetWindowLong( GetSafeHwnd(), GWL_STYLE, uiStyle );
	*/


}

/////////////////////////////////////////////////////////////////////////////////////
void CVTStaticText::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}



OLE_COLOR CVTStaticText::GetTransparentChangeToColor() 
{
	return m_clrTransparentChangeToColor;
}

void CVTStaticText::SetTransparentChangeToColor(OLE_COLOR nNewValue) 
{
	m_clrTransparentChangeToColor = nNewValue;
	SetModifiedFlag();
	InvalidateControl();
}


BOOL CVTStaticText::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

void CVTStaticText::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}

