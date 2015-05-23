// AuditTrailSingleView.cpp : implementation file
//

#include "stdafx.h"
#include "audittrail.h"
#include "AuditTrailSingleView.h"


////////////////////////////////////////////////////////////////////////////
// CAuditTrailSingleView

IMPLEMENT_DYNCREATE(CAuditTrailSingleView, CCmdTarget)

CAuditTrailSingleView::CAuditTrailSingleView()
{
	_OleLockApp( this );
	EnableAggregation(); 

	m_sName = c_szCAuditTrailSingleView;
	m_sUserName.LoadString(IDS_AT_SINGLE_VIEW_NAME);

	m_bSingle = true;
}

CAuditTrailSingleView::~CAuditTrailSingleView()
{
	_OleUnlockApp( this );
}

// {EB914565-5793-48db-8F6F-611236B6972C}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailSingleView, "AuditTrail.AuditTrailSingleView", 
0xeb914565, 0x5793, 0x48db, 0x8f, 0x6f, 0x61, 0x12, 0x36, 0xb6, 0x97, 0x2c);

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailSingleView 
