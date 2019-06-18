#include "stdafx.h"
#include "Data.h"

#include "send.h"
#include "SendToManager.h"

#include "dbase.h"
#include "docmiscint.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//
//	Class CSubMenu
//
//////////////////////////////////////////////////////////////////////
class CSubMenu : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CSubMenu);
	
protected:
	CSubMenu();
	~CSubMenu();

	UINT	m_uiStartItemID;

	void InitSendManager();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(SubMenu,ISubMenu)
		com_call SetStartItemID( UINT uiItemID );
		com_call SetSingleObjectName( BSTR bstrObjectName );
		com_call GetFirstItemPos( long* plPos );
		com_call GetNextItem(	
				UINT* puiFlags, 
				UINT* puiItemID, 
				BSTR* pbstrText,				
				UINT* puiParentID,
				long* plPos );
		com_call OnCommand( UINT uiCmd );	
	END_INTERFACE_PART(SubMenu)
public:
	
};

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSubMenu, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CSubMenu, SubMenu)

BEGIN_INTERFACE_MAP(CSubMenu, CCmdTargetEx)
	INTERFACE_PART(CSubMenu, IID_ISubMenu, SubMenu)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
CSubMenu::CSubMenu()
{			
	m_uiStartItemID = -1;
	
}

//////////////////////////////////////////////////////////////////////
CSubMenu::~CSubMenu()
{	
	//DestroyMenuDiscription();
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::SetStartItemID( UINT uiItemID )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	pThis->m_uiStartItemID = uiItemID;
	pThis->InitSendManager();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::SetSingleObjectName( BSTR bstrObjectName )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)	
	
	ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	ptrS2->SetSingleObjectName( bstrObjectName );	

	return S_OK;
}



//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::GetFirstItemPos( long* plPos )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)

	*plPos = 0;

	ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	if( ptrS2 )
	{
		ptrS2->GetFirstItemPos( plPos );		
	}		
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::GetNextItem(	
										UINT* puiFlags, 
										UINT* puiItemID, 
										BSTR* pbstrText, 							
										UINT* puiParentID,
										long* plPos 
										)
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	

	ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	if( ptrS2 )
	{
		ptrS2->GetNextItem( puiFlags, puiItemID, pbstrText, puiParentID, plPos );
	}		
	else
	{
		*plPos = 0;
	}
	
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::OnCommand( UINT uiCmd )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)

	ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	if( ptrS2 == NULL )
		return S_FALSE;

	ptrS2->OnCommand( uiCmd );

	return S_OK;
}

void CSubMenu::InitSendManager()
{
	ISendToManagerPtr ptrSM( ::GetAppUnknown() );
	if( ptrSM == NULL )
		return;
	
	ptrSM->SetSourceDoc( 0, 0 );

	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == NULL )
		return;

	IUnknown* punk = NULL;
	ptrApp->GetActiveDocument( &punk );
	if( !punk )
		return;	

	IDocumentSitePtr ptrDS( punk );
	punk->Release();
	punk = NULL;

	if( ptrDS == NULL )
		return;


	ptrDS->GetActiveView( &punk );
	if( !punk )
		return;

	IDataContext2Ptr ptrContext( punk );
	punk->Release();
	punk = NULL;

	if( ptrContext == NULL )
		return;


	ptrSM->SetSourceDoc( ptrDS, m_uiStartItemID );

	long lTypeCount = 0;
	ptrContext->GetObjectTypeCount( &lTypeCount );
	
	for( long i=0;i<lTypeCount;i++)
	{
		BSTR bstr = 0;		
		ptrContext->GetObjectTypeName( i, &bstr );
		if( !bstr )
			continue;
			
		_bstr_t bstrType = bstr;
		::SysFreeString( bstr );

		long lPos = 0;
//		ptrContext->GetFirstSelectedPos( bstrType, &lPos );
		ptrContext->GetFirstObjectPos( bstrType, &lPos );
		while( lPos )
		{
			IUnknown* punkObj = 0;
//			ptrContext->GetNextSelected( bstrType, &lPos, &punkObj );
			ptrContext->GetNextObject( bstrType, &lPos, &punkObj );
			if( punkObj )
			{
				INamedDataObject2Ptr ptrN( punkObj );				
				punkObj->Release();	punkObj = 0;
				if( ptrN == 0 )
					continue;				

				DWORD	dwFlags = 0;
				ptrN->GetObjectFlags( &dwFlags );

				if( dwFlags & nofCantSendTo )
					continue;

				ptrSM->AddObject( ptrN );				
			}
		}
	}


	ptrSM->CreateTargets();
	
	
}


