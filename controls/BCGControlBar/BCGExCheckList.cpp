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

// BCGExCheckList.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "BCGExCheckList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGExCheckList

CBCGExCheckList::CBCGExCheckList()
{
}
//*******************************************************************************************
CBCGExCheckList::~CBCGExCheckList()
{
}

BEGIN_MESSAGE_MAP(CBCGExCheckList, CCheckListBox)
	//{{AFX_MSG_MAP(CBCGExCheckList)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_ADDSTRING, OnLBAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnLBInsertString)
	ON_MESSAGE(LB_RESETCONTENT, OnLBResetContent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGExCheckList message handlers

void CBCGExCheckList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	// determine where the click is
	BOOL bInCheck;
	int nIndex = CheckFromPoint(point, bInCheck);

	if (bInCheck && nIndex != LB_ERR && !IsCheckEnabled (nIndex))
	{
		MessageBeep ((UINT) -1);
		return;
	}
	
	CCheckListBox::OnLButtonDown(nFlags, point);
}
//*******************************************************************************************
void CBCGExCheckList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SPACE)
	{
		int nIndex = GetCaretIndex();
		if (nIndex != LB_ERR && !IsCheckEnabled (nIndex))
		{
			MessageBeep ((UINT) -1);
			return;
		}
	}
	
	CCheckListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
//*******************************************************************************************
LRESULT CBCGExCheckList::OnLBAddString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = CCheckListBox::OnLBAddString(wParam, lParam);
	OnNewString ((int) lRes);
	return lRes;
}
//*******************************************************************************************
LRESULT CBCGExCheckList::OnLBInsertString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = CCheckListBox::OnLBInsertString(wParam, lParam);
	OnNewString ((int) lRes);
	return lRes;
}
//*******************************************************************************************
LRESULT CBCGExCheckList::OnLBResetContent(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_arCheckData.SetSize (0);
	return Default ();
}
//*******************************************************************************************
void CBCGExCheckList::EnableCheck (int nIndex, BOOL bEnable)
{
	ASSERT (nIndex >= 0 && nIndex < m_arCheckData.GetSize ());
	m_arCheckData.SetAt (nIndex, bEnable);
}
//*******************************************************************************************
BOOL CBCGExCheckList::IsCheckEnabled (int nIndex) const
{
	ASSERT (nIndex >= 0 && nIndex < m_arCheckData.GetSize ());
	return m_arCheckData.GetAt (nIndex);
}
//*******************************************************************************************
void CBCGExCheckList::OnNewString (int iIndex)
{
	if (iIndex >= 0)
	{
		int iSize = GetCount ();
		m_arCheckData.SetSize (iSize);

		for (int i = iSize - 1; i > iIndex; i --)
		{
			m_arCheckData.SetAt (i, m_arCheckData.GetAt (i - 1));
		}
		
		m_arCheckData.SetAt (iIndex, TRUE);	// Enabled by default
	}
}
//****************************************************************************************
void CBCGExCheckList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// determine where the click is
	BOOL bInCheck;
	int nIndex = CheckFromPoint(point, bInCheck);

	if (bInCheck && nIndex != LB_ERR && !IsCheckEnabled (nIndex))
	{
		MessageBeep ((UINT) -1);
		return;
	}
	
	CCheckListBox::OnLButtonDblClk(nFlags, point);
	GetParent()->SendMessage(WM_COMMAND,
                            MAKEWPARAM(GetDlgCtrlID(), CLBN_CHKCHANGE),
                            (LPARAM)m_hWnd);
}
