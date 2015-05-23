// PhaseMeasure.h : main header file for the PhaseMeasure DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#include "common.h"
//#include "Guard.h"


// CPhaseMeasureApp
// See PhaseMeasure.cpp for the implementation of this class
//

class CPhaseMeasureApp : public CWinApp
{
public:
	CPhaseMeasureApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
