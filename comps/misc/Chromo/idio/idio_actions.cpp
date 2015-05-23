#include "stdafx.h"
#include "idio_object.h"
#include "idio_actions.h"
#include "com_main.h"
#include <WinBase.h>
#include "class_utils.h"
#include "carioint.h"
#include "idiodbase_int.h"


#include <TCHAR.H>

#include "/vt5/awin/misc_map.h"
#include "/vt5/awin/misc_module.h"
#include "/vt5/awin/win_dlg.h"

#include "misc_list.h"
#include "docview5.h"
#include "nameconsts.h"

inline long cmp_CString( const CString psz1, const CString psz2 )
{	return psz1.Compare( psz2 ); };

CString GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	CString	str(bstr);
	::SysFreeString(bstr);

	return str;
}
IUnknown	*CreateTypedObject( const BSTR bstrType );
IUnknownPtr _GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType, bool bThrowException = true)
{
	_bstr_t	bstrObjectName = bstrObject;
	_bstr_t	bstrTypeName = bstrType;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrCurTypeName = 0;
		ptrDataType->GetType( nType, &bstrCurTypeName );

		_bstr_t	str( bstrCurTypeName, false );

		if( bstrType && str != bstrTypeName )
			continue;

		IUnknown	*punkObj = 0;
		LONG_PTR	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || GetObjectName( punkObj ) == (char*)bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			POSITION	lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetObjectName( punkObject ) == (char*)bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	return 0;
}
class insert_dialog : public dlg_impl, public ComObjectBase, public IScrollZoomSite
{
	CString m_main_path;
	action_insert	*m_paction;
	route_unknown();
	IUnknownPtr m_sptrView, m_sptrContext, m_sptrDoc;
	BOOL m_bMode;
	bool bStored;

	SIZE m_size;
	POINT m_ptScroll;
public:
	insert_dialog( action_insert *pai ) : dlg_impl( IDD_INSERT_IDIO )
	{
		m_bMode = true;
		bStored = false;
		m_paction = pai;

		::ZeroMemory( &m_size, sizeof( m_size ) );
		::ZeroMemory( &m_ptScroll, sizeof( m_ptScroll ) );
	}

	~insert_dialog()
	{
		deinit_db();
	}

	long init_db( CString str )
	{
		deinit_db();
		CLSID	clsidDoc = {0}, clsidView = {0}, clsidContext = {0}, clsidData = {0};

		if( ::CLSIDFromProgID( _bstr_t( "IdioDBase.Document" ), &clsidDoc ) != S_OK )
			return 0;

		if( ::CLSIDFromProgID( _bstr_t( "IdioDBase.IdioDBView" ), &clsidView ) != S_OK  )
			return 0;
		
		if( ::CLSIDFromProgID( _bstr_t( szContextProgID ), &clsidContext ) != S_OK  )
			return 0;

		if( ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) != S_OK  )
			return 0;

		IUnknown *punk = 0;

		if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
			return 0;
		
		m_sptrDoc = punk;
		if( punk )
			punk->Release(); punk = 0;

		if( m_sptrDoc == 0 )
			return 0;

		IFileDataObjectPtr	sptrFF = m_sptrDoc;

		if( sptrFF == 0 )
			return 0;

		sptrFF->LoadFile( _bstr_t( str ) );

