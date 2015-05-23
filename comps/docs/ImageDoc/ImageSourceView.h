#if !defined(AFX_IMAGESOURCEVIEW_H__666AEC80_9263_49D9_9523_531C74E202AF__INCLUDED_)
#define AFX_IMAGESOURCEVIEW_H__666AEC80_9263_49D9_9523_531C74E202AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageSourceView.h : header file
//


/// настройки рамки, расположенной по центру вьюхи, хранящиеся в NamedData объекта image //////
#define CF_SECTION		"\\CenterFrame"
#define CF_SHOWFRAME	"ShowFrame"
#define CF_LINEWIDTH	"LineWidth"
#define CF_LINECOLOR	"LineColor"
#define CF_FRAMEWIDTH	"FrameInnerWidth"
#define CF_FRAMEHEIGHT	"FrameInnerHeight"
/////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CImageSourceView command target

//Image pane force declare
class CImagePane;
class CConvertorPane;

#include "\vt5\awin\misc_map.h"
#include "\vt5\awin\misc_string.h"
#include "PropBagImpl.h"

class CImageSourceView : public CViewBase, public CEasyPreviewImpl, public CNamedPropBagImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CImageSourceView)
	PROVIDE_GUID_INFO( )

	enum ImageDrawFlags
	{
		DrawImage = 1,
		DrawMask = 2,
		DrawContours = 4,
		DrawClip = 8,
		DrawSelFrame = 16,
		DrawHighlight = 32
	};

	enum DrawFrameStage
	{
		StageDrawImage = 0,
		StageDrawFrame = 1,
	};

	BOOL m_bFromPrint, m_bObjCountingOnly;

	CRect m_rcPrevClass, m_rcPrevNum;

	bool m_bPhases;
	CString GetSectionName();

// Construction
public:
	CImageSourceView();

	virtual CString get_section( ) { return "\\ImageView"; }

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}
private:
	IUnknownPtr m_sptrAttached;
// Attributes
public:

// Operations
public:
	virtual void DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom){};
	virtual void _AttachObjects( IUnknown *punkContext );
	virtual void DoPocessNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize ){};
	virtual void DoNotifySelectableObjects( long lHint, IUnknown *punkObject, IUnknown *punkObjectList );
	virtual void DrawHilightedObjects(CDC &dc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint){};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageSourceView)
	public:
	virtual void OnFinalRelease();
	////virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageSourceView();

	virtual void OnChange() {Invalidate();};

//print
	bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX );
	bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

//drag-drop
	virtual ObjectFlags GetObjectFlags( const char *szType );
	virtual CWnd *GetWindow(){return this;}
	virtual bool DoDrag( CPoint point );
	
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
	// Generated message map functions
