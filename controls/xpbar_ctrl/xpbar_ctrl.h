#ifndef __xpbar_ctrl__
#define __xpbar_ctrl__

#include "comdef.h"

#include "\vt5\awin\win_base.h"
#include "\vt5\awin\misc_list.h"
#include "xp_bar.h"

//from MFC
#define WM_HELPHITTEST      0x0366  // lResult = dwContext,
									// lParam = MAKELONG(x,y)

enum state
{

	xpps_Collapse = 0,
	xpps_Expand,
	xpps_Expanding,
	xpps_Collapsing,
	xpps_Deleting,
};

class xp_button
{
public:
	int			image;
	_bstr_t		text;
	unsigned	cmd, style;
	bool		hilighted;
	
	_rect		rect;
public:
	xp_button();
	bool from_button( XPBUTTON *p );	
	void to_button( XPBUTTON *p );	
};


inline void free_button( void *p )
{
	delete(xp_button*)p;
}

class status_part
{
public:
	long	ref_counter;

	_rect	rect;
	_bstr_t	text;
	HWND	hwnd;
	HICON	hicon;
	GUID	guid;
	int		rows, text_height;
	unsigned	style, id;
	_size	size_icon;
	
	status_part();
	void from_part( XP_STATUS *p );
};

inline void free_status_item( void *p )
{	delete(status_part *)p;}


class xpbar_ctrl;

struct	caption_button
{
	unsigned	command, state;
	int			image;

	_rect	rect;
};

inline void free_caption_button( void *p )
{	delete (caption_button*)p;}

class xp_pane
{
public:
	char	*title;	
	_rect	rect;
	int		image;
	bool	hilighted;
	bool	color;

	long		height, param, caption_height, caption_offset;
	unsigned	transparency, progress, state;	//0-255
	xpbar_ctrl	*pctrl;
	HIMAGELIST	images_caption;
	_list_t<caption_button*, free_caption_button>	buttons;

	xp_pane( xpbar_ctrl * );
	virtual ~xp_pane();
public:
	virtual void from_item( XPPANEITEM *p );
	virtual void draw( Graphics &grapics, _rect &client );
	virtual long hit_test(const _point &point, XPHITTEST *phit)	{ return 0; }
	virtual unsigned get_style()	{return XPPS_UNKNOWN;}
	virtual void click( const _point &point );
	virtual void client_area( RECT *prect );
	virtual void show( bool show )		{};
	virtual void move()					{}

	void set_caption( XP_CAPTION_BUTTON *, bool bCreateNew );
	void remove_caption( unsigned id );
protected:
	void _redraw_caption( bool fUpdate = false );
};

inline void free_pane( void *p )
{
	delete(xp_pane*)p;
}

class xp_toolbar_pane : public xp_pane, public _list_t<xp_button*, free_button>
{
public:
	xp_toolbar_pane( xpbar_ctrl *p );
	virtual ~xp_toolbar_pane();

	virtual void from_item( XPPANEITEM *p );
	virtual void draw( Graphics &grapics, _rect &client );
	virtual long hit_test(const _point &point, XPHITTEST *phit);
	virtual void click( const _point &point );

	void set_button( XPBUTTON *p, bool bnew );
	void calc_layout( bool update_size );
	virtual void show( bool show );
	virtual void move();
	virtual unsigned get_style()	{return XPPS_TOOLBAR;}
	bool is_button_exist(xp_button*p);
public:
	HIMAGELIST	image_list;
	_size		size_button;
};

class xp_window_pane : public xp_pane
{
public:
	xp_window_pane( xpbar_ctrl *p );

	void set_window( HWND hwnd );
	virtual void show( bool show );
	virtual void move();
	virtual unsigned get_style()	{return XPPS_WINDOW;}
public:
	HWND	hwnd;
};

/*
com_call AddPane( const GUID &guidPane, int cxWidth, HWND hwnd, DWORD dwStyle ) = 0;
com_call RemovePane( const GUID &guidPane ) = 0;
com_call GetPaneID( const GUID &guidPane, long *pnID ) = 0;
com_call SetPaneIcon( const GUID &guidPane, HICON hIcon ) = 0;
*/


class xp_status_pane : public xp_pane, public _list_t<status_part*, free_status_item>
{
public:
	xp_status_pane( xpbar_ctrl *pctrl );

	void set_part_text(TPOS lpos, const char *psz);
	TPOS set_part(XP_STATUS *pst, bool bChangeRef);
	void remove_part(TPOS lpos);
	TPOS lpos_by_pos(long n);
	TPOS lpos_by_guid(const GUID &guid);
	void set_text( status_part *p, const char *psz );

