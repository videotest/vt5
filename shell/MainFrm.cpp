// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "shell.h"

#include "MainFrm.h"
#include "CommandManager.h"
#include "misc.h"
#include "ShellDockBar.h"
#include "ShellToolBar.h"
#include "targetman.h"
#include "ntfman.h"
#include "ShellFrame.h"
#include "ShellView.h"
#include "shellbutton.h"
#include "SplashWindow.h"
#include "docs.h"
#include "statusutils.h"
#include "activescript.h"
#include "MacroRecorder_int.h"
#include "method_int.h"
#include "AppDisp.h"

#include "\vt5\controls\xpbar_ctrl\xp_bar.h"
#include "..\awin\render.h"
#include "button_messages.h"

#include "\vt5\com_base\com_defs.h"
#include "\vt5\com_base\share_mem.h"
#include "\vt5\awin\profiler.h"


#define WM_SYSKEYFIRST  WM_SYSKEYDOWN
#define WM_SYSKEYLAST   WM_SYSDEADCHAR

#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

#define DOCKBARPRIORSEC _T("\\MainFrame\\DockBarsPriorities")

extern bool g_bShowTitle;

__declspec( dllimport )
CRuntimeClass *g_pclassToolbarRuntime;
__declspec( dllimport )
CRuntimeClass *g_pclassToolbarMenuButtonRuntime;
__declspec( dllimport )
CRuntimeClass *g_pclassDockBarRuntime;


/////////////////////////////////////////////////////////////////////////////////////////
//CMainFrameDropTarget
CMainFrameDropTarget::CMainFrameDropTarget()
{
	m_pMainFrame = 0;
}


void CMainFrameDropTarget::Register( CMainFrame *pFrm )
{
	m_pMainFrame = pFrm;
	COleDropTarget::Register( pFrm );
}

BOOL CMainFrameDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
{
	TRACE( "Object dropped to MainFrame\n" );
	HGLOBAL	hGlobal = pDataObject->GetGlobalData(CF_HDROP);
	if (hGlobal)
	{
		m_pMainFrame->SendMessage( WM_DROPFILES, (WPARAM)hGlobal );
		return TRUE;
	}	
	return FALSE;
}

DROPEFFECT CMainFrameDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
	if( pDataObject->IsDataAvailable( CF_HDROP ) )
		return DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
	
}



void CVtToolBarImages::SetUserFlag( bool bSet )
{
	m_bUserImagesList = bSet;
}

void CVtToolBarImages::DeleteAllImages()
{
	m_bModified  = false;
	m_iCount = 0;

	::DeleteObject( m_hbmImageWell );
	m_hbmImageWell = 0;
}

void CVtToolBarImages::SaveEx( LPCTSTR lpszBmpFileName, bool bsave_anyway /*= false*/ )
{
    // выставляем флаг модификации для гарантированного сохранения
	if( bsave_anyway && !m_bModified )
		m_bModified = TRUE;

	Save( lpszBmpFileName );
}


int  iMaxUserToolbars		= 50;

const UINT uiMenuBarId			= AFX_IDW_CONTROLBAR_FIRST + 20;
const UINT uiUserBarId			= AFX_IDW_CONTROLBAR_FIRST + 21;
const UINT uiXPHolderId			= AFX_IDW_CONTROLBAR_FIRST + 22;

// Changed by Maxim (for internal toolbar)
UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
UINT uiLastUserToolBarId	= uiFirstUserToolBarId + 2*iMaxUserToolbars - 1;

const UINT uiFirstDockToolBarId	= uiLastUserToolBarId + 1;
const UINT uiLaseDockToolBarId	= uiLastUserToolBarId + 101;





//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |


const DWORD CFrameWnd::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};




static bool s_bMiniFrameCaption = true;
IMPLEMENT_DYNCREATE(CVTMiniDockFrameWnd, CBCGMiniDockFrameWnd)


BEGIN_MESSAGE_MAP(CVTMiniDockFrameWnd, CBCGMiniDockFrameWnd)
	//{{AFX_MSG_MAP(CMyMiniDockFrameWnd)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void CVTMiniDockFrameWnd::RecalcLayout(BOOL bNotify)
{
	if (!m_bInRecalcLayout)
	{
		m_bInRecalcLayout = true;
		CWnd* pDockingWnd = GetWindow( GW_CHILD );	
		if( pDockingWnd )
		{
			CWnd* pChildBar = pDockingWnd->GetWindow( GW_CHILD );	
			if( pChildBar && pChildBar->IsKindOf( RUNTIME_CLASS(CShellDockBar)))
			{
				CShellDockBar* pShellDockBar = (CShellDockBar*)pChildBar;
				if(pShellDockBar->GetMiniFrameCaption())
					ModifyStyle(0, WS_CAPTION);
				else
					ModifyStyle(WS_CAPTION, 0);
			}
		}
		m_bInRecalcLayout = false;

		__super::RecalcLayout(bNotify);
	}
}

BOOL CVTMiniDockFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGMiniDockFrameWnd::PreCreateWindow( cs ) )
		return true;

	cs.style |= WS_CLIPCHILDREN;
	if (!s_bMiniFrameCaption)
		cs.style &= ~WS_CAPTION;
	
	return true;
}
BOOL CVTMiniDockFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->hwnd == GetSafeHwnd() )
	{
		if( 			
			pMsg->message == WM_KEYDOWN ||  
			pMsg->message == WM_SYSKEYDOWN ||  
			pMsg->message == WM_LBUTTONDOWN ||  
			pMsg->message == WM_RBUTTONDOWN ||  
			pMsg->message == WM_MBUTTONDOWN ||  
			pMsg->message == WM_NCLBUTTONDOWN ||  
			pMsg->message == WM_NCRBUTTONDOWN ||  
			pMsg->message == WM_NCMBUTTONDOWN ||
			pMsg->message == WM_NCACTIVATE ||
			pMsg->message == WM_ACTIVATE ||
			pMsg->message == WM_CREATE	||
			pMsg->message == WM_SETFOCUS ||
			pMsg->message == WM_SHOWWINDOW
			)
		{			
			CWnd* pDockingWnd = NULL;	
			
			
			pDockingWnd = GetWindow( GW_CHILD );	
			if( pDockingWnd )
			{
				CWnd* pChildBar = NULL;
				pChildBar = pDockingWnd->GetWindow( GW_CHILD );	
				if( pChildBar )
				{

					CWnd* pChildWnd = NULL;
					pChildWnd = pChildBar->GetWindow( GW_CHILD );	
					if( pChildWnd && pChildWnd->IsWindowVisible() && pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)) )
					{
						CShellFrame* pChildFrame = NULL;
						pChildFrame = (CShellFrame*)pChildWnd;						
						pChildFrame->SendMessage(WM_NCACTIVATE, (WPARAM)TRUE, 0L );

					}
				}
			}
		}
	}

	return CBCGMiniDockFrameWnd::PreTranslateMessage(pMsg);

		
}

void CVTMiniDockFrameWnd::OnClose()
{	
	//paul 30.07.2003. Try ask CShellDockBar about its "GetCanClose"
	bool bcan_close = true;
	CWnd* pwnd = GetWindow( GW_CHILD );
	
	if( pwnd->IsKindOf(RUNTIME_CLASS(CDockBar)) )
	{
		CDockBar* pdock_bar = (CDockBar*)pwnd;
		pwnd = pdock_bar->GetWindow( GW_CHILD );		
		if( pwnd && pwnd->IsKindOf(RUNTIME_CLASS(CShellDockBar)) )
		{
			CShellDockBar* pshell_bar = (CShellDockBar*)pwnd;
			bcan_close = pshell_bar->GetCanClose();
		}		
	}

	if( bcan_close )
	{
		CBCGMiniDockFrameWnd::OnClose();

		//paul 4.09.2002. When floating, not close. BT:2655,2651
		theApp.OnIdle( 0 );	
	}
}

void CVTMiniDockFrameWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((GetStyle()&WS_CAPTION)==0)
	{
		GetCursorPos(&m_ptStartDrag);
		CRect rc;
		GetWindowRect(&rc);
		m_ptStartPos = rc.TopLeft();
		SetCapture();
	}
	CBCGMiniDockFrameWnd::OnLButtonDown(nFlags, point);
}

void CVTMiniDockFrameWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((GetStyle()&WS_CAPTION)==0&&(nFlags&MK_LBUTTON)&&m_ptStartDrag.x>-1)
	{
		CPoint ptPos;
		GetCursorPos(&ptPos);
		int dx = ptPos.x-m_ptStartDrag.x,dy = ptPos.y-m_ptStartDrag.y;
		TRACE("m_ptStartPos.y = %d, ptPos.y = %d, dy = %d\n", m_ptStartPos.y, ptPos.y, dy);
		SetWindowPos(NULL,m_ptStartPos.x+dx,m_ptStartPos.y+dy,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	CBCGMiniDockFrameWnd::OnMouseMove(nFlags, point);
}

void CVTMiniDockFrameWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ((GetStyle()&WS_CAPTION)==0)
	{
		ReleaseCapture();
		m_ptStartPos = m_ptStartDrag = CPoint(-1,-1);
	}
	CBCGMiniDockFrameWnd::OnLButtonUp(nFlags, point);
}

void CVTMiniDockFrameWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CBCGMiniDockFrameWnd::OnWindowPosChanging(lpwndpos);
	// A.M. BT4429.
	if (lpwndpos->flags&SWP_NOMOVE)
		return;
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	int cx = rcWnd.Width();
	int cy = rcWnd.Height();
	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
//	int nSmCap = GetSystemMetrics(SM_CYSMCAPTION);
	int nFrame = GetSystemMetrics(SM_CXFRAME);
	int nMinMarg = nFrame+5;
	if (lpwndpos->y > rcWorkArea.bottom-nMinMarg)
		lpwndpos->y = rcWorkArea.bottom-nMinMarg;
	if (lpwndpos->y+cy < nMinMarg)
		lpwndpos->y = nMinMarg-cy;
	if (lpwndpos->x > rcWorkArea.right-nMinMarg)
		lpwndpos->x = rcWorkArea.right-nMinMarg;
	if (lpwndpos->x+cx < nMinMarg)
		lpwndpos->x = nMinMarg-cx;
}



//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************





//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
//	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateMDIWindowCmd)
//	ON_WM_NCACTIVATE()
//	ON_WM_ACTIVATE()
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
//from axform.h
#define WM_CAPTIONCHANGE	(WM_USER+776)


IMPLEMENT_DYNAMIC(CMainFrame, CBCGMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateMDIWindowCmd)
	ON_WM_NCACTIVATE()	
	ON_WM_ACTIVATE()
    ON_COMMAND_EX(ID_WINDOW_ARRANGE, OnMDIWindowCmd)
	ON_COMMAND(ID_WINDOW_NEW, OnWindowNew)
	ON_COMMAND_EX(ID_WINDOW_CASCADE, OnMDIWindowCmd)
	ON_COMMAND_EX(ID_WINDOW_TILE_HORZ, OnMDIWindowCmd)
	ON_COMMAND_EX(ID_WINDOW_TILE_VERT, OnMDIWindowCmd)	
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU, OnToolbarContextMenu)
	ON_REGISTERED_MESSAGE(BCGM_CUSTOMIZEHELP, OnHelpCustomizeToolbars)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(WM_FIND_APP, OnFindApp)

	ON_MESSAGE(WM_NOTIFYLONGOPERATION, OnLongNotify)
	ON_COMMAND_RANGE(ID_FIRST_DOCKBAR, ID_LAST_DOCKBAR, OnShowControlBar)
	ON_COMMAND_RANGE(ID_FIRST_TOOLBAR, ID_LAST_TOOLBAR, OnShowToolBar)
	ON_MESSAGE(WM_IDLEUPDATE, OnIdleUpdate)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutlookNotify)
	ON_COMMAND_RANGE(ID_OLE_VERB_MIN, ID_OLE_VERB_MAX, OnOleVerb)
	ON_MESSAGE(WM_SETSHEETBUTTON, OnSetSheetButton)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_MESSAGE(WM_CAPTIONCHANGE, OnFormCaptionChanged)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

static const IID IID_IMainFrameDisp =
{ 0x5e1f7e3, 0x38e6, 0x11d3, { 0xa6, 0x2, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };


BEGIN_INTERFACE_MAP(CMainFrame, CBCGMDIFrameWnd)
	INTERFACE_PART(CMainFrame, IID_IMainFrameDisp, Dispatch)
	INTERFACE_PART(CMainFrame, IID_IMainWindow, Main)
	INTERFACE_PART(CMainFrame, IID_IWindow, Wnd)
	INTERFACE_PART(CMainFrame, IID_IWindow2, Wnd2)
	INTERFACE_PART(CMainFrame, IID_INamedObject2, Name)
	INTERFACE_PART(CMainFrame, IID_IMenuInitializer, Toolbar)
	INTERFACE_PART(CMainFrame, IID_INumeredObject, Num)
	INTERFACE_PART(CMainFrame, IID_IUserInterface, UserInterface)
	INTERFACE_PART(CMainFrame, IID_ITranslatorWnd, TranslatorWnd)
	INTERFACE_PART(CMainFrame, IID_IOleFrame, OleFrame)
	INTERFACE_PART(CMainFrame, IID_IHelpInfo, Help)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CMainFrame, UserInterface)
IMPLEMENT_UNKNOWN(CMainFrame, TranslatorWnd)
IMPLEMENT_UNKNOWN(CMainFrame, OleFrame)

BEGIN_DISPATCH_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_DISPATCH_MAP(CMainFrame)
	DISP_PROPERTY_NOTIFY(CMainFrame, "Title", m_strTitle, OnTitleChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CMainFrame, "Label", m_strLabel, OnLabelChanged, VT_BSTR)
	DISP_PROPERTY_EX(CMainFrame, "ActiveFrame", GetActiveFrameDisp, SetActiveFrame, VT_DISPATCH)
	DISP_PROPERTY_EX(CMainFrame, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CMainFrame, "Maximized", GetMaximized, SetMaximized, VT_BOOL)
	DISP_PROPERTY_EX(CMainFrame, "Iconic", GetIconic, SetIconic, VT_BOOL)
	DISP_FUNCTION(CMainFrame, "SaveState", SaveState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "RestoreState", RestoreState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "SetWindowPos", SetWindowPos, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CMainFrame, "GetFramesCount", GetFramesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "GetFrame", GetFrame, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CMainFrame, "GetDesktopSize", GetDesktopSize, VT_EMPTY, VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CMainFrame, "GetWindowPos", GetWindowPos, VT_EMPTY, VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CMainFrame, "ResetUserButtonNames", ResetUserButtonNames, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "ShowSplash", ShowSplash, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "ShowXPBar", ShowXPBar, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CMainFrame, "AddToolbarPane", AddToolbarPane, VT_I4, VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPExpandPane", XPExpandPane, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPAddButton", XPAddButton, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPSetButton", XPSetButton, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPRemovePane", XPRemovePane, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPEnsureVisible", XPEnsureVisible, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMainFrame, "CreateImageList", CreateImageList, VT_I4, VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "DestroyImageList", DestroyImageList, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMainFrame, "DestroyAllImageLists", DestroyAllImageLists, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "XPSetPaneParam", XPSetPaneParam, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPGetPaneParam", XPGetPaneParam, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPGetFirstPane", XPGetFirstPane, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "XPGetNextPane", XPGetNextPane, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "XPSetImageList", XPSetImageList, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMainFrame, "LogFontSetBold", LogFontSetBold, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "LogFontSetFace", LogFontSetFace, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "LogFontSetColor", LogFontSetColor, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "LogFontSetHeight", LogFontSetHeight, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "RenderSetLayout", RenderSetLayout, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "RenderSetRect", RenderSetRect, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "RectCreate", RectCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "RectSetColor", RectSetColor, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "PageCreate", PageCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "TipCreate", TipCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "TipAddText", TipAddText, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "TipSetIcon", TipSetIcon, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "ImageCreate", ImageCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "ImageSetBitmap", ImageSetBitmap, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "ImageSetTransparent", ImageSetTransparent, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMainFrame, "TextCreate", TextCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "TextAddText", TextAddText, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "MenuCreate", MenuCreate, VT_I4, VTS_I4)
	DISP_FUNCTION(CMainFrame, "MenuAddItem", MenuAddItem, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "DeleteAllDrawing", DeleteAllDrawing, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "MDIClientUpdate", MDIClientUpdate, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "IdleUpdate", IdleUpdate, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "UpdateSystemSettings", UpdateSystemSettings, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "GetWindowLock", GetWindowLock, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "SetWindowLock", SetWindowLock, VT_EMPTY, VTS_BOOL VTS_BOOL)
	DISP_FUNCTION(CMainFrame, "XPCreateStatusItem", XPCreateStatusItem, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "XPSetStatusItem", XPSetStatusItem, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "XPRemoveStatusItem", XPRemoveStatusItem, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "XPSetDefaultStatusText", XPSetDefaultStatusText, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMainFrame, "UpdateSystemSettingsForCurrentDoc", UpdateSystemSettingsForCurrentDoc, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMainFrame, "UpdateSystemSettingsForDoc", UpdateSystemSettingsForDoc, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION(CMainFrame, "UpdateViewSettings", UpdateViewSettings, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CMainFrame, "ShowHiddenForm", ShowHiddenForm, VT_EMPTY, VTS_NONE )
	DISP_PROPERTY_EX(CMainFrame, "EnableAccelerators", GetEnableAccelerators, SetEnableAccelerators, VT_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CMainFrame, "ShowProgress", dispidShowProgress, ShowProgress, VT_EMPTY, VTS_BSTR VTS_I4)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pAM = 0;

	m_lposStatus = 0;
	m_lposProperties = 0;
	m_lposForm = 0;

	m_hImageListButtons = m_hImageListAction = 0;
	m_nXPWidth = 0;
	m_hwndXPBar = 0;
	m_pwndSplash = 0;
	m_bRestoreOnExit = false;
	EnableAutomation();

	m_bInSettingsMode = false;
	m_cySettingsWindowSize = 0;
	m_nOrigBottom = 0;

	m_sName = _T("MainWnd");

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	m_pActiveChildFrame = NULL;	
	m_nChildCount		= 0;
	m_bSDIMode			= FALSE;
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

	for( int i=0;i<OLE_MENU_SIZE;i++ )
		m_hOleMenu[i] = NULL;
	
	m_bInOleEditMode = false;
	m_bOleResizeFrame = false;

	m_guidOleObject = INVALID_KEY;

	m_hMenuMain		= 0;
	m_hMenuOleVerbs	= 0;

	m_bLockRecalc	= false;

	m_nTimerID		= -1;

	//script notify for LongOperation support
	m_nEnableScriptNotify = 0;
	m_lStep = -1;
	m_lLag = -1;
	m_nScriptSetPosLastPos = 0;
	m_dwScriptSetPosTime = 0;
	//_init_enable_script_notify( );

	m_bLockShow = false;
	m_bEnableAccelerators = TRUE;
}

CMainFrame::~CMainFrame()
{
	//if( m_pAM )
	//	m_pAM->Release();

	DestroyOleMenu();
	DestroyOleToolBarArray();

	HideSplash();
	//unlocked in CMainFrame::OnClose, 
	//unlocked in CWindowImpl::~CWindowImpl

	//locked in CWindowImpl::CWindowImpl() and here
	//theApp.m_pMainWnd = this;


	
	POSITION pos = 0;
	for( pos = m_arComboButtons.GetHeadPosition(); pos != NULL; )
	{		
		CComboBoxInfo* pInfo = (CComboBoxInfo*)m_arComboButtons.GetNext( pos );
		delete pInfo;
	}
	m_arComboButtons.RemoveAll( );
}


//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
void CMainFrame::AddChild( CShellFrame* pChildFrame )
{
	m_nChildCount++;
	m_children.AddTail((void*)pChildFrame);	
	//SetNewActiveChild( pChildFrame );

}

void CMainFrame::RemoveChild( CShellFrame* pChildFrame )
{
	m_nChildCount--;

	POSITION pos;
	
	if ((pos = m_children.Find(pChildFrame)) != NULL)
		m_children.RemoveAt(pos);
	else
	{
		TRACE("\nCan't find child frame position. Call from CMainFrame::RemoveChild");
	}

}


POSITION CMainFrame::GetFirstChildFramePos()
{ 
	return m_children.GetHeadPosition();
}

CShellFrame* CMainFrame::GetNextChildFrame( POSITION& pos )
{ 
	CShellFrame* pFrame = (CShellFrame*)m_children.GetNext(pos); 
	return pFrame;
}



void CMainFrame::SetNewActiveChild(CShellFrame* pNewChildFrame)
{
	static bool block = false;
	if( block )
		return;

	block = true;

	CShellFrame* poldActive = m_pActiveChildFrame;
	m_pActiveChildFrame = pNewChildFrame;

	if( m_pActiveChildFrame )
	{				
		m_pActiveChildFrame->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, 0L );
		m_pActiveChildFrame->TryActivateView();
	}
	else if( m_children.GetCount() == 1 )
	{
		POSITION pos = m_children.GetHeadPosition();
		while( pos != NULL)
		{
			CShellFrame* pChildFrame = (CShellFrame*)m_children.GetNext(pos);
			if( pChildFrame->GetFrameType() == ftMDI )
			{
				m_pActiveChildFrame = pChildFrame;
				MDIActivate( pChildFrame );
				break;
			}
		}
	}
	else
	{
		CMDIChildWnd *pActive = MDIGetActive(NULL);
		POSITION pos = m_children.GetHeadPosition();
		while( pos != NULL)
		{
			CShellFrame* pChildFrame = (CShellFrame*)m_children.GetNext(pos);
			if( pChildFrame->GetFrameType() == ftMDI  && pChildFrame == pActive )
			{
				m_pActiveChildFrame = pChildFrame;
				break;
			}
		}
	}

	block = false;
}


CFrameWnd* CMainFrame::GetActiveFrame()
{
	if( m_pActiveChildFrame )
	{
		CString str;
		m_pActiveChildFrame->GetWindowText( str );
		ASSERT( true );

	}
	if( VerifyChild( m_pActiveChildFrame ) )
		return (CFrameWnd*)m_pActiveChildFrame;

	
	return CMDIFrameWnd::GetActiveFrame();
}