		if( ::CoCreateInstance( clsidView, Unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
			return 0;
		
		m_sptrView = punk;
		if( punk )
			punk->Release(); punk = 0;

		IViewPtr sptrView = m_sptrView;

		if( sptrView == 0 )
			return 0;

		if( sptrView->Init( m_sptrDoc, 0 ) != S_OK )
			return 0;

		if( !bStored )
		{
			m_bMode = 1;
			IIdioDBaseViewPtr sptrV = sptrView;
			if( sptrV )
				sptrV->EnableWindowMessages( &m_bMode );
			bStored = true;
		}
		IIdioDBaseViewPtr sptrV = sptrView;
		if( sptrV )
			sptrV->EnableContextMenu( false );


		if( ::CoCreateInstance( clsidContext, Unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
			return 0;

		m_sptrContext = punk;
		if( punk )
			punk->Release(); punk = 0;

		IDataContext2Ptr sptrContext = m_sptrContext;

		if( sptrContext == 0 )
			return 0;

		sptrContext->AttachData( m_sptrDoc );

		_bstr_t bstrIDB = szArgumentTypeIdioDBase;

		long lCount = 0;
		sptrContext->GetObjectCount( bstrIDB, &lCount );

		if( lCount == 0 )
			return 0;
		
		IScrollZoomSitePtr	pstrS = Unknown();	

		if( pstrS == 0 )
			return 0;

		IWindowPtr sptrWin = m_sptrView;		

		if( sptrWin == 0 )
			return 0;

		RECT rcView = {0};
		::GetWindowRect( get_dlg_item( IDC_IDIO_LIST ), &rcView ) ;
		
		::ScreenToClient( handle(), (LPPOINT)&rcView );
		::ScreenToClient( handle(), (LPPOINT)&rcView + 1 );

		if( sptrWin->CreateWnd( handle(), rcView, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 23322323 ) != S_OK )
			return 0;

		LONG_PTR lPos = 0;
		sptrContext->GetFirstObjectPos( bstrIDB, &lPos );

		if( lPos )
		{
			IUnknown *punkIDB = 0;
			sptrContext->GetNextObject( bstrIDB, &lPos, &punkIDB );
			sptrContext->SetActiveObject( bstrIDB, punkIDB, 0 );

			if( punkIDB )
				punkIDB->Release();
		}
		return 1L;
	}

	long on_initdialog()
	{
		char	sz_path[MAX_PATH];

		GetModuleFileName( 0, sz_path, sizeof( sz_path ) );
		strcpy( strrchr( sz_path, '\\' ), "\\idio_db\\" );

		HWND hCombo = GetDlgItem( handle(), IDC_COMBO_TYPE );

		_fill_combo( hCombo, sz_path, "" );
		m_main_path = sz_path;

		CString strMain = (char*)::GetValueString( ::GetAppUnknown(), "\\IdioDBase", "ShowAllFile", "" );

		if( !strMain.IsEmpty() )
		{
			strMain = strMain.Left( strMain.GetLength() - 4 );
			strMain = strMain.Right( strMain.GetLength() - 1 - strMain.ReverseFind( '\\' ) );
			::SendMessage( hCombo, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)strMain );
			init_db( m_main_path + strMain + ".dbi" );
		}
		else
		{
			TCHAR tch[MAX_PATH];
			::SendMessage( hCombo, CB_GETLBTEXT, 0, (LPARAM)tch );
			init_db( m_main_path + tch + ".dbi" );
		}
		
		return 1L;
	}

	LRESULT on_destroy()
	{
		IWindowPtr sptrWin = m_sptrView;	
		if( sptrWin )
			sptrWin->DestroyWindow();

		IDataContextPtr	sptrDC = m_sptrContext;
		if( sptrDC )	
			sptrDC->AttachData(0);	


		if( bStored )
		{
			IIdioDBaseViewPtr sptrV = m_sptrView;
			if( sptrV )
				sptrV->EnableWindowMessages( &m_bMode );
			bStored = true;
		}

		return 0L;
	}

	void deinit_db()
	{
		m_sptrContext = 0;
		m_sptrView = 0;
		m_sptrDoc = 0;
	}

	IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IScrollZoomSite )
			return (IScrollZoomSite*)this;
		return 0;	
	}

