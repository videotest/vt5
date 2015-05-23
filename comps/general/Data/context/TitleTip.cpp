////////////////////////////////////////////////////////////////////////////
// TitleTip.cpp : implementation file
//
#include "stdafx.h"
#include "ContextTree.h"

#include "TitleTip.h"

#ifndef TITLETIP_CLASSNAME 
#define TITLETIP_CLASSNAME _T("VTTitleTip")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleTip

CTitleTip::CTitleTip()
{
	// Register the window class if it has not already been registered.
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if (!(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor(hInst, IDC_ARROW);
		wndcls.hbrBackground	= (HBRUSH)(COLOR_INFOBK +1);
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= TITLETIP_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}

    m_dwLastLButtonDown = ULONG_MAX;
    m_dwDblClickMsecs = GetDoubleClickTime();

	m_nTimer = 0;
	m_nCount = 0;
	m_rectShow = CRect(0, 0, 0, 0);
	m_clrBack = m_clrText = CLR_DEFAULT;
	m_pFont = 0;
	m_strShowTitle = _T("");
}

CTitleTip::~CTitleTip()
{
}

void CTitleTip::OnDestroy()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}

	CWnd::OnDestroy();
}

BEGIN_MESSAGE_MAP(CTitleTip, CWnd)
	//{{AFX_MSG_MAP(CTitleTip)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTitleTip message handlers

BOOL CTitleTip::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);

	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	m_pParentWnd = pParentWnd;

	return CreateEx(dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		            NULL, NULL, NULL);
}


// Show 		 - Show the titletip if needed
// rectTitle	 - The rectangle within which the original 
//				    title is constrained - in client coordinates
// lpszTitleText - The text to be displayed
// xoffset		 - Number of pixel that the text is offset from
//				   left border of the cell
void CTitleTip::Show(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*=0*/, 
					 int nExtent /* = 0*/, LPRECT lpHoverRect /*=NULL*/, CFont * pFont /*=NULL*/,
                     COLORREF crTextClr /* CLR_DEFAULT */, COLORREF crBackClr /* CLR_DEFAULT */)
{
	Hide();
	ASSERT(::IsWindow(GetSafeHwnd()));

    if (rectTitle.IsRectEmpty())
        return;

	// If titletip is already displayed, don't do anything.
	if (IsWindowVisible()) 
		return;

    m_rectHover = (lpHoverRect != NULL)? lpHoverRect : rectTitle;
    m_rectHover.right++; m_rectHover.bottom++;

	m_pParentWnd->ClientToScreen(m_rectHover);
    ScreenToClient(m_rectHover);

	// Do not display the titletip is app does not have focus
	if (GetFocus() == NULL)
		return;

	// Define the rectangle outside which the titletip will be hidden.
	// We add a buffer of one pixel around the rectangle
	m_rectTitle.top    = -1;
	m_rectTitle.left   = -xoffset-1;
	m_rectTitle.right  = rectTitle.Width()-xoffset;
	m_rectTitle.bottom = rectTitle.Height()+1;

	// Determine the width of the text
	m_pParentWnd->ClientToScreen(rectTitle);

	CClientDC dc(this);
	CString strTitle = _T("");
    strTitle += lpszTitleText; 

	CFont *pOldFont = dc.SelectObject(pFont ? pFont : m_pParentWnd->GetFont());
	CSize size = dc.GetTextExtent(strTitle);
    dc.SelectObject(pOldFont);

    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    size.cx += 2*nExtent;// tm.tmOverhang;

	CRect rectDisplay = rectTitle;
	rectDisplay.left += xoffset;
	rectDisplay.right = rectDisplay.left + size.cx + xoffset;
    
    // Do not display if the text fits within available space
    if (rectDisplay.right > rectTitle.right-xoffset)
    {
//		rectDisplay.right += 
		// save for future use

		{
			HDC hDC = ::GetDC( 0 );
			int nW = ::GetDeviceCaps( hDC, HORZRES );
			int nH = ::GetDeviceCaps( hDC, VERTRES );

			if( rectDisplay.right > nW )
				rectDisplay.OffsetRect( nW - rectDisplay.right, 0 );

			if( rectDisplay.bottom > nH )
				rectDisplay.OffsetRect( 0, nH - rectDisplay.bottom );

			::ReleaseDC( 0, hDC );
		}

		m_rectShow = rectDisplay;
		m_clrBack = crBackClr;
		m_clrText = crTextClr;
		m_pFont = pFont;
		m_strShowTitle = strTitle;

		if (m_nTimer)
		{
			KillTimer(m_nTimer);
			m_nTimer = 0;
		}
		m_nTimer = SetTimer(WM_TIMER, 50, 0);
    }
    
}

