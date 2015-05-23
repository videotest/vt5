#include "StdAfx.h"
#include "idiodbview.h"
#include "resource.h"
#include "drag_drop_defs.h"
#include <map>
#include "carioint.h"
#include "\vt5\common2\class_utils.h"
#include "ndata_api.h"
#include "MenuRegistrator.h"


IMPLEMENT_DYNCREATE(CIdioDBView, CCmdTarget)

BEGIN_INTERFACE_MAP( CIdioDBView, CViewBase)
	INTERFACE_PART( CIdioDBView, IID_IIdioDBaseView, View ) 
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN( CIdioDBView, View );

bool SetModifiedFlagToObj( IUnknown* punkObj )
{
	INamedDataObject2Ptr ptrNDO( punkObj );

	if( ptrNDO == 0 )
		return false;

	ptrNDO->SetModifiedFlag( TRUE );

	//set modified flag to doc
	IUnknown* punkDoc = 0;
	ptrNDO->GetData( &punkDoc );
	if( punkDoc )
	{
		IFileDataObject2Ptr ptrFDO2 = punkDoc;
		punkDoc->Release();	punkDoc = 0;

		if( ptrFDO2 )
		{
			ptrFDO2->SetModifiedFlag( TRUE );
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
void CGalleryIdioItem::DoDraw( CDC *pDc, CWnd *pWnd )
{
	CRect rc = GetItemRect();

	rc = ::ConvertToWindow( pWnd->GetControllingUnknown(), rc );

	IIdiogramPtr ptrIdio = GetObject();
	if( ptrIdio )
	{
		ptrIdio->set_show_num( m_bShowNums );

		CIdioDBView *pView = (CIdioDBView *)pWnd;

		CSize sz = pView->m_size;

		double z = GetZoom( pWnd->GetControllingUnknown() );
		if( m_bShowNums )
			sz.cx = (LONG)( m_nIdioWidth * z );
		else
			sz.cx = (long)( sz.cx * z );

		sz.cy = (long)( sz.cy * z );


		double	c = 0, l = 0;
		ptrIdio->get_params( &l, &c );

		ptrIdio->set_params( l / pView->m_fMaxLen, c );
 		ptrIdio->draw( *pDc, sz, rc, 0, IDF_IGNORECENTROMERE, z );
		ptrIdio->set_params( l, c );
	}

	CString str = GetText();
	
	if( !str.IsEmpty() )
	{
		CRect rc = GetTextRect();
		rc = ::ConvertToWindow( pWnd->GetControllingUnknown(), rc );
		pDc->DrawText( str, &rc, DT_WORDBREAK | DT_VCENTER | DT_CENTER );
	}		
}
////////////////////////////////////////////////////////////////////////////
CIdioDBView::CIdioDBView()
{
	m_bNoContext = false;
	m_nIdioWidth  = ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW, KARYO_IDIOWIDTH, 30 );
	m_nIdioHeight = ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW, KARYO_IDIOHEIGHT, 100 );
	m_bShowNums = ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW, CARIO_SHOW_NUMS, 0 ) != 0;

	m_sName = szIdioDBView;
	m_sUserName.LoadString( IDS_IDIOVIEW_NAME );

	m_centr = 0.33;
	
	m_fMaxLen = 0;


	EnableAggregation();
}
CIdioDBView::~CIdioDBView()
{
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::OnFinalRelease()
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}
////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIdioDBView, CGalleryViewBase)
	//{{AFX_MSG_MAP(CIdioDBView)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
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
////////////////////////////////////////////////////////////////////////////
POSITION CIdioDBView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)1;
}
////////////////////////////////////////////////////////////////////////////
IUnknown *CIdioDBView::GetNextVisibleObject( POSITION &rPos )
{
	if( m_sptrContext == NULL )
	{
		rPos = 0;
		return 0;
	}

	if( (int )rPos == 1 )
	{
		rPos = 0;
		m_sptrContext.AddRef();
		return m_sptrContext;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
int CIdioDBView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	::RegisterMouseCtrlTarget( GetControllingUnknown() );
	
	return 0;
}


////////////////////////////////////////////////////////////////////////////
DWORD CIdioDBView::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szTypeIdioDBase) )
		return mvFull;
	else if( !strcmp( szType, sz_idio_type) )
		return mvPartial;
	else
		return mvNone;
}

