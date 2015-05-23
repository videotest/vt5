// SelPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelPropertySheet

IMPLEMENT_DYNAMIC(CSelPropertySheet, CPropertySheet)

CSelPropertySheet::CSelPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_Page1);
	AddPage(&m_Page2);

	SetWizardMode();
}

CSelPropertySheet::~CSelPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CSelPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSelPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSelPropertySheet::SetActivePage(int nPage)
{
	if (nPage == 1)
		m_sCreator.LoadString(IDS_INPUT);
	CPropertySheet::SetActivePage(nPage);
}


/////////////////////////////////////////////////////////////////////////////
// CSelPropertySheet message handlers


