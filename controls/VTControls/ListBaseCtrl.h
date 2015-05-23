#if !defined(AFX_VTLISTCTRL_H__C4D83785_D42F_11D3_80E4_0000E8DF68C3__INCLUDED_)
#define AFX_VTLISTCTRL_H__C4D83785_D42F_11D3_80E4_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBaseCtrl.h : header file
//
#include "BaseColumn.h"


class CLVEdit : public CEdit
{
// Construction
public:
	CLVEdit() :m_x(0),m_y(0) {}

public:
	void BuildAppearance( COLORREF clrText, COLORREF clBack );

protected:
	CBrush m_brushCtrlEditBackColor;	
	COLORREF m_clText, m_clBack;

// Attributes
public:
	int m_x;
	int m_y;
	int m_width;
	int m_height;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLVEdit)
	BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM Param,
          LRESULT* pLResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLVEdit() {};

	// Generated message map functions
protected:
	//{{AFX_MSG(CLVEdit)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);   
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#include "\vt5\awin\misc_list.h"
class CHeaderMan
{
	_list_t<CHeaderCtrl*> m_list;
public:

	void ClearItems()
	{
		m_list.clear();
	}

	TPOS GetFirstPos() { return m_list.head(); }
	TPOS GetNext(TPOS lPos, CHeaderCtrl **pCtrl)
	{ 
		*pCtrl = m_list.get( lPos );
		return m_list.next( lPos ); 
	}

	void AddItem( CHeaderCtrl *pItem )
	{
		m_list.add_tail( pItem );
	}

	bool Find( CHeaderCtrl *pItem )
	{
		for (TPOS lPos = m_list.head(); lPos; lPos = m_list.next(lPos))
		{
			CHeaderCtrl *pCtrl = m_list.get( lPos );

			if( pCtrl == pItem )
			{
				return true;
			}
		}
		return false;
	}
};

// [vanek] BT2000: 3542 - 26.12.2003
// CListHeaderCtrl
#define LEFT_BORDER ( 1 << 0 ) 
#define TOP_BORDER ( 1 << 1 ) 
#define RIGHT_BORDER ( 1 << 2 ) 
#define BOTTOM_BORDER ( 1 << 3 ) 

#define ALL_BORDER ( LEFT_BORDER | TOP_BORDER | RIGHT_BORDER | BOTTOM_BORDER ) 

struct HEADER_DRAW_PROP
{
	char szText[MAX_PATH];
	short iGrCount;
	short iGrIndex;
	HWND hHdr;
	short iItem;
};

class CListHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CListHeaderCtrl)

	friend class CListBaseCtrl;

	static bool m_bLock;
	bool m_bUseExt;
public:
	CListHeaderCtrl( );
	virtual ~CListHeaderCtrl();

	CHeaderMan *m_pMan;
protected:
	//{{AFX_MSG(CListHeaderCtrl)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnHdnItemchanging(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
    CListBaseCtrl *m_plist;

protected:
	CPoint s_xy;
protected:
	void _apply_header_resize( HANDLE dwFrom, int iItem, int nWidth );
	BOOL _is_resizing_columns( HANDLE dwFrom, int iItem, int nDir );
public:
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
public:
	afx_msg BOOL OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


/////////////////////////////////////////////////////////////////////////////
// CListBaseCtrl window
class CVTList;

#define ID_COLOR_PICKER	100
#define ID_COMBO_PICKER	101
#define ID_SHADING_PICKER 102

#include "ColourPicker.h"
#include "ShadingPicker.h"

class CListBaseCtrl : public CListCtrl
{
	friend class CBaseColumn;
	friend class CListHeaderCtrl;

	// Attributes
public:
	//CString m_strTipText;

    // Operations
public:
    int CellRectFromPoint(CPoint &point, RECT *cellrect, int *col) const;
	//------------------------------------

// Construction
	DECLARE_SERIAL(CListBaseCtrl)
public:
	CListBaseCtrl();
	BOOL Create(CVTList* pParent);
protected:
	//Delete data 
	void DeleteDataContext();
	//Dlete col&row in listCtrl
	void DeleteListCtrlContext();


	void FireCurentColumnChange(short nColumn);
	void FireCurentRowChange(short nRow);
	void FireDataChange(short nColumn, short nRow);
	CColourPicker m_ColourPicker;

	void SetColorPickerToCell( short nColumn, short nRow );
	void SetValuePickerToCell( short nColumn, short nRow );

	void HideColorPicer();
	void PaintColorPicer();

	CShadingPicker m_ShadingPicker;

	void SetShadingPickerToCell( short nColumn, short nRow );
	//void SetValuePickerToCell( short nColumn, short nRow );

	void HideShadingPicker();
	void PaintShadingPicker();

	CComboBox m_Combo;
//Operations:
public:
	//Add column
	void AddColumn( CString strCaption, datatype_e datatype, int nWidth, DWORD dwCaptionAlign );

	//Add row
	void AddRow( int nRow = -1 );	
	//Get cell object from row,col
	CBaseCell* GetCell(int iRow, int iCol);
	//Get column type
	datatype_e GetColumnType( int iCol );

// Attributes
protected:

	//For prevent serialization Exception
	BOOL m_bWasCreated;
public:
	BOOL IsWasCreated(){
		return m_bWasCreated;
	}

protected:

	

	BOOL m_b_in_editMode;
	//COleControl parent window
	CVTList* m_pParent;

