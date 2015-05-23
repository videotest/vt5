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

// BCGOutlookButton.cpp: implementation of the CBCGOutlookButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGOutlookButton.h"
#include "BCGOutlookBar.h"
#include "MenuImages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGOutlookButton, CBCGToolbarButton, 1)

#define BUTTON_OFFSET		10

inline static COLORREF PixelAlpha (COLORREF srcPixel, int percent);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGOutlookButton::CBCGOutlookButton()
{
	m_pWndParentBar = NULL;
	m_iPageNumber = 0;
}
//***************************************************************************************
CBCGOutlookButton::~CBCGOutlookButton()
{
}
//***************************************************************************************
void CBCGOutlookButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
						BOOL bHorz, BOOL bCustomizeMode,
						BOOL bHighlight,
						BOOL /*bDrawBorder*/,
						BOOL /*bGrayDisabledButtons*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndParentBar);

	CSize csOffset (0, 0);
	if (!bCustomizeMode &&
		(bHighlight && (m_nStyle & TBBS_PRESSED)))
	{
		csOffset = CSize (1, 1);
	}

	CRect rectBorder = rect;
	CRect rectText = rect;
	rectText.top += BUTTON_OFFSET / 2;

	if (pImages != NULL && GetImage () >= 0)
	{
		int x, y;

		CSize csImage = pImages->GetImageSize ();

		if (!bHorz)
		{
			int iImageHorzOffset = (rect.Width () - csImage.cx) / 2;
			x = rect.left + iImageHorzOffset;
			y = rect.top + BUTTON_OFFSET / 2;

			rectText.top += csImage.cy + 2;
		}
		else
		{
			int iImageVertOffset = (rect.Height () - csImage.cy) / 2;
			x = rect.left + BUTTON_OFFSET / 2;
			y = rect.top + iImageVertOffset;

			rectText.left += csImage.cx + 2;

			CRect rect = rectText;
			int iTextHeight = pDC->DrawText (m_strText, rect, 
				DT_CALCRECT | DT_WORDBREAK);

			rectText.top = rect.top + (rect.Height () - iTextHeight) / 2;
		}

		rectBorder = CRect (CPoint (x, y), csImage);
		rectBorder.InflateRect (2, 2);

		if (bHighlight &&
			m_pWndParentBar->IsDrawShadedHighlight () && !bCustomizeMode)
		{
			ShadowRect (pDC, rectBorder);
		}

		pImages->Draw (pDC, 
			x + csOffset.cx, y + csOffset.cy, GetImage (), FALSE, 
			(m_nStyle & TBBS_DISABLED) && !m_pWndParentBar->IsBackgroundTexture ());
				// Draw grayed images for "non-backgound" only!
	}
	else
	{
		if (bHighlight &&
			m_pWndParentBar->IsDrawShadedHighlight () && !bCustomizeMode)
		{
			ShadowRect (pDC, rectBorder);
		}
	}

	if (!bCustomizeMode &&
		(bHighlight || (m_nStyle & TBBS_PRESSED)))
	{
		if ((m_nStyle & TBBS_PRESSED) && bHighlight)
		{
			pDC->Draw3dRect (rectBorder,::GetSysColor (COLOR_3DDKSHADOW),
										::GetSysColor (COLOR_3DHILIGHT));
		}
		else
		{							  
			pDC->Draw3dRect (rectBorder,::GetSysColor (COLOR_3DHILIGHT),
										::GetSysColor (COLOR_3DDKSHADOW));
		}
	}

	if (m_bTextBelow && !m_strText.IsEmpty ())
	{
		if (m_nStyle & TBBS_DISABLED)
		{
			if (m_pWndParentBar->IsBackgroundTexture ())
			{
				pDC->SetTextColor (::GetSysColor (COLOR_GRAYTEXT));
			}
			else
			{
				pDC->SetTextColor (globalData.clrBtnFace);
			}
		}
		else
		{
			pDC->SetTextColor (m_pWndParentBar->GetRegularColor ());
		}

		pDC->DrawText (m_strText, rectText, 
						DT_WORDBREAK | DT_CENTER);
	}
}
//****************************************************************************************
SIZE CBCGOutlookButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CSize sizeResult = sizeDefault;

	if (!bHorz)
	{
		CRect rectText (0, 0, sizeDefault.cx, 1);
		int iTextHeight = m_bTextBelow ? pDC->DrawText (m_strText, rectText, 
			DT_CALCRECT | DT_WORDBREAK) : 0;

		sizeResult.cy = sizeDefault.cy + iTextHeight + BUTTON_OFFSET;
	}
	else
	{
		CRect rectText (0, 0, 0, sizeDefault.cy);
		int iTextHeight = 0;

		if (m_bTextBelow)
		{
			do
			{
				rectText.right ++;
				iTextHeight = pDC->DrawText (m_strText, rectText, 
								DT_CALCRECT | DT_WORDBREAK);
			}
			while (iTextHeight < pDC->GetTextExtent (m_strText).cy &&
					rectText.Height () > sizeDefault.cy);
		}

		sizeResult.cx = sizeDefault.cx + rectText.Width () + BUTTON_OFFSET;
	}

	return sizeResult;
}
//***************************************************************************************
void CBCGOutlookButton::OnChangeParentWnd (CWnd* pWndParent)
{
	m_pWndParentBar = DYNAMIC_DOWNCAST (CBCGOutlookBar, pWndParent);
	ASSERT_VALID (m_pWndParentBar);
}
//***************************************************************************************
BOOL CBCGOutlookButton::CanBeDropped (CBCGToolBar* pToolbar)
{
	ASSERT_VALID (pToolbar);
	return pToolbar->IsKindOf (RUNTIME_CLASS (CBCGOutlookBar));
}
//***************************************************************************************
void CBCGOutlookButton::SetImage (int iImage)
{
	// Don't add image to hash!
	m_iImage = iImage;
}
//***************************************************************************************
void CBCGOutlookButton::ShadowRect (CDC* pDC, CRect& rect)
{
	ASSERT_VALID (pDC);

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (pDC))
	{
		return;
	}

	CBitmap	bmpMem;
	if (!bmpMem.CreateCompatibleBitmap (pDC, rect.Width (), rect.Height ()))
	{
		return;
	}

	CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);
	ASSERT (pOldBmp != NULL);

	LPBITMAPINFO lpbi;

		// Fill in the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) ];
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbi->bmiHeader.biWidth = rect.Width ();
	lpbi->bmiHeader.biHeight = rect.Height ();
	lpbi->bmiHeader.biPlanes = 1;
	lpbi->bmiHeader.biBitCount = 32;
	lpbi->bmiHeader.biCompression = BI_RGB;
	lpbi->bmiHeader.biSizeImage = rect.Width () * rect.Height ();
	lpbi->bmiHeader.biXPelsPerMeter = 0;
	lpbi->bmiHeader.biYPelsPerMeter = 0;
	lpbi->bmiHeader.biClrUsed = 0;
	lpbi->bmiHeader.biClrImportant = 0;

	COLORREF* pBits = NULL;
	HBITMAP hmbpDib = CreateDIBSection (
		dcMem.m_hDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
		NULL, NULL);

	if (hmbpDib == NULL || pBits == NULL)
	{
		delete lpbi;
		return;
	}

	dcMem.SelectObject (hmbpDib);
	dcMem.BitBlt (0, 0, rect.Width (), rect.Height (), pDC, rect.left, rect.top, SRCCOPY);

	for (int x = 0; x < rect.Width (); x++)
	{
		for (int y = 0; y < rect.Width (); y++)
		{
			COLORREF* pColor = (COLORREF*) (pBits + rect.Width () * y + x);
			*pColor = PixelAlpha (*pColor, 85);
		}
	}

	//-----------------------------------------
	// Copy shadowed bitmap back to the screen:
	//-----------------------------------------
	pDC->BitBlt (rect.left, rect.top, rect.Width (), rect.Height (),
				&dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject (pOldBmp);
	DeleteObject (hmbpDib);
	delete lpbi;
}

COLORREF PixelAlpha (COLORREF srcPixel, int percent)
{
	// My formula for calculating the transpareny is as
	// follows (for each single color):
	//
	//							   percent
	// destPixel = sourcePixel * ( ------- )
	//                               100
	//
	// This is not real alpha blending, as it only modifies the brightness,
	// but not the color (a real alpha blending had to mix the source and
	// destination pixels, e.g. mixing green and red makes yellow).
	// For our nice "menu" shadows its good enough.

	COLORREF clrFinal = RGB ( (GetRValue (srcPixel) * percent) / 100, 
							  (GetGValue (srcPixel) * percent) / 100, 
							  (GetBValue (srcPixel) * percent) / 100);

	return (clrFinal);

}
