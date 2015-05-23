// VTColorPickerCtl.cpp : Implementation of the CVTColorPickerCtrl ActiveX Control class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTColorPickerCtl.h"
#include "VTColorPickerPpg.h"

#include <comutil.h>
#include "defs.h"
#include "image5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTColorPickerCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTColorPickerCtrl, COleControl)
	//{{AFX_MSG_MAP(CVTColorPickerCtrl)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTColorPickerCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CVTColorPickerCtrl)
	DISP_PROPERTY_NOTIFY(CVTColorPickerCtrl, "LikeCombo", m_likeCombo, OnLikeComboChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTColorPickerCtrl, "BlackAndWhite", m_blackAndWhite, OnBlackAndWhiteChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTColorPickerCtrl, "Binary", m_binary, OnBinaryChanged, VT_BOOL)
	DISP_PROPERTY_EX(CVTColorPickerCtrl, "Color", GetColor, SetColor, VT_I4)
    DISP_FUNCTION(CVTColorPickerCtrl, "GetBinaryColor", GetBinaryColor, VT_COLOR, VTS_I2)
	DISP_FUNCTION(CVTColorPickerCtrl, "SetBinaryColor", SetBinaryColor, VT_EMPTY, VTS_I2 VTS_I4)
	DISP_FUNCTION(CVTColorPickerCtrl, "GetActiveBinaryIndex", GetActiveBinaryIndex, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTColorPickerCtrl, "GetBinaryColorsCount", GetBinaryColorsCount, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTColorPickerCtrl, "SetActiveBinaryIndex", SetActiveBinaryIndex, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTColorPickerCtrl, "GetBinaryColorRGB", GetBinaryColorRGB, VT_BSTR, VTS_I2 )
	DISP_FUNCTION(CVTColorPickerCtrl, "SetBinaryColorRGB", SetBinaryColorRGB, VT_EMPTY, VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTColorPickerCtrl, "SetColorRGB", SetColorRGB, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CVTColorPickerCtrl, "GetColorRGB", GetColorRGB, VT_BSTR, VTS_NONE )
	DISP_FUNCTION(CVTColorPickerCtrl, "SetPaletteEntry", SetPaletteEntry, VT_EMPTY, VTS_I2 VTS_I4)
	DISP_FUNCTION(CVTColorPickerCtrl, "InitializePicker", InitializePicker, VT_EMPTY, VTS_UNKNOWN VTS_UNKNOWN)
	DISP_FUNCTION(CVTColorPickerCtrl, "SetVisible", SetVisible, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CVTColorPickerCtrl, "GetVisible", GetVisible, VT_BOOL, VTS_NONE)
	DISP_STOCKPROP_ENABLED()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTColorPickerCtrl, COleControl)
	//{{AFX_EVENT_MAP(CVTColorPickerCtrl)
	EVENT_CUSTOM("OnChange", FireOnChange, VTS_NONE)
	EVENT_CUSTOM_ID("Click", DISPID_CLICK, FireClick, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTColorPickerCtrl, 1)
	PROPPAGEID(CVTColorPickerPropPage::guid)

END_PROPPAGEIDS(CVTColorPickerCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTColorPickerCtrl, "VTCONTROLS2.VTColorPickerCtrl.1",
	0xbc383bc9, 0x6fd1, 0x4ff7, 0x82, 0xad, 0xb4, 0x5, 0x34, 0x1d, 0x5e, 0x60)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTColorPickerCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTColorPicker =
		{ 0xf6816f65, 0xea89, 0x471c, { 0x99, 0x39, 0x31, 0x64, 0xb7, 0xd3, 0, 0x41 } };
