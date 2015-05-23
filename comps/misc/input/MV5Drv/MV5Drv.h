// MV5Drv.h : main header file for the MV5DRV DLL
//

#if !defined(AFX_MV5DRV_H__1EBBE4A7_099E_11D4_A98A_FA6524668D51__INCLUDED_)
#define AFX_MV5DRV_H__1EBBE4A7_099E_11D4_A98A_FA6524668D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMV5DrvApp
// See MV5Drv.cpp for the implementation of this class
//

class CMV5DrvApp : public CWinApp
{
public:
	CMV5DrvApp();
	void InitSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMV5DrvApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMV5DrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MV5DRV_H__1EBBE4A7_099E_11D4_A98A_FA6524668D51__INCLUDED_)
