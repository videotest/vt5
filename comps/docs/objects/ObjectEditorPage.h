//{{AFX_INCLUDES()
#include "vtwidth.h"
//}}AFX_INCLUDES
#if !defined(AFX_OBJECTEDITORPAGE_H__B1D47514_7EFB_467E_832A_1298D09C5DD4__INCLUDED_)
#define AFX_OBJECTEDITORPAGE_H__B1D47514_7EFB_467E_832A_1298D09C5DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectEditorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorPage dialog

class CObjectEditorPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CObjectEditorPage)
	GUARD_DECLARE_OLECREATE(CObjectEditorPage);

// Construction
public:
	CObjectEditorPage();
	~CObjectEditorPage();

// Dialog Data
	//{{AFX_DATA(CObjectEditorPage)
	enum { IDD = IDD_OBJECT_EDITOR1 };
	CVTWidth	m_back1;
	CVTWidth	m_back2;
	CVTWidth	m_fore1;
	CVTWidth	m_fore2;
	long	m_nObjectMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CObjectEditorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CObjectEditorPage)
	afx_msg void OnClickBack1();
	afx_msg void OnClickBack2();
	afx_msg void OnClickFore1();
	afx_msg void OnClickFore2();
	afx_msg void OnChangeWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditMinObject();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DoStoreSettings();
	void DoLoadSettings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTEDITORPAGE_H__B1D47514_7EFB_467E_832A_1298D09C5DD4__INCLUDED_)
