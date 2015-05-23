// GeneralObjectPage.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "GeneralObjectPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralObjectPage dialog


CGeneralObjectPage::CGeneralObjectPage(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralObjectPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralObjectPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGeneralObjectPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralObjectPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralObjectPage, CDialog)
	//{{AFX_MSG_MAP(CGeneralObjectPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralObjectPage message handlers
