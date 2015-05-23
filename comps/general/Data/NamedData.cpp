// CNamedData.cpp : implementation file
//

#include "stdafx.h"
#include "Data.h"
#include "NamedData.h"
#include "ObjectMgr.h"
#include "afxpriv2.h"
#include "Utils.h"
#include "aliaseint.h"
#include "guardint.h"
#include "utils.h"
#include "StreamUtils.h"
#include "\vt5\AWIN\profiler.h"
#include "vt5file.h"
#include "misc_utils.h"
#include "CryptedFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool IsUnknown( long vt )
{
	return vt == VT_UNKNOWN || vt == VT_DISPATCH;
}
//XEntries nested class implementation
CNamedData::XEntry::XEntry( CNamedData *pParent, XEntry *pparent, const char *pszEntryName )
{			  
	m_pData = pParent;
	m_pparent = pparent;

	DeInit();

	m_strEntry = pszEntryName;

}

CNamedData::XEntry::~XEntry()
{
	DeInit();
}

void CNamedData::XEntry::DeInit()
{
	ASSERT( m_pData );

//notify data context's if unknown 
	IUnknown	*pOld = 0;


	if( IsUnknown( m_variant.vt ) )
	{
		pOld = m_variant.punkVal;

 //		m_pData->_DeleteChilds( pOld );
	}
	else if( m_variant.vt == VT_BITS )
	{
		byte	*pbyte = (byte*)m_variant.lVal;
		TRACE("0x%x freed\n", pbyte);
		delete	pbyte;
	}


	for( int i = 0; i < GetEntriesCount(); i++ )
		delete GetEntry( i );
	
	//paul 11.11.2003. remove from map
	m_map.clear();
	m_arrChildEntries.RemoveAll();

	if( pOld )	
	{
		sptrINamedDataObject2	sptrO( pOld );
		sptrO->AttachData( 0 );
		m_pData->NotifyContexts(ncRemoveObject, pOld);

		XType	*pT = m_pData->GetType( pOld );
		pT->UnRegisterObject( pOld );
	}

	
	m_variant.Clear();
	m_strEntry = _T("Default");
}

//todo - work with VT_UNKNOWN variant
bool CNamedData::XEntry::Serialize( CStreamEx &ar )
{
#ifdef DBG_SERIALIZATION
	static int level=-1;
	++level;
	CString sLevel=CString(' ',level*4);
	long base=GetStreamPos(ar);
#endif 

	if( ar.IsLoading() )
	{
		DeInit();

		long	lSize;		//child entries count
		long	lVersion;
		short	lType;		//variant type

		ar>>lVersion;		//should be "1" or "2" now 3 - support IPersist, 4 - support SafeRead

		if( lVersion > 4 )
			throw new CFileException( CFileException::invalidFile );
		ar>>m_strEntry;
		ar>>lSize;
		ar>>lType;

		if( IsUnknown( lType ) )
		{
			if( lVersion >= 4 )
			{
				IUnknown	*punkObj = 0;

				CString strObjectName;
				GuidKey key;

				ar >> strObjectName;
				ar >> key;				
				
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );
				params.punkNamedData = &m_pData->m_xData;

				if( !::SafeReadObject( (IStream*)ar, &punkObj, (LPCSTR)strObjectName, 0, &params ) )
				{
					if( punkObj )
					{
						punkObj->Release();	punkObj = 0;
					}

				}
				else
				{
					INumeredObjectPtr sptrN = punkObj;
					if( sptrN )
						sptrN->AssignKey(key);

					::SetObjectName( punkObj, strObjectName );
					SetValue( _variant_t( punkObj ) );
					punkObj->Release();

				}

			}
			else
			{
				//1. load type
				CString	strType;
				ar>>strType;

				bool	bTextSerialized;
				ar>>(long&)bTextSerialized;


				IUnknown* punkObjFromPersist = 0;
				
				//try to read object CLSID(internal) and convert it to external and create instance
				if( lVersion >= 3 /*&& !bTextSerialized*/ )
				{
					GUID guidInternal = INVALID_KEY;	
					ar.Read( &guidInternal, sizeof(guid) );				 
					if( guidInternal != INVALID_KEY )
					{

						GUID guidExternal = INVALID_KEY;					
						IVTApplicationPtr ptrApp( ::GetAppUnknown() );
						if( ptrApp )
						{						
							if( S_OK != ptrApp->GetEntry( 1, (DWORD*)&guidInternal, (DWORD*)&guidExternal, 0 ) )
							{
								guidExternal = INVALID_KEY;
								ASSERT( false );
							}
						}
						else
						{
							ASSERT( false );
						}

						if( guidExternal != INVALID_KEY )
						{	
							INamedDataInfo	*pTypeInfo = ::FindTypeInfo( _bstr_t(strType) );
							if( pTypeInfo )						
							{

								HRESULT hres = ::CoCreateInstance( guidExternal, 0,  1, IID_IUnknown, (void**)&punkObjFromPersist );
								if( hres != S_OK  )
								{
									ASSERT( false );
									punkObjFromPersist = 0;
								}

								INamedDataObjectPtr ptrNO( punkObjFromPersist );
								if( ptrNO )
								{
									ptrNO->SetTypeInfo( pTypeInfo );
								}
								else
								{
									ASSERT( false );
								}
								
								pTypeInfo->Release();
								pTypeInfo = 0;
							}						
							else
							{
								ASSERT( false );
							}
						}
					}
					else
					{
						ASSERT( false );
					}
				}
				


				CString	strObjectName;
				ar>>strObjectName;

				//2. Construct object
				IUnknown	*punkObj = punkObjFromPersist;

				if( !punkObj )
					punkObj = ::CreateTypedObject( strType );			

				if( !punkObj )
					return true;
				//3. Read name
				if( bTextSerialized )
				{
					CString	strData;
					ar>>strData;

					_bstr_t	bstrData = strData;

					sptrITextObject	sptrTO( punkObj );
					sptrTO->SetText( bstrData );
					if (lVersion > 1)
					{
						GuidKey key;
						ar>>key;

						INumeredObjectPtr sptrN = punkObj;
						if (sptrN)
							sptrN->AssignKey(key);
					}
				}
				else
				{	 
					SerializeParams	params;
					ZeroMemory( &params, sizeof( params ) );
					params.punkNamedData = &m_pData->m_xData;

					sptrISerializableObject	sptrSO( punkObj );
					sptrSO->Load( (IStream*)ar, &params );
				}

				::SetObjectName( punkObj, strObjectName );
				SetValue( _variant_t(punkObj) );
				punkObj->Release();
			}
		}
		else if( lType == VT_BITS )
		{
			long	lMemSize = 0;
			ar>>lMemSize;
			byte	*pbyte = new byte[lMemSize+sizeof( lMemSize )];
			*(long*)pbyte = lMemSize;
			ar.Read( pbyte+sizeof( lMemSize ), lMemSize );
			m_variant.vt = VT_BITS;
			m_variant.lVal = (long)pbyte;
		}
		else
		{
			CString	str;

			ar>>str;

			_variant_t var( str );
			m_variant.Clear();
			_VarChangeType( m_variant, lType, &var);
		}
							//serialize the child entries
		for( int i = 0; i < lSize; i++ )
		{
			XEntry	*p = new XEntry( m_pData, this );
			if( p->Serialize( ar ) )
			{
				m_arrChildEntries.Add( p );
				//paul 11.11.2003. Add entry to map
				m_map.set( p, (const char*)p->m_strEntry );
			}
			else
			{
				delete p;
				return false;
			}
		}
	}
	else
	{
		ar<<4;
		ar<<m_strEntry;
		ar<<(int)m_arrChildEntries.GetSize();
		ar<<m_variant.vt;

		CString	str;
		if( IsUnknown( m_variant.vt ) )
		{			
			
			IUnknown	*punkObj = m_variant.punkVal;

			ar << GetObjectName( punkObj );
			GuidKey key = ::GetObjectKey( punkObj );
			ar<<key;
			
			bool bForceWriteAsText = false;
			if( m_pData )
				bForceWriteAsText = m_pData->m_bUseTextFileFormat;


			DWORD	dwSerializeFlags = 0;
			if( ::GetValueInt( ::GetAppUnknown(), "\\ImageCompress", "EnableCompress", 1 ) == 1 ) 
				dwSerializeFlags |= sf_ImageCompressed;

			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );
			params.flags = dwSerializeFlags;
			params.punkNamedData = &m_pData->m_xData;

			::WriteObject( (IStream*)ar, punkObj, 0, bForceWriteAsText, &params );
		}
		else if( m_variant.vt == VT_BITS )
		{
			byte	*pbyte = (byte	*)m_variant.lVal;
			long	lMemSize = *(long*)pbyte;
			ar<<lMemSize;
			ar.Write( pbyte+sizeof( lMemSize ), lMemSize );
		}
		else
		{
			_variant_t	var = m_variant;	
							//change type to string and serialize 
			_VarChangeType( var, VT_BSTR );
			str = var.bstrVal;
			ar<<str;
		}		
							//serialize the child entries
		for( int i = 0; i < m_arrChildEntries.GetSize(); i++ )
		{
			XEntry	*p = (XEntry	*)m_arrChildEntries[i];
			p->Serialize( ar );
		}
	}
#ifdef DBG_SERIALIZATION
	{
		CString s;
		s.Format("Entry %s size %d\n",
			(const char*)m_strEntry, GetStreamPos(ar)-base);
		OutputDebugString(sLevel+s);
	}
	--level;
#endif
	
	return true;
}


int CNamedData::XEntry::GetEntriesCount()
{
	return m_arrChildEntries.GetSize();
}

CNamedData::XEntry *
	CNamedData::XEntry::GetEntry( int ipos )
{
	return (XEntry*)m_arrChildEntries[ipos];
}

#include "\vt5\awin\misc_string.h"


CNamedData::XEntry *
	CNamedData::XEntry::Find( const char *pszEntryName )
{
	//PROFILE_TEST( "CNamedData::XEntry::Find" );

	TPOS lpos_find = m_map.find( pszEntryName );
	if( lpos_find )
		return m_map.get( lpos_find );

	CString	strPath = pszEntryName;
	if( strPath.IsEmpty() )
		return this;
	
	CString	strEntry, strSubPath;

	int	idx = strPath.Find( '\\' );

	if( idx != -1 )
	{
		strEntry = strPath.Left( idx );
		strSubPath = strPath.Right( strPath.GetLength()-idx-1 );
	}
	else
		strEntry = strPath;

	XEntry *pentryFound = 0;

	//paul 11.11.2003. Try to find in map first
	lpos_find = m_map.find( (const char*)strEntry );
	if( lpos_find )
	{
		pentryFound = m_map.get( lpos_find );
	}
	else
	{
		for( long n = 0; n < GetEntriesCount(); n++ )
		{
			XEntry *pentry = GetEntry( n );

			if( pentry->m_strEntry == strEntry )
			{
				pentryFound = pentry;
				if( pentryFound )break;
			}
		}
	}

	if( !pentryFound )
	{
		pentryFound = new XEntry( m_pData, this, strEntry );
		m_arrChildEntries.Add( pentryFound );
		//paul 11.11.2003. Add entry to map
		m_map.set( pentryFound, (const char*)pentryFound->m_strEntry );
	}

	if( !strSubPath.IsEmpty() )
		return pentryFound->Find( strSubPath );
	return pentryFound;
}