	//Columns array
	CTypedPtrArray <CObArray, CBaseColumn*> m_columns;

	//Curent row, ol
	int m_nCurRow, m_nCurCol;

	//Count of rows, col's
	int m_nRowCount, m_nColCount;

	//For determine col index
	int HitTestEx(CPoint &point, int *col);	
	
	//Get & Invalidate cell func
	BOOL GetCellRect(int iRow, int iCol, int nArea, CRect &rect);
	void InvalidateCell(int iRow, int iCol);

	//Edit control for editing labels
	CLVEdit	m_LVEdit;

public:

	void ResetDefaults();

	//Serialization
	virtual void Serialize(CArchive& ar);

	//Build after serialization
	BOOL BuildControl();

protected:
	
	//Go to edit cell value
	BOOL DoEdit();

	//Max cell height
	int GetMaxCellHeight();
	//Send when cell font change
public:
	void ReMeasureItem();

protected:

// Operations
public:
//properties
	BOOL UseGrid();
	BOOL UseFullRowSelect();
	BOOL IsValidColumnIndex(short nColumn);

	// [vanek] BT2000: 3542 - 26.12.2003
	BOOL  IsEnableResizeColumns( );

	OLE_COLOR GetColumnBackColor(short nColumn);
	OLE_COLOR GetColumnForeColor(short nColumn);
	CString GetColumnCaption(short nColumn);
	//CFont* GetColumnFont(short nColumn);
	short _GetColumnType(short nColumn);
	short GetColumnAlign(short nColumn);
	short GetColumnWidth(short nColumn);
	short GetColumnCount();
	
	void SetColumnAlign(short nColumn, short nAlign);
	void SetColumnBackColor(short nColumn, OLE_COLOR color);
	void SetColumnForeColor(short nColumn, OLE_COLOR color);
	void SetColumnType(short nColumn, short nType);
	void SetColumnWidth(short nColumn, short nWidth);
	void SetColumnCaption(short nColumn, CString strCaption);
	//void SetColumnFont(short nColumn, CFont* pFont);

	void _AddColumn();
	void _DeleteColumn( short nColumn );

	BOOL GetDisplayLogicalAsText(short nColumn);
	void SetDisplayLogicalAsText(short nColumn, BOOL bValue);

	short GetCurentColumn(){
		return (short)m_nCurCol;
	}
	short GetRowCount(){		
		return (short) m_nRowCount;
	}

	short GetCurentRow(){
		return (short)m_nCurRow;
	}
	short GetColCount(){		
		return (short) m_nColCount;
	}

	BOOL IsColumnUseIcon(short nColumn);
	void SetColumnUseIcon(short nColumn, BOOL bValue);

	void DeleteRow(short nRow);

	LPLOGFONT GetFont( short nColumn );
	void SetFont( short nColumn, LPLOGFONT lpLF );

	short CurColumnMove(short nDirection);

	void SetColumnReadOnly(short nColumn, BOOL bValue);
	BOOL IsColumnReadOnly(short nColumn); 

	void SetCurentRow(short nRow);
	void SetCurentCol(short nCol);

	void SetColumnLogicalIcon( short nColumn, CString &strFileName, BOOL b_forValue );

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	OLE_COLOR	m_clrEditTextColor, m_clrEditBackColor;
	OLE_COLOR	m_clrActiveCellBackColor, m_clrActiveCellTextColor;

public:
	OLE_COLOR GetEditBackColor();
	void SetEditBackColor(OLE_COLOR nNewValue);
	OLE_COLOR GetEditTextColor();
	void SetEditTextColor(OLE_COLOR nNewValue);
	OLE_COLOR GetActiveCellBackColor();
	void SetActiveCellBackColor(OLE_COLOR nNewValue);
	OLE_COLOR GetActiveCellTextColor();
	void SetActiveCellTextColor(OLE_COLOR nNewValue);
	void ForceEditCell( short nColumn, short nRow );

	OLE_COLOR GetCellBackColor(short nColumn, short nRow );
	OLE_COLOR GetCellForeColor(short nColumn, short nRow );
	void SetCellBackColor(short nColumn, short nRow, OLE_COLOR color);
	void SetCellForeColor(short nColumn, short nRow, OLE_COLOR color);

	OLE_COLOR GetListBkColor();
	void SetListBkColor(OLE_COLOR nNewValue);	

protected:
	OLE_COLOR	m_clrBack;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBaseCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	//-------- for tooltip --------------------
    virtual void PreSubclassWindow();
	//-----------------------------------------
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBaseCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBaseCtrl)
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	
	afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	afx_msg LRESULT OnSelEndOK(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnCloseupCombo();
	//}}AFX_MSG

	//---------  For tolltip -------------
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	//------------------------------------

	DECLARE_MESSAGE_MAP()

public:
	CImageList m_imageList;
	CListHeaderCtrl m_headerCtrl;  // [vanek] BT2000: 3542 - 26.12.2003

	BOOL b_tooltip;
	CPoint xy_tooltip;
protected:


	BOOL get_rects_near_real_cells( int iRow, int iCol, int nArea, LPRECT lpRectLeftCell, LPRECT lpRectRightCell );
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	BOOL bForceEdit;
	CString oldNameEdit;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTLISTCTRL_H__C4D83785_D42F_11D3_80E4_0000E8DF68C3__INCLUDED_)





