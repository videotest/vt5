#ifndef __BCGCONTROLBAR_H
#define __BCGCONTROLBAR_H

#ifdef _AFXDLL
	#ifdef _BCGCONTROLBAR_
	   #define BCGCONTROLBARDLLEXPORT  _declspec(dllexport)
	#else
	   #define BCGCONTROLBARDLLEXPORT  _declspec(dllimport)
	#endif
#else
	#define BCGCONTROLBARDLLEXPORT
#endif

#include "BCGUserToolsManager.h"

inline BOOL IsStandardCommand (UINT uiCmd)
{
	return	((uiCmd >= ID_FILE_MRU_FILE1 && 
				uiCmd <= ID_FILE_MRU_FILE16)		||	// MRU commands,
			(uiCmd >= 0xF000 && uiCmd < 0xF1F0)		||	// system commands,
			((int) uiCmd >= AFX_IDM_FIRST_MDICHILD)	||	// windows commands
			(uiCmd >= ID_OLE_VERB_FIRST && uiCmd <= ID_OLE_VERB_LAST) ||		// OLE commands
			g_pUserToolsManager != NULL && uiCmd == g_pUserToolsManager->GetToolsEntryCmd ());
}

#endif // __BCGCONTROLBAR_H
