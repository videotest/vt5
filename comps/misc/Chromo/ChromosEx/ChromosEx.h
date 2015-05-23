// ChromosEx.h : main header file for the CHROMOSEX DLL
//

#if !defined(AFX_CHROMOSEX_H__7BB4EF6D_9E30_4591_9914_0DDD5A15D879__INCLUDED_)
#define AFX_CHROMOSEX_H__7BB4EF6D_9E30_4591_9914_0DDD5A15D879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CChromosExApp
// See ChromosEx.cpp for the implementation of this class
//

class CChromosExApp : public CWinApp
{
public:
	CChromosExApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChromosExApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CChromosExApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHROMOSEX_H__7BB4EF6D_9E30_4591_9914_0DDD5A15D879__INCLUDED_)
