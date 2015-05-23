#include "stdafx.h"
#include "resource.h"
#include "dbasedoc.h"
#include "constant.h"
#include "selectquery.h"
//#include "FilterBuilderDlg.h"	

#include "VisualEdit.h"
#include "DBStructDlg.h"

#include "Wizards.h"
#include "undoint.h"
#include "UserInfo.h"


/////////////////////////////////////////////////////////////////////////////////////////
//CViewContext
CViewContext::CViewContext()
{
	m_guidKeyView = INVALID_KEY;

}

/////////////////////////////////////////////////////////////////////////////////////////
CViewContext::~CViewContext()
{
	POSITION pos = m_arObjectInfo.GetHeadPosition();
	
	while( pos )
	{
		CObjectInfo* poi = (CObjectInfo*)m_arObjectInfo.GetNext( pos );
		delete poi;
	}

	m_arObjectInfo.RemoveAll();

}


/////////////////////////////////////////////////////////////////////////////////////////
//CQueryObject

IMPLEMENT_DYNCREATE(CQueryObject, CAXFormBase);



// {C5ED0CDE-E2EC-41ff-B63B-DBAB7A9F0DDD}
GUARD_IMPLEMENT_OLECREATE(CQueryObject, "DBaseDoc.QueryObjectD", 
0xc5ed0cde, 0xe2ec, 0x41ff, 0xb6, 0x3b, 0xdb, 0xab, 0x7a, 0x9f, 0xd, 0xdd);
// {AB987CFA-EFCC-46fc-AF3E-24EB51D45D03}
static const GUID clsidSelectQueryDataObjectInfo = 
{ 0xab987cfa, 0xefcc, 0x46fc, { 0xaf, 0x3e, 0x24, 0xeb, 0x51, 0xd4, 0x5d, 0x3 } };


DATA_OBJECT_INFO(IDS_QUERY_TYPE, CQueryObject, szTypeQueryObject, clsidSelectQueryDataObjectInfo, IDI_QUERY_ICON)


BEGIN_INTERFACE_MAP(CQueryObject, CAXFormBase)
	INTERFACE_PART(CQueryObject, IID_IQueryObject, Query)
	INTERFACE_PART(CQueryObject, IID_ISelectQuery, SelectQuery)
	INTERFACE_PART(CQueryObject, IID_IDBaseListener, DBaseListener)
	INTERFACE_PART(CQueryObject, IID_IDBChild, DBChild)
	INTERFACE_PART(CQueryObject, IID_IBlankForm, BlankForm)
	INTERFACE_PART(CQueryObject, IID_INotifyObject, NotifyObject)
	INTERFACE_PART(CQueryObject, IID_IQueryMisc, QueryMisc)
	INTERFACE_PART(CQueryObject, IID_IEventListener, EvList)
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CQueryObject, Query)
IMPLEMENT_UNKNOWN(CQueryObject, SelectQuery)
IMPLEMENT_UNKNOWN(CQueryObject, DBChild)
IMPLEMENT_UNKNOWN(CQueryObject, BlankForm)
IMPLEMENT_UNKNOWN(CQueryObject, NotifyObject)
IMPLEMENT_UNKNOWN(CQueryObject, QueryMisc)



/////////////////////////////////////////////////////////////////////////////////////////
CQueryObject::CQueryObject()
{		
	m_bPrivateMode = false;
	m_strTumbnailTable = "";
	m_strTumbnailField = "";

	m_strTumbnailCaptionTable = "";
	m_strTumbnailCaptionField = "";

	m_sizeTumbnail = CSize( 100, 100 );
	m_sizeTumbnailView = CSize( 100, 100 );

	m_bWasRegister = false;

	m_bCanProcessFind = false;
	m_bFindInField = true;

	m_strFindTable.Empty();
	m_strFindField.Empty();
	m_bFindMatchCase = false;
	bFindRegularExpression = false;
	m_strTextToFind.Empty();

	m_sizeTumbnailBorder = CSize( 5, 5 );

	m_state = qsClose;
	
	m_lBookMark = -1;

	m_nActiveField = -1;

	//to catch AfterDocClose
	Register( ::GetAppUnknown() );	

	m_lCanModifyDatabase = -1;

	m_llast_primary_key = -1;
}	

/////////////////////////////////////////////////////////////////////////////////////////
CQueryObject::~CQueryObject()
{		
	DeInit();	

	UnRegister( ::GetAppUnknown() );
}

/////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::_RegisterOnDBController( )
{
	if( m_bWasRegister )
		return;

	if( m_sptrParent == NULL )
		return;

	//register on DBase controller
	m_sptrParent->RegisterDBaseListener( GetControllingUnknown() );

	//to catch szEventChangeObject
	Register( m_sptrParent );

	m_bWasRegister = true;
}	

