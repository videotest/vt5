// Classes.h : main header file for the CLASSES DLL
//

#if !defined(AFX_CLASSES_H__83233D41_349C_4507_8CCF_A5986B304D84__INCLUDED_)
#define AFX_CLASSES_H__83233D41_349C_4507_8CCF_A5986B304D84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClassesApp
// See Classes.cpp for the implementation of this class
//

class CClassesApp : public CWinApp
{
public:
	CClassesApp();
	~CClassesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CClassesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSES_H__83233D41_349C_4507_8CCF_A5986B304D84__INCLUDED_)
