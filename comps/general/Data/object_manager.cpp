// object_manager.cpp : implementation file
//

#include "stdafx.h"
#include "data.h"
#include "object_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// object_manager

IMPLEMENT_DYNCREATE(object_manager, CCmdTarget)

object_manager::object_manager()
{
	EnableAutomation();
	EnableAggregation();	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	Register( ::GetAppUnknown() );	

	m_sName = m_sUserName = "ObjectManager";
}

object_manager::~object_manager()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.

	_free_file_filters_list();

	UnRegister( ::GetAppUnknown() );
	_free_type_list();

	VERIFY( m_listDocs.GetCount() == 0 );
	_free_doc_list();
	
	AfxOleUnlockApp();
}


void object_manager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(object_manager, CCmdTarget)
	//{{AFX_MSG_MAP(object_manager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(object_manager, CCmdTarget)
	//{{AFX_DISPATCH_MAP(object_manager)
	DISP_FUNCTION(object_manager, "GetFirstObjectTypePos", GetFirstObjectTypePos, VT_I4, VTS_NONE)
	DISP_FUNCTION(object_manager, "GetNextObjectTypePos", GetNextObjectTypePos, VT_I4, VTS_I4)
	DISP_FUNCTION(object_manager, "GetObjectType", GetObjectType, VT_BSTR, VTS_I4)
	DISP_FUNCTION(object_manager, "GetNextObjectPos", GetNextObjectPos, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(object_manager, "GetFirstObjectPos", GetFirstObjectPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION(object_manager, "GetObjectName", GetObjectName, VT_BSTR, VTS_BSTR VTS_I4)
	DISP_FUNCTION(object_manager, "GetActiveObjectName", GetActiveObjectName, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(object_manager, "SetActiveObjectByName", SetActiveObjectByName, VT_BOOL, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(object_manager, "GetActiveObject", GetActiveObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(object_manager, "SetActiveObject", SetActiveObject, VT_BOOL, VTS_DISPATCH)
	DISP_FUNCTION(object_manager, "GetObjectByName", GetObjectByName, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(object_manager, "IsObjectInDocuments", IsObjectInDocuments, VT_BOOL, VTS_BSTR VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Iobject_manager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {46A13E4D-6738-4FB1-8BC3-BAEEB5B5D3AE}
static const IID IID_IObject_managerDisp =
{ 0x46a13e4d, 0x6738, 0x4fb1, { 0x8b, 0xc3, 0xba, 0xee, 0xb5, 0xb5, 0xd3, 0xae } };

BEGIN_INTERFACE_MAP(object_manager, CCmdTarget)
	INTERFACE_PART(object_manager, IID_IObject_managerDisp, Dispatch)
	INTERFACE_PART(object_manager, IID_IEventListener, EvList)
	INTERFACE_PART(object_manager, IID_IObjectManager, ObjectManager)
	INTERFACE_PART(object_manager, IID_INamedObject2, Name)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(object_manager, ObjectManager);

// {B0097FB7-FBA0-46A8-8FA6-C4865E9136F9}
GUARD_IMPLEMENT_OLECREATE(object_manager, "Data.ObjectManager", 0xb0097fb7, 0xfba0, 0x46a8, 0x8f, 0xa6, 0xc4, 0x86, 0x5e, 0x91, 0x36, 0xf9)



/////////////////////////////////////////////////////////////////////////////
bool object_manager::append_type_list_from_dirs()
{
	_try(object_manager, append_type_list_from_dirs)
	{

		INamedDataPtr ptrND( ::GetAppUnknown() );	
		if( ptrND == 0 )
			return false;

		INamedData* pINamedData = ptrND;

		if( S_OK != pINamedData->SetupSection( _bstr_t( szObjManSection ) ) )
			return false;

		CStringArray arPath;

		long lCount = 0;
		pINamedData->GetEntriesCount( &lCount );
		for( int i = 0;i<(int)lCount;i++ )
		{
			BSTR bstr = NULL;
			if( S_OK != pINamedData->GetEntryName( i, &bstr ) )
				continue;

			CString strEntryName = bstr;
			::SysFreeString( bstr );	bstr = 0;

			if( strEntryName.Find( szObjManPathPrefix ) == -1 )
				continue;

			_bstr_t bstrEntryPath = _bstr_t( szObjManSection ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntryName );
			
			_variant_t var;
			if( S_OK != pINamedData->GetValue( bstrEntryPath, &var ) )
				continue;

			if( var.vt != VT_BSTR )
				continue;

			arPath.Add( var.bstrVal );		
		}

		if( arPath.GetSize() < 1 )
			return true;		


		for( i=0;i<arPath.GetSize();i++ )
		{
			CFileFind finder;
			BOOL bWorking = finder.FindFile( arPath[i] + "\\*.*" );

			while( bWorking )
			{
				bWorking = finder.FindNextFile();
				if( finder.IsDirectory() || finder.IsDots() )
					continue;

				CString strFileName		= finder.GetFilePath();
				CString strExt			= _get_file_name_ext( strFileName );
				CFileFilterCache* pff	= _find_filter_by_ext( strExt );

				CString strType;
				if( pff )
					strType = pff->m_strType;

				if( strType.IsEmpty() )
				{

					CFileFilterCache* pff_new = _create_file_filter_item( strFileName );
					if( !pff_new )
						continue;

					strType = pff_new->m_strType;

				}

				CTypeInfo* pti = _find_type_info( (LPCSTR)strType );
				if( !pti )
					pti = _add_type_info( strType );

				CObjectInfo* poi = new CObjectInfo;				
				poi->m_strFileName	= strFileName;				

				pti->m_listObj.AddHead( poi );
			}
		}

	}
	_catch
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::append_type_list_from_doc( IUnknown* punkDoc )
{

	IDataContext2Ptr ptrDC( punkDoc );
	if( ptrDC == 0 )
		return false;

	long lTypeCount = 0;
	ptrDC->GetObjectTypeCount( &lTypeCount );
	for( long i=0;i<lTypeCount;i++ )
	{
		BSTR bstrType = 0;
		ptrDC->GetObjectTypeName( i, &bstrType );

		CString strType = bstrType;		

		CTypeInfo* pti = _find_type_info( strType );
		if( !pti )
		{
			pti = new CTypeInfo;
			pti->m_strTypeName = strType;
			m_listType.AddHead( pti );
		}

		long lObjCount = 0;
		ptrDC->GetObjectCount( bstrType, &lObjCount );

		long lPos = 0;
		ptrDC->GetFirstObjectPos( bstrType, &lPos );
		
		while( lPos )
		{

			IUnknown* punk = 0;
			ptrDC->GetNextObject( bstrType, &lPos, &punk );
			if( !punk )
				continue;

			CObjectInfo* poi = new CObjectInfo;
			pti->m_listObj.AddHead( poi );

			poi->m_ptrDoc = punkDoc;
			poi->m_ptrObj = punk;

			punk->Release();
		}
		

		::SysFreeString( bstrType );	bstrType = 0;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::remove_type_list_by_doc( IUnknown* punkDoc )
{
	GUID guidDoc = ::GetObjectKey( punkDoc );

	POSITION pos_ti = m_listType.GetHeadPosition();
	while( pos_ti )
	{
		CTypeInfo* pti = (CTypeInfo*)m_listType.GetNext( pos_ti );
		POSITION pos_oi = pti->m_listObj.GetHeadPosition();
		while( pos_oi )
		{
			POSITION pos_save = pos_oi;
			CObjectInfo* poi = (CObjectInfo*)pti->m_listObj.GetNext( pos_oi );
			if( poi->m_ptrDoc == 0 )
				continue;

			if( guidDoc == ::GetObjectKey( poi->m_ptrDoc ) )
			{
				delete poi;
				pti->m_listObj.RemoveAt( pos_save );
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
void object_manager::_free_type_list()
{
	POSITION pos = m_listType.GetHeadPosition();
	while( pos )
	{
		 CTypeInfo* p = (CTypeInfo*)m_listType.GetNext( pos );
		 delete p;
	}

	m_listType.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void object_manager::_free_doc_list()
{
	POSITION pos = m_listDocs.GetHeadPosition();
	while( pos )
	{
		 CDocumentInfo* p = (CDocumentInfo*)m_listDocs.GetNext( pos );
		 delete p;
	}

	m_listDocs.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void object_manager::_free_file_filters_list()
{
	POSITION pos = m_listFileFilters.GetHeadPosition();
	while( pos )
	{
		 CFileFilterCache* p = (CFileFilterCache*)m_listFileFilters.GetNext( pos );
		 delete p;
	}

	m_listFileFilters.RemoveAll();	
}

/////////////////////////////////////////////////////////////////////////////
CTypeInfo* object_manager::_add_type_info( const char* pszTypeInfoName )
{
	CTypeInfo* pti = new CTypeInfo;
	pti->m_strTypeName = pszTypeInfoName;
	m_listType.AddHead( pti );

	return pti;
}


/////////////////////////////////////////////////////////////////////////////
CTypeInfo* object_manager::_find_type_info( const char* pszTypeInfoName )
{
	POSITION pos = m_listType.GetHeadPosition();
	while( pos )
	{
		CTypeInfo* pti =(CTypeInfo*)m_listType.GetNext( pos );
		if( pti->m_strTypeName == pszTypeInfoName )
			return pti;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CObjectInfo* object_manager::_find_active_object_info( const char* pszTypeInfoName )
{
	CTypeInfo* pti = _find_type_info( pszTypeInfoName );
	if( !pti )
		return 0;

	POSITION pos = pti->m_listObj.GetHeadPosition();
	while( pos )
	{
		CObjectInfo* poi =(CObjectInfo*)pti->m_listObj.GetNext( pos );
		if( poi->m_bActive )
			return poi;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CObjectInfo* object_manager::_find_object_info( IUnknown* punkObj, CTypeInfo** ppti, POSITION* ppos )
{
	if( !punkObj )
		return 0;

	if( ppos )
		*ppos = 0;

	CString strType = ::GetObjectKind( punkObj );

	CTypeInfo* pti = _find_type_info( strType );
	if( !pti )
	{		
		return 0;
	}

	if( ppti )
		*ppti = pti;

	POSITION pos = pti->m_listObj.GetHeadPosition();
	while( pos )
	{
		POSITION pos_save = pos;
		CObjectInfo* poi =(CObjectInfo*)pti->m_listObj.GetNext( pos );

		if( poi->m_ptrObj != 0 && ( ::GetObjectKey( poi->m_ptrObj ) == ::GetObjectKey( punkObj ) ) )
		{
			if( ppos )
				*ppos = pos_save;

			return poi;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CObjectInfo* object_manager::_find_object_info_by_name( const char* pszTypeName, const char* pszObjectName )
{
	CTypeInfo* pti = _find_type_info( pszTypeName );
	if( !pti ) 
		return 0;

	POSITION pos = m_listType.GetHeadPosition();
	while( pos )
	{
		CObjectInfo* poi = (CObjectInfo*)m_listType.GetNext( pos );
		CString strName = get_object_name( poi );
		if( strName == pszObjectName )
			return poi;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CString object_manager::get_object_name( CObjectInfo* poi, bool bForceLoad )
{
	if( !poi )
		return "";

	if( poi->m_ptrDoc )
	{
		return ::GetObjectName( poi->m_ptrObj );	
	}
	else
	{
		if( bForceLoad )
			load_object( poi, false );

		if( poi->m_ptrObj )
			return ::GetObjectName( poi->m_ptrObj );		

		int nIdxPoint = poi->m_strFileName.ReverseFind( '.' );
		int nIdxSplash = poi->m_strFileName.ReverseFind( '\\' );

		if( nIdxPoint == -1 || nIdxSplash == -1 || nIdxPoint <= nIdxSplash )
			return "";

		CString str = poi->m_strFileName.Mid( nIdxSplash + 1, nIdxPoint - nIdxSplash - 1 );

		return str;
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::load_object( CObjectInfo* poi, bool bForceReloadLoad )
{
	_try(object_manager, load_object)
	{
		if( poi->m_ptrDoc )
		{
			return true;
		}
		else
		{
			if( bForceReloadLoad )
				poi->m_ptrObj = 0;

			if( poi->m_ptrObj )
				return true;

			CString strExt = _get_file_name_ext( poi->m_strFileName );

			CFileFilterCache* pff = _find_filter_by_ext( strExt );
			if( !pff )
			{
				pff = _create_file_filter_item( poi->m_strFileName );
			}			

			if( !pff )
				return false;

			if( pff->m_ptrFilter == 0 )
				return false;

			IFileDataObjectPtr ptrFile( pff->m_ptrFilter );
			if( ptrFile == 0 )
				return false;

			ptrFile->LoadFile( _bstr_t( poi->m_strFileName ) );

			int nCount = 0;
			pff->m_ptrFilter->GetCreatedObjectCount( &nCount );

			return true;
		}
	}
	_catch
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString object_manager::_get_file_name_ext( const char* pszFileName )
{
	CString strFileName = pszFileName;
	int nIdx = strFileName.ReverseFind( '.' );
	if( nIdx == -1 )
		return "";

	CString strExt = strFileName.Right( strFileName.GetLength() - nIdx - 1 );				
	strExt.MakeLower();

	return strExt;
}

/////////////////////////////////////////////////////////////////////////////
CFileFilterCache* object_manager::_find_filter_by_ext( const char* pszExt )
{
	CString strExt = pszExt;
	CString strType;
	POSITION pos = m_listFileFilters.GetHeadPosition();
	while( pos )
	{
		CFileFilterCache* pff = (CFileFilterCache*)m_listFileFilters.GetNext( pos );
		if( pff->m_strExtension == strExt )
			return pff;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CFileFilterCache* object_manager::_create_file_filter_item( const char* pszFileName )
{
	CString strExt = _get_file_name_ext( pszFileName );
	if( strExt.IsEmpty() )
		return 0;

	strExt.MakeLower();

	IUnknown* punk = ::GetFilterByFile( pszFileName );
	if( !punk )
		return 0;

	IFileFilter2Ptr ptrFilter = punk;
	punk->Release();	punk = 0;

	if( ptrFilter == 0 )
		return 0;
	
	int nTypeCount = 0;
	ptrFilter->GetObjectTypesCount( &nTypeCount );
	if( nTypeCount < 1 )
		return 0;

	CString strType;
	BSTR bstrType = 0;
	ptrFilter->GetObjectType( 0, &bstrType );

	strType = bstrType;
	::SysFreeString( bstrType );	bstrType = 0;

	if( strType.IsEmpty() )
		return 0;

	CFileFilterCache* pnew = new CFileFilterCache;

	pnew->m_ptrFilter		= ptrFilter;
	pnew->m_strExtension	= strExt;	
	pnew->m_strType			= strType;

	m_listFileFilters.AddHead( pnew );

	return pnew;
}

/////////////////////////////////////////////////////////////////////////////
void object_manager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	/*
	TRACE( "=--------------------------------------------------" );
	TRACE( pszEvent );
	TRACE( "\n" );
	*/

	if( !strcmp(pszEvent, "AppInit") )
	{
		append_type_list_from_dirs();
	}
	else if( !strcmp(pszEvent, szAppActivateView) )
	{	
		if( punkFrom && punkHit )
		{
			VERIFY( CheckInterface( punkFrom, IID_IView ) );
			VERIFY( CheckInterface( punkHit, IID_IDocument ) );
			_on_activate_view( punkHit, punkFrom );
		}
	}
	else if( !strcmp(pszEvent, "AfterClose"/*szEventBeforeClose*/) )
	{
		VERIFY( CheckInterface( punkFrom, IID_IDocument ) );
		_on_close_document( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		VERIFY( CheckInterface( punkFrom, IID_IView ) );
		_on_destroy_view( punkFrom );
	}
	else if( !strcmp( pszEvent, szEventActiveContextChange ) )
	{		
		if( cbSize == sizeof( NotifyCodes ) )
		{
			if( CheckInterface( punkFrom, IID_IView ) )
			{
				NotifyCodes* pnc = (NotifyCodes*)(pdata);
				_on_data_context_change( punkFrom, punkHit, *pnc );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//notifications
bool object_manager::_on_activate_view( IUnknown* punkDoc, IUnknown* punkView )
{
	if( !CheckInterface( punkDoc, IID_IDocument ) )
		return false;

	if( !CheckInterface( punkView, IID_IView ) )
		return false;


	CDocumentInfo*	pdocinfo = _find_doc_info( punkDoc );
	if( pdocinfo )
	{
		//process view
		if( pdocinfo->m_ptrView )
		{
			if( GetObjectKey( pdocinfo->m_ptrView ) != ::GetObjectKey( punkView ) )
			{
				UnRegister( pdocinfo->m_ptrView );

				pdocinfo->m_ptrView = punkView;

				Register( punkView );
			}
		}		
		else
		{
			pdocinfo->m_ptrView = punkView;
			Register( punkView );
		}
	}
	else
	{
		CDocumentInfo*	pdocinfo = new CDocumentInfo;

		m_listDocs.AddHead( pdocinfo );

		pdocinfo->m_ptrDocument	= punkDoc;
		pdocinfo->m_ptrView		= punkView;

		Register( punkDoc );
		Register( punkView );

		append_type_list_from_doc( punkDoc );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::_on_close_document( IUnknown* punkDoc )
{
	if( !CheckInterface( punkDoc, IID_IDocument ) )
		return false;

	POSITION pos = 0;

	CDocumentInfo*	pdocinfo = _find_doc_info( punkDoc, &pos );
	if( !pdocinfo )
	{
		ASSERT( false );
		return false;
	}

	if( pdocinfo->m_ptrView )
	{
		UnRegister( pdocinfo->m_ptrView );
		pdocinfo->m_ptrView = 0;
	}

	remove_type_list_by_doc( punkDoc );

	if( pdocinfo->m_ptrDocument )
	{
		UnRegister( pdocinfo->m_ptrDocument );
		pdocinfo->m_ptrDocument = 0;
	}

	delete pdocinfo;

	m_listDocs.RemoveAt( pos );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::_on_destroy_view( IUnknown* punkView )
{
	if( !CheckInterface( punkView, IID_IView ) )
		return false;

	IViewPtr ptrView = punkView;
	if( ptrView == 0 )
		return false;

	IUnknown* punkDoc =  0;
	ptrView->GetDocument( &punkDoc );
	if( !punkDoc )
		return false;

	IUnknownPtr ptrDoc = punkDoc;
	punkDoc->Release();	punkDoc = 0;

	CDocumentInfo*	pdocinfo = _find_doc_info( ptrDoc );
	if( !pdocinfo )
	{
		ASSERT( false );
		return false;
	}

	if( pdocinfo->m_ptrView )
	{
		if( ::GetObjectKey( pdocinfo->m_ptrView ) == ::GetObjectKey( punkView ) )
		{
			UnRegister( pdocinfo->m_ptrView );
			pdocinfo->m_ptrView = 0;
		}
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool object_manager::_on_data_context_change( IUnknown* punkView, IUnknown* punkObj, NotifyCodes nc )
{
	if( !punkView || !punkObj )
		return false;

	IViewPtr ptrView( punkView );
	if( ptrView ==0 )
		return false;

	IUnknownPtr ptrDoc;	
	{
		IUnknown* punk = 0;
		ptrView->GetDocument( &punk );
		if( !punk )
			return false;

		ptrDoc = punk;
		punk->Release();	punk = 0;
	}


	if( nc == ncAddObject )
	{		
		{
			CObjectInfo* poi = _find_object_info( punkObj );
			if( poi )
			{
				ASSERT( FALSE );
				return false;
			}
		}

		CString strObjectType = ::GetObjectKind( punkObj );

		CTypeInfo* pti = _find_type_info( strObjectType );
		if( !pti )
			pti = _add_type_info( strObjectType );

		CObjectInfo* poi = new CObjectInfo; 
		pti->m_listObj.AddHead( poi );
		poi->m_ptrDoc = ptrDoc;
		poi->m_ptrObj = punkObj;

	}
	else if( nc == ncRemoveObject )
	{
		CTypeInfo* pti = 0;
		POSITION pos = 0;
		CObjectInfo* poi = _find_object_info( punkObj, &pti, &pos );
		if( !poi )
		{
			ASSERT( FALSE );
			return false;
		}

		delete poi;	poi = 0;
		pti->m_listObj.RemoveAt( pos );


	}
	else if( nc == ncReplaceObject )
	{
		ASSERT( false );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
CDocumentInfo* object_manager::_find_doc_info( IUnknown* punkDoc, POSITION* ppos )
{
	if( ppos )
		*ppos = 0;

	if( !CheckInterface( punkDoc, IID_IDocument ) )
		return false;

	POSITION pos = m_listDocs.GetHeadPosition();
	while( pos )
	{
		POSITION pos_save = pos;
		CDocumentInfo* p = (CDocumentInfo*)m_listDocs.GetNext( pos );
		if( p->m_ptrDocument == 0 )
			continue;

		if( ::GetObjectKey( p->m_ptrDocument ) == ::GetObjectKey( punkDoc ) )
		{
			if( ppos )
				*ppos = pos_save;

			return p;
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
IUnknown* object_manager::get_active_document()
{
	IUnknown* punkView = NULL;
	punkView = get_active_view();
	if( punkView == NULL )
		return NULL;

	IViewPtr ptrView(punkView);
	punkView->Release();
	if( ptrView == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	ptrView->GetDocument( &punkDoc );

	return punkDoc;		
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* object_manager::get_active_view()
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	
	if( punkDoc == NULL )
		return NULL;
	

	sptrIDocumentSite sptrDS( punkDoc );
	punkDoc->Release();
	if( sptrDS == NULL )
		return NULL;

	IUnknown* punkView = NULL;
	sptrDS->GetActiveView( &punkView );

	return punkView;

}


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	IObjectManager interface part
//
//
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetFirstObjectTypePos( long* plPos )
{
	_try_nested(object_manager, ObjectManager, GetFirstObjectTypePos )
	{	
		if( !plPos )
			return E_INVALIDARG;

		*plPos = (long)pThis->m_listType.GetHeadPosition();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetNextObjectType( long* plPos, BSTR* pbstrTypeName )
{
	_try_nested(object_manager, ObjectManager, GetNextObjectType )
	{	
		if( !plPos )
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;

		CTypeInfo* pti = (CTypeInfo*)pThis->m_listType.GetNext( pos );

		if( pbstrTypeName )
			*pbstrTypeName = pti->m_strTypeName.AllocSysString();

		 *plPos = (long)pos;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetFirstObjectNamePos( BSTR bstrType, long * plPos )
{
	_try_nested(object_manager, ObjectManager, GetFirstObjectNamePos )
	{	
		if( !plPos )
			return E_INVALIDARG;

		CTypeInfo* pti = pThis->_find_type_info( (char*)_bstr_t( bstrType ) );
		if( !pti )
			return S_FALSE;

		*plPos = (long)pti->m_listObj.GetHeadPosition();		
		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetNextObjectByName( BSTR bstrType, long * plPos, BSTR* pbstrName )
{
	_try_nested(object_manager, ObjectManager, GetNextObjectByName )
	{	
		if( !plPos || !plPos  )
			return E_INVALIDARG;

		CTypeInfo* pti = pThis->_find_type_info( (char*)_bstr_t( bstrType ) );
		if( !pti )
			return S_FALSE;

		POSITION pos = (POSITION)*plPos;

		
		CObjectInfo* poi = (CObjectInfo*)pti->m_listObj.GetNext( pos );

		if( pbstrName )
		{
			CString strName = pThis->get_object_name( poi );
			*pbstrName = strName.AllocSysString();
		}

		*plPos = (long)pos;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetFirstObjectPos( BSTR bstrType, long * plPos )
{
	_try_nested(object_manager, ObjectManager, GetFirstObjectPos )
	{	
		return GetFirstObjectNamePos( bstrType, plPos );
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetNextObject( BSTR bstrType, long * plPos, IUnknown** ppunkObj )
{
	_try_nested(object_manager, ObjectManager, GetNextObject )
	{	
		if( !plPos || !plPos || !ppunkObj )
			return E_INVALIDARG;

		CTypeInfo* pti = pThis->_find_type_info( (char*)_bstr_t( bstrType ) );
		if( !pti )
			return S_FALSE;

		POSITION pos = (POSITION)*plPos;

		CObjectInfo* poi = (CObjectInfo*)pti->m_listObj.GetNext( pos );

		*plPos = (long)pos;

		if( poi->m_ptrObj == 0 )//not load yet
			pThis->load_object( poi );

		*ppunkObj = poi->m_ptrObj;
		if( poi->m_ptrObj )
			poi->m_ptrObj->AddRef();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetActiveObjectName( BSTR bstrType, BSTR* pbstrName )
{
	_try_nested(object_manager, ObjectManager, GetActiveObjectName )
	{	
		if( !pbstrName )
			return E_INVALIDARG;

		*pbstrName = 0;

		CObjectInfo* poi = pThis->_find_active_object_info( _bstr_t( bstrType ) );
		if( !poi )
			return E_FAIL;

		*pbstrName = pThis->get_object_name( poi ).AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::SetActiveObjectByName( BSTR bstrType, BSTR bstrName )
{
	_try_nested(object_manager, ObjectManager, SetActiveObjectByName )
	{	
		CObjectInfo* poi_old = pThis->_find_active_object_info( _bstr_t( bstrType ) );
		CObjectInfo* poi_new = pThis->_find_object_info_by_name( _bstr_t( bstrType ), _bstr_t( bstrName ) );

		if( !poi_new )
			return E_FAIL;

		if( poi_old )
			poi_old->m_bActive = false;

		poi_new->m_bActive = true;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetActiveObject( BSTR bstrType, IUnknown** ppunkObj )
{
	_try_nested(object_manager, ObjectManager, GetActiveObject )
	{	
		if( !ppunkObj )
			return E_INVALIDARG;

		CObjectInfo* poi = pThis->_find_active_object_info( _bstr_t( bstrType ) );

		if( poi->m_ptrObj == 0 )//not load yet
			pThis->load_object( poi );
		
		*ppunkObj = poi->m_ptrObj;
		if( poi->m_ptrObj )
			poi->m_ptrObj->AddRef();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::SetActiveObject( IUnknown* punkObj )
{
	_try_nested(object_manager, ObjectManager, SetActiveObject )
	{	
		CString strType = ::GetObjectKind( punkObj );
		CObjectInfo* poi_old = pThis->_find_active_object_info( _bstr_t( strType ) );
		CObjectInfo* poi_new = pThis->_find_object_info( punkObj );

		if( !poi_new )
			return E_FAIL;

		if( poi_old )
			poi_old->m_bActive = false;

		poi_new->m_bActive = true;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::FreeLoadedObject( BSTR bstrType, BSTR bstrName )
{
	_try_nested(object_manager, ObjectManager, FreeLoadedObject )
	{	
		CObjectInfo* poi = pThis->_find_object_info_by_name( _bstr_t( bstrType ), _bstr_t( bstrName ) );
		if( !poi )
			return E_FAIL;

		if( poi->m_ptrDoc )//do not load ptr if object in doc
			return S_OK;

		poi->m_ptrObj = 0;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetObject( BSTR bstrType, BSTR bstrName, IUnknown** ppunkObj )
{
	_try_nested(object_manager, ObjectManager, GetObject )
	{	
		if( !ppunkObj )
			return E_INVALIDARG;

		CObjectInfo* poi = pThis->_find_object_info_by_name( _bstr_t( bstrType ), _bstr_t( bstrName ) );
		if( !poi )
			return E_FAIL;

		if( poi->m_ptrObj == 0 )
			pThis->load_object( poi );

		*ppunkObj = poi->m_ptrObj;
		if( poi->m_ptrObj )
			poi->m_ptrObj->AddRef();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT object_manager::XObjectManager::GetObjectDocument( BSTR bstrType, BSTR bstrName, IUnknown** ppunkDoc )
{
	_try_nested(object_manager, ObjectManager, GetObjectDocument )
	{	
		if( !ppunkDoc )
			return E_INVALIDARG;

		CObjectInfo* poi = pThis->_find_object_info_by_name( _bstr_t( bstrType ), _bstr_t( bstrName ) );
		if( !poi )
			return E_FAIL;

		*ppunkDoc = poi->m_ptrDoc;
		if( poi->m_ptrDoc )
			poi->m_ptrDoc->AddRef();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
long object_manager::GetFirstObjectTypePos() 
{
	long lPos = 0;
	m_xObjectManager.GetFirstObjectTypePos( &lPos );
	return lPos;
}

////////////////////////////////////////////////////////////////////////////////////////////
long object_manager::GetNextObjectTypePos(long lPos) 
{
	m_xObjectManager.GetNextObjectType( &lPos, 0 );
	return lPos;
}

////////////////////////////////////////////////////////////////////////////////////////////
BSTR object_manager::GetObjectType(long lPos) 
{
	BSTR bstr = 0;
	m_xObjectManager.GetNextObjectType( &lPos, &bstr );
	return bstr;
}

////////////////////////////////////////////////////////////////////////////////////////////
long object_manager::GetFirstObjectPos(LPCTSTR pszType) 
{
	long lPos = 0;
	m_xObjectManager.GetFirstObjectNamePos( _bstr_t( pszType ), &lPos );

	return lPos;
}

////////////////////////////////////////////////////////////////////////////////////////////
long object_manager::GetNextObjectPos(LPCTSTR pszType, long lPos) 
{	
	m_xObjectManager.GetNextObjectByName( _bstr_t( pszType ), &lPos, 0 );
	return lPos;
}

////////////////////////////////////////////////////////////////////////////////////////////
BSTR object_manager::GetObjectName(LPCTSTR pszType, long lPos) 
{
	BSTR bstr = 0;
	m_xObjectManager.GetNextObjectByName( _bstr_t(pszType), &lPos, &bstr );
	return bstr;
}

////////////////////////////////////////////////////////////////////////////////////////////
BSTR object_manager::GetActiveObjectName(LPCTSTR pszType) 
{
	BSTR bstr = 0;
	m_xObjectManager.GetActiveObjectName( _bstr_t( pszType ), &bstr );
	return bstr;
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL object_manager::SetActiveObjectByName(LPCTSTR pszType, LPCTSTR pszName) 
{	
	return ( S_OK == m_xObjectManager.SetActiveObjectByName( _bstr_t( pszType ), _bstr_t( pszName ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////
LPDISPATCH object_manager::GetActiveObject(LPCTSTR pszType) 
{
	IUnknown* punk = 0;
	m_xObjectManager.GetActiveObject( _bstr_t( pszType ), &punk );
	if( !punk )
		return 0;

	IDispatchPtr ptrDisp( punk );
	punk->Release();	punk = 0;

	if( ptrDisp == 0 )
		return false;

	ptrDisp->AddRef();

	return ptrDisp;
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL object_manager::SetActiveObject(LPDISPATCH pIDispatch) 
{
	return ( S_OK == m_xObjectManager.SetActiveObject( pIDispatch ) );
}

////////////////////////////////////////////////////////////////////////////////////////////
LPDISPATCH object_manager::GetObjectByName(LPCTSTR pszType, LPCTSTR pszName) 
{
	IUnknown* punk = 0;
	m_xObjectManager.GetObject( _bstr_t( pszType ), _bstr_t( pszName ), &punk );
	if( !punk )
		return 0;

	IDispatchPtr ptrDisp( punk );
	punk->Release();	punk = 0;

	if( ptrDisp == 0 )
		return false;

	ptrDisp->AddRef();

	return ptrDisp;
}

////////////////////////////////////////////////////////////////////////////////////////////
BOOL object_manager::IsObjectInDocuments(LPCTSTR pszType, LPCTSTR pszName) 
{
	IUnknown* punk = 0;
	m_xObjectManager.GetObjectDocument( _bstr_t( pszType ), _bstr_t( pszName ), &punk );

	BOOL bRes = ( punk != 0 );

	if( punk )
		punk->Release();

	return bRes;
}
