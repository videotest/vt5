#include "stdafx.h"
#include "ax_host.h"
#include "EXDISPID.H"


LRESULT __stdcall InitAxWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	if( nMessage == WM_NCCREATE )
	{
		CAxContainerWin		*pwnd = new CAxContainerWin();
		pwnd->Subclass( hWnd );
		return pwnd->DoMessage( nMessage, wParam, lParam );
	}
	return 0;
}

//CAxContainerWin
CAxContainerWin::CAxContainerWin()
{
	m_bCapture = false;
	m_punkOuter = 0;	//??
	m_hwndControl = 0;	
}

CAxContainerWin::~CAxContainerWin()
{
}

IUnknown *CAxContainerWin::DoGetInterface( const IID &iid )
{
	if( iid == IID_IOleInPlaceSite )				return (IOleInPlaceSite*)this;
    else if( iid == IID_IOleControlSite )			return (IOleControlSite *)this;
    else if( iid == IID_IOleInPlaceSiteWindowless ) return (IOleInPlaceSiteWindowless *)this;
    else if( iid == IID_IDispatch )					return (IDispatch *)this;
    else return ComObjectBase::DoGetInterface( iid );
}

void CAxContainerWin::DoPaint()
{
	PAINTSTRUCT	paint;

	HDC hdc = ::BeginPaint( m_hwnd, &paint );

	HFONT	hFont = ::CreateFont( 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial" );
	HFONT	hOldFont = (HFONT)::SelectObject( hdc, hFont );

	HBRUSH	hBrush = ::CreateSolidBrush( RGB( 192, 192, 255 ) );
	HPEN	hPen = ::CreatePen( PS_SOLID, 0, RGB( 255, 255, 255 ) );

	HBRUSH	hOldBrush = (HBRUSH)::SelectObject( hdc, hBrush );
	HPEN	hOldPen = (HPEN)::SelectObject( hdc, hPen );

	RECT	rect;
	::GetClientRect( m_hwnd, &rect );

	::SetTextColor( hdc, RGB( 255, 255, 255 ) );
	::SetBkMode( hdc, TRANSPARENT );

	::Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
	::DrawText( hdc, m_bstrClassName, m_bstrClassName.length(), &rect, DT_SINGLELINE|DT_CENTER|DT_VCENTER );

	::SelectObject( hdc, hOldFont );
	::SelectObject( hdc, hOldPen );
	::SelectObject( hdc, hOldBrush );

	::EndPaint( m_hwnd, &paint );

	::DeleteObject( hBrush );
	::DeleteObject( hPen );
	::DeleteObject( hFont );
}

bool CAxContainerWin::CreateControl( const char *pszRegString, IStream *pstream  )
{
	::GetClientRect( m_hwnd, &m_rectPos );

	ClientToScreen( m_hwnd, ((POINT*)&m_rectPos) );
	ClientToScreen( m_hwnd, ((POINT*)&m_rectPos)+1 );
	ScreenToClient( GetHostWindow(), ((POINT*)&m_rectPos) );
	ScreenToClient( GetHostWindow(), ((POINT*)&m_rectPos)+1 );

	CLSID	clsid;
	BSTR		bstr;
	m_bstrClassName = pszRegString;

	::CLSIDFromString( m_bstrClassName, &clsid );
	::ProgIDFromCLSID( clsid, &bstr );

	m_bstrClassName = bstr;
	App::malloc()->Free( bstr );

	m_ptr.CreateInstance( clsid );

	if( m_ptr == 0 )return false;

	IOleObjectPtr	ptrOleObject( m_ptr );
	
	if( ptrOleObject== 0  )
	{
		m_ptr = 0;
		return false;
	}

	ptrOleObject->SetClientSite(this);

	IObjectWithSitePtr	ptrIObjectWithSite = m_ptr;
	if( ptrIObjectWithSite != 0 )
		ptrIObjectWithSite->SetSite( Unknown() );

	IPersistStreamInitPtr	ptrInit( m_ptr );
	if( ptrInit != 0 )
	{
		try
		{
			if( pstream ) 
				ptrInit->Load( pstream );
			else
				ptrInit->InitNew();
		}
		catch( ... )
		{
		}
	}


	ptrOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, GetHostWindow(), &m_rectPos);
	ptrOleObject->DoVerb(OLEIVERB_SHOW, NULL, this, 0, GetHostWindow(), &m_rectPos);

	
           

	m_hwndControl = 0;
    IOleInPlaceActiveObjectPtr ptrOleIPA( m_ptr );
       
    if( ptrOleIPA != 0 )
	{
		ptrOleIPA->GetWindow( &m_hwndControl );

		if( m_hwndControl != 0 )
		{
			::SetWindowLong( m_hwndControl, GWL_ID, ::GetDlgCtrlID( m_hwnd ) );
		}
	}

	ShowWindow( m_hwnd, SW_HIDE );

    return true;
}


