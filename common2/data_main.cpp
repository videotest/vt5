#include "stdafx.h"
#include "data_main.h"
#include "aliaseint.h"
#include "misc_utils.h"
#include "guardint.h"

void FreeObject(void *p)
{
	((IUnknown *)p)->Release();
}


//common

CObjectBase::CObjectBase()
{
	m_dwFlags = nofHasData;
	m_pinfo = 0;
	m_bModified = false;
	m_keyBase = m_key;

	m_punkNamedData = 0;
	m_punkAggrNamedData = 0;

	m_punkParent = NULL;
}

CObjectBase::~CObjectBase()
{
	if( m_pinfo )
		m_pinfo->Release();
	if( m_punkAggrNamedData )
		m_punkAggrNamedData->Release();
	m_punkAggrNamedData = 0;
}

IUnknown *CObjectBase::DoGetInterface( const IID &iid )
{
	if( iid == IID_INamedDataObject )		return (INamedDataObject*)this;
	else if( iid == IID_INamedDataObject2 )	return (INamedDataObject2*)this;
	else if( iid == IID_INamedObject2 )		return (INamedObject2*)this;
	else if( iid == IID_INumeredObject )	return (INumeredObject*)this;
	else if( iid == IID_ISerializableObject )	return (ISerializableObject*)this;
	else if( iid == IID_IPersist )				return (IPersist*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT CObjectBase::DoQueryAggregates(const IID &iid, void **ppRet)
{
	if (m_punkAggrNamedData)
		return m_punkAggrNamedData->QueryInterface(iid, ppRet);
	else
		return E_NOINTERFACE;
}

HRESULT CObjectBase::SetTypeInfo( INamedDataInfo *pinfo )
{
	if( m_pinfo )m_pinfo->Release();
	m_pinfo = pinfo;
	if( m_pinfo )m_pinfo->AddRef();

	return S_OK;

}

HRESULT CObjectBase::GetType( BSTR *pbstrType )
{
	INamedObject2Ptr	ptrN( m_pinfo );
	return ptrN->GetName( pbstrType );
}

HRESULT CObjectBase::GetDataInfo( IUnknown **ppunk )
{
	*ppunk = m_pinfo;
	if( *ppunk )(*ppunk)->AddRef();
	return S_OK;
}
HRESULT CObjectBase::IsModified( BOOL *pbModified )
{
	*pbModified = m_bModified;
	return S_OK;
}
HRESULT CObjectBase::SetModifiedFlag( BOOL bSet )
{
	m_bModified = bSet;
	return S_OK;
}


//lighty object base implementation
HRESULT CObjectBase::CreateChild( IUnknown **ppunkChild )
{	return E_NOTIMPL;	}

HRESULT CObjectBase::GetParent( IUnknown **ppunkParent )
{	
	*ppunkParent = m_punkParent;
	if (*ppunkParent)
		(*ppunkParent)->AddRef();
	return S_OK;	
}

HRESULT CObjectBase::SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ )
{
	if (m_punkParent)
	{
		INamedDataObject2Ptr sptrParent(m_punkParent);
		if (sptrParent != 0)
			sptrParent->RemoveChild(Unknown());
		m_punkParent = NULL;
	}
	if (punkParent)
	{
		INamedDataObject2Ptr sptrParent(punkParent);
		if (sptrParent != 0)
			sptrParent->AddChild(Unknown());
		m_punkParent = punkParent;
	}

	return S_OK;
}

HRESULT CObjectBase::StoreData( DWORD dwDirection )
{	return E_NOTIMPL;	}

TPOS CObjectBase::FindChildPos(IUnknown *punkFind)
{
	GuidKey keyFind = ::GetKey(punkFind);
	TPOS pos = m_listChilds.head();
	while (pos)
	{
		IUnknown *punkChild = m_listChilds.next(pos);
		if (::GetKey(punkChild) == keyFind)
			return pos;
	}
	return 0;
}


//for internal usage - it is called only from framework
HRESULT CObjectBase::RemoveChild( IUnknown *punkChild )
{
	TPOS pos = FindChildPos(punkChild);
	if (pos != 0)
		m_listChilds.remove(pos);
	return S_OK;
}

HRESULT CObjectBase::AddChild( IUnknown *punkChild )
{
	TPOS pos = FindChildPos(punkChild);
	if (pos == 0)
	{
		punkChild->AddRef();
		m_listChilds.insert(punkChild);
	}
	return S_OK;
}

HRESULT CObjectBase::GetChildsCount( long *plCount )
{	
	*plCount = m_listChilds.Count();
	return S_OK;	
}

HRESULT CObjectBase::GetFirstChildPosition(TPOS *plPos)
{	
	*plPos = m_listChilds.head();
	return S_OK;
}

HRESULT CObjectBase::GetNextChild(TPOS *plPos, IUnknown **ppunkChild)
{	
	if( ppunkChild && plPos )
	{
		*ppunkChild = m_listChilds.next(*plPos);
		if (*ppunkChild)
			(*ppunkChild)->AddRef();
	}

	return S_OK;
}

HRESULT CObjectBase::GetLastChildPosition(TPOS *plPos)
{	
	*plPos = m_listChilds.tail();
	return S_OK;
}

HRESULT CObjectBase::GetPrevChild(TPOS *plPos, IUnknown **ppunkChild)
{	
	if( ppunkChild && plPos)
	{
		*ppunkChild = m_listChilds.prev(*plPos);
		if (*ppunkChild)
			(*ppunkChild)->AddRef();
	}

	return S_OK;
}

HRESULT CObjectBase::AttachData( IUnknown *punkNamedData )
{	
	m_punkNamedData = punkNamedData;

	return S_OK;	
}
HRESULT CObjectBase::GetObjectFlags( DWORD *pdwObjectFlags )
{	
	*pdwObjectFlags = m_dwFlags;
	return S_OK;	
}

HRESULT CObjectBase::SetActiveChild(TPOS lPos)
{	return E_NOTIMPL;	}
HRESULT CObjectBase::GetActiveChild(TPOS *plPos)
{	
	*plPos = 0;
	return S_OK;	
}
HRESULT CObjectBase::SetUsedFlag( BOOL bSet )
{	
	if( bSet )m_dwFlags |= nofStoreByParent;
	else m_dwFlags &= ~nofStoreByParent;

	return S_OK;	
}
HRESULT CObjectBase::SetHostedFlag( BOOL bSet )
{	
	if( bSet )m_dwFlags |= nofHasHost;
	else m_dwFlags &= ~nofHasHost;
	return E_NOTIMPL;	
}
HRESULT CObjectBase::GetData(IUnknown **ppunkNamedData )
{	
	if( !ppunkNamedData )
		return E_INVALIDARG;

	*ppunkNamedData = m_punkNamedData;

	if( m_punkNamedData )
		m_punkNamedData->AddRef( );

	return S_OK;	
}

HRESULT CObjectBase::GetObjectPosInParent(TPOS *plPos)
{	
	*plPos = 0;
	return S_OK;
}
HRESULT CObjectBase::SetObjectPosInParent(TPOS lPos)
{	
	return E_NOTIMPL;	
}

HRESULT CObjectBase::GetChildPos(IUnknown *punkChild, TPOS *plPos)
{	
	*plPos = 0;
	return E_NOTIMPL;	
}
HRESULT CObjectBase::InitAttachedData()
{
	if( m_punkAggrNamedData )
		return S_OK;
	_bstr_t	bstr( "Data.NamedData" );
	CLSID	clsidNamedData;
	if( ::CLSIDFromProgID( bstr, &clsidNamedData ) )
		return E_FAIL;
	::CoCreateInstance( clsidNamedData, (ComObjectBase*)this, CLSCTX_INPROC, 
		IID_IUnknown, (void **)&m_punkAggrNamedData );
	INamedDataPtr ptrND( (ComObjectBase*)this );
	if( ptrND )
		ptrND->EnableBinaryStore( TRUE );	
	return S_OK;
}


// base and source key functionality
HRESULT CObjectBase::GetBaseKey(GUID * pBaseKey)
{
	*pBaseKey = m_keyBase;
	return S_OK;
}
HRESULT CObjectBase::SetBaseKey(GUID * pBaseKey)
{
	GuidKey prevKey = m_keyBase;

	m_keyBase = *pBaseKey;

	//[paul]27.03.2002
	if( m_punkNamedData )
	{
		INamedDataPtr ptrData( m_punkNamedData );
		if( ptrData )
			ptrData->NotifyContexts( ncChangeBase, Unknown(), 0, (LPARAM)&prevKey );
	}


	return S_OK;
}

HRESULT CObjectBase::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = true;
	return S_OK;
}


