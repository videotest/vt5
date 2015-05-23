#include "stdafx.h"
#include "sewLI_page.h"
#include "sewLI.h"




CSewLIPropertyPage::CSewLIPropertyPage()
{
	
}

CSewLIPropertyPage::~CSewLIPropertyPage()
{
	
}

CWinImpl *CSewLIPropertyPage::GetTarget()
{
	return this;
}

IUnknown *CSewLIPropertyPage::DoGetInterface( const IID &iid )
{
	IUnknown	*punk = DoGetDlgInterface( iid );
	if( punk )return punk;
	else if( iid == IID_IPersist )return (IPersist*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT CSewLIPropertyPage::GetClassID( CLSID *pclsid )
{
	*pclsid = clsidSewLIPage;
	return S_OK;
}

bool CSewLIPropertyPage::DoInitDialog()
{
	//cleanup if it is not first call
	UnAdviseAll();
	return true;
}

void CSewLIPropertyPage::DoCommand( UINT nCmd )
{
}