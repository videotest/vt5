// PaletteCtl.cpp : Implementation of the CPaletteCtrl ActiveX Control class.

#include "stdafx.h"
#include "Palette.h"
#include "PaletteCtl.h"
#include "PalettePpg.h"
#include "FramesPropPage.h"
#include "color.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPaletteCtrl, COleControl);

static char* pszProfileHead = "Palette colors";

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPaletteCtrl, COleControl)
	//{{AFX_MSG_MAP(CPaletteCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_MESSAGE(OCM_DRAWITEM, OnOcmDrawItem)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CPaletteCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPaletteCtrl)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "Cols", m_cols, OnColsChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "Rows", m_rows, OnRowsChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "CellWidth", m_cellWidth, OnCellWidthChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "CellHeight", m_cellHeight, OnCellHeightChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "IntercellSpace", m_intercellSpace, OnIntercellSpaceChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "InnerSunken", m_innerSunken, OnInnerSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "OuterRaised", m_outerRaised, OnOuterRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "OuterSunken", m_outerSunken, OnOuterSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "BorderSize", m_borderSize, OnBorderSizeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CPaletteCtrl, "InnerRaised", m_innerRaised, OnInnerRaisedChanged, VT_BOOL)
	DISP_FUNCTION(CPaletteCtrl, "SetDefaultColors", SetDefaultColors, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CPaletteCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CPaletteCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPaletteCtrl)
	EVENT_CUSTOM("LeftClick", FireLeftClick, VTS_COLOR)
	EVENT_CUSTOM("RightClick", FireRightClick, VTS_COLOR)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPaletteCtrl, 2)
	PROPPAGEID(CPalettePropPage::guid)
	PROPPAGEID(CFramesPropPage::guid)
