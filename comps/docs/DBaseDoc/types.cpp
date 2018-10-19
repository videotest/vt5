#include "stdafx.h"
#include "types.h"
#include "dbasedoc.h"
#include "docman.h"
#include "afxpriv2.h"

#include "compressint.h"
#include "thumbnail.h"
#include "StreamEx.h"
#include "StreamUtils.h"

/////////////////////////////////////////////////////////////////////////////
//
// _CFieldInfo
//
/////////////////////////////////////////////////////////////////////////////
_CFieldInfo::_CFieldInfo(
	CString strFieldName,
	CString strUserName,	
	FieldType FieldType,		
	CString strPrimaryTable,
	CString strPrimaryFieldName,
	BOOL		bRequiredValue,
	BOOL		bDefaultValue,
	CString		strDefaultValue,
	CString		strVTObjectType,
	BOOL		bCanEditStructure

					   )
{
	m_strFieldName	= strFieldName;
	m_strUserName	= strUserName;	
	m_FieldType		= FieldType;	
	

	m_strPrimaryTable		= strPrimaryTable;
	m_strPrimaryFieldName	= strPrimaryFieldName;	

	m_bRequiredValue	= bRequiredValue;
	m_bDefaultValue		= bDefaultValue;
	m_strDefaultValue	= strDefaultValue;
	m_strVTObjectType	= strVTObjectType;
	m_bCanEditStructure = bCanEditStructure;	

}

/////////////////////////////////////////////////////////////////////////////
_CFieldInfo::_CFieldInfo()
{
	m_FieldType = ftNotSupported;
	m_bRequiredValue = FALSE;
	m_bDefaultValue = FALSE;

	m_bCanEditStructure = FALSE;

}

/////////////////////////////////////////////////////////////////////////////
_CFieldInfo::~_CFieldInfo()
{

}

/////////////////////////////////////////////////////////////////////////////
void _CFieldInfo::CopyFrom( _CFieldInfo* pFieldCopyFrom )
{
	if( pFieldCopyFrom == NULL )
		return;

	m_strFieldName			= pFieldCopyFrom->m_strFieldName;
	m_strUserName			= pFieldCopyFrom->m_strUserName;
	m_FieldType				= pFieldCopyFrom->m_FieldType;
	
	m_strPrimaryTable		= pFieldCopyFrom->m_strPrimaryTable;
	m_strPrimaryFieldName	= pFieldCopyFrom->m_strPrimaryFieldName;

	m_bRequiredValue		= pFieldCopyFrom->m_bRequiredValue;
	m_bDefaultValue			= pFieldCopyFrom->m_bDefaultValue;
	m_strDefaultValue		= pFieldCopyFrom->m_strDefaultValue;
	m_strVTObjectType		= pFieldCopyFrom->m_strVTObjectType;

	m_bCanEditStructure		= pFieldCopyFrom->m_bCanEditStructure;

	m_strFormat				= pFieldCopyFrom->m_strFormat;

}


/////////////////////////////////////////////////////////////////////////////
//
// _CTableInfo
//
/////////////////////////////////////////////////////////////////////////////
_CTableInfo::_CTableInfo( CString strTableName )
{	
	m_strTableName = strTableName;
}

