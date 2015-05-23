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
//
// Created by Rui Godinho Lopes <ruiglopes@yahoo.com>
//
//////////////////////////////////////////////////////////////////////

//********************************************************************
//
// BCGPrintPreviewView.cpp : implementation file
//
// REVISION HISTORY
// ----------------
// 0.00 9february2000
//   creation
//
//********************************************************************

#include "stdafx.h"
#include "BCGPrintPreviewView.h"
#include "bcgbarres.h"
#include "bcglocalres.h"

IMPLEMENT_DYNCREATE(CBCGPrintPreviewView, CPreviewView)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPrintPreviewToolBar

BEGIN_MESSAGE_MAP(CBCGPrintPreviewToolBar, CBCGToolBar)
	//{{AFX_MSG_MAP(CBCGPrintPreviewToolBar)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBCGPrintPreviewToolBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*pos*/)
{
	// Prevent print preview toolbar context menu appearing
}

/////////////////////////////////////////////////////////////////////////////
// CBCGPrintPreviewView

CBCGPrintPreviewView::CBCGPrintPreviewView()
{
	m_iPagesBtnIndex = -1;
	m_iOnePageImageIndex = -1;
	m_iTwoPageImageIndex = -1;
}
//*********************************************************************************
CBCGPrintPreviewView::~CBCGPrintPreviewView()
{
}


BEGIN_MESSAGE_MAP(CBCGPrintPreviewView, CPreviewView)
	//{{AFX_MSG_MAP(CBCGPrintPreviewView)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(AFX_ID_PREVIEW_NUMPAGE, OnUpdatePreviewNumPage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGPrintPreviewView message handlers

int CBCGPrintPreviewView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPreviewView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBCGLocalResource locaRes;

	if (!m_wndToolBar.Create (m_pToolBar,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_BORDER_3D) ||
		!m_wndToolBar.LoadToolBar(IDR_BCGRES_PRINT_PREVIEW, 0, 0, TRUE /* Locked */))
	{
		TRACE0("Failed to create print preview toolbar\n");
		return FALSE;      // fail to create
	}

	//-------------------------------------------
	// Remember One Page/Two pages image indexes:
	//-------------------------------------------
	m_iPagesBtnIndex = m_wndToolBar.CommandToIndex (AFX_ID_PREVIEW_NUMPAGE);
	ASSERT (m_iPagesBtnIndex >= 0);
	
	CBCGToolbarButton* pButton= m_wndToolBar.GetButton (m_iPagesBtnIndex);
	ASSERT_VALID (pButton);

	m_iOnePageImageIndex = pButton->GetImage ();

	int iIndex = m_wndToolBar.CommandToIndex (ID_BCGRES_TWO_PAGES_DUMMY);
	ASSERT (iIndex >= 0);
	
	pButton= m_wndToolBar.GetButton (iIndex);
	ASSERT_VALID (pButton);

	m_iTwoPageImageIndex = pButton->GetImage ();

	//---------------------------------
	// Remove dummy "Two pages" button:
	//---------------------------------
	m_wndToolBar.RemoveButton (iIndex);

	//------------------------------------
	// Set "Print" button to image + text:
	//------------------------------------
	m_wndToolBar.SetToolBarBtnText (m_wndToolBar.CommandToIndex (AFX_ID_PREVIEW_PRINT));

	//---------------------------------
	// Set "Close" button to text only:
	//---------------------------------
	m_wndToolBar.SetToolBarBtnText (m_wndToolBar.CommandToIndex (AFX_ID_PREVIEW_CLOSE),
		NULL, TRUE, FALSE);

	//-------------------------
	// Change the Toolbar size:
	//-------------------------
	SetToolbarSize ();

	//TODO: Set Application Status Bar to Simple Text, that way only the page number is visible

	return 0;
}
//*********************************************************************************
void CBCGPrintPreviewView::OnUpdatePreviewNumPage(CCmdUI *pCmdUI) 
{
	CPreviewView::OnUpdateNumPageChange(pCmdUI);

	//--------------------------------------------------
	// Change the Icon of AFX_ID_PREVIEW_NUMPAGE button:
	//--------------------------------------------------
	CBCGToolbarButton* pButton = m_wndToolBar.GetButton (m_iPagesBtnIndex);
	ASSERT_VALID (pButton);

	UINT nPages = m_nZoomState == ZOOM_OUT ? m_nPages : m_nZoomOutPages;
	pButton->SetImage (nPages == 1 ? m_iTwoPageImageIndex : m_iOnePageImageIndex);
}
//*********************************************************************************
BCGCONTROLBARDLLEXPORT void BCGPrintPreview (CView* pView)
{
	ASSERT_VALID (pView);

	CPrintPreviewState *pState= new CPrintPreviewState;

	CBCGLocalResource locaRes;

	if (!pView->DoPrintPreview (IDD_BCGBAR_RES_PRINT_PREVIEW, pView, 
		RUNTIME_CLASS (CBCGPrintPreviewView), pState))
	{
		TRACE0("Error: OnFilePrintPreview failed.\n");
		AfxMessageBox (AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}
}
//*******************************************************************************
void CBCGPrintPreviewView::OnSize(UINT nType, int cx, int cy) 
{
	CPreviewView::OnSize(nType, cx, cy);
	
	//-------------------------
	// Change the Toolbar size:
	//-------------------------
	SetToolbarSize ();
}
//******************************************************************************
void CBCGPrintPreviewView::SetToolbarSize ()
{
	CSize szSize = m_wndToolBar.CalcFixedLayout (TRUE, TRUE);

	//----------------------------------------------------------------------
	// Print toolbar should occopy the whole width of the mainframe (Win9x):
	//----------------------------------------------------------------------
	CFrameWnd* pParent = GetParentFrame ();
	ASSERT_VALID (pParent);

	CRect rectParent;
	pParent->GetClientRect (rectParent);
	szSize.cx = rectParent.Width ();

	m_wndToolBar.SetWindowPos (NULL, 0, 0, szSize.cx, szSize.cy, 
				SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER);
}
