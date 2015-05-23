// ArrowPicker.cpp : implementation file
//

#include "stdafx.h"
#include "ArrowWnd.h"
#include "ArrowPicker.h"
#include "..\\resource.h"
#include "types.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_ArrowPicker(CDataExchange *pDX, int nIDC, ArrowType &arrowType)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CArrowPicker* pArrowPicker = (CArrowPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        arrowType = (ArrowType)pArrowPicker->GetArrow();
    }
    else // initializing
    {
        pArrowPicker->SetArrow((ArrowType)arrowType);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker

CArrowPicker::CArrowPicker()
{
    m_bTrackSelection = FALSE;
    m_bActive = FALSE;
	m_image.Create( IDB_ARROWS, 22, 0, RGB(255,0,0) );
}

CArrowPicker::~CArrowPicker()
{
}

IMPLEMENT_DYNCREATE(CArrowPicker, CButton)

BEGIN_MESSAGE_MAP(CArrowPicker, CButton)
    //{{AFX_MSG_MAP(CArrowPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(APN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(APN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(APN_SELCHANGEBT,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker message handlers

LRESULT CArrowPicker::OnSelEndOK(WPARAM lParam, LPARAM /*wParam*/)
{
    int arrowType = (int) lParam;
    m_bActive = FALSE;
    SetArrow(arrowType);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(APN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(APN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (arrowType != GetArrow())
        if (pParent) pParent->SendMessage(APN_SELCHANGEBT, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

LRESULT CArrowPicker::OnSelEndCancel(WPARAM lParam, LPARAM /*wParam*/)
{
    m_bActive = FALSE;
    SetArrow((int) lParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(APN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(APN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LRESULT CArrowPicker::OnSelChange(WPARAM lParam, LPARAM /*wParam*/)
{
    if (m_bTrackSelection) SetArrow((int) lParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(APN_SELCHANGEBT, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CArrowPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CColourPopupWnd window for colour selection
BOOL CArrowPicker::OnClicked()
{
    m_bActive = TRUE;
    CRect rect;
    GetWindowRect(rect);
    new CArrowWnd(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetArrow(),                       // Selected colour
                     this                              // parent
					 );

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(APN_DROPDOWN, (LPARAM)GetArrow(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

void CArrowPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    ASSERT(lpDrawItemStruct);
    
    CDC*    pDC     = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect   rect    = lpDrawItemStruct->rcItem;
    UINT    state   = lpDrawItemStruct->itemState;
    //CString m_strText;

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
    pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

	if( !IsWindowEnabled() )
		return;

 //center the arrow
    rect.DeflateRect(15, 0);
    
	POINT pt;
	pt.x=rect.left;
	pt.y=rect.top;
	
	
	
	m_image.Draw( pDC, m_Arrow, pt, ILD_TRANSPARENT );
		

/*
    CBrush brush( ((state & ODS_DISABLED) || m_crColourBk == CLR_DEFAULT)? 
                  ::GetSysColor(COLOR_3DFACE) : m_crColourBk);
    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
	pDC->SelectStockObject(NULL_PEN);
    pDC->Rectangle(rect);
    pDC->SelectObject(pOldBrush);
*/
    // Draw the window text (if any)
    // GetWindowText(m_strText);
    /*if (m_strText.GetLength())
    {
        pDC->SetBkMode(TRANSPARENT);
        if (state & ODS_DISABLED)
        {
            rect.OffsetRect(1,1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            rect.OffsetRect(-1,-1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
        else
        {
            pDC->SetTextColor((m_crColourText == CLR_DEFAULT)? 0 : m_crColourText);
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
    }
*/
    // Draw focus rect
	/*
    if (state & ODS_FOCUS) 
    {
        rect.DeflateRect(1,1);
        pDC->DrawFocusRect(rect);
    }
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker overrides

void CArrowPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker attributes

int CArrowPicker::GetArrow()
{ 
    return m_Arrow;
}

void CArrowPicker::SetArrow(int arrowType)
{ 
    m_Arrow=arrowType;
}

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker implementation

void CArrowPicker::SetWindowSize()
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
    MoveWindow(rect.left, rect.top, nWidth, ArrowSize.cy+2*MarginSize.cy, TRUE);

    // Get the new coords of this window
    GetWindowRect(rect);
    ScreenToClient(rect);

    // Get the rect where the arrow goes, and convert to client coords.
    m_ArrowRect.SetRect(rect.right - ArrowSize.cx - MarginSize.cx, 
                        rect.top + MarginSize.cy, rect.right - MarginSize.cx,
                        rect.bottom - MarginSize.cy);
}
