#if !defined(AFX_PRINTPREVIEW_H__79586C2C_D958_477E_8C68_30D22035FDBF__INCLUDED_)
#define AFX_PRINTPREVIEW_H__79586C2C_D958_477E_8C68_30D22035FDBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintPreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintPreview dialog

#define IDC_CONTEXT_AX	1005

#include "ReportConstructor.h"

#include "HelpDlgImpl.h"

class CPrintPreview;

class CEvRouter : public CCmdTargetEx, public CEventListenerImpl
{	
	CPrintPreview *m_ppv;
public:
	CEvRouter( CPrintPreview *ppv )
	{m_ppv = ppv;}


	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

};


class CPrintPreview  :	public CDialog,
						public CScrollZoomSiteImpl,
						public CNamedObjectImpl,
						public CRootedObjectImpl,
						public CNumeredObjectImpl,
						public CEventListenerImpl,
						public CHelpDlgImpl

{
	//Print To File
	BITMAPINFOHEADER	m_bmih;
	byte	*m_pBities;
	CDC *m_pDC;
	CRect m_rcClientPrint;

	CString m_strFileName;
	int m_nMultiPage;

	DECLARE_DYNCREATE(CPrintPreview)
	ENABLE_MULTYINTERFACE()


// Construction
public:
	CPrintPreview(CWnd* pParent = NULL);   // standard constructor
	~CPrintPreview();

	bool Init( );
	bool DeInit();

	TemplateSource			m_templateSource;
	CString					m_strTemplateName;
	CEvRouter				*m_pEvRouter;

	AutoTemplateAlghoritm	m_autoTemplateAlghoritm;


	void SetActiveDocument( IUnknown* punkDocument ){
		m_ptrTargetDocument = punkDocument;
	}
	CPoint m_ptOffset;
// Dialog Data
	//{{AFX_DATA(CPrintPreview)
	enum { IDD = IDD_PRINT_PREVIEW };
	CButton	m_WndCancel;
	CButton	m_wndOK;
	CStatic	m_wndStart;
	CStatic	m_previewWnd;
	CComboBox	m_zoomWnd;
	CScrollBar	m_VertScrlWnd;
	CScrollBar	m_HorzScrlWnd;
	int		m_nTemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintPreview)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintProgress(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	void SetNewScrollPos();
	virtual CScrollBar *GetScrollBarCtrl( int sbCode );
	virtual CWnd *GetWindow();

	void RecalcLayout();

	/*
	bool InitTemplate();	
	bool InitActiveDocument();
	*/

	bool KillPreviewFromActiveDocument();

	bool m_bInitOK;
	
	//Aggregates
	IUnknown* m_punkView;	
	IUnknown* m_punkContext;

	CStatic m_stPreview;		

	IUnknownPtr m_ptrTargetDocument;	
	IUnknownPtr m_ptrDocReportCopyFrom;		


//	IUnknownPtr m_ptrOldAciveView;
	GuidKey m_guidOldActiveView; // this view can be already closed !

	
	bool CreatePreviewObject();
	bool CreateAggregates();	
	
	BOOL ProcessMainFrameAccel( MSG* pMsg );

	// Generated message map functions
	//{{AFX_MSG(CPrintPreview)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPrintSetup();
	afx_msg void OnPrintToFile();
	afx_msg void OnPageSetup();
	afx_msg void OnPrint();
	afx_msg void OnEditchangeZoom();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnSelendokZoom();
	afx_msg void OnSelchangeTemplateName();	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtnPageColor();
	afx_msg void OnPaint();
	afx_msg void OnTemplateSetup();
	afx_msg void OnHelp();
	//}}AFX_MSG

	afx_msg void OnCommandToolBar(UINT nID);
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()


	int GetTemplateNameIndex();
	void FillTemplateCombo();	


	CString m_strAutoRptActiveView;
	CString m_strAutoRptOneObjectPerPage;
	CString m_strAutoRptVerticalAsIs;
	CString m_strAutoRptGallery;
	CString m_strAutoRptFromSplitter;


	//Add control support
	CToolBarCtrl m_toolbarAXCtrl;
	void CreateToolbar();
	CStringArray	m_arrProgIDs;	
	void _AddCtrlButton(CString strProgID, int nCodeID);
	HBITMAP _GetBitmapFromProgID(CString strProgID);
	CPtrArray		m_bitmaps;

	IUnknown* GetAXPropertyPage();
	void UnCheckCurrentButton();

	friend class CEvRouter;
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

protected:


	CWnd m_ctrlContextAX;


	void TerminateInteractive();

	bool RunAutoTemplateSetup();

	bool m_bLockCheckButton;

	
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	int		m_ntoolbar_height;
	bool	is_read_only();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTPREVIEW_H__79586C2C_D958_477E_8C68_30D22035FDBF__INCLUDED_)