/////////////////////////////////////////////////////////////////////////////
_CTableInfo::~_CTableInfo()
{
	for( int i=0;i<m_arrFieldInfo.GetSize();i++ )
		delete m_arrFieldInfo[i];

	m_arrFieldInfo.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void _CTableInfo::AddFieldInfo( _CFieldInfo* pFI )
{
	ASSERT( pFI );
	m_arrFieldInfo.Add( pFI );
}






/////////////////////////////////////////////////////////////////////////////
//
// CQuerySortField
//
/////////////////////////////////////////////////////////////////////////////
CQuerySortField::CQuerySortField()
{
	m_strTable.Empty();
	m_strField.Empty();
	m_bASC = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CQuerySortField::~CQuerySortField()
{

}

/////////////////////////////////////////////////////////////////////////////
void CQuerySortField::Serialize( CArchive& ar )
{
	long nVersion = 1;

	if( ar.IsLoading() )
	{
		ar >> nVersion;

		ar >> m_strTable;
		ar >> m_strField;				

		ar >> m_bASC;
	}
	else
	{
		ar << nVersion;

		ar << m_strTable;
		ar << m_strField;				

		ar << m_bASC;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
// CQuerySortField
//
/////////////////////////////////////////////////////////////////////////////
CGridColumnInfo::CGridColumnInfo()
{
	m_strTable.Empty();
	m_strField.Empty();

	m_strColumnCaption.Empty();
	m_nColumnWidth = 80;
	
}

/////////////////////////////////////////////////////////////////////////////
CGridColumnInfo::~CGridColumnInfo()
{

}

/////////////////////////////////////////////////////////////////////////////
void CGridColumnInfo::Serialize( CArchive& ar )
{
	long nVersion = 1;

	if( ar.IsLoading() )
	{
		ar >> nVersion;

		ar >> m_strTable;
		ar >> m_strField;				

		ar >> m_strColumnCaption;
		ar >> m_nColumnWidth;
	}
	else
	{
		ar << nVersion;

		ar << m_strTable;
		ar << m_strField;				

		ar << m_strColumnCaption;
		ar << m_nColumnWidth;
	}
}


//static referenceCount = 0;

/////////////////////////////////////////////////////////////////////////////
//
// CQueryField
//
/////////////////////////////////////////////////////////////////////////////
CQueryField::CQueryField()
{		
	m_pMasterField = NULL;
	DeInit();
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::SetMasterField( CQueryField* pMasterField )
{
	m_pMasterField = pMasterField;
}

/////////////////////////////////////////////////////////////////////////////
CQueryField::~CQueryField()
{
	DeInit();
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::InitFrom( CQueryField* pFieldSrc )
{
	if( !pFieldSrc )
		return;
	
	DeInit();
	
	m_strTable			= pFieldSrc->m_strTable;
	m_strField			= pFieldSrc->m_strField;
	m_fieldType			= pFieldSrc->m_fieldType;	
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::ResetContent()
{
	m_var.Clear();
	m_var.vt = VT_NULL;

	m_Tumbnail.DeInit();	
	::ZeroMemory( &m_recordHeader, sizeof(m_recordHeader) );

	m_bModified			= false;

	m_bWasReadTumbnail	= false;
	m_bWasReadHeader	= false;
	m_bWasReadData		= false;

	m_guidRoot			= INVALID_KEY;	
	m_arGuid.RemoveAll();

	m_bBaseObjectEmpty		= true;
	m_bNeedAskUpdateRecord	= false;
	
}


/////////////////////////////////////////////////////////////////////////////
void CQueryField::Init( FieldType fieldType,
				IUnknown* punkDBaseDoc, IUnknown* punkRecordset, int nFieldIndex )
{
	DeInit();

	m_fieldType			= fieldType;
	
	m_dbaseDocPtr		= punkDBaseDoc;
	m_recordsetPtr		= punkRecordset;	
	
	m_nFieldIndex		= nFieldIndex;	
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::DeInit()
{
	//m_pMasterField		= NULL;

	m_dbaseDocPtr		= NULL;
	m_recordsetPtr		= NULL;		

	m_fieldType			= ftNotSupported;

	m_nFieldIndex		= -1;

	ResetContent();

	m_bLockUpdateObject	= false;
}

/////////////////////////////////////////////////////////////////////////////
ADO::FieldPtr CQueryField::GetFieldPtr()
{
	if( m_recordsetPtr == NULL )
		return_and_assert(NULL);

	ADO::FieldsPtr FieldsPtr = m_recordsetPtr->Fields;
	if( FieldsPtr == NULL )
		return_and_assert(NULL);

	int nCount = FieldsPtr->Count;

	if( m_nFieldIndex < 0 || m_nFieldIndex >= nCount )
		return_and_assert(NULL);

	ADO::FieldPtr field = FieldsPtr->GetItem( _variant_t((long)m_nFieldIndex) );

	return field;
}


/////////////////////////////////////////////////////////////////////////////
CString CQueryField::GetAsTableField()
{
	if( !m_strTable.IsEmpty() )
	{
		CString strTemp;
		strTemp.Format( "%s.%s", (LPCTSTR)m_strTable, (LPCTSTR)m_strField );
		return strTemp;
	}
	
	return m_strField;
	
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::Serialize( CArchive& ar )
{
	long nVersion = 1;

	if( ar.IsLoading() )
	{
		ar >> nVersion;
		ar >> m_strTable;
		ar >> m_strField;				
	}
	else
	{
		ar << nVersion;
		ar << m_strTable;
		ar << m_strField;				
	}

}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::TumbnailStore( IStream* pStream )
{
	return m_Tumbnail.Store( pStream );
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::TumbnailLoad( IStream* pStream )
{
	return m_Tumbnail.Load( pStream );
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::TumbnailDraw( CDC* pDC, CRect rcOutput )
{
	return m_Tumbnail.Draw( pDC, rcOutput, true );
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::CreateThumbnail( IUnknown* punkDataObject, CSize sizeTumbnail )
{
	if( punkDataObject == NULL )
		return false;

	return m_Tumbnail.CreateFromDataObject( punkDataObject, sizeTumbnail );
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::GetModifiedFlag( )
{
	bool bModifiedChild = false;
	INamedDataPtr ptrND( GetPrivateNamedData() );
	if( ptrND == NULL )
		return false;//Not Init yet

	{
		_variant_t var;
		ptrND->GetValue( _bstr_t( (LPCSTR)GetFieldFullPath() ), &var );
		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )		
		{
			BOOL bModified = FALSE;
			INamedDataObjectPtr ptrNDO( var.punkVal );
			if( ptrNDO )
			{
				BOOL bModified = FALSE;
					ptrNDO->IsModified( &bModified );

				if( bModified )
					return true;
			}
		}
	}

	
	for( int i=0;i<m_arGuid.GetSize();i++ )
	{
		IUnknown* punkObject = ::GetObjectByKey( ptrND, m_arGuid[i] );
		if( !punkObject )
			continue;		
		
		BOOL bModified = FALSE;
		INamedDataObjectPtr ptrNDO( punkObject );
		punkObject->Release();

		if( ptrNDO )
			ptrNDO->IsModified( &bModified );

		if( bModified )
			return true;
	}
	return m_bModified;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::NeedAskUpdateRecord()
{
//	return /*GetModifiedFlag( ) &&  !m_bBaseObjectEmpty;//*/m_bNeedAskUpdateRecord;
	//[max] wish of Shef & E.I. Never ask on update record 
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::SetModifiedFlag( bool bModified )
{
	m_bModified = bModified;
}


/////////////////////////////////////////////////////////////////////////////
IUnknown* CQueryField::GetRecordset()
{
	if( m_recordsetPtr == NULL )
		return NULL;

	m_recordsetPtr.AddRef();

	return (IUnknown*)m_recordsetPtr;
}



//For quick access
//********************	
/////////////////////////////////////////////////////////////////////////////
bool CQueryField::GetPrivateValue( _variant_t& var) //Load header, Load Thumbnail, Load Object
{	

	if( m_fieldType == ftVTObject || m_fieldType == ftNotSupported )
		return false;

	if( !LoadHeader() )
		return false;

	if( !LoadThumbnail() )
		return false;
	
	if( !LoadObject( false ) )
		return false;

	var = m_var;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void* CQueryField::GetTumbnail()   //Load header, Load Thumbnail
{

	if( m_fieldType != ftVTObject )
		return NULL;

	if( !LoadHeader() )
		return NULL;

	if( !LoadThumbnail() )
		return NULL;

	return (void*)(&m_Tumbnail);
}



//Exchange
//********************
/////////////////////////////////////////////////////////////////////////////
bool CQueryField::Load( bool bPutToNamedData )
{
	if( IsRecordsetEmpty() )
		return false;

	if( !LoadHeader() )
		return false;

	if( !LoadThumbnail() )
		return false;

	if( !LoadObject( bPutToNamedData ) )
		return false;

	return true;
}

bool CQueryField::LoadHeader()
{

	_START_CATCH_REPORT

	if( m_bWasReadHeader )
		return true;

	if( IsRecordsetEmpty() )
		return false;

	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);
	
	if( !( fieldPtr->Attributes & ADO::adFldLong) || (m_fieldType != ftVTObject) )
	{
		m_bWasReadHeader = true;
		return true;
	}

	
	///////////////////////////////
	// 1 Read recordHeader
	::ZeroMemory( &m_recordHeader, sizeof(m_recordHeader) );

	bool bEmpty = false;
	bool bRes = ReadDataFromField( fieldPtr, &m_recordHeader, sizeof(m_recordHeader), &bEmpty );
	if( !bRes && bEmpty )
	{
		//after adding field all record consist empty value
		return true;
	}

	if( !bRes )
		return_and_assert(false);

	m_bBaseObjectEmpty = ( m_recordHeader.bBaseObjectEmpty ? true : false );	

	m_bWasReadHeader = true;

	_END_CATCH_REPORT(false)
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::LoadThumbnail()
{

	if( m_bWasReadTumbnail )
		return true;

	if( IsRecordsetEmpty() )
		return false;

	if( !m_bWasReadHeader )
	{
		return false;
		//return_and_assert(false);
	}

	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);

	
	if( !( fieldPtr->Attributes & ADO::adFldLong) || (m_fieldType != ftVTObject) )
	{
		m_bWasReadTumbnail = true;
		return true;
	}

	///////////////////////////////
	// 2 Read tumbnail		
	
	_variant_t varTData;
	if( S_OK != fieldPtr->raw_GetChunk( m_recordHeader.dwTumbnailSize, &varTData ) )
		return_and_assert(false);

	if( varTData.vt != (VT_ARRAY|VT_UI1) )
		return_and_assert(false);

	if( varTData.parray == NULL )
		return_and_assert(false);

	if( varTData.parray->rgsabound[0].cElements != m_recordHeader.dwTumbnailSize )
		return_and_assert(false);

	BYTE* pSrc = (BYTE*)varTData.parray->pvData;
	if( pSrc == NULL )
		return_and_assert(false);

	IStreamPtr ptrStream;
	
	{
		IStream* ps = 0;
		if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
			return false;

		ptrStream = ps;	ps->Release();	ps = 0;
		
	}

	ULONG uWrite = 0;
	ptrStream->Write( pSrc, (ULONG)m_recordHeader.dwTumbnailSize, &uWrite );

	ASSERT( uWrite == m_recordHeader.dwTumbnailSize );

	::StreamSeek( ptrStream, 0, 0 );
	if( !m_Tumbnail.Load( ptrStream ) )
		return_and_assert(false);

	/*
	CMemFile fileTumbnail( pSrc, (UINT)m_recordHeader.dwTumbnailSize, (UINT)0 );	
	fileTumbnail.SeekToBegin();
	CArchive arTumbnail(&fileTumbnail, CArchive::load );		
	if( m_Tumbnail.Serialize( arTumbnail ) == false )
		return S_FALSE;

	arTumbnail.Flush();		
	*/

	m_bWasReadTumbnail = true;	

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::LoadObject( bool bPutToNamedData )
{

	if( m_bWasReadData )
		return true;

	if( IsRecordsetEmpty() )
		return false;

	if( !m_bWasReadTumbnail )
		return_and_assert(false);


	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);

	if( bPutToNamedData && m_dbaseDocPtr == NULL )
		return_and_assert(false);


	CString strKey;
	strKey.Format( "%s\\%s", (LPCTSTR)SECTION_DBASEFIELDS, GetAsTableField() );	
	_bstr_t bstrKey = strKey;


	if( !( fieldPtr->Attributes & ADO::adFldLong) || (m_fieldType != ftVTObject) )
	{
		//ADO::PositionEnum pos = m_recordsetPtr->AbsolutePosition;
		m_var = fieldPtr->GetValue();

		m_bWasReadData = true;
		if( !bPutToNamedData )		
			return true;		

		INamedDataPtr ptrND( GetPrivateNamedData() );
		if( ptrND == NULL )
			return false;

		ptrND->SetValue( bstrKey, m_var );
		return true;
	}

	if( m_fieldType != ftVTObject )
		return_and_assert(false);

	
	INamedDataPtr ptrND( GetPrivateNamedData() );
	if( ptrND == NULL )
		return_and_assert(false);

	_variant_t varExistValue;
	ptrND->GetValue( bstrKey, &varExistValue );

	if( !( varExistValue.vt == VT_UNKNOWN || varExistValue.vt == VT_DISPATCH ) )
		return_and_assert(false);

	IUnknownPtr ptrObject = varExistValue.punkVal;
	if( ptrObject == NULL )
		return_and_assert(false);


	CString strObjectType = ::GetObjectKind( ptrObject );

	if( m_recordHeader.bVersion >= 3 )
	{
		///////////////////////////////
		// 2 Read RecordList
		_RecordList rl;
		::ZeroMemory( &rl, sizeof( rl ) );

		if( !ReadDataFromField( fieldPtr, &rl, sizeof(rl), 0 ) )
			return false;
		/*
		_variant_t varData = fieldPtr->GetChunk( sizeof(_RecordList) );
		if( varData.vt != (VT_ARRAY|VT_UI1) )
			return_and_assert(false);

		if( varData.parray == NULL )
			return_and_assert(false);

		if( varData.parray->rgsabound[0].cElements != sizeof(_RecordList) )
			return_and_assert(false);

		BYTE* pSrc = (BYTE*)varData.parray->pvData;
		if( pSrc == NULL )
			return_and_assert(false);

		_RecordList rl;
		::ZeroMemory( &rl, sizeof( rl ) );

		CMemFile file( pSrc, (UINT)sizeof(_RecordList), (UINT)0 );
		file.SeekToBegin();
		file.Read( &rl, sizeof(rl) );		
		*/

		strObjectType = rl.szObjectType;

	}

	if( strObjectType != ::GetObjectKind( ptrObject ) )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CString str_warning;
		str_warning.Format( IDS_DIFFERENT_OBJECT_TYPE, (LPCSTR)m_strTable, (LPCSTR)m_strField );
		AfxMessageBox( str_warning, MB_ICONSTOP );
		return false;
	}

	if( !LoadObjectContext( ptrObject, (UINT)m_recordHeader.dwObjectSize, fieldPtr, true ) )
		return false;

	{
		INamedDataObjectPtr ptrNDO( ptrObject );
		if( ptrNDO )
			ptrNDO->SetModifiedFlag( FALSE );

	}
	
	

	
	INamedObject2Ptr spNO2( ptrObject );
	if( spNO2 )	
		spNO2->SetName( _bstr_t( (LPCTSTR)GetAsTableField() ) );

	m_guidRoot = ::GetObjectKey( ptrObject );

	INamedDataObject2Ptr	ptrNamed( ptrObject );
	if( ptrNamed != 0 )ptrNamed->GetBaseKey( &m_guidBase );
	
	m_bWasReadData = true;

	if( !SerializeChilds( true, false ) )
		return false;

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::LoadObjectContext( ISerializableObjectPtr ptrso, UINT dwObjectSize, ADO::FieldPtr fieldPtr, bool bRootObject )
{
	if( ptrso == NULL )
		return_and_assert(false);

	if( fieldPtr == NULL )
		return_and_assert(false);

	if( dwObjectSize == 0 )
		return true;


	_variant_t varData;
	if( S_OK != fieldPtr->raw_GetChunk( dwObjectSize, &varData ) )
		return_and_assert(false);

	if( varData.vt != (VT_ARRAY|VT_UI1) )
		return_and_assert(false);

	if( varData.parray == NULL )
		return_and_assert(false);

	if( varData.parray->rgsabound[0].cElements != dwObjectSize )
		return_and_assert(false);

	BYTE* pSrc = (BYTE*)varData.parray->pvData;				
	if( pSrc == NULL )
		return_and_assert(false);


	IStreamPtr ptrStream;
	
	{
		IStream* ps = 0;
		if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
			return_and_assert(false);

		ptrStream = ps;	ps->Release();	ps = 0;		
	}


	ULONG uWrite = 0;
	ptrStream->Write( pSrc, (ULONG)dwObjectSize, &uWrite );

	ASSERT( uWrite == (ULONG)dwObjectSize );

	::StreamSeek( ptrStream, 0, 0 );

	SerializeParams	params;
	ZeroMemory( &params, sizeof( params ) );

	INamedDataPtr ptr_nd = m_dbaseDocPtr;
	params.punkNamedData = ptr_nd;
	ptrso->Load( ptrStream, &params );


	/*
	CMemFile fileData( (BYTE*)pSrc, (UINT)dwObjectSize, (UINT)0 );	
	fileData.SeekToBegin();
	CArchive arData(&fileData, CArchive::load );				
	
	{
	
		CArchiveStream ars( &arData );
		ptrso->Load( &ars );
	}

	arData.Flush();	
	fileData.Flush();	


	VERIFY( fileData.GetLength() == (DWORD)dwObjectSize );
	*/

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::KillChilds()
{
	INamedDataPtr ptrND( GetPrivateNamedData() );
	if( ptrND == NULL )
		return_and_assert(false);

	for( int i=0;i<m_arGuid.GetSize();i++ )
	{
		IUnknown* punkObject = ::GetObjectByKey( ptrND, m_arGuid[i] );

		if( !punkObject )
			continue;
		_bstr_t bstrEntry = GetObjectPath( ptrND, punkObject );
		ptrND->DeleteEntry( bstrEntry );
		
		punkObject->Release();		
	}

	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::Store()
{
	//Set shel data enable image compression
	_START_CATCH_REPORT

	if( !GetModifiedFlag() )
		return true;

	if( m_fieldType == ftPrimaryKey || m_fieldType == ftNotSupported ) 
	{
		m_bModified = false;
		return true;
	}

	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);

	if( m_dbaseDocPtr == NULL )
		return_and_assert(false);


	CString strKey;
	strKey.Format( "%s\\%s", (LPCTSTR)SECTION_DBASEFIELDS, GetAsTableField() );	
	_bstr_t bstrKey = strKey;


	INamedDataPtr ptrND( GetPrivateNamedData() );
	if( ptrND == NULL )
		return_and_assert(false);

	_variant_t varExistValue;
	ptrND->GetValue( bstrKey, &varExistValue );

	if( varExistValue.vt == VT_EMPTY )
	{
		varExistValue.ChangeType( VT_NULL );
	}


	if( !( fieldPtr->Attributes & ADO::adFldLong) || (m_fieldType != ftVTObject) )
	{		   
		fieldPtr->put_Value( varExistValue );
		m_bModified = false;
		return true;
	}

	if( m_fieldType != ftVTObject )
		return_and_assert(false);

	if(! (varExistValue.vt == VT_UNKNOWN || varExistValue.vt == VT_DISPATCH) )
		return_and_assert(false);

	IUnknownPtr ptrObject = varExistValue.punkVal;
	if( ptrObject == NULL )
		return_and_assert(false);



	IStreamPtr ptrStreamThumbnail = CreateMemoryStream();
	if( ptrStreamThumbnail==0) return false;
	
	m_Tumbnail.Store( ptrStreamThumbnail );
	//	return false;	

	DWORD dwThumbnailSize = ::GetStreamPos( ptrStreamThumbnail );	

	/*
	//Store  Tumbnail
	CMemFile fileTumbnail;
	CArchive arTumbnail(&fileTumbnail, CArchive::store );
	m_Tumbnail.Serialize( arTumbnail );
	arTumbnail.Flush();	
	fileTumbnail.Flush();	
	
	arTumbnail.Close();
	*/




	IStreamPtr ptrStreamData = CreateMemoryStream();
	if( ptrStreamData == 0 )
		return_and_assert(false);

	sptrISerializableObject	sptrO( ptrObject );
	if( sptrO == 0 )
		return_and_assert(false);


	SerializeParams	params;
	ZeroMemory( &params, sizeof( params ) );
	//params.flags = sf_ImageCompressed;
	// 14.06.2005 build 84
	params.flags = 0;
	if( ::GetValueInt( ::GetAppUnknown(), "\\Database\\ImageCompress", "EnableCompress", 1 ) == 1 ) 
		params.flags |= sf_ImageCompressed;

	long lPrevCompression = ::GetValueInt(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 100 );
	long lCompression = ::GetValueInt(GetAppUnknown(), "\\Database\\ImageCompress", "JPGCompressionLevel", lPrevCompression );
	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", lCompression );

	sptrO->Store( ptrStreamData, &params );

	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", lPrevCompression );

	DWORD dwDataSize = ::GetStreamPos( ptrStreamData );	

	
	/*
	//Store  Data
	CMemFile fileData;
	CArchive arData(&fileData, CArchive::store );


	{
		sptrISerializableObject	sptrO( ptrObject );
		if( sptrO )
		{
			CArchiveStream	ars( &arData );
			sptrO->Store( &ars );			
		}
	}
	arData.Flush();	
	fileData.Flush();	
	arData.Close();
	*/

	//prepare guid array
	KillNotSerializeChildGuid();


	m_recordHeader.bVersion = 3;
	m_recordHeader.bBaseObjectEmpty = ( m_bBaseObjectEmpty ? 1 : 0 );
	m_recordHeader.bStructSize = sizeof( m_recordHeader );
	m_recordHeader.bHaveChilds = ( m_arGuid.GetSize() > 0 ? 1 : 0 );
	
	m_recordHeader.dwTumbnailSize = dwThumbnailSize;//fileTumbnail.GetLength();

	m_recordHeader.dwObjectSize   = dwDataSize;//fileData.GetLength();



	//Store  Tumbnail Info
	/*
	IStreamPtr ptrStreamTumbnailInfo;
	
	{
		IStream* ps = 0;
		if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
			return_and_assert(false);

		ptrStreamTumbnailInfo = ps;	ps->Release();	ps = 0;		
	}	


	ULONG uWrite = 0;
	ptrStreamTumbnailInfo->Write( &m_recordHeader, sizeof(m_recordHeader), &uWrite );

	DWORD dwTumbnailInfoSize = ::GetStreamPos( ptrStreamTumbnailInfo );	

	*/
	

	/*
	CMemFile fileTumbnailInfo;
	fileTumbnailInfo.Write( &m_recordHeader, sizeof(m_recordHeader) );
	fileTumbnailInfo.Flush();	
	*/




	_RecordList rlBaseObject;
	::ZeroMemory( &rlBaseObject, sizeof(rlBaseObject) );
	rlBaseObject.wVersion = 1;
	rlBaseObject.dwObjectSize = dwDataSize;/*fileData.GetLength()*/;
	rlBaseObject.bContinue = TRUE;

	CString strObjectType = ::GetObjectKind( ptrObject );

	if( strObjectType.GetLength() < 30 )
		::strcpy( rlBaseObject.szObjectType , (LPCSTR)strObjectType );

	/*
	//Store  base object additional Info
	CMemFile fileBaseObjectInfo;
	fileBaseObjectInfo.Write( &rlBaseObject, sizeof(rlBaseObject) );
	fileBaseObjectInfo.Flush();	
	*/
	/*
	IStreamPtr ptrStreamBaseObjectInfo;
	
	{
		IStream* ps = 0;
		if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
			return_and_assert(false);

		ptrStreamBaseObjectInfo = ps;	ps->Release();	ps = 0;		
	}	

	uWrite = 0;
	ptrStreamBaseObjectInfo->Write( &rlBaseObject, sizeof(rlBaseObject), &uWrite );
	
	DWORD dwBaseObjectInfoSize = ::GetStreamPos( ptrStreamBaseObjectInfo );	
	*/


	if( !WriteDataToField( &m_recordHeader, sizeof(m_recordHeader) ) )
		return false;
	//if( !WriteFromStreamToBlob( ptrStreamTumbnailInfo, 0, dwTumbnailInfoSize ) )
	//	return false;

	if( !WriteFromStreamToBlob( ptrStreamThumbnail, 0, dwThumbnailSize ) )
		return false;

	//if( !WriteFromStreamToBlob( ptrStreamBaseObjectInfo, 0, dwBaseObjectInfoSize ) )
	//	return false;

	if( !WriteDataToField( &rlBaseObject, sizeof(rlBaseObject) ) )
		return false;

	if( !WriteFromStreamToBlob( ptrStreamData, 0, dwDataSize ) )
		return false;

	/*
	if( !WriteFromMemFileToBlob( fileTumbnailInfo ) )
		return false;
	
	if( !WriteFromMemFileToBlob( fileTumbnail ) )
		return false;

	if( !WriteFromMemFileToBlob( fileBaseObjectInfo ) )
		return false;


	if( !WriteFromMemFileToBlob( fileData ) )
		return false;
		*/

	INamedDataObjectPtr ptrNDO( ptrObject );
	if( ptrNDO )
		ptrNDO->SetModifiedFlag( FALSE );

	if( !SerializeChilds( false, true ) )
		return false;

	
	_END_CATCH_REPORT(false)

	m_bModified = false;


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::KillNotSerializeChildGuid()
{
	INamedDataPtr ptrND = GetPrivateNamedData();

	if( ptrND == NULL)
		return_and_assert( false );

	for( INT_PTR i=m_arGuid.GetSize()-1;i>=0;i-- )
	{
		IUnknown* punkObject = ::GetObjectByKey( ptrND, m_arGuid[i] );
		if( !punkObject )
		{
			m_arGuid.RemoveAt(i);
			continue;
		}

		if( !::CheckInterface( punkObject, IID_ISerializableObject) )
			m_arGuid.RemoveAt(i);
		
		punkObject->Release();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::SerializeChilds( bool bLoad, bool bClearModifiedFlag )
{
	if( m_fieldType != ftVTObject )
		return_and_assert(false);

	//prepare guid array
	KillNotSerializeChildGuid();	

	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);

	if( !( fieldPtr->Attributes & ADO::adFldLong) )
		return_and_assert(false);

	INamedDataPtr ptrND = GetPrivateNamedData();

	if( ptrND == NULL)
		return_and_assert( false );

	if( !bLoad )
	{		
		for( int i=0;i<m_arGuid.GetSize();i++ )
		{
			bool bLastChild = ( i == (m_arGuid.GetSize() - 1) );
			
			_RecordList rl;
			::ZeroMemory( &rl, sizeof( rl ) );
			
			IUnknown* punkObj = ::GetObjectByKey( ptrND, m_arGuid[i] );
			if( punkObj == NULL ) 
				continue;

			sptrISerializableObject ptrso( punkObj );
			punkObj->Release();	punkObj = 0;

			if( ptrso == NULL )
				continue;


			IStreamPtr ptrStreamData;
			
			{
				IStream* ps = 0;
				if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
					return_and_assert(false);

				ptrStreamData = ps;	ps->Release();	ps = 0;		
			}


			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );
			params.flags = sf_ImageCompressed;

			ptrso->Store( ptrStreamData, &params );
			DWORD dwDataSize = ::GetStreamPos( ptrStreamData );	


			/*
			//Store  Data
			CMemFile fileData;
			CArchive arData(&fileData, CArchive::store );			
			{
				CArchiveStream	ars( &arData );
				ptrso->Store( &ars );			
			}
			arData.Flush();	
			fileData.Flush();	
			arData.Close();
			*/

			CString strObjectType = ::GetObjectKind( ptrso );

			//Fill info struct
			rl.wVersion		= 1;

			if( strObjectType.GetLength() < 30 )
				::strcpy( rl.szObjectType , (LPCSTR)strObjectType );

			rl.dwObjectSize = dwDataSize;/*fileData.GetLength()*/;
			rl.bContinue = !bLastChild;

			/*
			CMemFile fileInfo;
			fileInfo.Write( &rl, sizeof(rl) );
			fileInfo.Flush();			

			if( !WriteFromMemFileToBlob( fileInfo ) )
				continue;
				

			if( !WriteFromMemFileToBlob( fileData ) )
				continue;
			*/

			if( !WriteDataToField( &rl, sizeof(rl) ) )
				continue;

			if( !WriteFromStreamToBlob( ptrStreamData, 0, dwDataSize ) )
				continue;

			if( bClearModifiedFlag )
			{
				INamedDataObjectPtr ptrNDO( ptrso );
				if( ptrNDO )
					ptrNDO->SetModifiedFlag( FALSE );
			}


		}
	}
	else//Loading
	{
		//test for prev version and need
		if( m_recordHeader.bVersion < 2 )
			return true;

		if( !m_recordHeader.bHaveChilds )
			return true;

		VERIFY( m_arGuid.GetSize() == 0 );

		bool bContinue = true;
		//Load header
		while( bContinue )
		{
			_RecordList rl;
			::ZeroMemory( &rl, sizeof( rl ) );
			if( !ReadDataFromField( fieldPtr, &rl, sizeof( rl ), 0 ) )
				return_and_assert(false);
			
			/*
			_variant_t varhdr = fieldPtr->GetChunk( sizeof(rl) );				

			if( varhdr.vt != (VT_ARRAY|VT_UI1) )
				return_and_assert(false);

			if( varhdr.parray == NULL )
				return_and_assert(false);

			if( varhdr.parray->rgsabound[0].cElements != sizeof(rl) )
				return_and_assert(false);

			BYTE* pSrc = (BYTE*)varhdr.parray->pvData;				
			if( pSrc == NULL )
				return_and_assert(false);


			::memcpy( &rl, pSrc, sizeof(rl) );
			*/
			
			bContinue = ( rl.bContinue == TRUE );

			IUnknown* punkObject = NULL;

			if( punkObject == NULL )
				punkObject = ::CreateTypedObject( rl.szObjectType );	

			if( !punkObject )
				return_and_assert(false);

			AttachData2Object( punkObject );

			IUnknownPtr ptrObj = punkObject;
			punkObject->Release();	punkObject = 0;

			if( !LoadObjectContext( ptrObj, (UINT)rl.dwObjectSize, fieldPtr ) )
				return_and_assert(false);

			CString strObjectName;
			INamedObject2Ptr ptrNO( ptrObj );
			if( ptrNO )
			{
				BSTR bstrObjectName = 0;
				ptrNO->GetName( &bstrObjectName );
				strObjectName = bstrObjectName;

				if( bstrObjectName )
					::SysFreeString( bstrObjectName );
			}

			INamedDataObject2Ptr	ptrNamed( ptrObj );
			if( ptrNamed != 0 )ptrNamed->SetBaseKey( &m_guidBase );


			m_arGuid.Add( ::GetObjectKey(ptrObj) );

			//Put to named data
			_bstr_t bstrPath;
			bstrPath = SECTION_DBCHILDOBJECTS;
			bstrPath += "\\";
			bstrPath += (LPCSTR)GetAsTableField();
			bstrPath += "\\";
			bstrPath += (LPCSTR)strObjectName;
			
			ptrND->SetValue( bstrPath, _variant_t((IUnknown*)ptrNO) );
		}
	}

	return true;	
	
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::WriteDataToField( void* pData, DWORD dwSize )
{

	IStreamPtr ptrStream;
	
	{
		IStream* ps = 0;
		if( S_OK != ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
			return_and_assert(false);

		ptrStream = ps;	ps->Release();	ps = 0;		
	}	

	ULONG uWrite = 0;
	ptrStream->Write( pData, (ULONG)dwSize, &uWrite );
	ASSERT( uWrite == dwSize );		
	ASSERT( uWrite == ::GetStreamPos( ptrStream ) );		

	return WriteFromStreamToBlob( ptrStream, 0, dwSize );
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::ReadDataFromField( ADO::FieldPtr fieldPtr, void* pData, DWORD dwSize, bool* pbEmpty )
{
	if( fieldPtr == 0 )
		return_and_assert(false);

	_variant_t varTI;
	fieldPtr->raw_GetChunk( (long)dwSize, &varTI );
	
	//_START_CATCH_REPORT
	//varTI = fieldPtr->GetChunk( dwSize );
	//_END_CATCH_REPORT(true)
	
	//after adding field all record consist empty value
	if( varTI.vt == VT_NULL )
	{	
		if( pbEmpty )//Don't need save empty object
			*pbEmpty = true;
		return false;
	}
	
	if( varTI.vt != (VT_ARRAY|VT_UI1) )
		return_and_assert(false);

	if( varTI.parray == NULL )
		return_and_assert(false);

	if( varTI.parray->rgsabound[0].cElements != dwSize )
		return_and_assert(false);

	BYTE* pSrc = (BYTE*)varTI.parray->pvData;				
	if( pSrc == NULL )
		return_and_assert(false);

	::memcpy( pData, pSrc, dwSize );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::WriteFromStreamToBlob( IStream* pStream, DWORD dwFromPos, DWORD dwSize )
{	
	//DWORD dwSize = file.GetLength();
	if( dwSize <= 0 )
		return true;

	if( !::StreamSeek( pStream, 0, dwFromPos ) )
		return false;


	SAFEARRAY FAR *psa;
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = dwSize;
	psa = ::SafeArrayCreate(VT_UI1,1,rgsabound);
	if( psa == NULL )
		return false;


	void HUGEP* FAR  pvData  = NULL;
	//BYTE* pbSrc =  file.Detach();	
	if( S_OK != ::SafeArrayAccessData( psa, &pvData ) )
		return false;

	if( !pvData )
		return false;

	//memcpy( pvData, pbSrc, dwSize );

	ULONG uRead = 0;
	pStream->Read( pvData, (ULONG)dwSize, &uRead );
	ASSERT( uRead == uRead );

	if( S_OK != SafeArrayUnaccessData(psa) )
		return false;


	_variant_t varChunk;

	varChunk.vt = VT_ARRAY|VT_UI1;
    varChunk.parray = psa;	


	bool bResult = false;

	ADO::FieldPtr fieldPtr = GetFieldPtr();
	if( fieldPtr == NULL )		
		return_and_assert(false);

	try{
	fieldPtr->AppendChunk( varChunk );
	bResult = true;
	}
	catch(...){}

		
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
INamedDataPtr CQueryField::GetPrivateNamedData()
{
	if( m_dbaseDocPtr == NULL )
		return NULL;

	IUnknown* punkPrivateND = NULL;
	m_dbaseDocPtr->GetPrivateNamedData( &punkPrivateND );
	
	if( punkPrivateND == NULL )
		return NULL;

	INamedDataPtr ptrND( punkPrivateND );
	punkPrivateND->Release();

	return ptrND;
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::InitDefValues( bool bAfterInsert )
{
	KillChilds();	
	ResetContent();
	
	sptrINamedData spND = GetPrivateNamedData();
	if( spND == NULL )
		return_and_assert( false );
	
	_bstr_t bstrPath;

	CString strTableField = GetAsTableField(); 
	_variant_t varDefValue;
	CString strObjectType;


	if( m_fieldType == ftDigit || m_fieldType == ftString || m_fieldType == ftDateTime )
	{

		VARIANT varIsDefValue={VT_EMPTY};
		bstrPath = SECTION_FIELD_DEFAULT_VALUES_CHECK;
		bstrPath += "\\";
		bstrPath += (LPCSTR)strTableField;

		spND->GetValue( bstrPath, &varIsDefValue );

		if( varIsDefValue.vt == VT_I4 && varIsDefValue.lVal == 1 )
		{						

			bstrPath = SECTION_FIELD_DEFAULT_VALUES;
			bstrPath += "\\";
			bstrPath += (LPCSTR)strTableField;
			spND->GetValue( bstrPath, &varDefValue );

			if( m_fieldType == ftDigit )
			{
				if( ::VariantChangeType( &varDefValue, &varDefValue, 0, VT_R8 ) != S_OK )
					varDefValue = _variant_t( 0.0 );
			}
			else
			if( m_fieldType == ftString )
			{
				if( ::VariantChangeType( &varDefValue, &varDefValue, 0, VT_BSTR ) != S_OK )
					varDefValue = _variant_t( 0.0 );
			}
		}

		//always for fields DateTime set current time
		if( m_fieldType == ftDateTime )
		{					
			COleDateTime dt( COleDateTime::GetCurrentTime() );
			CString str = dt.Format( VAR_DATEVALUEONLY );
			varDefValue = str;
			varDefValue.ChangeType( VT_DATE );
		}

	}

	if( m_fieldType != ftVTObject )
	{													

		bstrPath = SECTION_DBASEFIELDS;
		bstrPath += "\\";
		bstrPath += (LPCSTR)strTableField;

		spND->SetValue( bstrPath, varDefValue );			
	}
	else
	{
		strObjectType = ::GetFieldVTObjectType( spND, m_strTable, m_strField );
		
		IUnknown* pUnk = NULL;
		pUnk = ::CreateTypedObject( strObjectType );	

		if( pUnk )
		{			
			AttachData2Object( pUnk );

			INamedObject2Ptr spNO2(pUnk);
			if( spNO2 )
			{												
				spNO2->SetName( _bstr_t( (LPCTSTR)strTableField ) );								
			}			
			
			bstrPath = SECTION_DBASEFIELDS;
			bstrPath += "\\";
			bstrPath += (LPCSTR)strTableField;

			m_guidRoot = ::GetObjectKey( pUnk );
			INamedDataObject2Ptr	ptrNamed( pUnk );
			if( ptrNamed != 0 )ptrNamed->GetBaseKey( &m_guidBase );

			if( bAfterInsert )
			{
				bool bWasSetValue;				
				OnSetValue( bstrPath, _variant_t( (IUnknown*)pUnk ), true, bWasSetValue );
			}
			else			
				spND->SetValue( bstrPath, _variant_t( (IUnknown*)pUnk ) );


			pUnk->Release();
		}
		else
			ASSERT( false );
	}


	if( bAfterInsert )
	{
		SetModifiedFlag( true );
		m_bWasReadHeader = m_bWasReadTumbnail = m_bWasReadData = true;
		m_bBaseObjectEmpty = true;		
		m_bNeedAskUpdateRecord = false;
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::RecordChange()
{	
	KillChilds();
	ResetContent();

	if( IsRecordsetEmpty() )
	{
		InitDefValues( false );
		SetModifiedFlag( false );
		m_bWasReadHeader = m_bWasReadTumbnail = m_bWasReadData = false;
		return true;
	}

	bool bRes = Load( true );

	if( m_fieldType != ftVTObject )
		return true;

	if( !bRes )//field data len = 0
	{
		InitDefValues( true );
		SetModifiedFlag( false );
		m_bWasReadHeader = m_bWasReadTumbnail = m_bWasReadData = false;
		return true;
	}

	INamedDataPtr ptrPrivND = GetPrivateNamedData();
	if( ptrPrivND == NULL )
		return_and_assert( false );


	_variant_t var;
	ptrPrivND->GetValue( _bstr_t( (LPCSTR)GetFieldFullPath() ), &var );
	if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
	{
		m_bLockUpdateObject = true;
		::UpdateDataObject( m_dbaseDocPtr, var.punkVal );		
		m_bLockUpdateObject = false;
	}
	else
		return_and_assert( false );
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::Cancel()
{	
	bool bRes = RecordChange();

	sptrIDBaseDocument spDBDoc = m_dbaseDocPtr;
	INamedDataPtr ptrPrivND = GetPrivateNamedData();

	if( ptrPrivND == NULL || spDBDoc == NULL )
		return_and_assert( false );


	_variant_t varOld;
	ptrPrivND->GetValue( _bstr_t( (LPCSTR)GetFieldFullPath() ), &varOld );

	spDBDoc->FireEvent( _bstr_t(szDBaseEventFieldChange), spDBDoc, spDBDoc, 
								_bstr_t( (LPCSTR)GetTableName() ), 
								_bstr_t( (LPCSTR)GetFieldName() ), varOld );

	return bRes;
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryField::OnGetValue( BSTR bstrName, tagVARIANT *pvar )
{
	sptrIDBaseDocument spDBDoc = m_dbaseDocPtr;
	INamedDataPtr ptrPrivND = GetPrivateNamedData();

	//Not init yet
	if( ptrPrivND == NULL || spDBDoc == NULL )
		return false;


	CString strName = bstrName;

	if( strName.IsEmpty() )
		return false;

	CString strSection;
	{
		BSTR bstrSection = 0;
		ptrPrivND->GetCurrentSection( &bstrSection );
		strSection = bstrSection;
	}	
	
	CString strFullPath;
	if( !strName.IsEmpty() )
	{
		if( strName[0] == '\\' )
		{
			strFullPath = strName;
		}
		else
		{
			strFullPath = strSection + strName;
		}		
	}
	else
	{
		return_and_assert( false );
	}

	if( strFullPath == GetFieldFullPath() )
	{
		Load( true );
		return true;
	}


	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::OnSetValue( BSTR bstrName, const tagVARIANT var, bool bNeedSetValue, 
							  bool& bWasSetValue )
{
	
	sptrIDBaseDocument spDBDoc = m_dbaseDocPtr;
	INamedDataPtr ptrPrivND = GetPrivateNamedData();

	if( ptrPrivND == NULL || spDBDoc == NULL )
		return_and_assert( false );


	CString strName = bstrName;
	CString strSection = "\\";
	/*
	{
		BSTR bstrSection = 0;
		ptrPrivND->GetCurrentSection( &bstrSection );
		strSection = bstrSection;

		if( bstrSection )
			::SysFreeString( bstrSection );
	}	
	*/
	

	IUnknownPtr ptrObject;
	CString strObjectName;
	CString strObjectType;
	
	CString strOriginalObjectType = ::GetFieldVTObjectType( ptrPrivND, m_strTable, m_strField );

	if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
	{
		ptrObject = var.punkVal;
	}
	else
	{
		if( var.vt == ( VT_VARIANT|VT_BYREF) && var.pvarVal->vt == VT_DISPATCH )
		{
			ptrObject = var.pvarVal->pdispVal;
		}
	}



	if( ptrObject )
	{
		////AxForm, Report ... 
		//if( ::IsOwnDataObject( var ) && ptrObject != NULL )
		//	return false;

		INamedObject2Ptr spNO2( ptrObject );
		if( spNO2 )
		{
			BSTR bsrtObjectName = 0;
			spNO2->GetName( &bsrtObjectName );
			strObjectName = bsrtObjectName;

			if( bsrtObjectName )
			::SysFreeString( bsrtObjectName );
		}

		strObjectType = ::GetObjectKind( ptrObject );
	}


	CString strFullPath;
	if( !strName.IsEmpty() )
	{
		if( strName[0] == '\\' )
		{
			strFullPath = strName;
		}
		else
		{
			if( ptrObject )
				strFullPath = strSection + strObjectName;
			else
				strFullPath = strSection + strName;
		}		
	}
	else
	{
		if( ptrObject )
		{
			strFullPath = strSection + strObjectName;
		}
		else
			return_and_assert( false );
	}


	CString strTable, strField;

	

	//Process not object field
	if( ptrObject == NULL )
	{		
		if( GetFieldFullPath() != strFullPath )
			return false;

		if( IsRecordsetEmpty() )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_RECORDSET_EMPTY, MB_ICONSTOP );
			return true;
		}

		SetModifiedFlag( true );

		_variant_t _var( var );

		if( _var.vt != VT_NULL )
		{
			try
			{
				if( m_fieldType == ftDigit )
					_var.ChangeType( VT_R8 );
				else if( m_fieldType == ftString )
					_var.ChangeType( VT_BSTR );
				else if( m_fieldType == ftDateTime )
				{
					COleDateTime dt( var );										
					CString str = dt.Format( VAR_DATEVALUEONLY );
					_var = str;
					_var.ChangeType( VT_DATE );
				}
				else
				{
					bWasSetValue = true;//do nothing
					return true;
				}
			}
			catch(...)
			{
				
			}
		}

		ptrPrivND->SetValue( bstrName, _var );
		bWasSetValue = true;

		m_var = _var;//for cache fields

		spDBDoc->FireEvent( _bstr_t(szDBaseEventFieldChange), spDBDoc, spDBDoc,
						_bstr_t( (LPCTSTR)m_strTable ), 
						_bstr_t( (LPCTSTR)m_strField ), _var );
		

		return true;
	}

	if( m_fieldType != ftVTObject )
		return false;


	

	bool bChild = false;
	INamedDataObject2Ptr ptrNDO2( ptrObject );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
		{
			bChild = true;
			punkParent->Release();
		}
	}
			

	if( strOriginalObjectType.IsEmpty() || strObjectType.IsEmpty() )
		return false;

	GuidKey prevKey = GetObjectKey( ptrObject );
	
	
	//OnNewObjectSet2Data( spDBDoc, ptrObject );

	//Process root object
	if( !bChild && ( strOriginalObjectType == strObjectType ) && 
		( ( strObjectName == GetAsTableField() ) || (CString(bstrName) == GetFieldFullPath() ) )  
		)
	{

		if( IsRecordsetEmpty() )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_RECORDSET_EMPTY, MB_ICONSTOP );
			return true;
		}


		IUnknownPtr ptrPreObject;
		{
			IUnknown* punk = 0;
			punk = ::GetObjectByName( 
						ptrPrivND, GetAsTableField(), strOriginalObjectType );
			if( punk )
			{
				ptrPreObject = punk;
				punk->Release();	punk = 0;
			}
		}

		//[AY]
		/*GuidKey	guidNewBase;
		{
			INamedDataObject2Ptr	ptrNamed( ptrObject );
			ptrNamed->GetBaseKey( &guidNewBase );

			if( guidNewBase != m_guidBase )
				KillChilds();
		}*/


		INamedObject2Ptr ptrO( ptrObject );
		if( ptrO )
			ptrO->SetName( _bstr_t( GetAsTableField() ) );
		
		if( !m_bBaseObjectEmpty )
			m_bNeedAskUpdateRecord = true;

		m_bBaseObjectEmpty = false;
		SetModifiedFlag( true );		

		//was active prev?
		/*
		BOOL bActivateFlag = 0;
		IDataContext2Ptr ptrDataContext;
		if( ptrPreObject )
		{
			IDocumentSitePtr ptrDS( m_dbaseDocPtr );
			if( ptrDS )
			{
				IUnknown* punkV = 0;
				ptrDS->GetActiveView( &punkV );
				if( punkV )
				{
					ptrDataContext = punkV;
					punkV->Release();
					punkV = 0;				
				}

				if( ptrDataContext )
					ptrDataContext->GetObjectSelect( ptrPreObject, &bActivateFlag );
			}
		}
		*/

		//DeleteObject( ptrPrivND, ptrPreObject );

		if( bNeedSetValue )
		{
			ptrPrivND->SetValue( _bstr_t( (LPCSTR)GetFieldFullPath() ), _variant_t( (IUnknown*)ptrObject)  );
			bWasSetValue = true;


			/*28.03.2002	try kill GenerateNewKey
			INumeredObjectPtr ptrNumObj( ptrObject );
			if( ptrNumObj )
				ptrNumObj->GenerateNewKey( 0 );


			OnNewObjectSet2Data( spDBDoc, ptrObject, prevKey, bActivateFlag, ptrDataContext );
			*/
		}

		spDBDoc->FireEvent( _bstr_t(szDBaseEventFieldChange), spDBDoc, spDBDoc,
						_bstr_t( (LPCTSTR)m_strTable ), 
						_bstr_t( (LPCTSTR)m_strField ), _variant_t( (IUnknown*)ptrObject) );

		CSize sizeThumbnail( 100, 100 );
		{
			IUnknown* punkq = NULL;
			spDBDoc->GetActiveQuery( &punkq );
			if( punkq )
			{
				ISelectQueryPtr ptrQ( punkq );
				punkq->Release(); punkq = 0;
				if( ptrQ )
				{
					ptrQ->GetTumbnailSize( &sizeThumbnail );
				}
			}

		}		

		m_guidRoot = ::GetObjectKey( ptrObject );		
		INamedDataObject2Ptr	ptrNamed( ptrObject );
		if( ptrNamed != 0 )ptrNamed->GetBaseKey( &m_guidBase );

		CreateThumbnail( ptrObject, sizeThumbnail );
		spDBDoc->FireEvent( _bstr_t( szDBaseEventNewTumbnailGenerate ), spDBDoc, spDBDoc, NULL, NULL, _variant_t((long)1) );


		return true;
	}
	
	
	//Process child	
	strObjectName = GetObjectName( ptrObject );
	
	if( bNeedSetValue )	
	{
		bWasSetValue = true;

		if( CString(bstrName) == GetFieldFullPath() )
		{
			CString strPath;
			strPath.Format( "%s\\Temporary Trash\\%s", SECTION_DBCHILDOBJECTS, strObjectName );

			ptrPrivND->SetValue( _bstr_t( (LPCSTR)strPath ), _variant_t( (IUnknown*)ptrObject) );	

			/*28.03.2002	try kill GenerateNewKey
			INumeredObjectPtr ptrNumObj( ptrObject );
			if( ptrNumObj )
				ptrNumObj->GenerateNewKey( 0 );
			
			OnNewObjectSet2Data( spDBDoc, ptrObject, prevKey, 0, 0 );
			*/
		}
		else
		{

			ptrPrivND->SetValue( bstrName, _variant_t( (IUnknown*)ptrObject) );	

			/*28.03.2002	try kill GenerateNewKey
			INumeredObjectPtr ptrNumObj( ptrObject );
			if( ptrNumObj )
				ptrNumObj->GenerateNewKey( 0 );

			
			OnNewObjectSet2Data( spDBDoc, ptrObject, prevKey, 0, 0 );
			*/
		}

		//Key was change
		prevKey = GetObjectKey( ptrObject );
		
	}
	
	

	if( IsChild( ptrObject ) || 
		( CString(bstrName) == GetFieldFullPath() && ( strOriginalObjectType != strObjectType ) )
		)
	{

		if( IsRecordsetEmpty() )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_RECORDSET_EMPTY, MB_ICONSTOP );
			return true;
		}

		SetModifiedFlag( true );
		
		ptrPrivND->DeleteEntry( _bstr_t( ::GetObjectPath( ptrPrivND, ptrObject ) ) );
	

		GenerateUniqueNameForObject( ptrObject, ptrPrivND );
		strObjectName = GetObjectName( ptrObject );

		/*long lExist = 1;
		ptrPrivND->NameExists( _bstr_t( (LPCSTR)strObjectName ), &lExist );
		
		if( strObjectName.IsEmpty() || lExist )
		{
			BSTR bstrUniqName = 0;
			ptrPrivND->CreateUniqueName( _bstr_t( (LPCSTR)strObjectType ), &bstrUniqName );
			strObjectName = bstrUniqName;
			::SysFreeString( bstrUniqName  );

			VERIFY( SetObjectName( ptrObject, (LPCSTR)strObjectName ) );

		}*/
			

		//Correct path
		_bstr_t bstrPath;
		bstrPath = SECTION_DBCHILDOBJECTS;
		bstrPath += "\\";
		bstrPath += (LPCSTR)GetAsTableField();
		bstrPath += "\\";
		bstrPath += (LPCSTR)strObjectName;

		
		ptrPrivND->SetValue( bstrPath, _variant_t( (IUnknown*)ptrObject) );
		bWasSetValue = true;

		/*28.03.2002	try kill GenerateNewKey
		INumeredObjectPtr ptrNumObj( ptrObject );
		if( ptrNumObj )
			ptrNumObj->GenerateNewKey( 0 );

		
		OnNewObjectSet2Data( spDBDoc, ptrObject, prevKey, 0, 0 );
		*/

		AttachData2Object( ptrObject );		

		m_arGuid.Add( ::GetObjectKey( ptrObject ) );


		spDBDoc->FireEvent( _bstr_t(szDBaseEventFieldChange), spDBDoc, spDBDoc,
						_bstr_t( (LPCTSTR)m_strTable ), 
						_bstr_t( (LPCTSTR)m_strField ), _variant_t( (IUnknown*)ptrObject) );


		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::ProcessDataObjectChange( IUnknown* punkObject )
{
	if( m_bLockUpdateObject )
		return true;//force exit

	if( m_fieldType != ftVTObject )
		return false;

	INamedObject2Ptr ptrNO2( punkObject );
	if( ptrNO2 == 0 )
		return true;//force exit

	BSTR bstrName = 0;
	ptrNO2->GetName( &bstrName );

	bool bOwnRootObject = ( GetAsTableField() == bstrName );	

	if( bstrName )
		::SysFreeString( bstrName );	bstrName = 0;

	if( !bOwnRootObject )
		return false;

	CSize sizeThumbnail( 100, 100 );
	{
		IUnknown* punkq = NULL;
		m_dbaseDocPtr->GetActiveQuery( &punkq );
		if( punkq )
		{
			ISelectQueryPtr ptrQ( punkq );
			punkq->Release(); punkq = 0;
			if( ptrQ )
			{
				ptrQ->GetTumbnailSize( &sizeThumbnail );
			}
		}
	}		

	
	CreateThumbnail( punkObject, sizeThumbnail );
	m_dbaseDocPtr->FireEvent( _bstr_t( szDBaseEventNewTumbnailGenerate ), 
						m_dbaseDocPtr, m_dbaseDocPtr, NULL, NULL, _variant_t((long)1) );

	//if not set INAmedDataObject2->SetModifiedFlag();
	SetModifiedFlag( true );
	m_bNeedAskUpdateRecord = !m_bBaseObjectEmpty;

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::OnDeleteEntry( BSTR bstrName )
{	
	INamedDataPtr ptrPrivND = GetPrivateNamedData();
	if( ptrPrivND == NULL )
		return_and_assert( false );


	CString strEntry = bstrName;
	IUnknownPtr ptrObj;
	{
		_variant_t var;
		ptrPrivND->GetValue( bstrName, &var );
		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			ptrObj = var.punkVal;		
	}

	if( GetFieldFullPath().Find(strEntry) != -1 )
	{
//		KillChilds();
		SetModifiedFlag( true );		

		if( ptrObj )
		{
			CString strType = ::GetFieldVTObjectType( ptrPrivND, m_strTable, m_strField );
			IUnknown* punkObject = NULL;
			punkObject = ::CreateTypedObject( strType );
			if( punkObject == NULL )
				return_and_assert( false );


			AttachData2Object( punkObject );

			INamedObject2Ptr spNO2( punkObject );
			if( spNO2 )
				spNO2->SetName( _bstr_t( (LPCSTR)GetAsTableField() ) );

			m_bBaseObjectEmpty = true;

			bool bWasSetValue = false;
			
			VERIFY( OnSetValue( _bstr_t(GetFieldFullPath() ), _variant_t( (IUnknown*)punkObject ), true, bWasSetValue ) );		

			m_bBaseObjectEmpty = true;

			m_bNeedAskUpdateRecord = true;

			punkObject->Release();		

		}
		else//for text & digits fields
		{
			_variant_t vt;
			
			bool bWasSetValue = false;
			vt.ChangeType( VT_NULL );
			VERIFY( OnSetValue( _bstr_t(GetFieldFullPath() ), vt, true, bWasSetValue ) );		
		}


		return true;
	}


	CString strChildPath;
	strChildPath = SECTION_DBCHILDOBJECTS;
	strChildPath += "\\";
	strChildPath += (LPCSTR)GetAsTableField();


	if( strEntry.Find(strChildPath) != -1 )
	{
		ptrPrivND->DeleteEntry( bstrName );
		SetModifiedFlag( true );

		m_bNeedAskUpdateRecord = true;

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::IsChild( IUnknown* punkChild )
{
	if( m_guidRoot == INVALID_KEY )
		return false;

	GuidKey guidChild = GetObjectKey( punkChild );
	if( guidChild == INVALID_KEY )
		return false;

	INamedDataPtr ptrPrivND = GetPrivateNamedData();
	if( ptrPrivND == NULL )
		return_and_assert( false );

	IUnknownPtr ptrBaseObject;
	{
		_variant_t var;
		ptrPrivND->GetValue( _bstr_t( (LPCSTR)GetFieldFullPath() ), &var );
		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			ptrBaseObject = var.punkVal;
	}

	if( ptrBaseObject == NULL )
		return false;

	//try to test for parent
	INamedDataObject2Ptr ptrNDO2( punkChild );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
		{
			bool bChild = false;
			if( ::GetObjectKey( punkParent ) == ::GetObjectKey( ptrBaseObject ) )			
				bChild = true;

			punkParent->Release();

			if( bChild )
				return true;
		}
	}


	//At first find group guid
	GuidKey guidGroup = INVALID_KEY;
	
	TPOS lGroupPos = 0;
	ptrPrivND->GetBaseGroupFirstPos( &lGroupPos );
	while( lGroupPos )
	{
		GuidKey guid = INVALID_KEY;
		ptrPrivND->GetNextBaseGroup( &guid, &lGroupPos );
		IUnknown* punkObject = NULL;
		ptrPrivND->GetBaseGroupBaseObject( &guid, &punkObject );
		if( !punkObject )
			continue;

		if( ::GetObjectKey( ptrBaseObject ) == ::GetObjectKey(punkObject) )
			guidGroup = guid;

		punkObject->Release();
	}

	if( guidGroup == INVALID_KEY )
		return false;

	
	TPOS lPos = 0;
	ptrPrivND->GetBaseGroupObjectFirstPos( &guidGroup, &lPos );
	while( lPos )
	{
		IUnknown* punkObj = 0;
		ptrPrivND->GetBaseGroupNextObject( &guidGroup, &lPos, &punkObj );
		if( punkObj == NULL )
			continue;

		CString strName = ::GetObjectName( punkObj );

		if( ::GetObjectKey(punkObj) == guidChild && 
			( ::GetObjectKind( punkObj ) != ::GetObjectKind( ptrBaseObject ) ) )
		{
			punkObj->Release();
			return true;
		}

		punkObj->Release();
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////
CString CQueryField::GetFieldFullPath()
{
	CString strFormat;
	strFormat.Format( "%s\\%s", SECTION_DBASEFIELDS, GetAsTableField() );

	return strFormat;
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::IsRecordsetEmpty()
{
	if( m_recordsetPtr == NULL )
		return true;


	ADO::EditModeEnum em = ADO::adEditNone;
	if( S_OK != m_recordsetPtr->get_EditMode( &em ) )
		return true;

	if( em == ADO::adEditAdd )
		return false;

	ADO::PositionEnum pos = ADO::adPosUnknown;
	if( S_OK != m_recordsetPtr->get_AbsolutePosition( &pos ) )
		return true;	

	return ( ((long)pos) < 1 );
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryField::IsDBaseObject( IUnknown* punkObj, GUID* pGuidDBaseParent, bool& bForceExit, bool& bEmpty )
{
	if( pGuidDBaseParent )
		*pGuidDBaseParent = INVALID_KEY;

	if( !punkObj )
	{
		bForceExit = true;
		return false;
	}


	if( m_fieldType != ftVTObject )
		return false;

	
	if( IsRecordsetEmpty() )
	{
		bForceExit = true;
		return false;
	}	


	GUID guidTestObj = ::GetObjectKey( punkObj );

	if( guidTestObj == m_guidRoot )
	{
		if( pGuidDBaseParent )
			*pGuidDBaseParent = m_guidRoot;

		bEmpty = m_bBaseObjectEmpty;

		return true;
	}
	else
	{
		for( int i=0;i<m_arGuid.GetSize();i++ )
		{
			if( guidTestObj == m_arGuid[i] )
			{
				if( pGuidDBaseParent )
					*pGuidDBaseParent = m_guidRoot;

				bEmpty = m_bBaseObjectEmpty;

				return true;
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::AttachData2Object( IUnknown* punk )
{	
	return;

	if( !punk )
		return;

	if( m_dbaseDocPtr == 0 )
		return;

	INamedDataObject2Ptr ptrNDO2( punk );
	if( ptrNDO2 )
	{
		ptrNDO2->AttachData( m_dbaseDocPtr );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQueryField::OnNewObjectSet2Data( IUnknown* punkCtrl, IUnknown* punkObject, GUID prevKey, BOOL bFlag, IUnknown* punkContext )
{
	if( !punkObject || !punkCtrl )
		return;


	INamedDataPtr ptrND( punkCtrl );	
	if( ptrND )
	{
		ptrND->NotifyContexts(ncChangeKey, punkObject, 0, &prevKey);
	}
	else
	{
		ASSERT( FALSE );
	}

	if( punkContext )
	{
		IDataContext2Ptr ptrDC( punkContext );
		if( ptrDC != 0 && bFlag )
		{
			VERIFY( S_OK == ptrDC->SetObjectSelect( punkObject, bFlag ) );
		}
	}

}


