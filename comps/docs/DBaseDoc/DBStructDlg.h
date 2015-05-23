#if !defined(AFX_DBSTRUCTDLG_H__B17C85EA_144E_4FCF_9C68_645002591580__INCLUDED_)
#define AFX_DBSTRUCTDLG_H__B17C85EA_144E_4FCF_9C68_645002591580__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBStructDlg.h : header file
//


#include "GridCtrl.h"
#include "types.h"
#include "GridCellCombo.h"




#define FIELD_TYPE_DBASE			0x01
#define FIELD_TYPE_VT				0x02
#define FIELD_TYPE_FILTER_VT		0x04


class CDBStructDlg;

class CDBStructGrid : public CGridCtrl
{
	friend class CDBStructDlg;

public:
	CDBStructGrid( );
	~CDBStructGrid( );

	bool Init( );
	bool DeInit( );
	bool Build( CArray<_CFieldInfo*, _CFieldInfo*>* parFieldInfo );
	void Clear();

protected:
	CArray<_CFieldInfo*, _CFieldInfo*> m_arFieldInfo;	

public:	
	static void GetAvailableTypes( CStringArray& arrTypes, CStringArray& arrUserNames, DWORD dwFlags );
	static bool ConvertFromStringToFieldType( CString strType, FieldType& ft, CString& strObjectType );
	static CString GetFieldTypeAsString( _CFieldInfo* pfi );

protected:
	void InitComboRow( int nRow, bool bIncludeDBaseType );

	COLORREF GetReadOnlyColor();

	bool IsValidRow( int nRow );

	virtual void OnSetFocusCell(int nRow, int nCol);
	virtual void  OnEndEditCell(int nRow, int nCol, CString str);

	bool FillRow(_CFieldInfo* pfi, int nRow);

	bool FillFieldInfoFromRow( int nRow, _CFieldInfo* pfi );
	

	static CString LoadString( UINT uiRes );

	bool m_bWasModified;

public:
	void GetTableStruct( _CFieldInfo** parfi, int* pnSize );
	bool VerifyStruct( bool bTestForPrimaryKey, CString& strError );

protected:

	//{{AFX_MSG(CDBStructGrid)	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL SetColumnWidth(int col, int width);

public:
	void CreateKeyRow( CString strTableName );
};



#define IDC_GRID_CTRL 200

#define COL_NAV					0
#define COL_FIELD				1
#define COL_USER				2
#define COL_FIELD_TYPE			3
#define COL_IS_REQ				4
#define COL_IS_DEF				5
#define COL_DEF_VALUE			6
#define COL_FIELD_FORMAT		7
#define COL_REF_TABLE			8
#define COL_REF_TABLE_FIELD		9

#define COL_MIN	COL_NAV
#define COL_MAX	COL_REF_TABLE_FIELD


/////////////////////////////////////////////////////////////////////////////
// CDBStructDlg dialog

class CDBStructDlg : public CDialog
{
	friend class CDBStructGrid;
// Construction
public:
	CDBStructDlg(CWnd* pParent = NULL);   // standard constructor
	~CDBStructDlg();

// Dialog Data
	//{{AFX_DATA(CDBStructDlg)
	enum { IDD = IDD_DBSTUCT };
	CString	m_strTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBStructDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDBStructDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnAddTable();
	afx_msg void OnChoiceTable();
	afx_msg void OnDeleteTable();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDeleteRow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "DBStruct" );

	void OnResizeDlg();	

public:
	void SetDBaseDoc( IUnknown* punkDoc );
	bool SetMode( EditModeDlg editMode, CString strTable );
	
	_CTableInfo* m_pti;

protected:
	CDBStructGrid m_grid;
	IDBaseStructPtr m_ptrDB;

	EditModeDlg	m_editMode;

	bool TryToSave( );

protected:
	bool VerifyTableStruct( CString strTableName, _CFieldInfo* pArrFieldInfo, int nSize );		
	bool IsPossibleName( CString strName );

	bool ReadTableInfo( bool bForceReread );

	bool	m_bShowButtons;
};

/////////////////////////////////////////////////////////////////////////////
// CStructEditConfirmationDlg dialog

class CStructEditConfirmationDlg : public CDialog
{
// Construction
public:
	CStructEditConfirmationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStructEditConfirmationDlg)
	enum { IDD = IDD_STRUCT_CONFIRM };
	CListBox	m_ctrlRemove;
	CListBox	m_ctrlAdd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStructEditConfirmationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStructEditConfirmationDlg)
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "StructEditConfirmation" );

	CStringArray m_arrAddFields;
	CStringArray m_arrRemoveFields;

public:
	void AddField2Add( CString str );
	void AddField2Remove( CString str );



};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBSTRUCTDLG_H__B17C85EA_144E_4FCF_9C68_645002591580__INCLUDED_)
