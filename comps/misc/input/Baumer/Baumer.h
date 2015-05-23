
// Baumer.h : main header file for the BAUMER DLL
//

#if !defined(AFX_BAUMER_H__CC1D8AC2_917E_41FD_B9B1_C8E8D0FA2D28__INCLUDED_)
#define AFX_BAUMER_H__CC1D8AC2_917E_41FD_B9B1_C8E8D0FA2D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Cxlibapi.h"
#include "VT5Profile.h"

/////////////////////////////////////////////////////////////////////////////
// CBaumerApp
// See Baumer.cpp for the implementation of this class
//

class CBaumerApp : public CWinApp
{
public:
	CBaumerApp();

	BOOL m_bDiffProf;
	void InitProfile(LPCTSTR lpName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaumerApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBaumerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CLock
{
public:
	bool *m_pbLockVar;
	bool m_bPrev;
	CLock(bool *pbLockVar)
	{
		m_bPrev = *pbLockVar;
		m_pbLockVar = pbLockVar;
		*pbLockVar = true;
	};
	~CLock()
	{
		*m_pbLockVar = m_bPrev;
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAUMER_H__CC1D8AC2_917E_41FD_B9B1_C8E8D0FA2D28__INCLUDED_)
