#if !defined(AFX_DBNUMBERPPG_H__B8604CD6_CD9D_4E95_B66B_146A5CC65DFE__INCLUDED_)
#define AFX_DBNUMBERPPG_H__B8604CD6_CD9D_4E95_B66B_146A5CC65DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DBNumberPpg.h : Declaration of the CDBNumberPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CDBNumberPropPage : See DBNumberPpg.cpp.cpp for implementation.

class CDBNumberPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDBNumberPropPage)
	DECLARE_OLECREATE_EX(CDBNumberPropPage)

// Constructor
public:
	CDBNumberPropPage();

// Dialog Data
	//{{AFX_DATA(CDBNumberPropPage)
	enum { IDD = IDD_PROPPAGE_DBNUMBER };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDBNumberPropPage)
	afx_msg void OnSetfocusEdit1();
	afx_msg void OnKillfocusEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBNUMBERPPG_H__B8604CD6_CD9D_4E95_B66B_146A5CC65DFE__INCLUDED)