	virtual void draw( Graphics &grapics, _rect &client );
	virtual void show( bool show );
	virtual void move();
	void calc_layout( bool update_size = true );

	virtual unsigned get_style()	{return XPPS_STATUS;}
protected:
};



class xpbar_ctrl: public win_impl, public _list_t<xp_pane*, free_pane>
{
friend LPARAM __stdcall subclass_xpbar_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );
public:
	xpbar_ctrl();
	virtual ~xpbar_ctrl();

	long create( HWND hwnd );
	void start_timer();
	HICON get_caption_icon()	{return m_iconCaption;}

	void	redraw( _rect *prect, bool fUpdate = false );
	void	to_window( _rect *prect )		{	prect->top -= m_scroll;	prect->bottom-= m_scroll;}
	void	to_window( _point *ppoint )		{	ppoint->y -= m_scroll;}
	void	to_client( _rect *prect )		{	prect->top += m_scroll;	prect->bottom+= m_scroll;}
	void	to_client( _point *ppoint )		{	ppoint->y += m_scroll;}
	bool	is_pane_exist( xp_pane *ppane );
/////////////////////////
	static void init();
	static void deinit();

protected:
	virtual LRESULT on_paint();
	virtual LRESULT on_create(CREATESTRUCT *pcs);
	virtual LRESULT on_destroy();
	virtual LRESULT on_ncdestroy();
	virtual LRESULT on_timer(ulong lEvent);
	virtual LRESULT on_lbuttonup(const _point &point);
	virtual LRESULT on_lbuttondown(const _point &point)		{ m_fLB_pressed = true; return win_impl::on_lbuttondown(point); }
	virtual LRESULT on_mousemove(const _point &point);

	virtual TPOS on_insert(XPINSERTITEM *p);
	virtual long on_expand(TPOS lpos, unsigned type);
	virtual long on_setimagelist( HIMAGELIST hil );
	virtual long on_addbutton(TPOS lpos, XPBUTTON *pb);
	virtual long on_setbutton(TPOS lpos, XPBUTTON *pb);
	virtual TPOS on_hittest(const _point &point, XPHITTEST *phit);
	virtual long on_removeitem(TPOS lpos);
	virtual long on_setwindow(TPOS lpos, HWND hwnd);
	virtual LRESULT on_erasebkgnd(HDC)	{ return 0; }
	virtual long on_setpanetext( long n, const char *psz );	
	virtual TPOS on_addstatuspane(TPOS lpos, XP_STATUS *p, bool fChangeRef);
	virtual long on_removestatuspane(TPOS lpos, GUID*	pg);
	virtual long on_addcaptionbutton(TPOS lpos, XP_CAPTION_BUTTON*	p);
	virtual long on_removecaptionbutton(TPOS lpos, unsigned id);
	virtual long on_setcaptionbutton(TPOS lpos, XP_CAPTION_BUTTON*	p);
	virtual LRESULT on_vscroll(unsigned code, unsigned pos, HWND hwndScroll);
	virtual long on_ensurevisible(TPOS lpos);
	virtual LRESULT on_size(short cx, short cy, ulong fSizeType);
	virtual long on_getdefparams( unsigned param );
	virtual long on_getitem(TPOS lpos, XPPANEITEM *p);
	virtual long on_setitem(TPOS lpos, XPPANEITEM *p);
	virtual TPOS on_getnextitem(TPOS lpos);
	virtual long on_ctlcolor( int nCtlType, HDC hdc, HWND hwnd );
	virtual TPOS on_getnextbutton(TPOS lpos, TPOS lpos_b);
	virtual long on_getbuttonbypos(TPOS lpos_b, XPBUTTON*	p);
	virtual long on_setbuttonbypos(TPOS lpos_b, XPBUTTON*	p);
	virtual long on_helphittest( const _point &point );
	virtual long on_settimerparams( XP_TIMER_PARAMS *pparams );

	virtual LRESULT handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == XPB_INSERTITEM )
			return (LRESULT)on_insert( (XPINSERTITEM*)l );
		else if( m == XPB_EXPANDITEM )
			return on_expand( (TPOS)w, l );
		else if( m == XPB_SETIMAGELIST )
			return on_setimagelist( (HIMAGELIST)l );
		else if( m == XPB_ADDBUTTON )
			return on_addbutton( (TPOS)w, (XPBUTTON*)l );
		else if( m == XPB_SETBUTTON )
			return on_setbutton( (TPOS)w, (XPBUTTON*)l );
		else if( m == XPB_HITTEST )
			return (LRESULT)on_hittest( _point( l ), (XPHITTEST *)w );
		else if( m == XPB_REMOVEITEM )
			return on_removeitem((TPOS)w);
		else if( m == XPB_SETWINDOW )
			return on_setwindow((TPOS)w, (HWND)l);
		else if( m == SB_SETTEXT )
			return on_setpanetext( w, (const char *)l );
		else if( m == WM_SETTEXT )
			return on_setpanetext( 0, (const char *)l );
		else if( m == XPB_ADDSTATUSPANE )
			return (LRESULT)on_addstatuspane( (TPOS)w, (XP_STATUS *)l, true );
		else if( m == XPB_SETSTATUSPANE )
			return (LRESULT)on_addstatuspane((TPOS)w, (XP_STATUS *)l, false);
		else if( m == XBP_REMOVESTATUSPANE )
			return on_removestatuspane((TPOS)w, (GUID*)l);
		else if( m == XBP_ADDCAPTIONBUTTON )
			return on_addcaptionbutton((TPOS)w, (XP_CAPTION_BUTTON*)l);
		else if( m == XBP_REMOVECAPTIONBUTTON )
			return on_removecaptionbutton((TPOS)w, l);
		else if( m == XBP_SETCAPTIONBUTTON )
			return on_setcaptionbutton((TPOS)w, (XP_CAPTION_BUTTON*)l);
		else if( m == XBP_ENSUREVISIBLE )
			return on_ensurevisible((TPOS)w);
		else if( m == XPB_GETDEFPARAMS )
			return on_getdefparams( w );
		else if( m == XPB_GETITEM )
			return on_getitem((TPOS)w, (XPPANEITEM*)l);
		else if( m == XPB_SETITEM )
			return on_setitem((TPOS)w, (XPPANEITEM*)l);
		else if( m == XPB_GETFIRSTITEM )
			return (LRESULT)on_getnextitem( 0 );
		else if( m == XPB_GETNEXTITEM )
			return (LRESULT)on_getnextitem((TPOS)w);
		else if( m == WM_GETFONT )
			return (LRESULT)m_hfont;
		else if( m == XPB_GETFIRSTBUTTON )
			return (LRESULT)on_getnextbutton((TPOS)w, (TPOS)0);
		else if( m == XPB_GETNEXTBUTTON )
			return (LRESULT)on_getnextbutton((TPOS)w, (TPOS)l);
		else if( m == XPB_GETBUTTONBYPOS )
			return on_getbuttonbypos((TPOS)w, (XPBUTTON*)l);
		else if( m == XPB_SETBUTTONBYPOS )
			return on_setbuttonbypos((TPOS)w, (XPBUTTON*)l);
		else if( m == WM_HELPHITTEST )
			return on_helphittest( _point( l ) );
		else if( m == XPB_SETTIMERPARAMS )
			return on_settimerparams( (XP_TIMER_PARAMS*)l );
		else if(m == WM_CTLCOLORSTATIC||
				m == WM_CTLCOLORMSGBOX||
				m == WM_CTLCOLOREDIT ||
				m == WM_CTLCOLORLISTBOX ||
				m == WM_CTLCOLORBTN ||
				m == WM_CTLCOLORDLG ||
				m == WM_CTLCOLORSCROLLBAR )
				return on_ctlcolor( m, (HDC)w, (HWND)l );

		else
			return win_impl::handle_message( m, w, l );
	}
