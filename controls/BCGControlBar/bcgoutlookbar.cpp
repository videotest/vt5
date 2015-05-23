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

// bcgoutlookbar.cpp : implementation file
//

#include "stdafx.h"
#include "bcgoutlookbar.h"
#include "MenuImages.h"
#include "bcgbarres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BORDER_SIZE				3
#define MARGIN_SIZE				2

#define SCROLL_BUTTON_OFFSET	5

static const UINT idScrollUp	= 1;
static const UINT idScrollDn	= 2;

static const UINT uiScrollDelay = 200;	// ms

/////////////////////////////////////////////////////////////////////////////
// CBCGOutlookBar

IMPLEMENT_DYNAMIC(CBCGOutlookBar, CBCGToolBar)

CBCGOutlookBar::CBCGOutlookBar()
{
	m_iScrollOffset = 0;
	m_iFirstVisibleButton = 0;
	m_bScrollDown = FALSE;

	m_clrRegText = ::GetSysColor (COLOR_WINDOW);
	m_clrBackColor = ::GetSysColor (COLOR_3DSHADOW);

	m_clrTransparentColor = RGB (255, 0, 255);
	m_Images.SetTransparentColor (m_clrTransparentColor);

	m_csImage = CSize (0, 0);

	m_uiBackImageId = 0;

	m_btnUp.m_nFlatStyle = CBCGButton::BUTTONSTYLE_3D;
	m_btnDown.m_nFlatStyle = CBCGButton::BUTTONSTYLE_3D;

	m_bDrawShadedHighlight = FALSE;
}
//*************************************************************************************
CBCGOutlookBar::~CBCGOutlookBar()
{
	while (!m_Buttons.IsEmpty ())
	{
		delete m_Buttons.RemoveHead ();
	}
}

BEGIN_MESSAGE_MAP(CBCGOutlookBar, CBCGToolBar)
	//{{AFX_MSG_MAP(CBCGOutlookBar)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGOutlookBar message handlers

