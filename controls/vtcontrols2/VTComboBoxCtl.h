#if !defined(AFX_VTCOMBOBOXCTL_H__D4597797_1F13_471B_A2A0_C33D4EFD4EE8__INCLUDED_)
#define AFX_VTCOMBOBOXCTL_H__D4597797_1F13_471B_A2A0_C33D4EFD4EE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTComboBoxCtl.h : Declaration of the CVTComboBoxCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl : See VTComboBoxCtl.cpp for implementation.

class CVTComboBoxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTComboBoxCtrl)
	long m_nType;

// Constructor
public:
	CVTComboBoxCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTComboBoxCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	virtual BOOL DestroyWindow();
	virtual void OnFinalRelease();
	virtual void OnFontChanged();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTComboBoxCtrl();

	DECLARE_OLECREATE_EX(CVTComboBoxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTComboBoxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTComboBoxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTComboBoxCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	//{{AFX_MSG(CVTComboBoxCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTComboBoxCtrl)
	BOOL m_visible;
	afx_msg void OnVisibleChanged();
	afx_msg BSTR GetText();
	afx_msg long GetStringsCount();
	afx_msg BSTR GetString(long idx);
	afx_msg void DeleteString(long idx);
	afx_msg void ReplaceString(long idx, LPCTSTR NewVal);
	afx_msg void DeleteContents();
	afx_msg void InsertString(long idx, LPCTSTR NewVal);
	afx_msg long GetSel();
	afx_msg void SetSel(long idx);
	afx_msg long GetType();
	afx_msg void SetType(long idx);
	afx_msg void SetSortStrings(BOOL sStr);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CVTComboBoxCtrl)
	void FireOnChange()
		{FireEvent(eventidOnChange,EVENT_PARAM(VTS_NONE));}
	void FireOnSelectChange()
		{FireEvent(eventidOnSelectChange,EVENT_PARAM(VTS_NONE));}
	void FireClick()
		{FireEvent(DISPID_CLICK,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTComboBoxCtrl)
	dispidVisible = 1L,
	dispidGetText = 2L,
	dispidGetStringsCount = 3L,
	dispidGetString = 4L,
	dispidDeleteString = 5L,
	dispidReplaceString = 6L,
	dispidDeleteContents = 7L,
	dispidInsertString = 8L,
	dispidGetSel = 9L,
	dispidSetSel = 10L,
	eventidOnChange = 1L,
	eventidOnSelectChange = 2L,
	dispidSetType = 11L,
	dispidGetType = 12L,
	dispidSetSortStrings = 13L,
	//}}AFX_DISP_ID
	};
private:
	CStringArray m_tempStrings;
	//BOOL m_IsVisible;
	void RemoveBlankString();
	void InsertBlankString();
	BOOL m_IsBlankStringInserted;

	CFont	m_Font;

	CStringArray m_NameStrings;
	BOOL m_sort;
	int m_nSelStr;
};

class CVTComboBoxCtrl2 : public CVTComboBoxCtrl
{
	DECLARE_DYNCREATE(CVTComboBoxCtrl2)
	CVTComboBoxCtrl2();
protected:
	~CVTComboBoxCtrl2();

	DECLARE_OLECREATE_EX(CVTComboBoxCtrl2)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTComboBoxCtrl2)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTComboBoxCtrl2)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTComboBoxCtrl2)		// Type name and misc status
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCOMBOBOXCTL_H__D4597797_1F13_471B_A2A0_C33D4EFD4EE8__INCLUDED)