	HRESULT DoQueryAggregates( const IID &iid, void **ppRet )
	{		
		if( m_sptrView != 0 && m_sptrView->QueryInterface( iid, ppRet ) == S_OK )
			return S_OK;
		if( m_sptrContext != 0 && m_sptrContext->QueryInterface( iid, ppRet ) == S_OK )
			return S_OK;
		return S_FALSE;
	}

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
			*phCtrl = get_dlg_item( IDC_SCROLLBAR1 );
		else if( sbCode == SB_HORZ )
			*phCtrl = get_dlg_item( IDC_SCROLLBAR2 );
		return S_OK; 
	}
	com_call GetClientSize( SIZE *psize )
	{ 
		if( !psize )
			*psize = m_size;
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
	{ return E_NOTIMPL; }
	com_call EnsureVisible( RECT rect )
	{
		SIZE	sizeObject = m_size;
		double fZoom = 1;

		if (rect.left < 0)
			rect.left = 0;
		if (rect.right > sizeObject.cx)
			rect.right = sizeObject.cx;
		if (rect.top < 0)
			rect.top = 0;
		if (rect.bottom > sizeObject.cy)
			rect.bottom = sizeObject.cy;

		RECT	rcClient;
		::GetClientRect( get_dlg_item( IDC_IDIO_LIST ), &rcClient );

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
	{ return E_NOTIMPL; }
	com_call SetZoom( double fZoom )
	{ return E_NOTIMPL;  }
	com_call SetScrollOffsets( RECT rcOffsets )
	{ return E_NOTIMPL; }
	com_call SetAutoScrollMode( BOOL bSet )
	{ return E_NOTIMPL; }
	com_call GetAutoScrollMode( BOOL* pbSet )
	{ return E_NOTIMPL; }
	com_call LockScroll( BOOL bLock )
	{ return E_NOTIMPL; }
	
	com_call UpdateScroll( WORD sbCode )
	{
		if( sbCode == SB_BOTH )
		{
			UpdateScroll( SB_VERT );
			return UpdateScroll( SB_HORZ );
		}

		long	nZoom  = 1;

		RECT	rcClient = {0};
		::GetClientRect( get_dlg_item( IDC_IDIO_LIST ), &rcClient );

		HWND hscroll = 0;
		GetScrollBarCtrl( sbCode, (HANDLE *)&hscroll );

		SIZE	sizeClient = m_size;
	
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
		bool	bDisable = (si.nMax - int(si.nPage)) < 0;

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

		InvalidateRect( handle(), 0, 0 );
		return S_OK;
	}

	void _fill_combo( HWND hWnd, CString _str_path, CString _str_path2 )
	{
		WIN32_FIND_DATA	w32fd = {0};
		HANDLE	hFileFind = ::FindFirstFile( _str_path + _str_path2 + "*.dbi", &w32fd );
		
		bool	bContinue = hFileFind != INVALID_HANDLE_VALUE, bHasFiles = false;
		while( bContinue )
		{
			CString str( w32fd.cFileName );
			if( str != "." && str != ".." )
			{
				if( w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					_fill_combo( hWnd, _str_path, _str_path2 + "\\" + w32fd.cFileName + "\\" );
				else
				{
					bHasFiles = true;

					CString str;
					if( !_str_path2.IsEmpty() )
						str = _str_path2;

					str += w32fd.cFileName;

					str = str.Left( str.GetLength() - 4 );

		
					if( !str.IsEmpty() )
						::SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)( ( LPCTSTR )str ) );
				}
			}

			bContinue = ::FindNextFile( hFileFind, &w32fd )!=0;
		}

		if( hFileFind != INVALID_HANDLE_VALUE )
			::FindClose( hFileFind );
	}

	virtual void on_ok()
	{
		IIdioDBaseViewPtr sptrView = m_sptrView;
		if( sptrView )
		{
			IUnknown *punk = 0;

			sptrView->GetActiveObject( &punk );

			IIdiogramPtr sptrIdio = punk;
			m_paction->m_insert.insert_before( sptrIdio, 0 ); 
		}

		__super::on_ok();
	}
	LRESULT handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_COMMAND && IDC_COMBO_TYPE == LOWORD( w ) && CBN_SELCHANGE == HIWORD( w ) )
		{
			HWND hCombo = GetDlgItem( handle(), IDC_COMBO_TYPE );
			if( hCombo )
			{
				int nSel = ::SendMessage( hCombo, CB_GETCURSEL, 1, 0);

				if( nSel != -1 )
				{

					TCHAR tch[MAX_PATH];

					::SendMessage( hCombo, CB_GETLBTEXT, nSel, (LPARAM)tch );

					{
						IWindowPtr sptrWin = m_sptrView;	
						if( sptrWin )
						{
							HANDLE handle = 0;
							sptrWin->GetHandle( &handle );

							if( handle )
								sptrWin->DestroyWindow();
						}

						IDataContextPtr	sptrDC = m_sptrContext;
						if( sptrDC )	
							sptrDC->AttachData(0);	
					}

					::ZeroMemory( &m_size, sizeof( m_size ) );
					::ZeroMemory( &m_ptScroll, sizeof( m_ptScroll ) );

					init_db( m_main_path + tch + ".dbi" );
				}
			}
		}
		return __super::handle_message( m, w, l );
	}

	LRESULT on_hscroll(unsigned code, unsigned pos, HWND hwndScroll)
	{
		RECT	rcClient = {0};
		::GetClientRect( get_dlg_item( IDC_IDIO_LIST ), &rcClient );

		if( code == SB_LINEDOWN )
			m_ptScroll.x++;
		else if( code == SB_LINEUP )
			m_ptScroll.x--;
		else if( code == SB_PAGEDOWN )
			m_ptScroll.x += rcClient.right - rcClient.left;
		else if( code == SB_PAGEUP )
			m_ptScroll.x -= rcClient.right - rcClient.left;
		else if( code == SB_THUMBTRACK )
			m_ptScroll.x = pos;

		UpdateScroll( SB_HORZ );
		return 0L;
	}

	LRESULT on_vscroll(unsigned code, unsigned pos, HWND hwndScroll)
	{
		RECT	rcClient = {0};
		::GetClientRect( get_dlg_item( IDC_IDIO_LIST ), &rcClient );

		if( code == SB_LINEDOWN )
			m_ptScroll.y++;
		else if( code == SB_LINEUP )
			m_ptScroll.y--;
		else if( code == SB_PAGEDOWN )
			m_ptScroll.y += rcClient.bottom - rcClient.top;
		else if( code == SB_PAGEUP )
			m_ptScroll.y -= rcClient.bottom - rcClient.top;
		else if( code == SB_THUMBTRACK )
			m_ptScroll.y = pos;

		UpdateScroll( SB_VERT );
		return 0L;
	}
};
/**/

