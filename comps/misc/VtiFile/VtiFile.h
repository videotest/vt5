// VtiFile.h : main header file for the VTIFILE DLL
//

#if !defined(AFX_VTIFILE_H__02298FE0_646D_4563_AD54_F3756BD10B64__INCLUDED_)
#define AFX_VTIFILE_H__02298FE0_646D_4563_AD54_F3756BD10B64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVtiFileApp
// See VtiFile.cpp for the implementation of this class
//

class CVtiFileApp : public CWinApp
{
public:
	CVtiFileApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVtiFileApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CVtiFileApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTIFILE_H__02298FE0_646D_4563_AD54_F3756BD10B64__INCLUDED_)
