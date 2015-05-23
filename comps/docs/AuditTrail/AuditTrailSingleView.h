#if !defined(AFX_AUDITTRAILSINGLEVIEW_H__FB3DA3D2_CCED_491F_9D7D_1FDB517867C8__INCLUDED_)
#define AFX_AUDITTRAILSINGLEVIEW_H__FB3DA3D2_CCED_491F_9D7D_1FDB517867C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuditTrailSingleView.h : header file
//
#define szAuditTrailSingleViewProgID "AuditTrail.AuditTrailSingleView"

#include "AuditTrailView.h"
/////////////////////////////////////////////////////////////////////////////
// CAuditTrailSingleView window

class CAuditTrailSingleView : public CAuditTrailView
{
	DECLARE_DYNCREATE(CAuditTrailSingleView)
	GUARD_DECLARE_OLECREATE(CAuditTrailSingleView)
// Construction
public:
	CAuditTrailSingleView();
	virtual ~CAuditTrailSingleView();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDITTRAILSINGLEVIEW_H__FB3DA3D2_CCED_491F_9D7D_1FDB517867C8__INCLUDED_)
