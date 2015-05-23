#ifndef __drag_drop_defs_h__
#define __drag_drop_defs_h__

#include "defs.h"

struct std_dll drag_n_drop_data
{
	DROPEFFECT	dwDropEfect;
	DWORD		dwKeyState;
	POINT		point;
	HWND		hwndOwner;
	HWND		hwndDragTo;
	IUnknown*	punkDataObject;
};

#define WM_USER_MAX		0x7FFF


#define WM_DND_DO_ENTER_MODE	(WM_USER_MAX-1)
#define WM_DND_LAST			WM_DND_DO_ENTER_MODE
#define WM_DND_ON_DRAG_ENTER	(WM_USER_MAX-2)
#define WM_DND_ON_DRAG_OVER		(WM_USER_MAX-3)
#define WM_DND_ON_DROP			(WM_USER_MAX-4)
#define WM_DND_ON_DRAG_LEAVE	(WM_USER_MAX-5)
#define WM_DND_FIRST			WM_DND_ON_DRAG_LEAVE

#endif//__drag_drop_defs_h__