//ISerializableObject
HRESULT CObjectBase::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	int nUserNameLen = 0;
	int nNameLen = 0;
	char* pUserName = 0;
	char* pName = 0;

	ULONG nRead = 0;
	int nVersion = 0;

	pStream->Read(&nUserNameLen, sizeof(int), &nRead);
	if(nUserNameLen < 0)
	{
		pStream->Read(&nVersion, sizeof(int), &nRead);
		pStream->Read(&nUserNameLen, sizeof(int), &nRead);
	}
	if(nUserNameLen > 0)
	{
		pUserName = new char[nUserNameLen+1];
		pStream->Read(pUserName, nUserNameLen, &nRead);
		pUserName[nUserNameLen] = '\0';
		m_bstrUserName = pUserName;
		delete pUserName;
	}

	pStream->Read(&nNameLen, sizeof(int), &nRead);
	if(nNameLen > 0)
	{
		pName = new char[nNameLen+1];
		pStream->Read(pName, nNameLen, &nRead);
	    pName[nNameLen] = '\0';
		m_bstrName = pName;
		delete pName;
	}

	GuidKey guidPrev = m_key;
	GuidKey guidPrevBase = m_keyBase;

	pStream->Read( &m_key, sizeof( m_key ), &nRead );
	pStream->Read( &m_keyBase, sizeof( m_keyBase ), &nRead );

	if (nVersion > 0)
	{
		int nNameData = 0;
		pStream->Read(&nNameData, sizeof(int), &nRead);
		if (nNameData > 0)
		{
			InitAttachedData();
			SerializeParams	params;
			params.flags = pparams?pparams->flags:0;
			params.punkNamedData = (INamedData*)m_punkAggrNamedData;
			params.punkCurrentParent = 0;
			INamedDataPtr ptrND( m_punkAggrNamedData );
			ptrND->DeleteEntry(0);
			ISerializableObjectPtr	ptrS( m_punkAggrNamedData );
			ptrS->Load( pStream, &params );
		}
	}

	//paul 28.03.2002
	if( m_punkNamedData )
	{
		INamedDataPtr ptrData( m_punkNamedData );
		if( ptrData )
		{
			ptrData->NotifyContexts( ncChangeKey, Unknown(), 0, (LPARAM)&guidPrev );
			ptrData->NotifyContexts( ncChangeBase, Unknown(), 0, (LPARAM)&guidPrevBase );
		}
	}
	
	return S_OK;
}

