#include "stdafx.h"
#include "win_page.h"

//win_page
CPageImpl::CPageImpl() : m_size( 100, 100 )
{
	m_bstrCaption = "Unknown";
}

CPageImpl::~CPageImpl()
{
}

HRESULT CPageImpl::GetSize( SIZE *psize )
{
	psize->cx = 100;
	psize->cx = 20;
	if( m_pTemplate == 0 )
		LoadTemplate();
	if( m_pTemplate == 0 )
		return S_FALSE;

	psize->cx = m_pTemplate->cx;
	psize->cy = m_pTemplate->cy;

	return S_OK;
}
HRESULT CPageImpl::GetCaption( BSTR *pbstrCaption )
{
	*pbstrCaption = m_bstrCaption.copy();
	return S_OK;
}
HRESULT CPageImpl::OnSetActive()
{
	return S_OK;
}
HRESULT CPageImpl::LoadSettings()
{
	UpdateControls();
	return S_OK;
}
HRESULT CPageImpl::StoreSettings()
{
	return S_OK;
}

IUnknown	*CPageImpl::DoGetDlgInterface( const IID &iid )
{
	if( iid == IID_IOptionsPage )return (IOptionsPage*)this;
	else	return DoGetWinInterface( iid );
}
