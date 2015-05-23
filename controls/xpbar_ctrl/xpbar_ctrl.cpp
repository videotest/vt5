#include "stdafx.h"
#include "xpbar_ctrl.h"
#include "resource.h"
#include "\vt5\awin\misc_dbg.h"

#pragma comment(lib, "gdiplus.lib")
#define CACHED_PAINT


#define XPS_ENABLED			0x00001
#define XPS_VERTICAL		0x00002

#define MODE_NOTHING				0
#define MODE_PRESSED_TOP			1
#define MODE_PRESSED_BOTTOM			2
#define MODE_PRESSED_SCROLL_UP		3
#define MODE_PRESSED_SCROLL_DOWN	4
#define MODE_TRACK					5

#define	HILIGHT_NOTHING				0
#define HILIGHT_UP					1
#define HILIGHT_DOWN				2
#define HILIGHT_BTN					3

class xp_scroll : public win_impl
{
public:
	xp_scroll();
	~xp_scroll();
	virtual LRESULT handle_message( UINT m, WPARAM w, LPARAM l );
protected:

	virtual void handle_init();
	virtual LRESULT on_size(short cx, short cy, ulong fSizeType);
	virtual LRESULT on_paint();
	virtual LRESULT on_erasebkgnd(HDC);
	virtual long on_setscrollinfo( SCROLLINFO *pinfo, bool fRedraw );
	virtual long on_getscrollinfo( SCROLLINFO *pinfo );
	virtual long on_setrange( long min, long max );
	virtual long on_getrange( long *pmin, long *pmax );
	virtual long on_enable( bool fEnable );
	virtual long on_enable_scrollbar( long fDisable );

	virtual LRESULT on_lbuttondown(const _point &point);
	virtual LRESULT on_lbuttonup(const _point &point);
	virtual LRESULT on_mousemove(const _point &point);
	virtual LRESULT on_timer(ulong event);
	virtual LRESULT on_ncdestroy();
protected:
	void _draw_scroll_pane( HDC hdc, const _rect	&rect, unsigned flags, int item );
	void _layout();
	bool _is_vertical()		{	return (m_flags & XPS_VERTICAL)==XPS_VERTICAL;}
	void _check_hilight( const _point &point );

	long		m_min, m_max, m_pos, m_page, m_track_pos;
	unsigned	m_flags;
	HWND		m_hparent;

	_rect	m_rect_up;
	_rect	m_rect_down;
	_rect	m_rect_work;
	_rect	m_rect_btn;

	_rect	m_rect_hilight;

	static int scroll_msg[2];

	HBRUSH	m_hbrush_face;

	int		m_mode, m_hilight, m_track_offset;
	HIMAGELIST	m_images;

	int m_timercount; // счетчик тиков таймера при нажатой кнопке - чтобы до повтора был промежуток больше, чем между повторениями
};

int xp_scroll::scroll_msg[2] = {WM_HSCROLL, WM_VSCROLL};

LRESULT xp_scroll::on_ncdestroy()
{
	return win_impl::on_ncdestroy();
}

xp_scroll::xp_scroll()
{
	m_flags = XPS_ENABLED;
	m_hparent = 0;
	m_mode = MODE_NOTHING;
	m_hilight = HILIGHT_NOTHING;
	m_hbrush_face = 0;
	m_track_offset = 0;
	m_images = 0;
}

xp_scroll::~xp_scroll()
{
	DeleteObject( m_hbrush_face );
	ImageList_Destroy( m_images );
}

void xp_scroll::handle_init()
{
	win_impl::handle_init();

	if( (GetWindowLong( handle(), GWL_STYLE ) & SBS_VERT ) == SBS_VERT )
		m_flags |= XPS_VERTICAL;

	if( !m_images )
	{
		m_images = ImageList_Create( 16, 16, ILC_COLOR24|ILC_MASK, 20, 5 );
		HBITMAP	hbmp = ::LoadBitmap( module::hrc(), MAKEINTRESOURCE(IDB_SCROLL) );
		ImageList_AddMasked( m_images, hbmp, RGB( 214, 228, 201 ) );
		::DeleteObject( hbmp );
	}
	m_hparent = ::GetParent( handle() );
	m_min = 0;
	m_max = 100;
	m_page = 0;
	m_pos = m_track_pos = 0;

	if( !m_hbrush_face )
		m_hbrush_face = ::CreateSolidBrush( RGB( 246, 246, 236 ) );
}

void xp_scroll::_check_hilight( const _point &point )
{
	if( m_rect_hilight.pt_in_rect( point ) )
		return;	//ничего не поменялось

	int	new_hilight = HILIGHT_NOTHING;
	_rect	rect_new;
	if( m_rect_up.pt_in_rect( point ) )
	{
		rect_new = m_rect_up;
		new_hilight = HILIGHT_UP;
	}
	else if( m_rect_down.pt_in_rect( point ) )
	{
		rect_new = m_rect_down;
		new_hilight = HILIGHT_DOWN;
	}
	else if( m_rect_btn.pt_in_rect( point ) )
	{
		rect_new = m_rect_btn;
		new_hilight = HILIGHT_BTN;
	}

	if( m_hilight )
	{
		if( !new_hilight )
			KillTimer( handle(), 124 ); 
		InvalidateRect( handle(), 0, 0 );
	}
	m_rect_hilight = rect_new;
	if( new_hilight )
	{
		if( !m_hilight )
			SetTimer( handle(), 124, 50, 0 );
		m_hilight =new_hilight;
		InvalidateRect( handle(), &m_rect_hilight, 0 );
	}
	else
		m_hilight = HILIGHT_NOTHING;
}

void xp_scroll::_layout()
{
	_rect	rect;
	::GetClientRect( handle(), &rect );

	if( _is_vertical() )
	{
		int	cy = min( ::GetSystemMetrics( SM_CYVSCROLL ), rect.height()/2-2 );

		int	ncy = min( cy, rect.height()/3 );
		
		m_rect_up = rect;
		m_rect_up.bottom = m_rect_up.top + ncy;
		m_rect_down = rect;
		m_rect_down.top = m_rect_down.bottom - ncy;
		m_rect_work = rect;
		m_rect_work.top = m_rect_up.bottom;
		m_rect_work.bottom = m_rect_down.top;

		int	div1 = max( m_max-m_min+m_page, 1 );
		int	div2 = max( m_max-m_min, 1 );

		int	h2 = max( m_page ? (m_rect_work.height()*m_page/(div1))/2:ncy, ncy/2 );
		int pos = (m_rect_work.height()-h2*2)*(m_track_pos+m_min)/(div2)+m_rect_up.bottom+h2;
		m_rect_btn = rect;
		m_rect_btn.top = pos-h2;
		m_rect_btn.bottom = pos+h2;
	}
	else
	{
		int	 cx = min( ::GetSystemMetrics( SM_CXHSCROLL ), rect.width()/2-2 );
		int	ncx = min( cx, rect.width()/3 );

		m_rect_up = rect;
		m_rect_up.right = m_rect_up.left + ncx;
		m_rect_down = rect;
		m_rect_down.left = m_rect_down.right - ncx;
		m_rect_work = rect;
		m_rect_work.left = m_rect_up.right;
		m_rect_work.right = m_rect_down.left;

		int	div1 = max( m_max-m_min+m_page, 1 );
		int	div2 = max( m_max-m_min, 1 );

		int	h2 = max( m_page ? (m_rect_work.width()*m_page/(div1))/2:ncx, ncx/2 );
		int pos = (m_rect_work.width()-h2*2)*(m_track_pos+m_min)/(div2)+m_rect_up.right+h2;
		m_rect_btn = rect;
		m_rect_btn.left = pos-h2;
		m_rect_btn.right = pos+h2;
	}
}

LRESULT xp_scroll::on_size(short cx, short cy, ulong fSizeType)
{
	win_impl::on_size( cx, cy, fSizeType );
	_layout();
	InvalidateRect( handle(), 0, 0 );
	UpdateWindow( handle() );
	return true;
}

LRESULT xp_scroll::on_timer(ulong event)
{
	if( event == 123 )
	{
		m_timercount++;
		if(m_timercount>1 && m_timercount<4) return __super::on_timer( event );

		if( m_mode == MODE_PRESSED_TOP )
			SendMessage( m_hparent, scroll_msg[_is_vertical()], SB_LINELEFT, (LPARAM)handle() );
		else if( m_mode == MODE_PRESSED_BOTTOM )
			SendMessage( m_hparent, scroll_msg[_is_vertical()], SB_LINERIGHT, (LPARAM)handle() );
		else if( m_mode == MODE_PRESSED_SCROLL_UP || m_mode == MODE_PRESSED_SCROLL_DOWN )
		{
			_rect	rect1, rect2;

			if( _is_vertical() )
			{	
				rect1 = m_rect_work;
				rect1.top = m_rect_up.bottom;
				rect1.bottom = m_rect_btn.top;
				rect2 = m_rect_work;
				rect2.top = m_rect_btn.bottom;
				rect2.bottom = m_rect_down.top;
			}
			else
			{	
				rect1 = m_rect_work;
				rect1.left = m_rect_up.right;
				rect1.right = m_rect_btn.left;
				rect2 = m_rect_work;
				rect2.left = m_rect_btn.right;
				rect2.right = m_rect_down.left;
			}

			_point	point;
			::GetCursorPos( &point );
			::ScreenToClient( handle(), &point );

			if( m_mode == MODE_PRESSED_SCROLL_UP && rect1.pt_in_rect( point ) )
				::SendMessage( m_hparent, scroll_msg[_is_vertical()], SB_PAGELEFT, (LPARAM)handle() );
			if( m_mode == MODE_PRESSED_SCROLL_DOWN && rect2.pt_in_rect( point ) )
				::SendMessage( m_hparent, scroll_msg[_is_vertical()], SB_PAGERIGHT, (LPARAM)handle() );
		}
	}
	else if( event == 124 )
	{
		_point	point;
		GetCursorPos( &point );
		ScreenToClient( handle(), &point );
		_check_hilight( point );
	}
	return win_impl::on_timer( event );
}

#define DSF_ENABLED	0x00001
#define DSF_PRESSED 0x00002
#define DSF_HILIGHT	0x00004

#define DSI_TOP	0
#define DSI_BOTTOM	1
#define DSI_BUTTON	2

COLORREF	cr_back = RGB( 214, 231, 206 );
COLORREF	cr_back_hi = RGB( 184, 205, 158 );
COLORREF	cr_frame1 = RGB( 214, 228, 201 );
COLORREF	cr_frame2 = RGB( 78, 113, 66 );
COLORREF	cr_frame3 = RGB( 176, 192, 154 );

