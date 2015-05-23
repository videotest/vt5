#if !defined(AFX_ZOOMNAVIGATORPAGE_H__9BF01721_0B24_4F20_ABA9_83E77443E3B9__INCLUDED_)
#define AFX_ZOOMNAVIGATORPAGE_H__9BF01721_0B24_4F20_ABA9_83E77443E3B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZoomNavigatorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZoomNavigatorPage dialog

#define ZOOM_TICK_STEP			256

#define WM_SET_ZOOM	WM_USER + 10


class CZoomNavigatorPage : public CPropertyPageBase,
							public CEventListenerImpl
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CZoomNavigatorPage);
	GUARD_DECLARE_OLECREATE(CZoomNavigatorPage);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

// Construction
public:
	CZoomNavigatorPage();   // standard constructor
	~CZoomNavigatorPage();

// Dialog Data
	//{{AFX_DATA(CZoomNavigatorPage)
	enum { IDD = IDD_ZOOM_NAVIGATOR_PPG };
	CSliderCtrl	m_ctrlSlider;
	CComboBox	m_crtlCombo;
	int		m_nSliderPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomNavigatorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZoomNavigatorPage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClickZoomOut();
	afx_msg void OnClickZoom100();
	afx_msg void OnClickZoomIn();
	afx_msg void OnClickFeetToScreen();
	afx_msg void OnClickFeetToDoc();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeZoom();	
	afx_msg void OnEditchangeZoom();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	IUnknown* GetActiveView();
	IUnknown* GetActveObject();
	BYTE* GetThumbnail( IUnknown* punkObject, CSize sizeThumbnail );

	bool GetViewRectOnPage( IUnknown* punkView, BYTE* pbi, CRect& rcImage, CRect& rcVisible );


	CRect m_rcImage;
	CRect m_rcVisible;
	bool m_bStartDrag;	
	CPoint m_ptStartDragMousePos;


	CPoint m_viewSrollPosBeforDrag;
	double m_fZoomV2T;


protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	void RegisterDoc( IUnknown* punkDoc );
	void UnRegisterDoc( );

	IUnknownPtr m_ptrActiveDoc;

	IUnknown* GetActiveDocument();


	void InvalidatePreviewWnd();

	GuidKey	m_guidActiveObject;
	BYTE*	m_pThumbnailCache;	

	BEGIN_INTERFACE_PART(ZoomPage, IZoomPage)
		com_call Update(DWORD dwOptions);
	END_INTERFACE_PART(ZoomPage);

	bool UpdatePage( DWORD dwOptions );

public:
	static double FormatZoomFromS2D( CString strZoom );
	static CString FormatZoomFromD2S( double fZoom );


	static int ConvertFromDoubleToTick( double fZoom );
	static double ConvertFromTickToDouble( int nTick );

protected:	
	void SetZoomToCombo( double fZoom );
	void SetZoomToSlider( double fZoom );

	bool SetZoom( double fZoom );


	//void DrawFrameRect(CDC * pDC, CRect &rect);

	HCURSOR m_hMovePaneCur;

	bool m_bPageFireScrollEvent;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOMNAVIGATORPAGE_H__9BF01721_0B24_4F20_ABA9_83E77443E3B9__INCLUDED_)
