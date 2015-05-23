// AuditTrail.h : main header file for the AUDITTRAIL DLL
//

#if !defined(AFX_AUDITTRAIL_H__E2D804CA_4D81_4BF4_9F7D_5AE29759A54B__INCLUDED_)
#define AFX_AUDITTRAIL_H__E2D804CA_4D81_4BF4_9F7D_5AE29759A54B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailApp
// See AuditTrail.cpp for the implementation of this class
//

class CAuditTrailApp : public CWinApp
{
public:
	CAuditTrailApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuditTrailApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAuditTrailApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDITTRAIL_H__E2D804CA_4D81_4BF4_9F7D_5AE29759A54B__INCLUDED_)
