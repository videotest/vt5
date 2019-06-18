#include "stdafx.h"
#include "data.h"

#include "SendToManager.h"
#include "dbase.h"
#include "docmiscint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CSendToManager

IMPLEMENT_DYNCREATE(CSendToManager, CCmdTargetEx);


// {A97DC975-1042-4b79-B2C4-CCC7E65C5BA3}
GUARD_IMPLEMENT_OLECREATE(CSendToManager, szSendToManagerProgID,
0xa97dc975, 0x1042, 0x4b79, 0xb2, 0xc4, 0xcc, 0xc7, 0xe6, 0x5c, 0x5b, 0xa3);

/////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CSendToManager, CCmdTargetEx)
	INTERFACE_PART(CSendToManager, IID_ISendToManager, SendToManager)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CSendToManager, SendToManager);


/////////////////////////////////////////////////////////////////////////////
CSendToManager::CSendToManager()
{
	DeInit();

	_OleLockApp( this );
	EnableAggregation();
}

/////////////////////////////////////////////////////////////////////////////
CSendToManager::~CSendToManager()
{		
	DeInit();
	_OleUnlockApp( this );
}


/////////////////////////////////////////////////////////////////////////////
void CSendToManager::DeInit()
{
	//m_bContentSync = false;

	//Kill  Target Documents info
	POSITION pos = m_arGuidTargetDocs.GetHeadPosition();
	
	while( pos )
	{
		GuidKey* pkey = (GuidKey*)m_arGuidTargetDocs.GetNext( pos );
		delete pkey;
	}

	m_arGuidTargetDocs.RemoveAll();

	//Kill  Source Objects guid
	pos = m_arGuidSourceObject.GetHeadPosition();
	
	while( pos )
	{
		GuidKey* pkey = (GuidKey*)m_arGuidSourceObject.GetNext( pos );
		delete pkey;
	}

	m_arGuidSourceObject.RemoveAll();	

	m_guidSourceDoc = INVALID_KEY;


	//Kill  Source Objects guid
	pos = m_arItems.GetHeadPosition();
	
	while( pos )
	{
		CItemInfo* pi = (CItemInfo*)m_arItems.GetNext( pos );
		delete pi;
	}

	m_arItems.RemoveAll();	


	m_strSingleObjectName.Empty();	
	
}

/////////////////////////////////////////////////////////////////////////////
static bool IsCompatibleField(IUnknown *punkTargetDoc, LPCTSTR lpstrField, LPCTSTR lpstrObjectType)
{
	INamedDataPtr ptrNDTarget(punkTargetDoc);
	IDBaseDocumentPtr ptrDB( punkTargetDoc );
	if( ptrDB == NULL )
		return true; // Not DB document
	IUnknownPtr punkQuery;
	ptrDB->GetActiveQuery( &punkQuery );
	if( punkQuery == 0 )
		return false;
	ISelectQueryPtr ptrQ( punkQuery );
	if( ptrQ == NULL )
		return false;
	BOOL bOpen = FALSE;
	ptrQ->IsOpen( &bOpen );
	if( !bOpen )
		return false;
	_bstr_t bstrEntry( SECTION_DBASEFIELDS );
	bstrEntry += "\\";
	bstrEntry += lpstrField;
	_variant_t var;
	ptrNDTarget->GetValue( bstrEntry, &var );
	if( !( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH ) )
		return false;
	IUserNameProviderPtr ptrUNP( ptrDB );
	CString strFieldType = ::GetObjectKind( var.punkVal );
	return strFieldType == lpstrObjectType;
}

