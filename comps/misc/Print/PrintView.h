#ifndef __printview_h__
#define __printview_h__



//class CContainerWnd;

#include "wndmisc5.h"
#include "axbaseview.h"
#include "ruler.h"



enum PointPlace{
	ppAlign, 
	ppField,
	ppWork,
	ppOutside	
};
				  

struct DocumentObject{	
	CString strType;
	CString strObjName;
	BOOL bUse;
};


class CControlInfo
{
public:	
	CControlInfo();
	~CControlInfo();
	IUnknown* m_pCtrlData;		
	CRect m_rectOnPage;
	BOOL  m_bFirstEntry;	
	CPoint m_ptOffset;
	CSize m_ctrlSize;

	CSize m_sizeViewCtrl;
	CSize m_sizeUserOffset;
	CSize m_sizeUserOffsetDX;
};

class CPage
{
	
public:	
	CPage();
	~CPage();
	void AddCtrl( IUnknown* pCtrlData,
					CRect rcOnPageRect, CSize ctrlSize, CPoint ptOffset,
					BOOL bFirstEntry,
					CSize sizeViewCtrl,
					CSize sizeUserOffset,
					CSize sizeUserOffsetDX
					);
	void RemoveCtrl(  IUnknown* pCtrlData );

	CPoint GetClientStartPoint();
	void SetClientStartPoint( CPoint pt );


	void SetPaperSize ( CSize size );
	void SetPaperAlign( CRect rect );
	void SetPaperField( CRect rect );


	CRect  GetRectPage ( );
	CRect  GetRectAlign( );
	CRect  GetRectField( );

	void GetPageColRow(int& nCol, int& nRow);
	void SetPageColRow(int nCol, int nRow);

	void SetMainPage( int nMainPage );
	int GetMainPage( );
	BOOL IsMainPage();


	PointPlace GetPointPlace( CPoint point );

	CArray <CControlInfo*, CControlInfo*> m_ctrl;

	BOOL IsEmptyPage();
		

	//pos
	int m_nPageCol;
	int m_nPageRow;
		
	CPoint m_ptClient;

	CSize m_pageSize;
	CRect m_pageAlign;
	CRect m_pageField;
	
	int  m_nMainPage;

};


class CPrintView : public CAxBaseView
{
	bool m_bQuasiPrint;
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO();

	DECLARE_DYNCREATE(CPrintView);
	GUARD_DECLARE_OLECREATE(CPrintView);

	DECLARE_MESSAGE_MAP();
	DECLARE_INTERFACE_MAP();
protected:		
	BEGIN_INTERFACE_PART(ReportVw, IReportView2)
		com_call GetMode( int* pnMode );
		com_call EnterDesignMode();
		com_call EnterPreviewMode( DWORD dwFlags );
		com_call EnterPrintMode( );	
		com_call UpdateView();
		com_call FitToPage();
		com_call FitToPageWidth();
		com_call ScrollToPage( int nPageNum, RECT rcTarget);
		com_call GetPageCount( int* pnPageCount );	
		com_call SetTargetDocument( IUnknown* punkDocSite );	
		com_call LeavePreviewMode( );
		com_call OnPrinterSettingsChange();
		com_call DisconnectNotifyController( BOOL bDisconnect );
		com_call SetReportSourceDocument( IUnknown* punkDoc );

		com_call Draw( HDC hdc );
		com_call GetPageRect( HDC hDC, RECT *pRect );
	END_INTERFACE_PART(ReportVw);

	BEGIN_INTERFACE_PART(PageVw, IReportPageView)
		com_call DrawPage(HDC hDC, int idx, float zoom);
		com_call GetZoom(float* zoom);
	END_INTERFACE_PART(PageVw);

	void _draw( CDC *dc, CRect rcUpdate );
	
	//cache
	//cache view
	void	update_zoom_scroll();
	CPoint	get_scroll_pos();
	double	get_zoom();

	CPoint	m_pt_scroll;
	double	m_fzoom;


	//cache printer
	void	update_printer( CDC* pdc );
	CRect	convert_from_vt_to_prn( CRect rc_vt );
	CPoint	convert_from_vt_to_prn( CPoint pt_vt );
	
	long	m_lpaper_width_px, m_lpaper_height_px;
	long	m_lpaper_width_mm, m_lpaper_height_mm;
	long	m_lpaper_offset_x_px, m_lpaper_offset_y_px;


