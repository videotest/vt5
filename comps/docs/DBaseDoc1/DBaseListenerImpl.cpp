// DBaseListenerImpl.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBaseListenerImpl.h"


/////////////////////////////////////////////////////////////////////////////
// CDBaseEventController
CDBaseEventController::CDBaseEventController()
{
	m_listeners.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
CDBaseEventController::~CDBaseEventController()
{
	m_listeners.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEventController::RegisterListener( IUnknown* punk )
{
	if( punk && CheckInterface( punk, IID_IDBaseListener ))
	{
		ASSERT( !m_listeners.Find(punk) );

		if( CheckInterface( punk, IID_IDBaseDocument ) )
			m_listeners.AddHead( punk );//must recieve first
		else
			m_listeners.AddTail( punk );
	}
	else
	{
		ASSERT( FALSE );
		return false;
	}

	return true;
	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEventController::UnRegisterListener( IUnknown* punk )
{
	POSITION pos = m_listeners.Find( punk );
	ASSERT( pos );

	if( pos )
		m_listeners.RemoveAt( pos );		
	else
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseEventController::FireEvent(const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	POSITION	pos = m_listeners.GetHeadPosition();

	while( pos )
	{
		IUnknown *punk = (IUnknown *)m_listeners.GetNext( pos );
		

		sptrIDBaseListener spDBaseListener( punk );
		if( punk != punkObject && spDBaseListener != NULL )
		{			
			_bstr_t bstrEvent(pszEvent);
			spDBaseListener->OnNotify( bstrEvent, punkObject, punkDBaseDocument, bstrTableName, bstrFieldName, var );
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// CDBaseListenerImpl

IMPLEMENT_UNKNOWN_BASE(CDBaseListenerImpl, DBaseListener);

CDBaseListenerImpl::CDBaseListenerImpl()
{
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseListenerImpl::XDBaseListener::OnNotify( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var )
{
	METHOD_PROLOGUE_BASE(CDBaseListenerImpl, DBaseListener)	
	_variant_t	_var( var );	
	CString	str( bstrEvent );
	pThis->OnDBaseNotify( str, punkObject, punkDBaseDocument, bstrTableName, bstrFieldName, _var );
	
	return S_OK;
}