void xp_scroll::_draw_scroll_pane( HDC hdc, const _rect	&rect, unsigned flags, int item )
{
	int	image;

	if( item == DSI_TOP )image = _is_vertical()?0:8;
	else if( item == DSI_BOTTOM )image = _is_vertical()?4:12;
	else if( item == DSI_BUTTON )image = _is_vertical()?16:20;

	int	draw_mode = 0;
	if( flags &  DSF_HILIGHT )
		draw_mode = 0;
	else if( (flags & DSF_ENABLED)==DSF_ENABLED && (flags & DSF_PRESSED) == 0 )
		draw_mode = 1;
	else if( (flags & DSF_ENABLED)==DSF_ENABLED && (flags & DSF_PRESSED) == DSF_PRESSED )
		draw_mode = 2;
	else 
		draw_mode = 3;

	image+=draw_mode;


	_rect	rect_fill = rect;
	rect_fill.left++;
	rect_fill.right--;
	rect_fill.top++;
	rect_fill.bottom--;
	
	HPEN	hpen_old = (HPEN)::SelectObject( hdc, GetStockObject( NULL_PEN ) );

	HBRUSH	hbrush = ::CreateSolidBrush( ((draw_mode == 0)?RGB(153, 153, 102 ):RGB(153, 204, 153) ) );
	FillRect( hdc, &rect_fill, hbrush );
	::DeleteObject( hbrush );

	if( draw_mode == 0 )
	{
		HPEN	hpen1 = ::CreatePen( PS_SOLID, 0, RGB(255, 255, 255) );
		HPEN	hpen2 = ::CreatePen( PS_SOLID, 0, RGB(102, 153, 102) );
		HPEN	hpen3 = ::CreatePen( PS_SOLID, 0, RGB(153, 204, 153) );
		HPEN	hpen4 = ::CreatePen( PS_SOLID, 0, RGB(51, 102, 51) );

		::SelectObject( hdc, hpen1 );
		::MoveToEx( hdc, rect.left, rect.bottom-1, 0 );
		::LineTo( hdc, rect.left, rect.top );
		::LineTo( hdc, rect.right-1, rect.top );
		
		::SelectObject( hdc, hpen2 );
		::MoveToEx( hdc, rect.left+2, rect.top+1, 0 );
		::LineTo( hdc, rect.right-2, rect.top+1 );
		::LineTo( hdc, rect.right-2, rect.bottom-3 );
		::LineTo( hdc, rect.left+1, rect.bottom-3 );
		::LineTo( hdc, rect.left+1, rect.top+2 );
		::MoveToEx( hdc, rect.left+1, rect.bottom-2, 0 );
		::LineTo( hdc, rect.right-3, rect.bottom-2 );
		::LineTo( hdc, rect.right-3, rect.top+1 );

		::SelectObject( hdc, hpen3 );
		::MoveToEx( hdc, rect.right-4, rect.top+2, 0 );
		::LineTo( hdc, rect.left+2, rect.top+2 );
		::LineTo( hdc, rect.left+1, rect.top+1 );
		::LineTo( hdc, rect.left+2, rect.top+2 );
		::LineTo( hdc, rect.left+2, rect.bottom-4 );

		::SelectObject( hdc, hpen4 );
		::MoveToEx( hdc, rect.left+1, rect.bottom-1, 0 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-2, rect.bottom-2 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-1, rect.top+1 );

		
		::SelectObject( hdc, hpen_old );
		::DeleteObject( hpen1 );
		::DeleteObject( hpen2 );
		::DeleteObject( hpen3 );
		::DeleteObject( hpen4 );
	}
	else if( draw_mode == 1 )
	{
		HPEN	hpen1 = ::CreatePen( PS_SOLID, 0, RGB(255, 255, 255) );
		HPEN	hpen2 = ::CreatePen( PS_SOLID, 0, RGB(102, 153, 102) );
		HPEN	hpen4 = ::CreatePen( PS_SOLID, 0, RGB(51, 102, 51) );

		::SelectObject( hdc, hpen1 );
		::MoveToEx( hdc, rect.left, rect.bottom-1, 0 );
		::LineTo( hdc, rect.left, rect.top );
		::LineTo( hdc, rect.right-1, rect.top );
		
		::SelectObject( hdc, hpen2 );
		::MoveToEx( hdc, rect.left+2, rect.top+1, 0 );
		::LineTo( hdc, rect.right-2, rect.top+1 );
		::LineTo( hdc, rect.right-2, rect.bottom-3 );
		::MoveToEx( hdc, rect.right-3, rect.bottom-2, 0 );
		::LineTo( hdc, rect.left+1, rect.bottom-2 );
		::LineTo( hdc, rect.left+1, rect.top+2 );

		::SelectObject( hdc, hpen4 );
		::MoveToEx( hdc, rect.left+1, rect.bottom-1, 0 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-2, rect.bottom-2 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-1, rect.top+1 );

		
		::SelectObject( hdc, hpen_old );
		::DeleteObject( hpen1 );
		::DeleteObject( hpen2 );
		::DeleteObject( hpen4 );
	}
	else if( draw_mode == 2 )
	{
		HPEN	hpen4 = ::CreatePen( PS_SOLID, 0, RGB(255, 255, 255) );
		HPEN	hpen2 = ::CreatePen( PS_SOLID, 0, RGB(102, 153, 102) );
		HPEN	hpen1 = ::CreatePen( PS_SOLID, 0, RGB(51, 102, 51) );

		::SelectObject( hdc, hpen1 );
		::MoveToEx( hdc, rect.left, rect.bottom-1, 0 );
		::LineTo( hdc, rect.left, rect.top );
		::LineTo( hdc, rect.right-1, rect.top );
		
		::SelectObject( hdc, hpen2 );
		::MoveToEx( hdc, rect.left+2, rect.top+1, 0 );
		::LineTo( hdc, rect.right-2, rect.top+1 );
		::LineTo( hdc, rect.right-2, rect.bottom-3 );
		::MoveToEx( hdc, rect.right-3, rect.bottom-2, 0 );
		::LineTo( hdc, rect.left+1, rect.bottom-2 );
		::LineTo( hdc, rect.left+1, rect.top+2 );

		::SelectObject( hdc, hpen4 );
		::MoveToEx( hdc, rect.left+1, rect.bottom-1, 0 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-2, rect.bottom-2 );
		::LineTo( hdc, rect.right-1, rect.bottom-1 );
		::LineTo( hdc, rect.right-1, rect.top+1 );

		
		::SelectObject( hdc, hpen_old );
		::DeleteObject( hpen1 );
		::DeleteObject( hpen2 );
		::DeleteObject( hpen4 );
	}
	else if( draw_mode == 3 )
	{
		HPEN	hpen1 = ::CreatePen( PS_SOLID, 0, RGB(255, 255, 255) );

		::SelectObject( hdc, hpen1 );
		::MoveToEx( hdc, rect.left, rect.bottom-1, 0 );
		::LineTo( hdc, rect.left, rect.top );
		::LineTo( hdc, rect.right-1, rect.top );
		
		
		::SelectObject( hdc, hpen_old );
		::DeleteObject( hpen1 );
	}

	_point	point = _point((rect.left + rect.right-16)>>1, ((rect.top + rect.bottom-16)>>1)+1);
	ImageList_Draw( m_images, image, hdc, point.x, point.y, ILD_TRANSPARENT ); 
}

LRESULT xp_scroll::on_paint()
{
	PAINTSTRUCT	ps;
	HDC hdcPaint = BeginPaint( handle(), &ps );

	_rect	rect_paint( ps.rcPaint );

	BITMAPINFOHEADER	bmi;
	ZeroMemory( &bmi, sizeof( bmi ) );
	bmi.biBitCount = 24;
	bmi.biSize = sizeof( bmi );
	bmi.biPlanes = 1;
	bmi.biWidth = rect_paint.width();
	bmi.biHeight = rect_paint.height();

	byte	*pbits = 0;
	HBITMAP	hbmp = ::CreateDIBSection( hdcPaint, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, (void**)&pbits, 0, 0 );
	HDC	hdc = ::CreateCompatibleDC( hdcPaint );

	//1. copy source
	::SelectObject( hdc, hbmp );
	::SetViewportOrgEx( hdc, -rect_paint.left, -rect_paint.top, 0 );

	HBRUSH	hbrush = (HBRUSH)::SendMessage( m_hparent, WM_CTLCOLORSCROLLBAR, (WPARAM)hdc, (LPARAM)handle() );
	if( !hbrush ) hbrush = ::GetSysColorBrush( COLOR_SCROLLBAR );

	::FillRect( hdc, &rect_paint, hbrush );

	//рисуем
	unsigned flags;
	
	flags = 0;
	if( (m_flags & XPS_ENABLED) )flags |= DSF_ENABLED;
	if( m_mode == MODE_PRESSED_TOP )flags |= DSF_PRESSED;
	else if( m_hilight == HILIGHT_UP )flags |= DSF_HILIGHT;
	_draw_scroll_pane( hdc, m_rect_up, flags, DSI_TOP );
	
	flags = 0;
	if( (m_flags & XPS_ENABLED) )flags |= DSF_ENABLED;
	if( m_mode == MODE_PRESSED_BOTTOM )flags |= DSF_PRESSED;
	else if( m_hilight == HILIGHT_DOWN )flags |= DSF_HILIGHT;
	_draw_scroll_pane( hdc, m_rect_down, flags, DSI_BOTTOM );

	if( m_flags & XPS_ENABLED )
	{
		flags = DSF_ENABLED;
		if( m_mode == MODE_TRACK )flags |= DSF_PRESSED;
		else if( m_hilight == HILIGHT_BTN )flags |= DSF_HILIGHT;
		_draw_scroll_pane( hdc, m_rect_btn, flags, DSI_BUTTON );
	}	
	_rect	rect_pressed;

	if( m_mode == MODE_PRESSED_SCROLL_UP )
	{
		if( _is_vertical() )
		{	
			rect_pressed = m_rect_work;
			rect_pressed.top = m_rect_up.bottom;
			rect_pressed.bottom = m_rect_btn.top;
		}
		else
		{	
			rect_pressed = m_rect_work;
			rect_pressed.left = m_rect_up.right;
			rect_pressed.right = m_rect_btn.left;
		}
	}
	else if( m_mode == MODE_PRESSED_SCROLL_DOWN )
	{
		if( _is_vertical() )
		{	
			rect_pressed = m_rect_work;
			rect_pressed.top = m_rect_btn.bottom;
			rect_pressed.bottom = m_rect_down.top;
		}
		else
		{	
			rect_pressed = m_rect_work;
			rect_pressed.left = m_rect_btn.right;
			rect_pressed.right = m_rect_down.left;
		}
	}

	if( rect_pressed.height() != 0 )
		::InvertRect( hdc, &rect_pressed );

	int	cy = bmi.biHeight;
	int	cx = bmi.biWidth;
	
	::SetDIBitsToDevice( hdcPaint, rect_paint.left, rect_paint.top, cx, cy, 0, 0, 0, cy, pbits, (BITMAPINFO*)&bmi, DIB_RGB_COLORS );
	
	::DeleteDC( hdc );
	::DeleteObject( hbmp );
	::EndPaint( handle(), &ps ); 

	return 0;
}
LRESULT xp_scroll::on_erasebkgnd(HDC)
{
	return 0;
}

