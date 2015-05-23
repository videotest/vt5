// PropertySheetMan.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertySheet.h"
#include "PropertySheetMan.h"
#include "ScriptNotifyInt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetMan

IMPLEMENT_DYNCREATE(CPropertySheetMan, CPropertySheet)

CPropertySheetMan::CPropertySheetMan() : CPropertySheetBase()
{
	EnableAutomation();

	m_sName = "PropertySheet";
	_OleLockApp( this );
}

CPropertySheetMan::~CPropertySheetMan()
{
	_OleUnlockApp( this );
}


void CPropertySheetMan::OnFinalRelease()
{
	CPropertySheetBase::OnFinalRelease();

	delete this;
}


BEGIN_MESSAGE_MAP(CPropertySheetMan, CPropertySheetBase)
	//{{AFX_MSG_MAP(CPropertySheetMan)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CPropertySheetMan, CPropertySheetBase)
	INTERFACE_PART(CPropertySheetMan, IID_IDockableWindow, Dock)
END_INTERFACE_MAP()

// {FD74A2D8-7FAC-11D3-A50C-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CPropertySheetMan, "PropertySheet.PropertySheetMan", 0xfd74a2d8, 0x7fac, 0x11d3, 0xa5, 0xc, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

bool CPropertySheetMan::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	if (!Create(pparent, WS_CHILD|WS_VISIBLE|DS_CONTROL, 0 ))
		return false;

	DWORD dwStyleTab = GetTabControl()->GetStyle();
	dwStyleTab ^= TCS_MULTILINE;
	SetWindowLong( *GetTabControl(), GWL_STYLE, dwStyleTab );



	GetWindowRect(&rc);
	m_size = CSize(rc.Width(), rc.Height());
	m_bFixed = true;

//remove all pages

	while( GetPageCount() )
		RemovePage(0);

	LOGFONT logFont;
	GetTabControl()->GetFont()->GetLogFont(&logFont);
	GetTabControl()->SetItemSize( CSize(10,-logFont.lfHeight+4) );
	
	return true;
}

void CPropertySheetMan::OnHide()
{
	// AM BT4884
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS == 0) return;
	sptrSS->Invoke(_bstr_t("PropertySheetMan_OnHide"), NULL, 0, 0, fwFormScript);
	sptrSS->Invoke(_bstr_t("PropertySheetMan_OnHide"), NULL, 0, 0, fwAppScript);
}
