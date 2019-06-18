// CCarioView.cpp : implementation file
//

#include "stdafx.h"
#include "chromosEx.h"
#include "CarioView.h"
#include "consts.h"
#include "menuconst.h"
#include "MenuRegistrator.h"
#include "editorint.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "drag_drop_defs.h"

bool SetModifiedFlagToDoc( IUnknown* punkDoc )
{
	IFileDataObject2Ptr ptrFDO2;

	if( punkDoc == 0 )
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp )
		{
			IUnknown* punk = 0;
			ptrApp->GetActiveDocument( &punk );
			if( punk )
			{
				ptrFDO2 = punk;
				punk->Release();	punk = 0;
			}
		}
	}
	else
		ptrFDO2 = punkDoc;

	if( ptrFDO2 == 0 )
		return false;

	ptrFDO2->SetModifiedFlag( TRUE );


	return true;
}

bool IsInteractiveActionRunning()
{
	bool bInteractive = false;
	
	IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	IActionManagerPtr ptmAM( punkAM );
	punkAM->Release();	punkAM = 0;

	if( ptmAM )
	{
		IUnknown* punk = 0;
		ptmAM->GetRunningInteractiveAction( &punk );
		if( punk )
		{
			bInteractive = true;
			punk->Release();
		}
	}

	return bInteractive;
}

/////////////////////////////////////////////////////////////////////////////
// CCarioView

IMPLEMENT_DYNCREATE(CCarioView, CWnd)

CCarioView::CCarioView()
{
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	EnableAutomation();
	EnableAggregation();
	
	m_sName = szCarioView;
	m_sUserName.LoadString( IDS_CARIO_VIEW_NAME );			

	m_carioViewMode		= cvmPreview;
	m_hcurLast = 0;	  
	m_pframeRuntime = RUNTIME_CLASS(CCarioFrame);

	m_hNoDrag = LoadCursor( AfxGetResourceHandle(), MAKEINTRESOURCE( IDC_CURSOR_DODRAG ) );
	m_hDrop   = LoadCursor( AfxGetResourceHandle(), MAKEINTRESOURCE( IDC_CURSOR_DROP   ) );

	m_b_need_synchronize_dimmension = false;
}

CCarioView::~CCarioView()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.

	if( m_hNoDrag )
		::DeleteObject( m_hNoDrag );
	
	if( m_hDrop )
		::DeleteObject( m_hDrop );

	AfxOleUnlockApp();
}

void CCarioView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	//CWnd::OnFinalRelease();
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}


BEGIN_MESSAGE_MAP(CCarioView, CViewBase)
	//{{AFX_MSG_MAP(CCarioView)	
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/*
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	
*/


BEGIN_DISPATCH_MAP(CCarioView, CViewBase)
	//{{AFX_DISPATCH_MAP(CCarioView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICCarioView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {65272489-9943-4DE6-83BE-04B4732BC87D}
static const IID IID_ICCarioView =
{ 0x65272489, 0x9943, 0x4de6, { 0x83, 0xbe, 0x4, 0xb4, 0x73, 0x2b, 0xc8, 0x7d } };

BEGIN_INTERFACE_MAP(CCarioView, CViewBase)
	INTERFACE_PART(CCarioView, IID_ICCarioView, Dispatch)
	INTERFACE_PART(CCarioView, IID_IPrintView, PrintView)	
	INTERFACE_PART(CCarioView, IID_ICarioObject, CarioObject)
	INTERFACE_PART(CCarioView, IID_ICarioView, CarioView)
	INTERFACE_PART(CCarioView, IID_ICarioView2, CarioView)
	INTERFACE_PART(CCarioView, IID_IViewData, ViewData)
	INTERFACE_PART(CCarioView, IID_IEasyPreview, Preview)
	INTERFACE_PART(CCarioView, IID_ICarioVDnD, CarioVDnD )
//INTERFACE_PART(CCarioView, IID_INamedPropBag, NamedPropBag)
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CCarioView, CarioView);
IMPLEMENT_UNKNOWN(CCarioView, ViewData);
IMPLEMENT_UNKNOWN(CCarioView, CarioVDnD);

// {AE25F9DE-076C-4779-954E-DE3527D33149}
GUARD_IMPLEMENT_OLECREATE(CCarioView, szCarioViewProgID, 0xae25f9de, 0x76c, 0x4779, 0x95, 0x4e, 0xde, 0x35, 0x27, 0xd3, 0x31, 0x49)

bool CCarioView::DoDrag( CPoint point )
{
	return __super::DoDrag( point );
}
/////////////////////////////////////////////////////////////////////////////
// CCarioView message handlers

/////////////////////////////////////////////////////////////////////////////
int CCarioView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;	
	/*
	if( 1 == 3 )
	{
		m_pframeRuntime = RUNTIME_CLASS(CFrame);
		m_pframeDrag = (CFrame*)m_pframeRuntime->CreateObject();
		m_pframeDrag->Init(this, true);
		m_pframeRuntime = 0;
	}
	*/

	//reload_settings();
	on_activate_object_list( m_ptrActiveObjectList );
    
   	m_ToolTipCtrl.Create( this, TTS_ALWAYSTIP );

	CRect rcClient;
	GetClientRect( rcClient );
	
	rcClient.right  *= 16;
	rcClient.bottom *= 16;

	m_ToolTipCtrl.AddTool( this, "*-*-*", rcClient, 123 );
	
	m_ToolTipCtrl.SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_INITIAL, 200);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_RESHOW,  200);

	m_ToolTipCtrl.Activate(false);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::AttachActiveObjects()
{
	_AttachObjects( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::OnPaint() 
{
	CTimeTest	test( true, "CCarioView::OnPaint" );

	if(m_b_need_synchronize_dimmension)
	{
		synchronize_dimmension( SDF_RECALC_ALL );
		set_client_size();
		m_b_need_synchronize_dimmension = false;
	}

	update_zoom_scroll();

	CPoint ptScroll = get_scroll_pos();
	double fZoom = get_zoom();

	CRect rcPaint;
	GetUpdateRect( &rcPaint );


	CPaintDC	dcPaint( this );
	CDC			dcMemory;

	dcMemory.CreateCompatibleDC( &dcPaint );

	int nImageWidth		= rcPaint.Width( )  + 1;
	int nImageHeight	= rcPaint.Height( )  + 1;

	if( nImageWidth < 1 || nImageHeight < 1 )
		return;	

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );

	bmih.biBitCount = 24;	
	bmih.biWidth	= nImageWidth;
	bmih.biHeight	= nImageHeight;
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcPaint, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection );

	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

	dcMemory.SetMapMode( MM_TEXT );	

 	dcMemory.SetViewportOrg( -rcPaint.left, -rcPaint.top );
 	dcMemory.SetBrushOrg( -( rcPaint.left +  + ptScroll.x ) % 8, -( rcPaint.top + ptScroll.y ) % 8 );
	
	{
		//fill backgrund
		CRect rcFill = rcPaint;
		
		dcMemory.FillRect( &rcFill, &CBrush( m_clrBkg ) );
		
		//process drawing
 		do_draw( &dcMemory, rcPaint, &bmih, pdibBits );

		_DrawObjects( dcMemory, rcPaint, &bmih, pdibBits );

		if( m_pframe )
			m_pframe->Draw( dcMemory, rcPaint, &bmih, pdibBits);
		
		if( m_pframeDrag )
			m_pframeDrag->Draw( dcMemory, rcPaint, &bmih, pdibBits);

	}

	dcMemory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dcPaint, rcPaint.left, rcPaint.top, nImageWidth, nImageHeight,
		0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	ProcessDrawing( dcPaint, rcPaint, &bmih, pdibBits );
	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );

	if( psrcBitmap )
		psrcBitmap->DeleteObject();


	dcMemory.SelectObject( poldBmp );

	dcPaint.SetROP2( R2_COPYPEN );
	dcPaint.SelectStockObject( BLACK_PEN );

	//if( m_pframe )
	//	m_pframe->CResizingFrame::Draw( dcPaint, rcPaint );

}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::_DrawObjects( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	 //draw comment objects
	IDrawObjectPtr	ptrObj(m_drawobjects);
	if( ptrObj != 0 )
		ptrObj->Paint( dc.GetSafeHdc(), rectClip, GetControllingUnknown(), pbmih, pdibBits );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	IDrawObjectPtr	ptrObj(punkObject);
	if( ptrObj != 0 )
	{
		CPen	pen;
		if( state == odsActive )
			pen.CreatePen( PS_DOT, 0, RGB( 0, 0, 0 )  );
		else if( state == odsAnimation )
			pen.CreatePen( PS_SOLID, 0, RGB( 255, 255, 255 )  );

		dc.SetBkMode(OPAQUE);
		dc.SelectObject( &pen );
		dc.SelectStockObject( NULL_BRUSH );
		dc.SetBkColor( RGB( 255, 255, 255 ) );

		CRect rcObject = GetObjectRect( punkObject );
		rcObject = ConvertToWindow( GetControllingUnknown(), rcObject );
		dc.Rectangle( rcObject );

		dc.SelectStockObject( BLACK_PEN );
		dc.SelectStockObject( BLACK_BRUSH );
	}
}

