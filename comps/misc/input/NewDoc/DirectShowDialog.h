#if !defined(AFX_DIRECTSHOWDIALOG_H__326DCF8D_59AF_4477_A4E8_5336124904AB__INCLUDED_)
#define AFX_DIRECTSHOWDIALOG_H__326DCF8D_59AF_4477_A4E8_5336124904AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectShowDialog.h : header file
//

#include "Input.h"
#include "Reposition.h"

/////////////////////////////////////////////////////////////////////////////
// DirectShowDialog dialog

class DirectShowDialog : public CDialog, public CRepositionManager
{
public:
	enum CDialogPurpose
	{
		ForSettings = 0,
		ForInput = 1,
		ForAVI = 2,
	};
protected:
	IInputWindowPtr m_sptrIV;
	IDriver4Ptr m_sptrDrv;
	int m_nDev;
	long m_lWnd;
	long m_lCapStartTime,m_nTimeLimit;
	void RepositionWindow();
	CRect m_rcBase;
	CDialogPurpose m_Purpose;
// Construction
public:
	DirectShowDialog(IUnknown *punkDrv, int nDev, CDialogPurpose Purpose = ForInput, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DirectShowDialog)
	enum { IDD = IDD_DSHOW };
	CProgressCtrl	m_VideoProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DirectShowDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DirectShowDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSettings();
	afx_msg void OnCrossbar();
	afx_msg void OnTuner();
	afx_msg void OnFormat();
	afx_msg void OnVideoSettings();
	virtual void OnOK();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTSHOWDIALOG_H__326DCF8D_59AF_4477_A4E8_5336124904AB__INCLUDED_)