const IID BASED_CODE IID_DVTColorPickerEvents =
		{ 0xc187cfdd, 0x9aae, 0x4943, { 0x8f, 0xe1, 0x68, 0x72, 0xca, 0xa1, 0xc4, 0x56 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTColorPickerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTColorPickerCtrl, IDS_VTCOLORPICKER, _dwVTColorPickerOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::CVTColorPickerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTColorPickerCtrl

BOOL CVTColorPickerCtrl::CVTColorPickerCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTCOLORPICKER,
			IDB_VTCOLORPICKER,
			afxRegApartmentThreading,
			_dwVTColorPickerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::CVTColorPickerCtrl - Constructor

CVTColorPickerCtrl::CVTColorPickerCtrl()
{
	InitializeIIDs(&IID_DVTColorPicker, &IID_DVTColorPickerEvents);

	EnableSimpleFrame();

	currentcolor = RGB(255,255,255);
	

//	dlg.parent=NULL;


	m_blackAndWhite = false;
	m_binary = false;
	m_IsFocused=FALSE;
	m_IsMouseOver=FALSE;
	m_Checked=FALSE;
	m_Brush=RGB(198,198,198);
	m_Edge=BDR_RAISEDINNER;
	for (int i = 0; i < 256; i++)
		m_Palette[i] = RGB(i,i,i);
	m_bPalette = false;

	m_dlg.parent = this;
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::~CVTColorPickerCtrl - Destructor

CVTColorPickerCtrl::~CVTColorPickerCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::OnDraw - Drawing function

void CVTColorPickerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	//DoSuperclassPaint(pdc, rcBounds);
	CDC memDC;
	memDC.CreateCompatibleDC(pdc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pdc,rcBounds.Width(),rcBounds.Height());
	CBrush* oldBrush;
	CPen *oldPen;
	CBitmap* oldBitmap=memDC.SelectObject(&bmp);

	RECT rc;
	CRect rcDeflated;
	CRect rccombo;
	if(m_likeCombo)
	{
		rc=rcBounds;
		rcDeflated=rcBounds;
		rcDeflated.right-=rcBounds.Height()-3;
		rccombo=rcBounds;
		rccombo.left=rccombo.right-rcBounds.Height();
		rccombo.DeflateRect(2,2);
//		rcDeflated.DeflateRect(2,2);
	}
	else
	{
		rc=rcBounds;
		rcDeflated=rc;
		rcDeflated.DeflateRect(5,5);
	}
//	rcDeflated.DeflateRect(5,5);
	if(m_Checked)
	{
		rcDeflated+=CPoint(1,1);
	//	rccombo+=CPoint(1,1);
	}

	SelectBrushAndEdge();
	CBrush brush1,brush2;
	brush2.CreateSolidBrush(m_Brush);
	oldBrush=(CBrush*)memDC.SelectObject(&brush2);
	memDC.FillRect(rcBounds,&brush2);

	if(m_likeCombo)
	{
		if(m_Checked) memDC.DrawFrameControl(rccombo,DFC_SCROLL,DFCS_SCROLLDOWN | DFCS_PUSHED );
		else memDC.DrawFrameControl(rccombo,DFC_SCROLL,DFCS_SCROLLDOWN | (GetEnabled() ? 0 : DFCS_INACTIVE) );
	}

	CPen pen( PS_SOLID, 1, ::GetSysColor( (GetEnabled() ? COLOR_BTNTEXT : COLOR_GRAYTEXT) ) );
	oldPen=memDC.SelectObject(&pen);
	COLORREF clr = currentcolor;
	if (m_blackAndWhite && m_bPalette)
	{
		int n = GetGValue(clr);
		clr = m_Palette[n];
	}
	brush1.CreateSolidBrush(clr);
	memDC.SelectObject(&brush1);
	memDC.Rectangle(rcDeflated);

	if(!m_likeCombo)
	{
		if(m_IsMouseOver)
			memDC.DrawEdge(&rc,m_Edge,BF_BOTTOMRIGHT | (GetEnabled() ? 0 : BF_FLAT) );
		else
			memDC.DrawEdge(&rc,m_Edge,BF_RECT | (GetEnabled() ? 0 : BF_FLAT) );

		if(!m_Checked)
			memDC.DrawEdge(&rc,BDR_RAISEDINNER ,BF_TOPLEFT | (GetEnabled() ? 0 : BF_FLAT) );
		else
			memDC.DrawEdge(&rc,m_Edge,BF_TOPLEFT);
	}
	else
		memDC.DrawEdge(&rc,EDGE_SUNKEN,BF_RECT | (GetEnabled() ? 0 : BF_FLAT) );

	
	if(!m_likeCombo) 
		rcDeflated.InflateRect(2,2);
	else
		rcDeflated.DeflateRect(4,4);

	if(m_IsFocused) memDC.DrawFocusRect(rcDeflated);

	pdc->BitBlt(rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(oldPen);
	memDC.SelectObject(oldBrush);
	memDC.SelectObject(oldBitmap);

}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::DoPropExchange - Persistence support

void CVTColorPickerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
	PX_Bool(pPX,"LikeCombo",m_likeCombo,TRUE);
	if (pPX->GetVersion() >= MAKELONG(3, 1))
    {
        PX_Bool(pPX,"BlackAndWhite",m_blackAndWhite,false);
    }
	else
		m_blackAndWhite = false;
	if (pPX->GetVersion() >= MAKELONG(4, 1))
    {
        PX_Bool(pPX,"Binary",m_binary,false);
    }
	else
		m_binary = false;

}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::OnResetState - Reset control to default state

void CVTColorPickerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CVTColorPickerCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.lpszClass = _T("BUTTON");
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl message handlers



int CVTColorPickerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//CRect rect(0,0,lpCreateStruct->cx,lpCreateStruct->cy);
	int i;
	for(i=20;i<30;i++)
		m_dlg.colors[i]=RGB(255,255,255);
	
	m_dlg.ResetColors();

			
	return 0;
}

void CVTColorPickerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_IsMouseOver)
	{
	 m_IsMouseOver=TRUE;
	 InvalidateRect(NULL);
	}
	SetTimer(1,55,NULL);
	
	COleControl::OnMouseMove(nFlags, point);
}

void CVTColorPickerCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	m_Checked=FALSE;

	Refresh();

	COleControl::OnLButtonUp(nFlags, point);
}

void CVTColorPickerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Checked=TRUE;
	Refresh();
	FireClick();

	if (m_dlg.DoModal() == IDOK)
    {
        SetColor(m_dlg.colors[m_dlg.colorindex]);
    }

	m_Checked=FALSE;
    Refresh();
	
}

void CVTColorPickerCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rect;
    GetWindowRect (rect);

	if(!rect.PtInRect(pt))
	{
		m_IsMouseOver=FALSE;
		KillTimer(1);
		Refresh();
	}
}

void CVTColorPickerCtrl::SelectBrushAndEdge()
{
 	if(!m_Checked)
	{
		if(!m_IsMouseOver)
		{
			m_Brush=::GetSysColor(COLOR_BTNFACE);
			m_Edge=BDR_RAISEDINNER;
		}
		else
		{
			m_Brush=::GetSysColor(COLOR_BTNFACE);
			m_Edge=EDGE_RAISED;
		}
	}
	else
	{
		if(m_IsMouseOver)
		{
			COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
			COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
			COLORREF clrDither = RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
				                     GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
									 GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

			m_Brush=clrDither;
			m_Edge=BDR_SUNKENINNER;
		}
		else
		{
			COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
			COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
			COLORREF clrDither = RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
				                     GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
									 GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

			m_Brush=clrDither;
			m_Edge=BDR_SUNKENINNER;
		}
	}


}

void CVTColorPickerCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);

	m_IsFocused=TRUE;
	Refresh();

	
	// TODO: Add your message handler code here
	
}

void CVTColorPickerCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);

	m_IsFocused=FALSE;
	Refresh();
	
	// TODO: Add your message handler code here
	
}

void CVTColorPickerCtrl::Serialize(CArchive &ar)
{
	COleControl::Serialize( ar );
	m_dlg.ResetColors();

	/*
	    if (ar.IsStoring())
    {
        ar.Write((void *)CColorBtnDlg::colors,sizeof(COLORREF)*30);
        ar.Write((void *)CColorBtnDlg::used,sizeof(BYTE)*30);
    }
    else
    {
        ar.Read((void *)CColorBtnDlg::colors,sizeof(COLORREF)*30);
        ar.Read((void *)CColorBtnDlg::used,sizeof(BYTE)*30);
    }
	*/
}

long CVTColorPickerCtrl::GetColor() 
{
	return currentcolor;
}

void CVTColorPickerCtrl::SetColor(long nNewValue) 
{
	if(!m_blackAndWhite)
		currentcolor=nNewValue;
	else
	{
		int temp = (GetRValue(nNewValue) + GetGValue(nNewValue) + GetBValue(nNewValue));
		currentcolor=RGB(temp/3,temp/3,temp/3);
	}
	Refresh();
	FireOnChange();
	SetModifiedFlag();
}

void CVTColorPickerCtrl::SetPaletteEntry(short index, long color)
{
	if (index >= 0 && index < 256)
	{
		m_Palette[index] = color;
		m_bPalette = true;
	}
	else
		m_bPalette = false;
}

void CVTColorPickerCtrl::InitializePicker(LPUNKNOWN punkView, LPUNKNOWN punkImage)
{
	if (punkImage == NULL || punkView == NULL) return;
	IImagePtr sptrImage(punkImage);
	if (sptrImage == 0) return;
	IUnknownPtr punkCC;
	sptrImage->GetColorConvertor(&punkCC);
	IColorConvertor6Ptr sptrCC(punkCC);
	int nColorMode;
	COLORREF Palette[256];
	if (sptrCC != 0)
	{
		sptrCC->GetEditMode(sptrImage, &nColorMode);
		if (nColorMode == 2)
			sptrCC->GetPalette(sptrImage, punkView, Palette, 256);
	}
	else
		nColorMode = 0;
	m_bPalette = nColorMode == 2;
	if (m_bPalette)
		memcpy(m_Palette, Palette, sizeof(Palette));
	m_blackAndWhite = nColorMode!=1;
	OnBlackAndWhiteChanged();
	Invalidate();
}