/////////////////////////////////////////////////////////////////////////////
CRect CCarioView::GetObjectRect( IUnknown *punkObj )
{
	CRect rect = NORECT;
	ICommentObjPtr ptrComment(punkObj);
	if(ptrComment)
	{
		CRect	rcText = NORECT;
		CPoint	ptArr = CPoint(0,0);
		ptrComment->GetCoordinates(&rcText, &ptArr);
		if(ptArr == CPoint(-1,-1))
			ptArr = rcText.TopLeft();
		CRect rcArr(ptArr.x-1, ptArr.y-1, ptArr.x+1, ptArr.y+1);
		rcArr.NormalizeRect();
		rect.UnionRect(&rcText, &rcArr);
		rect.NormalizeRect();
		rect.InflateRect( 5, 5, 5, 5 );
		return rect;
	}

	IRectObjectPtr	ptrRectObject( punkObj );
	if( ptrRectObject ) ptrRectObject->GetRect( &rect );

	if( rect == NORECT )
		return rect;

	return rect;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::SetObjectOffset( IUnknown *punkObj, CPoint ptOffset )
{
	ICommentObjPtr ptrComment(punkObj);
	if(ptrComment)
	{
		CRect	rcText = NORECT;
		CPoint	ptArr = CPoint(0,0);
		ptrComment->GetCoordinates(&rcText, &ptArr);
		CPoint ptObjOffset(0,0);
		if(ptArr != CPoint(-1,-1))
		{
			CPoint ptLeftTop = ptArr;
			if(ptArr.x > rcText.left)
				ptLeftTop.x = rcText.left;
			if(ptArr.y > rcText.top)
				ptLeftTop.y = rcText.top;

			ptObjOffset = ptOffset - ptLeftTop;
			ptArr.Offset(ptObjOffset);
		}
		else
		{
			ptObjOffset = ptOffset - rcText.TopLeft();
		}
		rcText.OffsetRect(ptObjOffset);
		ptrComment->SetCoordinates(rcText, ptArr);

		return;
	}

	IRectObjectPtr	ptrRectObject( punkObj );
	if( ptrRectObject )
	{
		ptrRectObject->Move( ptOffset );
	}
}



/////////////////////////////////////////////////////////////////////////////
void CCarioView::do_draw( CDC* pdc, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{			
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		draw_chromo_line( pdc, i, rcUpdate, pbmih, pdibBits );
	}

	//cos may be draging
	if( GetCarioViewMode() != cvmMenu )
		draw_chromo_object( pdc, m_nActiveLine, m_nActiveCell, m_nActiveChromo, rcUpdate, pbmih, pdibBits );

}

/////////////////////////////////////////////////////////////////////////////
BOOL CCarioView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::OnSize(UINT nType, int cx, int cy) 
{
	if( m_carioViewMode != cvmCellar )
 		ensure_visible( m_nActiveLine, m_nActiveCell, m_nActiveChromo );	
	__super::OnSize(nType, cx, cy);		
}

/////////////////////////////////////////////////////////////////////////////
//
//
//print functions
//
//
/////////////////////////////////////////////////////////////////////////////
bool CCarioView::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{
	update_zoom_scroll();

	CRect rc = NORECT;
	GetWindowRect( rc );

	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	if( ptrZ == 0 )	return false;

	CSize size;
	ptrZ->GetClientSize( &size );

	double fZoom = 1.0;
	ptrZ->GetZoom( &fZoom );
	rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );	

	bool bcontinue = false;

	int nWidth = rc.Width();//size.cx;

	if( nWidth > nUserPosX + nMaxWidth )
	{
		nReturnWidth	 = nMaxWidth;
		nNewUserPosX	+= nReturnWidth;
		bcontinue = true;
	}
	else
	{
		nReturnWidth = nWidth - nUserPosX;
		nNewUserPosX = nWidth;
	}
	
	return bcontinue;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{	

	update_zoom_scroll();

	CRect rc = NORECT;
	GetWindowRect( rc );
	
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	if( ptrZ == 0 )	return false;

	CSize size;
	ptrZ->GetClientSize( &size );

	double fZoom = 1.0;
	ptrZ->GetZoom( &fZoom );
	rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );	

	bool bcontinue = false;

	int nHeight = rc.Height();//size.cy;

	if( nHeight > nUserPosY + nMaxHeight )
	{
		nReturnHeight	 = nMaxHeight;
		nNewUserPosY	+= nReturnHeight;
		bcontinue = true;
	}
	else
	{
		nReturnHeight = nHeight - nUserPosY;
		nNewUserPosY = nHeight;
	}
	
	return bcontinue;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
{
	m_bFromPrint = true;
	m_nActiveLine = m_nActiveCell = m_nActiveChromo = -1;
	CDC* pdc = CDC::FromHandle(hdc);
	update_zoom_scroll();

	m_fCachedZoom = 1.0;

	double freal_zoom = 1.0;
	{
		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		if( ptrZ )
			ptrZ->GetZoom( &freal_zoom );
	}

	CRect rcPaint = CRect(	nUserPosX / freal_zoom, nUserPosY / freal_zoom, 
							( nUserPosX + nUserPosDX ) / freal_zoom, 
							(nUserPosY + nUserPosDY) / freal_zoom );

	int nOldMM = pdc->GetMapMode();
	CSize sizeOldVE = pdc->GetViewportExt();
	CSize sizeOldWE = pdc->GetWindowExt();
	CPoint ptOldVO = pdc->GetViewportOrg();
	
	CSize sizeWindowExt = CSize( rcPaint.Width() , rcPaint.Height() );	
	CSize sizeViewPortExt = CSize( rectTarget.Width(), rectTarget.Height() );	
	CPoint ptViewPortOrg;
	
	double fZoomX = (double)rectTarget.Width()  / (double)( rcPaint.Width() );
	double fZoomY = (double)rectTarget.Height() / (double)( rcPaint.Height() );

	ptViewPortOrg.x = rectTarget.left - rcPaint.left * fZoomX;
	ptViewPortOrg.y = rectTarget.top  - rcPaint.top * fZoomY;

	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect( &rectTarget );
	pdc->SelectClipRgn( &rgnClip );


	pdc->SetMapMode( MM_ANISOTROPIC );

	pdc->SetWindowExt( sizeWindowExt );					
	pdc->SetViewportExt( sizeViewPortExt );
	pdc->SetViewportOrg( ptViewPortOrg );

	pdc->FillRect( &rcPaint, &CBrush( m_clrBkg ) );
	do_draw( pdc, rcPaint, 0, 0 );

	{
		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		if( ptrZ )
		{
			double _fzoom = 1.;
			double fdisplay_dpi = 96.;
			double fdevice_dpi	= 96.;
			{
				CClientDC dc_screen( 0 );
				fdisplay_dpi	= ::GetDeviceCaps( dc_screen.m_hDC, LOGPIXELSY );
				fdevice_dpi		= ::GetDeviceCaps( hdc, LOGPIXELSY );
			}
			
			if( fdevice_dpi > 0 && fdisplay_dpi > 0 )
				_fzoom = fdisplay_dpi / fdevice_dpi;

			ptrZ->SetZoom( 1 );
			_DrawObjects( *pdc, rcPaint, 0, 0 );
			ptrZ->SetZoom( freal_zoom );
		}
	}

	pdc->SetMapMode( nOldMM );
	pdc->SetViewportExt( sizeOldVE );
	pdc->SetWindowExt( sizeOldWE );
	pdc->SetViewportOrg( ptOldVO );

	pdc->SelectClipRgn( 0 );
	m_bFromPrint = false;

}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::rebuild_view()
{
	bool bres = true;
	//if no design
	bres &= build( m_ptrActiveObjectList );
	set_client_size();
	invalidate_rect( 0 );

	return bres;
}
/////////////////////////////////////////////////////////////////////////////
//
//ICarioView interface
//
/////////////////////////////////////////////////////////////////////////////

HRESULT CCarioView::XCarioView::GetChromoRect( long code, int nLine, int nCell, int nChromo, RECT *prect )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView);
	chromo_object* pobject	= 0;
	pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );

	CRect	rect = NORECT;

	if( code == GCR_CHROMOSOME )
		rect = pobject->m_rectImage;
	else if( code == GCR_IDIOGRAM )
		rect = pobject->m_rectEditableIdio;
	else if( code == GCR_GRAPH )
	{
		if( pThis->m_bShowCGHGraph )
			rect = pobject->m_rectCGHGraph;
		else
			rect.OffsetRect( -pobject->m_ptOffset );
	}
	else
	{
		rect = pobject->m_rectFull;
		rect.UnionRect( pobject->m_rectFull, pobject->m_rectText );
	}

	rect.OffsetRect( pobject->m_ptOffset );

	::CopyRect( prect, rect );
	return S_OK;
}
HRESULT CCarioView::XCarioView::StoreState( IUnknown *punkData )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView);

	pThis->save_cario_state_to_data( pThis->m_ptrActiveObjectList );

	if( pThis->GetCarioViewMode() == cvmDesign )
		pThis->save_cario_state_to_data( ::GetAppUnknown() );

	return S_OK;
}

