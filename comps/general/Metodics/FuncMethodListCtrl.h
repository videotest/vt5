#pragma once
#include "\vt5\awin\win_controls.h"
#include "method.h"
#include "methodman.h"

enum StepState
{
	essNotInLoop	= 1 << 0,
	essInLoop		= 1 << 1,
	essFirstInLoop	= 1 << 2,	// start loop
	essLastInLoop	= 1 << 3,	// end loop
};


///////////////////////////////////////////////////////////////////////
// Class CFuncMethodListCtrl
class CFuncMethodListCtrl:
	public list_ctrl
{
protected:
	IMethodManPtr	m_sptr_mtd_man;

	IMethodDataPtr	m_sptr_active_mtd;
	long			m_lpos_active_mtd;

	long			m_litem_hot;
	HRGN			m_hrgn_old;

	// drag-n-drop
	BOOL			m_bdragging;
	int				m_nstart_drag_item;
	int				m_ndrag_item;
	HCURSOR			m_hdrag_cursor;
	HCURSOR			m_hnodrag_cursor;
	HCURSOR			m_hbefore_drag_cursor;

	BOOL			m_block_items_operation;
	BOOL			m_block_steps_operation;

	BOOL			m_block_ui; // запрет юзверьского интерфейса - вместо Disable Window

public:
	CFuncMethodListCtrl( );
	~CFuncMethodListCtrl( );
	BOOL DisableUI(BOOL bDisable); // разрешить/запретить UI - вместо EnableWindow (чтобы работала прокрутка, SBT1354)
	// возвращает прошлое значение

public:
	virtual bool	subclass( HWND hwnd, bool f_nccreate = false );
	virtual long	handle_message( UINT m, WPARAM w, LPARAM l );

public:
	int		insert_after_step( long lpos_target, long lpos_step );
	int		insert_before_step( long lpos_target, long lpos_step );
	int		insert_step( long lpos_step, int nitem );
	
	BOOL	delete_step( long lpos_step );

	int		find_step( long lpos_step, int nstart_from = -1 );

	BOOL	get_step_data( CMethodStep *pstep_data, long lpos_step );
	BOOL	get_step_data( CMethodStep *pstep_data, int nitem_step, long *plpos_step = 0 );

	BOOL	set_step_data( CMethodStep *pstep_data, int nitem_step, bool bdont_clear_cache = false );

	BOOL	refresh_data(void);
	void	reset_active_mtd(void);
	void	clear(void);
	void	fill(void);

	void	update_check_states(void);
	void	update_check_state( long lpos_step );
	void	update_check_state( int nitem );

	long	update_active_step(void);
	void	set_active_step( long lpos_step );
	void	set_active_step_item( long lpos_step );
		
	BOOL	is_cached( int nitem_step );

	DWORD	get_step_state( long lpos_step );

	BOOL	get_mtd_man( IUnknown **ppunk_mtd_man );
	BOOL	get_mtd( IUnknown **ppunk_mtd );

	// on/off redraw
	void	set_redraw( BOOL bturn_on ) { send_message( WM_SETREDRAW, (WPARAM)bturn_on, 0); }

	// redraw step
	void	redraw_step( long lpos_step );
		
protected:
	virtual long on_rbuttondown( const _point &point );
	virtual long on_lbuttondown( const _point &point );
	virtual long on_lbuttonup( const _point &point );
	virtual long on_lbuttondblclk( const _point &point );
	virtual long on_rbuttondblclk( const _point &point );
	virtual long on_mousemove( const _point &point );
	virtual long on_destroy();
	virtual long on_keydown( long nVirtKey );
	virtual long on_killfocus( HWND hwndOld );
    	

	virtual long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );
	
	// reflected hanlers
    virtual long	OnCustomDraw( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка CustomDraw		
	virtual	long	OnPrePaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка до отрисовки
	virtual	long	OnPrePaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка до отрисовки ряда
    virtual	long	OnPrePaintCell(  NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка до отрисовки ячейки
	virtual	long	OnPostPaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка после отрисовки
	virtual	long	OnPostPaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка после отрисовки ряда
	virtual	long	OnPostPaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка после отрисовки ячейки

	virtual void	OnDispInfo( NMLVDISPINFO *plvdispinfo, long *plProcessed );
	virtual void	OnItemChanged( NMLISTVIEW *pnmlv, long *plProcessed );
	virtual void	OnBeginDrag( NMLISTVIEW *pnmlv, long *plProcessed );
	//

protected:
	void	set_def_styles(void);
	BOOL	init(void);
	
	int		get_icon_step( long lpos_step );

	BOOL	calc_loop_rect( RECT *ploop_rect );
	void	update_loop( );
	BOOL	draw_loop_frame( HDC hdc );

	void	update_hot_step( LPPOINT ppoint );
	void	reset_hot_step( );

	BOOL	set_next_active_step( long lvkey );

	// drag'n'drop
	BOOL	start_drag( int nstep_item );
	BOOL	end_drag( );
	int		drag_drop_step( int nstep_item, int ndrop_to );

	BOOL	synch_lparams( int nitem_from, int nitem_to );

	BOOL	is_begin_loop_action( BSTR bstrActionName );
	BOOL	is_end_loop_action( BSTR bstrActionName );
    
	BOOL	is_bound_loop( int nitem_step );
};

// CLockerFlag class
class CLockerFlag
{
public:
	CLockerFlag( BOOL *pbFlag ) { m_pbFlag = pbFlag; *m_pbFlag = TRUE; }
	~CLockerFlag()	{ *m_pbFlag = FALSE; }
	BOOL	*m_pbFlag;
};