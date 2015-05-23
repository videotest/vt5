#if !defined(AFX_INPUTDLG_H__21B6B2FF_F0D2_11D2_BB07_000000000000__INCLUDED_)
#define AFX_INPUTDLG_H__21B6B2FF_F0D2_11D2_BB07_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

class CInputDlg : public CDialog
{
	HWND m_hWndCap;
	CRect m_rcOrgButt;
	LPBITMAPINFOHEADER m_lpbi;
	bool m_bModalState;
	void RecalcLayout(CSize szPrev);
	int m_nDrvNum;
	CString m_sDrvDescr;
// Construction
public:
	CString m_strAVIName;
	static CInputDlg *m_pInputDlg;

	enum CDialogPurpose
	{
		ForSettings = 0,
		ForInput = 1,
		ForAVI = 2,
	};
	CDialogPurpose m_DialogPurpose;

	CInputDlg(int iNum, const char *pszDescr, CWnd* pParent = NULL);   // standard constructor
	~CInputDlg();

	LPBITMAPINFOHEADER SaveImage(); // Only returns previously saved bitmap, not saves itself
	void DoSaveAvi() ;
	void DoSave(); //Process save image. Used in non-modal mode, in modal mode called implicitly inside OnSave
	void BeginModalState(); // These two function affect the user interface
	void EndModalState();

	int GetDrvNum() {return m_nDrvNum;};
	const char *GetDrvDescr() {return m_sDrvDescr;};

// Dialog Data
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_INPUT };
	CButton	m_AVI;
	CButton	m_Cancel;
	CButton	m_Source;
	CButton	m_Save;
	CButton	m_Preview;
	CButton	m_Overlay;
	CButton	m_Display;
	CButton	m_Format;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOverlay();
	afx_msg void OnPreview();
	afx_msg void OnDisplay();
	afx_msg void OnFormat();
	afx_msg void OnSource();
	afx_msg void OnAvi();
	virtual void OnCancel();
	afx_msg void OnSave();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDLG_H__21B6B2FF_F0D2_11D2_BB07_000000000000__INCLUDED_)