CString CNamedData::XEntry::GetPath()
{
	if( m_pparent != NULL )
	{
		CString	strPath = m_pparent->GetPath();
		if( strPath.IsEmpty() )
			return m_strEntry;
		else
			return strPath+"\\"+m_strEntry;
	}
	else
		return "";
}


void CNamedData::XEntry::SetValue( const _variant_t var )
{
	ASSERT( m_pData );
	ASSERT(m_strEntry.GetLength() > 0);

	IUnknown	*pOld = 0;
	IUnknown	*pNew = 0;
//test values
	if( IsUnknown( m_variant.vt ) )
	{
		pOld = m_variant.punkVal;
		m_pData->_DeleteChilds( pOld );
	}

	if( IsUnknown( var.vt ) )	
	{
		pNew = var.punkVal;

		if( CheckInterface( pNew, IID_INamedObject2 ) )
			::SetObjectName( pNew, m_strEntry );
	}
//set value
	m_variant = var;
//notify data context's
	if( !pOld && pNew )	//add
	{
		sptrINamedDataObject2	sptrN( pNew );
		sptrN->AttachData( m_pData->GetDocument() );
		m_pData->NotifyContexts(ncAddObject, pNew);

		::FireEvent(m_pData->GetDocument(), szEventCreateTypedObject, sptrN );
	}
	else
	if( pOld && !pNew )	//remove
	{
		sptrINamedDataObject2	sptrO( pOld );
		sptrO->AttachData( 0 );

		m_pData->NotifyContexts(ncRemoveObject, pOld);
	}
	else
	if( pOld && pNew )	//replace
	{
		sptrINamedDataObject2	sptrO( pOld );
		sptrO->AttachData( 0 );
		sptrINamedDataObject2	sptrN( pNew );
		sptrN->AttachData( m_pData->GetDocument() );

		m_pData->NotifyContexts(ncReplaceObject, pNew, pOld);
	}

	//unregister object
	if( pOld )
	{
		pOld->AddRef();
		XType	*pT = m_pData->GetType( pOld );
		pT->UnRegisterObject( pOld );

		long	l = pOld->Release();

		//TRACE( "Ref counter %d\n", l );
	}
	//register object
	if( pNew )
	{
		XType	*pT = m_pData->GetType( pNew );
		pT->RegisterObject( pNew );

		m_pData->_InsertChilds( pNew );
	}
	
}


CNamedData::XType::XType( CNamedData *pData, IUnknown *punk )
{
	//use profile functions
	m_pData = pData;
	m_iNamesCounter = 1;

	sptrINamedDataObject	sptr( punk );

	IUnknown	*punkType = 0;
	sptr->GetDataInfo( &punkType );
	INamedObject2Ptr	ptrNamed( punkType );
	punkType->Release();	

	BSTR	bstrName, bstrUserName;
	ptrNamed->GetName( &bstrName );
	ptrNamed->GetUserName( &bstrUserName );

	m_bstrTypeName = bstrName;
	m_bstrTypeUserName = bstrUserName;

	::SysFreeString( bstrName );
	::SysFreeString( bstrUserName );


	flags = GetValueInt( GetAppUnknown(), "\\types", m_bstrTypeName, 0 );
}

CNamedData::XType::~XType()
{
	ASSERT( !GetFirstObjectPosition() );
}

// create unique name based on type name and internal conunter
void CNamedData::XType::InitObject( IUnknown *punk, CString &strPath )
{

	INamedObject2Ptr	sptr( punk );

//get object name
	CString	strEntryName = ::GetObjectName( punk );

	if( strEntryName.GetLength() == 0 )
	{
		GenerateUniqueNameForObject( sptr, m_pData->GetControllingUnknown() );
		strEntryName = ::GetObjectName( punk );
	}
//get object parent
	IUnknown *punkParent = 0;

	sptrINamedDataObject2	sptrN( sptr );
	sptrN->GetParent( &punkParent );

	//object has parent
	if( punkParent )
	{
		CString strParentPath = ::GetObjectPath( m_pData->GetDocument(), punkParent );
		strPath = strParentPath+"\\"+strEntryName;

		punkParent->Release();
	}
	else
	{
		flags = GetValueInt( GetAppUnknown(), "\\types", m_bstrTypeName, 0 );
		if( flags & TYPE_FLAG_SINGLEOBJECT )
		{
			//set the object bname
			sptr->SetName( m_bstrTypeName );
			strEntryName = (const char*)m_bstrTypeName;
		}


		if( strPath.IsEmpty() )
		{
			strPath.Format( "\\pool\\%s\\%s", (const char*)m_bstrTypeName, (const char*)strEntryName );
		}
		else
		{
			//Do nothing
		}
	}
}

//register object in object's list
void CNamedData::XType::RegisterObject( IUnknown *punk )
{
	ASSERT( punk );

	punk->AddRef();

	m_objects.AddHead( punk );
}

//unregister object
void CNamedData::XType::UnRegisterObject( IUnknown *punk )
{
	POSITION	pos = m_objects.Find( punk );

	ASSERT( pos );

	if( !pos )
		return;

	m_objects.RemoveAt( pos );

	punk->Release();
}

//returns count of objects give type in given named data
long CNamedData::XType::GetObjectsCount()
{
	return m_objects.GetCount();
}

//returns first object position
POSITION CNamedData::XType::GetFirstObjectPosition()
{
	return m_objects.GetHeadPosition();
}
	

//return object in given position. don't call AddRef()!!!
IUnknown *CNamedData::XType::GetNextObject( POSITION &pos )
{
	return (IUnknown*)m_objects.GetNext( pos );
}





/////////////////////////////////////////////////////////////////////////////
// CNamedData

IMPLEMENT_DYNCREATE(CNamedData, CCmdTargetEx)

// {BD7C7452-1D73-11d3-A5D0-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CNamedData, "Data.NamedData", 
0xbd7c7452, 0x1d73, 0x11d3, 0xa5, 0xd0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

CNamedData::CNamedData()
{
	_OleLockApp( this );

	m_bUseTextFileFormat = true;

	EnableAutomation();
	EnableAggregation();

	m_pEntryRoot = new XEntry( this, 0 );
	m_pEntryCurrent = m_pEntryRoot;

	SetupCurrentSection( "\\" );

	m_bContextNotificationEnabled = true;
	m_bEnableCheckChilds = true;
	m_punkExternalDoc = NULL;
}

IUnknown* CNamedData::GetDocument()
{
	return ( m_punkExternalDoc ? m_punkExternalDoc : GetControllingUnknown() );
}
/*
BOOL CNamedData::OnCreateAggregates()
{
	m_punkDoc = GetControllingUnknown();
	return CCmdTargetEx::OnCreateAggregates();
}
*/

CNamedData::~CNamedData()
{
	EnableContextNotification( false );
	DeInit();

	delete m_pEntryRoot;

	_OleUnlockApp( this );
}

void CNamedData::DeInit()
{
	XNotifyLock	lock( this );

	m_Contexts.Notify(ncEmpty, 0, 0);	

	/*_variant_t	var;
	SetSection(szLookup);
	var = GetValue(szLookupTable);
	if( var.vt != VT_EMPTY )
	{
		byte	*pbyte = (byte*)var.lVal;
		if(pbyte)	
			delete pbyte;
		pbyte = 0;
		SetValue(szLookupTable, _variant_t((long)0));
	}
	var = GetValue(szLookupState);
	if( var.vt != VT_EMPTY )
	{
		byte	*pbyte = (byte*)var.lVal;
		if(pbyte)	
			delete pbyte;
		pbyte = 0;
		SetValue(szLookupState, _variant_t((long)0));
	}*/
	
	delete m_pEntryRoot;

	for( int i = 0; i < m_ptrsTypes.GetSize(); i++ )
	{
		XType	*ptype = (XType*)m_ptrsTypes[i];
		delete ptype;
	}
	m_ptrsTypes.RemoveAll();
	
	m_pEntryRoot = new XEntry( this, 0 );
	m_pEntryCurrent = m_pEntryRoot;

//	SetModifiedFlag( false );
}


