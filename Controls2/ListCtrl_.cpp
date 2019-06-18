#include "StdAfx.h"
#include "listctrl_.h"
#include "vlistctrl.h"


CListCtrl_::CListCtrl_(void)
{
}

CListCtrl_::~CListCtrl_(void)
{
}

BOOL	CListCtrl_::delete_cells_info( int iRow, int iColumn )
{
	if( !m_hwnd )
		return FALSE;
	
	int iEndRow = iRow, iEndColumn = iColumn; 
	if( iRow < 0 )
	{
		iRow = 0;
		iEndRow = GetRowCount( ) - 1;
	}
	if( iColumn < 0 )
	{
		iColumn = 0;
		iEndColumn = GetColumnCount( ) - 1;
	}

	for( ; iRow <= iEndRow; iRow ++  )
	{
		for( int iCurrCol = iColumn; iCurrCol <= iEndColumn; iCurrCol ++ )
		{
			long lColPos = 0, lRowPos = 0;
			lColPos = m_Columns[iCurrCol];
			lRowPos = m_Rows[iRow];

			if( !lColPos || !lRowPos )
				continue;

			ListCellAdditionInfo *pLCAInfo = 0;
			long lPos = find_cell( iRow, iCurrCol );
			if( lPos )
			{
				pLCAInfo = (ListCellAdditionInfo *) m_Cells.get( lPos ).lpInfo;
				if( pLCAInfo )
				{
					delete pLCAInfo;
					pLCAInfo = 0;
				}

				m_Cells.remove( lPos );
			}

		}
	}
	return TRUE;
}

// return cells position
long	CListCtrl_::find_cell( int iRow, int iColumn )
{
	long 	lColPos = m_Columns[iColumn],
			lRowPos = m_Rows[iRow];

	if( lColPos && lRowPos )
	{
     	for( long lPos = m_Cells.head( ); lPos; lPos = m_Cells.next( lPos ) )
		{
			if( (m_Cells.get( lPos ).lColumnPos == lColPos) && (m_Cells.get( lPos ).lRowPos == lRowPos) )
				return lPos;
		}
	}
	
    return 0;     
}

bool	CListCtrl_::create_ex( DWORD style, const RECT &rect, const _char *pszTitle /*= 0*/, HWND parent /*= 0*/, HMENU hmenu /*= 0*/, const _char *pszClass /*= 0*/, DWORD ex_style /*= 0*/ )
{
	style |= ( WS_CHILDWINDOW | WS_CLIPSIBLINGS | LVS_ALIGNLEFT | LVS_REPORT );
	style &= ~LVS_OWNERDATA;
	style &= ~WS_CLIPCHILDREN;	// удаление стиля WS_CLIPCHILDREN, т.к. данный стиль приводит к ситуации, когда заголовок не перерисовывается
	ex_style |= ( WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY );
	return win_impl::create_ex( style, rect, pszTitle, parent, hmenu, WC_LISTVIEW, ex_style );
}

// работа со столбцами
int	CListCtrl_::AddColumn( int iColumn )
{
	if( !m_hwnd || !iColumn )
		return -1;

	LVCOLUMN lvC = {0};
	lvC.mask = LVCF_FMT | LVCF_WIDTH;
	lvC.fmt = LVCFMT_LEFT;
	lvC.cx = 100;
    return ListView_InsertColumn( m_hwnd, iColumn, &lvC ); // возвращаем индекс нового столбца
}

BOOL	CListCtrl_::DeleteColumn( int iColumn )
{
	if( !m_hwnd || !iColumn )
		return false;

	return ListView_DeleteColumn( m_hwnd, iColumn );
}

// работа со строками
int	CListCtrl_::AddRow( int iRow )
{
	if( !m_hwnd )
		return -1;
	
	LVITEM stItem = {0};
	stItem.iItem = iRow;
	return ListView_InsertItem( m_hwnd, &stItem );
}

BOOL	CListCtrl_::DeleteRow( int iRow )
{
	if( !handle( ) )
		return false;

	return ListView_DeleteItem( handle(), iRow );
}

BOOL	CListCtrl_::SetRowCount( int iCount, DWORD dwOption /*= 0*/ )
{
	if( !handle() || (iCount < 0) )
		return FALSE;

	// установка нового количества строк
	ListView_SetItemCount( m_hwnd, iCount );

	int iCurrRowCount = 0,
		iNeedAdd = 0;

	if( -1 == ( iCurrRowCount = GetRowCount( ) ) )
		return FALSE;

	if( iCount == 0 )
		return ListView_DeleteAllItems( handle() );	// delete all rows
	
	iNeedAdd = iCount - iCurrRowCount;
	if( iNeedAdd > 0 )
	{	// adding iNeedAdd rows
    	LVITEM stItem = {0};		
		for( int iIndex = iCurrRowCount; iIndex < iCount; iIndex ++ )
		{
			stItem.iItem = iIndex;
			if( ListView_InsertItem( m_hwnd, &stItem ) == -1 )
				return FALSE;		
		}
	}
	else
	{	// deleting iNeedAdd rows
		for( int iIndex = (iCurrRowCount - 1); iIndex >= iCount; iIndex -- )
		{
			if( !ListView_DeleteItem( m_hwnd, iIndex ) )
				return FALSE;
		}
	}

    // сброс выделенного диаппазона
	ResetSelection( );
	return TRUE;
}

