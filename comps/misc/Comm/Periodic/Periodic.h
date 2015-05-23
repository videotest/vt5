// Periodic.h : main header file for the PERIODIC DLL
//

#if !defined(AFX_PERIODIC_H__7CE74C7F_331A_4A19_A06B_2131FF739685__INCLUDED_)
#define AFX_PERIODIC_H__7CE74C7F_331A_4A19_A06B_2131FF739685__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPeriodicApp
// See Periodic.cpp for the implementation of this class
//

class CPeriodicApp : public CWinApp
{
public:
	CPeriodicApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeriodicApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPeriodicApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERIODIC_H__7CE74C7F_331A_4A19_A06B_2131FF739685__INCLUDED_)