protected:
	void _kill_timer();
	xp_status_pane *_find_statusbar();
	void	_update_sizes();
public:
	void _clear_hilight();
public:
	Color		
			color_pane,
			color_hdr_left,
			color_hdr_right,
			color_main_hdr_left,
			color_main_hdr_right,
			color_pane_frame, color_frame_top, color_frame_bottom,
			color_caption_text,
			color_caption_text_hilight,
			color_acaption_text,
			color_acaption_text_hilight,
			color_text, color_text_hilight,
			color_disabled_text;
	long	bar_width, 
			bar_height,
			pane_offset_x,
			pane_offset_y,
			pane_title_height,
			title_offset_x,
			title_offset_y;
	bool	timer_running;
	unsigned progress_count, timer_speed;
//subclass information
	HFONT			m_hfont;
	HBRUSH			m_hbrushStaticBackground, m_hbrushScrollBar;
private:
	HWND			hwnd_parent;
	WNDPROC			wndproc_parent;
	_wndproc_thunk	thunk_parent;

	HIMAGELIST		image_list;

	_rect			rect_hilight;
	bool			*pbhilight;
	HICON			m_iconCaption;

	int				m_scroll, m_scroll_cx, current_bar_width; 
	bool			m_fLB_pressed;
	HWND			m_hscroll;
	unsigned		sb_code;
	bool			m_fFlatSB;
};

#endif //__xpbar_ctrl__