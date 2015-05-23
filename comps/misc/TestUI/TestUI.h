// TestUI.h : main header file for the TestUI DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CTestUIApp
// See TestUI.cpp for the implementation of this class
//

class CTestUIApp : public CWinApp
{
public:
	CTestUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
