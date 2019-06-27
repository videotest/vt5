#include "stdafx.h"
#include "impl_event.h"


CEventListenerImpl::CEventListenerImpl()		
{
	m_bInsideNotify = false;
	m_nRegisterCount = 0;
}

CEventListenerImpl::~CEventListenerImpl()
{
}


void CEventListenerImpl::Register( IUnknown *punkNC, bool bRegister, char *pszReserved )
{
	INotifyControllerPtr	ptrNC( punkNC );
	_bstr_t	bstr( pszReserved );
	if(ptrNC)
	if( bRegister )
		ptrNC->RegisterEventListener( bstr, this );
	else
		ptrNC->UnRegisterEventListener( bstr, this );

	m_nRegisterCount += bRegister?+1:-1;
}

HRESULT CEventListenerImpl::IsInNotify(long *pbFlag)
{
	*pbFlag  =m_bInsideNotify;
	return S_OK;
}

HRESULT CEventListenerImpl::Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize )
{
	_bstr_t	bstr( szEventDesc );
	bool	bOldInside = m_bInsideNotify;
	m_bInsideNotify = true;

	OnNotify( bstr, pHint, pFrom, pdata, cbSize );

	m_bInsideNotify = bOldInside;

	return S_OK;
}