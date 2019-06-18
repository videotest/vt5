// VTWidthCtl.cpp : Implementation of the CVTWidthCtrl ActiveX Control class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTWidthCtl.h"
#include "VTWidthPpg.h"
#include "WidthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFST 3              //   bad style

IMPLEMENT_DYNCREATE(CVTWidthCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTWidthCtrl, COleControl)
	//{{AFX_MSG_MAP(CVTWidthCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTONWIDTH,OnButtonWidthClicked)
	ON_BN_CLICKED(IDC_BUTTONUP,OnButtonUpClicked)
	ON_BN_CLICKED(IDC_BUTTONDOWN,OnButtonDownClicked)
	ON_BN_CLICKED(IDC_BUTTONSLIDER,OnButtonSliderClicked)
	ON_WM_SIZE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTWidthCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CVTWidthCtrl)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "Visible", m_visible, OnVisibleChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorText", m_colorText, OnColorTextChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorTextHighlight", m_colorTextHighlight, OnColorTextHighlightChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorSlider", m_colorSlider, OnColorSliderChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorSliderArrows", m_colorSliderArrows, OnColorSliderArrowsChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorTextScale", m_colorTextScale, OnColorTextScaleChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTWidthCtrl, "ColorSliderArrowsHighlight", m_colorSliderArrowsHighlight, OnColorSliderArrowsHighlightChanged, VT_COLOR)
	DISP_PROPERTY_EX(CVTWidthCtrl, "Width", GetWidth, SetWidth, VT_I4)
	DISP_PROPERTY_EX(CVTWidthCtrl, "Checked", GetChecked, SetChecked, VT_BOOL)
	DISP_PROPERTY_EX(CVTWidthCtrl, "Min", GetMin, SetMin, VT_I4)
	DISP_PROPERTY_EX(CVTWidthCtrl, "Max", GetMax, SetMax, VT_I4)
	DISP_PROPERTY_EX(CVTWidthCtrl, "Orientation", GetOrientation, SetOrientation, VT_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTWidthCtrl, COleControl)
	//{{AFX_EVENT_MAP(CVTWidthCtrl)
	EVENT_CUSTOM("OnChange", FireOnChange, VTS_NONE)
	EVENT_CUSTOM_ID("Click", DISPID_CLICK, FireClick, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTWidthCtrl, 2)
	PROPPAGEID(CVTWidthPropPage::guid)
	PROPPAGEID(CLSID_CColorPropPage)
