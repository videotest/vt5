#if !defined(AFX_SOURCEDLG_H__35F9CAA0_A36A_11D3_A98A_FB295DF08C32__INCLUDED_)
#define AFX_SOURCEDLG_H__35F9CAA0_A36A_11D3_A98A_FB295DF08C32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceDlg.h : header file
//


interface ISourceSite
{
	// 0 - Reinit all, 1 - all without video std. and channel, 2 - only brightness/contrast
	virtual void Reinit(int nMode) = 0;
};



/////////////////////////////////////////////////////////////////////////////
// CSourceDlg dialog

class CSourceDlg : public CDialog
{
	int m_nDevice;
	int m_nIDD;
// Previous settings
	BOOL	m_bPrevSVideo;
	int		m_nPrevConnector2;
	int		m_nPrevConnector1;
	int		m_nPrevConnectorCam2;
	BOOL	m_bPrevAGC;
	int		m_nPrevHue;
	int		m_nPrevSaturation;
	int		m_nPrevBrightness;
	int		m_nPrevContrast;
	BOOL	m_bPrevVCR;
	UINT	m_nPrevGWDX;
	UINT	m_nPrevGWDY;
	UINT	m_nPrevGWX;
	UINT	m_nPrevGWY;
	int		m_nPrevVideoStandard;
// Construction
	void UpdateControls();
	ISourceSite *m_pSite;
	int  MaxDX();
	int  MaxDY();
	bool IsPalFormat();
	void SetGrabWindow(int x, int y, int cx, int cy);
	unsigned  m_nGWScope;
	unsigned  m_nGWStep;
public:
	CSourceDlg(int nDevice, CWnd* pParent = NULL, int idd = IDD_SOURCE);   // standard constructor

	void SetSite(ISourceSite *pSite) {m_pSite = pSite;}

// Dialog Data
	//{{AFX_DATA(CSourceDlg)
	enum { IDD = IDD_SOURCE };
	CSliderCtrl	m_SGWDY;
	CSliderCtrl	m_SGWDX;
	CSliderCtrl	m_SGWY;
	CSliderCtrl	m_SGWX;
	CSliderCtrl	m_Saturation;
	CSliderCtrl	m_Hue;
	CSliderCtrl	m_Contrast;
	CSliderCtrl	m_Brightness;
	BOOL	m_bSVideo;
	int		m_nConnector2;
	int		m_nConnector1;
	BOOL	m_bAGC;
	int		m_nHue;
	int		m_nSaturation;
	int		m_nBrightness;
	int		m_nContrast;
	BOOL	m_bVCR;
	UINT	m_nGWDX;
	UINT	m_nGWDY;
	UINT	m_nGWX;
	UINT	m_nGWY;
	int		m_nVideoStandard;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSourceDlg)
	afx_msg void On2svideo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void On1stsvideo();
	afx_msg void On2ndsvideo();
	afx_msg void OnAgc();
	afx_msg void OnAnalog();
	afx_msg void OnBlue();
	afx_msg void OnChangeEBrightness();
	afx_msg void OnChangeEContrast();
	afx_msg void OnChangeEHue();
	afx_msg void OnChangeESaturation();
	afx_msg void OnGreen();
	afx_msg void OnRed();
	afx_msg void OnSvideo();
	afx_msg void OnVcr();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	afx_msg void OnStandardNtsc();
	afx_msg void OnStandardNtsc44();
	afx_msg void OnStandardNtsccomb();
	afx_msg void OnStandardNtscmono();
	afx_msg void OnStandardPal();
	afx_msg void OnStandardPalm();
	afx_msg void OnStandardPaln();
	afx_msg void OnStandardSecam();
	afx_msg void OnStandardPal60();
	afx_msg void OnStandardSecammono();
	afx_msg void OnChangeEGwdx();
	afx_msg void OnChangeEGwdy();
	afx_msg void OnChangeEGwx();
	afx_msg void OnChangeEGwy();
	virtual void OnCancel();
	afx_msg void OnRed2();
	afx_msg void OnGreen2();
	afx_msg void OnBlue2();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEDLG_H__35F9CAA0_A36A_11D3_A98A_FB295DF08C32__INCLUDED_)
