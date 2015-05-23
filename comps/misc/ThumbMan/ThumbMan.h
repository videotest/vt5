// ThumbMan.h : main header file for the THUMBMAN DLL
//

#if !defined(AFX_THUMBMAN_H__3099960A_B35F_4155_BE33_B79F06BC5600__INCLUDED_)
#define AFX_THUMBMAN_H__3099960A_B35F_4155_BE33_B79F06BC5600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CThumbManApp
// See ThumbMan.cpp for the implementation of this class
//

class CThumbManApp : public CWinApp
{
public:
	CThumbManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThumbManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CThumbManApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBMAN_H__3099960A_B35F_4155_BE33_B79F06BC5600__INCLUDED_)
