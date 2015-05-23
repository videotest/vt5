#pragma once

// LineBinPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage dialog
#include "CommonPropPage.h"
#include "vtcolorpicker.h"
class CLineBinPropPage : public CEditorPropBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CLineBinPropPage);
	GUARD_DECLARE_OLECREATE(CLineBinPropPage);

	int m_nActiveWidth,m_nActiveWidth2;

	void ChangeLineWidth(int nWidth, const char *pszEntryName);
	void InitButtons();
	void SaveButtons();
//	void InitColors();
// Construction
public:
	CLineBinPropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLineBinPropPage)
	enum { IDD = IDD_PROPPAGE_LINE_BIN };
	CVTColorPicker m_clrpickFore;
	CVTColorPicker m_clrpickBack;
	CVTWidth m_LineWidth1,m_LineWidth2,m_EraseWidth1,m_EraseWidth2;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLine1PropPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	virtual void DoLoadSettings();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLine1PropPage)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnChangeVtwidthctrl1();
		afx_msg void OnChangeVtwidthctrl2();
		afx_msg void OnChangeVtwidthctrl3();
		afx_msg void OnChangeVtwidthctrl4();
		afx_msg void OnOnClickVtwidthctrl1();
		afx_msg void OnOnClickVtwidthctrl2();
		afx_msg void OnOnClickVtwidthctrl3();
		afx_msg void OnOnClickVtwidthctrl4();
		afx_msg void OnOnChangeVtcolorpickerctrl1();
		afx_msg void OnDestroy();
		DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
