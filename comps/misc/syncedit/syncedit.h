// syncedit.h : main header file for the SYNCEDIT DLL
//

#if !defined(AFX_SYNCEDIT_H__76F8F694_960D_4FDE_9BD7_8A302D5B1B9D__INCLUDED_)
#define AFX_SYNCEDIT_H__76F8F694_960D_4FDE_9BD7_8A302D5B1B9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSynceditApp
// See syncedit.cpp for the implementation of this class
//

class CSynceditApp : public CWinApp
{
public:
	CSynceditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSynceditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSynceditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CActionSyncEdit : public CActionBase
{
	DECLARE_DYNCREATE(CActionSyncEdit)
	GUARD_DECLARE_OLECREATE(CActionSyncEdit)

public:
	CActionSyncEdit();
	virtual ~CActionSyncEdit();
public:
	virtual bool Invoke();
	virtual DWORD GetActionState();
	virtual bool IsAvaible();
	virtual bool IsChecked();

};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCEDIT_H__76F8F694_960D_4FDE_9BD7_8A302D5B1B9D__INCLUDED_)
//[ag]1. classes

