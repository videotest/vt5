// BaseDialog.h: interface for the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BaseDialog_H__)
#define __BaseDialog_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "input.h"
#include "BaseDlg.h"
#include "SmartEdit.h"

class CBaseDialog : public CBaseDlg
{
	DECLARE_DYNAMIC(CBaseDialog);
	int m_nCurColor;
	void InitCurColor(int n);
public:

public:
	CBaseDialog(int idd, CWnd *pParent, CBaseDialog *pBase);
	virtual ~CBaseDialog();

	void InitControls();
	void InitControlsState();

protected:
	static BOOL m_bHistogram;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseDialog)
	//}}AFX_MSG
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnGrayscale();
	afx_msg void OnGray();
	afx_msg void OnRed();
	afx_msg void OnGreen();
	afx_msg void OnBlue();
	afx_msg void OnHistogram();
};


#endif