////////////////////////////////////////////////////////////////////////////
void CIdioDBView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !m_hWnd )
		return;

	if(!strcmp(pszEvent, szEventNewSettings))
	{
		attach_data( true );
		InvalidateRect( 0 );
	}
	else if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{	  
		int n = *( static_cast<int *>( pdata ) );

		if( n != cncActivate )
		{
			if( n ==  cncAdd)
				attach_data( true );
			else if( n == cncRemove )
				attach_data( true, punkHit );
			else
				attach_data();

			InvalidateRect( 0 );
		}
	}													
}
////////////////////////////////////////////////////////////////////////////
IUnknown *CIdioDBView::GetObjectByPoint( CPoint point )
{
	TPOS lPosLine = m_storage.GetFirst();
	while( lPosLine )
	{
		CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

		TPOS lPosCell = pLine->GetFirst();
		while( lPosCell )
		{
			CGalleryIdioCell *pCell = pLine->Get( lPosCell );

			TPOS lPos = pCell->GetFirst();
			while( lPos )
			{
				CGalleryIdioItem *pItem = pCell->Get( lPos );
				
				CRect rc = pItem->GetRect();

//				rc = ::ConvertToWindow( GetControllingUnknown(), rc );

				if( rc.PtInRect( point ) )
				{
					IUnknown *punk  = pItem->GetObject();
					if( punk )
						punk->AddRef();

					m_pt.x = point.x - rc.left;
					m_pt.y = point.y - rc.top;

//					m_pt = ::ConvertToWindow( GetControllingUnknown(), m_pt );

					IUnknown *punkObject = m_pframe->GetActiveObject();
					
					if( punkObject && GetObjectKey( punkObject ) != GetObjectKey( punk ) )
					{
						if( _stEdit )
						{
							::SendMessage( _stEdit->GetSafeHwnd(), WM_KILLFOCUS, 0, 0 );
							_stEdit = 0;
						}					
					}

					if( punkObject )
						punkObject->Release();
					
					return punk;
				}

				lPos = pCell->GetNext( lPos );
			}
			lPosCell = pLine->GetNext( lPosCell );
		}
		lPosLine = m_storage.GetNext( lPosLine );
	}

	if( _stEdit )
	{
		::SendMessage( _stEdit->GetSafeHwnd(), WM_KILLFOCUS, 0, 0 );
		_stEdit = 0;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	IRectObjectPtr	ptrRectObject = punkObject;

	if( ptrRectObject != 0 )
	{
		CRect	rc = NORECT;
		ptrRectObject->GetRect( &rc );

		rc = ::ConvertToWindow( GetControllingUnknown(), rc );

		if( state != odsAnimation )
		{
			IIdiogramPtr ptrIdio = ptrRectObject;
			if( ptrIdio )
			{
				CSize sz = m_size;

				double z = GetZoom( GetControllingUnknown() );
				if( m_bShowNums )
					sz.cx = (LONG)( m_nIdioWidth * z );
				else
					sz.cx = long( sz.cx * z );

				sz.cy = (long)( sz.cy * z );

				double	c = 0, l = 0;
				
				ptrIdio->set_show_num( m_bShowNums );

				ptrIdio->get_params( &l, &c );
				
				ptrIdio->set_params( l / m_fMaxLen, c );

				ptrIdio->draw( dc, sz, rc, 0, IDF_IGNORECENTROMERE, z );

				ptrIdio->set_params( l, c );
			}
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
////////////////////////////////////////////////////////////////////////////
LRESULT CIdioDBView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_DND_ON_DRAG_OVER )
	{
		if( lParam != sizeof( drag_n_drop_data ) )
			return true;

		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;
		
		IUnknown *punkObject = m_pframeDrag->GetActiveObject();
		IUnknown *punkObject2 = m_pframe->GetActiveObject();

		if( punkObject == 0 || punkObject2 == 0 )
		{
			if( punkObject )
				punkObject->Release();

			if( punkObject2 )
				punkObject2->Release();

			return false;
		}

		IRectObjectPtr ptrObject = punkObject;
		punkObject->Release();

		IUnknownPtr ptrUnk;
		ptrUnk.Attach( m_pframe->GetActiveObject() );
		IRectObjectPtr ptrObject2 = ptrUnk;

		punkObject2->Release();

		CRect rc = NORECT, rc2 = NORECT;
		
		CGalleryIdioItem *pItemDrag = _get_object( ptrObject );
		CGalleryIdioItem *pItemActive = _get_object( ptrObject2 );
		
		if( !pItemDrag )
			ptrObject->GetRect( &rc );
		else
			rc = pItemDrag->GetRect();

		if( !pItemActive )
			ptrObject2->GetRect( &rc2 );
		else
			rc2 = pItemActive->GetRect();

		TPOS lPosLine = m_storage.GetFirst();
		while( lPosLine )
		{

			CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

			CRect rcLine = pLine->GetRect();

			if( rcLine.IntersectRect( rcLine, rc ) )
			{
				TPOS lPosCell = pLine->GetFirst();
				while( lPosCell )
				{
					CGalleryIdioCell *pCell = pLine->Get( lPosCell );

					CRect rcCell = pCell->GetRect();

					if( CRect().IntersectRect( rcCell, rc2 ) )
					{
						if( CRect().IntersectRect( rcCell, rc ) )
						{
							//if( !CRect().IntersectRect( rc, rc2 ) )
							{
								if( pCell->GetCount() > 1 )
								{
									TPOS lPos = pCell->GetFirst();

									while( lPos )
									{

										CGalleryIdioItem *pItem = pCell->Get( lPos );

										CRect rcItem = pItem->GetRect();

										if( CRect().IntersectRect( rc, rcItem ) )
										{
											pdata->dwDropEfect = DROPEFFECT_MOVE;
											DoDrag( pdata->point );
											return 1;
										}

										lPos = pCell->GetNext( lPos );
									}
								}
							}
						}
					}

					lPosCell = pLine->GetNext( lPosCell );
				}
			}
			lPosLine = m_storage.GetNext( lPosLine );
		}
		pdata->dwDropEfect = DROPEFFECT_NONE;
		DoDrag( pdata->point );
		return true;
	}
	else if( message == WM_DND_ON_DROP )
	{
		if( lParam != sizeof( drag_n_drop_data ) )
			return true;

		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;

		if( !pdata )
			return false;
		
		IUnknownPtr ptrUnk;
		ptrUnk.Attach( m_pframeDrag->GetActiveObject() );
		IRectObjectPtr ptrObject = ptrUnk;

		if( ptrObject == 0 )
			return false;

		CRect rcDrag;
		ptrObject->GetRect( &rcDrag );


		
		TPOS lPosLine = m_storage.GetFirst();
		long lLine = 0;
		while( lPosLine )
		{
			CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

			CRect rcLine = pLine->GetRect();

			if( rcLine.IntersectRect( rcLine, rcDrag ) )
			{
				TPOS lPosCell = pLine->GetFirst();
				long lCell = 0;
				while( lPosCell )
				{
					CGalleryIdioCell *pCell = pLine->Get( lPosCell );

					CRect rcCell = pCell->GetRect();

					if( CRect().IntersectRect( rcCell, rcDrag ) )
					{
						TPOS lPos = pCell->GetFirst();
						long lItem = 0;

						IUnknownPtr ptrUnk;
						ptrUnk.Attach( m_pframe->GetActiveObject() );
						IRectObjectPtr ptrObject2 = ptrUnk;

						if( ptrObject2 == 0 )
							return false;

						RECT rc2 = {0};
						{
							CGalleryIdioItem *pItemActive = _get_object( ptrObject2 );
							
							if( !pItemActive )
								ptrObject2->GetRect( &rc2 );
							else
								rc2 = pItemActive->GetRect();
						}
						
//						if( CRect().IntersectRect( rcDrag, &rc2 ) ) 

						CGalleryIdioItem *pIt = _get_object( ptrObject2 );

						CRect rcMain2;

						if( !pIt )
							ptrObject2->GetRect( &rcMain2 );
						else
							rcMain2 = pIt->GetRect();


						long lMain = -1;
						long lMain2 = -1;

						CGalleryIdioItem *pMain  = 0;
						CGalleryIdioItem *pMain2  = 0;

						while( lPos )
						{
							CGalleryIdioItem *pItem = pCell->Get( lPos );

							CRect rc = pItem->GetRect();
							
							if( CRect().IntersectRect( rc, rcMain2 ) )
							{
								lMain2 = lItem;
								pMain2 = pItem;
							}

							if( CRect().IntersectRect( rc, rcDrag ) )
							{
								lMain = lItem;
								pMain = pItem;
							}
							if( lMain != -1 && lMain2 != -1 )
								break;

							lPos = pCell->GetNext( lPos );
							lItem++;
						}

						if( lMain2 != -1 && lMain != -1 )
						{
							CGalleryIdioItem *pItem = pMain;

							CRect rc = pItem->GetRect();
							
							if( rc != rcMain2 && CRect().IntersectRect( rc, rcDrag ) )
							{
								if( m_sptrContext )
								{
									INamedDataPtr sptrD = m_sptrContext;

									CString str;
									str.Format( "\\Line%d\\Cell%d", lLine, lCell  );

									CNDataCell cell, cell2;
									cell.Load( sptrD, KARYO_IDIODB + str );
									cell2.Load( sptrD, KARYO_IDIODB + str );

									if( rcDrag.left < rc.left ) // <<<
									{
										if( lMain2 < lMain )
										{
											cell.Move( cell2, lMain2, lMain - 1 );
											cell.Erase( lMain2 );
										}
										else
										{
											cell.Move( cell2, lMain2, lMain );
											cell.Erase( lMain2 + 1 );
										}
									}
									else//if( rcDrag.right >= rc.right ) // >>>
									{
										if( lMain2 < lMain )
										{
											cell.Move( cell2, lMain2, lMain + 1 );
											cell.Erase( lMain2 );
										}
										else
										{
											cell.Move( cell2, lMain2, lMain );
											cell.Erase( lMain2 + 1 );
										}
									}

									cell.Store( sptrD, KARYO_IDIODB + str );
									SetModifiedFlagToObj( m_sptrContext );
									attach_data();
									m_pframeDrag->EmptyFrame();
									return 1;
								}
							}
						}
					}
					lPosCell = pLine->GetNext( lPosCell );
					lCell++;
				}
			}
			lPosLine = m_storage.GetNext( lPosLine );
			lLine++;
		}
 		//m_pframeDrag->EmptyFrame();
		return 0;
	}
	else if( message == WM_KEYDOWN )
	{
		if( wParam == VK_F2 )
		{
			IUnknownPtr sptrSelected;
			sptrSelected.Attach( m_pframe->GetActiveObject() );

			if( sptrSelected )
			{
				TPOS lPosLine = m_storage.GetFirst();
				while( lPosLine )
				{
					CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

					TPOS lPosCell = pLine->GetFirst();
					while( lPosCell )
					{
						CGalleryIdioCell *pCell = pLine->Get( lPosCell );

						TPOS lPos = pCell->GetFirst();
						while( lPos )
						{
							CGalleryIdioItem *pItem = pCell->Get( lPos );

							IUnknownPtr sptrObject = pItem->GetObject();
							if( sptrSelected == sptrObject )
							{
								CRect rcText = pItem->GetTextRect();
								rcText = ::ConvertToWindow( GetControllingUnknown(), rcText );

								CString str = pItem->GetText();

								CEditCtrl* pEdit = new CEditCtrl;

								_stEdit = pEdit;

								CRect rc = pItem->GetItemRect();
								rc = ::ConvertToWindow( GetControllingUnknown(), rc );

								if( rcText.Width() < rc.Width() )
								{
									int x = rc.Width() - rcText.Width();
									rcText.right = rcText.left + rc.Width();
									rcText.OffsetRect( -x / 2, 0 );
								}
								if( rcText.Height() < 20 )
									rcText.bottom = rcText.top + 20;
								

								pEdit->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE, rcText,  this, 6543210 );

								pEdit->SetWindowText( str );

								pEdit->SetFocus();
								pEdit->SetSel( str.GetLength(), str.GetLength() );
								
								return __super::WindowProc(message, wParam, lParam);
							}

							lPos = pCell->GetNext( lPos );
						}
						lPosCell = pLine->GetNext( lPosCell );
					}
					lPosLine = m_storage.GetNext( lPosLine );
				}
			}
		}
	}
	if( message == WM_KEYDOWN )
	{
		if( (UINT)wParam >= 'a' && (UINT)wParam <= 'z' || 
			(UINT)wParam >= 'A' && (UINT)wParam <= 'Z' ||
			(UINT)wParam >= 'а' && (UINT)wParam <= 'я' ||
			(UINT)wParam >= 'А' && (UINT)wParam <= 'Я' ||
			(UINT)wParam >= '0' && (UINT)wParam <= '9' ||
			(UINT)wParam == VK_SPACE )
		{
			m_strTextSearch += char( (UINT)wParam );

			TPOS lPosLine = m_storage.GetFirst();
			while( lPosLine )
			{
				CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

				TPOS lPosCell = pLine->GetFirst();
				while( lPosCell )
				{
					CGalleryIdioCell *pCell = pLine->Get( lPosCell );

					TPOS lPos = pCell->GetFirst();
					while( lPos )
					{
						CGalleryIdioItem *pItem = pCell->Get( lPos );
						CString str = pItem->GetText();

						if( !str.IsEmpty() && str.Find( m_strTextSearch, 0 ) != -1 )
						{
							m_pframe->EmptyFrame();
							m_pframe->SelectObject( pItem->GetObject() );
							return 1;
						}
						lPos = pCell->GetNext( lPos );
					}
					lPosCell = pLine->GetNext( lPosCell );
				}
				lPosLine = m_storage.GetNext( lPosLine );
			}
		}
		else
			m_strTextSearch.Empty();
	}

	if( message == WM_KEYDOWN && (UINT)wParam == VK_ESCAPE )
	{
		m_strTextSearch.Empty();
		m_pframe->EmptyFrame();
	}

	if( message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN )
		m_strTextSearch.Empty();

	return __super::WindowProc(message, wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::DoDraw( CDC *pDc )
{
	m_storage.DoDraw( pDc, this );
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::attach_data( bool bHasChange, IUnknown *punkObjectToRemove  )
{
	if( m_sptrContext )
	{
		m_storage.RemoveAll();

		m_storage.Load( m_sptrContext, KARYO_IDIODB );
		if( bHasChange )
		{
			INamedDataObject2Ptr ptrObject = m_sptrContext;
			
			if( ptrObject )
			{
				POSITION lPos = 0;
				ptrObject->GetFirstChildPosition( &lPos );
				while( lPos ) 
				{
					IUnknown *punkI = 0;
					ptrObject->GetNextChild( &lPos, &punkI );
					if( punkI )
					{
						bool bExit = false;
						TPOS lPosLine = m_storage.GetFirst();
						
						while( lPosLine )
						{
							CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

							TPOS lPosCell = pLine->GetFirst();
							while( lPosCell )
							{
								CGalleryIdioCell *pCell = pLine->Get( lPosCell );

								TPOS lPosO = pCell->GetFirst();
								while( lPosO )
								{
									CGalleryIdioItem *pItem = pCell->Get( lPosO );
									
									GuidKey key = pItem->GetObjectKey();
									if( ::GetObjectKey( punkI ) == key )
									{
										pItem->SetObject( punkI );
										bExit = true;
										break;
									}

									lPosO = pCell->GetNext( lPosO );
								}
								if( bExit )
									break;
								lPosCell = pLine->GetNext( lPosCell );
							}
							
							if( bExit )
								break;
							
							lPosLine = m_storage.GetNext( lPosLine );
						}
						
						if( !bExit ) // если новый
						{
							IIdiogramPtr sptrIdio = punkI;
							punkI->Release();

							if( sptrIdio )
							{
								long lClass = -1;
								sptrIdio->get_class( &lClass );

								CString strName = get_class_name( lClass );

								TPOS lPosLine = m_storage.GetFirst();
								while( lPosLine )
								{
									CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

									TPOS lPosCell = pLine->GetFirst();
									while( lPosCell )
									{
										CGalleryIdioCell *pCell = pLine->Get( lPosCell );

										if( pCell->GetText() == strName )
										{
											CGalleryIdioItem *pItem = new CGalleryIdioItem;
											pItem->SetObject( punkI );
											pItem->SetObjectKey( ::GetObjectKey( punkI ) );

											pCell->AddItem( pItem );

											bExit = true;
											break;
										}
										lPosCell = pLine->GetNext( lPosCell );
									}
									if( bExit )
										break;

									lPosLine = m_storage.GetNext( lPosLine );
								}

								if( !bExit )
								{
									CGalleryIdioCell *pCell = new CGalleryIdioCell;
									CGalleryIdioItem *pItem = new CGalleryIdioItem;
									
									pCell->SetText( strName );
									pCell->SetClassNum( lClass );
									pCell->AddItem( pItem );

									pItem->SetObject( punkI );
									pItem->SetObjectKey( ::GetObjectKey( punkI ) );

									TPOS lPosLine = m_storage.GetFirst(); long nLine = 0;
									int nLines = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "LineCount", ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "LineCount", 1 ) );

									bool bOK = false;
									while( lPosLine )
									{
										CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

										if( nLine < nLines )
										{
											CString str;
											str.Format( "%d", nLine++ );

											int nCells = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "Line" + str, GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "Line" + str, 3 ) );

											if( pLine->GetCount() < nCells )
												bOK = true;
										}
										else
											bOK = true;

										if( bOK )
										{
											pLine->AddItem( pCell );
											break;
										}

										lPosLine = m_storage.GetNext( lPosLine );
									}
									
									if( !bOK )
									{
										CGalleryIdioLine *pLine = new CGalleryIdioLine;
										pLine->AddItem( pCell );
										
										m_storage.AddItem( pLine );
									}
								}
							}
						}
						else
							punkI->Release();
					}
				}
//////////////////////////////////////////////////////////////////////////////////////////////////
				TPOS lPosLine = m_storage.GetFirst();
				while( lPosLine )
				{
					CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

					TPOS lPosCell = pLine->GetFirst();
					while( lPosCell )
					{
						CGalleryIdioCell *pCell = pLine->Get( lPosCell );

						TPOS lPos = pCell->GetFirst();
						while( lPos )
						{
							CGalleryIdioItem *pItem = pCell->Get( lPos );
							
							GuidKey key = pItem->GetObjectKey();
							IUnknown *punk = _get_object_by_key(  m_sptrContext, key );

							if( punkObjectToRemove && ::GetObjectKey( punkObjectToRemove ) == key )
							{
								punk->Release();
								lPos = pCell->Remove( lPos );
							}
							else
							{
								if( punk )
								{
									punk->Release();
									lPos = pCell->GetNext( lPos );
								}
								else
									lPos = pCell->Remove( lPos );
							}
						}
						lPosCell = pLine->GetNext( lPosCell );
					}
					lPosLine = m_storage.GetNext( lPosLine );
				}
//////////////////////////////////////////////////////////////////////////////////////////////////
			}

			{
				CGalleryIdioLine *TmpLine = new CGalleryIdioLine;

				TPOS lPosLine = m_storage.GetFirst();
				while( lPosLine )
				{
					CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

					TPOS lPosCell = pLine->GetFirst();
					while( lPosCell )
					{
						CGalleryIdioCell *pCell = pLine->Get( lPosCell );

						if( pCell->GetCount() > 0 )
							TmpLine->AddItem( pCell );

						lPosCell = pLine->Remove( lPosCell );
					}

					lPosLine = m_storage.GetNext( lPosLine );
				}

				CGalleryIdioLine *_TmpLine = new CGalleryIdioLine;

				while( TmpLine->GetCount() )
				{
					TPOS lPosCell = TmpLine->GetFirst(), lMinPos = 0;
					long lMin = 1000000;
					while( lPosCell )
					{
						CGalleryIdioCell *pCell = TmpLine->Get( lPosCell );

						if( pCell->GetClassNum() < lMin )
						{
							lMin = pCell->GetClassNum();
							lMinPos = lPosCell;
						}
						lPosCell = TmpLine->GetNext( lPosCell );
					}

					if( lMinPos != 0 )
					{
						CGalleryIdioCell *pCell = TmpLine->Get( lMinPos );
						_TmpLine->AddItem( pCell );
						TmpLine->Remove( lMinPos );
					}
				}

				TPOS lPosCell = _TmpLine->GetFirst();
				while( lPosCell )
				{
					CGalleryIdioCell *pCell = _TmpLine->Get( lPosCell );
					TmpLine->AddItem( pCell );
					lPosCell = _TmpLine->Remove( lPosCell );
				}
				
				_TmpLine->RemoveAll( false );
				delete _TmpLine;

				lPosLine = m_storage.GetFirst();
				while( lPosLine )
				{
					CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

					pLine->RemoveAll( false );

					lPosLine = m_storage.GetNext( lPosLine );
				}

				m_storage.RemoveAll();


				int nLines = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "LineCount", ::GetValueInt( GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "LineCount", 1 ) );

				for( int i = 0; i < nLines; i++ )
				{
					CString str;
					str.Format( "%d", i );

					int nCells = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "Line" + str, ::GetValueInt( GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "Line" + str, 3 ) );

					CGalleryIdioLine *pLine = new CGalleryIdioLine;
					
					int n = 0;

					TPOS lPosCell = TmpLine->GetFirst();
					while( lPosCell )
					{
						if( n >= nCells )
							break;

						CGalleryIdioCell *pCell = pLine->Get( lPosCell );

						pLine->AddItem( pCell );

						lPosCell = TmpLine->Remove( lPosCell );
						n++;
					}

					m_storage.AddItem( pLine );
				}

				CGalleryIdioLine *pLine = new CGalleryIdioLine;

				lPosCell = TmpLine->GetFirst();
				while( lPosCell )
				{
					CGalleryIdioCell *pCell = pLine->Get( lPosCell );
					pLine->AddItem( pCell );
					lPosCell = TmpLine->GetNext( lPosCell );
				}

				m_storage.AddItem( pLine );
				m_storage.Store( m_sptrContext, KARYO_IDIODB );

				TmpLine->RemoveAll( false );
				delete TmpLine;
			}

			m_storage.Store( m_sptrContext, KARYO_IDIODB );
		}
		else
		{
			if( !m_storage.GetCount() )
			{
				std::map< long, CGalleryIdioCell *> map;
				typedef std::map< long, CGalleryIdioCell *>::iterator IT;

				INamedDataObject2Ptr ptrObject = m_sptrContext;

				if( ptrObject )
				{
					POSITION lPos = 0;
					ptrObject->GetFirstChildPosition( &lPos );
					while( lPos ) 
					{
						IUnknown *punkI = 0;
						ptrObject->GetNextChild( &lPos, &punkI );
						if( punkI )
						{
							IIdiogramPtr sptrIdio = punkI;
							punkI->Release();

							if( sptrIdio )
							{
								long lClass = -1;
								sptrIdio->get_class( &lClass );

								bool bOK = false;
								for( IT it = map.begin(); it != map.end(); ++it )
								{
									if( it->first == lClass )
									{
										bOK = true;
										break;
									}
								}
								if( !bOK )
									map[lClass] = new CGalleryIdioCell;

								CGalleryIdioCell *pCell = map[lClass];
								CGalleryIdioItem *pItem = new CGalleryIdioItem;
								
								pCell->SetText( get_class_name( lClass ) );
								pCell->SetClassNum( lClass );
								pCell->AddItem( pItem );

								pItem->SetObject( punkI );
								pItem->SetObjectKey( ::GetObjectKey( punkI ) );
							}
						}
					}
				}

				if( !map.empty() )
				{
					int nLines = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "LineCount", ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "LineCount", 1 ) );

					for( int i = 0; i < nLines; i++ )
					{
						CString str;
						str.Format( "%d", i );

						int nCells = GetValueInt( m_sptrContext, KARIO_IDIODB_PREVIEW_LINES, "Line" + str, ::GetValueInt( GetAppUnknown(), KARIO_IDIODB_PREVIEW_LINES, "Line" + str, 3 ) );

						CGalleryIdioLine *pLine = new CGalleryIdioLine;
						int n = 0;
						IT it = map.begin();
						for( ; it != map.end(); ++it, n++ )
						{
							if( n >= nCells )
								break;
							pLine->AddItem( it->second );
						}
						map.erase( map.begin(), it );

						m_storage.AddItem( pLine );
					}

					CGalleryIdioLine *pLine = new CGalleryIdioLine;
					for( IT it = map.begin(); it != map.end(); ++it )
						pLine->AddItem( it->second );

					m_storage.AddItem( pLine );

					m_storage.Store( m_sptrContext, KARYO_IDIODB );
				}
			}
			else
			{
				TPOS lPosLine = m_storage.GetFirst();
				while( lPosLine )
				{
					CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

					TPOS lPosCell = pLine->GetFirst();
					while( lPosCell )
					{
						CGalleryIdioCell *pCell = pLine->Get( lPosCell );

						TPOS lPos = pCell->GetFirst();
						while( lPos )
						{
							CGalleryIdioItem *pItem = pCell->Get( lPos );
							
							GuidKey key = pItem->GetObjectKey();
							IUnknown *punk = _get_object_by_key(  m_sptrContext, key );
							if( punk )
							{
								pItem->SetObject( punk );
								punk->Release();
							}
							else
							{
								ASSERT( punk );
							}

							lPos = pCell->GetNext( lPos );
						}
						lPosCell = pLine->GetNext( lPosCell );
					}
					lPosLine = m_storage.GetNext( lPosLine );
				}
			}
		}
	}
	set_layout();
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::set_layout()
{
	CDC *pDc = GetDC();
	int x = _CX, y = _CY;

	double	length_p = 0, length_q = 0;

	double fMaxLen = 0;

	TPOS lPosLine = m_storage.GetFirst();
	while( lPosLine )
	{
		CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

		TPOS lPosCell = pLine->GetFirst();
		while( lPosCell )
		{
			CGalleryIdioCell *pCell = pLine->Get( lPosCell );

			TPOS lPos = pCell->GetFirst();
			while( lPos )
			{
				CGalleryIdioItem *pItem = pCell->Get( lPos );

				IUnknownPtr sptrObject = pItem->GetObject();
				if( sptrObject )
				{
					double	c = 0, l = 0;
					
					IIdiogramPtr sptrI = sptrObject;
					
					sptrI->get_params( &l, &c );

					fMaxLen = max( l, fMaxLen );
				}
				lPos = pCell->GetNext( lPos );
			}
			lPosCell = pLine->GetNext( lPosCell );
		}
		lPosLine = m_storage.GetNext( lPosLine );
	}	

	if( fMaxLen == 0)
		return;

	lPosLine = m_storage.GetFirst();
	bool _bShow = false;

	while( lPosLine )
	{
		CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

		TPOS lPosCell = pLine->GetFirst();
		while( lPosCell )
		{
			CGalleryIdioCell *pCell = pLine->Get( lPosCell );

			TPOS lPos = pCell->GetFirst();
			while( lPos )
			{
				CGalleryIdioItem *pItem = pCell->Get( lPos );

				IUnknownPtr sptrObject = pItem->GetObject();
				if( sptrObject )
				{
					IIdiogramPtr sptrI = sptrObject;
					double	c = 0, l = 0;
					sptrI->get_params( &l, &c );

					l /= fMaxLen;

					_bShow = pItem->GetShowNums();

					length_q = max( c * l, length_q );
					length_p = max( ( 1. - c ) * l, length_p );
				}
				lPos = pCell->GetNext( lPos );
			}
			lPosCell = pLine->GetNext( lPosCell );
		}
		lPosLine = m_storage.GetNext( lPosLine );
	}	

	m_fMaxLen = fMaxLen;

	if( _bShow )
		m_size.cx = m_nIdioWidth + 10;
	else
		m_size.cx = m_nIdioWidth;

	m_size.cy = m_nIdioHeight;

	lPosLine = m_storage.GetFirst();
	while( lPosLine )
	{
		CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

		TPOS lPosCell = pLine->GetFirst();
		while( lPosCell )
		{
			CGalleryIdioCell *pCell = pLine->Get( lPosCell );

			TPOS lPos = pCell->GetFirst();
			while( lPos )
			{
				CGalleryIdioItem *pItem = pCell->Get( lPos );
				
				CRect rc( x, y, x + m_size.cx, y + m_size.cy );

				IUnknownPtr sptrObject = pItem->GetObject();

				if( sptrObject )
				{
					IIdiogramPtr sptrIdio = sptrObject;
					if( sptrIdio )
					{
						sptrIdio->set_show_num( pItem->GetShowNums() );

						double	c = 0, l = 0;
						sptrIdio->get_params( &l, &c );
					
						sptrIdio->set_params( l / fMaxLen, c );

						sptrIdio->draw( *pDc, m_size, &rc, 0, IDF_CALCRECT | IDF_IGNORECENTROMERE, 1 );

						sptrIdio->set_params( l, c );
					}
				}

 				rc.OffsetRect( 0, m_size.cy - rc.Height() );
 
				pItem->SetItemRect( rc );

				CString str = pItem->GetText();
				
				if( !str.IsEmpty() )
				{
					CRect rc( 0, 0, 300, 0 ), rcItem = pItem->GetItemRect();
					pDc->DrawText( str, &rc, DT_WORDBREAK | DT_VCENTER | DT_CENTER | DT_CALCRECT );

					rc.OffsetRect( rcItem.left + rcItem.Width() / 2 -  rc.Width() / 2, rcItem.bottom + 5 );
					pItem->SetTextRect( rc );
				}
				else
				{
					CRect rcItem = pItem->GetItemRect();
					pItem->SetTextRect( CRect( rcItem.left, rcItem.bottom + 5, rcItem.right, rcItem.bottom + 5 ) );
				}

				rc = pItem->GetRect();

				CRect rcText = pItem->GetTextRect();
				CRect rcItem = pItem->GetItemRect();
				
				if( rc.left < _CX )
				{
					int dx = _CX - rc.left;
					rcText.OffsetRect( dx, 0 );
					rcItem.OffsetRect( dx, 0 );
					pItem->SetItemRect( rcItem );
					pItem->SetTextRect( rcText );

					rc = pItem->GetRect();
				}

				if( rc.left < x )
				{
					int dx = x - rc.left;
					rcText.OffsetRect( dx, 0 );
					rcItem.OffsetRect( dx, 0 );
					pItem->SetItemRect( rcItem );
					pItem->SetTextRect( rcText );

					rc = pItem->GetRect();
				}

				if( rc.right > 0 )
					x = rc.right + _CX;

				lPos = pCell->GetNext( lPos );
			}
			pCell->CalcRect();

			CRect rc = pCell->GetRect();

			if( rc.right > 0 )
				x = rc.right + _CX;

			lPosCell = pLine->GetNext( lPosCell );
		}

		pLine->CalcRect();
		CRect rc = pLine->GetRect();

		x = _CX;

		if( rc.bottom > 0 )
			y = rc.bottom + _CY;

		lPosLine = m_storage.GetNext( lPosLine );
	}	

	m_storage.CalcRect();
	CRect rcClient = m_storage.GetRect();

	CRect rcCl = NORECT;
	GetClientRect( &rcCl );

	CSize sz = rcClient.Size();

/*
	if( sz.cx < rcCl.Size().cx )
		sz.cx = rcCl.Size().cx;

	if( sz.cy < rcCl.Size().cy )
		sz.cy = rcCl.Size().cy;
*/

	IScrollZoomSitePtr ptrSite = GetControllingUnknown();
	ptrSite->SetClientSize( CSize( sz.cx + _CX, sz.cy + _CY ) );

	ReleaseDC( pDc );
}
////////////////////////////////////////////////////////////////////////////
void CIdioDBView::OnEditCtrlChange( CString str )
{
	IUnknownPtr sptrSelected;
	sptrSelected.Attach( m_pframe->GetActiveObject() );
	
	if( sptrSelected )
	{
		TPOS lPosLine = m_storage.GetFirst();
		while( lPosLine )
		{
			CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

			TPOS lPosCell = pLine->GetFirst();
			while( lPosCell )
			{
				CGalleryIdioCell *pCell = pLine->Get( lPosCell );

				TPOS lPos = pCell->GetFirst();
				while( lPos )
				{
					CGalleryIdioItem *pItem = pCell->Get( lPos );

					IUnknownPtr sptrObject = pItem->GetObject();
					if( sptrSelected == sptrObject )
					{
						pItem->SetText( str );

						if( m_sptrContext )
						{
							m_storage.Store( m_sptrContext, KARYO_IDIODB );
							SetModifiedFlagToObj( m_sptrContext );
							set_layout();
							InvalidateRect( 0 );
						}
						return;
					}
					lPos = pCell->GetNext( lPos );
				}
				lPosCell = pLine->GetNext( lPosCell );
			}
			lPosLine = m_storage.GetNext( lPosLine );
		}
	}
}

