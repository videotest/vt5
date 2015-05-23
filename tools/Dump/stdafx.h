// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <time.h>
#include <Commdlg.h>
#include <stdlib.h>
#include <comdef.h>

//#define _AGNITUM_

#ifdef _AGNITUM_
#include "\agnitum\awin\type_defs.h"
#include "\agnitum\awin\misc_dbg.h"
#include "\agnitum\awin\win_dlg.h"
#include "\agnitum\awin\misc_string.h"
#include "\agnitum\awin\misc_list.h"
#include "\agnitum\awin\misc_ini.h"
#include "\agnitum\awin\misc_utils.h"
#else
#include "\vt5\awin\type_defs.h"
#include "\vt5\awin\misc_dbg.h"
#include "\vt5\awin\win_dlg.h"
#include "\vt5\awin\misc_string.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ini.h"
#include "\vt5\awin\misc_utils.h"
#endif//_AGNITUM_

//#include "detours.h"

// TODO: reference additional headers your program requires here
