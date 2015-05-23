#if !defined(AFX_SPLASHWINDOW_H__306E3A95_6B4F_4028_80CA_F30B9C80D7E5__INCLUDED_)
#define AFX_SPLASHWINDOW_H__306E3A95_6B4F_4028_80CA_F30B9C80D7E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashWindow dialog
class CMainFrame;

class CSplashWindow : public CDialog
{
// Construction
public:
	CSplashWindow( CMainFrame*pfrm );   // standard constructor
	~CSplashWindow();

// Dialog Data
	//{{AFX_DATA(CSplashWindow)
	enum { IDD = IDD_SPLASH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	void Create();
	bool LoadImage( const char *pszImage );
	void DeInit();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplashWindow)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BITMAPINFOHEADER	*m_pbi;
	byte				*m_pdib;
	CMainFrame			*m_pframe;
};

bool CreateLogoWnd();
/////////////////////////////////////////////////////////////////////////////
//   Logo class

class CLogoWnd : public CWnd
{
// Construction
protected:
	CLogoWnd();

public:		

// Attributes:
protected:
	HBITMAP		m_hBitmap;
	CBitmap		m_bitmap;
	bool		LoadBitmap();

	bool 		LoadSettings();

	CString		m_str_image_path;
	CRect		m_rect_indicator;
	CSize		m_size_image;
	bool		m_benable_indicator;
	long		m_lindicator_type;
	long		m_lcur_stage;
	long		m_lmax_stage;
	
	long		m_lindicator_len;
	long		m_lindicator_tail_len;
	long		m_lstep;


	COLORREF	m_clr_back;
	COLORREF	m_clr_high;
	
	long		m_llock;
	long		m_lstart_tick;
	long		m_lcan_destroy_by_click;

// Operations
public:
	static void ShowLogoWnd(CWnd* pParentWnd );
	static void DelayDestroy();
	static void Lock( long llock );
	static void CanDestroyByClick( long ldestroy );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CLogoWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideLogoWnd();
	void StartDestroy();
	void ProcessDrawing( CDC* pdc, CRect rcUpdate, BITMAPINFOHEADER* pbi, byte *pdib );

public:
	static CLogoWnd* m_pLogoWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CLogoWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHWINDOW_H__306E3A95_6B4F_4028_80CA_F30B9C80D7E5__INCLUDED_)
