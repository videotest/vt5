// AVIDocMan.h : main header file for the AVIDOCMAN DLL
//

#if !defined(AFX_AVIDOCMAN_H__7747DE85_106F_11D4_A98A_B0B0C5EF21FE__INCLUDED_)
#define AFX_AVIDOCMAN_H__7747DE85_106F_11D4_A98A_B0B0C5EF21FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAVIDocManApp
// See AVIDocMan.cpp for the implementation of this class
//

class CAVIDocManApp : public CWinApp
{
public:
	CAVIDocManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVIDocManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAVIDocManApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVIDOCMAN_H__7747DE85_106F_11D4_A98A_B0B0C5EF21FE__INCLUDED_)
