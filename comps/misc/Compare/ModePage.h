//{{AFX_INCLUDES()
#include "vtslider.h"
//}}AFX_INCLUDES
#if !defined(AFX_MODEPAGE_H__83998492_0A26_447A_9BC5_170D8CDD706E__INCLUDED_)
#define AFX_MODEPAGE_H__83998492_0A26_447A_9BC5_170D8CDD706E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModePage dialog

class CModePage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CModePage)
	GUARD_DECLARE_OLECREATE(CModePage)

// Construction
public:
	CModePage();
	~CModePage();

// Dialog Data
	//{{AFX_DATA(CModePage)
	enum { IDD = IDD_PAGE_MODE };
	CVTSlider	m_slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CModePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CModePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAdd();
	afx_msg void OnClickSub();
	afx_msg void OnPosChangedTransparency();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void DoStoreSettings();
	void DoLoadSettings();
	void _UpdateControls();

protected:
	bool	m_bAddMode;
	int		m_nTransparency;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEPAGE_H__83998492_0A26_447A_9BC5_170D8CDD706E__INCLUDED_)
