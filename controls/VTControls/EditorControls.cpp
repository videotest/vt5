// EditorControls.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols.h"
#include "EditorControls.h"

#include "VTEditorl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

CEditCtrl::CEditCtrl()
{
	m_pParent = NULL;
}

CEditCtrl::~CEditCtrl()
{
}


BOOL CEditCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.style		|= (ES_AUTOHSCROLL | ES_AUTOVSCROLL);	

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return CEdit::PreCreateWindow(cs);
}

void CEditCtrl::OnKillfocus() 
{
	if( m_pParent )
		m_pParent->OnEditCtrlKillFocus( );	
}


void CEditCtrl::OnChange() 
{
	if( m_pParent )
		m_pParent->OnFireDataChange();	
}




BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CEditCtrl)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CStaticCtrl

CStaticCtrl::CStaticCtrl()
{
}

CStaticCtrl::~CStaticCtrl()
{
}

BOOL CStaticCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{	
	return CStatic::PreCreateWindow(cs);
}


BEGIN_MESSAGE_MAP(CStaticCtrl, CStatic)
	//{{AFX_MSG_MAP(CStaticCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CSpinCtrl

CSpinCtrl::CSpinCtrl()
{
}

CSpinCtrl::~CSpinCtrl()
{
}


BEGIN_MESSAGE_MAP(CSpinCtrl, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CSpinCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinCtrl message handlers