//////////////////////////////////////////////////////////////////////
//
//	Class CBaseObject
//
//////////////////////////////////////////////////////////////////////
CBaseObject::CBaseObject()
{
	m_guidBaseObject	= INVALID_KEY;
	m_guidGroup			= INVALID_KEY;
}

//////////////////////////////////////////////////////////////////////
CBaseObject::~CBaseObject()
{	
	POSITION pos = m_arGuidChild.GetHeadPosition();	
	while( pos )
	{
		GuidKey* pkey = (GuidKey*)m_arGuidChild.GetNext( pos );
		delete pkey;
	}	
	
	m_arGuidChild.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
bool CBaseObject::IsChild( INamedDataPtr ptrND, GuidKey guidChild )
{	
	if( m_guidBaseObject == INVALID_KEY || m_guidGroup == INVALID_KEY )
		return false;	
	
	IUnknownPtr ptrParent;
	IUnknownPtr ptrChild;

	IUnknown* punk = 0;
	punk = ::GetObjectByKey( ptrND, m_guidBaseObject );
	if( punk )
	{
		ptrParent = punk;
		punk->Release();	punk = 0;
	}

	punk = ::GetObjectByKey( ptrND, guidChild );
	if( punk )
	{
		ptrChild = punk;
		punk->Release();	punk = 0;
	}

	if( ptrParent == NULL || ptrChild == NULL )
		return false;

	//At first test for parent

	//try to test for parent
	INamedDataObject2Ptr ptrNDO2( ptrChild );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
		{
			bool bChild = false;
			if( ::GetObjectKey( punkParent ) == ::GetObjectKey( ptrParent ) )
				bChild = true;

			punkParent->Release();

			if( bChild )
				return true;
		}
	}


	if( m_guidGroup == INVALID_KEY )
		return false;

	long lPos = 0;
	ptrND->GetBaseGroupObjectFirstPos( &m_guidGroup, &lPos );
	while( lPos )
	{
		IUnknown* punkObj = 0;
		ptrND->GetBaseGroupNextObject( &m_guidGroup, &lPos, &punkObj );
		if( punkObj == NULL )
			continue;		

		if( ::GetObjectKey(punkObj) == guidChild )
		{
			punkObj->Release();
			return true;
		}

		punkObj->Release();
	}

	
	return false;
}



// dyncreate

IMPLEMENT_DYNCREATE(CActionSendTo, CCmdTargetEx);
// olecreate 

// {D4F3C81C-70A6-4e1c-9B22-4568BEB6E1E3}
GUARD_IMPLEMENT_OLECREATE(CActionSendTo, "Data.SendTo",
0xd4f3c81c, 0x70a6, 0x4e1c, 0x9b, 0x22, 0x45, 0x68, 0xbe, 0xb6, 0xe1, 0xe3);

// {7CCD25B0-F440-43ed-97F1-9F16DCE095E7}
static const GUID guidSendTo =
{ 0x7ccd25b0, 0xf440, 0x43ed, { 0x97, 0xf1, 0x9f, 0x16, 0xdc, 0xe0, 0x95, 0xe7 } };


ACTION_INFO_FULL(CActionSendTo, IDS_SEND_TO, IDS_MENU_EDIT, IDS_TB_DATA, guidSendTo);
// targets

ACTION_TARGET(CActionSendTo, "anydoc");
// implementation



//////////////////////////////////////////////////////////////////////
ACTION_ARG_LIST(CActionSendTo)
	ARG_STRING("SourceDocGuid", "")
	ARG_STRING("TargetDocGuid", "")
	ARG_INT("PutToDocument", 1)
	ARG_STRING("DBaseFieldName", "")
	ARG_INT("NewDocument", 0)
	ARG_STRING("SingleObjectName", "")
END_ACTION_ARG_LIST()

ACTION_UI(CActionSendTo, CSubMenu)

BEGIN_INTERFACE_MAP(CActionSendTo, CActionBase)
	INTERFACE_PART(CActionSendTo, IID_ILongOperation, Long)
END_INTERFACE_MAP()



//////////////////////////////////////////////////////////////////////
//CActionSendTo implementation
CActionSendTo::CActionSendTo()
{	
	m_guidSourceDoc		= INVALID_KEY;
	m_guidTargetDoc		= INVALID_KEY;
	m_bPutToDocument	= FALSE;
	m_bNewDocument		= FALSE;
}

//////////////////////////////////////////////////////////////////////
CActionSendTo::~CActionSendTo()
{
	DestroyObjectList();
}

