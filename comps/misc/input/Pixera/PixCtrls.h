#if !defined(AFX_PIXCTRLS_H__CA822721_95A4_47F8_826C_68175A5052C9__INCLUDED_)
#define AFX_PIXCTRLS_H__CA822721_95A4_47F8_826C_68175A5052C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PixCtrls.h : header file
//

CWnd *CreateControlForDialog(HWND hwndDlg, int nId, HWND hwndCtrl);


/////////////////////////////////////////////////////////////////////////////
// CTimeAdjustEdit window

class CTimeAdjustEdit : public CEdit
{
// Construction
	DECLARE_DYNCREATE(CTimeAdjustEdit);
public:
	CTimeAdjustEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeAdjustEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeAdjustEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeAdjustEdit)
	afx_msg void OnChange();
	//}}AFX_MSG
	afx_msg LRESULT OnInit(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CAutoExposeButton window

class CAutoExposeButton : public CButton
{
// Construction
public:
	CAutoExposeButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoExposeButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAutoExposeButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoExposeButton)
	afx_msg void OnClicked();
	//}}AFX_MSG
	afx_msg LRESULT OnInit(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXCTRLS_H__CA822721_95A4_47F8_826C_68175A5052C9__INCLUDED_)