long xp_scroll::on_enable_scrollbar( long fDisable )
{
	::EnableWindow( handle(), fDisable == 0 );
	return true;
}

long xp_scroll::on_enable( bool fEnable )
{
	if( fEnable )
		m_flags |= XPS_ENABLED;

	else
		m_flags &= ~XPS_ENABLED;
	InvalidateRect( handle(), 0, 0 );
	return true;
}

LRESULT xp_scroll::handle_message( UINT m, WPARAM w, LPARAM l )
{
	switch( m )
	{
	case SBM_SETSCROLLINFO:
		return on_setscrollinfo( (SCROLLINFO*)l, w != 0 ); 
	case SBM_GETSCROLLINFO:
		return on_getscrollinfo( (SCROLLINFO*)l ); 
	case SBM_ENABLE_ARROWS:
		return on_enable_scrollbar( w );
	case SBM_GETRANGE:
		return on_getrange( (long*)w, (long*)l );
	case SBM_SETRANGE:
		return on_setrange( w, l );
	case WM_ENABLE:
		return on_enable( w != 0 );
	default:
		return win_impl::handle_message( m, w, l );

	}
}

long xp_scroll::on_getrange( long *pmin, long *pmax )
{
	*pmin = m_min;
	*pmax = m_max+m_page;
	return true;
}

long xp_scroll::on_setrange( long min, long max )
{
	m_min = min;
	m_max = max-m_page;
	return true;
}

long xp_scroll::on_setscrollinfo( SCROLLINFO *pinfo, bool fRedraw )
{
	if( pinfo->fMask & SIF_PAGE )
	{
		m_page = pinfo->nPage;
	}
	if( pinfo->fMask & SIF_RANGE )
	{
		m_min = min( pinfo->nMin, max( pinfo->nMin, pinfo->nMax-m_page ) );
		m_max = max( pinfo->nMin, max( pinfo->nMin, pinfo->nMax-m_page ) );
		m_pos = max( m_min, min( m_max, m_pos ) );
		m_track_pos = max( m_min, min( m_max, m_track_pos ) );
	}
	if( pinfo->fMask & SIF_POS )
	{
		m_track_pos = m_pos = max( m_min, min( m_max, pinfo->nPos ) );
	}
	if( pinfo->fMask & SIF_DISABLENOSCROLL )
	{
		EnableWindow( handle(), m_max-m_min > 0 );
	}
/*	if( pinfo->fMask & SIF_TRACKPOS )
	{
		m_track_pos = max( m_min, min( m_max, pinfo->nTrackPos ) );
	}*/

	_layout();
	_point	point;
	GetCursorPos( &point );
	ScreenToClient( handle(), &point );
	_check_hilight( point );
	::InvalidateRect( handle(), 0, 0 );
	if( fRedraw )
		::UpdateWindow( handle() );

	return true;
}

long xp_scroll::on_getscrollinfo( SCROLLINFO *pinfo )
{
	if( pinfo->fMask & SIF_RANGE )
	{
		pinfo->nMin = m_min;
		pinfo->nMax = m_max+m_page;
	}
	if( pinfo->fMask & SIF_PAGE )
	{
		pinfo->nPage = m_page;
	}
	if( pinfo->fMask & SIF_POS )
	{
		pinfo->nPos = m_pos;
	}
	if( pinfo->fMask & SIF_TRACKPOS )
	{
		pinfo->nTrackPos = m_track_pos;
	}

	return true;
}

LRESULT xp_scroll::on_lbuttondown(const _point &point)
{
	if( m_rect_up.pt_in_rect( point ) )
	{
		m_mode = MODE_PRESSED_TOP;
		InvalidateRect( handle(), &m_rect_up, false );
	}
	else if( m_rect_down.pt_in_rect( point ) )
	{
		m_mode = MODE_PRESSED_BOTTOM;
		InvalidateRect( handle(), &m_rect_down, false );
	}
	else if( m_rect_btn.pt_in_rect( point ) )
	{
		m_mode = MODE_TRACK;
		if( _is_vertical() )m_track_offset = point.y - m_rect_btn.top;
		else m_track_offset = point.x - m_rect_btn.left;
		InvalidateRect( handle(), &m_rect_btn, false );
	}
	else if( m_rect_work.pt_in_rect( point ) )
	{
		_rect	rect1, rect2;

		if( _is_vertical() )
		{	
			rect1 = m_rect_work;
			rect1.top = m_rect_up.bottom;
			rect1.bottom = m_rect_btn.top;
			rect2 = m_rect_work;
			rect2.top = m_rect_btn.bottom;
			rect2.bottom = m_rect_down.top;
		}
		else
		{	
			rect1 = m_rect_work;
			rect1.left = m_rect_up.right;
			rect1.right = m_rect_btn.left;
			rect2 = m_rect_work;
			rect2.left = m_rect_btn.right;
			rect2.right = m_rect_down.left;
		}

		if( rect1.pt_in_rect( point ) )
			m_mode = MODE_PRESSED_SCROLL_UP;
		else if( rect2.pt_in_rect( point ) )
			m_mode = MODE_PRESSED_SCROLL_DOWN;
	}
	if( m_mode != MODE_NOTHING )
	{
		SetCapture( handle() );
		m_timercount=0;
		SetTimer( handle(), 123, 100, 0 );
		on_timer(123); // сразу обработать 1 нажатие
	}
	return true;
}

LRESULT xp_scroll::on_lbuttonup(const _point &point)
{
	if( m_mode != MODE_NOTHING )
	{
		InvalidateRect( handle(), 0, 0 );
		KillTimer( handle(), 123 );
		::ReleaseCapture();
		m_mode = MODE_NOTHING;
	}
	return true;
}

LRESULT xp_scroll::on_mousemove(const _point &point)
{
	if( m_mode == MODE_TRACK )
	{
		if( _is_vertical() )
		{
			int	div1 = max( m_rect_work.height()-m_rect_btn.height(), 1 );
			m_track_pos = (point.y-m_track_offset-m_rect_work.top)*(m_max-m_min)/div1+m_min;
		}
		else 		
		{
			int	div1 = max( m_rect_work.width()-m_rect_btn.width(), 1 );
			m_track_pos = (point.x-m_track_offset-m_rect_work.bottom)*(m_max-m_min)/div1+m_min;
		}
		m_track_pos = max( m_min, min( m_max, m_track_pos ) );
		_layout();
		InvalidateRect( handle(), 0, 0 );
		SendMessage( m_hparent, scroll_msg[_is_vertical()], (m_track_pos<<16)|SB_THUMBPOSITION, (LPARAM)handle() );
		SendMessage( m_hparent, scroll_msg[_is_vertical()], (m_track_pos<<16)|SB_THUMBTRACK, (LPARAM)handle() );
		UpdateWindow( handle() );
	}
	if( m_mode == MODE_NOTHING )
	{
		_check_hilight( point );
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
xp_pane::xp_pane( xpbar_ctrl *p )
{
	pctrl = p;
	hilighted = false;
	title = 0;
	image = -1;
	caption_height = pctrl->pane_title_height;
	height = caption_height+100;
	param = 0;
	state = xpps_Collapse;
	progress = 0;
	color = 0;
	caption_offset = 0;
	images_caption = 0;	
}

xp_pane::~xp_pane()
{
	if( hilighted )
		pctrl->_clear_hilight();
	_assert( !hilighted );

	if( title )
	{
		delete title;
	}
}

void xp_pane::client_area( RECT *prect )
{
	prect->left = rect.left+6;
	prect->top = rect.top+caption_height+6;
	prect->bottom = rect.bottom-4;
	prect->right = rect.right-6;
}

void xp_pane::set_caption( XP_CAPTION_BUTTON *pbtn, bool bCreateNew )
{
	caption_button	*pfound = 0;
	for (TPOS lpos = buttons.head(); lpos; lpos = buttons.next(lpos))
	{
		caption_button *p = buttons.get( lpos );
		if( p->command == pbtn->nCmd )
		{
			pfound = p;
			break;
		}
	}
	if( !pfound )
	{
		if( !bCreateNew )return;
		pfound = new caption_button;
		buttons.insert_before( pfound, 0 );
		pfound->image = -1;
		pfound->state = XPBS_ENABLED;
	}

	pfound->command = pbtn->nCmd;
	if( pbtn->mask & XPCBM_IMAGE )
		pfound->image = pbtn->iImage;
	if( pbtn->mask & XPCBM_STATE )
		pfound->state = pbtn->dwState;

	_redraw_caption( true );
}

void xp_pane::click( const _point &point )
{
	for (TPOS lpos = buttons.head(); lpos; lpos = buttons.next(lpos))
	{
		caption_button *p = buttons.get( lpos );
		if( (p->state & XPBS_ENABLED )&&p->rect.pt_in_rect( point ) )
		{
			_redraw_caption();
			UpdateWindow( pctrl->handle() );
			::SendMessage( GetParent( pctrl->handle( )), WM_COMMAND, p->command, 0 );

			if( get_style() == XPPS_WINDOW )
			{
				xp_window_pane	*pw = (xp_window_pane*) this;
				::SendMessage( pw->hwnd, WM_COMMAND, p->command, 0 );
			}
			else
				::SendMessage( GetParent( pctrl->handle( )), WM_COMMAND, p->command, 0 );

			_redraw_caption();
		}
	}
}

void xp_pane::remove_caption( unsigned id )
{
	for (TPOS lpos = buttons.head(); lpos; lpos = buttons.next(lpos))
	{
		caption_button *p = buttons.get( lpos );
		if( p->command == id )
		{
			buttons.remove( lpos );
			delete p;
			break;
		}
	}
	_redraw_caption();
}

void xp_pane::draw( Graphics &graphics, _rect &client )
{
	_rect	rect_button;
	rect_button.left = rect.right-3;
	rect_button.top = rect.top+caption_offset+2;
	rect_button.bottom = rect_button.top+16;

	for (TPOS lpos = buttons.tail(); lpos; lpos = buttons.prev(lpos))
	{
		caption_button	*pbutton = buttons.get( lpos );
		rect_button.right = rect_button.left-2;
		rect_button.left = rect_button.right-16;

		pbutton->rect = rect_button;

		HDC	hdc = graphics.GetHDC();

		::DrawIconEx( hdc, rect_button.left, rect_button.top, pctrl->get_caption_icon(),
			rect_button.width(), rect_button.height(), 0, 0, DI_NORMAL );

		HICON hIcon  = ::ImageList_ExtractIcon( 0, images_caption, pbutton->image );

		UINT uiFlags = DST_ICON;

		if( !( pbutton->state & XPBS_ENABLED ) )
			uiFlags |= DSS_DISABLED;

		::DrawState( hdc, 0, 0,
			MAKELPARAM ( hIcon, 0),
			0,
			rect_button.left,
			rect_button.top,
			rect_button.width(),
			rect_button.height(),
			uiFlags );

//		::ImageList_Draw( images_caption, pbutton->image, hdc, 
//			rect_button.left, rect_button.top, (pbutton->state & XPBS_ENABLED) ? ILD_NORMAL: ILD_BLEND50 );

		::DestroyIcon( hIcon );

		graphics.ReleaseHDC( hdc );
	}
}



void xp_pane::_redraw_caption( bool fUpdate )
{
	pctrl->redraw( &_rect( rect.left, rect.top, rect.right, rect.top+caption_height ), fUpdate );
}

void xp_pane::from_item( XPPANEITEM *p )
{
	if( p->mask & XPF_TEXT )
	{
		if(	title )
		{
			delete title;
		}

		title = new char[strlen( p->pszText)+1];
		strcpy( title, p->pszText );
	}
	if( p->mask & XPF_IMAGE )
		image = p->nImage;

	if( p->mask & XPF_HEIGHT )
		height = p->nHeight+caption_height;

	if( p->mask & XPF_PARAM )
		param = p->lParam;

	if( p->mask & XPF_STYLE )
	{
		if( p->style & XPPS_HILIGHT )
			color = 1;
		else
			color = 0;
	}

	if( p->mask & XPF_IMAGELIST_CAPTION )
		images_caption = p->hImageListCaption; 
}


xp_toolbar_pane::xp_toolbar_pane( xpbar_ctrl *p ) : xp_pane( p )
{
	image_list = 0;
	height = caption_height;
}

xp_toolbar_pane::~xp_toolbar_pane()
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		if( pbutton->hilighted )
			pctrl->_clear_hilight();
	}
}


