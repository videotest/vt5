#pragma once
#include "\vt5\awin\win_base.h"
#include "\vt5\awin\misc_string.h"
#include "\vt5\awin\misc_list.h"

// ����������� ��������������� �������� 
#define	DIR_RIGHT	1
#define DIR_LEFT	2

// ����� ����� �������
#define	LSTINF_BKCOLOR			0x01000		// ��� ������ � ������ ����
#define	LSTINF_FORECOLOR		0x02000		// ��� ������ � ������ ������
#define LSTINF_FONT				0x04000		// ��� ������ �� �������
#define	LSTINF_FORMAT			0x08000		// ��� ������ � ������������� ������
#define	LSTINF_TEXT				0x10000		// ��� ������ � �������
// ����� ������� ��� �������
#define	LSTINF_WIDTH			0x20000		// ��� ������ � ������� �������
#define LSTINF_VISIBLE			0x40000		// ��� ������ � ��������� �������
#define	LSTINF_ORDER			HDI_ORDER	// ��� ������ � �������� ���������� ������� (����� �� �����)

// ����� �������� (� lParam) 
struct	ListGeneralInfo
{
	UINT			uiMask;			// �����
	char			*sText;			// �����
	int				iszText;		// ������ Text
	COLORREF		crBkColor;		// ���� ����		
	COLORREF		crForeColor;	// ���� ������
	LOGFONT			lfFont;			// �����
	int				fmt;			// ������������ ������ (��. �������� ��������� uFormat (text-drawing options) �-��� DrawText)

private:
	bool			m_bIAllocText;	// ���� ��������� ������ ��� sText

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
			iszText = strlen( str ) + 1;
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

// �������������� �������� ������� (���� � lParam)
struct ListColumnAdditionInfo: public ListGeneralInfo
{
	int		iWidth;		// ������ �������
    BOOL	iVisible;	// ��������� �������

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
	
// �������� �������
struct ListColumnInfo : public ListColumnAdditionInfo
{
	//int		iWidth;			// ������ �������
	int		iOrder;			// ��������� � ������ (����� �� �����)
	
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

// �������������� �������� ������ ( ���� � lParam )
struct ListCellAdditionInfo : public ListGeneralInfo
{
	// ���� �����

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

// �������� ������ (������, �.�. ���� ��� ��������� ������ ��������� ��������� � ������)
struct	ListCellInfo : public ListCellAdditionInfo
{
};



///////////////////////////////////////////////
// Virtual ListView class
class CVListCtrl :
	public win_impl
{
	// ��������� ���������
	struct	xSelectionInfo
	{
		int	iRowBegin;		// ����� ������ ������
		int	iRowEnd;		// ����� ��������� ������
        int iColumnBegin;	// ����� ������� �������
		int	iColumnEnd;		// ����� ���������� �������

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

	// ������ focused
	struct	xFocusedInfo
	{
		int	iRow;		// ����� ������
		int	iColumn;	// ����� �������

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

	xSelectionInfo				m_stSelection;	// ��������� ��������� �����
	xFocusedInfo				m_stFocused;	// focused-������

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
	BOOL	m_bHScrollbarRealHide;	// ���� ��������� ������� ��������������� scrollbar
	BOOL	m_bVScrollbarRealHide;	// ���� ��������� ������� ������������� scrollbar
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
	BOOL	CalcNewCellPos( int *piRow, int *piColumn, long lVKey );	// ���������� ����� ������� ������

public:
	CVListCtrl(void);
	virtual ~CVListCtrl(void);

	void	update_foreign_scrollbars();

	BOOL	GetSysLogFont( LOGFONT *plfFont, int iParam );	// ��������� ���������� ���������� ������

	// ��������
	virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
	virtual bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );

	// ���������/�������� ������ 
	BOOL	ModifyStyle( DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0 );
	BOOL	ModifyStyleEx( DWORD dwRemove, DWORD dwAdd, BOOL bWorksWithLVExStyles = TRUE, UINT nFlags = 0 );

	// ���������/��������� ����� ����
	BOOL	SetBackColor( COLORREF clrBk );
	BOOL	GetBackColor( COLORREF *pclrBk );
	
	//  ������ � ����������� ��������
	void	GetSelection( int *piRowBegin, int *piRowEnd, int *piColumnBegin, int *piColumnEnd );	// ��������� �������� ���������� ���������� �����
	void	SetSelection( int iRowBegin, int iRowEnd, int iColumnBegin, int iColumnEnd );			// ��������� �������� ���������� ���������� �����
	BOOL	IsSelected( int iRow, int iColumn );													// ����������� ��������� (�������/�� �������) ������
	void	ResetSelection( );																		// ����� �������� ����������
	BOOL	ExistSelections( );																		// ����������� ������������� ��������� �����
	
    // ������ � focused-�������
	BOOL	IsFocused( int iRow, int iColumn );					// ����������� ��������� (focused/unfocused) ������
	void	SetFocused( int iRow, int iColumn );				// ��������� 
	void	GetFocused( int *piRow, int *piColumn );			// ���������
	//void	ResetFocused( );									// �����
	void	EnsureVisibleFocused( );							// ����������� ��������� ������ focused, ��� ������������� ���������� scrolling
	BOOL	ExistFocused( );									// ����������� ������������� focused-������
	BOOL	GetFocusedCellProp( ListCellInfo * pLCInfo );		// ��������� ������� focused-������
	BOOL	SetFocusedCellProp( const ListCellInfo * pLCInfo );	// ������� ������� focused-������
	
	// ������ �� ���������
	int		GetColumnCount( );												// ��������� ���������� ��������
	BOOL	SetColumnCount( int iCount );									// ��������� ���������� ��������
	BOOL	SetColumnProp( int iColumn, const ListColumnInfo *pLCInfo );	// ��������� ������� �������
	BOOL	GetColumnProp( int iColumn, ListColumnInfo *pLCInfo  );			// ��������� ������� �������
	int		GetNextVisibleColumn( int iCurrColumn, int iHDirection, 
									BOOL CurrIncl = FALSE );					// ��������� ������� ���������� �������� ������� � ����������� �� �����������
	
	// ������ �� ��������
	virtual BOOL	SetRowCount( int iCount, DWORD dwOption = 0 /* LVSICF_NOINVALIDATEALL , LVSICF_NOSCROLL*/);	// ���������� ���������� �����, dwOption - only for virtual list
	int				GetRowCount( );			// ��������� ���������� �����
	BOOL			IsVisibleAllRow( );		// ����������� ��������� ���� ����� 
		
	
	// ������ � ����������� ������
	virtual	BOOL	GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo );		// ��������� ������� ������
	virtual BOOL	SetCellProp( int iRow, int iColumn, const ListCellInfo * pLCInfo );	// ������� ������� ������

	// ��������� ���������� 
	void	EnableDragDrop( BOOL bEnabled = TRUE );			// ����������/���������� ������� drag-drop
	void	EnableDrawGridLines( BOOL bEnabled = TRUE );	// ����������/���������� ��������� "�����"

	// 
	void	RedrawWindow( );
	BOOL	EnsureVisibleCell( int iRow, int iColumn );		// ����������� ��������� ������, ��� ������������� ���������� scrolling

	// scrollbar
	void	EnableShowScrollBars( int iBar, BOOL bEnabled = TRUE );		// ����������/���������� ��������� ����������� scrollbar-�� 
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

	virtual long	OnLButtonClickHeader( LPNMHEADER lpmnhdr );
	virtual	long	OnRButtonClickHeader( LPNMHEADER lpmnhdr );
	virtual	long	OnMButtonClickHeader( LPNMHEADER lpmnhdr );
	virtual long	OnItemChangingHeader( LPNMHEADER lpmnhdr );
	virtual long	OnItemChangedHeader( LPNMHEADER lpmnhdr );
	virtual	long	OnBeginTrackHeader( LPNMHEADER lpmnhdr );
	virtual long	OnEndTrackHeader( LPNMHEADER lpmnhdr );
	virtual long	OnBeginDragHeader( LPNMHEADER lpmnhdr );
	virtual long	OnEndDragHeader( LPNMHEADER lpmnhdr );

	// Header Custom Draw
	virtual long	OnCustomDrawHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPrePaintHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPrePaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw);		
    virtual	long	OnPostPaintHeader( NMCUSTOMDRAW *pnmCustomDraw);			
	virtual	long	OnPostPaintItemHeader( NMCUSTOMDRAW *pnmCustomDraw);
	

