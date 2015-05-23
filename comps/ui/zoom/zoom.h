// zoom.h : main header file for the ZOOM DLL
//

#if !defined(AFX_ZOOM_H__EA2A7040_4649_11D3_A614_0090275995FE__INCLUDED_)
#define AFX_ZOOM_H__EA2A7040_4649_11D3_A614_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZoomApp
// See zoom.cpp for the implementation of this class
//

class CZoomApp : public CWinApp
{
public:
	CZoomApp();
	~CZoomApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CZoomApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOM_H__EA2A7040_4649_11D3_A614_0090275995FE__INCLUDED_)