	//new drawing
	void	on_paint( );
	void	do_draw( CDC* pdc, CRect& rc_update );
	void	do_print( CDC* pdc );
	void	draw_page_content( CDC* pdc, int idx_page, CRect& rc_update, bool bprint );
	void	draw_empty_page( CDC* pdc, int idx_page, CRect& rc_update );
	void    draw_report_empty( CDC* pdc, CSize *pCalc = NULL );
	bool	is_rect_visible( CRect& rc_vt, CRect& rc_update );

	CRect	convert_from_vt_to_window( CRect rc_vt );
	CPoint	convert_from_vt_to_window( CPoint pt_vt );
	CRect	convert_from_window_to_vt( CRect rc_wnd );
	CPoint	convert_from_window_to_vt( CPoint pt_wnd );

protected:		

	bool m_bAttachActiveObject;
	bool	m_bReadOnly;

	BOOL m_bDisconnectNotifyContrl;

	CRect m_rectPrinterMagins;
	CSize	m_ScrollOffset;

	IDocumentSitePtr m_ptrTargetDoc;
	IUnknown* m_punkReportSourceDoc;

	BOOL IsViewCtrl( IUnknown* punk);

	void WriteToTargetTemplate();

	ViewMode	viewMode;
	bool		m_bUseAnimationDraw;

	void OnPrinterSettingsChange( bool bCorrectCtrlPos );
	bool OnDocumentPrint( bool bpreview );

	void EnterDesignMode();
	void EnterPrintMode();

	void EnterPreviewMode();
	void LeavePreviewMode();

	void UpdateView();

	void FitToPage();
	void FitToPageWidth();
	void ScrollToPage( int nPageNum, CRect rcTarget);

	BOOL PrintPageContext(); 
	
	//////////////////////////////////////

	BOOL m_bIsLoadingProcess;

	CArray <CPage*, CPage*> m_page;
	//////////////////////////////////////
	//	Initialization / Destruction
	//_______________________________	
	BOOL BuildContext();
	BOOL DeleteContext();
	//////////////////////////////////////

	void SetTargetDocument(IUnknown* punkTargDoc);	
	//CArray <DocumentObject*, DocumentObject*> m_docObjList;


	void InitViewCtrlData();
	void MoveCtlWindows();
	
	//////////////////////////////////////
	//	View scroll size support  
	//_______________________________
	//View scroll size
	int m_nPageCountHorz;		
	int m_nPageCountVert;
	//Set new view scroll size
	void SetNewScrollSize();	
	void RecalcColRowCount(IUnknown *punkControlDelete = NULL);

	void OnZoomChange();
	//////////////////////////////////////

	
	void GetPaneColRow( CPoint point, int& nCol, int& nRow);
	CRect GetPanePageRect ( int nCol, int nRow );	
	CRect GetPaneAlignRect( int nCol, int nRow );
	CRect GetPaneFieldRect( int nCol, int nRow );

	int FindFreeMainPage( IUnknown *punkControlDelete = NULL );

	void Update( IUnknown* punkDelete = NULL, IUnknown* punkAdd = NULL );
	void BuildPages( IUnknown* punkDelete = NULL );
	void SyncAXRect();	
	void SetPagesPos();
	void SortPages();
	void DefragPane( IUnknown* punkDelete = NULL  );

	CPage* FindPage( int nMainPage, int nCol, int nRow );

	CPoint FromClientToPane( CPoint point, CPage* page);


	void GenerateCtrlRect(IUnknown* punk, CPoint ctrlOffset, CSize ctrlSize,
						CRect*& pRect, int& nOccupedCol, int&nOccupedRow ,
						CSize*& pViewCtrlSize, CSize*& pUserSize,
						CSize*& pUserSizeDX,
						ViewMode mode);
	static void QueryCtrlPrintDimension(IUnknown* punk, 
						int* pOfferWidth,  int nOfferWidthCount,
						int* pOfferHeight, int nOfferHeightCount,
						int*& pRealWidth,  int& nRealWidthCount,
						int*& pRealHeight, int& nRealHeightCount,
						ViewMode mode = vmDesign 
						);

	PointPlace GetPointPlace( CPoint point );
	
	//////////////////////////////////////
	//	Control position
	//_______________________________	
	//Fire when control pos change
	void OnCtrlPosChange(IUnknown *punkControl);
	//Correct control position
	void CorrectCtrlPos(IUnknown *punkControl, BOOL bForceExit = FALSE );
	//////////////////////////////////////
	void VerifyCtrlPos(  IUnknown* punkAdd = NULL );

