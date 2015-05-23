// ColourPicker.cpp : implementation file
//

#include "stdafx.h"
#include "ColourPopupWnd.h"
#include "ColourPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_ColourPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CColourPicker* pColourPicker = (CColourPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        crColour = pColourPicker->GetColour();
    }
    else // initializing
    {
        pColourPicker->SetColour(crColour);
    }
}

void AFXAPI DDX_ColourPicker(CDataExchange *pDX, int nIDC, BOOL& bTransparent)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CColourPicker* pColourPicker = (CColourPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        bTransparent = pColourPicker->GetTransparent();
    }
    else // initializing
    {
        pColourPicker->SetTransparent( bTransparent );
    }
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker

CColourPicker::CColourPicker()
{
    SetBkColour(GetSysColor(COLOR_3DFACE));
    SetTextColour(GetSysColor(COLOR_BTNTEXT));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

    m_strDefaultText = _T("Transparent");
    m_strCustomText  = _T("More Colours...");

	m_bTransparent	 = FALSE;
}

CColourPicker::~CColourPicker()
{
}

IMPLEMENT_DYNCREATE(CColourPicker, CButton)

BEGIN_MESSAGE_MAP(CColourPicker, CButton)
    //{{AFX_MSG_MAP(CColourPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGEBT,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColourPicker message handlers

LRESULT CColourPicker::OnSelEndOK(WPARAM lParam, LPARAM /*wParam*/)
{
    COLORREF crNewColour = (COLORREF) lParam;
    m_bActive = FALSE;
    SetColour(crNewColour);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (crNewColour != GetColour())
        if (pParent) pParent->SendMessage(CPN_SELCHANGEBT, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

LRESULT CColourPicker::OnSelEndCancel(WPARAM lParam, LPARAM /*wParam*/)
{
    m_bActive = FALSE;
    SetColour((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LRESULT CColourPicker::OnSelChange(WPARAM lParam, LPARAM /*wParam*/)
{
    if (m_bTrackSelection) SetColour((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(CPN_SELCHANGEBT, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CColourPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CColourPopupWnd window for colour selection
BOOL CColourPicker::OnClicked()
{
    m_bActive = TRUE;
    CRect rect;
    GetWindowRect(rect);
    new CColourPopupWnd(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColour(),                       // Selected colour
                     this,                              // parent
                     m_strDefaultText,                  // "Default" text area
                     m_strCustomText);                  // Custom Text

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColour(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

void CColourPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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

    CBrush brush( ((state & ODS_DISABLED) || m_crColourBk == CLR_DEFAULT ||(m_bTransparent) )? 
                  ::GetSysColor(COLOR_3DFACE) : m_crColourBk);
    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
	pDC->SelectStockObject(NULL_PEN);
    pDC->Rectangle(rect);
    pDC->SelectObject(pOldBrush);

    // Draw the window text (if any)
    //GetWindowText(m_strText);

	if( m_bTransparent )
		m_strText = "Transparent";
	else
		m_strText = "";


    if (m_strText.GetLength())
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
			if( m_bTransparent )
				pDC->SetTextColor( 0 );
			else
				pDC->SetTextColor((m_crColourText == CLR_DEFAULT)? 0 : m_crColourText);

            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
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
// CColourPicker overrides

void CColourPicker::SetTransparent( BOOL bTransparent)
{
	m_bTransparent = bTransparent;
	if (IsWindow(m_hWnd)) RedrawWindow();
}

BOOL CColourPicker::GetTransparent()
{
	return m_bTransparent;
}


void CColourPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker attributes

COLORREF CColourPicker::GetColour()
{ 
    return (m_nSelectionMode == CP_MODE_TEXT)? 
        GetTextColour(): GetBkColour(); 
}

void CColourPicker::SetColour(COLORREF crColour)
{ 
    (m_nSelectionMode == CP_MODE_TEXT)? 
        SetTextColour(crColour): SetBkColour(crColour); 
}

void CColourPicker::SetBkColour(COLORREF crColourBk)
{
    m_crColourBk = crColourBk;

	if( m_crColourBk == CLR_DEFAULT )
		SetTransparent( TRUE );
	else
		SetTransparent( FALSE );

    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CColourPicker::SetTextColour(COLORREF crColourText)
{
    m_crColourText = crColourText;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CColourPicker::SetDefaultText(LPCTSTR szDefaultText)
{
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
}

void CColourPicker::SetCustomText(LPCTSTR szCustomText)
{
    m_strCustomText = (szCustomText)? szCustomText : _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker implementation

void CColourPicker::SetWindowSize()
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