void CNamedData::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CNamedData, CCmdTargetEx)
	//{{AFX_MSG_MAP(CNamedData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CNamedData, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CNamedData)
	DISP_FUNCTION(CNamedData, "SetValue", SetValue, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CNamedData, "GetValue", GetValue, VT_VARIANT, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "SetSection", SetSection, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "GetEntriesCount", GetEntriesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CNamedData, "GetEntryName", GetEntryName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CNamedData, "GetTypesCount", GetTypesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CNamedData, "GetTypeName", GetTypeName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CNamedData, "GetDefValue", GetDefValue, VT_VARIANT, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CNamedData, "RemoveValue", RemoveValue, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "WriteTextFile", DispWriteTextFile, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "WriteFile", DispWriteFile, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "ReadFile", DispReadFile, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "ExportEntry", ExportEntry, VT_BOOL, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION(CNamedData, "ImportEntry", ImportEntry, VT_BOOL, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION(CNamedData, "GetFirstObjectPos", GetFirstObjectPos, VT_I4, VTS_I4)
	DISP_FUNCTION(CNamedData, "GetNextObject", GetNextObject, VT_DISPATCH, VTS_I4 VTS_PVARIANT)
	DISP_FUNCTION(CNamedData, "CopyNamedData", CopyNamedData, VT_BOOL, VTS_DISPATCH VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION(CNamedData, "GetSection", GetSection, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CNamedData, "WriteProtectedFile", DispWriteProtectedFile, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CNamedData, "ReadProtectedFile", DispReadProtectedFile, VT_BOOL, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_INamedDataDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {ADBA6C03-1D68-11D3-A5D0-0000B493FF1B}
static const IID IID_INamedDataDisp =
{ 0xadba6c03, 0x1d68, 0x11d3, { 0xa5, 0xd0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CNamedData, CCmdTargetEx)
	INTERFACE_PART(CNamedData, IID_INamedDataDisp, Dispatch)
	INTERFACE_PART(CNamedData, IID_INamedData, Data)
	INTERFACE_PART(CNamedData, IID_INamedDataLogger, DataLogger)
	INTERFACE_PART(CNamedData, IID_IDataTypeManager, Types)
	INTERFACE_PART(CNamedData, IID_IFileDataObject, File)
	INTERFACE_PART(CNamedData, IID_IFileDataObject2, File)
	INTERFACE_PART(CNamedData, IID_IFileDataObject3, File)
	INTERFACE_PART(CNamedData, IID_ISerializableObject, Serialize )
	INTERFACE_PART(CNamedData, IID_IDocForNamedData, DocP )
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CNamedData, Data);
IMPLEMENT_UNKNOWN(CNamedData, DataLogger);
IMPLEMENT_UNKNOWN(CNamedData, DocP);
IMPLEMENT_UNKNOWN(CNamedData, Types);

HRESULT CNamedData::XDocP::AttachExternalDocument( IUnknown *punkND )
{
	METHOD_PROLOGUE_EX(CNamedData, DocP)

	pThis->m_punkExternalDoc = punkND;
	return S_OK;
}

void CNamedData::SetupCurrentSection( const char *pszPath )
{
	//PROFILE_TEST( "CNamedData::SetupCurrentSection" );
	//_trace_file_text("SetupCurrentSection.log", pszPath);
	//_trace_file_text("SetupCurrentSection.log", "\r\n");

	CString	strPath = pszPath;

	ASSERT( m_pEntryRoot );

	if( strPath.IsEmpty() )
	{
		m_pEntryCurrent = m_pEntryRoot;
		return;
	}

	if( strPath[0]== '\\' )
		m_pEntryCurrent = m_pEntryRoot->Find( pszPath+1 );
	else
		m_pEntryCurrent = m_pEntryRoot->Find( pszPath );


	if( !m_pEntryCurrent )
		m_pEntryCurrent = m_pEntryRoot;
	ASSERT( m_pEntryCurrent );
}

CNamedData::XEntry *CNamedData::GetEntry( const char *pszName )
{
	ASSERT( m_pEntryCurrent );
	ASSERT( m_pEntryRoot );

	if( !pszName || !strlen(pszName) )
		return m_pEntryRoot;

	if( pszName[0]== '\\' )
	{
		return m_pEntryRoot->Find( pszName+1 );
	}
	else
	{
		if( m_pEntryCurrent )
			return m_pEntryCurrent->Find( pszName );
		else
			return 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CNamedData message handlers

void CNamedData::_SetValue(LPCTSTR Entry, const VARIANT FAR& Value) 
{
	CString	str( Entry );

	_variant_t	var( Value );
	if( Value.vt == (VT_VARIANT|VT_BYREF) ) 
	{
		var = Value.pvarVal;
	}

	
	if( IsUnknown( var.vt ) )
	{
		INamedObject2Ptr	sptrNamed( var );
		XType	*ptype = GetType( (IUnknown*)sptrNamed );

		if( !ptype )
			return;

		ptype->InitObject( sptrNamed, str );
	}


	XEntry *pe = GetEntry( str );

	if( !pe )
		return;
	ASSERT( pe );

	pe->SetValue( var );

	// поддержка лога
	if(m_ptrLogWrite!=0)
	{
		CString	strPath = CString("\\") + pe->GetPath();
		int n = strPath.ReverseFind('\\');
		CString strEntry = strPath.Mid(n+1,4096);
		CString strSection = strPath.Left(n);
		::SetValue(m_ptrLogWrite, strSection, strEntry, Value);
	}
	if(m_ptrLogRead!=0)
	{ // при записи ставим сразу и в лог на чтение
		CString	strPath = CString("\\") + pe->GetPath();
		int n = strPath.ReverseFind('\\');
		CString strEntry = strPath.Mid(n+1,4096);
		CString strSection = strPath.Left(n);
		::SetValue(m_ptrLogRead, strSection, strEntry, Value);
	}

//	var.Detach();
}


void CNamedData::SetValue(LPCTSTR Entry, const VARIANT FAR& Value) 
{
	
	if( m_punkExternalDoc )
	{
		//это база данных
		IDBaseDocumentPtr	ptrDoc( m_punkExternalDoc );
		if( ptrDoc )
		{
			INamedDataPtr	ptrND( m_punkExternalDoc );
			ptrND->SetValue( _bstr_t( Entry ), Value );
			return;
		}
	}	
	

	_SetValue( Entry, Value );

}

VARIANT CNamedData::GetValue(LPCTSTR Entry) 
{
	//PROFILE_TEST( "CNamedData::GetValue" )

	_variant_t	var;

	XEntry *pe = GetEntry( Entry );
	
	if( pe )
	{
		var = pe->m_variant;
		// поддержка лога
		if(m_ptrLogRead!=0)
		{
			CString	strPath = CString("\\") + pe->GetPath();
			int n = strPath.ReverseFind('\\');
			CString strEntry = strPath.Mid(n+1,4096);
			CString strSection = strPath.Left(n);
			::SetValue(m_ptrLogRead, strSection, strEntry, var);
		}
	}

	return var.Detach();
}

void CNamedData::SetSection(LPCTSTR Section) 
{
	SetupCurrentSection( Section );
}

BSTR CNamedData::GetSection() 
{
	CString str = m_pEntryCurrent->GetPath();
	return str.AllocSysString();
}

long CNamedData::GetEntriesCount() 
{
	ASSERT( m_pEntryCurrent );

	return m_pEntryCurrent->GetEntriesCount();
}

BSTR CNamedData::GetEntryName(long index) 
{
	CString strResult;

	ASSERT( m_pEntryCurrent );

	if( index < 0 || index >= m_pEntryCurrent->GetEntriesCount() )
	{
		CString str;
		str.Format( "Current section  is \"%s\". Try to get %d of %d", m_pEntryCurrent->GetPath(), index,  m_pEntryCurrent->GetEntriesCount() );
		str = "\"GetEntryName\" is out of range! " + str; 

		AfxMessageBox( str );
	}
	else
	strResult = m_pEntryCurrent->GetEntry( index )->m_strEntry;

 	return strResult.AllocSysString();
}


long CNamedData::GetTypesCount() 
{
	return m_ptrsTypes.GetSize();
}

	//get type by index
CNamedData::XType	*CNamedData::GetType( long nPos )
{
	return (XType	*)m_ptrsTypes[nPos];
}



BSTR CNamedData::GetTypeName(long index)
{
	if( index < 0 || index > m_ptrsTypes.GetSize()-1 )
		return 0;

	XType	*p = (XType*)m_ptrsTypes[index];

	return p->m_bstrTypeName.copy();
}


///////////////////////////////////////////////////////////////////////////////
//INamedDataLogger interface implementation
HRESULT CNamedData::XDataLogger::SetLog(long nKind, IUnknown *punkLog)
{
	METHOD_PROLOGUE_EX(CNamedData, DataLogger)
	{
		if(ndlkRead == nKind) pThis->m_ptrLogRead = punkLog;
		else if(ndlkWrite == nKind) pThis->m_ptrLogWrite = punkLog;
		else return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT CNamedData::XDataLogger::GetLog(long nKind, IUnknown **ppunkLog)
{
	METHOD_PROLOGUE_EX(CNamedData, DataLogger)
	{
		if(!ppunkLog) return E_INVALIDARG;
		*ppunkLog = 0;
		if(ndlkRead == nKind) *ppunkLog = pThis->m_ptrLogRead;
		else if(ndlkWrite == nKind) *ppunkLog = pThis->m_ptrLogWrite;
		else return E_INVALIDARG;
		if(*ppunkLog) (*ppunkLog)->AddRef();
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//idata interface implementation


HRESULT CNamedData::XData::SetupSection( BSTR bstrSectionName )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)

	CString	str( bstrSectionName );
	pThis->SetupCurrentSection( str );
	return S_OK;
}

HRESULT CNamedData::XData::SetValue( BSTR bstrName, const tagVARIANT var )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		_bstr_t	entry = bstrName;
		pThis->_SetValue( entry, var );
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetValue( BSTR bstrName, tagVARIANT *pvar )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		//PROFILE_TEST( "CNamedData::XData::GetValue" )

		_bstr_t	entry = bstrName;
		_variant_t	var;

		XEntry *pe = pThis->GetEntry( entry );
		
		if( pe )
		{
			if( pe->m_variant.vt == VT_EMPTY &&
				pvar->vt != VT_EMPTY )
				pe->m_variant = *pvar;
			var = pe->m_variant;
			if(pThis->m_ptrLogRead!=0)
			{
				CString	strPath = CString("\\") + pe->GetPath();
				int n = strPath.ReverseFind('\\');
				CString strEntry = strPath.Mid(n+1,4096);
				CString strSection = strPath.Left(n);
				::SetValue(pThis->m_ptrLogRead, strSection, strEntry, var);
			}
		}

		VariantClear(pvar);
		*pvar = var.Detach();

		return S_OK;
	}
}

HRESULT CNamedData::XData::GetEntriesCount( long *piCount )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		*piCount = pThis->GetEntriesCount();
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetEntryName( long idx, BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		*pbstrName = pThis->GetEntryName( idx );
		return S_OK;
	}
}

HRESULT CNamedData::XData::RenameEntry( BSTR bstrOldName, BSTR bstrName )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		_bstr_t	bstrON = bstrOldName;
		CNamedData::XEntry *pe  = pThis->GetEntry( bstrON );

		if( !pe )
			return E_INVALIDARG;

		pe->m_strEntry = bstrName;
		return S_OK;
	}
}

HRESULT CNamedData::XData::DeleteEntry( BSTR bstrName )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{

		CString strDeleteEntry = bstrName;
		CString strCurrentEntry;
		CArray< CString, CString > arEntry;
		

		XEntry* pEntry = pThis->m_pEntryCurrent;
		while( pEntry )
		{			
			arEntry.Add( pEntry->m_strEntry );
			pEntry = pEntry->m_pparent;
		}

		for( int i=arEntry.GetSize()-1;i>=0;i-- )
		{
			if( arEntry[i].IsEmpty() )
				continue;

			strCurrentEntry += "\\";
			strCurrentEntry += arEntry[i];
		}

		arEntry.RemoveAll();


		if( !bstrName )
		{
			pThis->DeInit();
			return S_OK;
		}
		_bstr_t	bstrN = bstrName;

		if( bstrN.length() == 0 )
			return E_INVALIDARG;
		CNamedData::XEntry *pe  = pThis->GetEntry( bstrN );

		if( !pe )
			return E_INVALIDARG;


		CNamedData::XEntry *peParent = pe->m_pparent;

		int pos = -1;
		if( peParent )
		{
			for( int i = 0; i < peParent->m_arrChildEntries.GetSize(); i++ )
			{
				if( peParent->m_arrChildEntries[i] == pe )
				{
					pos = i;
					break;
				}
			}
		}

		//paul 11.11.2003. Comment & move down
		//delete pe;

		if (peParent && pos != -1 )
		{
			//paul 11.11.2003. remove from map
			XEntry* pchild_entry = (XEntry*)peParent->m_arrChildEntries[pos];
			TPOS lpos_map = peParent->m_map.find(pchild_entry->m_strEntry);
			ASSERT( lpos_map ); 
			if( lpos_map )
				peParent->m_map.remove( lpos_map );			
			peParent->m_arrChildEntries.RemoveAt( pos );
		}
		
		delete pe;

//		pThis->SetModifiedFlag();

		CString str_test = strCurrentEntry + "\\";
		if( strCurrentEntry.Find( strDeleteEntry ) != -1 || str_test.Find( strDeleteEntry ) != -1 )
			pThis->m_pEntryCurrent = pThis->m_pEntryRoot;

		

		return S_OK;
	}
}

// Get Active context 
// this function is needed to clone context to from existing context and not-empty NamedData 
HRESULT CNamedData::XData::GetActiveContext( IUnknown **punk )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		*punk = pThis->m_Contexts.GetActive();
		if( *punk )
			(*punk)->AddRef();

		return S_OK;
	}
}


// Set Active context 
// this function is needed to clone context to from existing context and not-empty NamedData 
HRESULT CNamedData::XData::SetActiveContext( IUnknown *punk )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		return pThis->SetActiveContext(punk);
	}
}



//Add context to NamedData 
// this function is needed to clone context to from existing context and not-empty NamedData 
HRESULT CNamedData::XData::AddContext( IUnknown *punk )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!punk)
			return E_INVALIDARG;

		pThis->m_Contexts.AddComponent(punk);
		
		return S_OK;
	}
}

// Remove context 
HRESULT CNamedData::XData::RemoveContext( IUnknown *punk )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!punk)
			return E_INVALIDARG;

		return pThis->m_Contexts.Remove(punk) ? S_OK : E_INVALIDARG;
	}
}

// check name exists
HRESULT CNamedData::XData::NameExists( BSTR bstrName, long* Exists )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		_bstr_t bstrCheckedName(bstrName);

		if( bstrCheckedName.length() == 0 )
		{
			*Exists = 2;
			return S_OK;
		}

		IUnknown* punk = ::GetObjectByName(this, bstrCheckedName, NULL);

		*Exists = (punk) ? 1 : 0;
		
		if (punk)
			punk->Release();

		return S_OK;
	}
}



