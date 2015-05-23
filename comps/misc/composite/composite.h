// composite.h : main header file for the composite DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CcompositeApp
// See composite.cpp for the implementation of this class
//

class CCompositeApp : public CWinApp
{
public:
	CCompositeApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