END_PROPPAGEIDS(CPaletteCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPaletteCtrl, "PALETTE.PaletteCtrl.1",
	0x57c4cb48, 0x9e87, 0x11d3, 0xa5, 0x37, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CPaletteCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DPalette =
		{ 0x57c4cb46, 0x9e87, 0x11d3, { 0xa5, 0x37, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };
const IID BASED_CODE IID_DPaletteEvents =
		{ 0x57c4cb47, 0x9e87, 0x11d3, { 0xa5, 0x37, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwPaletteOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKEBUTTON;

IMPLEMENT_OLECTLTYPE(CPaletteCtrl, IDS_PALETTE, _dwPaletteOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::CPaletteCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPaletteCtrl

BOOL CPaletteCtrl::CPaletteCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_PALETTE, IDB_PALETTE,
			afxRegApartmentThreading,
			_dwPaletteOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::CPaletteCtrl - Constructor

CPaletteCtrl::CPaletteCtrl() : m_sizeControl(30, 30)
{
	InitializeIIDs(&IID_DPalette, &IID_DPaletteEvents);

	// TODO: Initialize your control's instance data here.
	m_cols = 3;
	m_rows = 3;
	m_cellWidth = 20;
	m_cellHeight = 20;
	m_intercellSpace = 3;
	m_borderSize = 4;
	m_nPushedCell = -1;
	m_bRightClick = false;
	m_bMouseUp = false;
	m_innerRaised = FALSE;
	m_innerSunken = FALSE;
	m_outerRaised = FALSE;
	m_outerSunken = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::~CPaletteCtrl - Destructor

CPaletteCtrl::~CPaletteCtrl()
{
	// TODO: Cleanup your control's instance data here.
	_KillCellsInfoArray();
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::OnDraw - Drawing function

void CPaletteCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	pdc->FillRect(rcBounds, &CBrush(::GetSysColor(COLOR_BTNFACE)));

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
		pdc->DrawEdge(CRect(1,1,rcBounds.Width()-2,rcBounds.Height()-2), edgeType, BF_RECT);
	}
	DoSuperclassPaint(pdc, rcBounds);
	CellInfo* pCellInfo;
	CRect	rcCell;
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		pCellInfo = m_ptrCellsInfo[i];
		rcCell = pCellInfo->rcCell;
		pdc->DrawFrameControl(&rcCell, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_ADJUSTRECT);
		pdc->FillRect(&rcCell, &CBrush(pCellInfo->color));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::DoPropExchange - Persistence support

void CPaletteCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::OnResetState - Reset control to default state

void CPaletteCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	//_SetCellsInfoArray();
	//_ClearState();
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::AboutBox - Display an "About" box to the user

void CPaletteCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_PALETTE);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CPaletteCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	_SetCellsInfoArray();
	//_LoadState();
	cs.lpszClass = _T("BUTTON");
	cs.style |= BS_OWNERDRAW;
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::IsSubclassedControl - This is a subclassed control

BOOL CPaletteCtrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl::OnOcmCommand - Handle command messages

LRESULT CPaletteCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}


LRESULT CPaletteCtrl::OnOcmDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam; // item-drawing information 

	BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);

 	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CellInfo* pCellInfo;
	CRect	rcCell;

	/*if(bIsPressed)
	{
		TRACE0("bIsPressed");
	}*/
	
	
	if(bIsPressed == TRUE && m_nPushedCell >= 0)
	{
		//draw pushed button
		pCellInfo = m_ptrCellsInfo[m_nPushedCell];
		rcCell = pCellInfo->rcCell;
		pDC->DrawFrameControl(&rcCell, DFC_BUTTON, DFCS_PUSHED|DFCS_ADJUSTRECT|DFCS_BUTTONPUSH);
		pDC->FillRect(&rcCell, &CBrush(pCellInfo->color));
	}
	else if(m_nPushedCell >= 0)
	{
		pCellInfo = m_ptrCellsInfo[m_nPushedCell];
		rcCell = pCellInfo->rcCell;
		pDC->DrawFrameControl(&rcCell, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_ADJUSTRECT);
		pDC->FillRect(&rcCell, &CBrush(pCellInfo->color));
		if(m_bMouseUp)
		{
			m_bMouseUp = false;
			m_nPushedCell = -1;	
		}
			//m_nPushedCell = -1;
	}
	//TRACE("m_nPushedCell=%d\n", m_nPushedCell);
			
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl message handlers

void CPaletteCtrl::OnColsChanged() 
{
	// TODO: Add notification handler code
	_ClearState();
	_SetCellsInfoArray();
	SetModifiedFlag();
}

void CPaletteCtrl::OnRowsChanged() 
{
	// TODO: Add notification handler code
	_ClearState();
	_SetCellsInfoArray();
	SetModifiedFlag();
}


void CPaletteCtrl::SetDefaultColors() 
{
	_SetDefaultColors();
	_ClearState();
	InvalidateControl();
}

void CPaletteCtrl::_SetDefaultColors() 
{
	// TODO: Add your dispatch handler code here
	CColor	colorCnv;
	int nNumTrueColors = m_cols*(m_rows-1);
	//make true colors
	colorCnv.SetSaturation(1);
	colorCnv.SetLuminance(0.5);
	for(int i = 0; i < nNumTrueColors; i++)
	{
		colorCnv.SetHue(float(i*360.0/nNumTrueColors));
		m_ptrCellsInfo[i]->color = RGB(colorCnv.GetRed(), colorCnv.GetGreen(), colorCnv.GetBlue());
	}
	//make grayscale colors
	int nNumGrayscaleColors = m_cols;
	colorCnv.SetSaturation(0);
	colorCnv.SetHue(0);
	for(i = 0; i < nNumGrayscaleColors; i++)
	{
		colorCnv.SetLuminance(float( i*1.0/(nNumGrayscaleColors-1) ));
		m_ptrCellsInfo[i+nNumTrueColors]->color = RGB(colorCnv.GetRed(), colorCnv.GetGreen(), colorCnv.GetBlue());
	}
}

void CPaletteCtrl::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
		ar<<(int)m_cols;
		ar<<(int)m_rows;
		ar<<(int)m_cellWidth;
		ar<<(int)m_cellHeight;
		ar<<(int)m_intercellSpace;
		ar<<(int)m_sizeControl.cx;
		ar<<(int)m_sizeControl.cy;
		ar<<(BYTE)m_innerRaised;
		ar<<(BYTE)m_innerSunken;
		ar<<(BYTE)m_outerRaised;
		ar<<(BYTE)m_outerSunken;
		ar<<(int)m_borderSize;

		_SaveState();
	}
	else
	{	// loading code
		int nValue;
		ar>>nValue;
		m_cols = (short)nValue;
		ar>>nValue;
		m_rows = (short)nValue;
		ar>>nValue;
		m_cellWidth = (short)nValue;
		ar>>nValue;
		m_cellHeight = (short)nValue;
		ar>>nValue;
		m_intercellSpace = (short)nValue;
		ar>>nValue;
		m_sizeControl.cx = nValue;
		ar>>nValue;
		m_sizeControl.cy = nValue;
		BYTE byte;
		ar>>byte;
		m_innerRaised = (BOOL)byte;
		ar>>byte;
		m_innerSunken = (BOOL)byte;
		ar>>byte;
		m_outerRaised = (BOOL)byte;
		ar>>byte;
		m_outerSunken = (BOOL)byte;
		ar>>nValue;
		m_borderSize = (short)nValue;

		_LoadState();
	}
}

void CPaletteCtrl::OnCellWidthChanged() 
{
	// TODO: Add notification handler code
	_SetCellsInfoArray();
	SetModifiedFlag();
}

void CPaletteCtrl::OnCellHeightChanged() 
{
	// TODO: Add notification handler code
	_SetCellsInfoArray();
	SetModifiedFlag();
}

void CPaletteCtrl::_SetCellsInfoArray()
{
	_KillCellsInfoArray();
	CellInfo*	pCellInfo = 0;
	int nWidth = 0;
	int nHeight = 0;
	int nCellWidth = 0;
	int nCellHeight = 0;
	GetControlSize(&nWidth, &nHeight);
	nCellWidth = ((nWidth - m_borderSize*2 + m_intercellSpace)/m_cols) - m_intercellSpace;
	nCellHeight = ((nHeight - m_borderSize*2 + m_intercellSpace)/m_rows) - m_intercellSpace;
	//if(nCellHeight == m_cellHeight && nCellWidth == m_cellWidth) return;
	m_cellHeight = nCellHeight;
	m_cellWidth = nCellWidth;
	for(int j = 0; j < m_rows; j++)
	{
		for(int i = 0; i < m_cols; i++)	
		{
			pCellInfo = 0;
			pCellInfo = new CellInfo;
			VERIFY(pCellInfo != 0);
			pCellInfo->rcCell = CRect((i*(m_cellWidth+m_intercellSpace) + m_borderSize),
					    			  (j*(m_cellHeight+m_intercellSpace) + m_borderSize),
 									  (i*(m_cellWidth+m_intercellSpace) + m_borderSize + m_cellWidth),
									  (j*(m_cellHeight+m_intercellSpace) + m_borderSize + m_cellHeight));
			pCellInfo->color = RGB(0,0,0);
			pCellInfo->bDefault = true;
			m_ptrCellsInfo.Add(pCellInfo);
		}
	}
	int nUpBound = (int)m_ptrCellsInfo.GetUpperBound();
	m_sizeControl = CSize(30, 30);
	if (nUpBound >= 0)
	{
		m_sizeControl = m_ptrCellsInfo[nUpBound]->rcCell.BottomRight() - m_ptrCellsInfo[0]->rcCell.TopLeft();
		m_sizeControl += CSize(2*m_borderSize, 2*m_borderSize);
	}
	pCellInfo = 0;
	//SetControlSize(m_sizeControl.cx+3, m_sizeControl.cy+3);
	_SetDefaultColors();
	_LoadState();
	InvalidateControl();
}

void CPaletteCtrl::_KillCellsInfoArray()
{
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		delete m_ptrCellsInfo[i];
	}
	m_ptrCellsInfo.RemoveAll();
}

