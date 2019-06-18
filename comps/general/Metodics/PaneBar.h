#pragma once

#include <\vt5\awin\win_base.h>
#include <\vt5\awin\misc_list.h>

// vanek : drawing disabled icons - 27.02.2005
#include "\vt5\comps\misc\glib\glib.h"

namespace MetodicsTabSpace
{
namespace EnviromentClasses
{

#define WM_NEED_RECALC			( WM_USER + 0x7010 )
#define WM_GET_MAX_SIZE		( WM_NEED_RECALC + 1 )
#define WM_UPDATESCROLL		( WM_GET_MAX_SIZE + 1 )
#define WM_IS_PANE_OPEN		( WM_UPDATESCROLL + 1 )
#define WM_PREV_PANE_WIDTH		( WM_IS_PANE_OPEN + 1 )

class CPaneItemPlace;
class CPaneBar;
class CPaneItem
{
	friend class CPaneItemPlace;
	friend class CPaneBar;

	CString m_strActionName;
	CString m_strActionDescr;
	CString m_strActionUserName;

	COLORREF m_cl_color_breaker_1;
	COLORREF m_cl_color_breaker_2;

	
	POINT m_ptOffset;
	RECT m_rcItem;
	RECT m_rcImage;

	HICON m_hIcon;
	bool m_bcustom_icon;
	long m_nMainWidth;
	bool m_bEmptyCommand;
	long m_nid;
	bool m_bbreaker;
	CString m_str_custom_icon;
	COLORREF m_cl_back_color;
	static HWND m_hwnd_open_group;
	static CPaneItem *m_pactive_item;
	bool m_bsum_mode;
	static bool m_bhelp_mode;
public:
	static IActionInfo *sptrActionInfo;
	void set_sub_mode( bool bmode ) { m_bsum_mode = bmode; } 
	CPaneItem();
	~CPaneItem();
	void do_draw( HDC, DWORD dwFlags, bool bCalcOnly = false );

	int get_id() { return m_nid; }

	static void _self_destroyer( CPaneItem *pItem )
	{
		delete pItem;
	}
	bool is_empty() { return 0; return m_bEmptyCommand; }

	void store_content( INamedData *punkND, CString strRoot );
	void load_content( INamedData *punkND, CString strRoot );

	POINT get_offset() { return m_ptOffset; }
	void set_offset( POINT pt ) { m_ptOffset = pt; }

	void set_main_width( long lWidth );
	long get_main_width() { return m_nMainWidth; }

	RECT get_virtual_rect();

	void create_from_info( IActionInfo *punkInfo );
	void execute_cmd();

	void on_mouse_over( POINT pt, HWND hwnd );
	void on_mouse_leave();

	DWORD get_action_flag();
 protected:
	 CPaneItemPlace *m_psub_items;
public:
	// vanek : drawing disabled icons - 27.02.2005
	static CImageDrawer m_ImgDrawer;

	bool flag_activeItem;
	int NumGetTick;
	int nCountItem;
	int yHeightItem;
	bool m_bDropItemPane;
	bool bDragDrop;
};


class CPaneItemPlace : public win_impl
{
	friend class CPaneItem;
	_list_t<CPaneItem*, &CPaneItem::_self_destroyer > m_listItems;

	bool m_bsub_mode;

	POINT m_ptPrevMouse;
	HCURSOR m_hHand;
	HCURSOR m_hActive;

	POINT m_ptDown;
	POINT m_ptOver;
	RECT m_rcOver;

	bool m_bLDown, m_bMouseOver;

	bool m_bMove;
	CToolTipCtrl	*m_pCtrl;
	bool m_brunned;
	bool m_bToolTipVisible;
public:
	bool is_runned() { return m_brunned; }
	void set_main_width( int nw )
	{
		for( long lw = m_listItems.head(); lw; lw = m_listItems.next( lw ) )
		{
			CPaneItem *pp = m_listItems.get( lw );
			pp->set_main_width( nw );
		}
	}
	void sub_mode( bool bval = true )
	{ m_bsub_mode = bval; }

	void add_help_items( CStringArrayEx *pstrs, int &nidx, int nlevel );

	CPaneItemPlace();
	~CPaneItemPlace();
	void add_item( CPaneItem *p ) { m_listItems.add_tail( p ); }
	CPaneItemPlace *get_sub_place_from_handle( HWND hwnd );

	RECT get_virtual_rect();
	long on_erasebkgnd( HDC );
	long on_paint();
	void check_scroll( bool *pbUpScroll, bool *pbDownScroll );
	int get_row_height();
	void offset_items( POINT ptOffset );
	virtual long on_mousemove( const _point &point );
	virtual long on_lbuttondown( const _point &point );
	virtual long on_lbuttonup( const _point &point );
	virtual long	handle_message( UINT m, WPARAM w, LPARAM l );
	virtual long on_size( short cx, short cy, ulong fSizeType );

	void store_content( INamedData *punkND, CString strRoot );
	void load_content( INamedData *punkND, CString strRoot );
	virtual long on_timer( ulong lEvent );

