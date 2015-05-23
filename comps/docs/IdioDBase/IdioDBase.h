// IdioDBase.h : main header file for the IdioDBase DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CIdioDBaseApp
// See IdioDBase.cpp for the implementation of this class
//

class CIdioDBaseApp : public CWinApp
{
public:
	CIdioDBaseApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
