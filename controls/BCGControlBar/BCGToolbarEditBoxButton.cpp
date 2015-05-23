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

// BCGToolbarEditBoxButton.cpp: implementation of the CBCGToolbarEditBoxButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGToolbar.h"
#include "globals.h"
#include "BCGToolbarEditBoxButton.h"
#include "MenuImages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGToolbarEditBoxButton, CBCGToolbarButton, 1)

static const int iDefaultSize = 150;
static const int iHorzMargin = 3;
static const int iVertMargin = 1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolbarEditBoxButton::CBCGToolbarEditBoxButton()
{
	m_dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	m_iWidth = iDefaultSize;

	Initialize ();
}
//**************************************************************************************
CBCGToolbarEditBoxButton::CBCGToolbarEditBoxButton (UINT uiId,
			int iImage,
			DWORD dwStyle,
			int iWidth) :
			CBCGToolbarButton (uiId, iImage)
{
	m_dwStyle = dwStyle | WS_CHILD | WS_VISIBLE;
	m_iWidth = (iWidth == 0) ? iDefaultSize : iWidth;

	Initialize ();
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::Initialize ()
{
	m_pWndEdit = NULL;
	m_bHorz = TRUE;
	m_bChangingText = FALSE;
}
//**************************************************************************************
CBCGToolbarEditBoxButton::~CBCGToolbarEditBoxButton()
{
	if (m_pWndEdit != NULL)
	{
		TRACE1("Deleted edit with HWND %d\n", m_pWndEdit->GetSafeHwnd());
		m_pWndEdit->DestroyWindow ();
		delete m_pWndEdit;
	}
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::CopyFrom (const CBCGToolbarButton& s)
{
	CBCGToolbarButton::CopyFrom (s);

	const CBCGToolbarEditBoxButton& src = (const CBCGToolbarEditBoxButton&) s;

	m_dwStyle = src.m_dwStyle;
	m_iWidth = src.m_iWidth;
	m_strContents = src.m_strContents;
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::Serialize (CArchive& ar)
{
	CBCGToolbarButton::Serialize (ar);

	if (ar.IsLoading ())
	{
		ar >> m_iWidth;
		m_rect.right = m_rect.left + m_iWidth;
		ar >> m_dwStyle;
		ar >> m_strContents;
	}
	else
	{
		ar << m_iWidth;
		ar << m_dwStyle;

		if (m_pWndEdit != NULL)
		{
			m_pWndEdit->GetWindowText(m_strContents);
		}
		else
		{
			m_strContents.Empty();
		}

		ar << m_strContents;
	}
}
//**************************************************************************************
SIZE CBCGToolbarEditBoxButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	m_bHorz = bHorz;

	if (bHorz)
	{
		if (m_pWndEdit->GetSafeHwnd () != NULL && !m_bIsHidden)
		{
			m_pWndEdit->ShowWindow (SW_SHOWNOACTIVATE);
		}

		if (m_bTextBelow && !m_strText.IsEmpty())
		{
			CRect rectText (0, 0, 
				m_iWidth, sizeDefault.cy);
			pDC->DrawText (	m_strText, rectText, 
							DT_CENTER | DT_CALCRECT | DT_WORDBREAK);
			m_sizeText = rectText.Size ();
		}
		else
			m_sizeText = CSize(0,0);

		return CSize (m_iWidth, sizeDefault.cy + m_sizeText.cy);
	}
	else
	{
		if (m_pWndEdit->GetSafeHwnd () != NULL)
		{
			m_pWndEdit->ShowWindow (SW_HIDE);
		}
		m_sizeText = CSize(0,0);

		return CBCGToolbarButton::OnCalculateSize (pDC, sizeDefault, bHorz);
	}
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::OnMove ()
{
	if (m_pWndEdit->GetSafeHwnd () == NULL ||
		(m_pWndEdit->GetStyle () & WS_VISIBLE) == 0)
	{
		return;
	}

	CRect rectEdit;
	m_pWndEdit->GetWindowRect (rectEdit);

	m_pWndEdit->SetWindowPos (NULL, 
		m_rect.left + iHorzMargin, 
		m_rect.top + (m_rect.Height () - m_sizeText.cy - rectEdit.Height ()) / 2,
		m_rect.Width () - 2 * iHorzMargin, 
		globalData.GetTextHeight() + 2 * iVertMargin,
		SWP_NOZORDER | SWP_NOACTIVATE);
	m_pWndEdit->SetSel(-1, 0);

	AdjustRect ();
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::OnSize (int iSize)
{
	m_iWidth = iSize;
	m_rect.right = m_rect.left + m_iWidth;

	if (m_pWndEdit->GetSafeHwnd () != NULL &&
		(m_pWndEdit->GetStyle () & WS_VISIBLE))
	{
		m_pWndEdit->SetWindowPos (NULL, 
			m_rect.left + iHorzMargin, m_rect.top,
			m_rect.Width () - 2 * iHorzMargin, 
			globalData.GetTextHeight() + 2 * iVertMargin,
			SWP_NOZORDER | SWP_NOACTIVATE);
		m_pWndEdit->SetSel(-1, 0);

		AdjustRect ();
	}
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::OnChangeParentWnd (CWnd* pWndParent)
{
	if (m_pWndEdit->GetSafeHwnd () != NULL)
	{
		CWnd* pWndParentCurr = m_pWndEdit->GetParent ();
		ASSERT (pWndParentCurr != NULL);

		if (pWndParent != NULL &&
			pWndParentCurr->GetSafeHwnd () == pWndParent->GetSafeHwnd ())
		{
			return;
		}
		
		m_pWndEdit->GetWindowText(m_strContents);

		TRACE1("Deleted edit with HWND %d\n", m_pWndEdit->GetSafeHwnd());
		m_pWndEdit->DestroyWindow ();
		delete m_pWndEdit;
		m_pWndEdit = NULL;
	}

	if (pWndParent == NULL || pWndParent->GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rect = m_rect;
	rect.InflateRect (-2, 0);
	rect.bottom = rect.top + globalData.GetTextHeight() + 2 * iVertMargin;

	if ((m_pWndEdit = CreateEdit (pWndParent, rect)) == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	AdjustRect ();

	m_pWndEdit->SetFont (&globalData.fontRegular);

	CString sText;
	m_pWndEdit->GetWindowText(sText);
	if (sText.IsEmpty())
	{
		m_bChangingText = TRUE;
		m_pWndEdit->SetWindowText(m_strContents);
		m_bChangingText = FALSE;
	}
	else
	{
		m_strContents = sText;
	}
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::AdjustRect ()
{
	if (m_pWndEdit->GetSafeHwnd () == NULL ||
		(m_pWndEdit->GetStyle () & WS_VISIBLE) == 0 ||
		m_rect.IsRectEmpty ())
	{
		return;
	}

	m_pWndEdit->GetWindowRect (&m_rect);
	m_pWndEdit->ScreenToClient (&m_rect);
	m_pWndEdit->MapWindowPoints (m_pWndEdit->GetParent (), &m_rect);
	m_rect.InflateRect (iHorzMargin, iVertMargin);
}
//**************************************************************************************
BOOL CBCGToolbarEditBoxButton::NotifyCommand (int iNotifyCode)
{
	if (m_pWndEdit->GetSafeHwnd () == NULL)
	{
		return FALSE;
	}

	switch (iNotifyCode)
	{
		case EN_UPDATE:
		{
			m_pWndEdit->GetWindowText(m_strContents);

			//------------------------------------------------------
			// Try set selection in ALL editboxes with the same ID:
			//------------------------------------------------------
			CObList listButtons;
			if (CBCGToolBar::GetCommandButtons (m_nID, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
				{
					CBCGToolbarEditBoxButton* pEdit = 
						DYNAMIC_DOWNCAST (CBCGToolbarEditBoxButton, listButtons.GetNext (posCombo));

					if ((pEdit != NULL) && (pEdit != this))
					{
						pEdit->SetContents(m_strContents);
					}
				}
			}
		}

		return !m_bChangingText;
	}

	return FALSE;
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::OnAddToCustomizePage ()
{
	CObList listButtons;	// Existing buttons with the same command ID

	if (CBCGToolBar::GetCommandButtons (m_nID, listButtons) == 0)
	{
		return;
	}

	CBCGToolbarEditBoxButton* pOther = 
		(CBCGToolbarEditBoxButton*) listButtons.GetHead ();
	ASSERT_VALID (pOther);
	ASSERT_KINDOF (CBCGToolbarEditBoxButton, pOther);

	CopyFrom (*pOther);
}
//**************************************************************************************
HBRUSH CBCGToolbarEditBoxButton::OnCtlColor (CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor (::GetSysColor (COLOR_WINDOWTEXT));
	pDC->SetBkColor (::GetSysColor (COLOR_WINDOW));

	return ::GetSysColorBrush (COLOR_WINDOW);
}
//**************************************************************************************
void CBCGToolbarEditBoxButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
						BOOL bHorz, BOOL bCustomizeMode,
						BOOL bHighlight, BOOL bDrawBorder, 
						BOOL bGrayDisabledButtons)
{
	if (m_pWndEdit->GetSafeHwnd () == NULL ||
		(m_pWndEdit->GetStyle () & WS_VISIBLE) == 0)
	{
		CBCGToolbarButton::OnDraw (pDC, rect, pImages,
							bHorz, bCustomizeMode,
							bHighlight, bDrawBorder, 
							bGrayDisabledButtons);
	}
	else
	{
		// Draw edge around the place...
		CRect rectAround = rect;
		rectAround.InflateRect(0, 2);
		pDC->DrawEdge(rectAround, EDGE_SUNKEN, BF_RECT);
		if ((m_bTextBelow && bHorz) && !m_strText.IsEmpty())
		{
			//--------------------
			// Draw button's text:
			//--------------------
			BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
				(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

			pDC->SetTextColor (bDisabled ?
								globalData.clrGrayedText : 
									(bHighlight) ? CBCGToolBar::GetHotTextColor () :
											globalData.clrBtnText);
			CRect rectText;
			rectText.left = (rect.left + rect.right - m_sizeText.cx) / 2;
			rectText.right = (rect.left + rect.right + m_sizeText.cx) / 2;
			rectText.top = rect.bottom + rect.top;
			rectText.bottom = rectText.top + m_sizeText.cy;
			pDC->DrawText (m_strText, &rectText, DT_CENTER | DT_WORDBREAK);
		}
	}
}
//**************************************************************************************
BOOL CBCGToolbarEditBoxButton::OnClick (CWnd* /*pWnd*/, BOOL /*bDelay*/)
{	
	return m_pWndEdit->GetSafeHwnd () != NULL &&
			(m_pWndEdit->GetStyle () & WS_VISIBLE);
}
//**************************************************************************************
int CBCGToolbarEditBoxButton::OnDrawOnCustomizeList (
	CDC* pDC, const CRect& rect, BOOL bSelected)
{
	int iWidth = CBCGToolbarButton::OnDrawOnCustomizeList (pDC, rect, bSelected);

	//------------------------------
	// Simulate editbox appearance:
	//------------------------------
	CRect rectEdit = rect;
	int iEditWidth = rect.Width () - iWidth;

	if (iEditWidth < 8)
	{
		iEditWidth = 8;
	}

	rectEdit.left = rectEdit.right - iEditWidth;
	rectEdit.DeflateRect (2, 3);

	pDC->FillSolidRect (rectEdit, ::GetSysColor (COLOR_WINDOW));
	pDC->DrawEdge (&rectEdit, EDGE_SUNKEN, BF_RECT);

	return rect.Width ();
}
//********************************************************************************************
CEdit* CBCGToolbarEditBoxButton::CreateEdit (CWnd* pWndParent, const CRect& rect)
{
	CEdit* pWndEdit = new CEdit;
	if (!pWndEdit->Create (m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndEdit;
		return NULL;
	}
	return pWndEdit;
}
//****************************************************************************************
void CBCGToolbarEditBoxButton::OnShow (BOOL bShow)
{
	if (m_pWndEdit->GetSafeHwnd () != NULL)
	{
		if (bShow)
		{
			m_pWndEdit->ShowWindow (SW_SHOWNOACTIVATE);
			OnMove ();
		}
		else
		{
			m_pWndEdit->ShowWindow (SW_HIDE);
		}
	}
}

void CBCGToolbarEditBoxButton::SetContents(const CString& sContents)
{
	if (m_strContents == sContents)
		return;

	m_strContents = sContents;
	if (m_pWndEdit != NULL)
	{
		m_bChangingText = TRUE;
		m_pWndEdit->SetWindowText(m_strContents);
		m_bChangingText = FALSE;
	}
}

const CRect CBCGToolbarEditBoxButton::GetInvalidateRect () const
{
	if ((m_bTextBelow && m_bHorz) && !m_strText.IsEmpty())
	{
		CRect rect;
		rect.left = (m_rect.left + m_rect.right - m_sizeText.cx) / 2;
		rect.right = (m_rect.left + m_rect.right + m_sizeText.cx) / 2;
		rect.top = m_rect.top;
		rect.bottom = m_rect.bottom + m_rect.top + m_sizeText.cy;
		return rect;
	}
	else
		return m_rect;
}
