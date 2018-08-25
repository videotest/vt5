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

// bcgtoolbar.cpp : definition of CBCGToolBar
//
// This code is based on the Microsoft Visual C++ sample file
// TOOLBAR.C from the OLDBARS example
//

#include "stdafx.h"

#include "bcgbarres.h"
#include "BCGtoolbar.h"
#include "BCGMenuBar.h"
#include "BCGToolbarButton.h"
#include "BCGToolbarDropSource.h"
#include "ButtonAppearanceDlg.h"
#include "CBCGToolbarCustomize.h"
#include "bcglocalres.h"
#include "BCGRegistry.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "BCGKeyboardManager.h"
#include "BCGToolbarMenuButton.h"
#include "BCGToolbarSystemMenuButton.h"
#include "BCGPopupMenu.h"
#include "CustomizeButton.h"
#include "BCGCommandManager.h"
#include "RegPath.h"
#include "trackmouse.h"
#include "BCGOleIPFrameWnd.h"
#include "BCGUserToolsManager.h"
#include "bcgsound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define TEXT_MARGIN			3
#define STRETCH_DELTA		6
#define BUTTON_MIN_WIDTH	5
#define LINE_OFFSET			5

#define REG_SECTION_FMT					_T("%sBCGToolBar-%d")
#define REG_SECTION_FMT_EX				_T("%sBCGToolBar-%d%x")
#define REG_PARAMS_FMT					_T("%sBCGToolbarParameters")
#define REG_ENTRY_NAME					_T("Name")
#define REG_ENTRY_BUTTONS				_T("Buttons")
#define REG_ENTRY_TOOLTIPS				_T("Tooltips")
#define REG_ENTRY_KEYS					_T("ShortcutKeys")
#define REG_ENTRY_LARGE_ICONS			_T("LargeIcons")
#define REG_ENTRY_ANIMATION				_T("MenuAnimation")
#define REG_ENTRY_RU_MENUS				_T("RecentlyUsedMenus")
#define REG_ENTRY_MENU_SHADOWS			_T("MenuShadows")
#define REG_ENTRY_SHOW_ALL_MENUS_DELAY	_T("ShowAllMenusAfterDelay")
#define REG_ENTRY_CMD_USAGE_COUNT		_T("CommandsUsage")
#define REG_ENTRY_LOOK2000				_T("Look2000")

static const CString strToolbarProfile	= _T("BCGToolBars");

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar notification messages:

UINT BCGM_TOOLBARMENU		= ::RegisterWindowMessage (_T("BCGTOOLBAR_POPUPMENU"));
UINT BCGM_CUSTOMIZETOOLBAR	= ::RegisterWindowMessage (_T("BCGTOOLBAR_CUSTOMIZE"));
UINT BCGM_CREATETOOLBAR		= ::RegisterWindowMessage (_T("BCGTOOLBAR_CREATE"));
UINT BCGM_DELETETOOLBAR		= ::RegisterWindowMessage (_T("BCGTOOLBAR_DELETE"));
UINT BCGM_CUSTOMIZEHELP		= ::RegisterWindowMessage (_T("BCGTOOLBAR_CUSTOMIZEHELP"));
UINT BCGM_RESETTOOLBAR		= ::RegisterWindowMessage (_T("BCGTOOLBAR_RESETTOOLBAR"));
UINT BCGM_SHOWREGULARMENU	= ::RegisterWindowMessage (_T("BCGTOOLBAR_SHOWREGULARMENU"));


/////////////////////////////////////////////////////////////////////////////
// All CBCGToolBar collection:

//!!! andy modification - 2.06.99
bool	CBCGToolbarButton::s_bInitProgMode = true;
__declspec( dllexport ) CObList	gAllToolbars;
///end modification

static void AddToolbarToGlobalList(CBCGToolBar *pToolbarAdd, LPCTSTR lpszName)
{
	CString sName(lpszName);
	bool bInserted = false;
	POSITION pos = gAllToolbars.GetHeadPosition();
	while (pos)
	{
		POSITION posSaved = pos;
		CBCGToolBar *pToolBar = (CBCGToolBar *)gAllToolbars.GetNext(pos);
		CString s;
		if (pToolBar->IsKindOf(RUNTIME_CLASS(CBCGMenuBar)))
			continue;
		pToolBar->GetWindowText(s);
		if (s.CompareNoCase(sName) > 0)
		{
			gAllToolbars.InsertBefore(posSaved, pToolbarAdd);
			bInserted = true;
			break;
		}
	}
	if (!bInserted)
		gAllToolbars.AddTail(pToolbarAdd);
}


BOOL CBCGToolBar::m_bCustomizeMode = FALSE;
BOOL CBCGToolBar::m_bAltCustomizeMode = FALSE;
BOOL CBCGToolBar::m_bShowTooltips = TRUE;
BOOL CBCGToolBar::m_bShowShortcutKeys = TRUE;
BOOL CBCGToolBar::m_bLargeIcons = FALSE;
CBCGToolbarDropSource CBCGToolBar::m_DropSource;

CBCGToolBarImages	CBCGToolBar::m_Images;
CBCGToolBarImages	CBCGToolBar::m_ColdImages;
CBCGToolBarImages	CBCGToolBar::m_MenuImages;
CBCGToolBarImages	CBCGToolBar::m_DisabledImages;
CBCGToolBarImages	CBCGToolBar::m_DisabledMenuImages;
CBCGToolBarImages*	CBCGToolBar::m_pUserImages = NULL;

CSize CBCGToolBar::m_sizeButton = CSize (23, 22);
CSize CBCGToolBar::m_sizeImage	= CSize (16, 15);
CSize CBCGToolBar::m_sizeCurButton = CSize (23, 22);
CSize CBCGToolBar::m_sizeCurImage	= CSize (16, 15);
CSize CBCGToolBar::m_sizeMenuImage	= CSize (-1, -1);
CSize CBCGToolBar::m_sizeMenuButton	= CSize (-1, -1);

BOOL CBCGToolBar::m_bExtCharTranslation = FALSE;

CMap<UINT, UINT, int, int> CBCGToolBar::m_DefaultImages;

bool CBCGToolBar::m_bPressed  = false;
COLORREF CBCGToolBar::m_clrTextHot = (COLORREF) -1;
extern CBCGToolbarCustomize* g_pWndCustomize;

HHOOK CBCGToolBar::m_hookMouseHelp = NULL;
CBCGToolBar* CBCGToolBar::m_pLastHookedToolbar = NULL;

CList<UINT, UINT> CBCGToolBar::m_lstUnpermittedCommands;
CList<UINT, UINT> CBCGToolBar::m_lstBasicCommands;

CCmdUsageCount	CBCGToolBar::m_UsageCount;

BOOL CBCGToolBar::m_bAltCustomization = FALSE;
BOOL CBCGToolBar::m_bLook2000 = FALSE;

static inline BOOL IsSystemCommand (UINT uiCmd)
{
	return (uiCmd >= 0xF000 && uiCmd < 0xF1F0);
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar

IMPLEMENT_SERIAL(CBCGToolBar, CControlBar, VERSIONABLE_SCHEMA | 1)

#pragma warning (disable : 4355)

CBCGToolBar::CBCGToolBar() :
	m_bMenuMode (FALSE),
	m_Impl (this),
	m_bIgnoreSetText (FALSE),
	m_bInCommand (FALSE)
{
	m_iButtonCapture = -1;      // nothing captured
	m_iHighlighted = -1;
	m_iSelected = -1;
	m_iHot = -1;
	m_iDragIndex = -1;
	m_rectDrag.SetRectEmpty ();
	m_pDragButton = NULL;
	m_ptStartDrag = CPoint (-1, -1);
	m_bIsDragCopy = FALSE;

	m_bMasked = FALSE;
	m_bPermament = FALSE;

	m_pCustomizeBtn = NULL;

	//---------------------
	// UISG standard sizes:
	//---------------------
	m_cyTopBorder = m_cyBottomBorder = 1;   // 1 pixel for top/bottom gaps

	m_sizeCurButtonLocked = CSize (23, 22);
	m_sizeCurImageLocked = CSize (16, 15);
	m_sizeButtonLocked = CSize (23, 22);
	m_sizeImageLocked = CSize (16, 15);

	m_bStretchButton = FALSE;
	m_rectTrack.SetRectEmpty ();

	m_iImagesOffset = 0;
	m_uiOriginalResID = 0;

	m_bTracked = FALSE;
	m_ptLastMouse = CPoint (-1, -1);
	m_pWndLastCapture = NULL;
	m_hwndLastFocus = NULL;

	m_bLocked = FALSE;
	m_bShowHotBorder = TRUE;
	m_bGrayDisabledButtons = TRUE;
	m_bLargeIconsAreEnbaled = TRUE;

	m_iRebarPaneWidth = -1;
	m_bTextLabels = FALSE;
	m_bDrawTextLabels = FALSE;
	m_nMaxBtnHeight = 0;

	//andy
	m_bButtonApprernceMode = false;

	//max
	m_bChecked = false;
}

#pragma warning (default : 4355)

//******************************************************************************************
CBCGToolBar::~CBCGToolBar()
{
	RemoveAllButtons ();
}
//******************************************************************************************
BOOL CBCGToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID, const char *pszName)
{
	return CBCGToolBar::CreateEx (pParentWnd, TBSTYLE_FLAT,
							dwStyle,
							CRect(1, 1, 1, 1),
							nID,
							pszName);
}
//******************************************************************************************
BOOL CBCGToolBar::CreateEx (CWnd* pParentWnd, 
							DWORD dwCtrlStyle,
							DWORD dwStyle,
							CRect rcBorders,
							UINT nID,
							const char *pszName )
{
	dwStyle |= CBRS_GRIPPER;

	if (pParentWnd != NULL)
	{
		ASSERT_VALID(pParentWnd);   // must have a parent
	}

	if (rcBorders.left < 1)
	{
		rcBorders.left = 1;	// Otherwise, I have a problem woith a "double" grippers
	}

	if (rcBorders.top < 1)
	{
		rcBorders.top = 1;	// Otherwise, I have a problem woith a "double" grippers
	}

	SetBorders (rcBorders);

	//----------------
	// Save the style:
	//----------------
	m_dwStyle = (dwStyle & CBRS_ALL);
	if (nID == AFX_IDW_TOOLBAR)
	{
		m_dwStyle |= CBRS_HIDE_INPLACE;
	}

	dwStyle &= ~CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
	dwStyle |= dwCtrlStyle;

	OnBarStyleChange( m_dwStyle, m_dwStyle );


	//----------------------------
	// Initialize common controls:
	//----------------------------
	VERIFY (AfxDeferRegisterClass (AFX_WNDCOMMCTLS_REG));

	//-----------------
	// Create the HWND:
	//-----------------
	CRect rect;
	rect.SetRectEmpty();

	//-----------------------------
	// Register a new window class:
	//-----------------------------
	HINSTANCE hInst = AfxGetInstanceHandle();
	UINT uiClassStyle = CS_DBLCLKS;
	HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
	HBRUSH hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	CString strClassName;
	strClassName.Format (_T("BCGToolBar:%x:%x:%x:%x"), 
		(UINT)hInst, uiClassStyle, (UINT)hCursor, (UINT)hbrBackground);

	//---------------------------------
	// See if the class already exists:
	//---------------------------------
	WNDCLASS wndcls;
	if (::GetClassInfo (hInst, strClassName, &wndcls))
	{
		//-----------------------------------------------
		// Already registered, assert everything is good:
		//-----------------------------------------------
		ASSERT (wndcls.style == uiClassStyle);
	}
	else
	{
		//-------------------------------------------
		// Otherwise we need to register a new class:
		//-------------------------------------------
		wndcls.style = uiClassStyle;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = hCursor;
		wndcls.hbrBackground = hbrBackground;
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = strClassName;
		
		if (!AfxRegisterClass (&wndcls))
		{
			AfxThrowResourceException();
		}
	}


	if (!CControlBar::Create (strClassName, pszName, dwStyle, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	return TRUE;
}
//******************************************************************************************
void CBCGToolBar::SetSizes (SIZE sizeButton, SIZE sizeImage)
{
	ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);

	m_sizeButton = sizeButton;
	m_sizeImage = sizeImage;

	m_sizeCurButton = sizeButton;
	m_sizeCurImage = sizeImage;

	m_Images.SetImageSize (m_sizeImage);
	m_ColdImages.SetImageSize (m_sizeImage);
	m_DisabledImages.SetImageSize (m_sizeImage);

	if (m_bLargeIcons)
	{
		m_sizeCurButton.cx *= 2;
		m_sizeCurButton.cy *= 2;

		m_sizeCurImage.cx *= 2;
		m_sizeCurImage.cy *= 2;
	}

	if (m_pUserImages != NULL)
	{
		m_pUserImages->SetImageSize (m_sizeImage);
	}
}
//******************************************************************************************
void CBCGToolBar::SetLockedSizes (SIZE sizeButton, SIZE sizeImage)
{
	ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);

	m_sizeButtonLocked = sizeButton;
	m_sizeImageLocked = sizeImage;

	m_sizeCurButtonLocked = sizeButton;
	m_sizeCurImageLocked = sizeImage;

	m_ImagesLocked.SetImageSize (m_sizeImageLocked);
	m_ColdImagesLocked.SetImageSize (m_sizeImageLocked);
	m_DisabledImagesLocked.SetImageSize (m_sizeImageLocked);

	if (m_bLargeIcons)
	{
		m_sizeCurButtonLocked.cx *= 2;
		m_sizeCurButtonLocked.cy *= 2;

		m_sizeCurImageLocked.cx *= 2;
		m_sizeCurImageLocked.cy *= 2;
	}
}
//******************************************************************************************
void CBCGToolBar::SetHeight(int cyHeight)
{
	ASSERT_VALID (this);

	int nHeight = cyHeight;
	
	if (m_dwStyle & CBRS_BORDER_TOP)
	{
		cyHeight -= afxData.cyBorder2;
	}

	if (m_dwStyle & CBRS_BORDER_BOTTOM)
	{
		cyHeight -= afxData.cyBorder2;
	}

	m_cyBottomBorder = (cyHeight - GetRowHeight ()) / 2;
	
	//-------------------------------------------------------
	// If there is an extra pixel, m_cyTopBorder will get it:
	//-------------------------------------------------------
	m_cyTopBorder = cyHeight - GetRowHeight () - m_cyBottomBorder;
	
	if (m_cyTopBorder < 0)
	{
		TRACE(_T("Warning: CBCGToolBar::SetHeight(%d) is smaller than button.\n"),
			nHeight);
		m_cyBottomBorder += m_cyTopBorder;
		m_cyTopBorder = 0;  // will clip at bottom
	}

	if (GetSafeHwnd () != NULL)
	{
		Invalidate ();
	}
}
//******************************************************************************************
BOOL CBCGToolBar::SetUserImages (CBCGToolBarImages* pUserImages)
{
	ASSERT (pUserImages != NULL);
	if (!pUserImages->IsValid ())
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (m_sizeImage != pUserImages->GetImageSize ())
	{
		ASSERT (FALSE);
		return FALSE;
	}

	m_pUserImages = pUserImages;
	return TRUE;
}
//******************************************************************************************
BOOL CBCGToolBar::SetButtons(const UINT* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));

	//-----------------------
	// Save customize button:
	//-----------------------
	CCustomizeButton* pCustomizeBtn = NULL;
	if (m_pCustomizeBtn != NULL)
	{
		ASSERT_VALID (m_pCustomizeBtn);
		ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last

		pCustomizeBtn = new CCustomizeButton;
		pCustomizeBtn->CopyFrom (*m_pCustomizeBtn);
	}

	RemoveAllButtons ();

	if (lpIDArray == NULL)
	{
		while (nIDCount-- > 0)
		{
			InsertSeparator ();
		}
		
		return TRUE;
	}

	int iImage = m_iImagesOffset;

	//--------------------------------
	// Go through them adding buttons:
	//--------------------------------
	for (int i = 0; i < nIDCount; i ++)
	{
		int iCmd = *lpIDArray ++;

		if (iCmd == 0)	// Separator
		{
			InsertSeparator ();
		}
		else
		{
			if (InsertButton (CBCGToolbarButton (iCmd, iImage, NULL, FALSE, 
				m_bLocked)) >= 0 && !m_bLocked)
			{
				m_DefaultImages.SetAt (iCmd, iImage);
			}

			iImage ++;
		}
	}

	//--------------------------
	// Restore customize button:
	//--------------------------
	if (pCustomizeBtn != NULL)
	{
		InsertButton (pCustomizeBtn);
		m_pCustomizeBtn = pCustomizeBtn;
	}

	if (GetSafeHwnd () != NULL)
	{
		//------------------------------------
		// Allow to produce some user actions:
		//------------------------------------
		OnReset ();
		CWnd* pParentFrame = (m_pDockSite == NULL) ?
			GetParent () : m_pDockSite;
		if (pParentFrame != NULL)
		{
//			pParentFrame->SendMessage (BCGM_RESETTOOLBAR, (WPARAM) m_uiOriginalResID);
		}
	}

	return TRUE;
}
//******************************************************************************************
BOOL CBCGToolBar::LoadBitmap (UINT uiResID, UINT uiColdResID, UINT uiMenuResID, 
							  BOOL bLocked, 
							  UINT uiDisabledResID, UINT uiMenuDisabledResID)
{
	m_bLocked = bLocked;
	if (m_bLocked)
	{
		//------------------------------------------
		// Don't add bitmap to the shared resources!
		//------------------------------------------
		if (!m_ImagesLocked.Load (uiResID, AfxGetResourceHandle (), TRUE))
		{
			return FALSE;
		}

		if (uiColdResID != 0)
		{
			if (!m_ColdImagesLocked.Load (uiColdResID, AfxGetResourceHandle (), TRUE))
			{
				return FALSE;
			}

			ASSERT (m_ImagesLocked.GetCount () == m_ColdImagesLocked.GetCount ());
		}
		
		if (uiMenuResID != 0)
		{
			if (!m_MenuImagesLocked.Load (uiMenuResID, AfxGetResourceHandle (), TRUE))
			{
				return FALSE;
			}

			ASSERT (m_ImagesLocked.GetCount () == m_MenuImagesLocked.GetCount ());
		}

		if (uiDisabledResID != 0)
		{
			if (!m_DisabledImagesLocked.Load (uiDisabledResID, AfxGetResourceHandle (), TRUE))
			{
				return FALSE;
			}
			
			ASSERT (m_ImagesLocked.GetCount () == m_DisabledImagesLocked.GetCount ());
		}
	
		if (uiMenuDisabledResID != 0)
		{
			if (!m_MenuImagesLocked.Load (uiMenuResID, AfxGetResourceHandle (), TRUE))
			{
				return FALSE;
			}

			ASSERT (m_ImagesLocked.GetCount () == m_MenuImagesLocked.GetCount ());
		}
		
		return TRUE;
	}

	if (!m_Images.Load (uiResID, AfxGetResourceHandle (), TRUE))
	{
		return FALSE;
	}
	
	m_iImagesOffset = m_Images.GetResourceOffset (uiResID);
	ASSERT (m_iImagesOffset >= 0);

	if (uiColdResID != 0)
	{
		if (!m_ColdImages.Load (uiColdResID, AfxGetResourceHandle (), TRUE))
		{
			return FALSE;
		}

		ASSERT (m_Images.GetCount () == m_ColdImages.GetCount ());
		ASSERT (m_Images.GetImageSize ().cy == m_ColdImages.GetImageSize ().cy);
	}

	if (uiMenuResID != 0)
	{
		if (!m_MenuImages.Load (uiMenuResID, AfxGetResourceHandle (), TRUE))
		{
			return FALSE;
		}

		ASSERT (m_Images.GetCount () == m_MenuImages.GetCount ());
		ASSERT (m_MenuImages.GetImageSize ().cy == m_sizeMenuImage.cy);
	}

	if (uiDisabledResID != 0)
	{
		if (!m_DisabledImages.Load (uiDisabledResID, AfxGetResourceHandle (), TRUE))
		{
			return FALSE;
		}
		
		ASSERT (m_Images.GetCount () == m_DisabledImages.GetCount ());
	}
	
	if (uiMenuDisabledResID != 0)
	{
		if (!m_DisabledMenuImages.Load (uiMenuDisabledResID, AfxGetResourceHandle (), TRUE))
		{
			return FALSE;
		}
		
		ASSERT (m_Images.GetCount () == m_DisabledMenuImages.GetCount ());
	}

	ASSERT (m_Images.GetImageSize ().cy == m_sizeImage.cy);
	return TRUE;
}
//******************************************************************************************
BOOL CBCGToolBar::LoadToolBar(UINT uiResID, UINT uiColdResID, UINT uiMenuResID, 
							  BOOL bLocked,
							  UINT uiDisabledResID, UINT uiMenuDisabledResID)
{
	struct CToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;

		WORD* items()
			{ return (WORD*)(this+1); }
	};

	ASSERT_VALID(this);

	LPCTSTR lpszResourceName = MAKEINTRESOURCE (uiResID);
	ASSERT(lpszResourceName != NULL);

	//---------------------------------------------------
	// determine location of the bitmap in resource fork:
	//---------------------------------------------------
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];
	ASSERT (pItems != NULL);

	if (bLocked)
	{
		SetLockedSizes (CSize (pData->wWidth + 6, pData->wHeight + 6),
						CSize (pData->wWidth, pData->wHeight));
	}
	else
	{
		SetSizes (	CSize (pData->wWidth + 6, pData->wHeight + 6),
					CSize (pData->wWidth, pData->wHeight));
	}

	BOOL bResult = TRUE;

	if (m_uiOriginalResID != 0 ||
		LoadBitmap (uiResID, uiColdResID, uiMenuResID, bLocked,
		uiDisabledResID, uiMenuDisabledResID))
	{
		int iImageIndex = m_iImagesOffset;
		for (int i = 0; i < pData->wItemCount; i++)
		{
			pItems[i] = pData->items()[i];

			if (!bLocked && pItems [i] > 0)
			{
				m_DefaultImages.SetAt (pItems[i], iImageIndex ++);
			}
		}

		m_uiOriginalResID = uiResID;
		bResult = SetButtons(pItems, pData->wItemCount);

		if (!bResult)
		{
			m_uiOriginalResID = 0;
		}
	}

	delete[] pItems;

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return bResult;
}
//*****************************************************************************************
extern CRuntimeClass *g_pclassToolbarMenuButtonRuntime;
int CBCGToolBar::InsertButton (const CBCGToolbarButton& button, int iInsertAt)
{
	CRuntimeClass* pClass = button.GetRuntimeClass ();
	ASSERT (pClass != NULL);

	//andy
	if( pClass == RUNTIME_CLASS( CBCGToolbarMenuButton ) )
		pClass = g_pclassToolbarMenuButtonRuntime;


	CBCGToolbarButton* pButton = (CBCGToolbarButton*) pClass->CreateObject ();
	ASSERT_VALID(pButton);

	pButton->CopyFrom (button);

	int iIndex = InsertButton (pButton, iInsertAt);
	if (iIndex < 0)
	{
		delete pButton;
	}

	return iIndex;
}
//******************************************************************************************
BOOL CBCGToolBar::ReplaceButton (UINT uiCmd, const CBCGToolbarButton& button)
{
	ASSERT_VALID (this);

	int iIndex = CommandToIndex (uiCmd);
	if (iIndex < 0)
	{
		TRACE(_T("ReplaceButton: Can't find command %d\n"), uiCmd);
		return FALSE;
	}

	RemoveButton (iIndex);
	if (iIndex > GetCount ())
	{
		iIndex = GetCount ();
	}

	return InsertButton (button, iIndex) >= 0;
}
//******************************************************************************************
int CBCGToolBar::InsertButton (CBCGToolbarButton* pButton, int iInsertAt)
{
	ASSERT_VALID( this );
	ASSERT (pButton != NULL);

	if (!IsCommandPermitted (pButton->m_nID))
	{
		return -1;
	}

	if (iInsertAt != -1 &&
		(iInsertAt < 0 || iInsertAt > m_Buttons.GetCount ()))
	{
		return -1;
	}

	if (iInsertAt == -1 || iInsertAt == m_Buttons.GetCount ())
	{
		if (m_pCustomizeBtn != NULL)
		{
			ASSERT_VALID (m_pCustomizeBtn);
			ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last!

			iInsertAt = m_Buttons.GetCount () - 1;
		}
		else
		{
			//-------------------------
			// Add to the toolbar tail:
			//-------------------------
			m_Buttons.AddTail (pButton);
			
			m_DefaultImages.SetAt ( pButton->m_nID, pButton->GetImage() );

			pButton->OnChangeParentWnd (this);

			return m_Buttons.GetCount () - 1;
		}
	}

	POSITION pos = m_Buttons.FindIndex (iInsertAt);
	ASSERT (pos != NULL);

	m_Buttons.InsertBefore (pos, pButton);

	m_DefaultImages.SetAt ( pButton->m_nID, pButton->GetImage() );

	pButton->OnChangeParentWnd (this);

	return iInsertAt;
}
//******************************************************************************************
int CBCGToolBar::InsertSeparator (int iInsertAt)
{
	CBCGToolbarButton* pButton = new CBCGToolbarButton;
	ASSERT (pButton != NULL);

	pButton->m_nStyle = TBBS_SEPARATOR;

	int iNewButtonOndex = InsertButton (pButton, iInsertAt);
	if (iNewButtonOndex == -1)
	{
		delete pButton;
	}

	return iNewButtonOndex;
}
//******************************************************************************************
void CBCGToolBar::RemoveAllButtons ()
{
	ASSERT_VALID( this );
	m_iButtonCapture = -1;      // nothing captured
	m_iHighlighted = -1;
	m_iSelected = -1;

	while (!m_Buttons.IsEmpty ())
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.RemoveHead ();
		ASSERT_VALID (pButton);
		pButton->OnCancelMode ();

		delete pButton;
	}

	m_pCustomizeBtn = NULL;
}
//******************************************************************************************
BOOL CBCGToolBar::RemoveButton (int iIndex)
{
	ASSERT_VALID( this );
	POSITION pos = m_Buttons.FindIndex (iIndex);
	if (pos == NULL)
	{
		return FALSE;
	}

	if (iIndex == m_Buttons.GetCount () - 1 && m_pCustomizeBtn != NULL)
	{
		//-------------------------------------
		// Unable to remove "Customize" button:
		//-------------------------------------
		ASSERT_VALID (m_pCustomizeBtn);
		ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last!
		ASSERT (FALSE);

		return FALSE;
	}

	CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetAt (pos);
	ASSERT_VALID (pButton);

	m_Buttons.RemoveAt (pos);
	pButton->OnCancelMode ();

	delete pButton;

	if (iIndex == m_iSelected)
	{
		m_iSelected = -1;
	}
	else if (iIndex < m_iSelected && m_iSelected >= 0)
	{
		m_iSelected --;
	}

	if (iIndex == m_iButtonCapture)
	{
		m_iButtonCapture = -1;
	}
	else if (iIndex < m_iButtonCapture && m_iButtonCapture >= 0)
	{
		m_iButtonCapture --;
	}

	if (iIndex == m_iHighlighted)
	{
		m_iHighlighted = -1;
		OnChangeHot (m_iHighlighted);
	}
	else if (iIndex < m_iHighlighted && m_iHighlighted >= 0)
	{
		m_iHighlighted --;
		OnChangeHot (m_iHighlighted);
	}

	//-----------------------------------------
	// If last button is separator - remove it:
	//-----------------------------------------
	while (!m_Buttons.IsEmpty ())
	{
		CBCGToolbarButton* pLastButton = (CBCGToolbarButton*) m_Buttons.GetTail ();
		ASSERT_VALID (pLastButton);

		if (pLastButton->m_nStyle & TBBS_SEPARATOR)
		{
			delete m_Buttons.RemoveTail ();
		}
		else
		{
			//----------------------
			// Regular button, stop!
			//----------------------
			break;
		}
	}

	//----------------------------
	// Don't leave two separators:
	//----------------------------
	if (iIndex > 0 && iIndex < m_Buttons.GetCount ())
	{
		CBCGToolbarButton* pPrevButton = GetButton (iIndex - 1);
		ASSERT_VALID (pPrevButton);

		CBCGToolbarButton* pNextButton = GetButton (iIndex);
		ASSERT_VALID (pNextButton);

		if ((pPrevButton->m_nStyle & TBBS_SEPARATOR) &&
			(pNextButton->m_nStyle & TBBS_SEPARATOR))
		{
			RemoveButton (iIndex);
		}
	}

	RebuildAccelerationKeys ();
	return TRUE;
}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

