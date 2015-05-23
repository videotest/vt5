#if !defined(AFX_DBASEGRID_H__EB3CA2F1_6195_11D4_AEFA_0000E8DF68C3__INCLUDED_)
#define AFX_DBASEGRID_H__EB3CA2F1_6195_11D4_AEFA_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBaseGrid.h : header file
//

#include "GridCtrl.h"
#include "types.h"
#include "PopupList.h"


#define GRID_HEADER_RECORD	 0
#define GRID_NONE_RECORD	-1
#define GRID_FILTER_RECORD	-2
#define GRID_SORT_RECORD	-3



#define ID_CELL_WAS_NOT_READ	-3141842
#define ID_CELL_WAS_READ		-9300172

#define ID_SORT_NONE		0
#define ID_SORT_ASCENDING	1
#define ID_SORT_DESCENDING	2




/////////////////////////////////////////////////////////////////////////////
// CDBaseGridCtrl window


//Save this struct in Query object

class CDBaseGridCtrl : public CGridCtrl
{
// Construction

public:
	CDBaseGridCtrl();
	void BuildAppearance( );
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );	
	virtual BOOL MoveColumn(int nCol, int nColAfter);
	virtual BOOL SetColumnWidth(int col, int width);


public:
    class XPopupCtrlReciever : public IPopupCtrlReciever
    {
    public:
        virtual bool OnNotify( const char* szEvent,  CString strValue );
		virtual bool OnNotify( const char* szEvent,  UINT nValue );

    } m_xPopupCtrlReciever;


public:
	void Init();

	void OnParentGetSelChangeMessage();
protected:

	virtual void  OnEditCell(int nRow, int nCol, CPoint point, UINT nChar);
	virtual void  OnEndEditCell(int nRow, int nCol, CString str);

	bool m_bCanProcessOnChangeRecordScroll;		

public:

	
	void SetCurrentRecord( int nRecordNum );	

	int GetRecordNumFormRow( int nRow );
	int GetRowFormRecordNum( int nRecordNum );

	int GetCurrentRecord();	

	

	sptrISelectQuery GetActiveQuery();
	sptrIDBaseDocument GetDBaseDocument();

	bool SaveGridSettingsToQuery();

protected:
	void FillGridFirstTime();
	long GetRecordsetRowCount();

	bool GetValueFromData( sptrINamedData spND, int nCol, CString& strValue );
	bool SetValueToData( sptrINamedData spND, int nCol, CString strValue );
	

	//Columns info
	CArray<CGridColumnInfo*, CGridColumnInfo*> m_arColumnInfo;
	void DestroyColumnInfoArr();
	BOOL	m_bUseFilterRow;
	BOOL	m_bUseSortRow;

	//Header+Filter+Sort row
	int m_nSecondaryRowCount;

	//Empty grid if error
	void SetGridAsEmpty();

	void SaveGridToQuery();

	


	void AfterInsertRecord();
	void AfterDeleteRecord( int nRecordNum );
	void AfterFieldChange( CString strTable, CString strField, _variant_t var  );
	void AfterNavigation();
	
	void AnalizeNewValueSet();

// Controls Support
protected:	
	long m_nRecordDelete;	

public:
	int GetFilterRow();
	int GetSortRow();

public:
	void FillFilterRow();
	void SetCurrentFilter( int nColumnInfoIndex, int nSubIndex );	
	void OnFilterPopup( int nCol );	
	void PopupFilterMenu( CCellID cell, CPoint point );


	void FillSortRow();
	void OnSortPopup( int nCol );
	CString GetSortString( CString strTable, CString strField );
	void SetCurrentSort( int nColumnInfoIndex, int nSubIndex );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBaseGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBaseGridCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDBaseGridCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddFilter();
	afx_msg void OnEditFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	//Filter helper
	int GetFilterCount( CString strTable, CString strField );
	CString GetFilterString( CString strTable, CString strField, int nSubIndex );
	bool EditFilter( CString strTable, CString strField, int nSubIndex );
	bool DeleteFilter( CString strTable, CString strField, int nSubIndex );
	bool AddFilter( CString strTable, CString strField );

	bool GetTableFieldSubIndexFromCurrentCell( CString& strTable, CString& strField, int& nSubIndex );

	int GetIndexFromSubIndex(CString strTable, CString strField, int nSubIndex );

	int GetInfoColumnFromTableField( CString strTable, CString strField );

	void TestForGridColumns();

//Filter Support
public:
	virtual bool GetActiveField( CString& strTable, CString& strField );
	virtual bool GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue );

	virtual bool ActivateField( CString strTable, CString strField );

	int m_nPrevRow;
	void SetPrevRow( int nPrevRow ){ m_nPrevRow = nPrevRow;}

	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );


protected:
	bool GotoRecordSlow( long lNewCurrentRecord );
	long m_lCurrentTimerRecord;
	bool m_bTimerActivate;

	UINT m_nTimerID;
	void StartTimer();
	void StopTimer();
	long m_lRecordStartTickCount;

	bool ForceGoToRecord( );
	//long GetCurrentRecord();

	virtual CGridCellBase* CreateCell(int nRow, int nCol);
public:
	void ForceEndEditing();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASEGRID_H__EB3CA2F1_6195_11D4_AEFA_0000E8DF68C3__INCLUDED_)