LRESULT CAxContainerWin::DoCreate( CREATESTRUCT *lpCreate )
{
	int nCreateSize = 0;
	if (lpCreate && lpCreate->lpCreateParams)
		nCreateSize = *(WORD*)lpCreate->lpCreateParams;

	HGLOBAL h = GlobalAlloc(GHND, nCreateSize);
	IStreamPtr ptrStream;
	if (h && nCreateSize)
	{
		BYTE* pBytes = (BYTE*) GlobalLock(h);
		BYTE* pSource = ((BYTE*)(lpCreate->lpCreateParams)) + sizeof(WORD); 
		//Align to DWORD
		//pSource += (((~((DWORD)pSource)) + 1) & 3);

		memcpy(pBytes, pSource, nCreateSize);
		GlobalUnlock(h);
		CreateStreamOnHGlobal(h, TRUE, &ptrStream);
	}

	if( !CreateControl( lpCreate->lpszName, ptrStream ) )
		return 0;

	UINT	nID = (DWORD)lpCreate->hMenu;
	::SetWindowLong( m_hwndControl, GWL_STYLE, (DWORD)lpCreate->style );
	::SetWindowLong( m_hwndControl, GWL_EXSTYLE, (DWORD)lpCreate->dwExStyle );
	::SetWindowLong( m_hwndControl, GWL_ID, nID );

	nID = ::GetWindowLong( m_hwndControl, GWL_ID );
	return 1;
}

void CAxContainerWin::DestroyControl()
{

	IOleObjectPtr	ptrOleObject = m_ptr;

	if( ptrOleObject != 0 )
	{
        ptrOleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, m_hwnd, NULL);
        ptrOleObject->Close(OLECLOSE_NOSAVE);
        ptrOleObject->SetClientSite(NULL);
    }

	m_ptr = 0;
}

LRESULT	CAxContainerWin::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	//route to parent
	if( nMsg == WM_NOTIFY || nMsg == WM_COMMAND )
		return SendMessage( ::GetParent( m_hwnd ), nMsg, wParam, lParam );
	if( nMsg == WM_GETCONTROL )
		return DoGetControl();

	if( nMsg == WM_NCDESTROY )
	{
		DestroyControl();
		Unknown()->Release();

		return 1;
	}
	return CWinImpl::DoMessage( nMsg, wParam, lParam );
}

// *** IDispatch Methods ***
HRESULT CAxContainerWin::GetIDsOfNames( REFIID riid, OLECHAR FAR* FAR* rgszNames,	unsigned int cNames, LCID lcid,	DISPID FAR* rgdispid )
{
    HRESULT     hr;                     // standard ole return code
    LPOLESTR    pName;
    DISPID      *pdispid;

    hr      = S_OK;
    pName   = *rgszNames;
    pdispid = rgdispid;

    for (UINT i=0; i<cNames; i++)
    {
        if (pName != NULL)
        {
            if (_wcsicmp(pName, L"basehref") == 0)
                *pdispid = DISPID_BASEHREF;
            else if (_wcsicmp(pName, L"align") == 0)
                *pdispid = DISPID_ALIGN;
            else
            {
                *pdispid = DISPID_UNKNOWN;
                hr       = DISP_E_UNKNOWNNAME;
            }
        }

        pdispid++;
        pName++;
    }

    return (hr);
}

