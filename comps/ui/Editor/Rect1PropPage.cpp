// Rect1PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "Rect1PropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CRect1PropPage, CDialog)

// {A8E7F562-A272-11d3-A53A-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CRect1PropPage, szEditorRectPropPageProgID, 
0xa8e7f562, 0xa272, 0x11d3, 0xa5, 0x3a, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

/////////////////////////////////////////////////////////////////////////////
// CRect1PropPage dialog


CRect1PropPage::CRect1PropPage(CWnd* pParent /*=NULL*/)
	: CCircle1PropPage(CRect1PropPage::IDD)
{
	//{{AFX_DATA_INIT(CRect1PropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strFigure = "Rect";
	m_sName = c_szCRect1PropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_RECT );
}

BEGIN_MESSAGE_MAP(CRect1PropPage, CCircle1PropPage)
	//{{AFX_MSG_MAP(CRect1PropPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRect1PropPage message handlers

