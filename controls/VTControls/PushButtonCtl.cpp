// PushButtonCtl.cpp : Implementation of the CPushButtonCtrl ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "PushButtonCtl.h"
#include "PushButtonPpg.h"
#include "FramesPropPage.h"

#include <msstkppg.h>
#include "utils.h"
#include "basecolumn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}


IMPLEMENT_DYNCREATE(CPushButtonCtrl, COleControl)

#ifndef FOR_HOME_WORK
BEGIN_INTERFACE_MAP(CPushButtonCtrl, COleControl)
	INTERFACE_PART(CPushButtonCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CPushButtonCtrl, ActiveXCtrl)
#endif

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPushButtonCtrl, COleControl)
	//{{AFX_MSG_MAP(CPushButtonCtrl)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_MESSAGE(OCM_DRAWITEM, OnOcmDrawItem)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map
	//DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "BMPFileName", m_BMPFileName, OnBMPFileNameChanged, VT_BSTR)
BEGIN_DISPATCH_MAP(CPushButtonCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPushButtonCtrl)
	DISP_PROPERTY_EX(CPushButtonCtrl, "BMPFileName", GetBMPFileName, SetBMPFileName, VT_BSTR)	
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonWidth", m_buttonWidth, OnButtonWidthChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonHeight", m_buttonHeight, OnButtonHeightChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "AutoSize", m_autoSize, OnAutoSizeChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonDepth", m_buttonDepth, OnButtonDepthChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "DrawBorder", m_drawBorder, OnDrawBorderChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FlatButton", m_flatButton, OnFlatButtonChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "DefaultButton", m_defaultButton, OnDefaultButtonChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ThreeStateButton", m_threeStateButton, OnThreeStateButtonChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "MouseInPicColor", m_mouseInPicColor, OnMouseInPicColorChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "MouseOutPicColor", m_mouseOutPicColor, OnMouseOutPicColorChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "MouseOutShadow", m_mouseOutShadow, OnMouseOutShadowChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonPressedPicColor", m_buttonPressedPicColor, OnButtonPressedPicColorChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonPressedShadow", m_buttonPressedShadow, OnButtonPressedShadowChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "MouseInShadow", m_mouseInShadow, OnMouseInShadowChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "DrawFocusRect", m_drawFocusRect, OnDrawFocusRectChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "Stretch", m_stretch, OnStretchChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ButtonText", m_buttonText, OnButtonTextChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "TextAlign", m_textAlign, OnTextAlignChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "OffsetTextFromBMP", m_offsetTextFromBMP, OnOffsetTextFromBMPChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "DrawFigures", m_drawFigures, OnDrawFiguresChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FigureType", m_figureType, OnFigureTypeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FigureSize", m_figureSize, OnFigureSizeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ColorTransparent", m_colorTransparent, OnColorTransparentChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ColorFigure", m_colorFigure, OnColorFigureChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ColorFillFigure", m_colorFillFigure, OnColorFillFigureChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FillFigure", m_fillFigure, OnFillFigureChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "ActiveTextColor", m_activeTextColor, OnActiveTextColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "InactiveTextColor", m_inactiveTextColor, OnInactiveTextColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "InnerRaised", m_innerRaised, OnInnerRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "InnerSunken", m_innerSunken, OnInnerSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "OuterRaised", m_outerRaised, OnOuterRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "OuterSunken", m_outerSunken, OnOuterSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FigureMaxSize", m_figureMaxSize, OnFigureMaxSizeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "FigureBound", m_figureBound, OnFigureBoundChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "AdvancedShadow", m_advancedShadow, OnAdvancedShadowChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPushButtonCtrl, "DarkShadow", m_darkShadow, OnDarkShadowChanged, VT_BOOL)	
	DISP_PROPERTY_EX(CPushButtonCtrl, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_FUNCTION(CPushButtonCtrl, "ResetPressedState", ResetPressedState, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "SetPressedState", SetPressedState, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "IsPressed", IsPressed, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "SetAutoSize", SetAutoSize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "ReloadBitmap", ReloadBitmap, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_EX(CPushButtonCtrl, "ActionName", GetActionName, SetActionName, VT_BSTR)
	DISP_PROPERTY_EX(CPushButtonCtrl, "RunAction", GetRunAction, SetRunAction, VT_BOOL)	
	DISP_PROPERTY_EX(CPushButtonCtrl, "AutoRepeat", GetAutoRepeat, SetAutoRepeat, VT_BOOL)	
	DISP_PROPERTY_EX(CPushButtonCtrl, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CPushButtonCtrl, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CPushButtonCtrl, "DrawFocusedRect", GetDrawFocusedRect, SetDrawFocusedRect, VT_BOOL)
	DISP_PROPERTY_EX(CPushButtonCtrl, "SemiFlat", GetSemiFlat, SetSemiFlat, VT_BOOL)
	DISP_FUNCTION(CPushButtonCtrl, "CopyToClipboard", CopyToClipboard, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "PasteFromClipboard", PasteFromClipboard, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CPushButtonCtrl, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	DISP_PROPERTY_EX(CPushButtonCtrl, "ShowToolTip", GetShowToolTip, SetShowToolTip, VT_BOOL)
	DISP_PROPERTY_EX(CPushButtonCtrl, "ToolTip", GetToolTip, SetToolTip, VT_BSTR)
	DISP_PROPERTY_EX(CPushButtonCtrl, "LayoutTextAlign", GetLayoutTextAlign, SetLayoutTextAlign, VT_I2)
    DISP_FUNCTION(CPushButtonCtrl, "GetButtonLeft", GetButtonLeft, VT_I4, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "SetButtonLeft", SetButtonLeft, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CPushButtonCtrl, "GetButtonTop", GetButtonTop, VT_I4, VTS_NONE)
	DISP_FUNCTION(CPushButtonCtrl, "SetButtonTop", SetButtonTop, VT_EMPTY, VTS_I4)
	DISP_STOCKPROP_FONT()
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CPushButtonCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPushButtonCtrl)
	EVENT_CUSTOM_ID("Click", DISPID_CLICK, FireClick, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPushButtonCtrl, 4)
	PROPPAGEID(CPushButtonPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
	PROPPAGEID(CFramesPropPage::guid)
END_PROPPAGEIDS(CPushButtonCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPushButtonCtrl, "PUSHBUTTON.PushButtonCtrl.1",
	0x54a0bf86, 0x9a92, 0x11d3, 0xb1, 0xc5, 0xeb, 0x19, 0xbd, 0xba, 0xba, 0x39)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CPushButtonCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DPushButton =
		{ 0x54a0bf84, 0x9a92, 0x11d3, { 0xb1, 0xc5, 0xeb, 0x19, 0xbd, 0xba, 0xba, 0x39 } };
const IID BASED_CODE IID_DPushButtonEvents =
		{ 0x54a0bf85, 0x9a92, 0x11d3, { 0xb1, 0xc5, 0xeb, 0x19, 0xbd, 0xba, 0xba, 0x39 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwPushButtonOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKEBUTTON;

IMPLEMENT_OLECTLTYPE(CPushButtonCtrl, IDS_PUSHBUTTON, _dwPushButtonOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::CPushButtonCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPushButtonCtrl

BOOL CPushButtonCtrl::CPushButtonCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_PUSHBUTTON,
			IDB_PUSHBUTTON,
			afxRegApartmentThreading,
			_dwPushButtonOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::CPushButtonCtrl - Constructor

static const FONTDESC _fontdesc =
  { sizeof(FONTDESC), OLESTR("MS Sans Serif"), FONTSIZE( 8 ), FW_THIN, 
     ANSI_CHARSET, FALSE, FALSE, FALSE };

CPushButtonCtrl::CPushButtonCtrl() : m_sizeOriginBitmap(0, 0)/*,
									m_fontHolder( &m_xFontNotification )*/
									
{
	InitializeIIDs(&IID_DPushButton, &IID_DPushButtonEvents);

	m_MouseOnButton = FALSE;
	m_autoSize = FALSE;
	m_buttonDepth = 1;
	m_buttonHeight = 30;
	m_buttonWidth = 30;
	m_BMPFileName = "";	
	m_drawBorder = TRUE;
	m_threeStateButton = TRUE;
	m_defaultButton = FALSE;
	m_mouseInPicColor = TRUE;
	m_mouseOutPicColor = TRUE;
	m_mouseOutShadow = FALSE;
	m_buttonPressedPicColor = TRUE;
	m_buttonPressedShadow = FALSE;
	m_mouseInShadow = FALSE;
	m_hBitmapMono = 0;
	m_hBitmapColor = 0;
	m_hBitmapMonoDither = 0;
	m_hBitmapColorDither = 0;

	m_hBitmapMonoShadow = 0;
	m_hBitmapColorShadow = 0;
	m_hBitmapDisable = 0;
	m_colorTransparent = 0x80000000 + COLOR_WINDOW;

	m_bPushed = FALSE;
	m_strPrevFileName = "";
	m_bDisabled = FALSE;
	m_drawFocusRect = FALSE;
	m_drawFigures = FALSE;
	m_buttonText = "";
	m_stretch  = FALSE;
	m_textAlign = 0; //0 - right //1 - bottom 
	m_offsetTextFromBMP = 5;
	m_figureType = 0; //0 - filled circle, 1 - ring, 2 - square 
	m_figureSize = 5;
	m_colorFillFigure = 0x80000000 + COLOR_WINDOWTEXT;
	m_colorFigure = 0x80000000 + COLOR_WINDOWTEXT;
	m_fillFigure = TRUE;
	m_activeTextColor = 0x80000000 + COLOR_WINDOWTEXT;
	m_inactiveTextColor = 0x80000000 + COLOR_WINDOWTEXT;
	m_innerRaised = FALSE;
	m_innerSunken = FALSE;
	m_outerRaised = FALSE;
	m_outerSunken = FALSE;
	m_figureMaxSize = 20;
	m_advancedShadow = TRUE;
	m_darkShadow = TRUE;	

	m_strActionName	= "";
	m_bRunAction	= FALSE;

	m_flatButton = FALSE;
	m_threeStateButton = FALSE;
	m_buttonText = "Button";

	m_bAutoRepeat = FALSE;

	m_nTimerID = -1;


	m_bSemiFlat = true;

	m_bDrawFocusedRect = FALSE;

	m_bUseSystemFont = TRUE;

	// tooltip - vanek
	m_bShowToolTip = FALSE;
	m_sToolTipText.Empty( );

	// layout text aling
	m_LayoutTextAlign = 1; // default : center	
	m_nMouseTimer = 0;
/*
#ifndef FOR_HOME_WORK
	m_ptrSite = m_ptrApp = NULL;
#endif
*/
	// TODO: Initialize your control's instance data here.

	// vanek : drawing disabled icons - 27.02.2005
	m_imgdrawer.SetEnableDraw( 0 != _GetValueInt( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsEnable, 0 ) );
	m_imgdrawer.SetAlpha( (float)(_GetValueDouble( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsAlpha, 0.5 )) );
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::~CPushButtonCtrl - Destructor

CPushButtonCtrl::~CPushButtonCtrl()
{
	// TODO: Cleanup your control's instance data here.
	_FreeBMPs();
}

void CPushButtonCtrl::_FreeBMPs()
{
	if(m_hBitmapMono)
		::DeleteObject(m_hBitmapMono);
	m_hBitmapMono = 0;

	if(m_hBitmapColor)
		::DeleteObject(m_hBitmapColor);
	m_hBitmapColor = 0;

	if(m_hBitmapMonoDither)
		::DeleteObject(m_hBitmapMonoDither);
	m_hBitmapMonoDither = 0;

	if(m_hBitmapColorDither)
		::DeleteObject(m_hBitmapColorDither);
	m_hBitmapColorDither = 0;
	

	if(m_hBitmapMonoShadow)
		::DeleteObject(m_hBitmapMonoShadow);
	m_hBitmapMonoShadow = 0;

	if(m_hBitmapColorShadow)
		::DeleteObject(m_hBitmapColorShadow);
	m_hBitmapColorShadow = 0;

	if(m_hBitmapDisable)
		::DeleteObject(m_hBitmapDisable);
	m_hBitmapDisable = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::OnDraw - Drawing function

void CPushButtonCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
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
		pdc->DrawEdge(CRect(0,0,rcBounds.Width(),rcBounds.Height()), edgeType, BF_RECT);
	}
	/*if(m_hBitmapMono == 0) return;
	CDC memdc;
	CBitmap bmp;
	CBitmap *poldbmp;
	bmp.Attach (m_hBitmapMono);
	BITMAP bitmap;
	bmp.GetBitmap (&bitmap);
	memdc.CreateCompatibleDC( pdc );
	// Select the bitmap into the DC
	poldbmp = memdc.SelectObject( &bmp);
	// Copy (BitBlt) bitmap from memory DC to screen DC
	pdc->StretchBlt( rcBounds.left,rcBounds.top,
	rcBounds.right-rcBounds.left, rcBounds.bottom -rcBounds.top, &memdc, 0,0, bitmap.bmWidth,bitmap.bmHeight,SRCCOPY );
	memdc.SelectObject( poldbmp );
	bmp.Detach ();
	*/

	if (!IsOptimizedDraw())
	{
		// The container does not support optimized drawing.

		// TODO: if you selected any GDI objects into the device context *pdc,
		//		restore the previously-selected objects here.
		//		For more information, please see MFC technical note #nnn,
		//		"Optimizing an ActiveX Control".
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::DoPropExchange - Persistence support

void CPushButtonCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	//AfxMessageBox("ewrg");
	//PX_Font(pPX, "Font", InternalGetFont());
	///PX_Color(pPX, )
	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD CPushButtonCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::OnResetState - Reset control to default state

void CPushButtonCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	m_font.ReleaseFont();
	m_font.InitializeFont(&_fontdesc);	


	//SetControlSize(m_buttonWidth, m_buttonHeight);
	//InvalidateControl();
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CPushButtonCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("BUTTON");
	//cs.style |= BS_CHECKBOX;
	if(m_defaultButton == TRUE)
		cs.style |= BS_DEFPUSHBUTTON;
	cs.style |= BS_OWNERDRAW;
	//cs.style |= BS_PUSHLIKE|BS_AUTOCHECKBOX;
	//cs.style |= BS_PUSHLIKE|BS_FLAT;

	//_MakeBitmaps();

	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::IsSubclassedControl - This is a subclassed control

BOOL CPushButtonCtrl::IsSubclassedControl()
{
	return TRUE;
}


OLE_COLOR	CPushButtonCtrl::GetActiveFgColor()
{
	return m_activeTextColor;//RGB(0,0,0);
}

OLE_COLOR	CPushButtonCtrl::GetInactiveFgColor()
{
	return m_inactiveTextColor;//RGB(0,0,0);
}


OLE_COLOR CPushButtonCtrl::GetActiveBgColor()
{
	return 0x80000000 + COLOR_BTNFACE;
}

OLE_COLOR	CPushButtonCtrl::GetInactiveBgColor()
{
	return 0x80000000 + COLOR_BTNFACE;
}

/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl::OnOcmCommand - Handle command messages

BOOL CPushButtonCtrl::OnWndMsg(UINT nMessage, WPARAM wParam, LPARAM lParam, LRESULT *plResult )
{
	if( !COleControl::OnWndMsg( nMessage, wParam, lParam, plResult ) )
		return false;

	if( nMessage == OCM_COMMAND )
	{
		*plResult = 0;
		return false;
	}
	return true;
}

LRESULT CPushButtonCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	if( m_bDisabled )
		return 0;
	// TODO: Switch on wNotifyCode here.
	switch (wNotifyCode)
	{
	case BN_DOUBLECLICKED:
	case BN_CLICKED:
		// Fire click event when button is clicked
		if(m_bDisabled == FALSE)
		{
			m_bPushed = !m_bPushed;
			Invalidate();
			FireClick();
			RunAction();
			//GetParent()->SendMessage( WM_COMMAND, GetDlgCtrlID(), 0 );

		}
        break;
	}


	return 0;
}

COLORREF CPushButtonCtrl::GetDitherColor()
{
	COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
	COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
	COLORREF clrDither = RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
		   GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
		   GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

	return clrDither;
}

LRESULT CPushButtonCtrl::OnOcmDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam; // item-drawing information 

 	CDC  *pdrawDC = CDC::FromHandle(lpDIS->hDC);
	CMemDC memDC(pdrawDC);
	CDC  *pDC = &memDC;

	//CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CPen *pOldPen;

	BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);

	// [vanek] BT2000: 3632
	BOOL bIsDisabled = /*(lpDIS->itemState & ODS_DISABLED) ||*/ m_bDisabled;
	
	BOOL	bAnyVisibility = FALSE;
		
	BOOL	bWeHaveEdge = m_innerRaised||m_innerSunken||m_outerRaised||m_outerSunken;
		

	if(m_threeStateButton == TRUE)
		bIsPressed = m_bPushed;//bIsPressed || m_bPushed;
	//BOOL bIsChecked  = (lpDIS->itemState & ODS_CHECKED);
	//TRACE("bIsChecked = %d\n", bIsChecked);

	CRect itemRect = lpDIS->rcItem;
	
	if (m_flatButton == FALSE)
	{
	    if ((bIsFocused || (m_defaultButton == TRUE)) && bIsDisabled==FALSE)
		{
			if( m_bDrawFocusedRect )
			{
				CBrush br(RGB(0,0,0));  
				pDC->FrameRect(&itemRect, &br);
				itemRect.DeflateRect(1, 1);
				bAnyVisibility = TRUE;
			}
		}
	}

  // Prepare draw... paint button's area with background color
	COLORREF bgColor;
	if ((m_MouseOnButton == TRUE) || (bIsPressed))
		bgColor = TranslateColor( GetActiveBgColor() );
	else
		bgColor = TranslateColor( GetInactiveBgColor() );

	CBrush br(bgColor);
	// Draw transparent?
	//if (m_bDrawTransparent == TRUE)
	//{
	//	PaintBk(pDC);
	//}
	//else
	//{
		
	//}
	
	
	if( m_bSemiFlat && !m_MouseOnButton && bIsPressed && !bIsDisabled )
	{
		/*
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black
		*/

		pDC->FillRect(&itemRect, &CBrush( GetDitherColor() ) );


		/*
		pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );		
		pDC->FillRect(&itemRect, &CBrush( ::GetSysColor(COLOR_HIGHLIGHT) ) );
		*/
	}
	else
	{
		pDC->FillRect(&itemRect, &br);	
	}
	

	

	if(bWeHaveEdge == TRUE)
		itemRect.DeflateRect(3,3,3,3);
	

	// Draw pressed button
	if (bIsPressed==TRUE && bIsDisabled==FALSE)
	{
	    if (m_flatButton == TRUE)
	    {
			if (m_drawBorder == TRUE)
			{
				CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); 
				CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   

					
				
				// Dark gray line
				pOldPen = pDC->SelectObject(&penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				
				
				// White line
				pDC->SelectObject( &penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);

				bAnyVisibility = TRUE;
			}
		}
		else    
		{

			if( !m_bSemiFlat )
			{
				pDC->DrawFrameControl(&itemRect, DFC_BUTTON, DFCS_PUSHED|DFCS_BUTTONPUSH);
			}
			else
			{
				CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White			
				CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray			

				

  				// Dark gray line
				pOldPen = pDC->SelectObject(&penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				
				// White line
				pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
				bAnyVisibility = FALSE;
			}
		
			bAnyVisibility = TRUE;
		}
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_flatButton == TRUE)
		{
			if (m_MouseOnButton == TRUE && m_drawBorder == TRUE && bIsDisabled == FALSE)
			{
  				// White line
				pOldPen = pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
		        // Dark gray line
		        pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);				
			}
		}
		else
		{
			// White line
			pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.left, itemRect.top);
			pDC->LineTo(itemRect.right, itemRect.top);
			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(itemRect.left+1, itemRect.bottom-1);
			pDC->LineTo(itemRect.left+1, itemRect.top+1);
			pDC->LineTo(itemRect.right, itemRect.top+1);
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.top-1);
			
			if( !m_bSemiFlat )
			{
				// Dark gray line
				pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left+1, itemRect.bottom-2);
				pDC->LineTo(itemRect.right-2, itemRect.bottom-2);
				pDC->LineTo(itemRect.right-2, itemRect.top);			
			}

			pDC->SelectObject(pOldPen);
			bAnyVisibility = TRUE;
		}
	}


	if( m_bSemiFlat && m_MouseOnButton && !bIsPressed && !bIsDisabled )
	{
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		// Dark gray line
		CPen* pPrevPen = (CPen*)pDC->SelectObject( &penBtnShadow);
		pDC->MoveTo(itemRect.left, itemRect.bottom-2);
		pDC->LineTo(itemRect.right-2, itemRect.bottom-2);
		pDC->LineTo(itemRect.right-2, itemRect.top);
		pDC->SelectObject( pPrevPen );
		
	}
	
	
	// Draw the bitmap
	//DrawTheBMP(pDC->GetSafeHdc(), lpDIS->rcItem, bIsPressed, bIsDisabled);
	CRect rcItem = itemRect;
	//rcItem.DeflateRect(3,3,3,3);
	if (bIsPressed && !bIsDisabled) rcItem.OffsetRect(1, 1);
	HBITMAP hDrawBitmapNow = 0;

	rcItem.DeflateRect(3,3,3,3);

	//pDC->MoveTo(rcItem.BottomRight());
	//pDC->LineTo(rcItem.TopLeft());
	

	if(m_drawFigures == FALSE)
	{
	if(!bIsDisabled)
	{
		if( m_bSemiFlat && bIsPressed && !m_MouseOnButton )
		{
			if(m_buttonPressedPicColor == TRUE)
				hDrawBitmapNow = m_hBitmapColorDither;
			else
				hDrawBitmapNow = m_hBitmapMonoDither;
		}
		else
		if(bIsPressed)
		{
			if(m_buttonPressedPicColor == TRUE)
				hDrawBitmapNow = m_buttonPressedShadow==TRUE?m_hBitmapColorShadow:m_hBitmapColor;
			else
				hDrawBitmapNow = m_buttonPressedShadow==TRUE?m_hBitmapMonoShadow:m_hBitmapMono;
		}
		else if(m_MouseOnButton == TRUE)
		{
			if(m_mouseInPicColor == TRUE)
				hDrawBitmapNow = m_mouseInShadow==TRUE?m_hBitmapColorShadow:m_hBitmapColor;
			else
				hDrawBitmapNow = m_mouseInShadow==TRUE?m_hBitmapMonoShadow:m_hBitmapMono;
		}
		else
		{
			if(m_mouseOutPicColor == TRUE)
				hDrawBitmapNow = m_mouseOutShadow==TRUE?m_hBitmapColorShadow:m_hBitmapColor;
			else
				hDrawBitmapNow = m_mouseOutShadow==TRUE?m_hBitmapMonoShadow:m_hBitmapMono;
		}
	}
	else
		hDrawBitmapNow = m_hBitmapDisable;
	
	// vanek - 01.09.2003
	CRect rcBitmap;
	rcBitmap.SetRectEmpty();
	rcBitmap = rcItem;
	//

    if(hDrawBitmapNow != 0) 
	{
		CDC memdc;
		CBitmap bmp;
		CBitmap *poldbmp;
		bmp.Attach (hDrawBitmapNow);
		BITMAP bitmap;
		bmp.GetBitmap (&bitmap);
		memdc.CreateCompatibleDC( pDC );
		poldbmp = memdc.SelectObject( &bmp);

		if(m_stretch == TRUE)
		{
			// vanek : drawing disabled icons - 27.02.2005
			bool bdrawn_disabled = false;
			if( hDrawBitmapNow == m_hBitmapDisable )
				bdrawn_disabled = m_imgdrawer.DrawImage( pDC->m_hDC, m_hBitmapColor, rcItem, (idfScalingImage | idfUseAlpha));
			if( !bdrawn_disabled )
				pDC->StretchBlt( rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), &memdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		}
		else
		{
			int nOffsetX = max(0, (rcItem.Width() - bitmap.bmWidth)/2);
			int nOffsetY = max(0, (rcItem.Height() - bitmap.bmHeight)/2);
			if (!m_buttonText.IsEmpty())
			{
				if(m_textAlign == 0)
				{
					nOffsetX = 0;
					//nOffsetY = 0;
				}
				else if(m_textAlign == 1)
					nOffsetY = 0;
				// vanek - 01.09.2003
				else if(m_textAlign == 2)
					nOffsetX *= 2;
				// 
			}

			// vanek
			rcBitmap.right = rcBitmap.left + bitmap.bmWidth;
			rcBitmap.bottom = rcBitmap.top + bitmap.bmHeight;
			rcBitmap.OffsetRect( nOffsetX, nOffsetY );
			//

			// vanek : drawing disabled icons - 27.02.2005
			bool bdrawn_disabled = false;
			if( hDrawBitmapNow == m_hBitmapDisable )
				bdrawn_disabled = m_imgdrawer.DrawImage( pDC->m_hDC, m_hBitmapColor, rcBitmap, idfUseAlpha);
			if( !bdrawn_disabled )
				pDC->BitBlt( rcBitmap.left, rcBitmap.top, rcBitmap.Width(), rcBitmap.Height(), &memdc, 0, 0, SRCCOPY);
		}
		memdc.SelectObject( poldbmp );
		bmp.Detach ();
		bAnyVisibility = TRUE;
	}


	if (!m_buttonText.IsEmpty())
	{
		CRect captionRect = itemRect;
		
		if (bIsPressed)
			captionRect.OffsetRect(1, 1);
    
		//CFont *pCurrentFont = GetFont(); 
		//CFont *pOldFont = pDC->SelectObject(pCurrentFont);

		CFont font;
		if( ::GetFontFromDispatch( this, font, GetUseSystemFont() ) )
		{
			CFont* pOldFont = (CFont*)pDC->SelectObject( &font );//SelectStockFont( pDC );

			// create multiline string
			CString stMultiString = m_buttonText;
			stMultiString.Replace( "\\n", "\r\n" );

			LOGFONT lf;
			font.GetLogFont( &lf );
			if ((m_MouseOnButton == TRUE) || (bIsPressed)) 
			{
				pDC->SetTextColor( TranslateColor( GetActiveFgColor() ) );
				pDC->SetBkColor( TranslateColor( GetActiveBgColor() ) );
			} 
			else 
			{
				pDC->SetTextColor( TranslateColor( GetInactiveFgColor() ) );
				pDC->SetBkColor( TranslateColor( GetInactiveBgColor() ) );
			}

			// Align text
			CRect centerRect = captionRect;
			pDC->DrawText(stMultiString, -1, captionRect, DT_CALCRECT );
			
			int nOffsetX = 0;
			int nOffsetY = 0;
			switch(m_textAlign)
			{
			case 0://right
				nOffsetX = m_sizeOriginBitmap.cx + m_offsetTextFromBMP;
				nOffsetY = (centerRect.Height() - captionRect.Height())/2;
				break;
			case 1://bottom
				nOffsetX = 0;
				nOffsetY = m_sizeOriginBitmap.cy + m_offsetTextFromBMP;
				break;
			case 2:// left - vanek 01.09.2003
				nOffsetX = -1 * ( max( 0, captionRect.right - rcBitmap.left + m_offsetTextFromBMP) );
				nOffsetY = (centerRect.Height() - captionRect.Height())/2;
                break;
			}

			if(m_sizeOriginBitmap == CSize(0, 0) || m_stretch == TRUE)
			{
				captionRect.OffsetRect( 0, (centerRect.Height() - captionRect.Height())/2);
				captionRect.right = centerRect.right;
			}
			else
			{
				captionRect.OffsetRect(nOffsetX, nOffsetY);
				captionRect.right =	 m_textAlign == 2 ? rcBitmap.left - m_offsetTextFromBMP : centerRect.right;
			}

			// vanek
			captionRect.DeflateRect( 1, 0); 
			


			pDC->SetBkMode(TRANSPARENT);
			if( bIsDisabled )
			{
				COLORREF crOldTextColor = pDC->SetTextColor( ::GetSysColor( COLOR_3DHILIGHT ) );
				pDC->DrawText( stMultiString, captionRect, getDTLayoutTextAlign());
				RECT rtShifting = captionRect;
				::OffsetRect( &rtShifting, -1, -1 );
				pDC->SetTextColor( ::GetSysColor( COLOR_GRAYTEXT ) );
				pDC->DrawText( stMultiString, &rtShifting, getDTLayoutTextAlign());
                pDC->SetTextColor( crOldTextColor );
			}
			else
				pDC->DrawText( stMultiString, captionRect, getDTLayoutTextAlign() );


			pDC->SelectObject(pOldFont);
			bAnyVisibility = TRUE;			
		}
	} 
	
	}
	else
	{
		//draw figures
		//m_figureType == 0 filled circle
		//m_figureType == 1 ring
		//m_figureType == 2 square rect
		CRect rcBounds = itemRect;
		
		if (bIsPressed)
			rcBounds.OffsetRect(1, 1);
		CPen* pOldPen = 0;
		CBrush* pOldBrush = 0;
		CPen pen;
		CBrush brush;
		CRect rcFigure;
		switch(m_figureType)
		{
		case 0:
			pen.CreatePen(PS_SOLID, 0, TranslateColor( m_colorFigure ) );
			brush.CreateSolidBrush( TranslateColor( m_colorFigure ) );
			pOldPen = pDC->SelectObject(&pen);
			pOldBrush = pDC->SelectObject(&brush);
			{
			CPoint ptCenter = rcBounds.CenterPoint();
			rcFigure.left = ptCenter.x - (m_figureSize+1)/2;
			rcFigure.top = ptCenter.y - (m_figureSize+1)/2;
			}
			rcFigure.right = rcFigure.left + (m_figureSize+1);
			rcFigure.bottom = rcFigure.top + (m_figureSize+1);
			pDC->Ellipse(&rcFigure);
			bAnyVisibility = TRUE;
			break;
		case 1:
			pen.CreatePen(PS_SOLID, m_figureSize, TranslateColor( m_colorFigure ) );
			brush.CreateSolidBrush(m_fillFigure==TRUE ? TranslateColor( m_colorFillFigure ) : ::GetSysColor(COLOR_BTNFACE));
			pOldPen = pDC->SelectObject(&pen);
			pOldBrush = pDC->SelectObject(&brush);
			//rcBounds.DeflateRect(2+m_figureSize,2+m_figureSize,2+m_figureSize,2+m_figureSize);
			rcBounds.DeflateRect(m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2);
			//rcBounds.DeflateRect(5,5,5,5);
			pDC->Ellipse(&rcBounds);
			bAnyVisibility = TRUE;
			break;
		case 2:
			pen.CreatePen(PS_SOLID, m_figureSize, TranslateColor( m_colorFigure ) );
			brush.CreateSolidBrush(m_fillFigure==TRUE ? TranslateColor( m_colorFillFigure ): ::GetSysColor(COLOR_BTNFACE));
			pOldPen = pDC->SelectObject(&pen);
			pOldBrush = pDC->SelectObject(&brush);
			//rcBounds.DeflateRect(2+m_figureSize,2+m_figureSize,2+m_figureSize,2+m_figureSize);
			//rcBounds.DeflateRect(5,5,5,5);
			rcBounds.DeflateRect(m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2,m_figureBound+m_figureSize/2);
			pDC->Rectangle(&rcBounds);
			bAnyVisibility = TRUE;
			break;
		}
		
		if(pOldPen != 0)
			pDC->SelectObject(pOldPen);
		if(pOldBrush != 0)
			pDC->SelectObject(pOldBrush);
	}
	if (m_drawFocusRect == TRUE)
	{
	    // Draw the focus rect
	    if (bIsFocused)
	    {
			CRect focusRect = itemRect;
			focusRect.DeflateRect(3, 3);
			if( m_bDrawFocusedRect )
				pDC->DrawFocusRect(&focusRect);
		}
	}


	if(bAnyVisibility != TRUE)
	{
		if( m_bDrawFocusedRect )
			pDC->FrameRect(&itemRect, &CBrush(RGB(0,0,0)));
	}
	return 0;
}


void	CPushButtonCtrl::_CreateToolTip( )
{
	if( !m_ToolTip.GetSafeHwnd() )
		return;
    
	// Add a tool
    CRect rect;
    GetClientRect( rect);
    m_ToolTip.AddTool( this, m_sToolTipText, rect, 1 );
	m_ToolTip.Activate( m_bShowToolTip );
}

void	CPushButtonCtrl::_ReCreateToolTip( )
{
	if( !m_ToolTip.GetSafeHwnd() )
		return;

	m_ToolTip.DelTool( this, 1 );
	_CreateToolTip( );		
}

UINT	CPushButtonCtrl::getDTLayoutTextAlign()
{
	if( m_sizeOriginBitmap == CSize(0, 0) && (m_offsetTextFromBMP == 0) )
		return	DT_LEFT;

	switch( m_LayoutTextAlign )
	{
	case 0:
		return DT_LEFT;
	case 1:
		return DT_CENTER;
	case 2:
		return DT_RIGHT;
	default:
		return DT_LEFT;
	}
}


void	CPushButtonCtrl::_MakeBitmaps( )
{
	try
	{
		
		m_sizeOriginBitmap = CSize(0, 0);

		_FreeBMPs();

		if(!m_DIB.IsValid())
		{
			//AfxMessageBox("Not valid BMP!");
			//InvalidateControl();
			return;
		}

		if(m_drawFigures == TRUE) 
		{
			return;
		}

		CClientDC dc(0);

		
		BOOL bDoColor = m_mouseInPicColor||m_mouseOutPicColor||m_buttonPressedPicColor;
		BOOL bDoMono = (!m_mouseInPicColor)||(!m_mouseOutPicColor)||(!m_buttonPressedPicColor);
		BOOL bDoColorS = bDoColor&&(m_mouseInShadow||m_mouseOutShadow||m_buttonPressedShadow);
		BOOL bDoMonoS = bDoMono&&((!m_mouseInPicColor)||(!m_mouseOutPicColor)||(!m_buttonPressedPicColor));
		
		m_hBitmapColor = m_DIB.CreateBitmap(&dc); 

		m_hBitmapColorDither = m_DIB.CreateBitmap(&dc); 
		if(bDoMono)
			m_hBitmapMonoDither = m_DIB.CreateBitmap(&dc);

		if(bDoMono)
			m_hBitmapMono = m_DIB.CreateBitmap(&dc); 
		if(bDoColorS)
			m_hBitmapColorShadow = m_DIB.CreateBitmap(&dc); 
		if(bDoMonoS)
			m_hBitmapMonoShadow = m_DIB.CreateBitmap(&dc); 

		m_hBitmapDisable = m_DIB.CreateBitmap(&dc); 
		//m_hBitmapColor = (HBITMAP)::LoadImage(0, m_BMPFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//m_hBitmapMono = (HBITMAP)::LoadImage(0, m_BMPFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//m_hBitmapDisable = (HBITMAP)::LoadImage(0, m_BMPFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					
		if(!m_hBitmapColor || !m_hBitmapColorDither )return;

		BITMAP bmp;
		//ZeroMemory(&bmp, sizeof(BITMAP));

		CBitmap* pBitmap = CBitmap::FromHandle(m_hBitmapColor);
		VERIFY(pBitmap != 0);
		VERIFY(pBitmap->GetBitmap(&bmp) != 0);

		CBitmap* pBitmapDither = CBitmap::FromHandle(m_hBitmapColorDither);
		VERIFY(pBitmapDither != 0);
		VERIFY(pBitmapDither->GetBitmap(&bmp) != 0);

		m_sizeOriginBitmap = CSize(bmp.bmWidth, bmp.bmHeight);
		
		CDC dcColor;
		CDC dcColorDither;
		CDC dcMonoDither;
		CDC dcMono;
		CDC dcColorShadow;
		CDC dcMonoShadow;
		CDC dcDisable;

		VERIFY(dcColor.CreateCompatibleDC(&dc) == TRUE);
		
		VERIFY(dcColorDither.CreateCompatibleDC(&dc) == TRUE);
		if(bDoMono)
			VERIFY(dcMonoDither.CreateCompatibleDC(&dc) == TRUE);

		if(bDoMono)
			VERIFY(dcMono.CreateCompatibleDC(&dc) == TRUE);
		if(bDoColorS)
			VERIFY(dcColorShadow.CreateCompatibleDC(&dc) == TRUE);
		if(bDoMonoS)
			VERIFY(dcMonoShadow.CreateCompatibleDC(&dc) == TRUE);
		VERIFY(dcDisable.CreateCompatibleDC(&dc) == TRUE);
		
		CBitmap* pOldBitmapColor = dcColor.SelectObject(CBitmap::FromHandle(m_hBitmapColor));
		VERIFY(pOldBitmapColor != 0);

		CBitmap* pOldBitmapColorDither = dcColorDither.SelectObject(CBitmap::FromHandle(m_hBitmapColorDither));
		VERIFY(pOldBitmapColorDither != 0);

		CBitmap* pOldBitmapMonoDither;
		if(bDoMono)
		{
			pOldBitmapMonoDither = dcMonoDither.SelectObject(CBitmap::FromHandle(m_hBitmapMonoDither));
			VERIFY(pOldBitmapMonoDither!= 0);
		}

		CBitmap* pOldBitmapMono;
		CBitmap* pOldBitmapColorShadow;
		CBitmap* pOldBitmapMonoShadow;

		if(bDoMono)
		{
			pOldBitmapMono = dcMono.SelectObject(CBitmap::FromHandle(m_hBitmapMono));
			VERIFY(pOldBitmapMono != 0);
		}
		if(bDoColorS)
		{
			pOldBitmapColorShadow = dcColorShadow.SelectObject(CBitmap::FromHandle(m_hBitmapColorShadow));
			VERIFY(pOldBitmapColorShadow != 0);
		}
		if(bDoMonoS)
		{
			pOldBitmapMonoShadow = dcMonoShadow.SelectObject(CBitmap::FromHandle(m_hBitmapMonoShadow));
			VERIFY(pOldBitmapMonoShadow != 0);
		}

		CBitmap* pOldBitmapDisable = dcDisable.SelectObject(CBitmap::FromHandle(m_hBitmapDisable));
		VERIFY(pOldBitmapDisable != 0);

		COLORREF	pixColor;
		COLORREF	fillTransparent = ::GetSysColor(COLOR_BTNFACE);
		COLORREF	disableShadow = ::GetSysColor(COLOR_BTNHILIGHT);
		COLORREF	btnShadow = ::GetSysColor(m_darkShadow==TRUE?COLOR_3DDKSHADOW:COLOR_BTNSHADOW);
		long i;
		long j;
		/*for(long i = 0; i < m_sizeOriginBitmap.cx; i++)
		{
			for(long j = 0; j < m_sizeOriginBitmap.cy; j++)
			{
					dcColor.SetPixel(i, j, fillTransparent);
					dcMono.SetPixel(i, j, fillTransparent);
		
		*/

		COLORREF clrDither = GetDitherColor();
		int nDitherBr = Bright(GetRValue(clrDither),GetGValue(clrDither),GetBValue(clrDither));
		COLORREF clrMonoDither = RGB(nDitherBr,nDitherBr,nDitherBr);


		if(bDoColorS)
			dcColorShadow.FillRect(CRect(0,0, m_sizeOriginBitmap.cx, m_sizeOriginBitmap.cy), &CBrush(fillTransparent));
		if(bDoMonoS)
			dcMonoShadow.FillRect(CRect(0,0, m_sizeOriginBitmap.cx, m_sizeOriginBitmap.cy), &CBrush(fillTransparent));
		bool bDrawShadow = false;
		for(i = 0; i < m_sizeOriginBitmap.cx; i++)
		{
			for(j = 0; j < m_sizeOriginBitmap.cy; j++)
			{
				pixColor = dcColorDither.GetPixel(i, j);
				
				if( pixColor == TranslateColor( m_colorTransparent ) )
				{
					dcColorDither.SetPixel(i, j, clrDither);
					if (bDoMono)
						dcMonoDither.SetPixel(i, j, clrMonoDither);
				}
				else if (bDoMono)
				{
					int nBright = Bright(GetRValue(pixColor),GetGValue(pixColor),GetBValue(pixColor));
					dcMonoDither.SetPixel(i, j, RGB(nBright, nBright, nBright));
				}
				

				if(pixColor == (TranslateColor( m_colorTransparent ) ) )
				{
					if(!(bDoColorS || bDoMonoS))
						dcColor.SetPixel(i, j, fillTransparent);
					if(bDoMono)
						dcMono.SetPixel(i, j, fillTransparent);
				}
				else
				{
					if(bDoMono)
					{
						long nBright = (GetRValue(pixColor)*60+GetGValue(pixColor)*118+GetBValue(pixColor)*22)/200;
						dcMono.SetPixel(i, j, RGB(nBright, nBright, nBright));				
					}
					if(m_advancedShadow == TRUE)
					{
						bDrawShadow = !bDrawShadow;
					}
					else
						bDrawShadow = true;
					if(bDrawShadow)
					{
						if(bDoMonoS)
							dcMonoShadow.SetPixel(i+m_buttonDepth, j+m_buttonDepth, btnShadow);
						if(bDoColorS)
							dcColorShadow.SetPixel(i+m_buttonDepth, j+m_buttonDepth, btnShadow);
					}						
				}
				pixColor = dcDisable.GetPixel(i, j);
				if(pixColor == TranslateColor( m_colorTransparent ) )
				{
					dcDisable.SetPixel(i, j, fillTransparent);
				}
				else
				{
					if(pixColor != disableShadow)
					{
						dcDisable.SetPixel(i, j, ::GetSysColor(COLOR_3DSHADOW));
						//!!! shift disabled button
						pixColor = dcDisable.GetPixel(i+1, j+1);
						if(pixColor == TranslateColor( m_colorTransparent ) || pixColor == fillTransparent )// || pixColor == ::GetSysColor(COLOR_3DSHADOW))
						{
							dcDisable.SetPixel(i+1, j+1, disableShadow);
						}
					}
				}
				/*pixColor = dcColor.GetPixel(i, j);
				if(pixColor == m_ColorTransparent)
				{
					dcColorShadow.SetPixel(i, j, fillTransparent);
					dcMonoShadow.SetPixel(i, j, fillTransparent);
				}
				else
				{
					if(pixColor != darkShadow)
					{
						long nBright = (GetRValue(pixColor)*60+GetGValue(pixColor)*118+GetBValue(pixColor)*22)/200;
						dcMonoShadow.SetPixel(i, j, RGB(nBright, nBright, nBright));
						//shift shadow
						pixColor = dcColorShadow.GetPixel(i+m_buttonDepth, j+m_buttonDepth);
						if(pixColor == m_ColorTransparent || pixColor == fillTransparent )
						{
							dcColorShadow.SetPixel(i+m_buttonDepth, j+m_buttonDepth, darkShadow);
							dcMonoShadow.SetPixel(i+m_buttonDepth, j+m_buttonDepth, darkShadow);
						}
					}
				}*/
			}
		}

		if(bDoColorS || bDoMonoS)
		for(i = 0; i < m_sizeOriginBitmap.cx; i++)
		{
			for(j = 0; j < m_sizeOriginBitmap.cy; j++)
			{
				pixColor = dcColor.GetPixel(i, j);
				if(pixColor == TranslateColor( m_colorTransparent ) )
				{
					dcColor.SetPixel(i, j, fillTransparent);
				}
				else
				{
					if(bDoMonoS)
						dcMonoShadow.SetPixel(i, j, pixColor);
					if(bDoColorS)
						dcColorShadow.SetPixel(i, j, pixColor);
				}
			}
		}
		if (bDoMono)
		for(i = 0; i < m_sizeOriginBitmap.cx; i++)
		{
			for(j = 0; j < m_sizeOriginBitmap.cy; j++)
			{
				COLORREF pixColor1 = dcMonoDither.GetPixel(i, j);
				pixColor1 = pixColor1;
			}
		}
		
		m_hBitmapColor = (HBITMAP)*dcColor.SelectObject(pOldBitmapColor);
		VERIFY(m_hBitmapColor != 0);

		m_hBitmapColorDither = (HBITMAP)*dcColorDither.SelectObject(pOldBitmapColorDither);
		VERIFY(m_hBitmapColorDither != 0);


		if(bDoMono)
		{
			m_hBitmapMonoDither = (HBITMAP)*dcMonoDither.SelectObject(pOldBitmapMonoDither);
			VERIFY(m_hBitmapMonoDither != 0);
			m_hBitmapMono = (HBITMAP)*dcMono.SelectObject(pOldBitmapMono);
			VERIFY(m_hBitmapMono != 0);
		}
		m_hBitmapDisable = (HBITMAP)*dcDisable.SelectObject(pOldBitmapDisable);
		VERIFY(m_hBitmapDisable != 0);
		if(bDoColorS)
		{
			m_hBitmapColorShadow = (HBITMAP)*dcColorShadow.SelectObject(pOldBitmapColorShadow);
			VERIFY(m_hBitmapColorShadow != 0);
		}
		if(bDoMonoS)
		{
			m_hBitmapMonoShadow = (HBITMAP)*dcMonoShadow.SelectObject(pOldBitmapMonoShadow);
			VERIFY(m_hBitmapMonoShadow != 0);
		}

		/*
		if(m_autoSize == TRUE)
		{
			m_buttonWidth = (short)m_sizeOriginBitmap.cx + 4;
			m_buttonHeight = (short)m_sizeOriginBitmap.cy + 4;
			SetControlSize(m_buttonWidth, m_buttonHeight);
		}
		*/
		InvalidateControl();
	}
	catch(CException *pe)
	{
		m_DIB.Free();
		_FreeBMPs();
		pe->ReportError();
		pe->Delete();
	}


}
/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl message handlers
/*
void CPushButtonCtrl::OnBMPFileNameChanged() 
{
	// TODO: Add notification handler code
	try
	{
		if(!m_BMPFileName.IsEmpty())// && m_strPrevFileName != m_BMPFileName)
		{
			//if(m_DIB.AttachMapFile(m_BMPFileName, TRUE) == FALSE) return;
			{
			CFile file(m_BMPFileName, CFile::modeRead);
			m_DIB.Read(file);
			}
			m_strPrevFileName = m_BMPFileName;

			//AfxMessageBox("OnBMPFileNameChanged()");
			_MakeBitmaps();
		}
		else
		{
			_MakeBitmaps();
		}

		
		SetModifiedFlag();
	}
	catch(CException *pe)
	{
		m_DIB.Free();
		_FreeBMPs();
		pe->ReportError();
		pe->Delete();
	}
}
*/
BSTR CPushButtonCtrl::GetBMPFileName() 
{
	CString strResult = m_BMPFileName;	
	return strResult.AllocSysString();
}

void CPushButtonCtrl::SetBMPFileName(LPCTSTR lpszNewValue) 
{

	m_BMPFileName = lpszNewValue;

	if( m_BMPFileName.IsEmpty() )
	{
		m_DIB.Free();
		_FreeBMPs();
		SetModifiedFlag();
		return;
	}

	try
	{
		if(!m_BMPFileName.IsEmpty())// && m_strPrevFileName != m_BMPFileName)
		{
			//if(m_DIB.AttachMapFile(m_BMPFileName, TRUE) == FALSE) return;
			{
			CFile file(m_BMPFileName, CFile::modeRead|CFile::shareDenyNone);
			m_DIB.Read(file);
			}
			m_strPrevFileName = m_BMPFileName;

			//AfxMessageBox("OnBMPFileNameChanged()");
			_MakeBitmaps();
		}
		else
		{
			_MakeBitmaps();
		}

		
		SetModifiedFlag();
	}
	catch(CException *pe)
	{
		m_DIB.Free();
		_FreeBMPs();
		pe->ReportError();
		pe->Delete();
	}
}


void CPushButtonCtrl::CopyToClipboard()
{
	if( !m_DIB.IsValid() )
		return;

	::OpenClipboard( GetSafeHwnd() );
	::EmptyClipboard();

	HGLOBAL hGlobal = 0;
	m_DIB.CopyToClipboard( &hGlobal );
	 ::SetClipboardData( CF_DIB, hGlobal );
	::CloseClipboard();

}

void CPushButtonCtrl::PasteFromClipboard()
{
	if( !::IsClipboardFormatAvailable( CF_DIB ) )
		return;

	::OpenClipboard( GetSafeHwnd() );
	HANDLE hGlobal = ::GetClipboardData( CF_DIB );
	if( !hGlobal )
		return;

	m_BMPFileName.Empty();
	m_DIB.Free();
	_FreeBMPs();
	SetModifiedFlag();

	
	if( m_DIB.CreateFromClipboard( hGlobal ) )
		_MakeBitmaps();

	::CloseClipboard();
}


void CPushButtonCtrl::OnButtonWidthChanged() 
{
	// TODO: Add notification handler code
	//CRect	rc;
	//GetClientRect(&rc);
	//rc.right = rc.left + m_buttonWidth;
	//MoveWindow(&rc);
	SetControlSize(m_buttonWidth, m_buttonHeight);
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnButtonHeightChanged() 
{
	// TODO: Add notification handler code
	//CRect	rc;
	//GetClientRect(&rc);
	//rc.bottom = rc.top + m_buttonHeight;
	//MoveWindow(&rc);
	SetControlSize(m_buttonWidth, m_buttonHeight);
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnAutoSizeChanged() 
{
	/*
	// TODO: Add notification handler code
	if(m_autoSize == TRUE)
	{
		m_buttonWidth = (short)m_sizeOriginBitmap.cx + 4;
		m_buttonHeight = (short)m_sizeOriginBitmap.cy + 4;
		SetControlSize(m_buttonWidth, m_buttonHeight);
		InvalidateControl();
	}

	SetModifiedFlag();
	*/
}

void CPushButtonCtrl::OnButtonDepthChanged() 
{
	// TODO: Add notification handler code
	if(m_buttonDepth < 0)
		m_buttonDepth = 0;
	if(m_buttonDepth > 4)
		m_buttonDepth = 4;
	//OnBMPFileNameChanged();
	_MakeBitmaps();
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnDrawBorderChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnFlatButtonChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

int CPushButtonCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*m_strPrevFileName = "";	
	if( (CWnd*)m_pReflect )
		((CWnd*)m_pReflect)->DestroyWindow();*/
	/*
	CFontHolder fontHolder =  InternalGetFont( );

	LPFONTDISP lpFontDisp = NULL;
	lpFontDisp = fontHolder.GetFontDispatch( );
	if( lpFontDisp )
	{
		fontHolder.ReleaseFont( );
		fontHolder.InitializeFont( &_fontdesc, lpFontDisp );
	}
	*/
	
	

	// TODO: Add your specialized creation code here

	// vanek
	if( GetSafeHwnd() )
	{
		m_ToolTip.Create( 0, TTS_ALWAYSTIP);	// create tooltip for desktop window
		if(m_bShowToolTip)
		{
			_ReCreateToolTip();
		}
		m_ToolTip.SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
		m_ToolTip.SendMessage( TTM_SETDELAYTIME,  TTDT_INITIAL, 400);
		m_ToolTip.SendMessage( TTM_SETDELAYTIME,  TTDT_RESHOW, 5000);
		m_ToolTip.SendMessage( TTM_SETDELAYTIME,  TTDT_AUTOPOP, -1);



	}

	if (m_bSemiFlat || !(((m_flatButton == TRUE && m_drawBorder == FALSE)||m_flatButton == FALSE)&&((m_mouseInPicColor == m_mouseOutPicColor) && (m_mouseInShadow == m_mouseOutShadow))))
		m_nMouseTimer = SetTimer(2, 300, NULL);

	return 0;
}

void CPushButtonCtrl::OnDefaultButtonChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CPushButtonCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	COleControl::OnMouseMove(nFlags, point);

	// If the mouse enter the button with the left button pressed
	// then do nothing
	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

	// If our button is not flat & not uses different BMP then do nothing
	if( !m_bSemiFlat )
		if (((m_flatButton == TRUE && m_drawBorder == FALSE)||m_flatButton == FALSE)&&((m_mouseInPicColor == m_mouseOutPicColor) && (m_mouseInShadow == m_mouseOutShadow))) 
			return;
	
	if (GetCapture() != this) 
	{
		m_MouseOnButton = TRUE;
		/*m_ToolTip.Update();*/
		SetCapture();
		InvalidateControl();
	}
	else
	{
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* wndUnderMouse = WindowFromPoint(p2);
		if (wndUnderMouse != this)
		{
			// Redraw only if mouse goes out
			if (m_MouseOnButton == TRUE)
			{
				m_MouseOnButton = FALSE;
				InvalidateControl();
			}
			// If user is NOT pressing left button then release capture!
			if (!(nFlags & MK_LBUTTON)) 
				ReleaseCapture();
		}
	}
}

void CPushButtonCtrl::Serialize(CArchive& ar) 
{
	BYTE Version = 12;

	if (ar.IsStoring())
	{	// storing code
		ar<<(BYTE)Version;//m_autoSize;

		ar<<(BYTE)m_buttonDepth;
		ar<<(BYTE)m_flatButton;
		ar<<(BYTE)m_drawBorder;
		ar<<(BYTE)m_threeStateButton;
		ar<<(BYTE)m_defaultButton;
		ar<<(BYTE)GetRValue(RGB(0,0,0)/*m_ColorTransparent*/);
		ar<<(BYTE)GetGValue(RGB(0,0,0)/*m_ColorTransparent*/);
		ar<<(BYTE)GetBValue(RGB(0,0,0)/*m_ColorTransparent*/);
		ar<<m_BMPFileName;
		ar<<m_strPrevFileName;
		ar<<(BYTE)m_mouseInPicColor;
		ar<<(BYTE)m_mouseInShadow;
		ar<<(BYTE)m_mouseOutPicColor;
		ar<<(BYTE)m_mouseOutShadow;
		ar<<(BYTE)m_buttonPressedPicColor;
		ar<<(BYTE)m_buttonPressedShadow;
		ar<<(BYTE)m_bDisabled;
		ar<<(BYTE)m_drawFocusRect;
		ar<<(BYTE)m_drawFigures;
		ar<<m_buttonText;
		ar<<(BYTE)m_stretch;
		ar<<(BYTE)m_textAlign;
		ar<<(BYTE)m_offsetTextFromBMP;
		ar<<(BYTE)m_figureType;
		ar<<(BYTE)m_figureSize;
		ar<<(BYTE)GetRValue(RGB(0,0,0)/*m_ColorFigure*/);
		ar<<(BYTE)GetGValue(RGB(0,0,0)/*m_ColorFigure*/);
		ar<<(BYTE)GetBValue(RGB(0,0,0)/*m_ColorFigure*/);
		ar<<(BYTE)GetRValue(RGB(0,0,0)/*m_ColorFigureFill*/);
		ar<<(BYTE)GetGValue(RGB(0,0,0)/*m_ColorFigureFill*/);
		ar<<(BYTE)GetBValue(RGB(0,0,0)/*m_ColorFigureFill*/);
		ar<<(BYTE)m_fillFigure;
		COLORREF color = TranslateColor(m_activeTextColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		color = TranslateColor(m_inactiveTextColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		ar<<(BYTE)m_innerRaised;
		ar<<(BYTE)m_innerSunken;
		ar<<(BYTE)m_outerRaised;
		ar<<(BYTE)m_outerSunken;
		ar<<(BYTE)m_figureMaxSize;
		ar<<(BYTE)m_figureBound;
		ar<<(BYTE)m_advancedShadow;
		ar<<(BYTE)m_darkShadow;
		ar<<(int)m_buttonHeight;
		ar<<(int)m_buttonWidth;

		//ar<<(int)m_sizeOriginBitmap.cx;
		//ar<<(int)m_sizeOriginBitmap.cy;
		
	}	
	else
	{	// loading code
		BYTE	byte;
		ar>>byte;
		
		Version = byte;
		m_autoSize = FALSE;//(BOOL)byte;		

		ar>>byte;
		m_buttonDepth = (BOOL)byte;
		ar>>byte;
		m_flatButton = (BOOL)byte;
		ar>>byte;
		m_drawBorder = (BOOL)byte;
		ar>>byte;
		m_threeStateButton = (BOOL)byte;
		ar>>byte;
		m_defaultButton = (BOOL)byte;
		BYTE R, G, B;
		ar>>R;
		ar>>G;
		ar>>B;
		m_colorTransparent = (OLE_COLOR)RGB(R, G, B);
		ar>>m_BMPFileName;
		ar>>m_strPrevFileName;
		ar>>byte;
		m_mouseInPicColor = (BOOL)byte;
		ar>>byte;
		m_mouseInShadow = (BOOL)byte;
		ar>>byte;
		m_mouseOutPicColor = (BOOL)byte;
		ar>>byte;
		m_mouseOutShadow = (BOOL)byte;
		ar>>byte;
		m_buttonPressedPicColor = (BOOL)byte;
		ar>>byte;
		m_buttonPressedShadow = (BOOL)byte;
		ar>>byte;
		m_bDisabled = (BOOL)byte;
		ar>>byte;
		m_drawFocusRect = (BOOL)byte;
		ar>>byte;
		m_drawFigures = (BOOL)byte;
		ar>>m_buttonText;
		ar>>byte;
		m_stretch = (BOOL)byte;
		ar>>byte;
		m_textAlign = (short)byte;
		ar>>byte;
		m_offsetTextFromBMP = (short)byte;
		ar>>byte;
		m_figureType = (short)byte;
		ar>>byte;
		m_figureSize = (short)byte;
		ar>>R;
		ar>>G;
		ar>>B;
		m_colorFigure = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_colorFillFigure = (OLE_COLOR)RGB(R, G, B);


		//m_colorFillFigure = (OLE_COLOR)m_ColorFigureFill;
		//m_colorTransparent = (OLE_COLOR)m_ColorTransparent;

		ar>>byte;
		m_fillFigure = (BOOL)byte;

		ar>>R;
		ar>>G;
		ar>>B;
		m_activeTextColor = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_inactiveTextColor = (OLE_COLOR)RGB(R, G, B);

		ar>>byte;
		m_innerRaised = (BOOL)byte;
		ar>>byte;
		m_innerSunken = (BOOL)byte;
		ar>>byte;
		m_outerRaised = (BOOL)byte;
		ar>>byte;
		m_outerSunken = (BOOL)byte;
		ar>>byte;
		m_figureMaxSize = (short)byte;
		ar>>byte;
		m_figureBound = (short)byte;
		ar>>byte;
		m_advancedShadow = (BOOL)byte;
		ar>>byte;
		m_darkShadow = (BOOL)byte;
		int Int;
		ar>>Int;
		m_buttonHeight = (short)Int;
		ar>>Int;
		m_buttonWidth = (short)Int;
		//ar>>Int;
		//m_sizeOriginBitmap.cx = (long)Int;
		//ar>>Int;
		//m_sizeOriginBitmap.cy = (long)Int;
	}

	GetControlSize((int*)&m_sizeOriginBitmap.cx, (int*)&m_sizeOriginBitmap.cy);
	SerializeStockProps(ar);

	
	
	if( ar.IsStoring() )
	{
		ar<<m_strActionName;
		ar<<m_bRunAction;
		ar<<m_bAutoRepeat;

		//Support OLE_COLOR in version 7
		ar<<m_colorTransparent;
		ar<<m_colorFigure;
		ar<<m_colorFillFigure;
		ar<<m_activeTextColor;
		ar<<m_inactiveTextColor;

		ar<<m_bDrawFocusedRect;


		BOOL b = (m_bSemiFlat == true);
		ar<<b;

		ar<<m_bUseSystemFont;
		
		// [vanek]
		ar << m_bShowToolTip;
		ar << m_sToolTipText;

		ar << m_LayoutTextAlign;
	}
	else
	{
		if( Version >= 5 )
		{
			ar>>m_strActionName;
			ar>>m_bRunAction;
	
		}

		if( Version >= 6 )
		{
			ar>>m_bAutoRepeat;	

			if( m_bAutoRepeat )
  				RunTimer();
		}

		if( Version >= 7 )
		{
			//Support OLE_COLOR in version 7
			ar>>m_colorTransparent;
			ar>>m_colorFigure;
			ar>>m_colorFillFigure;
			ar>>m_activeTextColor;
			ar>>m_inactiveTextColor;

		}

		if( Version >= 8 )
			ar>>m_bDrawFocusedRect;

		if( Version >= 9 )
		{
			BOOL b;
			ar>>b;
			m_bSemiFlat = ( b == TRUE );
		}

		
		if( Version >= 10 )
			ar>>m_bUseSystemFont;

		// vanek
		if( Version >= 11 )
		{
			ar >> m_bShowToolTip;
			ar >> m_sToolTipText;

			_ReCreateToolTip( );
		}

		if( Version >= 12 )
			ar >> m_LayoutTextAlign;	
	}

	m_DIB.Serialize(ar);

	_MakeBitmaps();
	//OnBMPFileNameChanged();	
}


BOOL CPushButtonCtrl::ResetPressedState() 
{
	// TODO: Add your dispatch handler code here
	BOOL	prevState = m_bPushed;
	m_bPushed = FALSE;
	InvalidateControl();
	return prevState;
}

void CPushButtonCtrl::OnThreeStateButtonChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}


void CPushButtonCtrl::OnMouseInPicColorChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CPushButtonCtrl::OnMouseOutPicColorChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CPushButtonCtrl::OnMouseOutShadowChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CPushButtonCtrl::OnButtonPressedPicColorChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnButtonPressedShadowChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnMouseInShadowChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

BOOL CPushButtonCtrl::GetDisabled() 
{
	// TODO: Add your property handler here
	return m_bDisabled;
}

void CPushButtonCtrl::SetDisabled(BOOL bNewValue) 
{
	// TODO: Add your property handler here
	m_bDisabled = bNewValue;
	SetModifiedFlag();

	if( !IsWindow( m_hWnd ) )
		return;
	// If user is NOT pressing left button then release capture!
	if (GetCapture() == this && m_bDisabled) ReleaseCapture();
	InvalidateControl();
}

void CPushButtonCtrl::OnDrawFocusRectChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CPushButtonCtrl::OnStretchChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnButtonTextChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnTextAlignChanged() 
{
	// TODO: Add notification handler code
	if(m_textAlign < 0 || m_textAlign > 2) // vanek
		m_textAlign = 0;
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnOffsetTextFromBMPChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnDrawFiguresChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnFigureTypeChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	if(m_figureType < 0 || m_figureType > 2)
		m_figureType = 0;
	SetModifiedFlag();
}

void CPushButtonCtrl::OnFigureSizeChanged() 
{
	// TODO: Add notification handler code
	if(m_figureSize < 0)
		m_figureSize = 0;
	if(m_figureSize > m_figureMaxSize)
		m_figureSize = m_figureMaxSize;
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnColorTransparentChanged() 
{
	// TODO: Add notification handler code
	//m_ColorTransparent = m_colorTransparent;
	//OnBMPFileNameChanged();
	_MakeBitmaps();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnColorFigureChanged() 
{
	// TODO: Add notification handler code
//	m_ColorFigure = m_colorFigure;
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnColorFillFigureChanged() 
{
	// TODO: Add notification handler code
//	m_ColorFigureFill = m_colorFillFigure;
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnFillFigureChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnActiveTextColorChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnInactiveTextColorChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnInnerRaisedChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnInnerSunkenChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnOuterRaisedChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnOuterSunkenChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

BOOL CPushButtonCtrl::SetPressedState() 
{
	// TODO: Add your dispatch handler code here
	BOOL	prevState = m_bPushed;
	m_bPushed = TRUE;
	InvalidateControl();
	return prevState;
}

BOOL CPushButtonCtrl::IsPressed() 
{
	// TODO: Add your dispatch handler code here
	return m_bPushed == TRUE;
}

void CPushButtonCtrl::OnFigureMaxSizeChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnFigureBoundChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnAdvancedShadowChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

void CPushButtonCtrl::OnDarkShadowChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

LRESULT CPushButtonCtrl::OnSetCheck( WPARAM wCheck, LPARAM )
{
	if( wCheck )
		SetPressedState();
	else
		ResetPressedState();

	return 0;
}

void CPushButtonCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	
	m_buttonWidth = cx;
	m_buttonHeight = cy;

//	SetModifiedFlag();
}

BOOL CPushButtonCtrl::SetAutoSize() 
{
	int nAddSize = 0;

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
		nAddSize = 2 * VT_BORDER_WIDTH;
	}



	int nTextWidth  = 0;
	int nTextHeight = 0;

	if( !m_buttonText.IsEmpty() )
	{
		CClientDC dc(this);

		CFont font;
		if( ::GetFontFromDispatch( this, font, GetUseSystemFont() ) )
		{

			CFont* pOldFont = (CFont*)dc.SelectObject( &font );//SelectStockFont( &dc );

			CRect rcCalc(0,0,0,0);
			//dc.DrawText( m_buttonText, &rcCalc , DT_CALCRECT );
			CString stMultiString = m_buttonText;
			stMultiString.Replace( "\\n", "\r\n" );
            dc.DrawText( m_buttonText, &rcCalc , DT_CALCRECT );
			
			nTextWidth = rcCalc.Width();
			nTextHeight = rcCalc.Height();	
			dc.SelectObject( pOldFont );
		}

	}

	int nImageWidth  = 0;
	int nImageHeight = 0;

	nImageWidth  = m_sizeOriginBitmap.cx;
	nImageHeight = m_sizeOriginBitmap.cy;


	//m_textAlign = 0; //0 - right //1 - bottom 

	if( !m_buttonText.IsEmpty() )
	{
		if( m_textAlign == 0)
		{
			m_buttonWidth  = nTextWidth + nImageWidth + nAddSize + 10;
			m_buttonHeight = max( nTextHeight, nImageHeight ) + nAddSize + 5;
		}
		else
		{
			m_buttonWidth  = max( nTextWidth, nImageWidth)  + nAddSize + 5;
			m_buttonHeight = nTextHeight + nImageHeight + nAddSize + 10;
		}	 
	}
	else
	{
		m_buttonWidth	= nImageWidth + 5;
		m_buttonHeight	= nImageHeight + 5;
	}
	

	SetControlSize(m_buttonWidth, m_buttonHeight);
	InvalidateControl();

	SetModifiedFlag();

	return TRUE;
}


LRESULT CPushButtonCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	
	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_buttonText, wParam);
		if ((int)wParam > m_buttonText.GetLength())
		wParam = m_buttonText.GetLength();
		return TRUE;    
	}

	if( message == WM_SETTEXT )
	{
		if (lParam == NULL)
		{
			// NULL lParam means set caption to nothing
			m_buttonText.Empty();
		}
		else
		{
			// non-NULL sets caption to that specified by lParam
			lstrcpy(m_buttonText.GetBufferSetLength(lstrlen((LPCTSTR)lParam)),
				(LPCTSTR)lParam);
		}
		
		InvalidateControl();
		SetModifiedFlag();    
		return TRUE;
	}
	return COleControl::DefWindowProc(message, wParam, lParam);
}


BSTR CPushButtonCtrl::GetActionName() 
{
	CString strResult = m_strActionName;
	return strResult.AllocSysString();
}

void CPushButtonCtrl::SetActionName(LPCTSTR lpszNewValue) 
{
	m_strActionName = lpszNewValue;
	SetModifiedFlag();
}

BOOL CPushButtonCtrl::GetRunAction() 
{
	return m_bRunAction;
}

void CPushButtonCtrl::SetRunAction(BOOL bNewValue) 
{
	m_bRunAction = bNewValue;
	SetModifiedFlag();
}


BOOL CPushButtonCtrl::ReloadBitmap() 
{	
	try
	{
		if( !m_BMPFileName.IsEmpty() )
		{			
			CFile file(m_BMPFileName, CFile::modeRead);
			m_DIB.Read(file);
			
			_MakeBitmaps();
		}
		else
		{
			_MakeBitmaps();
		}
		
		SetModifiedFlag();
	}
	catch(CException *pe)
	{
		m_DIB.Free();
		_FreeBMPs();
		pe->ReportError();
		pe->Delete();

		return FALSE;
	}
	return TRUE;
}



#ifndef FOR_HOME_WORK
HRESULT CPushButtonCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CPushButtonCtrl, ActiveXCtrl)

	pThis->m_ptrSite = punkSite;
	pThis->m_ptrApp = punkVtApp;	

	return S_OK;
}
  