void xp_toolbar_pane::from_item( XPPANEITEM *p )
{
	xp_pane::from_item( p );

	if( p->mask & XPF_IMAGELIST )
		image_list = p->hImageList;


	if( image_list !=0 )
		::ImageList_GetIconSize( image_list, (int*)&size_button.cx, (int*)&size_button.cy ); 
	else
		size_button.cx = size_button.cy = 16;
	calc_layout( true );
}

void xp_toolbar_pane::draw( Graphics &graphics, _rect &client )
{

	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		_rect	rect_paint = pbutton->rect;

		if( rect_paint.width() == 0 )
			continue;

		bool	bChecked = (pbutton->style & XPBS_CHECKED )==XPBS_CHECKED;
		bool	bEnabled = (pbutton->style & XPBS_ENABLED )==XPBS_ENABLED;
		bool	fHilight = pbutton->hilighted && bEnabled;
		if( bChecked )
		{
			Rect	rect_fill;
			rect_fill.X = rect_paint.left+size_button.cx;
			rect_fill.Y = rect_paint.top;
			rect_fill.Width = rect_paint.width()-size_button.cx;
			rect_fill.Height= rect_paint.height();

			graphics.FillRectangle( &SolidBrush(Color(211, 221, 231)), rect_fill );
			//graphics.DrawRectangle( &Pen( &SolidBrush(Color(40, 55, 72)) ), rect_fill ); 
		}
		if( pbutton->image != -1 && image_list )
		{
			HDC	hdc = graphics.GetHDC();

			DWORD	flags = ILD_TRANSPARENT;

			if( !bEnabled )
				flags = ILD_BLEND50;
				

			IMAGELISTDRAWPARAMS	params;
			ZeroMemory( &params, sizeof( params ) );
			params.cbSize = sizeof( params );
			params.himl = image_list;
			params.hdcDst = hdc;
			params.x = rect_paint.left;
			params.y = rect_paint.top;
			params.i = pbutton->image;
			params.rgbBk = CLR_NONE;
			params.rgbFg = pctrl->color_pane.ToCOLORREF();
			params.fStyle = flags;
//			ImageList_DrawIndirect( &params );
			ImageList_DrawEx(image_list, pbutton->image, hdc, params.x, params.y, 0, 0, params.rgbBk, params.rgbFg, flags);

			graphics.ReleaseHDC( hdc );
		}


		StringFormat	format( 0 );
		format.SetLineAlignment( StringAlignmentCenter );

		graphics.DrawString( pbutton->text, 
			-1,
			&Font( _bstr_t("Tahoma"), 8, (pbutton->hilighted?FontStyleUnderline:0)|
										((pbutton->style & XPBS_CHECKED || pbutton->style & XPBS_PRESSED)?FontStyleBold:0)),
			RectF(  float( rect_paint.left+size_button.cy+5 ), 
					float( rect_paint.top ), 
					float( rect_paint.width()-size_button.cy-5 ), float( rect_paint.height() ) ),
					&format,
					&SolidBrush( bEnabled?(fHilight?
								pctrl->color_text_hilight:
								pctrl->color_text ):
								pctrl->color_disabled_text) );
		
		rect_paint.top = rect_paint.bottom+5;
		rect_paint.bottom = rect_paint.top+size_button.cy;
	}
}

void xp_toolbar_pane::set_button( XPBUTTON *p, bool bnew )
{
	xp_button *pb = 0;
	
	
	if( bnew )
	{
		pb = new xp_button;

		insert_before( pb, 0 );
		calc_layout( true );
	}
	else
	{
		for (TPOS lpos = head(); lpos; lpos = next(lpos))
		{
			xp_button *pbt = get( lpos );
			if( pbt->cmd == p->command )
			{
				pb = pbt;
				break;
			}
		}

	}

	if( pb )
	{
		bool bChanged = pb->from_button( p );
		if( bChanged )
			pctrl->redraw( &pb->rect, true );
	}
}

void xp_toolbar_pane::calc_layout( bool update_size )
{
	if( update_size )
	{
		long	old = height;
		height = count()*(size_button.cy+5)+10+caption_height;

		if( old != height && state == xpps_Expand )
			state = xpps_Expanding;
		pctrl->start_timer();
	}

	_rect	rect_paint;
	client_area( &rect_paint );

	rect_paint.bottom = rect_paint.top+size_button.cy;
	
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		
		if( rect_paint.bottom > rect.bottom )
			pbutton->rect = _rect();
		else
			pbutton->rect = rect_paint;

		rect_paint.top = rect_paint.bottom+5;
		rect_paint.bottom = rect_paint.top+size_button.cy;
	}
}

void xp_toolbar_pane::show( bool show )
{
	if( show )
		calc_layout( false );
}
void xp_toolbar_pane::move()
{
	calc_layout( false );
}

long xp_toolbar_pane::hit_test( const _point &point, XPHITTEST *phit )
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		if( pbutton->rect.pt_in_rect( point ) )
		{
			if( phit )
			{
				phit->flags = XPHT_BUTTON;
				phit->lparam = (LPARAM)lpos;
				phit->pbhilight = &pbutton->hilighted;

				if( pbutton->style & XPBS_ENABLED )
				{
					phit->rect_invalidate = pbutton->rect;
				}
				else
				{
					phit->rect_invalidate = _rect();
				}
			}
			return 1;
		}
	}

	return 0;
}

bool xp_toolbar_pane::is_button_exist(xp_button*p)
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		if (pbutton == p)
			return true;
	}
	return false;
}


void xp_toolbar_pane::click( const _point &point )
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_button	*pbutton = get( lpos );
		if( pbutton->rect.pt_in_rect( point ) )
		{
			if( pbutton->style & XPBS_ENABLED )
			{
				pbutton->style |= XPBS_PRESSED;
				pctrl->redraw( &pbutton->rect, true );

				xp_toolbar_pane	*local_this = this;
				xpbar_ctrl			*local_ctrl = pctrl;

				::SendMessage( GetParent( pctrl->handle( )), WM_COMMAND, pbutton->cmd, 0 );
				
				if( local_ctrl->is_pane_exist( local_this ) && local_this->is_button_exist(pbutton))
				{
					pbutton->style &= ~XPBS_PRESSED;
					pctrl->redraw( &pbutton->rect );
					
				}
				return;					
			}
		}
	}
}

xp_button::xp_button()
{
	style = XPBS_ENABLED;
	hilighted = false;
	cmd = -1;
	image = -1;
}

bool xp_button::from_button( XPBUTTON *p )
{
	bool	bChanged = false;
	if( p->mask & XPBF_IMAGE )
	{
		bChanged |= (image != p->nImage);
		image = p->nImage;
	}
	if( p->mask & XPBF_TEXT )
	{
		bChanged |= true;
		text = p->string;
	}
	if( p->mask & XPBF_STYLE )
	{
		bChanged |= (style != p->style);
		style = p->style;
	}
	cmd = p->command;

	return bChanged;
}

void xp_button::to_button( XPBUTTON *p )
{
	p->command = cmd;
	if( p->mask & XPBF_TEXT )
		strncpy( p->string, text, p->cchMax );
	if( p->mask & XPBF_IMAGE )
		p->nImage = image;
	if( p->mask & XPBF_STYLE )
		p->style = style;
}

///xp_window_pane
xp_window_pane::xp_window_pane( xpbar_ctrl *p ) : xp_pane( p )
{
	hwnd = 0;
}

void xp_window_pane::show( bool show )
{
	::ShowWindow( hwnd, show?SW_SHOW:SW_HIDE );

	if( show )move();
}

