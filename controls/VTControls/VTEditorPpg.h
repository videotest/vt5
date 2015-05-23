#if !defined(AFX_VTEDITORPPG_H__F93D5CE3_2972_4E81_8C79_A814DA6503FB__INCLUDED_)
#define AFX_VTEDITORPPG_H__F93D5CE3_2972_4E81_8C79_A814DA6503FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTEditorPpg.h : Declaration of the CVTEditorPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPage : See VTEditorPpg.cpp.cpp for implementation.

class CVTEditorPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTEditorPropPage)
	DECLARE_OLECREATE_EX(CVTEditorPropPage)

// Constructor
public:
	CVTEditorPropPage();

// Dialog Data
	//{{AFX_DATA(CVTEditorPropPage)
	enum { IDD = IDD_PROPPAGE_VTEDITOR };
	CString	m_strName;
	CString	m_strKey;
	BOOL	m_bAutoUpdate;
	BOOL	m_bUseSystemFont;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTEditorPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt : Property page dialog

class CVTEditorPropPageExt : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTEditorPropPageExt)
	DECLARE_OLECREATE_EX(CVTEditorPropPageExt)

// Constructors
public:
	CVTEditorPropPageExt();

// Dialog Data
	//{{AFX_DATA(CVTEditorPropPageExt)
	enum { IDD = IDD_PROPPAGE_VTEDITOR_EXT };
	int		m_nCaptionAlign;
	int		m_nDataType;
	BOOL	m_bCaptionEnable;
	BOOL	m_bSpinEnable;
	BOOL	m_bValidationEnable;
	CString	m_strCaptionText;
	short	m_nEditFieldWidth;
	float	m_fMaxValue;
	float	m_fMinValue;
	BOOL	m_bCaptionAutoWidth;
	BOOL	m_bAlignClient;
	BOOL	m_bMultiLine;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support	
	void SetControlStateOnCangeCaptionEnable( BOOL bSaveData );
	void SetControlStateOnCangeEnableValidation( BOOL bSaveData );
	void SetControlStateOnCangeComboDataType( BOOL bSaveData );
	void SetControlStateOnChangeComboCaptionAlign( BOOL bSaveData );
	void SetControlStateEnableCaptionAutoWidth( BOOL bSaveData );

// Message maps
protected:
	//{{AFX_MSG(CVTEditorPropPageExt)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckCaptionEnable();
	afx_msg void OnCheckEnableValidation();
	afx_msg void OnSelchangeComboDataType();	
	afx_msg void OnSelchangeComboCaptionAlign();
	afx_msg void OnCheckEnableCaptionAutoWidth();
	afx_msg void OnSetAutoSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTEDITORPPG_H__F93D5CE3_2972_4E81_8C79_A814DA6503FB__INCLUDED)