HRESULT CPushButtonCtrl::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CPushButtonCtrl, ActiveXCtrl)	
	//*pbstrName = pThis->m_strName.AllocSysString();
	*pbstrName = NULL;

	return S_OK;
}
  

#endif



BOOL CPushButtonCtrl::GetAutoRepeat() 
{
	return m_bAutoRepeat;
}

void CPushButtonCtrl::SetAutoRepeat(BOOL bNewValue) 
{
	
	m_bAutoRepeat = bNewValue;

	StopTimer();

	if( m_bAutoRepeat )
		RunTimer();

	SetModifiedFlag();
}

void CPushButtonCtrl::RunAction()
{
#ifndef FOR_HOME_WORK			
	if(m_bRunAction)
	{
		if( m_ptrSite != NULL )
		{
			CString strActionName = m_strActionName;
			CString strActionParam = "";

			IAXSite* pIActiveSite = 0;

			m_ptrSite->QueryInterface( IID_IAXSite,(void**) &pIActiveSite);
			
			if( pIActiveSite )
			{
				pIActiveSite->ExecuteAction( 
						strActionName.AllocSysString( ),
						strActionParam.AllocSysString( ),
						0
						);
				pIActiveSite->Release( );
			}
			
		}
			
	}
#endif			

}

void CPushButtonCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == m_nTimerID)
	{	
		CButton* pBtn = (CButton*)this;
		if( pBtn && pBtn->GetSafeHwnd() )
		{

		
			UINT nState = 0;
			nState = pBtn->GetState();
			if( nState & 0x0004 )
			{
				RunAction();		
				//TRACE("\nRunAction");
			}
			
		}
	}
	if( nIDEvent == m_nMouseTimer)
	{
		if (m_MouseOnButton && GetCapture() != this)
		{
			CPoint pt;
			GetCursorPos(&pt);
			CWnd *pWnd = WindowFromPoint(pt);
			if (pWnd != this)
			{
				m_MouseOnButton = FALSE;
				InvalidateControl();
			}
		}
	}
	
	COleControl::OnTimer(nIDEvent);
}