void CBCGToolBar::SetBarStyleEx( DWORD dwStyle )
{
	/*
	ASSERT((dwStyle & CBRS_ALL) == dwStyle);
	ASSERT((dwStyle & CBRS_ALIGN_ANY) == CBRS_ALIGN_TOP ||
		   (dwStyle & CBRS_ALIGN_ANY) == CBRS_ALIGN_BOTTOM ||
		   (dwStyle & CBRS_ALIGN_ANY) == CBRS_ALIGN_LEFT ||
		   (dwStyle & CBRS_ALIGN_ANY) == CBRS_ALIGN_RIGHT);
	*/

	EnableToolTips(dwStyle & CBRS_TOOLTIPS);

	if (m_dwStyle != dwStyle)
	{
		DWORD dwOldStyle = m_dwStyle;
		m_dwStyle = dwStyle;
		OnBarStyleChange(dwOldStyle, dwStyle);
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar attribute access

int CBCGToolBar::CommandToIndex(UINT nIDFind) const
{
	ASSERT_VALID(this);

	int i = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pButton->m_nID == nIDFind)
		{
			return i;
		}
	}

	return -1;
}
//*****************************************************************
UINT CBCGToolBar::GetItemID(int nIndex) const
{
	ASSERT_VALID(this);

	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return 0;
	}

	return pButton->m_nID;
}
//*****************************************************************
void CBCGToolBar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	ASSERT_VALID(this);

	ASSERT(nIndex >= 0 && nIndex < m_Buttons.GetCount ());
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));

	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		*lpRect = CRect (0, 0, 0, 0);
	}
	else
	{
		*lpRect = pButton->Rect ();
	}
}
//*****************************************************************
void CBCGToolBar::GetInvalidateItemRect(int nIndex, LPRECT lpRect) const
{
	//----------------
	// By Guy Hachlili
	//----------------
	ASSERT_VALID(this);
	
	ASSERT(nIndex >= 0 && nIndex < m_Buttons.GetCount ());
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));
	
	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		*lpRect = CRect (0, 0, 0, 0);
	}
	else
	{
		*lpRect = pButton->GetInvalidateRect ();
	}
}
//***************************************************************************
UINT CBCGToolBar::GetButtonStyle(int nIndex) const
{
	ASSERT_VALID( this );
	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return 0;
	}

	return pButton->m_nStyle;
}
//*****************************************************************
int CBCGToolBar::ButtonToIndex (const CBCGToolbarButton* pButton) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pButton);

	int i = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i ++)
	{
		CBCGToolbarButton* pListButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pListButton != NULL);

		if (pListButton == pButton)
		{
			return i;
		}
	}

	return -1;
}
//*****************************************************************
void CBCGToolBar::SetButtonStyle(int nIndex, UINT nStyle)
{
	ASSERT_VALID( this );
	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	UINT nOldStyle = pButton->m_nStyle;
	if (nOldStyle != nStyle)
	{
		// update the style and invalidate
		pButton->m_nStyle = nStyle;

		// invalidate the button only if both styles not "pressed"
		if (!(nOldStyle & nStyle & TBBS_PRESSED))
			InvalidateButton(nIndex);
	}
}
//****************************************************************
CSize CBCGToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout (dwMode);
}
//*************************************************************************************
CSize CBCGToolBar::CalcDynamicLayout (int nLength, DWORD dwMode)
{
	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}

	return CalcLayout(dwMode, nLength);
}
//*************************************************************************************
void CBCGToolBar::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
	ASSERT_VALID(this);

	CBCGToolbarButton* pButton = GetButton(nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);

		nID = 0;
		nStyle = 0;
		iImage = -1;

		return;
	}

	nID = pButton->m_nID;
	nStyle = pButton->m_nStyle;
	iImage = pButton->GetImage ();
}
//*************************************************************************************
void CBCGToolBar::SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage)
{
	ASSERT_VALID(this);

	CBCGToolbarButton* pButton = GetButton(nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT_VALID (pButton);

	pButton->m_nStyle = nStyle;
	pButton->m_nID = nID;
	pButton->SetImage (iImage);

	if ((nStyle & TBBS_SEPARATOR) && iImage > 0) // iImage parameter is a button width!
	{
		AdjustLayout ();
	}

	InvalidateButton(nIndex);
}
//*************************************************************************************
BOOL CBCGToolBar::SetButtonText(int nIndex, LPCTSTR lpszText)
{
	ASSERT_VALID(this);
	ASSERT(lpszText != NULL);

	CBCGToolbarButton* pButton = GetButton(nIndex);
	if (pButton == NULL)
	{
		return FALSE;
	}

	pButton->m_strText = lpszText;
	return TRUE;
}
//*************************************************************************************
CString CBCGToolBar::GetButtonText( int nIndex ) const
{
	ASSERT_VALID(this);

	CBCGToolbarButton* pButton = GetButton(nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return _T("");
	}

	ASSERT_VALID (pButton);

	return pButton->m_strText;
}
//*************************************************************************************
void CBCGToolBar::GetButtonText( int nIndex, CString& rString ) const
{
	ASSERT_VALID(this);

	CBCGToolbarButton* pButton = GetButton(nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		rString.Empty ();
		return;
	}

	ASSERT_VALID (pButton);

	rString = pButton->m_strText;
}
//*************************************************************************************
void CBCGToolBar::DoPaint(CDC* pDCPaint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDCPaint);

	CRect rectClip;
	pDCPaint->GetClipBox (rectClip);

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect (rectClient);

	CDC*		pDC = pDCPaint;
	BOOL		m_bMemDC = FALSE;
	CDC			dcMem;
	CBitmap		bmp;
	CBitmap*	pOldBmp = NULL;

	/*if ( dcMem.CreateCompatibleDC (pDCPaint) &&
		bmp.CreateCompatibleBitmap (pDCPaint, rectClient.Width (),
											  rectClient.Height ()) )
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		m_bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject (&bmp);
		pDC = &dcMem;
	}*/
	//andy
	if ((GetStyle () & TBSTYLE_TRANSPARENT) == 0)
	{
		pDC->FillRect (rectClip, &globalData.brBtnFace);
	}
	else
	{
		m_Impl.GetBackgroundFromParent (pDC);
	}

	OnFillBackground (pDC);

	pDC->SetTextColor (globalData.clrBtnText);
	pDC->SetBkMode (TRANSPARENT);

	CRect rect;
	GetClientRect(rect);

	//-----------------------------------
	// Force the full size of the button:
	//-----------------------------------
	if (bHorz)
	{
		rect.bottom = rect.top + GetRowHeight ();
	}
	else
	{
		rect.right = rect.left + GetColumnWidth ();
	}

	CBCGToolBarImages* pImages = !m_bLocked ? 
		&m_Images : &m_ImagesLocked;
	CBCGToolBarImages* pHotImages = pImages;
	CBCGToolBarImages* pColdImages = !m_bLocked ? 
		&m_ColdImages : &m_ColdImagesLocked;
	CBCGToolBarImages* pMenuImages = !m_bLocked ? 
		&m_MenuImages : &m_MenuImagesLocked;
	CBCGToolBarImages* pDisabledImages = !m_bLocked ?
		&m_DisabledImages : &m_DisabledImagesLocked;
	CBCGToolBarImages* pDisabledMenuImages = !m_bLocked ?
		&m_DisabledMenuImages : &m_DisabledMenuImagesLocked;

	BOOL bDrawImages = pImages->IsValid ();

	BOOL bIsTransparent = (GetStyle () & TBSTYLE_TRANSPARENT);
	if (bIsTransparent)
	{
		//[ay]
		//pHotImages->SetTransparentColor (RGB (192, 192, 192));
	}

	CBCGDrawState ds;
	if (bDrawImages && 
		!pHotImages->PrepareDrawImage (ds, 
			m_bMenuMode ? m_sizeMenuImage : GetImageSize ()))
	{
		return;     // something went wrong
	}

	CFont* pOldFont;
	//[AY]
	//if (bHorz)
	if ( true )
	{
		pOldFont = (CFont*) pDC->SelectObject (&globalData.fontRegular);
	}
	else
	{
		pOldFont = (CFont*) pDC->SelectObject (&globalData.fontVert);
	}

	//--------------
	// Draw buttons:
	//--------------
	int iButton = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		rect = pButton->Rect ();
		CRect rectInter;

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			BOOL bHorzSeparator = bHorz;
			CRect rectSeparator = rect;

			if (pButton->m_bWrap && bHorz)
			{
				rectSeparator.left = rectClient.left;
				rectSeparator.right = rectClient.right;

				rectSeparator.top = pButton->Rect ().bottom;
				rectSeparator.bottom = rectSeparator.top + LINE_OFFSET;

				bHorzSeparator = FALSE;
			}

			if (rectInter.IntersectRect (rectSeparator, rectClip))
			{
				DrawSeparator (pDC, rectSeparator, bHorzSeparator);
			}

			continue;
		}

		if (!rectInter.IntersectRect (rect, rectClip))
		{
			continue;
		}

		BOOL bHighlighted = FALSE;
		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) && !IsCustomizeMode ();

		if (IsCustomizeMode () && !m_bLocked)
		{
			bHighlighted = FALSE;
		}
		else
		{
			if (m_bMenuMode)
			{
				bHighlighted = (iButton == m_iHighlighted);
			}
			else
			{
				bHighlighted = ((iButton == m_iHighlighted ||
								iButton == m_iButtonCapture) &&
								(m_iButtonCapture == -1 ||
								iButton == m_iButtonCapture));
			}
		}

		if (pDC->RectVisible(&rect))
		{
			BOOL bDrawDisabledImages = FALSE;

			if (bDrawImages)
			{
				CBCGToolBarImages* pNewImages = NULL;

				if (pButton->m_bUserButton)
				{
					if (pButton->GetImage () >= 0)
					{
						pNewImages = m_pUserImages;
					}
				}
				else
				{
					if (m_bMenuMode)
					{
						if (bDisabled && pDisabledMenuImages->GetCount () > 0)
						{
							bDrawDisabledImages = TRUE;
							pNewImages = pDisabledMenuImages;
						}
						else if (pMenuImages->GetCount () > 0)
						{
							pNewImages = pMenuImages;
						}
						else
						{
							bDrawDisabledImages = 
								(bDisabled && pDisabledImages->GetCount () > 0);

							pNewImages =  bDrawDisabledImages ? 
											pDisabledImages : pHotImages;
						}
					}
					else	// Toolbar mode
					{
						bDrawDisabledImages = 
							(bDisabled && pDisabledImages->GetCount () > 0);

						pNewImages =  bDrawDisabledImages ? 
										pDisabledImages : pHotImages;

						if (!bHighlighted && !bDrawDisabledImages &&
							(pButton->m_nStyle & TBBS_PRESSED) == 0 &&
							pColdImages->GetCount () > 0 &&
							!pButton->IsDroppedDown ())
						{
							pNewImages = pColdImages;
						}
					}
				}

				if (bDrawImages && pNewImages != pImages && pNewImages != NULL)
				{
					pImages->EndDrawImage (ds);
					
					if (bIsTransparent)
					{
//[ay]это просто бред написан - все мапят цвета при загрузке.
//так что ставим ручками
//						pNewImages->SetTransparentColor (RGB (192, 192, 192));
					}

					pNewImages->PrepareDrawImage (ds,
						m_bMenuMode ? m_sizeMenuImage : GetImageSize ());

					pImages = pNewImages;
				}
			}

			DrawButton (pDC, pButton, bDrawImages ? pImages : NULL, 
						bHighlighted, bDrawDisabledImages);
		}
	}

	//-------------------------------------------------------------
	// Highlight selected button in the toolbar customization mode:
	//-------------------------------------------------------------
	if (m_iSelected >= m_Buttons.GetCount ())
	{
		m_iSelected = -1;
	}

	if (IsCustomizeMode () && m_iSelected >= 0 && !m_bLocked)
	{
		CBCGToolbarButton* pSelButton = GetButton (m_iSelected);
		ASSERT (pSelButton != NULL);

		if (pSelButton != NULL && pSelButton->CanBeStored ())
		{
			CRect rectDrag1 = pSelButton->Rect ();
			if (pSelButton->GetHwnd () != NULL)
			{
				rectDrag1.InflateRect (0, 1);
			}

			pDC->DrawDragRect (&rectDrag1, CSize (2, 2), NULL, CSize (2, 2));
		}
	}

	if (IsCustomizeMode () && m_iDragIndex >= 0 && !m_bLocked)
	{
		DrawDragMarker (pDC);
	}

	pDC->SelectObject (pOldFont);

	if (bDrawImages)
	{
		pImages->EndDrawImage (ds);
	}

	if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//-------------------------------------- 
		pDCPaint->BitBlt (rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(),
					   &dcMem, rectClip.left, rectClip.top, SRCCOPY);

		dcMem.SelectObject(pOldBmp);
	}
}
//*************************************************************************************
BOOL CBCGToolBar::DrawButton(CDC* pDC, CBCGToolbarButton* pButton,
							CBCGToolBarImages* pImages,
							BOOL bHighlighted, BOOL bDrawDisabledImages)
{
	ASSERT_VALID( this );
	ASSERT_VALID (pDC);
	ASSERT_VALID (pButton);

	if (pButton->IsHidden () || !pDC->RectVisible (pButton->Rect ()))
	{
		return TRUE;
	}

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	//---------------------
	// Draw button context:
	//---------------------
	pButton->OnDraw (pDC, pButton->Rect (), pImages, bHorz, 
		IsCustomizeMode () && !m_bLocked, bHighlighted, m_bShowHotBorder,
		m_bGrayDisabledButtons && !bDrawDisabledImages);
	return TRUE;
}
//*************************************************************************************
CBCGToolbarButton* CBCGToolBar::GetButton(int nIndex) const
{
	ASSERT_VALID( this );
	POSITION pos = m_Buttons.FindIndex (nIndex);
	if (pos == NULL)
	{
		return NULL;
	}

	CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetAt (pos);
	ASSERT (pButton != NULL);

	return pButton;
}
//*************************************************************************************
void CBCGToolBar::InvalidateButton(int nIndex)
{
	ASSERT_VALID(this);

	CRect rect;
	GetInvalidateItemRect (nIndex, &rect);	// By Guy Hachlili
	rect.InflateRect (3, 3);

	InvalidateRect (rect);
}
//*************************************************************************************
INT_PTR CBCGToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	if (!m_bShowTooltips)
	{
		return -1;
	}

	// check child windows first by calling CControlBar
	int nHit = CControlBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	// now hit test against CBCGToolBar buttons
	nHit = ((CBCGToolBar*)this)->HitTest(point);
	if (nHit != -1)
	{
		CBCGToolbarButton* pButton = GetButton(nHit);
		if (pButton == NULL)
		{
			return -1;
		}

		if (pTI != NULL)
		{
			CString strTipText;
			if (!OnUserToolTip (pButton, strTipText))
			{
				if ((pButton->m_nID == 0 || pButton->m_nID == (UINT) -1 ||
					pButton->m_bUserButton) &&
					!pButton->m_strText.IsEmpty ())
				{
					// Use button text as tooltip!
					strTipText = pButton->m_strText;

					strTipText.Remove (_T('&'));
				}
				else
				{
					if (g_pUserToolsManager != NULL &&
						g_pUserToolsManager->IsUserToolCmd (pButton->m_nID))
					{
						strTipText = pButton->m_strText;
					}
					else
					{
						TCHAR szFullText [256];

						AfxLoadString (pButton->m_nID, szFullText);
						AfxExtractSubString(strTipText, szFullText, 1, '\n');
					}
				}
			}

			if (strTipText.IsEmpty ())
			{
				return -1;
			}

			if (pButton->m_nID != 0 && pButton->m_nID != (UINT) -1 && 
				m_bShowShortcutKeys)
			{
				//--------------------
				// Add shortcut label:
				//--------------------
				CString strLabel;
				CFrameWnd* pParent = GetParentFrame () == NULL ?
					NULL : GetParentFrame ()->GetTopLevelFrame ();

				if (pParent != NULL &&
					(CBCGKeyboardManager::FindDefaultAccelerator (
						pButton->m_nID, strLabel, pParent, TRUE) ||
					CBCGKeyboardManager::FindDefaultAccelerator (
						pButton->m_nID, strLabel, pParent->GetActiveFrame (), FALSE)))
				{
					strTipText += _T(" (");
					strTipText += strLabel;
					strTipText += _T(')');
				}
			}

			pTI->lpszText = (LPTSTR) ::calloc ((strTipText.GetLength () + 1), sizeof (TCHAR));
			_tcscpy (pTI->lpszText, strTipText);

			GetItemRect(nHit, &pTI->rect);
			pTI->uId = (pButton->m_nID == (UINT) -1) ? 0 : pButton->m_nID;
			pTI->hwnd = m_hWnd;
		}

		nHit = (pButton->m_nID == (UINT) -1) ? 0 : pButton->m_nID;
	}