void CMainFrame::OnUpdateMDIWindowCmd(CCmdUI* pCmdUI)
{
	//CMDIFrameWnd::OnUpdateMDIWindowCmd(pCmdUI);
	
	if( VerifyChild( m_pActiveChildFrame ) )
		pCmdUI->Enable(m_pActiveChildFrame != NULL);	
	
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	

	if( m_pActiveChildFrame )
	{
		if( m_pActiveChildFrame->GetFrameType() != ftMDI )
		{			
			if(  message == WM_NCACTIVATE || message == WM_ACTIVATE || message == WM_SETFOCUS )
			{
				return CWnd::DefWindowProc( message, wParam, lParam );					
			}
		}
	}
		
	return CBCGMDIFrameWnd::DefWindowProc( message, wParam, lParam );
		
}

BOOL CMainFrame::OnNcActivate(BOOL bActive) 
{
  
	BOOL bResult = CBCGMDIFrameWnd::OnNcActivate(bActive);
	/*

	if( bActive )
	{
		if( m_pActiveChildFrame )
		{	
			if( (m_pActiveChildFrame->GetFrameType() == ftDDI) &&
				(m_pActiveChildFrame->IsFloatingDDIFrame() )
				) 
			{
				//m_pActiveChildFrame->SetActive( TRUE );
			}
		}	
	}
	*/

	if( m_bInOleEditMode )
		OnOleFrameActivate( bActive );

	return bResult;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	//AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	BOOL bIn = FALSE;

	if( nState == WA_INACTIVE )
	{
		if( m_pActiveChildFrame )
		{
			//PDI 
			if( m_pActiveChildFrame->GetFrameType() == ftPDI )
				return;
		}
	}

	//paul 27.02.2003 COMMENT. What about activization with no childs????
	//if( !m_pActiveChildFrame || !m_pActiveChildFrame->IsWindowVisible() )
	//	return;

	CBCGMDIFrameWnd::OnActivate( nState, pWndOther, bMinimized );
}


BOOL CMainFrame::VerifyChild( CShellFrame* pChild)
{
	if( pChild )
	{
		if( ::IsWindow( pChild->GetSafeHwnd()) )
			return TRUE;
	}

	return FALSE;

}


CMDIChildWnd* CMainFrame::_GetActiveFrame( )
{
	if( VerifyChild( m_pActiveChildFrame ) )
		return (CMDIChildWnd*)m_pActiveChildFrame;
	else
		return NULL;
}

void CMainFrame::EnableDocking(DWORD dwDockStyle)
{
	CBCGMDIFrameWnd::EnableDocking(dwDockStyle);
	
	
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	CBCGMDIFrameWnd::m_pFloatingFrameClass = RUNTIME_CLASS(CVTMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwDockBarMap[i][1], dwDockBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
	
	
	
}

//extern char sz_app_title[];

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CFrameWnd::m_strTitle = m_strTitle;
	//strcpy( sz_app_title, m_strTitle );

	CMDIChildWnd* pActiveChild = NULL;

	if( m_pActiveChildFrame != NULL )
	{
		pActiveChild = m_pActiveChildFrame;
		if( !::IsWindow(pActiveChild->GetSafeHwnd() ) )
			pActiveChild = NULL;
	}

	CDocument* pDocument = NULL;
	pDocument = GetActiveDocument();

	if( pDocument != NULL )
	{
		if( pActiveChild )
		{
			pDocument = pActiveChild->GetActiveDocument();	
			if( !pDocument )pActiveChild = 0;
		}
	}

	if( bAddToTitle && (pActiveChild != NULL) && (pDocument != NULL) 
		&& ((pActiveChild->GetStyle() & WS_MAXIMIZE) == 0) )
	{
		UpdateFrameTitleForDocument(pDocument->GetTitle());
	}
	else
	{
		LPCTSTR lpstrTitle = NULL;
		CString strTitle;
		if (pActiveChild != NULL)
		{
			strTitle = pActiveChild->GetTitle();
			if (!strTitle.IsEmpty())
				lpstrTitle = strTitle;
		}
		if( lpstrTitle )
			UpdateFrameTitleForDocument(lpstrTitle);
		else
			SetWindowText( m_strTitle );

	}

}

void CMainFrame::OnUpdateFrameMenu(HMENU hMenuAlt)
{
	//CBCGMDIFrameWnd::OnUpdateFrameMenu(hMenuAlt);
	//CMDIFrameWnd::OnUpdateFrameMenu(hMenuAlt);
	//return;
	
	
	CMDIChildWnd* pActiveWnd = _GetActiveFrame();//MDIGetActive();
	if (pActiveWnd != NULL)
	{
		// let child update the menu bar
		pActiveWnd->OnUpdateFrameMenu(TRUE, pActiveWnd, hMenuAlt);
	}
	else
	{
		// no child active, so have to update it ourselves
		//  (we can't send it to a child window, since pActiveWnd is NULL)
		if (hMenuAlt == NULL)
			hMenuAlt = m_hMenuDefault;  // use default
		::SetMenu( GetSafeHwnd(), hMenuAlt );		
	}
	

}




BOOL CMainFrame::EnterSDIMode()
{

	if( !m_children.Find( (void*)m_pActiveChildFrame ) )
		return FALSE;


	if( m_pActiveChildFrame->GetFrameType() != ftMDI )
		return FALSE;


	POSITION pos = m_children.GetHeadPosition();
	while( pos != NULL)
	{
		CShellFrame* pChildFrame = (CShellFrame*)m_children.GetNext(pos);
		if( pChildFrame != NULL)
		{
			if( pChildFrame != m_pActiveChildFrame )
			{
				pChildFrame->ShowWindow( SW_HIDE );

				if( pChildFrame->GetFrameType() == ftDDI )
				{
					CWnd* pControlBar = pChildFrame->GetFrameDockBar();
					CWnd* pMiniFrame = pChildFrame->GetFrameDockBar()->GetMiniDockWnd();

					if( pControlBar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
						((CShellDockBar*)pControlBar)->m_bStoreVisibleState = pControlBar->IsWindowVisible()!=0;

					if( pControlBar )
						pControlBar->ShowWindow( SW_HIDE );

					if( pMiniFrame )
						pMiniFrame->ShowWindow( SW_HIDE );
				}

			}

		}
	}	

	m_pActiveChildFrame->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, 0L );		

	m_bRestoreOnExit = m_pActiveChildFrame->IsZoomed() == FALSE;
	m_pActiveChildFrame->ShowWindow( SW_SHOWMAXIMIZED );

	m_bSDIMode = TRUE;

	//RecalcLayout( );

	return TRUE;
}

BOOL CMainFrame::LeaveSDIMode()
{

	if( !m_bSDIMode )
		return FALSE;

	if( m_pActiveChildFrame == NULL )
		return FALSE;

	m_bSDIMode = FALSE;


	POSITION pos = m_children.GetHeadPosition();
	while( pos != NULL)
	{
		CShellFrame* pChildFrame = (CShellFrame*)m_children.GetNext(pos);
		if( pChildFrame != NULL)
		{
			if( pChildFrame != m_pActiveChildFrame )
			{
				if( pChildFrame && !( pChildFrame->GetStyle() & WS_MINIMIZE ) )
					pChildFrame->ShowWindow( SW_SHOWNOACTIVATE );
				else
					pChildFrame->ShowWindow( SW_SHOWNA  );
					

				if( pChildFrame->GetFrameType() == ftDDI )
				{
					CWnd* pControlBar = pChildFrame->GetFrameDockBar();
					CWnd* pMiniFrame = pChildFrame->GetFrameDockBar()->GetMiniDockWnd();


					bool bMakeVisible = false;

					if( pControlBar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
						bMakeVisible = !((CShellDockBar*)pControlBar)->m_bStoreVisibleState;

					if( bMakeVisible )continue;

					if( pMiniFrame )
						pMiniFrame->ShowWindow( SW_SHOWNOACTIVATE );

					if( pControlBar )
						pControlBar->ShowWindow( SW_SHOWNOACTIVATE );

				}

			}

		}
	}

	if( m_bRestoreOnExit )
		m_pActiveChildFrame->ShowWindow( SW_SHOWNORMAL );

	//RecalcLayout( );

	return TRUE;

}



BOOL CMainFrame::OnMDIWindowCmd(UINT nID)
{
	if( IsSDIMode() )
		return FALSE;

	return CMDIFrameWnd::OnMDIWindowCmd( nID );
}

void CMainFrame::OnWindowNew()
{
	if( IsSDIMode() )
		return;

	CMDIFrameWnd::OnWindowNew();

}

//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************



const char *szUserImageFileName = _T("toolbars.bmp");

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style &= ~WS_VISIBLE;
	//lock recalc
	bool bOldLockRecalc = GetLockRecalc();
	SetLockRecalc( true );

	//lock logo wnd
	CLogoWnd::Lock( 1 );

	IUnknown	*punkAM = 0;
	GetAppUnknown()->GetActionManager( &punkAM );
	ASSERT( punkAM != 0 );
	if( punkAM )
	{
		punkAM->QueryInterface( IID_IActionManager, (void**)&m_pAM );
		punkAM->Release();
	}
	ASSERT( m_pAM != 0 );


/********************** Added by Maxim ( internal toolbars ) ***************************************/				
	{
		int nDeveloper = ::GetValueInt( GetAppUnknown() , "\\Toolbars", "Developer", 0 );

		if( !nDeveloper )
			uiFirstUserToolBarId	= uiLastUserToolBarId - iMaxUserToolbars;
	}
/********************** Added by Maxim ( internal toolbars ) ***************************************/				

	g_pclassToolbarRuntime = RUNTIME_CLASS( CShellToolBar );
	g_pclassToolbarMenuButtonRuntime = RUNTIME_CLASS( CShellToolbarButton );
	g_pclassDockBarRuntime = RUNTIME_CLASS( CShellToolDockBar );

	CBCGSizingControlBar::SetCaptionStyle( true, true );

//	if( ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "EnableDragDropOpen", 1 ) != 0 )
		m_target.Register( this );

	g_TargetManager.RegisterTarget( GetControllingUnknown(), szTargetMainFrame );

	if (CBCGMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_strTitle = CFrameWnd::m_strTitle;

	//init OleAutomation variables
	m_strLabel = m_wndMDIClient.GetLabel();

	//create settings window firstly
	m_wndSettings.Create( this );

	CRect	rcSettings;
	m_wndSettings.GetWindowRect( rcSettings );
	m_cySettingsWindowSize = rcSettings.Height();
	m_wndSettings.ShowWindow( SW_HIDE );

	CString	strPathName = ::MakeAppPathName( szUserImageFileName );
	
/*	if (!m_UserImages.Load( strPathName ))
	{
		HBITMAP	hbmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_USERIMAGES), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADMAP3DCOLORS );
		m_UserImages.AddImage( hbmp );
		m_UserImages.SetUserFlag( true );
		m_UserImages.Save( strPathName );
	}*/

	//setup default menu from MenuManager
	::SetMenu( *this, g_CmdManager.GetMenu() );

	if (!m_wndMenuBar.Create (this, WS_CHILD | WS_VISIBLE | CBRS_TOP, uiMenuBarId ) )
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	//set main menu name
	{
		m_wndMenuBar.SetMenuName();
	}

	m_wndStatusBar.Create( this );

	bool bShowStatusBar = ::GetValueInt( ::GetAppUnknown(), "\\StatusBar", "ShowStatusBar", 1 ) != 0;
	ShowControlBar( &m_wndStatusBar, bShowStatusBar, false );

	

	INewStatusBarPtr	ptrStatusBar( ::StatusGetBar(), false );
	HWND		hwndStatus = ::GetWindow( ptrStatusBar );

	if( ptrStatusBar != 0 )
	{
		m_wndShellProgress.Create( AfxRegisterWndClass( 0, 0, 0, 0 ), "Progress", WS_CHILD|WS_VISIBLE, 
			CRect( 0, 0, 0, 0 ), CWnd::FromHandle( hwndStatus ), 101, 0 );
		ptrStatusBar->AddPane( guidPaneHelpProgress, -1, m_wndShellProgress, 0 );
		//ptrStatusBar->AddPane( guidPaneAction, GetValueInt( GetAppUnknown(), "\\View", "ActionPaneWidth", 115 ), 0, 0 );
	}

	DWORD	style = m_wndMenuBar.GetBarStyle();

//	style &= ~CBRS_BORDER_ANY;
	style |= CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	m_wndMenuBar.SetBarStyleEx( style );

	m_wndMenuBar.EnableDocking (CBRS_ALIGN_ANY);


	m_wndOutlook.Create( WS_CHILD|WS_VISIBLE|CBRS_LEFT, CRect( 0, 0, 100, 100 ), this, uiFirstDockToolBarId,
		CGfxOutBarCtrl::fDragItems|CGfxOutBarCtrl::fEditGroups|CGfxOutBarCtrl::fEditItems|CGfxOutBarCtrl::fRemoveGroups|
				CGfxOutBarCtrl::fRemoveItems|CGfxOutBarCtrl::fAddGroups|CGfxOutBarCtrl::fAnimation );
	m_wndOutlook.SetBarStyleEx(m_wndMenuBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndOutlook.EnableDocking( CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT );

	m_wndXPBarHolder.Create(NULL, NULL, WS_CHILD|CBRS_ALIGN_LEFT, CRect(0,0,100,100), this,
		uiXPHolderId);
	m_wndXPBarHolder.SetBarStyle(CBRS_ALIGN_LEFT|CBRS_SIZE_DYNAMIC);
	m_wndXPBarHolder.EnableDocking(CBRS_ALIGN_LEFT);


	CString	strTitle = ::LanguageLoadCString( IDS_OUTLOOK_BAR );
	//strTitle.LoadString( IDS_OUTLOOK_BAR );
	m_wndOutlook.SetWindowText( strTitle );

	m_ptrDockWindows.AddTail( &m_wndOutlook );

	
	


	CPtrArray	m_tbs;
	int			i;
//add our images to toolbar
	CBCGToolBar::GetImages()->SetTransparentColor( GetSysColor( COLOR_3DFACE ) );

	
	for( i = 0; i < g_CmdManager.GetBitmapCount(); i++ )
	{
		CCommandManager::BitmapData	*pBitmapData = g_CmdManager.GetBitmapData( i );
		pBitmapData->lImagesOffset = CBCGToolBar::GetImages()->GetCount();
		CBCGToolBar::GetImages()->AddImage( pBitmapData->hBitmap );
	}
//init user toolbars
	CString strControlBarRegEntry = "Software\\VideoTest\\vt5\\dummies";

	InitUserToobars( strControlBarRegEntry,
				uiFirstUserToolBarId,
				uiLastUserToolBarId );

	
//construct user toolbars using pBitmapData
	for( i = 0; i < g_CmdManager.GetDockWindowsCount(); i++ )
	{
		IUnknown *punkDockWindow = g_CmdManager.GetDockWindow( i );
		bool bShowCaption = true;
		bool bFloat = true;

		INamedObject2Ptr ptrNamed(punkDockWindow);
		if(ptrNamed!=0)
		{
			BSTR bstrName = 0;
			CString strName = "Default";
			if (SUCCEEDED(ptrNamed->GetName(&bstrName)))
				strName = bstrName;
			bShowCaption = GetValueInt( GetAppUnknown(), "\\MainFrame\\ShowCaption", strName, 1 ) != 0;
			//bFloat = GetValueInt( GetAppUnknown(), "\\MainFrame\\Float", strName, 1 ) != 0;
		}
		_CreateDockBar( punkDockWindow , uiFirstDockToolBarId+i+1, 0, bFloat, bShowCaption );
		/*CShellDockBar	*pbar = new CShellDockBar( g_CmdManager.GetDockWindow( i ) );
//		pbar->m_bAutoDelete = true;

		BOOL bFixed = FALSE;
		DWORD dwSide = 0;
		DWORD dwStyle = CBRS_ALIGN_ANY;
		sptrIDockableWindow sptrDock(g_CmdManager.GetDockWindow( i ));

		CSize	sizeDefault = pbar->GetDefSize();//CSize( 100, 100 );
		pbar->Create( this, IDD_DUMMY, WS_CHILD|WS_VISIBLE, uiFirstDockToolBarId+i+1 );
		//pbar->Create( "qqq", this, sizeDefault, true, uiFirstDockToolBarId+i );

		CSize sizeActual(100, 100);
		if(sptrDock != 0)
		{
			sptrDock->GetDockSide(&dwSide);
			sptrDock->GetFixed(&bFixed);
			sptrDock->GetSize(&sizeActual, FALSE);
		}

		pbar->SetWindowPos(0, 0, 0, sizeActual.cx, sizeActual.cy, SWP_NOMOVE|SWP_NOZORDER);

		switch(dwSide)
		{
		case AFX_IDW_DOCKBAR_TOP:
			dwStyle = CBRS_ALIGN_TOP;
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			dwStyle = CBRS_ALIGN_BOTTOM;
			break;
		case AFX_IDW_DOCKBAR_LEFT:
			dwStyle = CBRS_ALIGN_LEFT;
			break;
		case AFX_IDW_DOCKBAR_RIGHT:
			dwStyle = CBRS_ALIGN_RIGHT;
			break;
		default:
			dwStyle = CBRS_ALIGN_ANY;
		}
		pbar->SetBarStyleEx (pbar->GetBarStyle () |
			CBRS_TOOLTIPS | CBRS_FLYBY | ((bFixed==TRUE) ? CBRS_SIZE_FIXED : CBRS_SIZE_DYNAMIC));
		pbar->EnableDocking( CBRS_ALIGN_LEFT|CBRS_ALIGN_BOTTOM|CBRS_ALIGN_RIGHT );
		
		m_ptrDockWindows.AddTail( pbar );*/
	}
	
	
	EnableDocking( CBRS_ALIGN_ANY );

#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME



	for( i = 0; i < g_CmdManager.GetToolBarCount(); i++ )
	{
		CCommandManager::ToolBarData *pToolBarData = g_CmdManager.GetToolBarData( i );

		CShellToolBar	*ptoolbar = (CShellToolBar	*)CreateNewToolbar( pToolBarData->m_sGroupName );

		if( !ptoolbar )
			continue;

		
		for( int iCmdIdx = 0; iCmdIdx < pToolBarData->GetSize(); iCmdIdx++ )
		{
			CCommandManager::BitmapData	*pBitmapData = g_CmdManager.GetBitmapData( pToolBarData->m_arrBmps[iCmdIdx] );

			if( pBitmapData )
			{
				ptoolbar->InsertButtonStoreImage( CShellToolbarButton(
						pToolBarData->m_arrIDs[iCmdIdx],	//command ID
						pToolBarData->m_arrPicts[iCmdIdx]+pBitmapData->lImagesOffset,			//picture index
						pToolBarData->m_arrStrs[iCmdIdx], FALSE ) );

			}
			else
				ptoolbar->InsertButtonStoreImage( CShellToolbarButton(
						pToolBarData->m_arrIDs[iCmdIdx],	//command ID
						/*pToolBarData->m_arrPicts[iCmdIdx]+pBitmapData->lImagesOffset,			//picture index*/
						-1,
						pToolBarData->m_arrStrs[iCmdIdx], FALSE ) );
		}

		
		//ptoolbar->m_bAutoDelete = true;

		m_tbs.Add( ptoolbar );
	}

	//DockControlBar( &m_wndMenuBar );
	DockControlBar( &m_wndOutlook, AFX_IDW_DOCKBAR_LEFT );

	BOOL bEnableVert = GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableDockToolbarsVertically", TRUE);
	CBCGToolBar	*pold = 0;

	for( i = 0; i < m_tbs.GetSize(); i++ )
	{
		CBCGToolBar	*ptoolbar = (CBCGToolBar	*)m_tbs[i];

		if( !pold )
			DockControlBar( ptoolbar, AFX_IDW_DOCKBAR_TOP );
		else
		{
			RecalcLayout();

			CRect	rc;
			pold->GetWindowRect( &rc );

			CPoint ptOffset = CPoint( rc.right, rc.top );

			ptoolbar->GetWindowRect( &rc );

			rc = CRect( ptOffset, rc.Size() );

			DockControlBar( ptoolbar, AFX_IDW_DOCKBAR_TOP, rc );

		}
		if (!bEnableVert)
			ptoolbar->EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
		pold = ptoolbar;
	}

	POSITION pos = GetFirstDockBarPos();

	CControlBar	*poldBar[4]; 
	long	lCounts[4];

	ZeroMemory( poldBar, sizeof( poldBar ) );
	ZeroMemory( lCounts, sizeof( lCounts ) );
	

	while( pos )
	{
		CControlBar		*pcbar = GetNextDockBar( pos );
		if( pcbar == NULL || !pcbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
			continue;
		CShellDockBar	*pbar = (CShellDockBar*)pcbar;

		long	lSide = pbar->GetDefDockSide();
		long	lDockIndex = lSide-AFX_IDW_DOCKBAR_TOP;
		CRect	rcDock;

		if( !poldBar[lDockIndex] )
			DockControlBar( pbar, lSide );
		else
		{
			poldBar[lDockIndex]->GetWindowRect( &rcDock );
			if( lSide ==  AFX_IDW_DOCKBAR_BOTTOM || lSide ==  AFX_IDW_DOCKBAR_TOP )
				rcDock.right = rcDock.right-rcDock.Width()/lCounts[lDockIndex];
			else
				rcDock.bottom = rcDock.bottom-rcDock.Height()/lCounts[lDockIndex];

			DockControlBar( pbar, lSide, rcDock );
		}

		lCounts[lDockIndex]++;
		poldBar[lDockIndex] = pbar;
	}

	pos = m_ptrDockWindows.GetHeadPosition();
	while( pos )
		((CShellDockBar	*)m_ptrDockWindows.GetNext( pos ))->InitDockSizes();

	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "Menu", 1, &m_wndMenuBar);
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "Status", 1, &m_wndStatusBar);
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "TopDockBar", 2, GetControlBar(AFX_IDW_DOCKBAR_TOP));
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "BottomDockBar", 2, GetControlBar(AFX_IDW_DOCKBAR_BOTTOM));
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "Outlook", 3, &m_wndOutlook);
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "XPBar", 3, &m_wndXPBarHolder);
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "LeftDockBar", 3, GetControlBar(AFX_IDW_DOCKBAR_LEFT));
	m_PrioritiesMgr.ReadPriority(DOCKBARPRIORSEC, "RightDockBar", 3, GetControlBar(AFX_IDW_DOCKBAR_RIGHT));


	SetLockRecalc( false );
	RecalcLayout();
	SetLockRecalc( bOldLockRecalc );


	//place all bar to single line
	for( i = 0; i < g_CmdManager.GetMenuCount(); i++ )
	{
		CCommandManager::MenuData *pdata = g_CmdManager.GetMenuData( i );

		if( pdata->bDocumentMenu )
			continue;
		if( pdata->strMenuName == "main" )
			continue;

		m_ContextMenuManager.AddMenu( pdata->strMenuName, pdata->idx, pdata->hMenu );
	}

	CBCGToolBar::EnableQuickCustomization ();

	CLogoWnd::Lock( 0 );	

	m_nTimerID = SetTimer( m_nTimerID, 500, NULL );

	_init_script_notify( );

	if( GetStealthKeysNumber() && StealthIndexIsAvailable( 30 ) )
	{
		
		RECT rc = { 0, 0, 300, 50 };
		m_DistrWnd.create( WS_BORDER | WS_POPUP | WS_VISIBLE, rc, 0, GetSafeHwnd() );
		m_DistrWnd.m_strDecr = ::LanguageLoadCString( IDS_STR_DISTR_VER );
	}

	return 0;
}

