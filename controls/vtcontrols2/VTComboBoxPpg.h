#if !defined(AFX_VTCOMBOBOXPPG_H__3C415A82_0F1E_4F9D_B6FE_DEA479321AB7__INCLUDED_)
#define AFX_VTCOMBOBOXPPG_H__3C415A82_0F1E_4F9D_B6FE_DEA479321AB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTComboBoxPpg.h : Declaration of the CVTComboBoxPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTComboBoxPropPage : See VTComboBoxPpg.cpp.cpp for implementation.

class CVTComboBoxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTComboBoxPropPage)
	DECLARE_OLECREATE_EX(CVTComboBoxPropPage)

// Constructor
public:
	bool m_IsCreated;
	CVTComboBoxPropPage();

// Dialog Data
	//{{AFX_DATA(CVTComboBoxPropPage)
	enum { IDD = IDD_PROPPAGE_VTCOMBOBOX };
	CListBox	m_dropdown;
	CString	m_string;
	int		m_sizeHeight;
	int		m_sizeWidth;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTComboBoxPropPage)
	afx_msg void OnButtondelete();
	afx_msg void OnButtondeleteall();
	afx_msg void OnButtoninsert();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BSTR GetString(long idx);
	long GetStringsCount();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCOMBOBOXPPG_H__3C415A82_0F1E_4F9D_B6FE_DEA479321AB7__INCLUDED)
