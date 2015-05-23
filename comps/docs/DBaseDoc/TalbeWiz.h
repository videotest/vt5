#if !defined(AFX_TALBEWIZ_H__677FD3A3_518F_457A_8165_3332FC7AC1BC__INCLUDED_)
#define AFX_TALBEWIZ_H__677FD3A3_518F_457A_8165_3332FC7AC1BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TalbeWiz.h : header file
//

#include "dbstructdlg.h"
/////////////////////////////////////////////////////////////////////////////
// CNewTablePage dialog

class CNewTablePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNewTablePage)

// Construction
public:
	CNewTablePage();
	~CNewTablePage();

// Dialog Data
	//{{AFX_DATA(CNewTablePage)
	enum { IDD = IDD_NEW_TABLE_WIZARD_NAME };
	CString	m_strTableName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewTablePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewTablePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CDefineFieldCreationPage dialog

class CDefineFieldCreationPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDefineFieldCreationPage)
// Construction
public:
	CDefineFieldCreationPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineFieldCreationPage)
	enum { IDD = IDD_NEW_TABLE_WIZARD_DEFINE_TYPE };
	int		m_nDefineType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineFieldCreationPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineFieldCreationPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CAdvFieldConstrPage dialog

class CAdvFieldConstrPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAdvFieldConstrPage)

// Construction
public:
	CAdvFieldConstrPage();
	~CAdvFieldConstrPage();

// Dialog Data
	//{{AFX_DATA(CAdvFieldConstrPage)
	enum { IDD = IDD_NEW_TABLE_WIZARD_DEF1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvFieldConstrPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvFieldConstrPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDBStructGrid m_grid;

};
/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldConstrPage dialog

class CSimpleFieldConstrPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSimpleFieldConstrPage)

// Construction
public:
	CSimpleFieldConstrPage();
	~CSimpleFieldConstrPage();

// Dialog Data
	//{{AFX_DATA(CSimpleFieldConstrPage)
	enum { IDD = IDD_NEW_TABLE_WIZARD_DEF2 };
	int		m_nImage;
	int		m_nNumber;
	int		m_nDateTime;
	int		m_nString;
	int		m_nOLE;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSimpleFieldConstrPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSimpleFieldConstrPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALBEWIZ_H__677FD3A3_518F_457A_8165_3332FC7AC1BC__INCLUDED_)
