// MeasManager.cpp : implementation file
//

#include "stdafx.h"
#include "measmanual.h"
#include "MeasManager.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasManager

IMPLEMENT_DYNCREATE(CMeasManager, CCmdTarget)

CMeasManager::CMeasManager()
{
	EnableAutomation();
	EnableAggregation();
	AfxOleLockApp();
}

CMeasManager::~CMeasManager()
{
	AfxOleUnlockApp();
}


void CMeasManager::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMeasManager, CCmdTarget)
	//{{AFX_MSG_MAP(CMeasManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMeasManager, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CMeasManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMeasManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {8DBC9EE3-A4EA-4FA8-BA47-08EE47932CA8}
static const IID IID_IMeasManager =
{ 0x8dbc9ee3, 0xa4ea, 0x4fa8, { 0xba, 0x47, 0x8, 0xee, 0x47, 0x93, 0x2c, 0xa8 } };

BEGIN_INTERFACE_MAP(CMeasManager, CCmdTarget)
	INTERFACE_PART(CMeasManager, IID_IMeasManager, Dispatch)
END_INTERFACE_MAP()


// {986021C1-0B2A-4C8C-843E-7FF6860B4BD3}
GUARD_IMPLEMENT_OLECREATE(CMeasManager, "MeasManual.MeasManager", 0x986021c1, 0xb2a, 0x4c8c, 0x84, 0x3e, 0x7f, 0xf6, 0x86, 0xb, 0x4b, 0xd3)

/////////////////////////////////////////////////////////////////////////////
