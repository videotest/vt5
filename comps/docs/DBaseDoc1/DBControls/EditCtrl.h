#if !defined(AFX_EDITCTRL_H__EAC1F8D5_E9DD_4459_AB0A_3927620F0873__INCLUDED_)
#define AFX_EDITCTRL_H__EAC1F8D5_E9DD_4459_AB0A_3927620F0873__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl window

#define WM_SET_CONTROL_TEXT	WM_USER + 1001
#define WM_SET_FOCUS_TO_CONTROL	WM_USER + 1002

class CEditCtrl : public CEdit
{
// Construction
public:
	CEditCtrl();
	void CanSendMessage( bool bCanSendMessage){ m_bCanSendMessage = bCanSendMessage; }
	bool IsSender(){ return m_bSender; }

// Attributes
public:

// Operations
public:

protected:
	bool m_bCanSendMessage;
	bool m_bSender;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCtrl)
	afx_msg void OnChange();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCTRL_H__EAC1F8D5_E9DD_4459_AB0A_3927620F0873__INCLUDED_)
