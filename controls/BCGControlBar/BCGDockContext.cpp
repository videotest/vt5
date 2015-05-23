//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// BCGDockContext.cpp:
//

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGDockContext.h"
#include "BCGDockBar.h"
#include "BCGSizingControlBar.h"
#include "bcgtoolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HORZF(dw) (dw & CBRS_ORIENT_HORZ)
#define VERTF(dw) (dw & CBRS_ORIENT_VERT)

AFX_STATIC void AFXAPI _AfxAdjustRectangle(CRect& rect, CPoint pt)
{
	int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
					(pt.x > rect.right) ? (pt.x - rect.right) : 0;
	int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
					(pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
	rect.OffsetRect(nXOffset, nYOffset);
}

#ifndef LAYOUT_LTR
#define LAYOUT_LTR	0
#endif //LAYOUT_LTR



/////////////////////////////////////////////////////////////////////////////
// CBCGDockContext
//
// this class is needed for 8-dim stretching of floated controlbars

CBCGDockContext::CBCGDockContext(CControlBar* pBar) : CDockContext( pBar )
{
}

CBCGDockContext::~CBCGDockContext()
{
}

// Aliases duplicated from CDockContext 

#define m_rectRequestedSize     m_rectDragHorz
#define m_rectActualSize        m_rectDragVert
#define m_rectActualFrameSize   m_rectFrameDragHorz
#define m_rectFrameBorders      m_rectFrameDragVert


void CBCGDockContext::StartDrag(CPoint pt)
{
	ASSERT_VALID(m_pBar);
	m_bDragging = TRUE;

	InitLoop();

	// GetWindowRect returns screen coordinates(not mirrored),
	// so if the desktop is mirrored then turn off mirroring
	// for the desktop dc so that we get correct focus rect drawn.
	// This layout change should be remembered, just in case ...

	if (m_pDC->GetLayout() & LAYOUT_RTL)
		m_pDC->SetLayout(LAYOUT_LTR);

	if (m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(0, LM_VERTDOCK);
		CSize sizeFloat = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);

		m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectDragVert = CRect(rect.TopLeft(), sizeVert);

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = CRect(rect.TopLeft(), sizeFloat);
		m_rectFrameDragVert = CRect(rect.TopLeft(), sizeFloat);

		DWORD dwStyle = WS_THICKFRAME | WS_CAPTION;
		if( m_pBar->GetSafeHwnd() )
		{
			CWnd	*pwndParent = m_pBar->GetParent();
			if( pwndParent->GetSafeHwnd() )
			{
				pwndParent = pwndParent->GetParent();
				if( pwndParent->GetSafeHwnd() )
				{
					dwStyle = GetWindowLong( pwndParent->GetSafeHwnd(), GWL_STYLE );
				}
			}
		}
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz, dwStyle);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert, dwStyle);

		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	else if (m_pBar->m_dwStyle & CBRS_SIZE_FIXED)
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(-1, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(-1, LM_VERTDOCK);

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectFrameDragVert = m_rectDragVert = CRect(rect.TopLeft(), sizeVert);

		DWORD dwStyle = WS_THICKFRAME | WS_CAPTION;
		if( m_pBar->GetSafeHwnd() )
		{
			CWnd	*pwndParent = m_pBar->GetParent();
			if( pwndParent->GetSafeHwnd() )
			{
				pwndParent = pwndParent->GetParent();
				if( pwndParent->GetSafeHwnd() )
				{
					dwStyle = GetWindowLong( pwndParent->GetSafeHwnd(), GWL_STYLE );
				}
			}
		}
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz, dwStyle);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert, dwStyle);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	else
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		BOOL bHorz = HORZF(m_dwStyle);
		DWORD dwMode = !bHorz ? (LM_HORZ | LM_HORZDOCK) : LM_VERTDOCK;
		CSize size = m_pBar->CalcDynamicLayout(-1, dwMode);

		// calculate inverted dragging rect
		if (bHorz)
		{
			m_rectDragHorz = rect;
			m_rectDragVert = CRect(CPoint(pt.x - rect.Height()/2, rect.top), size);
		}
		else // vertical orientation
		{
			m_rectDragVert = rect;
			m_rectDragHorz = CRect(CPoint(rect.left, pt.y - rect.Width()/2), size);
		}

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = m_rectDragHorz;
		m_rectFrameDragVert = m_rectDragVert;

		DWORD dwStyle = WS_THICKFRAME | WS_CAPTION;
		if( m_pBar->GetSafeHwnd() )
		{
			CWnd	*pwndParent = m_pBar->GetParent();
			if( pwndParent->GetSafeHwnd() )
			{
				pwndParent = pwndParent->GetParent();
				if( pwndParent->GetSafeHwnd() )
				{
					dwStyle = GetWindowLong( pwndParent->GetSafeHwnd(), GWL_STYLE );
				}
			}
		}
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz, dwStyle);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert, dwStyle);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}

	// adjust rectangles so that point is inside
	_AfxAdjustRectangle(m_rectDragHorz, pt);
	_AfxAdjustRectangle(m_rectDragVert, pt);
	_AfxAdjustRectangle(m_rectFrameDragHorz, pt);
	_AfxAdjustRectangle(m_rectFrameDragVert, pt);

	// initialize tracking state and enter tracking loop
	m_dwOverDockStyle = CanDock();
	Move(pt);   // call it here to handle special keys

	//andy
	CRect	rect;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
	ClipCursor( &rect );

	Track();

	ClipCursor( 0 );
}
// Duplicated from CDockContext in order to call Stretch()
// because it's not virtual

