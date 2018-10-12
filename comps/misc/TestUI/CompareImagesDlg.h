#pragma once
#include "stdafx.h"
#include "\vt5\awin\win_dlg.h"
#include "\vt5\awin\com_unknown.h"
#include "resource.h"

#include "test_man.h"

#include <math.h>


// CCompareImagesDlg dialog

class CVirtualDocument :
	public IDocumentSite,
	public IView,
	public com_unknown
{
	IUnknownPtr m_sptrDoc;
	IUnknownPtr m_sptrContext;
public:
	implement_unknown();
	virtual IUnknown *get_interface( const IID &iid )
	{
		IUnknown *punk = 0;

		if( iid == IID_IDocumentSite )
			return (IDocumentSite *)this;

		if( iid == IID_IView )
			return (IView *)this;
		
		if( m_sptrDoc != 0 && m_sptrDoc->QueryInterface( iid, (void**)&punk ) == S_OK )
		{
			punk->Release();
			return punk;
		}

		if( m_sptrContext != 0 && m_sptrContext->QueryInterface( iid, (void**)&punk ) == S_OK )
		{
			punk->Release();
			return punk;
		}
		
		return __super::get_interface( iid );
	}

	void SetData( IUnknown *punk ) {  m_sptrDoc = punk; }
	IUnknown *GetData() { return unknown(); }

	void SetContext( IUnknown *punk ) { m_sptrContext = punk; }
	IUnknown *GetContext() { return unknown(); }

	//----------------------------------

	// interface IDocumentSite
	com_call GetDocumentTemplate( IUnknown **punk )
	{
		return E_NOTIMPL;
	}
	com_call GetFirstViewPosition( TPOS *plPos )
	{
		return E_NOTIMPL;
	}
	com_call GetNextView(IUnknown **ppunkView, TPOS *plPos)
	{
		return E_NOTIMPL;
	}
	com_call GetActiveView( IUnknown **ppunkView )
	{
		if( !ppunkView )
			return E_FAIL;

		*ppunkView = unknown();

		(*ppunkView)->AddRef();

		return S_OK;
	}
	com_call GetPathName( BSTR *pbstrPathName )
	{
		return E_NOTIMPL;
	}
	com_call SetActiveView( IUnknown *punkView )
	{
		return E_NOTIMPL;
	}
	com_call GetDocType( BSTR *pbstrDocType )
	{
		return E_NOTIMPL;
	}
	com_call SaveDocument( BSTR bstrFileName )
	{
		return E_NOTIMPL;
	}
	//////////////
	// interface IView

	com_call Init( IUnknown *punkDoc, IUnknown *punkSite )
	{
		return E_NOTIMPL;
	}
	com_call GetDocument( IUnknown **ppunkDoc )
	{
		return E_NOTIMPL;
	}
	com_call OnUpdate( BSTR bstr, IUnknown *punkObject )
	{
		return E_NOTIMPL;
	}
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
	{
		return E_NOTIMPL;
	}

	com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/)
	{
		return E_NOTIMPL;
	}
	com_call DoDragDrop( DWORD *pdwDropEffect )
	{
		return E_NOTIMPL;
	}

	com_call GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame)
	{
		return E_NOTIMPL;
	}
	com_call MoveDragFrameToFrame()
	{
		return E_NOTIMPL;
	}

	com_call GetDroppedDataObject(IUnknown** punkDO)
	{
		return E_NOTIMPL;
	}
	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
	{
		return E_NOTIMPL;
	}

	com_call GetFirstVisibleObjectPosition(TPOS *plpos)
	{
		return E_NOTIMPL;
	}
	com_call GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos)
	{
		return E_NOTIMPL;
	}

};