void CTitleTip::StartShow()
{
	CClientDC dc(this);
    
	CFont *pOldFont = dc.SelectObject(m_pFont ? m_pFont : m_pParentWnd->GetFont());

	// Show the titletip
    SetWindowPos(&wndTop, m_rectShow.left, m_rectShow.top, 
				 m_rectShow.Width(), m_rectShow.Height(), 
				 SWP_SHOWWINDOW|SWP_NOACTIVATE);
    
    // FNA - handle colors correctly
    if (m_clrBack != CLR_DEFAULT)
    {
		CBrush backBrush(m_clrBack);
		CBrush* pOldBrush = dc.SelectObject(&backBrush);
		CRect rect;
		dc.GetClipBox(&rect);     // Erase the area needed 

		dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),  PATCOPY);
		dc.SelectObject(pOldBrush);
	}
    // Set color
    if (m_clrText != CLR_DEFAULT)
        dc.SetTextColor(m_clrText);

    dc.SetBkMode(TRANSPARENT);
	CRect rect;
	GetClientRect(rect);
    dc.DrawText(m_strShowTitle, rect, DT_VCENTER|DT_CENTER);
    SetCapture();
}

void CTitleTip::Hide()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}
//	m_nCount = 0;
  	if (!::IsWindow(GetSafeHwnd()))
        return;

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
        ReleaseCapture();

	ShowWindow(SW_HIDE);
}

void CTitleTip::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (!m_rectHover.PtInRect(point)) 
    {
        Hide();
        
        // Forward the message
        ClientToScreen(&point);
        CWnd *pWnd = WindowFromPoint(point);
        if (pWnd == this) 
            pWnd = m_pParentWnd;
        
        int hittest = (int)pWnd->SendMessage(WM_NCHITTEST, 0, MAKELONG(point.x,point.y));
        
        if (hittest == HTCLIENT) 
		{
            pWnd->ScreenToClient(&point);
            pWnd->PostMessage(WM_MOUSEMOVE, nFlags, MAKELONG(point.x,point.y));
        } 
		else 
		{
            pWnd->PostMessage(WM_NCMOUSEMOVE, hittest, MAKELONG(point.x,point.y));
        }
    }
}

BOOL CTitleTip::PreTranslateMessage(MSG* pMsg) 
{
    // Used to qualify WM_LBUTTONDOWN messages as double-clicks
    DWORD dwTick=0;
    BOOL bDoubleClick=FALSE;

    CWnd *pWnd;
	int hittest;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
       // Get tick count since last LButtonDown
        dwTick = GetTickCount();
        bDoubleClick = ((dwTick - m_dwLastLButtonDown) <= m_dwDblClickMsecs);
        m_dwLastLButtonDown = dwTick;
        // NOTE: DO NOT ADD break; STATEMENT HERE! Let code fall through

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
		POINTS pts = MAKEPOINTS(pMsg->lParam);
		POINT  point;
		point.x = pts.x;
		point.y = pts.y;
		ClientToScreen(&point);
		pWnd = WindowFromPoint(point);
		if (pWnd == this) 
			pWnd = m_pParentWnd;

		hittest = (int)pWnd->SendMessage(WM_NCHITTEST, 0, MAKELONG(point.x,point.y));

		if (hittest == HTCLIENT) 
		{
			pWnd->ScreenToClient(&point);
			pMsg->lParam = MAKELONG(point.x,point.y);
		} 
		else 
		{
			switch (pMsg->message) 
			{
			case WM_LBUTTONDOWN: 
				pMsg->message = WM_NCLBUTTONDOWN;
				break;
			case WM_RBUTTONDOWN: 
				pMsg->message = WM_NCRBUTTONDOWN;
				break;
			case WM_MBUTTONDOWN: 
				pMsg->message = WM_NCMBUTTONDOWN;
				break;
			}
			pMsg->wParam = hittest;
			pMsg->lParam = MAKELONG(point.x,point.y);
		}

        Hide();

        // If this is the 2nd WM_LBUTTONDOWN in x milliseconds,
        // post a WM_LBUTTONDBLCLK message instead of a single click.
        pWnd->PostMessage(bDoubleClick ? WM_LBUTTONDBLCLK : pMsg->message,
                          pMsg->wParam,
                          pMsg->lParam);
		return TRUE;	
		}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
        Hide();
		m_pParentWnd->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

	if (GetFocus() == NULL)
	{
        Hide();
		return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CTitleTip::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent != WM_TIMER)
	{	
		CWnd::OnTimer(nIDEvent);
		return;
	}
	// else 
	m_nCount++;
	if (m_nCount >= 10)
	{
		m_nCount = 0;
		if (m_nTimer)
		{
			KillTimer(m_nTimer);
			m_nTimer = 0;
		}
		StartShow();
	}
}