// get object by object's name
HRESULT CNamedData::XData::GetObject( BSTR bstrName, BSTR bstrType, IUnknown **punk )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		bool bWithType = false;
		// get params and check it to validity
		_bstr_t bstrObjName(bstrName);
		if (bstrObjName.length() <= 0)
			return E_INVALIDARG;
		
		_bstr_t bstrObjType(bstrType);

		*punk = ::GetObjectByName(this, bstrObjName, bstrObjType);

		return S_OK;
	}
}

HRESULT CNamedData::XData::NotifyContexts( DWORD dwNotifyCode, IUnknown *punkNew, IUnknown *punkOld, DWORD dwData)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		pThis->NotifyContexts((NotifyCodes)dwNotifyCode, punkNew, punkOld, dwData);
		return S_OK;
	}
}

HRESULT CNamedData::XData::EnableBinaryStore( BOOL bBinary )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		pThis->m_bUseTextFileFormat = bBinary != TRUE;
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetCurrentSection( BSTR* pbstrSection )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		CString strSection;
		if( pThis->m_pEntryCurrent && !pThis->m_pEntryCurrent->m_strEntry.IsEmpty() )
		{
			strSection = pThis->m_pEntryCurrent->GetPath();
		}
		*pbstrSection = strSection.AllocSysString();
		return S_OK;
	}
}

// base objects
HRESULT CNamedData::XData::GetBaseGroupCount(int * pnCount)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pnCount)
			return E_INVALIDARG;

		*pnCount = pThis->m_mapBase.GetCount();
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetBaseGroupFirstPos(long * plPos)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = (long)pThis->m_mapBase.GetStartPosition();
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetNextBaseGroup(GUID * pKey, long * plPos)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || !plPos)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		*pKey = INVALID_KEY;

		if (!pos)
			return E_INVALIDARG;

		CBaseGroup * pData = 0;
		pThis->m_mapBase.GetNextAssoc(pos, *pKey, pData);
		*plPos = (long)pos;
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetIsBaseGroup(GUID * pKey, BOOL * pbBase)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || !pbBase || (*pKey) == INVALID_KEY)
			return E_INVALIDARG;

		CBaseGroup * pData = 0;
		*pbBase = pThis->m_mapBase.Lookup(*pKey, pData) && pData;
		return S_OK;
	}
}

HRESULT CNamedData::XData::GetBaseGroupBaseObject(GUID * pKey, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || (*pKey) == INVALID_KEY || !ppunkObject)
			return E_INVALIDARG;

		*ppunkObject = 0;
		CBaseGroup * pData = 0;
		if (!pThis->m_mapBase.Lookup(*pKey, pData) || !pData)
			return E_INVALIDARG;

		POSITION pos = pData->FindBase(GuidKey(*pKey));
		if (pos)
			*ppunkObject = (IUnknown*)pData->list.GetAt(pos);

		if (*ppunkObject)
			(*ppunkObject)->AddRef();

		return S_OK;
	}
}

HRESULT CNamedData::XData::GetBaseGroupObjectFirstPos(GUID * pKey, long * plPos)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || !plPos || (*pKey) == INVALID_KEY)
			return E_INVALIDARG;

		*plPos = 0;
		CBaseGroup * pData = 0;
		if (!pThis->m_mapBase.Lookup(*pKey, pData))
			return E_INVALIDARG;

		if (pData && pData->list.GetCount())
			*plPos = (long)(pData->list.GetHeadPosition());

		return S_OK;
	}
}

HRESULT CNamedData::XData::GetBaseGroupNextObject(GUID * pKey, long * plPos, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || (*pKey) == INVALID_KEY || !plPos || (*plPos) == 0 || !ppunkObject)
			return E_INVALIDARG;

		*ppunkObject = 0;
		CBaseGroup * pData = 0;
		if (!pThis->m_mapBase.Lookup(*pKey, pData) || !pData)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		*ppunkObject = (IUnknown*)pData->list.GetNext(pos);
		*plPos = (long)pos;

		if (*ppunkObject)
			(*ppunkObject)->AddRef();

		return S_OK;
	}
}

HRESULT CNamedData::XData::GetBaseGroupObjectsCount(GUID * pKey, int * pnCount)
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	{
		if (!pKey || (*pKey) == INVALID_KEY || !pnCount)
			return E_INVALIDARG;

		*pnCount = 0;
		CBaseGroup * pData = 0;
		if (!pThis->m_mapBase.Lookup(*pKey, pData))
			return E_INVALIDARG;

		if (pData)
			*pnCount = pData->list.GetCount();

		return S_OK;
	}
}

HRESULT CNamedData::XData::SetEmptySection( BSTR* bstrSectionName )
{
	METHOD_PROLOGUE_EX(CNamedData, Data)
	_bstr_t section(*bstrSectionName,false);
	for(;;)
	{
		HRESULT hr=SetupSection(section);
		long lCount;
		hr=GetEntriesCount(&lCount);
		if(lCount>0){
			int iNumber=0;
			int i=section.length();
			for(int power10=1; i--; power10*=10){
				wchar_t ch=((wchar_t*)section)[i];
				if(L'0'<=ch && ch<=L'9'){
					iNumber += (ch-L'0')*power10;
				}else{
					break;
				}
			}
			section=_bstr_t(CStringW((wchar_t*)section).Left(i+1))+_bstr_t(iNumber+1);
		}else{
			break;
		}
	}
	return S_OK;
}


/*

////////////////
// get object by name and type
IUnknown* GetObjectByNameAndType(IUnknown * pNamedData, LPCTSTR szObjName, LPCTSTR szObjType)
{
	IUnknown* punk = NULL;
	_try(common, GetObjectByNameAndType)
	{
		// check validity of parameter 'type'
		CString strObjType(szObjType);
		//  if it's empty we shall try to get object by its name only 
		if (strObjType.IsEmpty())
			return ::GetObjectByName(pNamedData, NULL, szObjName);


		// check validity of parameter 'name'
		CString strObjName(szObjName);
		//  if it's empty we shall return NULL (named object always must have name)
		if (strObjName.IsEmpty())
			return NULL;
		
		// get IDataTypeManager
		if (!CheckInterface(pNamedData, IID_IDataTypeManager))
			return NULL;

		sptrIDataTypeManager sptrMgr(pNamedData);
		// get types' count 
		long nCount = 0;
		sptrMgr->GetTypesCount(&nCount);

		// looking for requested type 
		for (long index = 0; index < nCount; index++)
		{
			BSTR bstrType = NULL;
			sptrMgr->GetType(index, bstrType);
			
			if (!bstrType)
				continue;
			
			CString strType = bstrType;
			// if this type is not  equals with requested type
			if (strType != strObjType)
			{
				::SysFreeString(bstrType);
				continue;
			}
			::SysFreeString(bstrType);

			// else try to find object
			long lpos = 0;
			sptrMgr->GetObjectFirstPosition(index, &lpos);
			// looking for object with requested name
			while (lpos)
			{
				IUnknown* punk = NULL;
				GetNextObject(index, &plpos, &punk);

				CString strName = ::GetObjectName(punk);
				// if this object's name is equals with requested
				if (strName == strObjName)
					return punk;

				// else continue
				if (punk)
					punk->Release()

			} // while for objects of 'type'

		}// for types
	}
	_catch
	{
		return NULL;
	}
	// if object is not found 
	return NULL;
}

// get object by name only
IUnknown* GetObjectByName(IUnknown * pNamedData, LPCTSTR szpath, LPCTSTR szObjName)
{
	IUnknown* punk = NULL;

	try (common, GetObjectByName)
	{

		if (!CheckInterface(pNamedData, IID_INamedData))
			return NULL;

		sptrINamedData sptrData(pNamedData);

		CString strFindName(szObjName);
		// check strObjName valid; if not return NULL (object with empty name naver exists)
		if (strFindName.IsEmpty())
			return NULL;

		CString CurrentPath((!szpath) ? "" : szpath);
		_bstr_t bstrPath(CurrentPath);

		// set section
		sptrData->SetupSection(bstrPath);

		// get count of entries
		long EntryCount = 0;
		sptrData->GetEntriesCount(&EntryCount);
	
		// for all entries
		for (int  i = 0; i < (int)EntryCount; i++)
		{
			_try(common, SubGetObjectByName)
			{
				BSTR bstr = NULL;
				// get entry name
				sptrData->GetEntryName(i, &bstr);

				// destructor will release system memory from bstr
				_bstr_t bstrName(bstr, false);
				// format a full path for entry
				_bstr_t bstrPathNew = bstrPath + "\\" + bstrName;
		
				// get value
				_variant_t var;
				sptrData->GetValue(bstrPathNew, &var);

				// check for IUnknown
				if (var.vt == VT_UNKNOWN)
				{
					// it's must be namedObject
					punk = (IUnknown*)var;
					// get object name 
					CString strObjectName = ::GetObjectName(punk);
					
					// compare object's  name with Looking to  name
					if (strObjectName == strFindName)
						return punk;

					// else release this pointer and look to again
					punk->Release();
					punk = NULL;
						
				} // IUnknown
				
				// if this entry has children => we want to walk throught them
				if (EntryCount > 0)
				{
					if ((punk = GetObjectByName((IUnknown*)sptrData.GetInterfacePtr(), bstrPathNew, strFindName)) != NULL)
						return punk;
				}

				// for next entry on this level restore Section
				sptrData->SetupSection(bstrPath);

			}
			_catch // for all exceptions
			{
				continue;
			}
		} // for all entries

	}
	_catch
	{
		return NULL;
	}
	return NULL;
}
*/

//////////////

HRESULT CNamedData::XTypes::GetTypesCount( long *pnCount )
{
	METHOD_PROLOGUE_EX(CNamedData, Types)

	*pnCount = pThis->GetTypesCount();
	return S_OK;
}

HRESULT CNamedData::XTypes::GetType( long index, BSTR *pbstrType )
{
	METHOD_PROLOGUE_EX(CNamedData, Types)

	*pbstrType = pThis->GetTypeName( index );
	return S_OK;
}

HRESULT CNamedData::XTypes::GetObjectFirstPosition( long nType, LONG_PTR *plpos )
{
	METHOD_PROLOGUE_EX(CNamedData, Types)

	XType	*pT = pThis->GetType( nType );
	(*plpos) = (long)pT->GetFirstObjectPosition();
	return S_OK;
}

HRESULT CNamedData::XTypes::GetNextObject( long nType, LONG_PTR *plpos, IUnknown **ppunkObj )
{
	METHOD_PROLOGUE_EX(CNamedData, Types)

	XType	*pT = pThis->GetType( nType );
	*ppunkObj = pT->GetNextObject( (POSITION&)*plpos );
	(*ppunkObj)->AddRef();

	return S_OK;
}


CNamedData::XType 
	*CNamedData::GetType( IUnknown *punk )
{
	sptrINamedDataObject	sptr( punk );

	_bstr_t	bstrT;

	BSTR	bstr;
	sptr->GetType( &bstr );
	bstrT = bstr;
	::SysFreeString( bstr );

	XType *ptype = GetType( bstrT );

	if( ptype )
		return ptype;
	

	ptype = new XType( this, punk );

	m_ptrsTypes.Add( ptype );

	return ptype;
}

