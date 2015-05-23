#include "StdAfx.h"
#include "mics_class.h"

namespace GallerySpace
{
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

	class DrawDib
	{
	public:
		DrawDib()
		{		m_hDrawDib = ::DrawDibOpen();	}
		~DrawDib()
		{		::DrawDibClose( m_hDrawDib );	}
		operator HDRAWDIB()
		{	return m_hDrawDib;}
	protected:
		HDRAWDIB	m_hDrawDib;
	};
	DrawDib	DrawDibInstance;

	CEditCtrl* CGalleryViewBase::_stEdit = 0;

	CGalleryViewBase::CGalleryViewBase()
	{
		EnableAggregation();
		m_clBack = ::GetSysColor( COLOR_WINDOW );
	}
	 
	CGalleryViewBase::~CGalleryViewBase()
	{
	}

	BEGIN_MESSAGE_MAP(CGalleryViewBase, CViewBase)
		//{{AFX_MSG_MAP(CGalleryViewBase)
		ON_WM_PAINT()
		ON_WM_ERASEBKGND()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	BEGIN_INTERFACE_MAP(CGalleryViewBase, CViewBase)
	END_INTERFACE_MAP()

	 
	BOOL CGalleryViewBase::OnEraseBkgnd(CDC* pDC) 
	{
		return TRUE;
	}

	void CGalleryViewBase::OnPaint() 
	{
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

		{
			//fill backgrund
			CRect rcFill = rcPaint;
			
			dcMemory.FillRect( &rcFill, &CBrush( m_clBack ) );
			
			//process drawing
			DoDraw( &dcMemory );

			m_pframe->Draw(dcMemory, rcPaint, &bmih, pdibBits);
		
			dcMemory.SelectStockObject( WHITE_PEN );
			m_pframeDrag->Draw(dcMemory, rcPaint, &bmih, pdibBits);
		}


		dcMemory.SetViewportOrg( 0, 0 );

		//paint to paint DC
		::SetDIBitsToDevice( dcPaint, rcPaint.left, rcPaint.top, nImageWidth, nImageHeight,
			0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

		//prepare to delete objects
		dcMemory.SelectObject( poldBmp );

		if( psrcBitmap )
			psrcBitmap->DeleteObject();

		dcMemory.SelectObject( poldBmp );

		dcPaint.SetROP2( R2_COPYPEN );
		dcPaint.SelectStockObject( BLACK_PEN );

	}
	 
	bool	CGalleryViewBase::HitTest( CPoint point, INamedDataObject2 *pNamedObject )
	{
		IRectObjectPtr	ptrObject = pNamedObject;
		if( ptrObject == 0 )
			return 0;


		CRect rc; 
		ptrObject->GetRect( &rc );
//		rc = ::ConvertToWindow( GetControllingUnknown(), rc );

		m_pt.x = point.x - rc.left;
		m_pt.y = point.y - rc.top;

		long	lHitTest = 0;
		ptrObject->HitTest( point, &lHitTest );
		return lHitTest != 0;
	}

	CRect CGalleryViewBase::GetObjectRect( IUnknown *punkObj )
	{
		IRectObjectPtr	ptrRectObject = punkObj;
		CRect rect = NORECT;
		if( ptrRectObject ) 
			ptrRectObject->GetRect( &rect );

		return rect;
	}

	void CGalleryViewBase::SetObjectOffset( IUnknown *punkObj, CPoint ptOffset )
	{
		IRectObjectPtr	ptrRectObject = punkObj;

		if( ptrRectObject != 0 )
		{
			double fZoom = ::GetZoom( GetControllingUnknown() );
			POINT psScroll = ::GetScrollPos( GetControllingUnknown() );

			ptOffset.x = long( ( ptOffset.x + psScroll.x ) / fZoom );
			ptOffset.y = long( ( ptOffset.y + psScroll.y ) / fZoom );

			ptOffset.x -= m_pt.x;
			ptOffset.y -= m_pt.y;

			ptrRectObject->Move( ptOffset );
		}
	}

	bool CGalleryViewBase::PtInObject( IUnknown *punkObject, CPoint point )
	{
		INamedDataObject2Ptr sptr = punkObject;
		return HitTest( point, sptr );
	}
/////////////////////////////////////////////////////////////////////////////
	// CEditCtrl
	BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
		//{{AFX_MSG_MAP(CEditCtrl)
		ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	int CEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nResult = CEdit::OnCreate( lpCreateStruct );		
		SetFont( CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) ) );

		m_x = lpCreateStruct->x;
		m_y = lpCreateStruct->y;

		return nResult;
		
	}

	void CEditCtrl::OnKillfocus()
	{
		if( !m_bEscapeKey )
			SendMessage( WM_KEYDOWN, VK_RETURN, 0);
	}


	void CEditCtrl::PostNcDestroy()
	{
		CGalleryViewBase::_stEdit = 0;
		delete this;	
	}

	void CEditCtrl::SetCtrlFont( CFont* pFont )
	{
		LOGFONT lf;

		if( pFont )
		{
			pFont->GetLogFont( &lf );
			m_Font.CreateFontIndirect( &lf );
			SetFont( &m_Font );
		}
	}



	LRESULT CEditCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
 		if( message == WM_KEYUP )
			if( wParam == VK_CONTROL )
				m_bCtrlPressed = false;

 		if( message == WM_KEYDOWN )
		{		
			if( wParam == VK_CONTROL )
				m_bCtrlPressed = true;
			if( wParam == VK_RETURN )
			{
 				if( !m_bCtrlPressed )
				{
					CWnd* pParent = GetParent();
					GallerySpace::CGalleryViewBase* pView = static_cast<GallerySpace::CGalleryViewBase *>( pParent );
					if( pView )
					{
						CString str;
						GetWindowText( str );
						pView->OnEditCtrlChange( str );
					}		

					if( !m_bEscapeKey )
						SendMessage(WM_CLOSE, 0, 0);
					return 0;
				}
			}

			if( wParam == VK_ESCAPE )
			{
				PostMessage(WM_CLOSE, 0, 0);
				return m_bEscapeKey = TRUE;
			}		
		}

		
		return CEdit::DefWindowProc( message,  wParam, lParam);

	}
	LRESULT CEditCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if( message == UINT( WM_USER + 1234 ) )
		{
			m_bEscapeKey = true;
			SendMessage( WM_KEYDOWN, VK_RETURN, 0);
			PostMessage(WM_CLOSE, 0, 0);
		}

		return CEdit::WindowProc(message, wParam, lParam);
	}
