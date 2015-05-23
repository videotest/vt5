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

// BCGButton.cpp : implementation file
//

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "bcglocalres.h"
#include "bcgbarres.h"
#include "BCGButton.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nImageHorzMargin = 10;
static const int nVertMargin = 5;

/////////////////////////////////////////////////////////////////////////////
// CBCGButton

IMPLEMENT_DYNAMIC(CBCGButton, CButton)

CBCGButton::CBCGButton()
{
	m_bPushed = FALSE;
	m_bHighlighted = FALSE;
	m_bCaptured = FALSE;
	m_nFlatStyle = BUTTONSTYLE_3D;
	m_sizeImage = CSize (0, 0);
	m_nStdImageId = (CMenuImages::IMAGES_IDS) -1;
	m_bAutoDestroyImage = FALSE;
	m_hIcon = NULL;
	m_hBitmap = NULL;
	m_hIconHot = NULL;
	m_hBitmapHot = NULL;
	m_bRighImage = FALSE;
	m_hCursor = NULL;
	m_sizePushOffset = CSize (2, 2);
	m_bHover = FALSE;
}
//****************************************************************************
CBCGButton::~CBCGButton()
{
	CleanUp ();
}
//****************************************************************************
void CBCGButton::CleanUp ()
{
	if (m_bAutoDestroyImage)
	{
		if (m_hBitmap != NULL)
		{
			::DeleteObject (m_hBitmap);
		}

		if (m_hIcon != NULL)
		{
			::DestroyIcon (m_hIcon);
		}

		if (m_hBitmapHot != NULL)
		{
			::DeleteObject (m_hBitmapHot);
		}

		if (m_hIconHot != NULL)
		{
			::DestroyIcon (m_hIconHot);
		}
	}

	m_hBitmap = NULL;
	m_hIcon = NULL;
	m_hIconHot = NULL;
	m_hBitmapHot = NULL;
	m_nStdImageId = (CMenuImages::IMAGES_IDS) -1;
	m_bAutoDestroyImage = FALSE;

	m_sizeImage = CSize (0, 0);
}


BEGIN_MESSAGE_MAP(CBCGButton, CButton)
	//{{AFX_MSG_MAP(CBCGButton)
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGButton message handlers

void CBCGButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	ASSERT (lpDIS != NULL);
	ASSERT (lpDIS->CtlType == ODT_BUTTON);

	CDC* pDC = CDC::FromHandle (lpDIS->hDC);
	ASSERT_VALID (pDC);

	CRect rectClient = lpDIS->rcItem;

	//--------------
	// Fill coltrol:
	//--------------
	pDC->FillRect (rectClient, &globalData.brBtnFace);

	if (lpDIS->itemState & ODS_FOCUS)
	{
		OnDrawFocusRect (pDC, rectClient);
	}

	//----------------
	// Draw 3d border:
	//----------------
	if (m_nFlatStyle != BUTTONSTYLE_NOBORDERS)
	{
		if (m_bPushed && m_bHighlighted || (lpDIS->itemState & ODS_SELECTED))
		{
			pDC->Draw3dRect (rectClient,
						globalData.clrBtnDkShadow, globalData.clrBtnHilite);

			rectClient.DeflateRect (1, 1);

			if (m_nFlatStyle != BUTTONSTYLE_FLAT)
			{
				pDC->Draw3dRect (rectClient,
							globalData.clrBtnShadow, globalData.clrBtnLight);
			}

			rectClient.DeflateRect (1, 1);

			rectClient.left += m_sizePushOffset.cx;
			rectClient.top += m_sizePushOffset.cy;
		}
		else if (m_nFlatStyle != BUTTONSTYLE_FLAT || m_bHighlighted)
		{
			pDC->Draw3dRect (rectClient,
						globalData.clrBtnHilite, 
						globalData.clrBtnDkShadow);
			rectClient.DeflateRect (1, 1);

			if (m_nFlatStyle == BUTTONSTYLE_3D ||
				(m_nFlatStyle == BUTTONSTYLE_SEMIFLAT && m_bHighlighted))
			{
				pDC->Draw3dRect (rectClient,
							globalData.clrBtnLight, globalData.clrBtnShadow);
			}

			rectClient.DeflateRect (1, 1);
		}
		else
		{
			rectClient.DeflateRect (2, 2);
		}
	}
	else
	{
		rectClient.DeflateRect (2, 2);
	}

	//---------------------
	// Draw button content:
	//---------------------
	OnDraw (pDC, rectClient, lpDIS->itemState);
}
//****************************************************************************
void CBCGButton::PreSubclassWindow() 
{
	ModifyStyle (BS_DEFPUSHBUTTON, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}
//****************************************************************************
BOOL CBCGButton::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= BS_OWNERDRAW;
	cs.style &= ~BS_DEFPUSHBUTTON;

	return CButton::PreCreateWindow(cs);
}
//****************************************************************************
BOOL CBCGButton::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//****************************************************************************
void CBCGButton::OnDraw (CDC* pDC, const CRect& rect, UINT uiState)
{
	CRect rectText = rect;
	CRect rectImage = rect;

	CString strText;
	GetWindowText (strText);

	if (m_sizeImage.cx != 0)
	{
		if (!strText.IsEmpty ())
		{
			if (m_bRighImage)
			{
				rectText.right -= m_sizeImage.cx + nImageHorzMargin / 2;
				rectImage.left = rectText.right;
				rectImage.right -= nImageHorzMargin / 2;
			}
			else
			{
				rectText.left +=  m_sizeImage.cx + nImageHorzMargin / 2;
				rectImage.left += nImageHorzMargin / 2;
				rectImage.right = rectText.left;
			}
		}

		// Center image:
		rectImage.DeflateRect ((rectImage.Width () - m_sizeImage.cx) / 2,
			max (0, (rect.Height () - m_sizeImage.cy) / 2));
	}
	else
	{
		rectImage.SetRectEmpty ();
	}

	//-----------
	// Draw text:
	//-----------
	CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);
	ASSERT(pOldFont != NULL);

	pDC->SetBkMode (TRANSPARENT);
	pDC->SetTextColor (globalData.clrBtnText);

	UINT uiDTFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	if (uiState & ODS_DISABLED)
	{
		COLORREF clrText = pDC->SetTextColor (globalData.clrBtnHilite);

		CRect rectShft = rectText;
		rectShft.OffsetRect (1, 1);
		pDC->DrawText (strText, rectShft, uiDTFlags);

		pDC->SetTextColor (clrText);
	}

	pDC->DrawText (strText, rectText, uiDTFlags);

	//------------
	// Draw image:
	//------------
	if (!rectImage.IsRectEmpty ())
	{
		if (m_nStdImageId != (CMenuImages::IMAGES_IDS) -1)
		{
			CMenuImages::Draw (pDC, m_nStdImageId, rectImage.TopLeft ());
		}
		else
		{
			UINT uiFlags = (uiState & ODS_DISABLED) == 0 ? 
				DSS_NORMAL : DSS_DISABLED;

			if (m_hIcon != NULL)
			{
				ASSERT (m_hBitmap == NULL);

				HBRUSH hbr = NULL;
				pDC->DrawState (rectImage.TopLeft (), m_sizeImage, 
					m_bHighlighted && m_hIconHot != NULL ? m_hIconHot : m_hIcon,
					uiFlags, hbr);
			}
			else
			{
				ASSERT (m_hBitmap != NULL);
				pDC->DrawState (rectImage.TopLeft (), m_sizeImage, 
					m_bHighlighted && m_hBitmapHot != NULL ? m_hBitmapHot : m_hBitmap,
					uiFlags);
			}
		}
	}

	pDC->SelectObject (pOldFont);
}
//****************************************************************************
void CBCGButton::SetImage (HICON hIcon, BOOL bAutoDestroy, HICON hIconHot)
{
	CleanUp ();

	if (hIcon == NULL)
	{
		return;
	}

	m_hIcon = hIcon;
	m_hIconHot = hIconHot;
	m_bAutoDestroyImage = bAutoDestroy;

	ICONINFO info;
	::GetIconInfo (m_hIcon, &info);

	BITMAP bmp;
	::GetObject (info.hbmColor, sizeof (BITMAP), (LPVOID) &bmp);

	m_sizeImage.cx = bmp.bmWidth;
	m_sizeImage.cy = bmp.bmHeight;

	// [vanek] : kill bitmaps - 24.12.2004
	::DeleteObject( info.hbmColor ), info.hbmColor = 0;
	::DeleteObject( info.hbmMask ), info.hbmMask = 0;
}
//****************************************************************************
void CBCGButton::SetImage (HBITMAP hBitmap, BOOL bAutoDestroy, HBITMAP hBitmapHot)
{
	CleanUp ();

	if (hBitmap == NULL)
	{
		return;
	}

	m_hBitmap = hBitmap;
	m_hBitmapHot = hBitmapHot;
	m_bAutoDestroyImage = bAutoDestroy;

	BITMAP bmp;
	::GetObject (m_hBitmap, sizeof (BITMAP), (LPVOID) &bmp);

	m_sizeImage.cx = bmp.bmWidth;
	m_sizeImage.cy = bmp.bmHeight;
}
//****************************************************************************
void CBCGButton::SetImage (UINT uiBmpResId, UINT uiBmpHotResId)
{
	CleanUp ();

	if (uiBmpResId == 0)
	{
		return;
	}

	HBITMAP hbmp = (HBITMAP) ::LoadImage (
		AfxGetResourceHandle (),
		MAKEINTRESOURCE (uiBmpResId),
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	ASSERT (hbmp != NULL);

	HBITMAP hbmpHot = NULL;
	if (uiBmpHotResId != 0)
	{
		hbmpHot = (HBITMAP) ::LoadImage (
			AfxGetResourceHandle (),
			MAKEINTRESOURCE (uiBmpHotResId),
			IMAGE_BITMAP,
			0, 0,
			LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		ASSERT (hbmp != NULL);
	}

	SetImage (hbmp, TRUE /* AutoDestroy */, hbmpHot);
}
//****************************************************************************
void CBCGButton::SetStdImage (CMenuImages::IMAGES_IDS id)
{
	CleanUp ();

	m_sizeImage = CMenuImages::Size ();
	m_nStdImageId= id;
}
//****************************************************************************
void CBCGButton::OnCancelMode() 
{
	CButton::OnCancelMode();
	
	if (m_bCaptured)
	{
		ReleaseCapture ();
		m_bCaptured = FALSE;

		Invalidate ();
		UpdateWindow ();
	}
}
//****************************************************************************
void CBCGButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_bHover = FALSE;

	if ((nFlags & MK_LBUTTON) || m_nFlatStyle != BUTTONSTYLE_3D)
	{
		BOOL bRedraw = FALSE;

		CRect rectClient;
		GetClientRect (rectClient);

		if (rectClient.PtInRect (point))
		{
			m_bHover = TRUE;

			if (!m_bHighlighted)
			{
				m_bHighlighted = TRUE;
				bRedraw = TRUE;
			}

			if ((nFlags & MK_LBUTTON) && !m_bPushed)
			{
				m_bPushed = TRUE;
				bRedraw = TRUE;
			}

			if (!m_bCaptured)
			{
				SetCapture ();
				m_bCaptured = TRUE;
				bRedraw = TRUE;
			}
		}
		else
		{
			if (nFlags & MK_LBUTTON)
			{
				if (m_bPushed)
				{
					m_bPushed = FALSE;
					bRedraw = TRUE;
				}
			}
			else if (m_bHighlighted)
			{
				m_bHighlighted = FALSE;
				bRedraw = TRUE;
			}

			if (m_bCaptured && (!nFlags & MK_LBUTTON))
			{
				ReleaseCapture ();
				m_bCaptured = FALSE;

				bRedraw = TRUE;
			}
		}

		if (bRedraw)
		{
			Invalidate ();
			UpdateWindow ();
		}
	}
	
	CButton::OnMouseMove(nFlags, point);
}
//****************************************************************************
void CBCGButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bPushed = TRUE;
	m_bHighlighted = TRUE;

	if (!m_bCaptured)
	{
		SetCapture ();
		m_bCaptured = TRUE;
	}

	Invalidate ();
	UpdateWindow ();

	CButton::OnLButtonDown(nFlags, point);
}
//****************************************************************************
void CBCGButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bPushed = FALSE;
	m_bHighlighted = FALSE;

	Invalidate ();
	UpdateWindow ();

	CButton::OnLButtonUp(nFlags, point);

	if (m_bCaptured)
	{
		ReleaseCapture ();
		m_bCaptured = FALSE;
	}
}
//****************************************************************************
CSize CBCGButton::SizeToContent (BOOL bCalcOnly)
{
	ASSERT (GetSafeHwnd () != NULL);

	CClientDC dc (this);

	CFont* pOldFont = dc.SelectObject (&globalData.fontRegular);
	ASSERT(pOldFont != NULL);

	CString strText;
	GetWindowText (strText);
	CSize sizeText = dc.GetTextExtent (strText);

	int cx = sizeText.cx + m_sizeImage.cx + nImageHorzMargin;
	if (sizeText.cx > 0)
	{
		cx += nImageHorzMargin;
	}

	int cy = max (sizeText.cy, m_sizeImage.cy) + nVertMargin * 2;

	if (!bCalcOnly)
	{
		SetWindowPos (NULL, -1, -1, cx, cy,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	dc.SelectObject (pOldFont);

	return CSize (cx, cy);
}
//****************************************************************************
BOOL CBCGButton::PreTranslateMessage(MSG* pMsg) 
{
	if (m_wndToolTip.GetSafeHwnd () != NULL)
	{
		if (pMsg->message == WM_LBUTTONDOWN ||
			pMsg->message == WM_LBUTTONUP ||
			pMsg->message == WM_MOUSEMOVE)
		{
			m_wndToolTip.RelayEvent(pMsg);
		}
	}
	
	return CButton::PreTranslateMessage(pMsg);
}
//****************************************************************************
void CBCGButton::SetTooltip (LPCTSTR lpszToolTipText)
{
	ASSERT (GetSafeHwnd () != NULL);

	if (lpszToolTipText == NULL)
	{
		if (m_wndToolTip.GetSafeHwnd () != NULL)
		{
			m_wndToolTip.Activate (FALSE);
		}
	}
	else
	{
		if (m_wndToolTip.GetSafeHwnd () != NULL)
		{
			m_wndToolTip.UpdateTipText (lpszToolTipText, this);
		}
		else
		{
			m_wndToolTip.Create (this, TTS_ALWAYSTIP);
			m_wndToolTip.AddTool (this, lpszToolTipText);
		}

		m_wndToolTip.Activate (TRUE);
	}
}
//****************************************************************************
void CBCGButton::SetMouseCursor (HCURSOR hcursor)
{
	m_hCursor = hcursor;
}
//****************************************************************************
void CBCGButton::SetMouseCursorHand ()
{
	if (globalData.m_hcurHand == NULL)
	{
		CBCGLocalResource locaRes;
		globalData.m_hcurHand = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_HAND);
	}

	SetMouseCursor (globalData.m_hcurHand);
}
//*****************************************************************************
BOOL CBCGButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor != NULL)
	{
		CRect rectClient;
		GetClientRect (rectClient);

		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (rectClient.PtInRect (ptCursor))
		{
			::SetCursor (m_hCursor);
			return TRUE;
		}
	}
	
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
//*****************************************************************************
void CBCGButton::OnDrawFocusRect (CDC* pDC, const CRect& rectClient)
{
	ASSERT_VALID (pDC);

	CRect rectFocus = rectClient;

	if (m_nFlatStyle == BUTTONSTYLE_FLAT)
	{
		rectFocus.DeflateRect (2, 2);
	}
	else
	{
		rectFocus.DeflateRect (3, 3);
	}

	pDC->DrawFocusRect (rectFocus);
}