	void VerifyPageContext();

	bool IsControlInside(IUnknown *punkCtrl, CRect rcCtrl);
	bool IsObjectCanBeSelected(IUnknown *punkObj);

	//////////////////////////////////////
	//	Drawing
	//_______________________________	
	void DrawRulers(CDC* pDC);

	void DrawEmptyPage( CDC* pDC, int nNum );
	void DrawEmptyPages(CDC* pDC);	

	void DrawPagesContext(CDC* pDC, BOOL bPrint = FALSE, CDC* pDCSrc = 0, CRect* pRCUpdate = 0 );
	void InvalidateView();	
	//////////////////////////////////////	

	//	Preview mode 
	//_______________________________	
	void AddNewFieldControls();
	void CalcCtrlPreviewPos();

	CPoint ConvertFromMMToVTPixel( CPoint point);
	CRect ConvertFromMMToVTPixel( CRect rect);

	CPoint ConvertFromVTPixelToMM( CPoint point);
	CRect ConvertFromVTPixelToMM( CRect rect);

	CSize ConvertFromHimetricToDisplay( CSize size);	

	int ConvertFromMMToVTPixel( int mm );
	int ConvertFromVTPixelToMM( int VTpixel);
	
public:
	static double GetDisplayPixelPMM();	

protected:

	BOOL IsSimpleEditMode();

	BOOL IsVisiblePage( int nPageNum, CRect* pRCUpdate );

	void ScroolToVisiblePage();



public:
	CPrintView();
	~CPrintView();
	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual DWORD GetMatchType( const char *szType );
	//virtual bool DoDrag( CPoint point );
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest );

	//{{AFX_MSG(CPrintView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd *pWndPrev);
	afx_msg void OnSetFocus(CWnd *pWndPrev);
	//}}AFX_MSG
	//{{AFX_VIRTUAL(CPrintView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

public:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );	
	virtual BOOL IsVisibleCtrlWnd(){ return FALSE;}
	virtual void AttachActiveObjects();
protected:	

	virtual void _AddControl( IUnknown *punkControl );
	virtual void _RemoveControl( IUnknown *punkControl );

	virtual void _AddAllControls();
	virtual void _RemoveAllControls();

	virtual void _UpdateLayout( bool bRepositionControls = true );

	void DestroyViewCtrlContext();
	void SetReportViewModeToDBControls();


public:
	virtual CPoint GetScrollPos( );

	//coordinate mapping - screen to client
	virtual CRect ConvertToClient( CRect rc);
	virtual CPoint ConvertToClient( CPoint pt);
	virtual CSize ConvertToClient( CSize size);
	//coordinate mapping - client to screen
	virtual CRect ConvertToWindow( CRect rc);
	virtual CPoint ConvertToWindow( CPoint pt);
	virtual CSize ConvertToWindow( CSize size );


	virtual CPoint PaneConvertOffset( CPoint ptOffset );
	virtual CRect  PaneConvertObjectRect( IUnknown* punkObject );

protected:
	void InitCtrlMode( int nOldMode, int nNewMode );

	bool IsRectIntersect( CRect rc1, CRect rc2 );

	bool		m_bShowBorderAroundCtrl;
	COLORREF	m_clrBorderAroundCtrl;
	COLORREF	m_clrBack;
	COLORREF	m_clrAlign;
	COLORREF	m_clrField;
	COLORREF	m_clrMainWork;
	COLORREF	m_clrAdditionalWork;
	COLORREF	m_clrFreeWork;

	void		load_settings();
	void		draw_border( CDC* pdc, CRect rc_border );

	
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CPrintView)
	afx_msg long GetFirstCtrlPos( );
	afx_msg long GetNextCtrlPos( LPOS lPos );
	afx_msg LPDISPATCH GetCtrlDispatch( LPOS lPos );
	afx_msg BSTR GetCtrlProgID( LPOS lPos );
	afx_msg long GetSelectedPos( );
	afx_msg void StoreCtrlData( LPOS lPos );
	afx_msg void SetSelectedPos( LPOS lPos );
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	virtual GUID get_view_dispiid( );

	int		m_npage_align;

public:
	bool m_bViewMode;
};




#endif //__printview_h__
 

