#if !defined(AFX_FIELDPROP_H__3E4814C7_199E_40CD_816B_B433F8DD8D16__INCLUDED_)
#define AFX_FIELDPROP_H__3E4814C7_199E_40CD_816B_B433F8DD8D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FieldProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFieldProp dialog

class CFieldProp : public CPropertyPage
{
// Construction
public:
	CFieldProp();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldProp)
	enum { IDD = IDD_FIELD_PROPERTIES };
	BOOL	m_bAutoLabel;
	CString	m_strCaption;
	CString	m_strDefValue;
	BOOL	m_bEnableEnumeration;
	CString	m_strField;
	CString	m_strTable;
	BOOL	m_bHasDefValue;
	int		m_nLabelPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFieldProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnHasDefValue();
	afx_msg void OnAutolabel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	IUnknownPtr m_ptrDoc;

public:
	void SetDocument( IUnknown* punkDoc ){	m_ptrDoc = punkDoc;}

	void AnalizeVTObject();
};

/////////////////////////////////////////////////////////////////////////////
// CObjFieldPropPage dialog

class CObjFieldPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CObjFieldPropPage)

// Construction
public:
	CObjFieldPropPage();
	~CObjFieldPropPage();

// Dialog Data
	//{{AFX_DATA(CObjFieldPropPage)
	enum { IDD = IDD_FIELD_OBJ_PAGE };
	CString	m_strViewType;
	BOOL	m_bAutoAsign;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CObjFieldPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CObjFieldPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoAsign();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	CString m_strBaseType;

public:
	void SetBaseType( CString strBaseType );

	void AnalizeAppearance();

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDPROP_H__3E4814C7_199E_40CD_816B_B433F8DD8D16__INCLUDED_)