	//////////////////////////////// ��������� ���������  //////////////////////////////////////////////
	virtual	long	handle_message( UINT m, WPARAM w, LPARAM l );
	virtual long	on_create( CREATESTRUCT *pcs );
	virtual	long	on_keydown( long nVirtKey );
	virtual long	on_lbuttondown( const _point &point );
	virtual	long	on_notify( uint idc, NMHDR *pnmhdr );
	virtual long	on_size( short cx, short cy, ulong fSizeType );
	virtual long	on_destroy( );
	virtual long	on_hscroll( unsigned code, unsigned pos, HWND hwndScroll );
	virtual long	on_vscroll( unsigned code, unsigned pos, HWND hwndScroll );

	virtual long	OnWindowPosChanged( LPWINDOWPOS lpwp );
	virtual	long	OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	virtual	long	OnNcCalcSize( WPARAM w, LPARAM l );
	virtual	long	OnInsertColumn( int iCol, const LPLVCOLUMN pcol );
	virtual	long	OnDeleteColumn(	int iCol );
	virtual	long	OnInsertRow( const LPLVITEM prow );
	virtual	long	OnDeleteRow( int iRow );
	virtual	long	OnDeleteAllRows( );

	
    /////////////////////////// ��������� reflected message /////////////////////
	virtual long	on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );		// ������ reflected messages

