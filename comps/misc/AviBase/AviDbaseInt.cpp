#include "stdafx.h"

#include "AviObj.h"
#include "AviBase.h"

#include "aliaseint.h"
#include "misc_utils.h"

#include "resource.h"

//////////////////////////////////////////////////////////////////////
long CAviObj::FindChildPos( IUnknown* punkChild )
{	
	long lPos = m_listChilds.head();
	while( lPos )
	{
		long lSave = lPos;
		if( punkChild == m_listChilds.next( lPos ) )
			return lSave;			
	}

	return 0;	
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::CreateChild( IUnknown **ppunkChild )
{

	*ppunkChild = ::CreateTypedObject( _bstr_t(szTypeImage) );

	if( *ppunkChild )
	{
		INamedDataObject2Ptr ptrN( *ppunkChild );
		if( ptrN )
			ptrN->SetParent( Unknown(), apfNotifyNamedData|apfAttachParentData );
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetParent( IUnknown **ppunkParent )
{
	*ppunkParent = 0;

	if( m_ptrParent )
	{
		m_ptrParent->AddRef();
		*ppunkParent = m_ptrParent;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ )
{
	/*
	DWORD	dwThisObjectFlags = 0;
	GetObjectFlags( &dwThisObjectFlags );

	bool bNotifyNamedData = (dwFlags & apfNotifyNamedData) != 0 &&
							(dwThisObjectFlags & nofStoreByParent) == 0;

	if( punkParent != 0 && ( ::GetKey( m_ptrParent ) == ::GetKey( punkParent ) ) )
		return FALSE;

	if( m_ptrParent != 0)
	{
		//ExchangeDataWithParent( dwFlags );
		m_ptrParent->RemoveChild( Unknown() );
	}

	if( m_ptrNamedData != 0 && bNotifyNamedData )
	{
		INamedDataPtr	ptrN( m_ptrNamedData );
		if (ptrN != 0)
			ptrN->NotifyContexts( ncRemoveObject, Unknown(), 0, 0);
	}

	if( punkParent )
	{
		IUnknown* punk = 0;
		punkParent->QueryInterface( IID_INamedDataObject2, (void**)&punk );

		if( punk )
		{
			m_ptrParent = punk;
			punk->Release();	punk = 0;
		}
	}
	else
		m_ptrParent = 0;

	if( m_ptrNamedData != 0 && bNotifyNamedData )
	{
		sptrINamedData	sptrN( m_ptrNamedData );
		if (sptrN != 0)
			sptrN->NotifyContexts( ncAddObject, Unknown(), 0, 0);
	}

	if( m_ptrParent )
	{
		m_ptrParent->AddChild( Unknown() );
		
		//if( !ExchangeDataWithParent( dwFlags ) )
		//	return E_INVALIDARG;
	}
	*/

	return E_NOTIMPL;
}


//////////////////////////////////////////////////////////////////////
//for internal usage - it is called only from framework
HRESULT CAviObj::RemoveChild( IUnknown *punkChild )
{
	long pos = FindChildPos( punkChild );
	if( !pos )
		return E_FAIL;

	IUnknown* p = m_listChilds.get( pos );
	m_listChilds.remove( pos );	

	INamedDataObject2Ptr sptr( punkChild );
	
	if (sptr != 0)
		sptr->SetObjectPosInParent( (long)0 );	

	if( !m_bDummyStub )
		punkChild->Release();
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::AddChild( IUnknown *punkChild )
{	
	if( !punkChild )
		return E_INVALIDARG;

	long pos = m_listChilds.insert( punkChild );
	punkChild->AddRef();
	

	INamedDataObject2Ptr sptr( punkChild );
	
	if (sptr != 0)
		sptr->SetObjectPosInParent( (long)pos );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetChildsCount( long *plCount )
{
	long lCount = 0;

	long lPos = m_listChilds.head();
	while( lPos )
	{		
		m_listChilds.next( lPos );
		lCount++;		
	}

	*plCount = lCount;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetFirstChildPosition( long *plPos )
{
	*plPos = m_listChilds.head();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetNextChild( long *plPos, IUnknown **ppunkChild )
{
	if( !plPos || !ppunkChild )
		return E_INVALIDARG;

	long pos = *plPos;
	*ppunkChild = m_listChilds.next( pos );

	if(	*ppunkChild )
		(*ppunkChild)->AddRef();

	*plPos = pos;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetLastChildPosition(long *plPos)
{
	*plPos = m_listChilds.tail();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetPrevChild(long *plPos, IUnknown **ppunkChild)
{
	if( !plPos || !ppunkChild )
		return E_INVALIDARG;

	long pos = *plPos;
	*ppunkChild = m_listChilds.prev( pos );

	if( *ppunkChild)
		(*ppunkChild)->AddRef();

	*plPos = pos;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::SetActiveChild( long lPos )
{
	m_posActiveChild = lPos;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetActiveChild( long *plPos )
{
	*plPos = m_posActiveChild;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetObjectPosInParent( long *plPos ) // return object position in parent's object child list
{
	*plPos = m_posInParent;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::SetObjectPosInParent( long lPos )
{
	m_posInParent = lPos;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetChildPos(IUnknown *punkChild, long *plPos)
{
	*plPos = FindChildPos( punkChild );

	return S_OK;
}

void CAviObj::_DetachChildParent()
{
	long lPos = m_listChilds.head();
	while( lPos )
	{		
		IUnknown *punkChild = m_listChilds.next( lPos ); //��� AddRef
		INamedDataObject2Ptr ptrChild( punkChild );
		ptrChild->SetParent( 0, apfNotifyNamedData|sdfCopyParentData );
	}

	assert(m_listChilds.head() == 0 ); //���������, ��� ������ ���� - jfc

	if( m_ptrParent != 0 )
		m_ptrParent->RemoveChild( Unknown() );
	m_ptrParent = 0;
	m_posInParent = 0;
}