protected:
	bool _check_corners( RECT rcObj, RECT rcClient, BYTE **pSourceImgMask, int W, int H, POINT pt, int *pX, int *pY, IUnknown *punkObject );
	void _fix_letter( CRect *out, CRect textRc, IUnknown *punkObject );
	bool _check_cross( int x, int y, int w, int h, IUnknown *punkObject )	{ return _check_cross( CRect( x, y, x + w, y + h), punkObject ); }
	bool _check_cross( CRect rc, IUnknown *punkObject );
	void _select_active_object(IUnknown *punkOL);

	//{{AFX_MSG(CImageSourceView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CImageSourceView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CImageSourceView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ImView, IImageView2)
		com_call GetActiveImage( IUnknown **ppunkImage, IUnknown **ppunkSelection );
		com_call EnableControlFrame( DWORD dwFrameControlsFlags );
		com_call GetImageShowFlags( DWORD *pdwFlags );
		com_call SetSelectionMode( long lSelectType );
		com_call GetSelectionMode( long *plSelectType );
		com_call SetImageShowFlags( DWORD dwFlags );
		com_call SetActiveImageDuringPreview( IUnknown *punkImage, IUnknown *punkSelection );
		com_call GetActiveImage2( IUnknown **ppunkImage, IUnknown **ppunkSelection, BOOL bFromPreview );
	END_INTERFACE_PART(ImView)

	BEGIN_INTERFACE_PART(MenuInitializer, IMenuInitializer)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
	END_INTERFACE_PART(MenuInitializer)

	virtual DWORD GetMatchType( const char *szType );

	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );

	virtual void DoPaint( CDC &dc, CRect rectPaint );
	virtual bool CanStartTracking( CPoint pt );

	void _SetSelection( IUnknown *punkSelection );

	//draw image helper
	bool _DrawImage( CDC &dc, CImageWrp &image, CRect rectClip, DWORD dwFlags = 0, COLORREF crContour = RGB( 255, 0, 0 ), BITMAPINFOHEADER *pbih = 0, byte *pdibBits = 0, bool bScrollZoom = true );
	bool _DrawGrid( CDC &dc, CRect rectClip );
	bool _DrawObjects( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	bool _DrawMeasObjects( CDC &dc, CRect rectClip, COLORREF color, IUnknown * punkParent, ObjectDrawState state );
	bool _DrawObjectClass( CDC &dc, IUnknown * punkObject, CRect & rcInvalidate, CRect & rcObj, COLORREF cr );
	bool _DrawObjectNumber( CDC &dc, IUnknown * punkObject, CRect & rcInvalidate, CRect & rcObj, COLORREF cr );
	bool _DrawCalibr( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits, CPoint ptOffset, bool bPrint=false, double fPrintZoom=1, bool bPrintPreview=false);
	bool _IsCalibrVisible();
	CRect _VisibleImageRect(); //возвращает прямоугольник, содержащий видимую в окне часть image
	int m_nLastCalibrTop;	// последнее положение верха полосы с маркером - для invalidate при скролле
								// считается в экранных пикселах от угла Image
	//bool _GetCalibrRect();

	// vanek 24.10.2003
	COLORREF	m_crCFLine;
	long		m_lCFLineWidth;
	CSize		m_CFSize;
    
    bool _IsCenterFrameVisible( ) ;
	bool	_DrawCenterFrame( CDC &dc, BOOL	bUpdateData = TRUE );
	//


	virtual void AttachActiveObjects();	//attach the active image
	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual CRect GetObjectRectFull( IUnknown *punkObject );
	virtual CRect GetObjectAdditionRect(IUnknown *punkObject);
	CRect _GetMeasObjectRect(IUnknown * punkObject, bool bEx = false);
	CRect _GetObjectClassAndNumberRect(IUnknown * punkObject, CRect & rc, BOOL bFromSetOffset = false );

	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffset );
 
	virtual void DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe );
	virtual bool CanResizeObject( IUnknown *punkObject );
	virtual bool CanRotateObject( IUnknown *punkObject );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
	virtual IUnknown *GetObjectByPoint( CPoint point);
	virtual bool PtInObject( IUnknown *punkObject, CPoint point );

	void	_ChangeSelectionMode( long nMode );
	void	_CheckFrameState();
	void	_UpdateScrollSizes();
	virtual void OnEnterPreviewMode();
	virtual void OnLeavePreviewMode();
	virtual void OnPreviewReady();
	bool	_ShowImagePane( bool bShow );
	bool	_ShowImagePage( bool bShow );

	void _InitBinaryColorsTable();
	virtual void OnBeginDragDrop() {};

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);	

	virtual void CalcClientSize();

protected:
	//no-preview values
	CImageWrp	m_imageSource;
	CImageWrp	m_imageSourceSelection;

	CImageWrp	m_image;
	CImageWrp	m_imageSelection;

	
	CObjectListWrp		m_drawobjects;

	
	bool			m_bMouseImplWasEnabled;
	bool			m_bObjectSelected;
//drawing options
	
	CFont			m_fontEdit;
	byte			m_colors[3];
	byte			m_backcolors[3];

	int				m_nActiveNumber;

	bool			m_bShowName;
	int				m_nNamePos;
	bool			m_bShowNumber;
	int				m_nNumberPos;

	double			m_fZoom;
	CPoint			m_pointScroll;

	IColorConvertorLookupPtr	m_ptrCCLookup;
	IColorConvertorExPtr		m_ptrCC;
	CTypedPtrList<CPtrList, IUnknown*>m_listObjects;

	DWORD			m_dwFillColor;
	DWORD			m_dwBackColor;

	//SelectionMode	m_SelectionMode;
