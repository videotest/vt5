// EditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ChromosEx.h"
#include "EditCtrl.h"
#include "CarioView.h"


/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CEditCtrl)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nResult = CEdit::OnCreate( lpCreateStruct );		
	SetFont( CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) ) );

	m_x = lpCreateStruct->x;
	m_y = lpCreateStruct->y;

	return nResult;
	
}

void CEditCtrl::OnKillfocus()
{
	if( !m_bEscapeKey )
		SendMessage( WM_KEYDOWN, VK_RETURN, 0);
}


void CEditCtrl::PostNcDestroy()
{
	delete this;	
}

void CEditCtrl::SetCtrlFont( CFont* pFont )
{
	LOGFONT lf = {0};

	if( pFont )
	{
		pFont->GetLogFont( &lf );
		m_Font.CreateFontIndirect( &lf );
		SetFont( &m_Font );
	}
}



LRESULT CEditCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
 	if( message == WM_KEYUP )
		if( wParam == VK_CONTROL )
			m_bCtrlPressed = false;

 	if( message == WM_KEYDOWN )
	{		
		if( wParam == VK_CONTROL )
			m_bCtrlPressed = true;
		if( wParam == VK_RETURN )
		{
			if( !m_bCtrlPressed )
			{
				CWnd* pParent = GetParent();
				CCarioView* pView = (CCarioView *)pParent;
				if( pView )
				{
					CString str;
					GetWindowText( str );
					pView->OnEditCtrlChange( str );
				}
				
				if( !m_bEscapeKey )
					SendMessage(WM_CLOSE, 0, 0);
				return 0;
			}
		}


		if( wParam == VK_ESCAPE)
		{
			PostMessage(WM_CLOSE, 0, 0);
			return m_bEscapeKey = TRUE;
		}		
	}

	
	return CEdit::DefWindowProc( message,  wParam, lParam);

}
LRESULT CEditCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == UINT( WM_USER + 1234 ) )
	{
		m_bEscapeKey = true;
		SendMessage( WM_KEYDOWN, VK_RETURN, 0);
		PostMessage(WM_CLOSE, 0, 0);
	}

	return CEdit::WindowProc(message, wParam, lParam);
}