/////////////////////////////////////////////////////////////////////////////
void CSendToManager::Init()
{	

	//m_bContentSync = false;

	CString strCreateInDocument;
	strCreateInDocument.LoadString( IDS_CREATE_IN_DOCUMENT );

	CString strAllSelectedObjects;
	strAllSelectedObjects.LoadString( IDS_ALL_SELECTED_OBJECTS );

	CString strSendAllObjectsTo;
	CString strSendObjectToFormat;

	strSendAllObjectsTo.LoadString( IDS_SEND_SELECTED_OBJECTS );
	strSendObjectToFormat.LoadString( IDS_SEND_OBJECT_TO );		
	
	

	UINT uiCommand = m_uiStartItemID;	
	//Need create m_arTargetDocs

	IDocumentPtr ptrSourceDoc = GetDocumentByKey( m_guidSourceDoc );
	if( ptrSourceDoc == NULL )
		return;

	if( m_arGuidSourceObject.GetCount() < 1 )
		return;


	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return;
	

	long	lPosTemplate = 0;	
	sptrA->GetFirstDocTemplPosition( &lPosTemplate );	
	while( lPosTemplate )
	{
		long	lPosDoc = 0;
		sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );
		while( lPosDoc )
		{
			IUnknown *punkDoc = 0;
			sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

			if( !punkDoc )
				continue;


			IDataContext2Ptr ptrDC2( punkDoc );						
			punkDoc->Release();
			if( ptrDC2 == NULL )
				continue;

			GuidKey guidDocKey = ::GetObjectKey( ptrDC2 );

			if( (guidDocKey == m_guidSourceDoc) && !CheckInterface( ptrDC2, IID_IDBaseDocument) )
				continue;

			GuidKey* pkey = new GuidKey;
			*pkey = guidDocKey;
			m_arGuidTargetDocs.AddTail( pkey );

			CString strDocTitle;
			{
				IDocumentSitePtr ptr( ptrDC2 );
				if( ptr )
				{
					BSTR bstr = 0;
					ptr->GetPathName( &bstr );
					strDocTitle = bstr;
					::SysFreeString( bstr );
				}				
			}
			
			strDocTitle = GetObjectNameFromPath( strDocTitle );

			if( CheckInterface( ptrDC2, IID_IDBaseDocument) )
			{
				CStringArray arTable;
				CStringArray arField;
				CStringArray arCaption;

				GetFieldsFromDBaseDoc( ptrDC2, arTable, arField, arCaption );



				CItemInfo* piiRoot = AddItem( strDocTitle, "", "", MF_POPUP, iiDocTitle,
						 ++uiCommand, -1, 
						 guidDocKey, INVALID_KEY );

				if (GetValueInt(GetAppUnknown(), "\\SendToManager", "ShowPlaceToDocumentForDbd", 0))
				{
					//Place to document
					AddItem( strCreateInDocument, "", "", MF_STRING | MF_ENABLED, iiDBPutToDocument,
							++uiCommand, piiRoot->m_uiItemID, 
							guidDocKey, INVALID_KEY );
				}

				for( int i=0;i<arField.GetSize();i++ )
				{
//					bool bEnabled = IsCompatibleField(ptrDC2, arField[i], );
					CItemInfo* piField = AddItem(	arCaption[i], arTable[i], arField[i],
													MF_STRING | MF_ENABLED, iiDBFieldTitle,
													++uiCommand, piiRoot->m_uiItemID, 
													guidDocKey, INVALID_KEY);						
				}
					

			}
			else//Not dbase doc
			{
				AddItem( strDocTitle, "", "", MF_STRING | MF_ENABLED, iiDocTitle,
							 ++uiCommand, -1, 
							 guidDocKey, INVALID_KEY );
			}


		}
		sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
	} 


	if (::GetValueInt( GetAppUnknown(), "\\SendToManager", "EnableNewDocumentCreation", TRUE) )
	{
		CString strNewDocTitle;
		strNewDocTitle.LoadString( IDS_CREATE_IN_NEW_DOC );
		AddItem( strNewDocTitle, "", "", MF_STRING | MF_ENABLED, iiNewDocument,
				++uiCommand, -1, 
				INVALID_KEY, INVALID_KEY );
	}
	//m_bContentSync = true;

}

/////////////////////////////////////////////////////////////////////////////
void CSendToManager::AddObjectList(UINT uiParent, UINT& uiCommand, GuidKey guidDocKey )
{

	CString strCreateInDocument;
	strCreateInDocument.LoadString( IDS_CREATE_IN_DOCUMENT );

	CString strAllSelectedObjects;
	strAllSelectedObjects.LoadString( IDS_ALL_SELECTED_OBJECTS );	

	//Add all
	AddItem(	strAllSelectedObjects, "", "", MF_STRING | MF_ENABLED, iiAllObject, 
				++uiCommand, uiParent, 
				guidDocKey, INVALID_KEY);					

	//Add separator
	AddItem(	"", "", "", MF_SEPARATOR, iiSeparator, 
				0, uiParent, 
				guidDocKey, INVALID_KEY);

	POSITION pos = m_arGuidSourceObject.GetHeadPosition();
	
	while( pos )
	{
		GuidKey* pkey = (GuidKey*)m_arGuidSourceObject.GetNext( pos );
		
		IUnknownPtr ptrObj = GetObject( m_guidSourceDoc, *pkey );
		if( ptrObj == NULL )
			continue;

		CString strObjName = ::GetObjectName( ptrObj );
		if( strObjName.IsEmpty() )
			continue;

		AddItem( strObjName, "", "", MF_STRING | MF_ENABLED, iiSingleObject,
				 ++uiCommand, uiParent, 
				 guidDocKey, *pkey );
	}

}