HRESULT CObjectBase::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	int nSign = -1;
	int nVersion = 1;
	ULONG nWritten = 0;
	pStream->Write(&nSign, sizeof(int), &nWritten);
	pStream->Write(&nVersion, sizeof(int), &nWritten);

	int nUserNameLen = m_bstrUserName.length();
	int nNameLen = m_bstrName.length();

	pStream->Write(&nUserNameLen, sizeof(int), &nWritten);
	if(nUserNameLen)
		pStream->Write((char*)m_bstrUserName, nUserNameLen, &nWritten);
	
	pStream->Write(&nNameLen, sizeof(int), &nWritten);
	if(nNameLen)
		pStream->Write((char*)m_bstrName, nNameLen, &nWritten);

	pStream->Write( &m_key, sizeof( m_key ), &nWritten );
	pStream->Write( &m_keyBase, sizeof( m_keyBase ), &nWritten );

	int nNamesData = m_punkAggrNamedData==0?0:1;
	pStream->Write(&nNamesData, sizeof(int), &nWritten);
	if (m_punkAggrNamedData)
	{
		SerializeParams	params;
		params.flags = pparams?pparams->flags:0;
		params.punkNamedData = (INamedData*)m_punkAggrNamedData;
		params.punkCurrentParent = 0;
		ISerializableObjectPtr	ptrS( m_punkAggrNamedData );
		ptrS->Store( pStream, &params );
	}

	return S_OK;
}