public:
	virtual const char* GetViewMenuName();

	// Context menu for selection, drawing objects, etc...
	bool	m_bShowChildContextMenu;


//Pane support
protected:
	CConvertorPane	*m_pConvertPane;
	CImagePane		*m_pImagePane;

	int				m_nImagePaneNum;

	void			_CreateImagePane( bool bCreate );
	void			_UpdateImagePane();
	IUnknown*		GetImageByPoint( CPoint pt );


	DWORD			m_dwShowFlags;
	bool			m_bNeedSetAppropriateView;
	DWORD			m_dwFrameFlags;

	CSize			m_size_client;
	virtual bool _get_center_mode() { return GetValueInt( ::GetAppUnknown(), "\\ImageView", "CenterImageInView", 0 ) != 0; };
//selection
protected://supports - object, measurement, binary, text, drawing, 
	virtual void	FillSelectableList();
	virtual void	ClearSelectableList();
	virtual bool	HitTest( CPoint point, INamedDataObject2 *pNamedObject );
	virtual void	OnChangeObject( INamedDataObject2 *pNamedObject );
	virtual void	OnSelect( INamedDataObject2 *pNamedObject );
	virtual void	OnDeselect( INamedDataObject2 *punkWillBeDeselectedNow );

	virtual void	AddSelectableObject( INamedDataObject2 *pNamedObject, bool bInvalidate = true );
	virtual void	RemoveSelectableObject( INamedDataObject2 *pNamedObject, POSITION posFrom = 0 );
protected:
	CTypedPtrList<CPtrList, INamedDataObject2*>			m_selectableObjects;
	CMap<INamedDataObject2 *, INamedDataObject2 *, POSITION, POSITION&>	m_object2position;
	POSITION											m_posLastSelectable;

	void			_attach_msg_listeners_from_list( IUnknown* punkObjectList, bool bAttach );
	void			_attach_msg_listener( IUnknown* punkObject, bool bAttach );

	virtual bool	is_have_print_data();
	COLORREF		_get_class_color( long nclass );
	const char		*_get_class_name( long nclass );
	long _get_object_class( ICalcObject *pcalc );

	static void free_sz( char *p )
	{::free( p );}

	_map_t<COLORREF, long, cmp<long> >			m_class_colors;  
	_map_t<char *, long, cmp<long>, free_sz>		m_class_names;
	
	_map_t<COLORREF, long, cmp >			m_class_colors2;
	_map_t<char *, long, cmp_long, free_sz>		m_class_names2;

	_list_map_t<CRect, IUnknown*, cmp<IUnknown*> >			m_object_coords;
	_list_map_t<CRect, IUnknown*, cmp<IUnknown*> >			m_object_coords_num;

	COLORREF m_clWhite;
	BOOL m_bAsClassColor;
	double m_fOldZoom;
	
	virtual void _recalc_rect(BOOL bUpdate = false, IUnknown *punkExclude = 0);

	//allow move object outside image rect
	bool	is_enable_exended_move();
	long	m_lenable_exended_move;

	_bstr_t m_bstr_class_name;

protected:
	// [vanek] : manage of visibility avi's property page - 10.09.2004
    void	_UpdateAviPropPage( bool bShow = true );
private:
	bool m_bShowUnknown;
	int m_iSkip;
};

extern DWORD     g_pdwBinaryColors[256];
extern DWORD     g_clSyncColor;

#include "Binary.h"
extern void DrawBinaryRect( CBinImageWrp pbin, BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, RECT rectDest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwBackColor, BOOL bUseScrollZoom, DWORD dwShowFlags);
extern int _GetNamedDataInt(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, int nDefault);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGESOURCEVIEW_H__666AEC80_9263_49D9_9523_531C74E202AF__INCLUDED_)
