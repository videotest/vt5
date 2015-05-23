// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7D6E0154_3F70_4ABD_91C8_3FF1AE72ED93__INCLUDED_)
#define AFX_STDAFX_H__7D6E0154_3F70_4ABD_91C8_3FF1AE72ED93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0501
#define _WIN32_WINNT 0x501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
//#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "common.h"
#include "dbase.h"
#include "Guard.h"
#include "printconst.h"


#include <afxole.h>
#include <atlbase.h>
extern ATL::CComModule _Module;
#include <atlcom.h>
#include <atldbcli.h>
#include <atldbsch.h>


#include <comdef.h>

#import "msado15.dll" rename_namespace("ADO") rename("EOF", "ADOEOF")
#import "msadox.dll" rename_namespace("ADOX") rename("EOF", "ADOEOF")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7D6E0154_3F70_4ABD_91C8_3FF1AE72ED93__INCLUDED_)
