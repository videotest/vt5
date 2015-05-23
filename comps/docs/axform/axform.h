// axform.h : main header file for the AXFORM DLL
//

#if !defined(AFX_AXFORM_H__5D0317FA_E3A8_4BA9_9AD7_6D2EF2A4F3B3__INCLUDED_)
#define AFX_AXFORM_H__5D0317FA_E3A8_4BA9_9AD7_6D2EF2A4F3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAxformApp
// See axform.cpp for the implementation of this class
//

class CAxformApp : public CWinApp
{
public:
	CAxformApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAxformApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAxformApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline HWND GetNotChildParent(HWND hwnd)
{
	DWORD dwStyle;
	do
	{
		hwnd = ::GetParent(hwnd);
		if (hwnd) dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
	}
	while (hwnd&&(dwStyle&WS_CHILD)!=0);
	return hwnd;
}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AXFORM_H__5D0317FA_E3A8_4BA9_9AD7_6D2EF2A4F3B3__INCLUDED_)
