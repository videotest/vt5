#if !defined(AFX_RECTANGLEPPG_H__BC127B38_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_RECTANGLEPPG_H__BC127B38_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// RectanglePpg.h : Declaration of the CVTRectanglePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTRectanglePropPage : See RectanglePpg.cpp.cpp for implementation.

class CVTRectanglePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTRectanglePropPage)
	DECLARE_OLECREATE_EX(CVTRectanglePropPage)

// Constructor
public:
	CVTRectanglePropPage();

// Dialog Data
	//{{AFX_DATA(CVTRectanglePropPage)
	enum { IDD = IDD_PROPPAGE_VTDRECTANGLE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTRectanglePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECTANGLEPPG_H__BC127B38_026C_11D4_8124_0000E8DF68C3__INCLUDED)
