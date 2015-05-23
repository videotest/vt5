#if !defined(AFX_VISUALEDIT_H__3BA0CF71_FB14_4C75_90C8_D45B141F73A6__INCLUDED_)
#define AFX_VISUALEDIT_H__3BA0CF71_FB14_4C75_90C8_D45B141F73A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VisualEdit.h : header file
//
#include "dbase.h"
#include "afxwin.h"
#include "atlcomcli.h"
/////////////////////////////////////////////////////////////////////////////
// CAddField dialog

class CAddField : public CDialog
{
// Construction
public:
	CAddField(CWnd* pParent = NULL);   // standard constructor
	CAddField(IDBaseStruct* pDBStruct, BSTR bstrTable, CWnd* pParent=0);

// Dialog Data
	//{{AFX_DATA(CAddField)
	enum { IDD = IDD_NEW_FIELD };
	CString	m_strDefValue;
	CString	m_strFieldName;
	CString	m_strFieldType;
	BOOL	m_bDefValue;
	BOOL	m_bReqValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddField)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddField)
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "AddField" );

	FieldType	m_ft;
public:
	void SetFieldType( FieldType ft ){ m_ft = ft;}

	afx_msg void OnBnClickedNeedDef();
	CComboBox cbFieldName;
private:
	IDBaseStructPtr m_pDBStruct;
	CComBSTR m_bstrTable;
};

/////////////////////////////////////////////////////////////////////////////
// CDeleteField dialog

class CDeleteField : public CDialog
{
// Construction
public:
	CDeleteField(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteField)
	enum { IDD = IDD_DELETE_FIELD };
	CString	m_strField;
	CString	m_strTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteField)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteField)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "DeleteField" );

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALEDIT_H__3BA0CF71_FB14_4C75_90C8_D45B141F73A6__INCLUDED_)
