#if !defined(AFX_VIEWGRB_H__55672140_0976_11D4_895A_009027E630C7__INCLUDED_)
#define AFX_VIEWGRB_H__55672140_0976_11D4_895A_009027E630C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewGrb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewGrb window

class CViewGrb : public CWnd
{
// Construction
public:
	CViewGrb();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGrb)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewGrb();
	BOOL Attach(DWORD dwStyle, CWnd* pParentWnd, UINT nID);

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewGrb)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWGRB_H__55672140_0976_11D4_895A_009027E630C7__INCLUDED_)
