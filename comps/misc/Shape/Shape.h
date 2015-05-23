// Shape.h : main header file for the Shape DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CShapeApp
// See Shape.cpp for the implementation of this class
//

class CShapeApp : public CWinApp
{
public:
	CShapeApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
