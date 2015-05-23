// CGH_Chromos.h : main header file for the CGH_CHROMOS DLL
//

#if !defined(AFX_CGH_CHROMOS_H__C3E08E6C_F98B_4B3B_A266_0D8B1BBFCBAE__INCLUDED_)
#define AFX_CGH_CHROMOS_H__C3E08E6C_F98B_4B3B_A266_0D8B1BBFCBAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCGH_ChromosApp
// See CGH_Chromos.cpp for the implementation of this class
//

class CCGH_ChromosApp : public CWinApp
{
public:
	CCGH_ChromosApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCGH_ChromosApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCGH_ChromosApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGH_CHROMOS_H__C3E08E6C_F98B_4B3B_A266_0D8B1BBFCBAE__INCLUDED_)