class CDialogViewFrame :
	public IScrollZoomSite,
	public com_unknown
{
	IUnknownPtr m_sptrView, m_sptrContext;

	static SIZE m_size;
	static POINT m_ptScroll;
	static double m_fZoom;
	static RECT m_rcBound;
	static bool m_bAutoScrollMode;

	static HANDLE m_hScrollVert;
	static HANDLE m_hScrollHorz;

	CVirtualDocument m_doc;
public:
	bool m_bCheckScroll;

	
	~CDialogViewFrame()
	{
		m_bCheckScroll = true;
	}

	void de_init()
	{
		IWindowPtr sptrWin = m_sptrView;	
		if( sptrWin )
			sptrWin->DestroyWindow();

		sptrWin = 0;


		IDataContextPtr	sptrDC = m_sptrContext;
		if( sptrDC )	
			sptrDC->AttachData(0);

		sptrDC = 0;

		m_sptrView = 0;

		m_sptrContext = 0;
		m_doc.SetData( 0 );
		m_doc.SetContext( 0 );
	}

	HWND handle()
	{
		HWND hWnd = 0;
		IWindowPtr sptrWin = m_sptrView;	
		if( sptrWin )
			sptrWin->GetHandle( (HANDLE *)&hWnd );

		return hWnd;
	}

	implement_unknown();

	void SetView( IUnknown *punk ) { m_sptrView = punk;	}
	IUnknown *GetView() { return unknown(); }

	void SetContext( IUnknown *punk ) { m_sptrContext = punk; m_doc.SetContext( punk ); }
	IUnknown *GetContext() { return unknown(); }

	void SetData( IUnknown *punk ) { m_doc.SetData( punk );	}
	IUnknown *GetData() { return m_doc.GetData(); }

	void SetScroll( HANDLE hScroll, UINT uDir )
	{
		if( uDir == SB_VERT )
			m_hScrollVert = hScroll;
		if( uDir == SB_HORZ )
			m_hScrollHorz = hScroll;
	}

	RECT GetClientRect() 
	{ 
		RECT rc;
		::GetClientRect( handle(), &rc);
		return rc;
	};

	virtual IUnknown *get_interface( const IID &iid )
	{
		IUnknown *punk = 0;

		if( iid == IID_IScrollZoomSite )
			return (IScrollZoomSite*)this;
		
		if( iid == IID_IDocumentSite )
			return (IDocumentSite *)this;

		if( m_sptrView != 0 && m_sptrView->QueryInterface( iid, (void**)(&punk) ) == S_OK )
		{
			punk->Release();
			return punk;
		}
		if( m_sptrContext != 0 && m_sptrContext->QueryInterface( iid, (void**)&punk ) == S_OK )
		{
			punk->Release();
			return punk;
		}
		
		return __super::get_interface( iid );
	}



	//---------------------------
	//IScrollZoomSite
	POINT CenterPoint( RECT rc )
	{
		POINT pt = { rc.left + ( rc.right - rc.left ) / 2, rc.top + ( rc.bottom - rc.top ) / 2 };
		return pt;
	}
	int Width( RECT rc ) { return rc.right - rc.left; }
	int Height( RECT rc ) { return rc.bottom - rc.top; }

	com_call GetScrollBarCtrl( DWORD sbCode, HANDLE *phCtrl )
	{ 
		if( !phCtrl )
			return S_FALSE;

		if( sbCode == SB_VERT )
			*phCtrl = m_hScrollVert;
		else if( sbCode == SB_HORZ )
			*phCtrl = m_hScrollHorz;
		return S_OK; 
	}
	com_call GetClientSize( SIZE *psize )
	{ 
		if( psize )
		{
			// [vanek] - 20.08.2004
			psize->cx = (LONG)floor( m_fZoom * m_size.cx );
			psize->cy = (LONG)floor( m_fZoom * m_size.cy );
		}

		return S_OK; 
	}
	com_call SetClientSize( SIZE sizeNew )
	{ 
        m_size = sizeNew;

		UpdateScroll( SB_BOTH );

		return S_OK; 
	}
	com_call GetScrollPos( POINT *pptPos )
	{ 
		if( !pptPos )
			return S_FALSE;

		::ZeroMemory( pptPos, sizeof( POINT ) );

		if( m_bCheckScroll )
		{
			if( _can_scroll( false ) )
				pptPos->x = m_ptScroll.x;

			if( _can_scroll( false ) )
				pptPos->y = m_ptScroll.y;
		}
		else
			*pptPos = m_ptScroll;

		return S_OK; 
	}
	com_call SetScrollPos( POINT ptPos )
	{
		m_ptScroll = ptPos;

		UpdateScroll( SB_BOTH );

		return S_OK; 
	}
	com_call GetVisibleRect( RECT *pRect )
	{ 
		if( !pRect )
			return E_FAIL;

		*pRect = GetScrollRect();

		IImageViewPtr sptrIV = m_sptrView;

		if( sptrIV )
		{
			IUnknown *punkImage = 0;
			sptrIV->GetActiveImage( &punkImage, 0 );

			IImage3Ptr sptrImage = punkImage;
			
			if( punkImage )
				punkImage->Release();

			if( sptrImage )
			{
				RECT rcImage = {0};

				int cx = 0, cy = 0; 
				sptrImage->GetSize( &cx, &cy );
				
				rcImage.right  = cx;
				rcImage.bottom = cy;


				if( rcImage.right < pRect->right )
					pRect->right = rcImage.right;

				if( rcImage.bottom < pRect->bottom )
					pRect->bottom = rcImage.bottom;
			}

		}

		return S_OK; 
	}
	com_call EnsureVisible( RECT rect )
	{
		SIZE	sizeObject = m_size;
		double fZoom = m_fZoom;

		if (rect.left < 0)
			rect.left = 0;
		if (rect.right > sizeObject.cx)
			rect.right = sizeObject.cx;
		if (rect.top < 0)
			rect.top = 0;
		if (rect.bottom > sizeObject.cy)
			rect.bottom = sizeObject.cy;

		RECT	rcClient = GetClientRect();

		POINT	ptScroll, ptScrollOld = m_ptScroll;

		ptScroll = ptScrollOld;

		RECT	rc;
		rc.left = int( rect.left*fZoom + .5 );
		rc.top = int( rect.top*fZoom + .5 );
		rc.right = int( rect.right*fZoom + .5 );
		rc.bottom = int( rect.bottom*fZoom + .5 );

		RECT rcObject;
		rcObject.left = int( rect.left*fZoom + ptScroll.x + .5 );
		rcObject.top = int( rect.top*fZoom + ptScroll.y + .5 );
		rcObject.right = int( rect.right*fZoom + ptScroll.x + .5 );
		rcObject.bottom = int( rect.bottom*fZoom + ptScroll.y + .5 );

		::OffsetRect( &rcClient, ptScroll.x, ptScroll.y );

		POINT	ptMaxScroll;

		if( ( rcClient.left > rc.left ||
			rcClient.top > rc.top ||
			rcClient.right <= rc.right ||
			rcClient.bottom <= rc.bottom ) )
		{
	//get the current scroll info
			HWND hscrollH = 0;
			HWND hscrollV = 0;
			
			GetScrollBarCtrl( SB_HORZ, (HANDLE*)&hscrollH );
			GetScrollBarCtrl( SB_VERT, (HANDLE*)&hscrollV );

			if( hscrollH == 0 || hscrollV == 0 )
				return false;

			SCROLLINFO	sih = {0}, siv = {0};

			sih.fMask = SIF_ALL;
			siv.fMask = SIF_ALL;

			::SendMessage( hscrollH, SBM_GETSCROLLINFO, 0, (LPARAM)&sih );
			::SendMessage( hscrollV, SBM_GETSCROLLINFO, 0, (LPARAM)&siv );

	//get scroll range
			ptMaxScroll.x = max( sih.nMax-sih.nPage, 0 );
			ptMaxScroll.y = max( siv.nMax-siv.nPage, 0 );
	//calc new scroll position
			if( rcClient.left > rc.left )ptScroll.x-=rcClient.left - rc.left;
			if( rcClient.right < rc.right )ptScroll.x-=rcClient.right - rc.right-1;
			if( rcClient.top > rc.top )ptScroll.y-=rcClient.top - rc.top;
			if( rcClient.bottom < rc.bottom )ptScroll.y-=rcClient.bottom - rc.bottom-1;

			if( Width(rcClient) < Width(rc)||Width(rc)==0 ) ptScroll.x = CenterPoint(rcObject).x-CenterPoint(rcClient).x;
			if( Height(rcClient) < Height(rc)||Height(rc)==0 ) ptScroll.y = CenterPoint(rcObject).y-CenterPoint(rcClient).y;

	//ajust new scroll position
			m_ptScroll.x = max( 0, min( ptMaxScroll.x, ptScroll.x ) );
			m_ptScroll.y = max( 0, min( ptMaxScroll.y, ptScroll.y ) );

			sih.fMask = SIF_ALL|SIF_DISABLENOSCROLL;
			siv.fMask = SIF_ALL|SIF_DISABLENOSCROLL;

			sih.nPos = m_ptScroll.x;
			siv.nPos = m_ptScroll.y;

			::SendMessage( hscrollH, SBM_SETSCROLLINFO, true, (LPARAM)&sih );
			::SendMessage( hscrollV, SBM_SETSCROLLINFO, true, (LPARAM)&siv );

			int	dx = m_ptScroll.x - ptScrollOld.x;
			int	dy = m_ptScroll.y - ptScrollOld.y;

			m_ptScroll.x += dx;
			m_ptScroll.y += dy;
			UpdateScroll( SB_BOTH );

			return S_OK;
		}
		return S_FALSE;
	}
	com_call GetZoom( double *pfZoom )
	{ 
		if( !pfZoom )
			return E_FAIL;

		*pfZoom = m_fZoom;

		return S_OK; 
	}
	com_call SetZoom( double fZoom )
	{ 
		// [vanek] - 20.08.2004
		m_fZoom = fZoom;
	
		UpdateScroll( SB_BOTH );
		if( IsWindow( handle() ) )
			InvalidateRect( handle(), 0, false );
		
		return S_OK; 
	}
	com_call SetScrollOffsets( RECT rcOffsets )
	{ 
		m_rcBound = rcOffsets;
		return S_OK; 
	}

	com_call SetAutoScrollMode( BOOL bSet )
	{
		if( bSet )
		{
			m_bAutoScrollMode = true;
			UpdateScroll( SB_BOTH );

		}
		else
			m_bAutoScrollMode = false;
		return S_OK;
	}
	com_call GetAutoScrollMode( BOOL* pbSet )
	{
		if( !pbSet )
			return E_FAIL;

		*pbSet = m_bAutoScrollMode;

		return S_OK;
	}

	com_call LockScroll( BOOL bLock )
	{ return E_NOTIMPL; }
	
	com_call UpdateScroll( WORD sbCode )
	{
		if( sbCode == SB_BOTH )
		{
			UpdateScroll( SB_VERT );
			return UpdateScroll( SB_HORZ );
		}

		double	fZoom  = 1;

		RECT	rcClient = GetClientRect();

		HWND hscroll = 0;
		GetScrollBarCtrl( sbCode, (HANDLE *)&hscroll );

		SIZE	sizeClient = {0};
		// [vanek] - 20.08.2004
		GetClientSize( &sizeClient );
	
		SCROLLINFO	si = {0};

		si.cbSize = sizeof( si );
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS|SIF_DISABLENOSCROLL;
		SendMessage( hscroll, SBM_GETSCROLLINFO, 0, (LPARAM)&si );
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS|SIF_DISABLENOSCROLL;
		si.nMin = 0;

		if( sbCode == SB_HORZ )
		{
			si.nMax = sizeClient.cx;
			si.nPos = m_ptScroll.x;
			si.nPage = rcClient.right;
		}
		else
		{
			si.nMax = sizeClient.cy;
			si.nPos = m_ptScroll.y;
			si.nPage = rcClient.bottom;
		}

		si.nPos = max( min( si.nMax-(int)si.nPage, si.nPos ), si.nMin );
		bool	bDisable = (si.nMax - int(si.nPage)) <= 0;

		if( bDisable )
		{
			si.nPos = 0;
			::EnableWindow( hscroll, false );
			::SendMessage( hscroll, SBM_ENABLE_ARROWS, ESB_DISABLE_BOTH, 0 );
		}
		else
		{
			::EnableWindow( hscroll, true );
			::SendMessage( hscroll, SBM_SETSCROLLINFO, true, (LPARAM)&si );
		}

 		if( sbCode == SB_HORZ )
			m_ptScroll.x = si.nPos;
		else
			m_ptScroll.y = si.nPos;

		RECT rcVis;
		GetVisibleRect( &rcVis );

		InvalidateRect( handle(), &rcVis, false );
		return S_OK;
	}
protected:
	bool _can_scroll( bool bVert )
	{
		IImageViewPtr sptrIV = m_sptrView;

		RECT pRect = GetClientRect();

		if( sptrIV )
		{
			IUnknown *punkImage = 0;
			sptrIV->GetActiveImage( &punkImage, 0 );

			IImage3Ptr sptrImage = punkImage;
			
			if( punkImage )
				punkImage->Release();

			if( sptrImage )
			{
				RECT rcImage = {0};

				int cx = 0, cy = 0; 
				sptrImage->GetSize( &cx, &cy );
				
				rcImage.right  = cx;
				rcImage.bottom = cy;


				if( !bVert && rcImage.right > pRect.right )
					return true;

				if( bVert && rcImage.bottom > pRect.bottom )
					return true;
			}

		}
		return false;
	}

	RECT GetScrollRect()
	{
		RECT rc = GetClientRect();

		rc.left  -= -m_rcBound.left;			rc.top -= -m_rcBound.top;
		rc.right += -m_rcBound.right;			rc.bottom += -m_rcBound.bottom;

		return rc;
	}
};