//////////////////////////////////////////////////////////////////////
//extended UI
bool CheckForDBDoc(IUnknown *punkSrcDoc);
bool CActionSendTo::IsAvaible()
{
	ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	if (ptrS2 != 0)
	{
		bool bFoundEnabled = false;
		long lpos = 0;
		ptrS2->GetFirstItemPos(&lpos);
		while (lpos != 0)
		{
			UINT uiFlags;
			ptrS2->GetNextItem(&uiFlags, NULL, NULL, NULL, &lpos);
			if ((uiFlags&MF_GRAYED) == 0)
				return true;
		}
	}
	return false;

/*	IUnknownPtr punkAM(_GetOtherComponent(::GetAppUnknown(), IID_IActionManager), false);
	IActionManagerPtr sptrAM(punkAM);
	if (sptrAM != 0 && sptrAM->IsScriptRunning() != S_OK)
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp == NULL )
			return false;;
		IUnknownPtr punk;
		ptrApp->GetActiveDocument(&punk);
		if (punk == 0)
			return false;	
		IDocumentSitePtr ptrDS(punk);
		if (ptrDS == NULL)
			return false;
		if (CheckForDBDoc(ptrDS))
			return false;
	}*/
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::CreateObjectList()
{
	DestroyObjectList();

	if( m_guidSourceDoc == INVALID_KEY || m_guidTargetDoc == INVALID_KEY )
		return false;

	INamedDataPtr ptrNDSource = CSendToManager::GetDocumentByKey( m_guidSourceDoc );	

	if( ptrNDSource == NULL )
		return false;

	CPtrList arObjGuid;

	//Fill selected object array
	IDocumentSitePtr ptrDS( ptrNDSource );

	if( ptrDS == NULL )
		return false;

   IUnknown* punk = NULL;
	ptrDS->GetActiveView( &punk );
	if( !punk )
		return false;

	IDataContext2Ptr ptrContext( punk );
	punk->Release();
	punk = NULL;

	if( ptrContext == NULL )
		return false;

	if( !m_strSingleObjectName.IsEmpty() )
	{
		IUnknown* punkObj = ::GetObjectByName( ptrDS, (LPCSTR)m_strSingleObjectName, NULL );
		if( punkObj )
		{

			CBaseObject* pBaseObj = new CBaseObject;
			pBaseObj->m_guidBaseObject	= ::GetObjectKey( punkObj );
			//pBaseObj->m_guidGroup		= guidGroup;			
			
			m_arBaseObject.AddTail( pBaseObj );

			punkObj->Release();	punkObj = 0;
			
			return true;
		}

		return false;  
		
	}
	else
	{
		_bstr_t bstrViewName;
		INamedObject2Ptr sptrNDO2(ptrContext);
		sptrNDO2->GetName(bstrViewName.GetAddress());
		CString sViewName((LPCTSTR)bstrViewName);
		if (sViewName == _T("BlankView"))
		{
			short nFieldType;
			CString sVTObjType;
			if (GetDBActiveField(ptrNDSource, nFieldType, sVTObjType) &&
				nFieldType == ftVTObject && !sVTObjType.IsEmpty())
			{
				sViewName += _T("\\");
				sViewName += sVTObjType;
			}
		}

		long lTypeCount = 0;
		ptrContext->GetObjectTypeCount( &lTypeCount );
		
		for( long i=0;i<lTypeCount;i++)
		{
			BSTR bstr = 0;		
			ptrContext->GetObjectTypeName( i, &bstr );
			if( !bstr )
				continue;
				
			_bstr_t bstrType = bstr;
			::SysFreeString( bstr );

			if (bstrViewName.length() > 0 && bstrType.length() > 0)
			{
				CString sSection("\\SendToManager\\");
				sSection += sViewName;
				CString sType((LPCTSTR)bstrType);
				if (GetValueInt(GetAppUnknown(), sSection, sType, 1) == 0)
					continue;
			}

			long lPos = 0;
			ptrContext->GetFirstSelectedPos( bstrType, &lPos );
			while( lPos )
			{
				IUnknown* punkObj = 0;
				ptrContext->GetNextSelected( bstrType, &lPos, &punkObj );
				if( punkObj )
				{
					INamedDataObject2Ptr ptrN( punkObj );				
					punkObj->Release();	punkObj = 0;

					if( ptrN == 0 )
						continue;				

					DWORD	dwFlags = 0;
					ptrN->GetObjectFlags( &dwFlags );

					if( dwFlags & nofCantSendTo )
						continue;

					if (bstrType == _bstr_t(szArgumentTypeImage))
					{
						// A.M. BT4346. Ignore frame, only main image must be sent.
						IUnknownPtr punkParent;
						ptrN->GetParent(&punkParent);
						if (punkParent != 0)
						{
							BOOL bSelect;
							ptrContext->GetObjectSelect(punkParent, &bSelect);
							if (bSelect)
								continue;
						}
					}

					GuidKey* pKey = new GuidKey;
					(*pKey) = ::GetObjectKey( ptrN );
					arObjGuid.AddTail( pKey );					
				}
			}
		}
	}


	if( arObjGuid.GetCount() < 1 )
		return false;

	//Create base object list and not in base group object list

	//1. Create base object array
	POSITION pos = arObjGuid.GetHeadPosition();	
	while( pos )
	{
		GuidKey* pGuidObject = (GuidKey*)arObjGuid.GetNext( pos );

		CString strBOName = ::GetObjectName( CSendToManager::GetObject( m_guidSourceDoc, *pGuidObject ) );

		GuidKey guidGroup = IsBaseObject( ptrNDSource, *pGuidObject );
		if( guidGroup != INVALID_KEY )
		{
			CBaseObject* pBaseObj = new CBaseObject;
			pBaseObj->m_guidBaseObject	= *pGuidObject;
			pBaseObj->m_guidGroup		= guidGroup;			
			
			m_arBaseObject.AddTail( pBaseObj );
		}
	}

	//Put child to base object array and other to other array

	POSITION posNS = arObjGuid.GetHeadPosition();	//posNS - not sorted
	while( posNS )
	{
		GuidKey* pGuidObject = (GuidKey*)arObjGuid.GetNext( posNS );
		//CString strBOName = ::GetObjectName( CSendToManager::GetObject( m_guidSourceDoc, *pGuidObject ) );

		bool bFound = false;

		pos = m_arBaseObject.GetHeadPosition();	
		while( pos )
		{
			CBaseObject* pBaseObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			if( bFound )
				continue;

			if( IsObjectExistInBaseArray( *pGuidObject ) )
			{
				bFound = true;
				continue;
			}

			//CString strBase = ::GetObjectName( CSendToManager::GetObject( m_guidSourceDoc, pBaseObj->m_guidBaseObject ) );

			if( pBaseObj->IsChild( ptrNDSource, *pGuidObject ) )
			{
				GuidKey* pChildKey = new GuidKey;
				(*pChildKey) = (*pGuidObject);
				pBaseObj->m_arGuidChild.AddTail( pChildKey );
				bFound = true;
			}
		}

		if( !bFound )
		{
			GuidKey* pNotInGroupKey = new GuidKey;
			(*pNotInGroupKey) = (*pGuidObject);
			m_arGuidNotInGroup.AddTail( pNotInGroupKey );
		}

	}


	//Addition verify
	{

		int nCount = 0;
		pos = m_arBaseObject.GetHeadPosition();	
		while( pos )
		{
			CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			nCount++;
			nCount += pObj->m_arGuidChild.GetCount();			
		}

		nCount += m_arGuidNotInGroup.GetCount();


		VERIFY( nCount == arObjGuid.GetCount() );
	}	

	//Kill temp array
	{
		pos = arObjGuid.GetHeadPosition();	
		while( pos )
		{
			GuidKey* pkey = (GuidKey*)arObjGuid.GetNext( pos );
			delete pkey;
		}
		arObjGuid.RemoveAll();

	}	

	return true;

}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::IsObjectExistInBaseArray( GuidKey guidObj )
{
	POSITION pos = m_arBaseObject.GetHeadPosition();	
	while( pos )
	{
		CBaseObject* pBaseObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
		if( pBaseObj->m_guidBaseObject == guidObj )
			return true;		
	}

	return false;

}

//////////////////////////////////////////////////////////////////////
GuidKey CActionSendTo::IsBaseObject( INamedDataPtr ptrND, GuidKey guidObj )
{
	GuidKey keyGroup = INVALID_KEY;
	if( ptrND == NULL )
		return keyGroup;

	IUnknownPtr ptrTestObject;

	IUnknown* punk = NULL;
	punk = ::GetObjectByKey( ptrND, guidObj );	

	if( punk )
	{
		ptrTestObject = punk;
		punk->Release();	punk = 0;
	}

	if( ptrTestObject == NULL )
		return keyGroup;	

	
	//try to test for parent
	INamedDataObject2Ptr ptrNDO2( ptrTestObject );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
		{
			punkParent->Release();
			return keyGroup;
		}
	}


	//CString strTest = ::GetObjectName( ptrTestObject );


	long lGroupPos = 0;
	ptrND->GetBaseGroupFirstPos( &lGroupPos );
	while( lGroupPos )
	{
		GuidKey guidCurGroup = INVALID_KEY;
		ptrND->GetNextBaseGroup( &guidCurGroup, &lGroupPos );
		IUnknown* punkBaseObject = NULL;
		ptrND->GetBaseGroupBaseObject( &guidCurGroup, &punkBaseObject );
		if( !punkBaseObject )
			continue;

		/*
		CString strBase = ::GetObjectName( punkBaseObject );
		TRACE( "\nBO:" + strBase );
		*/

		if( guidObj == ::GetObjectKey( punkBaseObject ) )
			keyGroup = guidCurGroup;

		punkBaseObject->Release();
	}
	

	return keyGroup;
}

