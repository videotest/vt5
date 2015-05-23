// BaseDialog.h: interface for the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BaseDialog_H__)
#define __BaseDialog_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "input.h"
#include "SmartEdit.h"
#include "BaseDlg.h"

class CBaseDialog : public CBaseDlg
{
	DECLARE_DYNAMIC(CBaseDialog);
	int m_nCurColor;
	void InitCurColor(int n);
	int m_nCurSlope;
	int m_nIntTimeMax,m_nResetValueMin; // Maximums for current slope!
	void InitCurSlope(int n);
	void InitSlopesCB(int nSlopes);
	void OnChangeValue(CSmartIntEdit *pEdit, int nValue);


	void InitBitsCB(int datamode);
    
public:

public:
	CBaseDialog(int idd, CWnd *pParent, CBaseDialog *pBase);
	virtual ~CBaseDialog();


	void InitControls();

	void InitControlsState();

	BOOL m_bVideo;
protected:
	static BOOL m_bHistogram;
	bool m_bDisableAllCtrls;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseDialog)
//	virtual BOOL OnInitDialog();
	//}}AFX_MSG
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnGrayscale();
	afx_msg void OnBitnessChanged();
	afx_msg void OnBitsChanged();
	afx_msg void OnGray();
	afx_msg void OnRed();
	afx_msg void OnGreen();
	afx_msg void OnBlue();
	afx_msg void OnCurrentSlopeChanged();
	afx_msg void OnChangePathName();
	afx_msg void OnFPSTypeChanged();
	afx_msg void OnSameAsCapture();

	afx_msg void OnDefault();
	afx_msg void OnHistogram();
	afx_msg void OnWBBuild();

};


#endif
