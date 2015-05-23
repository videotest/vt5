#include "StdAfx.h"
#include "resource.h"
#include "idiodbasedataobject.h"
#include "idio_int.h"
#include "globals.h"

char	*CIdioDBaseDataObject::m_szType = szArgumentTypeIdioDBase;

IMPLEMENT_DYNCREATE(CIdioDBaseDataObject, CCmdTargetEx)

// {885EC6DE-1BCD-468d-BC0B-11D958200CA1}
GUARD_IMPLEMENT_OLECREATE(CIdioDBaseDataObject, "IdioDBase.IdioDBDataObject",
0x885ec6de, 0x1bcd, 0x468d, 0xbc, 0xb, 0x11, 0xd9, 0x58, 0x20, 0xc, 0xa1);

// {1B0E9CE0-C645-4a8f-BC7C-46AA394AE7E1}
static const GUID clsidCIdioDBDataObjectInfo = 
{ 0x1b0e9ce0, 0xc645, 0x4a8f, { 0xbc, 0x7c, 0x46, 0xaa, 0x39, 0x4a, 0xe7, 0xe1 } };

// define NamedDataObjectInfo
DATA_OBJECT_INFO(IDS_IDIODB_TYPE, CIdioDBaseDataObject, CIdioDBaseDataObject::m_szType, clsidCIdioDBDataObjectInfo, IDI_THUMBNAIL_IDIODB )

/////////////////////////////////////////////////////////////////////////////
// CScriptDataObject
CIdioDBaseDataObject::CIdioDBaseDataObject()
{
	_OleLockApp( this );
	EnableAutomation();
}

CIdioDBaseDataObject::~CIdioDBaseDataObject()
{
	_OleUnlockApp( this );
}

void CIdioDBaseDataObject::OnFinalRelease()
{
	__super::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CIdioDBaseDataObject, CDataObjectListBase)
	//{{AFX_MSG_MAP(CIdioDBaseDataObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CIdioDBaseDataObject, CDataObjectListBase)
	//{{AFX_DISPATCH_MAP(CIdioDBaseDataObject)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/*
