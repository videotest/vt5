// AppMan.h : main header file for the APPMAN DLL
//

#if !defined(AFX_APPMAN_H__E7901240_3A7D_11D3_A604_0090275995FE__INCLUDED_)
#define AFX_APPMAN_H__E7901240_3A7D_11D3_A604_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAppManApp
// See AppMan.cpp for the implementation of this class
//

class CAppManApp : public CWinApp
{
public:
	CAppManApp();
	~CAppManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAppManApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern GUID IID_ITypeLibID;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPMAN_H__E7901240_3A7D_11D3_A604_0090275995FE__INCLUDED_)