void CBCGDockContext::StartResize(int nHitTest, CPoint pt)
{
	ASSERT_VALID(m_pBar);
	ASSERT(m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC);
	m_bDragging = FALSE;

	InitLoop();

	// GetWindowRect returns screen coordinates(not mirrored)
	// So if the desktop is mirrored then turn off mirroring
	// for the desktop dc so that we draw correct focus rect

	if (m_pDC->GetLayout() & LAYOUT_RTL)
		m_pDC->SetLayout(LAYOUT_LTR);

	// get true bar size (including borders)
	CRect rect;
	m_pBar->GetWindowRect(rect);
	m_ptLast = pt;
	m_nHitTest = nHitTest;

	CSize size = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);
	m_rectRequestedSize = CRect(rect.TopLeft(), size);
	m_rectActualSize = CRect(rect.TopLeft(), size);
	m_rectActualFrameSize = CRect(rect.TopLeft(), size);

	DWORD dwStyle = WS_THICKFRAME | WS_CAPTION;
	if( m_pBar->GetSafeHwnd() )
	{
		CWnd	*pwndParent = m_pBar->GetParent();
		if( pwndParent->GetSafeHwnd() )
		{
			pwndParent = pwndParent->GetParent();
			if( pwndParent->GetSafeHwnd() )
			{
				dwStyle = GetWindowLong( pwndParent->GetSafeHwnd(), GWL_STYLE );
			}
		}
	}
	// calculate frame rectangle
	CMiniFrameWnd::CalcBorders(&m_rectActualFrameSize, dwStyle);
	m_rectActualFrameSize.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);

	m_rectFrameBorders = CRect(CPoint(0,0),
		m_rectActualFrameSize.Size() - m_rectActualSize.Size());

	// initialize tracking state and enter tracking loop
	m_dwOverDockStyle = 0;
	Stretch(pt);   // call it here to handle special keys
	Track();
}