//////////////////////////////////////////////////////////////////////
void CActionSendTo::DestroyObjectList()
{
	
	POSITION pos = m_arBaseObject.GetHeadPosition();	
	while( pos )
	{
		CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
		delete pObj;
	}
	
	m_arBaseObject.RemoveAll();

	pos = m_arGuidNotInGroup.GetHeadPosition();	
	while( pos )
	{
		GuidKey* pkey = (GuidKey*)m_arGuidNotInGroup.GetNext( pos );
		delete pkey;
	}	
	m_arGuidNotInGroup.RemoveAll();
	
}


//////////////////////////////////////////////////////////////////////
bool CActionSendTo::Invoke()
{

	CWaitCursor wait;

	CString strSourceGuid	= GetArgumentString( "SourceDocGuid" );
	CString strTargetGuid	= GetArgumentString( "TargetDocGuid" );
	m_bPutToDocument		= GetArgumentInt( "PutToDocument" );
	CString strFieldName	= GetArgumentString( "DBaseFieldName" );

	m_bNewDocument			= GetArgumentInt( "NewDocument" );

	m_strSingleObjectName	= GetArgumentString( "SingleObjectName" );

	CString strError; // сюда валим сообщение об ошибке, если она была


	__int64 lg, hg;
	if( ::sscanf( (LPCSTR)strSourceGuid, "%I64X-%I64X", &lg, &hg ) != 2 )
		return false;

	lo_guid( &m_guidSourceDoc) = lg;
	hi_guid( &m_guidSourceDoc ) = hg;	
	
	if( ::sscanf( (LPCSTR)strTargetGuid, "%I64X-%I64X", &lg, &hg ) != 2 )
		return false;

	lo_guid( &m_guidTargetDoc ) = lg;
	hi_guid( &m_guidTargetDoc ) = hg;


	if( m_bNewDocument )
	{
		IApplicationPtr ptrApp( GetAppUnknown() );
		//m_guidTargetDoc
		CString strDoctempl = ::GetValueString( ptrApp, "\\SendToManager", "NewDocTemplate", "Image" );
		CString strArg;
		strArg.Format( "\"%s\"", (LPCTSTR)strDoctempl );

		::ExecuteAction( "FileNew", strArg, 0 );
		
		IUnknown* punkDoc = NULL;
		ptrApp->GetActiveDocument( &punkDoc );
		if( punkDoc == NULL )
			return false;

		m_guidTargetDoc = ::GetObjectKey( punkDoc ); 
		punkDoc->Release();	punkDoc = 0;		

	}


	if( !CreateObjectList() )
		return false;


	INamedDataPtr ptrNDSource = CSendToManager::GetDocumentByKey( m_guidSourceDoc );
	INamedDataPtr ptrNDTarget = CSendToManager::GetDocumentByKey( m_guidTargetDoc );

	if( ptrNDSource == NULL || ptrNDTarget == NULL )
		return false;



	IUnknownPtr ptrActiveView;
	IUnknownPtr ptrActiveObj;

	{
		IUnknown	*punkView = 0;
		IDocumentSitePtr ptrDocSite = ptrNDTarget;
		
		if( ptrDocSite )
			ptrDocSite->GetActiveView( &punkView );

		if( punkView )
		{
			ptrActiveView = punkView;			
			punkView->Release();
		}
	}

	//Set base object and its childs
	if( IsPutToDocument() )
	{
		StartNotification( m_arBaseObject.GetCount() + m_arGuidNotInGroup.GetCount(), 1, 1 );
		int nIndex = 0;

		POSITION pos = m_arBaseObject.GetHeadPosition();
		while( pos )
		{
			CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			IUnknownPtr ptrObj = CSendToManager::GetObject( m_guidSourceDoc, pObj->m_guidBaseObject );
			if( ptrObj )
			{
				IUnknownPtr ptrNew = PutToDocument( ptrObj );
				if( !pos && ptrActiveObj == NULL )
					ptrActiveObj = ptrNew;
			}
			

			POSITION posChild = pObj->m_arGuidChild.GetHeadPosition();
			while( posChild )
			{
				GuidKey* pKey = (GuidKey*)pObj->m_arGuidChild.GetNext( posChild );

				IUnknownPtr ptrObj = CSendToManager::GetObject( m_guidSourceDoc, *pKey );
				if( ptrObj )
					PutToDocument( ptrObj );
			}

			Notify( nIndex++ );
		}		

		pos = m_arGuidNotInGroup.GetHeadPosition();
		while( pos )
		{
			GuidKey* pKey = (GuidKey*)m_arGuidNotInGroup.GetNext( pos );

			IUnknownPtr ptrObj = CSendToManager::GetObject( m_guidSourceDoc, *pKey );
			if( ptrObj )
			{
				IUnknownPtr ptrNew = PutToDocument( ptrObj );
				if( !pos && ptrActiveObj == NULL )
					ptrActiveObj = ptrNew;
			}

			Notify( nIndex++ );
		}		

		if( ptrActiveView != NULL && ptrActiveObj != NULL )
		{
			::AjustViewForObject( ptrActiveView, ptrActiveObj, avf_Force );
			IUnknownPtr punkView;
			IDocumentSitePtr ptrDocSite = ptrNDTarget;
			if( ptrDocSite != 0)
				ptrDocSite->GetActiveView( &punkView );
			ptrActiveView = punkView;
		}

		FinishNotification();
	}
	else
	{
		if( strFieldName.IsEmpty() )
			return false;

		IDBaseDocumentPtr ptrDB( ptrNDTarget );
		if( ptrDB == NULL )
			return false;


		IUnknown* punkQuery = 0;
		ptrDB->GetActiveQuery( &punkQuery );
		if( !punkQuery )
			return false;

		ISelectQueryPtr ptrQ( punkQuery );
		punkQuery->Release();
		if( ptrQ == NULL )
			return false;

		BOOL bOpen = FALSE;
		ptrQ->IsOpen( &bOpen );
		if( !bOpen )
			return false;

		_bstr_t bstrEntry( SECTION_DBASEFIELDS );
		bstrEntry += "\\";
		bstrEntry += (LPCSTR)strFieldName;


		_variant_t var;
		ptrNDTarget->GetValue( bstrEntry, &var );
		if( !( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH ) )
			return false;


		IUserNameProviderPtr ptrUNP( ptrDB );


		//Determine recod count
		int nObjectCount = 0;
		CString strFieldType = ::GetObjectKind( var.punkVal );
		
		POSITION pos = m_arBaseObject.GetHeadPosition();
		while( pos )
		{
			CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			IUnknownPtr ptrBaseObj = CSendToManager::GetObject( m_guidSourceDoc, pObj->m_guidBaseObject );
			if( strFieldType == ::GetObjectKind(ptrBaseObj) )
				nObjectCount++;
		}

		//Проверка типов - в основе тот же код, что для Insert base objects, только ничего не добавляем - лишь проверяем
		bool bTypesOk = true;
		pos = m_arBaseObject.GetHeadPosition();
		while( pos )
		{
			CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			IUnknownPtr ptrBaseObj = CSendToManager::GetObject( m_guidSourceDoc, pObj->m_guidBaseObject );
			if( ptrBaseObj == NULL || strFieldType != ::GetObjectKind(ptrBaseObj) )
			{
				CString strDBaseObjectName = ::GetObjectName( var.punkVal );

				if( ptrUNP )
				{
					BSTR bstr_user_name = 0;
					ptrUNP->GetUserName( var.punkVal,  &bstr_user_name );
					if( bstr_user_name )
					{
						strDBaseObjectName = bstr_user_name;
						::SysFreeString( bstr_user_name );	bstr_user_name = 0;
					}
				}
				
				CString strStage;
				strStage.Format( IDS_ERROR_DIF_TYPE, 
								GetObjectName(ptrBaseObj), /*strFieldName*/strDBaseObjectName,
								::GetTypeAliase(GetObjectKind(ptrBaseObj)),
								::GetTypeAliase(strFieldType) );
				
				if( !strError.IsEmpty() )
					strError += "\n";

				strError += strStage;

				continue;
			}
		}

		if( !strError.IsEmpty() )
		{
			AfxMessageBox( strError, MB_ICONERROR );
			return false;
		}


		int nCurRecord = 0;
		bool bInsertSomeRecord = (nObjectCount > 1);

		if( !bInsertSomeRecord )
		{
			BOOL bEmpty = false;
			ptrDB->IsDBaseObject( var.punkVal, 0, 0, &bEmpty );
			if( !bEmpty )
			{
				CString strWarning;
				strWarning.LoadString( IDS_WARNING_OBJECT_EXIST );
				if( AfxMessageBox( strWarning, MB_YESNO ) != IDYES )
				{				
					return false;
				}
			}
		}


		StartNotification( nObjectCount, 1, 1 );

		//Insert base objects
		pos = m_arBaseObject.GetHeadPosition();
		while( pos )
		{
			CBaseObject* pObj = (CBaseObject*)m_arBaseObject.GetNext( pos );
			IUnknownPtr ptrBaseObj = CSendToManager::GetObject( m_guidSourceDoc, pObj->m_guidBaseObject );
			if( ptrBaseObj == NULL || strFieldType != ::GetObjectKind(ptrBaseObj) )
			{
				CString strDBaseObjectName = ::GetObjectName( var.punkVal );

				if( ptrUNP )
				{
					BSTR bstr_user_name = 0;
					ptrUNP->GetUserName( var.punkVal,  &bstr_user_name );
					if( bstr_user_name )
					{
						strDBaseObjectName = bstr_user_name;
						::SysFreeString( bstr_user_name );	bstr_user_name = 0;
					}
				}
				
				CString strStage;
				strStage.Format( IDS_ERROR_DIF_TYPE, 
								GetObjectName(ptrBaseObj), /*strFieldName*/strDBaseObjectName,
								::GetTypeAliase(GetObjectKind(ptrBaseObj)),
								::GetTypeAliase(strFieldType) );
				
				if( !strError.IsEmpty() )
					strError += "\n";

				strError += strStage;

				continue;
			}
			
			if( bInsertSomeRecord )
			{
				if( S_OK != ptrQ->Insert() )
				{
					FinishNotification();
					return false;
				}
			}

			nCurRecord++;

			GUID guidBase;
			PutToData( ptrNDTarget, bstrEntry, ptrBaseObj, (LPCSTR)strFieldName, &guidBase );

			POSITION posChild = pObj->m_arGuidChild.GetHeadPosition();
			while( posChild )
			{
				GuidKey* pKey = (GuidKey*)pObj->m_arGuidChild.GetNext( posChild );
				IUnknownPtr ptrChildObj = CSendToManager::GetObject( m_guidSourceDoc, *pKey );
				if( ptrChildObj == NULL )
					continue;
				
				PutToData( ptrNDTarget, bstrEntry, ptrChildObj, NULL, &guidBase );				
			}

			
			if( bInsertSomeRecord )
			{
				CString strObjectName = ::GetObjectName( ptrBaseObj );
				BSTR bstrTable, bstrField;
				bstrTable = bstrField = 0;
				ptrQ->GetTumbnailCaptionLocation( &bstrTable, &bstrField );
				CString strTable = bstrTable;
				CString strField = bstrField;
				::SysFreeString( bstrTable );
				::SysFreeString( bstrField );
				
				if( !strTable.IsEmpty() && !strField.IsEmpty() )
				{
					_bstr_t bstrCaption( SECTION_DBASEFIELDS );
					bstrCaption += "\\";
					bstrCaption += (LPCSTR)strTable;
					bstrCaption += ".";
					bstrCaption += (LPCSTR)strField;

					_variant_t varValue( (LPCSTR)strObjectName );

					ptrNDTarget->SetValue( bstrCaption, varValue );
				}
			}			


			if( bInsertSomeRecord && nCurRecord != nObjectCount )
			{
				if( S_OK != ptrQ->Update() )
				{
					FinishNotification();
					return false;
				}
			}

			Notify( nCurRecord - 1 );
		}


		//Insert NotInGroup
		pos = m_arGuidNotInGroup.GetHeadPosition();
		while( pos )
		{
			GuidKey* pKey = (GuidKey*)m_arGuidNotInGroup.GetNext( pos );
			IUnknownPtr ptrObj = CSendToManager::GetObject( m_guidSourceDoc, *pKey );
			if( ptrObj == NULL )
				continue;

			PutToData( ptrNDTarget, bstrEntry, ptrObj );			
		}

		FinishNotification();


		if( !strError.IsEmpty() )
		{
			AfxMessageBox( strError, MB_ICONERROR );
		}

	}

	

	CWnd *pWnd = (CWnd *)::GetWindowFromUnknown( ptrActiveView );
	if( pWnd )
		pWnd = pWnd->GetParent();

	if( pWnd )
	{
		int nIsActivate = ::GetValueInt( GetAppUnknown(), "\\SendToManager", "ActivateAfter", 0 );
		if( !strError.IsEmpty() ) nIsActivate = false; // BT 4384

		if( nIsActivate )
		{
			CString Str;
			
			CWnd *pParent = pWnd->GetParent();
			if( pParent )
			{
				CWnd *ppParent = pParent->GetParent();
				if( ppParent )
				{
					ppParent->GetWindowText( Str );
					ExecuteAction( "WindowActivate", CString( "\"" )+Str+CString( "\"" ) );
				}
			}
		}
	}

	return true;
}


