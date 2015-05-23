// GalleryAviPage.cpp: implementation of the CGalleryAviPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GalleryAviPage.h"
#include "AviBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGalleryAviPage::CGalleryAviPage()
{
	m_bstrCaption = "AVI Info";
	m_bstrName		= "AVI Gallery";
	m_bstrUserName	= "AVI Gallery";
}

//////////////////////////////////////////////////////////////////////
CGalleryAviPage::~CGalleryAviPage()
{
	UnAdviseAll();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CWinImpl* CGalleryAviPage::GetTarget()
{
	return this;
}

//////////////////////////////////////////////////////////////////////
HRESULT CGalleryAviPage::GetClassID( CLSID *pclsid )
{
	*pclsid = clsidAviGalleryPage;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CGalleryAviPage::ReadInfo( IUnknown* punkAvi )
{
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CGalleryAviPage::DoGetInterface( const IID &iid )
{
	IUnknown	*punk = DoGetDlgInterface( iid );
	if( punk )return punk;
	else if( iid == IID_IPersist )return (IPersist*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;
	else if( iid == IID_IAviGalleryPropPage )return (IAviGalleryPropPage*)this;
	else if( iid == IID_INamedObject2 )return (INamedObject2*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
bool CGalleryAviPage::DoInitDialog()
{
	//cleanup if it is not first call
	UnAdviseAll();

	return true;
}


