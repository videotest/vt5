#pragma once
#include "\vt5\awin\win_base.h"
#include "\vt5\awin\misc_string.h"
#include "\vt5\awin\misc_list.h"

// Направление горизонтального движения 
#define	DIR_RIGHT	1
#define DIR_LEFT	2

// флаги общих свойств
#define	LSTINF_BKCOLOR			0x01000		// для работы с цветом фона
#define	LSTINF_FORECOLOR		0x02000		// для работы с цветом текста
#define LSTINF_FONT				0x04000		// для работы со шрифтом
#define	LSTINF_FORMAT			0x08000		// для работы с выравниванием текста
#define	LSTINF_TEXT				0x10000		// для работы с текстом
// флаги свойств для столбца
#define	LSTINF_WIDTH			0x20000		// для работы с шириной столбца
#define LSTINF_VISIBLE			0x40000		// для работы с видимотью столбца
#define	LSTINF_ORDER			HDI_ORDER	// для работы с порядком размещения столбца (слева на право)

// общие свойства (в lParam) 
struct	ListGeneralInfo
{
	UINT			uiMask;			// маска
	char			*sText;			// текст
	int				iszText;		// размер Text
	COLORREF		crBkColor;		// цвет фона		
	COLORREF		crForeColor;	// цвет текста
	LOGFONT			lfFont;			// шрифт
	int				fmt;			// выравнивание текста (см. описание параметра uFormat (text-drawing options) ф-ции DrawText)

private:
	bool			m_bIAllocText;	// флаг выделение памяти под sText

public :
	ListGeneralInfo( ) 
	{
		init( );
	}

	~ListGeneralInfo( )
	{
		if( sText && m_bIAllocText )
		{
			delete[] sText;
			sText = 0;
		}
	}

	ListGeneralInfo( const ListGeneralInfo &stInfo ) 
	{
		init( );
		*this = stInfo;
	}

	void init( )
	{
		uiMask = 0;
		sText = 0;
		iszText = 0;
		crBkColor = 0;
		crForeColor = 0;
		ZeroMemory( &lfFont, sizeof( lfFont ) );
		fmt = 0;
		m_bIAllocText = false;
	}

	void text_copy( char *str )
	{
		if( !str )
			return;
        if( sText && m_bIAllocText )
		{
			delete[] sText;
			sText = 0;
			m_bIAllocText = false;
		}
		if( !sText && !m_bIAllocText )
		{
			iszText = (int)strlen( str ) + 1;
			sText = new char[ iszText ];
			m_bIAllocText = true;
			strcpy( sText, str );
		}
	}


	void	SetInfoByMask( const ListGeneralInfo *psource )
	{
		if( !psource )
			return;
		if( ( psource->uiMask & LSTINF_BKCOLOR ) == LSTINF_BKCOLOR )
			crBkColor = psource->crBkColor;
		if( ( psource->uiMask & LSTINF_FORECOLOR ) == LSTINF_FORECOLOR )
			crForeColor = psource->crForeColor;
		if( ( psource->uiMask & LSTINF_FONT ) == LSTINF_FONT )
			lfFont = psource->lfFont;
		if( ( psource->uiMask & LSTINF_FORMAT ) == LSTINF_FORMAT )
			fmt = psource->fmt;
		if( ( psource->uiMask & LSTINF_TEXT ) == LSTINF_TEXT )
			text_copy( psource->sText );
		uiMask |= psource->uiMask;
	}

	void	GetInfoByMask( ListGeneralInfo *preceiver )
	{
		if( !preceiver )
			return;
        if( (uiMask & LSTINF_BKCOLOR) && (preceiver->uiMask & LSTINF_BKCOLOR) )
			preceiver->crBkColor = crBkColor;
		if( (uiMask & LSTINF_FORECOLOR) && (preceiver->uiMask & LSTINF_FORECOLOR) )
			preceiver->crForeColor = crForeColor;
		if( (uiMask & LSTINF_FONT) && (preceiver->uiMask & LSTINF_FONT) )
			preceiver->lfFont = lfFont;
		if( (uiMask & LSTINF_FORMAT) && (preceiver->uiMask & LSTINF_FORMAT) )
			preceiver->fmt = fmt;
		if( (uiMask & LSTINF_TEXT) && (preceiver->uiMask & LSTINF_TEXT) )
			preceiver->text_copy( sText );
 	}