#if _MSC_VER >= 1300
	//17.12.2002 paul
	CToolTipCtrl* pToolTip = 0;//AfxGetThreadState()->m_pModuleState->m_thread->m_pToolTip;
#else
		CToolTipCtrl* pToolTip = AfxGetThreadState()->m_pToolTip;
#endif
	if (pToolTip != NULL && pToolTip->GetSafeHwnd () != NULL)
	{
		pToolTip->SetFont (&globalData.fontRegular, FALSE);	// By Daniil Kolbasnikov
	}

	return nHit;
}
//*************************************************************************************
int CBCGToolBar::HitTest(CPoint point) // in window relative coords
{
	ASSERT_VALID( this );
	int iButton = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pButton->Rect ().PtInRect (point) && !pButton->IsHidden ())
		{
			return (pButton->m_nStyle & TBBS_SEPARATOR) ? -1 : iButton;
		}
	}

	return -1;      // nothing hit
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar message handlers

BEGIN_MESSAGE_MAP(CBCGToolBar, CControlBar)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CBCGToolBar)
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_APPEARANCE, OnToolbarAppearance)
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_DELETE, OnToolbarDelete)
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_IMAGE, OnToolbarImage)
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, OnToolbarImageAndText)
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_START_GROUP, OnToolbarStartGroup)
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_TEXT, OnToolbarText)
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_RESET, OnBcgbarresToolbarReset)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_COMMAND(ID_BCGBARRES_COPY_IMAGE, OnBcgbarresCopyImage)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_BCGBARRES_TOOLBAR_NEW_MENU, OnBcgbarresToolbarNewMenu)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_PAINT, OnPaint)
END_MESSAGE_MAP()

void CBCGToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	ASSERT_VALID( this );

	int iButton = HitTest(point);

	if (iButton < 0) // nothing hit
	{
		m_iButtonCapture = -1;

		if (IsCustomizeMode () && !m_bLocked)
		{
			int iSelected = m_iSelected;
			m_iSelected = -1;

			if (iSelected != -1)
			{
				InvalidateButton (iSelected);
				UpdateWindow ();
			}

			OnChangeHot (-1);
		}

		if( m_bEnableDrag && m_pDockContext )
		{
			CRect	rect;
			GetWindowRect( &rect );
			ScreenToClient( &rect );

			if( rect.PtInRect( point ) )
				CControlBar::OnLButtonDown(nFlags, point);
		}
		return;
	}

	CBCGToolbarButton* pButton = GetButton(iButton);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	//-----------------------------------------------------------------
	// Check for "Alt-customizible mode" (when ALT key is holded down):
	//-----------------------------------------------------------------
	m_bAltCustomizeMode = FALSE;
	if (m_bAltCustomization &&
		!m_bCustomizeMode &&
		GetAsyncKeyState (VK_MENU) & 0x8000)	// ALT is pressed
	{
		m_bAltCustomizeMode = TRUE;
	}

	if ((!IsCustomizeMode () && !m_bAltCustomizeMode) || m_bLocked)
	{
		m_iButtonCapture = iButton;

		// update the button before checking for disabled status
		UpdateButton(m_iButtonCapture);

		//AY
		if( pButton->OnPreClick( this ) )
		{
			m_iButtonCapture = -1;
			return;     // don't press it
		}

		if (pButton->m_nStyle & TBBS_DISABLED)
		{
			m_iButtonCapture = -1;
			return;     // don't press it
		}

		pButton->m_nStyle |= TBBS_PRESSED;

		InvalidateButton (iButton);

		ShowCommandMessageString (pButton->m_nID);

		DWORD dwWnd = (DWORD)m_hWnd;
		m_bPressed = true;
		if (pButton->OnClick (this, FALSE /* No delay*/))
		{
			if( ::IsWindow( HWND( dwWnd ) ) )
			{
				pButton->m_nStyle &= ~TBBS_PRESSED;

				m_iButtonCapture = -1;
				m_iHighlighted = -1;

				OnChangeHot (m_iHighlighted);

				InvalidateButton (iButton);
			}
		}
		else
		{
			m_pWndLastCapture = SetCapture ();
		}

		if( ::IsWindow( HWND( dwWnd ) ) )
			UpdateWindow(); // immediate feedback
	}
	else
	{
		int iSelected = m_iSelected;
		m_iSelected = iButton;

		CRect rect;
		GetItemRect (iButton, &rect);

		if (iSelected != -1)
		{
			InvalidateButton (iSelected);
		}

		m_pDragButton = GetButton (m_iSelected);
		ASSERT (m_pDragButton != NULL);

		m_bIsDragCopy = (nFlags & MK_CONTROL);

		if (!m_pDragButton->IsEditable () || !CanDragDropButtons() )
		{
			m_iSelected = -1;
			m_pDragButton = NULL;

			if (iSelected != -1)
			{
				InvalidateButton (iSelected);
			}
			return;
		}

		InvalidateButton (iButton);
		UpdateWindow(); // immediate feedback

		if (m_pDragButton->CanBeStretched () &&
			abs (point.x - rect.right) <= STRETCH_DELTA &&
			!m_bAltCustomizeMode)
		{
			m_bStretchButton = TRUE;

			m_rectTrack = m_pDragButton->Rect ();

			if (m_pDragButton->GetHwnd () != NULL)
			{
				m_rectTrack.InflateRect (2, 2);
			}

			m_pWndLastCapture = SetCapture ();
			::SetCursor (globalData.m_hcurStretch);
		}
		else if (m_pDragButton->CanBeStored () &&
			m_pDragButton->OnBeforeDrag ())
		{
			COleDataSource srcItem;
			m_pDragButton->PrepareDrag (srcItem);

			ShowCommandMessageString (pButton->m_nID);

			m_DropSource.m_bDragStarted = FALSE;
			m_ptStartDrag = point;

			HWND hwndSaved = m_hWnd;

			if (m_bAltCustomizeMode)
			{
				m_bCustomizeMode = TRUE;
			}

			DROPEFFECT dropEffect = srcItem.DoDragDrop 
				(DROPEFFECT_COPY|DROPEFFECT_MOVE, &rect, &m_DropSource);

			if (!::IsWindow (hwndSaved))
			{
				if (m_bAltCustomizeMode)
				{
					m_bCustomizeMode = FALSE;
					m_bAltCustomizeMode = FALSE;
				}

				return;
			}

			CPoint ptDrop;
			::GetCursorPos (&ptDrop);
			ScreenToClient (&ptDrop);

			if (m_DropSource.m_bDragStarted &&
				!rect.PtInRect (ptDrop))
			{
				if (dropEffect != DROPEFFECT_COPY && 
					m_pDragButton != NULL &&
					!m_DropSource.m_bEscapePressed)
				{
					//---------------------
					// Remove source button:
					//---------------------
					RemoveButton (ButtonToIndex (m_pDragButton));
					AdjustLayout ();

					Invalidate ();
				}
				else if (m_pDragButton != NULL)
				{
					InvalidateRect (m_pDragButton->Rect ());
				}
			}
			else
			{
				m_iHighlighted = iButton;
				OnChangeHot (m_iHighlighted);
			}

			m_pDragButton = NULL;
			m_ptStartDrag = CPoint (-1, -1);
		}
		else
		{
			m_pDragButton = NULL;
		}
	}

	if (m_bAltCustomizeMode)
	{
		m_bAltCustomizeMode = FALSE;
		SetCustomizeMode (FALSE);

		Invalidate ();
		UpdateWindow();
	}
	m_bPressed = false;
}
//**************************************************************************************
void CBCGToolBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	ASSERT_VALID( this );
	if (IsCustomizeMode () && !m_bLocked)
	{
		if (m_bStretchButton)
		{
			ASSERT_VALID (m_pDragButton);

			if (point.x - m_pDragButton->Rect ().left >= BUTTON_MIN_WIDTH)
			{
				CClientDC dc (this);

				CRect rectTrackOld = m_rectTrack;
				m_rectTrack.right = point.x;
				dc.DrawDragRect (&m_rectTrack, CSize (2, 2), &rectTrackOld, CSize (2, 2));
			}

			::SetCursor (globalData.m_hcurStretch);
		}

		return;
	}

	if (m_ptLastMouse != CPoint (-1, -1) &&
		abs (m_ptLastMouse.x - point.x) < 1 &&
		abs (m_ptLastMouse.y - point.y) < 1)
	{
		m_ptLastMouse = point;
		return;
	}

	m_ptLastMouse = point;

	int iPrevHighlighted = m_iHighlighted;
	m_iHighlighted = HitTest (point);

	if (m_iHighlighted == -1 && GetFocus () == this)
	{
		m_iHighlighted = iPrevHighlighted; 
		return;
	}

	CBCGToolbarButton* pButton = m_iHighlighted == -1 ?
							NULL : GetButton (m_iHighlighted);
	if (pButton != NULL &&
		(pButton->m_nStyle & TBBS_SEPARATOR || 
		(pButton->m_nStyle & TBBS_DISABLED && 
		!AllowSelectDisabled ())))
	{
		m_iHighlighted = -1;
	}

	if (!m_bTracked)
	{
		m_bTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		::BCGTrackMouse (&trackmouseevent);	
	}
	
	if (iPrevHighlighted != m_iHighlighted)
	{
		BOOL bNeedUpdate = FALSE;

		if (m_iButtonCapture != -1)
		{
			CBCGToolbarButton* pTBBCapt = GetButton (m_iButtonCapture);
			ASSERT (pTBBCapt != NULL);
			ASSERT (!(pTBBCapt->m_nStyle & TBBS_SEPARATOR));

			UINT nNewStyle = (pTBBCapt->m_nStyle & ~TBBS_PRESSED);
			if (m_iHighlighted == m_iButtonCapture)
			{
				nNewStyle |= TBBS_PRESSED;
			}

			if (nNewStyle != pTBBCapt->m_nStyle)
			{
				SetButtonStyle (m_iButtonCapture, nNewStyle);
				bNeedUpdate = TRUE;
			}
		}

		if ((m_bMenuMode || m_iButtonCapture == -1 || 
			iPrevHighlighted == m_iButtonCapture) &&
			iPrevHighlighted != -1)
		{
			InvalidateButton (iPrevHighlighted);
			bNeedUpdate = TRUE;
		}

		if ((m_bMenuMode || m_iButtonCapture == -1 || 
			m_iHighlighted == m_iButtonCapture) &&
			m_iHighlighted != -1)
		{
			InvalidateButton (m_iHighlighted);
			bNeedUpdate = TRUE;
		}

		if (bNeedUpdate)
		{
			UpdateWindow ();
		}

		if (m_iHighlighted != -1 && 
			(m_bMenuMode || m_iHighlighted == m_iButtonCapture || m_iButtonCapture == -1))
		{
			ASSERT (pButton != NULL);
			ShowCommandMessageString (pButton->m_nID);
		}
		else if ((m_iButtonCapture == -1 || (m_bMenuMode && m_iHighlighted == -1))
			&& m_hookMouseHelp == NULL)
		{
			GetOwner()->SendMessage (WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
		}


		if( !CBCGToolBar::m_bPressed )
			OnChangeHot (m_iHighlighted);
	}
}
//*************************************************************************************
void CBCGToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ASSERT_VALID( this );
	if (IsCustomizeMode () && !m_bLocked)
	{
		if (m_bStretchButton)
		{
			ASSERT_VALID (m_pDragButton);

			CRect rect = m_pDragButton->Rect ();
			rect.right = point.x;

			if (rect.Width () >= BUTTON_MIN_WIDTH &&
				abs (m_pDragButton->Rect ().right - point.x) > STRETCH_DELTA)
			{
				m_pDragButton->OnSize (rect.Width ());
				AdjustLayout ();
			}

			m_rectTrack.SetRectEmpty ();

			m_pDragButton = NULL;
			m_bStretchButton = FALSE;

			Invalidate ();
			::ReleaseCapture ();

			if (m_pWndLastCapture != NULL)
			{
				m_pWndLastCapture->SetCapture ();
				m_pWndLastCapture = NULL;
			}
		}
		return;
	}

	if (m_iButtonCapture == -1)
	{
		CControlBar::OnLButtonUp(nFlags, point);

		m_ptLastMouse = CPoint (-1, -1);
		OnMouseMove (0, point);
		return;     // not captured
	}

	::ReleaseCapture();
	if (m_pWndLastCapture != NULL)
	{
		if(m_pWndLastCapture != this) //AAM: не вполне уверен, но должно помочь от баги #2759
		{
			m_pWndLastCapture->SetCapture ();
		}
		m_pWndLastCapture = NULL;
	}

	m_iHighlighted = HitTest (point);

	CBCGToolbarButton* pButton = GetButton(m_iButtonCapture);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));
	UINT nIDCmd = 0;

	UINT nNewStyle = (pButton->m_nStyle & ~TBBS_PRESSED);
	if (m_iButtonCapture == m_iHighlighted)
	{
		// we did not lose the capture
		if (HitTest(point) == m_iButtonCapture)
		{
			// give button a chance to update
			UpdateButton(m_iButtonCapture);

			// then check for disabled state
			if (!(pButton->m_nStyle & TBBS_DISABLED))
			{
				// pressed, will send command notification
				nIDCmd = pButton->m_nID;

				if (pButton->m_nStyle & TBBS_CHECKBOX)
				{
					// auto check: three state => down
					if (nNewStyle & TBBS_INDETERMINATE)
						nNewStyle &= ~TBBS_INDETERMINATE;

					nNewStyle ^= TBBS_CHECKED;
				}
			}
		}
	}

	if (m_hookMouseHelp == NULL)
	{
		GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	}

	int iButtonCapture = m_iButtonCapture;
	m_iButtonCapture = -1;
	m_iHighlighted = -1;

	HWND hwndSaved = m_hWnd;
	RestoreFocus ();

	if (HitTest(point) == iButtonCapture && 
		!OnSendCommand (pButton) &&
		nIDCmd != 0 && nIDCmd != (UINT) -1)
	{
		InvalidateButton (iButtonCapture);
		UpdateWindow(); // immediate feedback

		AddCommandUsage (nIDCmd);

		if (!pButton->OnClickUp () && // By Alex Corazzin
			(g_pUserToolsManager == NULL ||
			!g_pUserToolsManager->InvokeTool (nIDCmd)))
		{
			GetOwner()->SendMessage (WM_COMMAND, nIDCmd);    // send command
		}
	}
	else
	{
		// By Alex Corazzin
		if (::IsWindow (hwndSaved) && !::IsIconic (hwndSaved) &&
			::IsZoomed (hwndSaved))
		{
			pButton->OnClickUp ();
		}
   }


	if (::IsWindow (hwndSaved) &&				// "This" may be destoyed now!
		iButtonCapture < m_Buttons.GetCount ())	// Button may disappear now!
	{
		SetButtonStyle(iButtonCapture, nNewStyle);
		UpdateButton(iButtonCapture);
		InvalidateButton (iButtonCapture);
		UpdateWindow(); // immediate feedback

		m_ptLastMouse = CPoint (-1, -1);
		OnMouseMove (0, point);
	}
}
//*************************************************************************************
void CBCGToolBar::OnCancelMode()
{
	ASSERT_VALID( this );
	CControlBar::OnCancelMode();

	if (m_bStretchButton)
	{
		m_pDragButton = NULL;
		m_bStretchButton = FALSE;

		m_rectTrack.SetRectEmpty ();

		::ReleaseCapture ();
		if (m_pWndLastCapture != NULL)
		{
			m_pWndLastCapture->SetCapture ();
			m_pWndLastCapture = NULL;
		}
	}

	if (m_iButtonCapture >= 0 ||
		m_iHighlighted >= 0)
	{
		if (m_iButtonCapture >= 0)
		{
			CBCGToolbarButton* pButton = GetButton(m_iButtonCapture);
			if (pButton == NULL)
			{
				ASSERT (FALSE);
			}
			else
			{
				ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));
				UINT nNewStyle = (pButton->m_nStyle & ~TBBS_PRESSED);
				if (GetCapture() == this)
				{
					::ReleaseCapture();

					if (m_pWndLastCapture != NULL)
					{
						m_pWndLastCapture->SetCapture ();
						m_pWndLastCapture = NULL;
					}
				}

				SetButtonStyle(m_iButtonCapture, nNewStyle);
			}
		}

		m_iButtonCapture = -1;
		m_iHighlighted = -1;

		OnChangeHot (m_iHighlighted);
	}

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);
	
		pButton->OnCancelMode ();
	}

	Invalidate ();
	UpdateWindow();
}
//*************************************************************************************
void CBCGToolBar::OnSysColorChange()
{
	ASSERT_VALID( this );
	globalData.UpdateSysColors ();

	m_Images.OnSysColorChange ();
	m_ColdImages.OnSysColorChange ();
	m_ImagesLocked.OnSysColorChange ();
	m_ColdImagesLocked.OnSysColorChange ();

	if (m_pUserImages != NULL)
	{
		m_pUserImages->OnSysColorChange ();
	}

	Invalidate ();
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar idle update through CToolCmdUI class

#define CToolCmdUI COldToolCmdUI

class CToolCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CBCGToolBar* pToolBar = (CBCGToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CBCGToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;

	if (!bOn)
		nNewStyle |= TBBS_DISABLED;
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}
//*************************************************************************************
void CToolCmdUI::SetCheck(int nCheck)
{
	ASSERT (nCheck >= 0);
	if (nCheck > 2)
	{
		nCheck = 1;
	}

	CBCGToolBar* pToolBar = (CBCGToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CBCGToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) &
				~(TBBS_CHECKED | TBBS_INDETERMINATE);
	if (nCheck == 1)
		nNewStyle |= TBBS_CHECKED;
	else if (nCheck == 2)
		nNewStyle |= TBBS_INDETERMINATE;
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}
//*************************************************************************************
void CToolCmdUI::SetText (LPCTSTR lpszText)
{
	ASSERT (lpszText != NULL);

	CBCGToolBar* pToolBar = (CBCGToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CBCGToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	if (pToolBar->GetIgnoreSetText ())	// By Jay Giganti 
	{
		return;
	}

	CBCGToolbarButton* pButton = pToolBar->GetButton (m_nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT_VALID (pButton);

	CString strOldText = pButton->m_strText.SpanExcluding (_T("\t"));
	if (strOldText == lpszText)
	{
		return;
	}

	pButton->m_strText = lpszText;
	pToolBar->AdjustLayout ();
}
//*************************************************************************************
void CBCGToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	ASSERT_VALID( this );
	CToolCmdUI state;
	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_Buttons.GetCount ();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		CBCGToolbarButton* pButton = GetButton(state.m_nIndex);
		if (pButton == NULL)
		{
			ASSERT (FALSE);
			return;
		}

		if (g_pUserToolsManager != NULL &&
			g_pUserToolsManager->IsUserToolCmd (pButton->m_nID))
		{
			bDisableIfNoHndler = FALSE;
		}

		state.m_nID = pButton->m_nID;

		// ignore separators and system commands
		if (!(pButton->m_nStyle & TBBS_SEPARATOR) &&
			pButton->m_nID != 0 &&
			!IsSystemCommand (pButton->m_nID) &&
			pButton->m_nID < AFX_IDM_FIRST_MDICHILD)
		{
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}
//*************************************************************************************
void CBCGToolBar::UpdateButton(int nIndex)
{
	ASSERT_VALID( this );
	CFrameWnd* pTarget = GetCommandTarget ();

	// send the update notification
	if (pTarget != NULL)
	{
		CToolCmdUI state;
		state.m_pOther = this;
		state.m_nIndex = nIndex;
		state.m_nIndexMax = (UINT)m_Buttons.GetCount ();
		CBCGToolbarButton* pButton = GetButton(nIndex);

		if (pButton == NULL)
		{
			ASSERT (FALSE);
			return;
		}

		if (!IsSystemCommand (pButton->m_nID) &&
			pButton->m_nID < AFX_IDM_FIRST_MDICHILD)
		{
			state.m_nID = pButton->m_nID;
			state.DoUpdate(pTarget, pTarget->m_bAutoMenuEnable &&
						(g_pUserToolsManager == NULL ||
						!g_pUserToolsManager->IsUserToolCmd (pButton->m_nID)));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolBar diagnostics

#ifdef _DEBUG
void CBCGToolBar::AssertValid() const
{
	CControlBar::AssertValid();
}

void CBCGToolBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump (dc);

	CString strName;

	if (::IsWindow (m_hWnd))
	{
		GetWindowText (strName);
	}

	dc << "\n**** Toolbar ***" << strName;
	dc << "\nButtons: " << m_Buttons.GetCount () << "\n";

	dc.SetDepth (dc.GetDepth () + 1);

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		pButton->Dump (dc);
		dc << "\n";
	}

	dc.SetDepth (dc.GetDepth () - 1);
	dc << "\n";
}
#endif

#undef new
#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

int CBCGToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (globalData.m_hcurStretch == NULL)
	{
		globalData.m_hcurStretch = AfxGetApp ()->LoadCursor (AFX_IDC_HSPLITBAR);
	}

	if (globalData.m_hcurStretchVert == NULL)
	{
		globalData.m_hcurStretchVert = AfxGetApp ()->LoadCursor (AFX_IDC_VSPLITBAR);
	}

	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (pState->m_bNeedTerm)	// AfxOleInit was called
	{
		m_DropTarget.Register (this);
	}

	m_penDrag.CreatePen (PS_SOLID, 1, globalData.clrBtnText);

	AddToolbarToGlobalList(this, lpCreateStruct->lpszName);

	if (!IsCustomizeMode () || g_pWndCustomize == NULL || m_bLocked)
		return 0;

	//[AY]
	g_pWndCustomize->AddToolBar( this );

	return 0;
}
//****************************************************************************************
DROPEFFECT CBCGToolBar::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_iDragIndex = -1;
	m_DropSource.m_bDeleteOnDrop = FALSE;

	return OnDragOver(pDataObject, dwKeyState, point);
}
//****************************************************************************************
void CBCGToolBar::OnDragLeave() 
{
	m_iDragIndex = -1;
	
	CRect rect = m_rectDrag;
	rect.InflateRect (2, 2);
	InvalidateRect (&rect);

	UpdateWindow ();

	m_rectDrag.SetRectEmpty ();
	m_iDragIndex = -1;

	m_DropSource.m_bDeleteOnDrop = TRUE;
}
//****************************************************************************************
DROPEFFECT CBCGToolBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, 
								   CPoint point) 
{
	if (m_bLocked)
	{
		return DROPEFFECT_NONE;
	}

	CBCGToolbarButton* pButton = CBCGToolbarButton::CreateFromOleData (pDataObject);
	if (pButton == NULL)
	{
		return DROPEFFECT_NONE;
	}

	BOOL bAllowDrop = pButton->CanBeDropped (this);
	delete pButton;

	if (!bAllowDrop)
	{
		return DROPEFFECT_NONE;
	}

	BOOL bCopy = (dwKeyState & MK_CONTROL);

	m_bIsDragCopy = bCopy;

	if (m_pDragButton == NULL)	// Drag from the other toolbar
	{
		//------------------
		// Remove selection:
		//------------------
		int iSelected = m_iSelected;
		m_iSelected = -1;

		if (iSelected != -1)
		{
			InvalidateButton (iSelected);
			UpdateWindow ();
		}
	}

	//---------------------
	// Find the drop place:
	//---------------------
	CRect rect = m_rectDrag;
	int iIndex = FindDropIndex (point, m_rectDrag);

	if (rect != m_rectDrag)
	{
		//--------------------
		// Redraw drop marker:
		//--------------------
		m_iDragIndex = iIndex;

		rect.InflateRect (2, 2);
		InvalidateRect (&rect);

		rect = m_rectDrag;
		rect.InflateRect (2, 2);
		InvalidateRect (&m_rectDrag);

		UpdateWindow ();
	}

	int iPrevHighlighted = m_iHighlighted;
	m_iHighlighted = HitTest (point);

	if (iPrevHighlighted != m_iHighlighted)
	{
		OnChangeHot (m_iHighlighted);
	}

	if (iIndex == -1)
	{
		return DROPEFFECT_NONE;
	}

	return (bCopy) ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
}
//****************************************************************************************
BOOL CBCGToolBar::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	ASSERT_VALID(this);

	int iDragIndex = m_iDragIndex;
	if (iDragIndex < 0)
	{
		return FALSE;
	}

	CBCGToolbarButton* pDragButton = m_pDragButton;
	m_pDragButton = NULL;

	OnDragLeave();

	//----------------------------------------------------
	// Create a button object from the OLE clipboard data:
	//----------------------------------------------------
	CBCGToolbarButton* pButton = CreateDroppedButton (pDataObject);
	if (pButton == NULL)
	{
		return FALSE;
	}

	pButton->m_bDragFromCollection = FALSE;

	if (pDragButton != NULL && dropEffect != DROPEFFECT_COPY)
	{
		int iOldIndex = ButtonToIndex (pDragButton);
		if (iDragIndex == iOldIndex || iDragIndex == iOldIndex + 1)
		{
			AddRemoveSeparator (pDragButton, m_ptStartDrag, point);
			delete pButton;
			return TRUE;
		}
		
		RemoveButton (iOldIndex);
		if (iDragIndex > iOldIndex)
		{
			iDragIndex --;
		}

		iDragIndex = min (iDragIndex, m_Buttons.GetCount ());
	}

	if (InsertButton (pButton, iDragIndex) == -1)
	{
		ASSERT (FALSE);
		delete pButton;
		return FALSE;
	}

	AdjustLayout ();

	if (m_bAltCustomizeMode)
	{
		//------------------------------
		// Immideatly save button state:
		//------------------------------
		pButton->SaveBarState ();
	}

	m_iSelected = -1;
	Invalidate ();
	return TRUE;
}
//****************************************************************************************
BOOL CBCGToolBar::SetCustomizeMode (BOOL bSet)
{
	if (m_bCustomizeMode == bSet)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------
	// First step - inform all toolbars about start/end customization mode:
	//---------------------------------------------------------------------
	for (BOOL bToolbarsListWasChanged = TRUE;
		bToolbarsListWasChanged;)
	{
		int iOrigCount = gAllToolbars.GetCount ();
		bToolbarsListWasChanged = FALSE;

		for (POSITION posTlb = gAllToolbars.GetHeadPosition (); 
			posTlb != NULL && !bToolbarsListWasChanged;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
			ASSERT (pToolBar != NULL);

			if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
			{
				ASSERT_VALID(pToolBar);

				pToolBar->OnCustomizeMode (bSet);

				//-------------------------------------------------
				// CBCGToolBar::OnCustomizeMode can add/remove some
				// "sub-toolbars". So, let's start loop again!
				//-------------------------------------------------
				if (gAllToolbars.GetCount () != iOrigCount)
				{
					bToolbarsListWasChanged = TRUE;
				}
			}
			}
	}

	m_bCustomizeMode = bSet;

	//-----------------------------------
	// Second step - redraw all toolbars:
	//-----------------------------------
	for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);

			if (pToolBar->m_pCustomizeBtn != NULL)
			{
				pToolBar->AdjustLayout ();
			}

			pToolBar->Invalidate ();
			pToolBar->UpdateWindow ();
		}
	}

	return TRUE;
}
//********************************************************************************
int CBCGToolBar::GetCommandButtons (UINT uiCmd, CObList& listButtons)
{
	listButtons.RemoveAll ();
	if (uiCmd == 0)
	{
		return 0;
	}

	for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);

			for (POSITION pos = pToolBar->m_Buttons.GetHeadPosition (); pos != NULL;)
			{
				CBCGToolbarButton* pButton = (CBCGToolbarButton*) pToolBar->m_Buttons.GetNext (pos);
				ASSERT (pButton != NULL);
				ASSERT_KINDOF(CBCGToolbarButton, pButton);

				if (pButton->m_nID == uiCmd)
				{
					listButtons.AddTail (pButton);
				}
			}
		}
	}

	return listButtons.GetCount ();
}
//********************************************************************************
int CBCGToolBar::FindDropIndex (const CPoint p, CRect& rectDrag) const
{
	ASSERT_VALID( this );
	int iDragButton = -1;
	rectDrag.SetRectEmpty ();

	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;

	CPoint point = p;
	if (point.y < 0)
	{
		point.y = 0;
	}

	if (m_Buttons.IsEmpty () || 
		(m_Buttons.GetCount () == 1 && m_pCustomizeBtn != NULL))
	{
		GetClientRect (&rectDrag);
		iDragButton = 0;
	}
	else
	{
		if (bHorz)
		{
			int iOffset = GetRowHeight ();
			int iButton = 0;
			CRect rectPrev;

			POSITION pos;
			for (pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
			{
				CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
				ASSERT (pButton != NULL);

				if (!pButton->IsHidden ())
				{
					CRect rect = pButton->Rect ();

					if (iButton > 0 && rect.top > rectPrev.bottom)
					{
						iOffset	= rect.top - rectPrev.bottom;
						break;
					}

					rectPrev = rect;
				}
			}

			int iCursorRow = point.y / (GetRowHeight () + iOffset);
			int iRow = 0;
			iButton = 0;

			for (pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
			{
				CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
				ASSERT (pButton != NULL);

				if (!pButton->IsHidden ())
				{
					CRect rect = pButton->Rect ();
					
					if (iButton > 0 && rect.top >= rectPrev.bottom)
					{
						iRow ++;
					}

					if (iRow > iCursorRow)
					{
						rectDrag = rectPrev;
						rectDrag.left = rectDrag.right;
						iDragButton = iButton - 1;
						break;
					}

					if (iRow == iCursorRow)
					{
						if (point.x < rect.left)
						{
							iDragButton = iButton;
							rectDrag = rect;
							rectDrag.right = rectDrag.left;
							break;
						}
						else if (point.x <= rect.right)
						{
							rectDrag = rect;
							if (point.x - rect.left > rect.right - point.x)
							{
								iDragButton = iButton + 1;
								rectDrag.left = rectDrag.right;
							}
							else
							{
								iDragButton = iButton;
								rectDrag.right = rectDrag.left;
							}
							break;
						}
					}

					rectPrev = rect;
				}
			}

			if (iDragButton == -1 && iRow == iCursorRow)
			{
				rectDrag = rectPrev;
				rectDrag.left = rectDrag.right;
				iDragButton = iButton;
			}
		}
		else
		{
			int iButton = 0;
			for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
			{
				CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
				ASSERT (pButton != NULL);

				CRect rect = pButton->Rect ();

				if (point.y < rect.top)
				{
					iDragButton = iButton;
					rectDrag = rect;
					rectDrag.bottom = rectDrag.top;
					break;
				}
				else if (point.y <= rect.bottom)
				{
					rectDrag = rect;
					if (point.y - rect.top > rect.bottom - point.y)
					{
						iDragButton = iButton + 1;
						rectDrag.top = rectDrag.bottom;
					}
					else
					{
						iDragButton = iButton;
						rectDrag.bottom = rectDrag.top;
					}
					break;
				}
			}
		}
	}

	if (iDragButton >= 0)
	{
		const int iCursorSize = 6;

		CRect rectClient;	// Client area rectangle
		GetClientRect (&rectClient);

		if (bHorz)
		{
			rectDrag.left = 
				max (rectClient.left, rectDrag.left - iCursorSize / 2);

			rectDrag.right = rectDrag.left + iCursorSize;
			if (rectDrag.right > rectClient.right)
			{
				rectDrag.right = rectClient.right;
				rectDrag.left = rectDrag.right - iCursorSize;
			}
		}
		else
		{
			rectDrag.top = 
				max (rectClient.top, rectDrag.top - iCursorSize / 2);

			rectDrag.bottom = rectDrag.top + iCursorSize;
			if (rectDrag.bottom > rectClient.bottom)
			{
				rectDrag.bottom = rectClient.bottom;
				rectDrag.top = rectDrag.bottom - iCursorSize;
			}
		}
	}

	if (m_pCustomizeBtn != NULL && iDragButton == m_Buttons.GetCount ())
	{
		iDragButton = -1;
		rectDrag.SetRectEmpty ();
	}

	return iDragButton;
}
//***********************************************************************
void CBCGToolBar::DrawDragMarker (CDC* pDC)
{
	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;

	CPen* pOldPen = (CPen*) pDC->SelectObject (&m_penDrag);

	for (int i = 0; i < 2; i ++)
	{
		if (bHorz)
		{
			pDC->MoveTo (m_rectDrag.left + m_rectDrag.Width () / 2 + i - 1, m_rectDrag.top);
			pDC->LineTo (m_rectDrag.left + m_rectDrag.Width () / 2 + i - 1, m_rectDrag.bottom);

			pDC->MoveTo (m_rectDrag.left + i, m_rectDrag.top + i);
			pDC->LineTo (m_rectDrag.right - i, m_rectDrag.top + i);

			pDC->MoveTo (m_rectDrag.left + i, m_rectDrag.bottom - i - 1);
			pDC->LineTo (m_rectDrag.right - i, m_rectDrag.bottom - i - 1);
		}
		else
		{
			pDC->MoveTo (m_rectDrag.left, m_rectDrag.top + m_rectDrag.Height () / 2 + i - 1);
			pDC->LineTo (m_rectDrag.right, m_rectDrag.top + m_rectDrag.Height () / 2 + i - 1);

			pDC->MoveTo (m_rectDrag.left + i, m_rectDrag.top + i);
			pDC->LineTo (m_rectDrag.left + i, m_rectDrag.bottom - i);

			pDC->MoveTo (m_rectDrag.right - i - 1, m_rectDrag.top + i);
			pDC->LineTo (m_rectDrag.right - i - 1, m_rectDrag.bottom - i);
		}
	}

	pDC->SelectObject (pOldPen);
}
//********************************************************************
void CBCGToolBar::OnDestroy() 
{
	ASSERT_VALID( this );
	CControlBar::OnDestroy();

	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		POSITION posSave = pos;

		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);

			if (pToolBar == this)
			{
				gAllToolbars.RemoveAt (posSave);
				break;
			}
		}
	}

	//[AY]
	if (!IsCustomizeMode () || g_pWndCustomize == NULL || m_bLocked)
		return;
	g_pWndCustomize->RemoveToolBar( this );
}
//********************************************************************
void CBCGToolBar::Serialize (CArchive& ar)
{
	CControlBar::Serialize (ar);

	POSITION pos;
	CString strName;

	if (ar.IsLoading ())
	{
		//-----------------------
		// Save customize button:
		//-----------------------
		CCustomizeButton* pCustomizeBtn = NULL;
		if (m_pCustomizeBtn != NULL)
		{
			ASSERT_VALID (m_pCustomizeBtn);
			ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last

			pCustomizeBtn = new CCustomizeButton;
			pCustomizeBtn->CopyFrom (*m_pCustomizeBtn);
		}

		RemoveAllButtons ();
		m_Buttons.Serialize (ar);

		for (pos = m_Buttons.GetHeadPosition (); pos != NULL;)
		{
			CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			pButton->m_nStyle &= ~TBBS_PRESSED;	// Fix for the "stuck" buttons.
			pButton->OnChangeParentWnd (this);
		}
		
		BOOL bTextLabels;
		ar >> bTextLabels;
		if (AllowChangeTextLabels ())
		{
			m_bTextLabels = bTextLabels;
		}

		//--------------------------
		// Restore customize button:
		//--------------------------
		if (pCustomizeBtn != NULL)
		{
			InsertButton (pCustomizeBtn);
			m_pCustomizeBtn = pCustomizeBtn;
		}

		AdjustLocations ();

		ar >> strName;

		if (::IsWindow (m_hWnd))
		{
			SetWindowText (strName);
		}

		//--------------------------
		// Remove all "bad" buttons:
		//--------------------------
		for (pos = m_lstUnpermittedCommands.GetHeadPosition (); pos != NULL;)
		{
			UINT uiCmd = m_lstUnpermittedCommands.GetNext (pos);

			int iIndex = CommandToIndex (uiCmd);
			if (iIndex >= 0)
			{
				RemoveButton (iIndex);
			}
		}
	}
	else
	{
		//-----------------------------------
		// Serialize just "Storable" buttons:
		//-----------------------------------
		CObList buttons;

		for (pos = m_Buttons.GetHeadPosition (); pos != NULL;)
		{
			CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
			ASSERT_VALID (pButton);

			if (pButton->CanBeStored ())
			{
				buttons.AddTail (pButton);
			}
		}

		buttons.Serialize (ar);
		ar << m_bTextLabels;

		if (::IsWindow (m_hWnd))
		{
			GetWindowText (strName);
		}

		ar << strName;
	}
}
//*********************************************************************
BOOL CBCGToolBar::SaveState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	CString strProfileName = ::BCGGetRegPath (strToolbarProfile, lpszProfileName);

	BOOL bResult = FALSE;

	if (nIndex == -1)
	{
		nIndex = GetDlgCtrlID ();
	}

	CString strSection;
	if (uiID == (UINT) -1)
	{
		strSection.Format (REG_SECTION_FMT, strProfileName, nIndex);
	}
	else
	{
		strSection.Format (REG_SECTION_FMT_EX, strProfileName, nIndex, uiID);
	}

	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);

			Serialize (ar);
			ar.Flush ();
		}

		UINT uiDataSize = (UINT)file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData != NULL)
		{
			CBCGRegistry reg (FALSE, FALSE);
			if (reg.CreateKey (strSection))
			{
				if (::IsWindow (m_hWnd))
				{
					CString strToolbarName;
					GetWindowText (strToolbarName);

					reg.Write (REG_ENTRY_NAME, strToolbarName);
				}

				bResult = reg.Write (REG_ENTRY_BUTTONS, lpbData, uiDataSize);
			}

			free (lpbData);
		}
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGToolBar::SaveState ()!\n"));
	}

	return bResult;
}
//*********************************************************************
BOOL CBCGToolBar::RemoveStateFromRegistry (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	ASSERT_VALID( this );
	CString strProfileName = ::BCGGetRegPath (strToolbarProfile, lpszProfileName);

	if (nIndex == -1)
	{
		nIndex = GetDlgCtrlID ();
	}

	CString strSection;
	if (uiID == (UINT) -1)
	{
		strSection.Format (REG_SECTION_FMT, strProfileName, nIndex);
	}
	else
	{
		strSection.Format (REG_SECTION_FMT_EX, strProfileName, nIndex, uiID);
	}

	CBCGRegistry reg (FALSE, FALSE);
	return reg.DeleteKey (strSection);
}
//*********************************************************************
BOOL CBCGToolBar::LoadState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	CString strProfileName = ::BCGGetRegPath (strToolbarProfile, lpszProfileName);

	BOOL bResult = FALSE;

	if (nIndex == -1)
	{
		nIndex = GetDlgCtrlID ();
	}

	CString strSection;
	if (uiID == (UINT) -1)
	{
		strSection.Format (REG_SECTION_FMT, strProfileName, nIndex);
	}
	else
	{
		strSection.Format (REG_SECTION_FMT_EX, strProfileName, nIndex, uiID);
	}

	LPBYTE	lpbData = NULL;
	UINT	uiDataSize;

	CBCGRegistry reg (FALSE, TRUE);
	if (!reg.Open (strSection))
	{
		return FALSE;
	}

	if (!reg.Read (REG_ENTRY_BUTTONS, &lpbData, &uiDataSize))
	{
		return FALSE;
	}

	try
	{
		CMemFile file (lpbData, uiDataSize);
		CArchive ar (&file, CArchive::load);

		Serialize (ar);
		bResult = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGToolBar::LoadState ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("CArchiveException exception in CBCGToolBar::LoadState ()!\n"));
	}

	if (lpbData != NULL)
	{
		delete lpbData;
	}

	AdjustLayout ();
	return bResult;
}
//*******************************************************************************************
void CBCGToolBar::OnContextMenu(CWnd*, CPoint point)
{
	ASSERT_VALID( this );
	if (m_bLocked && IsCustomizeMode ())
	{
		MessageBeep ((UINT) -1);
		return;
	}

	if (!CanDragDropButtons()) //BT5214
	{
		MessageBeep ((UINT) -1);
		return;
	}

	OnChangeHot (-1);

	if (!IsCustomizeMode ())
	{
		CFrameWnd* pParentFrame = (m_pDockSite == NULL) ?
			GetDockingFrame() : m_pDockSite;
		ASSERT_VALID(pParentFrame);

		pParentFrame->SendMessage (BCGM_TOOLBARMENU,
			(WPARAM) GetSafeHwnd (),
			MAKELPARAM(point.x, point.y));
		return;
	}

	SetFocus ();

	CPoint ptClient = point;
	ScreenToClient (&ptClient);

	int iButton = HitTest(ptClient);

	int iSelected = m_iSelected;
	m_iSelected = iButton;

	if (iSelected != -1)
	{
		InvalidateButton (iSelected);
	}

	if (m_iSelected != -1)
	{
		InvalidateButton (m_iSelected);
	}

	UpdateWindow ();

	if (iButton < 0) // nothing hit
	{
		return;
	}

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	if (!pButton->IsEditable ())
	{
		m_iSelected = -1;
		InvalidateButton (iButton);
		UpdateWindow ();

		return;
	}

	if (pButton->CanBeStored ())
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		

		CMenu menu;
		{
			CBCGLocalResource locaRes;
			VERIFY(menu.LoadMenu(IDR_BCGBARRES_POPUP_BCGTOOL_BAR));
		}

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		if (pButton->IsLocked ())
		{
			pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE, MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_TEXT, MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_RESET, MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			if (!EnableContextMenuItems (pButton, pPopup))
			{
				return;
			}
		}

		pPopup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON,
								point.x, point.y, this);
	}
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarAppearance() 
{
	ASSERT_VALID( this );
	CBCGLocalResource locaRes;

	ASSERT (IsCustomizeMode () && !m_bLocked);
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	if (pButton->m_strText.IsEmpty ())
	{
		OnSetDefaultButtonText (pButton);
	}

	//andy
	BOOL bOld = m_bButtonApprernceMode;
	m_bButtonApprernceMode = true;

	CButtonAppearanceDlg dlg (pButton, m_pUserImages, this, 0, m_bMenuMode);
	if (dlg.DoModal () == IDOK)
	{
		AdjustLayout ();
		Invalidate ();
	}
	m_bButtonApprernceMode = bOld;
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarDelete() 
{
	ASSERT_VALID( this );
	ASSERT (m_iSelected >= 0);
	RemoveButton (m_iSelected);
	
	m_iSelected = -1;

	AdjustLayout ();
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarImage() 
{
	ASSERT_VALID( this );
	CBCGLocalResource locaRes;

	ASSERT (IsCustomizeMode () && !m_bLocked);
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	BOOL bSaveText = pButton->m_bText;
	BOOL bSaveImage = pButton->m_bImage;

	pButton->m_bText = FALSE;
	pButton->m_bImage = TRUE;

	if (pButton->GetImage () < 0)
	{
		CButtonAppearanceDlg dlg (pButton, m_pUserImages, this, 0, m_bMenuMode);
		if (dlg.DoModal () != IDOK)
		{
			pButton->m_bText = bSaveText;
			pButton->m_bImage = bSaveImage;
			return;
		}
	}

	AdjustLayout ();
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarImageAndText() 
{
	ASSERT_VALID( this );
	ASSERT (IsCustomizeMode () && !m_bLocked);
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	BOOL bSaveText = pButton->m_bText;
	BOOL bSaveImage = pButton->m_bImage;

	pButton->m_bText = TRUE;
	pButton->m_bImage = TRUE;

	if (pButton->GetImage () < 0)
	{
		CBCGLocalResource locaRes;

		CButtonAppearanceDlg dlg (pButton, m_pUserImages, this, 0, m_bMenuMode);
		if (dlg.DoModal () != IDOK)
		{
			pButton->m_bText = bSaveText;
			pButton->m_bImage = bSaveImage;
			return;
		}
	}

	if (pButton->m_strText.IsEmpty ())
	{
		OnSetDefaultButtonText (pButton);
	}

	if (pButton->m_strText.IsEmpty ())
	{
		MessageBeep ((UINT) -1);

		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
	}

	AdjustLayout ();
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarStartGroup() 
{
	ASSERT_VALID( this );
	ASSERT (m_iSelected > 0);

	CBCGToolbarButton* pPrevButton = NULL;
	int i = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i ++)
	{
		CBCGToolbarButton* pCurrButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pCurrButton);

		if (i == m_iSelected)
		{
			ASSERT (pPrevButton != NULL);	// m_iSelected > 0!

			if (pPrevButton->m_nStyle & TBBS_SEPARATOR)
			{
				VERIFY (RemoveButton (m_iSelected - 1));
			}
			else
			{
				InsertSeparator (m_iSelected ++);
			}

			break;
		}

		pPrevButton = pCurrButton;
	}

	AdjustLayout ();
}
//*******************************************************************************************
void CBCGToolBar::OnToolbarText() 
{
	ASSERT_VALID( this );
	ASSERT (IsCustomizeMode () && !m_bLocked);
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	pButton->m_bText = TRUE;
	pButton->m_bImage = FALSE;

	if (pButton->m_strText.IsEmpty ())
	{
		OnSetDefaultButtonText (pButton);
	}

	if (pButton->m_strText.IsEmpty ())
	{
		MessageBeep ((UINT) -1);

		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
	}

	AdjustLayout ();
}
//************************************************************************************
void CBCGToolBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	ASSERT_VALID( this );
	CControlBar::OnWindowPosChanged(lpwndpos);
	RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

	if (!IsCustomizeMode () || g_pWndCustomize == NULL || m_bLocked)
	{
		return;
	}

	if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		g_pWndCustomize->ShowToolBar (this, FALSE);
	}
	
	if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		g_pWndCustomize->ShowToolBar (this, TRUE);
	}
}
//**************************************************************************************
HBRUSH CBCGToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	ASSERT_VALID( this );
	HBRUSH hbr = CControlBar::OnCtlColor (pDC, pWnd, nCtlColor);
	if (!IsCustomizeMode () || m_bLocked)
	{
		return hbr;
	}

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		HWND hwdList = pButton->GetHwnd ();
		if (hwdList == NULL)	// No control
		{
			continue;
		}

		if (hwdList == pWnd->GetSafeHwnd () ||
			::IsChild (hwdList, pWnd->GetSafeHwnd ()))
		{
			HBRUSH hbrButton = pButton->OnCtlColor (pDC, nCtlColor);
			return (hbrButton == NULL) ? hbr : hbrButton;
		}
	}

	return hbr;
}
//**************************************************************************************
int CBCGToolBar::GetCount () const
{
	return m_Buttons.GetCount ();
}
//*************************************************************************************
BOOL CBCGToolBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
	BOOL bStretch = m_bStretchButton;

	if (!bStretch && IsCustomizeMode () && m_iSelected != -1 && !m_bLocked)
	{
		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (HitTest (ptCursor) == m_iSelected)
		{
			CBCGToolbarButton* pButton = GetButton (m_iSelected);
			ASSERT_VALID (pButton);

			if (pButton->CanBeStretched () &&
				abs (ptCursor.x - pButton->Rect ().right) <= STRETCH_DELTA)
			{
				bStretch = TRUE;
			}
		}
	}

	if (bStretch)
	{
		::SetCursor (globalData.m_hcurStretch);
		return TRUE;
	}
	
	return CControlBar::OnSetCursor(pWnd, nHitTest, message);
}
//****************************************************************************************
BOOL CBCGToolBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_ESCAPE)
	{
		if (m_bStretchButton)
		{
			OnCancelMode ();
		}
		else
		{
			CBCGToolbarMenuButton* pMenuButon = GetDroppedDownMenu ();
			if (pMenuButon != NULL)
			{
				return CControlBar::PreTranslateMessage(pMsg);
			}

			Deactivate ();
			RestoreFocus ();
		}

		return TRUE;
	}

	return CControlBar::PreTranslateMessage(pMsg);
}
//**************************************************************************************
BOOL CBCGToolBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	if (IsCustomizeMode () && !m_bLocked)
	{
		return CControlBar::OnCommand (wParam, lParam);
	}

	BOOL bAccelerator = FALSE;
	int nNotifyCode = HIWORD (wParam);

	// Find the control send the message:
	HWND hWndCtrl = (HWND)lParam;
	if (hWndCtrl == NULL)
	{
		if (wParam == IDCANCEL)	// ESC was pressed
		{
			RestoreFocus ();
			return TRUE;
		}

		if (wParam != IDOK ||
			(hWndCtrl = ::GetFocus ()) == NULL)
		{
			return FALSE;
		}

		bAccelerator = TRUE;
		nNotifyCode = 0;
	}

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		HWND hwdList = pButton->GetHwnd ();
		if (hwdList == NULL)	// No control
		{
			continue;
		}

		if (hwdList == hWndCtrl || ::IsChild (hwdList, hWndCtrl))
		{
			if (!bAccelerator)
			{
				ASSERT (LOWORD (wParam) == pButton->m_nID);
				if (!pButton->NotifyCommand (nNotifyCode))
				{
					break;
				}
			}

			GetOwner()->PostMessage (WM_COMMAND,
				MAKEWPARAM (pButton->m_nID, nNotifyCode), lParam);
			return TRUE;
		}
	}

	return TRUE;
}
//*************************************************************************************
CBCGToolBar* CBCGToolBar::FromHandlePermanent (HWND hwnd)
{
	for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
		if (pToolBar->GetSafeHwnd () == hwnd)
		{
			return pToolBar;
		}
	}

	return NULL;
}
//**********************************************************************************
CSize CBCGToolBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	
	if (dwMode & LM_HORZDOCK)
	{
		ASSERT(dwMode & LM_HORZ);
	}

	m_nMaxBtnHeight = CalcMaxButtonHeight ();

	CSize sizeResult(0,0);

	if (!(m_dwStyle & CBRS_SIZE_FIXED))
	{
		BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

		if (bDynamic && (dwMode & LM_MRUWIDTH))
			SizeToolBar(m_nMRUWidth);
		else if (bDynamic && (dwMode & LM_HORZDOCK))
			SizeToolBar(32767);
		else if (bDynamic && (dwMode & LM_VERTDOCK))
		{
			SizeToolBar(0);
		}
		else if (bDynamic && (nLength != -1))
		{
			CRect rect; rect.SetRectEmpty();
			CalcInsideRect(rect, (dwMode & LM_HORZ));
			BOOL bVert = (dwMode & LM_LENGTHY);

			int nLen = nLength + (bVert ? rect.Height() : rect.Width());

			SizeToolBar(nLen, bVert);
		}
		else if (bDynamic && (m_dwStyle & CBRS_FLOATING))
			SizeToolBar(m_nMRUWidth);
		else
			SizeToolBar((dwMode & LM_HORZ) ? 32767 : 0);
	}

	sizeResult = CalcSize ((dwMode & LM_HORZ) == 0);

	if (dwMode & LM_COMMIT)
	{
		if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC) &&
			(dwMode & LM_HORZ))
		{
			m_nMRUWidth = sizeResult.cx;
		}
	}

	//BLOCK: Adjust Margins
	{
		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();

		CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}

	RebuildAccelerationKeys ();
	return sizeResult;
}
//**********************************************************************************
CSize CBCGToolBar::CalcSize (BOOL bVertDock)
{
	if (m_Buttons.IsEmpty ())
	{
		return GetButtonSize ();
	}

	CClientDC dc (this);
	CFont* pOldFont = (CFont*) dc.SelectObject (
		bVertDock ? &globalData.fontVert : &globalData.fontRegular);
	ASSERT (pOldFont != NULL);

	CSize sizeResult (GetColumnWidth (), GetRowHeight ());

	CRect rect; rect.SetRectEmpty();
	CalcInsideRect (rect, !bVertDock);

	int iStartX = bVertDock ? 0 : 1;
	int iStartY = bVertDock ? 1 : 0;

	CPoint cur (iStartX, iStartY);

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pos == NULL && m_pCustomizeBtn != NULL && IsFloating ())
		{
			ASSERT_VALID (m_pCustomizeBtn);
			ASSERT (m_pCustomizeBtn == pButton);	// Should be last
			break;
		}

		CSize sizeDefault (GetColumnWidth (), 
				m_bDrawTextLabels ? GetButtonSize ().cy : GetRowHeight ());
		CSize sizeButton = pButton->OnCalculateSize (&dc, sizeDefault, !bVertDock);

		if (m_bDrawTextLabels)
		{
			sizeButton.cy = m_nMaxBtnHeight;
		}

		if (!bVertDock)
		{
			if ((cur.x == iStartX || pButton->m_bWrap)
				&& pButton->m_nStyle & TBBS_SEPARATOR)
			{
				sizeButton = CSize (0, 0);
			}

			sizeResult.cx = max (cur.x + sizeButton.cx, sizeResult.cx);
			sizeResult.cy = max (cur.y + sizeButton.cy, sizeResult.cy);

			cur.x += sizeButton.cx;

			if (pButton->m_bWrap)
			{
				cur.x = iStartX;
				cur.y += GetRowHeight () + LINE_OFFSET;
			}
		}
		else
		{
			sizeResult.cx = max (cur.x + sizeButton.cx, sizeResult.cx);
			sizeResult.cy = max (cur.y + sizeButton.cy, sizeResult.cy);

			cur.x = iStartX;
			cur.y += sizeButton.cy;
		}
	}

	dc.SelectObject (pOldFont);
	return sizeResult;
}
//**********************************************************************************
int CBCGToolBar::WrapToolBar (int nWidth)
{
	ASSERT_VALID( this );
	int nResult = 0;
	
	BOOL bVertDock = (m_dwStyle & CBRS_ORIENT_HORZ) == 0;

	CClientDC dc (this);

	CFont* pOldFont = (CFont*) dc.SelectObject (
		bVertDock ? &globalData.fontVert : &globalData.fontRegular);
	ASSERT (pOldFont != NULL);

	CBCGToolbarButton* pPrevButton = NULL;

	CRect rect;
	GetClientRect(rect);

	int x = 0;
	int y = rect.top;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->m_bWrap = FALSE;

		if (pos == NULL && m_pCustomizeBtn != NULL && IsFloating ())
		{
			ASSERT_VALID (m_pCustomizeBtn);
			ASSERT (m_pCustomizeBtn == pButton);	// Should be last
			break;
		}

		CSize sizeButton = pButton->OnCalculateSize (&dc, 
			CSize (GetColumnWidth (), GetRowHeight ()), !bVertDock);
		
		if (x == 0 && (pButton->m_nStyle & TBBS_SEPARATOR))
		{
			// Don't show separator on the first column!
			sizeButton = CSize (0, 0);
		}

		if (x + sizeButton.cx > nWidth &&
			!(pButton->m_nStyle & TBBS_SEPARATOR))
		{
			if (pPrevButton != NULL)
			{
				pPrevButton->m_bWrap = TRUE;
				x = 0;
				y += sizeButton.cy + LINE_OFFSET;
				nResult ++;
			}
		}

		pPrevButton = pButton;
		x += sizeButton.cx;
	}

	dc.SelectObject (pOldFont);
	return nResult + 1;
}
//**********************************************************************************
void  CBCGToolBar::SizeToolBar (int nLength, BOOL bVert)
{
	ASSERT_VALID( this );
	CSize size;

	if (!bVert)
	{
		int nMin, nMax, nTarget, nCurrent, nMid;

		// Wrap ToolBar vertically
		nMin = 0;
		nCurrent = WrapToolBar(nMin);

		// Wrap ToolBar as specified
		nMax = nLength;
		nTarget = WrapToolBar(nMax);

		if (nCurrent != nTarget)
		{
			while (nMin < nMax)
			{
				nMid = (nMin + nMax) / 2;
				nCurrent = WrapToolBar(nMid);

				if (nCurrent == nTarget)
					nMax = nMid;
				else
				{
					if (nMin == nMid)
					{
						WrapToolBar(nMax);
						break;
					}
					nMin = nMid;
				}
			}
		}

		size = CalcSize (bVert);
		WrapToolBar (size.cx);
	}
	else
	{
		int iWidth = 32767;
		WrapToolBar (iWidth);

		size = CalcSize (FALSE);
		if (nLength > size.cy)
		{
			iWidth = 0;

			do
			{
				iWidth += GetButtonSize ().cx;
				WrapToolBar (iWidth);
				size = CalcSize (FALSE);
			}
			while (nLength < size.cy);
		}

		WrapToolBar (size.cx);
	}
//	m_pDockSite->
//	SetWindowPos( 0, 0, 0, size.cx, size.cy, SWP_NOREPOSITION|SWP_NOZORDER|SWP_NOMOVE );
}
//**********************************************************************************
void CBCGToolBar::OnSize(UINT nType, int cx, int cy) 
{
	ASSERT_VALID( this );
	CControlBar::OnSize(nType, cx, cy);
	
	if (IsCustomizeMode () && !m_bLocked)
	{
		OnCancelMode ();
	}

	AdjustLocations ();

	//------------------------------------------------------
	// Adjust system menu of the floating toolbar miniframe:
	//------------------------------------------------------
	if (IsFloating ())
	{
		CMiniFrameWnd* pMiniFrame = 
			DYNAMIC_DOWNCAST (CMiniFrameWnd, GetParentFrame ());
		if (pMiniFrame != NULL)
		{
			CMenu* pSysMenu = pMiniFrame->GetSystemMenu(FALSE);
			ASSERT (pSysMenu != NULL);

			pSysMenu->DeleteMenu (SC_RESTORE, MF_BYCOMMAND);
			pSysMenu->DeleteMenu (SC_MINIMIZE, MF_BYCOMMAND);
			pSysMenu->DeleteMenu (SC_MAXIMIZE, MF_BYCOMMAND);

			if (!CanBeClosed ())
			{
				pSysMenu->EnableMenuItem (SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
			}
		}
	}
}
//**********************************************************************************
void CBCGToolBar::AdjustLocations ()
{
	ASSERT_VALID(this);

	if (m_Buttons.IsEmpty ())
	{
		return;
	}

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect (rectClient);

	int xRight = m_iRebarPaneWidth == -1 ? 
		rectClient.right : 
		m_iRebarPaneWidth;

	CClientDC dc (this);
	CFont* pOldFont;
	if (bHorz)
	{
		pOldFont = (CFont*) dc.SelectObject (&globalData.fontRegular);
	}
	else
	{
		pOldFont = (CFont*) dc.SelectObject (&globalData.fontVert);
	}
	
	ASSERT (pOldFont != NULL);

	int iStartOffset;
	if (bHorz)
	{
		iStartOffset = rectClient.left + 1;
	}
	else
	{
		iStartOffset = rectClient.top + 1;
	}

	int iOffset = iStartOffset;
	int y = rectClient.top;

	CSize sizeCustButton (0, 0);

	if (m_pCustomizeBtn != NULL && !IsFloating () && !IsCustomizeMode ())
	{
		ASSERT_VALID (m_pCustomizeBtn);
		ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last

		m_pCustomizeBtn->m_lstInvisibleButtons.RemoveAll ();

		sizeCustButton = m_pCustomizeBtn->OnCalculateSize (&dc,
			CSize (	bHorz ? GetColumnWidth () : rectClient.Width (), 
			bHorz ? rectClient.Height () : GetRowHeight ()), bHorz);
	}

	BOOL bPrevWasSeparator = FALSE;
	int nRowActualWidth = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		POSITION posSave = pos;

		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		BOOL bVisible = TRUE;

		CSize sizeButton = pButton->OnCalculateSize (&dc, 
			CSize (GetColumnWidth (), GetRowHeight ()), bHorz);
		if (pButton->m_bTextBelow && bHorz)
		{
			sizeButton.cy =  GetRowHeight ();
		}

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			if (iOffset == iStartOffset || bPrevWasSeparator)
			{
				sizeButton = CSize (0, 0);
				bVisible = FALSE;
			}
			else
			{
				bPrevWasSeparator = TRUE;
			}
		}

		int iOffsetPrev = iOffset;

		CRect rectButton;
		if (bHorz)
		{
			rectButton.left = iOffset;
			rectButton.right = rectButton.left + sizeButton.cx;
			rectButton.top = y;
			rectButton.bottom = rectButton.top + sizeButton.cy;
			
			iOffset += sizeButton.cx;
			nRowActualWidth += sizeButton.cx;
		}
		else
		{
			rectButton.left = rectClient.left;
			rectButton.right = rectClient.left + sizeButton.cx;
			rectButton.top = iOffset;
			rectButton.bottom = iOffset + sizeButton.cy;

			iOffset += sizeButton.cy;
		}

		if (m_pCustomizeBtn != NULL && pButton != m_pCustomizeBtn &&
			!IsFloating () && !IsCustomizeMode ())
		{
			if ((bHorz && rectButton.right > xRight - sizeCustButton.cx) ||
				(!bHorz && rectButton.bottom > rectClient.bottom - sizeCustButton.cy))
			{
				bVisible = FALSE;
				iOffset = iOffsetPrev;

				m_pCustomizeBtn->m_lstInvisibleButtons.AddTail (pButton);
			}
		}

		pButton->Show (bVisible);
		pButton->SetRect (rectButton);

		if (bVisible)
		{
			bPrevWasSeparator = (pButton->m_nStyle & TBBS_SEPARATOR);
		}

		if ((pButton->m_bWrap || pos == NULL) && bHorz && (GetBarStyle()&CBRS_FLOATING || !(GetBarStyle()&CBRS_GRIPPER) ))
		{
			//-----------------------
			// Center buttons in row:
			//-----------------------
			int nShift = (xRight - nRowActualWidth - iStartOffset) / 2;
			if (IsFloating () && nShift > 0 && m_bTextLabels)
			{
				for (POSITION posRow = posSave; posRow != NULL;)
				{
					BOOL bThis = (posRow == posSave);

					CBCGToolbarButton* pButtonRow = (CBCGToolbarButton*) m_Buttons.GetPrev (posRow);
					ASSERT (pButtonRow != NULL);

					if (pButtonRow->m_bWrap && !bThis)
					{
						break;
					}

					CRect rect = pButtonRow->Rect ();
					rect.OffsetRect (nShift, 0);
					pButtonRow->SetRect (rect);
				}
			}

			iOffset = iStartOffset;
			nRowActualWidth = 0;
			y += GetRowHeight () + LINE_OFFSET;
		}
	}

	if (m_pCustomizeBtn != NULL)
	{
		CRect rectButton = rectClient;

		//andy
		if ((int) m_pCustomizeBtn->m_uiCustomizeCmdId <= 0 &&
			//m_pCustomizeBtn->m_lstInvisibleButtons.IsEmpty () ||
			IsFloating () ||
			IsCustomizeMode ())
		{
			// Hide customize button:
			m_pCustomizeBtn->SetRect (CRect (0, 0, 0, 0));
			m_pCustomizeBtn->Show (FALSE);
		}
		else
		{
			if (bHorz)
			{
				rectButton.right = xRight - 1;
				rectButton.left = rectButton.right - sizeCustButton.cx + 1;
			}
			else
			{
				rectButton.top = rectButton.bottom - sizeCustButton.cy;
			}

			m_pCustomizeBtn->SetRect (rectButton);
			m_pCustomizeBtn->Show (TRUE);
		}
	}

	dc.SelectObject (pOldFont);
}
//**********************************************************************************
DWORD CBCGToolBar::RecalcDelayShow(AFX_SIZEPARENTPARAMS* lpLayout)
{
	ASSERT_VALID( this );
	DWORD dwRes = CControlBar::RecalcDelayShow (lpLayout);

	if (!IsFloating ())
	{
		AdjustLocations ();
	}

	return dwRes;
}
//*********************************************************************************
void CBCGToolBar::AddRemoveSeparator (const CBCGToolbarButton* pButton,
						const CPoint& ptStart, const CPoint& ptDrop)
{
	ASSERT_VALID( this );
	ASSERT_VALID (pButton);
	
	int iIndex = ButtonToIndex (pButton);
	if (iIndex <= 0)
	{
		return;
	}

	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	int iDelta = (bHorz) ? ptDrop.x - ptStart.x : ptDrop.y - ptStart.y;

	if (abs (iDelta) < STRETCH_DELTA)
	{
		// Ignore small move....
		return;
	}

	if (iDelta > 0)	// Add a separator left of button
	{
		const CBCGToolbarButton* pLeftButton = GetButton (iIndex - 1);
		ASSERT_VALID (pLeftButton);

		if (pLeftButton->m_nStyle & TBBS_SEPARATOR)
		{
			// Already have separator, do nothing...
			return;
		}

		InsertSeparator (iIndex);
	}
	else	// Remove a separator in the left side
	{
		const CBCGToolbarButton* pLeftButton = GetButton (iIndex - 1);
		ASSERT_VALID (pLeftButton);

		if ((pLeftButton->m_nStyle & TBBS_SEPARATOR) == 0)
		{
			// Not a separator, do nothing...
			return;
		}

		RemoveButton (iIndex - 1);
	}

	AdjustLayout ();

	m_iSelected = -1;
	Invalidate ();
	UpdateWindow ();
}
//***************************************************************************************
void CBCGToolBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	int iButton = HitTest(point);
	if (iButton >= 0)
	{
		CBCGToolbarButton* pButton = GetButton (iButton);
		if (pButton == NULL)
		{
			ASSERT (FALSE);
			return;
		}

		pButton->OnDblClick (this);
		OnLButtonDown (nFlags, point);	// By Hekan Parting

		//----------------------------------------------------------
		// Don't permit dock/undock when user double clicks on item!
		//----------------------------------------------------------
	}
	else
	{
		if( m_bEnableDrag )
			CControlBar::OnLButtonDblClk(nFlags, point);
	}
}
//***************************************************************************************
void CBCGToolBar::DrawSeparator (CDC* pDC, const CRect& rect, BOOL bHorz)
{
	ASSERT_VALID( this );
	CRect rectSeparator = rect;
	if (bHorz)
	{
		rectSeparator.left += rectSeparator.Width () / 2 - 1;
		rectSeparator.right = rectSeparator.left + 2;
	}
	else
	{
		rectSeparator.top += rectSeparator.Height () / 2 - 1;
		rectSeparator.bottom = rectSeparator.top + 2;
	}

	pDC->Draw3dRect (rectSeparator, globalData.clrBtnShadow,
									globalData.clrBtnHilite);
}
//***************************************************************************************
CBCGToolbarButton* CBCGToolBar::CreateDroppedButton (COleDataObject* pDataObject)
{
	CBCGToolbarButton* pButton = CBCGToolbarButton::CreateFromOleData (pDataObject);
	ASSERT (pButton != NULL);

	//---------------------------
	// Remove accelerator string:
	//---------------------------
	int iOffset = pButton->m_strText.Find (_T('\t'));
	if (iOffset >= 0)
	{
		pButton->m_strText = pButton->m_strText.Left (iOffset);
	}

	pButton->m_bText = FALSE;
	pButton->m_bImage = TRUE;

	if (pButton->m_bDragFromCollection && 
		pButton->GetImage () == -1 &&
		pButton->m_strText.IsEmpty ())
	{
		//----------------------------------------------
		// User-defined button by default have no image
		// and text and empty. To avoid the empty button
		// appearance, ask user about it's properties:
		//----------------------------------------------
		CBCGLocalResource locaRes;
		CButtonAppearanceDlg dlg (pButton, m_pUserImages, this, 0, m_bMenuMode);

		if (dlg.DoModal () != IDOK)
		{
			delete pButton;
			return NULL;
		}
	}

	if (pButton->GetImage () < 0)
	{
		pButton->m_bText = TRUE;
		pButton->m_bImage = FALSE;
	}

	return pButton;
}
//****************************************************************************************
CBCGToolbarButton* CBCGToolBar::GetHighlightedButton () const
{
	ASSERT_VALID( this );
	if (m_iHighlighted < 0)
	{
		return NULL;
	}
	else
	{
		return GetButton (m_iHighlighted);
	}
}
//****************************************************************************************
void CBCGToolBar::RebuildAccelerationKeys ()
{
	ASSERT_VALID( this );
	m_AcellKeys.RemoveAll ();

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		if ((pButton->m_nStyle & TBBS_SEPARATOR) ||
			!pButton->m_bText)
		{
			continue;
		}

		int iAmpOffset = pButton->m_strText.Find (_T('&'));
		if (iAmpOffset >= 0 && iAmpOffset < pButton->m_strText.GetLength () - 1)
		{
			TCHAR szChar [2] = { pButton->m_strText.GetAt (iAmpOffset + 1), '\0' };
			CharUpper (szChar);
			UINT uiHotKey = (UINT) (szChar [0]);
			m_AcellKeys.SetAt (uiHotKey, pButton);
		}
	}
}
//****************************************************************************************
void CBCGToolBar::OnCustomizeMode (BOOL bSet)
{
	ASSERT_VALID( this );
	m_iButtonCapture = -1;
	m_iHighlighted = -1;
	m_iSelected = -1;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->OnCancelMode ();

		if (pButton->GetHwnd () != NULL)
		{
			::EnableWindow (pButton->GetHwnd (), !bSet);
		}
	}
}
//****************************************************************************************
BOOL CBCGToolBar::RestoreOriginalstate ()
{
	ASSERT_VALID( this );
	if (m_uiOriginalResID == 0)
	{
		return FALSE;
	}

	BOOL bRes = LoadToolBar (m_uiOriginalResID);

	AdjustLayout ();
	Invalidate ();

	return bRes;
}
//*****************************************************************************************
void CBCGToolBar::ShowCommandMessageString (UINT uiCmdId)
{
	ASSERT_VALID( this );
	if (m_hookMouseHelp != NULL)
	{
		return;
	}

	if (uiCmdId == (UINT) -1)
	{
		GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
		return;
	}

	UINT uiTrackId = uiCmdId;
	if (IsSystemCommand (uiCmdId))
	{
		uiTrackId = ID_COMMAND_FROM_SC (uiCmdId);
		ASSERT (uiTrackId >= AFX_IDS_SCFIRST &&
				uiTrackId < AFX_IDS_SCFIRST + 31);
	}
	else if (uiCmdId >= AFX_IDM_FIRST_MDICHILD)
	{
		// all MDI Child windows map to the same help id
		uiTrackId = AFX_IDS_MDICHILD;
	}

	GetOwner()->SendMessage (WM_SETMESSAGESTRING, (WPARAM) uiTrackId);
}
//*****************************************************************************************
afx_msg LRESULT CBCGToolBar::OnMouseLeave(WPARAM,LPARAM)
{
	ASSERT_VALID( this );
	if (m_hookMouseHelp != NULL || 
		(m_bMenuMode && !IsCustomizeMode () && GetDroppedDownMenu () != NULL))
	{
		return 0;
	}

	m_bTracked = FALSE;
	m_ptLastMouse = CPoint (-1, -1);

	if (m_iHighlighted >= 0 && GetFocus () != this) // [BT 4091]
	if (m_iHighlighted >= 0 /* && GetFocus () != this */)
	{
		int iButton = m_iHighlighted;
		m_iHighlighted = -1;

		OnChangeHot (m_iHighlighted);

		InvalidateButton (iButton);
		UpdateWindow(); // immediate feedback

		GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	}

	return 0;
}
//*****************************************************************************************
BOOL CBCGToolBar::CanBeRestored () const
{
	return (m_uiOriginalResID != 0);
}
//*****************************************************************************************
BOOL CBCGToolBar::IsLastCommandFromButton (CBCGToolbarButton* pButton)
{
	ASSERT_VALID (pButton);

	HWND hwnd = pButton->GetHwnd ();

	if (!::IsWindow(hwnd) ||
		!(::GetWindowLong (hwnd, GWL_STYLE) & WS_VISIBLE))
	{
		return FALSE;
	}

	const MSG* pMsg = CWnd::GetCurrentMessage();
	if (pMsg == NULL)
	{
		return FALSE;
	}

	return (hwnd == (HWND) pMsg->lParam || hwnd == pMsg->hwnd);
}
//*****************************************************************************************
BOOL CBCGToolBar::AddToolBarForImageCollection (UINT uiResID)
{
	CBCGToolBar tlbTmp;
	return tlbTmp.LoadToolBar (uiResID);
}
//*****************************************************************************************
void CBCGToolBar::SetHotTextColor (COLORREF clrText)
{
	m_clrTextHot = clrText;
}
//*****************************************************************************************
COLORREF CBCGToolBar::GetHotTextColor ()
{
	return m_clrTextHot == (COLORREF) -1 ?
		globalData.clrBtnText : m_clrTextHot;
}
//*****************************************************************************************
void CBCGToolBar::OnBcgbarresToolbarReset() 
{
	ASSERT_VALID( this );
	CBCGLocalResource locaRes;

	ASSERT (IsCustomizeMode () && !m_bLocked);
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	int iImage = 0;
	if (m_DefaultImages.Lookup (pButton->m_nID, iImage))
	{
//		pButton->m_bUserButton = FALSE;
		pButton->SetImage (iImage);
		pButton->m_bImage = TRUE;
	}
	else
	{
		pButton->m_bImage = FALSE;
	}

	pButton->m_bText = m_bMenuMode || !pButton->m_bImage;

	//----------------------
	// Restore default text:
	//----------------------
	OnSetDefaultButtonText (pButton);

	AdjustLayout ();
	CMD_MGR.ClearCmdImage (pButton->m_nID);

	Invalidate ();
}
//***************************************************************************************
afx_msg LRESULT CBCGToolBar::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	OnCancelMode ();

	int nIndex = HitTest ((DWORD) lParam);
	if (nIndex < 0)	// Click into the empty space or separator,
	{				// don't show HELP
		MessageBeep ((UINT) -1);
		return -1;
	}

	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return -1;
	}

	if (pButton->OnContextHelp (this))
	{
		return -1;	// Continue help mode
	}

	LRESULT lres = CControlBar::OnHelpHitTest (wParam, lParam);

	if (lres > 0)
	{
		SetHelpMode (FALSE);
	}

	return lres;
}
//****************************************************************************************
LRESULT CALLBACK CBCGToolBar::BCGToolBarMouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)
	{
		return CallNextHookEx (m_hookMouseHelp, nCode, wParam, lParam);
	}

	MOUSEHOOKSTRUCT* lpMS = (MOUSEHOOKSTRUCT*) lParam;
	ASSERT (lpMS != NULL);

	if (wParam == WM_MOUSEMOVE)
	{
		//------------------------------------------
		// Find a toolbar matched to the mouse hook:
		//------------------------------------------
		CBCGToolBar* pWndToolBar = 
			DYNAMIC_DOWNCAST (CBCGToolBar, CWnd::WindowFromPoint (lpMS->pt));
		if (pWndToolBar != NULL)
		{
			CPoint ptClient = lpMS->pt;
			pWndToolBar->ScreenToClient (&ptClient);
			pWndToolBar->OnMouseMove (0, ptClient);
		}

		if (m_pLastHookedToolbar != NULL &&
			m_pLastHookedToolbar != pWndToolBar)
		{
			m_pLastHookedToolbar->m_bTracked = FALSE;
			m_pLastHookedToolbar->m_ptLastMouse = CPoint (-1, -1);

			if (m_pLastHookedToolbar->m_iHighlighted >= 0)
			{
				int iButton = m_pLastHookedToolbar->m_iHighlighted;
				m_pLastHookedToolbar->m_iHighlighted = -1;

				m_pLastHookedToolbar->OnChangeHot (m_pLastHookedToolbar->m_iHighlighted);

				m_pLastHookedToolbar->InvalidateButton (iButton);
				m_pLastHookedToolbar->UpdateWindow(); // immediate feedback
			}
		}

		m_pLastHookedToolbar = pWndToolBar;
	}

	return 0;
}
//***************************************************************************************
void CBCGToolBar::SetHelpMode (BOOL bOn)
{
	if (bOn)
	{
		if (m_hookMouseHelp == NULL)	// Not installed yet, set it now!
		{
			m_hookMouseHelp = ::SetWindowsHookEx (WH_MOUSE, BCGToolBarMouseProc, 
				0, GetCurrentThreadId ());
			if (m_hookMouseHelp == NULL)
			{
				TRACE (_T("CBCGToolBar: Can't set mouse hook!\n"));
			}
		}
	}
	else if (m_hookMouseHelp != NULL)
	{
		::UnhookWindowsHookEx (m_hookMouseHelp);
		m_hookMouseHelp = NULL;

		m_pLastHookedToolbar = NULL;

		for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
			ASSERT (pToolBar != NULL);

			if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
			{
				ASSERT_VALID (pToolBar);
				pToolBar->OnCancelMode ();
			}
		}
	}
}
//***************************************************************************************
void CBCGToolBar::SetNonPermittedCommands (CList<UINT, UINT>& lstCommands)
{
	m_lstUnpermittedCommands.RemoveAll ();
	m_lstUnpermittedCommands.AddTail (&lstCommands);
}
//***************************************************************************************
void CBCGToolBar::SetBasicCommands (CList<UINT, UINT>& lstCommands)
{
	m_lstBasicCommands.RemoveAll ();
	m_lstBasicCommands.AddTail (&lstCommands);
}
//***************************************************************************************
void CBCGToolBar::AddBasicCommand (UINT uiCmd)
{
	if (m_lstBasicCommands.Find (uiCmd) == NULL)
	{
		m_lstBasicCommands.AddTail (uiCmd);
	}
}
//***************************************************************************************
void CBCGToolBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	ASSERT_VALID( this );
	m_Impl.CalcNcSize (lpncsp);
}
//***************************************************************************************
void CBCGToolBar::OnNcPaint() 
{
	ASSERT_VALID( this );
	m_Impl.DrawNcArea ();
}
//****************************************************************************************
LRESULT CBCGToolBar::OnNcHitTest(CPoint /*point*/) 
{
	return HTCLIENT;
}
//***************************************************************************************
void CBCGToolBar::AdjustLayout ()
{
	ASSERT_VALID( this );
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->m_bTextBelow = ((pButton->m_nStyle & TBBS_SEPARATOR) == 0) &&
								m_bTextLabels && bHorz;
	}	

	CReBar* pBar = DYNAMIC_DOWNCAST (CReBar, GetParent ());
	if (pBar != NULL)
	{
		CReBarCtrl& wndReBar = pBar->GetReBarCtrl ();
		UINT uiReBarsCount = wndReBar.GetBandCount ();

		REBARBANDINFO bandInfo;
		bandInfo.cbSize = sizeof (bandInfo);
		bandInfo.fMask = (RBBIM_CHILDSIZE | RBBIM_CHILD);

		for (UINT uiBand = 0; uiBand < uiReBarsCount; uiBand ++)
		{
			wndReBar.GetBandInfo (uiBand, &bandInfo);
			if (bandInfo.hwndChild == GetSafeHwnd ())
			{
				break;
			}
		}

		if (uiBand >= uiReBarsCount)
		{
			ASSERT (FALSE);
		}
		else
		{
			m_nMaxBtnHeight = CalcMaxButtonHeight ();
			CSize sizeMin = CalcSize (FALSE);

			CRect rect; rect.SetRectEmpty();
			CalcInsideRect (rect, TRUE);
			sizeMin.cy -= rect.Height();
			sizeMin.cx -= rect.Width();

			CSize size = CControlBar::CalcFixedLayout (FALSE, TRUE);
			sizeMin.cx = max(sizeMin.cx, size.cx);
			sizeMin.cy = max(sizeMin.cy, size.cy);

			bandInfo.cxMinChild = sizeMin.cx;
			bandInfo.cyMinChild = sizeMin.cy;

			wndReBar.SetBandInfo (uiBand, &bandInfo);
			wndReBar.MinimizeBand (uiBand);
		}
	}
	else
	{
		CFrameWnd* pParent = GetParentFrame ();
		if (pParent != NULL && pParent->GetSafeHwnd () != NULL)
		{
			pParent->RecalcLayout ();
		}
	}

	AdjustLocations ();
	UpdateWindow ();
	Invalidate ();
}
//****************************************************************************************
void CBCGToolBar::OnBcgbarresCopyImage() 
{
	ASSERT_VALID( this );
	ASSERT (m_iSelected >= 0);

	CBCGToolbarButton* pButton = GetButton(m_iSelected);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));
	ASSERT (pButton->GetImage () >= 0);

	CBCGToolBarImages* pImages = (pButton->m_bUserButton) ? 
			m_pUserImages : &m_Images;
	ASSERT (pImages != NULL);

	CWaitCursor wait;
	pImages->CopyImageToClipboard (pButton->GetImage ());
}
//****************************************************************************************
BOOL CBCGToolBar::OnSetDefaultButtonText (CBCGToolbarButton* pButton)
{
	ASSERT_VALID( this );
	ASSERT_VALID (pButton);

	if (pButton->m_nID == 0 || pButton->m_nID == (UINT) -1)
	{
		return FALSE;
	}

	CString strMessage;
	int iOffset;

	if (strMessage.LoadString (pButton->m_nID) &&
		(iOffset = strMessage.Find (_T('\n'))) != -1)
	{
		pButton->m_strText = strMessage.Mid (iOffset + 1);
		return TRUE;
	}

	return FALSE;
}
//****************************************************************************************
void CBCGToolBar::SetMenuSizes (SIZE sizeButton, SIZE sizeImage)
{
	ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);

	//-----------------------------------------------------------------
	// Button must be big enough to hold image + 3 pixels on each side:
	//-----------------------------------------------------------------
	ASSERT(sizeButton.cx >= sizeImage.cx + 6);
	ASSERT(sizeButton.cy >= sizeImage.cy + 6);

	m_sizeMenuButton = sizeButton;
	m_sizeMenuImage = sizeImage;

	m_MenuImages.SetImageSize (m_sizeMenuImage);
}
//****************************************************************************************
CSize CBCGToolBar::GetMenuImageSize ()
{
	if (m_sizeMenuImage.cx == -1)
	{
		return m_sizeImage;
	}
	else
	{
		return m_sizeMenuImage;
	}
}
//****************************************************************************************
CSize CBCGToolBar::GetMenuButtonSize ()
{
	if (m_sizeMenuButton.cx == -1)
	{
		return m_sizeButton;
	}
	else
	{
		return m_sizeMenuButton;
	}
}
//****************************************************************************************
BOOL CBCGToolBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
{
	ASSERT_VALID( this );
	ASSERT_VALID (pButton);
	ASSERT_VALID (pPopup);

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	if (!pButton->m_bImage || pButton->GetImage () < 0)
	{
		pPopup->EnableMenuItem (ID_BCGBARRES_COPY_IMAGE, MF_BYCOMMAND | MF_GRAYED);
	}

	if (pButton->m_nID == (UINT) -1 || pButton->m_nID == 0)
	{
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_RESET, MF_BYCOMMAND | MF_GRAYED);
	}

	if (pButton->m_bText || (pButton->m_bTextBelow && bHorz))
	{
		if (pButton->m_bImage)
		{
			pPopup->CheckMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, MF_CHECKED  | MF_BYCOMMAND);
		}
		else
		{
			pPopup->CheckMenuItem (ID_BCGBARRES_TOOLBAR_TEXT, MF_CHECKED  | MF_BYCOMMAND);
		}
	}
	else
	{
		ASSERT (pButton->m_bImage);
		pPopup->CheckMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE, MF_CHECKED | MF_BYCOMMAND);
	}

	if (pButton->m_bTextBelow && bHorz)
	{
		//------------------------
		// Text is always visible!
		//------------------------
		pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE, MF_BYCOMMAND | MF_GRAYED);
	}

	//---------------------------
	// Adjust "Start group" item:
	//---------------------------
	CBCGToolbarButton* pPrevButton = NULL;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pCurrButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pCurrButton);

		if (pCurrButton == pButton)
		{
			if (pPrevButton == NULL)	// First button
			{
				pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_START_GROUP, MF_BYCOMMAND | MF_GRAYED);
			}
			else if (pPrevButton->m_nStyle & TBBS_SEPARATOR)
			{
				pPopup->CheckMenuItem (ID_BCGBARRES_TOOLBAR_START_GROUP, MF_CHECKED  | MF_BYCOMMAND);
			}

			break;
		}

		pPrevButton = pCurrButton;
	}

	return TRUE;
}
//***************************************************************************************
void CBCGToolBar::OnChangeHot (int iHot)
{
	ASSERT_VALID( this );
	if (m_iHot == iHot && m_iHot >= 0)
	{
		iHot = -1;
	}

	m_iHot = iHot;

	CBCGToolbarMenuButton* pCurrPopupMenu = GetDroppedDownMenu ();
	if (pCurrPopupMenu == NULL && !CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

	if (iHot < 0 || iHot >= m_Buttons.GetCount())
	{
		m_iHot = -1;
		if (pCurrPopupMenu != NULL && CBCGToolBar::IsCustomizeMode () &&
			!m_bAltCustomizeMode)
		{
			pCurrPopupMenu->OnCancelMode ();
		}

		return;
	}

	CBCGToolbarMenuButton* pMenuButton =
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (iHot));

	if (pMenuButton != pCurrPopupMenu)
	{
		if (pCurrPopupMenu != NULL)
		{
			pCurrPopupMenu->OnCancelMode ();
		}

		if (pMenuButton != NULL &&
			(!CBCGToolBar::IsCustomizeMode () || 
			!pMenuButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarSystemMenuButton))))
		{
			pMenuButton->OnClick (this);
		}
	}
	else
	{
		if (CBCGToolBar::IsCustomizeMode () &&
			pCurrPopupMenu != NULL && pCurrPopupMenu->IsDroppedDown ())
		{
			pCurrPopupMenu->OnCancelMode ();
		}
	}

	//------------------------------------
	// Fixed by Sven Ritter (SpeedProject)
	//------------------------------------
	if (IsCustomizeMode () && m_iDragIndex < 0)
	{
		int nSelected = m_iHighlighted;
		m_iSelected = m_iHot;

		if (nSelected != -1)
		{
			InvalidateButton (nSelected);
		}

		CBCGToolbarButton* pSelButton = GetButton (m_iSelected);
		if (pSelButton == NULL)
		{
			ASSERT (FALSE);
			return;
		}

		if (pSelButton->m_nStyle & TBBS_SEPARATOR)
		{
			m_iSelected = -1;
		}
		else
		{
			InvalidateButton (m_iSelected);
		}
	}
	// --- End ----

	if (m_iHot >= 0 && m_iHot != m_iHighlighted)
	{
		int iCurrHighlighted = m_iHighlighted;
		if (iCurrHighlighted >= 0)
		{
			InvalidateButton (iCurrHighlighted);
		}

		m_iHighlighted = m_iHot;

		InvalidateButton (m_iHighlighted);
		UpdateWindow ();
	}
}
//*****************************************************************************************
BOOL CBCGToolBar::PrevMenu ()
{
	ASSERT_VALID( this );
	int iHot;
	CBCGToolbarMenuButton* pCurrPopupMenu = GetDroppedDownMenu (&iHot);
	if (pCurrPopupMenu == NULL)
	{
		return FALSE;
	}

	int iHotOriginal = iHot;
	int iTotalItems = GetCount ();

	while (--iHot != iHotOriginal)
	{
		if (iHot < 0)
		{
			iHot = iTotalItems - 1;
		}

		if (DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (iHot)) != NULL)
		{
			break;
		}
	}

	if (iHot == iHotOriginal)	// Only one menu item on the toolbar,
	{							// do nothing
		return TRUE;
	}

	//-------------------------------------------
	// Save animation type and disable animation:
	//-------------------------------------------
	CBCGPopupMenu::ANIMATION_TYPE animType = CBCGPopupMenu::GetAnimationType ();
	CBCGPopupMenu::SetAnimationType (CBCGPopupMenu::NO_ANIMATION);

	OnChangeHot (iHot);

	//-----------------------
	// Select the first item:
	//-----------------------
	if (m_iHot >= 0)
	{
		CBCGToolbarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (m_iHot));
		if (pMenuButton != NULL && pMenuButton->IsDroppedDown ())
		{
			pMenuButton->m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
		}
	}

	//-------------------
	// Restore animation:
	//-------------------
	CBCGPopupMenu::SetAnimationType (animType);
	return TRUE;
}
//*****************************************************************************************
BOOL CBCGToolBar::NextMenu ()
{
	ASSERT_VALID( this );
	int iHot;
	CBCGToolbarMenuButton* pCurrPopupMenu = GetDroppedDownMenu (&iHot);
	if (pCurrPopupMenu == NULL)
	{
		return FALSE;
	}

	int iHotOriginal = iHot;
	int iTotalItems = GetCount ();

	while (++iHot != iHotOriginal)
	{
		if (iHot >= iTotalItems)
		{
			iHot = 0;
		}

		if (DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (iHot)) != NULL)
		{
			break;
		}
	}

	if (iHot == iHotOriginal)	// Only one menu item on the toolbar,
	{							// do nothing
		return TRUE;
	}

	//-------------------------------------------
	// Save animation type and disable animation:
	//-------------------------------------------
	CBCGPopupMenu::ANIMATION_TYPE animType = CBCGPopupMenu::GetAnimationType ();
	CBCGPopupMenu::SetAnimationType (CBCGPopupMenu::NO_ANIMATION);

	OnChangeHot (iHot);

	//-----------------------
	// Select the first item:
	//-----------------------
	if (m_iHot >= 0)
	{
		CBCGToolbarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (m_iHot));
		if (pMenuButton != NULL && pMenuButton->IsDroppedDown ())
		{
			pMenuButton->m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
		}
	}

	//-------------------
	// Restore animation:
	//-------------------
	CBCGPopupMenu::SetAnimationType (animType);
	return TRUE;
}
//*****************************************************************************************
BOOL CBCGToolBar::SetHot (CBCGToolbarButton *pMenuButton)
{
	ASSERT_VALID( this );
	if (pMenuButton == NULL)
	{
		m_iHot = -1;
		return TRUE;
	}

	int i = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pMenuButton == pButton)
		{
			if (m_iHot != i)
			{
				OnChangeHot (i);
			}
			return TRUE;
		}
	}

	return FALSE;
}
//**************************************************************************************
BOOL CBCGToolBar::DropDownMenu (CBCGToolbarButton* pButton)
{
	ASSERT_VALID( this );
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT_VALID (pButton);

	CBCGToolbarMenuButton* pMenuButton =
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);
	if (pMenuButton == NULL)
	{
		return FALSE;
	}

	if (!pMenuButton->OnClick (this))
	{
		return FALSE;
	}

	if (m_iHot < 0)
	{
		return FALSE;
	}
			
	//-----------------------
	// Select the first item:
	//-----------------------
	CBCGToolbarMenuButton* pPopupMenuButton =
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (m_iHot));
	if (pPopupMenuButton != NULL && pPopupMenuButton->IsDroppedDown ())
	{
		pPopupMenuButton->m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
	}

	return TRUE;
}
//********************************************************************************************
BOOL CBCGToolBar::ProcessCommand (CBCGToolbarButton* pButton)
{
	ASSERT_VALID( this );
	ASSERT_VALID (pButton);

	if (pButton->m_nID == 0 ||
		pButton->m_nID == (UINT) -1)
	{
		return FALSE;
	}

	BCGPlaySystemSound (BCGSOUND_MENU_COMMAND);

	//----------------------------------
	// Send command to the parent frame:
	//----------------------------------
	CFrameWnd* pParentFrame = GetParentFrame ();
	ASSERT_VALID (pParentFrame);

	AddCommandUsage (pButton->m_nID);

	GetOwner()->PostMessage(WM_COMMAND, pButton->m_nID);
	return TRUE;
}
//********************************************************************************************
CBCGToolbarMenuButton* CBCGToolBar::GetDroppedDownMenu (int* pIndex) const
{
	ASSERT_VALID( this );
	if (m_Buttons.IsEmpty ())
	{
		return NULL;
	}

	int iIndex = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iIndex ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		CBCGToolbarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);

		if (pMenuButton != NULL && pMenuButton->IsDroppedDown ())
		{
			if (pIndex != NULL)
			{
				*pIndex = iIndex;
			}

			return pMenuButton;
		}
	}

	if (pIndex != NULL)
	{
		*pIndex = -1;
	}

	return NULL;
}
//******************************************************************
void CBCGToolBar::Deactivate ()
{
	ASSERT_VALID( this );
	if (m_iHighlighted >= 0 && m_iHighlighted < m_Buttons.GetCount ())
	{
		int iButton = m_iHighlighted;
		m_iHighlighted = m_iHot = -1;

		InvalidateButton (iButton);
		UpdateWindow ();

		GetOwner()->SendMessage (WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	}

	RestoreFocus ();
}
//*********************************************************************
BOOL CBCGToolBar::SaveParameters (LPCTSTR lpszProfileName)
{
	CString strProfileName = ::BCGGetRegPath (strToolbarProfile, lpszProfileName);

	BOOL bResult = FALSE;

	CString strSection;
	strSection.Format (REG_PARAMS_FMT, strProfileName);

	CBCGRegistry reg (FALSE, FALSE);
	if (reg.CreateKey (strSection))
	{
		bResult =	reg.Write (REG_ENTRY_TOOLTIPS, m_bShowTooltips) &&
					reg.Write (REG_ENTRY_KEYS, m_bShowShortcutKeys) &&
					reg.Write (REG_ENTRY_LARGE_ICONS, m_bLargeIcons) &&
					reg.Write (REG_ENTRY_ANIMATION, (int) CBCGPopupMenu::GetAnimationType ()) &&
					reg.Write (REG_ENTRY_RU_MENUS, CBCGMenuBar::m_bRecentlyUsedMenus) &&
					reg.Write (REG_ENTRY_MENU_SHADOWS, CBCGMenuBar::m_bMenuShadows) &&
					reg.Write (REG_ENTRY_SHOW_ALL_MENUS_DELAY, CBCGMenuBar::m_bShowAllMenusDelay) &&
					reg.Write (REG_ENTRY_LOOK2000, m_bLook2000) &&
					reg.Write (REG_ENTRY_CMD_USAGE_COUNT, m_UsageCount);
	}

	return bResult;
}
//*****************************************************************************************
BOOL CBCGToolBar::LoadParameters (LPCTSTR lpszProfileName)
{
	CString strProfileName = ::BCGGetRegPath (strToolbarProfile, lpszProfileName);

	BOOL bResult = FALSE;

	CString strSection;
	strSection.Format (REG_PARAMS_FMT, strProfileName);

	CBCGRegistry reg (FALSE, TRUE);
	if (!reg.Open (strSection))
	{
		return FALSE;
	}

	int iAnimType = CBCGPopupMenu::NO_ANIMATION;
	bResult =	reg.Read (REG_ENTRY_TOOLTIPS, m_bShowTooltips) &&
				reg.Read (REG_ENTRY_KEYS, m_bShowShortcutKeys) &&
				reg.Read (REG_ENTRY_LARGE_ICONS, m_bLargeIcons) &&
				reg.Read (REG_ENTRY_ANIMATION, iAnimType) &&
				reg.Read (REG_ENTRY_RU_MENUS, CBCGMenuBar::m_bRecentlyUsedMenus) &&
				reg.Read (REG_ENTRY_MENU_SHADOWS, CBCGMenuBar::m_bMenuShadows) &&
				reg.Read (REG_ENTRY_SHOW_ALL_MENUS_DELAY, CBCGMenuBar::m_bShowAllMenusDelay) &&
				reg.Read (REG_ENTRY_LOOK2000, m_bLook2000) &&
				reg.Read (REG_ENTRY_CMD_USAGE_COUNT, m_UsageCount);

	CBCGPopupMenu::SetAnimationType ((CBCGPopupMenu::ANIMATION_TYPE) iAnimType);
	SetLargeIcons (m_bLargeIcons);
	SetLook2000(m_bLook2000);

	return bResult;
}
//**********************************************************************************
void CBCGToolBar::OnSetFocus(CWnd* pOldWnd) 
{
	ASSERT_VALID( this );
	CControlBar::OnSetFocus(pOldWnd);

	if (pOldWnd != NULL &&
		DYNAMIC_DOWNCAST (CBCGToolBar, pOldWnd) == NULL &&
		DYNAMIC_DOWNCAST (CBCGToolBar, pOldWnd->GetParent ()) == NULL)
	{
		m_hwndLastFocus = pOldWnd->GetSafeHwnd ();
	}
}
//**********************************************************************************
void CBCGToolBar::RestoreFocus ()
{
	ASSERT_VALID( this );
	if (::IsWindow (m_hwndLastFocus))
	{
		::SetFocus (m_hwndLastFocus);
	}

	m_hwndLastFocus = NULL;
}
//*********************************************************************************
void CBCGToolBar::OnBcgbarresToolbarNewMenu() 
{
	ASSERT_VALID( this );
	CBCGToolbarMenuButton* pMenuButton = new CBCGToolbarMenuButton;
	pMenuButton->m_bText = TRUE;
	pMenuButton->m_bImage = FALSE;

	CBCGLocalResource locaRes;
	CButtonAppearanceDlg dlg (pMenuButton, m_pUserImages, this, 0, m_bMenuMode);
	if (dlg.DoModal () != IDOK)
	{
		delete pMenuButton;
		return;
	}

	m_iSelected = InsertButton (pMenuButton, m_iSelected);

	AdjustLayout ();
	Invalidate ();

	pMenuButton->OnClick (this, FALSE);
}
//**********************************************************************************
void CBCGToolBar::SetToolBarBtnText (UINT nBtnIndex,
									LPCTSTR szText,
									BOOL bShowText,
									BOOL bShowImage)
{
	ASSERT_VALID( this );
	//
	// Added by Christian Skovdal Andersen
	//

	CBCGToolbarButton* pButton = GetButton (nBtnIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ASSERT(!(pButton->m_nStyle & TBBS_SEPARATOR));

	if (bShowText)
	{
		if (szText == NULL)
		{
			OnSetDefaultButtonText(pButton);
		}
		else
		{
			SetButtonText(nBtnIndex, szText);
		}
	}

	pButton->m_bText = bShowText;
	pButton->m_bImage = bShowImage;
}
//*************************************************************************************
void CBCGToolBar::SetLook2000 (BOOL bLook2000/* = TRUE*/)
{
	m_bLook2000 = bLook2000;

	if (AfxGetMainWnd () != NULL)
	{
		AfxGetMainWnd()->RedrawWindow (NULL, NULL,
				RDW_INVALIDATE | RDW_ERASENOW | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
	}
}
//*************************************************************************************
void CBCGToolBar::SetLargeIcons (BOOL bLargeIcons/* = TRUE*/)
{
	m_bLargeIcons = bLargeIcons;

	if (m_bLargeIcons)
	{
		m_sizeCurButton.cx = m_sizeButton.cx * 2;
		m_sizeCurButton.cy = m_sizeButton.cy * 2;

		m_sizeCurImage.cx = m_sizeImage.cx * 2;
		m_sizeCurImage.cy = m_sizeImage.cy * 2;
	}
	else
	{
		m_sizeCurButton = m_sizeButton;
		m_sizeCurImage = m_sizeImage;
	}

	for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);

			if (pToolBar->m_bLocked)
			{
				// Locked toolbars have its individual sizes
				if (m_bLargeIcons)
				{
					pToolBar->m_sizeCurButtonLocked.cx = pToolBar->m_sizeButtonLocked.cx * 2;
					pToolBar->m_sizeCurButtonLocked.cy = pToolBar->m_sizeButtonLocked.cy * 2;

					pToolBar->m_sizeCurImageLocked.cx = pToolBar->m_sizeImageLocked.cx * 2;
					pToolBar->m_sizeCurImageLocked.cy = pToolBar->m_sizeImageLocked.cy * 2;
				}
				else
				{
					pToolBar->m_sizeCurButtonLocked = pToolBar->m_sizeButtonLocked;
					pToolBar->m_sizeCurImageLocked = pToolBar->m_sizeImageLocked;
				}
			}

			pToolBar->AdjustLayout ();
		}
	}
}
//************************************************************************************
BOOL CBCGToolBar::IsCommandRarelyUsed (UINT uiCmd)
{
	if (IsCustomizeMode () ||
		uiCmd == 0 || uiCmd == (UINT) -1 ||
		IsStandardCommand (uiCmd) ||
		m_lstBasicCommands.IsEmpty ())
	{
		return FALSE;
	}

	return !IsBasicCommand (uiCmd) &&
		!m_UsageCount.IsFreqeuntlyUsedCmd (uiCmd);
}
//************************************************************************************
void CBCGToolBar::EnableLargeIcons (BOOL bEnable)
{
	ASSERT_VALID( this );
	ASSERT (GetSafeHwnd () == NULL);	// Should not be created yet!
	m_bLargeIconsAreEnbaled = bEnable;
}
//*************************************************************************************
void CBCGToolBar::EnableCustomizeButton (BOOL bEnable, UINT uiCustomizeCmd, 
										 const CString& strCustomizeText)
{
	ASSERT_VALID( this );
	if (bEnable)
	{
		ASSERT (uiCustomizeCmd > 0);
		if (InsertButton (CCustomizeButton (uiCustomizeCmd, strCustomizeText)) < 0)
		{
			ASSERT (FALSE);
			return;
		}

		m_pCustomizeBtn = DYNAMIC_DOWNCAST (CCustomizeButton, m_Buttons.GetTail ());
		ASSERT_VALID (m_pCustomizeBtn);
	}
	else if (m_pCustomizeBtn != NULL)
	{
		ASSERT_VALID (m_pCustomizeBtn);
		ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last

		m_Buttons.RemoveTail ();
		delete m_pCustomizeBtn;
		m_pCustomizeBtn = NULL;
	}
}
//*******************************************************************************************
void CBCGToolBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	ASSERT_VALID( this );
	CControlBar::OnWindowPosChanging(lpwndpos);
	
	m_iRebarPaneWidth = -1;

	CReBar* pBar = DYNAMIC_DOWNCAST (CReBar, GetParent ());
	if (pBar != NULL)
	{
		CReBarCtrl& wndReBar = pBar->GetReBarCtrl ();
		UINT uiReBarsCount = wndReBar.GetBandCount ();

		REBARBANDINFO bandInfo;
		bandInfo.cbSize = sizeof (bandInfo);
		bandInfo.fMask = RBBIM_CHILD;

		for (UINT uiBand = 0; uiBand < uiReBarsCount; uiBand ++)
		{
			wndReBar.GetBandInfo (uiBand, &bandInfo);
			if (bandInfo.hwndChild == GetSafeHwnd ())
			{
				CRect rectRebar;
				wndReBar.GetClientRect (rectRebar);

				CRect rectBand;
				wndReBar.GetBandBorders (uiBand, rectBand);

				m_iRebarPaneWidth = rectRebar.Width () - rectBand.left - 1;
				AdjustLocations ();
				break;
			}
		}
	}
}
//*******************************************************************************************
void CBCGToolBar::EnableTextLabels (BOOL bEnable/* = TRUE*/)
{
	ASSERT_VALID( this );
	if (m_bMenuMode)
	{
		ASSERT (FALSE);
		return;
	}

	m_bTextLabels = bEnable;
	AdjustLayout ();
}
//****************************************************************************************
int CBCGToolBar::CalcMaxButtonHeight ()
{
	ASSERT_VALID (this);

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;
	m_bDrawTextLabels = FALSE;

	if (!m_bTextLabels || !bHorz)
	{
		return 0;
	}

	int nMaxBtnHeight = 0;
	CClientDC dc (this);

	CFont* pOldFont = (CFont*) dc.SelectObject (&globalData.fontRegular);
	ASSERT (pOldFont != NULL);

	//-----------------------------------------------------------------------
	// To better look, I'm assuming that all rows shoud be of the same height.
	// Calculate max. button height:
	//-----------------------------------------------------------------------
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);
		
		if (pButton->m_bTextBelow)
		{
			if (pButton->m_strText.IsEmpty ())
			{
				OnSetDefaultButtonText (pButton);
			}

			CSize sizeButton = pButton->OnCalculateSize (&dc, 
				GetButtonSize (), bHorz);

			nMaxBtnHeight = max (nMaxBtnHeight, sizeButton.cy);
		}
	}

	m_bDrawTextLabels = (nMaxBtnHeight > GetButtonSize ().cy);
	dc.SelectObject (pOldFont);
	return nMaxBtnHeight;
}
//***************************************************************************************
void CBCGToolBar::ResetAllImages()
{
	//----------------
	// By Guy Hachlili
	//----------------
	m_Images.Clear();
	m_ColdImages.Clear();
	m_MenuImages.Clear();
	m_DefaultImages.RemoveAll();
}
//**********************************************************************************
BOOL CBCGToolBar::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//**********************************************************************************
BOOL CBCGToolBar::OnUserToolTip (CBCGToolbarButton* pButton, CString& strTTText) const
{
	ASSERT_VALID( this );
	ASSERT_VALID (pButton);

	CFrameWnd* pTopFrame = GetTopLevelFrame ();
	if (pTopFrame == NULL)
	{
		return FALSE;
	}

	CBCGMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, pTopFrame);
	if (pMainFrame != NULL)
	{
		return pMainFrame->GetToolbarButtonToolTipText (pButton, strTTText);
	}
	else	// Maybe, SDI frame...
	{
		CBCGFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, pTopFrame);
		if (pFrame != NULL)
		{
			return pFrame->GetToolbarButtonToolTipText (pButton, strTTText);
		}
		else	// Maybe, OLE frame...
		{
			CBCGOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, pFrame);
			if (pOleFrame != NULL)
			{
				return pOleFrame->GetToolbarButtonToolTipText (pButton, strTTText);
			}
		}
	}

	return FALSE;
}
//*****************************************************************************************
void CBCGToolBar::OnKillFocus(CWnd* pNewWnd) 
{
	ASSERT_VALID( this );
	CControlBar::OnKillFocus(pNewWnd);
	
	if (!IsCustomizeMode ())
	{
		CBCGPopupMenu* pMenu = DYNAMIC_DOWNCAST	(CBCGPopupMenu, pNewWnd);
		if (pMenu == NULL || pMenu->GetParentToolBar () != this)
		{
			Deactivate ();
		}
	}
}
//********************************************************************************
void CBCGToolBar::ResetAll ()
{
	CMD_MGR.ClearAllCmdImages ();

	//------------------------------------------
	// Fill image hash by the default image ids:
	//------------------------------------------
	for (POSITION pos = CBCGToolBar::m_DefaultImages.GetStartPosition (); pos != NULL;)
	{
		UINT uiCmdId;
		int iImage;

		CBCGToolBar::m_DefaultImages.GetNextAssoc (pos, uiCmdId, iImage);
		CMD_MGR.SetCmdImage (uiCmdId, iImage, FALSE);
	}

	for (pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT (pToolBar != NULL);

		if (pToolBar->CanBeRestored ())
		{
			pToolBar->RestoreOriginalstate ();
		}
	}
}
//****************************************************************************************
BOOL CBCGToolBar::TranslateChar (UINT nChar)
{
	ASSERT_VALID( this );
	UINT nUpperChar = 0;

	if (m_bExtCharTranslation)
	{
		// By Daniil Kolbasnikov
		WORD wChar;
		BYTE lpKeyState [256];
		::GetKeyboardState (lpKeyState);
 
		::ToAsciiEx (nChar,
					MapVirtualKey (nChar, 0),
					lpKeyState,
					&wChar,
					1,
					::GetKeyboardLayout(AfxGetThread()->m_nThreadID));

		TCHAR szChar [2] = {(TCHAR) wChar, '\0'};
		CharUpper(szChar);

		nUpperChar = (UINT)szChar [0];
	}
	else
	{
		nUpperChar = toupper (nChar);
	}

	CBCGToolbarButton* pButton = NULL;
	if (!m_AcellKeys.Lookup (nUpperChar, pButton))
	{
		return FALSE;
	}

	ASSERT_VALID (pButton);

	//-------------------------------------------
	// Save animation type and disable animation:
	//-------------------------------------------
	CBCGPopupMenu::ANIMATION_TYPE animType = CBCGPopupMenu::GetAnimationType ();
	CBCGPopupMenu::SetAnimationType (CBCGPopupMenu::NO_ANIMATION);

	BOOL bRes = DropDownMenu (pButton);

	//-------------------
	// Restore animation:
	//-------------------
	CBCGPopupMenu::SetAnimationType (animType);

	if (bRes)
	{
		return TRUE;
	}

	return ProcessCommand (pButton);
}

LRESULT CBCGToolBar::OnPaint( WPARAM wParam, LPARAM ) 
{
	ASSERT_VALID( this );
	if( wParam )
	{
		CDC	*pdc = CDC::FromHandle( (HDC)wParam );
		DoPaint( pdc );
	}
	else
	{
		CPaintDC dc(this);
		if (IsVisible())
			DoPaint( &dc );
	}

	if( !m_bChecked )
	{
		int iButton = 0;
		for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
		{
			CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);
			if( !(pButton->m_nStyle & TBBS_SEPARATOR) )
				UpdateButton( iButton );
			
		}
		if( iButton > 0 )
			m_bChecked = true;
	}

	return 0;
}


