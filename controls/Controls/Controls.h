// Controls.h : main header file for the CONTROLS DLL
//

#if !defined(AFX_CONTROLS_H__8ED507C3_C4EA_4CD0_8FD5_A75151F2CFBD__INCLUDED_)
#define AFX_CONTROLS_H__8ED507C3_C4EA_4CD0_8FD5_A75151F2CFBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ControlDef.h"

#include "resource.h"		// main symbols

#ifndef _CONTROL_LIB_
//include .lib
#ifdef _DEBUG
#define _LIBNAME_CONTROLS_ "Controls_d.lib"
#else
#define _LIBNAME_CONTROLS_ "Controls.lib"
#endif //_DEBUG

#pragma message("Controls library linking with " _LIBNAME_CONTROLS_)
#pragma comment(lib, _LIBNAME_CONTROLS_)



#endif //_COMMON_LIB


/////////////////////////////////////////////////////////////////////////////
// CControlsApp
// See Controls.cpp for the implementation of this class
//

class CControlsApp : public CWinApp
{
public:
	CControlsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CControlsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLS_H__8ED507C3_C4EA_4CD0_8FD5_A75151F2CFBD__INCLUDED_)