// Mostly duplicated from CDockContext
// changes are marked
void CBCGDockContext::Stretch(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;

	//<---------------------------------------------------------->
	//<ET: Horizontally and vertically independently------------->
    CSize size;

	// horizontally
	if (m_nHitTest == HTLEFT 
		|| m_nHitTest == HTTOPLEFT 
		|| m_nHitTest == HTBOTTOMLEFT )
	{
		m_rectRequestedSize.left += ptOffset.x;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Width(), LM_HORZ);
	}
	if (m_nHitTest == HTRIGHT
		|| m_nHitTest == HTTOPRIGHT 
		|| m_nHitTest == HTBOTTOMRIGHT )
	{
		m_rectRequestedSize.right += ptOffset.x;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Width(), LM_HORZ);
	}

	// vertically
	if (m_nHitTest == HTTOP 
		|| m_nHitTest == HTTOPLEFT 
		|| m_nHitTest == HTTOPRIGHT )
	{
		m_rectRequestedSize.top += ptOffset.y;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Height(), LM_HORZ|LM_LENGTHY);
	}
	if (m_nHitTest == HTBOTTOM
		|| m_nHitTest == HTBOTTOMRIGHT 
		|| m_nHitTest == HTBOTTOMRIGHT )
	{
		m_rectRequestedSize.bottom += ptOffset.y;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Height(), LM_HORZ|LM_LENGTHY);
	}
	//<---------------------------------------------------------->
	//<---------------------------------------------------------->


	CRect rectDesk;
	HWND hWndDesk = ::GetDesktopWindow();
	::GetWindowRect(hWndDesk, &rectDesk);
	CRect rectTemp = m_rectActualFrameSize;

	//<---------------------------------------------------------->
	//<ET: cases for HT{X}{Y}------------------------------------>
    // left, top, top left
    if (m_nHitTest == HTLEFT || m_nHitTest == HTTOP || m_nHitTest == HTTOPLEFT)
    {
        rectTemp.left = rectTemp.right -
            (size.cx + m_rectFrameBorders.Width());
        rectTemp.top = rectTemp.bottom -
            (size.cy + m_rectFrameBorders.Height());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.left = m_rectActualSize.right - size.cx;
            m_rectActualSize.top = m_rectActualSize.bottom - size.cy;
            m_rectActualFrameSize.left = rectTemp.left;
            m_rectActualFrameSize.top = rectTemp.top;
        }
    } 
	else if (m_nHitTest == HTTOPRIGHT) // top right
    {
        rectTemp.top = rectTemp.bottom -
            (size.cy + m_rectFrameBorders.Height());
        rectTemp.right = rectTemp.left +
            (size.cx + m_rectFrameBorders.Width());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.left = m_rectActualSize.right - size.cx;
            m_rectActualSize.bottom = m_rectActualSize.top + size.cy;
            m_rectActualFrameSize.right = rectTemp.right;
            m_rectActualFrameSize.top = rectTemp.top;
        }
    } 
	else if (m_nHitTest == HTBOTTOMLEFT) // bottom left
    {
        rectTemp.bottom = rectTemp.top +
            (size.cy + m_rectFrameBorders.Height());
        rectTemp.left = rectTemp.right -
            (size.cx + m_rectFrameBorders.Width());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.right = m_rectActualSize.left + size.cx;
            m_rectActualSize.top = m_rectActualSize.bottom - size.cy;
            m_rectActualFrameSize.left = rectTemp.left;
            m_rectActualFrameSize.bottom = rectTemp.bottom;
        }
    } 
	else // bottom, right, bottom right
    {
        rectTemp.right = rectTemp.left +
            (size.cx + m_rectFrameBorders.Width());
        rectTemp.bottom = rectTemp.top +
            (size.cy + m_rectFrameBorders.Height());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.right = m_rectActualSize.left + size.cx;
            m_rectActualSize.bottom = m_rectActualSize.top + size.cy;
            m_rectActualFrameSize.right = rectTemp.right;
            m_rectActualFrameSize.bottom = rectTemp.bottom;
        }
    }
	//<---------------------------------------------------------->
	//<---------------------------------------------------------->

	
	m_ptLast = pt;

	// update feedback
	DrawFocusRect();
}