END_PROPPAGEIDS(CVTWidthCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid		  n

IMPLEMENT_OLECREATE_EX(CVTWidthCtrl, "VTCONTROLS2.VTWidthCtrl.1",
	0x5df91a6e, 0x34a7, 0x4ce0, 0xa2, 0x31, 0x8e, 0xe, 0x7e, 0xf3, 0x40, 0xe5)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTWidthCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTWidth =
		{ 0x92b9f891, 0xe68c, 0x4c52, { 0xa7, 0x63, 0x5c, 0x71, 0xcd, 0xe6, 0xe1, 0xc7 } };
const IID BASED_CODE IID_DVTWidthEvents =
		{ 0xeeb7070c, 0x932f, 0x41f1, { 0x90, 0x8f, 0x93, 0xdc, 0x32, 0x36, 0x27, 0x96 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTWidthOleMisc =
	OLEMISC_SIMPLEFRAME |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTWidthCtrl, IDS_VTWIDTH, _dwVTWidthOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::CVTWidthCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTWidthCtrl

BOOL CVTWidthCtrl::CVTWidthCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTWIDTH,
			IDB_VTWIDTH,
			afxRegApartmentThreading,
			_dwVTWidthOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::CVTWidthCtrl - Constructor

CVTWidthCtrl::CVTWidthCtrl()
{
	InitializeIIDs(&IID_DVTWidth, &IID_DVTWidthEvents);

	EnableSimpleFrame();

	// TODO: Initialize your control's instance data here.
//	m_Width=50;
//	m_Min=0;
//	m_Max=100;
	m_Checked=FALSE;
//	m_Orientation=TRUE;
	m_IsCreated=FALSE;
	m_IsSliderMoved=FALSE;
	m_IsFocused=FALSE;
	m_colorText=0x80000000+COLOR_BTNTEXT;
	m_colorTextScale=0x80000000+COLOR_BTNTEXT;
	m_colorSliderArrows=0x80000000+COLOR_BTNTEXT;
	m_colorSlider=0x80000000+COLOR_GRAYTEXT;
	m_colorTextHighlight=RGB(0,0,100);
	m_colorSliderArrowsHighlight=RGB(0,0,100);

	SelectBrushAndEdge();

	CClientDC	dc( 0 );

	m_Font.CreateFont(
    MulDiv(10, GetDeviceCaps(dc, LOGPIXELSY), 72),                        // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
    "Arial");	
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::~CVTWidthCtrl - Destructor

CVTWidthCtrl::~CVTWidthCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::OnDraw - Drawing function

void CVTWidthCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.

	CRect rc=rcBounds;
	CBrush brush;
	CDC memDC;
	memDC.CreateCompatibleDC(pdc);

	CBitmap bmp;
	CBitmap *oldBitmap;
	CPen *oldpen;
	CBrush *oldbrush;

	bmp.CreateCompatibleBitmap(pdc,rcBounds.Width(),rcBounds.Height());
	oldBitmap=memDC.SelectObject(&bmp);

	brush.CreateSolidBrush(m_Brush);
	oldbrush=(CBrush*)memDC.SelectObject(&brush);
	memDC.FillRect(rcBounds,&brush);
	
	if(m_IsMouseOver) 
		memDC.DrawEdge(&rc,m_Edge,BF_BOTTOMRIGHT);
	else
		memDC.DrawEdge(&rc,m_Edge,BF_RECT);

	if(!m_Checked)
		memDC.DrawEdge(&rc,BDR_RAISEDINNER ,BF_TOPLEFT);
	else
		memDC.DrawEdge(&rc,m_Edge,BF_TOPLEFT);

	DrawLineWidth(&memDC,rcBounds);

	CPen pen;
		
	pen.CreatePen(PS_SOLID,1,GetColorSlider());
	oldpen=(CPen*)memDC.SelectObject(&pen);

	if(m_IsMouseOver)
		if(!m_Orientation)
		{
			memDC.MoveTo(rcBounds.right-10,10);
			memDC.LineTo(rcBounds.right-10,rcBounds.bottom-10);
		}
		else
		{
			memDC.MoveTo(10,rcBounds.bottom-10);
			memDC.LineTo(rcBounds.right-10,rcBounds.bottom-10);
		}

	if(m_IsCreated)
	{
    	m_ButtonWidth.InvalidateRect(NULL);
		m_ButtonUp.InvalidateRect(NULL);
		m_ButtonDown.InvalidateRect(NULL);
		m_ButtonSlider.InvalidateRect(NULL);
	}
	else
	{				 
		CPen *oldpen;
		CBrush *oldbrush;
		CBrush brush(GetColorSliderArrows());
		CPen pen(PS_SOLID,1,GetColorSliderArrows());
		oldpen=(CPen*)memDC.SelectObject(&pen);
		oldbrush=(CBrush*)memDC.SelectObject(&brush);
		
		if(!m_Orientation)
		{
			CPoint points[3]={CPoint(rcBounds.right-6,10),CPoint(rcBounds.right-14,10),CPoint(rcBounds.right-10,3)};
			memDC.Polygon(points,3);
			CPoint points2[3]={CPoint(rcBounds.right-6,rcBounds.bottom-10),CPoint(rcBounds.right-14,rcBounds.bottom-10),CPoint(rcBounds.right-10,rcBounds.bottom-3)};
			memDC.Polygon(points2,3);
		} 
		else
		{
			CPoint points[3]={CPoint(3,rcBounds.bottom-10),CPoint(10,rcBounds.bottom-14),CPoint(10,rcBounds.bottom-4)};
			memDC.Polygon(points,3);
			CPoint points2[3]={CPoint(rcBounds.right-3,rcBounds.bottom-10),CPoint(rcBounds.right-10,rcBounds.bottom-14),CPoint(rcBounds.right-10,rcBounds.bottom-4)};
			memDC.Polygon(points2,3);
		}

		memDC.SelectObject(oldpen);
		memDC.SelectObject(oldbrush);
	}

	CRect rcFocus=rcBounds;
	rcFocus.DeflateRect(2,2);

	if(m_IsFocused) memDC.DrawFocusRect(&rcFocus);

	pdc->BitBlt(rcBounds.left,rcBounds.top,rcBounds.Width(),rcBounds.Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(oldpen);
	memDC.SelectObject(oldbrush);
	memDC.SelectObject(oldBitmap);

}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::DoPropExchange - Persistence support

void CVTWidthCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
	PX_Long(pPX,_T("Width"),m_Width,25);
	PX_Long(pPX,_T("Max"),m_Max,100);
	PX_Long(pPX,_T("Min"),m_Min,0);
	PX_Bool(pPX,_T("Visible"),m_visible,TRUE);
	PX_Bool(pPX,_T("Checked"),m_Checked,FALSE);
	PX_Bool(pPX,_T("Orientation"),m_Orientation,FALSE);
	PX_Color(pPX,"ColorText",m_colorText);
	PX_Color(pPX,"ColorTextScale",m_colorTextScale);
	PX_Color(pPX,"ColorTextHighlight",m_colorTextHighlight);
	PX_Color(pPX,"ColorSlider",m_colorSlider);
	PX_Color(pPX,"ColorSliderArrows",m_colorSliderArrows);
	
    if (pPX->GetVersion() >= MAKELONG(1, 1))
    {
        PX_Color(pPX,"ColorSliderArrowsHighlight",m_colorSliderArrowsHighlight);
    }
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl::OnResetState - Reset control to default state

void CVTWidthCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl message handlers

long CVTWidthCtrl::GetWidth() 
{
	return m_Width;
}

void CVTWidthCtrl::SetWidth(long nNewValue) 
{
	if(nNewValue>=m_Min && nNewValue<=m_Max)
	{
	 m_Width=nNewValue;
		if(m_IsCreated && !m_IsSliderMoved)
		{
			int offset;
			if(!m_Orientation)
			{
			offset= m_ButtonSlider.m_SliderTo - 5 - 
				         (m_Width - m_Min)*(m_ButtonSlider.m_SliderTo - 5 - m_ButtonSlider.m_SliderFrom) / 
						 (m_Max - m_Min) - m_ButtonSlider.m_SliderFrom;
			}
			else
			{
				offset=m_ButtonSlider.m_SliderTo - 5 - 
				         (m_Max-m_Width )*(m_ButtonSlider.m_SliderTo - 5 - m_ButtonSlider.m_SliderFrom) / 
						 (m_Max - m_Min) - m_ButtonSlider.m_SliderFrom;
			}
			CRect rcParent,rcButtonUp;
			GetWindowRect(rcParent);
			m_ButtonUp.GetWindowRect(rcButtonUp);
			rcButtonUp.OffsetRect(-rcParent.left,-rcParent.top);

			if(m_Orientation) m_ButtonSlider.SetWindowPos(NULL,rcButtonUp.right+offset,rcButtonUp.top+2,5,9,SWP_NOZORDER);
			else m_ButtonSlider.SetWindowPos(NULL,rcButtonUp.left+2,rcButtonUp.bottom+offset,9,5,SWP_NOZORDER);
			
		}

    Refresh();
	SetModifiedFlag();
    FireOnChange();
	}
}

BOOL CVTWidthCtrl::GetChecked() 
{
	return m_Checked;
}

void CVTWidthCtrl::SetChecked(BOOL bNewValue) 
{
	m_Checked=bNewValue;
	SelectBrushAndEdge();
//	FireClick();
	 
	Refresh();

	SetModifiedFlag();
}

long CVTWidthCtrl::GetMin() 
{
	return m_Min;
}

void CVTWidthCtrl::SetMin(long nNewValue) 
{
	m_Min=nNewValue;

	SetModifiedFlag();
}

long CVTWidthCtrl::GetMax() 
{
	return m_Max;
}

void CVTWidthCtrl::SetMax(long nNewValue) 
{
	m_Max=nNewValue;

	SetModifiedFlag();
}

BOOL CVTWidthCtrl::GetOrientation() 
{
	return m_Orientation;
}

void CVTWidthCtrl::SetOrientation(BOOL bNewValue) 
{
	m_Orientation=bNewValue;

	if(m_IsCreated)
	{
		CRect rect;
		GetClientRect(rect);
		OnSize(0,rect.Width(),rect.Height());
	}
	Refresh();
	SetModifiedFlag();
}

void CVTWidthCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_IsMouseOver)
	{
		SetTimer(1,55,NULL);
		SetMouseOver(TRUE);
		Refresh();
	}
	
	COleControl::OnMouseMove(nFlags, point);
}

int CVTWidthCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_IsCreated=TRUE;
	m_IsMouseOver=FALSE;
	CRect rectUp(75,10,88,23);
	CRect rectDown(75,70,88,83);

	m_ButtonWidth.Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(10,10,27,23),this,IDC_BUTTONWIDTH);
	m_ButtonUp.Create(_T(""),WS_CHILD|WS_VISIBLE,rectUp,this,IDC_BUTTONUP);
	m_ButtonDown.Create(_T(""),WS_CHILD|WS_VISIBLE,rectDown,this,IDC_BUTTONDOWN);
	m_ButtonSlider.Create(_T(""),WS_CHILD,CRect(10,10,18,13),this,IDC_BUTTONSLIDER);
	
	m_ButtonUp.m_IsAutoRepeat=TRUE;
	m_ButtonUp.m_IsArrow=TRUE;
	m_ButtonUp.m_IsUpButton=TRUE;
	m_ButtonDown.m_IsAutoRepeat=TRUE;
	m_ButtonDown.m_IsArrow=TRUE;
	m_ButtonDown.m_IsUpButton=FALSE;

	m_ButtonSlider.m_IsMovable=TRUE;
	m_ButtonSlider.SetBmp(0);
//	SetOrientation(m_Orientation);
	InstallSlider();
	
//	if(m_visible) ShowWindow(SW_SHOW);
//	else ShowWindow(SW_HIDE);
	return 0;
}

void CVTWidthCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rect;
    GetWindowRect (rect);

	BOOL bLButtonIsDown = (::GetKeyState(VK_LBUTTON) < 0);

	if(!rect.PtInRect(pt) && !bLButtonIsDown)
	{
		SetMouseOver(FALSE);//m_IsMouseOver=FALSE;
		KillTimer(1);
		Refresh();
	}

	COleControl::OnTimer(nIDEvent);
}

void CVTWidthCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(rect);
	if(!m_Orientation)
	{
		rect.left=rect.right-13;
		rect.right-=3;
		rect.top+=13;
		rect.bottom-=13;
	}
	else
	{
		rect.top=rect.bottom-13;
		rect.bottom-=3;
		rect.left+=13;
		rect.right-=13;
	}


	if(rect.PtInRect(point))
	{
		if(m_Orientation)
		{
			if(rect.right-point.x<5)
				point.x=rect.right-5;
			m_ButtonSlider.SetWindowPos(NULL,point.x,rect.top-1,0,0,SWP_NOSIZE | SWP_NOZORDER);
			m_ButtonSlider.m_SliderCurrent=point.x;
		}
		else
		{	
			if(rect.bottom-point.y<5)
			point.y=rect.bottom-5;

//			point.y=rect.CenterPoint().y-3;
			m_ButtonSlider.SetWindowPos(NULL,rect.left-1,point.y,0,0,SWP_NOSIZE | SWP_NOZORDER);
			m_ButtonSlider.m_SliderCurrent=point.y;
		}
		OnButtonSliderClicked();
		CPoint	ptSlider = point;
		ClientToScreen( &ptSlider );
		m_ButtonSlider.ScreenToClient( &ptSlider );

		SetChecked(true);

		SendMessage(WM_SETFOCUS);

		::SendMessage( m_ButtonSlider.GetSafeHwnd(), 
			WM_LBUTTONDOWN, /*MAKEWPARAM(*/MK_LBUTTON/*,0)*/, 
			MAKELPARAM(ptSlider.x,ptSlider.y));
		return;
	
	}
	else
		SetChecked(!m_Checked);

	FireClick();

	COleControl::OnLButtonDown(nFlags, point);
}

