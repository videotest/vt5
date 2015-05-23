#if !defined(AFX_CONTENTDLG_H__800AB706_EA3B_41E7_A1DF_15BEE849216C__INCLUDED_)
#define AFX_CONTENTDLG_H__800AB706_EA3B_41E7_A1DF_15BEE849216C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContentDlg.h : header file
//

// [vanek] : lParam of tree item - 23.03.2004
/////////////////////////////////////////////////////////////////////////////
// CItemData
class CItemData
{
public:
	CItemData()
	{ m_pstr_entry_name = 0; m_bobject = FALSE; }
	~CItemData()
	{ if( m_pstr_entry_name ) delete m_pstr_entry_name; }

	CString *m_pstr_entry_name; // 
	BOOL	m_bobject;	// if TRUE then m_pstr_entry_name - object's name
};

/////////////////////////////////////////////////////////////////////////////
// CContentDlg dialog


// CTreeMod

class CTreeMod : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeMod)

public:
	CTreeMod();
	virtual ~CTreeMod();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	HTREEITEM m_hActiveItem;
	afx_msg void OnModNamedVal();
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};

class CContentDlg : public CDialog
{
// Construction
public:
	CContentDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CContentDlg)
	enum { IDD = IDD_DATA_CONTENT };
	CTreeMod	m_treeData;
	int		m_nDataSource;
	CString m_sLastSelectedPath;
	//}}AFX_DATA
	CImageList	m_imageList;
	INamedDataPtr m_ptrData;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CContentDlg)
	afx_msg void OnSelchangeData();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillTree();
protected:
	void _FillDataBrunch( INamedData *pIData, HTREEITEM hti, const char *szPath, const char *szEntryName );
    BOOL _GetCurrSelPath( CString *pstr_path );
	BOOL _SetCurrSelPath( CString str_path );

	HTREEITEM	_FindItemByEntryName( HTREEITEM hti_parent, CString str_entry_name, BOOL bobject );

	void	_ExpandChilds( HTREEITEM hti_parent, UINT nCode );
	
	BOOL		_SelectPath( HTREEITEM hti_parent, CString str_path );
	HTREEITEM	_GetItemByPath( HTREEITEM hti_parent, CString str_path );
		
public:
	void	SetLastSelectedPath( CString str_path );
	BOOL	GetLastSelectedPath( CString *pstr_path );

	void	SetLastDataSource( int ndata_source );
	int		GetLastDataSource( );
    
public:
	afx_msg void OnTvnDeleteitemTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult);
	friend class CTreeMod;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTENTDLG_H__800AB706_EA3B_41E7_A1DF_15BEE849216C__INCLUDED_)