#include "\vt5\common\resize_dlg.h"

class CCompareImagesDlg : public dlg_impl, 
						  public resizable_dlg_impl< CCompareImagesDlg >
{
	CDialogViewFrame *m_pframe, *m_pframe2;
	CString m_strLang;

	CString m_strClassFile, m_strClassUnk;
	long m_lEnableSelection;
	long m_lLastHLPerc, m_lLastHLEnable;

	TEST_ERR_DESCR *m_pErrDescr;

	long m_lCurrentImage, m_lMaxImage;
	bool *m_pbShowStates;
public:
	CCompareImagesDlg();
	virtual ~CCompareImagesDlg();

	BEGIN_DLGRESIZE_MAP( CCompareImagesDlg )
		DLGRESIZE_CONTROL( IDC_SCROLLBAR1 , DLG_SIZE_Y | DLG_MOVE_X )
		DLGRESIZE_CONTROL( IDC_SCROLLBAR2 , DLG_SIZE_X | DLG_MOVE_Y )

		DLGRESIZE_CONTROL( IDC_VIEW_HERE2 , DLG_SIZE_Y )
		DLGRESIZE_CONTROL( IDC_VIEW_HERE  , DLG_SIZE_Y )

		DLGRESIZE_CONTROL( IDC_BUTTON2 , DLG_MOVE_X )
		DLGRESIZE_CONTROL( IDC_BUTTON1 , DLG_MOVE_X )

//		DLGRESIZE_CONTROL( IDC_BUTTON3, DLG_MOVE_X )
//		DLGRESIZE_CONTROL( IDC_BUTTON4, DLG_MOVE_X )

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( -1, DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_BUTTON4, DLG_MOVE_X )
	    END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( -1, DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_BUTTON3, DLG_MOVE_X )
	    END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( -1, DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_STATIC_RES, DLG_MOVE_X )
	    END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_VIEW_HERE2 , DLG_SIZE_X | DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_VIEW_HERE  , DLG_SIZE_X)
	    END_DLGRESIZE_GROUP()
	END_DLGRESIZE_MAP()

	IMPLEMENT_RESIZE( handle() );


	virtual LRESULT on_initdialog();
	
	LRESULT on_hscroll( unsigned code, unsigned pos, HWND hwndScroll )
	{
		RECT	rcClient = {0};
 		::GetClientRect( get_dlg_item( IDC_VIEW_HERE ), &rcClient );

		POINT ptScroll = {0};

		m_pframe->m_bCheckScroll = false;
		m_pframe->GetScrollPos( &ptScroll );
		m_pframe->m_bCheckScroll = true;

		if( code == SB_LINEDOWN )
			ptScroll.x++;
		else if( code == SB_LINEUP )
			ptScroll.x--;
		else if( code == SB_PAGEDOWN )
			ptScroll.x += rcClient.right - rcClient.left;
		else if( code == SB_PAGEUP )
			ptScroll.x -= rcClient.right - rcClient.left;
		else if( code == SB_THUMBTRACK )
			ptScroll.x = pos;

 		m_pframe->SetScrollPos( ptScroll );

		RECT rcVis = {0};
		double fzoom = 1;

		m_pframe->GetVisibleRect( &rcVis );
		m_pframe->GetZoom( &fzoom );

		rcVis.left		*= fzoom;
		rcVis.right		*= fzoom; 
		rcVis.top		*= fzoom;
		rcVis.bottom	*= fzoom;

		InvalidateRect( m_pframe->handle(), &rcVis, false );

		m_pframe2->GetVisibleRect( &rcVis );
		m_pframe2->GetZoom( &fzoom );

		rcVis.left		*= fzoom;
		rcVis.right		*= fzoom; 
		rcVis.top		*= fzoom;
		rcVis.bottom	*= fzoom;

		InvalidateRect( m_pframe2->handle(), &rcVis, false );

		UpdateWindow( m_pframe->handle() );
		UpdateWindow( m_pframe2->handle() );
		
		
		return 0L;
	}

	LRESULT on_vscroll( unsigned code, unsigned pos, HWND hwndScroll )
	{
		RECT	rcClient = {0};
 		::GetClientRect( get_dlg_item( IDC_VIEW_HERE ), &rcClient );

		POINT ptScroll = {0};
		m_pframe->GetScrollPos( &ptScroll );

		if( code == SB_LINEDOWN )
			ptScroll.y++;
		else if( code == SB_LINEUP )
			ptScroll.y--;

		else if( code == SB_PAGEDOWN )
			ptScroll.y += rcClient.bottom - rcClient.top;
		else if( code == SB_PAGEUP )
			ptScroll.y -= rcClient.bottom - rcClient.top;

		else if( code == SB_THUMBTRACK )
			ptScroll.y = pos;

		m_pframe->SetScrollPos( ptScroll );

		RECT rcVis = {0};
		double fzoom = 1;

		m_pframe->GetVisibleRect( &rcVis );
		m_pframe->GetZoom( &fzoom );

		rcVis.left		*= fzoom;
		rcVis.right		*= fzoom; 
		rcVis.top		*= fzoom;
		rcVis.bottom	*= fzoom;

		InvalidateRect( m_pframe->handle(), &rcVis, false );

		m_pframe2->GetVisibleRect( &rcVis );
		m_pframe2->GetZoom( &fzoom );

		rcVis.left		*= fzoom;
		rcVis.right		*= fzoom; 
		rcVis.top		*= fzoom;
		rcVis.bottom	*= fzoom;

		InvalidateRect( m_pframe2->handle(), &rcVis, false );

		UpdateWindow( m_pframe->handle() );
		UpdateWindow( m_pframe2->handle() );

		return 0L;
	}


	long on_size( short cx, short cy, ulong fSizeType )	
	{
		resize_control( cx, cy );

		// [vanek] - 20.08.2004
		_check_sizes( );

		m_pframe->UpdateScroll( SB_BOTH );

		return 0L;
	}

	void set_test_param( TEST_ERR_DESCR *pErrDescr )
	{
		m_pErrDescr = pErrDescr;
	}

	void _execute_script( IUnknown *punk, BSTR bstrScript );
	void _redraw_images();
	void _check_sizes( );  // [vanek] - 20.08.2004
	LRESULT on_destroy();

	LRESULT	handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_COMMAND && LOWORD( w ) == IDC_BUTTON1 && HIWORD( w ) == BN_CLICKED )
		{
			m_lCurrentImage--;
			_redraw_images();
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_BUTTON2 && HIWORD( w ) == BN_CLICKED )
		{
			m_lCurrentImage++;
			_redraw_images();
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_BUTTON3 && HIWORD( w ) == BN_CLICKED )
		{
			double fzoom = 1;
			double fzoom2 = 1;

			m_pframe->GetZoom( &fzoom );
			m_pframe2->GetZoom( &fzoom2 );

			if( fzoom < 16 )
			{
				fzoom  += 1;
				fzoom2 += 1;

				m_pframe->SetZoom( fzoom );
				m_pframe2->SetZoom( fzoom2 );
			}

			_redraw_images();
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_BUTTON4 && HIWORD( w ) == BN_CLICKED )
		{
			double fzoom = 1;
			double fzoom2 = 1;

			m_pframe->GetZoom( &fzoom );
			m_pframe2->GetZoom( &fzoom2 );

			if( fzoom > 1 )
			{
				fzoom  -= 1;
				fzoom2 -= 1;

				m_pframe->SetZoom( fzoom );
				m_pframe2->SetZoom( fzoom2 );
			}

			_redraw_images();
		}
		return __super::handle_message( m, w, l );
	}
};