//IPersist
HRESULT CObjectBase::GetClassID( CLSID* pClassID )
{
	if( !pClassID )
		return E_INVALIDARG;
	GuidKey guidInternal = GetInternalClassID();

	IVTApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp )
	{
		GuidKey guidExternal = INVALID_KEY;
		if( S_OK != ptrApp->GetEntry( 1, (DWORD*)&guidInternal, (DWORD*)&guidExternal, 0 ) )
			guidExternal = INVALID_KEY;

		memcpy( pClassID, &guidExternal, sizeof(GUID) );
	}
	

	return S_OK;
}


//CDataInfoBase
CDataInfoBase::CDataInfoBase( const CLSID &clsid, const char *pszTypeName, const char *pszContainerTypeName, UINT nIcon )
{
	m_bstrName = pszTypeName;
	
	if( pszContainerTypeName )
		m_bstrContainer = pszContainerTypeName;
	
	m_dwContainerState = (DWORD)(-1);	// [vanek] 24.12.2003
	m_nIconID = nIcon;
	m_clsid = clsid;
}

IUnknown *CDataInfoBase::DoGetInterface( const IID &iid )
{
	if( iid == IID_INamedDataInfo )		return (INamedDataInfo*)this;
	if( iid == IID_INamedObject2 )		return (INamedObject2*)this;	
	if( iid == IID_INamedDataInfo2 )		return (INamedDataInfo2*)this; // [vanek] 24.12.2003
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT CDataInfoBase::CreateObject( IUnknown **ppunk )
{
	INamedDataObject	*pnamed = 0;
	HRESULT hResult = ::CoCreateInstance( m_clsid, 0,  1, IID_INamedDataObject, (void**)&pnamed );
	if( hResult )return hResult;
	pnamed->SetTypeInfo( this );
	*ppunk = pnamed;
	return S_OK;

}
HRESULT CDataInfoBase::GetName( BSTR *pbstrName )
{
	*pbstrName = m_bstrName.copy();
	return S_OK;
}
HRESULT CDataInfoBase::GetContainerType( BSTR *pbstrTypeName )
{
	*pbstrTypeName = m_bstrContainer.copy();
	return S_OK;
}
HRESULT CDataInfoBase::GetProgID( BSTR *pbstrProgID )
{
	return ::ProgIDFromCLSID( m_clsid, pbstrProgID );
}

HRESULT CDataInfoBase::GetObjectIcon(HICON* phIcon, DWORD dwFlags )
{
	HICON	h = 0;
	if( dwFlags & oif_SmallIcon )
		h = (HICON)::LoadImage( App::handle(), MAKEINTRESOURCE(m_nIconID), IMAGE_ICON, 16, 16, 0 );
	if( !h )
		h = (HICON)::LoadImage( App::handle(), MAKEINTRESOURCE(m_nIconID), IMAGE_ICON, 32, 32, 0 );

	*phIcon = h;
	return (*phIcon)?S_OK:S_FALSE;
}

HRESULT CDataInfoBase::GetUserName( BSTR *pbstr )
{
	IAliaseManPtr	ptrAliase( GetAppUnknown() );
	if( ptrAliase != 0 )
		if( ptrAliase->GetObjectString( m_bstrName, pbstr ) == S_OK )
			return S_OK;

	*pbstr = m_bstrName.copy();
	return S_OK;
}

// [vanek] 24.12.2003
HRESULT CDataInfoBase::GetContainerState( DWORD *pdwState )
{
    if( !pdwState )	
		return S_FALSE;

	if( m_dwContainerState == (DWORD)(-1) )
	{
		_bstr_t bstrContainerKey = m_bstrContainer.length() ? m_bstrContainer : m_bstrName;

		m_dwContainerState = cntsEnabled;	
		if( bstrContainerKey.length() )
			m_dwContainerState = (DWORD)::GetValueInt( GetAppUnknown(), szContaintersStatesSection, bstrContainerKey, cntsEnabled );
	}

	*pdwState = m_dwContainerState;
	return S_OK;
}