void CPaletteCtrl::OnIntercellSpaceChanged() 
{
	// TODO: Add notification handler code
	_SetCellsInfoArray();
	SetModifiedFlag();
}


void CPaletteCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	_DeterminatePushedCell(point);
	if(m_nPushedCell >= 0)
	{
		if(m_bRightClick)
			FireRightClick((OLE_COLOR)m_ptrCellsInfo[m_nPushedCell]->color);
		else
			FireLeftClick((OLE_COLOR)m_ptrCellsInfo[m_nPushedCell]->color);
	}
	COleControl::OnLButtonDown(nFlags, point);
}

void	CPaletteCtrl::_DeterminatePushedCell(CPoint point)
{
	CRect rcCell;
	int idx = -1;
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		rcCell = m_ptrCellsInfo[i]->rcCell;
		rcCell.InflateRect(0,0,1,1);
		if(rcCell.PtInRect(point))
		{
			m_nPushedCell = ++idx;
			break;
		}
		idx++;
	}
}

void CPaletteCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	_DeterminatePushedCell(point);
	if(m_nPushedCell >= 0)
	{
		int nCurCell = m_nPushedCell;
		CColorDialog	dlg;
		dlg.m_cc.Flags |= CC_RGBINIT;
		COLORREF color = m_ptrCellsInfo[nCurCell]->color;
		dlg.m_cc.rgbResult = color;
		dlg.DoModal();
		m_ptrCellsInfo[nCurCell]->color = dlg.GetColor();
		m_ptrCellsInfo[nCurCell]->bDefault = !((!m_ptrCellsInfo[nCurCell]->bDefault) || (m_ptrCellsInfo[nCurCell]->bDefault && m_ptrCellsInfo[nCurCell]->color != color));
		m_nPushedCell = -1;
		InvalidateControl();
	}
	COleControl::OnLButtonDblClk(nFlags, point);
}

void CPaletteCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bRightClick =  true;
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, (LPARAM)MAKELONG(point.x, point.y));
	//COleControl::OnRButtonDown(nFlags, point);
}

void CPaletteCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SendMessage(WM_LBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELONG(point.x, point.y));
	m_bRightClick =  false;
	//COleControl::OnRButtonUp(nFlags, point);
}

void CPaletteCtrl::_LoadState()
{
	CString	strEntry;
	CString	strValue;
	int R, G, B;
	COLORREF color = 0;
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		strEntry.Format("Cell%d%d", i/m_cols, i%m_cols);
		color = m_ptrCellsInfo[i]->color;
		strValue = AfxGetApp()->GetProfileString(pszProfileHead, strEntry, "NOVAL");
		if(strValue != "NOVAL")
		{
			VERIFY(sscanf(strValue, "%d %d %d", &R, &G, &B) == 3);
			color = RGB((BYTE)R, (BYTE)G, (BYTE)B);
			if(color != m_ptrCellsInfo[i]->color)
			{
				m_ptrCellsInfo[i]->color = color;
				m_ptrCellsInfo[i]->bDefault = false;
			}
		}
		//AfxGetApp()->WriteProfileString(pszProfileHead, strEntry, NULL);
	}
}

void CPaletteCtrl::_SaveState()
{
	CString	strEntry;
	CString	strValue;
	COLORREF color = 0;
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		if(!m_ptrCellsInfo[i]->bDefault)
		{
			strEntry.Format("Cell%d%d", i/m_cols, i%m_cols);
			color = m_ptrCellsInfo[i]->color;
			strValue.Format("%d %d %d", GetRValue(color), GetGValue(color), GetBValue(color));
			AfxGetApp()->WriteProfileString(pszProfileHead, strEntry, strValue);
		}
	}
}

void CPaletteCtrl::_ClearState()
{
	CString	strEntry;
	for(int i = 0; i <= m_ptrCellsInfo.GetUpperBound(); i++)
	{
		strEntry.Format("Cell%d%d", i/m_cols, i%m_cols);
		AfxGetApp()->WriteProfileString(pszProfileHead, strEntry, NULL);
	}
}

void CPaletteCtrl::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	_SaveState();
	COleControl::PostNcDestroy();
}

DWORD CPaletteCtrl::GetControlFlags() 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}

void CPaletteCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	_SetCellsInfoArray();	
	// TODO: Add your message handler code here
}

void CPaletteCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bMouseUp = true;
	COleControl::OnLButtonUp(nFlags, point);
}

void CPaletteCtrl::OnInnerSunkenChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPaletteCtrl::OnOuterRaisedChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPaletteCtrl::OnOuterSunkenChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CPaletteCtrl::OnBorderSizeChanged() 
{
	// TODO: Add notification handler code
	//_SetCellsInfoArray();
	//InvalidateControl();
	SendMessage(WM_SIZE);
	SetModifiedFlag();
}

void CPaletteCtrl::OnInnerRaisedChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}
