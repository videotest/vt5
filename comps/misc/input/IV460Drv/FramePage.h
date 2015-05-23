#if !defined __FrmePage_H__
#define __FrmePage_H__

#include "SettingsSheet.h"
#include "DriverWrp.h"

/////////////////////////////////////////////////////////////////////////////
// CFramePage dialog

class CFramePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CFramePage)

// Construction
public:
	CFramePage();
	~CFramePage();

	CDriverWrp m_pDrv;

// Dialog Data
	//{{AFX_DATA(CFramePage)
	enum { IDD = IDD_FRAME };
	CEdit	m_EBottomMargin;
	CEdit	m_ETopMargin;
	CEdit	m_ERightMargin;
	CEdit	m_ELeftMargin;
	CSpinButtonCtrl	m_STopMargin;
	CSpinButtonCtrl	m_SRightMargin;
	CSpinButtonCtrl	m_SLeftMargin;
	CSpinButtonCtrl	m_SBottomMargin;
	//}}AFX_DATA

	void UpdateControls(int nHint = 0);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFramePage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFramePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeELeftMargin();
	afx_msg void OnChangeERightMargin();
	afx_msg void OnChangeETopMargin();
	afx_msg void OnChangeEBottomMargin();
	afx_msg void OnPreviewAllFrame();
	afx_msg void OnEnableFrame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};






#endif