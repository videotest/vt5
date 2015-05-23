// ImagePage.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "ImagePage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CImagePage dialog
IMPLEMENT_DYNCREATE(CImagePage, CDialog);

// {07D14A3B-AD16-4837-8552-3809E80E3F91}
GUARD_IMPLEMENT_OLECREATE(CImagePage, szImagePropPageProgID, 
0x7d14a3b, 0xad16, 0x4837, 0x85, 0x52, 0x38, 0x9, 0xe8, 0xe, 0x3f, 0x91);

CImagePage::CImagePage()
	: CPropertyPageBase( CImagePage::IDD )
{
	_OleLockApp( this );
	m_sName = c_szCImagePage;
	m_sUserName.LoadString( IDS_IMAGE_PAGE );
	//{{AFX_DATA_INIT(CImagePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CImagePage::~CImagePage()
{
	_OleUnlockApp( this );
}


void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImagePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImagePage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CImagePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePage message handlers