/////////////////////////////////////////////////////////////////////////////
CItemInfo* CSendToManager::AddItem( CString	strTitle, CString strTable, CString strField, 
									UINT uiFlags, ItemInfo itemInfo, 
									UINT uiItemID, UINT uiParentID, 
									GuidKey guidDoc, GuidKey guidObj )
{
	CItemInfo* pii = new CItemInfo;
	m_arItems.AddTail( pii );

	pii->m_strTitle		= strTitle;
	pii->m_uiItemID		= uiItemID;
	pii->m_uiParentID	= uiParentID;
	pii->m_uiFlags		= uiFlags;
	pii->m_itemInfo		= itemInfo;
	pii->m_guidDoc		= guidDoc;
	pii->m_guidObj		= guidObj;
	pii->m_strTable		= strTable;
	pii->m_strField		= strField;
	return pii;
}

/////////////////////////////////////////////////////////////////////////////
void CSendToManager::GetFieldsFromDBaseDoc( IUnknown* punkDoc, 
										   CStringArray& arTable, CStringArray& arField, CStringArray& arCaption )
{
	IDBaseDocumentPtr ptrDB( punkDoc );
	if( ptrDB == NULL )
		return;

	IDBaseStructPtr ptrDBS( ptrDB );
	if( ptrDBS == 0 )
		return;

	IUnknown* punkQuery = NULL;
	ptrDB->GetActiveQuery( &punkQuery );
	if( punkQuery == NULL )
		return;

	ISelectQueryPtr ptrSelQ( punkQuery );
	punkQuery->Release();

	if( ptrSelQ == NULL )
		return;

	BOOL bOpen = FALSE;
	ptrSelQ->IsOpen( &bOpen );
	if( !bOpen )
		return;

	IQueryObjectPtr ptrQ( ptrSelQ );
	if( ptrQ == 0 )
		return;

	int nCount = 0;
	ptrQ->GetFieldsCount( &nCount );
	for( int i=0;i<nCount;i++ )
	{
		BSTR bstrTable = 0;
		BSTR bstrField = 0;

		if( S_OK != ptrQ->GetField( i, 0, &bstrTable, &bstrField ) )
			continue;

		_bstr_t _bstrTable = bstrTable;
		_bstr_t _bstrField = bstrField;

		::SysFreeString( bstrTable );	bstrTable = 0;
		::SysFreeString( bstrField );	bstrField = 0;


		BSTR bstrCaption = 0;
		short nType = 0;
		if( S_OK != ptrDBS->GetFieldInfo( _bstrTable, _bstrField, &bstrCaption, &nType, 0, 0, 0, 0, 0, 0 ) )
			continue;

		_bstr_t _bstrCaption = bstrCaption;
		::SysFreeString( bstrCaption );	bstrCaption = 0;

		FieldType ft = (FieldType)nType;

		if( ft != ftVTObject )
			continue;

		arTable.Add( _bstrTable );
		arField.Add( _bstrField );
		arCaption.Add( _bstrCaption );
	}

/*
	INamedDataPtr ptrND( punkDoc );
	if( ptrND == NULL )
		return;

	
	ptrND->SetupSection( _bstr_t(SECTION_DBASEFIELDS) );
	long lEntryCount = 0;
	ptrND->GetEntriesCount( &lEntryCount );
	for ( long i = 0; i < lEntryCount; i++)
	{
		BSTR bstr = NULL;
		// get entry name
		
		_bstr_t bstrName;
		ptrND->GetEntryName(i, &bstr);
		bstrName = bstr;
		::SysFreeString( bstr );
		_bstr_t bstrPath = _bstr_t(SECTION_DBASEFIELDS) + "\\" + bstrName;
		
		// get value
		_variant_t var;
		ptrND->GetValue( bstrPath, &var);

		if( !( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH ) )
			continue;

		CString strName = ::GetObjectName( var.punkVal );
		arFields.Add( strName );
	}
	*/
	
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CSendToManager::GetDocumentByKey( GuidKey guidKey )
{	
	IUnknownPtr ptrDoc;

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{			
		long	lPosTemplate = 0;
		
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		
		while( lPosTemplate )
		{
			long	lPosDoc = 0;

			sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

				if( !punkDoc )
					continue;

				if( guidKey == ::GetObjectKey(punkDoc) )
				{
					ptrDoc = punkDoc;
				}					
				
				punkDoc->Release();

				if( ptrDoc != NULL )
					return (IUnknown*)ptrDoc;
			}

			sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
		} 
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CSendToManager::GetObject( GuidKey guidDoc, GuidKey guidObject )
{
	IUnknownPtr ptrObj;
	IUnknownPtr ptrDoc = GetDocumentByKey( guidDoc );
	if( ptrDoc == NULL )
		return NULL;

	IUnknown* punkObj = ::GetObjectByKey( ptrDoc, guidObject );
	if( punkObj == NULL ) 
		return NULL;

	ptrObj = punkObj;

	punkObj->Release();

	return ptrObj;	
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::SetSourceDoc( IUnknown* punkDoc, UINT uiStartItemID )
{
	_try_nested(CSendToManager, SendToManager, SetSource)
	{	
		pThis->DeInit();
		
		pThis->m_guidSourceDoc = ::GetObjectKey( punkDoc );
		pThis->m_uiStartItemID = uiStartItemID;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::SetSingleObjectName( BSTR bstrObjectName )
{
	_try_nested(CSendToManager, SendToManager, SetSingleObjectName)
	{	
		CString strMenuText = bstrObjectName;
		CString strFormat;
		strFormat.LoadString( IDS_SEND_OBJECT_TO );

		int nidx = strFormat.Find( "%s" );
		int nformatlen = strFormat.GetLength();

		int nleft = nidx;
		int nright = nformatlen - nidx - 2;

		int nmenu_len = strMenuText.GetLength();

		if( nleft < nmenu_len && nmenu_len - nright - nleft > 0 && strFormat.Left(nleft) == strMenuText.Left(nleft))
		{
			CString strObjectName = strMenuText.Mid( nleft, nmenu_len - nright - nleft );
			pThis->m_strSingleObjectName = strObjectName;
//			pThis->CheckItemsForSingleName();
		}
		pThis->CheckAvailableItems();
		return S_OK;
	}
	_catch_nested;
}

static bool _IsAnyObject( IUnknown *punkData, const char *szType )
{
	sptrIDataTypeManager	sptrM( punkData );
	if( sptrM == 0 )
		return false;
	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );
	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrTypeName = 0;
		sptrM->GetType( nType, &bstrTypeName );

		CString	str = bstrTypeName;

		::SysFreeString( bstrTypeName );

		if( str == szType )
		{
			long	lpos = 0;
			sptrM->GetObjectFirstPosition( nType, &lpos );
			return lpos!=0?true:false;
		}
	}
	return false;
}

bool CSendToManager::HasChildren(CItemInfo *piiTest)
{
	POSITION pos = m_arItems.GetHeadPosition();
	while (pos)
	{
		POSITION posPrev = pos;
		CItemInfo* pii = (CItemInfo*)m_arItems.GetNext(pos);
		if (pii->m_uiParentID == piiTest->m_uiItemID)
			return true;
	}
	return false;
}


/*void CSendToManager::CheckItemsForSingleName()
{
	IUnknownPtr punkSrcDoc = GetDocumentByKey(m_guidSourceDoc);
	IUnknownPtr punkObj(GetObjectByName(punkSrcDoc, m_strSingleObjectName), false);
	CString sObjType = GetObjectKind(punkObj);
	if (sObjType.IsEmpty())
		return;
	if (GetValueInt(::GetAppUnknown(), "\\ContextViewer\\EnableCopy", sObjType, 1) == 0)
	{
		POSITION pos = m_arItems.GetHeadPosition();
		while (pos)
		{
			POSITION posPrev = pos;
			CItemInfo* pii = (CItemInfo*)m_arItems.GetNext(pos);
			if (pii->m_strTable.IsEmpty() && pii->m_strField.IsEmpty() && !HasChildren(pii))
			{
				IUnknownPtr punkDoc = GetDocumentByKey(pii->m_guidDoc);
				if (_IsAnyObject(punkDoc, sObjType))
				{
					m_arItems.RemoveAt(posPrev);
					delete pii;
				}
			}
		}
	}
}*/

static bool IsBaseObject(INamedDataPtr ptrND, GuidKey guidObj)
{
	if( ptrND == NULL )	return false;
	IUnknownPtr ptrTestObject(::GetObjectByKey( ptrND, guidObj ), false);
	if( ptrTestObject == NULL )	return false;	
	//try to test for parent
	INamedDataObject2Ptr ptrNDO2( ptrTestObject );
	if( ptrNDO2 != 0 )
	{
		IUnknownPtr punkParent;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent != 0)
			return false;
	}
	long lGroupPos = 0;
	ptrND->GetBaseGroupFirstPos( &lGroupPos );
	while( lGroupPos )
	{
		GuidKey guidCurGroup = INVALID_KEY;
		ptrND->GetNextBaseGroup( &guidCurGroup, &lGroupPos );
		IUnknownPtr punkBaseObject;
		ptrND->GetBaseGroupBaseObject( &guidCurGroup, &punkBaseObject );
		if( punkBaseObject == 0 )
			continue;
		if( guidObj == ::GetObjectKey( punkBaseObject ) )
			return true;
	}
	return false;
}

bool GetDBActiveField(IUnknown *punkSrcDoc, short &nFieldType, CString &sVTObjType)
{
	IDBaseDocumentPtr ptrDBDoc(punkSrcDoc);
	if (ptrDBDoc == 0)
		return false;
	IUnknownPtr punkActiveQuery;
	ptrDBDoc->GetActiveQuery(&punkActiveQuery);
	if (punkActiveQuery == 0)
		return false;
//	ISelectQueryPtr ptrQ(punkActiveQuery);
//	if (ptrQ == 0)
//		return false;
	IQueryObjectPtr ptrQO(punkActiveQuery);
	if (ptrQO == 0)
		return false;
	int nActiveField = -1;
	ptrQO->GetActiveField(&nActiveField);
	if (nActiveField == -1)
		return false;
	_bstr_t bstrTable, bstrField;
	ptrQO->GetField(nActiveField, 0, bstrTable.GetAddress(), bstrField.GetAddress());
	IDBaseStructPtr ptrS(punkSrcDoc);
	if (ptrS == 0)
		return false;
	if (ptrS->GetFieldInfo(bstrTable, bstrField, 0, &nFieldType, 0, 0, 0, 0, 0, 0) != S_OK) 
		return false;
	if (nFieldType == ftVTObject)
	{
		sVTObjType.Empty();
		CString strFieldName;
		strFieldName.Format( "%s.%s", (LPCTSTR)bstrTable, (LPCTSTR)bstrField);
		_bstr_t bstrEntry( SECTION_DBASEFIELDS );
		bstrEntry += "\\";
		bstrEntry += (LPCTSTR)strFieldName;
		_variant_t var;
		INamedDataPtr ptrND(punkSrcDoc);
		ptrND->GetValue( bstrEntry, &var );
		if( !( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH ) )
			return false;
		sVTObjType = ::GetObjectKind( var.punkVal );
		return true;
	}
	return true;
}


void CSendToManager::MakeBaseObjectsArray(IUnknown *punkSrcDoc, CStringArray &arrObjTypes)
{
	// Sent from view. Gather all base objects.
	INamedDataPtr ptrND(punkSrcDoc);
	_bstr_t bstrViewName;
	IDocumentSitePtr ptrDS(punkSrcDoc);
	IUnknownPtr punkActiveView;
	if (ptrDS != 0)
		ptrDS->GetActiveView(&punkActiveView);
	if (punkActiveView == 0)
		return;
	INamedObject2Ptr sptrNDO2(punkActiveView);
	if (sptrNDO2 != 0)
		sptrNDO2->GetName(bstrViewName.GetAddress());
	CString sViewName((LPCTSTR)bstrViewName);
	IDataContext2Ptr ptrContext(punkActiveView);
	if (ptrContext == 0)
		return;
	if (sViewName == _T("BlankView"))
	{
		short nFieldType;
		CString sVTObjType;
		if (GetDBActiveField(punkSrcDoc, nFieldType, sVTObjType) &&
			nFieldType == ftVTObject && !sVTObjType.IsEmpty())
		{
			sViewName += _T("\\");
			sViewName += sVTObjType;
		}
	}
	CStringArray arrAnyObjects;
	long lTypeCount = 0;
	ptrContext->GetObjectTypeCount( &lTypeCount );
	for( long i=0;i<lTypeCount;i++)
	{
		_bstr_t bstrType;
		ptrContext->GetObjectTypeName( i, bstrType.GetAddress() );
		if (bstrType.length() <= 0)
			continue;
		if (bstrViewName.length() > 0 && bstrType.length() > 0)
		{
			CString sSection("\\SendToManager\\");
			sSection += sViewName;
			CString sType((LPCTSTR)bstrType);
			if (GetValueInt(GetAppUnknown(), sSection, sType, 1) == 0)
				continue;
		}
		bool bBaseObjectFound = false;
		bool bAnyObjectFound = false;
		long lPos = 0;
		ptrContext->GetFirstSelectedPos( bstrType, &lPos );
		while( lPos )
		{
			IUnknownPtr punkObj;
			ptrContext->GetNextSelected( bstrType, &lPos, &punkObj );
			if( punkObj != 0 )
			{
				INamedDataObject2Ptr ptrN( punkObj );				
				if( ptrN == 0 )	continue;				
				DWORD	dwFlags = 0;
				ptrN->GetObjectFlags( &dwFlags );
				if( dwFlags & nofCantSendTo )
					continue;
				bAnyObjectFound = true;
				if (IsBaseObject(ptrND, GetObjectKey(ptrN)))
				{
					bBaseObjectFound = true;
					break;
				}
			}
		}
		if (bBaseObjectFound)
			arrObjTypes.Add((LPCTSTR)bstrType);
		if (bAnyObjectFound)
			arrAnyObjects.Add((LPCTSTR)bstrType);
	}
	if (arrObjTypes.GetSize() == 0)
	{
		for (int i = 0; i < arrAnyObjects.GetSize(); i++)
			arrObjTypes.Add(arrAnyObjects.GetAt(i));
	}
}

bool CheckForDBDoc(IUnknown *punkSrcDoc)
{
	IDocumentSitePtr sptrDS(punkSrcDoc);
	if (sptrDS == 0)
		return false;
	IUnknownPtr punkAView;
	sptrDS->GetActiveView(&punkAView);
	if (!CheckInterface(punkAView, IID_IBlankView))
		return false;
	IDBaseDocumentPtr ptrDBDoc(punkSrcDoc);
	if (ptrDBDoc == 0)
		return false;
	IUnknownPtr punkActiveQuery;
	ptrDBDoc->GetActiveQuery(&punkActiveQuery);
	if (punkActiveQuery == 0)
		return false;
//	ISelectQueryPtr ptrQ(punkActiveQuery);
//	if (ptrQ == 0)
//		return false;
	IQueryObjectPtr ptrQO(punkActiveQuery);
	if (ptrQO == 0)
		return false;
	int nActiveField = -1;
	ptrQO->GetActiveField(&nActiveField);
	if (nActiveField == -1)
		return false;
	_bstr_t bstrTable, bstrField;
	ptrQO->GetField(nActiveField, 0, bstrTable.GetAddress(), bstrField.GetAddress());
	IDBaseStructPtr ptrS(punkSrcDoc);
	if (ptrS == 0)
		return false;
	short nFieldType;
	if (ptrS->GetFieldInfo(bstrTable, bstrField, 0, &nFieldType, 0, 0, 0, 0, 0, 0) != S_OK) 
		return false;
	if (nFieldType == ftVTObject)
		return false;
	return true;
}



void CSendToManager::DisableAll()
{
	POSITION pos = m_arItems.GetHeadPosition();
	while (pos)
	{
		POSITION posPrev = pos;
		CItemInfo* pii = (CItemInfo*)m_arItems.GetNext(pos);
		pii->m_uiFlags |= MF_GRAYED;
	}
}


void CSendToManager::CheckAvailableItems()
{
	IUnknownPtr punkSrcDoc = GetDocumentByKey(m_guidSourceDoc);
	if (m_strSingleObjectName.IsEmpty() && CheckForDBDoc(punkSrcDoc))
	{
		DisableAll();
		return;
	}
	CStringArray arrObjTypes;
	if (m_strSingleObjectName.IsEmpty())
		MakeBaseObjectsArray(punkSrcDoc, arrObjTypes);
	else
	{
		// Sent from context viewer, only one object
		IUnknownPtr punkObj(GetObjectByName(punkSrcDoc, m_strSingleObjectName), false);
		INamedDataObject2Ptr ptrN(punkObj);
		if (ptrN != 0)
		{
			DWORD dwFlags = 0;
			ptrN->GetObjectFlags(&dwFlags);
			if ((dwFlags&nofCantSendTo) == 0)
			{
				CString sObjType = GetObjectKind(punkObj);
				arrObjTypes.Add(sObjType);
			}
		}
	}
	if (arrObjTypes.GetSize() == 0)
	{
		DisableAll();
		return;
	}
	POSITION pos = m_arItems.GetHeadPosition();
	while (pos)
	{
		POSITION posPrev = pos;
		CItemInfo* pii = (CItemInfo*)m_arItems.GetNext(pos);
		if (pii->m_strTable.IsEmpty() && pii->m_strField.IsEmpty() && !HasChildren(pii))
		{
			IUnknownPtr punkDoc = GetDocumentByKey(pii->m_guidDoc);
			bool bAlreadyExist = false;
			for (int i = 0; i < arrObjTypes.GetSize(); i++)
			{
				CString sObjType = arrObjTypes[i];
				if (GetValueInt(::GetAppUnknown(), "\\ContextViewer\\EnableSendToImage", sObjType, 1) == 0)
				{ //BT5121
					bAlreadyExist = true;
					break;
				}
				if (GetValueInt(::GetAppUnknown(), "\\ContextViewer\\EnableCopy", sObjType, 1) == 0)
				{
					if (_IsAnyObject(punkDoc, sObjType))
					{
						bAlreadyExist = true;
						break;
					}
				}
			}

			if (bAlreadyExist)
//			{
				pii->m_uiFlags |= MF_GRAYED;
//				m_arItems.RemoveAt(posPrev);
//				delete pii;
//			}
		}
		if (!pii->m_strTable.IsEmpty() && !pii->m_strField.IsEmpty())
		{
			IUnknownPtr punkDoc = GetDocumentByKey(pii->m_guidDoc);
			bool bCompatible = true;
			for (int i = 0; i < arrObjTypes.GetSize(); i++)
			{
				CString sObjType = arrObjTypes[i];
				CString strFieldName;
				strFieldName.Format( "%s.%s", (LPCTSTR)pii->m_strTable, (LPCTSTR)pii->m_strField );
				if (!IsCompatibleField(punkDoc, strFieldName, sObjType))
					bCompatible = false;
			}
			if (!bCompatible)
				pii->m_uiFlags |= MF_GRAYED;
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::AddObject( IUnknown* punkObject )
{
	_try_nested(CSendToManager, SendToManager, AddObject)
	{	

		GuidKey* pkey = new GuidKey;
		*pkey = ::GetObjectKey( punkObject );

		pThis->m_arGuidSourceObject.AddTail( pkey );

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::CreateTargets( )
{
	_try_nested(CSendToManager, SendToManager, CreateTargets)
	{	
		pThis->Init();

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetFirstItemPos( long* plPos )
{
	_try_nested(CSendToManager, SendToManager, GetFirstItemPos)
	{				
		*plPos = (long)pThis->m_arItems.GetHeadPosition();
		return S_OK;
	}

	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetNextItem(	
													UINT* puiFlags, 
													UINT* puiItemID, 
													BSTR* pbstrText,			
													UINT* puiParentID,
													long* plPos 
													)
{
	_try_nested(CSendToManager, SendToManager, GetNextItem)
	{				
		POSITION pos = (POSITION)(*plPos);
		CItemInfo* pi = (CItemInfo*)pThis->m_arItems.GetNext( pos );
		*plPos = (long)pos;
		if( pi )
		{
			if (puiFlags != NULL)
				*puiFlags = pi->m_uiFlags;
			if (puiItemID != NULL)
				*puiItemID = pi->m_uiItemID;
			if (pbstrText != NULL)
				*pbstrText = pi->m_strTitle.AllocSysString();
			if (puiParentID != NULL)
				*puiParentID = pi->m_uiParentID;			
		}

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::OnCommand( UINT uiCommand )
{
	_try_nested(CSendToManager, SendToManager, OnCommand)
	{				
		//if( !pThis->m_bContentSync )
		//	return S_OK;


		CItemInfo* pItem = pThis->FindByID( uiCommand );
		if( !pItem )
			return S_FALSE;

		CItemInfo* pItemParent = pThis->FindByID( pItem->m_uiParentID );

		CString strSourceDocGuid, strTargetDocGuid;				
		strSourceDocGuid.Format(  "%I64X-%I64X",	lo_guid( &(pThis->m_guidSourceDoc) ), 
													hi_guid( &(pThis->m_guidSourceDoc) ) );		

		strTargetDocGuid.Format(  "%I64X-%I64X",	lo_guid( &(pItem->m_guidDoc) ), 
													hi_guid( &(pItem->m_guidDoc) ) );		

		
		CString strFieldName;
		if( pItem->m_itemInfo == iiDBFieldTitle )
		{
			strFieldName.Format( "%s.%s", pItem->m_strTable, pItem->m_strField );
		}

		BOOL bPutToDoc = ( pItem->m_itemInfo == iiDBPutToDocument );
		BOOL bNewDoc = ( pItem->m_itemInfo == iiNewDocument );
		
		CString strArg;
		strArg.Format( "\"%s\",\"%s\",%d,\"%s\",%d,\"%s\"",
						(LPCSTR)strSourceDocGuid, (LPCSTR)strTargetDocGuid, 
						(int)bPutToDoc, (LPCSTR)strFieldName, bNewDoc, pThis->m_strSingleObjectName );

		::ExecuteAction( GetActionNameByResID( IDS_SEND_TO ), strArg );	


		pThis->DeInit();


		return S_OK;
	}
	_catch_nested;
}

/*
/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetSourceDocGuid( GUID* pDocGuid )
{
	_try_nested(CSendToManager, SendToManager, GetSourceDocGuid)
	{				
		*pDocGuid = pThis->m_guidSourceDoc;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetFirstObjectPos( long* plPos )
{
	_try_nested(CSendToManager, SendToManager, GetFirstObjectPos)
	{				
		*plPos = (long)pThis->m_arGuidSourceObject.GetHeadPosition();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetNextObjectPos( GUID* pObjGuid, long* plPos )
{
	_try_nested(CSendToManager, SendToManager, GetNextObjectPos)
	{				
		POSITION pos = (POSITION)(*plPos);
		*pObjGuid = *((GuidKey*)pThis->m_arGuidSourceObject.GetNext( pos ));
		*plPos = (long)pos;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CSendToManager::XSendToManager::GetItemInfo( UINT uiItemID, CItemInfo** ppii )
{
	_try_nested(CSendToManager, SendToManager, GetItemInfo)
	{				
		*ppii = pThis->FindByID( uiItemID );		
		return S_OK;
	}
	_catch_nested;
}
*/
/////////////////////////////////////////////////////////////////////////////
CString CSendToManager::GetObjectNameFromPath( CString strPath )
{
	CString strResult;
	int nPos = -1;

	for( int i=strPath.GetLength() - 1;i>=0;i--)
	{
		if( nPos != -1 )
			continue;

		if( strPath[i] == '\\' )
			nPos = i;
	}

	if( nPos == -1 )
		return strPath;

	strResult = strPath.Right( strPath.GetLength() - nPos - 1 );

	return strResult;
}


/////////////////////////////////////////////////////////////////////////////
CItemInfo* CSendToManager::FindByID( UINT uiItemID )
{
	POSITION pos = m_arItems.GetHeadPosition();
	while( pos )
	{
		CItemInfo* pi = (CItemInfo*)m_arItems.GetNext( pos );
		if( pi->m_uiItemID == uiItemID )
			return pi;
	}

	return NULL;
}