#if !defined(AFX_FilterConditionDlg_H__799D5208_F710_41F2_939E_8C1641006FA7__INCLUDED_)
#define AFX_FilterConditionDlg_H__799D5208_F710_41F2_939E_8C1641006FA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterConditionDlg.h : header file
//



class _CField
{
public:
	CString m_strTable;
	CString m_strField;
	CString m_strCaption;
};


/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg dialog

class CFilterConditionDlg : public CDialog
{
// Construction
public:
	CFilterConditionDlg(CWnd* pParent = NULL);   // standard constructor
	~CFilterConditionDlg();

// Dialog Data
	//{{AFX_DATA(CFilterConditionDlg)
	enum { IDD = IDD_FILTER_CONDITION };
	int		m_bOrUnion;
	CString	m_strValue;
	CString	m_strValueTo;
	CString	m_strCondition;
	int		m_nFieldIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterConditionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterConditionDlg)
	afx_msg void OnEditchangeField();
	afx_msg void OnSelchangeField();
	afx_msg void OnEditchangeCondition();
	afx_msg void OnSelchangeCondition();
	afx_msg void OnEditchangeValue();
	afx_msg void OnSelchangeValue();
	afx_msg void OnEditchangeValueTo();
	afx_msg void OnSelchangeValueTo();
	afx_msg void OnAndUnion();
	afx_msg void OnOrUnion();
	virtual BOOL OnInitDialog();
	afx_msg void OnDropdownValue();
	afx_msg void OnDropdownValueTo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "FilterCondition" );

public:
	void SetFilterHolder( IUnknown* punk );

protected:
	IDBaseFilterHolderPtr m_ptrFilterHolder;	

	void FillTableField( );

	void FillCombo( CComboBox* pCombo, CString strTable, CString strField );

	void ShowPreview( bool bUpdateData = true );
	void AnalizeValueToEnable( bool bUpdateData = true );

	void _UpdateData();

	CString GetValue( UINT IDC_COMBO );

	CArray<_CField*, _CField*> m_arrfi;

public:
	CString m_strTable;
	CString m_strField;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FilterConditionDlg_H__799D5208_F710_41F2_939E_8C1641006FA7__INCLUDED_)