void CVTWidthCtrl::OnButtonUpClicked()
{
	if(m_Orientation)
		SetWidth(m_Width-1);
	else
	SetWidth(m_Width+1);
}

void CVTWidthCtrl::OnButtonDownClicked()
{
	if(m_Orientation)
		SetWidth(m_Width+1);
	else
	SetWidth(m_Width-1);
}

void CVTWidthCtrl::OnButtonSliderClicked()
{
	long width;
	if(!m_Orientation)
		width=m_Max-(m_Max-m_Min)*(m_ButtonSlider.m_SliderCurrent-m_ButtonSlider.m_SliderFrom)/(m_ButtonSlider.m_SliderTo-5-m_ButtonSlider.m_SliderFrom);
	else
		width=m_Min+(m_Max-m_Min)*(m_ButtonSlider.m_SliderCurrent-m_ButtonSlider.m_SliderFrom)/(m_ButtonSlider.m_SliderTo-5-m_ButtonSlider.m_SliderFrom);
	m_IsSliderMoved=TRUE;
	SetWidth(width);
	m_IsSliderMoved=FALSE;
}

void CVTWidthCtrl::OnButtonWidthClicked()
{
  m_WidthDlg.m_Max=this->m_Max;
  m_WidthDlg.m_Min=this->m_Min;
  m_WidthDlg.m_Width=this->m_Width;
  int result=m_WidthDlg.DoModal();
  switch(result)
  {
	  case -1: MessageBox(_T("Cannot create dialog box!")); break;
	  case IDABORT: break;
	  case IDCANCEL: break;
	  default: SetWidth(m_WidthDlg.m_nWidth); break;
  }

}