	void operator=( const ListGeneralInfo &stInfo )
	{
		uiMask = stInfo.uiMask;
		crBkColor = stInfo.crBkColor;
		crForeColor = stInfo.crForeColor;
		lfFont = stInfo.lfFont;
		fmt = stInfo.fmt;
		text_copy( stInfo.sText );
    }
};

// Дополнительные свойства столбца (тоже в lParam)
struct ListColumnAdditionInfo: public ListGeneralInfo
{
	int		iWidth;		// ширина столбца
    BOOL	iVisible;	// видимость столбца

	ListColumnAdditionInfo( )
	{
		init( );
	}

	ListColumnAdditionInfo( const ListColumnAdditionInfo &stCAInfo )
	{
		init( );
		*this = stCAInfo;
	}

	void	init( )
	{
		iWidth = 0;
		iVisible = TRUE;
	}

	void operator=( const ListColumnAdditionInfo &stCAInfo )
	{
		ListGeneralInfo::operator =( stCAInfo );
		iWidth = stCAInfo.iWidth;
		iVisible = stCAInfo.iVisible;
	}

	void	SetInfoByMask( const ListColumnAdditionInfo *psource )
	{
		if( !psource )
			return;

		ListGeneralInfo::SetInfoByMask( psource );
		if( ( uiMask & LSTINF_WIDTH ) == LSTINF_WIDTH )
			iWidth = psource->iWidth;
		if( ( uiMask & LSTINF_VISIBLE ) == LSTINF_VISIBLE )
			iVisible = psource->iVisible;
	}

	void	GetInfoByMask( ListColumnAdditionInfo *preceiver )
	{
		if( !preceiver )
			return;

		ListGeneralInfo::GetInfoByMask( preceiver );
		if( ( uiMask & LSTINF_WIDTH ) && (preceiver->uiMask & LSTINF_WIDTH)  )
			preceiver->iWidth = iWidth;
		if( ( uiMask & LSTINF_VISIBLE ) && (preceiver->uiMask & LSTINF_VISIBLE) )
			preceiver->iVisible = iVisible;
	}

};
	
// Свойства столбца
struct ListColumnInfo : public ListColumnAdditionInfo
{
	//int		iWidth;			// ширина столбца
	int		iOrder;			// положение в списке (слева на право)
	
	ListColumnInfo( )
	{
		init( );
	}
	
	ListColumnInfo( const ListColumnInfo &stCInfo )
	{
		*this = stCInfo;
	}

	void	init( )
	{
		iOrder = 0;
	}

	void operator=( const ListColumnInfo &stCInfo )
	{
		ListColumnAdditionInfo::operator =( stCInfo );
		iWidth = stCInfo.iWidth;
		iOrder = stCInfo.iOrder;
	}
};

// Дополнительные свойства ячейки ( тоже в lParam )
struct ListCellAdditionInfo : public ListGeneralInfo
{
	// пока пусто

	void	SetInfoByMask( const ListCellAdditionInfo *psource )
	{
		if( !psource )
			return;

		ListGeneralInfo::SetInfoByMask( psource );
	}

	void	GetInfoByMask( ListColumnAdditionInfo *preceiver )
	{
		if( !preceiver )
			return;

		ListGeneralInfo::GetInfoByMask( preceiver );
	}
};

// Свойства ячейки (пустая, т.к. пока все параметры ячейки полностью совпадают с общими)
struct	ListCellInfo : public ListCellAdditionInfo
{
};