HRESULT CAxContainerWin::GetTypeInfo( unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::GetTypeInfoCount( unsigned int FAR * pctinfo )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::Invoke( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR * pexecinfo, unsigned int FAR *puArgErr )
{
    HRESULT     hr = S_OK;  // standard ole return code

    if ((pvarResult == NULL) && (wFlags == DISPATCH_PROPERTYGET))
        return (E_INVALIDARG);
    
    switch (dispid)
    {
        case DISPID_AMBIENT_DISPLAYNAME:
            if (!pvarResult->bstrVal)
                pvarResult->bstrVal = ::SysAllocString(L"");

            hr = S_OK;

            // If we STILL don't have a bstrVal.  Clean up and return an empty variant.
            if (!pvarResult->bstrVal) 
            {
                VariantInit(pvarResult);
                hr = E_FAIL;
            }

            break;

        case DISPID_AMBIENT_USERMODE:
        case DISPID_AMBIENT_MESSAGEREFLECT:
            pvarResult->vt      = VT_BOOL;
            pvarResult->boolVal = TRUE;
            hr                  = S_OK;
            break;

        case DISPID_AMBIENT_SHOWHATCHING:
        case DISPID_AMBIENT_SHOWGRABHANDLES:
        case DISPID_AMBIENT_SUPPORTSMNEMONICS:
            pvarResult->vt      = VT_BOOL;
            pvarResult->boolVal = FALSE;
            hr                  = S_OK;
            break;

        // Not yet implemented!
        case DISPID_AMBIENT_BACKCOLOR:
        case DISPID_AMBIENT_FORECOLOR:
        case DISPID_AMBIENT_UIDEAD:
        case DISPID_AMBIENT_AUTOCLIP:
            hr = S_OK;
            break;

        // Extender Properties
        case DISPID_BASEHREF:
        case DISPID_ALIGN:
            hr = DISP_E_MEMBERNOTFOUND;
            break;

        default:
            hr = DISP_E_MEMBERNOTFOUND;
    }

    return (hr);
}
// *** IOleClientSite methods ***
HRESULT CAxContainerWin::SaveObject(  )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::GetMoniker( DWORD, DWORD, LPMONIKER * )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::GetContainer( LPOLECONTAINER * )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::ShowObject(  )
{
	return S_OK;
}
HRESULT CAxContainerWin::OnShowWindow( BOOL )
{
	return S_OK;
}
HRESULT CAxContainerWin::RequestNewObjectLayout(  )
{
	//return S_OK;
	return E_NOTIMPL;
}
// *** IOleWindow Methods ***
HRESULT CAxContainerWin::GetWindow( HWND * phwnd )
{
    *phwnd = GetHostWindow();
    return (S_OK);
}

HRESULT CAxContainerWin::ContextSensitiveHelp( BOOL fEnterMode )
{
	//return S_OK;
	return E_NOTIMPL;
}
// *** IOleInPlaceSite Methods ***
HRESULT CAxContainerWin::CanInPlaceActivate( void )
{
	return S_OK;
}
HRESULT CAxContainerWin::OnInPlaceActivate( void )
{
	return S_OK;
}
HRESULT CAxContainerWin::OnUIActivate( void )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::GetWindowContext( IOleInPlaceFrame ** ppFrame, 
										  IOleInPlaceUIWindow ** ppDoc, LPRECT lprcPosRect, 
										  LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo )
{
    *ppFrame = this;
    AddRef();

    *ppDoc = NULL;

    CopyRect(lprcPosRect, &m_rectPos);
    CopyRect(lprcClipRect, &m_rectPos);

    lpFrameInfo->cb             = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp        = FALSE;
    lpFrameInfo->hwndFrame      = GetHostWindow();
    lpFrameInfo->haccel         = 0;
    lpFrameInfo->cAccelEntries  = 0;

    return (S_OK);
}

HRESULT CAxContainerWin::Scroll( SIZE scrollExtent )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::OnUIDeactivate( BOOL fUndoable )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::OnInPlaceDeactivate( void )
{
	return S_OK;
}
HRESULT CAxContainerWin::DiscardUndoState( void )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::DeactivateAndUndo( void )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::OnPosRectChange( LPCRECT lprcPosRect )
{
    RECT rcPos, rcClient;
    HWND hwnd;
    GetWindow(&hwnd);
    GetClientRect(hwnd, &rcClient);
    GetWindowRect(hwnd, &rcPos);

    if(rcClient.bottom < lprcPosRect->bottom)
        rcPos.bottom += lprcPosRect->bottom - rcClient.bottom;
    else
        rcPos.bottom -= rcClient.bottom - lprcPosRect->bottom;

    if(rcClient.right < lprcPosRect->right)
        rcPos.right += lprcPosRect->right - rcClient.right; 
    else
        rcPos.right -= rcClient.right - lprcPosRect->right; 

    MoveWindow( hwnd, 
                rcPos.left, 
                rcPos.top, 
                rcPos.right - rcPos.left, 
                rcPos.bottom - rcPos.top,
                TRUE);

    return (S_OK);
}

// *** IOleInPlaceSiteEx Methods ***
HRESULT CAxContainerWin::OnInPlaceActivateEx( BOOL *pfNoRedraw, DWORD dwFlags )
{
    OleLockRunning( m_ptr, TRUE, FALSE );
    HRESULT hr = E_FAIL;

    return S_OK;
}

HRESULT CAxContainerWin::OnInPlaceDeactivateEx( BOOL fNoRedraw )
{
    OleLockRunning( m_ptr, FALSE, FALSE);
    return S_OK;
}

HRESULT CAxContainerWin::RequestUIActivate( void )
{
	return S_OK;
}

// *** IOleInPlaceSiteWindowless Methods ***
HRESULT CAxContainerWin::CanWindowlessActivate( void )
{
    return (TRUE); // m_bCanWindowlessActivate);
}

HRESULT CAxContainerWin::GetCapture( void )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::SetCapture( BOOL fCapture )
{

    if (fCapture)
    {
        ::SetCapture( GetHostWindow() );
        m_bCapture = TRUE;
    }
    else
    {
        ::ReleaseCapture();
        m_bCapture = FALSE;
    }
    return S_OK;
}

HRESULT CAxContainerWin::GetFocus( void )
{
	return S_OK;
}
HRESULT CAxContainerWin::SetFocus( BOOL fFocus )
{
	return S_OK;
}
HRESULT CAxContainerWin::GetDC( LPCRECT pRect, DWORD grfFlags, HDC *phDC )
{
    if (!phDC)
        return E_POINTER;

    *phDC = ::GetDC(GetHostWindow());
    return S_OK;
}
HRESULT CAxContainerWin::ReleaseDC( HDC hDC )
{
    ::ReleaseDC(GetHostWindow(), hDC);
    return S_OK;
}
HRESULT CAxContainerWin::InvalidateRect( LPCRECT pRect, BOOL fErase )
{
    ::InvalidateRect(GetHostWindow(), pRect, fErase);
    return S_OK;
}
HRESULT CAxContainerWin::InvalidateRgn( HRGN hRGN, BOOL fErase )
{
    ::InvalidateRgn(GetHostWindow(), hRGN, fErase);
    return S_OK;
}
HRESULT CAxContainerWin::ScrollRect( INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip )
{
	return S_OK;
}
HRESULT CAxContainerWin::AdjustRect( LPRECT prc )
{
	return S_OK;
}
HRESULT CAxContainerWin::OnDefWindowMessage( UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult )
{
    *plResult = ::DefWindowProc(GetHostWindow(), msg, wParam, lParam);
    return S_OK;
}
// *** IOleInPlaceUIWindow Methods ***
HRESULT CAxContainerWin::GetBorder( LPRECT lprectBorder )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::RequestBorderSpace( LPCBORDERWIDTHS lpborderwidths )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::SetBorderSpace( LPCBORDERWIDTHS lpborderwidths )
{
    /*if(m_punkOuter)
        return ((CAxContainerWin*)m_punkOuter)->SetBorderSpace(lpborderwidths);*/

	//???
	return S_OK;
}
HRESULT CAxContainerWin::SetActiveObject( IOleInPlaceActiveObject * pActiveObject,
					LPCOLESTR lpszObjName )
{
	//return S_OK;
	return E_NOTIMPL;
}
// *** IOleInPlaceFrame Methods ***
HRESULT CAxContainerWin::InsertMenus( HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::SetMenu( HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::RemoveMenus( HMENU hmenuShared )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::SetStatusText( LPCOLESTR pszStatusText )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::EnableModeless( BOOL fEnable )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::TranslateAccelerator( LPMSG lpmsg, WORD wID )
{
	//return S_OK;
	return E_NOTIMPL;
}
// *** IOleControlSite Methods ***
HRESULT CAxContainerWin::OnControlInfoChanged( void )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::LockInPlaceActive( BOOL fLock )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::GetExtendedControl( IDispatch **ppDisp )
{
    if (ppDisp == NULL)
        return (E_INVALIDARG);

    *ppDisp = (IDispatch *)this;
    AddRef();

    return (S_OK);
}
HRESULT CAxContainerWin::TransformCoords( POINTL *pptlHimetric, POINTF *pptfContainer, DWORD dwFlags )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::TranslateAccelerator( LPMSG pMsg, DWORD grfModifiers )
{
	return (S_FALSE);
}
HRESULT CAxContainerWin::OnFocus( BOOL fGotFocus )
{
	//return S_OK;
	return E_NOTIMPL;
}
HRESULT CAxContainerWin::ShowPropertyFrame( void )
{
	//return S_OK;
	return E_NOTIMPL;
}

HWND CAxContainerWin::GetHostWindow()
{
	::GetParent( m_hwnd );
	return m_hwnd;
}

LRESULT CAxContainerWin::DoGetControl()
{
	IUnknown	*punk = m_ptr;
	return (DWORD)punk;
}