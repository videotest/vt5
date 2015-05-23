// SettingsPages.h : header file
//

#ifndef __SETTINGSPAGES_H__
#define __SETTINGSPAGES_H__

#include "ExponentSlider.h"

class CSettingsSheet;
class CSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSettingsPage);
public:
	CSettingsPage(int nPage = 0) : CPropertyPage(nPage) {m_pSheet = NULL;};
	CSettingsSheet *GetSettingsParentSheet();

	CSettingsSheet *m_pSheet;

	void SetModified(BOOL bChanged = TRUE);
	virtual void UpdateControls(int nHint = 0) {};
};

/////////////////////////////////////////////////////////////////////////////
// CAmplPage dialog

class CAmplPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CAmplPage)
	CString m_sAutoSetup;
	BOOL m_bHardwareSetup; // Hardware setup in process

// Construction
public:
	CAmplPage();
	~CAmplPage();

// Dialog Data
	//{{AFX_DATA(CAmplPage)
	enum { IDD = IDD_AMPL };
	CButton	m_ShutterAuto;
	CSpinButtonCtrl	m_SpShutter;
	CSpinButtonCtrl	m_SpOffset;
	CSpinButtonCtrl	m_SpGain;
	CSpinButtonCtrl	m_SpProcWhite;
	CSliderCtrl	m_SProcWhite;
	CEdit	m_EProcWhite;
	CButton	m_Auto;
	CStatic	m_StaticShutter;
	CExponentSlider	m_SShutter;
	CEdit	m_EShutter;
	CSliderCtrl	m_SOffset;
	CEdit	m_EOffset;
	CSliderCtrl	m_SGain;
	CEdit	m_EGain;
	CProgressCtrl	m_ProgressAuto;
	//}}AFX_DATA

	void UpdateControls(int nHint = 0);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAmplPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAmplPage)
	afx_msg void OnAutoSetup();
	afx_msg void OnChangeEGain();
	afx_msg void OnChangeEOffset();
	afx_msg void OnChangeEShutter();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnChangeEProcWhite();
	afx_msg void OnDeltaposSpShutter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShutterAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CCameraPage dialog

class CCameraPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CCameraPage)

//	void SetTopBottom();
// Construction
public:
	CCameraPage();
	~CCameraPage();

// Dialog Data
	//{{AFX_DATA(CCameraPage)
	enum { IDD = IDD_CAMERA };
	CComboBox	m_Camera;
	CComboBox	m_Bits;
	BOOL	m_bGraffic;
	//}}AFX_DATA

	void UpdateControls(int nHint = 0);
	void InitCameraCB();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCameraPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCameraPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCamera();
	afx_msg void OnAgp();
	afx_msg void OnBining();
	afx_msg void OnGraffic();
	afx_msg void OnSelchangeComboBits();
	afx_msg void OnSelchangeComboColorPlane();
	afx_msg void OnGrayScale();
	afx_msg void OnNaturalSize();
	afx_msg void OnCameraSystemManager();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CWhitePage dialog

class CWhitePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CWhitePage)

// Construction
public:
	CWhitePage();
	~CWhitePage();

// Dialog Data
	//{{AFX_DATA(CWhitePage)
	enum { IDD = IDD_WHITE_BALANCE };
	CSpinButtonCtrl	m_SpWBRed;
	CSpinButtonCtrl	m_SpWBBlue;
	CSpinButtonCtrl	m_SpWBGreen;
	CButton	m_WBDefault;
	CSliderCtrl	m_SWBBlue;
	CSliderCtrl	m_SWBGreen;
	CSliderCtrl	m_SWBRed;
	CEdit	m_EWBBlue;
	CEdit	m_EWBGreen;
	CEdit	m_EWBRed;
	CButton	m_WBEnable;
	//}}AFX_DATA


	void UpdateControls(int nHint = 0);
	void DoChangeValue();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWhitePage)
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
	//{{AFX_MSG(CWhitePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnWbEnable();
	afx_msg void OnWbDefault();
	afx_msg void OnChangeEWbBlue();
	afx_msg void OnChangeEWbGreen();
	afx_msg void OnChangeEWbRed();
	afx_msg void OnDeltaposSpRed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpBlue(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CColorPage dialog

class CColorPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CColorPage)

// Construction
public:
	CColorPage();
	~CColorPage();

// Dialog Data
	//{{AFX_DATA(CColorPage)
	enum { IDD = IDD_COLOR_BALANCE };
	CSpinButtonCtrl	m_SpCBRed;
	CSpinButtonCtrl	m_SpCBGreen;
	CSpinButtonCtrl	m_SpCBBlue;
	CSliderCtrl	m_SCBBlue;
	CSliderCtrl	m_SCBGreen;
	CSliderCtrl	m_SCBRed;
	CEdit	m_ECBBlue;
	CEdit	m_ECBGreen;
	CEdit	m_ECBRed;
	CButton	m_CBEnable;
	//}}AFX_DATA


	void UpdateControls(int nHint = 0);
	void DoChangeValue();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
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
	//{{AFX_MSG(CColorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbEnable();
	afx_msg void OnChangeECbBlue();
	afx_msg void OnChangeECbGreen();
	afx_msg void OnChangeECbRed();
	afx_msg void OnDeltaposSpCbBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpCbGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpCbRed(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CFramePage dialog

class CFramePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CFramePage)

// Construction
public:
	CFramePage();
	~CFramePage();

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
	BOOL m_bPreviewAllFrame;
	//}}AFX_DATA

	BOOL m_bPreviewAllFrameOld;

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



#endif // __SETTINGSPAGES_H__