bool CMainFrame::IsVerticalDockEnabled()
{
	BOOL bEnableVert = GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableDockToolbarsVertically", TRUE);
	return bEnableVert?true:false;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	//CBCGMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LRESULT CMainFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
	if( GetValueInt( GetAppUnknown(), "\\MainFrame", "ShowPopupMenu", 1 ) == 0 )
		return 0;
	CPoint point (LOWORD (lp), HIWORD(lp));

	// get command Manager
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

	ASSERT(punk != NULL);
	if (!punk)
		return 1;

	sptrICommandManager2 sptrCmdMgr(punk);
	punk->Release();


	UINT uCMD;
	_bstr_t	bstrMenuName = _T("Toolbars");

	// register context menu initializer
	sptrCmdMgr->RegisterMenuCreator((IUnknown*)&(this->m_xToolbar));

	// execute context menu (return value is action's ID which selected by user)
	sptrCmdMgr->ExecuteContextMenu3(bstrMenuName, point, TPM_RETURNCMD, &uCMD);

	sptrCmdMgr->UnRegisterMenuCreator((IUnknown*)&(this->m_xToolbar));

	SendMessage( WM_COMMAND, uCMD );

		
	return 0;
}

LRESULT CMainFrame::OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp)
{
	int iPageNum = (int) wp;

	CBCGToolbarCustomize* pDlg = (CBCGToolbarCustomize*) lp;
	ASSERT_VALID (pDlg);

	CString	strTopic;

	if( iPageNum < 0 || iPageNum > 3 )
		return 0;

	char *ppszTopics[] = {"CustomizeCommands",
		"CustomizeToolbars",
		"CustomizeKeyboard",
		"CustomizeMenu"};

	::HelpDisplayTopic( "shell", 0, ppszTopics[iPageNum], "$GLOBAL_main" );

	return 0;
}

LRESULT CMainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	CString	strLastState = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );
	g_CmdManager.LoadState(  "default.state" );
	::SetValue( ::GetAppUnknown(), "General", "CurrentState", strLastState );
	return 0;
}

void FireScriptEvent( const char* psz_event );

//**********************************************************************************
void CMainFrame::OnClose() 
{
	IApplication	*pApp = GetAppUnknown();
	if( pApp->IsNestedMessageLoop() == S_OK )
		return;

	// [vanek] 24.02.2004 - не закрываем, если происходит выполнение или запись макропоследовательности
	IUnknown		*punk_macroman = 0;
	punk_macroman = _GetOtherComponent( GetAppUnknown(), IID_IMacroManager );
	if( punk_macroman )
	{
		IMacroManagerPtr	sptr_macroman = punk_macroman;
		punk_macroman->Release();
		punk_macroman = 0;
        if( sptr_macroman )
		{
			BOOL	bscript_rec = FALSE,
					bscript_exec = FALSE;

			sptr_macroman->IsRecording( &bscript_rec );
			sptr_macroman->IsExecuting( &bscript_exec );
			if( bscript_rec || bscript_exec )
				return;
		}		
	}

	if( IsSDIMode() )
		return;
	if( CBCGToolBar::IsCustomizeMode() )
		return;

	if( m_bInOleEditMode )
		OnDeActivateOleObject( false );

	// AM SBT1324. Если методика пишется или выполняется, то не выходим.
	IUnknownPtr punkMtdMan(_GetOtherComponent( GetAppUnknown(), IID_IMethodMan2 ), false);
	IMethodMan2Ptr	sptrMtdMan = punkMtdMan;
	if (sptrMtdMan != 0)
	{
		BOOL bRecording,bRunning;
		if (SUCCEEDED(sptrMtdMan->IsRecording(&bRecording)) && bRecording)
		{
			AfxMessageBox(IDS_METHODIC_RECORDING, MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		if (SUCCEEDED(sptrMtdMan->IsRunning(&bRunning)) && bRunning)
		{
			AfxMessageBox(IDS_METHODIC_RUNNING, MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}

	CShellDocManager	*pman = theApp.GetDocManager();
	if( pman && !pman->SaveAllModified() )
		return;

	// [vanek]: reset active method - 19.05.2004
	IUnknown		*punk_mtd_man = 0;
	punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan2 );
	if( punk_mtd_man )
	{
		IMethodMan2Ptr	sptr_mtd_man = punk_mtd_man;
		punk_mtd_man->Release();
		punk_mtd_man = 0;
        if( sptr_mtd_man )
		{ // получить и записать в shell.data имя активной методики, а потом сбросить ее lPos
			TPOS lActiveMethodPos = 0;
			sptr_mtd_man->GetActiveMethodPos(&lActiveMethodPos);
			IUnknownPtr ptrActiveMethod;
			sptr_mtd_man->GetNextMethod(&lActiveMethodPos, &ptrActiveMethod);
			IMethodPtr sptrActiveMethod(ptrActiveMethod);
			_bstr_t bstrActive;
			if(sptrActiveMethod)
				sptrActiveMethod->GetName(bstrActive.GetAddress());
			else
				bstrActive="";
			::SetValue(::GetAppUnknown(),"\\Methodics","ActiveMethod", (char *)(bstrActive));
			sptr_mtd_man->LockNotification();
			sptr_mtd_man->SetActiveMethodPos( 0 );
			sptr_mtd_man->UnlockNotification();
		}
	}


	//paul 01.08.2003
	//fire app start
	FireScriptEvent( "Application_OnExit" );

	//paul 6.08.2003. Move down to save users toolbars
	//_KillDockBar();

	CString	strLastStateFileName = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );

	if( GetValueInt( ::GetAppUnknown(), "\\General", "SaveStateOnExit", 1 ) == 1 )
	{
		::ExecuteAction( "ToolsSaveState", 
			CString( "\"" )+strLastStateFileName+CString( "\"" ), 
			aefNoShowInterfaceAnyway );

		// [vanek] : store main frame's state - 15.10.2004
		::ExecuteScript( _bstr_t( _T("MainWnd.SaveState(\"\")") ), "CMainFrame::OnClose" );
	}

	CString	strPathName = ::MakeAppPathName( szUserImageFileName );

	if( m_pAM )
		m_pAM->TerminateInteractiveAction();

	//Execute AutoExec script
	g_script.ExecuteAppScript( ENTRY_AUTOEXIT );
	//g_script.DeInit();

	_KillDockBar();
	
	for( int i = 0; i < theApp.GetComponentCount(); i++ )
	{
		IUnknown *punk = theApp.GetComponent( i, false );

		if( CheckInterface( punk, IID_IMainFrameDisp ) )
			theApp.RemoveComponent( i );
	}


	BOOL b = AfxOleGetUserCtrl();
	if( !b )
		AfxOleSetUserCtrl( TRUE );


	if( m_pAM )
		m_pAM->Release();	m_pAM = 0;

	theApp.FreeComponents();

	if( !b )
		AfxOleSetUserCtrl( FALSE );	

	IScriptSitePtr sptrScriptSite = pApp;
	sptrScriptSite->UnregisterAll();

	{
		//sptrIStatusBar	sptrStatus(m_wndStatusBar.GetControllingUnknown());
		//ASSERT(sptrStatus != 0);
		//sptrStatus->ExcludePane(m_nPaneNum);
		//if(m_pLeftPane)delete m_pLeftPane;
		//m_pLeftPane = 0;
	}

	//m_wndStatusBar.m_manPanes.FreeComponents();


	//GetControllingUnknown()->Release();
//	AfxOleSetUserCtrl( FALSE );
	m_DistrWnd.m_bRecreateOnDistr = false;

	DestroyWindow();

	//CBCGMDIFrameWnd::OnClose();
}	

	
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	
	if( nID >= ID_CMD_POPUP_MIN && nID <= ID_CMD_POPUP_MAX )
	{
		if (nCode == CN_UPDATE_COMMAND_UI)
			g_CmdManager.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		return true;
	}

	//add routing to command manager
	if( g_CmdManager.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
	{
		return true;
	}

	
//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	if( VerifyChild( m_pActiveChildFrame ) )	
		if( m_pActiveChildFrame->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo)  )
			return TRUE;
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


	return CBCGMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/*BOOL CMainFrame::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	if (theApp.m_bDialog)
	{
		if(message == WM_LBUTTONDOWN||message == WM_ACTIVATEAPP)
			return TRUE;
	}

	if( message != WM_ENTERIDLE &&
		message != WM_SETCURSOR )
	{
		TRACE( "CMainFrame::OnWndMsg %x\n", message  );
	}
	return CBCGMDIFrameWnd::OnWndMsg(message, wParam, lParam, pResult);
}
*/

#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_CHAR ||
		pMsg->message == WM_SYSKEYDOWN ||
		pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN ||
		pMsg->message == WM_MBUTTONDOWN )
	{
		HideSplash();
	}

	CheckActivePopup( pMsg );

	//paul 04.08.2003
	static long lkbd_switch = -1;
	if( lkbd_switch == -1 )
	{
		lkbd_switch = ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "AllowKeyboardSwitch", 1 );
	}
	if( lkbd_switch == 0 && ( pMsg->message == WM_KEYDOWN && IsCTRLpressed() && 
		( pMsg->wParam == VK_TAB || pMsg->wParam == VK_F6 ) ) )
		return true;


	LRESULT lRes = 0;
	if( ProcessMessage( pMsg, &lRes ) )
	{
		return true;
	}

	if( pMsg->message >= WM_KEYFIRST &&
		pMsg->message <= WM_KEYLAST )
	{
		if (!m_bEnableAccelerators)
			return FALSE;
		CFrameWnd	*pwndFrame = GetActiveChildFrame();
		if( pwndFrame && pwndFrame->PreTranslateMessage(pMsg) )
		{
			return true;
		}
	}
	else
	{
		if( VerifyChild( m_pActiveChildFrame ) )	
			if( m_pActiveChildFrame->PreTranslateMessage(pMsg) )
			{
				return TRUE;
			}
	}

	// A.M. BT 2686
	if (pMsg->message == WM_SYSKEYUP && pMsg->wParam == VK_MENU)
	{
		IUnknownPtr punkApp = GetAppUnknown(false);
		IApplicationPtr pApp(punkApp);
		if (pApp != 0)
		{
			IUnknownPtr punkAM;
			pApp->GetActionManager(&punkAM);
			IActionManagerPtr sptrAM(punkAM);
			if (sptrAM != 0)
			{
				DWORD dwFlags;
				if (SUCCEEDED(sptrAM->GetCurrentExecuteFlags(&dwFlags)) && (dwFlags&aefInteractiveRunning))
					return CMDIFrameWnd::PreTranslateMessage(pMsg);
			}
		}
	}
	// end of A.M. fix
	BOOL bReturn = CBCGMDIFrameWnd::PreTranslateMessage(pMsg);
	return bReturn;
}

void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	if( !g_CmdManager.GetMessageString( nID, rMessage ) )
	{
		if( nID == 57345 )
		{
			IUnknown	*punkInter = 0;
			if( m_pAM )
				m_pAM->GetRunningInteractiveActionInfo( &punkInter );

			if( punkInter )
			{
				IActionInfoPtr	ptrAI( punkInter );
				punkInter->Release();
				BSTR	bstrHelp=  0;
				ptrAI->GetCommandInfo( 0, 0, &bstrHelp, 0 );
				rMessage = bstrHelp;
				::SysFreeString( bstrHelp );

				int	idx = rMessage.Find( '\n' );
				if( idx != -1 )rMessage = rMessage.Left( idx );
				return;
			}

			if( !m_strDefText.IsEmpty() )
			{
				rMessage = m_strDefText;
				return;
			}

		}
		CBCGMDIFrameWnd::GetMessageString( nID, rMessage );
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//IMenuInitializer implementation
IMPLEMENT_UNKNOWN(CMainFrame, Toolbar)

HRESULT CMainFrame::XToolbar::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	_try_nested(CMainFrame, Toolbar, OnInitPopup)
	{
		if (!hMenu)
			return E_INVALIDARG;

		*phOutMenu = ::CopyMenu(hMenu);
		if (!*phOutMenu)
			return E_INVALIDARG;
		// get menu
		CMenu* pmenu = CMenu::FromHandle(*phOutMenu);
		if (!pmenu)
			return E_FAIL;

		// get command Manager
		IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

		ASSERT(punk != NULL);
		if (!punk)
			return E_FAIL;

		sptrICommandManager2 sptrCmdMgr(punk);
		punk->Release();
		

		CControlBar* pDock;
		BSTR bstrName = 0;

		
		POSITION pos = pThis->GetFirstDockBarPos();
		int nMenuIdx = 0;
//modify controlbars items
		int ndock_bar_idx = 0;
		while(pos)
		{
			CString	strName;
			pDock = pThis->GetNextDockBar(pos);

			if( !pDock )
				continue;
			if( !pDock->IsKindOf( RUNTIME_CLASS(CShellDockBar ) ) )
				continue;
			//BSTR	bstr;
			//((CShellDockBar*)pDock)->GetObjectName( &bstr );
			pDock->GetWindowText( strName );
			//strName = bstr;
			//::SysFreeString( bstr );

			// [vanek] : наличие/отсутсвие соответсвующего пункта контекстном меню для докбара - 16.11.2004
			if( GetValueInt( GetAppUnknown(), "\\MainFrame\\DockBarsInContextMenu", ((CShellDockBar*)pDock)->GetName(), 1) != 0  )
			{
				pmenu->InsertMenu( nMenuIdx, MF_STRING|MF_BYPOSITION| (pDock->IsWindowVisible()? MF_CHECKED : MF_UNCHECKED), 
					/*ID_FIRST_DOCKBAR+nMenuIdx+1*/ID_FIRST_DOCKBAR+ndock_bar_idx+1, strName );

				nMenuIdx++;
			}
            
			ndock_bar_idx ++;
		}

//insert separator if it's not exist
		if( GetValueInt( GetAppUnknown(), "\\Interface", "ShowOutlookBar", 1 ) )
		{
			if (nMenuIdx != 0)
			{
				if (!((pmenu->GetMenuState(nMenuIdx, MF_BYPOSITION) & MF_SEPARATOR) == MF_SEPARATOR))
					pmenu->InsertMenu(nMenuIdx, MF_BYPOSITION | MF_SEPARATOR);
				nMenuIdx++;
			}


			CString	strName = ::LanguageLoadCString( IDS_OUTLOOK_BAR );
			//strName.LoadString( IDS_OUTLOOK_BAR );
			pmenu->InsertMenu( nMenuIdx, MF_STRING|MF_BYPOSITION | (pThis->m_wndOutlook.IsWindowVisible()? MF_CHECKED : MF_UNCHECKED), 
					ID_FIRST_DOCKBAR, strName );
			nMenuIdx++;
		}

		if (nMenuIdx != 0)
		{
			if (!((pmenu->GetMenuState(nMenuIdx, MF_BYPOSITION) & MF_SEPARATOR) == MF_SEPARATOR))
				pmenu->InsertMenu(nMenuIdx, MF_BYPOSITION | MF_SEPARATOR);
			nMenuIdx++;
		}
//modify toolbars items
		int	nToolBar = 1;
		for( pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);

			if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
				continue;
			if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
				continue;

//extract tb data
			CString	strName;
			pToolBar->GetWindowText( strName );


			pmenu->InsertMenu( nMenuIdx, MF_STRING|MF_BYPOSITION | (pToolBar->IsWindowVisible()? MF_CHECKED : MF_UNCHECKED), 
				nToolBar+ID_FIRST_TOOLBAR, strName );
			nMenuIdx++;
			nToolBar++;
		}
//insert separator if it's not exist
		if (nMenuIdx != 0)
		{
			if (!((pmenu->GetMenuState(nMenuIdx, MF_BYPOSITION) & MF_SEPARATOR) == MF_SEPARATOR))
				pmenu->InsertMenu(nMenuIdx, MF_BYPOSITION | MF_SEPARATOR);
			nMenuIdx++;
		}


		*phOutMenu = pmenu->Detach();

		return S_OK;
	}
	_catch_nested;
}

void CMainFrame::UpdateAllowCustomizeButton()
{
	long lAllowCustomize = ::GetValueInt( GetAppUnknown(), "\\MainFrame", "EnableCustomizeButton", 1L );
	if (lAllowCustomize != g_lAllowCustomize)
	{
		g_lAllowCustomize = lAllowCustomize;
		CString	strCustomize = ::LanguageLoadCString( IDS_CUSTOMIZE );
		//strCustomize.LoadString( IDS_CUSTOMIZE );
		int	nCustomizeCommand = g_CmdManager.GetActionCommand( "ToolsCustomize" );
		for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
			if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
				continue;
			if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
				continue;
			pToolBar->EnableCustomizeButton(g_lAllowCustomize == 1, nCustomizeCommand, strCustomize);
		}
		RecalcLayout();
	}
}

