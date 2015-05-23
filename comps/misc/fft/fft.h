// fft.h : main header file for the FFT DLL
//

#if !defined(AFX_FFT_H__0227801D_EF1F_4E40_A606_0312D58E2D08__INCLUDED_)
#define AFX_FFT_H__0227801D_EF1F_4E40_A606_0312D58E2D08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFftApp
// See fft.cpp for the implementation of this class
//

class CFftApp : public CWinApp
{
public:
	CFftApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFftApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFftApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FFT_H__0227801D_EF1F_4E40_A606_0312D58E2D08__INCLUDED_)