void xp_window_pane::move()
{
	_rect	rect;
	client_area( &rect );
	pctrl->to_window( &rect );
	SetWindowPos( hwnd, 0, rect.left, rect.top, rect.width(), rect.height(), 
		SWP_NOZORDER/*|SWP_NOREDRAW*/ );
	//UpdateWindow( hwnd );
}


void xp_window_pane::set_window( HWND hwnd_new )
{
	if( hwnd == hwnd_new )return;
	if( hwnd )
	{
		DestroyWindow( hwnd );
	}
	SetParent( hwnd_new, pctrl->handle() );

	hwnd = hwnd_new;

}


//////
status_part::status_part()
{
	hicon = 0;
	ZeroMemory( &guid, sizeof(guid) );
	hwnd = 0;
	ref_counter = 1;
	rows = 1;
	style = 0;
	text_height=16;
	id = 0;
}

void status_part::from_part( XP_STATUS *p )
{
	if( p->mask & XPSM_HWND )
	{
		hwnd = p->hwnd;
	}
	if( p->mask & XPSM_GUID )
		guid = p->guid;
	if( p->mask & XPSM_IMAGE )
	{
		hicon = p->hIcon;
		if( hicon )
		{
			ICONINFO	info;
			::GetIconInfo( hicon, &info );
			BITMAP	bitmap;
			::GetObject( info.hbmColor, sizeof( bitmap ), &bitmap );
			size_icon.cx = bitmap.bmWidth;
			size_icon.cy = bitmap.bmHeight;

			// [vanek] : kill bitmaps - 24.12.2004
			::DeleteObject( info.hbmColor ), info.hbmColor = 0;
			::DeleteObject( info.hbmMask ), info.hbmMask = 0;
		}
	}
	if( p->mask & XPSM_STYLE )
	{
		style = p->dwStyle;
		if( style & XPSS_BOLD )
			text_height = 16;
		else
			text_height = 16;
	}
	if( p->mask & XPSM_ROWCOUNT )
		rows = p->nRows;
	if( p->mask & XPSM_INITREFCOUNT )
		ref_counter = p->nRefCounter;
	if( p->mask & XPSM_ID )
		id = p->nID;
}

xp_status_pane::xp_status_pane( xpbar_ctrl *pctrl ) : xp_pane( pctrl )
{
	height = 100;
}

void xp_status_pane::set_part_text(TPOS lpos, const char *psz)
{
	status_part	*ppart = get( lpos );
	ppart->text = psz;
	calc_layout();

	pctrl->redraw( &ppart->rect );
}

void xp_status_pane::show( bool show )
{
	if( show )calc_layout( false );

	for (TPOS lpos = head(); lpos; lpos = next(lpos))
		if( get( lpos )->hwnd )
			::ShowWindow( get( lpos )->hwnd, show?SW_SHOW:SW_HIDE );

	if( show )move();
}

void xp_status_pane::move()
{
	calc_layout( false );

	for (TPOS lpos = head(); lpos; lpos = next(lpos))
		if( get( lpos )->hwnd )
		{
			_rect	rect = get( lpos )->rect;
			pctrl->to_window( &rect );
			SetWindowPos( get(lpos)->hwnd, 0, rect.left, rect.top, rect.width(), rect.height(), 
				SWP_NOCOPYBITS|SWP_NOZORDER );
			UpdateWindow( get( lpos )->hwnd );
		}
}


TPOS xp_status_pane::set_part(XP_STATUS *pst, bool fChangeRef)
{
	TPOS	lpos = lpos_by_guid(pst->guid);
	status_part	*p = 0;
	if( lpos )
	{
		p = get( lpos );
		if( fChangeRef )p->ref_counter++;
	}
	else
	{
		if( !fChangeRef )return false;
		p = new status_part;

		lpos = insert_before( p, 0 );
	}
	p->from_part( pst );

	if( pst->mask & XPSM_TEXT )
	{
		_rect	rect_paint;
		client_area( &rect_paint );
		p->rect.left = rect_paint.left;
		p->rect.right = rect_paint.right;
		
		set_text( p, pst->pszText );
	}

	if( fChangeRef && p->hwnd )
	{
		::SetParent( p->hwnd, pctrl->handle() );
		::ShowWindow( p->hwnd, SW_HIDE );

		SendMessage( p->hwnd, WM_SETFONT, (WPARAM)pctrl->m_hfont, 0 );
	}

	calc_layout();
	pctrl->redraw( &p->rect );


	return lpos;
}

void xp_status_pane::set_text( status_part *p, const char *psz )
{
	if( p->text.length() && !strcmp( p->text, psz ) )
		return;
	p->text = psz;
	if( p->hwnd )::SetWindowText( p->hwnd, psz );

	HFONT	hfont = 0;
	_rect	rect_text = p->rect;
	if( p->hwnd )
	{
		hfont = (HFONT)::SendMessage( p->hwnd, WM_GETFONT, 0, 0 );
		::InflateRect( &rect_text, -1, -1 );
	}
	if( !hfont )hfont = pctrl->m_hfont;

	int	new_height = 0;
	if( psz )
	{
		HDC	hdc = ::GetDC( pctrl->handle() );
		::SelectObject( hdc, hfont );
		::DrawText( hdc, psz, strlen( psz ), &rect_text, DT_CALCRECT|DT_WORDBREAK );
		::ReleaseDC( pctrl->handle(), hdc );
		new_height = rect_text.height();
	}

	int	rows_count = (new_height+p->text_height-1)/p->text_height;

	if( p->rows != rows_count )
	{
		p->rows = rows_count;
		calc_layout();
	}
}

void xp_status_pane::remove_part(TPOS lpos)
{
	status_part	*p = get( lpos );
	p->ref_counter--;
	if( !p->ref_counter )
	{
		remove( lpos );
		calc_layout();
	}
}

TPOS	xp_status_pane::lpos_by_pos(long n)
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos), n--)
		if( !n )break;
	return lpos;
}
TPOS	xp_status_pane::lpos_by_guid(const GUID &guid)
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
		if( get( lpos )->guid==guid )
			break;
	return lpos;
}

void xp_status_pane::draw( Graphics &graphics, _rect &client )
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		status_part	*ppart = get( lpos );
		_rect	rect_paint = ppart->rect;

		if( rect_paint.width() == 0 )
			continue;

		if( ppart->hwnd )
			continue;

		if( ppart->hicon )
		{
			HDC	hdc = graphics.GetHDC();
			DrawIconEx( hdc, rect_paint.left, rect_paint.top, ppart->hicon, ppart->size_icon.cx, ppart->size_icon.cy, 0, 0, DI_NORMAL );
			graphics.ReleaseHDC( hdc );

			rect_paint.left += ppart->size_icon.cx+2;
		}

		StringFormat	format( 0 );
		format.SetLineAlignment( StringAlignmentCenter );

		graphics.DrawString( ppart->text, 
			-1,
			&Font( _bstr_t("Tahoma"), 
				(ppart->style & XPSS_BOLD)?8.f:8.f, 
				(ppart->style & XPSS_BOLD)?FontStyleBold:0),
			RectF(  float( rect_paint.left ), 
					float( rect_paint.top ), 
					float( rect_paint.width() ), 
					float( rect_paint.height() ) ),
			&format,
			&SolidBrush( pctrl->color_text ) );
	}
}

void xp_status_pane::calc_layout( bool update_size )
{
	_rect	rect_paint;
	client_area( &rect_paint );
	rect_paint.bottom = rect_paint.top;
	long	calc_height = 11+caption_height;

	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		status_part	*pbutton = get( lpos );
		
		int	max_heigth =max( pbutton->size_icon.cy, pbutton->text_height*pbutton->rows );
		rect_paint.top = rect_paint.bottom+5;
		rect_paint.bottom = rect_paint.top+max_heigth;
		calc_height += 5+max_heigth;

		if( rect_paint.bottom > rect.bottom )
			pbutton->rect = _rect();
		else
		{
			pbutton->rect = rect_paint;

			if( pbutton->hwnd )
			{
				_rect	rect = pbutton->rect;
				pctrl->to_window( &rect );

				if( update_size )
					MoveWindow( pbutton->hwnd, rect.left, rect.top, 
						rect.width(), rect.height(), true );
			}
		}
	}

	

	if( update_size )
	{
		long	old = height;
		height = calc_height;

		if( old != height && state == xpps_Expand )
			state = xpps_Expanding;

		pctrl->start_timer();
	}
}



/////////////////////////////////////////////////////////////////////////////////////////
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;
long				g_nInstanceCounter = 0;



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	static module	_module( 0 );

	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		_module.init( (HINSTANCE)hModule );
		xpbar_ctrl::init();
	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		xpbar_ctrl::deinit();
	}
    return TRUE;
}


LRESULT __stdcall subclass_xpbar_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_NCCREATE )
	{
		CREATESTRUCT	*pcs = (CREATESTRUCT*)l;

		if( !stricmp( pcs->lpszClass, XPBAR_CLASS ) )
		{
			xpbar_ctrl	*pbar = new xpbar_ctrl;

			pbar->subclass( hwnd, true );
			pbar->m_kill_on_destroy = true;
			return pbar->handle_message( m, w, l );
		}
		if( !stricmp( pcs->lpszClass, XPSCROLL_CLASS ) )
		{
			xp_scroll	*pbar = new xp_scroll;

			pbar->subclass( hwnd, true );
			pbar->m_kill_on_destroy = true;
			return pbar->handle_message( m, w, l );
		} 
	}
	return 0;
}

void xpbar_ctrl::init()
{
	WNDCLASS	wndclass;
	ZeroMemory( &wndclass, sizeof( wndclass ) );
	wndclass.lpszClassName = XPBAR_CLASS;
	wndclass.hInstance = module::hinst();
	wndclass.lpfnWndProc = subclass_xpbar_proc;
	wndclass.hbrBackground = 0;
	wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	wndclass.style = CS_GLOBALCLASS;
	::RegisterClass( &wndclass );

	ZeroMemory( &wndclass, sizeof( wndclass ) );
	wndclass.lpszClassName = XPSCROLL_CLASS;
	wndclass.hInstance = module::hinst();
	wndclass.lpfnWndProc = subclass_xpbar_proc;
	wndclass.hbrBackground = 0;
	wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	wndclass.style = CS_GLOBALCLASS;
	::RegisterClass( &wndclass );
}
void xpbar_ctrl::deinit()
{
	UnregisterClass( XPBAR_CLASS, module::hinst() );
}

