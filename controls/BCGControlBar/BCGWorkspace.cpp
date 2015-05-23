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

//----------------
// By Erwin Tratar
//----------------

#include "stdafx.h"
#include "globals.h"
#include "bcgcontrolbar.h"
#include "BCGToolBar.h"
#include "BCGSizingControlBar.h"
#include "BCGWorkspace.h"

#include "BCGFrameImpl.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "BCGOleIPFrameWnd.h"

#include "BCGMouseManager.h"
#include "BCGContextMenuManager.h"
#include "BCGKeyboardManager.h"
#include "BCGUserToolsManager.h"
#include "RebarState.h"

#include "BCGRegistry.h"
#include "RegPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBCGWorkspace* g_pWorkspace = NULL;
BOOL           g_bWorkspaceAutocreated = FALSE;

CBCGWorkspace* GetWorkspace()
{ 
	//---------------------------------------------------------------------
	// You must either:
	// ----------------
	// a) construct a CBCGWorkspace object
	// b) mix a CBCGWorkspace class somewhere in (e.g. your CWinApp object)
	// c) call CBCGWorkspace::UseWorkspaceManager() to automatically
	//    initialize an object for you
	//---------------------------------------------------------------------
	ASSERT (g_pWorkspace != NULL);
	return g_pWorkspace; 
}

//-----------------------
// clean up if necessary:
//-----------------------
struct _WORKSPACE_TERM
{
	~_WORKSPACE_TERM()
	{
		if (g_pWorkspace != NULL && g_bWorkspaceAutocreated)
		{
			delete g_pWorkspace;
			g_pWorkspace = NULL;
			g_bWorkspaceAutocreated = FALSE;
		}
	}
};
static const _WORKSPACE_TERM workspaceTerm;

//*************************************************************************************

static const CString strRegEntryNameControlBars		= _T("\\ControlBars");
static const CString strWindowPlacementRegSection	= _T("WindowPlacement");
static const CString strRectMainKey					= _T("MainWindowRect");
static const CString strFlagsKey					= _T("Flags");
static const CString strShowCmdKey					= _T("ShowCmd");
static const CString strRegEntryNameSizingBars		= _T("\\SizingBars");

extern CObList	gAllToolbars;
extern CObList	gAllSizingControlBars;