	// LVN_BEGINLABELEDIT
	//virtual long	OnBeginLableEdit( NMLVDISPINFO *pnmDispInfo );			// ��������� ������ �������������� ������
	// LVN_ENDLABELEDIT
	//virtual long	OnEndLableEdit(  NMLVDISPINFO *pnmDispInfo );			// ��������� ���������� �������������� ������
	// LVN_DELETEALLITEMS
	//virtual	long	OnDeleteAllRows( NMLISTVIEW *pnmListView );				// ��������� �������� ���� �����
	// LVN_DELETEITEM
	//virtual	long	OnDeleteRow( NMLISTVIEW *pnmListView );					// ��������� �������� ������
	// LVN_COLUMNCLICK
	//virtual	long	OnColumnClick( NMLISTVIEW *pnmListView );				// ��������� ������ �� �������
	// LVN_GETDISPINFO
	////virtual	long	OnSetCellText( NMLVDISPINFO *pnmDispInfo );				// ��������� ��������� ������ ������
	////virtual	long	OnSetCellIndexImage( NMLVDISPINFO *pnmDispInfo );		// ��������� ��������� ������� ����������� � ������
	////virtual	long	OnSetCellState( NMLVDISPINFO *pnmDispInfo );			// ��������� ��������� �������� ������
	// LVN_ITEMACTIVATE
	//virtual	long	OnCellActivate( NMITEMACTIVATE *pnmItemActivate );		// ��������� ����������� ������
	// LVN_ITEMCHANGED
	//virtual	long	OnCellChanged( NMLISTVIEW *pnmListView );				// ��������� ��������� ���������� ������
	// LVN_ITEMCHANGING
	//virtual	long	OnCellChaning( NMLISTVIEW *pnmListView );				// ��������� �������� ��������� ���������� ������
	// LVN_MARQUEEBEGIN
	//virtual	long	OnMarqueeBegin( NMLISTVIEW *pnmListView );				// ��������� ���������
	// LVN_ODCACHEHINT
	//virtual	long	OnODCacheInt( NMLVCACHEHINT *pnmCacheInt );		
	// LVN_ODSTATECHANGED
	//virtual	long	OnRowsStateChanged(  NMLVODSTATECHANGE *pnmStateChange );	// ��������� ��������� ��������� �����(������)
	// NM_CLICK
	// virtual	long	OnCellLClick( NMITEMACTIVATE *pnmItemActivate );		// ��������� ������ ����� ������ ���� �� ������
	// NM_DBLCLIK 
	//virtual	long	OnCellLDbClick( NMITEMACTIVATE *pnmItemActivate );		// ��������� �������� ������ ����� ������ ���� �� ������
	// NM_RCLICK
	//virtual	long	OnCellRClick( NMITEMACTIVATE *pnmItemActivate );		// ��������� ������ ������ ������ ���� �� ������
	// NM_RDBLCLICK 
	//virtual	long	OnCellLRDbClick( NMITEMACTIVATE *pnmItemActivate );		// ��������� �������� ������ ������ ������ ���� �� ������
	// NM_HOVER 
	//virtual	long	OnCellHover( NMHDR *pnmHdr );							// ��������� hover ��� ������
	// NM_CUSTOMDRAW 
	virtual long	OnCustomDraw( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// ��������� CustomDraw		
	virtual	long	OnPrePaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// ��������� �� ���������
	virtual	long	OnPrePaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// ��������� �� ��������� ����
    virtual	long	OnPrePaintCell(  NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// ��������� �� ��������� ������
	virtual	long	OnPostPaint( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );			// ��������� ����� ���������
	virtual	long	OnPostPaintRow( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// ��������� ����� ��������� ����
	virtual	long	OnPostPaintCell( NMLVCUSTOMDRAW *pnmCustomDraw, long *plProcessed );		// ��������� ����� ��������� ������

	virtual	long	OnODCacheInt( int nRowFrom, int	nRowTo );

	// NM_RETURN
	//virtual	long	OnReturn( NMHDR *pnmHdr );								// ��������� ������� �� ������� Enter, ����� ������ ����� ����� �����
	
};