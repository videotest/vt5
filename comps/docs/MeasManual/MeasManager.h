#if !defined(AFX_MEASMANAGER_H__0FA21FE6_D68F_4DD2_9C1D_D8A211527D05__INCLUDED_)
#define AFX_MEASMANAGER_H__0FA21FE6_D68F_4DD2_9C1D_D8A211527D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasManager.h : header file
//

#include "Measure5.h"
#include "classbase.h"


/////////////////////////////////////////////////////////////////////////////
// CMeasManager command target

class CMeasManager : public CCmdTarget
{	
	DECLARE_DYNCREATE(CMeasManager);
	GUARD_DECLARE_OLECREATE(CMeasManager);	
	ENABLE_MULTYINTERFACE();


	CMeasManager();           // protected constructor used by dynamic creation

protected:

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMeasManager();

	// Generated message map functions
	//{{AFX_MSG(CMeasManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()	

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMeasManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASMANAGER_H__0FA21FE6_D68F_4DD2_9C1D_D8A211527D05__INCLUDED_)
