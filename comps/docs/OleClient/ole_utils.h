#ifndef __ole_utils_h__
#define __ole_utils_h__

#include "oledata.h"


IOleFramePtr GetMainFrame();

void GetIconInfo(LPCTSTR lpszRegInfo, LPTSTR lpszImagePath,UINT& nIndex);
HICON GetIconFromRegistry( HMODULE hInst, CLSID& clsid );

#endif //__ole_utils_h__
