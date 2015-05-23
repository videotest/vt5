#if !defined(AFX_MONSTERWINDOW_H__223D1DC3_70AD_11D3_A65B_0090275995FE__INCLUDED_)
#define AFX_MONSTERWINDOW_H__223D1DC3_70AD_11D3_A65B_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonsterWindow.h : header file
//

#include "TransparentWnd.h"
#include "MessageWindow.h"
/////////////////////////////////////////////////////////////////////////////
// CMonsterWindow window

class CMonsterWindow : public CTransparentWnd,
						public CRootedObjectImpl,
						public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CMonsterWindow)

// Construction
public:
	static	CMonsterWindow	*s_pMonsterWindow;

	CMonsterWindow();

// Attributes
public:
	enum ShowType
	{
		stInvisible = 0,
		stNormal = IDB_MONSTER_NORM,
		stFun = IDB_MONSTER_FUN,
		stPuzz = IDB_MONSTER_PUZZ,
		stBlue = IDB_MONSTER_BLUE,
		stHid = IDB_MONSTER_HID,
	};


// Operations
public:
	void SetMessageWindow( CMessageWindow *pwnd )
	{	m_pMessageWindow = pwnd;	}
	CMessageWindow	*GetMessageWindow() const
	{	return m_pMessageWindow;	}
	void SheduleOperation( DWORD dwMSecs, ShowType ShowType );
	void PreformOperation();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterWindow)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMonsterWindow();

	virtual void OnChangeParent();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	// Generated message map functions
protected:
	//{{AFX_MSG(CMonsterWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CMonsterWindow)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMonsterWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	

	UINT	m_nTimerID;
	UINT	m_nTimerMsgID;

	DWORD	m_dwLastOperationTickCount;
	DWORD	m_dwMonsterActionInterval;
	bool	m_bAutomaticMode;
	bool	m_bCanDestroy;

	CMessageWindow	*m_pMessageWindow;

	struct Shedule
	{
		DWORD	m_dwTickCount;
		ShowType	m_ShowType;	//change view style
	};

	CTypedPtrList<CPtrList, Shedule*> 
		m_listShedule;
};

void GiveFocusToApp();
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTERWINDOW_H__223D1DC3_70AD_11D3_A65B_0090275995FE__INCLUDED_)
