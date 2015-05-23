#if !defined(AFX_VIDEDRIVERSLIST_H__178C23E8_B08F_4E68_A330_8D9C5EACFDE3__INCLUDED_)
#define AFX_VIDEDRIVERSLIST_H__178C23E8_B08F_4E68_A330_8D9C5EACFDE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideDriversList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideDriversList dialog

class CVideDriversList : public CDialog
{
// Construction
	CStringArray m_saShortNames;
	CStringArray m_saLongNames;
public:
	CVideDriversList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVideDriversList)
	enum { IDD = IDD_VIDEO_DRIVERS_LIST };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideDriversList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideDriversList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEDRIVERSLIST_H__178C23E8_B08F_4E68_A330_8D9C5EACFDE3__INCLUDED_)
