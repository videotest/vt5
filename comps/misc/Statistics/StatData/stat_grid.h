#pragma once

#include "\vt5\common2\misc_classes.h"
#include "\vt5\controls2\vlistctrl.h"
#include "\vt5\awin\misc_map.h"

#include "statistics.h"
#include "measure5.h"

#include "stat_types.h"
#include "stat_object.h"

class CStatView;


#define EXCHANGE_VISIBLE	(1<<0)
#define EXCHANGE_WIDTH		(1<<1)
#define EXCHANGE_ORDER		(1<<2)

#define EXCHANGE_ALL	EXCHANGE_VISIBLE | EXCHANGE_WIDTH | EXCHANGE_ORDER

typedef vector<stat_col_info> StatColVector;
//////////////////////////////////////////////////////////////////////
//
//	class CStatGrid
//
//////////////////////////////////////////////////////////////////////
class CStatGrid : public CVListCtrl
{
	friend class CStatView;
public:
	CStatGrid();
	virtual ~CStatGrid();

	
	//attach StatTable
	bool				buid( IUnknown* punk_tbl );
	bool				fill_grid( bool bload_from_shell_data);
	
	//message handlers
	virtual LRESULT		on_paint();
	virtual LRESULT		on_contextmenu(const _point &point);
	virtual LRESULT		on_destroy();
	virtual LRESULT		on_rbuttondown(const _point &point);

	virtual LRESULT		OnEndTrackHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT		OnEndDragHeader(LPNMHEADER lpmnhdr);

	//active object
	IStatTablePtr		m_ptr_table;
//	_string				m_str_class_name;


protected:
	//parent
	CStatView*			m_pparent;
public:
	void				SetParent( CStatView* pparent );

protected:
	//message handlers
	virtual void		handle_init();	
	virtual LRESULT		handle_message(UINT m, WPARAM w, LPARAM l);

public:
	//scrollbars
	virtual	HWND		GetScrollbar( int iBar );
protected:
	//overide
	virtual	BOOL		GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo );

	//scroll bars
	HWND				m_hwnd_horz_scroll;
	HWND				m_hwnd_vert_scroll;

	//Serailization
	bool				load_state( bool bload_from_shell_data);
	bool				save_state( long lflags = EXCHANGE_ALL );

	//list of all columns
	StatColVector m_list_col;
public:
	//column cache
	CMapColInfo* m_map_pcol;
protected:
	//row cache	
	_list_map_t<TPOS/*row pos*/, long/*row num*/, cmp_long>			m_map_row;


	//measure params cache	
	map_meas_params		m_map_meas_params;

public:
	stat_col_info*		find_col_info_by_num( long ncol );


	long				get_rows_count( ) const;
	long				get_columns_count( );
//	long				get_row_height( int nrow );
//	long				get_col_width( int ncol );
	CRect				get_cell_rect( int nrow, int ncol );
	CString			get_item_text( int nrow, int ncol );
	
	//Print support
	int GetTableWidth() const;
	int GetTableHeight() const;
	RECT RectHitTest(const POINT& pt);
	POINT RowCol(const POINT& pt);
	virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
	virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, const double& fzoom_view, DWORD dwFlags);

	virtual	long		OnODCacheInt( int nRowFrom, int	nRowTo );
};