/////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::_UnRegisterOnDBController()
{
	if( !m_bWasRegister )
		return;

	if( m_sptrParent == NULL )
		return;
	
	//unregister on DBase controller
	m_sptrParent->UnRegisterDBaseListener( GetControllingUnknown() );
	
	UnRegister( m_sptrParent );

	m_bWasRegister = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::CanModifyDatabase()
{
	if( m_lCanModifyDatabase == -1 )
	{
		if( m_sptrParent == 0 )
		{
			m_lCanModifyDatabase = 1;	
		}
		else
		{
			BOOL bReadOnly = false;
			m_sptrParent->IsReadOnly( &bReadOnly );
			m_lCanModifyDatabase = ( bReadOnly ? 0 : 1 );
		}
	}	
	return ( m_lCanModifyDatabase == 1 );
}
			


//reason of func OnCatchEventDocumentAfterClose()
//shell.dsp->ShellDoc.cpp.CShellDoc::OnClose()
//{
//	...
//	delete this; //but com object
//
//}
/////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::DeInit()
{
	ISelectQueryPtr ptrQ( GetControllingUnknown() );
	if( ptrQ )
		ptrQ->Close();

	_UnRegisterOnDBController();
	
	m_sptrParent = 0;	

	DestroyCacheBuffers();	

	DestroyContextState();

}

/////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( m_sptrParent )
	{
		if( !strcmp(pszEvent, "AfterClose") && ( ::GetObjectKey( m_sptrParent ) == ::GetObjectKey(punkFrom) ) )
		{
			//m_xSelectQuery.UpdateInteractive( TRUE );
			DeInit();
		}
		else if( !strcmp(pszEvent, szEventBeforeClose ) && ( ::GetObjectKey( m_sptrParent ) == ::GetObjectKey(punkFrom) ) )
		{
			
		}
		if( !strcmp(pszEvent, szEventChangeObject) )
		{
			if( IsOpen() )
			{
				for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
				{
					CQueryField* pField = m_query.m_arrFields[iField];
					if( pField->ProcessDataObjectChange( punkFrom ) )
						break;
				}
			}
			
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::IsOpen()
{
	if( m_spRecordset == NULL )
		return false;

	long lState;
    HRESULT _hr = m_spRecordset->get_State( &lState );
	if( _hr != S_OK )
		return false;

	return ( lState == ADO::adStateOpen );
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{	
	CAXFormBase::SerializeObject( ar, pparams );

	long nVersion = 5;

	if( ar.IsLoading() )
	{
		ar >> nVersion;
		m_query.Serialize( ar );
		if( nVersion >= 2 )
		{
			ar >> m_strTumbnailTable;
			ar >> m_strTumbnailField;
			ar >> m_strTumbnailCaptionTable;
			ar >> m_strTumbnailCaptionField;
			ar >> m_sizeTumbnail;
		}
		
		if( nVersion >= 3 )
			ar >> m_sizeTumbnailView;

		if( nVersion >= 4 )
			ar >> m_sizeTumbnailBorder;

		if( nVersion >= 5 )
			ar >> m_llast_primary_key;
	}
	else
	{
		ar << nVersion;
		m_query.Serialize( ar );
		ar << m_strTumbnailTable;
		ar << m_strTumbnailField;
		ar << m_strTumbnailCaptionTable;
		ar << m_strTumbnailCaptionField;
		ar << m_sizeTumbnail;
		ar << m_sizeTumbnailView;
		ar << m_sizeTumbnailBorder;
		ar << m_llast_primary_key;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::CanBeBaseObject()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
DWORD CQueryObject::GetNamedObjectFlags()
{
	DWORD dwFlags = CDataObjectListBase::GetNamedObjectFlags();
	dwFlags |= nofCantSendTo;

	return dwFlags;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::ClearUndoRedoList()
{
	if( m_sptrParent == NULL )
		return false;

	IUndoListPtr ptrUL( m_sptrParent );

	if( ptrUL == NULL )
		return false;

	ptrUL->ClearUndoRedoList();

	return true;	
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
// ContextState
//
//
////////////////////////////////////////////////////////////////////////////////////////////
/*void CQueryObject::SaveContextState()
{
	DestroyContextState();

	if( m_sptrParent == NULL )
		return;


	IDocumentSitePtr ptrDS( m_sptrParent );
	if( ptrDS == NULL )
		return;

	
	long lPos = 0;
	ptrDS->GetFirstViewPosition( &lPos );
	while( lPos )
	{
		IUnknown* punkView = NULL;
		ptrDS->GetNextView( &punkView, &lPos );
		if( !punkView )
			continue;


		IDataContext2Ptr ptrC( punkView );
		punkView->Release();

		if( ptrC == NULL )
			continue;

		CViewContext* pi = new CViewContext;
		m_arViewContext.AddHead( pi );

		pi->m_guidKeyView = ::GetObjectKey( ptrC );



		long nObjTypeCount = 0;
		ptrC->GetObjectTypeCount( &nObjTypeCount );
		for( int i=0;i<nObjTypeCount;i++ )
		{
			BSTR bstrObjType = 0;
			ptrC->GetObjectTypeName( i, &bstrObjType );
			_bstr_t bstrType = bstrObjType;
			::SysFreeString( bstrObjType );

			long lObjPos = 0;
			ptrC->GetFirstSelectedPos( bstrType, &lObjPos );
			while( lObjPos )
			{
				IUnknown* punkObj = NULL;
				ptrC->GetNextSelected( bstrType, &lObjPos, &punkObj );
				if( !punkObj )
					continue;

				CObjectInfo* poi = new CObjectInfo;
				pi->m_arObjectInfo.AddHead( poi );
				
				poi->m_strObjectName	= ::GetObjectName( punkObj );
				poi->strObjectType		= ::GetObjectKind( punkObj );				
				poi->m_bActive = false;
				punkObj->Release();
			}

			IUnknown* punkObj = NULL;
			ptrC->GetActiveObject( bstrType, &punkObj );
			if( punkObj )
			{
				CObjectInfo* poi = new CObjectInfo;
				pi->m_arObjectInfo.AddHead( poi );
				poi->m_strObjectName	= ::GetObjectName( punkObj );
				poi->strObjectType		= ::GetObjectKind( punkObj );				
				poi->m_bActive = true;
				punkObj->Release();
			}
		}		
	}

}*/


////////////////////////////////////////////////////////////////////////////////////////////
/*void CQueryObject::RestoreContextState()
{
	

	if( m_sptrParent == NULL )
		return;

	INamedDataPtr ptrND( m_sptrParent );

	IDocumentSitePtr ptrDS( m_sptrParent );
	if( ptrDS == NULL )
		return;

	
	//set m_bBaseObject flag
	{
		POSITION posVC = m_arViewContext.GetHeadPosition();
		
		while( posVC )
		{			
			CViewContext* pvc = (CViewContext*)m_arViewContext.GetNext( posVC );

			POSITION posObj = pvc->m_arObjectInfo.GetHeadPosition();
			while( posObj )
			{
				CObjectInfo* poi = (CObjectInfo*)pvc->m_arObjectInfo.GetNext( posObj );				

				IUnknown* punkTestObj = NULL;
				punkTestObj = ::GetObjectByName( ptrND, poi->m_strObjectName, poi->strObjectType );
				IUnknownPtr ptrTestObj = punkTestObj;
				if( punkTestObj )
					punkTestObj->Release();

				
				long lPosGrp = -1;
				ptrND->GetBaseGroupFirstPos( &lPosGrp );
				while( lPosGrp )
				{
					IUnknown* punkObject = NULL;
					GuidKey guid = INVALID_KEY;
					ptrND->GetNextBaseGroup( &guid, &lPosGrp );
					ptrND->GetBaseGroupBaseObject( &guid, &punkObject );
					if( punkObject )
					{
						if( ::GetObjectKey( ptrTestObj ) == ::GetObjectKey( punkObject ) )
						{
							poi->m_bBaseObject = true;
							punkObject->Release();
							break;
						}
						
						punkObject->Release();

					}
				}

			}
		}

	}
	
	long lViewPos = 0;
	ptrDS->GetFirstViewPosition( &lViewPos );
	while( lViewPos )
	{
		IUnknown* punkView = NULL;
		ptrDS->GetNextView( &punkView, &lViewPos );
		if( !punkView )
			continue;


		IDataContext2Ptr ptrC( punkView );
		punkView->Release();

		if( ptrC == NULL )
			continue;


		CViewContext* pvcFind = NULL;
		POSITION pos = m_arViewContext.GetHeadPosition();
		
		while( pos )
		{			
			CViewContext* pvc = (CViewContext*)m_arViewContext.GetNext( pos );
			if( pvc->m_guidKeyView == ::GetObjectKey( ptrC ) )
			{
				pvcFind = pvc;
				break;
			}
		}

		if( pvcFind )
		{			


			//Deselect all
			{
				long lCount = 0;
				VERIFY( S_OK == ptrC->GetObjectTypeCount( &lCount ) );
				for( long l=0;l<lCount;l++ )
				{
					BSTR bstrTypeName = 0;
					VERIFY( S_OK == ptrC->GetObjectTypeName( l, &bstrTypeName ) );
					VERIFY( S_OK == ptrC->UnselectAll( bstrTypeName ) );
					::SysFreeString( bstrTypeName );	bstrTypeName = 0;
				}					
			}


			//at first activate base object
			POSITION posObj = pvcFind->m_arObjectInfo.GetHeadPosition();
			while( posObj )
			{
				CObjectInfo* poi = (CObjectInfo*)pvcFind->m_arObjectInfo.GetNext( posObj );				
				if( poi->m_bActive )continue;
				if( poi->m_bBaseObject )
				{
					IUnknown* punkObj = NULL;
					punkObj = ::GetObjectByName( ptrND, poi->m_strObjectName, poi->strObjectType );
					if( punkObj )
					{
						//[AY]???
						ptrC->SetObjectSelect( punkObj, TRUE );
						//ptrC->SetActiveObject( 0, punkObj, TRUE );
						punkObj->Release();
						return;
					}

				}
			}
			//and now select not base object
			posObj = pvcFind->m_arObjectInfo.GetHeadPosition();
			while( posObj )
			{
				CObjectInfo* poi = (CObjectInfo*)pvcFind->m_arObjectInfo.GetNext( posObj );				
				if( poi->m_bActive )continue;
				if( !poi->m_bBaseObject )
				{
					IUnknown* punkObj = NULL;
					punkObj = ::GetObjectByName( ptrND, poi->m_strObjectName, poi->strObjectType );
					if( punkObj )
					{
						ptrC->SetObjectSelect( punkObj, TRUE );
						punkObj->Release();
					}

				}
			}

			posObj = pvcFind->m_arObjectInfo.GetHeadPosition();
			while( posObj )
			{
				CObjectInfo* poi = (CObjectInfo*)pvcFind->m_arObjectInfo.GetNext( posObj );				
				if( poi->m_bActive )continue;
				if( !poi->m_bBaseObject )
				{
					IUnknown* punkObj = NULL;
					punkObj = ::GetObjectByName( ptrND, poi->m_strObjectName, poi->strObjectType );
					if( punkObj )
					{
						ptrC->SetObjectSelect( punkObj, TRUE );
						punkObj->Release();
					}

				}
			}
		}
	}

}*/

////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::DestroyContextState()
{
	
	POSITION pos = m_arViewContext.GetHeadPosition();
	
	while( pos )
	{
		CViewContext* pkey = (CViewContext*)m_arViewContext.GetNext( pos );
		delete pkey;
	}

	m_arViewContext.RemoveAll();
	

}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::SetBaseTable( BSTR bstr )
{
	_try_nested(CQueryObject, Query, SetBaseTable )
	{	
		pThis->m_query.m_strBaseTable = ::_MakeLower( bstr );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetBaseTable( BSTR *pbstr )
{
	_try_nested(CQueryObject, Query, GetBaseTable)
	{	
		*pbstr = ::_MakeLower( pThis->m_query.m_strBaseTable ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::IsSimpleQuery( BOOL* pbSimple )
{
	_try_nested(CQueryObject, Query, IsSimpleQuery )
	{	
		*pbSimple = pThis->m_query.m_bSimpleQuery;
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::SetSimpleQuery( BOOL bSimple )
{
	_try_nested(CQueryObject, Query, IsSimpleQuery )
	{	
		pThis->m_query.m_bSimpleQuery = bSimple;
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::SetManualSQL( BSTR bstrSQL )
{
	_try_nested(CQueryObject, Query, GetNativeSQL)
	{	
		pThis->m_query.m_strSQL = bstrSQL;
		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetManualSQL( BSTR* pbstrSQL )
{
	_try_nested(CQueryObject, Query, GetNativeSQL)
	{	
		*pbstrSQL = pThis->m_query.m_strSQL.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetSQL( BSTR* pbstrSQL )
{
	_try_nested(CQueryObject, Query, GetFieldsCount)
	{					
		*pbstrSQL = pThis->m_query.GetSQL( pThis->m_sptrParent ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetFieldsCount( int* pnCount )
{
	_try_nested(CQueryObject, Query, GetFieldsCount)
	{					
		*pnCount = 0;

		*pnCount = pThis->m_query.m_arrFields.GetSize();

		return S_OK;
	}
	_catch_nested;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetField( int nIndex, BSTR* pbstrInnerName, BSTR* pbstrTable, BSTR* pbstrField )
{
	_try_nested(CQueryObject, Query, GetField)
	{	
		if( nIndex < 0 || nIndex >= pThis->m_query.m_arrFields.GetSize() )
			return S_FALSE;

		CQueryField* pf = pThis->m_query.m_arrFields[nIndex];

		if( pbstrInnerName )
			*pbstrInnerName		= pf->m_strInnerName.AllocSysString();

		if( pbstrTable )
			*pbstrTable			= pf->m_strTable.AllocSysString();

		if( pbstrField )		
			*pbstrField			= pf->m_strField.AllocSysString();


		return S_OK;
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::SetActiveField( int nIndex )
{
	_try_nested(CQueryObject, Query, SetActiveField)
	{	
		if( pThis->m_sptrParent )
		{
			if( pThis->m_nActiveField >= 0 && pThis->m_nActiveField < pThis->m_query.m_arrFields.GetSize() )
			{
				CQueryField* pField = pThis->m_query.m_arrFields[pThis->m_nActiveField];
				pThis->m_sptrParent->FireEvent( _bstr_t(szDBaseEventActiveFieldLost), 
									pThis->m_sptrParent, pThis->m_sptrParent, 
									_bstr_t( (LPCSTR)pField->m_strTable ), 
									_bstr_t( (LPCSTR)pField->m_strField ), _variant_t( (long)pThis->m_nActiveField ) );

			}
		}

		if( nIndex < 0 || nIndex >= pThis->m_query.m_arrFields.GetSize() )
		{
			//support toolbar update in ShellFrame
			pThis->m_nActiveField = -1;
			INamedDataPtr ptrPrivND = pThis->GetPrivateNamedData();
			if( ptrPrivND )
				::SetValue( ptrPrivND, SECTION_ACTIVE_QUERY_PROPERTY, "ActiveField", (long)pThis->m_nActiveField );
			::ForceAppIdleUpdate();
			return S_OK;
		}

		CQueryField* pField = pThis->m_query.m_arrFields[nIndex];
		pThis->m_sptrParent->FireEvent( _bstr_t(szDBaseEventActiveFieldSet), 
							pThis->m_sptrParent, pThis->m_sptrParent, 
							_bstr_t( (LPCSTR)pField->m_strTable ), 
							_bstr_t( (LPCSTR)pField->m_strField ), _variant_t( (long)nIndex ) );


		pThis->m_nActiveField = nIndex;
		
		//Write active field property to private named data
		{
			INamedDataPtr ptrPrivND = pThis->GetPrivateNamedData();
			if( ptrPrivND )
				::SetValue( ptrPrivND, SECTION_ACTIVE_QUERY_PROPERTY, "ActiveField", (long)pThis->m_nActiveField );
		}

		//support toolbar update in ShellFrame
		::ForceAppIdleUpdate();

		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetActiveField( int* pnIndex )
{
	_try_nested(CQueryObject, Query, GetActiveField)
	{	
		*pnIndex = pThis->m_nActiveField;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQuery::GetFieldLen( int nIndex, long* pLen )
{
	_try_nested(CQueryObject, Query, GetActiveField)
	{	
		if( !pLen )
			return E_INVALIDARG;

		*pLen = 0;
		if( !pThis->IsOpen() )
			return S_FALSE;

		if( nIndex < 0 || nIndex >= pThis->m_query.m_arrFields.GetSize() )
			return S_FALSE;

		CQueryField* pField = pThis->m_query.m_arrFields[nIndex];

		ADO::FieldPtr ptrField = pField->GetFieldPtr();
		if( ptrField )
		{
			if( S_OK == ptrField->get_ActualSize( pLen ) )
			{
				return S_OK;
			}
		}
		
		*pLen = 0;
		 
		return S_FALSE;
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
tagVARIANT g_varEmpty={VT_EMPTY};
//::ZeroMemory( &g_varEmpty, sizeof(tagVARIANT) );



////////////////////////////////////////////////////////////////////////////////////////////
QueryState CQueryObject::GetState()
{
	QueryState qs = qsClose;
		
	if( IsOpen( ) )
	{
		if( IsRecordsetModified() )
			qs = qsEdit;
		else
			qs = qsBrowse;

	}

	return qs;
}

/////////////////////////////////////////////////////////////////////////////
void CQueryObject::InitFieldsBuffers()
{
	ClearFieldsBuffers();

	if( !IsOpen() )
		return;	

	ISelectQueryPtr spQuery( GetControllingUnknown() );	


	INamedDataPtr ptrPrivND = GetPrivateNamedData();
	int nfield = 0;
	
	try
	{

		ADO::FieldsPtr ptrFields = m_spRecordset->Fields;
		if( ptrFields == 0 ) 
			return;

		{
			CString strSection = SECTION_ACTIVE_QUERY_PROPERTY;
			strSection += "\\Tumbnail";				

			::SetValue( ptrPrivND, strSection, "Table", (LPCSTR)m_strTumbnailTable );
			::SetValue( ptrPrivND, strSection, "Field", (LPCSTR)m_strTumbnailField );

			strSection = SECTION_ACTIVE_QUERY_PROPERTY;
			strSection += "\\TumbnailCaption";				

			::SetValue( ptrPrivND, strSection, "Table", (LPCSTR)m_strTumbnailCaptionTable );
			::SetValue( ptrPrivND, strSection, "Field", (LPCSTR)m_strTumbnailCaptionField );
		}

		long lFieldsCount = ptrFields->Count;

		for( long i=0;i<lFieldsCount;i++ )
		{

			ADO::FieldPtr ptrField = ptrFields->GetItem( i );
			if( ptrField == 0 )
				continue;

			ADO::PropertiesPtr ptrProperties = ptrField->Properties;
			if( ptrProperties == 0 )
				continue;

			ADO::PropertyPtr ptrPropTable = ptrProperties->GetItem( "BASETABLENAME" );
			ADO::PropertyPtr ptrPropField = ptrProperties->GetItem( "BASECOLUMNNAME" );

			if( ptrPropTable == 0 || ptrPropField == 0 )
				continue;

			CQueryField* pField = new CQueryField;
			m_query.m_arrFields.Add( pField );


			pField->m_strInnerName	= (LPCSTR)ptrField->Name;

			pField->m_strInnerName.MakeLower();

			pField->m_strTable	= ptrPropTable->Value.bstrVal;
			pField->m_strField	= ptrPropField->Value.bstrVal;


			pField->m_strTable.MakeLower();
			pField->m_strField.MakeLower();
			

			FieldType fieldType = ::GetFieldType( m_sptrParent, pField->m_strTable, pField->m_strField );

			pField->Init( fieldType, m_sptrParent, m_spRecordset, i );

			if( ptrPrivND )
			{
				CString strFieldName;
				strFieldName.Format( "field%d", nfield++ );

				CString strSection = SECTION_DBASEFIELDS_INFO;
				strSection += "\\";
				strSection += strFieldName;

				::SetValue( ptrPrivND, strSection, "Table", (LPCSTR)pField->m_strTable );
				::SetValue( ptrPrivND, strSection, "Field", (LPCSTR)pField->m_strField );
				::SetValue( ptrPrivND, strSection, "InnerName", (LPCSTR)pField->m_strInnerName );

				CString strType, strVT5Type;
				if( fieldType == ftDigit )
					strType	= "Number";
				else if( fieldType == ftString )
					strType	= "String";
				else if( fieldType == ftDateTime )
					strType	= "DateTime";
				else if( fieldType == ftVTObject )
				{
					strType	= "VTObject";
					strVT5Type = GetFieldVTObjectType( ptrPrivND, pField->m_strTable, pField->m_strField );
				}
				else if( fieldType == ftPrimaryKey )
					strType	= "PrimaryKey";
				else if( fieldType == ftForeignKey )
					strType	= "ForeignKey";
				else if( fieldType == ftNotSupported )
					strType	= "NotSupported";				

				::SetValue( ptrPrivND, strSection, "Type", (LPCSTR)strType );
				::SetValue( ptrPrivND, strSection, "VTtype", (LPCSTR)strVT5Type );

				//set caption if not exeist

				if( !pField->m_strTable.IsEmpty() && !pField->m_strField.IsEmpty() )
				{
					CString strCaption = GetFieldCaption( m_sptrParent, pField->m_strTable, pField->m_strField );
					IDBaseStructPtr ptrS( m_sptrParent );
					if( ptrS )
					{
						CString strEntry;
						strEntry.Format( "%s.%s", (LPCSTR)pField->m_strTable, (LPCSTR)pField->m_strField );

						::_SetValue( ptrPrivND, SECTION_FIELD_CAPTIONS,	strEntry, strCaption );						
					}
				}

			}
			
		}	
	}
	catch(...)
	{

	}

	DeleteNotExistGridFields();
}

/////////////////////////////////////////////////////////////////////////////
void CQueryObject::ClearFieldsBuffers()
{
	for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
		delete m_query.m_arrFields[i];

	m_query.m_arrFields.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryObject::IsRecordsetModified()
{
	for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
	{
		CQueryField* pField = m_query.m_arrFields[i];
		if( pField->GetModifiedFlag() )
			return true;
	}

	return false;
}

bool CQueryObject::InitDefValues( bool bAfterInsert )
{	
	for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
	{
		CQueryField* pField = m_query.m_arrFields[iField];
		pField->InitDefValues( bAfterInsert );
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//Force reRead VTObject, which visible now and update views
bool CQueryObject::OnRecordChange()
{
	CWaitCursor cursor;

	if( !IsOpen() )
		return false;

	for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
	{
		CQueryField* pField = m_query.m_arrFields[iField];
		pField->RecordChange();
	}

	return true;

	::ForceAppIdleUpdate();
}

bool CQueryObject::OnCancel()
{
	if( !IsOpen() )
		return false;


	if( m_spRecordset )
	{
		ADO::FieldsPtr ptrFields = m_spRecordset->Fields;
		if( ptrFields )
			VERIFY( S_OK == ptrFields->Refresh() );
	}


	for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
	{
		CQueryField* pField = m_query.m_arrFields[iField];		
		pField->Cancel();		
	}
	return true;

}

bool CQueryObject::OnUpdate()
{
	if( !IsOpen() )
		return false;

	for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
	{
		CQueryField* pField = m_query.m_arrFields[iField];
		if( !pField->Store() )
			return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	/*
	if( !strcmp( pszEvent, szDBaseEventConnectionOpen ) ||
		!strcmp( pszEvent, szDBaseEventConnectionClose ) ||
		!strcmp( pszEvent, szDBaseEventConnectionFailed ) ||
		!strcmp( pszEvent, szDBaseEventAfterNavigation ) ||
		!strcmp( pszEvent, szDBaseEventAfterQueryOpen ) ||
		!strcmp( pszEvent, szDBaseEventBeforQueryClose ) ||
		!strcmp( pszEvent, szDBaseEventQueryOpenFailed )
		)
		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );
		*/
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::CanGetValue()
{
	if( !IsOpen() )
		return false;
	return true;
}


int CQueryObject::GetFieldIndexFromSection( BSTR bstrTableField, sptrIDBaseDocument spDBDoc, short& nFieldType )
{
	IDBaseStructPtr ptrDBS( spDBDoc );
	if( ptrDBS == 0 ) 
		return -1;

	CString strField, strTable;
	::GetTableFieldFromSection( bstrTableField, strTable, strField, false );


	if( S_OK != ptrDBS->GetFieldInfo(
			_bstr_t(strTable), _bstr_t(strField),
			NULL, &nFieldType,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL
			 )  )
		return -1;

	
	//now find field index in query
	int nIndex = -1;
	for( int i=0;i<m_query.m_arrFields.GetSize();i++)
	{
		CQueryField* pQueryField = m_query.m_arrFields[i];
		if( pQueryField->m_strTable == strTable &&
			pQueryField->m_strField == strField
			)
			nIndex = i;
	}
	

	return nIndex;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Open()
{
	_try_nested(CQueryObject, SelectQuery, Open)
	{	
		CWaitCursor cursor;

		pThis->m_bCanProcessFind = false;

		UpdateInteractive( TRUE );
		

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		pThis->ClearFieldsBuffers();		

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforQueryOpen), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		

		IDBConnectionPtr ptrDBC( spDBDoc );
		if( ptrDBC == 0 )
			return S_FALSE;

		IUnknown* pUnkConnection = NULL;
		ptrDBC->GetConnection( &pUnkConnection );

		if( pUnkConnection == NULL )
			return S_FALSE;

		ADO::_ConnectionPtr spConnection = pUnkConnection;
		pUnkConnection->Release();

		if( spConnection == NULL )
			return S_FALSE;

		if( pThis->IsOpen() )
			Close();

		if( pThis->m_spRecordset == NULL )
		{
			_START_CATCH
			pThis->m_spRecordset.CreateInstance( __uuidof(ADO::Recordset) );
			_END_CATCH(S_FALSE)
		}
		
		pThis->ClearUndoRedoList();


		_bstr_t bstrDBaseFieldsSection(SECTION_DBASERECORDNAMEDDATA);

		//at first remove all SECTION_DBASEFIELDS entries in NamedData
		sptrINamedData spND( spDBDoc );
		if( spND == NULL )
			return E_FAIL;

		IS_OK( spND->DeleteEntry( bstrDBaseFieldsSection ) );


		//Try to open query
		CString strQuery = pThis->m_query.GetSQL( spDBDoc );
		strQuery.MakeLower();

		_variant_t vConn = (IDispatch*)spConnection;

		if( spConnection->State != ADO::adStateOpen )
			return S_FALSE;
		
		

		_START_CATCH_REPORT

		pThis->m_spRecordset->CursorLocation = ::GetCursorLocation( strQuery );

		
		//adUseClient
		CTimeTest time(true, "Open recordset ...:");
		time.m_bEnableFileOutput = false;				


		pThis->m_spRecordset->Open( _bstr_t(strQuery), vConn, 
					ADO::adOpenKeyset, ADO::adLockPessimistic, ADO::adCmdText);

		pThis->m_state = qsBrowse;		

		ADO::CursorLocationEnum curLocNew = pThis->m_spRecordset->CursorLocation;
		bool bNeedRequery = false;

		pThis->InitFieldsBuffers();

		if( pThis->ResolveSortProblem( strQuery, pThis->m_spRecordset->CursorLocation, 
									bNeedRequery, curLocNew ) )
		{
			if( bNeedRequery )
			{
				pThis->m_spRecordset = 0;
				pThis->m_spRecordset.CreateInstance( __uuidof(ADO::Recordset) );
				
				pThis->m_spRecordset->CursorLocation = curLocNew;
				
				pThis->m_spRecordset->Open( _bstr_t(strQuery), vConn, 
						ADO::adOpenKeyset, ADO::adLockPessimistic, ADO::adCmdText);

				pThis->InitFieldsBuffers();
			}

		}

		time.Report();

		_END_CATCH_REPORT(S_FALSE)		

		pThis->InitDefValues( false );
		
		long lrecord = pThis->_find_record( pThis->_get_last_primary_key() );
		if( lrecord > 0 )
			GoToRecord( lrecord );
		else
			pThis->OnRecordChange();		

		pThis->AnalizeBlank();
		pThis->AnalizeQueryProperty();

		//Register only when succeeded open
		pThis->_RegisterOnDBController( );
		pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterQueryOpen), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );				


		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		::ForceAppIdleUpdate();
		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Close()
{
	_try_nested(CQueryObject, SelectQuery, Close)
	{	
		pThis->m_bCanProcessFind = false;

		//store primary key
		if( pThis->IsOpen() )
			pThis->_save_primary_key( true );

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();		

		if( !pThis->IsOpen() )
		{
			pThis->m_spRecordset = 0;
			pThis->ClearFieldsBuffers();

			return S_OK;
		}


		pThis->ClearUndoRedoList();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforQueryClose), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		pThis->ClearFieldsBuffers();

		//Closing if Open
		_START_CATCH
		if( pThis->IsOpen() )
		{
			pThis->SaveContextState();
			if( pThis->m_spRecordset )
				pThis->m_spRecordset->Close();						

			pThis->m_spRecordset = 0;
			pThis->ClearFieldsBuffers();
		}

		pThis->m_state = qsClose;		

		_END_CATCH(S_FALSE)		


		{
			_bstr_t bstrDBaseFieldsSection(SECTION_DBASERECORDNAMEDDATA);

			//at first remove all SECTION_DBASEFIELDS entries in NamedData
			sptrINamedData spND( spDBDoc );
			if( spND == NULL )
				return E_FAIL;
			IS_OK( spND->DeleteEntry( bstrDBaseFieldsSection ) );
		}		

		
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterQueryClose), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		::ForceAppIdleUpdate();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::IsOpen(BOOL* pbOpen)
{
	_try_nested(CQueryObject, SelectQuery, IsOpen)
	{			
		*pbOpen = (pThis->IsOpen() == TRUE);
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Edit()
{
	_try_nested(CQueryObject, SelectQuery, Edit)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		
		BOOL bSucceded = FALSE;
		GetSupports( (short)ADO::adUpdate, &bSucceded );
		if( !bSucceded)
			return S_FALSE;

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforEditRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );		
		pThis->m_state = qsEdit;		
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterEditRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::UpdateInteractive( BOOL bAskOnlyObjectChanges )
{
	_try_nested(CQueryObject, SelectQuery, UpdateInteractive)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		if( !pThis->IsRecordsetModified() )
			return S_OK;

		bool bNeedAsk = false;
		for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
		{
			CQueryField* pField = pThis->m_query.m_arrFields[i];
			if( pField->NeedAskUpdateRecord() )
			{
				bNeedAsk = true;
				break;
			}
		}

		if( !bAskOnlyObjectChanges )
			bNeedAsk = true;

		if( bNeedAsk )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if( AfxMessageBox( IDS_WARNING_UPDATE, MB_YESNO ) == IDYES )
				return Update();
			else
				return Cancel();
		}

		return Update();
		
	}
	_catch_nested;

}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Update()
{
	_try_nested(CQueryObject, SelectQuery, Update)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		CWaitCursor wait;

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		if( !pThis->IsOpen() )
			return S_FALSE;

		long lCount = -1;
		GetRecordCount( &lCount );
		if( lCount < 1 )
			return S_FALSE;

		BOOL bSucceded = FALSE;
		
		bool bNeedUpdate = pThis->IsRecordsetModified();

		if( !pThis->OnUpdate() )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_WARNING_UPDATE_FAILED, MB_ICONSTOP );
			pThis->m_spRecordset = 0;
			Close();
			Open();			
			return E_FAIL;
		}


		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforUpdateRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );


		CTimeTest time(true, "Update time...:");
		time.m_bEnableFileOutput = false;				
		_START_CATCH_REPORT
		if( bNeedUpdate )
		{			
			pThis->m_spRecordset->Update();
			//pThis->m_spRecordset->UpdateBatch( ADO::adAffectCurrent );
		}
		_END_CATCH_REPORT(S_FALSE)		

		pThis->m_state = qsBrowse;
		
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterUpdateRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Cancel()
{
	_try_nested(CQueryObject, SelectQuery, Cancel)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		CWaitCursor wait;


		//pThis->SaveContextState();

		if( !pThis->IsRecordsetModified() )
			return_and_assert(S_FALSE);


		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		INamedDataPtr ptrND( spDBDoc );

		if( spDBDoc == NULL || ptrND == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		if( !pThis->IsOpen() )
			return S_FALSE;

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforCancelRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		
		_START_CATCH_REPORT
		pThis->m_spRecordset->Cancel();
		_END_CATCH_REPORT(S_FALSE)
		
		pThis->m_state = qsBrowse;
		
		pThis->OnCancel();		

		
		
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterCancelRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		//pThis->RestoreContextState();

		return S_OK;
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Insert()
{
	_try_nested(CQueryObject, SelectQuery, Insert)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		if( !pThis->IsOpen() )
			return S_FALSE;

		//pThis->SaveContextState();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		UpdateInteractive( TRUE );

		BOOL bSucceded = FALSE;
		GetSupports( (short)ADO::adAddNew, &bSucceded );
		if( !bSucceded)
			return S_FALSE;

		long lRecCount = 0;
		GetRecordCount( &lRecCount );

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforInsertRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH_REPORT
		pThis->m_spRecordset->AddNew();		
		
		pThis->m_state = qsInsert;
		
		pThis->InitDefValues( true );

		if( lRecCount == 0 )
			pThis->m_spRecordset->AbsolutePosition = (ADO::PositionEnum)1;			

		Update();

		//long lcur_record = pThis->m_spRecordset->AbsolutePosition;
		//TRACE("Current record = %d\n", lcur_record );

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );

		//pThis->m_spRecordset->UpdateBatch( ADO::adAffectCurrent );

		pThis->OnRecordChange();

		_END_CATCH_REPORT(S_FALSE)
		
		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterInsertRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		::ForceAppIdleUpdate();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::Delete()
{
	return DeleteEx(FALSE);
}


HRESULT CQueryObject::XSelectQuery::DeleteEx(BOOL bQuiet)
{
	_try_nested(CQueryObject, SelectQuery, Delete)
	{	
		if( !pThis->CanModifyDatabase() )
			return S_FALSE;

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if (!bQuiet)
		{
			if( AfxMessageBox( IDS_WARNING_DELETE_RECORD, MB_ICONSTOP | MB_YESNO ) != IDYES )
				return S_FALSE;
		}

		if( !pThis->IsOpen() )
			return S_FALSE;


		//pThis->SaveContextState();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		BOOL bSucceded = FALSE;
		GetSupports( (short)ADO::adDelete, &bSucceded );
		if( !bSucceded )
			return S_FALSE;

		long nRecordCount = 0;
		GetRecordCount( &nRecordCount );
		if( nRecordCount < 1 )
			return S_FALSE;

		long nCurRecord = -1;
		GetCurrentRecord( &nCurRecord );
		if( nCurRecord < 1 || nCurRecord > nRecordCount )
			return false;

		BOOL bBOF, bEOF;
		IsBOF( &bBOF );
		IsEOF( &bEOF );			

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforDeleteRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );


		_START_CATCH_REPORT
		pThis->m_spRecordset->Delete( ADO::adAffectCurrent );
		_END_CATCH_REPORT(S_FALSE)
		
		
		_START_CATCH_REPORT

		if( nRecordCount > 1 )
		{	
			if( !bEOF )
				pThis->m_spRecordset->AbsolutePosition = (ADO::PositionEnum)nCurRecord;
			else if( !bBOF )				
			{
				pThis->m_spRecordset->AbsolutePosition = (ADO::PositionEnum)(nCurRecord-1);
			}
		}
		_END_CATCH_REPORT(S_FALSE)
		
		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );		

		pThis->m_state = qsBrowse;

		//pThis->InitFieldsBuffers();
		pThis->OnRecordChange();
		//pThis->TestBofEof();		


		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterDeleteRecord), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::MoveFirst()
{
	_try_nested(CQueryObject, SelectQuery, MoveFirst)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;

		//pThis->SaveContextState();

		UpdateInteractive( TRUE );
		pThis->ClearUndoRedoList();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		BOOL bSucceded = FALSE;
		IsBOF( &bSucceded );
		if( bSucceded)
			return S_FALSE;


		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH_REPORT
		pThis->m_spRecordset->MoveFirst();
		_END_CATCH_REPORT(S_FALSE)

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );
		

		pThis->m_state = qsBrowse;

		//pThis->InitFieldsBuffers();
		pThis->OnRecordChange();

		//pThis->TestBofEof();

		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventMoveFirst), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::MoveNext()
{
	_try_nested(CQueryObject, SelectQuery, MoveNext)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;

		//pThis->SaveContextState();

		UpdateInteractive( TRUE );
		pThis->ClearUndoRedoList();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		BOOL bSucceded = FALSE;
		IsEOF( &bSucceded );
		if( bSucceded)
			return S_FALSE;

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH_REPORT
		pThis->m_spRecordset->MoveNext();
		_END_CATCH_REPORT(S_FALSE)

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );

		pThis->m_state = qsBrowse;
		//pThis->TestBofEof();

		pThis->OnRecordChange();

		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventMoveNext), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::MovePrev()
{
	_try_nested(CQueryObject, SelectQuery, MovePrev)
	{	

		if( !pThis->IsOpen() )
			return S_FALSE;

		//pThis->SaveContextState();

		UpdateInteractive( TRUE );
		pThis->ClearUndoRedoList();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		BOOL bSucceded = FALSE;
		IsBOF( &bSucceded );
		if( bSucceded)
			return S_FALSE;

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH_REPORT
		pThis->m_spRecordset->MovePrevious();
		_END_CATCH_REPORT(S_FALSE)

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );
		

		pThis->m_state = qsBrowse;
		//pThis->TestBofEof();
		pThis->OnRecordChange();

		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventMovePrev), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::MoveLast()
{
	_try_nested(CQueryObject, SelectQuery, MoveLast)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;
		
		//pThis->SaveContextState();

		UpdateInteractive( TRUE );
		pThis->ClearUndoRedoList();

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();


		BOOL bSucceded = FALSE;
		IsEOF( &bSucceded );
		if( bSucceded)
			return S_FALSE;

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH_REPORT
		pThis->m_spRecordset->MoveLast();
		_END_CATCH_REPORT(S_FALSE)

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );
		
		pThis->m_state = qsBrowse;
		//pThis->TestBofEof();
		pThis->OnRecordChange();
		//pThis->RestoreContextState();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventMoveLast), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetLastError(BSTR* bstrError)
{
	_try_nested(CQueryObject, SelectQuery, GetLastError)
	{			
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetValue( BSTR bstrName, tagVARIANT *pvar )
{
	_try_nested(CQueryObject, SelectQuery, GetValue)
	{	
		sptrIDBaseDocument spDBDoc = pThis->m_sptrParent;
		if( spDBDoc == NULL )
			return E_FAIL;		

		INamedDataPtr ptrPrivND = pThis->GetPrivateNamedData();

		if( ptrPrivND == NULL )
			return_and_assert( E_FAIL );


		bool bOpen = pThis->IsOpen();

		if( pThis->m_bPrivateMode )
		{
			if( !bOpen )
				return_and_assert( E_FAIL );
			
			CQueryField* pFieldCache = NULL;
			short nFieldType;
			int nIndex = pThis->GetFieldIndexFromSection( bstrName, spDBDoc, nFieldType );
			if( nIndex >= 0 && nIndex < pThis->m_arCacheFields.GetSize() )
			{
				pFieldCache = pThis->m_arCacheFields[nIndex];
			}				

			if( pFieldCache )
			{
				_variant_t var;
				
				long lCurRecord = -1;
				GetCurrentRecord( &lCurRecord );			
				
				if( lCurRecord == pThis->m_lBookMark ) 			
					pFieldCache = pThis->m_query.m_arrFields[nIndex];				
				
				VERIFY( pFieldCache->GetPrivateValue( var ) );				
				
				
				*pvar = var.Detach();
				
				return S_OK;
			}			
		}		
		
		//13.02.2003. Optimization. Cos values read OnRecordChange
		/*
		bool bProcess = false;
		for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
		{
			if( bProcess )
				continue;

			CQueryField* pField = pThis->m_query.m_arrFields[i];
			bProcess = pField->OnGetValue( bstrName, pvar );
		}
		*/
		
		return ptrPrivND->GetValue( bstrName, pvar );
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetValue( BSTR bstrName, const tagVARIANT var, BOOL* pbPlaceToRecordset )
{
	_try_nested(CQueryObject, SelectQuery, SetValue)
	{	
		if( pThis->m_bPrivateMode )
			return_and_assert( E_FAIL );

		bool bOpen = pThis->IsOpen();

		sptrIDBaseDocument spDBDoc = pThis->m_sptrParent;
		INamedDataPtr ptrPrivND = pThis->GetPrivateNamedData();

		if( ptrPrivND == NULL || spDBDoc == NULL )
			return_and_assert( E_FAIL );

		//If not open get out here
		if( !bOpen || !pThis->CanModifyDatabase() )
			return ptrPrivND->SetValue( bstrName, var );

		//Field router
		bool bProcess = false;

		//try to force( find exact field by entry name)
		CString strObjectName;
		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			strObjectName = ::GetObjectName( var.punkVal );

		CQueryField* pFieldForce = NULL;
		{
			CString strEntry = bstrName;
			for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
			{
				CQueryField* pField = pThis->m_query.m_arrFields[i];
				if( pField->GetFieldFullPath() == strEntry )
				{
					pFieldForce = pField;
					break;
				}
				
				if( !strObjectName.IsEmpty() && pField->GetAsTableField() == strObjectName )
				{
					pFieldForce = pField;
					break;
				}				
			}
		}

		bool bWasSetValue = false;

		if( pFieldForce )
			bProcess = pFieldForce->OnSetValue( bstrName, var, !bWasSetValue, bWasSetValue );

		if( !bProcess )
		{
			for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
			{
				if( bProcess )
					continue;			
				
				CQueryField* pField = pThis->m_query.m_arrFields[i];
				bProcess = pField->OnSetValue( bstrName, var, !bWasSetValue, bWasSetValue );			
			}
		}

		if( !bProcess && !bWasSetValue )
		{
			ptrPrivND->SetValue( bstrName, var );
			return S_OK;
		}
		else
		{
			if( pbPlaceToRecordset )
				*pbPlaceToRecordset = TRUE;
		}



		pThis->m_state = qsEdit;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::DeleteEntry( BSTR bstrName )
{
	_try_nested(CQueryObject, SelectQuery, DeleteEntry)
	{			
		bool bOpen = pThis->IsOpen();

		sptrIDBaseDocument spDBDoc = pThis->m_sptrParent;
		INamedDataPtr ptrPrivND = pThis->GetPrivateNamedData();

		if( ptrPrivND == NULL || spDBDoc == NULL )
			return_and_assert( E_FAIL );

		//If not open get out here
		if( !bOpen || !pThis->CanModifyDatabase() )
			return ptrPrivND->DeleteEntry( bstrName );

		CString strEntry = bstrName;
		//try to force( find exact field by entry name)
		CQueryField* pFieldForce = NULL;
		{
			CString strEntry = bstrName;
			for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
			{
				CQueryField* pField = pThis->m_query.m_arrFields[i];
				if( pField->GetFieldFullPath() == strEntry )
					pFieldForce = pField;
			}
		}

		bool bProcess = false;

		if( pFieldForce )
			bProcess = pFieldForce->OnDeleteEntry( bstrName );

		//Field router		
		for( int i=0;i<pThis->m_query.m_arrFields.GetSize();i++ )
		{
			if( bProcess )
				break;

			CQueryField* pField = pThis->m_query.m_arrFields[i];
			bProcess = pField->OnDeleteEntry( bstrName );
		}

		if( !bProcess )
			return ptrPrivND->DeleteEntry( bstrName );

		if( bProcess )
			pThis->m_state = qsEdit;

		return S_OK;

	}
	_catch_nested;

}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::IsAvailableField( BSTR bstrTable, BSTR bstrField, BOOL* pbAvailable )
{
	_try_nested(CQueryObject, SelectQuery, IsAvailableField)
	{			
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		IDBaseStructPtr spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;


		*pbAvailable = FALSE;

		short nFieldType = -1;		

		if(  S_OK != spDBDoc->GetFieldInfo(	bstrTable, bstrField, 
											0, &nFieldType,
											0, 0, 0, 0, 0, 0 )
			) 
			return S_FALSE;


		if( (FieldType)nFieldType == ftNotSupported)
			return S_FALSE;

		*pbAvailable = TRUE;

		return S_OK;


	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetRecordCount( long* pnRecordCount )
{
	_try_nested(CQueryObject, SelectQuery, GetRecordCount)
	{	

		if( !pThis->IsOpen() )
			return S_FALSE;
		
		*pnRecordCount = 0;

		_START_CATCH		

		*pnRecordCount = pThis->m_spRecordset->RecordCount;

		_END_CATCH(S_FALSE)


		return S_OK;
	}
	_catch_nested;

}


//In this mode only simple data & tumbnail can read

////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::InitCacheBuffers()
{
	DestroyCacheBuffers();
	if( m_spRecordset == NULL )
		return;	

	ADO::FieldsPtr FieldsPtr = m_spRecordset->Fields;
	if( FieldsPtr == NULL )
		return;

	int nCount = FieldsPtr->Count;

	for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
	{
		if( i >= nCount )
			continue;

		CQueryField* pFieldSrc = m_query.m_arrFields[i];
		
		CQueryField* pFieldCache = new CQueryField;
		m_arCacheFields.Add( pFieldCache );
		pFieldCache->InitFrom( pFieldSrc );		
		pFieldCache->SetMasterField( pFieldSrc );

		pFieldCache->Init( pFieldCache->GetFieldType(), 
							m_sptrParent, m_spRecordset, i 
							);
		
		//pFieldCache->SetMasterField( pFieldSrc );
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::DestroyCacheBuffers()
{
	for( int i=0;i<m_arCacheFields.GetSize();i++ )
		delete m_arCacheFields[i];

	m_arCacheFields.RemoveAll();
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::EnterPrivateMode()
{
	_try_nested(CQueryObject, SelectQuery, EnterPrivateMode)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;

		if( pThis->m_bPrivateMode )
			return S_OK;

		pThis->m_bPrivateMode = true;

		pThis->InitCacheBuffers();

		pThis->varPrivateBookmark = pThis->m_spRecordset->Bookmark;
		GetCurrentRecord( &pThis->m_lBookMark );		

		return S_OK;

	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetCurrentRecord( long nCurRecord, long* pnRealRecord )
{
	_try_nested(CQueryObject, SelectQuery, SetPrivateRecord)
	{	

		//pThis->DestroyCacheBuffers();

		for( int i=0;i<pThis->m_arCacheFields.GetSize();i++ )
		{			
			pThis->m_arCacheFields[i]->ResetContent();
		}			


		if( !pThis->m_bPrivateMode )
		{
			ASSERT( FALSE );
			//Only for tumbnail reading and grid update
			return S_OK;
		}

		if( nCurRecord <= 0 || nCurRecord > pThis->m_spRecordset->RecordCount ) 
			return S_FALSE;


		*pnRealRecord = ADO::adPosUnknown;
		
		_START_CATCH
		pThis->m_spRecordset->AbsolutePosition = (ADO::PositionEnum)nCurRecord;
		//pThis->m_spRecordset->AbsolutePosition = (PositionEnum)nCurRecord;

		if( pnRealRecord )
			*pnRealRecord = (long)pThis->m_spRecordset->AbsolutePosition;
		

		_END_CATCH(S_FALSE)
		
		//pThis->InitCacheBuffers();

		return S_OK;

	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::LeavePrivateMode( BOOL bResotoreBookmark, BOOL bRereadVisibleObject )
{
	_try_nested(CQueryObject, SelectQuery, LeavePrivateMode)
	{	
		pThis->m_bPrivateMode = false;

		pThis->m_spRecordset->Bookmark = pThis->varPrivateBookmark;

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetCurrentRecord( long* pnCurrentRecord )
{
	_try_nested(CQueryObject, SelectQuery, GetCurrentRecord)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;

		*pnCurrentRecord = ADO::adPosUnknown;
		
		ADO::PositionEnum _result = ADO::adPosUnknown;
		if( S_OK == pThis->m_spRecordset->get_AbsolutePosition( &_result ) )
			*pnCurrentRecord = (long)_result;		

		return S_OK;

	}
	_catch_nested;				
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GoToRecord( long nRecordNum )
{
	_try_nested(CQueryObject, SelectQuery, GoToRecord)
	{	
		if( !pThis->IsOpen() )
			return S_FALSE;

		if( nRecordNum <= 0 || nRecordNum > pThis->m_spRecordset->RecordCount ) 
			return S_FALSE;

		//pThis->SaveContextState();

		UpdateInteractive( TRUE );
		
		pThis->ClearUndoRedoList();

		//DB_SELECT_QUERY_DOCUMENT_IMPL /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		spDBDoc->FireEvent( _bstr_t(szDBaseEventBeforNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		_START_CATCH
			pThis->m_spRecordset->AbsolutePosition = (ADO::PositionEnum)nRecordNum;		
		_END_CATCH(S_FALSE)

		//т.к. поменялась позиция последней записи
		pThis->_save_primary_key( true );


		pThis->m_state = qsBrowse;

		pThis->OnRecordChange();

		//pThis->RestoreContextState();
		spDBDoc->FireEvent( _bstr_t(szDBaseEventAfterNavigation), ptrThis, spDBDoc, NULL, NULL, g_varEmpty );

		

		UpdateRecordPaneInfo( pThis->GetControllingUnknown() );

		return S_OK;

	}
	_catch_nested;
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//
//Tumbnail support
//
//
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnail( void** ppTumbnail )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnail)
	{	
		*ppTumbnail = NULL;
		
		if( !pThis->m_bPrivateMode )
		{
			ASSERT(false);
			return S_FALSE;
		}

		//DB_SELECT_QUERY_DOCUMENT_IMPL
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;		


		if( !pThis->IsOpen() )
			return S_FALSE;

		short nFieldType;
		CString strTableFieldSection;
		strTableFieldSection.Format( "%s\\%s.%s", (LPCTSTR)SECTION_DBASEFIELDS, 
									(LPCTSTR)pThis->m_strTumbnailTable, 
									(LPCTSTR)pThis->m_strTumbnailField );		
		
		int nIndex = pThis->GetFieldIndexFromSection( 
				_bstr_t( (LPCTSTR)strTableFieldSection ), spDBDoc, nFieldType );		

		if( nIndex == -1 || (FieldType)nFieldType != ftVTObject )
			return S_FALSE;

		
		if( nIndex < 0 || nIndex >= pThis->m_arCacheFields.GetSize() )
			return S_FALSE;

		CQueryField* pQueryField = pThis->m_arCacheFields[nIndex];


		long lCurRecord = -1;
		GetCurrentRecord( &lCurRecord );
			
		if( lCurRecord == pThis->m_lBookMark ) 			
			pQueryField = pThis->m_query.m_arrFields[nIndex];
		

		*ppTumbnail = pQueryField->GetTumbnail();

		return S_OK;

	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailCaption( BSTR* pbstrCaption )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailCaption)
	{	

		CString strCaption;

		//DB_SELECT_QUERY_DOCUMENT_IMPL; /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;


		_variant_t var;

		sptrINamedData spND(spDBDoc);
		if( spND )
		{
			CString strKey;
			strKey.Format( "%s\\%s.%s", 
						(LPCTSTR)SECTION_DBASEFIELDS, 
						(LPCTSTR)pThis->m_strTumbnailCaptionTable, 
						(LPCTSTR)pThis->m_strTumbnailCaptionField );				
			
			spND->GetValue( _bstr_t( (LPCTSTR)strKey ), &var );
			if( var.vt == VT_BSTR )
			{
				strCaption = var.bstrVal;
			}
		}

		*pbstrCaption = strCaption.AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailLocation( BSTR* pbstrTable, BSTR* pbstrField )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailLocation)
	{	
		*pbstrTable = pThis->m_strTumbnailTable.AllocSysString();
		*pbstrField = pThis->m_strTumbnailField.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetTumbnailLocation( BSTR bstrTable, BSTR bstrField )
{
	_try_nested(CQueryObject, SelectQuery, SetTumbnailLocation)
	{	
		pThis->m_strTumbnailTable = ::_MakeLower( bstrTable );
		pThis->m_strTumbnailField = ::_MakeLower( bstrField );

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		variant_t var;
		spDBDoc->FireEvent( _bstr_t(szDBaseEventGalleryOptionsChange), ptrThis, spDBDoc, NULL, NULL, var );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailCaptionLocation( BSTR* pbstrTable, BSTR* pbstrField )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailCaptionLocation)
	{	
		*pbstrTable = pThis->m_strTumbnailCaptionTable.AllocSysString();
		*pbstrField = pThis->m_strTumbnailCaptionField.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetTumbnailCaptionLocation( BSTR bstrTable, BSTR bstrField )
{
	_try_nested(CQueryObject, SelectQuery, SetTumbnailCaptionLocation)
	{	
		pThis->m_strTumbnailCaptionTable = ::_MakeLower( bstrTable );
		pThis->m_strTumbnailCaptionField = ::_MakeLower( bstrField );

		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		variant_t var;
		spDBDoc->FireEvent( _bstr_t(szDBaseEventGalleryOptionsChange), ptrThis, spDBDoc, NULL, NULL, var );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailSize( SIZE* pSize )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailSize)
	{	
		*pSize = pThis->m_sizeTumbnail;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetTumbnailSize( SIZE size )
{
	_try_nested(CQueryObject, SelectQuery, SetTumbnailSize)
	{	
		pThis->m_sizeTumbnail = size;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailViewSize( SIZE* pSize )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailViewSize)
	{	
		*pSize = pThis->m_sizeTumbnailView;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetTumbnailViewSize( SIZE size )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailViewSize)
	{	
		pThis->m_sizeTumbnailView = size;

		//DB_SELECT_QUERY_DOCUMENT_IMPL /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();		


		variant_t var;
		spDBDoc->FireEvent( _bstr_t(szDBaseEventGalleryOptionsChange), ptrThis, spDBDoc, NULL, NULL, var );
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetCanProcessFind( BOOL bCanProcessFind )
{
	_try_nested(CQueryObject, SelectQuery, SetCanProcessFind)
	{	
		pThis->m_bCanProcessFind = ( bCanProcessFind == TRUE );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetCanProcessFind( BOOL* pbCanProcessFind )
{
	_try_nested(CQueryObject, SelectQuery, GetCanProcessFind)
	{	
		*pbCanProcessFind = ( pThis->m_bCanProcessFind == TRUE );		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetFindSettings( BOOL bFindInField, BSTR bstrTable, BSTR bstrField,
								BOOL bMatchCase, BOOL bRegularExpression, BSTR bstrTextToFind  )
{
	_try_nested(CQueryObject, SelectQuery, SetFindSettings)
	{	
		pThis->m_bFindInField			= (bFindInField == TRUE);
		pThis->m_strFindTable			= bstrTable;
		pThis->m_strFindField			= bstrField;
		pThis->m_bFindMatchCase			= (bMatchCase == TRUE);
		pThis->bFindRegularExpression	= (bRegularExpression == TRUE);
		pThis->m_strTextToFind			= bstrTextToFind;


		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetFindSettings( BOOL* pbFindInField, BSTR* pbstrTable, BSTR* pbstrField,
								BOOL* pbMatchCase, BOOL* pbRegularExpression, BSTR* pbstrTextToFind )
{
	_try_nested(CQueryObject, SelectQuery, GetFindSettings)
	{	
		*pbFindInField			= (pThis->m_bFindInField == true);
		*pbstrTable				= pThis->m_strFindTable.AllocSysString();
		*pbstrField				= pThis->m_strFindField.AllocSysString();
		*pbMatchCase			= (pThis->m_bFindMatchCase == true);
		*pbRegularExpression	= (pThis->bFindRegularExpression == true);
		*pbstrTextToFind		= pThis->m_strTextToFind.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetTumbnailBorderSize( SIZE* pSize )
{
	_try_nested(CQueryObject, SelectQuery, GetTumbnailBorderSize)
	{	
		*pSize = pThis->m_sizeTumbnailBorder;		
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SetTumbnailBorderSize( SIZE size )
{
	_try_nested(CQueryObject, SelectQuery, SetTumbnailBorderSize)
	{	
		pThis->m_sizeTumbnailBorder = size;		

		//DB_SELECT_QUERY_DOCUMENT_IMPL /*spNDO2 - this, spDBDoc - document*/
		if( pThis->m_sptrParent == NULL )
			return S_FALSE;

		sptrIDBaseDocument spDBDoc( pThis->m_sptrParent );
		if( spDBDoc == NULL )
			return S_FALSE;

		IUnknownPtr ptrThis = pThis->GetControllingUnknown();

		
		variant_t var;
		spDBDoc->FireEvent( _bstr_t(szDBaseEventGalleryOptionsChange), ptrThis, spDBDoc, NULL, NULL, var );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XDBChild::SetParent( IUnknown* punkParent )
{
	_try_nested(CQueryObject, DBChild, SetParent)
	{	
		if( pThis->m_sptrParent == NULL )
			pThis->m_sptrParent = punkParent;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XDBChild::GetParent( IUnknown** ppunkParent )
{
	_try_nested(CQueryObject, DBChild, GetParent)
	{	
		if( pThis->m_sptrParent != NULL )
		{
			pThis->m_sptrParent->AddRef();
			*ppunkParent = pThis->m_sptrParent;
		}
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
INamedDataPtr CQueryObject::GetPrivateNamedData()
{
	if( m_sptrParent == NULL )
		return NULL;

	IUnknown* punkPrivateND = NULL;
	m_sptrParent->GetPrivateNamedData( &punkPrivateND );
	
	if( punkPrivateND == NULL )
		return NULL;

	INamedDataPtr ptrND( punkPrivateND );
	punkPrivateND->Release();

	return ptrND;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetSupports( short nCursorOptionEnum, BOOL* pbSucceeded )
{
	_try_nested(CQueryObject, SelectQuery, GetSupports)
	{	
		*pbSucceeded = FALSE;

		if( pThis->m_spRecordset == NULL )
			return S_FALSE;

		if( !pThis->IsOpen() )
			return S_FALSE;


		ADO::CursorOptionEnum Supports = (ADO::CursorOptionEnum)nCursorOptionEnum;

		VARIANT_BOOL varbSupport = VARIANT_FALSE;

		_START_CATCH
		varbSupport = pThis->m_spRecordset->Supports( Supports );
		_END_CATCH(S_FALSE)

		*pbSucceeded = ( varbSupport == VARIANT_TRUE );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetState( short* pnQueryState )
{
	_try_nested(CQueryObject, SelectQuery, GetState)
	{	
		*pnQueryState = pThis->GetState();		
		return S_OK;
	}
	_catch_nested;
}





////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::IsBOF( BOOL* pBResult )
{
	_try_nested(CQueryObject, SelectQuery, IsBOF)
	{	
		*pBResult = TRUE;

		
		if( !pThis->IsOpen() )
			return S_OK;


		long nRecordCount	= pThis->m_spRecordset->RecordCount;

		if( nRecordCount == 0 )
		{
			*pBResult = TRUE;
			return S_OK;
		}
		
		long nCurRecord		= (long)pThis->m_spRecordset->AbsolutePosition;
		

		if( nCurRecord == (long) ADO::adPosUnknown )
		{
			*pBResult = TRUE;
			return S_OK;
		}

		if( nCurRecord <= 1 )
			*pBResult = TRUE;
		else
			*pBResult = FALSE;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::IsEOF( BOOL* pBResult )
{
	_try_nested(CQueryObject, SelectQuery, IsEOF)
	{	
		*pBResult = TRUE;
		
		if( !pThis->IsOpen() )
			return S_OK;

		long nRecordCount	= pThis->m_spRecordset->RecordCount;

		if( nRecordCount == 0 )
		{
			*pBResult = TRUE;
			return S_OK;
		}

		long nCurRecord		= (long)pThis->m_spRecordset->AbsolutePosition;

		if( nCurRecord == (long) ADO::adPosUnknown )
		{
			*pBResult = TRUE;
			return S_OK;
		}

		if( nCurRecord >= nRecordCount )
			*pBResult = TRUE;
		else
			*pBResult = FALSE;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty )
{
	_try_nested(CQueryObject, SelectQuery, IsEOF)
	{	
		if( pbDBaseObject )
			*pbDBaseObject = FALSE;
		
		if( pGuidDBaseParent )
			*pGuidDBaseParent = INVALID_KEY;
		
		if( !pThis->IsOpen() )
			return S_OK;

		bool bFound = false;
		bool bForceExit = false;
		bool bEmpty = false;

		CString str = ::GetObjectName( punkObj );		

		for( int iField=0;iField<pThis->m_query.m_arrFields.GetSize();iField++ )
		{
			CQueryField* pField = pThis->m_query.m_arrFields[iField];
			bFound = pField->IsDBaseObject( punkObj, pGuidDBaseParent, bForceExit, bEmpty );			

			if( bFound )
			{
				if( pbDBaseObject )
					*pbDBaseObject = TRUE;

				if( pbEmpty )
					*pbEmpty = bEmpty;			

				return S_OK;
			}

			if( bForceExit )
				return S_OK;

		}

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetSortFieldsCount( int* pnCount )
{
	_try_nested(CQueryObject, SelectQuery, GetSortFieldsCount)
	{	
		if( !pnCount )
			return E_INVALIDARG;

		*pnCount = pThis->m_query.m_arrSortFields.GetSize();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::AddSortField( BSTR bstrTable, BSTR bstrField, BOOL bAscending )
{
	_try_nested(CQueryObject, SelectQuery, AddSortField)
	{	
		//at first remove old field if exist
		for( int i=0;i<pThis->m_query.m_arrSortFields.GetSize(); i++ )
		{
			CSortField* psf = pThis->m_query.m_arrSortFields[i];
			if( !_tcsicmp( psf->m_strTable, CString(bstrTable) ) && 
				!_tcsicmp( psf->m_strField, CString(bstrField) ) )
			{
				delete psf;
				pThis->m_query.m_arrSortFields.RemoveAt( i );
				break;
			}
		}
		CSortField* psf = new CSortField;
		psf->m_strTable		= bstrTable;
		psf->m_strField		= bstrField;
		psf->m_bAcsending	= ( bAscending == TRUE );

		pThis->m_query.m_arrSortFields.Add( psf );

		//надо сохранить порядок сортировки
		SetModifiedFlagToObj( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::GetSortField( int nIndex, BSTR* pbstrTable, BSTR* pbstrField, BOOL* pbAscending )
{
	_try_nested(CQueryObject, SelectQuery, GetSortField)
	{	
		if( nIndex < 0 || nIndex >= pThis->m_query.m_arrSortFields.GetSize() )
			return E_INVALIDARG;
		
		CSortField* psf = pThis->m_query.m_arrSortFields[nIndex];

		if( pbstrTable )
			*pbstrTable = psf->m_strTable.AllocSysString();

		if( pbstrField )
			*pbstrField = psf->m_strField.AllocSysString();

		if( pbAscending )
			*pbAscending = ( psf->m_bAcsending == true );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::RemoveSortFields( )
{
	_try_nested(CQueryObject, SelectQuery, RemoveSortFields)
	{	
		pThis->m_query.DeleteSortFields();
		
		//надо сохранить порядок сортировки
		SetModifiedFlagToObj( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::SaveContextsState()
{
	_try_nested(CQueryObject, SelectQuery, SaveContextsState)
	{	
		pThis->SaveContextState();
		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XSelectQuery::RestoreContextsState()
{
	_try_nested(CQueryObject, SelectQuery, RestoreContextsState)
	{	
		pThis->RestoreContextState();
		
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XBlankForm::GetControlsCount( BSTR bstr_table, BSTR bstr_field, long* plcount )
{
	_try_nested(CQueryObject, BlankForm, GetControlsCount )
	{	
		if( !plcount )
			return E_POINTER;

		CArray<CCtrlFormData*, CCtrlFormData*> arrCtrls;

		if( !pThis->GetFormCtrls( arrCtrls ) )
			return S_FALSE;

		for( int i=0;i<arrCtrls.GetSize();i++ )
		{
			CCtrlFormData* pdata = arrCtrls[i];

			CString str_field = pdata->strField;
			CString str_table = pdata->strTable;

			str_field.MakeLower();
			str_table.MakeLower();
			
			CString str_field1 = bstr_field;
			CString str_table1 = bstr_table;

			str_field1.MakeLower();
			str_table1.MakeLower();

				
			if( str_field == str_field1 &&  str_table == str_table1 )
				(*plcount)++;
			
		}

		for(i=0;i<arrCtrls.GetSize();i++ )
			delete arrCtrls[i];

		arrCtrls.RemoveAll();

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XNotifyObject::NotifyCreate()
{
	_try_nested(CQueryObject, NotifyObject, NotifyCreate )
	{	
		CWizardPool wp;		
		if( !wp.NewQueryWizard( pThis->GetControllingUnknown() ) )
			return S_FALSE;
		return S_OK;

	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XNotifyObject::NotifyDestroy()
{
	_try_nested(CQueryObject, NotifyObject, NotifyDestroy )
	{	
		pThis->DestroyCacheBuffers();
		INamedDataObject2Ptr prtNO2( pThis->GetControllingUnknown() );
		if( prtNO2 == 0 )
			return S_OK;

		IUnknown* punkData = 0;
		prtNO2->GetData( &punkData );
		if( punkData == 0 )
			return S_OK;

		IDBaseDocumentPtr ptrDB( punkData );
		punkData->Release();	punkData = 0;

		if( ptrDB == 0 )
			return S_OK;

		//test for query not last
		{
			IDataContext3Ptr	ptr_dc( ptrDB );
			if( ptr_dc )
			{
				long lcount = 0;
				ptr_dc->GetObjectCount( _bstr_t(szTypeQueryObject), &lcount );
				if( lcount == 1 )
					return S_FALSE;
			}
		}
		IUnknown* punkQ = 0;
		ptrDB->GetActiveQuery( &punkQ );
		if( punkQ )
		{
			ISelectQueryPtr ptrQ( punkQ );
			punkQ->Release();	punkQ = 0;
			if( ptrQ )
			{
				GuidKey guidActiveQuery = ::GetObjectKey( ptrQ );
				if( guidActiveQuery == ::GetObjectKey( pThis->GetControllingUnknown() ) )
				{					
					ptrDB->SetActiveQuery( 0 );

					BOOL bOpen = FALSE;
					ptrQ->IsOpen( &bOpen );
					if( bOpen )
						ptrQ->Close();

					//get other query from context
					IDataContext2Ptr ptrDC( ptrDB );
					if( ptrDC )
					{
						LONG_PTR lPos = 0;
						ptrDC->GetFirstObjectPos( _bstr_t(szTypeQueryObject), &lPos );
						while( lPos )
						{
							IUnknown* punk = 0;
							ptrDC->GetNextObject( _bstr_t(szTypeQueryObject), &lPos, &punk );

							if( !punk )
								continue;

							ISelectQueryPtr ptrNewQ( punk );
							punk->Release();	punk = 0;

							if( ::GetObjectKey( ptrNewQ ) ==  guidActiveQuery )
								continue;


							ptrDB->SetActiveQuery( ptrNewQ );
							ptrNewQ->Open();

							//and activate in active view
							{
								IDocumentSitePtr ptrDS( ptrDB );
								if( ptrDS )
								{
									IUnknown* punkView = 0;
									ptrDS->GetActiveView( &punkView );
									if( punkView )
									{
										IDataContext2Ptr ptrDC( punkView );
										if( ptrDC )
										{
											ptrDC->SetObjectSelect( ptrNewQ, 1 );
										}
										punkView->Release();	punkView = 0;
									}

								}
							}

							return S_OK;
						}
					}
				}
			}
		}


		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XNotifyObject::NotifyActivate( bool bActivate )
{
	_try_nested(CQueryObject, NotifyObject, NotifyActivate )
	{	

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XNotifyObject::NotifySelect( bool bSelect )
{
	_try_nested(CQueryObject, NotifyObject, NotifySelect )
	{	
		if( !bSelect )
		{
			pThis->DestroyCacheBuffers();
			return S_OK;
		}

		INamedDataObject2Ptr prtNO2( pThis->GetControllingUnknown() );
		if( prtNO2 == 0 )
			return S_OK;

		IUnknown* punkData = 0;
		prtNO2->GetData( &punkData );
		if( punkData == 0 )
			return S_OK;

		IDBaseDocumentPtr ptrDB( punkData );
		punkData->Release();	punkData = 0;

		if( ptrDB == 0 )
			return S_OK;

		IUnknown* punkPrevQ = 0;
		ptrDB->GetActiveQuery( &punkPrevQ );
		if( punkPrevQ )
		{
			ISelectQueryPtr ptrPrevQ( punkPrevQ );
			punkPrevQ->Release();	punkPrevQ = 0;
			if( ptrPrevQ )
			{
				if( ::GetObjectKey( ptrPrevQ ) == ::GetObjectKey( pThis->GetControllingUnknown() ) )
					return S_OK;

				BOOL bOpen = FALSE;
				ptrPrevQ->IsOpen( &bOpen );
				if( bOpen )
				{
					ptrPrevQ->UpdateInteractive( TRUE );
					ptrPrevQ->Close();
				}
			}
		}
		

		ISelectQueryPtr ptrQ( pThis->GetControllingUnknown() );
		
		if( ptrQ )
		{
			ptrDB->SetActiveQuery( ptrQ );
			ptrQ->Open();
		}


		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	interface QueryMisc
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQueryMisc::GetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int* pnWidth, int* pnPos )
{
	_try_nested(CQueryObject, QueryMisc, GetGridFieldInfo )
	{	
		CGridField* pgf = 0;
		for( int i=0;i<pThis->m_query.m_arrGridFields.GetSize();i++ )
		{
			CGridField* p = pThis->m_query.m_arrGridFields[i];
			if( p->m_strTable == bstrTable && p->m_strField == bstrField )
			{
				pgf = p;
				break;
			}
		}

		if( pgf == 0 )
			return S_FALSE;

		if( pnWidth )
			*pnWidth = pgf->m_nWidth;

		if( pnPos )
			*pnPos = pgf->m_nPos;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQueryMisc::SetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int nWidth, int nPos )
{
	_try_nested(CQueryObject, QueryMisc, SetGridFieldInfo )
	{	
		CGridField* pgf = 0;
		for( int i=0;i<pThis->m_query.m_arrGridFields.GetSize();i++ )
		{
			CGridField* p = pThis->m_query.m_arrGridFields[i];
			if( p->m_strTable == bstrTable && p->m_strField == bstrField )
			{
				pgf = p;
				break;
			}
		}

		if( pgf == 0 )
		{
			pgf = new CGridField;
			pgf->m_strTable = bstrTable;
			pgf->m_strField = bstrField;
			pThis->m_query.m_arrGridFields.Add( pgf );
		}

		pgf->m_nWidth = nWidth;
		pgf->m_nPos = nPos;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CQueryObject::XQueryMisc::EmptyGridFieldInfo()
{
	_try_nested(CQueryObject, QueryMisc, SetGridFieldInfo )
	{	
		pThis->DeleteNotExistGridFields();
		return S_OK;

	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::DeleteNotExistGridFields()
{
	for( int i=m_query.m_arrGridFields.GetSize()-1;i>=0;i-- )
	{
		CGridField* pgf = m_query.m_arrGridFields[i];
		bool bFound = false;

		for( int j=0;j<m_query.m_arrFields.GetSize();j++ )
		{
			CQueryField* pqf = m_query.m_arrFields[j];
			if( pgf->m_strTable == pqf->m_strTable && pgf->m_strField == pqf->m_strField )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
		{
			delete pgf;
			m_query.m_arrGridFields.RemoveAt( i );
		}
	}
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::ResolveSortProblem( CString strSQL, ADO::CursorLocationEnum curLocOld, 
									bool& bNeedRequery, ADO::CursorLocationEnum& curLocNew )
{
	try
	{			
		CString _strSQL = strSQL;
		_strSQL.MakeLower();

		bNeedRequery = false;

		if( _strSQL.Find( " order " ) == -1 )
			return true;

		if( curLocOld == ADO::adUseClient )
			return true;

		long lFirstBlobIndex = -1;
		for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
		{
			CQueryField* pField = m_query.m_arrFields[i];			
			if( pField->GetFieldType() == ftVTObject )
			{
				lFirstBlobIndex = i;
				break;
			}
		}

		if( lFirstBlobIndex == -1 )
			return true;


		long lRecCount = m_spRecordset->RecordCount;

		if( lRecCount < 1 )
			return false;

		if( lRecCount == 1 )//Ups!!! can't seek
		{
			curLocNew = ADO::adUseClient;
			bNeedRequery = true;
			return true;
		}

		long lCurRecord = (long)m_spRecordset->AbsolutePosition;
		if( lCurRecord < 1 || lCurRecord >= lRecCount )
			return false;

		//now read  blob seek to another record, read blob and return to record;
		ADO::FieldsPtr ptrFields = m_spRecordset->Fields;
		if( ptrFields == 0 )
			return false;

		ADO::FieldPtr ptrField = ptrFields->GetItem( lFirstBlobIndex );
		if( !( ptrField->Attributes & ADO::adFldLong ) )
			return false;

		_variant_t _var1;
		ptrField->raw_GetChunk( 1, &_var1 );

		if( lCurRecord >= 1 )
			m_spRecordset->AbsolutePosition = (ADO::PositionEnum)( lCurRecord + 1 );
		else
			m_spRecordset->AbsolutePosition = (ADO::PositionEnum)( lCurRecord - 1 );
		
		_variant_t _var2;
		ptrField->raw_GetChunk( 1, &_var2 );


		m_spRecordset->AbsolutePosition = (ADO::PositionEnum)lCurRecord;

		return true;

	}
	catch(...)
	{
		return false;
	}

	return false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	class CBlankCtrl
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBlankCtrl, CAXCtrlBase);


// {BF960F7D-A5C3-49a8-A830-3E948DE2D06F}
GUARD_IMPLEMENT_OLECREATE(CBlankCtrl, "Blank.BlankCtrl", 
0xbf960f7d, 0xa5c3, 0x49a8, 0xa8, 0x30, 0x3e, 0x94, 0x8d, 0xe2, 0xd0, 0x6f);
// {BABCE2E0-31A8-481a-9A1B-B982A85AB364}
static const GUID clsidBlankCtrlDataObjectInfo = 
{ 0xbabce2e0, 0x31a8, 0x481a, { 0x9a, 0x1b, 0xb9, 0x82, 0xa8, 0x5a, 0xb3, 0x64 } };

DATA_OBJECT_INFO_FULL(IDS_BLABK_TYPE, CBlankCtrl, szTypeBlankCtrl, szTypeBlankForm, clsidBlankCtrlDataObjectInfo, IDI_QUERY_ICON)


BEGIN_INTERFACE_MAP(CBlankCtrl, CAXCtrlBase)
	INTERFACE_PART(CBlankCtrl, IID_INotifyObject, NotifyObject)
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CBlankCtrl, NotifyObject)

////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankCtrl::CBlankCtrl()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankCtrl::~CBlankCtrl()
{

}

bool CBlankCtrl::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	CAXCtrlBase::SerializeObject( ar, pparams );

	long	lVersion = 4;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
		ar<<m_ViewSubType;
	}
	else
	{
		ar>>lVersion;
		if( lVersion >= 4 )
		{
			ar>>m_ViewSubType;
			lVersion=lVersion;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CBlankCtrl::XNotifyObject::NotifyCreate()
{
	_try_nested(CBlankCtrl, NotifyObject, NotifyCreate )
	{	
		FieldType ft = ftNotSupported;
		if( !pThis->IsDBaseControl( &ft ) )
			return S_OK;

		CArray<user_info,user_info&> arr_users;
		GetUsersInfo( &arr_users );
		if( arr_users.GetSize() > 1 )
		{
			CDDLErrorDlg dlg;
			dlg.DoModal();
			return S_FALSE;
		}


		if( ft == ftNotSupported )
			return S_OK;


		IQueryObjectPtr ptrQ	= pThis->GetQuery();
		IDBaseStructPtr ptrDBS	= pThis->GetDBStruct();

		if( ptrQ == 0 || ptrDBS == 0 ) 
			return E_FAIL;

		
		IUnknownPtr ptrAXCtrl = pThis->GetAXCtrl();
		if( ptrAXCtrl == 0 )
			return E_FAIL;

		bool bViewSubType=false;
		IDBControlPtr pCtrlDB( ptrAXCtrl );
		if( pCtrlDB )
		{
			if(IDBObjectControlPtr pDBObject=pCtrlDB)
			{
				if(pThis->m_ViewSubType>0){
					pCtrlDB->SetValue(ATL::CComVariant(pThis->m_ViewSubType));
					bViewSubType=true;
				}
			}
		}

		BOOL bSimple = FALSE;
		ptrQ->IsSimpleQuery( &bSimple );

		if( !bSimple || !::IsAccessDBase( ptrDBS ) )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_WARNING_CANT_VISUAL_EDIT, MB_ICONSTOP | MB_OK );
			return E_FAIL;
		}

		CString strTable;
		{
			BSTR bstr = 0;
			ptrQ->GetBaseTable( &bstr );
			strTable = bstr;

			if( bstr )
				::SysFreeString( bstr );
		}
		_bstr_t bstrTable( (LPCSTR)strTable );

		CAddField dlg(bViewSubType?ptrDBS:0, bstrTable);

		dlg.m_strFieldName = ::GenerateUniqFieldName( ptrDBS, strTable, ft );
		
		dlg.SetFieldType( ft );
		if( dlg.DoModal() != IDOK )
			return E_FAIL;

		_bstr_t bstrField( (LPCSTR)dlg.m_strFieldName );


		ft = ftNotSupported;
		CString strObjectType;
		if( !CDBStructGrid::ConvertFromStringToFieldType( dlg.m_strFieldType, ft, strObjectType ) )
		{
			return E_FAIL;
		}

		ISelectQueryPtr ptrSQ( ptrQ );
		BOOL bWasOpen = FALSE;

		ptrSQ->IsOpen( &bWasOpen );
		if( bWasOpen )
		{
			ptrSQ->UpdateInteractive( TRUE );
			ptrSQ->Close();
		}

		if(IDBControlPtr ptrCtrlDB=ptrAXCtrl)
		{
			if(IDBObjectControlPtr pDBObject=ptrCtrlDB)
			{
				ptrCtrlDB->SetValue(ATL::CComVariant(pThis->m_ViewSubType));
			}
		}

		if( S_OK != ptrDBS->GetFieldInfo(bstrTable, bstrField, 
				0, 0,
				0, 0, 0, 0, 0, 0 ))
		{
			if( S_OK != ptrDBS->AddField( bstrTable, bstrField, (short)ft, 0, 0 ) )
			{
				if( bWasOpen )
					ptrSQ->Open();
				return E_FAIL;
			}
			ptrDBS->SetFieldInfo( bstrTable, bstrField, bstrField,
				dlg.m_bReqValue, dlg.m_bDefValue, _bstr_t(dlg.m_strDefValue), _bstr_t( (LPCSTR)strObjectType ) );
		}
		
		IDBControlPtr ptrCtrlDB( ptrAXCtrl );
		if( ptrCtrlDB )
		{
			ptrCtrlDB->SetTableName( bstrTable );
			ptrCtrlDB->SetFieldName( bstrField );

			::StoreContainerToDataObject( pThis->GetControllingUnknown(), ptrCtrlDB );			

			IDataObjectListPtr	ptrList( ptrQ );
			if( ptrList )
				ptrList->UpdateObject( pThis->GetControllingUnknown() );
		}

		if( bWasOpen )
			ptrSQ->Open();

		{
			// Field must be active after creation. Find this field in database and use SetActiveField to activize.
			_bstr_t bstrTableName;
			ptrCtrlDB->GetTableName(bstrTableName.GetAddress());	
			_bstr_t bstrFieldName;
			ptrCtrlDB->GetFieldName(bstrFieldName.GetAddress());
			CString sTableName = (const char *)bstrTableName;
			sTableName.MakeLower();
			CString sFieldName = (const char *)bstrFieldName;
			sFieldName.MakeLower();
			int nCount;
			ptrQ->GetFieldsCount(&nCount);
			_bstr_t bsInnerName,bsQueryTable,bsQueryField;
			for(int nIndex=nCount; --nIndex>=0;)
			{
				ptrQ->GetField(nIndex, bsInnerName.GetAddress(), 
					bsQueryTable.GetAddress(), bsQueryField.GetAddress());
				if (sTableName == CString((const char *)bsQueryTable) &&
					sFieldName == CString((const char *)bsQueryField))
				{
					ptrQ->SetActiveField(nIndex);
					break;
				}
			}
		}

		SaveDBaseDocument( ptrDBS );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CBlankCtrl::XNotifyObject::NotifyDestroy()
{
	_try_nested(CBlankCtrl, NotifyObject, NotifyDestroy )
	{	
		if( !pThis->IsDBaseControl( ) )
			return S_OK;

		CArray<user_info,user_info&> arr_users;
		GetUsersInfo( &arr_users );
		if( arr_users.GetSize() > 1 )
		{
			CDDLErrorDlg dlg;
			dlg.DoModal();
			return S_FALSE;
		}

		IQueryObjectPtr ptrQ	= pThis->GetQuery();
		IDBaseStructPtr ptrDBS	= pThis->GetDBStruct();

		if( ptrQ == 0 || ptrDBS == 0 ) 
			return E_FAIL;

		
		IUnknownPtr ptrAXCtrl = pThis->GetAXCtrl();
		if( ptrAXCtrl == 0 )
			return E_FAIL;


		BOOL bSimple = FALSE;
		ptrQ->IsSimpleQuery( &bSimple );

		if( !bSimple || !::IsAccessDBase( ptrDBS ) )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_WARNING_CANT_VISUAL_EDIT, MB_ICONSTOP | MB_OK );
			return E_FAIL;
		}


		IDBControlPtr ptrCtrlDB( ptrAXCtrl );
		if( ptrCtrlDB == 0 )
			return E_FAIL;

		::RestoreContainerFromDataObject( pThis->GetControllingUnknown(), ptrCtrlDB );		


		_bstr_t bstrTableQ;
		{
			BSTR bstr = 0;
			ptrQ->GetBaseTable( &bstr );
			bstrTableQ = bstr;

			if( bstr )
				::SysFreeString( bstr );
		}

		_bstr_t bstrTable, bstrField;
		{
			BSTR bstr = 0;
			ptrCtrlDB->GetTableName( &bstr );
			bstrTable = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;

			ptrCtrlDB->GetFieldName( &bstr );
			bstrField = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;
		}

		if( bstrTable != bstrTableQ )
			return E_FAIL;

		{
			IBlankFormPtr ptr_form( ptrQ );
			if( ptr_form )
			{
				long lcount = 0;
				ptr_form->GetControlsCount( bstrTable, bstrField, &lcount );
				if( lcount > 1 )
					return S_OK;

			}
			
		}
		
		CDeleteField dlg;
		dlg.m_strTable = (LPCSTR)bstrTable;
		dlg.m_strField = (LPCSTR)bstrField;
		if( dlg.DoModal() != IDOK )
			return E_FAIL;



		ISelectQueryPtr ptrSQ( ptrQ );
		BOOL bWasOpen = FALSE;

		ptrSQ->IsOpen( &bWasOpen );
		if( bWasOpen )
		{
			ptrSQ->UpdateInteractive( TRUE );
			ptrSQ->Close();
		}


		if( S_OK != ptrDBS->DeleteField( bstrTable, bstrField ) )
		{
			if( bWasOpen )
				ptrSQ->Open();

			return E_FAIL;
		}

		if( bWasOpen )
			ptrSQ->Open();

		SaveDBaseDocument( ptrDBS );


		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CBlankCtrl::XNotifyObject::NotifyActivate( bool bActivate )
{
	_try_nested(CBlankCtrl, NotifyObject, NotifyActivate )
	{	

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CBlankCtrl::XNotifyObject::NotifySelect( bool bSelect )
{
	_try_nested(CBlankCtrl, NotifyObject, NotifySelect )
	{	

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ISelectQueryPtr CBlankCtrl::GetQuery()
{
	INamedDataObject2Ptr ptrChild( GetControllingUnknown() );
	if( ptrChild == 0 )
		return 0;

	IUnknown* punkParent = 0;
	ptrChild->GetParent( &punkParent );
	if( !punkParent )
		return 0;

	ISelectQueryPtr ptrQ( punkParent );
	punkParent->Release();	punkParent = 0;

	return ptrQ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IDBaseStructPtr CBlankCtrl::GetDBStruct()
{
	ISelectQueryPtr ptrQ( GetQuery() );
	if( ptrQ == 0 )
		return 0;

	INamedDataObject2Ptr prtNO2( ptrQ );
	if( prtNO2 == 0 )
		return 0;

	IUnknown* punkData = 0;
	prtNO2->GetData( &punkData );
	if( punkData == 0 )
		return 0;

	IDBaseStructPtr ptrDBS( punkData );
	punkData->Release();	punkData = 0;
		
	return ptrDBS;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CBlankCtrl::GetAXCtrl( )
{
	IActiveXCtrlPtr ptrCtrData( GetControllingUnknown() );
	if( ptrCtrData == 0 )
		return 0;

	BSTR bstr = 0;	
	ptrCtrData->GetProgID( &bstr );

	_bstr_t bstrProgID = bstr;

	if( bstr )
		::SysFreeString( bstr );	bstr = 0;


	CLSID	clsid;		
	if( S_OK != ::CLSIDFromProgID( bstrProgID, &clsid ) )
		return 0;
	
	IUnknown* punkAX = 0;
	if( S_OK != CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punkAX ) )
		return 0;

	IUnknownPtr ptrCtrl = punkAX;	
	punkAX->Release();

	return (IUnknown*)ptrCtrl;
}

/////////////////////////////////////////////////////////////////////////////
bool CBlankCtrl::IsDBaseControl( FieldType* pftype )
{
	IDBControlPtr prtDBCtrl( GetAXCtrl( ) );	

	if( prtDBCtrl == 0 )
		return false;

	if( pftype )
	{
		IActiveXCtrlPtr ptrCtrData( GetControllingUnknown() );
		if( ptrCtrData == 0 )
			return 0;

		BSTR bstr = 0;	
		ptrCtrData->GetProgID( &bstr );

		CString  strProgID = bstr;

		if( bstr )
			::SysFreeString( bstr );	bstr = 0;

		FieldType ft = ftNotSupported;

		if( strProgID == DBDateTimeProgID )
			ft = ftDateTime;
		else if( strProgID == DBNumberProgID )
			ft = ftDigit;	
		else if( strProgID == DBVTObjectProgID )
			ft = ftVTObject;
		else if( strProgID == DBTextProgID )
			ft = ftString;
		
		*pftype = ft;
	}

	return true;
}


//[AY]
//we save only active objects with field's code
void CQueryObject::SaveContextState()
{
	DestroyContextState();

	if( m_sptrParent == NULL )
		return;


	IDocumentSitePtr ptrDS( m_sptrParent );
	if( ptrDS == NULL )
		return;

	
	TPOS lPos = 0;
	ptrDS->GetFirstViewPosition( &lPos );
	while( lPos )
	{
		IUnknown* punkView = NULL;
		ptrDS->GetNextView( &punkView, &lPos );
		if( !punkView )
			continue;


		IDataContext2Ptr ptrC( punkView );
		punkView->Release();

		if( ptrC == NULL )
			continue;

		CViewContext* pi = new CViewContext;
		m_arViewContext.AddHead( pi );

		pi->m_guidKeyView = ::GetObjectKey( ptrC );



		long nObjTypeCount = 0;
		ptrC->GetObjectTypeCount( &nObjTypeCount );
		for( int i=0;i<nObjTypeCount;i++ )
		{
			BSTR bstrObjType = 0;
			ptrC->GetObjectTypeName( i, &bstrObjType );
			_bstr_t bstrType = bstrObjType;

			if( bstrObjType )
				::SysFreeString( bstrObjType );

			IUnknown* punkObj = NULL;
			ptrC->GetActiveObject( bstrType, &punkObj );
			if( punkObj )
			{
				CObjectInfo* poi = new CObjectInfo;
				pi->m_arObjectInfo.AddHead( poi );
				poi->m_strObjectName	= ::GetObjectName( punkObj );
				poi->strObjectType		= ::GetObjectKind( punkObj );				
				poi->m_bActive = true;
				punkObj->Release();
			}
		}		
	}

}

void CQueryObject::RestoreContextState()
{
	

	if( m_sptrParent == NULL )
		return;

	INamedDataPtr ptrND( m_sptrParent );

	IDocumentSitePtr ptrDS( m_sptrParent );
	if( ptrDS == NULL )
		return;

	TPOS lViewPos = 0;
	ptrDS->GetFirstViewPosition( &lViewPos );
	while( lViewPos )
	{
		IUnknown* punkView = NULL;
		ptrDS->GetNextView( &punkView, &lViewPos );
		if( !punkView )
			continue;


		IDataContext2Ptr ptrC( punkView );
		punkView->Release();

		if( ptrC == NULL )
			continue;


		CViewContext* pvcFind = NULL;
		POSITION pos = m_arViewContext.GetHeadPosition();
		
		while( pos )
		{			
			CViewContext* pvc = (CViewContext*)m_arViewContext.GetNext( pos );
			if( pvc->m_guidKeyView == ::GetObjectKey( ptrC ) )
			{
				pvcFind = pvc;
				break;
			}
		}

		if( pvcFind )
		{			

			{
				long lCount = 0;
				VERIFY( S_OK == ptrC->GetObjectTypeCount( &lCount ) );
				for( long l=0;l<lCount;l++ )
				{
					BSTR bstrTypeName = 0;
					VERIFY( S_OK == ptrC->GetObjectTypeName( l, &bstrTypeName ) );
					//VERIFY( S_OK == ptrC->UnselectAll( bstrTypeName ) );
					::SysFreeString( bstrTypeName );	bstrTypeName = 0;
				}					
			}

			POSITION posObj = pvcFind->m_arObjectInfo.GetHeadPosition();
			while( posObj )
			{
				CObjectInfo* poi = (CObjectInfo*)pvcFind->m_arObjectInfo.GetNext( posObj );				
				if( !poi->m_bActive )continue;
				//if( poi->m_bBaseObject )
				{
					IUnknown* punkObj = NULL;
					punkObj = ::GetObjectByName( ptrND, poi->m_strObjectName, poi->strObjectType );
					if( punkObj )
					{
						ptrC->SetObjectSelect( punkObj, TRUE );
						punkObj->Release();
					}

				}
			}
		}
	}
}

long CQueryObject::_get_primary_key_value()
{
	long lprimary_key = -1;
	try
	{
		if( !IsOpen() )	return -1;

		long lidx = _get_primary_key_idx();
		if( lidx == -1 )	return -1;

		ADO::FieldsPtr ptr_fields = m_spRecordset->Fields;
		if( ptr_fields == 0 )	return -1;

		ADO::FieldPtr ptr_field = ptr_fields->GetItem( _variant_t((long)lidx) );
		if( ptr_field == 0 )	return -1;

		_variant_t var;
		ptr_field->get_Value( &var );
		if( var.vt != VT_I4 )	return -1; 

		return var.lVal;
	}
	catch(...)
	{}

	return lprimary_key;
}
		
long CQueryObject::_find_record( long lprimary_key )
{
	if( lprimary_key == -1 )
		return -1;
	
	try
	{
		if( !IsOpen() )	return -1;

		long lidx = _get_primary_key_idx();
		if( lidx == -1 )	return -1;

		ADO::PositionEnum old_record = ADO::adPosUnknown;
		if( S_OK != m_spRecordset->get_AbsolutePosition( &old_record ) )
			return -1;

		if( old_record < 1 )return -1;

		long lcur_record = 0;

		while( !m_spRecordset->ADOEOF )
		{
			lcur_record++;

  			ADO::FieldsPtr ptr_fields = m_spRecordset->Fields;
			if( ptr_fields )
			{
				ADO::FieldPtr ptr_field = ptr_fields->GetItem( lidx );
				variant_t var;
				var = ptr_field->GetValue();
				if( var.vt == VT_I4 )
				{
					if( var.lVal == lprimary_key )
						return lcur_record;
				}
			}			
			m_spRecordset->MoveNext();
		}

		//restore record
		m_spRecordset->put_AbsolutePosition( (ADO::PositionEnum)old_record );		

		return -1;

	}
	catch(...)
	{}

	return -1;
}

long CQueryObject::_get_primary_key_idx()
{
	for( int i=0; i<m_query.m_arrFields.GetSize(); i++ )
	{
		CQueryField* pfield = m_query.m_arrFields[i];
		if( ftPrimaryKey == pfield->GetFieldType() )
			return i;
	}
	return -1;
}

void CQueryObject::_save_primary_key( bool bset_modify_flag )
{
	if( bset_modify_flag )
		SetModifiedFlagToObj( GetControllingUnknown() );
	
	//save in query
	m_llast_primary_key = _get_primary_key_value();

	//and in document too
	::_SetValue( m_sptrParent, SECTION_DBASE, "LastPrimaryKeyValue", (long)m_llast_primary_key );
}

long CQueryObject::_get_last_primary_key()
{
	m_llast_primary_key = ::_GetValueInt( m_sptrParent, SECTION_DBASE, "LastPrimaryKeyValue", -1 );
	return m_llast_primary_key;
}
