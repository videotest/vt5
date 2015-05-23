//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************


// BCGControlBar.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "bcgsound.h"
#include "bcglocalres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _MSC_VER >= 1300
AFX_DATADEF AUX_DATA afxData;
#endif

static AFX_EXTENSION_MODULE BCGControlBarDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("BCGCONTROLBAR.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(BCGControlBarDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		#ifndef _BCGCB_IN_OTHER_DLL
			new CDynLinkLibrary(BCGControlBarDLL);
		#endif	// _BCGCB_IN_OTHER_DLL
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("BCGCONTROLBAR.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(BCGControlBarDLL);
	}
	return 1;   // ok
}

///////////////////////////////////////////////////////////////////////
// CBCGLocalResource implementation:

CBCGLocalResource::CBCGLocalResource()
{
	m_hInstOld = AfxGetResourceHandle ();
	ASSERT (m_hInstOld != NULL);

	AfxSetResourceHandle (BCGControlBarDLL.hModule);
}

CBCGLocalResource::~CBCGLocalResource()
{
	AfxSetResourceHandle (m_hInstOld);
}

//----------------
// By Guy Hachlili
//----------------
#ifdef _BCGCB_IN_OTHER_DLL
	__declspec(dllexport) void BCGControlBarDllInitialize ()
	{
		new CDynLinkLibrary(BCGControlBarDLL);
	}
#endif	// _BCGCB_IN_OTHER_DLL
