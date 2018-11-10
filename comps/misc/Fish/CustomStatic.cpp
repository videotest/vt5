// CustomStatic.cpp : implementation file
//

#include "stdafx.h"
#include "Fish.h"
#include "CustomStatic.h"
#include "FishSettings.h"


// CCustomStatic

IMPLEMENT_DYNAMIC(CCustomStatic, CStatic)
CCustomStatic::CCustomStatic()
{
	m_bPushed = 0;
	m_bOver = 0;
}

CCustomStatic::~CCustomStatic()
{
}

BEGIN_MESSAGE_MAP(CCustomStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()
// CCustomStatic message handlers
bool CCustomStatic::SetState(bool bState, int nType )
{
	if( nType == 0 )
	{
		bool bCh = ( m_bPushed != bState );
		m_bPushed = bState;
		return bCh;
	}
	else if( nType == 1 )
	{
		bool bCh = ( m_bOver != bState );
		m_bOver = bState;
		return bCh;
	}
	return false;
}

void CCustomStatic::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(rect);

	CWnd* pThis = GetParent();
	CWnd* pChild = pThis->GetWindow(GW_CHILD);

	while (pChild && ::IsWindow(pChild->GetSafeHwnd()))
	{

		CRect ChildRect;
		if ( pChild != (CWnd*)this && ::IsWindowVisible(pChild->GetSafeHwnd()))
		{
			pChild->GetWindowRect(ChildRect);
			ScreenToClient(ChildRect);
			if (!ChildRect.IsRectEmpty())
				dc.ExcludeClipRect(ChildRect);
		}

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
	
	if( m_bPushed )
	{
//		COLORREF colorA = ::GetSysColor(g_3DLightColor); // ÷вет не тот
//		COLORREF colorB = ::GetSysColor(g_3DDKShadowColor);
		CBrush brush( g_ActiveColor ); 
		dc.FillRect(rect, &brush);

		dc.Draw3dRect( &rect, g_3DDKShadowColor, g_3DHilightColor);
		if( m_bOver )
		{
			CRect rect1(rect);
			rect1.DeflateRect(1,1);
			dc.Draw3dRect( &rect1, g_3DHilightColor, g_3DDKShadowColor);
		}
	}
	else
	{
		CBrush brush(g_BackgroundColor);
		dc.FillRect(rect, &brush);

		if( m_bOver )
			dc.Draw3dRect( &rect, g_3DHilightColor, g_3DDKShadowColor);
	}
}
