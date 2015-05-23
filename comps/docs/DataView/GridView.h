#if !defined(AFX_GRIDVIEW_H__CB3A604D_77A1_4D5B_BBE4_ACA02AAA5F82__INCLUDED_)
#define AFX_GRIDVIEW_H__CB3A604D_77A1_4D5B_BBE4_ACA02AAA5F82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridView.h : header file
//

#include "DocViewBase.h"
#include "ResultBar.h"
#include "VTGridCtrl.h"
#include "Classes5.h"
#include <AfxTempl.h>
#include "DataView.h"
#include "ObListWrp.h"


class CResultBar;
/////////////////////////////////////////////////////////////////////////////
// CGridViewBase command target

class CGridViewBase : public CViewBase/*, public CGridNotifyTarget*/
{
public:
	virtual int	rowMeasureUnit()	const {return 1;}
	virtual int	rowFirstObject()	const {return 2;}
	virtual int	colFirstParameter()	const {return 3;}
	virtual int	colNumber()			const {return 0;}


	DECLARE_DYNAMIC(CGridViewBase)
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO_NULL( )

	friend class CResultBar;
	friend class CVTGridCell;

public:
	CGridViewBase();           // protected constructor used by dynamic creation
	virtual ~CGridViewBase();
//rows - calc objects
	virtual bool AddAllObjects();
	virtual bool RemoveAllObjects();
	virtual long AddObject( ICalcObjectPtr ptrCalc );
	virtual long UpdateObject( ICalcObjectPtr ptrCalc, long row = -1 );
	virtual bool RemoveObject( ICalcObjectPtr ptrCalc );
//cols - params
	virtual bool AddAllParams();
	virtual bool RemoveAllParams();
	virtual bool AddParameter( ParameterContainer *pc );
	virtual bool UpdateParameter( ParameterContainer *pc );
	virtual bool RemoveParameter( ParameterContainer *pc );
	virtual bool DefineParameter( ParameterContainer *pc, long col );
	virtual bool UpdateActiveCell();
	virtual void StoreColumnWidth( long col, long width ){};
	virtual void Clear();
	virtual void FillAll();
	virtual void CalcLayout();
	virtual bool SaveState();
	virtual bool LoadState();

// Operations
public:
	virtual void AttachActiveObjects();
	POSITION GetFisrtVisibleObjectPosition();
	IUnknown *GetNextVisibleObject( POSITION &rPos );


	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual IUnknown *GetObjectByPoint(CPoint pt); 
	virtual void GetMultiFrame(IUnknown **ppunkObject, bool bDragFrame);

//print functions
	virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
	virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	virtual void Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags);

// [vanek]: BT610 - 19.11.2003
//views misc settings 
	virtual	DWORD GetViewFlags() { return 0; }


	void TestInit();

	virtual CScrollBar	*GetScrollBarCtrl(int iSBCode) const
	{	return CViewImpl::GetScrollBarCtrl(iSBCode);	}

	void	SetParamMgrName(LPCTSTR szName)
	{	m_strGroupManagerName = szName;	}
	LPCTSTR	GetParamMgrName()
	{	return m_strGroupManagerName;	}

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridViewBase)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
public:
	bool AlignByDefault(){return true;}
	bool AlignByCell( int nRow, int nCol){return true;};
	virtual COLORREF	GetCellBackColor(int nRow, int nCol);
	virtual CGridCellBase* CreateCell(int nRow, int nCol);
	
protected:
	virtual bool DefaultInit();
	virtual bool InitResultBar();

	void SelectRow(int nRow);
	void UpdateRows(int nStartRow = 1);
	void UpdateColumns(int nStartCol = -1, bool bUpdate = false);

	bool UpdateColumn(int nIndex);
	CString	m_strGroupManagerName;

	virtual LPCTSTR	GetObjectType()  {	return "";	}
// support context menu
	bool CheckValidNewID(UINT uID);
	bool ActivateFromFocus();