CNamedData::XType	
	*CNamedData::GetType( const char *psz )
{
	_bstr_t	bstrType = psz;
	for( int i = 0; i < m_ptrsTypes.GetSize(); i++ )
	{
		XType	*ptype = (XType*)m_ptrsTypes[i];


		if( ptype->m_bstrTypeName == bstrType )
			return ptype;
	}
	return 0;
}

// set active context and sync namedData's context with it
bool CNamedData::SetActiveContext(IUnknown *punk)
{
	if (!m_Contexts.GetComponentCount())
		return true;

	IDataContext2Ptr sptrC = punk;
	if (sptrC == 0)
		sptrC = GetDocument();

	GuidKey lNewKey = ::GetObjectKey(sptrC);
	GuidKey lOldKey = ::GetObjectKey(m_Contexts.GetActive());
	if (lOldKey == lNewKey)
		return true;

	if (!m_Contexts.SetActive(sptrC))
	{
		ASSERT(false);
		return false;
	}

	IDataContext2Ptr sptrThis = GetDocument();
	if (sptrThis == 0 || ::GetObjectKey(sptrThis) == lNewKey)
		return true;

	return SUCCEEDED(sptrThis->SynchronizeWithContext(sptrC)) == TRUE;
}



bool CNamedData::BeforeReadFile()
{
	_try(CNamedData, BeforeReadFile)
	{
		CFile	file( m_strCurrentFileName, CFile::modeRead );

		char	szBuffer[6];

		file.Read( szBuffer, 5 );
		szBuffer[5] = 0;

		if( !strcmp( szBuffer, _T("Named") ) )
			m_bUseTextFileFormat = true;
		else
			m_bUseTextFileFormat = false;

		// 10.05.2006 build 104
		// Поддержка защищенных NamedData
		{
		}

		// init key cache
		//InitKeyCache();
	}
	_catch;
	return true;
}

bool CNamedData::AfterReadFile()
{
	bool bRet = true;
	// restore relation between prev object_key and new object_key
	// after named data loaded from file
	bRet = RestoreRelationObjectsAndClasses();
	ASSERT(bRet);

	// restore relation between source and base keys for all objects
	bRet = RestoreBaseSourceKeys();
	ASSERT(bRet);

	// 10.05.2006 build 104
#ifndef DEMOVERSION
	// Поддержка защищенных NamedData (в демо-версии читаем их свободно)
	if(m_bUseProtectedFormat)
	{
		int nStealthIndex = ::GetValueInt(GetControllingUnknown(), "\\General", "StealthIndex",-1);
		if( nStealthIndex>=0 && GetStealthKeysNumber() && !StealthIndexIsAvailable( nStealthIndex ) )
		{ // Задан индекс птички в ключе и птичка выключена
            // AfxMessageBox("Not allowed!!!");
			DeInit();
			return false;
		}
	}
#endif

	// empty key cache
	//ReleaseKeyCache();
	return CFileDataObjectImpl::AfterReadFile();
}

//#include "ObListWrp.h"
bool CNamedData::RestoreRelationObjectsAndClasses()
{
	//DUMMY
	return true;
}

bool CNamedData::RestoreBaseSourceKeys()
{
	//DUMMY
	return true;

	bool bRet = true;
	// get type object_list
	XType *pT = 0;
	for (long nType = 0; nType < GetTypesCount(); nType++) 
	{
		pT = GetType(nType);
		if (!pT)
			continue;

		// for all objects of such type
		for (POSITION pos = pT->GetFirstObjectPosition(); pos != NULL; )
		{
			IUnknown * punk = pT->GetNextObject(pos);
			
			INamedDataObject2Ptr sptrObj = punk;
			if (sptrObj == 0)
				continue;

			GuidKey lPrevBaseKey;

			// get old base_key & src_key
			if (FAILED(sptrObj->GetBaseKey(&lPrevBaseKey)))
			{
				bRet = false;
				continue;
			}

			GuidKey lNewBaseKey;

			// find new key in key_cache by old key
			/*if (lPrevBaseKey != INVALID_KEY)
				lNewBaseKey = ::GetKeyFromCache(lPrevBaseKey);

			if (lPrevSrcKey != INVALID_KEY)
				lNewSrcKey = ::GetKeyFromCache(lPrevSrcKey);

			// change keys
			if (FAILED(sptrObj->SetBaseSourceKey(&lNewBaseKey, &lNewSrcKey)))
			{
				bRet = false;
				continue;
			}*/
		}// for all objects
	} // for all types

	return true;
}

bool CNamedData::DoReadFile( const char *pszFileName, bool bSilent )
{
	try
	{
		// 12.05.2006 build 104
		// Добавляю изврат для "недошифрования"
		//CFile	file( pszFileName, CFile::modeRead );
		CCryptedFile	file( pszFileName, CFile::modeRead );
		file.m_bCrypted = m_bUseProtectedFormat;
		
		VT5File::VT5FileType ft = VT5File::CheckFile(file);
		if (ft == VT5File::CorruptNewVT5File || ft == VT5File::UnknownFile ||
			ft == VT5File::UnsupportedNewVT5File)
			return false;

		CArchive	ar( &file, CArchive::load );

		CArchiveStream arcStream( &ar );

		IStreamPtr ptrStream = (&arcStream);

		if( ptrStream == 0 )
			return false;

		CStreamEx streamEx( ptrStream, true );

		{
			XNotifyLock	lock( this );

			InitNew();

			//TIME_TEST( "m_pEntryRoot->Serialize" )
			if (ft == VT5File::ValidNewVT5File)
				VT5File::SkipHeaderForFileStream(streamEx);
			m_pEntryRoot->Serialize( streamEx );		
		}


//		ar.Flush();
		bool bRead = false;
		// old files not contains context so we must not read context
		try
		{
//			if ((file.GetPosition() + sizeof(int)) < file.GetLength())
//			{
				int nF = 0;
				streamEx >> nF;

				//paul 5.03.2003
				if( nF == -1 || nF == -2 )
				{
					BOOL bcontext_serialize = TRUE;

					if( nF == -2 )
						streamEx >> bcontext_serialize;
					if( bcontext_serialize )
					{
						bRead = LoadContextFromArchive( ptrStream );
						NotifyContexts( ncSyncWithMaster, GetControllingUnknown() );
					}
				}
/*				else
				{
					CFile * pFile = ar.GetFile();
					long nOff = (long)sizeof(int);
					if (pFile)
						pFile->Seek(-nOff, CFile::current);
				}
*/
//			}
		}
		catch (CArchiveException * e)
		{
			e->Delete();
			bRead = false;
		}
		if (!bRead)
		{
			IDataContext2Ptr sptrC = GetDocument();
			if (sptrC)
			{
				sptrC->AttachData(GetDocument());

				LONG_PTR lPos = 0;
				sptrC->GetFirstObjectPos( _bstr_t( szTypeImage ), &lPos );
				if( lPos )
				{
					IUnknown *punk = 0;

					sptrC->GetNextObject( _bstr_t( szTypeImage ), &lPos, &punk );
					sptrC->SetActiveObject( _bstr_t( szTypeImage ), punk, 0 );	

					if( punk )
						punk->Release();
				}
				else
				{
					long lCount = 0;
					sptrC->GetObjectTypeCount( &lCount );
					if( lCount > 0 )
					{
						_bstr_t bstr;
						sptrC->GetObjectTypeName( 0, bstr.GetAddress() );
						if( bstr.length() != 0 )
						{
							LONG_PTR lPos = 0;
							sptrC->GetFirstObjectPos( bstr, &lPos );
							if( lPos )
							{
								IUnknown *punk = 0;

								sptrC->GetNextObject( bstr, &lPos, &punk );
								sptrC->SetActiveObject( bstr, punk, 0 );	

								if( punk )
									punk->Release();
							}

						}
					}
				}
			}
		}

		

		return true;
	}
	catch( CException *pe )
	{
		Unlock();
		if (!bSilent)
			pe->ReportError();
		pe->Delete();

		return false;
	}
}

bool CNamedData::ReadFile( const char *pszFileName )
{
	CFileFind ff;
	if (!ff.FindFile(pszFileName))
		return false;
	bool b = DoReadFile(pszFileName);
	if (!b && GetValueInt(GetAppUnknown(), "General", "RestoreFromBackups", 1))
	{
		CString sBackup(pszFileName);
		sBackup += _T(".bak");
		if (ff.FindFile(sBackup))
		{
			BOOL bSilent = GetValueInt(GetAppUnknown(), "General", "SilentRestoreFromBackups", 0);
			if (bSilent)
				b = DoReadFile(sBackup, true);
			else
			{
				CString sMsg;
				sMsg.Format(IDS_RESTORE_DATA_FILE_FROM_BACKUP, pszFileName, (LPCTSTR)sBackup);
				if (AfxMessageBox(sMsg, MB_YESNO|MB_ICONQUESTION) == IDYES)
					b = DoReadFile(sBackup, true);
				else
					return false;
			}
			if (b)
			{
				::CopyFile(sBackup,pszFileName,FALSE);
			}
			else
			{
				::DeleteFile(sBackup);
				if (!bSilent)
					AfxMessageBox(IDS_CAN_NOT_RESTORE_FROM_BACKUP);
			}
			return b;
		}
	}
	return b;
}

bool CNamedData::BeforeWriteFile()
{
	return true;
}

bool CNamedData::AfterWriteFile()
{
	return true;
}

