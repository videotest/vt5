#if !defined(AFX_RECT1PROPPAGE_H__D53AA1AF_1F57_4112_BB69_CC37A721D678__INCLUDED_)
#define AFX_RECT1PROPPAGE_H__D53AA1AF_1F57_4112_BB69_CC37A721D678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Rect1PropPage.h : header file
//
#include "Circle1PropPage.h"
/////////////////////////////////////////////////////////////////////////////
// CRect1PropPage dialog

class CRect1PropPage : public CCircle1PropPage
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CRect1PropPage);
	GUARD_DECLARE_OLECREATE(CRect1PropPage);
// Construction
public:
	CRect1PropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRect1PropPage)
	enum { IDD = IDD_PROPPAGE_RECT1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRect1PropPage)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRect1PropPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECT1PROPPAGE_H__D53AA1AF_1F57_4112_BB69_CC37A721D678__INCLUDED_)
