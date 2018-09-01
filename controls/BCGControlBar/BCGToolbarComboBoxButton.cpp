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

// BCGToolbarComboBoxButton.cpp: implementation of the CBCGToolbarComboBoxButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGToolbar.h"
#include "globals.h"
#include "BCGToolbarComboBoxButton.h"
#include "BCGToolbarMenuButton.h"
#include "MenuImages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGToolbarComboBoxButton, CBCGToolbarButton, 1)

static const int iComboHeight = 150;
static const int iDefaultSize = 150;
static const int iHorzMargin = 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolbarComboBoxButton::CBCGToolbarComboBoxButton()
{
	m_dwStyle = WS_CHILD | WS_VISIBLE | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | WS_VSCROLL;
	m_iWidth = iDefaultSize;

	Initialize ();
}
//**************************************************************************************
CBCGToolbarComboBoxButton::CBCGToolbarComboBoxButton (UINT uiId,
			int iImage,
			DWORD dwStyle,
			int iWidth) :
			CBCGToolbarButton (uiId, iImage)
{
	m_dwStyle = dwStyle | WS_CHILD | WS_VISIBLE | WS_VSCROLL;
	m_iWidth = (iWidth == 0) ? iDefaultSize : iWidth;

	Initialize ();
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::Initialize ()
{
	m_iSelIndex = -1;
	m_pWndCombo = NULL;
	m_bHorz = TRUE;
	m_rectCombo.SetRectEmpty ();
	m_nDropDownHeight = iComboHeight;
}
//**************************************************************************************
CBCGToolbarComboBoxButton::~CBCGToolbarComboBoxButton()
{
	if (m_pWndCombo != NULL)
	{
		m_pWndCombo->DestroyWindow ();
		delete m_pWndCombo;
	}
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::CopyFrom (const CBCGToolbarButton& s)
{
	CBCGToolbarButton::CopyFrom (s);
	POSITION pos;

	m_lstItems.RemoveAll ();

	const CBCGToolbarComboBoxButton& src = (const CBCGToolbarComboBoxButton&) s;
	for (pos = src.m_lstItems.GetHeadPosition (); pos != NULL;)
	{
		m_lstItems.AddTail (src.m_lstItems.GetNext (pos));
	}

	m_lstItemData.RemoveAll ();
	for (pos = src.m_lstItemData.GetHeadPosition (); pos != NULL;)
	{
		m_lstItemData.AddTail (src.m_lstItemData.GetNext (pos));
	}

	DuplicateData ();
	ASSERT (m_lstItemData.GetCount () == m_lstItems.GetCount ());

	m_dwStyle = src.m_dwStyle;
	m_iWidth = src.m_iWidth;
	m_iSelIndex = src.m_iSelIndex;
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::Serialize (CArchive& ar)
{
	CBCGToolbarButton::Serialize (ar);

	if (ar.IsLoading ())
	{
		ar >> m_iWidth;
		m_rect.right = m_rect.left + m_iWidth;
		ar >> m_dwStyle;
		ar >> m_iSelIndex;
		ar >> m_strEdit;

		m_lstItems.Serialize (ar);

		m_lstItemData.RemoveAll ();
		for (int i = 0; i < m_lstItems.GetCount (); i ++)
		{
			long lData;
			ar >> lData;
			m_lstItemData.AddTail ((DWORD) lData);
		}

		DuplicateData ();
		ASSERT (m_lstItemData.GetCount () == m_lstItems.GetCount ());

		SelectItem (m_iSelIndex);
	}
	else
	{
		ar << m_iWidth;
		ar << m_dwStyle;
		ar << m_iSelIndex;
		ar << m_strEdit;

		if (m_pWndCombo != NULL)
		{
			m_lstItems.RemoveAll ();
			m_lstItemData.RemoveAll ();

			for (int i = 0; i < m_pWndCombo->GetCount (); i ++)
			{
				CString str;
				m_pWndCombo->GetLBText (i, str);

				m_lstItems.AddTail (str);
				m_lstItemData.AddTail (m_pWndCombo->GetItemData (i));
			}
		}

		m_lstItems.Serialize (ar);

		for (POSITION pos = m_lstItemData.GetHeadPosition (); pos != NULL;)
		{
			DWORD dwData = m_lstItemData.GetNext (pos);
			ar << (long) dwData;
		}

		ASSERT (m_lstItemData.GetCount () == m_lstItems.GetCount ());
	}
}
//**************************************************************************************
SIZE CBCGToolbarComboBoxButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	m_bHorz = bHorz;
	m_sizeText = CSize (0, 0);

	if (bHorz)
	{
		if (m_pWndCombo->GetSafeHwnd () != NULL && !m_bIsHidden)
		{
			m_pWndCombo->ShowWindow (SW_SHOWNOACTIVATE);
		}

		//----------------
		// By Guy Hachlili
		//----------------
		if (m_bTextBelow && !m_strText.IsEmpty())
		{
			CRect rectText (0, 0, m_iWidth, sizeDefault.cy);
			pDC->DrawText (m_strText, rectText, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);
			m_sizeText = rectText.Size ();
		}

		return CSize (m_iWidth, sizeDefault.cy + m_sizeText.cy);
	}
	else
	{
		if (m_pWndCombo->GetSafeHwnd () != NULL &&
			(m_pWndCombo->GetStyle () & WS_VISIBLE))
		{
			m_pWndCombo->ShowWindow (SW_HIDE);
		}

		return CBCGToolbarButton::OnCalculateSize (pDC, sizeDefault, bHorz);
	}
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::OnMove ()
{
	if (m_pWndCombo->GetSafeHwnd () == NULL ||
		(m_pWndCombo->GetStyle () & WS_VISIBLE) == 0)
	{
		return;
	}

	CRect rectCombo;
	m_pWndCombo->GetWindowRect (rectCombo);

	m_pWndCombo->SetWindowPos (NULL, 
		m_rect.left + iHorzMargin, 
		m_rect.top + (m_rect.Height () - m_sizeText.cy - rectCombo.Height ()) / 2,
		m_rect.Width () - 2 * iHorzMargin, m_nDropDownHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);
	m_pWndCombo->SetEditSel (-1, 0);

	AdjustRect ();
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::OnSize (int iSize)
{
	m_iWidth = iSize;
	m_rect.right = m_rect.left + m_iWidth;

	if (m_pWndCombo->GetSafeHwnd () != NULL &&
		(m_pWndCombo->GetStyle () & WS_VISIBLE))
	{
		m_pWndCombo->SetWindowPos (NULL, 
			m_rect.left + iHorzMargin, m_rect.top,
			m_rect.Width () - 2 * iHorzMargin, m_nDropDownHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);
		m_pWndCombo->SetEditSel (-1, 0);

		AdjustRect ();
	}
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::OnChangeParentWnd (CWnd* pWndParent)
{
	if (m_pWndCombo->GetSafeHwnd () != NULL)
	{
		CWnd* pWndParentCurr = m_pWndCombo->GetParent ();
		ASSERT (pWndParentCurr != NULL);

		if (pWndParent != NULL &&
			pWndParentCurr->GetSafeHwnd () == pWndParent->GetSafeHwnd ())
		{
			return;
		}

		m_pWndCombo->DestroyWindow ();
		delete m_pWndCombo;
		m_pWndCombo = NULL;
	}

	if (pWndParent == NULL || pWndParent->GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rect = m_rect;
	rect.InflateRect (-2, 0);
	rect.bottom = rect.top + m_nDropDownHeight;

	if ((m_pWndCombo = CreateCombo (pWndParent, rect)) == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	AdjustRect ();

	m_pWndCombo->SetFont (&globalData.fontRegular);

	if (m_pWndCombo->GetCount () > 0)
	{
		m_lstItems.RemoveAll ();
		m_lstItemData.RemoveAll ();

		for (int i = 0; i < m_pWndCombo->GetCount (); i ++)
		{
			CString str;
			m_pWndCombo->GetLBText (i, str);

			m_lstItems.AddTail (str);
			m_lstItemData.AddTail (m_pWndCombo->GetItemData (i));
		}

		m_iSelIndex = m_pWndCombo->GetCurSel ();
	}
	else
	{
		m_pWndCombo->ResetContent ();
		ASSERT (m_lstItemData.GetCount () == m_lstItems.GetCount ());

		POSITION posData = m_lstItemData.GetHeadPosition ();
		for (POSITION pos = m_lstItems.GetHeadPosition (); pos != NULL;)
		{
			ASSERT (posData != NULL);

			CString strItem = m_lstItems.GetNext (pos);
			int iIndex = m_pWndCombo->AddString (strItem);
			
			m_pWndCombo->SetItemData (iIndex, m_lstItemData.GetNext (posData));
		}

		if (m_iSelIndex != CB_ERR)
		{
			m_pWndCombo->SetCurSel (m_iSelIndex);
		}
	}
}
//**************************************************************************************
int CBCGToolbarComboBoxButton::AddItem (LPCTSTR lpszItem, DWORD dwData)
{
	ASSERT (lpszItem != NULL);

	if (m_strEdit.IsEmpty ())
	{
		m_strEdit = lpszItem;
	}

	if (m_lstItems.Find (lpszItem) == NULL)
	{
		m_lstItems.AddTail (lpszItem);
		m_lstItemData.AddTail (dwData);
	}

	if (m_pWndCombo->GetSafeHwnd () != NULL)
	{
		int iIndex = m_pWndCombo->FindStringExact (-1, lpszItem);

		if (iIndex == CB_ERR)
		{
			iIndex = m_pWndCombo->AddString (lpszItem);
		}

		m_pWndCombo->SetCurSel (iIndex);
		m_pWndCombo->SetItemData (iIndex, dwData);
		m_pWndCombo->SetEditSel (-1, 0);
	}

	return m_lstItems.GetCount () - 1;
}
//**************************************************************************************
LPCTSTR CBCGToolbarComboBoxButton::GetItem (int iIndex) const
{
	if (iIndex == -1)	// Current selection
	{
		if (m_pWndCombo->GetSafeHwnd () == NULL)
		{
			return NULL;
		}

		iIndex = m_pWndCombo->GetCurSel ();
	}

	POSITION pos = m_lstItems.FindIndex (iIndex);
	if (pos == NULL)
	{
		return NULL;
	}

	return m_lstItems.GetAt (pos);
}
//**************************************************************************************
DWORD CBCGToolbarComboBoxButton::GetItemData (int iIndex) const
{
	if (iIndex == -1)	// Current selection
	{
		if (m_pWndCombo->GetSafeHwnd () == NULL)
		{
			return 0;
		}

		iIndex = m_pWndCombo->GetCurSel ();
	}

	if (m_pWndCombo->GetSafeHwnd () != NULL)
	{
		return m_pWndCombo->GetItemData (iIndex);
	}
	else
	{
		POSITION pos = m_lstItemData.FindIndex (iIndex);
		if (pos == NULL)
		{
			return 0;
		}

		return m_lstItemData.GetAt (pos);
	}
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::RemoveAllItems ()
{
	m_lstItems.RemoveAll ();
	m_lstItemData.RemoveAll ();

	if (m_pWndCombo->GetSafeHwnd () != NULL)
	{
		m_pWndCombo->ResetContent ();
	}

	m_strEdit.Empty ();
}
//**************************************************************************************
int CBCGToolbarComboBoxButton::GetCount () const
{
	return m_lstItems.GetCount ();
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::AdjustRect ()
{
	if (m_pWndCombo->GetSafeHwnd () == NULL ||
		(m_pWndCombo->GetStyle () & WS_VISIBLE) == 0 ||
		m_rect.IsRectEmpty ())
	{
		m_rectCombo.SetRectEmpty ();
		return;
	}

	CRect rect;
	m_pWndCombo->GetWindowRect (&m_rectCombo);
	m_pWndCombo->ScreenToClient (&m_rectCombo);
	m_pWndCombo->MapWindowPoints (m_pWndCombo->GetParent (), &m_rectCombo);

	m_rect.left = m_rectCombo.left - iHorzMargin;
	m_rect.right = m_rectCombo.right + iHorzMargin;

	if (!m_bTextBelow || m_strText.IsEmpty ())
	{
		m_rect.top = m_rectCombo.top;
		m_rect.bottom = m_rectCombo.bottom;
	}
}
//**************************************************************************************
BOOL CBCGToolbarComboBoxButton::NotifyCommand (int iNotifyCode)
{
	if (m_pWndCombo->GetSafeHwnd () == NULL)
	{
		return FALSE;
	}

	switch (iNotifyCode)
	{
	case CBN_SELENDOK:
		{
			m_iSelIndex = m_pWndCombo->GetCurSel ();
			if (m_iSelIndex < 0)
			{
				return FALSE;
			}

			m_pWndCombo->GetLBText (m_iSelIndex, m_strEdit);

			//------------------------------------------------------
			// Try set selection in ALL comboboxes with the same ID:
			//------------------------------------------------------
			CObList listButtons;
			if (CBCGToolBar::GetCommandButtons (m_nID, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
				{
					CBCGToolbarComboBoxButton* pCombo = 
						DYNAMIC_DOWNCAST (CBCGToolbarComboBoxButton, listButtons.GetNext (posCombo));
					ASSERT (pCombo != NULL);

					if (pCombo != this)
					{
						pCombo->SelectItem (m_pWndCombo->GetCurSel ());
					}
				}
			}
		}

		return TRUE;

	//////// By Guy Hachlili:
	case CBN_KILLFOCUS:
	case CBN_EDITUPDATE:
		return TRUE;

	case CBN_EDITCHANGE:
		{
			m_pWndCombo->GetWindowText (m_strEdit);
			//------------------------------------------------------
			// Try set text of ALL comboboxes with the same ID:
			//------------------------------------------------------
			CObList listButtons;
			if (CBCGToolBar::GetCommandButtons (m_nID, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo !=
					NULL;)
				{
					CBCGToolbarComboBoxButton* pCombo = 
						DYNAMIC_DOWNCAST (CBCGToolbarComboBoxButton, listButtons.GetNext
						(posCombo));
					ASSERT (pCombo != NULL);
					
					if (pCombo != this)
					{
						pCombo->GetComboBox()->SetWindowText(m_strEdit);
						pCombo->m_strEdit = m_strEdit;
					}
				}
			}
		}
		///////////////////////////////////
		return TRUE;
	}

	return FALSE;
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::OnAddToCustomizePage ()
{
	CObList listButtons;	// Existing buttons with the same command ID

	if (CBCGToolBar::GetCommandButtons (m_nID, listButtons) == 0)
	{
		return;
	}

	CBCGToolbarComboBoxButton* pOther = 
		(CBCGToolbarComboBoxButton*) listButtons.GetHead ();
	ASSERT_VALID (pOther);
	//ASSERT_KINDOF (CBCGToolbarComboBoxButton, pOther);

	CopyFrom (*pOther);
}
//**************************************************************************************
HBRUSH CBCGToolbarComboBoxButton::OnCtlColor (CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor (::GetSysColor (COLOR_WINDOWTEXT));
	pDC->SetBkColor (::GetSysColor (COLOR_WINDOW));

	return ::GetSysColorBrush (COLOR_WINDOW);
}
//**************************************************************************************
void CBCGToolbarComboBoxButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
						BOOL bHorz, BOOL bCustomizeMode,
						BOOL bHighlight,
						BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	if (m_pWndCombo->GetSafeHwnd () == NULL ||
		(m_pWndCombo->GetStyle () & WS_VISIBLE) == 0)
	{
		CBCGToolbarButton::OnDraw (pDC, rect, pImages,
							bHorz, bCustomizeMode,
							bHighlight, bDrawBorder, bGrayDisabledButtons);
	}
	else if ((m_bTextBelow && bHorz) && !m_strText.IsEmpty())
	{
		//-----------------------------------
		// Draw button's text - Guy Hachlili:
		//-----------------------------------
		BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
			(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));
		
		pDC->SetTextColor (bDisabled ?
			globalData.clrGrayedText : 
				(bHighlight) ?	CBCGToolBar::GetHotTextColor () :
								globalData.clrBtnText);
		CRect rectText = rect;
		rectText.top = (m_rectCombo.bottom + rect.bottom - m_sizeText.cy) / 2;
		
		pDC->DrawText (m_strText, &rectText, DT_CENTER | DT_WORDBREAK);
	}
}
//**************************************************************************************
BOOL CBCGToolbarComboBoxButton::OnClick (CWnd* /*pWnd*/, BOOL /*bDelay*/)
{	
	return m_pWndCombo->GetSafeHwnd () != NULL &&
			(m_pWndCombo->GetStyle () & WS_VISIBLE);
}
//**************************************************************************************
BOOL CBCGToolbarComboBoxButton::SelectItem (int iIndex)
{
	if (iIndex < 0 || iIndex >= m_lstItems.GetCount ())
	{
		return FALSE;
	}

	m_iSelIndex = iIndex;

	if (m_pWndCombo->GetSafeHwnd () == NULL)
	{
		return TRUE;
	}

	m_pWndCombo->GetLBText (iIndex, m_strEdit);
	return m_pWndCombo->SetCurSel (iIndex) != CB_ERR;
}
//**************************************************************************************
BOOL CBCGToolbarComboBoxButton::SelectItem (DWORD dwData)
{
	int iIndex = 0;
	for (POSITION pos = m_lstItemData.GetHeadPosition (); pos != NULL; iIndex ++)
	{
		if (m_lstItemData.GetNext (pos) == dwData)
		{
			return SelectItem (iIndex);
		}
	}

	return FALSE;
}
//**************************************************************************************
BOOL CBCGToolbarComboBoxButton::SelectItem (LPCTSTR lpszText)
{
	ASSERT (lpszText != NULL);

	int iIndex = 0;
	for (POSITION pos = m_lstItems.GetHeadPosition (); pos != NULL; iIndex ++)
	{
		if (m_lstItems.GetNext (pos) == lpszText)
		{
			return SelectItem (iIndex);
		}
	}

	return FALSE;
}
//******************************************************************************************
int CBCGToolbarComboBoxButton::OnDrawOnCustomizeList (
	CDC* pDC, const CRect& rect, BOOL bSelected)
{
	int iWidth = CBCGToolbarButton::OnDrawOnCustomizeList (pDC, rect, bSelected);

	//------------------------------
	// Simulate combobox appearance:
	//------------------------------
	CRect rectCombo = rect;
	int iComboWidth = rect.Width () - iWidth;

	if (iComboWidth < 20)
	{
		iComboWidth = 20;
	}

	rectCombo.left = rectCombo.right - iComboWidth;
	rectCombo.DeflateRect (2, 3);

	pDC->FillSolidRect (rectCombo, ::GetSysColor (COLOR_WINDOW));
	pDC->Draw3dRect (&rectCombo,
		globalData.clrBtnDkShadow,
		globalData.clrBtnHilite);

	rectCombo.DeflateRect (1, 1);

	pDC->Draw3dRect (&rectCombo,
		globalData.clrBtnShadow,
		globalData.clrBtnLight);

	CRect rectBtn = rectCombo;
	rectBtn.left = rectBtn.right - rectBtn.Height ();
	rectBtn.DeflateRect (1, 1);

	pDC->FillSolidRect (rectBtn, globalData.clrBtnFace);
	pDC->Draw3dRect (&rectBtn,
		globalData.clrBtnHilite,
		globalData.clrBtnDkShadow);

	CPoint pointTriangle (
		rectBtn.left + (rectBtn.Width () - CMenuImages::Size ().cx) / 2,
		rectBtn.top + (rectBtn.Height () - CMenuImages::Size ().cy) / 2);

	CMenuImages::Draw (pDC, CMenuImages::IdArowDown, pointTriangle);
	return rect.Width ();
}
//********************************************************************************************
CComboBox* CBCGToolbarComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CComboBox* pWndCombo = new CComboBox;
	if (!pWndCombo->Create (m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}

	return pWndCombo;
}
//****************************************************************************************
void CBCGToolbarComboBoxButton::OnShow (BOOL bShow)
{
	if (m_pWndCombo->GetSafeHwnd () != NULL)
	{
		if (bShow)
		{
			m_pWndCombo->ShowWindow (SW_SHOWNOACTIVATE);
			OnMove ();
		}
		else
		{
			m_pWndCombo->ShowWindow (SW_HIDE);
		}
	}
}
//*************************************************************************************
BOOL CBCGToolbarComboBoxButton::ExportToMenuButton (CBCGToolbarMenuButton& menuButton) const
{
	CString strMessage;
	int iOffset;

	if (strMessage.LoadString (m_nID) &&
		(iOffset = strMessage.Find (_T('\n'))) != -1)
	{
		menuButton.m_strText = strMessage.Mid (iOffset + 1);
	}

	return TRUE;
}
//*********************************************************************************
void CBCGToolbarComboBoxButton::SetDropDownHeight (int nHeight)
{
	if (m_nDropDownHeight == nHeight)
	{
		return;
	}

	m_nDropDownHeight = nHeight;
	OnMove ();
}
