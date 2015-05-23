#if !defined(AFX_DBOBJECTPPG_H__1F312ED6_2388_449E_8F6B_BDD7437F7F69__INCLUDED_)
#define AFX_DBOBJECTPPG_H__1F312ED6_2388_449E_8F6B_BDD7437F7F69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBObjectPpg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage window

class CDBObjectPropertyPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDBObjectPropertyPage)
	DECLARE_OLECREATE_EX(CDBObjectPropertyPage)

// Constructor
public:
	CDBObjectPropertyPage();

// Dialog Data
	//{{AFX_DATA(CDBObjectPropertyPage)
	enum { IDD = IDD_PROPPAGE_DBOBJECT };
	CString	m_strViewType;
	BOOL	m_bAutoViewType;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDBObjectPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoAsign();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AnalizeAppearance();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBOBJECTPPG_H__1F312ED6_2388_449E_8F6B_BDD7437F7F69__INCLUDED_)
