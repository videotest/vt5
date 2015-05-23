// AbacusUI.h : main header file for the AbacusUI DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CAbacusUIApp
// See AbacusUI.cpp for the implementation of this class
//

class CAbacusUIApp : public CWinApp
{
public:
	CAbacusUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