bool CMainFrame::GetEnableChangeMenu(LPCTSTR lpstrMenuName)
{
	if (lpstrMenuName == NULL || lpstrMenuName[0] == 0)
		return true;
	BOOL bEnable = ::GetValueInt(::GetAppUnknown(), "\\MainFrame\\EnableChangeMenu", lpstrMenuName, TRUE);
	return bEnable?true:false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//IMainWindow implementation
IMPLEMENT_UNKNOWN(CMainFrame, Main)

HRESULT CMainFrame::XMain::IsHelpMode( BOOL *pbool )
{
	_try_nested(CMainFrame, Main, CreateDocBar)
	{
		if( !pbool )
			return S_FALSE;

		*pbool = pThis->m_bHelpMode;

		return S_OK;
	}
	_catch_nested;
	return S_FALSE;
}

HRESULT CMainFrame::XMain::ShowControlBar( HWND hwnd, long bShow )
{
	METHOD_PROLOGUE_EX(CMainFrame, Main);
	CWnd	*p = CWnd::FromHandlePermanent( ::GetParent( hwnd ) );
	if( !p )return E_INVALIDARG;
	if( !p->IsKindOf( RUNTIME_CLASS(CControlBar) ) )return E_INVALIDARG;
	pThis->ShowControlBar( (CControlBar*)p, bShow, false );

	return S_OK;
}

HRESULT CMainFrame::XMain::GetFirstDockWndPosition( POSITION *plPosition )
{
	METHOD_PROLOGUE_EX(CMainFrame, Main)

	*plPosition = pThis->GetFirstDockBarPos();
	return S_OK;
}
HRESULT CMainFrame::XMain::GetNextDockWnd(IUnknown **ppunkDock, POSITION *plPosition)
{
	METHOD_PROLOGUE_EX(CMainFrame, Main)

	*ppunkDock  = 0;

	POSITION	pos = (POSITION)*plPosition;
	CControlBar		*pcbar = pThis->GetNextDockBar( pos );
	if( pcbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
	{
		CShellDockBar	*pbar = (CShellDockBar*)pcbar;
		*ppunkDock = pbar->GetClientUnknown();
	}
	*plPosition = pos;

	if( *ppunkDock )
		(*ppunkDock)->AddRef();

	return S_OK;
}

HRESULT CMainFrame::XMain::GetFirstToolbarPosition(POSITION *plPosition)
{
	METHOD_PROLOGUE_EX(CMainFrame, Main)
	*plPosition = gAllToolbars.GetHeadPosition();
	return S_OK;
}
HRESULT CMainFrame::XMain::GetNextToolbar(HWND *phwnd, POSITION *plPosition)
{
	METHOD_PROLOGUE_EX(CMainFrame, Main)
	POSITION	pos = *plPosition;
	CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext( pos );
	*phwnd = pToolBar->GetSafeHwnd();
	*plPosition = pos;
	return S_OK;
}

HRESULT CMainFrame::XMain::CreateDockBarEx(IUnknown* punk, UINT nID, HWND* pHWND, DWORD dwFlags/*1 - not float now*/)
{
	_try_nested(CMainFrame, Main, CreateDocBar)
	{
		pThis->_CreateDockBar( punk, nID, pHWND, (dwFlags&1)==1, (dwFlags&2)==0 );
		return S_OK;
	}
	_catch_nested;

}

/*HRESULT CMainFrame::XMain::ResetPagesList()
{
	_try_nested(CMainFrame, Main, ResetPagesList)
	{
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMainFrame::XMain::AddSettingsPage( IUnknown *punk )
{
	_try_nested(CMainFrame, Main, AddSettingsPage)
	{
		return S_OK;
	}
	_catch_nested;
}
*/

HRESULT CMainFrame::XMain::EnterSettingsMode(HWND* hwndSettings, LONG nHeight)
{
	_try_nested(CMainFrame, Main, EnterSettingsMode)
	{
		pThis->EnterMode(nHeight);
		if(hwndSettings)
		{
			*hwndSettings = pThis->m_wndSettings;
		}
			
		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XMain::LeaveSettingsMode()
{
	_try_nested(CMainFrame, Main, EnterSettingsMode)
	{
		pThis->LeaveMode();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CMainFrame::XMain::CreateDockBar(IUnknown* punk, UINT nID, HWND* pHWND)
{
	_try_nested(CMainFrame, Main, CreateDocBar)
	{
		pThis->_CreateDockBar(punk, nID, pHWND);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XMain::KillDockBar(BSTR bstrName)
{
	_try_nested(CMainFrame, Main, KillDockBar)
	{
		pThis->_KillDockBar(bstrName);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XMain::GetMDIClientWnd(HWND* pHwnd)
{
	_try_nested(CMainFrame, Main, GetMDIClientWnd)
	{
		if(pHwnd)
			*pHwnd = pThis->m_wndMDIClient.GetSafeHwnd();
		return S_OK;
	}
	_catch_nested;
}



void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	if( IsIconic() ) return;
	// AAM: 28.11.2006 BT 5314
	// при скрытом окне в RecalcLayout оказывалось, что окно с документом
	// нулевого размера - и вьюха дизаблилась, документ убирался из списка таргетов TargetMan

	if( m_bLockRecalc )
		return;

	m_PrioritiesMgr.Reposition();
	CBCGMDIFrameWnd::RecalcLayout(bNotify);
	m_wndXPBarHolder.SetXPBarPos();

	CRect	rcMainFrame;
	m_wndMDIClient.GetWindowRect( rcMainFrame );
	ScreenToClient( rcMainFrame );

/*	if( m_hwndXPBar )
	{
		CRect	rectXP = rcMainFrame;
		rectXP.right = rcMainFrame.left;
		rectXP.left = rcMainFrame.left-m_nXPWidth;
		
		::MoveWindow( m_hwndXPBar, rectXP.left, rectXP.top, rectXP.Width(), rectXP.Height(), true );
	}
	else */if( m_hwndXPBar == NULL && m_bInSettingsMode )
	{

//		rcMainFrame.bottom-=m_rectBorder.bottom;
//		rcMainFrame.top+=m_rectBorder.top;
//		rcMainFrame.right-=m_rectBorder.right;
		rcMainFrame.left;

		if(m_wndSettings.IsWindowVisible())
		{
			CRect	rcSettings;
			m_wndSettings.GetWindowRect( rcSettings );

			rcSettings.bottom = m_cySettingsWindowSize + rcMainFrame.bottom;
			rcSettings.top = rcMainFrame.bottom;
			rcSettings.left = rcMainFrame.left+=m_rectBorder.left;
			rcSettings.right = rcMainFrame.right;

			m_wndSettings.MoveWindow( rcSettings );
		}

		m_wndSettings.Invalidate();
	}


	if( m_bInOleEditMode )
		OnOleFrameResizeBorders();


}
#pragma optimize("", on)


#define IMAGE_INDEX_OK		0
#define IMAGE_INDEX_CANCEL	1
#define IMAGE_INDEX_HELP	2
#define IMAGE_INDEX_BACK	3
#define IMAGE_INDEX_NEXT	4
#define IMAGE_INDEX_FINISH	5
#define IMAGE_INDEX_APPLY	6

inline int __image_from_button( unsigned id )
{
	static int	arr_images_indexes[] = 
	{
		IDOK,
		IDCANCEL,
		IDHELP,
		WIZB_BACK,
		WIZB_NEXT,
		WIZB_FINISH,
		IDAPPLY
	};
	for( long l = 0; l < sizeof(arr_images_indexes)/sizeof(arr_images_indexes[0]); l++ )
		if( arr_images_indexes[l] == id )
			return l;
	return -1;
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if( CMDIFrameWnd::OnCreateClient( lpcs, pContext ) )
	{
		m_wndMDIClient.SubclassWindow( m_hWndMDIClient );
		//m_wndMDIClient.SetBitmap( IDB_PATTERN );
		//m_wndMDIClient.SetBitmap(IDB_STONE);
		return TRUE;
	}				   
	else
		return FALSE;
}

void CMainFrame::EnterMode(long nHeight)
{
	::FireEvent(GetAppUnknown(), szEventMainFrameOnEnterMode);
	m_nModeHeight = nHeight; // запомним - вдруг потом пригодится
	// --- поддержка методик: хитрая настройка в shell.data, запрещающая показ форм
	if( ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", 0 ) != 0 )
		nHeight=0; // если форма невидима, то не показывать
	// ---

	KillActivePopup();

	if( m_hwndXPBar )
	{
		if( m_bInSettingsMode )
		{
			XPPANEITEM	item;
			item.mask = XPF_HEIGHT;
			item.nHeight = nHeight;
			::SendMessage( m_hwndXPBar, XPB_SETITEM, (WPARAM)m_lposForm, (LPARAM)&item );

			return;
		}

/*		CRect rcSettings = NORECT;
		m_wndSettings.GetWindowRect( rcSettings );
		rcSettings.bottom = rcSettings.top + nHeight;
		m_wndSettings.MoveWindow(&rcSettings, TRUE);
*/

		if( !EnterSDIMode() )
			return;
		if( !m_hImageListButtons )
		{
			m_hImageListButtons = ::ImageList_Create( 16, 16, ILC_COLOR24|ILC_MASK, 0, 4 );
			HBITMAP	hbmp = ::LoadBitmap( AfxGetResourceHandle(), MAKEINTRESOURCE( IDB_CAPIONBUTTONS ) );
			::ImageList_AddMasked( m_hImageListButtons, hbmp, RGB( 192, 192, 192 ) );
		}
		::SetParent( m_wndSettings, m_hwndXPBar );

		TPOS lpos_insert = 0;
		if( m_lposStatus )
			lpos_insert = m_lposStatus;//*(long*)m_lposStatus;

		XPINSERTITEM	insert;
		insert.insert_pos = lpos_insert;
		insert.item.mask = XPF_TEXT|XPF_STYLE|XPF_HEIGHT|XPF_IMAGELIST_CAPTION;
		insert.item.pszText = "Form";
		insert.item.style = XPPS_WINDOW;
		insert.item.nHeight = nHeight;
		insert.item.hImageListCaption = m_hImageListButtons;


		m_lposForm = (TPOS)::SendMessage( m_hwndXPBar, XPB_INSERTITEM, 0, (LPARAM)&insert );
		::SendMessage(m_hwndXPBar, XPB_SETWINDOW, (WPARAM)m_lposForm, (LPARAM)m_wndSettings.GetSafeHwnd());

		/*
		XP_CAPTION_BUTTON	button;
		button.mask = XPCBM_IMAGE;
		button.nCmd = IDOK;
		button.iImage = 0;
		::SendMessage( m_hwndXPBar, XBP_ADDCAPTIONBUTTON, m_lposForm, (LPARAM)&button );
		button.nCmd = IDCANCEL;
		button.iImage = 1;
		::SendMessage( m_hwndXPBar, XBP_ADDCAPTIONBUTTON, m_lposForm, (LPARAM)&button );
		button.nCmd = IDHELP;
		button.iImage = 2;
		::SendMessage( m_hwndXPBar, XBP_ADDCAPTIONBUTTON, m_lposForm, (LPARAM)&button );
		*/

		m_bInSettingsMode = true;
		return;

	}

	m_cySettingsWindowSize = nHeight;


	if( m_bInSettingsMode )
	{
		m_rectBorder.bottom =  m_cySettingsWindowSize;
		m_wndSettings.ShowWindow( SW_SHOW );
		RecalcLayout();
		return;
	}

	if( !EnterSDIMode() )
		return;

	m_nOrigBottom = m_rectBorder.bottom;
	//TRACE( "m_nOrigBottom = %d\n", m_nOrigBottom );
	m_rectBorder.bottom =  m_cySettingsWindowSize;
	m_wndSettings.ShowWindow( SW_SHOW );

	m_bInSettingsMode = true;
	RecalcLayout();
}

void CMainFrame::LeaveMode()
{
	::FireEvent(GetAppUnknown(), szEventMainFrameOnLeaveMode);

	KillActivePopup();
	
	if( !m_bInSettingsMode )
		return;


	if( m_hwndXPBar && m_lposForm )
	{
		m_wndSettings.ShowWindow( SW_HIDE );
		::SetParent( m_wndSettings, GetSafeHwnd() );
		::SendMessage(m_hwndXPBar, XPB_REMOVEITEM, (WPARAM)m_lposForm, 0);
		m_lposForm = 0;
	}

	m_bInSettingsMode = false;
	//m_rectBorder.bottom =  m_cySettingsWindowSize;
	m_rectBorder.bottom = m_nOrigBottom;
	m_wndSettings.ShowWindow( SW_HIDE );
	
	if( !LeaveSDIMode() )
		return;

	RecalcLayout();

	//solve toolbar problem
	if( AfxGetApp() )
		AfxGetApp()->OnIdle( 0 );
}

//overload for replace accel table
BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if( !CBCGMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext) )
		return FALSE;

	if( m_hAccelTable )
		::FreeResource( m_hAccelTable );
	m_hAccelTable = g_CmdManager.GetAcceleratorTable();

	// AAM personal code to disable fucking steal focus
	int nDisableSteal = 
		GetPrivateProfileInt("Shell", "DisableStealFocus", 0, "vt5_monster.ini");
	if(nDisableSteal)
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, LPVOID(10000), 0);
	else
		SetFocus();

	return true;
}

void CMainFrame::HtmlHelp(DWORD_PTR dwData, UINT nCmd) 
{
	return; // не будет тут хелпа. Все равно сюда попадаем только после комбинации Shift-F1 F1 (или дважды Shift-F1)
	// __super::HtmlHelp(dwData, nCmd);
}

//window action's helper - nawigate throw windows
bool CMainFrame::ExecuteMDICommand( UINT nID )
{
	return OnMDIWindowCmd( nID ) == TRUE;
}

void CMainFrame::CreateNewWindow()
{
	OnWindowNew();
}

CMDIChildWnd	*CMainFrame::GetWindowByTitle( const char *sz )
{
	POSITION	pos = GetFirstMDIChildPosition();

	while( pos )
	{
		CMDIChildWnd	*pwnd = GetNextMDIChild( pos );
		CString	str;

		pwnd->GetWindowText( str );

		if( str == sz )
			return pwnd;
	}

	return 0;
}

POSITION CMainFrame::GetFirstMDIChildPosition()
{
	return m_ptrChildWindows.GetHeadPosition();
}

CMDIChildWnd	*CMainFrame::GetNextMDIChild( POSITION &pos )
{
	return (CMDIChildWnd*)m_ptrChildWindows.GetNext( pos );
}

void CMainFrame::RegisterMDIChild( CMDIChildWnd *pwnd )
{
	if( m_ptrChildWindows.Find( pwnd ) )
		return;

	m_ptrChildWindows.AddTail( pwnd );

}

void CMainFrame::UnRegisterMDIChild( CMDIChildWnd *pwnd )
{
	POSITION	pos = m_ptrChildWindows.Find( pwnd );
	if( !pos )
		return;

	m_ptrChildWindows.RemoveAt( pos );
}

POSITION		CMainFrame::GetFirstDockBarPos()
{
	POSITION	pos = m_ptrDockWindows.GetHeadPosition();
	while( pos )
	{
		POSITION	posSave = pos;
		CControlBar	*p = (CControlBar	*)m_ptrDockWindows.GetNext( pos );
		if( p->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )return posSave;
	}

	return 0;
}

CShellDockBar	*CMainFrame::GetNextDockBar( POSITION &pos )
{
	while( pos )
	{
		CControlBar	*p = (CControlBar	*)m_ptrDockWindows.GetNext( pos );
		if( p->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )return (CShellDockBar*)p;
	}
	return 0;
}


LPDISPATCH CMainFrame::GetActiveFrameDisp() 
{
	//************************************************
	// Old code here
	//CMDIChildWnd	*pwnd = MDIGetActive();
		// End old code
	//************************************************

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |

	CMDIChildWnd	*pwnd = _GetActiveFrame( );

//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************



	if( !pwnd )
		return 0;

	return pwnd->GetIDispatch( TRUE );
}

//activate MDI child by given dispatch. 
//Do not change focus if dispatch is invalid or not MDI child
void CMainFrame::SetActiveFrame(LPDISPATCH newValue) 
{
				//get key of given window
	GuidKey lKey = ::GetObjectKey( newValue );

	if( lKey == INVALID_KEY )
		return;
				//enum all MDI childs
	POSITION	pos = GetFirstMDIChildPosition();

	while( pos )
	{
		CMDIChildWnd	*pwnd = GetNextMDIChild( pos );
				//test and activate
		GuidKey lKeyTest = ::GetObjectKey( pwnd->GetControlUnknown() );

		if( lKeyTest == lKey )
			MDIActivate( pwnd );
	}

}

//Is window visible?
BOOL CMainFrame::GetVisible() 
{
	return IsWindowVisible();
}

//make window visible or not
void CMainFrame::SetVisible(BOOL bNewValue) 
{
	ShowWindow( bNewValue ? SW_SHOW:SW_HIDE );

}

BOOL CMainFrame::GetEnableAccelerators()
{
	return m_bEnableAccelerators;
}

void CMainFrame::SetEnableAccelerators(BOOL bNewValue)
{
	m_bEnableAccelerators = bNewValue;
}

//Is window maximized?
BOOL CMainFrame::GetMaximized() 
{
	return IsZoomed();
}

//make window maximized or not
void CMainFrame::SetMaximized(BOOL bNewValue) 
{
	ShowWindow( bNewValue ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL );
}

//Is window iconic?
BOOL CMainFrame::GetIconic() 
{
	return IsIconic();
}

//make window iconic or not
void CMainFrame::SetIconic(BOOL bNewValue) 
{
	ShowWindow( bNewValue ? SW_SHOWMINIMIZED : SW_SHOWNORMAL );
}

const char cszWindowPlacement[] = _T("WindowPlacement");

void CMainFrame::SaveState(LPCTSTR szFileName) 
{	 
	_try(CMainFrame, SaveState)
	{
		WINDOWPLACEMENT	wp;

		ZeroMemory( &wp, sizeof( wp ) );
		wp.length = sizeof( wp );
		wp.flags = WPF_SETMINPOSITION|WPF_RESTORETOMAXIMIZED;

		GetWindowPlacement( &wp );

		CStringArrayEx	sae;
		CString	str;

		sae.Add( cszWindowPlacement );

		sae.Add(  _T("'Show command") );
		str.Format( _T("%d"), wp.showCmd );
		sae.Add( str );

		str.Format( _T("%d %d %d %d"), wp.rcNormalPosition.left, wp.rcNormalPosition.top, 
				wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );
		sae.Add(  _T("'Normal position") );
		sae.Add( str );
		
		str.Format( _T("%d %d"), wp.ptMaxPosition.x, wp.ptMaxPosition.y );
		sae.Add(  _T("'MaxPosition") );
		sae.Add( str );

		str.Format( _T("%d %d"), wp.ptMinPosition.x, wp.ptMinPosition.y );
		sae.Add(  _T("'MinPosition") );
		sae.Add( str );

		// [vanek] : make full path - 15.10.2004
		TCHAR szDrvDef[_MAX_DRIVE] = {0}, szPathDef[_MAX_PATH] = {0}, szFileDef[_MAX_PATH] = {0}, 
			szExtDef[_MAX_PATH] = {0};
		TCHAR szDrv[_MAX_DRIVE] = {0}, szPath[_MAX_PATH] = {0}, szFile[_MAX_PATH] = {0}, szExt[_MAX_PATH] = {0};
        
		CString	strPath( _T("") );
		strPath = ::MakeAppPathName( "window.pos" );

		::_tsplitpath( szFileName, szDrv, szPath, szFile, szExt );
		::_tsplitpath( strPath, szDrvDef, szPathDef, szFileDef, szExtDef );

		if( !_tcslen( szDrv ) )
			_tcscpy( szDrv, szDrvDef );
		if( !_tcslen( szPath ) )
			_tcscpy( szPath, szPathDef );
		if( !_tcslen( szFile ) )
			_tcscpy( szFile, szFileDef );
		if( !_tcslen( szExt ) )
			_tcscpy( szExt, szExtDef );
			
		::_makepath( strPath.GetBuffer( MAX_PATH ), szDrv, szPath, szFile, szExt );

		sae.WriteFile( /*szFileName*/ strPath );
	}
	_catch_report;
}

void CMainFrame::RestoreState(LPCTSTR szFileName) 
{
	_try(CMainFrame, SaveState)
	{
		// [vanek] : make full path - 15.10.2004
		TCHAR szDrvDef[_MAX_DRIVE] = {0}, szPathDef[_MAX_PATH] = {0}, szFileDef[_MAX_PATH] = {0}, 
			szExtDef[_MAX_PATH] = {0};
		TCHAR szDrv[_MAX_DRIVE] = {0}, szPath[_MAX_PATH] = {0}, szFile[_MAX_PATH] = {0}, szExt[_MAX_PATH] = {0};
        
		CString	strPath( _T("") );
		strPath = ::MakeAppPathName( "window.pos" );

		::_tsplitpath( szFileName, szDrv, szPath, szFile, szExt );
		::_tsplitpath( strPath, szDrvDef, szPathDef, szFileDef, szExtDef );

		if( !_tcslen( szDrv ) )
			_tcscpy( szDrv, szDrvDef );
		if( !_tcslen( szPath ) )
			_tcscpy( szPath, szPathDef );
		if( !_tcslen( szFile ) )
			_tcscpy( szFile, szFileDef );
		if( !_tcslen( szExt ) )
			_tcscpy( szExt, szExtDef );
			
		::_makepath( strPath.GetBuffer( MAX_PATH ), szDrv, szPath, szFile, szExt );
		

		CStringArrayEx	sae;

		sae.ReadFile( /*szFileName*/strPath );
		sae.SkipComments();

		if( sae[0] != cszWindowPlacement )
			return;

		WINDOWPLACEMENT	wp;

		ZeroMemory( &wp, sizeof( wp ) );
		wp.length = sizeof( wp );
		wp.flags = WPF_SETMINPOSITION|WPF_RESTORETOMAXIMIZED;

		if( ::sscanf( sae[1], _T("%d"), &wp.showCmd ) != 1 )
			return;

		if( ::sscanf( sae[2], _T("%d %d %d %d"), &wp.rcNormalPosition.left, &wp.rcNormalPosition.top, 
				&wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom ) != 4 )
			return;
		if( ::sscanf( sae[3], _T("%d %d"), &wp.ptMaxPosition.x, &wp.ptMaxPosition.y ) != 2 )
			return;
		if( ::sscanf( sae[4], _T("%d %d"), &wp.ptMinPosition.x, &wp.ptMinPosition.y ) != 2 )
			return;

		bool bOldLockRecalc = GetLockRecalc();
		SetLockRecalc( false );

		CLogoWnd::Lock( 1 );		


		HWND hwnd_newparent = 0;
		HWND hwnd_oldparent = 0;
		
		bool bhide = false;
		if( bhide = !IsWindowVisible() )
			// [vanek] : если окно скрыто - выставляем блокировку на его показ - 20.10.2004
		    SetLockShow( true );
		
		SetWindowPlacement( &wp );

		if( bhide )
			// [vanek] : снимаем установленную ранее блокировку на показ окна - 20.10.2004
			SetLockShow( false );


//		RecalcLayout();
		SetLockRecalc( bOldLockRecalc );
		CLogoWnd::Lock( 0 );

		AfxGetApp()->m_nCmdShow = wp.showCmd;
	}
	_catch_report;
}

void CMainFrame::SetWindowPos(short X, short Y, short CX, short CY) 
{
	MoveWindow( X, Y, CX, CY );
}

long CMainFrame::GetFramesCount() 
{
	return m_ptrChildWindows.GetCount();

	return 0;
}

LPDISPATCH CMainFrame::GetFrame(long nPos) 
{
	POSITION	pos = GetFirstMDIChildPosition();

	while( pos )
	{
		CMDIChildWnd	*pwnd = GetNextMDIChild( pos );

		if( !nPos-- )
			return pwnd->GetIDispatch( TRUE );
	}

	return NULL;
}

void CMainFrame::GetDesktopSize(VARIANT FAR* varCX, VARIANT FAR* varCY) 
{
	CRect	rc;	

	::GetClientRect( ::GetDesktopWindow(), &rc );

	*varCX = _variant_t( (long)rc.Width() );
	*varCY = _variant_t( (long)rc.Height() );
}

void CMainFrame::GetWindowPos(VARIANT FAR* varX, VARIANT FAR* varY, VARIANT FAR* varCX, VARIANT FAR* varCY) 
{
	CRect	rc;
	GetWindowRect( &rc );

	*varX = _variant_t( (long)rc.left );
	*varY = _variant_t( (long)rc.top );
	*varCX = _variant_t( (long)rc.Width() );
	*varCY = _variant_t( (long)rc.Height() );
}

void CMainFrame::OnTitleChanged() 
{
	//SetWindowText( m_strTitle );
	OnUpdateFrameTitle( true );

}

void CMainFrame::OnLabelChanged() 
{
	m_wndMDIClient.SetLabel( m_strLabel );

}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |	

	if(VerifyChild( m_pActiveChildFrame ))
		if (AfxCallWndProc(m_pActiveChildFrame,
			m_pActiveChildFrame->m_hWnd, WM_COMMAND, wParam, lParam) != 0)
			return TRUE; // handled by child
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


	return CBCGMDIFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// A.M. BT4869.
	if( ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "EnableDragDropOpen", 1 ) == 0 )
		return;

	SetActiveWindow();      // activate us first !
	int nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);


	//find the view. If objects dropped to the view, load it to the document
	bool	bLoad = false;
	CPoint	point;
	GetCursorPos( &point );

	m_wndMDIClient.ScreenToClient( &point );

	HWND hwnd = ::ChildWindowFromPoint( m_wndMDIClient, point );
	   
	if( !hwnd )
	{
		//1. get the main window
		IUnknownPtr		sptrUnkMainWnd(::_GetOtherComponent(::GetAppUnknown(false), IID_IMainWindow), false);
		IMainWindowPtr	sptrM = sptrUnkMainWnd;
		if (sptrM != 0)
		{
			//find the context_view window between dockbars
			POSITION lPos = 0;
			sptrM->GetFirstDockWndPosition(&lPos);
			IUnknownPtr	sptrDockBar;

			while (lPos)
			{
				sptrM->GetNextDockWnd(&sptrDockBar, &lPos);

				if (CheckInterface(sptrDockBar, IID_IContextView))
					break;
			}

			IWindowPtr pContext = sptrDockBar;


			CPoint	pt;
			GetCursorPos( &pt );

			HANDLE handle;

			pContext->GetHandle( &handle );

			if( handle )
			{
				CWnd *ptrWnd = CWnd::FromHandle( (HWND)handle );

				if( ptrWnd )
				{
					RECT rc;
					ptrWnd->GetWindowRect( &rc );

					if( ::PtInRect( &rc, pt ) )
						hwnd = *MDIGetActive();
				}
			}
		}
	}


	CWnd *pwnd = CWnd::FromHandle( hwnd );
	if( pwnd )
	{
		//CFrameWnd	*pframe = pwnd->GetParentFrame();
		if( pwnd && pwnd->IsKindOf( RUNTIME_CLASS( CShellFrame ) ) )
		{
			MDIActivate( pwnd );
			bLoad = true;
		}
	}


	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	for (int iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, (UINT)iFile, szFileName, _MAX_PATH);

		BOOL bPrevActionManagerState = 0;
		sptrIApplication sptrApp(GetAppUnknown());
		IUnknown* punk = 0;
		
		if ( m_pAM != 0 && m_pAM->EnableManager(TRUE) == S_OK)
			bPrevActionManagerState = TRUE;
		else
			bPrevActionManagerState = FALSE;

		CString	strParam;
		
		if( bLoad )
		{
			strParam.Format("\"\", \"%s\"",szFileName);
			::ExecuteAction(_T("ObjectLoad"), strParam, 0);
		}
		else
		{
			strParam.Format("\"%s\"",szFileName);
			::ExecuteAction(_T("FileOpen"), strParam, 0);
			bLoad = true;
		}
		
		
		if( m_pAM )
			m_pAM->EnableManager(bPrevActionManagerState);
		//pApp->OpenDocumentFile(szFileName);
	}
	::DragFinish(hDropInfo);


	IUnknown	*punk = _GetOtherComponent( GetAppUnknown(), IID_IOpenFileDialog );
	sptrIOpenFileDialog sptrFileOpen(punk);
	if(punk)
		punk->Release();
	if (sptrFileOpen)
	{
		HWND hWnd = 0;
		sptrFileOpen->GetDlgWindowHandle( &hWnd );
		if(::IsWindow(hWnd))
		{
			::SendMessage(hWnd, WM_SETFOCUS, 0, 0);
		}
	}
	//CBCGMDIFrameWnd::OnDropFiles(hDropInfo);
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBCGMDIFrameWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	//if(m_pLeftPane != 0)return;
	//m_pLeftPane = new CLeftPane;
	//HWND hwnd = 0;
	//m_pLeftPane->m_sptrStatus->GetHandle((HANDLE*)&hwnd);
	//if(hwnd)
	//{
		//m_pLeftPane->Create(NULL, "", WS_CHILD|WS_VISIBLE, NORECT, CWnd::FromHandle(hwnd), ID_SEPARATOR);
		//pLeftPane->Create(NULL, "QWERTY", WS_CHILD|WS_VISIBLE, NORECT, CWnd::FromHandle(hwnd), 294);
		//m_pLeftPane->m_sptrStatus->IncludePane(m_pLeftPane->GetControllingUnknown(), &m_nPaneNum);
	//}
	
	
}

void CMainFrame::PostNcDestroy() 
{
	//CBCGMDIFrameWnd::PostNcDestroy();
	/*POSITION	pos = gAllToolbars.GetHeadPosition();
	while( pos )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext( pos );

		if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
			continue;
		if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
			continue;

		DeleteToolBar( pToolBar );
	}*/

	delete this;
}

LRESULT CMainFrame::OnLongNotify(WPARAM wParam, LPARAM lParam)
{
	//script notify for LongOperation support
	DWORD dwCurrTime = ::GetTickCount( );
	//

	g_CmdManager.SetLongOperationActionState( (LongOperationAction)wParam );

	g_NtfManager.ProcessMessage( (IUnknown*)lParam, (LongOperationAction)wParam );

	int pnStage, pnPosition, pnPercent;

	if(wParam == loaStart)
	{
		pnPercent = 0;
		IUnknown	*punkLongOperation = (IUnknown*)lParam;

		//m_wndShellProgressm_wndStatusBar.InitOperation( sptrLong ); 
		CString	strOperationName = "Long operation, please wait...";

		if( CheckInterface( punkLongOperation, IID_IAction ) )
		{
			IUnknown	*punkAI = 0;
			IActionPtr	ptrA( punkLongOperation );

			ptrA->GetActionInfo( &punkAI );

			ASSERT( punkAI );

			IActionInfoPtr	ptrAI( punkAI );
			punkAI->Release();

			BSTR	bstrActionName = 0;
			ptrAI->GetCommandInfo( 0, &bstrActionName, 0, 0 );

			strOperationName = bstrActionName;
			::SysFreeString( bstrActionName );
		}
		
		m_wndStatusBar.SetWindowText( strOperationName );
		m_wndShellProgress.LockUpdateText( true );
		m_wndShellProgress.SetPercent(pnPercent);

		//script notify for LongOperation support
		if( m_nEnableScriptNotify )
		{
			m_nScriptSetPosLastPos = 0;
			m_dwScriptSetPosTime = 0;
			CString strActionName( _T("") );
			_get_action_name( (IUnknown*)lParam, &strActionName );
			m_dwScriptSetPosTime = dwCurrTime;
			_fire_script_event( _T("LongOperation_Start"), strActionName );

			/*FILE *f=fopen("C:\\NotifySetPos.log","a");
			fprintf(f,"Acttion %s is start...\n", strActionName );
			//fflush(f);
			fclose(f);*/

		}

	}
	else if(lParam != 0)	
	{
		sptrILongOperation	sptrLong((IUnknown*)lParam);
		if (sptrLong)
		sptrLong->GetCurrentPosition(&pnStage, &pnPosition, &pnPercent);
		m_wndShellProgress.SetPercent(pnPercent);

		//script notify for LongOperation support
		if( m_nEnableScriptNotify )
		{
			DWORD dwCurrLag = 0;
			dwCurrLag = dwCurrTime - m_dwScriptSetPosTime;
            long lCurrDeltaPos = 0;
			lCurrDeltaPos = pnPercent - m_nScriptSetPosLastPos;

			if( ( (m_lStep == -1) ? 0 : ( lCurrDeltaPos >= m_lStep ) ) || ( (m_lLag == -1) ? 0 : ( dwCurrLag >= (DWORD)m_lLag ) ) )
			{
				CString strActionName( _T("") );
				_get_action_name( (IUnknown*)lParam, &strActionName );
				_fire_script_event( _T("LongOperation_SetPos"), strActionName, TRUE, pnPercent );

				if( (m_lStep != -1) /*&& (lCurrDeltaPos >= lStep)*/ )
					m_nScriptSetPosLastPos = pnPercent;
				if( m_lLag != -1 /*&& (dwCurrLag >= (DWORD)lLag)*/ )
					m_dwScriptSetPosTime = dwCurrTime;

				/*FILE *f=fopen("C:\\NotifySetPos.log","a");
				fprintf(f,"%s\t%i\t%i\n", strActionName, pnPercent, dwCurrLag );
				//fflush(f);
				fclose(f);*/
			}
		}

	}

	if(wParam == loaTerminate || wParam == loaComplete)
	{
		pnPercent = 0;
		m_wndShellProgress.SetPercent( 0 );
		m_wndShellProgress.LockUpdateText( 0 );

		CString	str = ::LanguageLoadCString( AFX_IDS_IDLEMESSAGE );
		//str.LoadString( AFX_IDS_IDLEMESSAGE );
		m_wndStatusBar.SetWindowText( str );

		if( m_nEnableScriptNotify )
		{
			//script notify for LongOperation support
			m_nScriptSetPosLastPos = 0;
			m_dwScriptSetPosTime = 0;
			CString strActionName( _T("") );
			_get_action_name( (IUnknown*)lParam, &strActionName );
			CString strOp( _T("") );
			switch( wParam  )
			{
			case loaTerminate:
				{
				strOp = _T( "LongOperation_Terminate" );

				/*FILE *f=fopen("C:\\NotifySetPos.log","a");
				fprintf(f,"Acttion %s terminate.\n", strActionName );
				//fflush(f);
				fclose(f);*/

				}
				break;
			case loaComplete:
				{
				strOp = _T( "LongOperation_Complete" );
				/*FILE *f=fopen("C:\\NotifySetPos.log","a"); 
				fprintf(f,"Acttion %s complete.\n", strActionName ); 
				//fflush(f);
				fclose(f);*/
				}
				break;
			}				  
			if( !strOp.IsEmpty( ) )
				_fire_script_event( strOp, strActionName );
		}

	}

	
	return 0;
}


void CMainFrame::_CreateDockBar(IUnknown* punk, UINT nID, HWND* pHWND, bool bfloat, bool bCaption )
{
		CShellDockBar	*pbar = new CShellDockBar( punk );

		INamedObject2Ptr ptrNamed(punk);
		bool bEnableDock=true;
		if(ptrNamed!=0)
		{
			BSTR bstrName = 0;
			CString strName = "Default";
			if (SUCCEEDED(ptrNamed->GetName(&bstrName)))
				strName = bstrName;
			bEnableDock = GetValueInt( GetAppUnknown(), "\\MainFrame\\EnableDock", strName, 1 ) != 0;
		}

		BOOL bFixed = FALSE;
		DWORD dwSide = 0;
		DWORD dwStyle = CBRS_ALIGN_ANY;
		sptrIDockableWindow sptrDock(punk);

		CSize	sizeDefault = pbar->GetDefSize();
		pbar->Create( this, IDD_DUMMY, WS_CHILD|WS_VISIBLE, nID );
		//pbar->Create( "qqq", this, sizeDefault, true, nID );

		if( pHWND )
			*pHWND = pbar->GetSafeHwnd();

		CSize sizeActual(200, 150);
		if(sptrDock != 0)
		{
			sptrDock->GetDockSide(&dwSide);
			sptrDock->GetFixed(&bFixed);
			sptrDock->GetSize(&sizeActual, FALSE);
		}

		pbar->m_szFloat = sizeActual;
		pbar->SetMiniFrameCaption(bCaption);
    
		pbar->SetWindowPos(0, 0, 0, sizeActual.cx, sizeActual.cy, SWP_NOMOVE|SWP_NOZORDER);

		switch(dwSide)
		{
		case AFX_IDW_DOCKBAR_TOP:
			dwStyle = CBRS_ALIGN_TOP;
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			dwStyle = CBRS_ALIGN_BOTTOM;
			break;
		case AFX_IDW_DOCKBAR_LEFT:
			dwStyle = CBRS_ALIGN_LEFT;
			break;
		case AFX_IDW_DOCKBAR_RIGHT:
			dwStyle = CBRS_ALIGN_RIGHT;
			break;
		case AFX_IDW_DOCKBAR_FLOAT:
			dwStyle = 0;
			break;
		default:
			dwStyle = CBRS_ALIGN_ANY;
		}
		if(!bEnableDock) dwStyle=0;

		pbar->SetBarStyleEx (pbar->GetBarStyle () |
			CBRS_TOOLTIPS | CBRS_FLYBY | ((bFixed==TRUE) ? CBRS_SIZE_FIXED : CBRS_SIZE_DYNAMIC));
		pbar->EnableDocking( dwStyle );
		pbar->m_pDockContext->m_uMRUDockID = dwSide; //A.M. BT5164

		CString sName;
		pbar->GetWindowText(sName);
		POSITION pos = m_ptrDockWindows.GetHeadPosition();
		bool bInserted = false;
		while (pos)
		{
			POSITION posSaved = pos;
			CControlBar	*p = (CControlBar *)m_ptrDockWindows.GetNext(pos);
			CString s;
			p->GetWindowText(s);
			if (sName.CompareNoCase(s) < 0)
			{
				m_ptrDockWindows.InsertBefore(posSaved, pbar);
				bInserted = true;
				break;
			}
		}
		if (!bInserted)
			m_ptrDockWindows.AddTail( pbar );

		
		//DockControlBar( pbar, dwSide );
		if( m_pFloatingFrameClass )
		{
			CRect rcFrame = NORECT;
			GetWindowRect(&rcFrame);

			bool bPrevCaption = s_bMiniFrameCaption;
			s_bMiniFrameCaption = bCaption;
			if( bfloat )
				FloatControlBar( pbar, rcFrame.TopLeft(), dwSide );
			s_bMiniFrameCaption = bPrevCaption;
			
			CString str = ::GetObjectName( pbar->GetClientUnknown() );
			m_mapFormDockDialogs.SetAt(str, true);
		}
}

void CMainFrame::_KillDockBar(CString strName)
{
	CShellDockBar* pDock = 0;
	POSITION		pos = GetFirstDockBarPos();

	while( pos )
	{
		POSITION pos2Remove = pos;
		pDock = GetNextDockBar( pos );
		if(pDock)
		{
			CString str;

			if( !pDock->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
				continue;

			str = pDock->GetName();

			bool bPresent = false;
			m_mapFormDockDialogs.Lookup(str, bPresent);
			if(str == strName || (strName.IsEmpty() && bPresent))
			{
				IUnknown	*punk = ((CShellDockBar*)pDock)->GetClientUnknown();
				if( punk )punk->Release();
				//pDock->DestroyWindow();
				delete pDock;
				pDock = 0;
				m_ptrDockWindows.RemoveAt(pos2Remove);

				m_mapFormDockDialogs.SetAt(str, false);
			}
		}
	}
}

void CMainFrame::OnShowControlBar( UINT nCmd )
{
	int	nControlBarID = nCmd - ID_FIRST_DOCKBAR;

	if( !nControlBarID )
	{
		if( m_wndOutlook.IsWindowVisible() )
			ShowControlBar(&m_wndOutlook, false, false);			
		else
			ShowControlBar(&m_wndOutlook, true, false);
		return;
	}
	else
		nControlBarID--;

	POSITION	pos = GetFirstDockBarPos();
	while( pos )
	{
		CShellDockBar	*pbar = GetNextDockBar( pos );

		if( !nControlBarID )
		{
			if( pbar->IsWindowVisible() )
				ShowControlBar(pbar, false, false);			
			else
				ShowControlBar(pbar, true, false);
			RecalcLayout();	
			return;
		}
		nControlBarID--;
	}
}

void CMainFrame::OnShowToolBar( UINT nCmd )
{
	int	nControlBarID = nCmd - ID_FIRST_TOOLBAR - 1;

	POSITION	pos = gAllToolbars.GetHeadPosition();
	while( pos )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext( pos );

		if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
			continue;
		if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
			continue;
		if( !nControlBarID )
		{
			if( pToolBar->IsWindowVisible() )
				ShowControlBar(pToolBar, false, false);			
			else
				ShowControlBar(pToolBar, true, false);
			RecalcLayout();	
			return;
		}
		nControlBarID--;
	}
}

void CMainFrame::ResetDockSites()
{
	CShellToolDockBar	*pbar = 
	GetDockSite( AFX_IDW_DOCKBAR_TOP );
	pbar->ResetArray();
	pbar->AddDockBar( &m_wndMenuBar );

	pbar = GetDockSite( AFX_IDW_DOCKBAR_LEFT );
	pbar->ResetArray();
	pbar->AddDockBar( &m_wndXPBarHolder );
	pbar->AddDockBar( &m_wndOutlook );

	pbar = GetDockSite( AFX_IDW_DOCKBAR_RIGHT );
	pbar->ResetArray();
	pbar = GetDockSite( AFX_IDW_DOCKBAR_BOTTOM );
	pbar->ResetArray();


}

CShellToolDockBar	*CMainFrame::GetDockSite( int nDSCode )
{
	CDockBar* pDock = (CDockBar*)GetControlBar( nDSCode );
	ASSERT( nDSCode );
	ASSERT( pDock->IsKindOf( RUNTIME_CLASS( CShellToolDockBar ) ) );
	return (CShellToolDockBar*)pDock;
}

LRESULT CMainFrame::OnIdleUpdate( WPARAM, LPARAM )
{
	int	nCount = 0;
	MSG		msg;
	while( PeekMessage( &msg, GetSafeHwnd(), WM_IDLEUPDATE, WM_IDLEUPDATE, PM_REMOVE ) )nCount++;


	AfxGetApp()->OnIdle( 0 );
//	AfxGetApp()->OnIdle( 1 );

	return 0;
}



LRESULT CMainFrame::OnOutlookNotify( WPARAM wParam, LPARAM )
{
	if( wParam == NM_FOLDERCHANGE )
	{
		PostMessage( WM_IDLEUPDATE );
	}

	return 0;
}


bool CMainFrame::RegisterComboButton( HWND hWnd, IUnknown* punkInfo )
{

	bool bFound = false;

	POSITION pos = 0;
	for( pos = m_arComboButtons.GetHeadPosition(); pos != NULL; )
	{
		CComboBoxInfo* pInfo = (CComboBoxInfo*)m_arComboButtons.GetNext( pos );
		if( pInfo->hWnd == hWnd )
			bFound = true;
	}

	if( bFound )
		return false;

	CComboBoxInfo* pComboInfo = new CComboBoxInfo;
	pComboInfo->hWnd =hWnd;
	pComboInfo->ptrActionInfo = punkInfo;

	m_arComboButtons.AddTail( pComboInfo );

	return true;
}

bool CMainFrame::UnRegisterComboButton( HWND hWnd )
{

	POSITION posRemove = 0;
	POSITION pos = 0;
	for( pos = m_arComboButtons.GetHeadPosition(); pos != NULL; )
	{
		posRemove = pos;
		CComboBoxInfo* pInfo = (CComboBoxInfo*)m_arComboButtons.GetNext( pos );
		if( pInfo->hWnd != hWnd )
			posRemove = 0;
			
	}

	if( posRemove != 0 )
	{
		CComboBoxInfo* pInfo = (CComboBoxInfo*)m_arComboButtons.GetAt( posRemove );
		delete pInfo;
		m_arComboButtons.RemoveAt( posRemove );
	}
	

	return true;
}


POSITION CMainFrame::GetFirstComboButton()
{
	return m_arComboButtons.GetHeadPosition();
}

CComboBoxInfo* CMainFrame::GetNextComboButton( POSITION &pos )
{
	return (CComboBoxInfo*)m_arComboButtons.GetNext( pos );
}

HRESULT CMainFrame::XUserInterface::GetFirstComboButton(POSITION *plPosition)
{
	_try_nested(CMainFrame, UserInterface, GetFirstComboButton)
	{		
		*plPosition = pThis->GetFirstComboButton();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XUserInterface::GetNextComboButton(HWND* phWnd, IUnknown** ppunkInfo, POSITION *plPosition)
{
	_try_nested(CMainFrame, UserInterface, GetNextComboButton)
	{		
		POSITION	pos = *plPosition;
		CComboBoxInfo* pInfo = pThis->GetNextComboButton( pos );
		*phWnd = pInfo->hWnd;
		pInfo->ptrActionInfo->AddRef();
		*ppunkInfo = pInfo->ptrActionInfo;
		*plPosition = pos;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XTranslatorWnd::TranslateAccelerator( MSG* pMsg, BOOL* pbTranslate )
{
	_try_nested(CMainFrame, TranslatorWnd, TranslateAccelerator)
	{		
		*pbTranslate = FALSE;

		if (pThis->m_hAccelTable != NULL &&
			::TranslateAccelerator( pThis->m_hWnd, pThis->m_hAccelTable, pMsg ) )
		{
			*pbTranslate = TRUE;
		}

		return S_OK;
	}
	_catch_nested;
}


void CMainFrame::KillActivePopup()
{
	if( ::IsWindow( m_Impl.GetActivePopup()->GetSafeHwnd() ) )
					m_Impl.GetActivePopup()->SendMessage (WM_CLOSE);
}


bool CMainFrame::HelpMode()
{
	// don't enter twice, and don't enter if initialization fails
	if (m_bHelpMode || !CanEnterHelpMode())
		return false;

	// don't enter help mode with pending WM_EXITHELPMODE message
	MSG msg;
	if (PeekMessage(&msg, m_hWnd, WM_EXITHELPMODE, WM_EXITHELPMODE, PM_REMOVE|PM_NOYIELD))
		return false;

	BOOL bHelpMode = m_bHelpMode;
	m_bHelpMode = true;

	// allow any in-place active servers to go into help mode
	/*if (bHelpMode && m_pNotifyHook != NULL &&
		!m_pNotifyHook->OnContextHelp(TRUE))
	{
		TRACE0("Error: an in-place server failed to enter context help mode.\n");
		m_pNotifyHook->OnContextHelp(FALSE);    // undo partial help mode
		m_bHelpMode = HELP_INACTIVE;
		return;
	}

	ASSERT(m_bHelpMode == HELP_ACTIVE);*/

	// display special help mode message on status bar
	UINT nMsgSave = (UINT)SendMessage(WM_SETMESSAGESTRING,
		(WPARAM)AFX_IDS_HELPMODEMESSAGE);
	if (nMsgSave == 0)
		nMsgSave = AFX_IDS_IDLEMESSAGE;

	DWORD   dwContext = 0;
	LONG lIdleCount = 0;
	CWinApp* pApp = AfxGetApp();

	IUnknown	*punkHelpTarget = 0;

	POINT   point;

	GetCursorPos(&point);
	SetHelpCapture(point, NULL);

	while (m_bHelpMode)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if( !ProcessHelpMessage( &msg, &punkHelpTarget ) )
				break;
		}
		else if (!pApp->OnIdle(lIdleCount++))
		{
			lIdleCount = 0;
			WaitMessage();
		}
	}

	ReleaseCapture();

	m_bHelpMode = false;
	// restore original status bar text
	SendMessage(WM_SETMESSAGESTRING, (WPARAM)nMsgSave);

	// tell in-place servers to exit Shift+F1 help mode
	//if (m_pNotifyHook != NULL)
	//	m_pNotifyHook->OnContextHelp(FALSE);

	PostMessage(WM_KICKIDLE);    // trigger idle update

	if( punkHelpTarget )
	{
		::HelpDisplay( punkHelpTarget );
		return true;
	}
	return false;
}

bool CMainFrame::ProcessHelpMessage( MSG *pmsg, IUnknown **ppunkHelpInfo )
/*{
	if( pmsg->message == WM_CHAR ) 
	{
		if( pmsg->wParam == VK_ESCAPE )
			return false;
	}
	else if( pmsg->message == WM_LBUTTONDOWN ) 
	{
		CPoint	point = pmsg->pt;
		::ClientToScreen( pmsg->hwnd, &point );
		HWND	hwnd = ::ChildWindowFromPoint( ::GetDesktopWindow(), point );

 		while( hwnd )
		{
			CPoint	pointLocal;
			::ScreenToClient( hwnd, &point );
			
			//check is it VideoTesT widnow
			DWORD	dw = ::SendMessage( hwnd, WM_GETINTERFACE, 0, MAKELONG(pointLocal.x, pointLocal.y ) );
			if( dw )
			{
				*ppunkHelpInfo = (IUnknown *)dw;
				return false;
			}
			//else try to send MFC message			
			dw = ::SendMessage( hwnd, WM_HELPHITTEST, pmsg->wParam, MAKELONG(pointLocal.x, pointLocal.y ) );

			if( dw != (DWORD)-1 && dw >= ID_CMDMAN_BASE && dw < ID_CMDMAN_MAX )
			{
				CActionInfoWrp	*pwrp = g_CmdManager.GetActionInfo( dw-ID_CMDMAN_BASE );
				if( !pwrp )
					return false;
				*ppunkHelpInfo = pwrp->m_pActionInfo;

				return false;
			}
			hwnd = ::GetParent( hwnd );
		}
		if( !hwnd )
		{
			*ppunkHelpInfo = GetControllingUnknown();
			return false;
		}
	}
	else  if( pmsg->message == WM_CANCELMODE )
		return false;
	
	return true;
}*/
{
	//return if ESCAPE pressed
	if (pmsg->message == WM_EXITHELPMODE || pmsg->message == WM_CANCELMODE || 
		(pmsg->message == WM_KEYDOWN && pmsg->wParam == VK_ESCAPE))
	{
		PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_REMOVE);
		return false;
	}

	//mouse messages
	CPoint point;
	if ((pmsg->message >= WM_MOUSEFIRST && pmsg->message <= WM_MOUSELAST) ||
		(pmsg->message >= WM_NCMOUSEFIRST && pmsg->message <= WM_NCMOUSELAST))
	{
		BOOL bDescendant;
		HWND hWndHit = SetHelpCapture(pmsg->pt, &bDescendant);
		if (hWndHit == NULL)
			return TRUE;

		if (bDescendant)
		{
			if (pmsg->message != WM_LBUTTONDOWN)
			{
				// Hit one of our owned windows -- eat the message.
				PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_REMOVE);
				return TRUE;
			}
			int iHit = (int)::SendMessage(hWndHit, WM_NCHITTEST, 0, MAKELONG(pmsg->pt.x, pmsg->pt.y) );

			//if we have clicked on menu, expand it
			if (iHit == HTMENU || iHit == HTSYSMENU)
			{
				ASSERT(::GetCapture() == m_hWnd);
				ReleaseCapture();
				// the message we peeked changes into a non-client because
				// of the release capture.
				GetMessage(pmsg, NULL, WM_NCLBUTTONDOWN, WM_NCLBUTTONDOWN);
				DispatchMessage(pmsg);
				GetCursorPos(&point);
				SetHelpCapture(point, NULL);
			}
			else 
			//if (iHit == HTCLIENT)
			{
				//*pContext = _AfxMapClientArea(hWndHit, pmsg->pt);
				PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_REMOVE);

				//check window sheme
				//check is it VideoTesT widnow
				CPoint	pointScreen( pmsg->lParam );
				::ClientToScreen( pmsg->hwnd, &pointScreen );


				while( hWndHit )
				{
					CPoint	pointClient( pointScreen );
					::ScreenToClient( hWndHit, &pointClient );


					DWORD	dwExStyle = ::GetWindowLong( hWndHit, GWL_EXSTYLE );
/*					if( dwExStyle & WS_EX_CONTEXTHELP )
					{
						HELPINFO	info;
						info.cbSize = sizeof( info );
						info.dwContextId = ::GetWindowContextHelpId( hWndHit );
						info.hItemHandle = 0;
						info.iContextType = HELPINFO_WINDOW;
						info.iCtrlId = ::GetDlgCtrlID( hWndHit );
						info.MousePos.x=  pointClient.x;
						info.MousePos.y=  pointClient.y;

						LONG	lResult = ::SendMessage( ::GetParent( hWndHit ), WM_HELP, 0, (LPARAM)&info );
						if( !lResult )return false;
					}*/

					DWORD	dw;

					//else try to send MFC message			
					dw = ::SendMessage( hWndHit, WM_HELPHITTEST, pmsg->wParam, MAKELONG(pointClient.x, pointClient.y ) );
					if( dw == (DWORD)-2 )
					{
						*ppunkHelpInfo = 0;
						return false;
					}

					if( dw != (DWORD)-1 && dw >= ID_CMDMAN_BASE && dw < ID_CMDMAN_MAX )
					{
						CActionInfoWrp	*pwrp = g_CmdManager.GetActionInfo( dw-ID_CMDMAN_BASE );
						if( !pwrp )
							return false;
						*ppunkHelpInfo = pwrp->m_pActionInfo;

						return false;
					}

					dw = ::SendMessage( hWndHit, WM_GETINTERFACE, 0, MAKELONG(pointClient.x, pointClient.y ) );
					if( dw )
					{
						*ppunkHelpInfo = (IUnknown *)dw;
						return false;
					}


					hWndHit = ::GetParent( hWndHit );
				}

				//else it ir main window
				*ppunkHelpInfo = GetControllingUnknown();
				return false;
			}
			/*else
			{
				*pContext = _AfxMapNonClientArea(iHit);
				PeekMessage(&msg, NULL, pmsg->message, pmsg->message, PM_REMOVE);
				return FALSE;
			}*/
		}
		else
		{
			// Hit one of our apps windows (or desktop) -- dispatch the message.
			PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_REMOVE);

			// Dispatch mouse messages that hit the desktop!
			DispatchMessage(pmsg);
		}
	}
	else if (pmsg->message == WM_SYSCOMMAND ||
			 (pmsg->message >= WM_KEYFIRST && pmsg->message <= WM_KEYLAST))
	{
		if (::GetCapture() != NULL)
		{
			ReleaseCapture();
			MSG msg;
			while (PeekMessage(&msg, NULL, WM_MOUSEFIRST,
				WM_MOUSELAST, PM_REMOVE|PM_NOYIELD));
		}
		if (PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_NOREMOVE))
		{
			GetMessage(pmsg, NULL, pmsg->message, pmsg->message);
			if (!PreTranslateMessage(pmsg))
			{
				TranslateMessage(pmsg);
				if (pmsg->message == WM_SYSCOMMAND ||
				  (pmsg->message >= WM_SYSKEYFIRST &&
					pmsg->message <= WM_SYSKEYLAST))
				{
					// only dispatch system keys and system commands
					ASSERT(pmsg->message == WM_SYSCOMMAND ||
						 (pmsg->message >= WM_SYSKEYFIRST &&
						  pmsg->message <= WM_SYSKEYLAST));
					DispatchMessage(pmsg);
				}
			}
		}
		GetCursorPos(&point);
		SetHelpCapture(point, NULL);
	}
	else
	{
		// allow all other messages to go through (capture still set)
		if (PeekMessage(pmsg, NULL, pmsg->message, pmsg->message, PM_REMOVE))
			DispatchMessage(pmsg);
	}

	return TRUE;
}

