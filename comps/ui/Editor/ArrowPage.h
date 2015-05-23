//{{AFX_INCLUDES()
#include "vtcolorpicker.h"
//}}AFX_INCLUDES
#if !defined(AFX_ARROWPAGE_H__FA99ECF4_83ED_4A5A_97C3_95DE60062264__INCLUDED_)
#define AFX_ARROWPAGE_H__FA99ECF4_83ED_4A5A_97C3_95DE60062264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArrowPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArrowPage dialog

class CArrowPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CArrowPage)
	GUARD_DECLARE_OLECREATE(CArrowPage)

// Construction
public:
	CArrowPage();
	~CArrowPage();

// Dialog Data
	//{{AFX_DATA(CArrowPage)
	enum { IDD = IDD_PROPPAGE_ARROW1 };
	int		m_nSize;
	int		m_nWidth;
	CVTColorPicker	m_clr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CArrowPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArrowPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnDestroy();
	afx_msg void OnOnChangeColor();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void _UpdateSettings( bool bRead );
	void DoLoadSettings();
	void _SetColor(COLORREF color);

	bool	m_bInit;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWPAGE_H__FA99ECF4_83ED_4A5A_97C3_95DE60062264__INCLUDED_)
