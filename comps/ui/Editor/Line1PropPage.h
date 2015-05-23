#if !defined(AFX_LINE1PROPPAGE_H__0A977CA5_C84D_451E_B4D9_40BF6B2B535F__INCLUDED_)
#define AFX_LINE1PROPPAGE_H__0A977CA5_C84D_451E_B4D9_40BF6B2B535F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Line1PropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage dialog
#include "CommonPropPage.h"
#include "vtcolorpicker.h"
class CLine1PropPage : public CEditorPropBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CLine1PropPage);
	GUARD_DECLARE_OLECREATE(CLine1PropPage);

	void InitColors();
// Construction
public:
	CLine1PropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLine1PropPage)
	enum { IDD = IDD_PROPPAGE_LINE1 };
	CVTColorPicker m_clrpickFore;
	CVTColorPicker m_clrpickBack;
	CVTWidth m_EraseWidth1;
	CVTWidth m_EraseWidth2;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLine1PropPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	virtual void DoLoadSettings();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLine1PropPage)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnChangeVtwidthctrl1();
		afx_msg void OnChangeVtwidthctrl2();
		afx_msg void OnChangeVtwidthctrl3();
		afx_msg void OnChangeVtwidthctrl4();
		afx_msg void OnChangeVtwidthctrl5();
		afx_msg void OnOnClickVtwidthctrl1();
		afx_msg void OnOnClickVtwidthctrl2();
		afx_msg void OnOnClickVtwidthctrl3();
		afx_msg void OnOnClickVtwidthctrl4();
		afx_msg void OnOnClickVtwidthctrl5();
		afx_msg void OnOnChangeVtcolorpickerctrl1();
		afx_msg void OnOnChangeVtcolorpickerctrl2();
		afx_msg void OnDestroy();
		DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  bool    m_bBinaryMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINE1PROPPAGE_H__0A977CA5_C84D_451E_B4D9_40BF6B2B535F__INCLUDED_)
