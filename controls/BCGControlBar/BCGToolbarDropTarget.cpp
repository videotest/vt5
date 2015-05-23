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

// BCGToolbarDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "BCGToolbarButton.h"
#include "BCGToolbarDropTarget.h"
#include "BCGToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarDropTarget

CBCGToolbarDropTarget::CBCGToolbarDropTarget()
{
	m_pOwner = NULL;
}

CBCGToolbarDropTarget::~CBCGToolbarDropTarget()
{
}


BEGIN_MESSAGE_MAP(CBCGToolbarDropTarget, COleDropTarget)
	//{{AFX_MSG_MAP(CBCGToolbarDropTarget)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBCGToolbarDropTarget::Register (CBCGToolBar* pOwner)
{
	m_pOwner = pOwner;
	return COleDropTarget::Register (pOwner);
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarDropTarget message handlers

DROPEFFECT CBCGToolbarDropTarget::OnDragEnter(CWnd* /*pWnd*/, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	ASSERT (m_pOwner != NULL);

	if (!m_pOwner->IsCustomizeMode () ||
		!pDataObject->IsDataAvailable (CBCGToolbarButton::m_cFormat))
	{
		return DROPEFFECT_NONE;
	}

	return m_pOwner -> OnDragEnter(pDataObject, dwKeyState, point);
}

void CBCGToolbarDropTarget::OnDragLeave(CWnd* /*pWnd*/) 
{
	ASSERT (m_pOwner != NULL);
	m_pOwner->OnDragLeave ();
}

DROPEFFECT CBCGToolbarDropTarget::OnDragOver(CWnd* /*pWnd*/, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	ASSERT (m_pOwner != NULL);

	if (!m_pOwner->IsCustomizeMode () ||
		!pDataObject->IsDataAvailable (CBCGToolbarButton::m_cFormat))
	{
		return DROPEFFECT_NONE;
	}

	return m_pOwner -> OnDragOver(pDataObject, dwKeyState, point);
}

DROPEFFECT CBCGToolbarDropTarget::OnDropEx(CWnd* /*pWnd*/, 
							COleDataObject* pDataObject, 
							DROPEFFECT dropEffect, 
							DROPEFFECT /*dropList*/, CPoint point) 
{
	ASSERT (m_pOwner != NULL);

	if (!m_pOwner->IsCustomizeMode () ||
		!pDataObject->IsDataAvailable (CBCGToolbarButton::m_cFormat))
	{
		return DROPEFFECT_NONE;
	}

	return m_pOwner -> OnDrop(pDataObject, dropEffect, point) ?
			dropEffect : DROPEFFECT_NONE;
}
