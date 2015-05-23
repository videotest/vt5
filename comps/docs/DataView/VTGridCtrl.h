#if !defined(AFX_VTGRIDCTRL_H__2CBB79DB_F2A0_4AFB_843E_C4F5B29D308B__INCLUDED_)
#define AFX_VTGRIDCTRL_H__2CBB79DB_F2A0_4AFB_843E_C4F5B29D308B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTGridCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVTGridCtrl window

#include "DataViewDef.h"

#include "GridCell.h"
#include "GridCtrl.h"

class CGridViewBase;
class CVTGridCtrl;

class CVTGridCell : public CGridCell
{
	friend class CVTGridCtrl;

    DECLARE_DYNCREATE(CVTGridCell)
public:
	CVTGridCell() {	m_eCellType = egctText;m_row = -1;	};
	virtual ~CVTGridCell() {};

public:
	virtual EGridCellType	GetType()							{	return m_eCellType;	}
	virtual void			SetType(EGridCellType eCellType)	{	m_eCellType = eCellType;	}
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);

	virtual void SetCoords( int row, int /* nCol */)			{m_row = row;}
	virtual LPCTSTR  GetText();

	CVTGridCtrl* GetVTGrid();
protected:
	EGridCellType	m_eCellType;
	long	m_row;
};



class CVTGridCtrl : public CGridCtrl
{
	friend class CVTGridCell;
	friend class CGridViewBase;
    DECLARE_DYNCREATE(CVTGridCtrl)
// Construction
public:
	CVTGridCtrl();
	virtual ~CVTGridCtrl();

// Attributes
public:
	CGridViewBase * m_pGridView;
	CCellID m_RClickedCell;
// Operations
public:
	virtual BOOL GetDispInfoRow( long row );
    virtual int  InsertColumn(LPCTSTR strHeading, UINT nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS, int nColumn = -1);
    virtual int  InsertRow(LPCTSTR strHeading, int nRow = -1);
    virtual BOOL DeleteColumn(int nColumn);
    virtual BOOL DeleteRow(int nRow);
    virtual BOOL DeleteNonFixedRows();
    virtual BOOL DeleteAllItems();
    virtual BOOL SetRowHeight(int row, int height);
    virtual BOOL SetColumnWidth(int col, int width);

    virtual BOOL SetColumnCount(int nCols = 10);
	virtual BOOL SetRowCount(int nRows = 10);
    virtual BOOL SetFixedColumnCount(int nFixedCols = 1);

    virtual BOOL AutoSizeRow(int nRow);
    virtual BOOL AutoSizeColumn(int nCol, BOOL bIgnoreHeader = FALSE);
    virtual void AutoSizeRows();
    virtual void AutoSizeColumns();
    virtual void AutoSize(BOOL bIgnoreColumnHeader = FALSE);
    virtual void ExpandColumnsToFit();
    virtual void ExpandRowsToFit();
    virtual void ExpandToFit();
    virtual CCellID SetFocusCell(CCellID cell);
    virtual CCellID SetFocusCell(int nRow, int nCol){return CGridCtrl::SetFocusCell( nRow, nCol );}
	virtual void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
					      BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
	virtual void EnsureVisible(CCellID &cell)       { EnsureVisible(cell.row, cell.col); }
	virtual void EnsureVisible(int nRow, int nCol);



	virtual int		GetFirstDragableColumn()	{	return GetFixedColumnCount();	}
	virtual bool	EnableColumnDrag(int nCol);
    virtual void	OnBeginColumnDrag();
	virtual void	OnEndColumnDrag(int nCol, int nColAfter);

	virtual COLORREF GetCellBackColor(int nRow, int nCol);
	void ChangeCellType(int nRow, int nCol, EGridCellType type);

    CCellID GetCellFromPoint(CPoint & point)
	{	return GetCellFromPt(point, true);	}

	CVTGridCell* GetGridCell(int nRow, int nCol)
	{	return (CVTGridCell*)GetCell(nRow, nCol);	}

	CCellID GetRClickedCell()
	{	return m_RClickedCell;	}
	void	ResetRClickedCell()
	{	m_RClickedCell = CCellID(-1, -1);	}

	CCellID GetLClickedCell()
	{	return m_LeftClickDownCell;	}

//	void OnPrintDraw(CDC* pDC, CRect & RectDraw);
//	CCellRange GetCellRangeFromRect(LPRECT pRect);

protected:
    // GridCell Creation and Cleanup
    virtual CGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
    virtual COleDataSource* CopyTextFromGrid();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVTGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT, CPoint); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	bool	m_bEnableWidthNotify;
	bool	m_bEnableHeightNotify;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTGRIDCTRL_H__2CBB79DB_F2A0_4AFB_843E_C4F5B29D308B__INCLUDED_)
