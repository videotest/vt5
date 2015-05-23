// BaseDialog.h: interface for the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEDIALOG_H__83979130_96E8_49FA_952A_DFA70CC54058__INCLUDED_)
#define AFX_BASEDIALOG_H__83979130_96E8_49FA_952A_DFA70CC54058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PixCtrls.h"
#include "input.h"
#include "ExposureMask.h"
#include "DrvPreview.h"


//extern CDriverPreview grab;
class CBaseDialog : public CDialog//, public CCtrlHandler
{
	DECLARE_DYNAMIC(CBaseDialog);
	CBaseDialog *m_pBase;
	CObArray m_arrCtrls;
	bool m_bFocus;
	bool m_bInited;

   

	void InitSlider(int id, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable = TRUE);
	void InitSpin(int id, int nMin, int nMax, int nPos, BOOL bEnable = TRUE);
	void InitSmartEdit(int idSlider, int idSpin, int idEdit, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable = TRUE);
	void InitComboBox(int id, int nCurSel);
	void InitComboBoxValue(int id, int nValue);
	bool IsSmartEdit(int idSlider, int idEdit);
	bool IsControl(int id) {return ::GetDlgItem(m_hWnd, id) != 0;}
	int  GetComboBoxValue(int id);
	void ChangeValue(int idSlider, int idSpin, int idEdit, int nValue);
	void SetSliderPos(int id, int nPos);
	int  GetSliderPos(int id);
	void SetSpinPos(int id, int nPos);

	//sergey 13/01/06
	 int _InitListBoxByReprData(CListBox *pLB,COMBOBOXREPRDATA *pReprData);
	 void InitListBox1( int id, int nCurSel,COMBOBOXREPRDATA *pReprData);

	void InitListBox(int id, int nCurSel);
	void InitListBoxValue(int id, int nValue);	
	int  GetListBoxValue(int id);
	CListBox *pCombo;
	CListBox *pCombo1;
	//COMBOBOXREPRDATA *m_pReprData;
	CString csListTextItem;	// возвращаемое значение
	CString csEdit1TextItem;	// возвращаемое значение имя файла
	CEdit *pEdit;
	
	void InitEdit(int id);
	//int GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg, bool bIgnoreMethodic);
	virtual int GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual double GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual double _GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	void RestoreSettingsS();
	void SaveSettingsS();
	void RestoreLevelAdjustment(int nLevel);
	void DoSelectMethodic(int nMethodic);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);
	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	virtual void OnResetValues() {} // called when all values changes (e.g on change of the methodic)
	virtual void ReadSettings();
	CArray<IDriverDialog *,IDriverDialog *&> m_arrDialogs;
	bool m_bSettingsChanged; // Some settings changed since last call of PreinitGrab
	CExposureMask m_ExposureMask;
	void ResetAllControls();
	void UpdateCmdUI(); // calls IDriverDialog::UpdateCmdUI for all dialogs

	void SaveSettingsNew();
	void RestoreSettingsNew();

	//end

	void InitAutoExposure();
	void InitManualExposure();
	void InitLevelAdjustment(int nLevel);
	void InitCBFactor(int idEdit, int idSpin, int idSlider, double dFactor, BOOL bEnable);
	void EnableWB(int nWBMode);
	void ChangeLevelAdjustment(int nHighlight, int nShadow, double dGamma, unsigned nMask);
	void ChangeWBValues(int nMode = -1);
	void EnableBB(BOOL bBB);
	
public:
	//sergey 18/01/06
	CWnd* cwndGlob;
	
	
  int m_nIdNewMeth,m_nIdDeleteMeth;
  
	
	//end
	
	CBaseDialog(int idd, CWnd *pParent, CBaseDialog *pBase);
	virtual ~CBaseDialog();

	void InitControls();
	void DestroyControls();

	void InitControlsState();
	virtual void OnNewImage() {};
	virtual void OnDelayedNewImage();

	
	
protected:

	//serey 27/01/06
