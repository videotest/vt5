// ShadingPicker.cpp : implementation file
//
// ShadingPicker is a drop-in colour picker control. Check out the 
// header file or the accompanying HTML doc file for details.
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// Updated 16 May 1998
//         31 May 1998 - added Default text (CJM)
//          9 Jan 1999 - minor vis update

#include "stdafx.h"
#include "ShadingPopup.h"
#include "ShadingPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_ShadingPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CShadingPicker* pShadingPicker = (CShadingPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        crColour = pShadingPicker->GetColour();
    }
    else // initializing
    {
        pShadingPicker->SetColour(crColour);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker

CShadingPicker::CShadingPicker()
{
    m_bTrackSelection = TRUE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;
	m_indexShading = -1;
}

CShadingPicker::~CShadingPicker()
{
}

IMPLEMENT_DYNCREATE(CShadingPicker, CButton)

BEGIN_MESSAGE_MAP(CShadingPicker, CButton)
    //{{AFX_MSG_MAP(CShadingPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker message handlers

LRESULT CShadingPicker::OnSelEndOK(WPARAM lParam, LPARAM wParam)
{
	short nNewShading = (short)wParam;
    m_bActive = FALSE;
	SetIndexShading(nNewShading);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

	if(nNewShading != GetIndexShading())
        if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

LRESULT CShadingPicker::OnSelEndCancel(WPARAM lParam, LPARAM wParam)
{
    m_bActive = FALSE;
	SetIndexShading((short)wParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LRESULT CShadingPicker::OnSelChange(WPARAM lParam, LPARAM wParam)
{
	if (m_bTrackSelection)
		SetIndexShading((short)wParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CShadingPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CColourPopup window for colour selection
BOOL CShadingPicker::OnClicked()
{
    m_bActive = TRUE;
    CRect rect;
    GetWindowRect(rect);
    new CShadingPopup(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColour(),                        // Selected colour
                     this);                              // parent

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColour(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

void CShadingPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    ASSERT(lpDrawItemStruct);
    
    CDC*    pDC     = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect   rect    = lpDrawItemStruct->rcItem;
    UINT    state   = lpDrawItemStruct->itemState;
    CString m_strText;

    CSize Margins(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Draw arrow
    if (m_bActive) state |= ODS_SELECTED;
    pDC->DrawFrameControl(&m_ArrowRect, DFC_SCROLL, DFCS_SCROLLDOWN  | 
                          ((state & ODS_SELECTED) ? DFCS_PUSHED : 0) |
                          ((state & ODS_DISABLED) ? DFCS_INACTIVE : 0));

    pDC->DrawEdge(rect, EDGE_SUNKEN, BF_RECT);

    // Must reduce the size of the "client" area of the button due to edge thickness.
    rect.DeflateRect(Margins.cx, Margins.cy);

    // Fill remaining area with colour
    rect.right -= m_ArrowRect.Width();

    CBrush brush( ((state & ODS_DISABLED) || m_crColourBk == CLR_DEFAULT)? 
                  ::GetSysColor(COLOR_3DFACE) : m_crColourBk);
    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
	pDC->SelectStockObject(NULL_PEN);
    pDC->Rectangle(rect);
    pDC->SelectObject(pOldBrush);

	if(GetIndexShading()>=0)
	{
		for(int i=0;;i++)
		{
			if(rect.left+16*(i+1)<=rect.right)
			{
				HICON hShadingIcon;
				hShadingIcon = AfxGetApp()->LoadIcon( IDI_SHADING1+GetIndexShading() );
				::DrawIconEx( pDC->GetSafeHdc(), rect.left+16*i , rect.top-1 , hShadingIcon, 16, 16, 1, NULL, DI_NORMAL);
				DestroyIcon( hShadingIcon );
			}
			else
				break;
		}
	}
    // Draw focus rect
    if (state & ODS_FOCUS) 
    {
        rect.DeflateRect(1,1);
        pDC->DrawFocusRect(rect);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker overrides

void CShadingPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker attributes

COLORREF CShadingPicker::GetColour()
{ 
    return GetBkColour(); 
}

void CShadingPicker::SetColour(COLORREF crColour)
{ 
    SetBkColour(crColour); 
}

void CShadingPicker::SetBkColour(COLORREF crColourBk)
{
    m_crColourBk = crColourBk;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CShadingPicker::SetIndexShading(short nValue)
{
	m_indexShading=nValue; 
	if (IsWindow(m_hWnd)) RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
// CShadingPicker implementation

void CShadingPicker::SetWindowSize()
{
    // Get size dimensions of edges
    CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Get size of dropdown arrow
    int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
    int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
    CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

    // Get window size
    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

    // Set window size at least as wide as 2 arrows, and as high as arrow + margins
    int nWidth = max(rect.Width(), 2*ArrowSize.cx + 2*MarginSize.cx);
    MoveWindow(rect.left, rect.top, nWidth, rect.Height()/*ArrowSize.cy+2*MarginSize.cy*/, TRUE);

    // Get the new coords of this window
    GetWindowRect(rect);
    ScreenToClient(rect);

    // Get the rect where the arrow goes, and convert to client coords.
    m_ArrowRect.SetRect(rect.right - ArrowSize.cx - MarginSize.cx, 
                        rect.top + MarginSize.cy, rect.right - MarginSize.cx,
                        rect.bottom - MarginSize.cy);
}

void CShadingPicker::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	
	SetWindowSize();
}