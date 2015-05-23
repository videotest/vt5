#if !defined(AFX_SETTINGSWINDOW_H__87F6BED4_11A3_11D3_A5BE_0000B493FF1B__INCLUDED_)
#define AFX_SETTINGSWINDOW_H__87F6BED4_11A3_11D3_A5BE_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsWindow dialog

class CSettingsWindow : public CDialog
{
	
public:
	CSettingsWindow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsWindow)
	enum { IDD = IDD_ACTION_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsWindow)
	public:
	virtual void OnFinalRelease();
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsWindow)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnBtnClick(UINT nID);

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSettingsWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	sptrIDockableWindow	m_sptrDock;
	sptrIWindow			m_sptrWindow;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSWINDOW_H__87F6BED4_11A3_11D3_A5BE_0000B493FF1B__INCLUDED_)
