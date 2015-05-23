#if !defined(AFX_EDITORCONTROLS_H__2F18A839_4595_4ED7_9240_348C30A819F2__INCLUDED_)
#define AFX_EDITORCONTROLS_H__2F18A839_4595_4ED7_9240_348C30A819F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditorControls.h : header file
//


#define ID_EDIT_CTRL	10100
#define ID_STATIC_CTRL	10101
#define ID_SPIN_CTRL	10102

typedef enum
{
	caAlignUp = 0,
	caAlignLeft = 1,

} CaptionAlignEnum;


typedef enum
{
	dtTypeString = 0,
	dtTypeInt = 1,
	dtTypeFloat = 2,

} DataTypeEnum;

class CVTEditor;

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl window

class CEditCtrl : public CEdit
{
// Construction
public:
	CEditCtrl();
	void SetParentCtrl( CVTEditor* pParent){
		m_pParent = pParent;
	}

protected:
	CVTEditor* m_pParent;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCtrl)
	afx_msg void OnKillfocus();
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CStaticCtrl window

class CStaticCtrl : public CStatic
{
// Construction
public:
	CStaticCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSpinCtrl window

class CSpinCtrl : public CSpinButtonCtrl
{
// Construction
public:
	CSpinCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpinCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpinCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpinCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORCONTROLS_H__2F18A839_4595_4ED7_9240_348C30A819F2__INCLUDED_)
