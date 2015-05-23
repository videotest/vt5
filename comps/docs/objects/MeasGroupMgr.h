#if !defined(AFX_MEASGROUPMGR_H__32D64A9A_0FFE_4021_92AE_A7C3BAC924AA__INCLUDED_)
#define AFX_MEASGROUPMGR_H__32D64A9A_0FFE_4021_92AE_A7C3BAC924AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasGroupMgr.h : header file
//


#include "ClassBase.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasurementManager command target

class CMeasurementManager : 
	public CCmdTargetEx,
	public CRootedObjectImpl,
	public CCompManagerImpl,
	public CNamedObjectImpl
{
	DECLARE_DYNCREATE(CMeasurementManager)
	GUARD_DECLARE_OLECREATE(CMeasurementManager)
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE();

public:
	CMeasurementManager();           // protected constructor used by dynamic creation
	virtual ~CMeasurementManager();

	virtual void DeInit(); // default deinitialization
	virtual bool Init();// default initialization

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasurementManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual LPCTSTR GetGroupLoadParamString();

	bool m_bInConstructor;
	// Generated message map functions
	//{{AFX_MSG(CMeasurementManager)
	//}}AFX_MSG

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMeasurementManager)
	afx_msg void LoadState();
	afx_msg void SaveState();
	//}}AFX_DISPATCH
	DECLARE_MESSAGE_MAP()

	BEGIN_INTERFACE_PART(MeasManager, IMeasureManager)
	END_INTERFACE_PART(MeasManager)

private:
	long	get_groups_priority( IUnknown *punk_group );
	void	sort_groups( );
    
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASGROUPMGR_H__32D64A9A_0FFE_4021_92AE_A7C3BAC924AA__INCLUDED_)
