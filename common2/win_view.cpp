#include "stdafx.h"
#include "com_main.h"
#include "win_view.h"
#include "nameconsts.h"

CWinViewBase::CWinViewBase()
{
	m_punkDocument = 0;
}

CWinViewBase::~CWinViewBase()
{
	if( m_punkDocument )
	{
		Register( m_punkDocument, false, szEventActiveObjectChange );
		m_punkDocument = 0;
	}
}

void CWinViewBase::DoAttachObjects()
{
	if( m_punkDocument )m_punkDocument->Release();
}


IUnknown *CWinViewBase::DoGetInterface( const IID &iid )
{
	if( iid == IID_IView )					return (IView*)this;
	else if( iid == IID_IEventListener )	return (IEventListener*)this;
	else if( iid == IID_IWindow )			return (IWindow*)this;
	else if( iid == IID_IWindow2 )			return (IWindow2*)this;
	else return ComAggregableBase::DoGetInterface( iid );
}

//IView implementation
HRESULT CWinViewBase::Init( IUnknown *punkDoc, IUnknown *punkSite )
{
	dbg_assert( m_punkDocument == 0 );
	punkDoc->QueryInterface( IID_IDocument, (void ** )&m_punkDocument );
	Register( punkDoc, true, szEventActiveObjectChange );
	return S_OK;
}

HRESULT CWinViewBase::GetDocument( IUnknown **ppunkDoc )
{
	(*ppunkDoc) = m_punkDocument;
	if( *ppunkDoc ) (*ppunkDoc)->AddRef();
	return S_OK;
}

HRESULT CWinViewBase::OnUpdate( BSTR bstr, IUnknown *punkObject )
{
	DoAttachObjects();
	return S_OK;
}
HRESULT CWinViewBase::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	*pdwMatch = mvNone;
	return S_OK;
}

HRESULT CWinViewBase::GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags )
{
	*pdwFlags = ofNormal;
	return S_OK;
}

HRESULT CWinViewBase::DoDragDrop( DWORD *pdwDropEffect )
{
	*pdwDropEffect = DROPEFFECT_NONE;
	return S_OK;
}

HRESULT CWinViewBase::GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame)
{
	*ppunkObject = 0;
	return E_NOTIMPL;
}

HRESULT CWinViewBase::MoveDragFrameToFrame()
{
	return E_NOTIMPL;
}

HRESULT CWinViewBase::GetDroppedDataObject(IUnknown** punkDO)
{
	*punkDO = 0;
	return E_NOTIMPL;
}

HRESULT CWinViewBase::OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
{
	return S_OK;
}

HRESULT CWinViewBase::GetFirstVisibleObjectPosition( long *plpos )
{
	(*plpos) = m_listObjects.head();
	return S_OK;
}

HRESULT CWinViewBase::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{
	IUnknown	*punkNext = m_listObjects.next( *plPos );
	if( ppunkObject )
	{
		punkNext->AddRef();
		(*ppunkObject) = punkNext;
	}
	return S_OK;
}

LRESULT	CWinViewBase::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	return CWinImpl::DoMessage( nMsg, wParam, lParam );
}

//message cracker prototipes
LRESULT	CWinViewBase::DoCreate( CREATESTRUCT *pcs )
{
	if( CWinImpl::DoCreate( pcs ) == -1 )
		return -1;

	DoAttachObjects();

	return 0;
}

void CWinViewBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	/*OutputDebugString( pszEvent );
	OutputDebugString( "\n" );
	
	if( !strcmp( pszEvent, szEventActiveObjectChange ) )
	{
		DoAttachObjects();
	}*/
}