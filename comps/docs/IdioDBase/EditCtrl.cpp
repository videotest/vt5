// EditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EditCtrl.h"
#include "galleryviewbase.h"
/*
namespace GallerySpace
{
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
		PostMessage(WM_CLOSE, 0, 0);
	}


	void CEditCtrl::PostNcDestroy()
	{
		delete this;	
	}

	void CEditCtrl::SetCtrlFont( CFont* pFont )
	{
		LOGFONT lf;

		if( pFont )
		{
			pFont->GetLogFont( &lf );
			m_Font.CreateFontIndirect( &lf );
			SetFont( &m_Font );
		}
	}



	LRESULT CEditCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{

		if( message == WM_KEYDOWN )
		{		
			if(wParam == VK_RETURN)
			{
				if( !GetAsyncKeyState(VK_CONTROL) )
				{
					CWnd* pParent = GetParent();
					GallerySpace::CGalleryViewBase* pView = static_cast<GallerySpace::CGalleryViewBase *>( pParent );
					if( pView )
					{
						CString str;
						GetWindowText( str );
						pView->OnEditCtrlChange( str );
					}		

					SendMessage(WM_CLOSE, 0, 0);
					return 0;
				}
			}


			if(wParam == VK_ESCAPE)
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
			PostMessage(WM_CLOSE, 0, 0);
			m_bEscapeKey = TRUE;
		}

		return CEdit::WindowProc(message, wParam, lParam);
	}
}
*/