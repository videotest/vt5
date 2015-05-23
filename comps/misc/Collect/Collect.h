// Collect.h : main header file for the COLLECT DLL
//

#if !defined(AFX_COLLECT_H__ED3BE9A7_0293_41EA_87C1_CF98F10235CE__INCLUDED_)
#define AFX_COLLECT_H__ED3BE9A7_0293_41EA_87C1_CF98F10235CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCollectApp
// See Collect.cpp for the implementation of this class
//

class CCollectApp : public CWinApp
{
public:
	CCollectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCollectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLLECT_H__ED3BE9A7_0293_41EA_87C1_CF98F10235CE__INCLUDED_)