//If parent not found kill parent ptr in child...
void CActionSendTo::SafeCorrectParent( IUnknown* punkNamedData, IUnknown* punkObj )
{

	INamedDataObject2Ptr ptrNDO2( punkObj );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
		{
			IUnknownPtr ptrParent = punkParent;
			punkParent->Release();	punkParent = 0;
			
			
			GuidKey guidParent = ::GetObjectKey( ptrParent );
			IUnknown* punkTest = 0;
			punkTest = GetObjectByKey( punkNamedData, guidParent );
			if( punkTest )
			{
				punkTest->Release(); punkTest = 0;
			}
			else//Sorry, parent not found
			{
				ptrNDO2->SetParent( NULL, 0 );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
bool CActionSendTo::PutToData( IUnknown* punkND, _bstr_t bstrName, IUnknown* punkObject, const char* psz_obj_name, GUID *pGuidBase)
{
	INamedDataPtr ptrND( punkND );
	if( ptrND == NULL ) 
		return false;

	IUnknown* punkClone = NULL;
	punkClone = ::CloneObject( punkObject );
	if( punkClone == NULL )
		return false;


	INumeredObjectPtr ptrNumObj( punkClone );
	if( ptrNumObj )
		ptrNumObj->GenerateNewKey( 0 );


	SafeCorrectParent( punkND, punkClone );

	if( psz_obj_name )
	{
		GUID guidKey = INVALID_KEY;
		if( ptrNumObj )
			ptrNumObj->GetKey( &guidKey );

		INamedDataObject2Ptr ptrNDO( punkClone );
		if( ptrNDO )
			ptrNDO->SetBaseKey( &guidKey );
		if( pGuidBase )
			*pGuidBase = guidKey;

		SetObjectName( punkClone, psz_obj_name );
		
	}
	else if( pGuidBase )
	{
		INamedDataObject2Ptr ptrNDO( punkClone );
		if( ptrNDO )
			ptrNDO->SetBaseKey( pGuidBase );
	}
	ptrND->SetValue( bstrName, _variant_t( (IUnknown*)punkClone ) );
	punkClone->Release();


	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::IsPutToDocument()
{
	INamedDataPtr ptrNDTarget = CSendToManager::GetDocumentByKey( m_guidTargetDoc );
	if( ptrNDTarget == NULL )
		return true;

	if( CheckInterface( ptrNDTarget, IID_IDBaseDocument ) && !m_bPutToDocument )
		return false;

	return true;	
}


//////////////////////////////////////////////////////////////////////
IUnknownPtr CActionSendTo::PutToDocument( IUnknown* punkObjectSrc )
{
	INamedDataPtr ptrNDTarget = CSendToManager::GetDocumentByKey( m_guidTargetDoc );
	if( ptrNDTarget == NULL )
		return 0;

	m_lTargetKey = ::GetObjectKey( ptrNDTarget );

	IUnknown* punkClone = NULL;
	punkClone = ::CloneObject( punkObjectSrc );
	if( punkClone == NULL )
		return 0;

	INumeredObjectPtr ptrNumObj( punkClone );
	if( ptrNumObj )
		ptrNumObj->GenerateNewKey( 0 );

	SafeCorrectParent( ptrNDTarget, punkClone );

	IUnknownPtr ptrObj = punkClone;
	punkClone->Release();


	INumeredObjectPtr	ptrNum( ptrObj );
	if( ptrNum )
		ptrNum->GenerateNewKey( 0 );


	INamedObject2Ptr ptrNamed( ptrObj );
	if( ptrNamed )
	{
		BSTR bstrName = 0;
		ptrNamed->GetName( &bstrName );
		if( bstrName )
		{
			INamedDataPtr ptrNamedData( ptrNDTarget );
			if( ptrNamedData )
			{
				long lNameExist = 0;
				ptrNamedData->NameExists( bstrName, &lNameExist );
				if( lNameExist )
				{
					::SetObjectName( ptrObj, 0 );

				}
			}
			::SysFreeString( bstrName );	bstrName = 0;
		}
	}

	m_changes.SetToDocData( ptrNDTarget, ptrObj );	

	return ptrObj;
}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::DoUndo()
{
	if( IsPutToDocument() )
		m_changes.DoUndo( m_punkTarget );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionSendTo::DoRedo()
{
	if( IsPutToDocument() )
		m_changes.DoRedo( m_punkTarget );

	return true;
}