// работа с параметрами ячейки
BOOL	CListCtrl_::SetCellProp( int iRow, int iColumn, const ListCellInfo *pLCInfo )
{
	if( !handle( ) || !pLCInfo )
		return FALSE;

	long	lColPos = m_Columns[ iColumn ], 
			lRowPos = m_Rows[ iRow ];
	
    if( !lColPos || !lRowPos )
		return FALSE;

	//BOOL	bFind = FALSE;
	ListCellAdditionInfo *pLCAInfo = 0;
    long lPos = find_cell( iRow, iColumn );
	if( lPos )
		pLCAInfo = (ListCellAdditionInfo *) m_Cells.get( lPos ).lpInfo;
	else
	{
		XCell stCell;
		stCell.lColumnPos = lColPos;
		stCell.lRowPos = lRowPos;
		pLCAInfo = new ListCellAdditionInfo;
		stCell.lpInfo = (long) pLCAInfo;
		m_Cells.add_tail( stCell );
	}

	pLCAInfo->SetInfoByMask( pLCInfo );
    pLCAInfo = 0;
	
	return TRUE;
}

BOOL	CListCtrl_::GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo )
{
	if( !handle( ) || !pLCInfo )
		return FALSE;

	/*LVITEM stItem = {0};
	stItem.iItem = iRow;
	stItem.iSubItem = iColumn;
	stItem.mask = pLCInfo->uiMask | LVIF_PARAM;
    if( ListView_GetItem( m_hwnd, &stItem ) )
		return FALSE;*/

	long lPos = find_cell( iRow, iColumn );
	
	//
	//long	lRow = m_Rows.get( m_Rows[iRow] ),
	//		lColumn = m_Columns.get( m_Columns[iColumn] );
	//

	// копирование значений общих параметров параметров 
	ListCellAdditionInfo *pstLAInfo = 0;
    //pstLAInfo = (ListCellAdditionInfo *) stItem.lParam;
	if( lPos )
		pstLAInfo = (ListCellAdditionInfo *) m_Cells.get( lPos ).lpInfo;
	
	BOOL bRetVal = FALSE;
	bRetVal = __super::GetCellProp( iRow, iColumn, pLCInfo );
	if( pstLAInfo )
	{
		//EXCHANGE_CELL_ADDITIONAL_INFO_BY_MASK( pLCInfo->uiMask, pstLAInfo, pLCInfo );
		//pLCInfo->SetInfoByMask( pstLAInfo );
		pstLAInfo->ListGeneralInfo::GetInfoByMask( pLCInfo );
        bRetVal = TRUE;
	}
	
	//bRetVal = __super::GetCellProp( iRow, iColumn, pLCInfo );
	//return __super::GetCellProp( iRow, iColumn, pLCInfo );
	

	// получение специфических параметров
		// пока отсутствуют...


  /*
	// Получаем все, кроме текста, из свойств столбца
	if( !__super::GetCellProp( iRow, iColumn, pLCInfo ) )
		return FALSE;

	// берем текст, если надо
	if( ( pLCInfo->uiMask & LSTINF_TEXT ) == LSTINF_TEXT )
	{
		LVITEM lvitem = {0};
		lvitem.iItem = iRow;
		lvitem.iSubItem = iColumn;
		lvitem.mask = LVIF_TEXT;
		if( !ListView_GetItem( handle(), &lvitem ) )
			return FALSE;
		pLCInfo->text_copy( lvitem.pszText );
	}*/

	return bRetVal;
	//return TRUE;
}

long	CListCtrl_::on_destroy( )
{
	int iRowCount = 0,
		iColCount = 0;
	iRowCount = GetRowCount( );
	iColCount = GetColumnCount( );
	for( int iRow = 0; iRow < iRowCount; iRow ++ )
		for( int iColumn = 0; iColumn < iColCount; iColumn ++  )
			delete_cells_info( iRow, iColumn );

	m_Rows.clear( );
	m_Columns.clear( );
	m_Cells.clear( );

	return CVListCtrl::on_destroy( );
}

long	CListCtrl_::OnInsertColumn( int iCol, const LPLVCOLUMN pcol )
{
	long lRes = __super::OnInsertColumn( iCol, pcol );
	
	/*if( !clearing_cells_lparam( -1, lRes ) )
	{
			ListView_DeleteColumn( m_hwnd, lRes );
			lRes = -1;
	}  */
	
	if( lRes != -1 )
	{
		long lPos = 0;
		lPos = m_Columns[ lRes ];
		if( lPos )
			m_Columns.insert_before( lRes, lPos );
		else
			m_Columns.add_tail( lRes );
	}

	return lRes;
}

long	CListCtrl_::OnDeleteColumn(	int iCol )
{
	if( iCol >= 0 )
	{
		delete_cells_info( -1, iCol );
		long lPos = 0;
		lPos = m_Columns[ iCol ];
		if( lPos )
			m_Columns.remove( lPos );
	}
	return __super::OnDeleteColumn( iCol );
}

long	CListCtrl_::OnInsertRow( const LPLVITEM prow )
{
	long lRes = __super::OnInsertRow( prow );
	if( lRes != -1 )
	{
		/*if( !clearing_cells_lparam( lRes, -1 ) )
		{
			ListView_DeleteItem( m_hwnd, lRes);
			lRes = -1;
		}*/
		long lPos = 0;
		lPos = m_Rows[ lRes ];
        if( lPos )
			m_Rows.insert_before( lRes, lPos );
		else
            m_Rows.add_tail( lRes );
	}

	return lRes;
}

long	CListCtrl_::OnDeleteRow( int iRow )
{
	if( iRow >= 0 )
	{
		delete_cells_info( iRow, -1 );
		long lPos = 0;
		lPos = m_Rows[ iRow ];
		if( lPos )
			m_Rows.remove( lPos );
	}
	return __super::OnDeleteRow( iRow );
}

long	CListCtrl_::OnDeleteAllRows( )
{
	delete_cells_info( -1, -1 );
	m_Rows.clear();
	return __super::OnDeleteAllRows( );
}