//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_DODGE1PROPPAGE_H__F48938F0_3B3B_43A7_A34C_663D635C6E89__INCLUDED_)
#define AFX_DODGE1PROPPAGE_H__F48938F0_3B3B_43A7_A34C_663D635C6E89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dodge1PropPage.h : header file
//
#include "CommonPropPage.h"
/////////////////////////////////////////////////////////////////////////////
// CDodge1PropPage dialog

class CDodge1PropPage : public CEditorPropBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CDodge1PropPage);
	GUARD_DECLARE_OLECREATE(CDodge1PropPage);
// Construction
public:
	CDodge1PropPage(CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CDodge1PropPage)
	CSliderCtrl	m_sliderExposure;
	enum { IDD = IDD_PROPPAGE_DODGE1 };
	UINT	m_nExposure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDodge1PropPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDodge1PropPage)
	afx_msg void OnChangeVtwidthctrl1();
	afx_msg void OnChangeVtwidthctrl2();
	afx_msg void OnChangeVtwidthctrl3();
	afx_msg void OnOnClickVtwidthctrl1();
	afx_msg void OnOnClickVtwidthctrl2();
	afx_msg void OnOnClickVtwidthctrl3();
	afx_msg void OnDestroy();
	afx_msg void OnChangeEditExposure();
	afx_msg void OnTypeChange();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
	afx_msg void OnKillFocus();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void _SetEdit(int nVal);

	void Validation();

	int old_nExposure;

protected:
	HWND  hTip;
	//---------  For tolltip -------------
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	//------------------------------------
	BOOL bEditTip;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DODGE1PROPPAGE_H__F48938F0_3B3B_43A7_A34C_663D635C6E89__INCLUDED_)
