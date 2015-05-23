#include "stdafx.h"
#include "resource.h"
#include "dbasedoc.h"
#include "docman.h"
#include "thumbnail.h"

#include "placetargetdlg.h"




bool CDBaseDocument::IsSerializePath( CString strPath )
{
	if(
		( strPath.Find(SECTION_FIELD_CAPTIONS) != -1 ) ||
		( strPath.Find(SECTION_FIELD_VTOBJECT_TYPE) != -1 ) ||
		( strPath.Find(SECTION_FIELD_DEFAULT_VALUES_CHECK) != -1 ) ||
		( strPath.Find(SECTION_FIELD_DEFAULT_VALUES) != -1 ) ||
		( strPath.Find(DB_OWN_OBJECT_PATH) != -1 )		

		)
		return true;

	return false;

}

/////////////////////////////////////////////////////////////////////////////////////////
//
// IDataTypeManager implement
//
/////////////////////////////////////////////////////////////////////////////////////////

HRESULT CDBaseDocument::XTypes::GetTypesCount( long *pnCount )
{
	_try_nested(CDBaseDocument, Types, GetTypesCount )
	{	
		START_RECORD_IMPL()
		sptrIDataTypeManager spRecordDTM( punkRecordData );
		return spRecordDTM->GetTypesCount( pnCount );
		STOP_RECORD_IMPL()		
	}
	_catch_nested;
}
  
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XTypes::GetType( long index, BSTR *pbstrType )
{
	_try_nested(CDBaseDocument, Types, GetType )
	{	
		START_RECORD_IMPL()
		sptrIDataTypeManager spRecordDTM( punkRecordData );
		return spRecordDTM->GetType( index, pbstrType );
		STOP_RECORD_IMPL()		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XTypes::GetObjectFirstPosition( long nType, LONG_PTR *plpos )
{
	_try_nested(CDBaseDocument, Types, GetObjectFirstPosition )
	{	
		START_RECORD_IMPL()
		sptrIDataTypeManager spRecordDTM( punkRecordData );
		return spRecordDTM->GetObjectFirstPosition( nType, plpos );
		STOP_RECORD_IMPL()		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XTypes::GetNextObject( long nType, LONG_PTR *plpos, IUnknown **ppunkObj )
{
	_try_nested(CDBaseDocument, Types, GetNextObject )
	{	
		START_RECORD_IMPL()
		sptrIDataTypeManager spRecordDTM( punkRecordData );
		return spRecordDTM->GetNextObject( nType, plpos, ppunkObj );
		STOP_RECORD_IMPL()		
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// INamedData implement
//
/////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::SetupSection( BSTR bstrSectionName )
{
	_try_nested(CDBaseDocument, Data, SetupSection )
	{	
		START_RECORD_IMPL()
		return spRecordND->SetupSection( bstrSectionName );
		STOP_RECORD_IMPL()

		/*
		DECLARE_RECORD_AND_DOC_IMPL() //spRecordND, spDocND, spThisND
		
		spRecordND->SetupSection( bstrSectionName );
		spDocND->SetupSection( bstrSectionName );		

		return S_OK;
		*/
		
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::SetValue( BSTR bstrName, const tagVARIANT var )
{
	_try_nested(CDBaseDocument, Data, SetValue)
	{
		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
		{
			sptrIDBChild sptrChild( var.punkVal );
			if( sptrChild ) sptrChild->SetParent( pThis->GetControllingUnknown() );
		}
		

		IUnknown* punkRecordData = NULL;
		punkRecordData = pThis->GetRecordNamedData( );
		if( punkRecordData == NULL )
			return E_FAIL;		
		
		sptrINamedData spRecordND(punkRecordData);
		punkRecordData->Release();

		if( spRecordND == NULL )
			return E_FAIL;

		sptrIDBaseDocument spDBDoc( pThis->GetControllingUnknown() );
		if( spDBDoc == NULL )
			return E_FAIL;

		sptrISelectQuery spQuery;

		IUnknown* pUnkQuery = NULL;
		spDBDoc->GetActiveQuery( &pUnkQuery );
		if( pUnkQuery != NULL )
		{
			spQuery =  pUnkQuery;
			pUnkQuery->Release();
		}

		//Route to query
		HRESULT hRes;
		if( spQuery )
		{
			//Not set m_punkTarget in Query
			hRes = spQuery->SetValue( bstrName, var, 0 );
			if( hRes == E_FAIL )
				return spRecordND->SetValue( bstrName, var );		
			else
				return hRes;

		}

		return spRecordND->SetValue( bstrName, var );		

		
		
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetValue( BSTR bstrName, tagVARIANT *pvar )
{
	_try_nested(CDBaseDocument, Data, GetValue)
	{		
		IUnknown* punkRecordData = NULL;
		punkRecordData = pThis->GetRecordNamedData( );
		if( punkRecordData == NULL )
			return E_FAIL;
		
		sptrINamedData spRecordND(punkRecordData);
		punkRecordData->Release();

		if( spRecordND == NULL )
			return E_FAIL;

		sptrIDBaseDocument spDBDoc( pThis->GetControllingUnknown() );
		if( spDBDoc == NULL )
			return E_FAIL;

		sptrISelectQuery spQuery;

		IUnknown* pUnkQuery = NULL;
		spDBDoc->GetActiveQuery( &pUnkQuery );
		if( pUnkQuery != NULL )
		{
			spQuery =  pUnkQuery;
			pUnkQuery->Release();
		}

		//Route to query		
		HRESULT hRes;
		if( spQuery )
		{
			//Not set m_punkTarget in Query
			hRes = spQuery->GetValue( bstrName, pvar );
			if( hRes == E_FAIL )
				return spRecordND->GetValue( bstrName, pvar );		
			else
				return hRes;


		}

		return spRecordND->GetValue( bstrName, pvar );		
		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetEntriesCount( long *piCount )
{
	_try_nested(CDBaseDocument, Data, GetEntriesCount)
	{	
		START_RECORD_IMPL()
		return spRecordND->GetEntriesCount( piCount );
		STOP_RECORD_IMPL()

	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetEntryName( long idx, BSTR *pbstrName )
{
	_try_nested(CDBaseDocument, Data, GetEntryName)
	{	
		START_RECORD_IMPL()
		return spRecordND->GetEntryName( idx, pbstrName );
		STOP_RECORD_IMPL()
	}

	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::RenameEntry( BSTR bstrName, BSTR bstrOldName )
{
	_try_nested(CDBaseDocument, Data, RenameEntry)
	{	
		START_RECORD_IMPL()
		return spRecordND->RenameEntry( bstrName, bstrOldName );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::DeleteEntry( BSTR bstrName )
{
	_try_nested(CDBaseDocument, Data, DeleteEntry)
	{	

		IUnknown* punkRecordData = NULL;
		punkRecordData = pThis->GetRecordNamedData( );
		if( punkRecordData == NULL )
			return E_FAIL;
		
		sptrINamedData spRecordND(punkRecordData);
		punkRecordData->Release();

		if( spRecordND == NULL )
			return E_FAIL;

		sptrIDBaseDocument spDBDoc( pThis->GetControllingUnknown() );
		if( spDBDoc == NULL )
			return E_FAIL;

		sptrISelectQuery spQuery;

		IUnknown* pUnkQuery = NULL;
		spDBDoc->GetActiveQuery( &pUnkQuery );
		if( pUnkQuery != NULL )
		{
			spQuery =  pUnkQuery;
			pUnkQuery->Release();
		}

		//Route to query
		if( spQuery )
			return spQuery->DeleteEntry( bstrName );

		return spRecordND->DeleteEntry( bstrName );		
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetActiveContext( IUnknown **punk ) 
{
	_try_nested(CDBaseDocument, Data, GetActiveContext)
	{	
		START_RECORD_IMPL()
		return spRecordND->GetActiveContext( punk ); 		
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::SetActiveContext( IUnknown *punk )
{
	_try_nested(CDBaseDocument, Data, SetActiveContext)
	{	
		START_RECORD_IMPL()
		return spRecordND->SetActiveContext( punk );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::AddContext( IUnknown *punk )
{
	_try_nested(CDBaseDocument, Data, AddContext)
	{	
		START_RECORD_IMPL()
		return spRecordND->AddContext( punk );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::RemoveContext( IUnknown *punk )
{
	_try_nested(CDBaseDocument, Data, RemoveContext)
	{	
		START_RECORD_IMPL()
		return spRecordND->RemoveContext( punk );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::NameExists( BSTR bstrName, long* Exists )
{
	_try_nested(CDBaseDocument, Data, NameExists)
	{	
		START_RECORD_IMPL()
		return spRecordND->NameExists( bstrName, Exists );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetObject(	BSTR bstrName, BSTR bstrType, IUnknown **punk )
{
	_try_nested(CDBaseDocument, Data, GetObject)
	{	
		START_RECORD_IMPL()
		return spRecordND->GetObject( bstrName, bstrType, punk );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::NotifyContexts( DWORD dwNotifyCode, IUnknown *punkNew, IUnknown *punkOld, DWORD dwData)
{
	_try_nested(CDBaseDocument, Data, NotifyContexts)
	{	
		START_RECORD_IMPL()
		return spRecordND->NotifyContexts( dwNotifyCode, punkNew, punkOld, dwData);
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::EnableBinaryStore( BOOL bBinary )
{
	_try_nested(CDBaseDocument, Data, EnableBinaryStore)
	{	
		START_RECORD_IMPL()
		return spRecordND->EnableBinaryStore( bBinary );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
// IDBaseDocument implement
//
/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::GetPrivateNamedData(IUnknown** ppunkNamedData)
{
	_try_nested(CDBaseDocument, DBaseDocument, GetPrivateNamedData)
	{		
		*ppunkNamedData = pThis->GetRecordNamedData( ); //with AddRef		
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::InitNamedData()
{
	_try_nested(CDBaseDocument, DBaseDocument, InitRecordNamedData)
	{		
		pThis->InitRecordNamedData(); 
		pThis->InitDocNamedData(); 
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetCurrentSection( BSTR* pbstrSection )
{
	_try_nested(CDBaseDocument, Data, GetCurrentSection )
	{	
		//DECLARE_RECORD_AND_DOC_IMPL() //spRecordND, spDocND, spThisND, strCurSection

		START_RECORD_IMPL()
		return spRecordND->GetCurrentSection( pbstrSection );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupCount(int * pnCount)
{
	_try_nested(CDBaseDocument, Data,  GetBaseGroupCount)
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupCount( pnCount );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupFirstPos(long * plPos)
{
	_try_nested(CDBaseDocument, Data, GetBaseGroupFirstPos )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupFirstPos( plPos );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetNextBaseGroup(GUID * pKey, long * plPos)
{
	_try_nested(CDBaseDocument, Data, GetNextBaseGroup )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetNextBaseGroup( pKey, plPos );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetIsBaseGroup(GUID * pKey, BOOL * pbBase)
{
	_try_nested(CDBaseDocument, Data, GetIsBaseGroup )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetIsBaseGroup( pKey, pbBase );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupBaseObject(GUID * pKey, IUnknown ** ppunkObject)
{
	_try_nested(CDBaseDocument, Data, GetBaseGroupBaseObject )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupBaseObject( pKey, ppunkObject );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupObjectsCount(GUID * pKey, int * pnCount)
{
	_try_nested(CDBaseDocument, Data, GetBaseGroupObjectsCount )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupObjectsCount( pKey, pnCount );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupObjectFirstPos(GUID * pKey, long * plPos)
{
	_try_nested(CDBaseDocument, Data, GetBaseGroupObjectFirstPos )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupObjectFirstPos( pKey, plPos );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XData::GetBaseGroupNextObject(GUID * pKey, long * plPos, IUnknown ** ppunkObject)
{
	_try_nested(CDBaseDocument, Data, GetBaseGroupNextObject )
	{		
		START_RECORD_IMPL()
		return spRecordND->GetBaseGroupNextObject( pKey, plPos, ppunkObject );
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}
HRESULT CDBaseDocument::XData::SetEmptySection(BSTR* bstrSectionName )
{
	_try_nested(CDBaseDocument, Data, SetupSection )
	{	
		START_RECORD_IMPL()
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
							iNumber += ch*power10 - L'0';
						}else{
							break;
						}
					}
					section=_bstr_t(CStringW((wchar_t*)section).Left(i+1))+_bstr_t(iNumber+1);
				}else{
					break;
				}
			}
		STOP_RECORD_IMPL()
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
// Query support / Start
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::GetActiveQuery( IUnknown** punk )
{
	_try_nested(CDBaseDocument, DBaseDocument, GetActiveQueryName)
	{
		*punk = 0;
		*punk = ::GetObjectByKey( pThis->GetControllingUnknown(), pThis->m_activeQueryKey );
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::SetActiveQuery( IUnknown* punk )
{
	_try_nested(CDBaseDocument, DBaseDocument, SetActiveQuery)
	{
		pThis->m_activeQueryKey = ::GetObjectKey( punk );
		IDataContextPtr sptrDC = pThis->GetControllingUnknown();

		if( sptrDC )
			sptrDC->SetActiveObject( _bstr_t( szTypeQueryObject ), punk, 0  );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
// Query support/ End
////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////
//Temp table info array state
///////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::IsDirtyTableInfo( BOOL* pbDirty )
{
	_try_nested(CDBaseDocument, DBaseStruct, IsDirtyTableInfo)
	{
		BOOL bSucceeded	= pThis->m_dbEngine.IsConnectionOK();
		*pbDirty		= pThis->m_dbEngine.IsDirtyTableInfo();

		if( !bSucceeded )
			return S_FALSE;		

		return S_OK;
	}
	_catch_nested;
}

HRESULT CDBaseDocument::XDBaseStruct::RefreshTableInfo( BOOL bForceRefresh )
{
	_try_nested(CDBaseDocument, DBaseStruct, RefreshTableInfo)
	{
		BOOL bSucceeded = ConvertToBOOL(
			(pThis->m_dbEngine.GetTablesInfo( NULL, ConvertToBoolean(bForceRefresh)) ) );

		if( !bSucceeded )
			return S_FALSE;		

		return S_OK;
	}
	_catch_nested;
}

#define DB_TABLE_DEFINITION_IMPL										\
		if( !pThis->m_dbEngine.IsConnectionOK() )							\
			return S_FALSE;												\
																		\
		CArray<_CTableInfo*, _CTableInfo*>* pArrTableInfo = NULL;		\
																		\
		pThis->m_dbEngine.GetTablesInfo( &pArrTableInfo, false );		\
																		\
		if( pArrTableInfo == NULL)										\
			return S_FALSE;												\


///////////////////////////////////
//Type info
///////////////////////////////////
//Table
HRESULT CDBaseDocument::XDBaseStruct::GetTableCount( int* pnCount )
{
	_try_nested(CDBaseDocument, DBaseStruct, GetTableCount)
	{
		DB_TABLE_DEFINITION_IMPL;
		*pnCount = pArrTableInfo->GetSize();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::GetTableName( int nIndex, BSTR* pbstrName )
{
	_try_nested(CDBaseDocument, DBaseStruct, GetTableName)
	{
		DB_TABLE_DEFINITION_IMPL;
		
		if( nIndex <0 || nIndex >= pArrTableInfo->GetSize() )
			return S_FALSE;
		
		*pbstrName = (*pArrTableInfo)[nIndex]->m_strTableName.AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

		
//Fields
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::GetFieldCount( BSTR bstrTable, int* pnFieldsCount )
{
	_try_nested(CDBaseDocument, DBaseStruct, GetTableFieldsCount)
	{
		DB_TABLE_DEFINITION_IMPL;

		CString strTableName = bstrTable;
		strTableName.MakeLower();

		int nTableIndex = -1;
		for( int i=0;i<pArrTableInfo->GetSize();i++ )
		{
			if( (*pArrTableInfo)[i]->m_strTableName == strTableName )
			{
				nTableIndex = i;
				break;
			}
		}

		if( nTableIndex == -1 )
			return S_FALSE;

		*pnFieldsCount = (*pArrTableInfo)[nTableIndex]->m_arrFieldInfo.GetSize();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::GetFieldName( BSTR bstrTable, int nFieldIndex, BSTR* pbstrFieldName )
{
	_try_nested(CDBaseDocument, DBaseStruct, GetTableFieldsCount)
	{
		DB_TABLE_DEFINITION_IMPL;

		CString strTableName = bstrTable;
		strTableName.MakeLower();

		int nTableIndex = -1;
		for( int i=0;i<pArrTableInfo->GetSize();i++ )
		{
			if( (*pArrTableInfo)[i]->m_strTableName == strTableName )
			{
				nTableIndex = i;
				break;
			}
		}

		if( nTableIndex == -1 )
			return S_FALSE;


		_CTableInfo* pti = (*pArrTableInfo)[nTableIndex];
		if( nFieldIndex < 0 || nFieldIndex >= pti->m_arrFieldInfo.GetSize()  )
			return S_FALSE;

		*pbstrFieldName = pti->m_arrFieldInfo[nFieldIndex]->m_strFieldName.AllocSysString();


		return S_OK;
	}
	_catch_nested;

}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::GetFieldInfo( BSTR bstrTable, BSTR bstrField,  
			BSTR* pbstrUserName/*CBN*/, short* pnFieldType/*CBN*/,
			BSTR* pbstrPrimaryTable/*CBN*/, BSTR* pbstrPrimaryFieldName/*CBN*/,
			BOOL* pbRequiredValue/*CBN*/, BOOL* pbDefaultValue/*CBN*/,
			BSTR* pbstrDefaultValue/*CBN*/, BSTR* pbstrVTObjectType/*CBN*/
			)
		// /*CBN*/ - can be null
{
	_try_nested(CDBaseDocument, DBaseStruct, GetFieldInfo)
	{
		DB_TABLE_DEFINITION_IMPL;

		CString strTableName = bstrTable;
		CString strFieldName = bstrField;

		strTableName.MakeLower();
		strFieldName.MakeLower();

		int nTableIndex = -1;
		int nFieldIndex = -1;
		for( int i=0;i<pArrTableInfo->GetSize();i++ )
		{
			if( (*pArrTableInfo)[i]->m_strTableName == strTableName )
			{				
				nTableIndex = i;
				break;
			}
		}

		if( nTableIndex == -1 )
			return S_FALSE;

		_CTableInfo* pti = (*pArrTableInfo)[i];
		for( i=0;i<pti->m_arrFieldInfo.GetSize();i++ )
		{
			if( pti->m_arrFieldInfo[i]->m_strFieldName == strFieldName )
			{
				nFieldIndex = i;
				break;
			}
		}

		if( nFieldIndex == -1 )
			return S_FALSE;		


		if( nTableIndex <0 || nTableIndex >= pArrTableInfo->GetSize() )
			return S_FALSE;

		if( nFieldIndex<0 || nFieldIndex >= (*pArrTableInfo)[nTableIndex]->m_arrFieldInfo.GetSize() )
			return S_FALSE;

		_CFieldInfo* pFInfo = (*pArrTableInfo)[nTableIndex]->m_arrFieldInfo[nFieldIndex];

		if( pbstrUserName != NULL )
			*pbstrUserName = pFInfo->m_strUserName.AllocSysString();

		if( pnFieldType != NULL )
			*pnFieldType = (short)pFInfo->m_FieldType;

		if( pbstrPrimaryTable != NULL )
			*pbstrPrimaryTable = pFInfo->m_strPrimaryTable.AllocSysString();

		if( pbstrPrimaryFieldName != NULL )
			*pbstrPrimaryFieldName = pFInfo->m_strPrimaryFieldName.AllocSysString();

		if( pbRequiredValue != NULL )
			*pbRequiredValue = pFInfo->m_bRequiredValue;
		
		if( pbDefaultValue != NULL )
			*pbDefaultValue = pFInfo->m_bDefaultValue;

		if( pbstrDefaultValue != NULL )
			*pbstrDefaultValue = pFInfo->m_strDefaultValue.AllocSysString();		

		if( pbstrVTObjectType != NULL )
			*pbstrVTObjectType = pFInfo->m_strVTObjectType.AllocSysString();		

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::AddTable( BSTR bstrTableName )
{
	_try_nested(CDBaseDocument, DBaseStruct, AddTable)
	{
		return pThis->m_dbEngine.AddTable( bstrTableName );
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::DeleteTable( BSTR bstrTableName )
{
	_try_nested(CDBaseDocument, DBaseStruct, DeleteTable)
	{

		return pThis->m_dbEngine.DeleteTable( bstrTableName );
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::AddField( BSTR bstrTableName, BSTR bstrFieldName, 
												short nFieldType,
												BSTR bstrPrimaryTable, BSTR bstrPrimaryFieldName
												)
{
	_try_nested(CDBaseDocument, DBaseStruct, AddField)
	{

		return pThis->m_dbEngine.AddField( 
								bstrTableName, bstrFieldName, 
								nFieldType,
								bstrPrimaryTable, bstrPrimaryFieldName );
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::SetFieldInfo( BSTR bstrTableName, BSTR bstrFieldName, 
													BSTR bstrUserName,
													BOOL bRequiredValue, BOOL bDefaultValue,
													BSTR bstrDefaultValue, BSTR bstrVTObjectType
													)
{
	_try_nested(CDBaseDocument, DBaseStruct, EditField )
	{
		return pThis->m_dbEngine.SetFieldInfo( bstrTableName, bstrFieldName, 
								bstrUserName, bRequiredValue, 
								bDefaultValue, bstrDefaultValue, bstrVTObjectType );
		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseStruct::DeleteField( BSTR bstrTableName, BSTR bstrFieldName )
{
	_try_nested(CDBaseDocument, DBaseStruct, DeleteField)
	{
		return pThis->m_dbEngine.DeleteField( bstrTableName, bstrFieldName );
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::RegisterDBaseListener( IUnknown* punk )
{
	_try_nested(CDBaseDocument, DBaseDocument, RegisterDBaseListener)
	{
		pThis->m_eventController.RegisterListener( punk );
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::UnRegisterDBaseListener( IUnknown* punk )
{
	_try_nested(CDBaseDocument, DBaseDocument, UnRegisterDBaseListener)
	{
		pThis->m_eventController.UnRegisterListener( punk );
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::FireEvent( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var )
{
	_try_nested(CDBaseDocument, DBaseDocument, FireEvent)
	{
		CString strEvent = bstrEvent;
		_variant_t data(var);
		pThis->m_eventController.FireEvent( strEvent, punkObject, punkDBaseDocument, bstrTableName, bstrFieldName, data );		
		return S_OK;
	}
	_catch_nested;
}





////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::GetConnection( IUnknown** ppUnkSession )
{
	_try_nested(CDBaseDocument, DBConnection, GetConnection)
	{
		*ppUnkSession = pThis->m_dbEngine.GetConnection();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::GetMDBFileName( BSTR* bstrFileName )
{
	_try_nested(CDBaseDocument, DBConnection, GetMDBFileName)
	{
		CString strConnectionString = pThis->GetConnectionString();

		CString strFileName = pThis->GetMDBFileNameFromConnectionCtring( strConnectionString, pThis->m_str_dbd_open_file_name );

		*bstrFileName = strFileName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::OpenConnectionFromString( BSTR bstrConnection )
{
	_try_nested(CDBaseDocument, DBConnection, OpenConnectionFromString)
	{
		CString strNewConnString = bstrConnection;		
		if( pThis->m_dbEngine.OpenFromString( strNewConnString ) == true )
		{
			pThis->SetConnectionString( strNewConnString );			
			return S_OK;
		}

		return S_FALSE;
		
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::LoadAccessDBase( BSTR bstrMDBFilePath )
{
	_try_nested(CDBaseDocument, DBConnection, LoadDatabase)
	{

		CString strNewConnString;
		CString strMDBFileName = bstrMDBFilePath;
		if( pThis->m_dbEngine.OpenAccessFile( strMDBFileName, strNewConnString ) == true )
		{
			pThis->SetConnectionString( strNewConnString );

			AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
			::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );	

/*************************** Maxim [26.06.02] - Save DBD *******************************************/
			/*
			{

				CString strTmp = bstrMDBFilePath, strFile;

				strFile = strTmp.Left( strTmp.GetLength() - 3 );
				strFile += "dbd";


				IDocumentSitePtr ptr_ds( pThis->GetControllingUnknown() );
					if( ptr_ds )
					ptr_ds->SaveDocument( _bstr_t( (LPCSTR)strFile ) );
			}
			*/
/*************************** Maxim [26.06.02] ******************************************************/
			
			return S_OK;
		}

		return S_FALSE;
		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::CreateAdvancedConnection( )
{
	_try_nested(CDBaseDocument, DBConnection, CreateAdvancedConnection)
	{
		CString strNewConnString;		
		if( pThis->m_dbEngine.OpenAdvancedConnection( strNewConnString ) == true )
		{
			pThis->SetConnectionString( strNewConnString );			
			return S_OK;
		}

		return S_FALSE;
		
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::CloseConnection()
{
	_try_nested(CDBaseDocument, DBConnection, CloseConnection)
	{
		pThis->m_dbEngine.Close();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::OpenConnection()
{
	_try_nested(CDBaseDocument, DBConnection, OpenConnection)
	{
		bool bResult = pThis->m_dbEngine.OpenFromString( pThis->GetConnectionString() );

		if( !bResult )
			return S_FALSE;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBConnection::ExecuteSQL( BSTR bstrSQL, IUnknown** ppUnkRecordset )
{
	_try_nested(CDBaseDocument, DBConnection, ExecuteSQL)
	{
		*ppUnkRecordset = NULL;

		bool bResult = pThis->m_dbEngine.IsConnectionOK();

		if( !bResult )
			return S_FALSE;

		IUnknown* pUnkConnection = NULL;
		GetConnection( &pUnkConnection );
		if( pUnkConnection == NULL )
			return S_FALSE;

		ADO::_ConnectionPtr spConnection = pUnkConnection;
		pUnkConnection->Release();

		if( spConnection == NULL )
			return S_FALSE;


		CString strQuery = bstrSQL;

		strQuery.MakeLower();

		bool bSelectQuery = false;
		if( strQuery.Find("select") != -1 )
			bSelectQuery = true;


		if( bSelectQuery )
		{
			
			try
			{
				_variant_t vConn = (IDispatch*)spConnection;
				if( spConnection->State != ADO::adStateOpen )
					return S_FALSE;				
				
				ADO::_RecordsetPtr spRecordset;
				spRecordset.CreateInstance( __uuidof(ADO::Recordset) );
				spRecordset->CursorLocation = ADO::adUseClient;
				spRecordset->Open( _bstr_t(strQuery), vConn, 
							ADO::adOpenKeyset, ADO::adLockOptimistic, ADO::adCmdText);
				bResult = true;
				*ppUnkRecordset = spRecordset;
				spRecordset->AddRef();
			}
			catch (_com_error &e)
			{
				bResult = false;
				dump_com_error(e);						
			}
		}
		else
		{
			try
			{
				//VARIANT var_afectRecord;
				spConnection->Execute( _bstr_t(strQuery), NULL/*&var_afectRecord*/, ADO::adCmdText );
				bResult = true;
				pThis->m_dbEngine.SetDirtyTableInfo( true );
			}
			catch (_com_error &e)
			{
				bResult = false;
				dump_com_error(e);						
			}
		}

		if( bResult )
			return S_OK;

		return S_FALSE;
	}
	_catch_nested;
}





//Active iamge operation

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::SetActiveImage( BSTR bstrName )
{
	_try_nested(CDBaseDocument, DBaseDocument, SetActiveImage);
	{		
		pThis->SetActiveImage( bstrName );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::GetActiveImage( BSTR* pbstrName )
{
	_try_nested(CDBaseDocument, DBaseDocument, GetActiveImage);
	{	
		*pbstrName = pThis->GetActiveImage( ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
//is read only?
HRESULT CDBaseDocument::XDBaseDocument::IsReadOnly( BOOL* pbReadOnly )
{
	_try_nested(CDBaseDocument, DBaseDocument, IsReadOnly);
	{	
		*pbReadOnly = ( pThis->m_bReadOnly ? 1 : 0 );
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::GetNamedDataFieldsSection( BSTR* pbstrSection )
{
	_try_nested(CDBaseDocument, DBaseDocument, GetNamedDataFieldsSection);
	{	
		CString strSection = SECTION_DBASEFIELDS;
		*pbstrSection = strSection.AllocSysString();
		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty )
{
	_try_nested(CDBaseDocument, DBaseDocument, IsDBaseObject);
	{	
		if( pbDBaseObject )
			*pbDBaseObject = FALSE;

		if( pGuidDBaseParent )
			*pGuidDBaseParent = INVALID_KEY;

		IDBaseDocumentPtr ptrDBDoc( pThis->GetControllingUnknown() );
		if( ptrDBDoc == NULL )
			return S_OK;

		IUnknown* punkQ = 0;
		ptrDBDoc->GetActiveQuery( &punkQ );
		if( punkQ == NULL )
			return S_OK;

		ISelectQueryPtr ptrQ( punkQ );
		punkQ->Release();

		if( ptrQ == NULL )
			return S_OK;

		ptrQ->IsDBaseObject( punkObj, pbDBaseObject, pGuidDBaseParent, pbEmpty );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XNotifyPlace::NotifyPutToData( IUnknown* punkObj )
{
	return NotifyPutToDataEx(punkObj, NULL);
};

class CDBaseUndoObj: public IUnknown
{
	DWORD m_dwRefs;
public:
	CDBaseUndoObj(IUnknown *punkQ, IUnknown *punkPrevData, _bstr_t bstrEntry, bool bNewRecord,
		long nRecordNum)
	{
		m_dwRefs = 1;
		m_punkQ = punkQ;
		m_punkPrevData = punkPrevData;
		m_bstrEntry = bstrEntry;
		m_bNewRecord = bNewRecord;
		m_nRecordNum = nRecordNum;
	};
	~CDBaseUndoObj()
	{
	}
	virtual ULONG  __stdcall AddRef()
	{
		return ++m_dwRefs;
	};
	virtual ULONG  __stdcall Release()
	{
		DWORD dwRefs = --m_dwRefs;
		if (dwRefs <= 0)
			delete this;
		return dwRefs;
	};
	virtual HRESULT __stdcall QueryInterface( const IID &iid, void **pret )
	{
		return E_NOINTERFACE;
	};

	IUnknownPtr m_punkQ;
	IUnknownPtr m_punkPrevData;
	bool m_bNewRecord;
	long m_nRecordNum;
	_bstr_t m_bstrEntry;
};

HRESULT CDBaseDocument::XNotifyPlace::NotifyPutToDataEx( IUnknown* punkObj, IUnknown** ppunkUndoObj )
{
	_try_nested(CDBaseDocument, NotifyPlace, NotifyPutToData);
	{	
		if( ::GetValueInt( pThis->GetControllingUnknown(), SHELL_DATA_DB_SECTION, "NotifyPutToData", 1L ) == 0L )
			return S_FALSE;

		IDBaseDocumentPtr ptrDBDoc( pThis->GetControllingUnknown() );
		if( ptrDBDoc == NULL )
			return S_FALSE;

		IUnknown* punkQ = 0;
		ptrDBDoc->GetActiveQuery( &punkQ );
		if( punkQ == NULL )
			return S_FALSE;

		ISelectQueryPtr ptrQ( punkQ );
		punkQ->Release();

		if( ptrQ == NULL )
			return S_FALSE;

		BOOL bOpen = FALSE;
		ptrQ->IsOpen( &bOpen );

		if( !bOpen )
			return S_FALSE;

		{
			INamedDataObject2Ptr ptrNDO( punkObj );
			if( ptrNDO )			
			{
				GUID guid;
				::CoCreateGuid( &guid );
				ptrNDO->SetBaseKey( &guid );
			}
		}

		//Если активное вью - бланк, активное поле в БД - изображение, 
		//и загружаемый объект - тоже изображение, то ничего не спрашиваем
		for(;;)
		{
			if( ::GetObjectKind( punkObj ) != szTypeImage )
				break;

			IDocumentSitePtr ptr_ds( pThis->GetControllingUnknown() );
			if( ptr_ds == 0 )
				break;

			IUnknownPtr ptr_view;
			ptr_ds->GetActiveView( &ptr_view );
			if( ptr_view == 0 )
				break;

			if( !CheckInterface( ptr_view, IID_IBlankView ) )
				break;

			//Смотрим активное поле в запросе
			IQueryObjectPtr ptr_query = ptrQ;
			if( ptr_query == 0 )
				break;

			int nidx = 0;
			ptr_query->GetActiveField( &nidx );
			_bstr_t bstr_table, bstr_field;
			ptr_query->GetField( nidx, 0, bstr_table.GetAddress(), bstr_field.GetAddress() ); 

			if( !bstr_table.length() || !bstr_field.length() )
				break;

			CString str_obj_name;
			str_obj_name.Format( "%s.%s", (char*)bstr_table, (char*)bstr_field );
			
			//Проверяем какого типа объект в документе
			
			{
				IUnknownPtr ptr_exist_obj;

				IUnknown* punk_exist_obj = ::GetObjectByName( ptr_ds, str_obj_name, szTypeImage );
				if( !punk_exist_obj )
					break;
				
				ptr_exist_obj = punk_exist_obj;
				punk_exist_obj->Release();	punk_exist_obj = 0;
	
				if( ::GetObjectKind( ptr_exist_obj ) != szTypeImage )
					break;			

				//Проверим не нустой-ли объект
				BOOL bempty = false;
				ptrDBDoc->IsDBaseObject( ptr_exist_obj, 0, 0, &bempty );
				if( !bempty )
				{
					if( AfxMessageBox( IDS_WARNING_OBJECT_EXIST, MB_YESNO ) != IDYES )
						return E_FAIL;
				}
			}


			//Все условия выполнены, вставляем в поле
			_bstr_t bstr_entry( SECTION_DBASEFIELDS );
			bstr_entry += "\\";
			bstr_entry += (LPCSTR)str_obj_name;

			ptrQ->SetValue( bstr_entry, _variant_t( punkObj ), 0 );

			return S_OK;
		}

		BOOL bUseDialog = ::GetValueInt(::GetAppUnknown(), "\\Database\\NotifyPutToData", "UseDialog", TRUE); 
		if (!bUseDialog)
			return S_FALSE;
		CPlaceTargetDlg dlg;
		dlg.SetSettings( ptrQ, pThis->GetControllingUnknown(), ::GetObjectKind( punkObj ), punkObj );
		if( dlg.DoModal() == IDOK )
		{
			if( dlg.m_nTarget == 1 )//put to doc
				return S_FALSE;

			if( dlg.m_strField.IsEmpty() || dlg.m_strTable.IsEmpty() )
				return S_FALSE;

			if( dlg.m_bNewRecord )			
				ptrQ->Insert();

			CString strFieldName;
			strFieldName.Format( "%s.%s", dlg.m_strTable, dlg.m_strField );

			/*
			INamedObject2Ptr ptrNDO( punkObj );
			if( ptrNDO )
				ptrNDO->SetName( _bstr_t( (LPCSTR)strFieldName ) );
				*/

			
			_bstr_t bstrEntry( SECTION_DBASEFIELDS );
			bstrEntry += "\\";
			bstrEntry += (LPCSTR)strFieldName;


			if (ppunkUndoObj)
			{
				_variant_t var;
				ptrQ->GetValue( bstrEntry, &var );
				long lRecord = 0;
				ptrQ->GetCurrentRecord(&lRecord);
				CDBaseUndoObj *p = new CDBaseUndoObj(ptrQ,var.vt==VT_UNKNOWN?var.punkVal:0,
					bstrEntry,dlg.m_bNewRecord,lRecord);
				*ppunkUndoObj = p;
			}

			//ptrQ->SaveContextsState();
			ptrQ->SetValue( bstrEntry, _variant_t( punkObj ), 0 );
			//ptrQ->RestoreContextsState();



			IDocumentSitePtr ptrDS( pThis->GetControllingUnknown() );
			if( ptrDS )
			{
				IUnknown* punkView = 0;
				ptrDS->GetActiveView( &punkView );
				if( punkView )
				{
					IDataContext2Ptr ptrDC( punkView );
					punkView->Release();

					if( ptrDC )
					{
						CString strType = GetObjectKind( punkObj );
						ptrDC->UnselectAll( _bstr_t( (LPCSTR)strType ) );
						ptrDC->SetObjectSelect( punkObj, TRUE );
					}
				}
			}


			return S_OK;
		}
		else
			return E_FAIL;


		return S_FALSE;
	}
	_catch_nested;
}

bool CDBaseDocument::DoUndoRedo(IUnknown *punkUndoObj, bool bRedo)
{
	CDBaseUndoObj *p = (CDBaseUndoObj*)punkUndoObj;
	ISelectQueryPtr ptrQ(p->m_punkQ);
	// For redo: insert record, for undo go to record
	if (bRedo && p->m_bNewRecord)
		ptrQ->Insert();
	// Exchange data: old save in undo object, new put to data
	_variant_t var;
	ptrQ->GetValue( p->m_bstrEntry, &var );
	ptrQ->SetValue( p->m_bstrEntry, _variant_t((IUnknown*)p->m_punkPrevData), 0);
	if (var.vt == VT_UNKNOWN)
		p->m_punkPrevData = var.punkVal;
	else
		p->m_punkPrevData = 0;
	// For undo: delete record
	if (!bRedo && p->m_bNewRecord)
		ptrQ->DeleteEx(TRUE);
	return true;
}

HRESULT CDBaseDocument::XNotifyPlace::Undo( IUnknown* punkUndoObj )
{
	_try_nested(CDBaseDocument, NotifyPlace, Undo);
	{	
		return pThis->DoUndoRedo(punkUndoObj, false)?S_OK:E_FAIL;
	}
	_catch_nested;
}

HRESULT CDBaseDocument::XNotifyPlace::Redo( IUnknown* punkUndoObj )
{
	_try_nested(CDBaseDocument, NotifyPlace, Redo);
	{	
		return pThis->DoUndoRedo(punkUndoObj, true)?S_OK:E_FAIL;
	}
	_catch_nested;
}


bool ParseTableField( CString strPath, CString& strTable, CString& strField )
{
	int nIndex = 0;
	nIndex = strPath.Find( "." );
	if( nIndex == -1  ) 
		return false;

	strTable = strPath.Left( nIndex );
	strField = strPath.Right( strPath.GetLength() - nIndex - 1 );

	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XUserNameProvider::GetUserName( IUnknown* punkObject, BSTR* pbstrUserName )
{
	_try_nested(CDBaseDocument, UserNameProvider, GetUserName);
	{	
		if( !pbstrUserName )
			return E_INVALIDARG;

		INamedObject2Ptr ptrNO2( punkObject );
		if( ptrNO2 == 0 )
			return S_FALSE;

		BSTR bstrName = 0;
		HRESULT hr = E_FAIL;

		if( S_OK != ( hr = ptrNO2->GetName( &bstrName ) ) )
			return hr;

		if( !pThis->m_bShowUserNameInContext )
		{
			*pbstrUserName = bstrName;
			return S_OK;
		}

		CString strObjName = bstrName;

		if( bstrName )
			::SysFreeString( bstrName );	bstrName = 0;
		

		if( pThis->m_bAddRootFieldNameInContext )
		{
			IDBaseDocumentPtr ptrDBDoc( pThis->GetControllingUnknown() );
			if( ptrDBDoc )
			{
				BOOL bDBaseObject = FALSE;			
				GuidKey guidObjRoot;
				ptrDBDoc->IsDBaseObject( punkObject, &bDBaseObject, &guidObjRoot, 0 );
				if( bDBaseObject && guidObjRoot != ::GetObjectKey( punkObject ) )
				{
					IUnknown* punkRoot = 0;					
					punkRoot = GetObjectByKey( ptrDBDoc, guidObjRoot );

					if( punkRoot )
					{
						IUnknownPtr ptrRoot = punkRoot;
						punkRoot->Release();	punkRoot = 0;						

						CString strRootName = ::GetObjectName( ptrRoot );
						CString strTableRoot, strFieldRoot;
						if( ParseTableField( strRootName, strTableRoot, strFieldRoot ) )
						{
							CString strCaptionRoot = 
								::GetFieldCaption( pThis->GetControllingUnknown(), strTableRoot, strFieldRoot );							

							if( !strCaptionRoot.IsEmpty() && !strObjName.IsEmpty() )
							{
								CString strFormat;
								strFormat.Format( "%s (%s)", strObjName, strCaptionRoot );
								*pbstrUserName = strFormat.AllocSysString();
								return S_OK;
							}
						}
						
					}
				}
			}
		}


		CString strTable;
		CString strField;
		if( !ParseTableField( strObjName, strTable, strField ) )
			return S_FALSE;

		CString strCaption = ::GetFieldCaption( pThis->GetControllingUnknown(), strTable, strField );
		if( strCaption.IsEmpty() )
			return S_FALSE;

		*pbstrUserName = strCaption.AllocSysString();

		return S_OK;

	}
	_catch_nested;
}