// Implementation
protected:
	CVTGridCtrl		m_Grid;
	CResultBar		m_ResultBar;
	bool			m_bShowResBar;
	UINT			m_uViewMode; // mode of view
	CString			m_strSection;
	long			m_lStartPrintResultBarPosition;


	struct RowInfo
	{
		long		row;
		ICalcObject	*pcalc;
		bool		bFilled;
	};
	struct ColInfo
	{
		long	col;
		ParameterContainer	*pparam;
	};

	void	ArrangeRows();
	void	ArrangeCols();
	

	COLORREF	m_clrLinear, m_clrAngle, m_clrCounter, m_clrUndefine;		// 205, 255, 205 - linear// green
	COLORREF	m_clrUnknownClass;
	CString		m_strUnknownClass;

	INamedDataObject2Ptr		m_objects;
	ICalcObjectContainerPtr		m_container;

	// Generated message map functions
	//{{AFX_MSG(CGridViewBase)
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL	OnEraseBkgnd(CDC *); 
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnSelChangingGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelChangedGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnActivateCell(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnDestroy();
	//}}AFX_MSG

	BEGIN_INTERFACE_PART(Grid, IGridView)
		// vanek
		com_call GetObjectByRow( long lrow, IUnknown **ppunkObject );
		com_call GetRowHeight( long lrow, long *lpheight );
		com_call SetRowHeight( long lrow, long lheight );
		com_call GetColumnWidth( long lcolumn, long *lpwidth );
		com_call SetColumnWidth( long lcolumn, long lwidth );
	END_INTERFACE_PART(Grid)

	BEGIN_INTERFACE_PART(Frame, IMultiSelection)
		com_call SelectObject( IUnknown* punkObj, BOOL bAddToExist );
		com_call UnselectObject( IUnknown* punkObj );
		com_call GetObjectsCount(DWORD* pnCount);
		com_call GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj);
		com_call GetFlags(DWORD* pnFlags);
		com_call SetFlags(DWORD nFlags);
		com_call GetFrameOffset(POINT* pptOffset);
		com_call SetFrameOffset(POINT ptOffset);
		com_call GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset);
		com_call RedrawFrame();
		com_call EmptyFrame();
		com_call SyncObjectsRect();
	END_INTERFACE_PART(Frame)

    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnSetColumnWidth(long col, long width);
	virtual void OnEndColumnDrag(int nCol, int nColAfter);
	virtual void OnInsertColumn(long nIndex);
	virtual void OnInsertRow(long nIndex);
	virtual void OnDeleteColumn(long nColumn);
	virtual void OnDeleteRow(long nRow);
	virtual void OnSetColumnCount(long nCols);
	virtual void OnSetRowCount(long nRows);
	virtual bool OnGetDispInfoRow( long row );
	virtual void OnHorizontalScroll(long nSBCode, long nPos, CScrollBar*pScrollBar);
	virtual void OnVertivalScroll(long nSBCode, long nPos, CScrollBar*pScrollBar){};
	virtual bool SyncResBar();
	virtual void OnChangeSelection();
	virtual void OnSetRowHeight(long row, long height);
	virtual void OnSetFixedColumnCount(long nFixedCols){};
	virtual void OnBeginDragColumn(int nCol){};
	virtual bool EnableDragColumn(int nCol){return true;};
	virtual void OnSetFocusCell(long row, long col, BOOL bScroll ){};

	virtual const char *GetMenuName()	= 0;
protected:
	RowInfo*	_row_by_object( ICalcObject *pcalc );
	ColInfo*	_col_by_param( ParameterContainer *pparam );
	ParameterContainer	*_param_by_key( long lKey );
	virtual void	_selection_from_range();
	virtual void	_range_from_selection();
	virtual void	_clear_selection();
	virtual IUnknown *_object_from_cell( long row, long &col, bool bChParam = false );
	virtual bool	_is_selected( IUnknown *punk );
	virtual void _add_result_to_grid();
	virtual void _remove_result_from_grid();

protected:
	CMap<ICalcObject*, ICalcObject*, RowInfo*, RowInfo*&>					m_mapObjectToRow;
	CMap<ParameterContainer *, ParameterContainer *&, ColInfo*, ColInfo*&>	m_mapParamDefToCol;
	CMap<long, long&, ParameterContainer *, ParameterContainer *&>			m_mapParamKeyToParamDef;
	CTypedPtrArray<CPtrArray, ColInfo*>		m_cols;
	CTypedPtrArray<CPtrArray, RowInfo*>		m_rows;
	CTypedPtrArray<CPtrArray, IUnknown*>	m_selection;
	long	m_rowPrintResult;

public:
	BOOL	SetRowHeightByObject( long lrow, IUnknown *punkObject );

	// [vanek]: accelerate table
	HACCEL m_hAccel;
	virtual bool DoTranslateAccelerator( MSG *pMsg );

	// update active cell but no scroll horizontally
	void UpdateActiveCellNoHScroll(void);
};

#define IDC_GRID_CTRL	100100 /// ID for grid
#define IDC_RESULT_BAR	100101 // ID for result bar


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDVIEW_H__CB3A604D_77A1_4D5B_BBE4_ACA02AAA5F82__INCLUDED_)
