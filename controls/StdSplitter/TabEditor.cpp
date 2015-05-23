// TabEditor.cpp : implementation file
//

#include "stdafx.h"
#include "stdsplitter.h"
#include "TabBeam.h"
#include "TabEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabEditor

CTabEditor::CTabEditor()
{
	pEditTab = NULL;
	pTabBeam = NULL;
}

CTabEditor::~CTabEditor()
{
	pEditTab = NULL;
	pTabBeam = NULL;
}


BEGIN_MESSAGE_MAP(CTabEditor, CEdit)
	//{{AFX_MSG_MAP(CTabEditor)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabEditor message handlers

void CTabEditor::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);
	GetParent()->SetFocus();
	if (pEditTab)
	{
		CString str;
		GetWindowText(str);
		pTabBeam->RenameTab(pEditTab->nIndex, str);
		//pTabBeam->Invalidate();
	}
	// TODO: Add your message handler code here
	
}



void CTabEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 13)
		GetParent()->SetFocus();
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


