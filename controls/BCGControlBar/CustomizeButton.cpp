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

// CustomizeButton.cpp: implementation of the CCustomizeButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomizeButton.h"
#include "Globals.h"
#include "BCGToolbar.h"
#include "MenuImages.h"
#include "BCGToolbarComboBoxButton.h"
#include "bcgbarres.h"
#include "bcglocalres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CCustomizeButton, CBCGToolbarMenuButton, VERSIONABLE_SCHEMA | 1)

static const int iMargin = 2;
static const int iBlank = 10;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomizeButton::CCustomizeButton()
{
	m_uiCustomizeCmdId = 0;
	m_pWndParentToolbar = NULL;
}
//****************************************************************************************
CCustomizeButton::CCustomizeButton(UINT uiCustomizeCmdId, const CString& strCustomizeText)
{
	m_uiCustomizeCmdId = uiCustomizeCmdId;
	m_strCustomizeText = strCustomizeText;
	m_pWndParentToolbar = NULL;
}
//****************************************************************************************
CCustomizeButton::~CCustomizeButton()
{
}
//****************************************************************************************
void CCustomizeButton::OnChangeParentWnd (CWnd* pWndParent)
{
	m_pWndParentToolbar = DYNAMIC_DOWNCAST (CBCGToolBar, pWndParent);
	m_pWndParent = pWndParent;
	m_bText = FALSE;
}
//****************************************************************************************
void CCustomizeButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* /*pImages*/,
			BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight,
			BOOL /*bDrawBorder*/, BOOL /*bGrayDisabledButtons*/)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	if (m_bMenuMode)
	{
		ASSERT (FALSE);	// Customize button is available for 
						// the "pure" toolbars only!
		return;
	}

	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	//--------------------
	// Draw button border:
	//--------------------
	if (!bCustomizeMode)
	{
		CRect rectBorder = rect;
		if (bHorz)
		{
			rectBorder.left += iBlank;
		}
		else
		{
			rectBorder.top += iBlank;
		}

		if ((m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) ||
			m_pPopupMenu != NULL)
		{
			//-----------------------
			// Pressed in or checked:
			//-----------------------
			pDC->Draw3dRect (&rectBorder,
				afxData.clrBtnShadow,
				afxData.clrBtnHilite);
		}
		else if (bHighlight && !(m_nStyle & TBBS_DISABLED) &&
			!(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			pDC->Draw3dRect (&rectBorder,	afxData.clrBtnHilite,
											afxData.clrBtnShadow);
		}
	}

	CSize sizeImage = CMenuImages::Size ();

	if ((int) m_uiCustomizeCmdId > 0)
	{
		//-----------------
		// Draw menu image:
		//-----------------
		CRect rectMenu = rect;
		if (bHorz)
		{
			rectMenu.top = rectMenu.bottom - sizeImage.cy - 2 * iMargin;
			rectMenu.left += iBlank;
		}
		else
		{
			rectMenu.right = rectMenu.left + sizeImage.cx + 2 * iMargin;
			rectMenu.top += iBlank;
		}

		rectMenu.DeflateRect (
			(rectMenu.Width () - sizeImage.cx) / 2,
			(rectMenu.Height () - sizeImage.cy) / 2);

		if ((m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) || m_pPopupMenu != NULL)
		{
			rectMenu.OffsetRect (1, 1);
		}

		CMenuImages::Draw (	pDC, 
			bHorz ? CMenuImages::IdArowDown : CMenuImages::IdArowLeft,
							rectMenu.TopLeft ());
	}

	if (!m_lstInvisibleButtons.IsEmpty ())
	{
		//-------------------
		// Draw "more" image:
		//-------------------
		CRect rectMore = rect;
		if (bHorz)
		{
			rectMore.bottom = rectMore.top + sizeImage.cy + 2 * iMargin;
			rectMore.left += iBlank;
		}
		else
		{
			rectMore.left = rectMore.right - sizeImage.cx - 2 * iMargin;
			rectMore.top += iBlank;
		}

		rectMore.DeflateRect (
			(rectMore.Width () - sizeImage.cx) / 2,
			(rectMore.Height () - sizeImage.cy) / 2);

		if ((m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) || m_pPopupMenu != NULL)
		{
			rectMore.OffsetRect (1, 1);
		}

		CMenuImages::Draw (	pDC, 
							bHorz ? CMenuImages::IdMoreButtons : CMenuImages::IdArowShowAll, 
							rectMore.TopLeft ());
	}
}
//*****************************************************************************************
CBCGPopupMenu* CCustomizeButton::CreatePopupMenu ()
{
	CBCGPopupMenu* pMenu = CBCGToolbarMenuButton::CreatePopupMenu ();
	if (pMenu == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	BOOL bIsFirst = TRUE;

	for (POSITION pos = m_lstInvisibleButtons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_lstInvisibleButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		//--------------------------------------
		// Don't insert first or last separator:
		//--------------------------------------
		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			if (bIsFirst)
			{
				continue;
			}

			if (pos == NULL)	// Last
			{
				break;
			}
		}

		int iIndex = -1;

		bIsFirst = FALSE;

		if (pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarMenuButton)))
		{
			iIndex = pMenu->InsertItem (*((CBCGToolbarMenuButton*) pButton));
		}
		else
		{
			if (pButton->m_nID == 0)
			{
				iIndex = pMenu->InsertSeparator ();
			}
			else
			{
				iIndex = pMenu->InsertItem (
					CBCGToolbarMenuButton (pButton->m_nID, NULL, 
						pButton->GetImage (),
						pButton->m_strText,
						pButton->m_bUserButton));
			}
		}

		if (iIndex < 0)
		{
			ASSERT (FALSE);
			continue;
		}

		CBCGToolbarMenuButton* pMenuButton = pMenu->GetMenuItem (iIndex);
		if (pMenuButton == NULL)
		{
			ASSERT (pMenuButton != NULL);
			continue;
		}

		//-----------------------------------------------------
		// Text may be undefined, bring it from the tooltip :-(
		//-----------------------------------------------------
		if ((pMenuButton->m_strText.IsEmpty () || 
			pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarComboBoxButton)))
				&& pMenuButton->m_nID != 0)
		{
			CString strMessage;
			int iOffset;
			if (strMessage.LoadString (pMenuButton->m_nID) &&
				(iOffset = strMessage.Find (_T('\n'))) != -1)
			{
				pMenuButton->m_strText = strMessage.Mid (iOffset + 1);
			}
		}
	}

	if ((int) m_uiCustomizeCmdId > 0)
	{
		if (!m_lstInvisibleButtons.IsEmpty ())
		{
			pMenu->InsertSeparator ();
		}

		pMenu->InsertItem (CBCGToolbarMenuButton (m_uiCustomizeCmdId, NULL, -1,
			m_strCustomizeText));
	}

	return pMenu;
}
//*****************************************************************************************
SIZE CCustomizeButton::OnCalculateSize (CDC* /*pDC*/, const CSize& sizeDefault, BOOL bHorz)
{
	if (m_strText.IsEmpty ())
	{
		CBCGLocalResource locaRes;
		m_strText.LoadString (IDS_BCGBARRES_MORE_BUTTONS);

		ASSERT (!m_strText.IsEmpty ());
	}

	if (m_pWndParentToolbar != NULL && m_pWndParentToolbar->IsFloating ())
	{
		return CSize (0, 0);
	}

	if (bHorz)
	{
		return CSize (	iBlank + CMenuImages::Size ().cx + 2 * iMargin, 
						sizeDefault.cy);
	}
	else
	{
		return CSize (	sizeDefault.cx, 
						iBlank + CMenuImages::Size ().cy + 2 * iMargin);
	}
}
//*****************************************************************************************
void CCustomizeButton::CopyFrom (const CBCGToolbarButton& s)
{
	CBCGToolbarMenuButton::CopyFrom (s);
	const CCustomizeButton& src = (const CCustomizeButton&) s;

	m_uiCustomizeCmdId = src.m_uiCustomizeCmdId;
	m_strCustomizeText = src.m_strCustomizeText;
}
