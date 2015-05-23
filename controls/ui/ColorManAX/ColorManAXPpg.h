#if !defined(AFX_COLORMANAXPPG_H__5DB73BC5_F590_11D3_A0BC_0000B493A187__INCLUDED_)
#define AFX_COLORMANAXPPG_H__5DB73BC5_F590_11D3_A0BC_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ColorManAXPpg.h : Declaration of the CColorManAXPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CColorManAXPropPage : See ColorManAXPpg.cpp.cpp for implementation.

class CColorManAXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CColorManAXPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CColorManAXPropPage)

// Constructor
public:
	CColorManAXPropPage();

// Dialog Data
	//{{AFX_DATA(CColorManAXPropPage)
	enum { IDD = IDD_PROPPAGE_COLORMANAX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CColorManAXPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORMANAXPPG_H__5DB73BC5_F590_11D3_A0BC_0000B493A187__INCLUDED)
