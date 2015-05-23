//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "TrackMouse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

VOID CALLBACK BCGTrackMouseTimerProc (HWND hWnd, 
									  UINT /*uMsg*/,
									  UINT idEvent, 
									  DWORD /*dwTime*/)
{
	RECT	rect;
	POINT	pt;
	
	::GetClientRect (hWnd, &rect);
	::MapWindowPoints (hWnd, NULL, (LPPOINT)&rect, 2);

	::GetCursorPos (&pt);
	if (!::PtInRect (&rect, pt) || (WindowFromPoint(pt) != hWnd)) 
	{
		if (!::KillTimer (hWnd, idEvent))
		{
			// Error killing the timer!
		}
		
		::PostMessage (hWnd,WM_MOUSELEAVE, 0, 0);
	}
}
//************************************************************************************
BOOL BCGTrackMouse (LPTRACKMOUSEEVENT ptme)
{
	ASSERT (ptme != NULL);
	if (ptme->cbSize < sizeof (TRACKMOUSEEVENT))
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	if (!::IsWindow(ptme->hwndTrack)) 
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	if (!(ptme->dwFlags & TME_LEAVE)) 
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	return ::SetTimer (ptme->hwndTrack, ptme->dwFlags, 100,
			(TIMERPROC) BCGTrackMouseTimerProc);
}
