#if !defined(AFX_GRIDOPTIONS_H__0EB4A1A3_61ED_11D4_AEFB_0000E8DF68C3__INCLUDED_)
#define AFX_GRIDOPTIONS_H__0EB4A1A3_61ED_11D4_AEFB_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridOptions.h : header file
//

#include "GridCtrl.h"

#define GRID_SOURCE_ID	200
#define GRID_TARGET_ID	201

#define WM_ON_SELECT_CHANGE	WM_USER + 100
/////////////////////////////////////////////////////////////////////////////
// CGridOptionsDlg dialog



class CBrowseGrid: public CGridCtrl
{
public:
	CBrowseGrid();

protected:	

};



class CGridOptionsDlg : public CDialog
{
// Construction
public:
	CGridOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	~CGridOptionsDlg();

// Dialog Data
	//{{AFX_DATA(CGridOptionsDlg)
	enum { IDD = IDD_GRID_OPTIONS_DIALOG };
	BOOL	m_bUseFilterRow;
	BOOL	m_bUseSortRow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrowseGrid m_ctrlGridSource, m_ctrlGridTarget;

	void InitSourceGrid();
	void InitTargetGrid();

	void SaveChanges();	

	void SetButtonAvailable();


	sptrIQueryObject m_spActiveQuery;
public:
	void SetActiveQuery( IUnknown* pUnkActiveQuery );
protected:


	// Generated message map functions
	//{{AFX_MSG(CGridOptionsDlg)
	afx_msg void OnAddAll();
	afx_msg void OnRemoveAll();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();	
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDOPTIONS_H__0EB4A1A3_61ED_11D4_AEFB_0000E8DF68C3__INCLUDED_)
