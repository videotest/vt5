#if !defined(AFX_VIEWAXPPG_H__6EF4317F_8765_4608_8B00_4FD628AE0144__INCLUDED_)
#define AFX_VIEWAXPPG_H__6EF4317F_8765_4608_8B00_4FD628AE0144__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ViewAXPpg.h : Declaration of the CViewAXPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage : See ViewAXPpg.cpp.cpp for implementation.

/*
class CViewAXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CViewAXPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXPropPage)

// Constructor
public:
	CViewAXPropPage();

// Dialog Data
	//{{AFX_DATA(CViewAXPropPage)
	enum { IDD = IDD_PROPPAGE_VIEWAX };
	CComboBox	m_cbView;
	CString	m_strProgID;
	UINT	m_nDPI;
	double	m_fZoom;
	int		m_nAlign;
	CString	m_strObject;
	BOOL	m_bUseActiveView;
	int		m_nObjectDefinition;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void _RebuildViewsDesc();
	IUnknown* _GetDataByObjectName(LONG_PTR nTemplPos, const char *szName);
	void _UpdateProgID(int nSel = -1);

// Message maps
protected:
	//{{AFX_MSG(CViewAXPropPage)
	afx_msg void OnCloseupComboView();
	afx_msg void OnSetfocusEditObject();
	afx_msg void OnKillfocusEditObject();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboDefinition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	IApplicationPtr		m_sptrApp;
	CStringArray		m_arrProgID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
*/
#endif // !defined(AFX_VIEWAXPPG_H__6EF4317F_8765_4608_8B00_4FD628AE0144__INCLUDED)
