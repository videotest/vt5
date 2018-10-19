#include "stdafx.h"
#include "com_main.h"
#include "com_unknown.h"

//com object base
ComObjectBase::ComObjectBase()
{
	m_nRefCounter = 1;
	m_bLockApp = App::instance() != 0;
	if( m_bLockApp )
		App::instance()->AddRef();
}

ComObjectBase::~ComObjectBase()
{
	if( m_bLockApp && App::instance() )
		App::instance()->Release();
	dbg_assert( m_nRefCounter == 0 );
}

HRESULT ComObjectBase::_qi( const IID &iid, void **pret )
{
	IUnknown	*punk = 0;
	if( iid == IID_IUnknown )
		punk = (IUnknown*)this;
	if( !punk )
		punk = DoGetInterface( iid );
	if( punk )
	{
		punk->AddRef();
		(*pret) = punk;
		return S_OK;
	}
	
	return DoQueryAggregates( iid, pret );
}


//arggr
HRESULT ComAggregableBase::_qi( const IID &iid, void **pret )
{
	if( m_punkOuter )
	{
		if( iid == IID_IUnknown )
		{
			*pret = m_punkOuter;
			m_punkOuter->AddRef();
			return S_OK;
		}
		HRESULT h = ComObjectBase::_qi( iid, pret );
		if( h == S_OK )return S_OK;
		return m_punkOuter->QueryInterface( iid, pret );
	}
	else
		return ComObjectBase::_qi( iid, pret );
}
HRESULT ComAggregableBase::_qi_own( const IID &iid, void **pret )
{
	if( m_punkOuter )
	{
		*pret = 0;
		if( iid == IID_IUnknown )
		{
			m_punkInner->AddRef();
			*pret = m_punkInner;
			return S_OK;
		}
	}
	return ComObjectBase::_qi( iid, pret );
}