#if !defined(AFX_CIRCLE1PROPPAGE_H__5164FE90_9761_482E_BE44_0997104B76F7__INCLUDED_)
#define AFX_CIRCLE1PROPPAGE_H__5164FE90_9761_482E_BE44_0997104B76F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Circle1PropPage.h : header file
//
#include "CommonPropPage.h"
#include "pushbutton.h"


/////////////////////////////////////////////////////////////////////////////
// CCircle1PropPage dialog

class CCircle1PropPage : public CEditorPropBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CCircle1PropPage);
	GUARD_DECLARE_OLECREATE(CCircle1PropPage);
// Construction
public:
	CCircle1PropPage(CWnd* pParent = NULL);   // standard constructor
	CCircle1PropPage(UINT nIDTemplate);

// Dialog Data
	//{{AFX_DATA(CCircle1PropPage)
	enum { IDD = IDD_PROPPAGE_CIRCLE1 };
	CPushButton m_pbType[3];
	CVTColorPicker m_clrpickFore;
	CVTColorPicker m_clrpickBack;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCircle1PropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void DoLoadSettings();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCircle1PropPage)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnOnChangeVtwidthctrl2();
	afx_msg void OnOnClickVtwidthctrl2();
	afx_msg void OnOnChangeVtwidthctrl3();
	afx_msg void OnOnClickVtwidthctrl3();
	afx_msg void OnOnChangeVtwidthctrl4();
	afx_msg void OnOnClickVtwidthctrl4();
	afx_msg void OnOnChangeVtwidthctrl1();
	afx_msg void OnOnClickVtwidthctrl1();
	afx_msg void OnClickPushbuttonctrlType1();
	afx_msg void OnClickPushbuttonctrlType2();
	afx_msg void OnClickPushbuttonctrlType3();
	afx_msg void OnOnChangeVtcolorpickerctrl1();
	afx_msg void OnOnChangeVtcolorpickerctrl2();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void _ManageTypeButtonState(int nButton);
	void _SetTypeValue(int nButton);
	void InitColors();


	long			m_nCurButtonType;
	CString			m_strFigure;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIRCLE1PROPPAGE_H__5164FE90_9761_482E_BE44_0997104B76F7__INCLUDED_)
