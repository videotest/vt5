#ifndef __statusint_h__
#define __statusint_h__

#include "defs.h"

//get IStatusBar from IMainFrame
//get the hwnd from IWindow
//get pane no using GetPaneID
//send as usual messages to the status window

interface INewStatusBar : public IUnknown
{
	com_call AddPane( const GUID &guidPane, int cxWidth, HWND hwnd, DWORD dwStyle ) = 0;
	com_call RemovePane( const GUID &guidPane ) = 0;
	com_call GetPaneID( const GUID &guidPane, long *pnID ) = 0;
	com_call SetPaneIcon( const GUID &guidPane, HICON hIcon ) = 0;
};

declare_interface( INewStatusBar, "71F6349C-35A4-40bd-AF11-1461988B4BE3" )

#endif //__statusint_h__