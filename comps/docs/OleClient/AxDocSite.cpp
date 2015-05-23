// AxDocSite.cpp: implementation of the CAxDocSite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AxDocSite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAxDocSite::CAxDocSite( CAxDocContainer* pContainer )
{
	m_pContainer = pContainer;
}

//////////////////////////////////////////////////////////////////////
CAxDocSite::~CAxDocSite()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CAxDocSite::DoGetInterface( const IID &iid )
{
	if( iid == IID_IOleClientSite )
	{
		return (IOleClientSite*)this;
	}
	else
	if( iid == IID_IAdviseSink )
	{
		return (IAdviseSink*)this;
	}		
	else
	if( iid == IID_IOleInPlaceSite )
	{
		return (IOleInPlaceSite*)this;
	}		
	else
	if( iid == IID_IOleDocumentSite )
	{
		return (IOleDocumentSite*)this;
	}		
	else 
		return ComObjectBase::DoGetInterface( iid );
}
