#if !defined(AFX_DBDATETIMEPPG_H__345FBCF1_3D2A_4760_8420_C35436728CDB__INCLUDED_)
#define AFX_DBDATETIMEPPG_H__345FBCF1_3D2A_4760_8420_C35436728CDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DBDateTimePpg.h : Declaration of the CDBDateTimePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage : See DBDateTimePpg.cpp.cpp for implementation.

class CDBDateTimePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDBDateTimePropPage)
	DECLARE_OLECREATE_EX(CDBDateTimePropPage)

// Constructor
public:
	CDBDateTimePropPage();

// Dialog Data
	//{{AFX_DATA(CDBDateTimePropPage)
	enum { IDD = IDD_PROPPAGE_DBDATETIME };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDBDateTimePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBDATETIMEPPG_H__345FBCF1_3D2A_4760_8420_C35436728CDB__INCLUDED)