void CPushButtonCtrl::RunTimer()
{
	m_nTimerID = 1;
	m_nTimerID = SetTimer( m_nTimerID, 100, NULL );
}

void CPushButtonCtrl::StopTimer()
{
	if( m_nTimerID != -1)
		KillTimer( m_nTimerID );

	m_nTimerID = -1;
}


void CPushButtonCtrl::OnDestroy() 
{
	StopTimer();
	if (m_nMouseTimer != 0) KillTimer(m_nMouseTimer);
	COleControl::OnDestroy();		
}



/////////////////////////////////////////////////////////////////////////////////////
// function for visible & enable control status & SetFocus
/////////////////////////////////////////////////////////////////////////////////////
BOOL CPushButtonCtrl::GetVisible()
{
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );



}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetDrawFocusedRect(BOOL bNewValue)
{
	m_bDrawFocusedRect = bNewValue;
	Refresh();
	SetModifiedFlag();

}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CPushButtonCtrl::GetDrawFocusedRect()
{
	return m_bDrawFocusedRect;
}


/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetSemiFlat(BOOL bNewValue)
{
	m_bSemiFlat = ( bNewValue == TRUE );
	Refresh();
	SetModifiedFlag();

}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CPushButtonCtrl::GetSemiFlat()
{
	return (m_bSemiFlat == true);
}


