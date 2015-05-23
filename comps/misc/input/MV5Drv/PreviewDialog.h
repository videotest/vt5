#if !defined(AFX_PREVIEWDIALOG_H__EABF2A60_A334_11D3_A98A_D902D1F5C7CF__INCLUDED_)
#define AFX_PREVIEWDIALOG_H__EABF2A60_A334_11D3_A98A_D902D1F5C7CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewDialog.h : header file
//

#include "PreviewStatic.h"
#include "SourceDlg.h"
#include "MV500Grab.h"


/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog dialog

class CPreviewDialog : public CDialog, public ISourceSite, public IMV500ProgressNotify
{
// Construction
	CSize m_szButton;
	void RecalcLayout();
	int m_nMax;
public:
	CPreviewDialog(int nDevice, CWnd* pParent = NULL);   // standard constructor
	virtual void Reinit(int nMode);
	virtual void Start(const char *psz, int nMax);
	virtual void End();
	virtual void Notify(int nCur);
	bool m_bNotDestroy;
	enum WindowType
	{
		ForSettings = 0,
		ForImageCapture = 1,
		ForAVICapture = 2,
	};
	WindowType m_WindowType;
	CString m_strAVI;

// Dialog Data
	//{{AFX_DATA(CPreviewDialog)
	enum { IDD = IDD_PREVIEW };
	CButton	m_AVISettings;
	CButton	m_Cancel;
	CButton	m_Reset;
	CProgressCtrl	m_Progress;
	CStatic	m_Message;
	CStatic	m_Frames;
	CButton	m_OK;
	CButton	m_Source;
	CButton	m_Format;
	CPreviewStatic	m_PreviewStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreviewDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSource();
	afx_msg void OnFormat();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCaptureVideo();
	afx_msg void OnReset();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWDIALOG_H__EABF2A60_A334_11D3_A98A_D902D1F5C7CF__INCLUDED_)