void CVTColorPickerCtrl::OnLikeComboChanged() 
{
	Refresh();

	SetModifiedFlag();
}

UINT CVTColorPickerCtrl::OnGetDlgCode() 
{
   return DLGC_WANTARROWS;
}

void CVTColorPickerCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case VK_F4: OnLButtonDown(0,CPoint(1,1)); break;
	}	
	//COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CVTColorPickerCtrl::OnBlackAndWhiteChanged() 
{
	SetModifiedFlag();
	m_dlg.ResetColors();
}

BOOL CVTColorPickerCtrl::IsBlackAndWhite()
{
	return m_blackAndWhite;
}

void CVTColorPickerCtrl::OnBinaryChanged() 
{
	if(m_binary==false)
		m_dlg.ResetColors();
	SetModifiedFlag();
}

OLE_COLOR CVTColorPickerCtrl::GetBinaryColor(short index) 
{
	if(index>=0 && index<GetBinaryColorsCount())
		return m_dlg.colors[index];
	return RGB(0,0,0);
}

BSTR CVTColorPickerCtrl::GetBinaryColorRGB(short index)
{	
	OLE_COLOR color;
	if(index>=0 && index<GetBinaryColorsCount())
		color = m_dlg.colors[index];
	else
		color = RGB(0,0,0);

	COLORREF Color = (COLORREF)color;

	return _color_to_str( Color ).AllocSysString();
}

BSTR CVTColorPickerCtrl::GetColorRGB()
{	
	return _color_to_str( currentcolor ).AllocSysString();
}

void CVTColorPickerCtrl::SetBinaryColorRGB(short index, LPCTSTR strRGBColor )
{	
	if(index>=0 && index<GetBinaryColorsCount())
	{
		m_dlg.colors[index] = TranslateColor( _str_to_color( strRGBColor ) );
	}
}

void CVTColorPickerCtrl::SetColorRGB(LPCTSTR strRGBColor )
{	
	currentcolor = _str_to_color( strRGBColor );

	Refresh();
	FireOnChange();
	SetModifiedFlag();
}


BOOL CVTColorPickerCtrl::IsBinary()
{
	return m_binary;
}

void CVTColorPickerCtrl::SetBinaryColor(short index, long color) 
{
	if(index>=0 && index<GetBinaryColorsCount())
		m_dlg.colors[index] = TranslateColor(color);
}


short CVTColorPickerCtrl::GetActiveBinaryIndex() 
{
	return m_dlg.colorindex;
}

short CVTColorPickerCtrl::GetBinaryColorsCount() 
{
	return 20;
}

void CVTColorPickerCtrl::SetActiveBinaryIndex(short nIdx) 
{
	if(nIdx>=0 && nIdx<GetBinaryColorsCount())
	{
		m_dlg.colorindex = nIdx;
		SetColor(m_dlg.colors[nIdx]);
	}
}

long CVTColorPickerCtrl::_str_to_color( CString strRGBColor )
{
	OLE_COLOR color = (OLE_COLOR)RGB(255,255,255);
	CString str = strRGBColor;
	CString strR = "", strG = "", strB = "";

	if( str.Find("(") != -1 && str.Find(")") != -1 )
	{
		int brCount = 0;
		for( int i = 1; i < str.GetLength() - 1 ;i++)
		{
			char ch = str.GetAt( i );
			if( ch != ',' )
			{
				switch( brCount )
				{
				case 0:	strR += ch; 
						break;
				case 1:	strG += ch; 
						break;
				case 2:	strB += ch; 
						break;
				}
			}
			else
				brCount++;
		}
		
		if( !strR.IsEmpty() && !strG.IsEmpty() && !strB.IsEmpty() )
			color = (OLE_COLOR)RGB( atoi(strR), atoi(strG), atoi(strB) );
	}
	return color;
}

CString CVTColorPickerCtrl::_color_to_str( long Color )
{
	CString str;
	str.Format( "(%d,%d,%d)", GetRValue(Color), GetGValue(Color), GetBValue(Color) );

	return str;
}

void CVTColorPickerCtrl::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

BOOL CVTColorPickerCtrl::GetVisible()
{
	return COleControl::IsWindowVisible();
}
