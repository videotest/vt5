#if !defined(AFX_CREATEPARAMDLG_H__BC768C83_D6E1_42BD_B023_91AD298419DA__INCLUDED_)
#define AFX_CREATEPARAMDLG_H__BC768C83_D6E1_42BD_B023_91AD298419DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateParamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateParamDlg dialog
#include "ClassBase.h"
#include "Resource.h"

#include <AfxTempl.h>

struct TParam
{
	DWORD	dwType;
	CString strName;
	int		nCount;

};

typedef CArray <TParam, TParam> CParamArray;

class CCreateParamDlg : public CDialog
{
// Construction
public:
	CCreateParamDlg(CWnd* pParent = NULL);   // standard constructor
	~CCreateParamDlg();

// Dialog Data
	//{{AFX_DATA(CCreateParamDlg)
	enum { IDD = IDD_DIALOG_PARAM };
	CComboBox	m_Combo;
	int		m_nNumber;
	CString	m_strName;
	//}}AFX_DATA

	DWORD m_dwType;

	CParamArray & GetParamArray()
	{	return m_arrParam;	}

	CParamArray				m_arrParam;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool GetManualParam(DWORD dwType, CString & strName);
	void UpdateName();
	void EnableApply(bool bEnable = true);

	// Generated message map functions
	//{{AFX_MSG(CCreateParamDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioAngle();
	afx_msg void OnRadioCount();
	afx_msg void OnRadioLinear();
	afx_msg void OnRadioUndefined();
	afx_msg void OnApply();
	afx_msg void OnChangeEditNumber();
	afx_msg void OnChangeEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEPARAMDLG_H__BC768C83_D6E1_42BD_B023_91AD298419DA__INCLUDED_)
