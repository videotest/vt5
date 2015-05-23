#if !defined(AFX_CONTEXTAXPPG_H__40D57BA2_81AF_4226_9C9F_C21ED09DAF89__INCLUDED_)
#define AFX_CONTEXTAXPPG_H__40D57BA2_81AF_4226_9C9F_C21ED09DAF89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContextAXPpg.h : Declaration of the CContextAXPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CContextAXPropPage : See ContextAXPpg.cpp.cpp for implementation.

class CContextAXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CContextAXPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CContextAXPropPage)

// Constructor
public:
	CContextAXPropPage();

// Dialog Data
	//{{AFX_DATA(CContextAXPropPage)
	enum { IDD = IDD_PROPPAGE_CONTEXTAX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CContextAXPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTAXPPG_H__40D57BA2_81AF_4226_9C9F_C21ED09DAF89__INCLUDED)
