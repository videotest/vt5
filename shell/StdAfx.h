// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C48CE9BD_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_STDAFX_H__C48CE9BD_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0600
#define _WIN32_WINNT 0x600

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define __IActiveScriptParse_INTERFACE_DEFINED__
#include <ActivScp.h>
#include <comdef.h>

#pragma warning(disable: 4786)

#include "afxtempl.h"

//#include <../src/mfc/stdafx.h>
//#include <afxext.h>
//#include <afxtoolbar.h>
//#include <afxcontrolbars.h>
//#include <afxpriv.h>
//#include <afxtoolbarbutton.h>
//#include <afxMDIFrameWndEx.h>
//#include <afxtoolbarmenubutton.h>
//#include <afxpopupmenu.h>
//#include <afxtoolbarcomboboxbutton.h>
//#include <afxdockablepane.h>
//#include <afxmenuhash.h>

#include <map>
using std::map;

#include "common.h"
#include "guard.h"
#include "dbase.h"
//#include "menueditor.h"
#include "BCGCB.h"
#include "BCGMultiDocTemplate.h"
#include "BCGControlBarImpl.h"
#include "\vt5\awin\type_defs.h"


#include "\vt5\com_base\debug_misc.h"			
#ifdef _DEBUG
#define new DBG_NEW
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C48CE9BD_F947_11D2_A596_0000B493FF1B__INCLUDED_)
