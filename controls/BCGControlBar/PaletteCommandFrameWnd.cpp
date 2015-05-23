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

// PaletteCommandFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "bcgbarres.h"
#include "ImageEditDlg.h"
#include "PaletteCommandFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteCommandFrameWnd

IMPLEMENT_DYNAMIC(CPaletteCommandFrameWnd, CFrameWnd)

CPaletteCommandFrameWnd::CPaletteCommandFrameWnd()
{
	m_pDlgEdit = NULL;
}

CPaletteCommandFrameWnd::~CPaletteCommandFrameWnd()
{
}

BEGIN_MESSAGE_MAP(CPaletteCommandFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CPaletteCommandFrameWnd)
	ON_COMMAND(ID_BCG_TOOL_CLEAR, OnBcgToolClear)
	ON_COMMAND(ID_BCG_TOOL_COPY, OnBcgToolCopy)
	ON_COMMAND(ID_BCG_TOOL_PASTE, OnBcgToolPaste)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_PASTE, OnUpdateBcgToolPaste)
	ON_COMMAND(ID_BCG_TOOL_ELLIPSE, OnBcgToolEllipse)
	ON_COMMAND(ID_BCG_TOOL_FILL, OnBcgToolFill)
	ON_COMMAND(ID_BCG_TOOL_LINE, OnBcgToolLine)
	ON_COMMAND(ID_BCG_TOOL_PEN, OnBcgToolPen)
	ON_COMMAND(ID_BCG_TOOL_PICK, OnBcgToolPick)
	ON_COMMAND(ID_BCG_TOOL_RECT, OnBcgToolRect)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_ELLIPSE, OnUpdateBcgToolEllipse)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_FILL, OnUpdateBcgToolFill)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_LINE, OnUpdateBcgToolLine)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_PEN, OnUpdateBcgToolPen)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_PICK, OnUpdateBcgToolPick)
	ON_UPDATE_COMMAND_UI(ID_BCG_TOOL_RECT, OnUpdateBcgToolRect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteCommandFrameWnd message handlers

void CPaletteCommandFrameWnd::OnBcgToolClear() 
{
	m_pDlgEdit->Clear ();
}

void CPaletteCommandFrameWnd::OnBcgToolCopy() 
{
	m_pDlgEdit->Copy ();
}

void CPaletteCommandFrameWnd::OnBcgToolPaste() 
{
	m_pDlgEdit->Paste ();
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(::IsClipboardFormatAvailable (CF_BITMAP));
}

void CPaletteCommandFrameWnd::OnBcgToolEllipse() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_ELLIPSE);
}

void CPaletteCommandFrameWnd::OnBcgToolFill() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_FILL);
}

void CPaletteCommandFrameWnd::OnBcgToolLine() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_LINE);
}

void CPaletteCommandFrameWnd::OnBcgToolPen() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_PEN);
}

void CPaletteCommandFrameWnd::OnBcgToolPick() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_COLOR);
}

void CPaletteCommandFrameWnd::OnBcgToolRect() 
{
	m_pDlgEdit->SetMode (CImagePaintArea::IMAGE_EDIT_MODE_RECT);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolEllipse(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_ELLIPSE);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_FILL);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_LINE);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolPen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_PEN);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolPick(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_COLOR);
}

void CPaletteCommandFrameWnd::OnUpdateBcgToolRect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_pDlgEdit->GetMode () ==
		CImagePaintArea::IMAGE_EDIT_MODE_RECT);
}

BOOL CPaletteCommandFrameWnd::Create(DWORD dwStyle, const RECT& rect, 
	CBCGImageEditDlg* pDlgEdit) 
{
	m_pDlgEdit = pDlgEdit;
	ASSERT (m_pDlgEdit != NULL);

	return CFrameWnd::Create(NULL, _T(""), dwStyle, rect, pDlgEdit);
}