/////////////////////////////////////////////////////////////////////////////
	bool CGalleryViewBase::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
	{
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

	bool CGalleryViewBase::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
	{
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

	void CGalleryViewBase::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
	{
		if( !is_have_print_data() )
			return;

		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		CSize client_size;
		ptrZ->GetClientSize( &client_size );

		double freal_zoom = 1.0;
		{
			IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
			if( ptrZ != 0 )
			{
				ptrZ->GetZoom( &freal_zoom );
				ptrZ->SetZoom( 1 );
			}
		}

		CDC* pdc = CDC::FromHandle(hdc);

		CRect rcPaint = CRect(	int(nUserPosX / freal_zoom), int(nUserPosY / freal_zoom), 
								int(( nUserPosX + nUserPosDX ) / freal_zoom), 
								int((nUserPosY + nUserPosDY) / freal_zoom) );

		int nOldMM = pdc->GetMapMode();
		CSize sizeOldVE = pdc->GetViewportExt();
		CSize sizeOldWE = pdc->GetWindowExt();
		CPoint ptOldVO = pdc->GetViewportOrg();
		
		CSize sizeWindowExt = CSize( rcPaint.Width() , rcPaint.Height() );	
		CSize sizeViewPortExt = CSize( rectTarget.Width(), rectTarget.Height() );	
		CPoint ptViewPortOrg;
		
		double fZoomX = (double)rectTarget.Width()  / (double)( rcPaint.Width() );
		double fZoomY = (double)rectTarget.Height() / (double)( rcPaint.Height() );

		ptViewPortOrg.x = long(rectTarget.left - rcPaint.left * fZoomX);
		ptViewPortOrg.y = long(rectTarget.top  - rcPaint.top * fZoomY);

		CRgn rgnClip;

		rgnClip.CreateRectRgnIndirect( &rectTarget );
		
		pdc->SelectClipRgn( &rgnClip );

		pdc->SetMapMode( MM_ANISOTROPIC );

		pdc->SetWindowExt( sizeWindowExt );					
		pdc->SetViewportExt( sizeViewPortExt );
		pdc->SetViewportOrg( ptViewPortOrg );

		pdc->FillRect( &rcPaint, &CBrush( m_clBack ) );
		DoDraw( pdc );

		pdc->SetMapMode( nOldMM );
		pdc->SetViewportExt( sizeOldVE );
		pdc->SetWindowExt( sizeOldWE );
		pdc->SetViewportOrg( ptOldVO );

		pdc->SelectClipRgn( 0 );

		{
			IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
			if( ptrZ != 0 )
			{
				ptrZ->SetZoom( freal_zoom );
			}
		}

	}
	void CGalleryViewBase::AttachActiveObjects()
	{
		m_pframe->EmptyFrame();
		_AttachObjects( GetControllingUnknown() );
		IViewCtrlPtr ptr = GetControllingUnknown();

		if( GetSafeHwnd() )
			Invalidate( TRUE );
	}

	void CGalleryViewBase::_AttachObjects( IUnknown *punkContextFrom )
	{
		IUnknown* punkObject	= ::GetActiveObjectFromContext( punkContextFrom, szTypeIdioDBase );
		IUnknownPtr ptr_new = punkObject;
		if( punkObject )
			punkObject->Release();	punkObject = 0;

		if( ptr_new )
		{
			bool bChangeList = ( ::GetObjectKey( ptr_new ) != ::GetObjectKey( m_sptrContext ) );
			if( bChangeList )
			{
				m_sptrContext = ptr_new;
				attach_data( true );
			}
		}
		else
			clear_data();
	}
}
