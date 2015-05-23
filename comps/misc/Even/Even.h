// Even.h : main header file for the EVEN DLL
//

#if !defined(AFX_EVEN_H__D2DDCA8A_FD9B_47CA_BFF2_DFD9E76C6691__INCLUDED_)
#define AFX_EVEN_H__D2DDCA8A_FD9B_47CA_BFF2_DFD9E76C6691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEvenApp
// See Even.cpp for the implementation of this class
//

class CEvenApp : public CWinApp
{
public:
	CEvenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEvenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVEN_H__D2DDCA8A_FD9B_47CA_BFF2_DFD9E76C6691__INCLUDED_)