bool CNamedData::DoWriteFile( const char *pszFileName, BOOL bUnchecked )
{
	if( !pszFileName )	return false;

	try
	{
		//CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
		CCryptedFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
		file.m_bCrypted = m_bUseProtectedFormat;

		CArchive	ar( &file, CArchive::store );
		CArchiveStream arcStream( &ar );

		IStreamPtr ptrStream = (&arcStream);

		if( ptrStream == 0 )
			return false;

		//paul 25.04.2003 cos CArchive raise Abnormal program termination if free space = 0
		{
			char sz_drive[10];	sz_drive[0] = 0;
			::_splitpath( pszFileName, sz_drive, 0, 0, 0 );
			if( strlen( sz_drive ) )
			{
				unsigned __int64 nfree_space = 0;				
				if( ::GetDiskFreeSpaceEx( sz_drive, 0, 0, (PULARGE_INTEGER)&nfree_space ) )
				{
					if( nfree_space <= 0 )	
						CFileException::ThrowOsError( 112, pszFileName );
				}
			}
		}



		CStreamEx streamEx( ptrStream, false );
		VT5File::WriteHeaderToStream(streamEx,bUnchecked);

		m_pEntryRoot->Serialize( streamEx );

		streamEx << (int)-2;
		//paul 5.03.2003
		BOOL bcontext_serialize = FALSE;
		{
			IDataContext2Ptr sptrC = GetDocument();
			if( sptrC )
				bcontext_serialize = TRUE;
		}
		streamEx << bcontext_serialize;

		if (!StoreContextToArchive( ptrStream ))
			return false;		

		streamEx <<CString(_T("End on NamedData archive. VideoTesT 5.0 unified file format.") );

		return true;
	}
	catch (CArchiveException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch( CFileException *pe )
	{
		pe->ReportError();		
		if( pe->m_cause == CFileException::diskFull )
			::DeleteFile( pszFileName );
		pe->Delete();
		return false;
	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
		return false;
	}
}


bool CNamedData::WriteFile( const char *pszFileName )
{
	if( !pszFileName )	return false;
	BOOL bUnchecked = GetValueInt(GetAppUnknown(), "General", "WriteUncheckedFiles", 0);
	CString sTempFileName(pszFileName);
	sTempFileName += _T(".tmp");
	if (DoWriteFile(sTempFileName,bUnchecked)&&
		(bUnchecked||VT5File::UpdateHeader(sTempFileName, m_bUseProtectedFormat)))
	{
		if (IsNeedSaveBackup(pszFileName))
		{
			CString sBakFileName(pszFileName);
			sBakFileName += _T(".bak");
			::DeleteFile(sBakFileName);
			::MoveFile(pszFileName,sBakFileName);
		}
		else
			::DeleteFile(pszFileName);
		::MoveFile(sTempFileName,pszFileName);
		return true;
	}
	else
	{
		::DeleteFile(sTempFileName);
		return false;
	}
}

bool CNamedData::InitNew()
{
	XNotifyLock	lock( this );

	DeInit();

	return true;
}

#include "misc_types.h"
//store entry without subentries to the text array
void StoreEntryToText( CNamedData *pdata, CNamedData::XEntry *pe, CStringArrayEx &sa, bool bStoreSub )
{
//entry - only if not empty
	if( !pe->IsEmpty() && !bStoreSub )
	{
		CString	str, strType, strValue;

		if( pe->m_variant.vt == VT_BSTR )
			strValue = pe->m_variant.bstrVal;
		else if( IsUnknown( pe->m_variant.vt ) )
		{
			sptrINamedDataObject	sptrN( pe->m_variant );
			strValue = ::GetObjectKind( sptrN );
		}
		else if( pe->m_variant.vt == VT_BITS )
		{
			strType = ::VarEnumToString( &pe->m_variant );
			str.Format( "%s:%s=", (const char *)pe->m_strEntry, (const char *)strType );
			sa.Add( str );

			DWORD	*ptr = (DWORD*)pe->m_variant.lVal;
			if( ptr )::MemToText( (byte*)ptr, *ptr+sizeof(DWORD), sa );

			return;
		}
		else
		{
			_variant_t	var;
			_VarChangeType( var, VT_BSTR, &pe->m_variant );
			strValue = var.bstrVal;
		}

		strType = ::VarEnumToString( &pe->m_variant );

		if( strType == TYPE_MULTILINESTRING )
		{
			str.Format( "%s:%s=\n%s\n%s", (const char *)pe->m_strEntry, (const char *)strType, (const char *)strValue, (const char *)END_OF_MLS );
			str.Remove('\r'); // удалим все CR - хватит и LF
			if( !sa[sa.GetSize()-1].IsEmpty() )
				sa.Add( "" );
			sa.Add( str );
			sa.Add( "" );
		}
		else
		{
			ASSERT(pe->m_strEntry.GetLength() > 0);
			str.Format( "%s:%s=%s", (const char *)pe->m_strEntry, (const char *)strType, (const char *)strValue );
			sa.Add( str );
		}

		if( strType == TYPE_UNKNOWN )
		{
			sptrINamedDataObject	sptrN( pe->m_variant );
			bool	bTextSerialized = CheckInterface( sptrN, IID_ITextObject );
			
			BSTR	bstrText = 0;
			if( bTextSerialized )
			{
				sptrITextObject	sptrTO( sptrN );
				if( sptrTO->GetText( &bstrText ) != S_OK )
					bTextSerialized = false;
			}

			if( bTextSerialized )
			{
				CString	s( bstrText );
				::SysFreeString( bstrText );

				sa.Add( METHOD_TEXT );
				sa.Add( s );
				sa.Add( END_OF_MLS );
			}
			else
			{
				//CMemFile		file;

				IStreamPtr ptrStream;
				IStream* pStream = 0;
				::CreateStreamOnHGlobal( 0, TRUE, &pStream );

				if( pStream )
				{
					ptrStream = pStream;
					pStream->Release();	pStream = 0;
				}


				if( ptrStream )
				{
					SerializeParams	params;
					ZeroMemory( &params, sizeof( params ) );
					params.punkNamedData = &pdata->m_xData;


					sptrISerializableObject	sptrSO( sptrN );
					sptrSO->Store( ptrStream, &params );

					HGLOBAL h = 0;
					GetHGlobalFromStream( ptrStream, &h );
					if( h )
					{
						DWORD  dwLen = GlobalSize( h );

						sa.Add( METHOD_BINARY );

						byte	*ptr = (byte*)::GlobalLock( h );
						::MemToText( ptr, dwLen, sa );

						::GlobalUnlock( h );						

					}

				}

			}
		}
		
	}
//subentries
	if( pe->GetEntriesCount() != 0 && bStoreSub )
	{
		CString	strSection;
		CString	strPath = pe->GetPath();

		if( !strPath.IsEmpty() )
		{
			strSection.Format( "[%s]", (const char *)strPath );
			sa.Add( strSection );
		}

		for( int i = 0; i < pe->GetEntriesCount(); i++ )
			::StoreEntryToText( pdata, pe->GetEntry( i ), sa, false );
		for( i = 0; i < pe->GetEntriesCount(); i++ )
			::StoreEntryToText( pdata, pe->GetEntry( i ), sa, true );
		sa.Add( "" );
	}
}

//load from CStringArrayEx 
bool CNamedData::LoadFromText( CStringArrayEx &sa )
{
	CStringArray arrContext;
	XNotifyLock	lock( this );

	InitNew();

	if( sa[0] != NAMED_DATA_SIGN )
		return false;
	//skip 2nd line - possible comment
	
	for( int iPos = 1; iPos < sa.GetSize(); iPos++ )
	{
		CString	s = sa[iPos];

		if( s.IsEmpty() )
			continue;

		if (s == CONTEXT_SIGN)
		{
			for (int iC = iPos + 1; iC < sa.GetSize(); iC++)
			{
				CString str = sa[iC];
				if (str == CONTEXT_END_SIGN)
					break;

				arrContext.Add(str);
			}
			iPos = iC;
			continue;
		}

		if( s[0] == '[' )
		{
			CString	strSectionName = s.Mid( 1, s.GetLength()-2 );
			SetupCurrentSection( strSectionName );
			continue;
		}

		CString	strEntry, strType, strValue;

		//int	nPos1 = s.Find( ":" );
		int	nPos1 = -1;
		int	nPos2 = s.Find( "=" );
		if( nPos2 != -1 )
		{
			for( int i=nPos2;i>=0;i-- )
			{
				if( s[i] == ':' )
				{
					nPos1 = i;
					break;
				}
			}
		}

		if( nPos1 == -1 || nPos2 == -1 || nPos2 < nPos1 )
		{
			TRACE( "Invalid record:%s\n", (const char*)s );
			continue;
		}

		strEntry = s.Left( nPos1 );
		strType = s.Mid( nPos1+1, nPos2-nPos1-1 );
		strValue = s.Right( s.GetLength()-nPos2-1 );

		if( strEntry.IsEmpty()||strType.IsEmpty() )
			return false;

		if( strType == TYPE_MULTILINESTRING )
		{
			bool	b1stTime = true;
			for( iPos++; iPos < sa.GetSize() && sa[iPos] != END_OF_MLS; iPos++ )
			{
				if( !b1stTime )
					strValue += "\r\n";
				b1stTime = false;
				if( iPos == sa.GetSize() )
					return false;
				strValue += sa[iPos];
			}
		}

		if( strType == TYPE_UNKNOWN )
		{
			iPos++;
			bool	bText = sa[iPos++] == METHOD_TEXT;

			IUnknown *punk = ::CreateTypedObject( strValue );

			if( !punk )
				return false;

			::SetObjectName( punk, strEntry );

			sptrINamedDataObject	sptrN( punk );
			punk->Release();

			if( bText )
			{
				bool	b1stTime = true;
				strValue.Empty();
				for( ; sa[iPos] != END_OF_MLS && iPos < sa.GetSize(); iPos++ )
				{
					if( !b1stTime )
						strValue += "\n";
					b1stTime = false;
					if( iPos == sa.GetSize() )
						return false;
					strValue += sa[iPos];
				}
				_bstr_t	bstrText = strValue;

				sptrITextObject	sptrT( sptrN );
				sptrT->SetText( bstrText );
			}
			else
			{
				try
				{
					long	nSize = 0;
					byte	*p = ::TextToMem( sa, iPos, nSize );

					IStreamPtr ptrStream;
					IStream* pStream = 0;
					::CreateStreamOnHGlobal( 0, TRUE, &pStream );

					if( pStream )
					{
						ptrStream = pStream;
						pStream->Release();	pStream = 0;
					}

					if( ptrStream != 0 && p )
					{
						CStreamEx stream( ptrStream, false );
						stream.Write( p, nSize );

						::StreamSeek( ptrStream, 0, 0 );
					}

					SerializeParams	params;
					ZeroMemory( &params, sizeof( params ) );
					params.punkNamedData = &m_xData;

					sptrISerializableObject	sptrS( sptrN );
					sptrS->Load( ptrStream, &params );

					delete p;

				}
				catch( CException *pe )
				{
					pe->ReportError();
					pe->Delete();
				}
			}

			_variant_t	var( (IUnknown*)sptrN );
			_SetValue( strEntry, var );
		}
		else
		{
			VARENUM	vt = ::StringToVarEnum( strType );

			if( vt == VT_BITS ) 
			{
				long	nSize = 0;
				iPos++;
				byte	*p = ::TextToMem( sa, iPos, nSize );

				_variant_t	var;
				var.vt = VT_BITS;
				var.lVal = (long)p;

				_SetValue( strEntry, var );
			}
			else
			{
				_variant_t	var( strValue );

				if( var.vt != vt )
					_VarChangeType( var, vt );

				_SetValue( strEntry, var );
			}
		}
	}

	if (!LoadContextFromText(arrContext))
	{
		IDataContext2Ptr sptrC = GetDocument();
		if (sptrC)
			sptrC->AttachData(GetDocument());
	}

	return true;
}

//store to  CStringArrayEx 
bool CNamedData::StoreToText( CStringArrayEx &sa )
{
	IDocumentSitePtr sptrD = GetControllingUnknown();
	IDataContext2Ptr strC = GetControllingUnknown();
	if (sptrD != 0 && strC != 0)
	{
		IUnknownPtr sptrUV;
		sptrD->GetActiveView(&sptrUV);
		if (sptrUV != 0)
			strC->SynchronizeWithContext(sptrUV);
	}

	sa.Add( NAMED_DATA_SIGN	);
	sa.Add( "" );

	StoreEntryToText( this, m_pEntryRoot, sa, false );
	StoreEntryToText( this, m_pEntryRoot, sa, true );

	CStringArray arrContext;
	StoreContextToText(arrContext);

	sa.Add(CONTEXT_SIGN);
	for (int i = 0; i < arrContext.GetSize(); i++)
		sa.Add(arrContext[i]);

	sa.Add(CONTEXT_END_SIGN);

	return true;
}

bool CNamedData::LoadContextFromText(CStringArray & sa)
{
	IDataContext2Ptr sptrC = GetDocument();
	if (sptrC == 0)
		return true;

	CString strTarget = _T("");
	for (int i = 0; i < sa.GetSize(); i++)
		strTarget += sa[i] + _T('\n');

	_bstr_t bstrString(strTarget);

	if (FAILED(sptrC->LoadFromString(bstrString)))
		return false;

	return true;
}

bool CNamedData::StoreContextToText(CStringArray & sa)
{
	sa.RemoveAll();
	IDataContext2Ptr sptrC = GetDocument();
	if (sptrC == 0)
		return true;

	BSTR bstrString = 0;

	if (FAILED(sptrC->StoreToString(&bstrString)))
		return false;

	CString strContextString = bstrString;
	::SysFreeString(bstrString);

	if (strContextString.IsEmpty())
		return true;

	// add last '\n'
	if (strContextString[strContextString.GetLength() - 1] != _T('\n'))
		strContextString += _T('\n');

	int nFirst = 0;
	int nLast = strContextString.Find(_T('\n'));

	while (nLast != -1)
	{
		CString str = strContextString.Mid(nFirst, nLast - nFirst);
		sa.Add(str);
		nFirst = nLast + 1;
		nLast = strContextString.Find(_T('\n'), nFirst);
	}
	return true;
}

bool CNamedData::LoadContextFromArchive(IStream* pStream)
{
	IDataContext2Ptr sptrC = GetDocument();
	if (sptrC == 0)
		return true;

	//ar.Flush();
	//CArchiveStream stream(&ar);

	if (FAILED(sptrC->LoadFromArchive( pStream ) ) )
		return false;

	return true;
}

bool CNamedData::StoreContextToArchive(IStream* pStream)
{
	IDataContext2Ptr sptrC = GetDocument();
	if (sptrC == 0)
		return true;

	//ar.Flush();

	//CArchiveStream stream(&ar);
	if (FAILED(sptrC->StoreToArchive( pStream ) ) )
		return false;

	return true;
}



bool CNamedData::EnableContextNotification( bool bEnable )
{
	bool bOldNotifyState = m_bContextNotificationEnabled;
	m_bContextNotificationEnabled = bEnable;
	if( bEnable )
		NotifyContexts(ncRescan);

	return bOldNotifyState;
}

bool CNamedData::EnableChildsNotification( bool bEnable )
{
	bool bOldCheckState = m_bEnableCheckChilds;
	m_bEnableCheckChilds = bEnable;

	return bOldCheckState;
}

void CNamedData::Lock()
{
	EnableContextNotification(false);
	EnableChildsNotification(false);
}

void CNamedData::Unlock()
{
	EnableContextNotification(true);
	EnableChildsNotification(true);
}


POSITION CBaseGroup::Find(INamedDataObject2 * pObject)
{
	GuidKey key = ::GetObjectKey(pObject);
	if (key == INVALID_KEY)
		return 0;

	for (POSITION pos = list.GetHeadPosition(); pos != NULL; )
	{
		POSITION PrevPos = pos;
		if (key == ::GetObjectKey(list.GetNext(pos)))
			return PrevPos;
	}
	return 0;
}

POSITION CBaseGroup::FindBase(GuidKey & key)
{
	if (key == INVALID_KEY)
		return 0;

	POSITION posRet = 0;
	for (POSITION pos = list.GetHeadPosition(); pos != NULL; )
	{
		POSITION PrevPos = pos;
		INamedDataObject2Ptr sptr = list.GetNext(pos);
		if (key == ::GetObjectKey(sptr))
			return PrevPos;

		if (!posRet)
		{
			BOOL bBase = false;
			sptr->IsBaseObject(&bBase);
			if (bBase)
				posRet = PrevPos;
		}
	}
	return posRet;
}

//notify contexts, if enabled
void CNamedData::NotifyContexts(NotifyCodes nc, IUnknown * punkNew, IUnknown * punkOld, LPARAM lParam)
{
	INamedDataObject2Ptr sptrNew = punkNew;
	INamedDataObject2Ptr sptrOld = punkOld;

	switch (nc)
	{
	case ncChangeBase: // change base key for given punkNew
		{
			// lparam contains pointer to previous base key
			GuidKey PrevKey = lParam ? *((GUID*)lParam) : INVALID_KEY;//

			if (sptrNew != 0)
			{
				// get new base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// remove object from prev base group (if it's exists)
				if (PrevKey != INVALID_KEY)
				{
					CBaseGroup * pData = 0;
					// get base group
					if (m_mapBase.Lookup(PrevKey, pData) || pData)
					{
						// try to find object in group
						POSITION pos = pData->Find(sptrNew);
						// if found object pos 
						if (pos)
							// remove it from list
							pData->list.RemoveAt(pos);

						// if group is empty
						if (pData->list.GetCount() == 0)
						{
							// remove it
							m_mapBase.RemoveKey(PrevKey);
							delete pData;
						}
					}
				} // key is valid

				// set to group 
				if (Key != INVALID_KEY)
				{
					// get base group
					CBaseGroup * pData = 0;
					if (!m_mapBase.Lookup(Key, pData) || !pData)
					{
						// if group does not exists -> create it
						pData = new CBaseGroup;
						ASSERT (pData != 0);
						m_mapBase.SetAt(Key, pData);
					}

					ASSERT (pData != 0);
					if (pData)
					{
						//[AY]
						if(!pData->list.Find( sptrNew ) )
							pData->list.AddTail(sptrNew);
					}

				} // key is valid
			} // object is valid
		}
		break;

	case ncChangeKey: // change object's key
		{
/*
			if (sptrNew != 0)
			{
				// get base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// remove from group 
				if (Key != INVALID_KEY)
				{
					// get base group
					CBaseGroup * pData = 0;
					if (m_mapBase.Lookup(Key, pData) && pData)
					{
						// try to find object in group
						POSITION pos = pData->Find(sptrOld);
						// if found object pos 
						if (pos)
							// remove it from list
							pData->list.RemoveAt(pos);

						// if group is empty
						if (pData->list.GetCount() == 0)
						{
							// remove it
							m_mapBase.RemoveKey(Key);
							delete pData;
						}
					}
				} // key is valid

				// get base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// set to group 
				if (Key != INVALID_KEY)
				{
					// get base data
					CBaseGroup * pData = 0;
					if (!m_mapBase.Lookup(Key, pData) || !pData)
					{
						// if data does not exists -> create it
						pData = new CBaseGroup;
						m_mapBase.SetAt(Key, pData);
					}
					ASSERT (pData != 0);
					if (pData)
						// add object to group
						pData->list.AddTail(sptrNew);
				} // key is valid
			} // object is valid
*/		}
		break;

	case ncAddObject:// add punkNew
		{
			if (sptrNew != 0)
			{
				// get base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// set to group 
				if (Key != INVALID_KEY)
				{
					// get base data
					CBaseGroup * pData = 0;
					if (!m_mapBase.Lookup(Key, pData) || !pData)
					{
						// if data does not exists -> create it
						pData = new CBaseGroup;
						m_mapBase.SetAt(Key, pData);
					}
					ASSERT (pData != 0);
					if (pData)
					{
						ASSERT(!pData->list.Find( sptrNew ) );
						pData->list.AddTail(sptrNew);
					}
				} // key is valid
			} // object is valid
		}
		break;

	case ncRemoveObject:// remove punkNew
		{
			if (sptrNew != 0)
			{
				// get new base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// remove from group 
				if (Key != INVALID_KEY)
				{
					// get base group
					CBaseGroup * pData = 0;
					if (m_mapBase.Lookup(Key, pData) && pData)
					{
						// try to find object in group
						POSITION pos = pData->Find(sptrNew);
						// if found object pos 
						if (pos)
							// remove it from list
							pData->list.RemoveAt(pos);

						// if group is empty
						if (pData->list.GetCount() == 0)
						{
							// remove it
							m_mapBase.RemoveKey(Key);
							delete pData;
						}
					}
				} // key is valid
			} // object is valid
		}
		break;

	case ncReplaceObject:// replace punkNew to punkOld
		{
			// for old object
			if (sptrOld != 0)
			{
				// get base key
				GuidKey Key;
				sptrOld->GetBaseKey(&Key);

				// remove from group 
				if (Key != INVALID_KEY)
				{
					// get base group
					CBaseGroup * pData = 0;
					if (m_mapBase.Lookup(Key, pData) && pData)
					{
						// try to find object in group
						POSITION pos = pData->Find(sptrOld);
						// if found object pos 
						if (pos)
							// remove it from list
							pData->list.RemoveAt(pos);

						// if group is empty
						if (pData->list.GetCount() == 0)
						{
							// remove it
							m_mapBase.RemoveKey(Key);
							delete pData;
						}
					}
				} // key is valid
			} // object is valid

			if (sptrNew != 0)
			{
				// get base key
				GuidKey Key;
				sptrNew->GetBaseKey(&Key);

				// set to group 
				if (Key != INVALID_KEY)
				{
					// get base data
					CBaseGroup * pData = 0;
					if (!m_mapBase.Lookup(Key, pData) || !pData)
					{
						// if data does not exists -> create it
						pData = new CBaseGroup;
						m_mapBase.SetAt(Key, pData);
					}
					ASSERT (pData != 0);
					if (pData)
					{
						ASSERT(!pData->list.Find( sptrNew ) );
						pData->list.AddTail(sptrNew);
					}
				} // key is valid
			} // object is valid
		}
		break;

	case ncEmpty:
		{
			for (POSITION pos = m_mapBase.GetStartPosition(); pos != NULL; )
			{
				GuidKey key;
				CBaseGroup * pData = 0;
				m_mapBase.GetNextAssoc(pos, key, pData);
				if (pData)
				{
					pData->list.RemoveAll();
					delete pData;
				}
			}
			m_mapBase.RemoveAll();
		}
		break;

	default:
		break;
	}// switch

	if (m_bContextNotificationEnabled)
		m_Contexts.Notify(nc, punkNew, punkOld, lParam);	
}

//ask NamedData objects is data modified
bool CNamedData::IsModified()
{
	if( CFileDataObjectImpl::IsModified() )
		return true;

	for( long nType = 0; nType < GetTypesCount(); nType ++ )
	{
		XType *pT = GetType( nType );

		POSITION	pos = pT->GetFirstObjectPosition();

		while( pos )
		{
			sptrINamedDataObject	sptrO( pT->GetNextObject( pos ) );

			BOOL	bVal = false;

			sptrO->IsModified( &bVal );

			if( bVal )
				return true;
		}
	}
	return false;
}

//reset modified flag for every object
void CNamedData::SetModifiedFlag( bool bSet )
{
	CFileDataObjectImpl::SetModifiedFlag( bSet );

	for( long nType = 0; nType < GetTypesCount(); nType ++ )
	{
		XType *pT = GetType( nType );

		POSITION	pos = pT->GetFirstObjectPosition();

		while( pos )
		{
			sptrINamedDataObject	sptrO( pT->GetNextObject( pos ) );

			sptrO->SetModifiedFlag( bSet );
		}
	}
}

//delete all childs of this data object from NamedData
void CNamedData::_DeleteChilds( IUnknown *punk )
{
	if( !m_bEnableCheckChilds )
		return;
	sptrINamedDataObject2	sptrN( punk );

	TPOS	lPos;

	//bool ret = GetDataDump("e:\\vt5\\vt5\\nd.log", GetControllingUnknown());

	sptrN->GetFirstChildPosition( &lPos );

	while( lPos )
	{
		IUnknown *punkChild = 0;
		sptrN->GetNextChild( &lPos, &punkChild );


		if( !::IsObjectStoredByParent( punkChild ) )
		{
			CString	strPath = ::GetObjectPath( GetDocument(), punkChild);
			::DeleteEntry( GetDocument(), strPath );
		}

		punkChild->Release();
	}
}

//insert all childs of this data object to NamedData
void CNamedData::_InsertChilds( IUnknown *punk )
{
	if( !m_bEnableCheckChilds )
		return;
	sptrINamedDataObject2	sptrN( punk );

	TPOS	lPos;

	sptrN->GetFirstChildPosition( &lPos );

	while( lPos )
	{
		IUnknown *punkChild = 0;
		sptrN->GetNextChild( &lPos, &punkChild );

		if( !::IsObjectStoredByParent( punkChild ) )
		{
			_variant_t	var( punkChild );
			_SetValue( 0, var );
		}

		punkChild->Release();
	}
}

bool CNamedData::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	bool bRet = true;
	m_pEntryRoot->Serialize( ar );


	long lVer = 1;

	if( ar.IsStoring() )
	{
		ar << lVer;
	}
	else
	{
		ar >> lVer;
	}
	/*



	if( ar.IsStoring() )
	{
		ar << (int)-1;
		bRet = StoreContextToArchive(ar);
//		ar<<CString(_T("End on NamedData archive. VideoTesT 5.0 unified file format.") );
	}
	else
	{
		bool bRead = false;
		CFile * pFile = ar.GetFile();

		// old files not contains context so we must not read context
		if (pFile && (pFile->GetPosition() + sizeof(int)) < pFile->GetLength())
		{
			int nF = 0;
			ar >> nF;
			if (nF == -1)
			{
				bRead = LoadContextFromArchive(ar);
			}
			else
			{
				CFile * pFile = ar.GetFile();
				long nOff = (long)sizeof(int);
				if (pFile)
					pFile->Seek(-nOff, CFile::current);
			}
		}
		if (!bRead)
		{
			IDataContext2Ptr sptrC = GetDocument();
			if (sptrC)
				sptrC->AttachData(GetDocument());
		}

		CString	str;
//		ar>>str;
	}
	*/
	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CNamedDataDispWrp
class CNamedDataDispWrp : public CDispatchWrapperBase
{
	DECLARE_DYNCREATE(CNamedDataDispWrp)
	GUARD_DECLARE_OLECREATE(CNamedDataDispWrp)

	CNamedDataDispWrp();           // protected constructor used by dynamic creation
	virtual const IID GetIID()
	{return IID_INamedDataDisp;}

// Implementation
protected:
	virtual ~CNamedDataDispWrp();
};


IMPLEMENT_DYNCREATE(CNamedDataDispWrp, CCmdTargetEx)

// {EFE66482-3AC0-11d3-A604-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CNamedDataDispWrp, "Data.DataDispWrp", 
0xefe66482, 0x3ac0, 0x11d3, 0xa6, 0x4, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

CNamedDataDispWrp::CNamedDataDispWrp()
{
	SetName( _T("Data") );
	_OleLockApp( this );
}

CNamedDataDispWrp::~CNamedDataDispWrp()
{
	_OleUnlockApp( this );
}



VARIANT CNamedData::GetDefValue(LPCTSTR Entry, const VARIANT FAR& DefVal) 
{
	_variant_t	var;

	XEntry *pe = GetEntry( Entry );
	
	if( !pe )return var.Detach();
	if( pe->m_variant.vt == VT_EMPTY )
		pe->m_variant = DefVal;

	var = pe->m_variant;

	return var.Detach();
}

void CNamedData::RemoveValue(LPCTSTR szPath) 
{
	m_xData.DeleteEntry( _bstr_t( szPath ) );
}

BOOL CNamedData::DispWriteTextFile(LPCTSTR fullPath) 
{
	IFileDataObject2Ptr ptrFile( GetControllingUnknown() );
	if( ptrFile == 0 )
		return FALSE;

	m_bUseTextFileFormat = true;

	return ptrFile->SaveTextFile( _bstr_t( fullPath ) ) == S_OK;
}

BOOL CNamedData::DispWriteFile(LPCTSTR fullPath) 
{
	//IFileDataObject2* pFile = IFileDataObject2Ptr( GetControllingUnknown() );
	IFileDataObject2Ptr ptrFile( GetControllingUnknown() );
	if( ptrFile == 0 )
		return FALSE;

	m_bUseTextFileFormat = false;

	return ptrFile->SaveFile( _bstr_t( fullPath ) ) == S_OK;
}

BOOL CNamedData::DispReadFile( LPCTSTR fullPath ) 
{
	IFileDataObject2Ptr ptrFile( GetControllingUnknown() );
	if( ptrFile == 0 )
		return FALSE;

	return ptrFile->LoadFile( _bstr_t( fullPath ) ) == S_OK;
}

BOOL CNamedData::DispWriteProtectedFile(LPCTSTR fullPath) 
{
	//IFileDataObject2* pFile = IFileDataObject2Ptr( GetControllingUnknown() );
	IFileDataObject3Ptr ptrFile( GetControllingUnknown() );
	if( ptrFile == 0 )
		return FALSE;

	m_bUseTextFileFormat = false;

	return ptrFile->SaveProtectedFile( _bstr_t( fullPath ) ) == S_OK;
}

BOOL CNamedData::DispReadProtectedFile( LPCTSTR fullPath ) 
{
	IFileDataObject3Ptr ptrFile( GetControllingUnknown() );
	if( ptrFile == 0 )
		return FALSE;

	return ptrFile->LoadProtectedFile( _bstr_t( fullPath ) ) == S_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool _write_entry(	INamedData* ptrSrc, INamedData* ptrTarget, 
					LPCSTR pszSrcEntry, LPCSTR pszTargetEntry, bool bOvewrite )
{

	if( !ptrSrc || !ptrTarget || !pszSrcEntry || !pszTargetEntry )
		return false;


	_bstr_t bstrPathSrc( pszSrcEntry );
	_bstr_t bstrPathTarget( pszTargetEntry );

	// set section
	if( S_OK != ptrSrc->SetupSection( bstrPathSrc ) )
		return false;
	
	// get count of entries
	long EntryCount = 0;
	ptrSrc->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0;i<(int)EntryCount;i++ )
	{
		try
		{
			BSTR bstr = NULL;
			// get entry name
			
			ptrSrc->GetEntryName( i, &bstr );
			_bstr_t bstrEntryName = bstr;
			::SysFreeString( bstr );	bstr = 0;
			if(0==bstrEntryName.length())
				continue;
			
			// format a full path for entry
			_bstr_t bstrPathNewSrc;			
			int nLenSrc = bstrPathSrc.length();
			if( nLenSrc > 0 && ((char*)bstrPathSrc)[nLenSrc-1] == '\\' )
				bstrPathNewSrc = bstrPathSrc + bstrEntryName;
			else
				bstrPathNewSrc = bstrPathSrc + "\\" + bstrEntryName;

			_bstr_t bstrPathNewTarget;			
			int nLenTarget = bstrPathTarget.length();
			if( nLenTarget > 0 && ((char*)bstrPathTarget)[nLenTarget-1] == '\\' )
				bstrPathNewTarget = bstrPathTarget + bstrEntryName;
			else
				bstrPathNewTarget = bstrPathTarget + "\\" + bstrEntryName;
	
			// get value
			_variant_t var;
			ptrSrc->GetValue( bstrPathNewSrc, &var );

			bool bCanSetValue = true;
			if( !bOvewrite )
			{
				_variant_t _var;
				ptrTarget->GetValue( bstrPathNewTarget, &_var );

				bCanSetValue = ( _var.vt == VT_EMPTY );				
			}
				
			if( bCanSetValue )
			{
				if( var.vt == VT_BITS )
				{								
					long lsz = 0; 
					byte *pByte = GetValuePtr( ptrSrc, bstrPathTarget, bstrEntryName, &lsz );
					*(long*)pByte = lsz;
					var.lVal = (long)pByte;
				}

				ptrTarget->SetupSection( bstrPathNewTarget );
				ptrTarget->SetValue( bstrPathNewTarget, var );
			}
			
			// if this entry has children => we want to walk to them
			if( EntryCount > 0 )
				_write_entry( ptrSrc, ptrTarget, bstrPathNewSrc, bstrPathNewTarget, bOvewrite );

			// for next entry on this level restore Section
			ptrSrc->SetupSection( bstrPathSrc );

		}
		catch(...)
		{
			continue;
		}

	}



	return true;
}

#define EXP_IMP_OVEWRITE	1
#define EXP_IMP_CREATE_NEW	2

/////////////////////////////////////////////////////////////////////////////
BOOL CNamedData::ExportEntry(LPCTSTR pszEntry, LPCTSTR pszFileName, long dwFlags) 
{
	try
	{
		INamedDataPtr ptrTargetND( szNamedDataProgID );

		IFileDataObjectPtr ptrTargetFO( ptrTargetND );

		if( ptrTargetFO == NULL )
			return false;

		if( !( dwFlags & EXP_IMP_CREATE_NEW ) )
		{
			if( S_OK != ptrTargetFO->LoadFile( _bstr_t( pszFileName ) ) )
				return false;
		}

		if( ptrTargetND == 0 )
			return false;

		INamedDataPtr ptrSrc = GetControllingUnknown();
		if( ptrSrc == 0 )
			return false;


		_write_entry( ptrSrc, ptrTargetND, pszEntry, pszEntry, ( dwFlags & 1 ) );


		return ( S_OK == ptrTargetFO->SaveFile( _bstr_t( pszFileName ) ) );

	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();		
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CNamedData::ImportEntry(LPCTSTR pszEntry, LPCTSTR pszFileName, long dwFlags) 
{	
	try
	{
		INamedDataPtr ptrSrcND( szNamedDataProgID );
		IFileDataObjectPtr ptrSrcFO( ptrSrcND );

		if( ptrSrcFO == 0 || ptrSrcND == 0 )
			return false;

		if( S_OK != ptrSrcFO->LoadFile( _bstr_t( pszFileName ) ) )
			return false;

		INamedDataPtr ptrTargetND = GetControllingUnknown();
		if( ptrTargetND == 0 )
			return false;

		_write_entry( ptrSrcND, ptrTargetND, pszEntry, pszEntry, ( dwFlags & 1 ) );
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();		
		return false;
	}

	return true;
}

LONG_PTR CNamedData::GetFirstObjectPos(long ltype) 
{
	LONG_PTR lpos = 0;
	m_xTypes.GetObjectFirstPosition( ltype, &lpos );

	return lpos;
}

LPDISPATCH CNamedData::GetNextObject(long ltype, VARIANT FAR* var_pos) 
{
	if( var_pos->vt != VT_I4 )
		return 0;

	IUnknown* punk = 0;
	m_xTypes.GetNextObject( ltype, (LONG_PTR*)&var_pos->pintVal, &punk );
	if( !punk )	return 0;

	IDispatchPtr ptr_disp = punk;
	punk->Release();	punk = 0;

	if( ptr_disp )	ptr_disp->AddRef();

	return ptr_disp;
}

BOOL CNamedData::CopyNamedData( LPDISPATCH pDispSrc, 
								LPCTSTR pszSrcEntry, LPCTSTR pszTargetEntry, long dwFlags )

{
	INamedDataPtr ptr_src = pDispSrc;
	if( ptr_src == 0 )
	{
		IRootedObjectPtr sptrRooted = pDispSrc;
		if( sptrRooted )
		{
			IUnknown *punkParent = 0;
			sptrRooted->GetParent( &punkParent );
			if( punkParent )
			{
				ptr_src = punkParent;
				punkParent->Release(); punkParent = 0;
			}
		}
	}
	
	INamedDataPtr ptr_target = GetControllingUnknown();
	if( ptr_src == 0 )	return false;

	_write_entry( ptr_src, ptr_target, pszSrcEntry, pszTargetEntry, ( dwFlags & 1 ) );

	return true;
}
