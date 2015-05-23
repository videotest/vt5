#include "stdafx.h"
#include "axbaseview.h"
#include "resource.h"

#include "desktop_frame.h"


IMPLEMENT_DYNCREATE(CDesktopFrame, CAxBaseFrame);


CDesktopFrame::CDesktopFrame()
{
	m_bDragDropFrame = false;
	m_bSelectFrameMode = false;
	m_rectSelect = NORECT;

	m_bUseDesktopDC = true;
}


CRect CDesktopFrame::ConvertToDeskTop( CRect rc, bool bCorrectOffset )
{
	if( !m_pView )
		return rc;

	if( bCorrectOffset )
		rc += ((CAxBaseView*)m_pView)->Offset();
	
	m_pView->ClientToScreen( &rc );

	return rc;
}