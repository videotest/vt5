#ifndef __ctrl_hlink_h__
#define __ctrl_hlink_h__

#include "win_base.h"
////
class hlink_button : public win_impl
{
public:
	hlink_button()
	{
		m_hcur = ::LoadCursor( module::hinst(), MAKEINTRESOURCE(IDC_CURSOR_HAND) );
	}
	~hlink_button()
	{
		::DestroyCursor( m_hcur );
	}
	virtual long on_erasebkgnd( HDC hdc )
	{
		_rect	rect;
		::GetClientRect( handle(), &rect );
		::FillRect( hdc, &rect, ::GetSysColorBrush( COLOR_3DFACE ) );
		return 1;
	}

	virtual long on_paint()
	{
		PAINTSTRUCT	paint;
		HDC	hdc = ::BeginPaint( handle(), &paint );


		HFONT	hfont = (HFONT)::SendMessage( GetParent(handle()), WM_GETFONT, 0, 0 );

		LOGFONT	lf;
		::GetObject( hfont, sizeof( lf), &lf );
		lf.lfUnderline = true;
		HFONT hfont_u = ::CreateFontIndirect( &lf );

		::SelectObject( hdc, hfont_u );
		::SetBkMode( hdc, TRANSPARENT );

		_rect	rect;
		::GetClientRect( handle(), &rect );

		_char	sz[100];
		::GetWindowText( handle(), sz, sizeof( sz ) );
		
		::SetTextColor( hdc, RGB( 0, 0, 192) );
		::InflateRect( &rect, -2, 0 );
			::DrawText( hdc, sz, _tcslen( sz ), &rect, DT_SINGLELINE );

		if( ::GetFocus() == handle() )
		{
			_rect	rect1 = rect;
			::DrawText( hdc, sz, _tcslen( sz ), &rect1, DT_CALCRECT|DT_SINGLELINE );
			rect.right = rect.left+rect1.width();
			::DrawFocusRect( hdc, &rect );
		}

		::EndPaint( handle(), &paint );
		
		::DeleteObject( hfont_u );
		return 1;
	}
	virtual long on_lbuttondblclk( const _point &point )
	{
		return 1;
	}
	virtual long on_lbuttondown( const _point &point )
	{
		::SetFocus( handle() );
		return 1;
	}
	virtual long on_lbuttonup( const _point &point )
	{
		::SendMessage( ::GetParent( handle() ), WM_COMMAND, ::GetDlgCtrlID( handle() ), 0 );
		return 1;
	}
	virtual long on_setcursor( unsigned code, unsigned hit )				
	{
		::SetCursor( m_hcur );
		return 1;
	}

	virtual long on_setfocus( HWND )
	{
		::InvalidateRect( handle(), 0, 1 );
		return 0;
	}
	virtual long on_killfocus( HWND  )
	{
		::InvalidateRect( handle(), 0, 1 );
		return 0;	
	}

	virtual long on_char( long nVirtKey )
	{
		if( nVirtKey == VK_SPACE )
			::SendMessage( ::GetParent( handle() ), WM_COMMAND, ::GetDlgCtrlID( handle() ), 0 );
		return 1;
	}
protected:
	HCURSOR	m_hcur;
};

#endif  //__ctrl_hlink_h__