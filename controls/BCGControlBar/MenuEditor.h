#ifndef __menueditor_h__
#define __menueditor_h__

#include "bcgbarres.h"
#include "BCGContextMenuManager.h"
#include "BCGControlBar.h"
//#include "BCGDockBar.h"
#include "BCGExCheckList.h"
#include "BCGFrameImpl.h"
#include "BCGFrameWnd.h"
#include "BCGKeyboardManager.h"
#include "bcglocalres.h"
#include "BCGMainClientAreaWnd.h"
#include "BCGMDIFrameWnd.h"
#include "BCGMenuBar.h"
#include "BCGMouseManager.h"
#include "BCGMultiDocTemplate.h"
#include "BCGPopupMenu.h"
#include "BCGPopupMenuBar.h"
#include "BCGToolBar.h"
#include "BCGToolbarButton.h"
#include "BCGToolbarComboBoxButton.h"
#include "BCGToolbarDropSource.h"
#include "BCGToolbarDropTarget.h"
#include "BCGToolBarImages.h"
#include "BCGToolbarManuBarButton.h"
#include "BCGToolbarMenuButton.h"
#include "BCGToolbarMenuButtonsButton.h"
#include "BCGToolbarSystemMenuButton.h"
#include "ButtonAppearanceDlg.h"
#include "ButtonsList.h"
#include "ButtonsTextList.h"
#include "CBCGToolbarCustomize.h"
#include "CBCGToolbarCustomizePages.h"
#include "ColorSelector.h"
#include "CustToolbarDlg.h"
#include "GLOBALS.H"
#include "ImageEditDlg.h"
#include "ImageHash.h"
#include "ImagePaintArea.h"
#include "KeyAssign.h"
#include "KeyboardPage.h"
#include "KeyHelper.h"
#include "MenuHash.h"
#include "MenuPage.h"
#include "MousePage.h"
#include "PaletteCommandFrameWnd.h"
#include "PropertyFrame.h"
#include "Registry.h"
#include "ToolbarNameDlg.h"
#include "sizecbar.h"

//include .lib
#ifdef _DEBUG
#define _LIBNAME "menueditor_d.lib"
#else
#define _LIBNAME "menueditor.lib"
#endif //_DEBUG

#pragma message( "Menu editor library linking with " _LIBNAME )
#pragma comment(lib, _LIBNAME)

#endif //__menueditor_h__