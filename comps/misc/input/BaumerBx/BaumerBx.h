// BaumerBx.h : main header file for the BaumerBx DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CBaumerBxApp
// See BaumerBx.cpp for the implementation of this class
//

class CBaumerBxApp : public CWinApp
{
public:
	CBaumerBxApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
};
