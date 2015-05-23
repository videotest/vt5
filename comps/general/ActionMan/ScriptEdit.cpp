// ScriptEdit.cpp : implementation file
//

#include "stdafx.h"
#include "actionman.h"
#include "ScriptEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit

CScriptEdit::CScriptEdit()
{
}

CScriptEdit::~CScriptEdit()
{
}


BEGIN_MESSAGE_MAP(CScriptEdit, CEdit)
	//{{AFX_MSG_MAP(CScriptEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit message handlers

void CScriptEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	switch(nChar)
	{
	case 10:
		{	//Ctrl-Enter - OK (run script)
			CWnd *pOwner = GetOwner();
			if (pOwner && IsWindow(pOwner->m_hWnd))
				pOwner->PostMessage(WM_COMMAND, MAKELONG(IDOK, BN_CLICKED),
				(LPARAM) 0);
			nChar=13;
			return; //do not call inherited proc
		}
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CScriptEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar==VK_DOWN)
	{
		int ctrl_state=::GetAsyncKeyState(VK_CONTROL);
		if(ctrl_state&0x8000)
		{
			CWnd *pOwner = GetOwner();
			if (pOwner && IsWindow(pOwner->m_hWnd))
				pOwner->PostMessage(WM_COMMAND, MAKELONG(IDC_PREV, BN_CLICKED),
				(LPARAM) 0);
		}
	}
	
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
