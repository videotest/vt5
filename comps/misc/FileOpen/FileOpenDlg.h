#if !defined(AFX_FILEOPENDLG_H__B189049A_7180_11D3_A4F6_0000B493A187__INCLUDED_)
#define AFX_FILEOPENDLG_H__B189049A_7180_11D3_A4F6_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileOpenDlg.h : header file
//
//#include "GrayListBox.h"

enum DlgModes {dmList = 1, dmReport, dmPreview};

#include "HelpDlgImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CFileOpenDlg command target

class CFileOpenDlg : public CFileDialog,
					 public COpenFileDialogImpl,
					 public CScrollZoomSiteImpl,
					 public CRootedObjectImpl,
					 public COleEventSourceImpl,
					 public CMsgListenerImpl,
					 public CNamedObjectImpl,
					 public CHelpDlgImpl
{
	DECLARE_DYNCREATE(CFileOpenDlg)
	ENABLE_MULTYINTERFACE()
	ROUTE_OLE_VIRTUALS()
	
	CFileOpenDlg();
// Attributes
public:
	//CEdit m_edFileInfo;
	//CGrayListBox m_lbFileInfo;
	CButton m_lbFileInfo;
	CStatic m_stFileInfo;

	CStatic m_stPreview;
	//CWnd m_stPreview;
	CFont m_fontInfo;
	CProgressCtrl m_Progress;
	int m_nListBoxWidth;
	int m_nListBoxHeight;
	int m_nMinDlgWidth;
	int m_nMinDlgHeight;
	bool m_bCurViewNeedScroll;
	bool m_bNeedShowFileInfo;
	bool m_bNewPreview;	
	CRect m_rectPreview;
	CString m_strLastPath;
	CString m_strType;
	int m_nExtention;
	CWndSizer m_stSizer;
	CScrollBar* m_pscrollHorz;
	CScrollBar* m_pscrollVert;

	//sptrINamedData		m_sptrData; 
	//sptrIDataContext	m_sptrContext; 
	//sptrIWindow			m_sptrWin;
	sptrILongOperation	m_sptrLong;

	IUnknown* m_punkData;
	IUnknown* m_punkContext;
	IUnknown* m_punkView;

	sptrIWindow2		m_sptrMainFrame;

	DlgModes			m_curMode;
	bool				m_bWeUnder98Chikaka;
	bool				m_bSeveralFilesInUse;
	CRect				m_rcLastDraw;
	int                 m_nRes;	
	
// Operations
public:

	virtual int Execute(DWORD dwFlags);
	virtual void GetOFN(LPOPENFILENAME& lpofn);
	virtual CWnd *GetWindow();//{return (CWnd*)GetWindowFromUnknown(m_punkView);};//&m_stPreview;};
	virtual bool SaveState(IUnknown *punkDD, const char* pszSection = "");
	virtual bool LoadState(IUnknown *punkDD, const char* pszSection = "");
	virtual void GetDlgWindowHandle( HWND *phWnd );
	virtual CScrollBar *GetScrollBarCtrl( int sbCode );

	virtual BOOL OnFileNameOK();

	void DoRecalcLayout();
	void OnPreview();
	void OnEndPreview();
	void OnFileNameChange();
	void DoFileNameChange();
	void OnTypeChange();
	void OnFolderChange();
	void OnInitDone();
	void OnOurSize(CPoint point);
	void OnEndSize();
	void OnStartSize();


	CString	GetCurrentFilter();
	void	SetCurrentFilter();



	// ClassWizard generated virtual 
	

// Overridesfunction overrides
	//{{AFX_VIRTUAL(CFileOpenDlg)
	public:
	virtual void OnFinalRelease();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL
	//virtual BOOL OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );

// Implementation
protected:
	virtual ~CFileOpenDlg();

	// Generated message map functions
	//{{AFX_MSG(CFileOpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	afx_msg LRESULT OnLongNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CFileOpenDlg)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFileOpenDlg)
	afx_msg long GetFlags();
	afx_msg void SetFlags(long nNewValue);
	afx_msg BSTR GetTitle();
	afx_msg void SetTitle(LPCTSTR lpszNewValue);
	afx_msg BSTR GetFileName();
	afx_msg void SetFileName(LPCTSTR lpszNewValue);
	afx_msg BSTR GetExtentions();
	afx_msg void SetExtentions(LPCTSTR lpszNewValue);
	afx_msg BOOL GetPreview();
	afx_msg void SetPreview(BOOL bNewValue);
	afx_msg BSTR GetDefaultExtention();
	afx_msg void SetDefaultExtention(LPCTSTR lpszNewValue);
	afx_msg BSTR GetInitialDir();
	afx_msg void SetInitialDir(LPCTSTR lpszNewValue);
	afx_msg BOOL SaveDlgState(LPUNKNOWN punkNameData, LPCTSTR pszSection);
	afx_msg BOOL LoadDlgState(LPUNKNOWN punkNameData, LPCTSTR pszSection);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_EVENT_MAP()
	DECLARE_OLETYPELIB(CFileOpenDlg)
	enum {
	//{{AFX_DISP_ID(CFileOpenDlg)
	dispidFlags = 1L,
	dispidTitle = 2L,
	dispidFileName = 3L,
	dispidExtentions = 4L,
	dispidPreview = 5L,
	dispidDefaultExtention = 6L,
	dispidInitialDir = 7L,
	dispidSaveState = 8L,
	dispidLoadState = 9L,
	eventidOnBeforeExecute = 1L,
	eventidOnDialogInited = 2L,
	eventidOnAfterExecute = 3L,
	//}}AFX_DISP_ID
	};
	//{{AFX_EVENT(CFileOpenDlg)
	void FireOnBeforeExecute()
		{FireEvent(eventidOnBeforeExecute,EVENT_PARAM(VTS_NONE));}
	void FireOnDialogInited()
		{FireEvent(eventidOnDialogInited,EVENT_PARAM(VTS_NONE));}
	void FireOnAfterExecute()
		{FireEvent(eventidOnAfterExecute,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT

protected:
	int m_nWidthDlg;
	int m_nHeightDlg;
	CString			m_strExt;
	bool m_bDrawContentWhileDrag;

	COleDropTarget	m_target;

	CString m_strTitle;
	CString m_strDefExt;

public:
	//favorites
	CStringArray	m_arrFavorites;
};

void _SetDefaultExt(int nFilter = 0);
void CALLBACK ___TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEOPENDLG_H__B189049A_7180_11D3_A4F6_0000B493A187__INCLUDED_)
