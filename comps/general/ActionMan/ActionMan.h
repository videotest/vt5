// ActionMan.h : main header file for the ACTIONMAN DLL
//

#if !defined(AFX_ACTIONMAN_H__8069CA4B_02B2_11D3_A5A0_0000B493FF1B__INCLUDED_)
#define AFX_ACTIONMAN_H__8069CA4B_02B2_11D3_A5A0_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CActionManApp
// See ActionMan.cpp for the implementation of this class
//

class CActionManApp : public CWinApp
{
public:
	CActionManApp();
	~CActionManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CActionManApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONMAN_H__8069CA4B_02B2_11D3_A5A0_0000B493FF1B__INCLUDED_)
