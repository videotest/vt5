// Compare.h : main header file for the COMPARE DLL
//

#if !defined(AFX_COMPARE_H__0A17E6D6_0591_4075_96BB_35FF1FDCCF61__INCLUDED_)
#define AFX_COMPARE_H__0A17E6D6_0591_4075_96BB_35FF1FDCCF61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCompareApp
// See Compare.cpp for the implementation of this class
//

class CCompareApp : public CWinApp
{
public:
	CCompareApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCompareApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPARE_H__0A17E6D6_0591_4075_96BB_35FF1FDCCF61__INCLUDED_)