action_insert::~action_insert()
{
	TPOS	lpos = m_insert.head();
	while( lpos )m_insert.next( lpos )->Release();
}



HRESULT action_insert::DoInvoke()
{
	if( !m_insert.Count() )return S_FALSE;

	IUnknown	*punkChromo = get_object();
	if( !punkChromo )return S_FALSE;

	INamedDataObject2Ptr	ptrChromo( punkChromo );
	punkChromo->Release();

	IUnknown	*punkMeas =0;
	ptrChromo->GetParent( &punkMeas );
	if( !punkMeas )return E_INVALIDARG;

	IUnknownPtr	ptrActiveObject = punkMeas;
	punkMeas->Release();

	//check the active measure object have no ifiogram


	bool	fHasIdio  = false;
	if( ptrActiveObject != 0 )
	{
		INamedDataObject2Ptr	ptrNamed( ptrActiveObject );

		POSITION	lpos_c = 0;
		ptrNamed->GetFirstChildPosition( &lpos_c );
		while( lpos_c )
		{
			IUnknown	*punk_child = 0;
			ptrNamed->GetNextChild( &lpos_c, &punk_child );

			IIdiogramPtr	ptrITest = punk_child;
			if( punk_child )
				punk_child->Release();

			if( ptrITest != 0 )
			{
				unsigned dwFlags = 0;

				ptrITest->get_flags( &dwFlags );
				
				if( dwFlags == IDIT_INSERTED )
				{
					RemoveFromDocData( m_ptrTarget, ptrITest );
					break;
				}
			}
		}
	}

	//insert the selected idiogrtam
	bool	fFirst = true;
	for( TPOS lpos = m_insert.head(); lpos; m_insert.next( lpos ) )
	{
		IIdiogram	*p = m_insert.get( lpos );

		if( p )
			p->set_flags( IDIT_INSERTED );
	//attach the first idiogram to the active object, if it exist
		if( fFirst )
		{
			if( ptrActiveObject != 0 )
			{
				INamedDataObject2Ptr	ptrND( p );
				ptrND->SetParent( ptrActiveObject, 0 );
		 		SetToDocData( m_ptrTarget, p );
			}
			fFirst = false;
		}
/*		else
		{
			ptrList->SetActiveChild( 0 );
	 		SetToDocData( m_ptrTarget, p );
		}*/
	}

	return S_OK;
}

HRESULT action_insert::CreateSettingWindow( HWND hwnd )
{
	module	_module( app::handle() );
	insert_dialog *dlg = new insert_dialog( this );

	if( dlg->do_modal( hwnd ) == IDOK )
	{
		dlg->Release();
		return S_OK;
	}
	else
	{
		dlg->Release();
		return S_FALSE;
	}
}

IUnknown *action_insert::get_object()
{
	IDocumentSitePtr ptr_ds( m_ptrTarget );
	if( ptr_ds == 0 )
		return 0;

	IUnknown* punk_view = 0;
	ptr_ds->GetActiveView( &punk_view );
	if( punk_view == 0 )
		return 0;

	ICarioViewPtr ptrView = punk_view;
	punk_view->Release();	punk_view = 0;

	if( ptrView == 0 )
		return 0;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return 0;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	IUnknown	*punk = 0;
	ptrView->GetChromoObject( nLine, nCell, nChromo, &punk );
	return punk;
}

HRESULT action_insert::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;

	IUnknown	*punk = get_object();
	if( punk == 0 )return S_OK;
	punk->Release();

	*pdwState = afEnabled;
	return S_OK;
}
