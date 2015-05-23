#if !defined(AFX_CLASSIFIERDLG_H__C83B028B_94C0_444B_BB36_6F029DD454DB__INCLUDED_)
#define AFX_CLASSIFIERDLG_H__C83B028B_94C0_444B_BB36_6F029DD454DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassifierDlg.h : header file
//

#define IDC_GRID 555

#include "GridCtrl.h"
#include "GridCellCombo.h"


/////////////////////////////////////////////////////////////////////////////
// CClassifierDlg dialog

class CClassifierDlg : public CDialog
{
// Construction
public:
	CClassifierDlg(CWnd* pParent = NULL, CString strSectionName = "Objects\\ObjectLimitsClassifier");   // standard constructor
	~CClassifierDlg();

// Dialog Data
	//{{AFX_DATA(CClassifierDlg)
	enum { IDD = IDD_LIMITS_CLASSIFIER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassifierDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:


	void _GetAllParams();
	void _UpdateRows(long nColLo);



	// Generated message map functions
	//{{AFX_MSG(CClassifierDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokComboClasses();
	afx_msg void OnButtonAddParam();
	afx_msg void OnButtonRemoveParam();
	afx_msg void OnButtonRemoveClass();
	afx_msg void OnButtonAddClass();
	afx_msg void OnButtonRenameClass();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CArray<IUnknown*, IUnknown*&>      m_arrClassLists;
	CMap<long, long, CString, LPCTSTR> m_mapParams1;
	CMap<CString, LPCTSTR, long, long> m_mapParams2;

	CMap<CString, LPCTSTR, long, long> m_mapPresentParams;			

	CMap<long, long, IUnknown*, IUnknown*> m_mapClassesByRow;

	CGridCtrl			m_grid;
	CString				m_strSectionName;
	GUID				m_keyPrevClassList;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSIFIERDLG_H__C83B028B_94C0_444B_BB36_6F029DD454DB__INCLUDED_)
