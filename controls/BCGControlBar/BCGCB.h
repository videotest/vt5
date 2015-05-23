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

#ifndef __BCGCB_H
#define __BCGCB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#define _BCGCB_VERSION_			0x40640
#define _BCGCB_VERSION_MAJOR	4
#define _BCGCB_VERSION_MINOR	64

#ifdef _BCGCB_IN_OTHER_DLL
BCGCONTROLBARDLLEXPORT void BCGControlBarDllInitialize ();	// By Guy Hachlili
#endif // _BCGCB_IN_OTHER_DLL

#ifdef _AFXDLL
	#ifdef _DEBUG
		#ifdef _UNICODE
			#pragma comment(lib,"BCGCB464UD.lib") 
			#pragma message("Automatically linking with BCGCB464UD.dll")
		#else
			#pragma comment(lib,"BCGCB464d.lib") 
			#pragma message("Automatically linking with BCGCB464d.dll")
		#endif
	#else
		#ifdef _UNICODE
			#pragma comment(lib,"BCGCB464U.lib") 
			#pragma message("Automatically linking with BCGCB464U.dll") 
		#else
			#pragma comment(lib,"BCGCB464.lib") 
			#pragma message("Automatically linking with BCGCB464.dll") 
		#endif
	#endif
#else
	#ifdef _DEBUG
		#ifdef _UNICODE
			#pragma comment(lib,"BCGCB464Staticud.lib") 
			#pragma message("Automatically linking with BCGCB464Staticud.lib") 
		#else
			#pragma comment(lib,"BCGCB464Staticd.lib") 
			#pragma message("Automatically linking with BCGCB464Staticd.lib") 
		#endif
	#else
		#ifdef _UNICODE
			#pragma comment(lib,"BCGCB464StaticU.lib") 
			#pragma message("Automatically linking with BCGCB464StaticU.lib") 
		#else
			#pragma comment(lib,"BCGCB464Static.lib") 
			#pragma message("Automatically linking with BCGCB464Static.lib") 
		#endif
	#endif
#endif

//------------------
// BCG control bars:
//------------------
#include "BCGToolBar.h"
#include "BCGMenuBar.h"
#include "BCGDialogBar.h"
#include "BCGOutlookBar.h"

//-------------------------
// BCG control bar buttons:
//-------------------------
#include "BCGToolbarButton.h"
#include "BCGToolbarComboBoxButton.h"
#include "BCGToolbarMenuButton.h"
#include "BCGToolbarRegularMenuButton.h"
#include "BCGToolbarEditBoxButton.h"
#include "BCGDropDown.h"

//----------------------------------------------------------------------
// BCG frame windows (replaces CFrameWnd, CMDIFrameWnd, CMDIChildWnd and
// COleIPFrameWnd):
//----------------------------------------------------------------------
#include "BCGFrameWnd.h"
#include "BCGMDIFrameWnd.h"
#include "BCGMDIChildWnd.h"
#include "BCGOleIPFrameWnd.h"

//-------------------------
// BCG customization stuff:
//-------------------------
#include "CBCGToolbarCustomize.h"

#include "BCGContextMenuManager.h"
#include "BCGKeyboardManager.h"
#include "BCGMouseManager.h"

#include "BCGUserTool.h"

//-----------------------
// BCG workspace manager
//-----------------------
#include "BCGWorkspace.h"
#include "BCGRegistry.h"
#include "RebarState.h"

//-----------------------
// BCG menu replacements:
//-----------------------
#include "BCGPopupMenu.h"
#include "BCGPopupMenuBar.h"
#include "BCGToolBarImages.h"

//------------------
// BCG docking bars:
//------------------
#include "BCGSizingControlBar.h"
#include "BCGTabWnd.h"

//--------------
// BCG controls:
//--------------
#include "BCGButton.h"
#include "BCGMenuButton.h"
#include "BCGURLLinkButton.h"
#include "BCGEditListBox.h"

//-------------
// BCG dialogs:
//-------------
#include "BCGFileDialog.h"
#include "BCGWindowsManagerDlg.h"
#include "BCGPrintPreviewView.h"
#include "ImageEditDlg.h"

#endif // __BCGCB_H
