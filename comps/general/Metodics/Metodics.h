// Metodics.h : main header file for the Metodics DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CMetodicsApp
// See Metodics.cpp for the implementation of this class
//

class CMetodicsApp : public CWinApp
{
public:
	CMetodicsApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