void CVTWidthCtrl::DrawLineWidth(CDC *pdc, CRect rcBounds)
{
  CRect rcForstr(3,3,25,15);
  POINT center;
  CBrush Brush;
  CPen pen;
  CBrush* oldBrush;
  
  if(m_IsMouseOver) 
  {
	  Brush.CreateSolidBrush(GetColorTextHighlight());
	  pen.CreatePen(PS_SOLID,1,GetColorTextHighlight());
  }
  else
  {
	  Brush.CreateSolidBrush(GetColorText());
	  pen.CreatePen(PS_SOLID,1,GetColorText());
  }

  oldBrush=pdc->SelectObject(&Brush);
  center.x=rcBounds.Width()/2;
  center.y=rcBounds.Height()/2;

  int bound =(center.x<center.y) ? center.x-12 : center.y-12;

  int k=(int) (((m_Width+10)/2)/bound+1);
  CRect rc(center.x-m_Width/(2*k),center.y-m_Width/(2*k),center.x+m_Width/(2*k),center.y+m_Width/(2*k));
  if(k>1)
  {
	 char str[10]="";
	 //str.FormatMessage("1:%i",k);

	 sprintf(str,"1:%i",k);
	 pdc->SetBkColor(m_Brush);
	 COLORREF clrOld=pdc->SetTextColor(GetColorTextScale());
	 CFont *oldFont;
	 oldFont=(CFont*)pdc->SelectObject(&m_Font);
	 pdc->DrawText(str,-1,&rcForstr,DT_SINGLELINE | DT_VCENTER );
	 pdc->SelectObject(oldFont);
	 pdc->SetTextColor(clrOld);
  }

  rc.InflateRect(1,1);   
  if(m_Checked)	rc.OffsetRect(1,1);

  CPen* oldPen;
  oldPen = (CPen*) pdc->SelectObject(&pen);
  pdc->Ellipse(rc);

  pdc->SelectObject(oldBrush);
  pdc->SelectObject(oldPen);

}





void CVTWidthCtrl::OnVisibleChanged() 
{
	// TODO: Add notification handler code

	if(m_IsCreated)
    if(m_visible) ShowWindow(SW_SHOW);
	else ShowWindow(SW_HIDE);

	SetModifiedFlag();
}



BOOL CVTWidthCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style |= WS_CLIPCHILDREN;

	return COleControl::PreCreateWindow(cs);
}

void CVTWidthCtrl::SetMouseOver(BOOL bNewVal)
{
	m_IsMouseOver=bNewVal;
	if(m_IsMouseOver)
		m_ButtonSlider.ShowWindow(SW_SHOW);
	else
		m_ButtonSlider.ShowWindow(SW_HIDE);
	SelectBrushAndEdge();
}

void CVTWidthCtrl::SelectBrushAndEdge()
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
			m_Edge=BDR_SUNKENOUTER;
		}
		else
		{
			COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
			COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
			COLORREF clrDither = RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
				                     GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
									 GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

			m_Brush=clrDither;
			m_Edge=BDR_SUNKENOUTER;
		}
	}

}


void CVTWidthCtrl::InstallSlider()
{
	CRect rectUp,rectDown,rectParent;
	m_ButtonUp.GetWindowRect(rectUp);
	m_ButtonDown.GetWindowRect(rectDown);
	GetWindowRect(rectParent);
	long curr;

	if(m_Orientation)  //horizontal
	{
		m_ButtonSlider.m_SliderFrom=rectUp.right-rectParent.left;
		m_ButtonSlider.m_SliderTo=rectDown.left-rectParent.left;
		curr=m_ButtonSlider.m_SliderTo-5-(m_Width-m_Min)*(m_ButtonSlider.m_SliderTo-5-m_ButtonSlider.m_SliderFrom)/(m_Max-m_Min)-m_ButtonSlider.m_SliderFrom;
		m_ButtonSlider.SetWindowPos(NULL,rectUp.right+curr-rectParent.left,rectUp.top-rectParent.top+2,5,9,SWP_NOZORDER);
		//m_ButtonSlider.SetBmp(IDB_BITMAPSLIDERH);
	}
	else
	{
		m_ButtonSlider.m_SliderFrom=rectUp.bottom-rectParent.top;
		m_ButtonSlider.m_SliderTo=rectDown.top-rectParent.top;
		curr=m_ButtonSlider.m_SliderTo-5-(m_Width-m_Min)*(m_ButtonSlider.m_SliderTo-5-m_ButtonSlider.m_SliderFrom)/(m_Max-m_Min)-m_ButtonSlider.m_SliderFrom;
		m_ButtonSlider.SetWindowPos(NULL,rectUp.left-rectParent.left+2,rectUp.bottom+curr-rectParent.top,9,5,SWP_NOZORDER);
		//m_ButtonSlider.SetBmp(IDB_BITMAPSLIDERV);
	}

}

void CVTWidthCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);

	CRect rectUp,rectDown,rectWidth;
	m_ButtonUp.GetClientRect(rectUp);
	m_ButtonDown.GetClientRect(rectDown);
	m_ButtonWidth.GetClientRect(rectWidth);

	 if(!m_Orientation)     // vertical
	 {
		 m_ButtonUp.SetWindowPos(NULL,cx-rectUp.Width()-OFST,OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
		 m_ButtonDown.SetWindowPos(NULL,cx-rectUp.Width()-OFST,cy-rectUp.Height()-OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
		 m_ButtonWidth.SetWindowPos(NULL,OFST,cy-rectWidth.Height()-OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
	 }
	 else
	 {
		 m_ButtonUp.SetWindowPos(NULL,OFST,cy-rectUp.Height()-OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
		 m_ButtonDown.SetWindowPos(NULL,cx-rectUp.Width()-OFST,cy-rectUp.Height()-OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
		 m_ButtonWidth.SetWindowPos(NULL,cx-rectWidth.Width()-OFST,OFST,0,0,SWP_NOSIZE | SWP_NOZORDER);
	 }
	 InstallSlider();
}

UINT CVTWidthCtrl::OnGetDlgCode() 
{
	return DLGC_WANTARROWS;
}

void CVTWidthCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_SPACE: OnButtonWidthClicked(); break;
	case VK_UP: OnButtonUpClicked(); break;
	case VK_DOWN: OnButtonDownClicked(); break;
	}
//	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVTWidthCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);

	m_IsFocused=FALSE;
	Refresh();
	
}

void CVTWidthCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);

	m_IsFocused=TRUE;
	Refresh();
	
	// TODO: Add your message handler code here
	
}

void CVTWidthCtrl::OnColorTextChanged() 
{
	Refresh();
	SetModifiedFlag();
}

void CVTWidthCtrl::OnColorTextHighlightChanged() 
{
	// TODO: Add notification handler code

	Refresh();
	SetModifiedFlag();
}

COLORREF CVTWidthCtrl::GetColorText()
{
	return TranslateColor(m_colorText);
}

COLORREF CVTWidthCtrl::GetColorTextHighlight()
{
	return TranslateColor(m_colorTextHighlight);
}

void CVTWidthCtrl::OnColorSliderChanged() 
{
	Refresh();

	SetModifiedFlag();
}

COLORREF CVTWidthCtrl::GetColorSlider()
{
	return TranslateColor(m_colorSlider);
}

void CVTWidthCtrl::OnColorSliderArrowsChanged() 
{
	Refresh();

	SetModifiedFlag();
}

COLORREF CVTWidthCtrl::GetColorSliderArrows()
{
	return TranslateColor(m_colorSliderArrows);
}

void CVTWidthCtrl::OnColorTextScaleChanged() 
{
	Refresh();
	SetModifiedFlag();
}

COLORREF CVTWidthCtrl::GetColorTextScale()
{
 return TranslateColor(m_colorTextScale);
}

BOOL CVTWidthCtrl::OnEraseBkgnd(CDC* pDC) 
{
	//return COleControl::OnEraseBkgnd(pDC);
	return false;
}

void CVTWidthCtrl::Check(bool bNewVal)
{
	FireClick();
	SetChecked(bNewVal);
}

void CVTWidthCtrl::OnColorSliderArrowsHighlightChanged() 
{
	Refresh();
	SetModifiedFlag();
}

COLORREF CVTWidthCtrl::GetColorSliderArrowsHighlight()
{
return TranslateColor(m_colorSliderArrowsHighlight);
}
