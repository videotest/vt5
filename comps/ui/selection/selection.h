// selection.h : main header file for the SELECTION DLL
//

#if !defined(AFX_SELECTION_H__06B1A26B_53A3_11D3_A626_0090275995FE__INCLUDED_)
#define AFX_SELECTION_H__06B1A26B_53A3_11D3_A626_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


#include "PropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectionApp
// See selection.cpp for the implementation of this class
//

class CSelectionApp : public CWinApp
{
public:
	CSelectionApp();
	~CSelectionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSelectionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTION_H__06B1A26B_53A3_11D3_A626_0090275995FE__INCLUDED_)
