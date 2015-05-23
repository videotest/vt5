#if !defined(AFX_DBCOMMONPROPPAGE_H__981588B8_E6A8_4617_B945_5F94D90A2F77__INCLUDED_)
#define AFX_DBCOMMONPROPPAGE_H__981588B8_E6A8_4617_B945_5F94D90A2F77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBCommonPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBCommonPropPage dialog

enum DBControlType{
	ctNumber	= 0,
	ctText		= 1,
	ctDateTime	= 2,
	ctObject	= 3,
	ctNotSupported  = 4
};

class CDBCommonPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDBCommonPropPage)
	DECLARE_OLECREATE_EX(CDBCommonPropPage)


// Construction
public:
	CDBCommonPropPage();	

// Dialog Data
	//{{AFX_DATA(CDBCommonPropPage)
	enum { IDD = IDD_PROPPAGE_COMMON };
	BOOL	m_bEnableEnumeration;
	BOOL	m_bAutoLabel;
	int		m_bLabelLeft;
	CString	m_strField;
	CString	m_strTable;
	BOOL	m_bReadOnly;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDBCommonPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDBCommonPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeTableName();
	afx_msg void OnSelchangeTableName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	DBControlType m_controlType;

	void FillTablesName( );
	void OnTableNameChange( bool bAfterLoad );	
	void FillFieldsName( CString strTableName, bool bAfterLoad );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCOMMONPROPPAGE_H__981588B8_E6A8_4617_B945_5F94D90A2F77__INCLUDED_)
