// SettingsSheet.h : header file
//
// This class defines custom modal property sheet 
// CSettingsSheet.
 
#ifndef __SETTINGSSHEET_H__
#define __SETTINGSSHEET_H__

#include "SettingsPages.h"
#include "SettingsPages1.h"
#include "HistoDlg.h"
#include "input.h"

interface ISettingsSite
{
	virtual void OnChangeOvrMode(bool bOvrMode) = 0;
	virtual void OnChangeNaturalSize(bool bNaturalSize) = 0;
};

interface IRedrawHook
{
	virtual void OnRedraw() = 0;
	virtual void OnDelayedRedraw() = 0;
};

int __Range(int n, int nMin, int nMax);
void SetSlider(CSliderCtrl &Sld, BOOL bEnable, int nPos, int nMin, int nMax);
void SetSlider(CSliderCtrl &Sld, CSpinButtonCtrl &Spin, BOOL bEnable, int nPos, int nMin, int nMax);

struct BitsModeDescr
{
	int nNum;
	int nBitsMode;
	int nConversion;
	int idStr;
};

BitsModeDescr *BitsModeDescrByNum(int nNum);
BitsModeDescr *BitsModeDescrByName(LPCTSTR lpstrName);
BitsModeDescr *BitsModeDescrByBM(int nBitsMode, int nConversion);
void InitComboByModes(CComboBox *pCombo, int nCurSelNum);

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet

enum ESettingsHint
{
	ESH_Start_Auto = 1,
	ESH_Stop_Auto = 2,
	ESH_Camera_Changed = 3,
	ESH_Binning = 4,
	ESH_BitMode = 5,
	ESH_Trigger = 6,
};

class CSettingsSheet : public CPropertySheet, /*public IRedrawHook, */public IDriverSite
{
	DECLARE_DYNAMIC(CSettingsSheet)
	bool m_bInited;
	IDriver3Ptr m_sptrDrv3;

// Construction
public:
	CSettingsSheet(ISettingsSite *pSite, IUnknown *punkDriver, CWnd* pWndParent = NULL);

	bool m_bOvrMode;
	bool m_bCameraChanged;
	bool m_bOvrModeChanged;
	unsigned  m_nMaxMarginX;
	unsigned  m_nMaxMarginY;
	ISettingsSite *m_pSettingsSite;
//	STT *m_pSTT;
//	unsigned m_nMaxShutter;
	BOOL m_bFinished; // OnOK or OnCancel already called
	BOOL m_bModified;
	BOOL m_bGraffic;
	BOOL m_bFreezeGraffic;
	CRect m_rcNonGraffic,m_rcOk,m_rcCancel,m_rcApply,m_rcHelp;

	CHistoDlg m_Graffic;

	void ReadSettings();
	void WriteSettings();
	void InitCamera();
//	void FormatShutterMessages(CString &sHutter, CString &sValue);
//	int  IndexByShutterH(int nShutter);
//	int  IndexByShutterL(int nShutter);
	void UpdateControls(int nHint = 0);
//	void InitShutterValues();

	void OnOK();
	void OnCancel();
	void OnApply();

	void RestorePosition();
	void SetGrafficMode(BOOL bGrafficMode = TRUE);

/*	// IRedrawHook
	virtual void OnRedraw();
	virtual void OnDelayedRedraw();*/

	// IDriverSite
	virtual ULONG __stdcall AddRef() {return 1;}
	virtual ULONG __stdcall Release() {return 1;}
	com_call QueryInterface(const IID &iid, void **pret) {return E_NOINTERFACE;}
	com_call Invalidate();
	com_call OnChangeSize();


// Attributes
public:
	CAmplPage m_PageAmpl;
	CCameraPage m_PageCam;
	CManualPage m_ManualPage;
	CWhitePage m_PageWB;
	CColorPage m_PageCB;
	CFramePage m_PageFrame;
	CTriggerDlg m_PageTrigger;

	int		m_nGain;
	BOOL    m_bAutoShutter;
	int		m_nShutter; // Shutter in microseconds
	int		m_nOffset;
	int		m_bLowSpeed;
	BOOL	m_bCBEnable;
	BOOL	m_bWBEnable;
	double	m_dWBBlue;
	double	m_dWBGreen;
	double	m_dWBRed;
	double	m_dCBGreen;
	double	m_dCBRed;
	double	m_dCBBlue;
	BOOL	m_bOverlay;
	int		m_nCamera;
	BOOL    m_bCameraDisabled;
	int     m_nBitsMode;
	BOOL	m_bAGP;
	BOOL	m_bBinning;
	BOOL    m_bGrayScale;
	int     m_nPlane;
	UINT	m_nLeftMargin;
	UINT	m_nRightMargin;
	UINT	m_nTopMargin;
	UINT	m_nBottomMargin;
	int     m_nProcWhite;
	BOOL    m_bEnableMargins;
	BOOL    m_bNaturalSize;
	int     m_nConvStart;
	int     m_nConvEnd;
	double  m_dGamma;
	int     m_nConvStart12;
	int     m_nConvEnd12;
	double  m_dGamma12;
	double &Gamma();
	int    &ConvStart();
	int    &ConvEnd();
	BOOL    m_bTriggerMode;
	BOOL    m_bDischarge;
	BOOL    m_bDC300QuickView;

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
	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SETTINGSSHEET_H__