IUnknown *CIdioDBView::_get_object_by_key( IUnknownPtr punkDB, GuidKey key )
{
	INamedDataObject2Ptr ptrObject = punkDB;

	if( ptrObject )
	{
		POSITION lPos = 0;
		ptrObject->GetFirstChildPosition( &lPos );
		while( lPos ) 
		{
			IUnknown *punkI = 0;
			ptrObject->GetNextChild( &lPos, &punkI );
			if( punkI )
			{
				if( GetObjectKey( punkI ) == key )
					return punkI;
				punkI->Release();
			}
		}
	}
	return 0;
}

bool CIdioDBView::is_have_print_data()
{
	if( m_storage.GetCount() )
		return true;
	return false;
}

CGalleryIdioItem *CIdioDBView::_get_object( IUnknownPtr sptrO )
{
	GuidKey key = ::GetObjectKey( sptrO );

	TPOS lPosLine = m_storage.GetFirst();
	while( lPosLine )
	{
		CGalleryIdioLine *pLine = m_storage.Get( lPosLine );

		TPOS lPosCell = pLine->GetFirst();
		while( lPosCell )
		{
			CGalleryIdioCell *pCell = pLine->Get( lPosCell );

			TPOS lPos = pCell->GetFirst();
			while( lPos )
			{
				CGalleryIdioItem *pItem = pCell->Get( lPos );

				IUnknownPtr sptrObject = pItem->GetObject();
				if( ::GetObjectKey( sptrObject ) == key )
				{
					return pItem;
				}
				lPos = pCell->GetNext( lPos );
			}
			lPosCell = pLine->GetNext( lPosCell );
		}
		lPosLine = m_storage.GetNext( lPosLine );
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void CIdioDBView::clear_data()
{
	m_storage.RemoveAll();
	m_sptrContext = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CIdioDBView::XView::GetActiveObject( IUnknown **punk )
{
	METHOD_PROLOGUE_EX( CIdioDBView, View );
	if( !punk )
		return S_FALSE;

	*punk = pThis->m_pframe->GetActiveObject();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CIdioDBView::XView::EnableWindowMessages( BOOL *pbEnable )
{
	if( !pbEnable )
		return S_FALSE;

	METHOD_PROLOGUE_EX( CIdioDBView, View );
	bool b = ((CMouseImpl *)pThis)->Enable( (bool)*pbEnable );
	*pbEnable = b;	
	return S_OK;
}

HRESULT CIdioDBView::XView::EnableContextMenu( BOOL bEnable )
{
	METHOD_PROLOGUE_EX( CIdioDBView, View );
	pThis->m_bNoContext = !bEnable;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CIdioDBView::XView::SelectByText( BSTR bstr )
{
	METHOD_PROLOGUE_EX( CIdioDBView, View );

	pThis->m_strTextSearch = (char*)_bstr_t( bstr );

	IUnknownPtr sptrActive;
	sptrActive.Attach( pThis->m_pframe->GetActiveObject() );

	GuidKey key;

	if( sptrActive != 0 )
		key = GetObjectKey( sptrActive );

 	for( int i = 0; i < 2;i++ )
	{
		TPOS lPosLine = pThis->m_storage.GetFirst();
		while( lPosLine )
		{
			CGalleryIdioLine *pLine = pThis->m_storage.Get( lPosLine );

			TPOS lPosCell = pLine->GetFirst();
			while( lPosCell )
			{
				CGalleryIdioCell *pCell = pLine->Get( lPosCell );

				TPOS lPos = pCell->GetFirst();
				while( lPos )
				{
					CGalleryIdioItem *pItem = pCell->Get( lPos );

					if( sptrActive != 0 && key != pItem->GetObjectKey() )
					{
						lPos = pCell->GetNext( lPos );
						continue;
					}

					if( sptrActive != 0 && key == pItem->GetObjectKey() )
					{
						sptrActive = 0;
						lPos = pCell->GetNext( lPos );
						continue;
					}

					CString str = pItem->GetText();

					if( !str.IsEmpty() && str.Find( pThis->m_strTextSearch, 0 ) != -1 )
					{
						pThis->m_pframe->EmptyFrame();
						pThis->m_pframe->SelectObject( pItem->GetObject() );
						return S_OK;
					}
					lPos = pCell->GetNext( lPos );
				}
				lPosCell = pLine->GetNext( lPosCell );
			}
			lPosLine = pThis->m_storage.GetNext( lPosLine );
		}
		if( sptrActive != 0 )
			sptrActive = 0;
	}
	return S_FALSE;
}

const char* CIdioDBView::GetViewMenuName()
{
	return szIdioDBView;
}

void CIdioDBView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( m_bNoContext )
		return;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;

	CPoint pt = point;
	ScreenToClient( &pt );


	pt = ::ConvertToClient( GetControllingUnknown(), pt );

	CMenuRegistrator rcm;	

	CString strMenuName;

	IUnknownPtr ptrObject;


	// Context menu for selection, drawing objects, etc...
	{

		IUnknown* punkObject = NULL;
		punkObject = GetObjectByPoint( pt );

		if( punkObject )
		{
			ptrObject = punkObject;
			punkObject->Release();
			punkObject = NULL;		

			CString strObjectType = ::GetObjectKind( ptrObject );
			
			CString strObjectMenuName = (LPCSTR)rcm.GetMenu( szIdioDBViewMenu, strObjectType );

			if( !strObjectMenuName.IsEmpty() )
			{
				strMenuName = strObjectMenuName;
			}		
		}
	}

	if( strMenuName.IsEmpty() )
	{
		CString strViewName = GetViewMenuName();
		strMenuName = (LPCSTR)rcm.GetMenu( strViewName, 0 );
	}		

	if( strMenuName.IsEmpty() )
		return;
	
	_bstr_t	bstrMenuName = strMenuName;
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
	
		
}