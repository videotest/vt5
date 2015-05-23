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

// BCGDialogBar.cpp : implementation file
//

#include "stdafx.h"

#include "bcgcontrolbar.h"
#include "globals.h"
#include "BCGDialogBar.h"

//#include <../src/occimpl.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGDialogBar

IMPLEMENT_SERIAL(CBCGDialogBar, CBCGSizingControlBar, VERSIONABLE_SCHEMA | 1)

CBCGDialogBar::CBCGDialogBar()
{
	m_bAllowSizing = FALSE;

#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = NULL;
	m_pOccDialogInfo = NULL;
#endif
}

CBCGDialogBar::~CBCGDialogBar()
{
//	DestroyWindow();    // avoid PostNcDestroy problems
}

/////////////////////////////////////////////////////////////////////////////
// CBCGDialogBar message handlers

//****************************************************************************************
CSize CBCGDialogBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if(m_bAllowSizing)
	{
		return CBCGSizingControlBar::CalcDynamicLayout(nLength, dwMode);
	}
	else
	{
		return CControlBar::CalcDynamicLayout(nLength, dwMode);
	}
}
//****************************************************************************************
CSize CBCGDialogBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if(m_bAllowSizing)
	{
		return CBCGSizingControlBar::CalcFixedLayout(bStretch, bHorz);
	}
	else
	{
		CSize sizeResult = m_sizeDefault;

		if (bStretch) // if not docked stretch to fit
		{
			if (bHorz)
			{
				sizeResult.cx = 32767;
			}
			else
			{
				sizeResult.cy = 32767;
			}
		}

		//BLOCK: Adjust Margins
		{
			CRect rect; rect.SetRectEmpty();
			CalcInsideRect(rect, bHorz);
			sizeResult.cy -= rect.Height();
			sizeResult.cx -= rect.Width();

			CSize size = CControlBar::CalcFixedLayout (bStretch, bHorz);
			sizeResult.cx = max(sizeResult.cx, size.cx);
			sizeResult.cy = max(sizeResult.cy, size.cy);
		}

		return sizeResult;

	}
}
//****************************************************************************************
BOOL CBCGDialogBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, BOOL bHasGripper, 
						   UINT nIDTemplate, UINT nStyle, UINT nID)
{ 
	return Create(lpszWindowName, pParentWnd, bHasGripper, MAKEINTRESOURCE(nIDTemplate), nStyle, nID); 
}
//****************************************************************************************
BOOL CBCGDialogBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, BOOL bHasGripper, 
						   LPCTSTR lpszTemplateName, UINT nStyle, UINT nID)
{
	ASSERT(pParentWnd != NULL);
	ASSERT(lpszTemplateName != NULL);

	//------------------------------------------------------
    // cannot be both fixed and dynamic
    // (CBRS_SIZE_DYNAMIC is used for resizng when floating)
	//------------------------------------------------------
    ASSERT (!((nStyle & CBRS_SIZE_FIXED) &&
              (nStyle & CBRS_SIZE_DYNAMIC)));

    m_cyGripper = bHasGripper ? 12 : 0; // set the gripper width
	m_bAllowSizing = nStyle & CBRS_SIZE_DYNAMIC ? TRUE : FALSE;

	//------------------------------
	// allow chance to modify styles
	//------------------------------
	m_dwStyle = (nStyle & CBRS_ALL);

	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(cs));
	cs.lpszClass = AFX_WNDCONTROLBAR;
	cs.lpszName = lpszWindowName;
	cs.style = (DWORD)nStyle | WS_CHILD;
	cs.hMenu = (HMENU)nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.hwndParent = pParentWnd->GetSafeHwnd();

	if (!PreCreateWindow(cs))
	{
		return FALSE;
	}

#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = lpszTemplateName;
#endif

	//----------------------------
	// initialize common controls
	//----------------------------
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));
	AfxDeferRegisterClass(AFX_WNDCOMMCTLSNEW_REG);

	//--------------------------
	// create a modeless dialog
	//--------------------------
	if (!CreateDlg (lpszTemplateName, pParentWnd))
	{
		TRACE(_T("Can't create diaog: %s\n"), lpszTemplateName);
		return FALSE;
	}

#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = NULL;
#endif

    SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LPARAM)::GetSysColorBrush(COLOR_BTNFACE));
    SetBarStyleEx (GetBarStyle() | CBRS_SIZE_DYNAMIC);

	//----------------------------------------------
	// dialog template MUST specify that the dialog
	// is an invisible child window
	//----------------------------------------------
	SetDlgCtrlID(nID);

	CRect rect;
	GetWindowRect(&rect);
	m_sizeDefault = rect.Size();    // set fixed size

    m_szHorz = m_sizeDefault; // set the size members
    m_szVert = m_sizeDefault;
    m_szFloat = m_sizeDefault;

	//-----------------------
	// force WS_CLIPSIBLINGS
	//-----------------------
	ModifyStyle(0, WS_CLIPSIBLINGS);

	if (!ExecuteDlgInit(lpszTemplateName))
		return FALSE;

	//--------------------------------------------------------
	// force the size to zero - resizing bar will occur later
	//--------------------------------------------------------
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);

	if (lpszWindowName != NULL)
	{
		SetWindowText (lpszWindowName);
	}

	return TRUE;
}
//****************************************************************************************
void CBCGDialogBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

#ifndef _AFX_NO_OCC_SUPPORT

BEGIN_MESSAGE_MAP(CBCGDialogBar, CBCGSizingControlBar)
	//{{AFX_MSG_MAP(CBCGDialogBar)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
END_MESSAGE_MAP()

LRESULT CBCGDialogBar::HandleInitDialog(WPARAM w, LPARAM l)
{
	Default();  // allow default to initialize first (common dialogs/etc)

	// create OLE controls
/*	COccManager* pOccManager = afxOccManager;
	if ((pOccManager != NULL) && (m_pOccDialogInfo != NULL))
	{
		if (!pOccManager->CreateDlgControls(this, m_lpszTemplateName,
			m_pOccDialogInfo))
		{
			TRACE (_T("Warning: CreateDlgControls failed during dialog bar init.\n"));
			return FALSE;
		}
	}*/


	return TRUE;
}
//*****************************************************************************************
BOOL CBCGDialogBar::SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo)
{
	m_pOccDialogInfo = pOccDialogInfo;
	return TRUE;
}

#endif //!_AFX_NO_OCC_SUPPORT

BOOL CBCGDialogBar::OnEraseBkgnd(CDC* pDC) 
{
	if (!pDC)
		return true;

	CRect rectClient;
	GetClientRect (rectClient);

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild && ::IsWindow(pChild->GetSafeHwnd()))
	{
		CRect ChildRect;
		if (::IsWindowVisible(pChild->GetSafeHwnd()))
		{
			pChild->GetWindowRect(ChildRect);
			ScreenToClient(ChildRect);
			if (!ChildRect.IsRectEmpty())
				pDC->ExcludeClipRect(ChildRect);
		}

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
	
	pDC->FillRect(rectClient, &globalData.brBtnFace);
	return TRUE;
}
