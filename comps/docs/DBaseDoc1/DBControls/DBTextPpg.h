#if !defined(AFX_DBTEXTPPG_H__DA3E4771_3204_4685_A1BD_888997010B60__INCLUDED_)
#define AFX_DBTEXTPPG_H__DA3E4771_3204_4685_A1BD_888997010B60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DBTextPpg.h : Declaration of the CDBTextPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CDBTextPropPage : See DBTextPpg.cpp.cpp for implementation.

class CDBTextPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDBTextPropPage)
	DECLARE_OLECREATE_EX(CDBTextPropPage)

// Constructor
public:
	CDBTextPropPage();

// Dialog Data
	//{{AFX_DATA(CDBTextPropPage)
	enum { IDD = IDD_PROPPAGE_DBTEXT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDBTextPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBTEXTPPG_H__DA3E4771_3204_4685_A1BD_888997010B60__INCLUDED)
