#pragma once

#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include "thumbnail.h"
#include "context_listener.h"
#include "StorageObj_int.h"

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

class image_info
{
public:
	image_info(){
		ptr_image = 0;
		sptr_storage = 0;
		nstorage_record = -1;
		m_nnumber = -1;
		m_str_short_name = _T("");
		m_licon_index = 0;
	}
	virtual ~image_info(){}

	IUnknownPtr			ptr_image;

	IStorageObjectPtr	sptr_storage;
	int					nstorage_record;
	long				m_licon_index;	// [vanek]: icon index for image in storage - 09.04.2004

	// for sort    
	CString				m_str_short_name;
	int					m_nnumber;
};

struct item_draw_info
{
	long		lidx;
	image_info*	pi;
	CRect		rc_wnd_full;
	CRect		rc_wnd_image;
};


inline long compare_func( long key1, long lkey2 )
{	return (long)key1-(long)key1;	}

inline void free_func( void* pdata )
{	delete (image_info*)pdata;		}


/////////////////////////////////////////////////////////////////////////////
// image_bar
class image_bar :	public CWnd,
					public context_listener,
					public CWindowImpl,
					public CDockWindowImpl,
					public CRootedObjectImpl,
					public CEventListenerImpl,
					public CNamedObjectImpl,
					public CMsgListenerImpl,
					public CHelpInfoImpl
					
{
	DECLARE_DYNCREATE(image_bar)
	GUARD_DECLARE_OLECREATE(image_bar)
	ENABLE_MULTYINTERFACE();

	CScrollBar *m_phrzScoll;
	CScrollBar *m_pvrtScoll;
public:
	image_bar();
	virtual ~image_bar();

	virtual void	OnFinalRelease();

protected:
	virtual void	_ReadParams();

protected:
	DECLARE_MESSAGE_MAP()	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	virtual CScrollBar* GetScrollBarCtrl(int nType) const
	{
		if( nType == SB_HORZ )
			return m_phrzScoll;
		else if( nType == SB_VERT )
			return m_pvrtScoll;
		return __super::GetScrollBarCtrl( nType ); 
	}


protected:
	virtual void	register_on_controller( IUnknown* punk_ctrl );
	virtual void	unregister_on_controller( IUnknown* punk_ctrl );

	virtual CWnd*	GetWindow();
	virtual void	OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

public:
	afx_msg void	OnNcDestroy();

protected:
	virtual bool	_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view );
	virtual bool	_on_close_document( IUnknown* punk_doc );
	virtual bool	_on_destroy_view( IUnknown* punk_view );
	virtual bool	_on_data_context_change( IUnknown* punk_view, IUnknown* punk_obj, NotifyCodes nc );

	_list_map_t<long, CString, cmp_cstring>		m_map_image_names_in_doc;
	_list_t<image_info*, free_func>	m_list_image;
	
	//synchronize
	bool			sync_info( );
	bool			sync_activity( );
	bool			destroy_info( bool bset_client_size = true );

	//appearance
	long			m_nitem_width;
	long			m_nitem_height;
	CRect			m_rect_offset; // отступы от краев прямоугольника до картинки
	CRect			m_rect_offset_with_text; // для случая с текстом
	CRect			m_rect_offset_without_text; // для случая без текста
	long			m_nfooter_offset; // расстояние от image до надписи

	CPoint			m_ptBarOffset; // сдвиг всего бара относительно (0,0)

	COLORREF		m_clBackColor;
	COLORREF		m_clHighlightColor;
	BOOL			m_bUseSysColors;
	COLORREF        GetBackColor() {return m_bUseSysColors?::GetSysColor(COLOR_BTNFACE):m_clBackColor;}
	COLORREF        GetHighlightColor() {return m_bUseSysColors?::GetSysColor(COLOR_APPWORKSPACE):m_clHighlightColor;}

	bool			m_bDrawText;
	bool			m_bDrawIcon;

	CFont			m_font;

	//active item
	long			m_nactive_item;
	long			get_item_from_point( CPoint pt_client );

	//item
	long			get_item_count();
	CRect			get_item_rect( long nitem, bool bfull );	
	image_info*		get_image_info_by_idx( long lidx );

	//convertion
	CRect			convert_from_client_to_wnd( CRect rc );
	CRect			convert_from_wnd_to_client( CRect rc );
	CPoint			convert_from_client_to_wnd( CPoint pt );
	CPoint			convert_from_wnd_to_client( CPoint pt );

	//scrolls
	bool			set_client_size( );
	CSize			get_client_size( );
	CPoint			get_scroll_pos();
	void			set_scroll_pos( CPoint pt );
	void			show_scrollbars();

	bool			do_draw( CDC* pdc, CRect rc_update, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	bool			draw_item( item_draw_info* pid, CDC* pdc, CRect rc_update, BITMAPINFOHEADER* pbmih, byte* pdibBits );	

	
	//render manager
	IThumbnailManagerPtr	m_ptr_thumb_man;
	
	//[vanek] - 09.04.2004
	CImageList				m_imagelist;
	CString					m_str_icon_prop_name;	// property's name of icon's index
	
	long	get_icon_index( image_info* pii );

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);


	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(image_bar)	
	afx_msg BOOL GetChangeContext();
	afx_msg void SetChangeContext(BOOL bNewValue);
	afx_msg void Refresh();
	afx_msg BSTR GetNameFilter();
	afx_msg void SetNameFilter(LPCTSTR strNewValue);
	afx_msg	BSTR GetObjectInfoByIndex( long lIdx, VARIANT FAR* varStorageName, VARIANT FAR* varRecord );
	afx_msg long GetActiveItem();
	afx_msg void SetActiveItem(long lIdx);
	afx_msg void Invalidate();
	afx_msg long GetItemCount();
	// vanek - 15.10.2003
	afx_msg BOOL GetLockSync();
	afx_msg void SetLockSync(BOOL bNewValue);
	//}}AFX_DISPATCH
	bool	m_bchange_context;
	CString	m_str_name_filter;
public:

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	bool		m_block_sync;
};