HRESULT CCarioView::XCarioView::SetMode( CarioViewMode viewMode )
{
	_try_nested(CCarioView, CarioView, SetMode)
	{	
		if( viewMode != cvmMenu )
		{
			IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
			IActionManagerPtr ptrAM( punkAM );
			punkAM->Release();	punkAM = 0;

			ptrAM->TerminateInteractiveAction();
		}

		if( viewMode == pThis->m_carioViewMode )
			return S_OK;

		pThis->m_carioViewMode = viewMode;		
		pThis->m_bHasBeenChange = false;
		pThis->rebuild_view();

		if( pThis->m_carioViewMode == cvmDesign )
		{
			pThis->m_nActiveLine = pThis->m_nActiveCell = pThis->m_nActiveChromo = 0;
			chromo_object	*po = pThis->get_chromo_object( pThis->m_nActiveLine, 
															pThis->m_nActiveCell, 
															pThis->m_nActiveChromo );
			INamedDataObject2Ptr	ptrObjectList( pThis->m_ptrActiveObjectList );

			if( ptrObjectList != 0 )
			{
				long	lpos = 0;
				if( po && po->m_ptrMeasureObject != 0 )
				{
					INamedDataObject2Ptr	ptrN( po->m_ptrMeasureObject );
					ptrN->GetObjectPosInParent( &lpos );
				}
				ptrObjectList->SetActiveChild( lpos );
			}
			SetActiveChromo( 0, 0, -1 );
		}		

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetMode( CarioViewMode* pViewMode )
{
	_try_nested(CCarioView, CarioView, GetMode)
	{	
		if( !pViewMode )
			return E_INVALIDARG;

		*pViewMode = pThis->m_carioViewMode;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetLineCount( int* pnLineCount )
{
	_try_nested(CCarioView, CarioView, GetLineCount)
	{	
		if( !pnLineCount )
			return E_INVALIDARG;

		*pnLineCount = pThis->m_arCarioLine.GetSize();

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetCellCount( int nLine, int* pnCellCount )
{
	_try_nested(CCarioView, CarioView, GetCellCount)
	{	
		if( !pnCellCount )
			return E_INVALIDARG;		

		if( nLine < 0 || nLine >= pThis->m_arCarioLine.GetSize() )
			return E_INVALIDARG;

		*pnCellCount = pThis->m_arCarioLine[nLine]->m_arCell.GetSize();

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetActiveLine( int* pnLine )
{
	_try_nested(CCarioView, CarioView, GetActiveLine)
	{	
		if( !pnLine )
			return E_INVALIDARG;

		*pnLine = pThis->m_nActiveLine;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetActiveCell( int* pnCell )
{
	_try_nested(CCarioView, CarioView, GetActiveCell)
	{	
		if( !pnCell )
			return E_INVALIDARG;

		*pnCell = pThis->m_nActiveCell;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::AddLineAtPos( int nLine )
{
	_try_nested(CCarioView, CarioView, AddLineAtPos)
	{	
		chromo_line* pline = pThis->add_line_at_pos( nLine );
		if( !pline )
			return S_FALSE;

		pline->m_nLineHeight = pThis->m_sizeDefaultLine.cy;

		pThis->rebuild_view();
		
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::AddCellAtPos( int nLine, int nCell )
{
	_try_nested(CCarioView, CarioView, AddCellAtPos)
	{	
		chromo_cell* pcell = pThis->add_cell_at_pos( nLine, nCell );
		if( !pcell )
			return S_FALSE;

		pcell->m_nCellWidth = pThis->m_sizeDefaultLine.cx;		

		pThis->rebuild_view();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::DeleteLine( int nLine )
{
	_try_nested(CCarioView, CarioView, DeleteLine)
	{	
		if( !pThis->delete_line( nLine ) )
			return S_FALSE;

		pThis->rebuild_view();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::DeleteCell( int nLine, int nCell )
{
	_try_nested(CCarioView, CarioView, DeleteCell)
	{	
		if( !pThis->delete_cell( nLine, nCell ) )
			return S_FALSE;

		pThis->rebuild_view();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::DeleteObj( int nLine, int nCell, int nObj )
{
	_try_nested(CCarioView, CarioView, DeleteObj)
	{	

		chromo_cell* pcell		= 0;
		chromo_object* pobject	= 0;

		pThis->get_pointers( nLine, nCell, nObj, 0, &pcell, &pobject );

		if( pcell && pobject )
		{
			pcell->m_arChromos.RemoveAt( nObj );
			delete pobject;
		}

		pThis->rebuild_view();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetActiveChromo( int* pnLine, int* pnCell, int* pnChromo )
{
	_try_nested(CCarioView, CarioView, GetActiveChromo)
	{	
		if( pnLine )
			*pnLine		= pThis->m_nActiveLine;

		if( pnCell )
			*pnCell		= pThis->m_nActiveCell;

		if( pnChromo )
			*pnChromo	= pThis->m_nActiveChromo;

		chromo_object* pobject = 0;
		pThis->get_pointers( pThis->m_nActiveLine, pThis->m_nActiveCell, pThis->m_nActiveChromo,
									0, 0, &pobject );

		if( !pobject && pnChromo )
			*pnChromo = -1;
	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::SetActiveChromo( int nLine, int nCell, int nChromo )
{
	_try_nested(CCarioView, CarioView, SetActiveChromo)
	{	
		pThis->m_nActiveLine	= nLine;
		pThis->m_nActiveCell	= nCell;
		pThis->m_nActiveChromo	= nChromo;
		pThis->invalidate_rect( 0 );

	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::SetChromoAngle( int nLine, int nCell, int nChromo, double fAngle )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView)
	chromo_object* pobject = 0;
	pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
	if( !pobject )
		return S_FALSE;

	IChromosomePtr ptrChromo( pobject->m_ptrChromos );
	if( ptrChromo == 0 )
		return S_FALSE;

	ROTATEPARAMS rp;
	::ZeroMemory( &rp, sizeof(ROTATEPARAMS) );

	ptrChromo->GetRotateParams( &rp );
	rp.dAngleVisible = fAngle-rp.dAngle;
	rp.bManual = TRUE;

	ptrChromo->SetRotateParams( &rp );		
	ptrChromo->SetRotatedImage( 0 );
	ptrChromo->SetEqualizedRotatedImage( 0 );

	INamedDataObject2Ptr ptrNDO( ptrChromo );
	if( ptrNDO )
	{
		IUnknown* punkParent = 0;
		ptrNDO->GetParent( &punkParent );
		if( punkParent )
		{
			//calc here
//			::CalcObject( punkParent, 0, true, 0 );
			punkParent->Release();
		}
	}
	

	pThis->synchronize_dimmension( SDF_RECALC_OBJECT, pobject->m_ptrMeasureObject );
	pThis->set_client_size();
	pThis->Invalidate();
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetChromoAngle( int nLine, int nCell, int nChromo, double* pfAngle )
{
	_try_nested(CCarioView, CarioView, GetChromoAngle)
	{	
		if( !pfAngle )
			return E_INVALIDARG;

		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		IChromosomePtr ptrChromo( pobject->m_ptrChromos );
		if( ptrChromo == 0 )
			return S_FALSE;

		ROTATEPARAMS rp;
		::ZeroMemory( &rp, sizeof(ROTATEPARAMS) );

		ptrChromo->GetRotateParams( &rp );

		*pfAngle = rp.dAngle+rp.dAngleVisible;
	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL* pbShow )
{
	_try_nested(CCarioView, CarioView, GetShowChromoIdiogramm)
	{	
		if( !pbShow )
			return E_INVALIDARG;

		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject || pobject->m_ptrIdio == 0 )
			return S_FALSE;

		*pbShow = ( pobject->m_bShowIdiogramm == true );
	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::SetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL bShow )
{
	_try_nested(CCarioView, CarioView, SetShowChromoIdiogramm)
	{	
		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		pobject->m_bShowIdiogramm = ( bShow == TRUE ) && pobject->m_ptrIdio != 0;

		pThis->synchronize_dimmension( SDF_RECALC_OBJECT, pobject->m_ptrMeasureObject );
		pThis->set_client_size();
		pThis->Invalidate();


	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetShowChromoAllIdiogramm( BOOL *pbShow )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView)
	*pbShow = pThis->m_bShowCellIdio;
	return S_OK;
}

HRESULT CCarioView::XCarioView::SetShowChromoAllIdiogramm( BOOL bShow )
{
	_try_nested(CCarioView, CarioView, SetShowChromoAllIdiogramm)
	{	
		pThis->m_bShowCellIdio = bShow != 0;
		
		CString	strSect = pThis->get_view_settings_section();
		::SetValue( GetAppUnknown(), strSect, "ShowCellIdio", (long)pThis->m_bShowCellIdio );
		::SetValue( pThis->m_ptrActiveObjectList, strSect, "ShowCellIdio", (long)pThis->m_bShowCellIdio );
		
		if( bShow )
			pThis->load_idiograms();
		
		pThis->synchronize_dimmension( SDF_RECALC_ALL );
		pThis->set_client_size();
		pThis->Invalidate();
	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetChromoObject( int nLine, int nCell, int nChromo, IUnknown** ppunk )
{
	_try_nested(CCarioView, CarioView, GetChromoZoom)
	{	
		if( !ppunk )
			return E_INVALIDARG;

		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		if( pobject->m_ptrChromos )
			pobject->m_ptrChromos->AddRef();

		*ppunk = pobject->m_ptrChromos;

	}
	
	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::SetChromoZoom( int nLine, int nCell, int nChromo, double fZoom )
{
	_try_nested(CCarioView, CarioView, SetChromoZoom)
	{	
		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		pobject->m_fZoom = fZoom;

		pThis->synchronize_dimmension( SDF_RECALC_ALL );
		pThis->set_client_size();
		pThis->Invalidate();

	}
	
	return S_OK;

	_catch_nested;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetChromoZoom( int nLine, int nCell, int nChromo, double *fZoom )
{
	_try_nested(CCarioView, CarioView, GetChromoZoom)
	{	
		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		*fZoom = pobject->m_fZoom;
	}
	
	return S_OK;

	_catch_nested;
}
HRESULT CCarioView::XCarioView::GetChromoPath( int nLine, int nCell, int nChromo, char **pstr )
{
	_try_nested(CCarioView, CarioView, GetChromoZoom)
	{	
		CString strSection = pThis->get_lines_section();

		if( strSection.IsEmpty() )
			return S_FALSE;	

		if( nLine == -1 || nCell == -1 || nChromo == -1 )
			return S_FALSE;

		CString strSection2;

		strSection2.Format( "%s\\%s%d", strSection, CARIO_LINE_PREFIX, nLine );
		strSection.Format( "%s\\%s\\%s%d", strSection2, CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, nCell );
		strSection2.Format( "%s\\%s\\%s%d", strSection, CARIO_CHROMOS_PREFIX, CARIO_CHROMO_PREFIX, nChromo );

		strcpy( *pstr, strSection2 );
	}
	
	return S_OK;

	_catch_nested;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// A.M.
HRESULT CCarioView::XCarioView::GetCarioSizes( int* pcx, int* pcy)
{
	_try_nested(CCarioView, CarioView, GetCarioSizes)
	{
		CSize sz = pThis->get_occuped_size();
		if (pcx) *pcx = sz.cx;
		if (pcy) *pcy = sz.cy;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCarioView::XCarioView::GetCellByPoint( POINT ptObj, int *pnLine, int *pnCell, int *pnChromoObj, BOOL bTakeLineHeight )
{
	_try_nested(CCarioView, CarioView, GetCellByPoint)
	{
		pThis->hit_test_area(ptObj, pnLine, pnCell, pnChromoObj, bTakeLineHeight == TRUE );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCarioView::XCarioView::GetCellClass( int nLine, int nCell, int *pnClass )
{
	_try_nested(CCarioView, CarioView, GetCellClass)
	{
		chromo_cell* pcell = NULL; 
		pThis->get_pointers(nLine, nCell, -1, NULL, &pcell, NULL);
		if( pnClass )
			*pnClass = pcell?pcell->m_nClassNum:-1;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCarioView::XCarioView::SetActiveLine( int nLine )
{
	_try_nested(CCarioView, CarioView, SetActiveLine)
	{
		pThis->m_nActiveLine = nLine;
		pThis->invalidate_rect(NULL);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCarioView::XCarioView::SetActiveCell( int nCell )
{
	_try_nested(CCarioView, CarioView, SetActiveCell)
	{
		pThis->m_nActiveCell = nCell;
		pThis->invalidate_rect(NULL);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCarioView::XCarioView::ArrageObjectsByClasses()
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView)
	pThis->set_class_num();
	return pThis->arrage_by_classes()?S_OK:E_INVALIDARG;
}

HRESULT CCarioView::XCarioView::MirrorChromo( int nLine, int nCell, int nChromo )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView)
	chromo_object* pobject = 0;
	pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
	if( !pobject )
		return S_FALSE;

	IChromosomePtr ptrChromo( pobject->m_ptrChromos );
	if( ptrChromo == 0 )
		return S_FALSE;

	ROTATEPARAMS rp;
	::ZeroMemory( &rp, sizeof(ROTATEPARAMS) );

	ptrChromo->GetRotateParams( &rp );
	rp.bMirrorV = true;
	rp.bManual = TRUE;

	ptrChromo->SetRotateParams( &rp );		
	ptrChromo->SetRotatedImage( 0 );
	ptrChromo->SetEqualizedRotatedImage( 0 );

	INamedDataObject2Ptr ptrNDO( ptrChromo );
	if( ptrNDO )
	{
		IUnknown* punkParent = 0;
		ptrNDO->GetParent( &punkParent );
		if( punkParent )
		{
			//calc here
//			::CalcObject( punkParent, 0, true, 0 );
			punkParent->Release();
		}
	}
	

	pThis->synchronize_dimmension( SDF_RECALC_OBJECT, pobject->m_ptrMeasureObject );
	pThis->set_client_size();
	pThis->Invalidate();
	
	return S_OK;
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


/////////////////////////////////////////////////////////////////////////////
void CCarioView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( m_mouseMode != mmNone )
		return;

	if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
		return;

	bool bhave_object = false;
	{
		CPoint pt = point;
		ScreenToClient( &pt );
		pt = ::ConvertToClient( GetControllingUnknown(), pt );

		IUnknown* punkObject = NULL;
		punkObject = GetObjectByPoint( pt );
		if( punkObject )
		{
			bhave_object = true;
			punkObject->Release();	punkObject = 0;
		}
	}


	CPoint	pointClient = point;
	ScreenToClient( &pointClient );
	_OnLButtonDown(0, pointClient );
	_OnLButtonUp(0, pointClient );
	
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;

	CMenuRegistrator rcm;	

	if( m_carioViewMode == cvmPreview || m_carioViewMode == cvmDesign )
	{
		_bstr_t	bstrMenuName = ( bhave_object ? rcm.GetMenu( szCarioObjectView, szTypeObject ) : rcm.GetMenu( szCarioView, 0 ) );
		sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
	}
	else if( m_carioViewMode == cvmCellar )
	{
		_bstr_t	bstrMenuName = ( bhave_object ? rcm.GetMenu( szCarioObjectAnalizeView, szTypeObject ) : rcm.GetMenu( szCarioAnalizeView, 0 ) );
		sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
	}

}

/////////////////////////////////////////////////////////////////////////////
HBRUSH CCarioView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd && pWnd->GetDlgCtrlID() == 6543210 )
	{
		pDC->SetTextColor( RGB( 0, 0, 0 ) );
		pDC->SetBkMode(OPAQUE);		
		return hbr; 
	}

	if(nCtlColor != CTLCOLOR_EDIT)
		return hbr; 

	COLORREF color;
	color = ::GetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Fore", RGB(0,255,0));	
	pDC->SetTextColor( color );
	color = ::GetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Back", RGB(127,255,127));	

	int transparent = ::GetValueInt(GetAppUnknown(), "Editor", "EditTransparentCommentText", 0);

	if(transparent == 1)
	{
		pDC->SetBkMode( TRANSPARENT);
		return (HBRUSH)::GetStockObject( NULL_BRUSH );
	}
	else
	{
		pDC->SetBkMode(OPAQUE);		
		pDC->SetBkColor( color );
		return (HBRUSH)::GetStockObject( NULL_BRUSH );
	}
}


/////////////////////////////////////////////////////////////////////////////
DWORD CCarioView::GetMatchType( const char *szType )
{
	if( ::GetValueInt( ::GetAppUnknown(), get_view_settings_section(), "EnableViewMatch", 1L ) != 1L )
		return mvNone;

	if( !strcmp(szType, szTypeObjectList )  )
		return mvFull;
	else if( !strcmp( szType, szDrawingObjectList ) )
		return mvPartial;

	return mvNone;
}

/////////////////////////////////////////////////////////////////////////////
POSITION CCarioView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)m_listVisibleObjects.head();
}

/////////////////////////////////////////////////////////////////////////////
IUnknown * CCarioView::GetNextVisibleObject( POSITION &rPos )
{
	long	lpos = (long)rPos;
	IUnknown	*punk = m_listVisibleObjects.get( lpos );
	(long&)rPos = m_listVisibleObjects.next( lpos );
	if( punk )punk->AddRef();
	return punk;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CCarioView::GetObjectByPoint( CPoint pt )
{
//	return 0;

	if( m_drawobjects == 0 ) return 0;

	INamedDataObject2Ptr ptrNDO( m_drawobjects );
	if( ptrNDO == 0 ) return 0;

	long lpos = 0;
	ptrNDO->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown* punk = 0;
		ptrNDO->GetNextChild( &lpos, &punk );

		IRectObjectPtr	ptrObject( punk );
		punk->Release();	punk = 0;

		if( ptrObject == 0 )	continue;

		ICommentObjPtr sptrComment = ptrObject;

		if( sptrComment != 0 && m_carioViewMode == cvmDesign )
				continue;

		long lhit_test = 0;
		ptrObject->HitTest( pt, &lhit_test );

		if( lhit_test != 0 )
		{
			ptrObject->AddRef();
			return ptrObject;
		}
	}
	

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CCarioView::GetViewPtr()
{
	return GetControllingUnknown();
}

/////////////////////////////////////////////////////////////////////////////
HWND CCarioView::GetHwnd()
{
	return GetSafeHwnd();
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::on_deactivate_object_list( IUnknown* punkObjectList )
{
	m_drawobjects = 0;	
	m_pframe->EmptyFrame();
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::on_activate_object_list( IUnknown* punkObjectList )
{
	//if( punkObjectList )
	{
		IUnknown* punkObj = punkObjectList;
		init_attached_object_named_data( punkObjectList );

		if( !is_correct_object_list( punkObj ) )
			punkObj = 0;

		bool b1 = m_bCanSaveToObjectAndShellData;
		bool b2 = m_bCanFireEventDataChange;

		m_bCanSaveToObjectAndShellData	= false;
		m_bCanFireEventDataChange		= false;

		if( !reload_view_settings( punkObj ) )
			reload_view_settings( ::GetAppUnknown() );

		reload_object_from_data( punkObj );

		m_bCanSaveToObjectAndShellData	= b1;
		m_bCanFireEventDataChange		= b2;

		//load drawings
		if( punkObj )
		{			
			CString str_path = get_drawing_path();
			INamedDataPtr ptrND( punkObj );

			if( str_path.GetLength() && ptrND != 0 )
			{			
				IUnknownPtr ptrDR;

				_variant_t var;
				ptrND->GetValue( _bstr_t( (LPCSTR)str_path ), &var );
				if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
				{
					ptrDR = var.punkVal;					
				}
				else
				{
					IUnknown* punk_drawings = ::CreateTypedObject( szDrawingObjectList );
					if( punk_drawings )
					{
						ptrND->SetValue( _bstr_t( (LPCSTR)str_path ), _variant_t( (IUnknown*)punk_drawings ) );
						ptrDR = punk_drawings;
						punk_drawings->Release();	punk_drawings = 0;
					}
				}

				m_drawobjects = ptrDR;

				ensure_visible( m_nActiveLine, m_nActiveCell, m_nActiveChromo );

				/*
				if( ptrDR )
				{
					m_listVisibleObjects.insert_before( ptrDR, 0 );
					ptrDR->Release();	punk = 0;
				}

				if( ::GetObjectKey( ptrDR ) != ::GetObjectKey( m_drawobjects ) )
				{
					m_drawobjects = ptrDR;
					Invalidate( 0 );
				}
				*/

			}
		}	
		set_client_size();

	}

	if( GetSafeHwnd() ) 
		Invalidate( 0 );

	{
		CString	strSect = get_view_settings_section();
		bool bShow = ( 1L == ::GetValueInt( m_ptrActiveObjectList, strSect, "ShowCellIdio", 0 ) );

		if( bShow != m_bShowCellIdio )
		{
			ICarioViewPtr spView = GetControllingUnknown();
			spView->SetShowChromoAllIdiogramm( bShow );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CCarioView::set_client_size()
{
	CSize size = get_occuped_size();

	IScrollZoomSitePtr ptrSZS( GetControllingUnknown() );
	if( ptrSZS == 0 )
		return;

	ptrSZS->SetClientSize( size );
}


/////////////////////////////////////////////////////////////////////////////
bool CCarioView::_OnLButtonDown(UINT nFlags, CPoint point) 
{	
	::SendMessage( ::GetFocus(), WM_USER + 1234, 0, 0 );

	CTimeTest	test( true, "CCarioView::_OnLButtonDown" );
	{
		CPoint ptClient = ConvertToClient( GetControllingUnknown(), point );

		IUnknown* punk = GetObjectByPoint( ptClient );
		if( punk )
		{
			punk->Release();	punk = 0;
			return false;
		}
	}

	SetCapture();

	CPoint ptObj = convert_from_window_to_object( point );

	m_ptMousePrevPos	= point;
	m_ptOffset			= CPoint( 0, 0 );
	m_ptFirstClickPos	= point;

	//сначала по клику активизируем строку и хромосому

	int nLine, nCell, nChromo;
	m_mouseMode = get_mouse_mode_by_point( ptObj, &nLine, &nCell, &nChromo );

	if( nFlags != UINT(-1) && m_mouseMode == mmMoveChromo && GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) 
	{
		m_mouseMode = mmNone;
		ReleaseCapture();
		SetCursor( m_hcurLast = m_hCursorArrow );	
		GetAppUnknown()->IdleUpdate();
		::ExecuteAction( "SetChromoAngle", "0,1" );
		m_bActionExecuted = true;
	}


	bool bChangeLine = false;
	bool bChangeCell = false;

	if( nLine != -1 && nLine != m_nActiveLine )
	{
		bChangeLine = true;
		CRect rcOldLine = get_chromo_line_rect( m_nActiveLine, 0 );
		rcOldLine = convert_from_object_to_wnd( rcOldLine );

		m_nActiveLine = nLine;

		CRect rcNewLine = get_chromo_line_rect( m_nActiveLine, 0 );
		rcNewLine = convert_from_object_to_wnd( rcNewLine );

		invalidate_rect( &rcOldLine );
		invalidate_rect( &rcNewLine );
	}

	if( bChangeLine || ( nCell != m_nActiveCell ) )
	{
		bChangeCell = true;
		CRect rcOldCell = get_chromo_cell_rect( m_nActiveLine, m_nActiveCell, 0, 0 );
		rcOldCell = convert_from_object_to_wnd( rcOldCell );

		m_nActiveCell=  nCell;

		CRect rcNewCell = get_chromo_cell_rect( m_nActiveLine, m_nActiveCell, 0, 0 );
		rcNewCell = convert_from_object_to_wnd( rcNewCell );

		invalidate_rect( &rcOldCell );
		invalidate_rect( &rcNewCell );
	}	

	if( nChromo == -1 && nCell != -1 && nLine != -1 )
	{
		chromo_line* pline = m_arCarioLine[m_nActiveLine];
		chromo_cell* pcell = pline->m_arCell[m_nActiveCell];

		CPoint ptScroll = get_scroll_pos();
		double fMin = 1e308;
		for( int i = 0; i < pcell->m_arChromos.GetSize(); i++ )
		{
			CRect rcOldChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, i, 0, 0, 0 );
			rcOldChromo = convert_from_object_to_wnd( rcOldChromo );

			double h = _hypot( rcOldChromo.CenterPoint().x - ptObj.x + ptScroll.x,  rcOldChromo.CenterPoint().y - ptObj.y + ptScroll.y );
			if( h < fMin )
			{
				fMin = h;
				nChromo = i;
			}
		}
		
	}

	if( nChromo != -1 && ( bChangeLine || bChangeCell || ( nChromo != m_nActiveChromo ) ) )
	{
		CRect rcOldChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, 0 );
		rcOldChromo = convert_from_object_to_wnd( rcOldChromo );

		bool	fChangeActive = m_nActiveChromo != nChromo;

		m_nActiveChromo =  nChromo;

		CRect rcNewChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, 0 );
		rcNewChromo = convert_from_object_to_wnd( rcNewChromo );

		invalidate_rect( &rcOldChromo );
		invalidate_rect( &rcNewChromo );

//		if( fChangeActive )
		{
			chromo_object	*po = get_chromo_object( m_nActiveLine, m_nActiveCell, m_nActiveChromo );
			INamedDataObject2Ptr	ptrObjectList( m_ptrActiveObjectList );

			if( ptrObjectList != 0 )
			{
				long	lpos = 0;
				if( po && po->m_ptrMeasureObject != 0 )
				{
					INamedDataObject2Ptr	ptrN( po->m_ptrMeasureObject );
					ptrN->GetObjectPosInParent( &lpos );
				}
				ptrObjectList->SetActiveChild( lpos );
			}
		}
	}
	if( m_carioViewMode == cvmDesign )
		m_nActiveChromo =  -1;

//[ay] - убираем активность из objectlist
	if( m_nActiveCell == -1 ||
		m_nActiveChromo == -1 ||
		m_nActiveLine == -1 )
	{

		if( m_carioViewMode != cvmDesign )
		{
			INamedDataObject2Ptr	ptrObjectList( m_ptrActiveObjectList );
			if( ptrObjectList != 0 )ptrObjectList->SetActiveChild( 0 ); 
		}
	}


	SetCursor( m_hcurLast = get_cursor_by_mode( m_mouseMode ) );	
	
	//return 0; 
	//__super::OnLButtonDown(nFlags, point);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::_OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_pframe && m_pframe->GetObjectsCount() )
		return false;

	MouseMode mm = m_mouseMode;

	if( mm == mmMoveChromo )
	{
		CPoint ptObj = convert_from_window_to_object( point );

		chromo_object* pchromo = 0;
		CRect rcOldChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, &pchromo );
		
		m_mouseMode = mmNone;

		bool bDrop = false;

		if( GetCursor() == m_hDrop )
		{
			CRect Rect;
			GetWindowRect( &Rect );
			POINT pt;
			::GetCursorPos( &pt );

			if( !Rect.PtInRect( pt ) )
			{
				HWND hWnd = ::WindowFromPoint( pt );
				if( hWnd )
				{
					int nPos = ::SendMessage( hWnd, WM_USER + 102, WPARAM(&pt), LPARAM( sizeof( POINT ) ) );

					CString str;
					if( nPos > 0 )
						str.Format( "%d", nPos - 1 );

					::ExecuteAction( "AddToAnalize", str );
					::SendMessage( hWnd, WM_USER + 103, 0, 0 );

					if( m_bErectAfterDrop )
						::ExecuteAction( "ErectOneChromOnOff", "1,1" );

				}
				bDrop = true;
			}
		}

		if( !( m_ptOffset.x == 0 && m_ptOffset.y == 0 ) )
		{			
			int nLine, nCell, nChromo;
			nLine = nCell = nChromo = -1;
		
			if( !bDrop )
				hit_test_area( rcOldChromo.CenterPoint(), &nLine, &nCell, &nChromo );
			
			chromo_line* pOldLine = 0;
			chromo_cell* pOldCell = 0;		
			chromo_object* pOldObject = 0;		
			get_pointers( m_nActiveLine, m_nActiveCell, m_nActiveChromo, &pOldLine, &pOldCell, &pOldObject );

			chromo_line* pNewLine = 0;
			chromo_cell* pNewCell = 0;		

			get_pointers( nLine, nCell, 0, &pNewLine, &pNewCell, 0 );


			int nLine2 = m_nActiveLine, 
				nCell2 = m_nActiveCell, 
				nChromo2 = m_nActiveChromo;


			if( pOldObject && pOldLine && pOldCell && pNewLine && pNewCell )
			{
				CRect	rectCell = get_chromo_cell_rect( nLine, nCell, 0, 0, false );
				int	xpos = rectCell.left+10;

				int nIdx2Insert = -1;
				for( int i=0;i<pNewCell->m_arChromos.GetSize();i++ )
				{
					CRect rcObject = get_chromo_object_rect( nLine, nCell, i, 0, 0, 0 );
					xpos+=rcObject.Width();
					if( ptObj.x < xpos )
					{
						nIdx2Insert = i;
						break;
					}
				}

				if( !(pOldLine == pNewLine && pOldCell == pNewCell && nIdx2Insert == m_nActiveChromo ) )
				{
					pOldCell->m_arChromos.RemoveAt( m_nActiveChromo );

					if( (m_nActiveChromo = nIdx2Insert) != - 1 )
						pNewCell->m_arChromos.InsertAt( nIdx2Insert, pOldObject );
					else
						m_nActiveChromo = pNewCell->m_arChromos.Add( pOldObject );
					set_chromo_class( pOldObject, pNewCell->m_nClassNum );

					if( pOldObject->m_ptrChromos != 0 )
					{
//						pOldObject->m_ptrChromos->SetManualClass( pNewCell->m_nClassNum );
						pOldObject->m_ptrChromos->SetManualClass( 1 );
					}
				}
				m_nActiveLine = nLine;
				m_nActiveCell = nCell; 
			
			}
			synchronize_dimmension( SDF_NORECALC );
			set_client_size();				

			if( !bDrop && m_carioViewMode != cvmCellar )
			{
				CString strArgs;
				strArgs.Format( "%d, %d, %d, %d, %d, %d", nLine2, nCell2, nChromo2, m_nActiveLine, m_nActiveCell, m_nActiveChromo );

				if( nLine2 != m_nActiveLine || nCell2 != m_nActiveCell || nChromo2 != m_nActiveChromo )
					::ExecuteAction( "SetChromoClass", strArgs );
			}

			ensure_visible( m_nActiveLine, m_nActiveCell, m_nActiveChromo );
			invalidate_rect( 0 );
			
		}
	}
	else if( m_mouseMode == mmSetChromoOffset )
	{
		chromo_object* pchromo = 0;
		get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, &pchromo );
		
		synchronize_dimmension( SDF_RECALC_OBJECT, pchromo->m_ptrMeasureObject );
		set_client_size();				

		invalidate_rect( 0 );
	}
	m_mouseMode = mmNone;
	ReleaseCapture();
	SetCursor( m_hcurLast = m_hCursorArrow );	
	GetAppUnknown()->IdleUpdate();
	if( mm != mmNone )
	{
		save_cario_state_to_data( m_ptrActiveObjectList );
		if( m_carioViewMode == cvmDesign )
			save_cario_state_to_data( GetAppUnknown() );
	}
	

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::_OnRButtonUp(UINT nFlags, CPoint point) 
{
	_OnLButtonDown( -1, point );
	if( m_mouseMode == mmMoveChromo && GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) 
	{

		ICarioViewPtr ptrView = GetControllingUnknown();
		if( ptrView == 0 )
			return false;

		double fAngle = 0;
		ptrView->GetChromoAngle( m_nActiveLine, m_nActiveCell, m_nActiveChromo, &fAngle );

		GetAppUnknown()->IdleUpdate();

		CString str;
		str.Format( "%lf", fAngle * 180 / PI + 180 );
		::ExecuteAction( "SetChromoAngle", str );
		m_bActionExecuted = true;
		m_mouseMode = mmNone;
		return true;
	}
	_OnLButtonUp( nFlags, point );

	return false;
}



/////////////////////////////////////////////////////////////////////////////
bool CCarioView::_OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_pframe && m_pframe->GetObjectsCount() )
		return false;

	CPoint ptObj = convert_from_window_to_object( point );		
	m_bActionExecuted = false;
	
	//set cursor

	if( m_mouseMode == mmNone )
	{
		MouseMode mm = get_mouse_mode_by_point( ptObj, 0, 0, 0 );	
		SetCursor( m_hcurLast = get_cursor_by_mode( mm ) );
	}
	else if( m_mouseMode != mmMoveChromo )
		SetCursor( m_hcurLast = get_cursor_by_mode( m_mouseMode ) );

	if( m_mouseMode == mmMoveCell )
	{
		if( m_nActiveLine >= 0 && m_nActiveLine < m_arCarioLine.GetSize() )
		{			
			chromo_line* pline = m_arCarioLine[m_nActiveLine];
			if( m_nActiveCell >= 0 && m_nActiveCell < pline->m_arCell.GetSize() )
			{
				m_bHasBeenChange = true;
				chromo_cell* pcell = pline->m_arCell[m_nActiveCell];

				CRect rcLineOld = get_chromo_line_rect( m_nActiveLine, 0 );
				rcLineOld = convert_from_object_to_wnd( rcLineOld );				
				
				int nDelta = ( point.x - m_ptMousePrevPos.x ) / get_zoom();
				pcell->m_nCellOffset += nDelta;

				if( pcell->m_nCellOffset < 0 )
					pcell->m_nCellOffset = 0;

				CPoint	pt_old( 0, 0 ), pt_new( 0, 0 );

				IScrollZoomSitePtr ptr_site( GetControllingUnknown() );
				if( ptr_site )
					ptr_site->GetScrollPos( &pt_old );

				synchronize_dimmension( SDF_NORECALC );
				set_client_size();

				if( ptr_site )
					ptr_site->GetScrollPos( &pt_new );

				CRect rcLineNew = get_chromo_line_rect( m_nActiveLine, 0 );
				rcLineNew = convert_from_object_to_wnd( rcLineNew );

				rcLineOld.InflateRect( 5, 5, 5, 5 );
				rcLineNew.InflateRect( 5, 5, 5, 5 );

				if( pt_old != pt_new )
					Invalidate( 0 );
				else
				{
					invalidate_rect( &rcLineOld );
					invalidate_rect( &rcLineNew );
				}
			}
		}
		UpdateWindow();
	}
	else if( /*m_mouseMode == mmResizeLineUp ||*/ m_mouseMode == mmResizeLineDown )
	{
		int nDelta = ( point.y - m_ptMousePrevPos.y ) / get_zoom( );

		if( m_nActiveLine >= 0 && m_nActiveLine < m_arCarioLine.GetSize() )
		{
			m_bHasBeenChange = true;

			chromo_line* pline = m_arCarioLine[m_nActiveLine];			
			pline->m_nLineHeight += nDelta;
			if( pline->m_nLineHeight < m_nMinLineHeight )
				pline->m_nLineHeight = m_nMinLineHeight;

			synchronize_dimmension( SDF_NORECALC );
			set_client_size();

			Invalidate( 0 );
		}
		UpdateWindow();
	}
	else if( /*m_mouseMode == mmResizeCellLeft || */ m_mouseMode == mmResizeCellRight )
	{

		if( m_nActiveLine >= 0 && m_nActiveLine < m_arCarioLine.GetSize() )
		{			
			chromo_cell* pcell = 0;
			
			if( m_nActiveLine >= 0 && m_nActiveLine < m_arCarioLine.GetSize() )
			{
				chromo_line* pline = m_arCarioLine[m_nActiveLine];
				if( m_nActiveCell >= 0 && m_nActiveCell < pline->m_arCell.GetSize() )
				{
					pcell = pline->m_arCell[m_nActiveCell];
				}
			}

			if( pcell )
			{
				int nDelta = ( point.x - m_ptMousePrevPos.x ) / get_zoom( );

				CRect rcOldLine = get_chromo_line_rect( m_nActiveLine, 0 );				
				rcOldLine = convert_from_object_to_wnd( rcOldLine );				
				
				if( !( pcell->m_nCellOffset == 0 && false/*m_mouseMode == mmResizeCellLeft*/ ) )
				{
					CPoint	pt_old( 0, 0 ), pt_new( 0, 0 );

					IScrollZoomSitePtr ptr_site( GetControllingUnknown() );
					if( ptr_site )
						ptr_site->GetScrollPos( &pt_old );


					if( pcell->m_nCellOffset && false/*m_mouseMode == mmResizeCellLeft*/ )
					{
						pcell->m_nCellOffset += nDelta;
						pcell->m_nCellWidth -= nDelta;
					}
					else
					{
						pcell->m_nCellWidth += nDelta;
					}
					
					if( pcell->m_nCellWidth < m_nMinCellWidth )
						pcell->m_nCellWidth = m_nMinCellWidth;

					if( pcell->m_nCellOffset < 0 )
						pcell->m_nCellOffset = 0;


					if( ptr_site )
						ptr_site->GetScrollPos( &pt_new );
					
					m_bHasBeenChange = true;
					synchronize_dimmension( SDF_NORECALC );

					CRect rcNewLine = get_chromo_line_rect( m_nActiveLine, 0 );
					rcNewLine = convert_from_object_to_wnd( rcNewLine );

					if( pt_old != pt_new )
						Invalidate( 0 );
					else
					{
						invalidate_rect( &rcOldLine );
						invalidate_rect( &rcNewLine );
					}
					
					set_client_size();
				}
			}
		}
		UpdateWindow();
	}
	else if( m_mouseMode == mmMoveChromo )
	{
		update_zoom_scroll();
		chromo_object* pchromo = 0;

		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		CRect rc;

		{
			RECT rcT;
			ptrZ->GetVisibleRect( &rcT );

			rc = rcT;
		}
		{
			CRect Rect = rc;

			GetWindowRect( &Rect );
			POINT pt;
			::GetCursorPos( &pt );

			if( !Rect.PtInRect( pt ) )
			{
				HWND hWnd = ::WindowFromPoint( pt );
				bool bNoDrag = true;
				
				if( hWnd )
					bNoDrag = ::SendMessage( hWnd, WM_USER+101, 0, 0 ) != 1;
					
				if( bNoDrag )
					SetCursor( m_hNoDrag );
				else
					SetCursor( m_hDrop );

				return false;
			}
			else
				SetCursor( m_hcurLast = get_cursor_by_mode( m_mouseMode ) );
		}

		CRect rcOldChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, &pchromo );
		rcOldChromo = convert_from_object_to_wnd( rcOldChromo );
		
		m_ptOffset = ptObj - convert_from_wnd_to_object( m_ptFirstClickPos );
		m_ptFirstClickPos = point;
		pchromo->m_ptOffset += m_ptOffset;

		CRect rcNewChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, 0 );
		rcNewChromo = convert_from_object_to_wnd( rcNewChromo );		

		if( !m_ptOffset.x && !m_ptOffset.y )
			m_ptOffset.x = 1;

/******** Maxim [21.05.2002] **********************************************************************/
	

		CPoint ptScroll = get_scroll_pos();

		CPoint ptS[2] = { rcOldChromo.TopLeft(), rcOldChromo.BottomRight() };
		int dw = (rcOldChromo.Width() - rc.Width()) / 2;
		if( dw > 0 )
		{
			ptS[0].x = point.x-rc.Width()/4;//+= dw;
			ptS[1].x = point.x+rc.Width()/4;// -= dw;
		}
		int dh = (rcOldChromo.Height() - rc.Height() + 1) / 2;
		if( dh > 0 )
		{
			ptS[0].y = point.y-rc.Height()/4;// += dh;
			ptS[1].y = point.y+rc.Height()/4;// -= dh;
		}

		BOOL bOK = true;
		double fZoom = get_zoom();

		CPoint ptOffset;
		for( int i = 0; i < 2; i++ )
		{
			if( !rc.PtInRect( ptS[i] ) )
			{
				int dx = 0, dy = 0;
				if( ptS[i].x >= rc.right )
					dx = ptS[i].x - rc.right + 1;
				else if( ptS[i].x < rc.left )
					dx = ptS[i].x - rc.left;

				if( ptS[i].y >= rc.bottom )
					dy = ptS[i].y - rc.bottom + 1;
				else if( ptS[i].y < rc.top )
					dy = ptS[i].y - rc.top;

				ptScroll.Offset( dx, dy );

				SIZE sz;
				ptrZ->GetClientSize( &sz );

				if( CRect( 0, 0, sz.cx*fZoom, sz.cy*fZoom ).PtInRect( ptScroll ) )
				{
					ptrZ->SetScrollPos( ptScroll );

					CPoint ptObj1 = convert_from_window_to_object( point );		
					
					m_ptOffset.x = m_ptOffset.y = 1;
					 
					POINT pt;

					::GetCursorPos( &pt );
					 pt.x -= ( ptObj1.x - ptObj.x ) * fZoom;
					 pt.y -= ( ptObj1.y - ptObj.y ) * fZoom;
					
					 ::SetCursorPos( pt.x, pt.y );

					 point.x -=  ( ptObj1.x - ptObj.x ) * fZoom;
					 point.y -=  ( ptObj1.y - ptObj.y ) * fZoom;

					 m_ptFirstClickPos = point;

					 bOK = false;

				}
			}
		}

		{
			rc = convert_from_object_to_wnd( rc );	

			if( rcNewChromo.left < rc.left )
				pchromo->m_ptOffset.x -= rcNewChromo.left - rc.left;

			if( rcNewChromo.top < rc.top )
				pchromo->m_ptOffset.y -= rcNewChromo.top - rc.top;
		}

		if( bOK )
		{
			CRect rc;
			rc.UnionRect( &rcOldChromo, &rcNewChromo);
//			invalidate_rect( &rcNewChromo );
//			invalidate_rect( &rcOldChromo );
			invalidate_rect( &rc );
			UpdateWindow();
		}

/******** Maxim [21.05.2002] **********************************************************************/
	}
	else if( m_mouseMode == mmSetChromoOffset )
	{
		m_bHasBeenChange = true;
		chromo_object* pchromo = 0;
		CRect rcOldChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, &pchromo );
		
		rcOldChromo = convert_from_object_to_wnd( rcOldChromo );

		m_ptOffset.x = 0;
		m_ptOffset.y = ptObj.y - convert_from_wnd_to_object( m_ptFirstClickPos ).y;
		m_ptFirstClickPos = point;
		pchromo->m_ptOffset += m_ptOffset;
		pchromo->m_nYOffset += m_ptOffset.y;
		
		CRect rcNewChromo = get_chromo_object_rect( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, 0 );
		rcNewChromo = convert_from_object_to_wnd( rcNewChromo );		

		invalidate_rect( &rcOldChromo );
		invalidate_rect( &rcNewChromo );
		UpdateWindow();
	}
	
	m_ptMousePrevPos = point;
	
	if( m_mouseMode == mmNone && !m_bActionExecuted )
	{
		CPoint ptClient = point;

		BOOL bOK = false;

		int l = -1, c = -1, o = -1;
		if( get_mouse_mode_by_point( ptObj, &l, &c, &o ) == mmMoveChromo )
			bOK = true;  

		CString sToolText;
		if( bOK )
		{
			bOK = false;

			chromo_object *pobj = get_chromo_object( l, c, o );

			if( pobj )
			{
				CRect rectDest = pobj->m_rectCGHGraph;
				rectDest.OffsetRect( pobj->m_ptOffset );
				rectDest = convert_from_object_to_wnd( rectDest );

				if( rectDest.PtInRect( ptClient ) )
				{
					bOK = true;

					if( GetAsyncKeyState( _MASTER_KEY_ ) & 0x8000 )
						invalidate_rect( &rectDest );
				}
			}

			if( pobj && !bOK )
			{
				CRect rectDest = pobj->m_rectCGHRatio;
				rectDest.OffsetRect( pobj->m_ptOffset );
				rectDest = convert_from_object_to_wnd( rectDest );

				if( rectDest.PtInRect( ptClient ) )
				{
					if( m_bShowCGHRatio && pobj->m_sptrRatio != 0 )
					{
						double x = 0, y = 0;
						if( pobj->m_sptrRatio->GetToolTipValue( 0, ptClient, &x, &y, 0, 1 ) == S_OK )
							sToolText.Format( "%lf", y );
				
						if( GetAsyncKeyState( _MASTER_KEY_ ) & 0x8000 )
							invalidate_rect( &rectDest );
					}
				}
			}
			else if( pobj && bOK )
			{
 				if( m_bShowCGHGraph && pobj->m_sptrChart != 0 )
				{
					double x = 0, y = 0;
					if( pobj->m_sptrChart->GetToolTipValue( 0, ptClient, &x, &y, 0, 1 ) == S_OK )
						sToolText.Format( "%lf", y );
					else
					{
						double x = 0, y = 0;
						if( pobj->m_sptrChart->GetToolTipValue( 1, ptClient, &x, &y, 0, 1 ) == S_OK )
						sToolText.Format( "%lf", y );
					}
				}
			}
		}
		
		if( sToolText.IsEmpty() )
			sToolText = get_idio_hint( l, c, o, ptClient );

		m_ToolTipCtrl.UpdateTipText( sToolText, this, 123 );
		m_ToolTipCtrl.Activate( !sToolText.IsEmpty() );
	}

	return true;
}



/////////////////////////////////////////////////////////////////////////////
BOOL CCarioView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{	
	if( !m_hcurLast )m_hcurLast = m_hCursorArrow;

	SetCursor( m_hcurLast );
	return true;//__super::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
void CCarioView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( m_bSkipNotify )
		return;

	if(!strcmp(pszEvent, szEventNewSettings))
	{
		//if( m_ptrActiveObjectList )
		{
			load_idiograms();

			reload_view_settings( ::GetAppUnknown() );		
			save_view_settings( m_ptrActiveObjectList );			

			synchronize_dimmension( SDF_RECALC_ALL );
			set_client_size();

			invalidate_rect( 0 );
		}
	}
	else if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		if( m_bLockObjectUpdate )return;

		INamedDataObject2Ptr ptrND = punkFrom;

		if( ptrND != 0 )
		{
			IUnknown *punkListParent = 0;
			ptrND->GetParent( &punkListParent );

			if( punkListParent )
			{
				punkListParent->Release();

				IUnknown	*punkObject = punkHit;
				long	nAction = *(long*)pdata;


				if( nAction == cncRemove )
					m_guidExclude = ::GetObjectKey( punkObject );
			}
		}

		if( GetObjectKey( punkFrom ) == GetObjectKey( m_ptrActiveObjectList ) )
		{			
			IUnknown	*punkObject = punkHit;
			long	nAction = *(long*)pdata;

			if( nAction == cncAdd )
			{
				if( m_carioViewMode == cvmPreview ||
					m_carioViewMode == cvmDesign )
					if( update_chromosome( punkObject, CHROMO_OBJECT_ADD ) )
					{
						_clear_cache();
						synchronize_dimmension( SDF_RECALC_OBJECT, punkObject );
						set_client_size();
						invalidate_rect( 0 );
					}
			}
			else if( nAction == cncRemove )
			{
				if( update_chromosome( punkObject, CHROMO_OBJECT_DELETE ) )
				{
					_clear_cache();
					synchronize_dimmension( SDF_NORECALC );
					set_client_size();
					invalidate_rect( 0 );
				}
			}
			else if( nAction == cncChange )
			{
				if( update_chromosome( punkObject, CHROMO_OBJECT_CHANGED ) )
				{
					_clear_cache();
					synchronize_dimmension( SDF_RECALC_OBJECT, punkObject );
					set_client_size();
					invalidate_rect( 0 );
				}
			}
			else if( nAction == cncActivate )
			{
				// _clear_cache(); // AAM 01.07.2003 - см. SBT, ## 5, 108
				// проверить - вдруг все-таки надо что-то делать
				on_activate_object( punkObject );
				ensure_visible( m_nActiveLine, m_nActiveCell, m_nActiveChromo );
			}
			else if( nAction == cncReset )
			{
				_clear_cache();
				on_activate_object_list( m_ptrActiveObjectList );
				arrage_by_classes();			
				m_pframe->EmptyFrame();
			}
		}
		/*else if( GetParentKey( punkFrom ) == GetObjectKey( m_ptrActiveObjectList ) )
		{
			long	nAction = *(long*)pdata;
			if( nAction == cncAdd )
				if( m_carioViewMode != cvmPreview &&
					m_carioViewMode != cvmDesign )
					return;

			if( update_chromosome( punkFrom, CHROMO_OBJECT_CHANGED ) )
			{
				synchronize_dimmension( SDF_RECALC_OBJECT, punkFrom );
				set_client_size();
				invalidate_rect( 0 );
			}
		}*/
		else if( GetObjectKey( punkFrom ) == GetObjectKey( m_drawobjects ) )
		{
			long	nAction = *(long*)pdata;
			if( m_pframe )
			{
				if( nAction == cncActivate ||
					nAction == cncAdd || nAction == cncChange )
				{
					if( m_pframe->UnselectObject( punkHit ) )
						m_pframe->SelectObject( punkHit, false );

					// [vanek] - 13.11.2003
					if( nAction == cncAdd )
						set_client_size();
				}
				else if( nAction == cncRemove )
				{
					m_pframe->UnselectObject( punkHit );
                    
					// [vanek] - 13.11.2003
					{
						synchronize_dimmension( SDF_RECALC_ALL );
                        CSize size = get_occuped_size( punkHit );
						IScrollZoomSitePtr ptrSZS( GetControllingUnknown() );
						if( ptrSZS != 0 )
							ptrSZS->SetClientSize( size );					
					}
				}
				else
					m_pframe->EmptyFrame();

			}

						

			invalidate_rect( 0 );
		}
	}
	else if( !strcmp(pszEvent, szEventCarioChange) )
	{
		if( pdata && CarioViewMode( *( ( int * )pdata ) ) == cvmCellar && m_carioViewMode != cvmCellar )
			return;

		if( ::GetObjectKey( GetControllingUnknown() ) == ::GetObjectKey( punkFrom ) )
			return;

		if( ::GetObjectKey( m_ptrActiveObjectList ) != ::GetObjectKey( punkHit ) )
			return;

		ICarioView2Ptr sptrView = GetControllingUnknown();

		bool b2 = m_bCanFireEventDataChange;
		m_bCanFireEventDataChange		= false;

		reload_object_from_data( m_ptrActiveObjectList );
		set_client_size();
		
		if( GetSafeHwnd() )
			Invalidate( 0 );

		if( pdata && CarioViewMode( *( ( int * )pdata ) ) == cvmCellar )
		{
			ICarioViewPtr sptrView = GetControllingUnknown();
			sptrView->SetActiveChromo( 0, 0, cbSize );

			if( m_bErectAfterDrop )	  // !!!!!
			{
				SetFocus();
				IViewPtr sptrV = sptrView;
				if( sptrV )
				{
					IUnknown *punkDoc = 0;
					sptrV->GetDocument( &punkDoc );

					if( punkDoc )
					{
						IDocumentSitePtr ptrD = punkDoc;
						punkDoc->Release();

						if( ptrD )
						{
							ptrD->SetActiveView( sptrV );
							::ExecuteAction( "ErectOneChromOnOff", "1,1" );
						}
					}
				}
			}

			CRect rc = get_chromo_object_rect( 0, 0, cbSize, 0, 0, 0 );
			IScrollZoomSitePtr ptrZ = sptrView;
			ptrZ->EnsureVisible( rc );
		}

		m_bCanFireEventDataChange		= b2;		
	}
	else if( !strcmp(pszEvent, szEventCarioBuild) )
	{
		if( ::GetObjectKey( m_ptrActiveObjectList ) != ::GetObjectKey( punkHit ) )
			return;

		on_activate_object_list( m_ptrActiveObjectList );
		arrage_by_classes();
	}
	else if( !strcmp( pszEvent, szEventNewSettingsView ) )
	{	// [aam] 24.08.2004
		InvalidateRect( 0 );
	}
	/*
	else if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( m_ptrActiveObjectList ) )
		{
			on_activate_object_list( m_ptrActiveObjectList );
			arrage_by_classes();
			Invalidate( 0 );
		}
	}	
	*/

}

/////////////////////////////////////////////////////////////////////////////
BOOL CCarioView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
	{
		if( !IsInteractiveActionRunning() )
		{
			if( message == WM_LBUTTONDOWN )
			{
				if( _OnLButtonDown( wParam, CPoint( LOWORD(lParam), HIWORD(lParam) ) ) )
					return true;
			}
			else if( message == WM_LBUTTONUP )
			{
				if( _OnLButtonUp( wParam, CPoint( (short)LOWORD(lParam), (short)HIWORD(lParam) ) ) )
					return true;

			}
			else if( message == WM_RBUTTONUP )
			{
				if( _OnRButtonUp( wParam, CPoint( (short)LOWORD(lParam), (short)HIWORD(lParam) ) ) )
					return true;

			}
			else if( message == WM_MOUSEMOVE )
			{
				if( _OnMouseMove( wParam, CPoint( (short)LOWORD(lParam), (short)HIWORD(lParam) ) ) )
					return true;
			}
		}
	}
	else if( !m_bActionExecuted )
	{
		if( message == WM_KEYDOWN && (UINT)wParam == _MASTER_KEY_ )
			invalidate_rect( 0 );
		else if( message == WM_KEYUP && (UINT)wParam == _MASTER_KEY_ )
			invalidate_rect( 0 );
	}


	return __super::OnWndMsg( message, wParam, lParam, pResult );
}

static bool fDragChromosome = false;

/////////////////////////////////////////////////////////////////////////////

LRESULT CCarioView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
	if( message == WM_DND_ON_DRAG_OVER )
	{
		if( !fDragChromosome )return true;
		if( GetCarioViewMode() != cvmCellar && (GetCarioViewMode() != cvmPreview ||
			!GetValueInt(GetAppUnknown(), "Karyo\\KaryoViewSharedSettings", "EnableDragAndDropFromMetaphase", 0)))
			return true;

		if( lParam != sizeof(drag_n_drop_data) )
			return true;
		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;
		
		pdata->dwDropEfect = DROPEFFECT_COPY;
		
		return true;
	}
	else if( message == WM_KILLFOCUS )
	{
		m_ToolTipCtrl.Activate( false );
		return false;
	}
	else if( message == WM_DND_ON_DROP )
	{
		if( !fDragChromosome )return false;
		if( GetCarioViewMode() != cvmCellar && (GetCarioViewMode() != cvmPreview ||
			!GetValueInt(GetAppUnknown(), "Karyo\\KaryoViewSharedSettings", "EnableDragAndDropFromMetaphase", 0)))
			return true;

		if( lParam != sizeof(drag_n_drop_data) )
			return true;

		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;

		INamedDataPtr sptrData = m_ptrActiveObjectList;

		{
			CString strSect,tmp;
			strSect.Format( "%s\\%s%d", CARIO_ANALIZE, CARIO_LINE_PREFIX, int(0) );
			tmp.Format( "\\%s\\%s%d", CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, int(0) );
			strSect += tmp;
			
			m_section = strSect;
			tmp.Format( "\\%s", CARIO_CHROMOS_PREFIX );

			m_section += tmp;
			m_undoState.Load( sptrData, m_section);
		}



		bool bres = on_objects_drop( pdata );
		if( bres )
		{
			int l = m_nActiveLine, c = m_nActiveCell, ch = m_nActiveChromo;
			
			build( m_ptrActiveObjectList );
			update_classes();

			m_nActiveLine = l;
			m_nActiveCell = c;
			m_nActiveChromo = ch;

			CRect rc = get_chromo_object_rect( l, c, ch, 0, 0, 0 );

			IScrollZoomSitePtr ptrZ = GetControllingUnknown();
			
			ptrZ->EnsureVisible( rc );

			set_client_size();
			invalidate_rect( 0 );

// [Max] Activate view after drop
			IUnknown *punkDoc = 0;
			IViewPtr sptrView = GetControllingUnknown();
			sptrView->GetDocument( &punkDoc );

			IDocumentSitePtr sptrDoc = punkDoc;
			if( punkDoc )
				punkDoc->Release();
			punkDoc = 0;

			if( sptrDoc != 0 )
				sptrDoc->SetActiveView( sptrView );

			CWnd *pWnd = ::GetWindowFromUnknown( sptrView );
			
			if( pWnd )
			{
				CWnd *pParWnd = pWnd->GetParent();

				if( pParWnd )
					pParWnd->SetFocus();
			}
			
			{
				ICarioView2Ptr ptrView = sptrView;
				if( ptrView == 0 )
					return false;

				BOOL	fShowR;
				ptrView->GetShowChromoAllRatio( &fShowR );

				BOOL	fShowA;
				ptrView->GetShowChromoAllAmplification( &fShowA );

				BOOL	fShowB;
				ptrView->GetShowChromoAllBrightness( &fShowB );

				BOOL	fShowC;
				ptrView->GetShowAllChromoObject( &fShowC );

				BOOL	fShowE;
				ptrView->GetShowAllEditableIdio( &fShowE );

				ptrView->SetShowChromoAllRatio( fShowR );
				ptrView->SetShowChromoAllAmplification( fShowA );
				ptrView->SetShowChromoAllBrightness( fShowB );
				ptrView->SetShowAllChromoObject( fShowC );
				ptrView->SetShowAllEditableIdio( fShowE );
			}

			m_redoState.Load( sptrData, m_section);

			::ExecuteAction( "AddToAnalize3", "" );

			if( m_bErectAfterDrop )
				::ExecuteAction( "ErectOneChromOnOff", "1,1" );
		}
		else
			return true;

		m_pframeDrag->EmptyFrame();
		pdata->dwDropEfect = DROPEFFECT_NONE;
		
		return true;
	}
	else if( message == WM_DND_ON_DRAG_LEAVE )
	{
		if( !fDragChromosome )return false;
		return true;
	}
	else if( message == WM_DND_ON_DRAG_ENTER )
	{
		if( lParam != sizeof(drag_n_drop_data) )
			return true;

		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;
		if( GetCarioViewMode() != cvmCellar &&
			!GetValueInt(GetAppUnknown(), "Karyo\\KaryoViewSharedSettings", "EnableDragAndDropFromMetaphase", 0) )
			return true;

		HGLOBAL hGlobal = 0;
		IDataObjectPtr	ptrDataObject(pdata->punkDataObject);
		
		COleDataObject	oleObject;
		oleObject.Attach(  ptrDataObject, false );
		hGlobal = oleObject.GetGlobalData( ::GetClipboardFormat() );

		if( !hGlobal )return false;

		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );

		CPoint	pointDummy;
		::RestoreMultiFrameFromHandle( hGlobal,  
									m_pframeDrag->GetControllingUnknown(), 
									&pointDummy,
									&params);

		::GlobalUnlock (hGlobal);
		//::GlobalFree(hGlobal);

		CArray<GUID, GUID>	arGuidChromo;
		fDragChromosome = create_chromo_array_from_dd( pdata, arGuidChromo );

		if( fDragChromosome )
			pdata->dwDropEfect = DROPEFFECT_LINK;
	
		return fDragChromosome;
	}
	if( message == WM_KEYDOWN && m_carioViewMode == cvmCellar )
	{
		if( wParam == VK_F2 )
		{

			chromo_object *Obj = get_chromo_object( m_nActiveLine, m_nActiveCell, m_nActiveChromo );
			if( Obj )
			{
				CEditCtrl* pEdit = new CEditCtrl;
				
				pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER  |
								ES_AUTOVSCROLL | ES_MULTILINE,
						CRect( 0, 0, 100, 20 ),  this, 6543210 );			

				pEdit->SetWindowText( Obj->m_strTextInfo );

				CRect rect = Obj->m_rectText;
				rect.OffsetRect( Obj->m_ptOffset );

				rect = convert_from_object_to_wnd( rect );
				CRect rc2 = convert_from_object_to_wnd( Obj->m_rectFull );

				if( rect.Width() < rc2.Width() )
					rect.right = rect.left + rc2.Width();

				pEdit->MoveWindow( &rect, true );
				pEdit->SetFocus();
				pEdit->SetSel( Obj->m_strTextInfo.GetLength(), Obj->m_strTextInfo.GetLength() );
			}
		}
	}

	if( message == ( WM_USER + 0x4456 ) )
	{
		POINT pt = { LOBYTE( wParam ), HIBYTE( wParam ) };
		{
			HWND hWnd = ::WindowFromPoint( pt );
			if( hWnd )
			{
				int nPos = ::SendMessage( hWnd, WM_USER + 102, WPARAM(&pt), LPARAM( sizeof( POINT ) ) );

				CString str;
				if( nPos > 0 )
					str.Format( "%d", nPos - 1 );

				::ExecuteAction( "AddToAnalize", str );
				::SendMessage( hWnd, WM_USER + 103, 0, 0 );
			}
		}

		if( m_bErectAfterDrop )
			::ExecuteAction( "ErectOneChromOnOff", "1,1" );
	}


	return __super::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioView::ensure_visible( int nline, int ncell, int nobj )
{
	if( !GetSafeHwnd() )
		return false;

	CRect rc_visible = NORECT;
	if( m_nActiveChromo >= 0 && m_nActiveLine >= 0 && m_nActiveCell >= 0 )
	{
		rc_visible = get_chromo_object_rect( nline, ncell, nobj, 0, 0, 0 );
	}
/*	else if( m_nActiveLine >= 0 && m_nActiveCell >= 0 )
	{
		rc_visible = get_chromo_cell_rect( nline, ncell, 0, 0 );

	}
	else if( m_nActiveLine )
	{
		rc_visible = get_chromo_line_rect( nline, 0 );
	}*/

	if( rc_visible == NORECT )
		return false;

	rc_visible.InflateRect( 10, 10 );

	sptrIScrollZoomSite	sptrS( GetControllingUnknown() );

	if( sptrS ) 
	{
		CRect rc;
		{
			RECT rcT;
			sptrS->GetVisibleRect( &rcT );
			rc = rcT;
		}

		/*
		int dw = (rc_visible.Width() - rc.Width() + 1) / 2;
		if( dw > 0 )
		{
			rc_visible.left += dw;
			rc_visible.right -= dw;
		}
		int dh = (rc_visible.Height() - rc.Height() + 1) / 2;
		if( dh > 0 )
		{
			rc_visible.top += dh;
			rc_visible.bottom -= dh;
		}
		*/

		sptrS->EnsureVisible( rc_visible );	
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::SetChromoMirror( int nLine, int nCell, int nChromo, int bMirror )
{
	METHOD_PROLOGUE_EX(CCarioView, CarioView)

	chromo_object* pobject = 0;
	pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
	if( !pobject )
		return S_FALSE;

	IChromosomePtr ptrChromo( pobject->m_ptrChromos );
	if( ptrChromo == 0 )
		return S_FALSE;

	ROTATEPARAMS rp;
	::ZeroMemory( &rp, sizeof(ROTATEPARAMS) );

	ptrChromo->GetRotateParams( &rp );

	rp.bMirrorH = BOOL(bMirror);

	rp.bManual = TRUE;

	ptrChromo->SetRotateParams( &rp );		
	ptrChromo->SetRotatedImage( 0 );
	ptrChromo->SetEqualizedRotatedImage( 0 );


	INamedDataObject2Ptr ptrNDO( ptrChromo );
	if( ptrNDO )
	{
		IUnknown* punkParent = 0;
		ptrNDO->GetParent( &punkParent );
		if( punkParent )
			punkParent->Release();
	}

//	pThis->synchronize_dimmension( SDF_RECALC_OBJECT, pobject->m_ptrMeasureObject );
	pThis->set_client_size();
	pThis->Invalidate();
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XCarioView::GetChromoMirror( int nLine, int nCell, int nChromo, int *bMirror )
{
	_try_nested(CCarioView, CarioView, GetChromoMirror)
	{	
		if( !bMirror )
			return E_INVALIDARG;

		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		IChromosomePtr ptrChromo( pobject->m_ptrChromos );
		if( ptrChromo == 0 )
			return S_FALSE;

		ROTATEPARAMS rp = {0};

		ptrChromo->GetRotateParams( &rp );

		*bMirror = rp.bMirrorH;
	}
	
	return S_OK;

	_catch_nested;
}
/////////////////////////////////////////////////////////////////////////////
//
//	IViewData interface
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XViewData::GetObject( IUnknown** ppunkObject, BSTR bstr_type )
{
	_try_nested(CCarioView, ViewData, GetObject)

		if( !ppunkObject )
			return E_POINTER;

		//[ay]
		//если у нас вьюха вообще ничего не кажет, то возвратим S_FALSE для устрашения
		//чтобы акции не пытались ничего делать
		if( pThis->m_ptrActiveObjectList == 0 )
			return S_FALSE;

		if( !strcmp( szDrawingObjectList, (char*)_bstr_t(bstr_type) ) )
		{
			*ppunkObject = pThis->m_drawobjects;
			if( pThis->m_drawobjects )
				pThis->m_drawobjects->AddRef();
		}
		

	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioView::XViewData::SetObject( IUnknown* punkObject, BSTR bstr_type )
{
	_try_nested(CCarioView, ViewData, SetObject)

	if( !punkObject )
		return S_FALSE;

	if( !strcpy( szDrawingObjectList, (char*)_bstr_t(bstr_type) ) )
	{
		INamedDataPtr ptrND( pThis->m_ptrActiveObjectList );
		if( ptrND == 0 )
			return S_FALSE;

		CString str_path = pThis->get_drawing_path();
		if( !str_path.GetLength() )
			return false;

		ptrND->SetValue( _bstr_t( (LPCSTR)str_path ), _variant_t( (IUnknown*)punkObject ) );

		pThis->m_drawobjects = punkObject;

	}

	return S_OK;

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
/************ Brightness ********************/
HRESULT CCarioView::XCarioView::GetShowChromoAllBrightness( BOOL *pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, GetShowChromoAllBrightness )		
	{																	
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowCGHGraph == true );
		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;	   
}					   

HRESULT CCarioView::XCarioView::SetShowChromoAllBrightness( BOOL pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, SetShowChromoAllBrightness )		
	{																			
		pThis->m_bShowCGHGraph = pbShow != 0;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}										             
	return S_OK;										 
	_catch_nested;										 
}														 
/*********** Ratio *********************/
HRESULT CCarioView::XCarioView::GetShowChromoAllRatio( BOOL *pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, GetShowChromoAllRatio )		
	{																			
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowCGHRatio == true );
		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;	   
}					   

HRESULT CCarioView::XCarioView::SetShowChromoAllRatio( BOOL pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, SetShowChromoAllRatio )		
	{																			
		pThis->m_bShowCGHRatio = pbShow != 0;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}


		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}										             
	return S_OK;										 
	_catch_nested;										 
}														 
/************ Amplification ********************/
HRESULT CCarioView::XCarioView::GetShowChromoAllAmplification( BOOL *pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, GetShowChromoAllAmplification )		
	{																			
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowCGHAmpl == true );
		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;	   
}					   

HRESULT CCarioView::XCarioView::SetShowChromoAllAmplification( BOOL pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, SetShowChromoAllAmplification )		
	{																			
		pThis->m_bShowCGHAmpl = pbShow != 0;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}										             
	return S_OK;										 
	_catch_nested;										 
}														 

HRESULT CCarioView::XCarioView::SetShowAllChromoObject( BOOL bShow )
{
	_try_nested(CCarioView, CarioView, SetShowAllChromoObject )		
	{																			
		pThis->m_bShowChromosome = bShow != 0;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}										             
	return S_OK;										 
	_catch_nested;										 
}

HRESULT CCarioView::XCarioView::GetShowAllChromoObject( BOOL *pbShow )
{
	_try_nested(CCarioView, CarioView, GetShowAllChromoObject )		
	{																			
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowChromosome == true );
		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;
}

HRESULT CCarioView::XCarioView::SetShowAllChromoAddLines( BOOL bShow )
{
	_try_nested(CCarioView, CarioView, SetShowAllChromoAddLines )		
	{																			
		pThis->m_bShowAdditionLines = bShow != 0;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}										             
	return S_OK;										 
	_catch_nested;										 
}

HRESULT CCarioView::XCarioView::GetShowAllChromoAddLines( BOOL *pbShow )
{
	_try_nested(CCarioView, CarioView, GetShowAllChromoAddLines )		
	{																			
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowAdditionLines == true );
		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;
}

HRESULT CCarioView::XCarioView::GetChromoOffsets( int nLine, int nCell, int nChromo, double *pfZoom,long *pnOffset  )
{
	_try_nested(CCarioView, CarioView, GetChromoOffsets )		
	{																			
		if( !pfZoom || !pnOffset )
			return S_FALSE;

		chromo_object* pobject = 0;
		pThis->get_pointers( nLine, nCell, nChromo, 0, 0, &pobject );
		if( !pobject )
			return S_FALSE;

		*pfZoom = pobject->m_fZoom;
		*pnOffset = pobject->m_nYOffset;

		return S_OK;
	}				   
	return S_FALSE;
	_catch_nested;
}


BOOL CCarioView::PreTranslateMessage(MSG* pMsg)
{

	switch( pMsg->message )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			m_ToolTipCtrl.RelayEvent( pMsg );
	};

	return __super::PreTranslateMessage( pMsg );
}

HRESULT CCarioView::XCarioView::IsEmpty()
{
	_try_nested(CCarioView, CarioView, IsEmpty )
	{																			
		for( int l = 0; l < pThis->m_arCarioLine.GetSize(); l++ )
		{
			chromo_line* pline = pThis->m_arCarioLine[l];
			for( int c = 0; c < pline->m_arCell.GetSize(); c++ )
			{
				chromo_cell* pcell = pline->m_arCell[c];
				
				if( pcell->m_arChromos.GetSize() > 0 )
					return S_FALSE;
			}
		}
	}
	return S_OK;
	_catch_nested;
}

void CCarioView::_clear_cache()
{
	for( int l = 0; l < m_arCarioLine.GetSize(); l++ )
	{
		chromo_line* pline = m_arCarioLine[l];
		for( int c = 0; c < pline->m_arCell.GetSize(); c++ )
		{
			chromo_cell* pcell = pline->m_arCell[c];
			
			for( int o = 0; o < pcell->m_arChromos.GetSize(); o++ )
			{
				pcell->m_arChromos[o]->m_ptrChromos->SetRotatedImage( 0 );
				pcell->m_arChromos[o]->m_ptrChromos->SetEqualizedRotatedImage( 0 );
			}
		}
	}
}


HRESULT CCarioView::XCarioView::SetShowAllEditableIdio( BOOL pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, SetShowAllEditableIdio )
	{	
		pThis->m_bSkipNotify = true;

		pThis->m_bShowEditableIdio =  pbShow != 0;
		for( int l = 0; l < pThis->m_arCarioLine.GetSize(); l++ )
		{
			chromo_line* pline = pThis->m_arCarioLine[l];
			for( int c = 0; c < pline->m_arCell.GetSize(); c++ )
			{
				chromo_cell* pcell = pline->m_arCell[c];
				
				for( int o = 0; o < pcell->m_arChromos.GetSize(); o++ )
				{
					if( pcell->m_arChromos[o]->m_sptrEditableIdio == 0 )
						pcell->m_arChromos[o]->m_sptrEditableIdio = pThis->_get_editable_idio( pcell->m_arChromos[o]->m_ptrMeasureObject );

					if( pcell->m_arChromos[o]->m_sptrEditableIdio == 0 )
					{
						IUnknown	*punkIdio = CreateTypedObject( _bstr_t( sz_idio_type ) );
						if( punkIdio )
						{
							IIdioBendsPtr ptrBends = punkIdio;
							punkIdio->Release();

//							ptrBends->Add( 0, pcell->m_arChromos[o]->m_nCentromereY, IDIC_WHITE, 0, IDIB_SIMPLE );
//							ptrBends->Add( pcell->m_arChromos[o]->m_nCentromereY, pcell->m_arChromos[o]->m_fLength, IDIC_WHITE, 0, IDIB_CENTROMERE );

							pcell->m_arChromos[o]->m_sptrEditableIdio = ptrBends;
							pcell->m_arChromos[o]->m_sptrEditableIdio->set_flags( IDIT_EDITABLE );

							INamedDataObject2Ptr	ptrND = ptrBends;
							ptrND->SetParent( pcell->m_arChromos[o]->m_ptrMeasureObject, 0 );
						}
					}
				}
			}
		}
		pThis->m_bSkipNotify = false;

		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
	}	
	return S_OK;										 
	_catch_nested;										 
}														 

HRESULT CCarioView::XCarioView::SetErectedState(  int nLine, int nCell, int nChromo, bool bState )
{																				
	_try_nested(CCarioView, CarioView, SetErectedState )
	{	
		if( nLine >= 0 && nLine < pThis->m_arCarioLine.GetSize() )
		{
			chromo_line* pline = pThis->m_arCarioLine[nLine];
			if( nCell >= 0 && nCell < pline->m_arCell.GetSize() )
			{
				chromo_cell* pcell = pline->m_arCell[nCell];
				
				if( nChromo >= 0 && nChromo < pcell->m_arChromos.GetSize() )
					pcell->m_arChromos[nChromo]->m_bErected = bState;
			}
		}
		
		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
		return S_OK;
	}
	return S_FALSE;
	_catch_nested;										 
}

HRESULT CCarioView::XCarioView::GetErectedState(  int nLine, int nCell, int nChromo, bool *bState )
{																				
	_try_nested(CCarioView, CarioView, GetErectedState )
	{	
		if( nLine >= 0 && nLine < pThis->m_arCarioLine.GetSize() )
		{
			chromo_line* pline = pThis->m_arCarioLine[nLine];
			if( nCell >= 0 && nCell < pline->m_arCell.GetSize() )
			{
				chromo_cell* pcell = pline->m_arCell[nCell];
				
				if( nChromo >= 0 && nChromo < pcell->m_arChromos.GetSize() )
					*bState = pcell->m_arChromos[nChromo]->m_bErected;
			}
		}
		return S_OK;
	}
	return S_FALSE;
	_catch_nested;										 
}
HRESULT CCarioView::XCarioView::SetErectedStateAll( bool bState )
{																				
	_try_nested(CCarioView, CarioView, SetErectedStateAll )
	{	
		for( int l = 0; l < pThis->m_arCarioLine.GetSize(); l++ )
		{
			chromo_line* pline = pThis->m_arCarioLine[l];
			for( int c = 0; c < pline->m_arCell.GetSize(); c++ )
			{
				chromo_cell* pcell = pline->m_arCell[c];
				
				for( int o = 0; o < pcell->m_arChromos.GetSize(); o++ )
					pcell->m_arChromos[o]->m_bErected = bState;
			}
		}
		IViewPtr sptrView = pThis->GetControllingUnknown();
		if( sptrView != 0 )
		{
			IUnknown *punkDoc = 0;

			sptrView->GetDocument( &punkDoc );
			
			if( punkDoc )
			{
				SetModifiedFlagToDoc( punkDoc );
				punkDoc->Release();
			}
		}

		pThis->synchronize_dimmension( SDF_RECALC_ALL ); 
		pThis->set_client_size();                        
		pThis->Invalidate();                             
		return S_OK;
	}
	return S_FALSE;
	_catch_nested;										 
}

HRESULT CCarioView::XCarioView::GetShowAllEditableIdio( BOOL *pbShow ) 
{																				
	_try_nested(CCarioView, CarioView, GetShowAllEditableIdio )		
	{																			
		if( !pbShow )
			return S_FALSE;

		*pbShow = ( pThis->m_bShowEditableIdio == true );
		return S_OK;

	}				   
	return S_FALSE;
	_catch_nested;
}														 

HRESULT CCarioView::XCarioView::GetIdioObject(  int nLine, int nCell, int nChromo, IdioType idt, IUnknown **punkIdio )
{
	_try_nested(CCarioView, CarioView, GetIdioObject )
	{	
		if( nLine == -1 && nCell == -1 && nChromo == -1 )
			return S_FALSE;

		if( !punkIdio ) 
			return S_FALSE;

		chromo_object *pChromo = pThis->get_chromo_object( nLine, nCell, nChromo );

		if( pChromo->m_sptrEditableIdio == 0 )
			return S_FALSE;

		if( idt == idtEditable)
			*punkIdio = IUnknownPtr( pChromo->m_sptrEditableIdio ).GetInterfacePtr();
		else if( idt == idtInserted )
			*punkIdio = IUnknownPtr( pChromo->m_ptrIdio ).GetInterfacePtr();

		if( *punkIdio )
		{
			(*punkIdio)->AddRef();
			return S_OK;
		}
	}
	return S_FALSE;
	_catch_nested; 
}

HRESULT CCarioView::XCarioView::GetShowChromoAddLinesCoord( int nLine, int nCell, int nChromo, int nInd, double *fPtX, double *fPtY, double *fPtX2, double *fPtY2 )
{
	_try_nested(CCarioView, CarioView, GetShowChromoAddLinesCoord )
	{	
		chromo_object *pObj = pThis->get_chromo_object( nLine, nCell, nChromo );

		if( pObj && pObj->m_strAddLinesFull )
		{
			*fPtX = pObj->m_strAddLinesFull[nInd].m_ptsLines.left;
			*fPtY = pObj->m_strAddLinesFull[nInd].m_ptsLines.top;
			*fPtX2 = pObj->m_strAddLinesFull[nInd].m_ptsLines.right;
			*fPtY2 = pObj->m_strAddLinesFull[nInd].m_ptsLines.bottom;
		}
		return S_OK;
	}
	return S_FALSE;
	_catch_nested; 
}


void CCarioView::OnEditCtrlChange(CString str)
{
	chromo_object *pObj = get_chromo_object( m_nActiveLine, m_nActiveCell, m_nActiveChromo );
	
	if( pObj )
	{
		pObj->m_strTextInfo = str;

		synchronize_dimmension( SDF_RECALC_ALL );
		set_client_size();
		invalidate_rect( 0 );
	}
}

void CCarioView::OnPreviewReady()
{
	_AttachObjects( m_ptrPreviewData );

}

void CCarioView::OnEnterPreviewMode()
{
	_AttachObjects( m_ptrPreviewData );
}

void CCarioView::OnLeavePreviewMode()
{
	_AttachObjects( GetControllingUnknown() );
}

void CCarioView::_AttachObjects( IUnknown *punkContextFrom )
{
	m_listVisibleObjects.clear();
	_clear_cache();

	IUnknown* punk = ::GetActiveObjectFromContext( punkContextFrom, szTypeObjectList );
	IUnknownPtr ptrOL = punk;
	if( punk )
	{
//		if( !is_correct_object_list( ptrOL ) )ptrOL = 0;
//		else 
		m_listVisibleObjects.insert_before( punk, 0 );
		punk->Release();	
		punk = 0;
	}

	if( ::GetObjectKey( ptrOL ) != ::GetObjectKey( m_ptrActiveObjectList ) )
	{
		on_deactivate_object_list( m_ptrActiveObjectList );
		
		m_ptrActiveObjectList = ptrOL;		
		on_activate_object_list( m_ptrActiveObjectList );

		Invalidate( 0 );
	}	
}

HRESULT CCarioView::XCarioVDnD::GetUndoData( BSTR *pBstr, void *pUndoData, void *pRedoData )
{
	_try_nested(CCarioView, CarioVDnD, GetUndoData )
	{	
			*pBstr = pThis->m_section.AllocSysString();

			CNDataCell  *m_pUndoState = (CNDataCell  *)pUndoData, 
							*m_pRedoState  = (CNDataCell  *)pRedoData;

			*m_pUndoState = pThis->m_undoState;
			*m_pRedoState = pThis->m_redoState;


			return S_OK;
	}
	return S_FALSE;
	_catch_nested; 
}