#if !defined(AFX_OBJECTFILTERDLG_H__37C61A07_5FEA_463D_A192_8624940FC8BF__INCLUDED_)
#define AFX_OBJECTFILTERDLG_H__37C61A07_5FEA_463D_A192_8624940FC8BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectFilterDlg.h : header file
//

#define IDC_GRID 555

#include "GridCtrl.h"
#include "GridCellCombo.h"


/////////////////////////////////////////////////////////////////////////////
// CObjectFilterDlg dialog

class CObjectFilterDlg : public CDialog
{
// Construction
public:
	CObjectFilterDlg(CWnd* pParent = NULL, CString strSection = "\\Objects\\ObjectFilter");   // standard constructor
	~CObjectFilterDlg();

// Dialog Data
	//{{AFX_DATA(CObjectFilterDlg)
	enum { IDD = IDD_FILTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	void _LoadParamsToGrid();
	void SetComboBox(int nRow);

	// Generated message map functions
	//{{AFX_MSG(CObjectFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CGridCtrl		m_grid;
	CString			m_strNameDataSection;
	CMap<long, long, CString, LPCTSTR> m_mapParams1;
	CMap<CString, LPCTSTR, long, long> m_mapParams2;
	CMap<long, long, double, double> m_mapParams1Unit;
	CString			m_strFirstParam;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTFILTERDLG_H__37C61A07_5FEA_463D_A192_8624940FC8BF__INCLUDED_)
