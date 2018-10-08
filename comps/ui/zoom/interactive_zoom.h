#ifndef __interactive_zoom_h__
#define __interactive_zoom_h__

#include "action_ui_int.h"

#include "\vt5\awin\win_base.h"
#include "\vt5\awin\win_dlg.h"
#include "\vt5\awin\com_unknown.h"
/////////////////////////////////////////////////////////////////////////////////////
//
//	class CActionInteractiveZoom
//
/////////////////////////////////////////////////////////////////////////////////////
class CActionInteractiveZoom : public CActionBase
{
	DECLARE_DYNCREATE(CActionInteractiveZoom)
	GUARD_DECLARE_OLECREATE(CActionInteractiveZoom)	

public:
	CActionInteractiveZoom();
	virtual ~CActionInteractiveZoom();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

#define	SLIDER_ID	10025
#define	EDITOR_ID	10026


#define ICON_WIDTH 0
#define EDITOR_WIDTH 60
#define DROP_BTN_WIDTH 12


#define ZOOM_CTRL_WIDTH (ICON_WIDTH+EDITOR_WIDTH+DROP_BTN_WIDTH)
#define ZOOM_CTRL_HEIGHT 20

#define SLIDER_HEIGHT 30


//////////////////////////////////////////////////////////////////////
//	Class slider_wnd
//////////////////////////////////////////////////////////////////////
class slider_wnd : public dlg_impl
{
public:
	slider_wnd();
	virtual ~slider_wnd();

	void			init();	

//message handlers
	virtual LRESULT	on_initdialog();
	virtual LRESULT	on_killfocus(HWND hwndOld);
	virtual LRESULT	handle_message(UINT m, WPARAM w, LPARAM l);
	virtual LRESULT	on_notify(uint idc, NMHDR *pnmhdr);
	virtual LRESULT	on_hscroll(unsigned code, unsigned pos, HWND hwndScroll);

	void			kill_slider();

	bool			m_inside_killing;

};

class win_impl_with_tooltip : public win_impl
{
	CToolTipCtrl m_ToolTipCtrl;
	HWND	m_hwndOld;
public:
	win_impl_with_tooltip() { m_hwndOld = 0; }
	virtual LRESULT	handle_message(UINT m, WPARAM w, LPARAM l);
	
	// vanek - 18.09.2003
	virtual LRESULT	on_setfocus(HWND hwndOld)
	{
		m_hwndOld = hwndOld;
		return __super::on_setfocus( hwndOld );
	}
	
	virtual LRESULT on_keydown(long nVirtKey)
	{		
		if( nVirtKey == VK_ESCAPE && ::IsWindow( m_hwndOld ) )
		{
			::SetFocus( m_hwndOld );
			m_hwndOld = 0;
		}
		return __super::on_keydown( nVirtKey );	
    }
	//

protected:
	void _enable_tooltip();
};

//////////////////////////////////////////////////////////////////////
//	Class edotor_wnd
//////////////////////////////////////////////////////////////////////
class editor_wnd : public win_impl_with_tooltip
{
public:
	editor_wnd();
	virtual ~editor_wnd();
//message handlers
};

//////////////////////////////////////////////////////////////////////
//	Class zoom_wnd
//////////////////////////////////////////////////////////////////////

class zoom_wnd : public win_impl_with_tooltip, public com_unknown, public IEventListener
{
public:
	zoom_wnd();
	virtual ~zoom_wnd();
	implement_unknown();

	virtual IUnknown* get_interface( const IID &iid );

//message handlers
	virtual void	handle_init();	
	virtual LRESULT	on_destroy();

	virtual LRESULT	on_paint();
	virtual LRESULT	on_erasebkgnd(HDC);

	virtual LRESULT	handle_message( UINT m, WPARAM w, LPARAM l );
	virtual LRESULT	on_mousemove(const _point &point);
	virtual LRESULT	on_lbuttondown(const _point &point);
	virtual LRESULT	on_lbuttonup(const _point &point);
	virtual LRESULT	on_size(short cx, short cy, ulong fSizeType);
	
	bool			show_slider();

	_rect			get_icon_rect();
	_rect			get_editor_rect();
	_rect			get_button_rect();

	void			recalc_layout( int cx, int cy );
	_rect			get_ctrl_rect();
	
	void			invalidate_icon_rect();
	void			invalidate_btn_rect();

	
	editor_wnd		m_editor_wnd;

	HICON			m_icon_arrow;
	HICON			m_icon_zoom;

	bool			m_btrack_leave;
	_point			m_pt_prev;
	_point			m_pt_click;

	void			set_zoom_to_editor();
//notify
protected:
	bool			m_in_notify;
	bool			m_binternal_set_wnd_text;
	//IEventListener
	com_call Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize );
	com_call IsInNotify( long *pbFlag );
	bool _fixx( char *pStr, int len );
};


//////////////////////////////////////////////////////////////////////
//
//	Class CZoomSlider
//
//////////////////////////////////////////////////////////////////////
class CZoomSlider : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CZoomSlider);
	
protected:
	CZoomSlider();
	~CZoomSlider();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(UIControl,IUIControl)
		com_call Create( HWND hwnd, RECT rc );
		com_call GetSize( SIZE* psize, BOOL bhorz );
		com_call GetHwnd( HWND* phwnd );
		com_call Draw( HDC hdc, RECT rc, DWORD dw_flags );
	END_INTERFACE_PART(UIControl)

protected:
	zoom_wnd	m_zoom_wnd;	
	HWND		m_hwnd_parent;
};



#endif//__interactive_zoom_h__
