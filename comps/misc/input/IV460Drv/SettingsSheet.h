#if !defined(AFX_SETTINGSSHEET_H__F4CF8F72_B596_486E_88F1_5DA2D603AA5B__INCLUDED_)
#define AFX_SETTINGSSHEET_H__F4CF8F72_B596_486E_88F1_5DA2D603AA5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsSheet.h : header file
//

#include "HistoDlg.h"
#include "InputInt.h"
#include "DriverWrp.h"

enum ESettingsHint
{
	ESH_Start_Auto = 1,
	ESH_Stop_Auto = 2,
	ESH_Camera_Changed = 3,
	ESH_Binning = 4,
	ESH_Graffic_Changed = 5,
};


class CSettingsSheet;
class CSettingsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSettingsPage)
public:
	CSettingsPage(int nPage, IUnknown *punk = NULL) : CPropertyPage(nPage)
	{
		m_pSheet = NULL;
		if (punk) m_pDrv.Attach(punk);
	};
	CSettingsSheet *GetParentSheet();

	CSettingsSheet *m_pSheet;
	CDriverWrp m_pDrv;

	void SetModified(BOOL bChanged = TRUE);
	virtual void UpdateControls(int nHint = 0) {};
};


/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet

class CSettingsSheet : public CPropertySheet, public IRedrawHook
{
	DECLARE_DYNAMIC(CSettingsSheet)
	bool m_bInited;

// Construction
public:
	CSettingsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CRect m_rcNonGraffic,m_rcOk,m_rcCancel,m_rcApply,m_rcHelp;
	BOOL m_bFinished; // OnOK or OnCancel already called
	BOOL m_bModified;
	BOOL m_bGraffic;
	BOOL m_bFreezeGraffic;
	CHistoDlg m_Graffic;

	void OnOK();
	void OnCancel();
	void OnApply();

	void RestorePosition();
	void SetGrafficMode(BOOL bGrafficMode = TRUE);
	void UpdateControls(int nHint = 0);
	// IRedrawHook
	virtual void OnRedraw();
	virtual void OnDelayedRedraw();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSettingsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSettingsSheet)
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSSHEET_H__F4CF8F72_B596_486E_88F1_5DA2D603AA5B__INCLUDED_)
