#if !defined(AFX_ACTIONSTATE_H__5E292EB6_81E4_4F27_B5A8_DD6B3AA769C3__INCLUDED_)
#define AFX_ACTIONSTATE_H__5E292EB6_81E4_4F27_B5A8_DD6B3AA769C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionState.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CActionState command target

class CActionState : public CCmdTargetEx, 
				public CNamedObjectImpl,
				public CRootedObjectImpl,
				public COleEventSourceImpl
{
	ENABLE_MULTYINTERFACE();

	DECLARE_EVENT_MAP()
	DECLARE_OLETYPELIB(CActionState)


	DECLARE_DYNCREATE(CActionState)
	GUARD_DECLARE_OLECREATE(CActionState)

	ROUTE_OLE_VIRTUALS()

	CActionState();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionState)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CActionState();
	virtual void OnChangeParent();

	// Generated message map functions
	//{{AFX_MSG(CActionState)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CActionState)
	afx_msg void TerminateCurrentAction();
	afx_msg BSTR GetCurrentAction();
	afx_msg long GetActionState(LPCTSTR szAction);
	afx_msg void SetActionState(LPCTSTR szAction, long dwNewState);
	afx_msg void ResetActionState(LPCTSTR szAction);
	afx_msg void ResetAllActionStates();
	afx_msg BOOL IsActionAvaible(LPCTSTR szAction);
	afx_msg long RunInteractiveWait(LPCTSTR szAction);
	afx_msg long BeginGroupUndo(LPCTSTR szGroupName);
	afx_msg long BeginGroupUndo2(LPCTSTR szGroupName, long dwStepsBack);
	afx_msg void EndGroupUndo(long lCancel);
	afx_msg void ClearUndoList();
	afx_msg long RunInteractive(LPCTSTR szAction);
	afx_msg void ClearRedoList();
	afx_msg BOOL FireNotify( LPCTSTR lpctNotifyDesc, long dwFlagLevel, VARIANT &varValue );
	afx_msg BOOL GetEnableUndo();
	afx_msg void SetEnableUndo(BOOL bNewValue);
	afx_msg BOOL TerminateLongOperation();
	afx_msg long RunInteractiveEx(LPCTSTR szAction, LPCTSTR szActionParam);
	afx_msg long RunInteractiveWaitEx(LPCTSTR szAction, LPCTSTR szActionParam);
	afx_msg void DeleteLastUndo();
	afx_msg void EndGroupUndo2(LPCTSTR szGroupName, long lCancel);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	enum {
	//{{AFX_DISP_ID(CActionState)
	dispidTerminateCurrentAction = 1L,
	dispidGetCurrentAction = 2L,
	dispidGetActionState = 3L,
	dispidSetActionState = 4L,
	dispidResetActionState = 5L,
	dispidResetAllActionStates = 6L,
	dispidIsActionAvaible = 7L,
	dispidRunInteractiveWait = 8L,
	dispidClearUndoList = 11L,
	dispidClearRedoList = 13L,
	dispidFireNotify = 14L,
	dispidUndoEnable = 15L,
	dispidTerminateLongOperation = 16L,
	eventidOnActionComplete=1L,
	eventidOnActionRunning=2L,
	dispidRunInteractiveEx = 17L,
	dispidRunInteractiveWaitEx = 18L,
	dispidDeleteLastUndo = 20L
	//}}AFX_DISP_ID
	};
public:
	//{{AFX_EVENT(CActionState)
	void FireOnActionComplete(LPCTSTR szActionName, long nResult)
		{FireEvent(eventidOnActionComplete,EVENT_PARAM(VTS_BSTR  VTS_I4), szActionName, nResult);}
	void FireOnActionRunning(LPCTSTR szActionName)
		{FireEvent(eventidOnActionRunning,EVENT_PARAM(VTS_BSTR), szActionName);}

	//}}AFX_EVENT


	IActionManagerPtr	m_ptrActionManager;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONSTATE_H__5E292EB6_81E4_4F27_B5A8_DD6B3AA769C3__INCLUDED_)