	CPaneItem *get_item( long lIndex );
	long get_item_count( ) { return m_listItems.count(); }
	CPaneItem* get_item_by_pos( POINT ptScreen );
	void   remove_item( long lIndex ); 

	void on_context_help();
	void add_item( IActionInfo *punkActionInfo, POINT pt );
	void clear_items();

	int iKeyDown;
	//bool fKeyDown;
	int iCountPaint;
	BOOL flag_alpha;
};

class CPaneScrollBtn : public win_impl
{
	bool m_bVert, m_bMiceOver, m_bDown;
public:
	CPaneScrollBtn( bool bDir )
	{
		m_bVert = bDir;
		m_bMiceOver = false;
		m_bDown = false;
	}
	long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );
	long handle_reflect_message( MSG *pmsg, long *plProcessed );
	long on_erasebkgnd( HDC );

	long on_mousemove( const _point &point );
	long	handle_message( UINT m, WPARAM w, LPARAM l );
	long on_mouseleave();
	long on_lbuttondown( const _point &point );
	long on_lbuttonup( const _point &point );
};

class CPaneTitle : public win_impl
{
	bool m_bMiceOver;
	HICON m_hIcon;

public:
	CPaneTitle();
	~CPaneTitle();
	long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );
	long handle_reflect_message( MSG *pmsg, long *plProcessed );
	long on_mousemove( const _point &point );
	long	handle_message( UINT m, WPARAM w, LPARAM l );
	long on_mouseleave();
	long on_lbuttonup( const _point &point );
	void set_icon( HICON hIcon ) { m_hIcon = hIcon; }
};

class CPaneBar;
class CPane : public win_impl
{
	friend class CPaneBar;
	CPaneTitle m_TitleBar;
	CPaneItemPlace m_ItemPlace;

	CPaneScrollBtn m_btnUp;
	CPaneScrollBtn m_btnDown;
	CString m_strFile;
	COLORREF m_cl_back_color;

	CString m_str_pane;

public:
	void set_name( CString sname ) { m_str_pane = sname; }
	CString get_name() { return m_str_pane; }

	RECT get_virtual_rect();
	RECT get_virtual_rect_lite();

	CPane();
	~CPane();
	virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
	virtual long on_size( short cx, short cy, ulong fSizeType );
	virtual long handle_message( UINT m, WPARAM w, LPARAM l );
	long on_erasebkgnd( HDC );
	void open_pane( bool bOpen );

	CPaneItemPlace *get_item_place() { return &m_ItemPlace; }
	CPaneItem *get_item( long lIndex ) { return m_ItemPlace.get_item( lIndex ); }
	CPaneItem* get_item_by_pos( POINT ptScreen ) { return m_ItemPlace.get_item_by_pos( ptScreen ); }

//	CPane* get_subpane_by_pos( POINT ptScreen );
//	CPaneBar* get_bar_by_pos( POINT ptScreen );

	void on_context_help();

	long get_item_count( ) { return m_ItemPlace.get_item_count(); }
	void   remove_item( long lIndex )  { return m_ItemPlace.remove_item( lIndex ); } 

	bool is_open() { return bool( m_ItemPlace.handle() != 0 ); }

	void store_content( INamedData *punkND, CString strRoot );
	void load_content( INamedData *punkND , CString strRoot );

	void add_item( IActionInfo *punkActionInfo, POINT pt ) { m_ItemPlace.add_item( punkActionInfo, pt ); }
	void clear_items() { m_ItemPlace.clear_items(); };
	bool open_mtd_pane( CString str_name, bool bopen );

	void setDragDrop(IActionInfo *sptrActionInfoMtb);
};

// CPaneBar
class CPaneBar : public win_impl
{
	UINT			m_nLastPaneID;
	CString			m_strClassName;
	bool m_block_send;
public:
	static HICON m_hicon_group_open;
	CPaneBar();
	virtual ~CPaneBar();

	long on_size( short cx, short cy, ulong fSizeType );
	long on_erasebkgnd( HDC );
	long handle_message( UINT m, WPARAM w, LPARAM l );

	LRESULT help_hit_test( WPARAM wParam, LPARAM lParam );

	CPane* get_item( long lIndex );
	CPane* get_item_by_pos( POINT ptScreen );
	void   remove_item( long lIndex );

	CPaneBar* get_bar_by_pos( POINT ptScreen );
	
	CPaneItemPlace *get_pane_place_from_handle( HWND hwnd );

	long get_item_count( ) { return m_PaneList.count(); }

	CPane*	AddNewPane( CString strName, long lInsertAfterIndex = -1 );
	
	void store_content( INamedData *punkND, CString str_root  );
	void load_content( INamedData *punkND, CString str_root );

	void clear_items();
	RECT get_virtual_rect();
	bool open_mtd_pane( CString str_name, bool bopen );

	BOOL bLoadContent;

protected: 
	void _update_scroll_info(RECT rc);
	// Datas

	UINT _GetNewPaneID( ) { return ++m_nLastPaneID; }
	
	static void _pane_cleaner( CPane *pData )
	{
		delete pData;
	}
private:
	_list_t<CPane*, &CPaneBar::_pane_cleaner> m_PaneList;
};

}
}