BOOL CBCGOutlookBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	// initialize common controls
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));

	return CBCGToolBar::Create (pParentWnd, dwStyle | WS_CHILD | WS_VISIBLE, nID);
}
//*************************************************************************************
BOOL CBCGOutlookBar::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//*************************************************************************************
BOOL CBCGOutlookBar::AddButton (UINT uiImage, UINT uiLabel, UINT iIdCommand)
{
	CString strLable;
	strLable.LoadString (uiLabel);

	return AddButton (uiImage, strLable, iIdCommand);
}
//*************************************************************************************
BOOL CBCGOutlookBar::AddButton (UINT uiImage, LPCTSTR lpszLabel, UINT iIdCommand)
{
	int iImageIndex = -1;

	if (uiImage != 0)
	{
		CBitmap bmp;
		if (!bmp.LoadBitmap (uiImage))
		{
			TRACE(_T("Can't load bitmap resource: %d"), uiImage);
			return FALSE;
		}

		BITMAP bitmap;
		bmp.GetBitmap (&bitmap);

		CSize csImage = CSize (bitmap.bmWidth, bitmap.bmHeight);
		if (m_Images.GetCount () == 0)	// First image
		{
			m_csImage = csImage;
			m_Images.SetImageSize (csImage);
		}
		else
		{
			ASSERT (m_csImage == csImage);	// All buttons should be of the same size!
		}

		iImageIndex = m_Images.AddImage (bmp);
	}

	CBCGOutlookButton* pButton = new CBCGOutlookButton;
	ASSERT (pButton != NULL);

	pButton->m_nID = iIdCommand;
	pButton->m_strText = (lpszLabel == NULL) ? _T("") : lpszLabel;
	pButton->SetImage (iImageIndex);
	pButton->m_bTextBelow = m_bTextLabels;

	InsertButton (pButton, m_Buttons.GetCount () - 1);

	AdjustLayout ();
	return TRUE;
}
//**************************************************************************************
BOOL CBCGOutlookBar::RemoveButton (UINT iIdCommand)
{
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		POSITION posSave = pos;

		CBCGOutlookButton* pButton = (CBCGOutlookButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pButton->m_nID == iIdCommand)
		{
			m_Buttons.RemoveAt (posSave);
			delete pButton;

			AdjustLocations ();
			UpdateWindow ();
			Invalidate ();

			return TRUE;
		}
	}

	return FALSE;
}
//*************************************************************************************
void CBCGOutlookBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGToolBar::OnSize(nType, cx, cy);

	int iButtons = m_Buttons.GetCount ();
	if (iButtons > 0)
	{
		AdjustLayout ();

		POSITION posLast = m_Buttons.FindIndex (iButtons - 1);
		CBCGOutlookButton* pButtonLast = (CBCGOutlookButton*) m_Buttons.GetAt (posLast);
		ASSERT (pButtonLast != NULL);

		while (m_iScrollOffset > 0 &&
			pButtonLast->Rect ().bottom < cy)
		{
			ScrollUp ();
		}
	}
}
//*************************************************************************************
void CBCGOutlookBar::SetTextColor (COLORREF clrRegText, COLORREF/* clrSelText obsolete*/)
{
	m_clrRegText = clrRegText;
	if (GetSafeHwnd () != NULL)
	{
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
int CBCGOutlookBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetBarStyleEx (m_dwStyle & ~(CBRS_BORDER_ANY | CBRS_GRIPPER));

	m_cxLeftBorder = m_cxRightBorder = 0;
	m_cyTopBorder = m_cyBottomBorder = 0;

	//-------------------------------------------
	// Adjust Z-order in the parent frame window:
	//-------------------------------------------
	BOOL bVert = (m_dwStyle & CBRS_ORIENT_HORZ) == 0;
	if (bVert)
	{
		SetWindowPos(&wndBottom, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
	else
	{
		CFrameWnd* pParent = GetParentFrame ();
		ASSERT (pParent != NULL);

		CWnd* pWndStatusBar = pParent->GetDlgItem (AFX_IDW_STATUS_BAR);

		if (pWndStatusBar != NULL)
		{
			SetWindowPos(pWndStatusBar, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		}
	}

	//-----------------------
	// Create scroll buttons:
	//-----------------------
	CRect rectDummy (CPoint (0, 0), CMenuImages::Size ());
	rectDummy.InflateRect (MARGIN_SIZE, MARGIN_SIZE);

	m_btnUp.Create (_T(""), WS_CHILD | BS_PUSHBUTTON, rectDummy,
					this, idScrollUp);
	m_btnUp.SetStdImage (bVert ? CMenuImages::IdArowUp : CMenuImages::IdArowLeft);

	m_btnDown.Create (_T(""), WS_CHILD | BS_PUSHBUTTON, rectDummy,
					this, idScrollDn);
	m_btnDown.SetStdImage (bVert ? CMenuImages::IdArowDown : CMenuImages::IdArowRight);

	SetWindowText (_T("Shortcuts bar"));
	return 0;
}
//*************************************************************************************
void CBCGOutlookBar::ScrollUp ()
{
	if (m_iScrollOffset <= 0 ||
		m_iFirstVisibleButton <= 0)
	{
		KillTimer (idScrollUp);
		return;
	}

	POSITION pos = m_Buttons.FindIndex (--m_iFirstVisibleButton);
	CBCGOutlookButton* pButton = (CBCGOutlookButton*) m_Buttons.GetAt (pos);
	ASSERT (pButton != NULL);

	BOOL bVert = (m_dwStyle & CBRS_ORIENT_HORZ) == 0;
	if (bVert)
	{
		m_iScrollOffset -= pButton->Rect ().Height ();
	}
	else
	{
		m_iScrollOffset -= pButton->Rect ().Width ();
	}

	if (m_iFirstVisibleButton == 0)
	{
		m_iScrollOffset = 0;
	}

	ASSERT (m_iScrollOffset >= 0);
	AdjustLayout ();
}
//*************************************************************************************
void CBCGOutlookBar::ScrollDown ()
{
	if (!m_bScrollDown ||
		m_iFirstVisibleButton + 1 >= m_Buttons.GetCount ())
	{
		KillTimer (idScrollDn);
		return;
	}

	POSITION pos = m_Buttons.FindIndex (++m_iFirstVisibleButton);
	CBCGOutlookButton* pButton = (CBCGOutlookButton*) m_Buttons.GetAt (pos);
	ASSERT (pButton != NULL);

	BOOL bVert = (m_dwStyle & CBRS_ORIENT_HORZ) == 0;
	if (bVert)
	{
		m_iScrollOffset += pButton->Rect ().Height ();
	}
	else
	{
		m_iScrollOffset += pButton->Rect ().Width ();
	}

	AdjustLayout ();
}
//*************************************************************************************
CSize CBCGOutlookBar::CalcFixedLayout (BOOL /*bStretch*/, BOOL bHorz)
{
	int iMinSize = (bHorz) ? 
		m_csImage.cy + 2 * MARGIN_SIZE + 2 * BORDER_SIZE:
		m_csImage.cx + 2 * MARGIN_SIZE + 2 * BORDER_SIZE;

	if (m_bTextLabels)
	{
		CClientDC dc (this);

		for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
		{
			CBCGOutlookButton* pButton = (CBCGOutlookButton*) m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			CRect rectText (0, 0, 1, 1);
			dc.DrawText (pButton->m_strText, rectText, DT_CALCRECT | DT_WORDBREAK);

			if (!bHorz)
			{
				iMinSize = max (iMinSize, rectText.Width ());
			}
			else
			{
				iMinSize = max (iMinSize, rectText.Height ());
			}
		}
	}

	CSize size = (bHorz) ? CSize (32767, iMinSize) : CSize (iMinSize, 32767);
	return size;
}
//*************************************************************************************
void CBCGOutlookBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CRect rect; 
	rect.SetRectEmpty();

	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	CControlBar::CalcInsideRect(rect, bHorz);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}
//*************************************************************************************
void CBCGOutlookBar::SetBackImage (UINT uiImageID)
{
	if (m_uiBackImageId == uiImageID)
	{
		return;
	}

	m_bDrawShadedHighlight = FALSE;
	if (m_bmpBack.GetCount () > 0)
	{
		m_bmpBack.Clear ();
	}

	m_uiBackImageId = 0;
	if (uiImageID != 0)
	{
		HBITMAP hbmp = (HBITMAP) ::LoadImage (
				AfxGetResourceHandle (),
				MAKEINTRESOURCE (uiImageID),
				IMAGE_BITMAP,
				0, 0,
				LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		if (hbmp != NULL)
		{
			BITMAP bitmap;
			::GetObject (hbmp, sizeof (BITMAP), (LPVOID) &bitmap);

			m_bmpBack.SetImageSize (CSize (bitmap.bmWidth, bitmap.bmHeight));
			m_bmpBack.AddImage (hbmp);
			m_uiBackImageId = uiImageID;
		}

		m_bDrawShadedHighlight = (globalData.m_nBitsPerPixel > 8);	// For 16 bits or greater
	}

	if (GetSafeHwnd () != NULL)
	{
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
void CBCGOutlookBar::SetBackColor (COLORREF color)
{
	m_clrBackColor = color;
	if (GetSafeHwnd () != NULL)
	{
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
void CBCGOutlookBar::SetTransparentColor (COLORREF color)
{
	m_clrTransparentColor = color;
	if (GetSafeHwnd () != NULL)
	{
		m_Images.SetTransparentColor (m_clrTransparentColor);
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
void CBCGOutlookBar::OnSysColorChange() 
{
	CBCGToolBar::OnSysColorChange();
	
	m_clrBackColor = ::GetSysColor (COLOR_3DSHADOW);

	if (m_uiBackImageId != 0)
	{
		int uiImage = m_uiBackImageId;
		m_uiBackImageId = (UINT) -1;

		SetBackImage (uiImage);
	}
	else
	{
		m_clrRegText = ::GetSysColor (COLOR_WINDOW);
		Invalidate ();
	}
}
//*****************************************************************************************
void CBCGOutlookBar::AdjustLocations ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	ASSERT_VALID(this);

	if (m_Buttons.IsEmpty ())
	{
		return;
	}

	CSize sizeScrollBtn = CMenuImages::Size () + CSize (MARGIN_SIZE, MARGIN_SIZE);

	BOOL bVert = (m_dwStyle & CBRS_ORIENT_HORZ) == 0;

	CClientDC dc (this);
	CFont* pOldFont = dc.SelectObject (&globalData.fontRegular);

	CRect rectClient;
	GetClientRect (&rectClient);

	CSize sizeDefault;

	if (bVert)
	{
		rectClient.InflateRect (-BORDER_SIZE, -2);
		sizeDefault = CSize (rectClient.Width () - 2 * MARGIN_SIZE, m_csImage.cy);
	}
	else
	{
		rectClient.InflateRect (-1, -BORDER_SIZE);
		sizeDefault = CSize (m_csImage.cx, rectClient.Height () - 2 * MARGIN_SIZE);
	}

	int iOffset = bVert ? 
				rectClient.top - m_iScrollOffset + SCROLL_BUTTON_OFFSET:
				rectClient.left - m_iScrollOffset + SCROLL_BUTTON_OFFSET;

	if (m_iFirstVisibleButton > 0)
	{
		m_btnUp.SetWindowPos (NULL,
			rectClient.right - sizeScrollBtn.cx - SCROLL_BUTTON_OFFSET,
			rectClient.top + SCROLL_BUTTON_OFFSET,
			-1, -1,
			SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

		m_btnUp.ShowWindow (SW_SHOWNOACTIVATE);
	}
	else
	{
		m_btnUp.ShowWindow (SW_HIDE);
	}

	int iIndex = 1;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iIndex ++)
	{
		CBCGOutlookButton* pButton = (CBCGOutlookButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->m_bTextBelow = m_bTextLabels;

		CSize sizeButton = pButton->OnCalculateSize (&dc, sizeDefault, !bVert);

		CRect rectButton;

		if (bVert)
		{
			rectButton = CRect (rectClient.left + MARGIN_SIZE, 
								iOffset, 
								rectClient.right - 1,
								iOffset + sizeButton.cy);
			iOffset = rectButton.bottom;
		}
		else
		{
			rectButton = CRect (rectClient.left + iOffset, 
								rectClient.top + MARGIN_SIZE,
								rectClient.left + iOffset + sizeButton.cx,
								rectClient.bottom - 1);
			iOffset = rectButton.right;
		}

		pButton->SetRect (rectButton);
	}

	if (bVert)
	{
		m_bScrollDown = (iOffset > rectClient.bottom);
	}
	else
	{
		m_bScrollDown = (iOffset > rectClient.right);
	}

	if (m_bScrollDown)
	{
		m_btnDown.SetWindowPos (&wndTop,
			rectClient.right - sizeScrollBtn.cx - SCROLL_BUTTON_OFFSET,
			bVert ?	
				rectClient.bottom - sizeScrollBtn.cy - SCROLL_BUTTON_OFFSET :
				rectClient.top + SCROLL_BUTTON_OFFSET,
			-1, -1,
			SWP_NOSIZE | SWP_NOACTIVATE);

		m_btnDown.ShowWindow (SW_SHOWNOACTIVATE);
	}
	else
	{
		m_btnDown.ShowWindow (SW_HIDE);
	}

	dc.SelectObject (pOldFont);

	OnMouseLeave (0, 0);
}
//*************************************************************************************
void CBCGOutlookBar::DoPaint(CDC* pDCPaint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDCPaint);

	CRect rectClip;
	pDCPaint->GetClipBox (rectClip);

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect (rectClient);

	CDC*		pDC = pDCPaint;
	BOOL		m_bMemDC = FALSE;
	CDC			dcMem;
	CBitmap		bmp;
	CBitmap*	pOldBmp = NULL;

	if (dcMem.CreateCompatibleDC (pDCPaint) &&
		bmp.CreateCompatibleBitmap (pDCPaint, rectClient.Width (),
								  rectClient.Height ()))
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		m_bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject (&bmp);
		pDC = &dcMem;
	}

	if (m_bmpBack.GetCount () == 0)
	{
		pDC->FillSolidRect (rectClient, m_clrBackColor);
	}
	else
	{
		ASSERT (m_bmpBack.GetCount () == 1);

		CBCGDrawState ds;
		m_bmpBack.PrepareDrawImage (ds);
		CSize sizeBack = m_bmpBack.GetImageSize ();

		for (int x = 0; x < rectClient.Width (); x += sizeBack.cx)
		{
			for (int y = 0; y < rectClient.Height (); y += sizeBack.cy)
			{
				m_bmpBack.Draw (pDC, x, y, 0);
			}
		}

		m_bmpBack.EndDrawImage (ds);
	}

	pDC->SetTextColor (globalData.clrBtnText);
	pDC->SetBkMode (TRANSPARENT);

	CRect rectBorder = rectClient;

	if (!bHorz)
	{
		rectBorder.InflateRect (0, 2);
		rectBorder.left = rectBorder.right - BORDER_SIZE;
	}
	else
	{
		rectBorder.InflateRect (2, 0);
		rectBorder.bottom = rectBorder.top + BORDER_SIZE;
	}

	pDC->FillRect (rectBorder, &globalData.brBtnFace);
	pDC->Draw3dRect (rectBorder,	::GetSysColor (COLOR_3DHILIGHT),
								::GetSysColor (COLOR_3DSHADOW));

	CRect rectSpace = rectClient;

	if (!bHorz)
	{
		CRect rectLeft = rectClient;
		rectLeft.right = BORDER_SIZE;
		pDC->FillRect (rectLeft, &globalData.brBtnFace);

		rectSpace.left = rectLeft.right;
		rectSpace.right = rectBorder.left;
	}
	else
	{
		CRect rectBottom = rectClient;
		rectBottom.top = rectBottom.bottom - BORDER_SIZE;
		pDC->FillRect (rectBottom, &globalData.brBtnFace);

		rectSpace.top = rectBorder.bottom;
		rectSpace.bottom = rectBottom.top;
	}

	pDC->Draw3dRect (rectSpace,	::GetSysColor (COLOR_3DSHADOW),
								::GetSysColor (COLOR_3DHILIGHT));

	if (!bHorz)
	{
		rectSpace.InflateRect (0, -1);
		rectSpace.left ++;
	}
	else
	{
		rectSpace.InflateRect (-1, 0);
		rectSpace.top ++;
	}

	pDC->Draw3dRect (rectSpace,	::GetSysColor (COLOR_3DDKSHADOW),
								::GetSysColor (COLOR_3DLIGHT));

	CBCGDrawState ds;
	if (!m_Images.PrepareDrawImage (ds))
	{
		return;     // something went wrong
	}

	CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);

	//--------------
	// Draw buttons:
	//--------------
	if (!bHorz)
	{
		rectClient.InflateRect (-BORDER_SIZE, -2);
	}
	else
	{
		rectClient.InflateRect (-1, -BORDER_SIZE);
	}

	CRgn rgn;
	rgn.CreateRectRgnIndirect (rectClient);

	pDC->SelectClipRgn (&rgn);

	POSITION pos = m_Buttons.GetHeadPosition ();
	int iButton = 1;
	m_Buttons.GetNext (pos);	// Skip "Up" button
	BOOL bStop = FALSE;

	for (; !bStop; iButton ++)
	{
		CBCGOutlookButton* pButton = NULL;
		if (pos == NULL)
		{
			pButton = (CBCGOutlookButton*) m_Buttons.GetHead ();
			iButton = 0;
			bStop = TRUE;
		}
		else
		{
			pButton = (CBCGOutlookButton*) m_Buttons.GetNext (pos);
		}

		ASSERT_VALID (pButton);

		CRect rect = pButton->Rect ();

		BOOL bHighlighted = FALSE;

		if (IsCustomizeMode () && !m_bLocked)
		{
			bHighlighted = FALSE;
		}
		else
		{
			bHighlighted = ((iButton == m_iHighlighted ||
							iButton == m_iButtonCapture) &&
							(m_iButtonCapture == -1 ||
							iButton == m_iButtonCapture));
		}

		CRect rectInter;
		if (rectInter.IntersectRect (rect, rectClip))
		{
			pButton->OnDraw (pDC, rect, &m_Images, bHorz, IsCustomizeMode (),
							bHighlighted);
		}
	}

	//-------------------------------------------------------------
	// Highlight selected button in the toolbar customization mode:
	//-------------------------------------------------------------
	if (m_iSelected >= m_Buttons.GetCount ())
	{
		m_iSelected = -1;
	}

	if (IsCustomizeMode () && m_iSelected >= 0 && !m_bLocked)
	{
		CBCGToolbarButton* pSelButton = GetButton (m_iSelected);
		ASSERT (pSelButton != NULL);

		if (pSelButton != NULL && pSelButton->CanBeStored ())
		{
			CRect rectDrag = pSelButton->Rect ();
			if (pSelButton->GetHwnd () != NULL)
			{
				rectDrag.InflateRect (0, 1);
			}

			pDC->DrawDragRect (&rectDrag, CSize (2, 2), NULL, CSize (2, 2));
		}
	}

	if (IsCustomizeMode () && m_iDragIndex >= 0 && !m_bLocked)
	{
		DrawDragMarker (pDC);
	}

	pDC->SelectClipRgn (NULL);
	pDC->SelectObject (pOldFont);
	m_Images.EndDrawImage (ds);

	if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//-------------------------------------- 
		pDCPaint->BitBlt (rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(),
					   &dcMem, rectClip.left, rectClip.top, SRCCOPY);

		dcMem.SelectObject(pOldBmp);
	}
}
//****************************************************************************************
int CBCGOutlookBar::HitTest(CPoint point)
{
	CRect rect;
	m_btnDown.GetClientRect (rect);
	m_btnDown.MapWindowPoints (this, rect);

	if (rect.PtInRect (point))
	{
		return -1;
	}

	m_btnUp.GetClientRect (rect);
	m_btnUp.MapWindowPoints (this, rect);

	if (rect.PtInRect (point))
	{
		return -1;
	}

	return CBCGToolBar::HitTest (point);
}
//****************************************************************************************
DROPEFFECT CBCGOutlookBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CBCGToolbarButton* pButton = CBCGToolbarButton::CreateFromOleData (pDataObject);
	if (pButton == NULL)
	{
		return DROPEFFECT_NONE;
	}

	BOOL bAllowDrop = pButton->IsKindOf (RUNTIME_CLASS (CBCGOutlookButton));
	delete pButton;

	if (!bAllowDrop)
	{
		return DROPEFFECT_NONE;
	}

	int iHit = HitTest (point);
	if (iHit == 0)
	{
		ScrollUp ();
		return DROPEFFECT_NONE;
	}

	if (iHit == m_Buttons.GetCount () - 1)
	{
		ScrollDown ();
		return DROPEFFECT_NONE;
	}

	return CBCGToolBar::OnDragOver (pDataObject, dwKeyState, point);
}
//***************************************************************************************
BOOL CBCGOutlookBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
{
	ASSERT_VALID (pButton);
	ASSERT_VALID (pPopup);

	if (IsCustomizeMode ())
	{
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_TEXT, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_APPEARANCE, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_START_GROUP, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_RESET, MF_GRAYED | MF_BYCOMMAND);
		pPopup->EnableMenuItem (ID_BCGBARRES_COPY_IMAGE, MF_GRAYED | MF_BYCOMMAND);
	}

	CBCGToolBar::EnableContextMenuItems (pButton, pPopup);
	return TRUE;
}
//**************************************************************************************
BOOL CBCGOutlookBar::PreTranslateMessage(MSG* pMsg) 
{
   	switch (pMsg->message)
	{
	case WM_LBUTTONUP:
		KillTimer (idScrollUp);
		KillTimer (idScrollDn);

	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		{
			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			CRect rect;
			m_btnDown.GetClientRect (rect);
			m_btnDown.MapWindowPoints (this, rect);

			if (rect.PtInRect (ptCursor))
			{
				m_btnDown.SendMessage (pMsg->message, pMsg->wParam, pMsg->wParam);
				if (pMsg->message == WM_LBUTTONDOWN)
				{
					SetTimer (idScrollDn, uiScrollDelay, NULL);
					ScrollDown ();
				}

				return TRUE;
			}

			m_btnUp.GetClientRect (rect);
			m_btnUp.MapWindowPoints (this, rect);

			if (rect.PtInRect (ptCursor))
			{
				m_btnUp.SendMessage (pMsg->message, pMsg->wParam, pMsg->wParam);

				if (pMsg->message == WM_LBUTTONDOWN)
				{
					SetTimer (idScrollUp, uiScrollDelay, NULL);
					ScrollUp ();
				}
				return TRUE;
			}
		}
	}

	return CBCGToolBar::PreTranslateMessage(pMsg);
}
//**************************************************************************************
void CBCGOutlookBar::OnTimer(UINT_PTR nIDEvent) 
{
	switch (nIDEvent)
	{
	case idScrollUp:
		if (m_btnUp.IsPressed ())
		{
			ScrollUp ();
		}
		return;

	case idScrollDn:
		if (m_btnDown.IsPressed ())
		{
			ScrollDown ();
		}
		return;
	}

	CBCGToolBar::OnTimer(nIDEvent);
}
