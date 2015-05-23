#pragma once
#include "vlistctrl.h"
#include "_list_idx_t.h"

class CListCtrl_ :
	public CVListCtrl
{

	struct XCell
	{
		TPOS	lColumnPos;	// позиция столбца в списке столбцов
		TPOS	lRowPos;	// позиция строки в списке строк
		ListCellAdditionInfo*	lpInfo;		// указатель на инфор-ю

		XCell()
		{
			lColumnPos = 0;
			lRowPos = 0;
			lpInfo = 0;
		}
	};

	_list_idx_t<long>	m_Columns;
	_list_idx_t<long>	m_Rows;
	_list_idx_t<XCell>	m_Cells;


	BOOL delete_cells_info( int iRow, int iColumn );		// удаление информации о ячейке(ах): если (-1) - удаление всех ячеек
	TPOS find_cell( int iRow, int iColumn ); 
	
public:
	CListCtrl_(void);
	virtual ~CListCtrl_(void);

	virtual bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );

	// работа со столбцами
	int		AddColumn( int iColumn );		// добавление нового столбца
	BOOL	DeleteColumn( int iColumn );	// удаление столбца
	
	// работа со строками
	int		AddRow( int iRow );								// добавление строк
	BOOL	DeleteRow( int iRow );							// удаление строк
	BOOL	SetRowCount( int iCount, DWORD dwOption = 0 );	// добавление количества строк, dwOption - only for virtual list

	// работа с параметрами ячейки
	virtual BOOL	SetCellProp( int iRow, int iColumn, const ListCellInfo * pLCInfo );	// задание свойств ячейки
	virtual	BOOL	GetCellProp( int iRow, int iColumn, ListCellInfo * pLCInfo );		// получение свойств ячейки

	// обработка сообщений листа
	virtual LRESULT	on_destroy();

	virtual	LRESULT	OnInsertColumn(int iCol, const LPLVCOLUMN pcol);
	virtual	LRESULT	OnDeleteColumn(int iCol);
	virtual	LRESULT	OnInsertRow(const LPLVITEM prow);
	virtual	long	OnDeleteRow( int iRow );
	virtual	long	OnDeleteAllRows( );

};
