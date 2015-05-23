// SpermParams.h : main header file for the SpermParams DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#include "common.h"
//#include "Guard.h"


// CSpermParamsApp
// See SpermParams.cpp for the implementation of this class
//

class CSpermParamsApp : public CWinApp
{
public:
	CSpermParamsApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
