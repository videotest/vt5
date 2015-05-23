#if !defined(AFX_STDAFX_H__18B8A3CF_D918_4CEC_B1DB_85673EEFC55B__INCLUDED_)
#define AFX_STDAFX_H__18B8A3CF_D918_4CEC_B1DB_85673EEFC55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0501
#define _WIN32_WINNT 0x501

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes

#include "common.h"
#include "dbase.h"
#include "Guard.h"

#define SECTION_DBASEFIELDS	"\\RecordNamedData\\Fields"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__18B8A3CF_D918_4CEC_B1DB_85673EEFC55B__INCLUDED_)