/////////////////////////////////////////////////////////////////////////////////////////
xpbar_ctrl::xpbar_ctrl()
{
	m_fFlatSB = false;
	sb_code = SB_CTL;
	m_hscroll = 0;
	m_fLB_pressed = false;
	current_bar_width = 0;
	m_scroll = 0;
	m_scroll_cx = 0;
	m_iconCaption = 0;
	color_pane= Color(  246, 246, 236 );
	color_hdr_left= Color(  255, 252, 236 );
	color_hdr_right= Color(  224, 231, 184 );
	color_main_hdr_left= Color(  119, 140, 64 );
	color_main_hdr_right= Color(  150, 168, 103 );
	color_pane_frame= Color(  255, 255, 255 );
	color_frame_top= Color(  214, 221, 185 );
	color_frame_bottom= Color(  165, 189, 132 );
	color_acaption_text_hilight = Color( 224, 231, 184 );
	color_acaption_text = Color( 255, 255, 255 );
	color_caption_text_hilight = Color( 114, 146, 29 );
	color_caption_text = Color( 86, 102, 45 );
	color_text_hilight = Color( 114, 146, 29 );
	color_text = Color( 86, 102, 45 );
	color_disabled_text = Color( 216, 220, 170 );

	bar_width = 210;
	bar_height = 0;

	hwnd_parent = 0;
	wndproc_parent = 0;

	image_list = 0;

	pane_offset_x = 16;
	pane_offset_y = 16;
	pane_title_height = 20;

	timer_running = 0;
	progress_count = 10;
	timer_speed = 20;

	pbhilight = 0;
}

xpbar_ctrl::~xpbar_ctrl()
{
}

long xpbar_ctrl::create( HWND hwnd )
{
	win_impl::create( WS_CHILD|WS_VISIBLE		, 
		_rect( 0, 0, bar_width, bar_height ), 0, hwnd );

	return 1;
}

void xpbar_ctrl::redraw( _rect *prect, bool fUpdate )
{
	if( prect )
	{
		_rect	rect = *prect;
		to_window( &rect );
		::InvalidateRect( handle(), &rect, 0 );
	}
	else
		::InvalidateRect( handle(), 0, 0 );

	if( fUpdate )
		::UpdateWindow( handle() );
}


LRESULT xpbar_ctrl::on_create(CREATESTRUCT *pcs)
{
	win_impl::on_create( pcs );

	if( !g_nInstanceCounter )
	{
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		g_nInstanceCounter++;
	}

	m_iconCaption = (HICON)::LoadImage( module::hrc(), 
		MAKEINTRESOURCE(IDI_BUTTON), IMAGE_ICON, 0, 0, 0 );

	hwnd_parent = pcs->hwndParent;


	//init colors and fonts
	{
		Graphics graphics( handle() );
		Font	font( _bstr_t("Tahoma"), 8, 0 );
		LOGFONT	lf;
		font.GetLogFontA( &graphics, &lf );
		lf.lfCharSet = DEFAULT_CHARSET;


		m_hfont = ::CreateFontIndirect( &lf );

		m_hbrushStaticBackground = 
			::CreateSolidBrush( color_pane.ToCOLORREF() );
		m_hbrushScrollBar = 
			::CreateSolidBrush( color_frame_top.ToCOLORREF() );

	}

	if( m_fFlatSB )
	{
		::InitializeFlatSB( handle() );
//		::FlatSB_SetScrollProp( handle(), WSB_PROP_VBKGCOLOR, (long)m_hbrushScrollBar, false );
//		::FlatSB_SetScrollProp( handle(), WSB_PROP_VSTYLE, FSB_ENCARTA_MODE, false );
	}


	_update_sizes();

	return 0;
}

LRESULT xpbar_ctrl::on_destroy()
{
	::DeleteObject( m_hfont );
	::DeleteObject( m_hbrushScrollBar );
	::DeleteObject( m_hbrushStaticBackground );
	_kill_timer();


	g_nInstanceCounter--;
	if( !g_nInstanceCounter )
	{
		GdiplusShutdown(gdiplusToken);
	}
	return win_impl::on_destroy();
}

LRESULT xpbar_ctrl::on_size(short cx, short cy, ulong fSizeType)
{
	if( current_bar_width != cx )
	{
		current_bar_width = cx;
		for (TPOS lpos = head(); lpos; lpos = next(lpos))
		{
			xp_pane *p = get( lpos );
			p->rect.right = current_bar_width-pane_offset_x;
			if( sb_code == SB_VERT )p->rect.right += m_scroll_cx;
			p->move();
		}

		redraw( 0 );
	}
	_update_sizes();

	if( m_hscroll && sb_code == SB_CTL )
		SetWindowPos( m_hscroll, 0, 0, 0, m_scroll_cx, cy, SWP_NOZORDER|SWP_NOMOVE );
	
	return win_impl::on_size( cx, cy, fSizeType );
}


xp_status_pane *xpbar_ctrl::_find_statusbar()
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_pane *p = get( lpos );
		if( p->get_style() == XPPS_STATUS )return (xp_status_pane *)p;
	}
	return 0;
}

#define WS_EX_LAYERED           0x00080000

#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002

void xpbar_ctrl::_update_sizes()
{
	long	old_height = bar_height;
	bar_height = 500;
	if( tail() )
	{
		xp_pane	*p = get( tail() );
		bar_height = p->rect.bottom+pane_offset_y;
	}

	_rect	rect;
	GetClientRect( handle(), &rect );

	long	new_pos = m_scroll;
	bool	bChangeVisible = false;
	if( rect.height() < bar_height )
	{
		if( m_scroll_cx == 0 )
		{
			m_scroll_cx = ::GetSystemMetrics( SM_CXVSCROLL );
			
			_assert( m_hscroll == 0 );

			_rect	rect;
			GetClientRect( handle(), &rect );
			rect.left = rect.right - m_scroll_cx;
			
			if( sb_code == SB_CTL )
			{
				m_hscroll = ::CreateWindowEx( 0, XPSCROLL_CLASS, "", WS_CHILD|WS_VISIBLE|SBS_VERT, rect.left, rect.top,
					rect.width(), rect.height(), handle(), (HMENU)2, module::hinst(), 0 );

			}
			else
			{
				if( m_fFlatSB )::FlatSB_ShowScrollBar( handle(), sb_code, true );
				else ::ShowScrollBar( handle(), sb_code, true );
				
				m_hscroll = handle();
			}

			bChangeVisible = true;
			
		}

		SCROLLINFO	si;
		ZeroMemory( &si, sizeof( si ) );
		si.cbSize = sizeof( si );
		si.fMask = SIF_PAGE|SIF_RANGE;
		si.nMax = bar_height-1;
		si.nPage = rect.height();
		if( m_fFlatSB )::FlatSB_SetScrollInfo( m_hscroll, sb_code, &si, true );
		else if( sb_code != SB_CTL )::SetScrollInfo( m_hscroll, sb_code, &si, true );
		else SendMessage( m_hscroll, SBM_SETSCROLLINFO, true, (LPARAM)&si );

		si.fMask = SIF_POS;
		if( m_fFlatSB )::FlatSB_GetScrollInfo( m_hscroll, sb_code, &si );
		else if( sb_code != SB_CTL )::GetScrollInfo( m_hscroll, sb_code, &si );
		else SendMessage( m_hscroll, SBM_GETSCROLLINFO, 0, (LPARAM)&si );
		new_pos = si.nPos;
	}
	else
	{
		if( m_scroll_cx )
		{
			_assert( m_hscroll );

			bChangeVisible = true;
			m_scroll_cx = 0;

			if( sb_code == SB_CTL )
				::DestroyWindow( m_hscroll );
			else
			{
				if( m_fFlatSB )::FlatSB_ShowScrollBar( m_hscroll, sb_code, false );
				else ::ShowScrollBar( m_hscroll, sb_code, false );
			}
			m_hscroll = 0;
		}
		bar_height = rect.height();
		new_pos = 0;
	}

	if( bar_height != old_height )
		redraw( 0 );

	if( m_scroll != new_pos )
		on_vscroll( SB_THUMBPOSITION, new_pos, 0 );
}

LRESULT xpbar_ctrl::on_paint()
{
	PAINTSTRUCT	paint;

	_rect	rect_update;
	GetUpdateRect( handle(), &rect_update, false );

#ifdef CACHED_PAINT
	HDC	hdc_paint = ::BeginPaint( handle(), &paint );

	int	cx_paint = rect_update.right-rect_update.left;
	int	cy_paint = rect_update.bottom-rect_update.top;
	HBITMAP	hbmp = ::CreateCompatibleBitmap( hdc_paint, cx_paint, cy_paint );
	HDC	hdc = ::CreateCompatibleDC( hdc_paint );
	::SelectObject( hdc, hbmp );
	::SetViewportOrgEx( hdc, -rect_update.left, -rect_update.top-m_scroll, 0 );
#else
	HDC	hdc = ::BeginPaint( handle(), &paint );
	::SetViewportOrgEx( hdc, -rect_update.left, -rect_update.top-m_scroll, 0 );
#endif
	{


	Graphics	graphics( hdc );
//draw gradient
	Rect	rect_total( 0, 0, current_bar_width-1, bar_height-1 );

	LinearGradientBrush	brush_frame(  
		Point( 0, 0 ), 
		Point( 0, bar_height ),
		color_frame_top, 
		color_frame_bottom );

	LinearGradientBrush	brush_fill(  
		Point( 0, 0 ), 
		Point( 0, bar_height ),
		color_frame_top, 
		color_frame_bottom );

	Pen	pen_frame( &brush_frame );

	graphics.FillRectangle( &brush_frame, rect_total );
	graphics.DrawRectangle( &pen_frame, rect_total ); 
//draw panes
	
	int	delta = 3;
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_pane	*p = get( lpos );

		LinearGradientBrush	brush_caption( 
			Point( p->rect.left, 0 ),
			Point( p->rect.right, 0 ),
			p->color?color_main_hdr_left:color_hdr_left,
			p->color?color_main_hdr_right:color_hdr_right );

		Point	points[10];
		int	c = 0, top = p->rect.top+p->caption_height-pane_title_height;
		points[c].X = p->rect.left+delta;
		points[c].Y = top;
		c++;
		points[c].X = p->rect.right-delta;
		points[c].Y = top;
		c++;
		points[c].X = p->rect.right;
		points[c].Y = top+delta;
		c++;
		points[c].X = p->rect.right;
		points[c].Y = top+pane_title_height;
		c++;
		points[c].X = p->rect.left;
		points[c].Y = top+pane_title_height;
		c++;
		points[c].X = p->rect.left;
		points[c].Y = top+delta;
		c++;
		
		graphics.FillPolygon( &brush_caption, points, c );

		graphics.DrawString( _bstr_t( p->title ), 
			-1,
			&Font( _bstr_t("Tahoma"), 9, FontStyleBold ),
			PointF( float(p->rect.left+10+p->caption_offset), float(top+4) ),
			&SolidBrush( p->color?
				(p->hilighted?color_acaption_text_hilight:color_acaption_text):
				(p->hilighted?color_caption_text_hilight:color_caption_text) ) );

		if( p->image != -1 )
		{
			HDC	hdc = graphics.GetHDC();

			IMAGELISTDRAWPARAMS	params;
			ZeroMemory( &params, sizeof( params ) );
			params.cbSize = sizeof( params );
			params.himl = image_list;
			params.hdcDst = hdc;
			params.x = p->rect.left;
			params.y = p->rect.top;
			params.i = p->image;
			params.rgbBk = CLR_NONE;

			ImageList_DrawIndirect( &params );

			graphics.ReleaseHDC( hdc );
		}
		if( top + pane_title_height != p->rect.bottom )
		{
			//draw pane contents
			Rect	rect( p->rect.left, 
				top+pane_title_height,
				p->rect.width()-1,
				p->rect.height()-p->caption_height-1 );

			byte alpha = 255*p->progress/progress_count;
			Color	cur_pane( alpha, color_pane.GetRed(), color_pane.GetGreen(), color_pane.GetBlue() );
			Color	cur_frame( alpha, color_pane.GetRed(), color_pane.GetGreen(), color_pane.GetBlue() );


			graphics.FillRectangle( &SolidBrush( cur_pane ), rect );
			graphics.DrawRectangle( &Pen( &SolidBrush( cur_frame ) ), rect );

		}
		_rect	client = p->rect;
		client.top += pane_title_height;
		client.left+=1;
		client.right-=1;
		client.bottom-=1;

		p->draw( graphics, client );
	}
	}
