// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AF2E7905_1A6B_4B5C_B62A_6BB8FE059A14__INCLUDED_)
#define AFX_STDAFX_H__AF2E7905_1A6B_4B5C_B62A_6BB8FE059A14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include <objidl.h>
#include <commctrl.h>


#include "objbase.h"
#include "comdef.h"
#include "docobj.h"


//common2
#include "com_main.h"
#include "com_unknown.h"
#include "data_main.h"

#include "nameconsts.h"

#include <oledlg.h>
#if _MSC_VER >= 1300
	#define OLEUI_CCHPATHMAX                256
#else
	#include <olestd.h>
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AF2E7905_1A6B_4B5C_B62A_6BB8FE059A14__INCLUDED_)
