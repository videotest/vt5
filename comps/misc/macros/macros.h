// macros.h : main header file for the MACROS DLL
//

#if !defined(AFX_MACROS_H__8E8E833E_6B33_11D3_A652_0090275995FE__INCLUDED_)
#define AFX_MACROS_H__8E8E833E_6B33_11D3_A652_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMacrosApp
// See macros.cpp for the implementation of this class
//

class CMacrosApp : public CWinApp
{
public:
	CMacrosApp();
	~CMacrosApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacrosApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMacrosApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACROS_H__8E8E833E_6B33_11D3_A652_0090275995FE__INCLUDED_)
