#if !defined(AFX_FILTERCOMBO_H__0C0F69D9_4C5C_41B4_9C22_30D747DC8DB5__INCLUDED_)
#define AFX_FILTERCOMBO_H__0C0F69D9_4C5C_41B4_9C22_30D747DC8DB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterCombo window

class CFilterCombo : public CComboBox
{
// Construction
public:
	CFilterCombo();

// Attributes
public:	
	void BuildAppearance( bool bSetSelectionToFirstItem = false );

protected:	

	sptrIQueryObject GetActiveQuery();

	CFont m_Font;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFilterCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterCombo)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddFilter();
	afx_msg void OnEditFilter();
	afx_msg void OnDeleteFilter();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERCOMBO_H__0C0F69D9_4C5C_41B4_9C22_30D747DC8DB5__INCLUDED_)
