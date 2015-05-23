#ifndef __common_h__
#define __common_h__


//#include "\vt5\common2\misc_new.h"

#include "comdef.h"
#include "afxtempl.h"
#include "afxdisp.h"

#include "htmlhelp.h"
#include "defs.h"

#include "dibsectionlite.h"

#include "core5.h"
#include "action5.h"
#include "window5.h"
#include "docview5.h"
#include "data5.h"
#include "image5.h"
#include "object5.h"
#include "Classes5.h"
#include "axint.h"


#include "utils.h"
#include "contourapi.h"
#include "nameconsts.h"
#include "registry.h"
#include "CompManager.h"
#include "cmnbase.h"
#include "actionbase.h"
#include "argbase.h"
#include "wndbase.h"
#include "frame.h"
#include "docviewbase.h"
#include "filebase.h"
#include "DataBase.h"
#include "imagebase.h"
#include "splitbase.h"
#include "filterbase.h"
#include "cmdtargex.h"
#include "helpbase.h"
#include "timetest.h"
#include "objbase_.h"
#include "ClassBase.h"

#include "oblistwrp.h"
#include "clipboard.h"

#include "ImageCont.h"

#include "memory"

using namespace std;

#define _debugger
#define COMMON1

#ifndef _COMMON_LIB
//include .lib
#define _LIBNAME_COMMON "common.lib"

#pragma message( "Common library linking with " _LIBNAME_COMMON )
#pragma comment(lib, _LIBNAME_COMMON)




#endif //_COMMON_LIB

#endif //__common_h__