// PropertySheetCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "PropertySheetCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl

IMPLEMENT_DYNAMIC(CPropertySheetCtrl, CPropertySheet)

CPropertySheetCtrl::CPropertySheetCtrl(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropertySheetCtrl::CPropertySheetCtrl(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPropertySheetCtrl::~CPropertySheetCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropertySheetCtrl, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl message handlers
