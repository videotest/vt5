// NotifyContr.h : main header file for the NOTIFYCONTR DLL
//

#if !defined(AFX_NOTIFYCONTR_H__83C0057F_1E35_11D3_A5D1_0000B493FF1B__INCLUDED_)
#define AFX_NOTIFYCONTR_H__83C0057F_1E35_11D3_A5D1_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNotifyContrApp
// See NotifyContr.cpp for the implementation of this class
//

class CNotifyContrApp : public CWinApp
{
public:
	CNotifyContrApp();
	~CNotifyContrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifyContrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNotifyContrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTIFYCONTR_H__83C0057F_1E35_11D3_A5D1_0000B493FF1B__INCLUDED_)
