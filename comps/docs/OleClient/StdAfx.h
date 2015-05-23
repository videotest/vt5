// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4EC6FA16_15DB_48CD_A323_D2DBEA3C5B9A__INCLUDED_)
#define AFX_STDAFX_H__4EC6FA16_15DB_48CD_A323_D2DBEA3C5B9A__INCLUDED_

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

#include <oledlg.h>

#if _MSC_VER >= 1300
	#define OLEUI_CCHPATHMAX                256
#else
	#include <olestd.h>
#endif

//ReleaseInterface calls 'Release' and NULLs the pointer
#define ReleaseInterface(p)\
            {\
            IUnknown *pt=(IUnknown *)p;\
            p=NULL;\
            if (NULL!=pt)\
                pt->Release();\
            }



// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4EC6FA16_15DB_48CD_A323_D2DBEA3C5B9A__INCLUDED_)
