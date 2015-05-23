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

// BCGControlBarImpl.cpp: implementation of the CBCGControlBarImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGControlBarImpl.h"
#include "BCGToolBar.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGControlBarImpl::CBCGControlBarImpl(CControlBar* pBar) :
	m_pBar (pBar)
{
	ASSERT_VALID (m_pBar);
}
//*****************************************************************************************
CBCGControlBarImpl::~CBCGControlBarImpl()
{
}
//****************************************************************************************
void CBCGControlBarImpl::DrawNcArea ()
{
	CWindowDC dc(m_pBar);

	CRect rectClient;
	m_pBar->GetClientRect(rectClient);

	CRect rectWindow;
	m_pBar->GetWindowRect(rectWindow);

	m_pBar->ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect (rectClient);

	// By Guillaume Nodet
	{
		MSG* pMsg = &AfxGetThreadState()->m_lastSentMsg;

		ASSERT (pMsg->hwnd == m_pBar->m_hWnd);
		ASSERT (pMsg->message == WM_NCPAINT);

		CRgn* pRgn = NULL;
		if (pMsg->wParam != 1 && 
			(pRgn = CRgn::FromHandle ((HRGN) pMsg->wParam)) != NULL)
		{
			CRect rect;
			m_pBar->GetWindowRect (rect);
			pRgn->OffsetRgn (- rect.TopLeft ());
			dc.SelectClipRgn (pRgn, RGN_AND);
		}
	}

	// draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);

	// erase parts not drawn
	dc.IntersectClipRect(rectWindow);
	dc.FillRect (rectWindow, &globalData.brBtnFace);

	// draw gripper in non-client area
	if ((m_pBar->m_dwStyle & (CBRS_GRIPPER|CBRS_FLOATING)) != CBRS_GRIPPER)
	{
		dc.SelectClipRgn (NULL);
		return;
	}

	BOOL bHorz = (m_pBar->m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
	CRect rectGripper;
	
	m_pBar->GetWindowRect (&rectGripper);

	// ET: Set gripper rect only to actual area, needed for
	// centering the gripper
	CRect rcClient;
	m_pBar->GetClientRect(&rcClient);
	m_pBar->ClientToScreen(&rcClient);

	if(bHorz) 
	{
		rectGripper.right = min( rectGripper.right, rcClient.left - 1);
	}
	else 
	{
		rectGripper.bottom = min( rectGripper.bottom, rcClient.top - 1);
	}
	////////////////////////////////////////////////////////

	rectGripper.OffsetRect (-rectGripper.left, -rectGripper.top);
	DrawGripper(&dc, rectGripper, bHorz);

	dc.SelectClipRgn (NULL);
}
//**********************************************************************************************
void CBCGControlBarImpl::DrawGripper (CDC* pDC, CRect rectGripper, BOOL bHorz)
//--------------------------------------------
// Support for single-gripper - by Alan Fritz
//--------------------------------------------
{
	ASSERT_VALID (pDC);

	const BOOL bSingleGripper = CBCGToolBar::IsLook2000 ();

	const int iGripperSize = 3;
	const int iGripperOffset = bSingleGripper ? 0 : 1;
	const int iLinesNum = bSingleGripper ? 1 : 2;

	if (bHorz)
	{
		//-----------------
		// Gripper at left:
		//-----------------
		rectGripper.DeflateRect (0, bSingleGripper ? 3 : 2);

		// ET: Center the grippers
		rectGripper.left = iGripperOffset + rectGripper.CenterPoint().x - 
			( iLinesNum*iGripperSize + (iLinesNum-1)*iGripperOffset) / 2;

		rectGripper.right = rectGripper.left + iGripperSize;

 
		for (int i = 0; i < iLinesNum; i ++)
		{
			pDC->Draw3dRect (rectGripper, 
							globalData.clrBtnHilite,
							globalData.clrBtnShadow);

			// ET: not used for NewFlat look
			if(! bSingleGripper ) {
				//-----------------------------------
				// To look same as MS Office Gripper!
				//-----------------------------------
				pDC->SetPixel (CPoint (rectGripper.left, rectGripper.bottom - 1),
								globalData.clrBtnHilite);
			}

			rectGripper.OffsetRect (iGripperSize+1, 0);
		}
	} 
	else 
	{
		//----------------
		// Gripper at top:
		//----------------
		rectGripper.top += iGripperOffset;
		rectGripper.DeflateRect (bSingleGripper ? 3 : 2, 0);

		// ET: Center the grippers
		rectGripper.top = iGripperOffset + rectGripper.CenterPoint().y - 
			( iLinesNum*iGripperSize + (iLinesNum-1)) / 2;

		rectGripper.bottom = rectGripper.top + iGripperSize;

		for (int i = 0; i < iLinesNum; i ++)
		{
			pDC->Draw3dRect (rectGripper,
							globalData.clrBtnHilite,
							globalData.clrBtnShadow);

			// ET: not used for NewFlat look
			if(! bSingleGripper ) {
				//-----------------------------------
				// To look same as MS Office Gripper!
				//-----------------------------------
				pDC->SetPixel (CPoint (rectGripper.right - 1, rectGripper.top),
								globalData.clrBtnHilite);
			}

			rectGripper.OffsetRect (0, iGripperSize+1);
		}
	}
}
//******************************************************************************************
void CBCGControlBarImpl::DrawBorders(CDC* pDC, CRect& rect)
{
	ASSERT_VALID(m_pBar);
	ASSERT_VALID(pDC);

	DWORD dwStyle = m_pBar->m_dwStyle;
	if (!(dwStyle & CBRS_BORDER_ANY))
		return;

	COLORREF clrBckOld = pDC->GetBkColor ();	// FillSolidRect changes it

	// Copyright (C) 1997,'98 by Joerg Koenig
	// All rights reserved
	//

	COLORREF clr = (m_pBar->m_dwStyle & CBRS_BORDER_3D) ? globalData.clrBtnHilite : globalData.clrBtnShadow;
	if(m_pBar->m_dwStyle & CBRS_BORDER_LEFT)
		pDC->FillSolidRect(0, 0, 1, rect.Height() - 1, clr);
	if(m_pBar->m_dwStyle & CBRS_BORDER_TOP)
		pDC->FillSolidRect(0, 0, rect.Width()-1 , 1, clr);
	if(m_pBar->m_dwStyle & CBRS_BORDER_RIGHT)
		pDC->FillSolidRect(rect.right, 1, -1, rect.Height() - 1, globalData.clrBtnShadow);
	if(m_pBar->m_dwStyle & CBRS_BORDER_BOTTOM)
		pDC->FillSolidRect(0, rect.bottom, rect.Width()-1, -1, globalData.clrBtnShadow);

	// if undockable toolbar at top of frame, apply special formatting to mesh
	// properly with frame menu
	if(!m_pBar->m_pDockContext) 
	{
		pDC->FillSolidRect(0,0,rect.Width(),1,globalData.clrBtnShadow);
		pDC->FillSolidRect(0,1,rect.Width(),1,globalData.clrBtnHilite);
	}

	if (dwStyle & CBRS_BORDER_LEFT)
		++rect.left;
	if (dwStyle & CBRS_BORDER_TOP)
		++rect.top;
	if (dwStyle & CBRS_BORDER_RIGHT)
		--rect.right;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		--rect.bottom;

	// Restore Bk color:
	pDC->SetBkColor (clrBckOld);
}
//****************************************************************************************
void CBCGControlBarImpl::CalcNcSize (NCCALCSIZE_PARAMS FAR* lpncsp)
{
	ASSERT_VALID(m_pBar);

	CRect rect; rect.SetRectEmpty();
	BOOL bHorz = (m_pBar->m_dwStyle & CBRS_ORIENT_HORZ) != 0;

	m_pBar->CalcInsideRect(rect, bHorz);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top;

	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}
//*************************************************************************************
BOOL CBCGControlBarImpl::GetBackgroundFromParent (CDC* pDC)
{
	ASSERT_VALID (pDC);

	CWnd* pParent = m_pBar->GetParent ();
	ASSERT_VALID (pParent);

	CPoint pt (0, 0);
	m_pBar->MapWindowPoints (pParent, &pt, 1);

	pt = pDC->OffsetWindowOrg (pt.x, pt.y);
	LRESULT lRes = pParent->SendMessage (WM_ERASEBKGND, (WPARAM)pDC->m_hDC);
	pDC->SetWindowOrg(pt.x, pt.y);

	return (BOOL) lRes;
}
