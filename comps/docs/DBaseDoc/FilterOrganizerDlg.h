#if !defined(AFX_FILTERORGANIZERDLG_H__720CF217_5F4B_4990_8275_D8F66952FA35__INCLUDED_)
#define AFX_FILTERORGANIZERDLG_H__720CF217_5F4B_4990_8275_D8F66952FA35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterOrganizerDlg.h : header file
//

#include "FilterPropPage.h"
#include "FilterBase.h"

#include "HelpDlgImpl.h"
/////////////////////////////////////////////////////////////////////////////
// CFilterOrganizerDlg dialog

class CFilterOrganizerDlg : public CDialog,
					public CHelpDlgImpl
{
// Construction
public:
	CFilterOrganizerDlg(CWnd* pParent = NULL);   // standard constructor
	~CFilterOrganizerDlg();

// Dialog Data
	//{{AFX_DATA(CFilterOrganizerDlg)
	enum { IDD = IDD_FILTER_ORGANIZER };
	CFilterListBox	m_ctrlConditionList;
	CListBox	m_ctrlFilterList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterOrganizerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterOrganizerDlg)
	afx_msg void OnSelchangeFilterList();
	afx_msg void OnSelchangeConditionList();
	virtual BOOL OnInitDialog();
	afx_msg void OnNewFilter();
	afx_msg void OnRenameFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnNewCond();
	afx_msg void OnEditCond();
	afx_msg void OnCopyCond();
	afx_msg void OnRemoveCond();
	afx_msg void OnLeftBracket();
	afx_msg void OnRightBracket();
	afx_msg void OnResetBrackets();
	afx_msg void OnSelectAndExit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "FilterOrganizer" );

public:
	void SetFilterHolder( IUnknown* punkQuery );
	CFilterHolder m_filterHolder;

protected:
	IDBaseFilterHolderPtr m_ptrFilterHolder;	

	void BuildFilterArray();
	void BuildAppearance();

	CString GetActiveFilterName();
	int GetActiveConditionIndex();

	void GetActiveCondition();

	void SetVisibleButton();	

	int nLastActiveFilterIndex;
	int nLastActiveConditionIndex;

	bool SaveQueryObject();

protected:
	void SetLastActiveFilterIndex();
	void SetLastActiveConditionIndex();

	//CString m_strOldActiveFilter;

public:
	bool	m_bSelectFilter;
	CString m_strActiveFilter;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERORGANIZERDLG_H__720CF217_5F4B_4990_8275_D8F66952FA35__INCLUDED_)