///////////////////////////////////////////////
// Virtual ListView class
class CVListCtrl :
	public win_impl
{
	// выбранный диаппазон
	struct	xSelectionInfo
	{
		int	iRowBegin;		// номер первой строки
		int	iRowEnd;		// номер последней строки
        int iColumnBegin;	// номер первого столбца
		int	iColumnEnd;		// номер последнего столбца

		enum XState{ xsNoSelected =-2, xsFullRow = -1, xsFullColumn = xsFullRow };

		void	init( )
		{
			iRowBegin = iRowEnd = iColumnBegin = iColumnEnd = xsNoSelected;
		}

		bool	operator==( xSelectionInfo &sel )
		{
			arrange( );
			sel.arrange( );
			return ( ( iRowBegin == sel.iRowBegin ) && ( iRowEnd == sel.iRowEnd ) && ( iColumnBegin == sel.iColumnBegin ) && ( iColumnEnd == sel.iColumnEnd ) ) ? true : false;
		}

		bool	operator!=( xSelectionInfo &sel )
		{
			return !( operator == (sel) );
		}

		void	arrange( )
		{
			if( ( iRowEnd >= 0 ) && ( iRowBegin > iRowEnd ) )
			{
                int iTemp = iRowBegin;
				iRowBegin = iRowEnd;
				iRowEnd = iTemp;
			}
			if( ( iColumnBegin >= 0 ) && ( iColumnBegin > iColumnEnd ) )
			{
				int iTemp = iColumnBegin;
                iColumnBegin = iColumnEnd;
				iColumnEnd = iTemp;
			}
		}

		BOOL	IsFullRowSelected( )
		{
			return ( ( iRowBegin == iRowEnd ) && ( iRowEnd == xsFullRow ) );
		}

		BOOL	IsFullColumnSeleted( )
		{
			return ( (iColumnBegin == iColumnEnd) && (iColumnEnd == xsFullColumn ) );
		}

		BOOL	IsExist( )
		{
			if( iRowBegin == xsNoSelected )
				return FALSE;
			else if( iRowEnd == xsNoSelected )
				return FALSE;
			else if( iColumnBegin == xsNoSelected ) 
				return FALSE;
			else if( iColumnEnd == xsNoSelected )
				return FALSE;
			else
				return TRUE;

		}

		BOOL	InSelection( int iRow, int iColumn )
		{
            if( ( iRow == xsNoSelected ) || ( iColumn == xsNoSelected ) )
					return FALSE; 

			arrange( );

			if( ( iRow <= iRowEnd )  && ( iColumn <= iColumnEnd ) &&
				  ( iRow >= iRowBegin ) && (iColumn >= iColumnBegin ) )
				  return TRUE;
			else if( IsFullRowSelected( ) && ( iColumn <= iColumnEnd ) && ( iColumn >= iColumnBegin ) )
				return TRUE;
			else if( IsFullColumnSeleted( ) && ( iRow <= iRowEnd ) && ( iRow >= iRowBegin ) )
				return TRUE;
			else 
				return FALSE;
		}

		BOOL	Redraw( HWND hwndLV )
		{
			if( !hwndLV )
				return FALSE;
			
			RECT rtRedraw = {0};
			
            if( !ListView_GetSubItemRect( hwndLV, iRowBegin, iColumnBegin, LVIR_BOUNDS, &rtRedraw ) )
				return FALSE;

			if( ( iRowBegin != iColumnBegin ) && ( iRowEnd != iColumnEnd ) )
			{
				int iCachedTop = rtRedraw.top,
					iCachedLeft = rtRedraw.left;

				if( !ListView_GetSubItemRect( hwndLV, iRowEnd, iColumnEnd, LVIR_BOUNDS, &rtRedraw ) )
					return FALSE;
				
				rtRedraw.top = iCachedTop;
				rtRedraw.left = iCachedLeft;
			}

			return ::InvalidateRect( hwndLV, &rtRedraw, FALSE );
		}
	};	

	// ячейка focused
	struct	xFocusedInfo
	{
		int	iRow;		// номер строки
		int	iColumn;	// номер столбца

		enum XState{ xsNoSelected = -2 };

		void	init( )
		{
			/*iRow = -1;
			iColumn = -1;*/
			iRow = xsNoSelected;
			iColumn = xsNoSelected;
		}

		void	reset( )
		{
			init ( );
		}

		bool	operator==( xFocusedInfo &stFocused )
		{
			return ( ( iRow == stFocused.iRow ) && ( iColumn == stFocused.iColumn) ) ? true : false;
		}

		bool	operator!=( xFocusedInfo &stFocused )
		{
			return !(operator ==( stFocused ));
		}
	};

	xSelectionInfo				m_stSelection;	// диаппазон выбранных ячеек
	xFocusedInfo				m_stFocused;	// focused-ячейка

	HFONT						m_hfHeaderFont;	//
	COLORREF					m_crHeaderText; 

	BOOL						m_bDrawGridLines;
	BOOL						m_bEnableDragDrop;

	// params of client aria
	long	m_lWidthClientArea;
	long	m_lHeightClientArea;

	// scrollbars
	int		m_nHScrollState; // HScroll state. -1: automode
	int		m_nVScrollState; // VScroll state. -1: automode
	BOOL	m_bHScrollbarRealHide;	// флаг реального скрытия горизонтального scrollbar
	BOOL	m_bVScrollbarRealHide;	// флаг реального скрытия вертикального scrollbar
    BOOL	m_bLockForeignHSB;		// default - FALSE
	BOOL	m_bLockForeignVSB;		// default - FALSE
	BOOL	m_bUpdateForeignHSB;	// default - TRUE
	BOOL	m_bUpdateForeignVSB;	// default - TRUE

public:
	BOOL	m_bHideScrollbarsOnVisibleAllRows;	// default - FALSE
private:
	enum xDirection{ XD_Forward, XD_Backward };

	// selection
	BOOL	m_bEnableMultiSelection;	// multiselection

	LPARAM	get_lparam( int iItem );	// get value of lParam for Item Headers
	BOOL	need_paint_cell( const RECT *pCellRect );
	BOOL	set_foreign_scrollbar_info( int iBar );
	void	redirect_scrollbar( int iBar, int iCode, unsigned uPos, xDirection dir = XD_Forward ); 
	BOOL	modify_windows_style( DWORD dwRemove, int nStyleOffset, DWORD dwAdd, UINT nFlags );
	BOOL	modify_listview_style_ex( DWORD dwRemove, DWORD dwAdd, UINT nFlags );

	SCROLLINFO m_horz_sb, m_vert_sb;

protected:
	BOOL	CalcNewCellPos( int *piRow, int *piColumn, long lVKey );	// вычисление новой позиции ячейки

public:
	CVListCtrl(void);
	virtual ~CVListCtrl(void);

	void	update_foreign_scrollbars();

	BOOL	GetSysLogFont( LOGFONT *plfFont, int iParam );	// получение параметров системного шрифта

	// создание
	virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
	virtual bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );

	// установка/удаление стилей 
	BOOL	ModifyStyle( DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0 );
	BOOL	ModifyStyleEx( DWORD dwRemove, DWORD dwAdd, BOOL bWorksWithLVExStyles = TRUE, UINT nFlags = 0 );

	// установка/получение цвета фона
	BOOL	SetBackColor( COLORREF clrBk );
	BOOL	GetBackColor( COLORREF *pclrBk );
	
	//  работа с выделенными ячейками
	void	GetSelection( int *piRowBegin, int *piRowEnd, int *piColumnBegin, int *piColumnEnd );	// получение текущего диаппазона выделенных ячеек
	void	SetSelection( int iRowBegin, int iRowEnd, int iColumnBegin, int iColumnEnd );			// установка текущего диаппазона выделенных ячеек
	BOOL	IsSelected( int iRow, int iColumn );													// определение состояния (выбрана/не выбрана) ячейки
	void	ResetSelection( );																		// сброс текущего диаппазона
	BOOL	ExistSelections( );																		// определение существования выбранных ячеек
	
    // работа с focused-ячейкой
	BOOL	IsFocused( int iRow, int iColumn );					// определение состояния (focused/unfocused) ячейки
	void	SetFocused( int iRow, int iColumn );				// установка 
	void	GetFocused( int *piRow, int *piColumn );			// получение
	//void	ResetFocused( );									// сброс
	void	EnsureVisibleFocused( );							// обеспечение видимости ячейки focused, при необходимости выполнение scrolling
	BOOL	ExistFocused( );									// определение существования focused-ячейки
	BOOL	GetFocusedCellProp( ListCellInfo * pLCInfo );		// получение свойств focused-ячейки
	BOOL	SetFocusedCellProp( const ListCellInfo * pLCInfo );	// задание свойств focused-ячейки
	
	// работа со столбцами
	int		GetColumnCount( );												// получение количества столбцов
	BOOL	SetColumnCount( int iCount );									// установка количества столбцов
	BOOL	SetColumnProp( int iColumn, const ListColumnInfo *pLCInfo );	// установка свойств столбца
	BOOL	GetColumnProp( int iColumn, ListColumnInfo *pLCInfo  );			// получение свойств столбца
	int		GetNextVisibleColumn( int iCurrColumn, int iHDirection, 
									BOOL CurrIncl = FALSE );					// получение индекса следующего видимого столбца в зависимости от направления
	
	// работа со строками
	virtual BOOL	SetRowCount( int iCount, DWORD dwOption = 0 /* LVSICF_NOINVALIDATEALL , LVSICF_NOSCROLL*/);	// добавление количества строк, dwOption - only for virtual list
	int				GetRowCount( );			// получение количества строк
	BOOL			IsVisibleAllRow( );		// определение видимости всех строк 
		
	
	// работа с параметрами ячейки
	virtual	BOOL	GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo );		// получение свойств ячейки
	virtual BOOL	SetCellProp( int iRow, int iColumn, const ListCellInfo * pLCInfo );	// задание свойств ячейки

	// установка параметров 
	void	EnableDragDrop( BOOL bEnabled = TRUE );			// разрешение/запрещение функции drag-drop
	void	EnableDrawGridLines( BOOL bEnabled = TRUE );	// разрешение/запрещение отрисовки "сетки"

	// 
	void	RedrawWindow( );
	BOOL	EnsureVisibleCell( int iRow, int iColumn );		// обеспечение видимости ячейки, при необходимости выполнение scrolling

	// scrollbar
	void	EnableShowScrollBars( int iBar, BOOL bEnabled = TRUE );		// разрешение/запрещение видимости собственных scrollbar-ов 
	virtual	HWND	GetScrollbar( int iBar );							// iBar: SB_HORZ, SB_VERT
	BOOL	IsScrollBarCtrl( HWND hwndSB );								
	void	DisableUpdateForeignScrollBar( int iBar, BOOL bDisable = TRUE );
	
	
	
	// 
	virtual bool subclass( HWND hwnd, bool f_nccreate = false );
	
	////////// Firing events
	virtual void OnFocusedChanged( int iRow, int iColumn );
	virtual void OnFocusedLButtonDown( );
		
	///// Header /////
	BOOL	Header_SetFont( LOGFONT *plf );
	BOOL	Header_SetTextColor( COLORREF *pcr );

	virtual LRESULT	OnLButtonClickHeader(LPNMHEADER lpmnhdr);
	virtual	LRESULT	OnRButtonClickHeader(LPNMHEADER lpmnhdr);
	virtual	LRESULT	OnMButtonClickHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT	OnItemChangingHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT	OnItemChangedHeader(LPNMHEADER lpmnhdr);
	virtual	LRESULT	OnBeginTrackHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT	OnEndTrackHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT	OnBeginDragHeader(LPNMHEADER lpmnhdr);
	virtual LRESULT	OnEndDragHeader(LPNMHEADER lpmnhdr);

	// Header Custom Draw
	virtual long	OnCustomDrawHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPrePaintHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPrePaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw);		
    virtual	long	OnPostPaintHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPostPaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw);
	

	//////////////////////////////// обработка сообщений  //////////////////////////////////////////////
	virtual	LRESULT	handle_message(UINT m, WPARAM w, LPARAM l);
	virtual LRESULT	on_create(CREATESTRUCT *pcs);
	virtual	LRESULT	on_keydown(long nVirtKey);
	virtual LRESULT	on_lbuttondown(const _point &point);
	virtual	LRESULT	on_notify(uint idc, NMHDR *pnmhdr);
	virtual LRESULT	on_size(short cx, short cy, ulong fSizeType);
	virtual LRESULT	on_destroy();
	virtual LRESULT	on_hscroll(unsigned code, unsigned pos, HWND hwndScroll);
	virtual LRESULT	on_vscroll(unsigned code, unsigned pos, HWND hwndScroll);

	virtual LRESULT	OnWindowPosChanged(LPWINDOWPOS lpwp);
	virtual	long	OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	virtual	long	OnNcCalcSize( WPARAM w, LPARAM l );
	virtual	LRESULT	OnInsertColumn(int iCol, const LPLVCOLUMN pcol);
	virtual	LRESULT	OnDeleteColumn(int iCol);
	virtual	LRESULT	OnInsertRow(const LPLVITEM prow);
	virtual	long	OnDeleteRow( int iRow );
	virtual	long	OnDeleteAllRows( );

	
    /////////////////////////// обработка reflected message /////////////////////
	virtual long	on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );		// анализ reflected messages

	// LVN_BEGINLABELEDIT
	//virtual long	OnBeginLableEdit( NMLVDISPINFO *pnmDispInfo );			// обработка начала редактирования ячейки
	// LVN_ENDLABELEDIT
	//virtual long	OnEndLableEdit(  NMLVDISPINFO *pnmDispInfo );			// обработка завершения редактирования ячейки
	// LVN_DELETEALLITEMS
	//virtual	long	OnDeleteAllRows( NMLISTVIEW *pnmListView );				// обработка удаления всех строк
	// LVN_DELETEITEM
	//virtual	long	OnDeleteRow( NMLISTVIEW *pnmListView );					// обработка удаления строки
	// LVN_COLUMNCLICK
	//virtual	long	OnColumnClick( NMLISTVIEW *pnmListView );				// обработка щелчка по столбцу
	// LVN_GETDISPINFO
	////virtual	long	OnSetCellText( NMLVDISPINFO *pnmDispInfo );				// обработка установки текста ячейки
	////virtual	long	OnSetCellIndexImage( NMLVDISPINFO *pnmDispInfo );		// обработка установки индекса изображения в ячейке
	////virtual	long	OnSetCellState( NMLVDISPINFO *pnmDispInfo );			// обработка установки сотояния ячейки
	// LVN_ITEMACTIVATE
	//virtual	long	OnCellActivate( NMITEMACTIVATE *pnmItemActivate );		// обработка активизации ячейки
	// LVN_ITEMCHANGED
	//virtual	long	OnCellChanged( NMLISTVIEW *pnmListView );				// обработка изменения параметров ячейки
	// LVN_ITEMCHANGING
	//virtual	long	OnCellChaning( NMLISTVIEW *pnmListView );				// обработка процесса изменения параметров ячейки
	// LVN_MARQUEEBEGIN
	//virtual	long	OnMarqueeBegin( NMLISTVIEW *pnmListView );				// обработка выделения
	// LVN_ODCACHEHINT
	//virtual	long	OnODCacheInt( NMLVCACHEHINT *pnmCacheInt );		
	// LVN_ODSTATECHANGED
	//virtual	long	OnRowsStateChanged(  NMLVODSTATECHANGE *pnmStateChange );	// обработка изменения состояния строк(строки)
	// NM_CLICK
	// virtual	long	OnCellLClick( NMITEMACTIVATE *pnmItemActivate );		// обработка щелчка левой кнопки мыши по ячейке
	// NM_DBLCLIK 
	//virtual	long	OnCellLDbClick( NMITEMACTIVATE *pnmItemActivate );		// обработка двойного щелчка левой кнопки мыши по ячейке
	// NM_RCLICK
	//virtual	long	OnCellRClick( NMITEMACTIVATE *pnmItemActivate );		// обработка щелчка правой кнопки мыши по ячейке
	// NM_RDBLCLICK 
	//virtual	long	OnCellLRDbClick( NMITEMACTIVATE *pnmItemActivate );		// обработка двойного щелчка правой кнопки мыши по ячейке
	// NM_HOVER 
	//virtual	long	OnCellHover( NMHDR *pnmHdr );							// обработка hover для ячейки
	// NM_CUSTOMDRAW 
	virtual long	OnCustomDraw( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка CustomDraw		
	virtual	long	OnPrePaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка до отрисовки
	virtual	long	OnPrePaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка до отрисовки ряда
    virtual	long	OnPrePaintCell(  NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка до отрисовки ячейки
	virtual	long	OnPostPaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// обработка после отрисовки
	virtual	long	OnPostPaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка после отрисовки ряда
	virtual	long	OnPostPaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// обработка после отрисовки ячейки

	virtual	long	OnODCacheInt( int nRowFrom, int	nRowTo );

	// NM_RETURN
	//virtual	long	OnReturn( NMHDR *pnmHdr );								// обработка нажатия на клавишу Enter, когда список имеет фокус ввода
	
};