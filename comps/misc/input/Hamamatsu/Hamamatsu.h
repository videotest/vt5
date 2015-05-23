// Hamamatsu.h : main header file for the Hamamatsu DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CHamamatsuApp
// See Hamamatsu.cpp for the implementation of this class
//

class CHamamatsuApp : public CWinApp
{
public:
	CHamamatsuApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
};
