#if !defined(AFX_VIEWAXOBJECTPROPPAGE_H__D31804E0_67AA_4505_96CD_FBDA7A40DE28__INCLUDED_)
#define AFX_VIEWAXOBJECTPROPPAGE_H__D31804E0_67AA_4505_96CD_FBDA7A40DE28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewAXObjectPropPage.h : header file
//
#include "GridCtrl.h"

#define IDC_GRID	200

/////////////////////////////////////////////////////////////////////////////
// CViewAXObjectPropPage dialog

class CViewAXObjectPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CViewAXObjectPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXObjectPropPage)
// Construction
public:
	CViewAXObjectPropPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewAXObjectPropPage)
	enum { IDD = IDD_PROPPAGE_OBJECT };
	BOOL	m_bUseActiveView;
	CString	m_strViewName;
	BOOL	m_bAutoAssignedView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewAXObjectPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewAXObjectPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseActiveView();
	afx_msg void OnAutoAssignedView();
	afx_msg void OnAutoAssigned();
	afx_msg void OnViewAutoAssigned();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CGridCtrl m_grid;
	void SetOnOf( BOOL bUpdate = TRUE );

	void ReBuildGridContent( int nFocusedRow, int nFocusedCol );

	sptrIViewCtrl GetViewAXControl();

	long GetPositionByIndex( int nIndex );
	void SetComboBox( int nRow, int nColumn );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWAXOBJECTPROPPAGE_H__D31804E0_67AA_4505_96CD_FBDA7A40DE28__INCLUDED_)
