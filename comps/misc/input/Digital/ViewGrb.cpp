// ViewGrb.cpp : implementation file
//

#include "stdafx.h"
//#include "fgdrv.h"
#include "ViewGrb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewGrb

CViewGrb::CViewGrb()
{
}

CViewGrb::~CViewGrb()
{
}


BEGIN_MESSAGE_MAP(CViewGrb, CWnd)
	//{{AFX_MSG_MAP(CViewGrb)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CViewGrb message handlers

BOOL CViewGrb::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW), 
						"", dwStyle | WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID, NULL);
}

BOOL CViewGrb::Attach(DWORD dwStyle, CWnd* pParentWnd, UINT nID)
{
	//Gotta have a parent and he/she must be valid
	ASSERT_VALID(pParentWnd);
	
	//Get the control to replace
	CWnd* pCtrlToReplace = pParentWnd->GetDlgItem(nID);
	ASSERT_VALID(pCtrlToReplace);

	CRect rcOld;
	pCtrlToReplace->GetWindowRect(&rcOld);
	pParentWnd->ScreenToClient(&rcOld);

	BOOL bRet = this->Create(dwStyle, rcOld, pParentWnd, nID);
	ASSERT(bRet);

	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	this->SetWindowPos(pCtrlToReplace, rcOld.left, rcOld.top, rcOld.Width(), rcOld.Height(), 
		               SWP_SHOWWINDOW|SWP_NOREDRAW|SWP_NOACTIVATE);
	
	//Destroy the old control...we don't need it anymore
	pCtrlToReplace->DestroyWindow();

	UpdateWindow();

	return bRet;
}