#ifdef CACHED_PAINT
	::SetViewportOrgEx( hdc, 0, 0, 0 );
	::BitBlt( hdc_paint, rect_update.left, rect_update.top, 
		cx_paint, cy_paint,
		hdc, 0, 0, SRCCOPY );
	::DeleteDC( hdc );
	::DeleteObject( hbmp );
#endif //CACHED_PAINT
	::EndPaint( handle(), &paint );
	return 0;
}

TPOS xpbar_ctrl::on_insert(XPINSERTITEM *p)
{
	unsigned style = XPPS_UNKNOWN;

	if( p->item.mask & XPF_STYLE )
		style = p->item.style & XPPS_STYLE_MASK;

	xp_pane	*ppane = 0;
	if( style== XPPS_TOOLBAR)
		ppane = new xp_toolbar_pane( this );
	else if( style == XPPS_WINDOW )
		ppane = new xp_window_pane( this );
	else if( style == XPPS_STATUS )
		ppane = new xp_status_pane( this );
	else
		ppane = new xp_pane( this );



	ppane->from_item( &p->item );

	if( image_list == 0 )
		ppane->image = -1;

	if( ppane->image != -1 )
	{
		::ImageList_GetIconSize( image_list, (int*)&ppane->caption_offset, (int*)&ppane->caption_height );
	}
	
	TPOS lpos = insert_before(ppane, (TPOS)p->insert_pos);


	int	ypos = pane_offset_y;
	TPOS	lprev_pos = prev(lpos);

	if( lprev_pos )
		ypos = get( lprev_pos )->rect.bottom+pane_offset_y;

	//calc caption rect
	ppane->rect.top = ypos;
	ppane->rect.bottom = ypos+pane_title_height;
	ppane->rect.left = pane_offset_x;
	ppane->rect.right = current_bar_width-pane_offset_x;

	long	delta = ppane->rect.height()+pane_offset_y;
	for (TPOS lpos1 = next(lpos); lpos1; lpos1 = next(lpos1))
	{
		xp_pane *pn = get( lpos1 );
		pn->rect.top += delta;
		pn->rect.bottom += delta;
		pn->move();
	}

	InvalidateRect( handle(), 0, 0 );
	UpdateWindow( handle());

	ppane->progress = 0;
	ppane->state = xpps_Expanding;

	start_timer();
	return lpos;
}

void xpbar_ctrl::start_timer()
{
	if( timer_running )return;
	SetTimer( handle(), 777, timer_speed, 0 );
	timer_running = true;
}


void xpbar_ctrl::_kill_timer()
{
	if( !timer_running )return;
	KillTimer( handle(), 777 );
	timer_running = false;
}

LRESULT xpbar_ctrl::on_timer(ulong lEvent)
{
	if( lEvent == 777 )
	{
		//preform timer operation
		bool	bChanged = false;

		for (TPOS lpos = head(); lpos; lpos = next(lpos))
		{
			xp_pane	*p = get( lpos );

			if( p->state == xpps_Expand ||
				p->state == xpps_Collapse )
				continue;

			if( p->state == xpps_Expanding ||
				p->state == xpps_Collapsing ||
				p->state == xpps_Deleting )
			{
				if( p->state == xpps_Expanding )
				{
					if( p->progress < progress_count )p->progress++;
				}
				else
				{
					if( p->progress )p->progress--;
				}
				bChanged = true;

				long	new_height = p->caption_height+(p->height-p->caption_height)*p->progress/progress_count;
				long	delta = new_height-p->rect.height();
				long	max_bottom = p->rect.bottom; 

				p->rect.bottom = p->rect.top+new_height;
				p->move();				

				_rect	rect;
				rect.left = p->rect.left;
				rect.right = p->rect.right;
				rect.top = p->rect.top;

				bool	fOperationFinished = false;

				if( p->state == xpps_Expanding )
					if( p->progress == progress_count )
					{
						p->show( true );
						p->state = xpps_Expand;
						fOperationFinished  = true;
					}
				if( p->state == xpps_Collapsing )
					if( p->progress == 0 )
					{
						p->state = xpps_Collapse;
						fOperationFinished  = true;
					}
				
					TPOS lpos1 = next(lpos);
				max_bottom = max( p->rect.bottom, max_bottom );

				if( p->state == xpps_Deleting )
					if( p->progress == 0 )
					{
						delta-=p->rect.height()+pane_offset_y;
						remove( lpos );
						fOperationFinished  = true;
						lpos = 0;
					}

				for( ; lpos1; lpos1= next( lpos1 ) )
				{
					xp_pane *pn = get( lpos1 );

					max_bottom = max( pn->rect.bottom, max_bottom );
					pn->rect.top += delta;
					pn->rect.bottom += delta;
					max_bottom = max( pn->rect.bottom, max_bottom );
					pn->move();
				}
				
				rect.bottom = max_bottom;

				redraw( &rect, true );

				if( fOperationFinished )
				{
					_update_sizes();
					if( lpos && get( lpos )->get_style() != XPPS_STATUS )
						on_ensurevisible( lpos );
				}

				break;	//only one
			}
		}
		if( !bChanged )
			_kill_timer();
	}
	else if( lEvent == 778 )
	{
		_point	point;
		_rect	rect;

		GetCursorPos( &point );
		GetWindowRect( handle(), &rect );

		if( !rect.pt_in_rect( point ) )
			_clear_hilight();
	}

	return win_impl::on_timer( lEvent );
}

LRESULT xpbar_ctrl::on_lbuttonup(const _point &point)
{
	if( !m_fLB_pressed )return 0;
	m_fLB_pressed  = false;

	XPHITTEST	ht;
	TPOS	lpos = on_hittest(point, &ht);

	if( !lpos )return 0;

	if( ht.flags == XPHT_CAPTION )
		return on_expand( lpos, 2 );

	xp_pane	*p = get( lpos );

	_point	point_client = point;
	to_client( &point_client );
	p->click( point_client );

	return true;
}

long xpbar_ctrl::on_expand(TPOS lpos, unsigned expand)
{
	xp_pane	*p = get( lpos );

	if( p->state == xpps_Deleting )
		return 0;

	if( expand == 2 )
	{
		if( p->state == xpps_Collapse )
			expand = 1;
		else 
			expand = 0;
	}

	if( expand == p->state )
		return 0;

	if( expand )
		p->state = xpps_Expanding;
	else
	{
		p->show( false );
		p->state = xpps_Collapsing;
	}

	start_timer();

	return 1;
}

long xpbar_ctrl::on_setimagelist( HIMAGELIST hil )
{
	image_list = hil;
	return 1;
}

long xpbar_ctrl::on_addbutton(TPOS lpos, XPBUTTON *pb)
{
	xp_pane	*p = get( lpos );
	if( p->get_style() != XPPS_TOOLBAR )return 0;
	xp_toolbar_pane	*ptb = (xp_toolbar_pane*)p;
	ptb->set_button( pb, true );

	return 1;
}

long xpbar_ctrl::on_setbutton(TPOS lpos, XPBUTTON *pb)
{
	xp_pane	*p = get( lpos );
	if( p->get_style() != XPPS_TOOLBAR )return 0;
	xp_toolbar_pane	*ptb = (xp_toolbar_pane*)p;
	ptb->set_button( pb, false );

	return 1;
}
 
TPOS xpbar_ctrl::on_hittest(const _point &point_window, XPHITTEST *phit)
{
	_point	point = point_window;
	to_client( &point );


	for (TPOS lpos = head(); lpos; lpos = next(lpos))
	{
		xp_pane	*p = get( lpos );
		if( p->rect.pt_in_rect( point ) )
		{
			for (TPOS lpos_btn = p->buttons.head(); lpos_btn;
				lpos_btn = p->buttons.next( lpos_btn ) )
				{
					caption_button	*pbutton = p->buttons.get( lpos_btn );
					if( pbutton->rect.pt_in_rect( point ) )
					{
						phit->flags = XPHT_CAPTIONBUTTON;
						phit->rect_invalidate = p->rect;
						phit->rect_invalidate.bottom = p->rect.top+pane_title_height;
						phit->pbhilight = 0;
						return lpos;
					}
				}
			//hit test on caption
			if( point.y < p->rect.top + p->caption_height )
			{
				if( phit )
				{
					phit->flags = XPHT_CAPTION;
					phit->rect_invalidate = p->rect;
					phit->rect_invalidate.bottom = p->rect.top+pane_title_height;
					phit->pbhilight = &p->hilighted;
				}
				return lpos;
			}
			else
			{
				long lret = p->hit_test(point, phit);
				if( lret )return lpos;
				if( phit )
				{
					phit->flags = XPHT_CLIENT;
					phit->pbhilight = 0;
				}
				return lpos;
			}
		}
	}
	return 0;
}