/*	class XStopPreview
	{		

		CDriverPreview *m_pDrv;
		bool m_bStopPreview;
		bool m_bReinitPreview;
	public:
		bool m_bSizeChanged;
		XStopPreview(CDriverPreview *pDrv, bool bStopPreview, bool bReinitPreview)
		{
			m_pDrv = pDrv;
			m_bStopPreview = bStopPreview;
			m_bReinitPreview = bReinitPreview;
			if (m_bStopPreview)
			{
				if (m_pDrv->m_nStopGrabCount == 0)
					m_pDrv->OnStopGrab();
				m_pDrv->m_nStopGrabCount++;
			}
			m_bSizeChanged = false;
		}
		~XStopPreview()
		{
			if (m_bStopPreview)
			{
				m_pDrv->m_nStopGrabCount--;
				if (m_pDrv->m_nStopGrabCount == 0)
				{
					m_pDrv->PreinitGrab2(CaptureMode_Preview, 0);
					if (m_bReinitPreview)
					{
						BITMAPINFO256 bi;
						m_pDrv->GetFormat(false, &bi.bmiHeader);
						m_pDrv->m_Conv16To8.Init(bi.bmiHeader.biBitCount, (int)g_Brightness, (int)g_Contrast,
							double((int)g_Gamma)/100.);
						m_pDrv->m_Dib.InitBitmap(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, (bool)g_Gray?8:24);
						if (m_bSizeChanged)
							m_pDrv->NotifyChangeSizes();
					}
					m_pDrv->OnStartGrab();
				  }
			  }
		  }
	};*/
	
	//end


	

protected:

	bool m_bDelayedInitCtrllSite;
	static BOOL m_bHistogram;
	bool m_bDisableAllCtrls;
	virtual void OnLayoutChanged() {};
   //sergey 05/02/06
	static int m_bChancedProf;
	static int m_bbuld;
	static int m_bNotB;
	//end
	
	 
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseDialog)
	virtual BOOL OnInitDialog();
	
	//}}AFX_MSG
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoExposure();
	afx_msg void OnExpChange();
	afx_msg void OnSensitivityChange();
	afx_msg void OnDeltaposExpTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboSensitivity();
	afx_msg void OnSelchangeLevelAdjustment();
	afx_msg void OnShadowChange();
	afx_msg void OnGammaChange();
	afx_msg void OnHighlightChange();
	afx_msg void OnDeltaposSpinGamma(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAccumulationMode();
	afx_msg void OnAccumulateTimes();
	afx_msg void OnKillFocusAccumulateTimes();
	afx_msg void OnWBChange();
	afx_msg void OnWBRedChange();
	afx_msg void OnWBGreenChange();
	afx_msg void OnWBBlueChange();
	afx_msg void OnWBShowRect();
	afx_msg void OnWBBuild();
	afx_msg void OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlip();
	afx_msg void OnSelchangePreviewMode();
	afx_msg void OnSelchangeCaptureMode();
	afx_msg void OnGrayScale();
	afx_msg void OnMicro();
	afx_msg void OnFocus();
	afx_msg void OnFocusRect();
	afx_msg void OnBBChange();
	afx_msg void OnHistogram();
	afx_msg void OnBBShowRect();
	afx_msg void OnAEShowRect();
	afx_msg void OnTimer(UINT);
	afx_msg void OnDefault();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//sergey 11/01/2006
	afx_msg void OnEdit1Change();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnSave();
	afx_msg void OnFileName();
	afx_msg void OnRestoreMetodick();
	afx_msg void OnNew11();
	afx_msg void OnCancel11();
	//com_call OnCmdMsg(int nCode, UINT nID, long lHWND);

	afx_msg void OnShadowChange1();	
	afx_msg void OnGammaChange1();
	afx_msg void OnHighlightChange1();
	afx_msg void OnExpChange1();
	//end
};


#endif // !defined(AFX_BASEDIALOG_H__83979130_96E8_49FA_952A_DFA70CC54058__INCLUDED_)
