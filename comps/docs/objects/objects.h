// objects.h : main header file for the OBJECTS DLL
//

#if !defined(AFX_OBJECTS_H__A594B230_8B74_11D3_A684_0090275995FE__INCLUDED_)
#define AFX_OBJECTS_H__A594B230_8B74_11D3_A684_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CObjectsApp
// See objects.cpp for the implementation of this class
//

class CObjectsApp : public CWinApp
{
public:
	CObjectsApp();
	~CObjectsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CObjectsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTS_H__A594B230_8B74_11D3_A684_0090275995FE__INCLUDED_)