#define WM_SETMESSAGESTRING 0x0362  // wParam = nIDS (or 0),

LRESULT xpbar_ctrl::on_mousemove(const _point &point)
{
	XPHITTEST	ht;
	TPOS	lpos = on_hittest(point, &ht);

	if( !lpos )
	{
		_clear_hilight();
		return 0;
	}
	
	if( ht.pbhilight == pbhilight )
		return 0;

	if( ht.flags == XPHT_BUTTON )
	{
		TPOS	lpos_button = (TPOS)ht.lparam;
		xp_button	*pbtn = _list_t<xp_button*, free_button>::get( lpos_button );
		::SendMessage( ::GetParent( handle() ), WM_SETMESSAGESTRING, pbtn->cmd, 0 );
	}


	_clear_hilight();

	pbhilight = ht.pbhilight;
	rect_hilight = ht.rect_invalidate;

	if( pbhilight )
	{
		*pbhilight = true;

		redraw( &rect_hilight );
		::SetTimer( handle(), 778, 100, 0 );
	}


	return 1;
}

void xpbar_ctrl::_clear_hilight()
{
	if( !pbhilight )return;
	*pbhilight = false;
	pbhilight = 0;
	redraw( &rect_hilight );
	::KillTimer( handle(), 778 );
	::SendMessage( ::GetParent( handle() ), WM_SETMESSAGESTRING, 0, 0 );

}

long xpbar_ctrl::on_removeitem(TPOS lpos)
{
	if( !lpos )return false;
	xp_pane	*p = get( lpos );
	p->state = xpps_Deleting;
	p->show( false );
	start_timer();

	return true;
}

long xpbar_ctrl::on_setwindow(TPOS lpos, HWND hwnd)
{
	if( !lpos )return 0;
	xp_pane	*p = get( lpos );
	if( p->get_style() != XPPS_WINDOW )
		return 0;
	xp_window_pane	*pwp = (xp_window_pane*)p;
	pwp->set_window( hwnd );

	return true;
}


long xpbar_ctrl::on_setpanetext( long n, const char *psz )
{
	xp_status_pane *pst = _find_statusbar();
	if( !pst )return 0;

	TPOS	lpos = pst->lpos_by_pos(n);
	if( lpos )pst->set_part_text( lpos, psz );

	return 1;
}

TPOS xpbar_ctrl::on_addstatuspane(TPOS lpos, XP_STATUS *p, bool fChangeRef)
{
	if( !lpos )return 0;
	xp_pane	*pp = get( lpos );
	if( pp->get_style() != XPPS_STATUS )
		return 0;
	return ((xp_status_pane*)pp)->set_part( p, fChangeRef );
}

long xpbar_ctrl::on_removestatuspane(TPOS lpos, GUID*	pg)
{
	if( !lpos )return 0;
	xp_pane	*pp = get( lpos );
	if( pp->get_style() != XPPS_STATUS )
		return 0;

	xp_status_pane	*pst = (xp_status_pane*)pp;
	TPOS	lpos_p = pst->lpos_by_guid(*pg);
	if( lpos_p )pst->remove_part( lpos_p );

	return 1;
}

long xpbar_ctrl::on_addcaptionbutton(TPOS lpos, XP_CAPTION_BUTTON*	p)
{
	if( !lpos )return 0;
	xp_pane	*pp = get( lpos );
	pp->set_caption( p, true );
	return 1;
}

long xpbar_ctrl::on_removecaptionbutton(TPOS lpos, unsigned id)
{
	if( !lpos )return 0;
	xp_pane	*pp = get( lpos );
	pp->remove_caption( id );
	return 1;
}

long xpbar_ctrl::on_setcaptionbutton(TPOS lpos, XP_CAPTION_BUTTON*	p)
{
	if( !lpos )return 0;
	xp_pane	*pp = get( lpos );
	pp->set_caption( p, false );
	return 1;
}

LRESULT xpbar_ctrl::on_vscroll(unsigned code, unsigned pos, HWND hwndScroll)
{
	SCROLLINFO	si;
	ZeroMemory( &si, sizeof(si) );
	si.cbSize = sizeof( si );

	long	scroll_pos = m_scroll;

	switch( code )
	{
	case SB_LINEUP:	scroll_pos -= 10;break;
	case SB_LINEDOWN:	scroll_pos += 10;break;
	case SB_PAGEUP:	scroll_pos -= 100;break;
	case SB_PAGEDOWN:	scroll_pos += 100;break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:		scroll_pos = pos; break;
	}

	if( m_scroll != scroll_pos )
	{
		si.nPos = si.nTrackPos = scroll_pos;
		si.fMask = SIF_TRACKPOS|SIF_POS;			

		if( m_fFlatSB )::FlatSB_SetScrollInfo( m_hscroll, sb_code, &si, true );
		else if( sb_code != SB_CTL )::SetScrollInfo( m_hscroll, sb_code, &si, true );
		else ::SendMessage( m_hscroll, SBM_SETSCROLLINFO, true, (LPARAM)&si );

		if( m_fFlatSB )::FlatSB_GetScrollInfo( m_hscroll, sb_code, &si );
		else if( sb_code != SB_CTL )::GetScrollInfo( m_hscroll, sb_code, &si );
		else ::SendMessage( m_hscroll, SBM_GETSCROLLINFO, 0, (LPARAM)&si );

		int	delta = si.nPos-m_scroll;
		m_scroll = si.nPos;

		_rect	rect;
		::GetClientRect( handle(), &rect );
		if( sb_code == SB_CTL )rect.right -= m_scroll_cx;

		::ScrollWindow( handle(), 0, -delta, &rect, 0 );

		if( delta > 0 )
			rect.bottom = rect.top+delta-1;
		else
			rect.top = rect.bottom+delta+1;
		for (TPOS lpos = head(); lpos; lpos = next(lpos))
			get( lpos )->move();

		::InvalidateRect( handle(), &rect, false );
		::UpdateWindow( handle() );

	}
	return true;
}

long xpbar_ctrl::on_ensurevisible(TPOS lpos)
{
	if( !lpos )return 0;

	_rect	rect_client;
	GetClientRect( handle(), &rect_client );
	xp_pane	*p = get( lpos );
	_rect	rect_pane;
	rect_pane = p->rect;
	to_window( &rect_pane );
	rect_pane.top-= pane_offset_y;
	rect_pane.bottom+=pane_offset_y;

	long	scroll = m_scroll;

	if( rect_pane.top < rect_client.top )
		scroll += rect_client.top-rect_pane.top;
	if( rect_pane.bottom >  rect_client.bottom )
		scroll -= rect_client.bottom-rect_pane.bottom;

	on_vscroll( SB_THUMBPOSITION, scroll, 0 ); 

	return 1;
}

LRESULT xpbar_ctrl::on_ncdestroy()
{
	win_impl::on_ncdestroy(); 
	return 0;
}

long xpbar_ctrl::on_getdefparams( unsigned param )
{
	return bar_width;
}

long xpbar_ctrl::on_getitem(TPOS lpos, XPPANEITEM *p)
{
	xp_pane	*ppane = get( lpos );
	
	if( p->mask & XPF_TEXT )
		strncpy( p->pszText, ppane->title, p->cchMaxText );
	if( p->mask & XPF_IMAGE )
		p->nImage = ppane->image;
	if( p->mask & XPF_IMAGELIST )
	{
		if( ppane->get_style()  == XPPS_TOOLBAR )
			p->hImageList = ((xp_toolbar_pane	*)ppane)->image_list;
		else 
			p->hImageList = 0;
	}
	if( p->mask & XPF_HEIGHT )
		p->nHeight = ppane->height;
	if( p->mask & XPF_PARAM )
		p->lParam = ppane->param;
	if( p->mask & XPF_STYLE )
		p->style = ppane->get_style();
	if( p->mask & XPF_IMAGELIST_CAPTION )
		p->hImageListCaption = ppane->images_caption;

	return 1;
}

long xpbar_ctrl::on_setitem(TPOS lpos, XPPANEITEM *p)
{
	xp_pane	*ppane = get( lpos );
	ppane->from_item( p );

	if( ppane->state== xpps_Expand )
		redraw( &ppane->rect );

	return 0;
}

TPOS xpbar_ctrl::on_getnextitem(TPOS lpos)
{
	if( !lpos )return head();
	else return next( lpos );
}

long xpbar_ctrl::on_ctlcolor( int nCtlType, HDC hdc, HWND hwnd )
{
	::SelectObject( hdc, m_hfont );
	::SetTextColor( hdc, color_text.ToCOLORREF() );
	
	if( nCtlType == WM_CTLCOLORSCROLLBAR )
	{
		return (long)m_hbrushScrollBar;
	}
	return (long)m_hbrushStaticBackground;
}

TPOS xpbar_ctrl::on_getnextbutton(TPOS lpos, TPOS lpos_b)
{
	xp_pane	*ppane = get( lpos );
	if( ppane->get_style() != XPPS_TOOLBAR )return 0;

	xp_toolbar_pane	*pt = (xp_toolbar_pane*)ppane;

	return !!lpos_b?pt->next( lpos_b ):pt->head();
}

long xpbar_ctrl::on_getbuttonbypos(TPOS lpos_b, XPBUTTON*	p)
{
	xp_button	*pbutton = xp_toolbar_pane::get( lpos_b );
	pbutton->to_button( p );
	return 0;
}

long xpbar_ctrl::on_setbuttonbypos(TPOS lpos_b, XPBUTTON*	p)
{
	xp_button	*pbutton = xp_toolbar_pane::get( lpos_b );
	if( pbutton->from_button( p ) )
		redraw( &pbutton->rect );
	return 0;
}

long xpbar_ctrl::on_helphittest( const _point &point )
{
	XPHITTEST	ht;
	TPOS	lpos = on_hittest(point, &ht);
	if( !lpos )return -1;

	if( ht.flags == XPHT_BUTTON )
	{
		TPOS	lpos_button = (TPOS)ht.lparam;
		xp_button	*pbtn = _list_t<xp_button*, free_button>::get( lpos_button );
		return pbtn->cmd;
	}
	return -1;
}

long xpbar_ctrl::on_settimerparams( XP_TIMER_PARAMS *pparams )
{
	progress_count = pparams->stages_count;
	timer_speed = pparams->timer_speed;

	return 0;
}

bool xpbar_ctrl::is_pane_exist( xp_pane *ppane )
{
	for (TPOS lpos = head(); lpos; lpos = next(lpos))
		if( get( lpos ) == ppane )
			return true;
	return false;
}