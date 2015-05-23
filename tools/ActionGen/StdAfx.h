// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if !defined(_AFXDLL) && defined(_GXDLL)
#pragma message("If _AFXDLL is not specified the grid cannot be linked to as an Extension DLL\n")
#pragma message("To remedy this either change to link to MFC dynamically or remove _GXDLL from the project setting\n")
#error invalid configuration
#endif


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT





