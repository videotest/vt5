#include "StdAfx.h"
#include "view.h"
#include "resource.h"
#include "idio_int.h"
#include <vector>
#include "\vt5\common2\class_utils.h"
#include "drag_drop_defs.h"
/*
IMPLEMENT_DYNCREATE(CIdioDBView, CCmdTarget)

const int _BORDER = 10;
const int _CX = 2 * _BORDER + 15;
const int _CY = 2 * _BORDER + 30;

BEGIN_INTERFACE_MAP(CIdioDBView, CViewBase)
END_INTERFACE_MAP()

CIdioDBView::CIdioDBView()
{
	m_sUserName.LoadString( IDS_IDIOVIEW_NAME );
	m_bDrag = false;
}

CIdioDBView::~CIdioDBView()
{
}

void CIdioDBView::OnFinalRelease()
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}

BEGIN_MESSAGE_MAP(CIdioDBView, CGalleryViewBase)
	//{{AFX_MSG_MAP(CIdioDBView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CIdioDBView, CGalleryViewBase)
	//{{AFX_DISPATCH_MAP(CIdioDBView)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// {0215C42D-33AC-4c63-8F17-11A52AF41858}
static const IID IID_IIdioDBViewDisp =
{ 0x215c42d, 0x33ac, 0x4c63, { 0x8f, 0x17, 0x11, 0xa5, 0x2a, 0xf4, 0x18, 0x58 } };

//IMPLEMENT_UNKNOWN(CIdioDBView, ImView)

// {88A29390-C51C-4fa2-9544-76F01BED5340}
GUARD_IMPLEMENT_OLECREATE(CIdioDBView, szIdioDBViewProgID, 
0x88a29390, 0xc51c, 0x4fa2, 0x95, 0x44, 0x76, 0xf0, 0x1b, 0xed, 0x53, 0x40);

POSITION CIdioDBView::GetFisrtVisibleObjectPosition()
{
	return 0;
}

IUnknown *CIdioDBView::GetNextVisibleObject( POSITION &rPos )
{
	return 0;
}


int CIdioDBView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	::RegisterMouseCtrlTarget( GetControllingUnknown() );
	attach_data();

	return 0;
}

DWORD CIdioDBView::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szTypeIdioDBase) )
		return mvFull;
	else
		return mvNone;
}

void CIdioDBView::OnActivateObject( IUnknown *punkDataObject )
{
}

void CIdioDBView::set_layout()
{
/*
	int x = _CX, y = _CY;
	RECT rcClient = {0};
	
	int cx = 10;
	
	std::vector<long> arr;

    for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
	{
		GallerySpace::CGalleryItemBase *pObj = m_pDataArray.get( lPos );

		CGalleryIdioItem *pIdioItem = static_cast<CGalleryIdioItem *>(pObj);

		IIdiogramPtr ptrIdio = pIdioItem->GetObject();

		long lClass = -1;
		ptrIdio->get_class( &lClass );

		pIdioItem->SetClass( lClass );

		bool bOK = true;
		for( int i = 0; i < (int)arr.size(); i++ )
		{
			if( *(arr.begin() + i ) == lClass )
			{
				bOK = false;
				break;
			}
		}
		if( bOK )
			arr.push_back( lClass );
	}

	typedef std::vector<long>::iterator IT;

	if( !m_strCellText.count() )
	{
		for( IT it = arr.begin(); it != arr.end(); ++it )
		{
			CString str_name = get_class_name( *it );

			m_strCellText.add_tail( str_name );
		}
	}

	CDC *pDc = GetDC();
	for( long lPos = m_strCellText.head(); lPos; lPos = m_strCellText.next( lPos ) )
	{
		CRect rcText( 0, 0, 300, 0 );

		CString str = m_strCellText.get( lPos );

		pDc->DrawText( str, rcText, DT_WORDBREAK | DT_VCENTER | DT_CENTER | DT_CALCRECT );
		m_rcCellTextArr.add_tail( rcText );
	}
	ReleaseDC( pDc );

	text_layout();

	long lPos2 = 0;
	long lPos3 = m_rcCellTextArr.head();
	CRect rcPrev = NORECT; 
	for( IT it = arr.begin(); it != arr.end(); ++it, lPos3 = m_rcCellTextArr.next( lPos3 ) )
	{
		for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
		{
			GallerySpace::CGalleryItemBase *pObj = m_pDataArray.get( lPos );

			CGalleryIdioItem *pIdioItem = static_cast<CGalleryIdioItem *>(pObj);

			if( pIdioItem->GetClass() == *it )
			{
				pObj->SetOffset( x, y );

				SIZE sz = pObj->GetSize();

				CRect rcItemText = pObj->GetTextRect();

				if( rcItemText.Width() > sz.cx )
					sz.cx = rcItemText.Width();

				{
					CRect rc = pObj->GetRect();

					if( rc.left - _BORDER < _CX )
					{
						x += -rc.left + _CX;
						pObj->SetOffset( x, y );
					}
				}
				{
					CRect rc = pObj->GetRect();
					if( rc.left - 2 * _BORDER < rcPrev.right + 2 * _BORDER  )
					{
						x += rcPrev.right - rc.left + 4 * _BORDER;
						pObj->SetOffset( x, y );
					}
				}


				if( m_rcCellArr.count() == int( it - arr.begin() ) )
				{
					CRect rc = pObj->GetRect();
					m_rcCellArr.add_tail( CRect( rc.left - _BORDER, rc.top - _BORDER, rc.left + sz.cx + _BORDER, rc.top + sz.cy + rcItemText.Height() + _BORDER ) );
					lPos2 = m_rcCellArr.tail();
				}
				else
				{
					CRect rc = m_rcCellArr.get( lPos2 );
					CRect rc2 = pObj->GetRect();
					m_rcCellArr.set( CRect( rc.left, rc.top, rc2.left + sz.cx + _BORDER, rc2.top + sz.cy + rcItemText.Height() + _BORDER ), lPos2 );
				}

				CRect rc = m_rcCellArr.get( lPos2 );
				rcPrev = pObj->GetRect();

				IRectObjectPtr ptrRect = pIdioItem->GetObject();
				if( ptrRect != 0)
					ptrRect->SetRect( pObj->GetItemRect() );

				rcClient.right = max( rc.right, rcClient.right );
				rcClient.bottom = max( rc.bottom, rcClient.bottom );

				x = rcPrev.right + _CX;
			}
		}
		if( int( it - arr.begin() + 1 ) > cx )
		{
			x = _CX;
			y = rcClient.bottom + _CY;
			rcPrev = NORECT;
		}
	}
	for( long lPos = m_rcCellTextArr.head(), lPos2 = m_rcCellArr.head(); lPos; lPos = m_rcCellTextArr.next( lPos ), lPos2 = m_rcCellArr.next( lPos2 ) )
	{
		CRect rc = m_rcCellTextArr.get( lPos );
		CRect rcItem = m_rcCellArr.get( lPos2 );
		rc.OffsetRect( rcItem.left + _BORDER, rcItem.bottom + _BORDER );

		m_rcCellTextArr.set( rc, lPos );
	}

	IScrollZoomSitePtr ptrSite = GetControllingUnknown();
	ptrSite->SetClientSize( CSize( rcClient.right + _CX, rcClient.bottom + _CY ) );

}

void CIdioDBView::attach_data()
{
/*
	IUnknown *punkDB = GetActiveObjectFromContext( GetControllingUnknown(), szTypeIdioDBase );
	clear_data();

	if( punkDB )
	{
		INamedDataObject2Ptr ptrObject = punkDB;
		punkDB->Release();

		if( ptrObject )
		{
			long lPos = 0;
			ptrObject->GetFirstChildPosition( &lPos );
			while( lPos ) 
			{
				IUnknown *punkI = 0;
				ptrObject->GetNextChild( &lPos, &punkI );
				if( punkI )
				{

					m_pDataArray.add_tail( new CGalleryIdioItem( punkI ) );

					_bstr_t bstr = "Hello";

					m_pDataArray.get( m_pDataArray.tail() )->SetText( (char *)bstr );

					punkI->Release();
				}
			}
		}
	}
	set_layout();

}

void CIdioDBView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventActiveObjectChange ) )
		attach_data();
}

void CIdioDBView::DrawCell( CDC *pDc, const CRect &rc )
{
	CPoint pt;

	int br = 10;
	pt = pDc->MoveTo( rc.left, rc.bottom - br );
	pDc->LineTo( rc.left, rc.bottom );
	pDc->LineTo( rc.right, rc.bottom );
	pDc->LineTo( rc.right, rc.bottom - br );

	pDc->MoveTo( pt );
}

void CIdioDBView::OnEditCtrlChange( CString str )
{
/*
	for( long lPos = m_pDataArray.head(), lCount = 0; lPos; lPos = m_pDataArray.next( lPos ), lCount++ )
	{
		if( m_nActiveItem == lCount )
		{
			GallerySpace::CGalleryItemBase *pObj = m_pDataArray.get( lPos );

			pObj->SetText( str );

			IIdioDBasePtr ptrDB = m_sptrDB;
			ptrDB->SetItemText( m_nActiveItem, _bstr_t( str ) );
			attach_data();
			InvalidateRect( 0 );
			break;
		}
	}

}

void CIdioDBView::text_layout(void)
{
/*
	CDC *pDc = GetDC();
	for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos )  )
	{
		GallerySpace::CGalleryItemBase *pObj = m_pDataArray.get( lPos );

		CString str = pObj->GetText();
		
		if( !str.IsEmpty() )
		{
			CRect rc( 0, 0, 300, 0 ), rcItem = pObj->GetItemRect();
			pDc->DrawText( str, &rc, DT_WORDBREAK | DT_VCENTER | DT_CENTER | DT_CALCRECT );

			rc.OffsetRect( rcItem.left + rcItem.Width() / 2 -  rc.Width() / 2, rcItem.bottom + 5 );
			pObj->SetTextRect( rc );
		}
		else
		{
			CRect rcItem = pObj->GetItemRect();
			pObj->SetTextRect( CRect( rcItem.left, rcItem.bottom + 5, rcItem.right, rcItem.bottom + 5 ) );
		}
	}
	ReleaseDC( pDc );

}
////////////////////////////////////////////////////////////////////////////
IUnknown *CIdioDBView::GetObjectByPoint( CPoint point )
{
	for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
	{
		GallerySpace::CGalleryItemBase *pObj = m_pDataArray.get( lPos );
		CGalleryIdioItem *pIdioItem = static_cast<CGalleryIdioItem *>(pObj);

		CRect rcObject = pObj->GetRect();

		rcObject = convert_to_wnd( rcObject );

		if( rcObject.PtInRect( point ) )
		{
			IUnknown *punk = pIdioItem->GetObject();
			
			punk->AddRef();
			return punk;
		}
	}
	return 0;
}
void CIdioDBView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	IRectObjectPtr	ptrRectObject = punkObject;

	if( ptrRectObject != 0 )
	{
		CRect	rc = NORECT;
		ptrRectObject->GetRect( &rc );

		rc = convert_to_wnd( rc );

		if( state != odsAnimation )
		{
			IIdiogramPtr ptrIdio = ptrRectObject;
			if( ptrIdio )
				ptrIdio->draw( dc, rc.Size(), rc, 0, IDF_IGNORECENTROMERE|IDF_IGNORELENGTH );
		}

		CPen	pen;

		if( state == odsSelected )
			pen.CreatePen( PS_SOLID, 0, RGB( 0, 0, 0 )  );
		
		else if( state == odsActive )
			pen.CreatePen( PS_DOT, 0, RGB( 0, 0, 0 )  );
		else if( state == odsAnimation )
			pen.CreatePen( PS_SOLID, 0, RGB( 255, 255, 255 )  );
		else
			return;

		dc.SetBkMode(OPAQUE);
		dc.SelectObject( &pen );
		dc.SelectStockObject( NULL_BRUSH );
		dc.SetBkColor( m_clBack );

		if( state == odsActive || state == odsAnimation )
			dc.Rectangle( &rc );
		else if( state == odsSelected )
			dc.DrawFocusRect( &rc );

		dc.SelectStockObject( BLACK_PEN );
		dc.SelectStockObject( BLACK_BRUSH );
	}
}

LRESULT CIdioDBView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_DND_ON_DRAG_OVER )
	{
		if( lParam != sizeof( drag_n_drop_data ) )
			return true;

		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;
		
		IRectObjectPtr ptrObject = m_pframeDrag->GetActiveObject();

		CRect rc;
		ptrObject->GetRect( &rc );
		rc = convert_to_wnd( rc );

		for( long lPos = m_rcCellArr.head(), lID = 0; lPos; lPos = m_rcCellArr.next( lPos ), lID++ )
		{
			CRect rcCell = m_rcCellArr.get( lPos );
			rcCell = convert_to_wnd( rcCell );

			if( CRect().IntersectRect( rc, rcCell ) )
			{
				pdata->dwDropEfect = DROPEFFECT_MOVE;
				DoDrag( pdata->point );
				return 1;
			}
		}


		pdata->dwDropEfect = DROPEFFECT_NONE;
		DoDrag( pdata->point );
		return true;
	}
	else if( message == WM_DND_ON_DROP )
	{
	}
	return __super::WindowProc(message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////
void CGalleryIdioItem::DoDraw( CDC *pDc, bool bSelected, CWnd *pWnd  )
{
	CRect rc = GetItemRect();

	GallerySpace::CGalleryViewBase *pView = static_cast<GallerySpace::CGalleryViewBase *>( pWnd );

	rc = pView->convert_to_wnd( rc );

	IIdiogramPtr ptrIdio = m_sptrIdio;
	if( ptrIdio )
		ptrIdio->draw( *pDc, rc.Size(), rc, 0, IDF_IGNORECENTROMERE|IDF_IGNORELENGTH );

	CString str = GetText();
	
	if( !str.IsEmpty() )
	{
		CRect rc = GetTextRect();
		rc = pView->convert_to_wnd( rc );
		pDc->DrawText( str, &rc, DT_WORDBREAK | DT_VCENTER | DT_CENTER );
	}
}
*/