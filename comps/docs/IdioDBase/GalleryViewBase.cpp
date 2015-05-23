#include "StdAfx.h"
#include "galleryviewbase.h"
#include "EditCtrl.h"
/*
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

	CGalleryViewBase::CGalleryViewBase()
	{
		EnableAggregation();
		m_clBack = ::GetSysColor( COLOR_WINDOW );
		m_nActiveItem = -1;
		m_fZoom = 1;
		m_ptScroll = CPoint( 0, 0 );
	}

	CGalleryViewBase::~CGalleryViewBase()
	{
		clear_data();
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

	void CGalleryViewBase::clear_data()
	{
		for( long lPos = m_pDataArray.head(); lPos != m_pDataArray.tail(); lPos = m_pDataArray.next( lPos ) )
		{
			CGalleryItemBase *pObj = m_pDataArray.get( lPos );
			pObj->Destroy();
		}
		m_pDataArray.clear();
		m_rcCellArr.clear();
		m_strCellText.clear();
		m_rcCellTextArr.clear();
	}

	void CGalleryViewBase::DoDraw( CDC *pDc )
	{
		_update_scroll_zoom();
		long lMainPos = 0;
		for( long lPos = m_pDataArray.head(), nCount = 0; lPos; lPos = m_pDataArray.next( lPos ), nCount++ )
		{

			CGalleryItemBase *pObj = m_pDataArray.get( lPos );

			if( m_nActiveItem == nCount )
				lMainPos = lPos;
			else
				pObj->DoDraw( pDc, false, this );
		}

		for( long lPos = m_rcCellArr.head(); lPos; lPos = m_rcCellArr.next( lPos ) )
		{
			CRect rc = m_rcCellArr.get( lPos );
			rc = convert_to_wnd( rc );
			DrawCell( pDc, rc );
		}

		for( long lPos = m_rcCellTextArr.head(), lPos2 = m_strCellText.head(); lPos; lPos = m_rcCellArr.next( lPos ), lPos2 = m_strCellText.next( lPos2 ) )
		{
			CRect rc = m_rcCellTextArr.get( lPos );
			CString str = m_strCellText.get( lPos2 );
			rc = convert_to_wnd( rc );
			pDc->DrawText( str, rc, DT_CENTER|DT_VCENTER|DT_WORDBREAK );
		}
		if( lMainPos )
		{

			CGalleryItemBase *pObj = m_pDataArray.get( lMainPos );
			pObj->DoDraw( pDc, true , this );
		}

	}
	LRESULT CGalleryViewBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( message == WM_KEYDOWN )
		{
			if( wParam == VK_F2 )
			{
				for( long lPos = m_pDataArray.head(), lCount = 0; lPos; lPos = m_pDataArray.next( lPos ), lCount++ )
				{
					if( m_nActiveItem == lCount )
					{
						CGalleryItemBase *pObj = m_pDataArray.get( lPos );
						
						CRect rcText = pObj->GetTextRect();
						rcText = convert_to_wnd( rcText );

						CString str = pObj->GetText();

						CEditCtrl* pEdit = new CEditCtrl;

						CRect rc = pObj->GetItemRect();
						rc = convert_to_wnd( rc );

						if( rcText.Width() < rc.Width() )
						{
							int x = rc.Width() - rcText.Width();
							rcText.right = rcText.left + rc.Width();
							rcText.OffsetRect( -x / 2, 0 );
						}
						if( rcText.Height() < 20 )
						{
							rcText.bottom = rcText.top + 20;
						}
						
						pEdit->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE, rcText,  this, 6543210 );

						pEdit->SetWindowText( str );

						pEdit->SetFocus();
						pEdit->SetSel( str.GetLength(), str.GetLength() );
					}
				}
			}
		}
		return __super::WindowProc(message, wParam, lParam);
	}
	void CGalleryViewBase::_update_scroll_zoom()
	{
		IScrollZoomSitePtr ptrSite = GetControllingUnknown();

		ptrSite->GetZoom( &m_fZoom );
		ptrSite->GetScrollPos( &m_ptScroll );
	}
	CPoint CGalleryViewBase::convert_to_wnd( CPoint pt )
	{ return CPoint( int( pt.x * m_fZoom - m_ptScroll.x ), int( pt.y * m_fZoom - m_ptScroll.y ) ); }
	CRect CGalleryViewBase::convert_to_wnd( CRect rc )
	{ return CRect( int ( rc.left * m_fZoom - m_ptScroll.x ), int( rc.top * m_fZoom - m_ptScroll.y ), int( rc.right * m_fZoom - m_ptScroll.x ), int( rc.bottom * m_fZoom - m_ptScroll.y ) );	}

	CPoint CGalleryViewBase::convert_to_view( CPoint pt )
	{ return CPoint( int( ( pt.x + m_ptScroll.x ) / m_fZoom ), int( ( pt.y + m_ptScroll.y ) / m_fZoom ) ); }
	CRect CGalleryViewBase::convert_to_view( CRect rc )
	{ return CRect( int ( ( rc.left + m_ptScroll.x ) / m_fZoom ), int( ( rc.top + m_ptScroll.y ) / m_fZoom ), int( rc.right * m_fZoom - m_ptScroll.x ), int( ( rc.bottom + m_ptScroll.y ) / m_fZoom ) ); }

	bool	CGalleryViewBase::HitTest( CPoint point, INamedDataObject2 *pNamedObject )
	{
		IRectObjectPtr	ptrObject = pNamedObject;
		if( ptrObject == 0 )
			return 0;

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
			CRect	rect = NORECT;
			ptrRectObject->GetRect( &rect );

	//		ptOffset -= rect.TopLeft();
			ptrRectObject->Move( ptOffset );
		}
	}

	bool CGalleryViewBase::PtInObject( IUnknown *punkObject, CPoint point )
	{
		INamedDataObject2Ptr sptr = punkObject;
		return HitTest( point, sptr );
	}

}
*/