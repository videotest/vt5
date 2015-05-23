// AbacusViewerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAbacusViewerDlg dialog
class CAbacusViewerDlg : public CDHtmlDialog
{
// Construction
public:
	CAbacusViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ABACUSVIEWER_DIALOG, IDH = IDR_HTML_ABACUSVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

	void AddAbacusData();
	void SaveAbacusData();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	virtual void PostNcDestroy();
public:
	CListCtrl m_List;
	afx_msg void OnBnClickedButtonView();
	afx_msg void OnTimer(UINT nIDEvent);
};