//*************************************************************************************
BOOL CBCGWorkspace::UseWorkspaceManager(LPCTSTR lpszSectionName /*=NULL*/)
{
	if(g_pWorkspace != NULL)
	{
		return FALSE;	// already exists
	}

	g_pWorkspace = new CBCGWorkspace;
	g_bWorkspaceAutocreated = TRUE;	// Cleanup

	if(lpszSectionName != NULL)
	{
		g_pWorkspace->m_strRegSection = lpszSectionName;
	}
	
	return TRUE;
}
//*************************************************************************************
LPCTSTR CBCGWorkspace::SetRegistryBase(LPCTSTR lpszSectionName /*= NULL*/)
{
	m_strRegSection = (lpszSectionName != NULL) ? 
			lpszSectionName : 
			lpszSectionName;

	return m_strRegSection;
}
//*************************************************************************************
CBCGWorkspace::CBCGWorkspace()
{
	// ONLY ONE ALLOWED
	ASSERT(g_pWorkspace == NULL);
	g_pWorkspace = this;

	const CString strRegEntryNameWorkspace = _T("BCGWorkspace");
	m_strRegSection = strRegEntryNameWorkspace;
}
//*************************************************************************************
CBCGWorkspace::~CBCGWorkspace()
{
	// NO OTHER !!
	ASSERT(g_pWorkspace == this);
	g_pWorkspace = NULL;

	// Delete autocreated managers
	if(m_bKeyboardManagerAutocreated && g_pKeyboardManager != NULL)
	{
		delete g_pKeyboardManager;
		g_pKeyboardManager = NULL;
	}

	if (m_bContextMenuManagerAutocreated && g_pContextMenuManager != NULL)
	{
		delete g_pContextMenuManager;
		g_pContextMenuManager = NULL;
	}

	if (m_bMouseManagerAutocreated && g_pMouseManager != NULL)
	{
		delete g_pMouseManager;
		g_pMouseManager = NULL;
	}

	if (m_bUserToolsManagerAutoCreated && g_pUserToolsManager != NULL)
	{
		delete g_pUserToolsManager;
		g_pUserToolsManager = NULL;
	}
}
//*************************************************************************************
BOOL CBCGWorkspace::InitMouseManager()
{
	if (g_pMouseManager != NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	g_pMouseManager = new CBCGMouseManager;
	m_bMouseManagerAutocreated = TRUE;
	return TRUE;
}
//*************************************************************************************
BOOL CBCGWorkspace::InitContextMenuManager()
{
	if (g_pContextMenuManager != NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	g_pContextMenuManager = new CBCGContextMenuManager;
	m_bContextMenuManagerAutocreated = TRUE;
	
	return TRUE;
}
//*************************************************************************************
BOOL CBCGWorkspace::InitKeyboardManager()
{
	if (g_pKeyboardManager != NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	g_pKeyboardManager = new CBCGKeyboardManager;
	m_bKeyboardManagerAutocreated = TRUE;

	return TRUE;
}
//*************************************************************************************
BOOL CBCGWorkspace::EnableUserTools (const UINT uiCmdToolsDummy,
									 const UINT uiCmdFirst, const UINT uiCmdLast,
									CRuntimeClass* pToolRTC)

{
	if (g_pUserToolsManager != NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	g_pUserToolsManager = new 
		CBCGUserToolsManager (uiCmdToolsDummy, uiCmdFirst, uiCmdLast, pToolRTC);
	m_bUserToolsManagerAutoCreated = TRUE;

	return TRUE;
}
//*************************************************************************************
CBCGMouseManager* CBCGWorkspace::GetMouseManager()
{
	if (g_pMouseManager == NULL)
	{
		InitMouseManager ();
	}

	ASSERT_VALID (g_pMouseManager);
	return g_pMouseManager;
}
//*************************************************************************************
CBCGContextMenuManager* CBCGWorkspace::GetContextMenuManager()
{
	if (g_pContextMenuManager == NULL)
	{
		InitContextMenuManager();
	}

	ASSERT_VALID (g_pContextMenuManager);
	return g_pContextMenuManager;
}
//*************************************************************************************
CBCGKeyboardManager* CBCGWorkspace::GetKeyboardManager()
{
	if (g_pKeyboardManager == NULL)
	{
		InitKeyboardManager ();
	}

	ASSERT_VALID (g_pKeyboardManager);
	return g_pKeyboardManager;
}
//*************************************************************************************
CBCGUserToolsManager* CBCGWorkspace::GetUserToolsManager()
{
	return g_pUserToolsManager;
}
//*************************************************************************************
CString	CBCGWorkspace::GetRegSectionPath(LPCTSTR szSectionAdd /*=NULL*/)
{
	CString strSectionPath = ::BCGGetRegPath (m_strRegSection);
	if (szSectionAdd != NULL && _tcslen (szSectionAdd) != 0)
	{
		strSectionPath += szSectionAdd;
		strSectionPath += _T("\\");
	}

	return strSectionPath;
}
//*************************************************************************************
BOOL CBCGWorkspace::LoadState (LPCTSTR lpszSectionName /*=NULL*/, CBCGFrameImpl* pFrameImpl /*= NULL*/)
{
	if (lpszSectionName != NULL)
	{
		m_strRegSection = lpszSectionName;
	}

	CString strSection = GetRegSectionPath ();

	//-----------------------------
	// Other things to do before ?:
	//-----------------------------
	PreLoadState();

	//--------------------------------------
	// Save general toolbar/menu parameters:
	//--------------------------------------
	CBCGToolBar::LoadParameters (strSection);

	if (pFrameImpl != NULL) 
	{
		//-----------------------------------------------------
		// Load all toolbars, menubar and docking control bars:
		//-----------------------------------------------------
		for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
			ASSERT (pToolBar != NULL);

			if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
			{
				ASSERT_VALID(pToolBar);

				if (!pToolBar->m_bLocked && 
					!pFrameImpl->IsUserDefinedToolbar(pToolBar)) 
				{
					pToolBar->LoadState (strSection);
				}
			}
		}

		for (POSITION posCb = gAllSizingControlBars.GetHeadPosition (); posCb != NULL;)
		{
			CBCGSizingControlBar* pBar = (CBCGSizingControlBar*) gAllSizingControlBars.GetNext (posCb);
			ASSERT (pBar != NULL);

			if (CWnd::FromHandlePermanent (pBar->m_hWnd) != NULL)
			{
				ASSERT_VALID (pBar);
				pBar->LoadState (m_strRegSection + strRegEntryNameSizingBars);
			}
		}

		//----------------------------
		// Load user defined toolbars:
		//----------------------------
		pFrameImpl->LoadUserToolbars ();

		CDockState dockState;
		dockState.LoadState(m_strRegSection + strRegEntryNameControlBars);

		if (pFrameImpl->IsDockStateValid (dockState))
		{
			pFrameImpl->m_pFrame->SetDockState (dockState);
		}

		//-------------------
		// Load rebars state:
		//-------------------
		CBCGRebarState::LoadState (strSection, pFrameImpl->m_pFrame);
	}

	//--------------------------------------
	// Load mouse/keyboard/menu managers:
	//--------------------------------------
	if (g_pMouseManager != NULL)
	{
		g_pMouseManager->LoadState (strSection);
	}

	if (g_pContextMenuManager != NULL)
	{
		g_pContextMenuManager->LoadState(strSection);
	}

	if (g_pKeyboardManager != NULL)
	{
		g_pKeyboardManager->LoadState (strSection,
			pFrameImpl == NULL ? NULL : pFrameImpl->m_pFrame);
	}

	if (g_pUserToolsManager != NULL)
	{
		g_pUserToolsManager->LoadState (strSection);
	}

	//----------
	// Call Hook
	//----------
	LoadCustomState();
	return true;
}
//*************************************************************************************
BOOL CBCGWorkspace::LoadState (CBCGMDIFrameWnd* pFrame, LPCTSTR lpszSectionName /*=NULL*/)
{ 
	ASSERT_VALID (pFrame);
	return LoadState (lpszSectionName, &pFrame->m_Impl); 
}
//*************************************************************************************
BOOL CBCGWorkspace::LoadState (CBCGFrameWnd* pFrame, LPCTSTR lpszSectionName /*=NULL*/)
{ 
	ASSERT_VALID (pFrame);
	return LoadState (lpszSectionName, &pFrame->m_Impl);
}
//***********************************************************************************
BOOL CBCGWorkspace::LoadState (CBCGOleIPFrameWnd* pFrame, LPCTSTR lpszSectionName /*=NULL*/)
{ 
	ASSERT_VALID (pFrame);
	return LoadState (lpszSectionName, &pFrame->m_Impl);
}
//*************************************************************************************
BOOL CBCGWorkspace::CleanState (LPCTSTR lpszSectionName /*=NULL*/)
{
	if (lpszSectionName != NULL)
	{
		m_strRegSection = lpszSectionName;
	}

	CString strSection = GetRegSectionPath ();

	CBCGRegistry reg (FALSE, FALSE);
	return reg.DeleteKey(strSection);
}
//*************************************************************************************
BOOL CBCGWorkspace::SaveState (LPCTSTR lpszSectionName  /*=NULL*/, CBCGFrameImpl* pFrameImpl /*= NULL*/)
{
	if (lpszSectionName != NULL)
	{
		m_strRegSection = lpszSectionName;
	}

	CString strSection = GetRegSectionPath ();

	//-----------------------------
	// Other things to do before ?:
	//-----------------------------
	PreSaveState();

	//--------------------------------------
	// Save general toolbar/menu parameters:
	//--------------------------------------
	CBCGToolBar::SaveParameters (strSection);

	if (pFrameImpl != NULL) 
	{
		CDockState dockState;
		
		pFrameImpl->m_pFrame->GetDockState (dockState);
		dockState.SaveState (m_strRegSection + strRegEntryNameControlBars);

		//-----------------------------------------------------
		// Save all toolbars, menubar and docking control bars:
		//-----------------------------------------------------
		for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
			ASSERT (pToolBar != NULL);

			if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
			{
				ASSERT_VALID(pToolBar);

				if (!pToolBar->m_bLocked && 
					!pFrameImpl->IsUserDefinedToolbar (pToolBar))
				{
					pToolBar->SaveState (strSection);
				}
			}
		}

		for (POSITION posCb = gAllSizingControlBars.GetHeadPosition (); posCb != NULL;)
		{
			CBCGSizingControlBar* pBar = (CBCGSizingControlBar*) gAllSizingControlBars.GetNext (posCb);
			ASSERT (pBar != NULL);

			if (CWnd::FromHandlePermanent (pBar->m_hWnd) != NULL)
			{
				ASSERT_VALID (pBar);
				pBar->SaveState (m_strRegSection + strRegEntryNameSizingBars);
			}
		}

		//----------------------------
		// Save user defined toolbars:
		//----------------------------
		pFrameImpl->SaveUserToolbars ();

		//-------------------
		// Save rebars state:
		//-------------------
		CBCGRebarState::SaveState (strSection, pFrameImpl->m_pFrame);
	}

	//------------------
	// Save user images:
	//------------------
	if (CBCGToolBar::m_pUserImages != NULL)
	{
		ASSERT_VALID (CBCGToolBar::m_pUserImages);
		CBCGToolBar::m_pUserImages->Save ();
	}

	//--------------------------------------
	// Save mouse/keyboard/menu managers:
	//--------------------------------------
	if (g_pMouseManager != NULL)
	{
		g_pMouseManager->SaveState (strSection);
	}

	if (g_pContextMenuManager != NULL)
	{
		g_pContextMenuManager->SaveState (strSection);
	}

	if (g_pKeyboardManager != NULL)
	{
		g_pKeyboardManager->SaveState (strSection,
			pFrameImpl == NULL ? NULL : pFrameImpl->m_pFrame);
	}

	if (g_pUserToolsManager != NULL)
	{
		g_pUserToolsManager->SaveState (strSection);
	}

	SaveCustomState();
	return TRUE;
}

//*************************************************************************************
// Overidables for customization

void CBCGWorkspace::OnClosingMainFrame (CBCGFrameImpl* pFrame)
{
	// Defaults to automatically saving state.
	SaveState(0, pFrame);
}

//--------------------------------------------------------
// the next one have to be called explicitly in your code:
//--------------------------------------------------------
BOOL CBCGWorkspace::OnViewDoubleClick (CWnd* pWnd, int iViewId)
{
	if (g_pMouseManager == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT_VALID (g_pMouseManager);

	UINT uiCmd = g_pMouseManager->GetViewDblClickCommand (iViewId);
	if (uiCmd > 0 && uiCmd != (UINT) -1)
	{
		if (g_pUserToolsManager != NULL &&
			g_pUserToolsManager->InvokeTool (uiCmd))
		{
			return TRUE;
		}

		CWnd* pTargetWnd = (pWnd == NULL) ? 
			AfxGetMainWnd () : 
			pWnd->GetTopLevelFrame ();
		ASSERT_VALID (pTargetWnd);

		pTargetWnd->SendMessage (WM_COMMAND, uiCmd);
		return TRUE;
	}

	MessageBeep ((UINT) -1);
	return FALSE;
}
//***********************************************************************************
BOOL CBCGWorkspace::ShowPopupMenu (UINT uiMenuResId, const CPoint& point, CWnd* pWnd)
{
	if (g_pContextMenuManager == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT_VALID (g_pContextMenuManager);
	return g_pContextMenuManager->ShowPopupMenu (uiMenuResId,
				point.x, point.y, pWnd);
}
//***********************************************************************************
BOOL CBCGWorkspace::LoadWindowPlacement (
					CRect& rectNormalPosition, int& nFlags, int& nShowCmd)
{
	CBCGRegistry reg (FALSE, TRUE);
	if (!reg.Open (GetRegSectionPath (strWindowPlacementRegSection)))
	{
		return FALSE;
	}

	return	reg.Read (strRectMainKey, rectNormalPosition) &&
			reg.Read (strFlagsKey, nFlags) &&
			reg.Read (strShowCmdKey, nShowCmd);
}
//***********************************************************************************
BOOL CBCGWorkspace::StoreWindowPlacement (
					const CRect& rectNormalPosition, int nFlags, int nShowCmd)
{
	CBCGRegistry reg (FALSE, FALSE);
	if (!reg.CreateKey (GetRegSectionPath (strWindowPlacementRegSection)))
	{
		return FALSE;
	}

	return	reg.Write (strRectMainKey, rectNormalPosition) &&
			reg.Write (strFlagsKey, nFlags) &&
			reg.Write (strShowCmdKey, nShowCmd);
}
//*************************************************************************************
//*************************************************************************************
// These functions load and store values from the "Custom" subkey
// To use subkeys of the "Custom" subkey use GetSectionInt() etc.
// instead
int CBCGWorkspace::GetInt(LPCTSTR lpszEntry, int nDefault /*= 0*/)
{
	return GetSectionInt(_T(""), lpszEntry, nDefault);
}
//*************************************************************************************
CString	CBCGWorkspace::GetString(LPCTSTR lpszEntry, LPCTSTR lpszDefault /*= ""*/)
{
	return GetSectionString(_T(""), lpszEntry, lpszDefault);
}
//*************************************************************************************
BOOL CBCGWorkspace::GetBinary(LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes)
{
	return GetSectionBinary(_T(""), lpszEntry, ppData, pBytes);
}
//*************************************************************************************
BOOL CBCGWorkspace::GetObject(LPCTSTR lpszEntry, CObject& obj)
{
	return GetSectionObject(_T(""), lpszEntry, obj);
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteInt(LPCTSTR lpszEntry, int nValue )
{
	return WriteSectionInt(_T(""), lpszEntry, nValue);
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteString(LPCTSTR lpszEntry, LPCTSTR lpszValue )
{
	return WriteSectionString(_T(""), lpszEntry, lpszValue);
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteBinary(LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes)
{
	return WriteSectionBinary(_T(""), lpszEntry, pData, nBytes);
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteObject(LPCTSTR lpszEntry, CObject& obj)
{
	return WriteSectionObject(_T(""), lpszEntry, obj);
}
//*************************************************************************************
//*************************************************************************************
// These functions load and store values from a given subkey
// of the "Custom" subkey. For simpler access you may use
// GetInt() etc.
int CBCGWorkspace::GetSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nDefault /*= 0*/)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	
	int nRet = nDefault;

	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, TRUE);
	if (reg.Open (strSection))
	{
		reg.Read (lpszEntry, nRet);
	}
	return nRet;
}
//*************************************************************************************
CString CBCGWorkspace::GetSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault /*= ""*/)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT(lpszDefault);
	
	CString strRet = lpszDefault;

	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, TRUE);
	if (reg.Open (strSection))
	{
		reg.Read (lpszEntry, strRet);
	}
	return strRet;
}
//*************************************************************************************
BOOL CBCGWorkspace::GetSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT(ppData);
	
	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, TRUE);
	if (reg.Open (strSection) 
		&& reg.Read (lpszEntry, ppData, pBytes) ) 
	{
		return TRUE;
	}
	return FALSE;
}
//*************************************************************************************
BOOL CBCGWorkspace::GetSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT_VALID(&obj);
	
	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, TRUE);
	if (reg.Open (strSection) && reg.Read (lpszEntry, obj)) 
	{
		return TRUE;
	}
	return FALSE;
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nValue )
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	
	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, FALSE);
	if (reg.CreateKey (strSection))
	{
		return reg.Write (lpszEntry, nValue);
	}
	return FALSE;
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszValue )
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT(lpszValue);

	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, FALSE);
	if (reg.CreateKey (strSection))
	{
		return reg.Write (lpszEntry, lpszValue);
	}
	return FALSE;
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT(pData);

	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, FALSE);
	if (reg.CreateKey (strSection))
	{
		return reg.Write (lpszEntry, pData, nBytes);
	}
	return FALSE;
}
//*************************************************************************************
BOOL CBCGWorkspace::WriteSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj)
{
	ASSERT(lpszSubSection);
	ASSERT(lpszEntry);
	ASSERT_VALID(&obj);

	CString strSection = GetRegSectionPath(lpszSubSection);

	CBCGRegistry reg (FALSE, FALSE);
	if (reg.CreateKey (strSection))
	{
		return reg.Write (lpszEntry, obj);
	}

	return FALSE;
}
//**********************************************************************************
void CBCGWorkspace::OnAppContextHelp (CWnd* pWndControl, const DWORD dwHelpIDArray [])
{
	// By Sven Ritter
	::WinHelp (pWndControl->GetSafeHwnd (), 
				AfxGetApp()->m_pszHelpFilePath, 
				HELP_CONTEXTMENU, (DWORD)(LPVOID) dwHelpIDArray);
}