/////////////////////////////////////////////////////////////////////////////////////
BOOL CPushButtonCtrl::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
	
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CPushButtonCtrl::GetShowToolTip() 
{
	return m_bShowToolTip;
}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetShowToolTip(BOOL bNewValue)
{
	m_bShowToolTip = bNewValue;
	m_ToolTip.Activate( m_bShowToolTip );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////////////
BSTR CPushButtonCtrl::GetToolTip() 
{
	return m_sToolTipText.AllocSysString( );
}

/////////////////////////////////////////////////////////////////////////////////////
void CPushButtonCtrl::SetToolTip(LPCTSTR lpszNewValue)
{
	if( lpszNewValue )
	{
		m_sToolTipText = lpszNewValue;
		m_ToolTip.UpdateTipText( m_sToolTipText, this, 1 );
		SetModifiedFlag();
	}
}

SHORT CPushButtonCtrl::GetLayoutTextAlign(void)
{
	return m_LayoutTextAlign;
}

void CPushButtonCtrl::SetLayoutTextAlign(SHORT newVal)
{
	if( newVal >= 0 && newVal <= 2 )
	{
		m_LayoutTextAlign = newVal;
		SetModifiedFlag();
	}
}

long CPushButtonCtrl::GetButtonLeft( )
{
	RECT rc_in_cnt = {0};
	GetRectInContainer( &rc_in_cnt );
	return rc_in_cnt.left;
}

void CPushButtonCtrl::SetButtonLeft(long lLeft)
{
    RECT rc_in_cnt = {0};
	if( !GetRectInContainer( &rc_in_cnt ) )
		return;
    
	::OffsetRect( &rc_in_cnt, (lLeft - rc_in_cnt.left), 0 );
	SetRectInContainer( &rc_in_cnt );
}

long CPushButtonCtrl::GetButtonTop( )
{
	RECT rc_in_cnt = {0};
	GetRectInContainer( &rc_in_cnt );
	return rc_in_cnt.top;
}

void CPushButtonCtrl::SetButtonTop(long lTop)
{
	RECT rc_in_cnt = {0};
	if( !GetRectInContainer( &rc_in_cnt ) )
		return;
    
	::OffsetRect( &rc_in_cnt, 0, (lTop - rc_in_cnt.top) );
	SetRectInContainer( &rc_in_cnt );
}

LRESULT CPushButtonCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bIsMouseMessage = FALSE;

	switch( message )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:				 
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			bIsMouseMessage = TRUE;

			MSG MsgCopy = {0};
			MsgCopy.hwnd = m_hWnd;
			MsgCopy.message = message;
			MsgCopy.wParam = wParam;
			MsgCopy.lParam = lParam;
			MsgCopy.time = 0;
			MsgCopy.pt.x = ( (int)(short)LOWORD(lParam) );
			MsgCopy.pt.y = ( (int)(short)HIWORD(lParam) );
			m_ToolTip.RelayEvent( &MsgCopy );
		}
	}

	return (bIsMouseMessage && m_bDisabled) ? TRUE : COleControl::WindowProc(message, wParam, lParam);
}