void CMainFrame::MakeForeground()
{
	// AAM personal code to disable fucking steal focus
	int nDisableSteal = 
		GetPrivateProfileInt("Shell", "DisableStealFocus", 0, "vt5_monster.ini");
	if(nDisableSteal)
	{
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, LPVOID(10000), 0);
		return; // ЗАЕБАЛО!
	}

	HWND hCurrWnd;
	int iMyTID;
	int iCurrTID;

	hCurrWnd = ::GetForegroundWindow();
	iMyTID   = ::GetCurrentThreadId();
	iCurrTID = ::GetWindowThreadProcessId(hCurrWnd,0);

	::AttachThreadInput( iMyTID, iCurrTID, TRUE );	

	if( ::IsIconic( GetSafeHwnd() ) )
		::ShowWindow( GetSafeHwnd(), SW_RESTORE );

	::BringWindowToTop( GetSafeHwnd() );
	::SetForegroundWindow( GetSafeHwnd() );

	::AttachThreadInput(iMyTID, iCurrTID, FALSE);
}

LRESULT CMainFrame::OnFindApp( WPARAM w, LPARAM l )
{
	if (!GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableRegisterExtensions", 1))
		return 0;
	MakeForeground();

	char* psz_file_name = "SHEL_COMMAND_LINE_MMF";	
	share_mem mem;
	mem.open_exist( psz_file_name, w );
	BYTE* pfile_buf = (BYTE*)mem.lock();
	if( pfile_buf )	
	{		
		_bstr_t bstr_line = (wchar_t*)pfile_buf;
		mem.unlock();

		if( bstr_line.length() )	
		{
			if( bstr_line.length() > 2*MAX_PATH - 1 )
				return false;

			char sz_line[2*MAX_PATH];
			strcpy( sz_line, (char*)bstr_line );
			char* psz_first = strchr( sz_line, '"' );
			if( psz_first )
				psz_first++;
			else
				psz_first = sz_line;
			char* psz_last = strrchr( sz_line, '"' );
			if( psz_last )
				*psz_last = 0;
			    
			CFileFind file;
			if( file.FindFile( psz_first ) )
			{
				CString strParam = CString( "\"" ) + psz_first + CString( "\"" );
				::ExecuteAction( "FileOpen", strParam );				
			}			
		}
	}

	return TRUE;
}

void CMainFrame::ResetUserButtonNames() 
{
	// TODO: Add your dispatch handler code here

}


BOOL CMainFrame::GetToolbarButtonToolTipText( CBCGToolbarButton* pButton, CString& strTTText )
{
	if( !pButton )return false;

	CActionInfoWrp	*pwrp = g_CmdManager.GetActionInfo( pButton->m_nID - ID_CMDMAN_BASE );
	if( !pwrp )
	{
		return false;
	}

	CString	str = pwrp->GetActionHelpString();

	int	idx = str.Find( "\n" );
	if( idx == str.GetLength()-1 )
		strTTText = str.Left( str.GetLength()-1 );
	else if( idx != -1 )
		strTTText  = str.Right( str.GetLength()-idx-1 );
	else
		strTTText = str;

	return true;
}

void CMainFrame::ShowSplash(LPCTSTR szFileName) 
{
	HideSplash();

	m_pwndSplash = new CSplashWindow( this );
	if( !m_pwndSplash->LoadImage( szFileName ) )
	{
		delete m_pwndSplash;
		m_pwndSplash = 0;
		return;
	}
	m_pwndSplash->Create();
}

void CMainFrame::HideSplash()
{
	if( !m_pwndSplash )return;

	if( m_pwndSplash->GetSafeHwnd() )
		m_pwndSplash->DestroyWindow();
	delete m_pwndSplash;
	m_pwndSplash = 0;
}

bool CMainFrame::EnableOutlookAnimation( bool bEnable )
{
	bool	bOldAnimation = (m_wndOutlook.dwFlags&CGfxOutBarCtrl::fAnimation)==CGfxOutBarCtrl::fAnimation;
	if( bEnable )m_wndOutlook.dwFlags |= CGfxOutBarCtrl::fAnimation;
	else m_wndOutlook.dwFlags &= ~CGfxOutBarCtrl::fAnimation;
	return bOldAnimation;
}


/////////////////////////////////////////////////////////////////////////////////
//
//
//	Ole in place edit support
//
//

HRESULT CMainFrame::XOleFrame::GetMainHwnd( HWND* phWndMain )
{
	_try_nested(CMainFrame, OleFrame, GetMainHwnd)
	{				
		*phWndMain = pThis->GetSafeHwnd();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CMainFrame::XOleFrame::ActivateOleObject( IUnknown* punkOleObject, HWND* phwndFrame )
{
	_try_nested(CMainFrame, OleFrame, ActivateOleObject)
	{				
		pThis->m_guidOleObject = ::GetObjectKey( punkOleObject );
		pThis->OnActivateOleObject();

		if( phwndFrame )
		{
			*phwndFrame = 0;
			CShellFrame* pFrame = pThis->GetActiveChildFrame();		
			if( pFrame )
			{
				*phwndFrame = pFrame->GetSafeHwnd();
			}
		}		


		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XOleFrame::CreateStdOLEMenu( IUnknown* punkOleObjectData, IUnknown* punkOleObject )
{
	_try_nested(CMainFrame, OleFrame, ActivateOleObject)
	{				
		if( !pThis->m_bInOleEditMode )
			return S_FALSE;
		//Need create menu cos oleObjecvt not support in place edit

		pThis->CreateOleMenu();		

		pThis->m_hMenuMain = ::CreateMenu();
		if( !pThis->m_hMenuMain )
			return E_FAIL;

		
		int nIndex = 0;
		for( int i=0;i<OLE_MENU_SIZE;i++ )
		{
			if( pThis->m_hOleMenu[i] != 0 )
			{
				::InsertMenu( pThis->m_hMenuMain, 0, MF_BYPOSITION | MF_POPUP, 
								(UINT)pThis->m_hOleMenu[i], pThis->m_strOleMenuCaption[i] );
				nIndex++;
			}
		}

		//add ole verb menu
		if( punkOleObject )
		{
			IOleObjectPtr ptr( punkOleObject );
			if( ptr )
				::OleUIAddVerbMenu( ptr, NULL, pThis->m_hMenuMain, 
							nIndex, ID_OLE_VERB_MIN, ID_OLE_VERB_MAX, FALSE, 0, &pThis->m_hMenuOleVerbs );
		}
		

		::SetMenu( pThis->GetSafeHwnd(), pThis->m_hMenuMain );

		pThis->DelayRecalcLayout();
		pThis->_RecalcLayout();

		return S_OK;
	}
	_catch_nested;
}

void CMainFrame::OnOleVerb( UINT nCmd )
{

	UINT uiVerb = nCmd - ID_OLE_VERB_MIN;
	IOleObjectUIPtr ptr( GetActiveOleObject() );
	if( ptr )
		ptr->DoVerb( uiVerb );
}


HRESULT CMainFrame::XOleFrame::DeactivateOleObject( IUnknown* punkOleObject )
{
	_try_nested(CMainFrame, OleFrame, DeactivateOleObject)
	{				
		pThis->OnDeActivateOleObject();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CMainFrame::XOleFrame::GetPseudoClientRect( RECT* prect, BOOL bIncludeBorders )
{
	_try_nested(CMainFrame, OleFrame, GetPseudoClientRect)
	{	
		*prect = NORECT;
		if( pThis->m_wndMDIClient.GetSafeHwnd() )
		{			
			CRect rcMDI;
			pThis->m_wndMDIClient.GetWindowRect( &rcMDI );
			pThis->ScreenToClient( &rcMDI );

			int nWidth = rcMDI.Width();
			int nHeight = rcMDI.Height();

			
			if( bIncludeBorders )
			{
				rcMDI.left	-= pThis->m_rcOleBorders.left;
				rcMDI.top	-= pThis->m_rcOleBorders.top;

				rcMDI.right	+= pThis->m_rcOleBorders.right;
				rcMDI.bottom+= pThis->m_rcOleBorders.bottom;
			}			
			

			*prect = rcMDI;
		}
		

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XOleFrame::SetOleBorders( LPCBORDERWIDTHS pBW )
{
	_try_nested(CMainFrame, OleFrame, SetAvailableSpace)
	{	
		if( !pThis->m_bInOleEditMode )
		{
			//ASSERT( FALSE );
			return S_FALSE;
		}

		pThis->m_rcOleBorders = NORECT;
		if( pBW )
			pThis->m_rcOleBorders = *pBW;

		pThis->NegotiateBorderSpace(CFrameWnd::borderSet, pThis->m_rcOleBorders );
		pThis->DelayRecalcLayout();
		//pThis->PostMessage(WM_KICKIDLE);
		pThis->_RecalcLayout();//Delay

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMainFrame::XOleFrame::GetAccelInfo( HACCEL* phaccel, UINT* pcAccelEntries )
{
	_try_nested(CMainFrame, OleFrame, GetAccelInfo)
	{	
		*phaccel		= 0;
		*pcAccelEntries	= 0;

		*phaccel		= g_CmdManager.GetAcceleratorTable( );
		*pcAccelEntries	= g_CmdManager.GetAcceleratorEntries( );

		return S_OK;
	}
	_catch_nested;
}


void CMainFrame::OnActivateOleObject()
{
	if( m_bInOleEditMode )// Deactivate prev
		OnDeActivateOleObject( );

	m_bInOleEditMode = true;	
	m_rcOleBorders = NORECT;		

	CreateOleToolBarArray();
	HideOleToolbars();

	{
		CRect rcBorders;
		NegotiateBorderSpace( CFrameWnd::borderGet, &rcBorders );	
		if( rcBorders.left != rcBorders.right != rcBorders.top != rcBorders.bottom != 0 )
		{
			NegotiateBorderSpace( CFrameWnd::borderSet, &m_rcOleBorders );				
			//pThis->DelayRecalcLayout();
		}
		
	}

	DelayRecalcLayout();
	_RecalcLayout();
	
	//PostMessage(WM_KICKIDLE);

}




void CMainFrame::OnDeActivateOleObject( bool brecalc_layout )
{

	m_rcOleBorders = NORECT;

	NegotiateBorderSpace( CFrameWnd::borderSet, m_rcOleBorders );		

	m_guidOleObject = INVALID_KEY;

	ShowOleToolbars();

	//_RecalcLayout();

	//cos BCG has no menu
	::SetMenu( GetSafeHwnd(), NULL );

	m_bInOleEditMode = false;

	if( brecalc_layout )
	{
		DelayRecalcLayout();
		_RecalcLayout( );
	}
	//PostMessage(WM_KICKIDLE);

}


void CMainFrame::OnOleFrameResizeBorders()
{
	if( !m_bInOleEditMode )
		return;

	IOleObjectUIPtr ptr = GetActiveOleObject();
	if( ptr == 0 )
		return;

	IOleFramePtr ptrFrame( GetControllingUnknown() );
	CRect rc;
	ptrFrame->GetPseudoClientRect( &rc, TRUE );

	ptr->OnFrameResizeBorder( rc );

}

void CMainFrame::OnOleFrameActivate( BOOL bActivate )
{
	if( !m_bInOleEditMode )
		return;

	IOleObjectUIPtr ptr = GetActiveOleObject();
	if( ptr == 0 )
		return;

	ptr->OnFrameActivate( bActivate );
}


void CMainFrame::_RecalcLayout( )
{
	//MFC style "support" to avoid IOleObject::SetBorders call
	if( m_bOleResizeFrame )
		return;

	m_bOleResizeFrame = true;
	if( GetSafeHwnd() )
	{		
		RecalcLayout();
	}

	//and recalc layout in Shell Frames
	POSITION pos = m_children.GetHeadPosition();
	while( pos != NULL)
	{
		CShellFrame* pChildFrame = (CShellFrame*)m_children.GetNext(pos);
		if( pChildFrame != NULL && ::IsWindow( pChildFrame->GetSafeHwnd() ) )
		{
			if( pChildFrame->GetFrameType() == ftMDI && pChildFrame && pChildFrame->IsIconic( ) )
				continue;

			if( !IsSDIMode() )
				pChildFrame->ModifyStyle( 0, WS_SYSMENU );

			pChildFrame->_RecalcLayoyt( true );			


		}
	}

	m_bOleResizeFrame = false;
}



HRESULT CMainFrame::XOleFrame::GetMenu( HMENU* phMenu, BSTR* pbstrCaption/*[size - 6]*/ )
{
	_try_nested(CMainFrame, OleFrame, GetMenu)
	{				
		pThis->CreateOleMenu();
		for( int i=0;i<OLE_MENU_SIZE;i++)
		{
			phMenu[i] = pThis->m_hOleMenu[i];
			pbstrCaption[i] = NULL;
			if( pThis->m_hOleMenu[i] )
				pbstrCaption[i] = pThis->m_strOleMenuCaption[i].AllocSysString();
			
		}
		return S_OK;
	}
	_catch_nested;
}




void CMainFrame::CreateOleMenu()
{
	DestroyOleMenu();

	INamedDataPtr ptrData( ::GetAppUnknown() );
	if( ptrData == 0 )
		return;


	CString strTemp = ::LanguageLoadCString( IDS_OLE_FILE );
	//strTemp.LoadString( IDS_OLE_FILE );

	CStringArray arMenuCaption;	
	arMenuCaption.Add( strTemp );
	arMenuCaption.Add( "Edit" );
	arMenuCaption.Add( "View" );
	arMenuCaption.Add( "Object" );
	arMenuCaption.Add( "Window" );
	arMenuCaption.Add( "Help" );

	CStringArray arMenuSection;
	arMenuSection.Add( OLE_FILE );
	arMenuSection.Add( OLE_EDIT );
	arMenuSection.Add( OLE_VIEW );
	arMenuSection.Add( OLE_OBJECT );
	arMenuSection.Add( OLE_WINDOW );
	arMenuSection.Add( OLE_HELP );


	int nOleIndex = 0;
	while( nOleIndex < OLE_MENU_SIZE )
	{
		long nEntriesCount = 0;
		ptrData->SetupSection( _bstr_t( (LPCSTR)arMenuSection[nOleIndex] ) );
		ptrData->GetEntriesCount( &nEntriesCount );
		bool bFirstntry = true;

		for( int i=0;i<nEntriesCount;i++ )
		{			
			BSTR bstrEntryName;
			ptrData->GetEntryName( i, &bstrEntryName );
			CString strEntry = bstrEntryName;
			::SysFreeString( bstrEntryName );	bstrEntryName = 0;

			CString strPrefix = OLE_ACTION_PREFIX;
			int nIndex = strEntry.Find( strPrefix );
			if( nIndex != -1 )
			{
				CString strActionName = strEntry.Right( strEntry.GetLength() - strPrefix.GetLength() );
				CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( strActionName );
				if( pi )
				{
					if( bFirstntry )//Need create Menu;
					{
						bFirstntry = false;
						m_hOleMenu[nOleIndex] = ::CreateMenu();						
						m_strOleMenuCaption[nOleIndex] = arMenuCaption[nOleIndex];
					}

					CString strMenuName = pi->GetActionUserName();
					int nCommandID = pi->GetLocalID();				

					::AppendMenu( m_hOleMenu[nOleIndex], MF_STRING|MF_ENABLED, nCommandID, strMenuName );
				}				
			}		
		}
		nOleIndex++;
	}

	ptrData->SetupSection( _bstr_t( "\\" ) );
}


void CMainFrame::DestroyOleMenu()
{

	for( int i=0;i<OLE_MENU_SIZE;i++ )
	{
		if( m_hOleMenu[i] != NULL )
			::DestroyMenu( m_hOleMenu[i] );
		
		m_strOleMenuCaption[i].Empty();

		m_hOleMenu[i] = NULL;
	}

	if( m_hMenuMain )
	{
		::DestroyMenu( m_hMenuMain );
		m_hMenuMain = 0;
	}

	if( m_hMenuOleVerbs )
	{
		::DestroyMenu( m_hMenuOleVerbs );
		m_hMenuOleVerbs = 0;
	}

}

void CMainFrame::DestroyOleToolBarArray()
{
	m_arOleToolbar.RemoveAll();
}



void CMainFrame::CreateOleToolBarArray()
{
	DestroyOleToolBarArray();

	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext( pos );
			if( pToolBar && pToolBar->IsKindOf( RUNTIME_CLASS(CBCGToolBar) ) )
			{
				if( pToolBar->IsWindowVisible() )
					m_arOleToolbar.Add( pToolBar->GetSafeHwnd() );
			}
		}

}


void CMainFrame::HideOleToolbars()
{
	for( int i=0;i<m_arOleToolbar.GetSize();i++ )
	{
		::ShowWindow( m_arOleToolbar[i], SW_HIDE );
	}
	
}

void CMainFrame::ShowOleToolbars()
{
	for( int i=0;i<m_arOleToolbar.GetSize();i++ )
	{
		::ShowWindow( m_arOleToolbar[i], SW_SHOW );
	}

}


IOleObjectUIPtr CMainFrame::GetActiveOleObject()
{

	if( !m_pActiveChildFrame || !m_pActiveChildFrame->GetSafeHwnd() )
		return 0;

	IOleObjectUIPtr ptr;
	CDocument* pDoc = m_pActiveChildFrame->GetActiveDocument();
	if( !pDoc )
		return 0;

	IUnknown* punkObject = ::GetObjectByKey( pDoc->GetControllingUnknown(), m_guidOleObject );
	if( !punkObject )
		return 0;

	ptr = punkObject;
	punkObject->Release();	punkObject = 0;


	short nState = -1;
	ptr->GetState( &nState );

	if( nState != (short)iaFullActivate )
		return 0;

	return ptr;
	 

}

void	CMainFrame::CheckActivePopup( MSG *pMsg )
{
	if( pMsg->message == WM_NCLBUTTONDOWN ||
		pMsg->message == WM_NCLBUTTONUP ||
		pMsg->message == WM_NCRBUTTONDOWN ||
		pMsg->message == WM_NCRBUTTONUP ||
		pMsg->message == WM_DESTROY )
	{
		KillActivePopup();
	}

	if( pMsg->message == WM_ACTIVATE ||
		pMsg->message == WM_CLOSE ||
		pMsg->message == WM_SETFOCUS )
	{

		HWND	hwndPopup = m_Impl.GetActivePopup()->GetSafeHwnd();
		if( hwndPopup )
		{
			HWND	hwnd = pMsg->hwnd;

			while( hwnd )
			{
				if( hwnd == hwndPopup )
					break;
				hwnd = ::GetParent( hwnd );
			}

			if( !hwnd )KillActivePopup();
		}
	}
}

void CMainFrame::ShowXPBar(BOOL bShow) 
{
	if( bShow )
	{
		if( m_hwndXPBar )return;
		m_hwndXPBar = ::CreateWindow( XPBAR_CLASS, "",
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, 100, 100, GetSafeHwnd(),
			(HMENU)1002, theApp.m_hInstance, 0 );

		m_nXPWidth = ::SendMessage( m_hwndXPBar, XPB_GETDEFPARAMS, 0, 0 );


		XP_TIMER_PARAMS	params;
		params.cb_size = sizeof( params );

		if( GetVersion() & 0x80000000 )//9x
		{
			params.stages_count = GetValueInt( GetAppUnknown(), "\\xpbar", "StagesCount9x", 5 );
			params.timer_speed = GetValueInt( GetAppUnknown(), "\\xpbar", "TimerSpeed9x", 20 );
		}
		else
		{
			params.stages_count = GetValueInt( GetAppUnknown(), "\\xpbar", "StagesCountNT", 10 );
			params.timer_speed = GetValueInt( GetAppUnknown(), "\\xpbar", "TimerSpeedNT", 20 );
		}

		::SendMessage( m_hwndXPBar, XPB_SETTIMERPARAMS, 0, (LPARAM)&params );
//		m_rectBorder.left += m_nXPWidth;
		m_wndXPBarHolder.m_hWndXPbar = m_hwndXPBar;
		m_wndXPBarHolder.m_nWidth = m_nXPWidth;
//		m_wndXPBarHolder.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		ShowControlBar(&m_wndXPBarHolder, TRUE, FALSE);


		m_lposStatus = m_wndStatusBar.SetXPBar( m_hwndXPBar );
		ShowControlBar( &m_wndStatusBar, false, false );

		//m_wndMDIClient.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	}
	else
	{
		bool bShowStatusBar = ::GetValueInt( ::GetAppUnknown(), "\\StatusBar", "ShowStatusBar", 1 ) != 0;

		m_wndStatusBar.SetXPBar( 0 );
		ShowControlBar( &m_wndStatusBar, bShowStatusBar, false );

		if( !m_hwndXPBar )return;

		m_lposStatus = m_lposProperties = 0;
		m_lposForm = 0;
		::DestroyWindow(m_hwndXPBar);
		m_hwndXPBar = 0;
//		m_rectBorder.left -= m_nXPWidth;

		m_wndXPBarHolder.m_hWndXPbar = 0;
		ShowControlBar(&m_wndXPBarHolder, FALSE, FALSE);


		CMDIChildWnd	*pChild = MDIGetActive();
		if( pChild )
			((CShellFrame*)pChild)->UpdateClientEdge( 0 );
		else 		
			m_wndMDIClient.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

	}

	RecalcLayout();
}

long CMainFrame::AddToolbarPane(LPCTSTR szCaption, long lInsertPos, long lImageList, long nImage) 
{
	if( !m_hwndXPBar )return 0;

	// [vanek] : переводим lInsertPos во внутреннюю позицию	паны - 18.08.2004
	TPOS posInt = xppane_pos_ext2int( lInsertPos );
	
	XPINSERTITEM	insert;
	ZeroMemory( &insert, sizeof( insert ) );

	HIMAGELIST	hImageList = 0;
	if( lImageList == -1 )
	{
		if( !m_hImageListAction )m_hImageListAction = 
			::ImageList_Create( 16, 15, ILC_COLOR24|ILC_MASK, 4, 4);
		hImageList = m_hImageListAction;
	}
	else if( lImageList )
	{
		hImageList = m_listImageList.GetAt( (POSITION)lImageList );
	}

	if( !lInsertPos )
	{
		TPOS	lposBefore = m_lposStatus;
		if( m_lposProperties )lposBefore = m_lposProperties;
		if( m_lposForm )lposBefore = m_lposForm;

		///!!!using _list_t feature
		if( lposBefore )
			posInt = lposBefore;//*(LONG_PTR*)lposBefore;
	}

	insert.insert_pos = posInt;
	insert.item.pszText = (char*)szCaption;
	insert.item.style = XPPS_TOOLBAR;
	insert.item.hImageList = hImageList;
	insert.item.nImage = nImage;
	insert.item.mask |= XPF_TEXT|XPF_IMAGE|XPF_STYLE|XPF_IMAGELIST|XPF_IMAGE;

    // [vanek] : обновляем мапы и переводим внутреннюю позицию во внешнюю - 18.08.2004	 
	TPOS  lpos_added = 0;
	lpos_added = (TPOS)::SendMessage(m_hwndXPBar, XPB_INSERTITEM, 0, (LPARAM)&insert);
	if (!lpos_added)
		return 0;

	_bstr_t bstr_name( szCaption );

		// обновляем мапы
	TPOS lpos_in_extmap = 0;
	lpos_in_extmap = m_map_xppanename2extpos.find(bstr_name);
	if( !lpos_in_extmap )
		lpos_in_extmap = m_map_xppanename2extpos.set(1 + m_map_xppanename2extpos.count(), bstr_name);

	m_map_xppanename2intpos.set( lpos_added, bstr_name );

	// переводим внутреннюю позицию во внешнюю
  long index = m_map_xppanename2extpos.get( lpos_in_extmap );
    
	return index;
}

void CMainFrame::XPExpandPane(long lpos, long state) 
{
	if( !m_hwndXPBar )return;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	::SendMessage( m_hwndXPBar, XPB_EXPANDITEM, (WPARAM)tpos, state );
}


#include "\vt5\awin\misc_map.h"
#include "\vt5\awin\misc_string.h"

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };


void CMainFrame::XPAddButton(long lpos, LPCTSTR pszAction, long nImage, long nState) 
{
	if( !m_hwndXPBar )return;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	XPBUTTON	button;
	ZeroMemory( &button, sizeof( button ) );

	CActionInfoWrp	*pAI = g_CmdManager.GetActionInfo( pszAction );
	if( !pAI )
	{
		CString	str;
		str.Format( "Action %s not found", pszAction );
		return;
	}

	XPPANEITEM	item;
	item.mask = XPF_IMAGELIST;
	::SendMessage(m_hwndXPBar, XPB_GETITEM, (WPARAM)tpos, (LPARAM)&item);

	if( item.hImageList == m_hImageListAction )
	{
		
		static _list_map_t<int, CString, cmp_cstring>	m_action_images_map;
		nImage = -1;

		TPOS	lpos_found = m_action_images_map.find(pszAction);
		if( lpos_found )
		{
			nImage = m_action_images_map.get( lpos_found );
		}
		else
		{
			//add action to image list
			CCommandManager::BitmapData *pBitmapData = pAI->m_pBmpData;
			if( pBitmapData && pAI->GetPictureIdx() != -1 )
			{
				HDC	hdc = ::GetDC( GetSafeHwnd() );
				HDC	hdcMem = ::CreateCompatibleDC( hdc );
				HDC	hdcSrc = ::CreateCompatibleDC( hdc );
				
				HBITMAP	hbmp_single = ::CreateCompatibleBitmap( hdc, 16, 15 );

				::SelectObject( hdcSrc, pBitmapData->hBitmap );
				::SelectObject( hdcMem, hbmp_single );

				::BitBlt( hdcMem, 0, 0, 16, 15, hdcSrc, 16*pAI->GetPictureIdx(), 0, SRCCOPY );
				::DeleteDC( hdcMem );
				::DeleteDC( hdcSrc );
				::ReleaseDC( GetSafeHwnd(), hdc );

				nImage = ::ImageList_AddMasked( m_hImageListAction, hbmp_single, GetSysColor( COLOR_3DFACE ) );
				::DeleteObject( hbmp_single );
			}
			m_action_images_map.set( nImage, pszAction );
		}
	}
	
	//SendMesage

	button.mask = XPBF_TEXT|XPBF_IMAGE|XPBF_STYLE;
	button.nImage = nImage;
	button.style = nState;
	button.string = (char*)(const char*)pAI->GetActionUserName();
	button.command = (unsigned)pAI->GetLocalID();

	::SendMessage(m_hwndXPBar, XPB_ADDBUTTON, (WPARAM)tpos, (LPARAM)&button);
}

void CMainFrame::XPSetButton(long lpos, LPCTSTR pszAction, long nImage, long nState)
{
	if( !m_hwndXPBar )return;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	XPBUTTON	button;
	ZeroMemory( &button, sizeof( button ) );

	CActionInfoWrp	*pAI = g_CmdManager.GetActionInfo( pszAction );
	if( !pAI )
	{
		CString	str;
		str.Format( "Action %s not found", pszAction );
		return;
	}

	XPPANEITEM	item;
	item.mask = XPF_IMAGELIST;
	::SendMessage(m_hwndXPBar, XPB_GETITEM, (WPARAM)tpos, (LPARAM)&item);

	if( item.hImageList == m_hImageListAction )
		button.mask = XPBF_TEXT|XPBF_STYLE;
	else
		button.mask = XPBF_TEXT|XPBF_IMAGE|XPBF_STYLE;
	button.nImage = nImage;
	button.style = nState;
	button.string = (char*)(const char*)pAI->GetActionUserName();
	button.command = (unsigned)pAI->GetLocalID();

	::SendMessage( m_hwndXPBar, XPB_SETBUTTON, (WPARAM)tpos, (LPARAM)&button );
}

void CMainFrame::XPRemovePane(long lpos) 
{
	if( !m_hwndXPBar )return;

    // [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	if( ::SendMessage( m_hwndXPBar, XPB_REMOVEITEM, lpos, 0 ) )
	{	// удаляем lpos из внутренней мапы 
		TPOS lpos_del = 0;
		lpos_del = xppane_find_by_pos( tpos, 0 );
		if( lpos_del )
			m_map_xppanename2intpos.remove( lpos_del );
    }
}

void CMainFrame::XPEnsureVisible(long lpos) 
{
	if( !m_hwndXPBar )return;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	::SendMessage( m_hwndXPBar, XBP_ENSUREVISIBLE, (WPARAM)tpos, 0 );
}

long CMainFrame::CreateImageList(LPCTSTR szFileName, long cx, long cy, long nTransparent) 
{
	HIMAGELIST	hImageList = ::ImageList_Create( cx, cy, ILC_COLOR24|ILC_MASK, 4, 4);
	HBITMAP	hBitmap = (HBITMAP)::LoadImage( 0, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

	if( !hBitmap )
	{
		CString	str;
		str.Format( "Can't open image list bitmap %s", szFileName );
		AfxMessageBox( str );
		return 0;
	}

	ImageList_AddMasked( hImageList, hBitmap, (COLORREF)nTransparent );
	::DeleteObject( hBitmap );

	return (long)m_listImageList.AddTail( hImageList );
}

void CMainFrame::DestroyImageList(long lImageList) 
{
	POSITION	pos = (POSITION)lImageList;
	HIMAGELIST	hImageList= m_listImageList.GetAt( pos );
	m_listImageList.RemoveAt( pos );

	::ImageList_Destroy( hImageList );
}

void CMainFrame::DestroyAllImageLists() 
{
	for( POSITION pos = m_listImageList.GetHeadPosition(); pos; ImageList_Destroy( m_listImageList.GetNext( pos ) ) );
	m_listImageList.RemoveAll();
}

void CMainFrame::XPSetPaneParam(long lpos, long param) 
{
	if( !m_hwndXPBar )return;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	XPPANEITEM	item;
	item.mask = XPF_PARAM;
	item.lParam = param;
	::SendMessage( m_hwndXPBar, XPB_SETITEM, (WPARAM)tpos, (LPARAM)&item );
}

long CMainFrame::XPGetPaneParam(long lpos)
{
	if( !m_hwndXPBar )return 0;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	XPPANEITEM	item;
	item.mask = XPF_PARAM;
	::SendMessage( m_hwndXPBar, XPB_GETITEM, (WPARAM)tpos, (LPARAM)&item );
	 
	return item.lParam;
}

long CMainFrame::XPGetFirstPane() 
{
	if( !m_hwndXPBar )return 0;

	TPOS lint_pos = 0;
	lint_pos=(TPOS)::SendMessage(m_hwndXPBar, XPB_GETFIRSTITEM, 0, 0);

	// [vanek] : переводим возвращаемое значение внутренней позиции паны во внешнюю - 18.08.2004
	long lext_pos = 0;
	lext_pos=xppane_pos_int2ext(lint_pos);

	// если текущая пана не имеет внешней позиции - ищем дальше
	if( lint_pos && !lext_pos )
		lext_pos = XPGetNextPane( lext_pos );

	return lext_pos;
}

long CMainFrame::XPGetNextPane(long lpos)
{
	if( !m_hwndXPBar )return 0;

	// [vanek] : переводим lpos во внутреннюю позицию паны - 18.08.2004
	TPOS tpos = xppane_pos_ext2int( lpos );

	long lext_pos = 0;
	TPOS lint_pos = 0;
	// ищем следующую пану, которая имеет внешнюю позиицю
  do
	{
		lint_pos = (TPOS)::SendMessage( m_hwndXPBar, XPB_GETNEXTITEM, (WPARAM)lint_pos, 0 );

		// переводим возвращаемое значение внутренней позиции паны во внешнюю
		lext_pos = xppane_pos_int2ext( lint_pos  );
	}
	while( lint_pos && !lext_pos );

	return lext_pos;
}

void CMainFrame::XPSetImageList(long lImageList) 
{
	if( !m_hwndXPBar )return;
	if( !lImageList )
	{
		AfxMessageBox( "Invalid image list to MainWnd::XPSetImageList" );
		return;
	}
	POSITION	pos = (POSITION)lImageList;
	HIMAGELIST	hImageList= m_listImageList.GetAt( pos );
	::SendMessage( m_hwndXPBar, XPB_SETIMAGELIST, 0, (LPARAM)hImageList );
}

void CMainFrame::LogFontSetBold(TPOS lpos, long fBold) 
{
	render	*p = get_render( lpos, -1, 0 );
	_logfont	*plf = logfont_from_render( p );

	if( plf )plf->set_bold( fBold != 0 );
}

void CMainFrame::LogFontSetFace(TPOS lpos, LPCTSTR pszFaceName)
{
	render	*p = get_render( lpos, -1, 0 );
	_logfont	*plf = logfont_from_render( p );

	if( plf )plf->set_face( pszFaceName );
}

void CMainFrame::LogFontSetColor(TPOS lpos, long color)
{
	render	*p = get_render( lpos, -1, 0 );
	_logfont	*plf = logfont_from_render( p );

	if( plf )plf->set_text_color( color );
}

void CMainFrame::LogFontSetHeight(TPOS lpos, long nHeight)
{
	render	*p = get_render( lpos, -1, 0 );
	_logfont	*plf = logfont_from_render( p );

	if( plf )plf->set_height( nHeight );
}

void CMainFrame::RenderSetLayout(TPOS lpos, long left, long top, long right, long bottom)
{
	render	*p = get_render( lpos, -1, 0 );
	p->set_layout( (layout_side)left, 
					(layout_side)top, 
					(layout_side)right, 
					(layout_side)bottom );
}

void CMainFrame::RenderSetRect(TPOS lpos, long left, long top, long right, long bottom)
{
	render	*p = get_render( lpos, -1, 0 );
	p->set_rect( _rect(left, top, right, bottom) );
}

TPOS CMainFrame::RectCreate(TPOS lpos_parent)
{
	render_rect	*p = new render_rect;
	return set_render( p, lpos_parent );
}

void CMainFrame::RectSetColor(TPOS lpos, long color)
{
	render_rect *prect = (render_rect*)
		get_render( lpos, type_render_rect, "RectSetColor" );
	if( !prect )return;

	prect->set_color( color );
}

TPOS CMainFrame::PageCreate(TPOS lpos_parent)
{
	render_page	*p = new render_page;
	p->set_window( m_wndMDIClient );
	return set_render( p, lpos_parent );
}

TPOS CMainFrame::TipCreate(TPOS lpos_parent)
{
	render_tip	*p = new render_tip;
	return set_render( p, lpos_parent );
}

void CMainFrame::TipAddText(TPOS lpos, LPCTSTR psz) 
{
	render_tip *ptype = (render_tip*)
		get_render( lpos, type_render_tip, "TipAddText" );
	if( !ptype )return;

	ptype->add_text( psz );
}

void CMainFrame::TipSetIcon(TPOS lpos, LPCTSTR pszFileName)
{
	render_tip *ptype = (render_tip*)
		get_render( lpos, type_render_tip, "TipSetIcon" );
	if( !ptype )return;

	HICON	h = (HICON)::LoadImage( 0, pszFileName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE );
	if( !h )
	{
		char	sz[MAX_PATH];
		sprintf( sz, "Can't extract icon %s", pszFileName );
		AfxMessageBox( sz );
	}
	ptype->set_icon( h );
}

TPOS CMainFrame::ImageCreate(TPOS lpos_parent)
{
	render_image	*p = new render_image;
	return set_render( p, lpos_parent );
}

void CMainFrame::ImageSetBitmap(TPOS lpos, LPCTSTR pszFileName)
{
	render_image *pimage = (render_image*)
		get_render( lpos, type_render_image, "ImageSetBitmap" );
	if( !pimage )return;

	HBITMAP	hbmp = (HBITMAP)
		::LoadImage( 0, pszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( hbmp == 0 )
	{
		char	sz[MAX_PATH];
		sprintf( sz, "Can't load bitmap %s", pszFileName );
		AfxMessageBox( sz );
	}
	pimage->set_bitmap( hbmp );
}

void CMainFrame::ImageSetTransparent(TPOS lpos, long color)
{
	render_image *pimage = (render_image*)
		get_render( lpos, type_render_image, "ImageSetTransparent" );
	if( !pimage )return;

	pimage->set_transparent( color );
}

TPOS CMainFrame::TextCreate(TPOS lpos_parent)
{
	render_text	*p = new render_text;
	return set_render( p, lpos_parent );
}

void CMainFrame::TextAddText(TPOS lpos, LPCTSTR psz)
{
	render_text *ptext = (render_text*)
		get_render( lpos, type_render_text, "TextAddText" );
	if( !ptext )return;

	ptext->add_text( psz );
}

TPOS CMainFrame::MenuCreate(TPOS lpos_parent)
{
	render_menu	*p = new render_menu;
	p->set_target( *this );
	p->m_fModifyFont = ::GetValueInt( GetAppUnknown(), "\\Interface", "ModifyFont", 1 )!=0;
	p->m_clrHilight = ::GetValueColor( GetAppUnknown(), "\\Interface", "MenuHilightColor", RGB( 224, 231, 184 ) );
	p->m_clrEnabled = ::GetValueColor( GetAppUnknown(), "\\Interface", "MenuColor", RGB( 255, 255, 255 ) );
	p->m_clrDisabled = ::GetValueColor( GetAppUnknown(), "\\Interface", "MenuDisabledColor", RGB( 216, 220, 170 ) );

	return set_render( p, lpos_parent );
}

void CMainFrame::MenuAddItem(TPOS lpos, LPCTSTR pszAction)
{
	render_menu *pmenu = (render_menu*)
		get_render( lpos, type_render_menu, "MenuAddItem" );
	if( !pmenu )return;

	CActionInfoWrp	*pAI = g_CmdManager.GetActionInfo( pszAction );
	if( !pAI )
	{
		CString	str;
		str.Format( "Action %s not found", pszAction );

		AfxMessageBox( str );
		return;
	}

	pmenu->add_item( pAI->GetActionUserName(), pAI->GetLocalID() );

}

void CMainFrame::DeleteAllDrawing() 
{
	delete m_wndMDIClient.m_prender;
	m_wndMDIClient.m_prender = 0;	
	SetWindowLong( m_wndMDIClient, GWL_EXSTYLE, GetWindowLong( m_wndMDIClient, GWL_EXSTYLE ) | WS_EX_CLIENTEDGE );
	m_wndMDIClient.Invalidate();
}

render *CMainFrame::get_render(TPOS lpos, long type, const char *pfunc )
{
	render	*p = 0;
	if( lpos == 0 ) return 0;
	else if( lpos == (TPOS)(-1) ) p = m_wndMDIClient.m_prender;
	else p = render_from_pos( lpos );

	if( type != -1 && p->type() != type )
	{
		char	sz[200];
		sprintf( sz, "Invalid render type specified for %s", pfunc );
		AfxMessageBox( sz );
		return 0;
	}

	return p;
}

TPOS CMainFrame::set_render(render *p, TPOS lpos_p)
{
	render *pp = get_render( lpos_p, -1, 0 );
	if( !pp )
	{
		delete m_wndMDIClient.m_prender;
		m_wndMDIClient.m_prender = p;
		m_wndMDIClient.Invalidate();

		return (TPOS)(-1);
	}
	else
		return pp->insert_child( p );
}



void CMainFrame::MDIClientUpdate() 
{
	if( !m_wndMDIClient.m_prender )
		return;
	_rect	rect;
	m_wndMDIClient.GetClientRect( &rect );
	m_wndMDIClient.m_prender->layout( rect );
	m_wndMDIClient.Invalidate();

}

void CMainFrame::IdleUpdate() 
{
	theApp.ForceIdleUpdate();
}


void CMainFrame::UpdateClientStyle()
{
	if( !m_hwndXPBar )
		return;
	if( m_wndMDIClient.GetExStyle() & WS_EX_CLIENTEDGE )
		m_wndMDIClient.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
}

void find_menus( render	*pbase, _list_t<render_menu*> &menus )
{
	for( TPOS lpos = pbase->m_childs.head(); lpos; lpos = pbase->m_childs.next( lpos ) )
	{
		render	*p = pbase->m_childs.get( lpos );
		if( p->type() == type_render_menu)
			menus.insert_before( (render_menu*)p, 0 );
		find_menus( p, menus );
	}
}

LRESULT CMainFrame::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	if( m_hwndXPBar )
	{
		for( long lpos = ::SendMessage( m_hwndXPBar, XPB_GETFIRSTITEM, 0, 0 );
			lpos; lpos = ::SendMessage( m_hwndXPBar, XPB_GETNEXTITEM, lpos, 0 ) )
		{
				XPPANEITEM	item;
				item.mask = XPF_STYLE;
				::SendMessage( m_hwndXPBar, XPB_GETITEM, lpos, (LPARAM)&item );

				if( item.style == XPPS_TOOLBAR )
				{
					for( long lpos_b = ::SendMessage( m_hwndXPBar, XPB_GETFIRSTBUTTON, lpos, 0 );
						lpos_b; lpos_b = ::SendMessage( m_hwndXPBar, XPB_GETNEXTBUTTON, lpos, lpos_b ) )
					{
						XPBUTTON	button;
						button.mask = XPBF_STYLE;

						::SendMessage( m_hwndXPBar, XPB_GETBUTTONBYPOS, lpos_b, (LPARAM)&button );

						CActionInfoWrp	*pAI = g_CmdManager.GetActionInfo( button.command-ID_CMDMAN_BASE );
						if( pAI )
						{
							_bstr_t	bstrActionName( pAI->GetActionShortName() );
							DWORD	dwStyle = 0;
							if( m_pAM )
								m_pAM->GetActionFlags( bstrActionName, &dwStyle );

							if( dwStyle != button.style )
							{
								button.style = dwStyle;
								::SendMessage( m_hwndXPBar, XPB_SETBUTTONBYPOS, lpos_b, (LPARAM)&button );
							}
						}
					}
				}
		}
	}

	if( m_wndMDIClient.m_prender )
	{
		_list_t<render_menu*>	menus;
		find_menus( m_wndMDIClient.m_prender, menus );
		_point	point;
		GetCursorPos( &point );
		m_wndMDIClient.ScreenToClient( &point );

		for (TPOS lpos_m = menus.head(); lpos_m; lpos_m = menus.next(lpos_m))
		{
			render_menu	*p = menus.get( lpos_m );
			for (TPOS lpos = p->m_items.head(); lpos; lpos = p->m_items.next(lpos))
			{
				render_menu::item*	pitem = p->m_items.get( lpos );

				if( !pitem->visible )continue;

				CActionInfoWrp	*pAI = g_CmdManager.GetActionInfo( pitem->id-ID_CMDMAN_BASE );
				if( pAI )
				{
					_bstr_t	bstrActionName( pAI->GetActionShortName() );
					DWORD	dwStyle = 0;

					if( m_pAM )
						m_pAM->GetActionFlags( bstrActionName, &dwStyle );
				
					bool	bEnabled = (dwStyle & XPBS_ENABLED)==XPBS_ENABLED;
					bool	bChecked = (dwStyle & XPBS_CHECKED)==XPBS_CHECKED;
					bool	bHilight = pitem->rect.pt_in_rect( point );

					if( bEnabled != pitem->enabled ||
						bChecked != pitem->checked ||
						bHilight != pitem->hilighted )
					{
						pitem->enabled = bEnabled;
						pitem->checked = bChecked;
						pitem->hilighted = bHilight;

						m_wndMDIClient.InvalidateRect( &pitem->rect );
					}
				}
			}
		}
	}
	CBCGMDIFrameWnd::OnIdleUpdateCmdUI();
	return 0;
}

BOOL CMainFrame::CanShowMenuBar()
{
	if( GetValueInt( GetAppUnknown(), "\\MainFrame", "ShowMenuBar", 1 ) == 0 )
		return false;

	return CBCGMDIFrameWnd::CanShowMenuBar();
}

LRESULT CMainFrame::OnSetSheetButton( WPARAM id, LPARAM code )
{
	if( !m_hwndXPBar || !m_bInSettingsMode || !m_lposForm )
		return 0;

	if( id == IDAPPLY )
		if( GetValueInt( GetAppUnknown(), "\\xpbar", "ApplyOnCaptioEnabled", 1 ) == 0 )	
			return 0;
	if( id == IDOK )
		if( GetValueInt( GetAppUnknown(), "\\xpbar", "OkOnOnCaptioEnabled", 1 ) == 0 )	
			return 0;
	if( id == IDCANCEL )
		if( GetValueInt( GetAppUnknown(), "\\xpbar", "CancelOnCaptioEnabled", 1 ) == 0 )	
			return 0;

	XP_CAPTION_BUTTON	button;
	button.nCmd = id;
	if( code == SHEET_BUTTON_CREATED )
	{ 
		button.mask = XPCBM_IMAGE;
		button.iImage = __image_from_button( id );

		if( button.iImage != -1 )
		{
			::SendMessage(m_hwndXPBar, XBP_ADDCAPTIONBUTTON, (WPARAM)m_lposForm, (LPARAM)&button);
			return 1;
		}
	}
	else if( code == SHEET_BUTTON_ENABLED ||
		code == SHEET_BUTTON_DISABLED )
	{
		button.mask = XPCBM_STATE;
		button.dwState = (code == SHEET_BUTTON_ENABLED)?XPBS_ENABLED:0;
		::SendMessage(m_hwndXPBar, XBP_SETCAPTIONBUTTON, (WPARAM)m_lposForm, (LPARAM)&button);
	}
	return 0;
}

class CCmdTargetF:public CCmdTarget{
public:
	CCmdTarget* CCmdTargetPtr(IDispatch* pIAppDisp)
	{
		size_t offs=(size_t)&m_xDispatch-(size_t)this;
		return (CCmdTarget*)((size_t)pIAppDisp-offs);
	}
};

static const IID IID_IAppDisp =
{ 0xbbc55473, 0xae6d, 0x4864, { 0xaa, 0xf2, 0x26, 0x96, 0x9b, 0xf, 0x0, 0xcb } };
//CLSID clsidAppDisp={0xdbc8fba9, 0xdffd, 0x4f86, 0x90, 0xb4, 0x66, 0x4a, 0x6c, 0x70, 0x9e, 0x76};
void CMainFrame::UpdateSystemSettings() 
{
	time_test tt("CMainFrame::UpdateSystemSettings");

	IApplicationPtr	ptrA( GetAppUnknown() );
	::FireEvent( ptrA, szEventNewSettings );
	{
		ICompManagerPtr ptrGroupMan( ::GetAppUnknown() );
		if( ptrGroupMan == 0 )
			return;
		IUnknownPtr pIAppDisp;
		HRESULT hr = ptrGroupMan->GetComponentUnknownByIID(IID_IAppDisp,&pIAppDisp);
		IDispatchPtr pD(pIAppDisp);
		CCmdTargetF*pF=(CCmdTargetF*)80;
		CCmdTarget*pT=pF->CCmdTargetPtr((IDispatch*)pD);

		CAppDisp* pAppDisp=(CAppDisp*)pT;
		pAppDisp->UpdateUnits();
	}

	LONG_PTR	lposTempl = 0;

	ptrA->GetFirstDocTemplPosition( &lposTempl );

	while( lposTempl )
	{
		LONG_PTR	lposDoc = 0;
		ptrA->GetFirstDocPosition( lposTempl, &lposDoc );


		while( lposDoc )
		{
			IUnknown	*punk = 0;
			ptrA->GetNextDoc( lposTempl, &lposDoc, &punk );

			::FireEvent( punk, szEventNewSettings );

			ASSERT( punk );
			punk->Release();
		}
		ptrA->GetNextDocTempl( &lposTempl, 0, 0 );
	}
	UpdateAllowCustomizeButton();

	//script notify for LongOperation support
	_init_script_notify( );
}

void CMainFrame::UpdateSystemSettingsForCurrentDoc() 
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	int nType = 1; // Current Doc
	::FireEvent( ptrA, szEventNewSettings, 0, 0 , &nType, sizeof(int)  );

	IUnknown	*punk = 0;
	ptrA->GetActiveDocument( &punk );

	if( punk )
	{
		::FireEvent( punk, szEventNewSettings );
/*
		IDocumentSitePtr ptrDoc = punk;
		punk->Release();
		if( ptrDoc != 0 )
		{
			IUnknown *punkView = 0;
			ptrDoc->GetActiveView( &punkView );
			if( punkView )
			{
				punkView->Release();
				CWnd *pWnd = ::GetWindowFromUnknown( punkView );
				if( pWnd )
					pWnd->Invalidate();
			}

		}
*/
	}

	//script notify for LongOperation support
	_init_script_notify( );
}

void CMainFrame::ShowHiddenForm() 
{
	if( ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", 0 ) != 0 )
	{
		::SetValue( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", long(0) );
		EnterMode(m_nModeHeight);
	}
}

void CMainFrame::UpdateViewSettings() 
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	IUnknown	*punk = 0;
	ptrA->GetActiveDocument( &punk );

	if( punk )
		::FireEvent( punk, szEventNewSettingsView );

	//script notify for LongOperation support
	_init_script_notify( );
}

void CMainFrame::UpdateSystemSettingsForDoc( LPDISPATCH lpDisp ) 
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	int nType = 2; // For doc dispatch
	::FireEvent( ptrA, szEventNewSettings, 0, lpDisp, &nType, sizeof(int)  );

	if( lpDisp )
	{
		::FireEvent( lpDisp, szEventNewSettings );
/*
		IDocumentSitePtr ptrDoc = lpDisp;
		if( ptrDoc != 0 )
		{

			IUnknown *punkView = 0;
			ptrDoc->GetActiveView( &punkView );
			if( punkView )
			{
				punkView->Release();
				CWnd *pWnd = ::GetWindowFromUnknown( punkView );
				if( pWnd )
					pWnd->Invalidate();
			}
		}
*/
	}


	//script notify for LongOperation support
	_init_script_notify( );
}



LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	IUnknown	*punkInter = 0;

	if( m_pAM )
		m_pAM->GetRunningInteractiveActionInfo( &punkInter );

	if( punkInter )
	{
		IActionInfoPtr	ptrAI( punkInter );
		punkInter->Release();
		
		BSTR	bstrHelp=  0;
		ptrAI->GetCommandInfo( 0, 0, &bstrHelp, 0 );
		CString	str = bstrHelp;
		SysFreeString( bstrHelp );

		int	idx = str.Find( '\n' );
		if( idx != -1 )str = str.Left( idx );
		
		m_wndStatusBar.SendMessage( WM_SETTEXT, 0, (LPARAM)(const char*)str );
		return 0;
	}

	if( !wParam && !m_strDefText.IsEmpty() )
	{
		m_wndStatusBar.SendMessage( WM_SETTEXT, 0, (LPARAM)(const char*)m_strDefText );
		return 0;
	}
	return CFrameWnd::OnSetMessageString( wParam, lParam );

}

void CMainFrame::SetLockRecalc( bool bset )
{
	m_bLockRecalc = bset;
}

bool CMainFrame::GetLockRecalc( )
{
	return m_bLockRecalc;
}

BOOL CMainFrame::GetWindowLock() 
{
	return GetLockRecalc( ) == true;
}

void CMainFrame::SetWindowLock(BOOL bWindowLock, BOOL bRecalcLayout) 
{
	SetLockRecalc( bWindowLock == TRUE );
	if( bRecalcLayout )
		RecalcLayout();

}
LRESULT CMainFrame::OnFormCaptionChanged( WPARAM, LPARAM l )
{
	const char *psz = (const char *)l;

	if( m_hwndXPBar && m_lposForm )
	{
		XPPANEITEM	item;
		item.mask = XPF_TEXT;
		item.pszText = (char*)psz;
		::SendMessage(m_hwndXPBar, XPB_SETITEM, (WPARAM)m_lposForm, (LPARAM)&item);
	}
	return 0;
}

BSTR CMainFrame::XPCreateStatusItem(LPCTSTR pszItem) 
{
	if( !m_lposStatus || !m_hwndXPBar )return 0;
	
	XP_STATUS	item;
	item.mask = XPSM_GUID|XPSM_TEXT;
	item.pszText = (char*)pszItem;
	::CoCreateGuid( &item.guid );
	::SendMessage( m_hwndXPBar, XPB_ADDSTATUSPANE, (WPARAM)m_lposStatus, (LPARAM)&item );
//	::SendMessage( m_hwndXPBar, XPB_SETSTATUSPANE, m_lposStatus, (LPARAM)&item );

	BSTR	bstr_sys;
	::StringFromCLSID( item.guid, &bstr_sys );
	_bstr_t	bstr = bstr_sys;
	::CoTaskMemFree( bstr_sys );
	
	return bstr.copy();
}

void CMainFrame::XPSetStatusItem(LPCTSTR pszGUID, LPCTSTR pszText) 
{
	if( !m_lposStatus || !m_hwndXPBar )return;
	GUID	guid;
	
	if( ::CLSIDFromString( _bstr_t(pszGUID), &guid )!= S_OK )
		return;

	XP_STATUS	item;
	item.mask = XPSM_GUID|XPSM_TEXT;
	item.pszText = (char*)pszText;
	item.guid = guid;
	::SendMessage(m_hwndXPBar, XPB_SETSTATUSPANE, (WPARAM)m_lposStatus, (LPARAM)&item);
}

void CMainFrame::XPRemoveStatusItem( LPCTSTR pszGUID ) 
{
	if( !m_lposStatus || !m_hwndXPBar )return;
	GUID	guid;
	
	if( ::CLSIDFromString( _bstr_t(pszGUID), &guid )!= S_OK )
		return;

	::SendMessage(m_hwndXPBar, XBP_REMOVESTATUSPANE, (WPARAM)m_lposStatus, (LPARAM)&guid);
}

void CMainFrame::XPSetDefaultStatusText(LPCTSTR sz) 
{
	if( sz && strlen( sz ) )
		m_strDefText = sz;
	else
		m_strDefText.Empty();
	::SendMessage( m_hWnd, WM_SETMESSAGESTRING, 0, 0 );
}

void CMainFrame::SetAccelTable( HACCEL haccel )
{
	if( m_hAccelTable )
		::DestroyAcceleratorTable( m_hAccelTable );	m_hAccelTable = 0;
	
	m_hAccelTable = haccel;
}
void CMainFrame::OnDestroy()
{
	if( m_nTimerID != -1)
		KillTimer( m_nTimerID );

	m_nTimerID = -1;

	DestroyAllImageLists();

	__super::OnDestroy();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == m_nTimerID )
	{
		static long g_ltick = 0;
		if( g_ltick == 0 )
			g_ltick = ::GetTickCount();

		long ldiff = ::GetTickCount() - g_ltick;
		if( ldiff > 1000*60*5 || ldiff < 0 )//each 5 min
		{
			TIME_TEST("HeapCompact");
			DWORD dw_count = ::GetProcessHeaps( 0, 0 );
			if( dw_count )
			{
				_ptr_t<HANDLE> ptr_heaps;
				HANDLE* pheaps = ptr_heaps.alloc( dw_count );
				if( pheaps )
					if( dw_count == GetProcessHeaps( dw_count, pheaps ) )
						for( DWORD idx=0; idx<dw_count; idx++ )						
							::HeapCompact( pheaps[idx], 0 );
			}
			g_ltick = ::GetTickCount();
		}

		//theApp.OnTimerFire();
	}

	__super::OnTimer(nIDEvent);
}

//script notify for LongOperation support
void		CMainFrame::_init_script_notify( )
{
	m_nEnableScriptNotify = (int) ::GetValueInt( ::GetAppUnknown( ), "LongOperation", "ScriptNotify", 0 );
	m_lStep = ::GetValueInt( ::GetAppUnknown( ), "LongOperation", "ScriptFireStep", 10 ),
	m_lLag =  ::GetValueInt( ::GetAppUnknown( ), "LongOperation", "ScriptFireLag", -1 );
}

HRESULT		CMainFrame::_fire_script_event( LPCTSTR lpctstrEvent, LPCTSTR lpctstrActionName, BOOL bSetPos /*= FALSE*/, long lPos /*= 0*/ )
{
	if( !lpctstrActionName || !lpctstrEvent )
		return S_FALSE;

	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	
	if( sptrScriptSite == 0 )
		return S_FALSE;

	int iParamsCount  = bSetPos ? 2 : 1;
	VARIANT *pvarArgs = 0;
	pvarArgs = new VARIANT[ iParamsCount ];
	for( int i = 0; i < iParamsCount; i++ )
	{
		_variant_t vartArg;
		switch( i )
		{
		case 0:
			 vartArg = lpctstrActionName;
			 break;
		case 1:
			 vartArg = lPos;
			 break;
		}
		*(pvarArgs + i) = vartArg.Detach( );
	}
	

	HRESULT hRes = sptrScriptSite->Invoke( _bstr_t( lpctstrEvent ), pvarArgs, iParamsCount, 0, fwFormScript | fwAppScript );

	if( pvarArgs )
	{
		for( int idx=0; idx<iParamsCount; idx++ )
			::VariantClear( &(pvarArgs[idx]) );
		delete[] pvarArgs;
		pvarArgs = 0;
	}

	return hRes;
}

BOOL		CMainFrame::_get_action_name( IUnknown *punk, CString *pstrActionName )
{
	if( !punk || !pstrActionName )
		return FALSE;

	if( !CheckInterface( punk, IID_IAction ) )
		return FALSE;
	
	IUnknown	*punkAI = 0;
	IActionPtr	ptrA( punk );

	if( ptrA == 0 )
		return FALSE;

	ptrA->GetActionInfo( &punkAI );
    if( !punkAI )
	{
		ptrA = 0;
		return FALSE;
	}

	IActionInfoPtr	ptrAI( punkAI );
	punkAI->Release();

	if( ptrAI == 0 )
		return FALSE;

	BSTR	bstrActionName = 0;
	ptrAI->GetCommandInfo( 0, &bstrActionName, 0, 0 );

	if( bstrActionName )
	{
		*pstrActionName = bstrActionName;
		::SysFreeString( bstrActionName );
		bstrActionName = 0;
	}

	ptrAI = 0;
	return TRUE;
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_USER + 54321 )
	{
		CString strName = CString( (char*)wParam, (int)lParam );
		::ExecuteAction( "ToolsLoadState \"" + strName + "\"");

		::delete (char*)wParam;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

TPOS	CMainFrame::xppane_pos_ext2int( long lpos_ext ) 
{   
	_bstr_t bstr_found_name;
	if( !xppane_find_by_pos( lpos_ext, &bstr_found_name ) )
		return 0;

	TPOS lpos_found = 0;
	lpos_found = m_map_xppanename2intpos.find( bstr_found_name );
	return lpos_found ? m_map_xppanename2intpos.get( lpos_found ): 0;
}

long	CMainFrame::xppane_pos_int2ext( TPOS lpos_int )
{
	_bstr_t bstr_found_name;
	if( !xppane_find_by_pos( lpos_int, &bstr_found_name ) )
		return 0;

	TPOS lpos_found = 0;
	lpos_found = m_map_xppanename2extpos.find( bstr_found_name );
	return lpos_found ? m_map_xppanename2extpos.get( lpos_found ): 0;
}

TPOS	CMainFrame::xppane_find_by_pos(long lpos_find, _bstr_t *pbstr_found_name)
{
	for (TPOS lpos = m_map_xppanename2extpos.head(); lpos; lpos = m_map_xppanename2extpos.next(lpos))
	{
		if (lpos_find == m_map_xppanename2extpos.get(lpos))
		{
			if (pbstr_found_name)
				*pbstr_found_name = m_map_xppanename2intpos.get_key(lpos);

			return lpos;
		}
	}
	return 0;
}

TPOS	CMainFrame::xppane_find_by_pos(TPOS lpos_find, _bstr_t *pbstr_found_name)
{
		for( TPOS lpos = m_map_xppanename2intpos.head(); lpos; lpos = m_map_xppanename2intpos.next( lpos ) )
		{
			if( lpos_find == m_map_xppanename2intpos.get( lpos ) )
			{
				if( pbstr_found_name )
					*pbstr_found_name = m_map_xppanename2intpos.get_key( lpos );

				return lpos;
			}
		}
	return 0;
}

void CMainFrame::SetLockShow(bool block)
{
	m_bLockShow = block;
}

bool CMainFrame::GetLockShow(void)
{
	return m_bLockShow;
}

void CMainFrame::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	if( m_bLockShow && (lpWndPos->flags & SWP_SHOWWINDOW) )
		lpWndPos->flags &= ~SWP_SHOWWINDOW;  

	__super::OnWindowPosChanging( lpWndPos );
}
void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	__super::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}



void CMainFrame::ShowProgress(LPCTSTR text, LONG percent)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	if(percent<0) 
	{	
		m_wndShellProgress.SetPercent(0);
		m_wndShellProgress.LockUpdateText( false );			
		CString	str = ::LanguageLoadCString( AFX_IDS_IDLEMESSAGE );
		m_wndStatusBar.SetWindowText( str );

		return;
	}
	m_wndStatusBar.SetWindowText(text);
	m_wndShellProgress.SetPercent(percent);
	m_wndShellProgress.LockUpdateText( true );
}
