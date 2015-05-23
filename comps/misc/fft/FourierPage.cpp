// FourierPage.cpp   : implementation file
//

#include "stdafx.h"
#include "fft.h"
#include "FourierPage.h"

/////////////////////////////////////////////////////////////////////////////
// CFourierPage property page

IMPLEMENT_DYNCREATE(CFourierPage, CWnd)

// {26C9C23B-AE06-4e6f-99B9-3D42DBD18F7F}
GUARD_IMPLEMENT_OLECREATE(CFourierPage, "fft.FourierPage", 
0x26c9c23b, 0xae06, 0x4e6f, 0x99, 0xb9, 0x3d, 0x42, 0xdb, 0xd1, 0x8f, 0x7f);

CFourierPage::CFourierPage() : CPropertyPageBase(CFourierPage::IDD)
{
	//{{AFX_DATA_INIT(CFourierPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCFourierPage;
	m_sUserName.LoadString( IDS_PAGE_TITLE );
}

CFourierPage::~CFourierPage()
{
}

void CFourierPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFourierPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFourierPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CFourierPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFourierPage message handlers