// {39346386-2480-11D3-A5D9-0000B493FF1B}
static const IID IID_IScriptDataObjectDisp =
{ 0x39346386, 0x2480, 0x11d3, { 0xa5, 0xd9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
*/

BEGIN_INTERFACE_MAP(CIdioDBaseDataObject, CDataObjectListBase)
//	INTERFACE_PART(CIdioDBaseDataObject, IID_IIdioDBase, DataObject )
//	INTERFACE_PART(CIdioDBaseDataObject, IID_IIdioDBaseStorage, DataStorage )
END_INTERFACE_MAP()
/*
IMPLEMENT_UNKNOWN(CIdioDBaseDataObject, DataObject)
IMPLEMENT_UNKNOWN(CIdioDBaseDataObject, DataStorage)
*/
/////////////////////////////////////////////////////////////////////////////
// CScriptDataObject message handlers
bool CIdioDBaseDataObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	__super::SerializeObject( ar, pparams );

	if( ar.IsStoring() )
	{
		long lVersion = 0;
		ar << lVersion;
	}
	else
	{
		long lVersion = -1;

		ar >> lVersion;
	}
	return true;
}

bool CIdioDBaseDataObject::CanBeBaseObject()
{
	return true;
}
bool CIdioDBaseDataObject::IsModified()
{
	return false;
}

void CIdioDBaseDataObject::SetModifiedFlag( bool bSet )
{
}
/*********************************************************************************************
HRESULT CIdioDBaseDataObject::XDataStorage::ReadFile( BSTR bstr )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataStorage );
	CFile fl( CString( bstr ), CFile::modeWrite | CFile::modeCreate );
	CArchive ar( &fl, CArchive::load );
	
	IStream *punkStream = _AfxCreateMemoryStream();
	
	if( !punkStream )
		return S_FALSE;

	DWORD dwLen = 0;

	ar >> dwLen;
	if( dwLen )
	{
		BYTE *lpBuf = new BYTE[dwLen];

		ar.Read( (LPVOID)lpBuf, dwLen );

		COleStreamFile sfile( punkStream );
		sfile.Write( (void*)lpBuf, dwLen );
		sfile.Seek( 0, CFile::begin );

		CStreamEx strm( punkStream, 1 );
		pThis->SerializeObject( strm, 0 );
		delete []lpBuf;
	}

	return S_OK;
}
HRESULT CIdioDBaseDataObject::XDataStorage::WriteFile( BSTR bstr )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataStorage );
	CFile fl( CString( bstr ), CFile::modeWrite | CFile::modeCreate );
	CArchive ar( &fl, CArchive::store );
	
	IStream *punkStream = _AfxCreateMemoryStream();
	
	if( !punkStream )
		return S_FALSE;

	CStreamEx strm( punkStream, 0 );  
	pThis->SerializeObject( strm, 0 );

	COleStreamFile sfile( punkStream );
	DWORD dwLen = (DWORD)sfile.GetLength();

	if( dwLen )
	{
		BYTE *lpBuf = new BYTE[dwLen];

		sfile.Seek( 0, CFile::begin );
		sfile.Read( (void*)lpBuf, dwLen );
					
		ar << dwLen;
		ar.Write( lpBuf, dwLen );

		delete []lpBuf;
	}

	return S_OK;
}
/*********************************************************************************************

HRESULT CIdioDBaseDataObject::XDataObject::AddItem( IUnknown *punkIdio )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );
	if( !punkIdio )
		return S_FALSE;

	pThis->m_vIdio.push_back( punkIdio );
	pThis->m_vIdioText.push_back( "" );

	return S_OK;
}

HRESULT CIdioDBaseDataObject::XDataObject::AddItemAfter( IUnknown *punkIdio, long lID )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );
	if( !punkIdio )
		return S_FALSE;

	if( lID >= 0 && lID < (long)pThis->m_vIdio.size() )
	{
		pThis->m_vIdio.insert( pThis->m_vIdio.begin() + lID, punkIdio );
		pThis->m_vIdioText.insert( pThis->m_vIdioText.begin() + lID, "" );
	}

	return S_OK;
}


HRESULT CIdioDBaseDataObject::XDataObject::SetItemText( long lID,BSTR bstr )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );
	
	if( lID >= 0 && lID < (long)pThis->m_vIdioText.size() )
		pThis->m_vIdioText[lID] = CString( bstr );
    return S_OK;
}

HRESULT CIdioDBaseDataObject::XDataObject::GetItemText( long lID,BSTR *bstr )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );

	if( !bstr )
		return S_FALSE;

	if( lID >= 0 && lID < (long)pThis->m_vIdio.size() )
	{
		*bstr = pThis->m_vIdioText[lID].AllocSysString();
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CIdioDBaseDataObject::XDataObject::GetItem( long lID, IUnknown **punkIdio )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );

	if( !punkIdio )
		return S_FALSE;

	if( lID >= 0 && lID < (long)pThis->m_vIdio.size() )
	{
		*punkIdio = pThis->m_vIdio[lID];
		
		if( *punkIdio )
			(*punkIdio)->AddRef();

		return S_OK;
	}
	return S_FALSE;
}

HRESULT CIdioDBaseDataObject::XDataObject::GetCount( long *lCount )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );

	if( !lCount )
		return S_FALSE;
	
	*lCount = pThis->m_vIdio.size();

	return S_OK;
}

HRESULT CIdioDBaseDataObject::XDataObject::Remove( long lID )
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );

	if( lID >= 0 && lID < (long)pThis->m_vIdio.size() )
	{
		pThis->m_vIdio.erase( pThis->m_vIdio.begin() + lID );
		pThis->m_vIdioText.erase( pThis->m_vIdioText.begin() + lID );
		return S_OK;
	}
	return S_FALSE;
}

HRESULT CIdioDBaseDataObject::XDataObject::RemoveAll()
{
	METHOD_PROLOGUE_EX( CIdioDBaseDataObject, DataObject );
	pThis->m_vIdio.clear();
	return S_OK;
}
/**********************************************************************/
