#if !defined(AFX_DIALOGS_H__E71161A3_4007_11D3_A60C_0090275995FE__INCLUDED_)
#define AFX_DIALOGS_H__E71161A3_4007_11D3_A60C_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogs.h : header file
//
#include "DataTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDlg dialog

class CDeleteObjectDlg : public CDialog
{
// Construction
public:
	CDeleteObjectDlg( IUnknown *pDD, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteObjectDlg)
	enum { IDD = IDD_DELETE_OBJECT };
	//}}AFX_DATA
	CContextTreeCtrl	m_tree;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteObjectDlg)
	virtual void OnOK();
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void		SetSelectedObject(LPCTSTR szName);	//should be called before DoModal()
	IUnknown *	GetSelectedObject();				//with AddRef
protected:
	CString		m_strSelectedObject;
	IUnknown *	m_punkSelected;

};

/////////////////////////////////////////////////////////////////////////////
// CActivateObjectDlg dialog

class CActivateObjectDlg : public CDialog
{
// Construction
public:
	CActivateObjectDlg(IUnknown *punkContext, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActivateObjectDlg)
	enum { IDD = IDD_SELECT_OBJECT };
	//}}AFX_DATA
	CContextTreeCtrl	m_tree;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActivateObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActivateObjectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LPCTSTR GetObjectName()
	{	return m_strObjectName;	}
	void	SetObjectName(LPCTSTR szName)
	{	m_strObjectName = szName;	}
	
	LPCTSTR GetObjectKind()
	{	return m_strObjectType;	}
	void	SetObjectKind(LPCTSTR szKind)
	{	m_strObjectType = szKind;	}

	IUnknown * GetActiveObject(bool bAddRef = false)	
	{	
		if (bAddRef && m_sptrObjToActivate != 0) 
			m_sptrObjToActivate.AddRef(); 
		return m_sptrObjToActivate.GetInterfacePtr();	
	}

	void SetDlgTitle(LPCTSTR szTitle)
	{	m_strDlgTitle = szTitle;	}

	void SetBoundTitle(LPCTSTR szTitle)
	{	m_strBoundTitle = szTitle;	}
	
	void SetObjectTitle(LPCTSTR szTitle)
	{	m_strObjectTitle = szTitle;	}

protected:
	CString	m_strObjectName;	//last selected objet type
	CString	m_strObjectType;	//last selected object name

	CString	m_strDlgTitle;
	CString	m_strBoundTitle;
	CString	m_strObjectTitle;

	IUnknownPtr	m_sptrObjToActivate;
	

};
/////////////////////////////////////////////////////////////////////////////
// CCreateObjectDlg dialog

class CCreateObjectDlg : public CDialog
{
// Construction
public:
	CCreateObjectDlg(IUnknown *punkData, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateObjectDlg)
	enum { IDD = IDD_CREATE_OBJECT };
	CComboBox	m_comboKinds;
	CString		m_strName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateObjectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LPCTSTR GetObjectName()
	{	return m_strObjectName;	}
	void	SetObjectName(LPCTSTR szName)
	{	m_strObjectName = szName;	}

	LPCTSTR GetObjectKind()
	{	return m_strObjectType;	}
	void	SetObjectKind(LPCTSTR szKind)
	{	m_strObjectType = szKind;	}

protected:
	CString			m_strObjectName;	//last selected objet type
	CString			m_strObjectType;	//last selected object name
	sptrINamedData	m_sptrD;

	void FillTypesCombo();	//fills the combo box by avaible types
};


/////////////////////////////////////////////////////////////////////////////
// CRenameObjectDlg dialog

class CRenameObjectDlg : public CDialog
{
// Construction
public:
	CRenameObjectDlg(IUnknown* punkTarget,  CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenameObjectDlg)
	enum { IDD = IDD_RENAME_OBJECT };
	CString				m_strName;
	//}}AFX_DATA
	CContextTreeCtrl	m_tree;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRenameObjectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetOldObjectName(LPCTSTR szOld)
	{	m_strOldName = szOld;	}

	void SetNewObjectName(LPCTSTR szNew)
	{	m_strNewName = szNew;	}

	LPCTSTR		GetNewObjectName()
	{	return m_strNewName;	}

	IUnknown*	GetRenamedObject();

protected:
	CString			m_strOldName;	//Old object Name
	CString			m_strNewName;	//new object name
	sptrINamedData	m_sptrD;		// smart pointer on INamedData
	IUnknown *		m_punkRenameObject; // pointer on renamed object
};
/////////////////////////////////////////////////////////////////////////////
// COrganizerDlg dialog

class COrganizerDlg : public CDialog
{
// Construction
public:
	COrganizerDlg(IUnknown *punkDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~COrganizerDlg();

// Dialog Data
	//{{AFX_DATA(COrganizerDlg)
	enum { IDD = IDD_ORGANIZE_OBJECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrganizerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COrganizerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnToapp();
	afx_msg void OnTodoc();
	afx_msg void OnUndo();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void ChangeUndoButtonState( bool bAddAction );
	void UpdateTrees();

	CContextTreeCtrl	m_treeApp;
	CContextTreeCtrl	m_treeDoc;

	sptrIDataContext	m_contextApp;
	sptrIDataContext	m_contextDoc;

	IUnknown	*m_punkDoc;
	IUnknown	*m_punkApp;

	long	m_nUndoCounter;

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGS_H__E71161A3_4007_11D3_A60C_0090275995FE__INCLUDED_)
