#if !defined(AFX_ELLIPSEPPG_H__BC127B3D_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_ELLIPSEPPG_H__BC127B3D_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EllipsePpg.h : Declaration of the CVTEllipsePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTEllipsePropPage : See EllipsePpg.cpp.cpp for implementation.

class CVTEllipsePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTEllipsePropPage)
	DECLARE_OLECREATE_EX(CVTEllipsePropPage)

// Constructor
public:
	CVTEllipsePropPage();

// Dialog Data
	//{{AFX_DATA(CVTEllipsePropPage)
	enum { IDD = IDD_PROPPAGE_VTDELLIPSE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTEllipsePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELLIPSEPPG_H__BC127B3D_026C_11D4_8124_0000E8DF68C3__INCLUDED)
