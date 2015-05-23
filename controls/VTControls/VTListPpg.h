#if !defined(AFX_VTLISTPPG_H__66955FFF_5A2A_4914_A7BC_769058AF5859__INCLUDED_)
#define AFX_VTLISTPPG_H__66955FFF_5A2A_4914_A7BC_769058AF5859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTListPpg.h : Declaration of the CVTListPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTListPropPage : See VTListPpg.cpp.cpp for implementation.

class CVTListPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTListPropPage)
	DECLARE_OLECREATE_EX(CVTListPropPage)

// Constructor
public:
	CVTListPropPage();

// Dialog Data
	//{{AFX_DATA(CVTListPropPage)
	enum { IDD = IDD_PROPPAGE_VTLIST };
	BOOL	m_b_use_grid;
	BOOL	m_b_UseFullRowSelect;
	BOOL	m_bAutoSaveToShell;
	BOOL	m_bEnabled;
	CString	m_strName;
	BOOL	m_bUseSystemFont;
	BOOL	m_bUseSimpleFontType;
	BOOL	m_bEnableResizeColumns;	// [vanek] BT2000: 3542 - 08.01.2004
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTListPropPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



class CVTList;

/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol : Property page dialog

class CVTListPropPageCol : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTListPropPageCol)
	DECLARE_OLECREATE_EX(CVTListPropPageCol)

protected:
	BOOL UpdatePageAndScrollToNewItem( int nItem );
// Constructors
public:
	CVTListPropPageCol();

// Dialog Data
	//{{AFX_DATA(CVTListPropPageCol)
	enum { IDD = IDD_PROPPAGE_VTLISTEX };
	CEdit	m_colWidthCtrl;
	CButton	m_readOnlyCtrl;
	CButton	m_useIcon;	
	CButton	m_logFormatCtrl;
	CComboBox	m_typeCtrl;	
	CListCtrl	m_listCtrl;
	CComboBox	m_alignCtrl;
	//}}AFX_DATA
protected:

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

	BOOL BuildColumnList( );
	BOOL ChangeVisual( int nItem );

public:
	static BOOL GetDISPIDFromName(LPOLESTR* ppszNames, DISPID &id, LPDISPATCH lpDispatch);

protected:
	LPDISPATCH GetDispatch();

// Message maps
protected:
	//{{AFX_MSG(CVTListPropPageCol)
	virtual BOOL OnInitDialog();	
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboColumnType();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboAlign();
	afx_msg void OnButtonForeColor();
	afx_msg void OnButtonBackColor();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnCheckLogAsText();
	afx_msg void OnCheckUseIcon();
	afx_msg void OnButtonFont();
	afx_msg void OnButtonSetTrueIcon();
	afx_msg void OnButtonSetFalseIcon();
	afx_msg void OnCheckReadOnly();
	afx_msg void OnChangeEditColWidth();
	afx_msg void OnButtonSetGrayIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnPaint();
};
/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow : Property page dialog

class CVTListPropPageRow : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTListPropPageRow)
	DECLARE_OLECREATE_EX(CVTListPropPageRow)

// Constructors
public:
	CVTListPropPageRow();	
// Dialog Data
	//{{AFX_DATA(CVTListPropPageRow)
	enum { IDD = IDD_PROPPAGE_VTLIST_ROW };
	CListCtrl	m_listRowCtrl;
	CStatic	m_curColumnCtrl;		
	//}}AFX_DATA

protected:
	BOOL BuildRowList( );
	LPDISPATCH GetDispatch();
	BOOL ScrollToNewItem( int nItem );
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTListPropPageRow)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAddRow();
	afx_msg void OnButtonDeleteRow();
	afx_msg void OnButtonSetIcon();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnButtonClearIcon();
	afx_msg void OnButtonCurColumnLeft();
	afx_msg void OnButtonCurColumnRight();	
	afx_msg void OnItemchangedListRow(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnEndlabeleditListRow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRefresh();
	afx_msg void OnChangeEditKeyValue();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonForeColor();
	afx_msg void OnBnClickedButtonBackColor();
};

#include "\vt5\awin\misc_list.h"


// CVTListPropPageHeader dialog

class CVTListPropPageHeader : public COlePropertyPage
{
	CButton m_btnUseHeader;
	CListCtrl m_lstCollumn;
	DECLARE_DYNCREATE( CVTListPropPageHeader )
	DECLARE_OLECREATE_EX( CVTListPropPageHeader )

	bool m_bEditChangeLock;
public:
	struct XLevelData
	{
		struct XData
		{
			CString strName;
			int nAssign;
			int nWidth;
			
			static void _cleaner( XData *data )
			{
				delete data;
			}
		};

		_list_t<XData*, XData::_cleaner> listData;

		static void _cleaner( XLevelData *data )
		{
			delete data;
		}
	};

	_list_t<XLevelData*, XLevelData::_cleaner > listHeaders;
	int m_nCurrent, m_nLevels;


	CVTListPropPageHeader();
	virtual ~CVTListPropPageHeader();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VTLIST_HEADER };

	short m_nDelCol;
	BOOL m_FirstColumn;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	LPDISPATCH GetDispatch();

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedUseHeader();
	void EnableWindow( UINT nId, bool bState );
	afx_msg void OnBnClickedButtonAddHeader();
	afx_msg void OnBnClickedButtonDeleteHeader();

	afx_msg void OnDeltaposLevels(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposcurLevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposNoOfCol(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	void _update_column_info();
	XLevelData::XData *_get_item( int nIndex );
	void _update_ctrl();
	void _fill_data_from_ctrl();


	BOOL _exec(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames );
	BOOL _exec_set(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nVal );
	BOOL _exec_set_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, CString strname, int nAssign, int nWidth, int nDel );

	BOOL _exec_get(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int &nVal );
	BOOL _exec_get2(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nLev, int &nVal );
	BOOL _exec_get_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, CString &str );
	BOOL _exec_get_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, int &nVal );
public:
	afx_msg void OnLvnEndlabeleditListRow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListRow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnEnChangeEditAssign();
	afx_msg void OnEnChangeEditCLev();
	afx_msg void OnEnChangeEditLev();
	afx_msg void OnPaint();
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTLISTPPG_H__66955FFF_5A2A_4914_A7BC_769058AF5859__INCLUDED)
