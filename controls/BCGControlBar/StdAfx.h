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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EBD69F5A_C937_11D1_A647_00A0C93A70EC__INCLUDED_)
#define AFX_STDAFX_H__EBD69F5A_C937_11D1_A647_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WINVER 0x0501
#define _WIN32_WINNT 0x501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmenuimages.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#include <AfxAdv.h>
#endif // _AFX_NO_OLE_SUPPORT

#include <afxtempl.h>
#include <afxpriv.h>

#if _MSC_VER >= 1300
#include "mfc_defs.h"
#else
	#include <..\src\afximpl.h>
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning (disable : 4201)
	#include "mmsystem.h"
	#pragma comment(lib,"winmm.lib") 
	#pragma message("Automatically linking with winmm.lib")
#pragma warning (default : 4201)

#include "defs.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EBD69F5A_C937_11D1_A647_00A0C93A70EC__INCLUDED_)
