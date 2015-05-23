#if !defined(AFX_PRINTPROGRESS_H__541FE75C_D37C_4B62_A873_5EEEEE70ADC1__INCLUDED_)
#define AFX_PRINTPROGRESS_H__541FE75C_D37C_4B62_A873_5EEEEE70ADC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintProgress dialog

class CPrintProgress :	public CDialog,
						public CScrollZoomSiteImpl,
						public CNamedObjectImpl,
						public CRootedObjectImpl
{
// Construction
	
	DECLARE_DYNCREATE(CPrintProgress)
	ENABLE_MULTYINTERFACE()
	
public:
	CPrintProgress(CWnd* pParent = NULL);   // standard constructor
	~CPrintProgress();
	void SetActiveDocument( IUnknown* punkDocument){
		m_punkDocument = punkDocument;
	}
	void SetReportForm( IUnknown* punkReportForm ){
		m_pReportForm = punkReportForm;
	}	

// Dialog Data
	//{{AFX_DATA(CPrintProgress)
	enum { IDD = IDD_PRINT_PROGRESS };
	CStatic	m_statusWnd;
	CProgressCtrl	m_progressWnd;
	CStatic	m_previewWnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL m_bKillPreview;
	virtual CScrollBar *GetScrollBarCtrl( int sbCode );
	virtual CWnd *GetWindow();

	

	BOOL InitTemplate();
	BOOL InitActiveDocument();

	BOOL m_bInitOK;
	
	IUnknown* m_pReportForm;

	CStatic m_stPreview;
	IUnknown* m_punkView;
	//IUnknown* m_punkData;
	IUnknown* m_punkContext;

	IUnknownPtr m_punkDocument;

	/*
	CString m_strTemplateName;
	BOOL	m_bTemplateInFile;
	*/

	// Generated message map functions
	//{{AFX_MSG(CPrintProgress)
	virtual BOOL OnInitDialog();	
	afx_msg void OnDestroy();
	afx_msg LRESULT OnPrintProgress(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTPROGRESS_H__541FE75C_D37C_4B62_A873_5EEEEE70ADC1__INCLUDED_)
