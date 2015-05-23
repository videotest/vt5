// EditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "dbcontrols.h"
#include "EditCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

CEditCtrl::CEditCtrl()
{
	m_bCanSendMessage = true;
	m_bSender = false;
}

CEditCtrl::~CEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CEditCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl message handlers



void CEditCtrl::OnChange() 
{
	if( !m_bCanSendMessage )
		return;
	bool bOldState = m_bCanSendMessage;

	m_bCanSendMessage = false;
	CString strText;
	GetWindowText( strText );

	/*
	TCHAR szBuf[256];
	lstrcpy( szBuf, strText );
	*/
	m_bSender = true;

	VERIFY( GetParent()->SendMessage( WM_SET_CONTROL_TEXT, 0, (LPARAM)(LPCSTR)strText ) );

	m_bSender = false;
	
	m_bCanSendMessage = bOldState;
}

void CEditCtrl::OnKillfocus() 
{
	GetParent()->Invalidate();
	
}

void CEditCtrl::OnSetfocus() 
{
	GetParent()->Invalidate();
	
}

BOOL CEditCtrl::PreTranslateMessage(MSG* pMsg) 
{
	return CEdit::PreTranslateMessage(pMsg);
}

LRESULT CEditCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
	if( message == WM_KEYDOWN && wParam == VK_F2 )
	{
		CWnd* pwnd1 = GetParent();
		if( pwnd1 )		
		{
			CWnd* pwnd2 = pwnd1->GetParent();			
			if( pwnd2 && pwnd2->GetSafeHwnd() )
			{
				::PostMessage( pwnd2->GetSafeHwnd(), message, wParam, lParam );
			}
			
		}
		return TRUE;
	}
	else if( message == WM_MOUSEWHEEL )
	{
		GetParent()->PostMessage(message, wParam, lParam);
		return FALSE;
	}
	
	return CEdit::DefWindowProc(message, wParam, lParam);
}

BOOL CEditCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{		
	return CEdit::PreCreateWindow(cs);
}

void CEditCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);		
	if( GetParent() )
		VERIFY( GetParent()->SendMessage( WM_SET_FOCUS_TO_CONTROL, 0, 0 ) );
}

BOOL CEditCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CEdit::OnCommand(wParam, lParam);
}


void CEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// To prevent activation of readonly window
	if( (int)g_DoNotActivateReadonly && (GetWindowLong(m_hWnd, GWL_STYLE) & ES_READONLY) )
		return;

	CEdit::OnLButtonDown(nFlags, point);
}
