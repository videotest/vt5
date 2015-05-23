#ifndef __win_caption_h__
#define __win_caption_h__

#include "..\awin\win_base.h"
#include "..\awin\misc_string.h"

class win_caption_impl : public win_impl
{
public:
	win_caption_impl()
	{
		m_font = 0;	
		m_font_bold = 0;

		m_caption = "[ids]caption";
		m_subcaption = "[ids]subcaption";
	}
	virtual void handle_init()
	{
		handle_font( HFONT (SendMessage( handle(), WM_GETFONT, 0, 0) ) );
	}

	void handle_font( HFONT hfont )
	{
		m_font = hfont;
		if( !m_font )m_font = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );

		LOGFONT	lf;
		ZeroMemory( &lf, sizeof( lf ) );

		::GetObject( m_font, sizeof( lf ), &lf );

		lf.lfWeight = FW_BOLD;

		if( m_font_bold )::DeleteObject( m_font_bold );
		m_font_bold = ::CreateFontIndirect( &lf );
	}

	virtual void layout()
	{
		if( !handle() )return;

		_rect	rect;
		GetClientRect( handle(), &rect );

		m_rect_caption = rect;
		m_rect_caption.bottom = m_rect_caption.top+40;
	}

	virtual long on_size( short cx, short cy, ulong fSizeType )
	{
		layout();

		return win_impl::on_size( cx, cy, fSizeType );
	}

	virtual long on_paint()
	{
		PAINTSTRUCT	ps;
		HDC	hdc = ::BeginPaint( handle(), &ps );

		draw_caption( hdc );
		
		::EndPaint( handle(), &ps );

		return 1;
	}
protected:
	virtual void draw_caption( HDC hdc )
	{
		::SetBkMode( hdc, TRANSPARENT );
		::FillRect( hdc, &m_rect_caption, ::GetSysColorBrush( COLOR_INACTIVECAPTION ) );
		::SetTextColor( hdc, ::GetSysColor( COLOR_INACTIVECAPTIONTEXT ) );

		_rect	
		rect = m_rect_caption;
		rect.left += 16;
		rect.bottom = rect.vcenter();
		::SelectObject( hdc, m_font_bold );
		::DrawText( hdc, m_caption, -1, &rect, DT_SINGLELINE|DT_VCENTER );

		rect = m_rect_caption;
		rect.left += 16;
		rect.top = rect.vcenter();
		::SelectObject( hdc, m_font );
		::DrawText( hdc, m_subcaption, -1, &rect, DT_SINGLELINE|DT_VCENTER );
	}

protected:
	_rect					m_rect_caption;
	HFONT					m_font, m_font_bold;

	_string					m_caption, m_subcaption;
};

#endif //__win_caption_h__