// Duplicated from CDockContext in order to call Stretch()
// because it's not virtual
BOOL CBCGDockContext::Track()
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// set capture to the window which received this message
	m_pBar->SetCapture();
	ASSERT(m_pBar == CWnd::GetCapture());


	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == m_pBar)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage(msg.wParam);
			break;
		}

		switch (msg.message)
		{
		case WM_LBUTTONUP:
			if (m_bDragging)
				EndDrag();
			else
				EndResize();
			return TRUE;
		case WM_MOUSEMOVE:
			if (m_bDragging)
				Move(msg.pt);
			else
				Stretch(msg.pt);
			break;
		case WM_KEYUP:
			if (m_bDragging)
				OnKey((int)msg.wParam, FALSE);
			break;
		case WM_KEYDOWN:
			if (m_bDragging)
				OnKey((int)msg.wParam, TRUE);
			if (msg.wParam == VK_ESCAPE)
			{
				CancelLoop();
				return FALSE;
			}
			break;
		case WM_RBUTTONDOWN:
			CancelLoop();
			return FALSE;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	CancelLoop();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////
// CBCGMiniDockFrameWnd

IMPLEMENT_DYNCREATE(CBCGMiniDockFrameWnd, CMiniDockFrameWnd)

CBCGMiniDockFrameWnd::CBCGMiniDockFrameWnd()
{
}

CBCGMiniDockFrameWnd::~CBCGMiniDockFrameWnd()
{
}

BOOL CBCGMiniDockFrameWnd::Create(CWnd* pParent, DWORD dwBarStyle)
{
	if (!CMiniDockFrameWnd::Create(pParent, dwBarStyle))
		return FALSE;

	ModifyStyle(MFS_4THICKFRAME, 0);
	return true;
}

BEGIN_MESSAGE_MAP(CBCGMiniDockFrameWnd, CMiniDockFrameWnd)
	//{{AFX_MSG_MAP(CBCGMiniDockFrameWnd)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBCGMiniDockFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (nHitTest == HTCAPTION)
	{
		// special activation for floating toolbars
		ActivateTopParent();

		// initiate toolbar drag for non-CBRS_FLOAT_MULTI toolbars
		if ((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0)
		{
			int nPos = 1;
			CControlBar* pBar = NULL;
			while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
				pBar = reinterpret_cast<CBCGDockBar&>(m_wndDockBar).GetDockedControlBar(nPos++);

			ASSERT(pBar != NULL);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL);
			pBar->m_pDockContext->StartDrag(point);
			return;
		}
	}
	else if (nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST)
	{
		// special activation for floating toolbars
		ActivateTopParent();

		int nPos = 1;
		CControlBar* pBar = NULL;
		while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
			pBar = reinterpret_cast<CBCGDockBar&>(m_wndDockBar).GetDockedControlBar(nPos++);

		ASSERT(pBar != NULL);
		ASSERT_KINDOF(CControlBar, pBar);
		ASSERT(pBar->m_pDockContext != NULL);

		// CBRS_SIZE_DYNAMIC toolbars cannot have the CBRS_FLOAT_MULTI style
		ASSERT((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0);

		
		if (!pBar->IsKindOf(RUNTIME_CLASS(CBCGSizingControlBar))) {
			CMiniDockFrameWnd::OnNcLButtonDown(nHitTest,point);
		}
		else {
			pBar->m_pDockContext->StartResize(nHitTest, point);
		}
		return;
	}
	CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
}
//*********************************************************************************
void CBCGMiniDockFrameWnd::OnNcRButtonDown(UINT /*nHitTest*/, CPoint point)
{
	// By Alan Fritz

	if (CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

    CWnd* pParentWnd = GetParent ();
	ASSERT_VALID (pParentWnd);

    if (pParentWnd->IsKindOf (RUNTIME_CLASS (CFrameWnd)))
    {
        ((CFrameWnd*)pParentWnd)->SendMessage(BCGM_TOOLBARMENU,
					(WPARAM) GetSafeHwnd (),
					MAKELPARAM (point.x, point.y));
    }
    else
    {
        TRACE(_T("CBCGMiniDockFrameWnd parent is not a CFrameWnd\n"));
        ASSERT (FALSE);
    }
}
