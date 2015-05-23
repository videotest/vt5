// Albom.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "Albom.h"
#include "constant.h"
#include "utils.h"

#include "OleData.h"





/////////////////////////////////////////////////////////////////////////////
// CAlbomServerFactory

IMPLEMENT_DYNCREATE(CAlbomServerFactory, CCmdTarget)

CAlbomServerFactory::CAlbomServerFactory()
{
	EnableAutomation();
	EnableAggregation();
	
	_OleLockApp( this );
	
	m_pCF = 0;
	m_dwCF = 0;

	
}

CAlbomServerFactory::~CAlbomServerFactory()
{
	if( m_pCF )
	{
		delete m_pCF;
		m_pCF = 0;
	}

	_OleUnlockApp( this );
	
}


void CAlbomServerFactory::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();	
}


BEGIN_MESSAGE_MAP(CAlbomServerFactory, CCmdTarget)
	//{{AFX_MSG_MAP(CAlbomServerFactory)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAlbomServerFactory, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CAlbomServerFactory)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAlbomFactory to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2838997B-3E4F-4130-947F-9B0BFBE83CA5}
static const IID IID_IAlbomFactory =
{ 0x2838997b, 0x3e4f, 0x4130, { 0x94, 0x7f, 0x9b, 0xb, 0xfb, 0xe8, 0x3c, 0xa5 } };

BEGIN_INTERFACE_MAP(CAlbomServerFactory, CCmdTarget)
	INTERFACE_PART(CAlbomServerFactory, IID_IAlbomFactory, Dispatch)
	INTERFACE_PART(CAlbomServerFactory, IID_IServerFactory, ServerFactory)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAlbomServerFactory, ServerFactory);

// {9518D746-CD50-40C5-960A-E31A27D73FAB}
GUARD_IMPLEMENT_OLECREATE(CAlbomServerFactory, szAlbomFactoryProgID4, 0x9518d746, 0xcd50, 0x40c5, 0x96, 0xa, 0xe3, 0x1a, 0x27, 0xd7, 0x3f, 0xab)


/////////////////////////////////////////
//
//
//	 Warning!!! change this "extern" clsid & prog id here
//
//

// {511ED0B9-BAB5-47a3-A497-3D3789E6CFE0}
const GUID CAlbomClassFactory::m_guid = 
{ 0x511ed0b9, 0xbab5, 0x47a3, { 0xa4, 0x97, 0x3d, 0x37, 0x89, 0xe6, 0xcf, 0xe0 } };
// {51906377-AC2C-11D0-91F5-008048EC2D88}

#define szComponentName				"VT 5 Album DataBase router"
#define szIDB32DatabaseProgID		"IDB32.DataBase"
#define szIDB32DatabaseProgIDVer	"IDB32.DataBase.2"

/////////////////////////////////////////////////////////////////////////////
bool CAlbomServerFactory::CanUseDBRouter()
{
	CString	strPathName = ::MakeAppPathName( "shell.data" );

	char sz_buf[1024];
	strcpy( sz_buf, "1" );

	::GetPrivateProfileString( "OleServer", "UseAlbumRouter:Long", sz_buf, sz_buf, sizeof(sz_buf), strPathName );
	::WritePrivateProfileString( "OleServer", "UseAlbumRouter:Long", sz_buf, strPathName );
	long lVal = atol( sz_buf );

	if( lVal == 1 )
		return true;

	return false;	
}

/////////////////////////////////////////////////////////////////////////////
//
//		IServerFactory
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::OpenFactory()
{
	_try_nested(CAlbomServerFactory, ServerFactory, OpenFactory)
	{
		if( !pThis->CanUseDBRouter() )
			return S_FALSE;

		if( pThis->m_pCF )
		{
			ASSERT( false );
			return E_FAIL;
		}

		CAlbomClassFactory* pFactory = new CAlbomClassFactory( );

		HRESULT hr = CoRegisterClassObject(
						CAlbomClassFactory::m_guid,
						pFactory->GetControllingUnknown(),
						CLSCTX_LOCAL_SERVER,
						REGCLS_MULTIPLEUSE,
						&pThis->m_dwCF );


		if( hr != S_OK )
		{
			ASSERT( FALSE );						
			delete pFactory;
			return E_FAIL;
		}

		pThis->m_pCF = pFactory;
		
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::CloseFactory()
{
	_try_nested(CAlbomServerFactory, ServerFactory, CloseFactory)
	{
		HRESULT hr = S_FALSE;
		if( pThis->m_dwCF != 0)
		{
			hr = CoRevokeClassObject( pThis->m_dwCF );
			VERIFY( hr == S_OK );

			if( pThis->m_pCF )
			{
				delete pThis->m_pCF;
				pThis->m_pCF = 0;				
			}			
		}

		return hr;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::GetRegisterInfo( CLSID* pCLSID, BSTR* pbstrComponentName, BSTR* pbstrProgID, BSTR* pbstrProgIDVer )
{
	_try_nested(CAlbomServerFactory, ServerFactory, GetRegisterInfo )
	{
		if( !pThis->CanUseDBRouter() )
			return S_FALSE;

		if( pCLSID )
			memcpy( pCLSID, &CAlbomClassFactory::m_guid, sizeof(CLSID) );

		CString strComponentName	= szComponentName;
		CString strProgID			= szIDB32DatabaseProgID;
		CString strProgIDVer		= szIDB32DatabaseProgIDVer;

		if( pbstrComponentName )
			*pbstrComponentName = strComponentName.AllocSysString();


		if( pbstrProgID )
			*pbstrProgID = strProgID.AllocSysString();

		if( pbstrProgIDVer )
			*pbstrProgIDVer = strProgIDVer.AllocSysString();

		return S_OK;
	}

	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::GetObjectsCount( long* plCount )
{
	_try_nested(CAlbomServerFactory, ServerFactory, GetObjectsCount)
	{
		*plCount = 0;

		if( !pThis->m_pCF )
		{
			ASSERT( FALSE );
			return E_FAIL;
		}

		*plCount = pThis->m_pCF->m_arObjects.GetSize();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::GetObjectRefCount( long lObjectIndex, long* plCount )
{
	_try_nested(CAlbomServerFactory, ServerFactory, GetObjectRefCount)
	{
		*plCount = 0;

		if( !pThis->m_pCF )
		{
			ASSERT( FALSE );
			return E_FAIL;
		}
		
		if( lObjectIndex < 0 || lObjectIndex >= pThis->m_pCF->m_arObjects.GetSize() )
		{
			ASSERT( FALSE );
			return E_FAIL;
		}

		*plCount = pThis->m_pCF->m_arObjects[lObjectIndex]->m_dwRef;

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomServerFactory::XServerFactory::GetServerLocks( long* plCount )
{
	_try_nested(CAlbomServerFactory, ServerFactory, GetServerLocks)
	{
		*plCount = 0;
		if( !pThis->m_pCF )
		{
			ASSERT( FALSE );
			return E_FAIL;
		}

		*plCount = pThis->m_pCF->m_dwLockCount;
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
//CAlbomClassFactory

BEGIN_INTERFACE_MAP(CAlbomClassFactory, CCmdTarget)
	INTERFACE_PART(CAlbomClassFactory, IID_IClassFactory, ClassFactory2)	
	//INTERFACE_PART(CAlbomClassFactory, IID_IClassFactory2, ClassFactory2)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAlbomClassFactory, ClassFactory2);


/////////////////////////////////////////////////////////////////////////////
CAlbomClassFactory::CAlbomClassFactory( )
{
	_OleLockApp( this );
	m_dwLockCount = 0;
}

/////////////////////////////////////////////////////////////////////////////
CAlbomClassFactory::~CAlbomClassFactory()
{
	m_arObjects.RemoveAll();

	_OleUnlockApp( this );
}

void CAlbomClassFactory::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}
/////////////////////////////////////////////////////////////////////////////
//
//		IClassFactory2
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomClassFactory::XClassFactory2::CreateInstance( IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
{
	_try_nested(CAlbomClassFactory, ClassFactory2, CreateInstance)
	{
		if (ppvObject == NULL)
			return E_POINTER;

		*ppvObject = NULL;

		// outer objects must ask for IUnknown only
		ASSERT(pUnkOuter == NULL || riid == IID_IUnknown);

		// attempt to create the object
		CAlbomDB* pTarget = NULL;
		SCODE sc = E_OUTOFMEMORY;
		TRY
		{
			// attempt to create the object
			pTarget = new CAlbomDB( );
			if (pTarget != NULL)
			{
				// check for aggregation on object not supporting it
				sc = CLASS_E_NOAGGREGATION;
				if (pUnkOuter == NULL || pTarget->m_xInnerUnknown != 0)
				{
					// create aggregates used by the object
					pTarget->m_pOuterUnknown = pUnkOuter;
					sc = E_OUTOFMEMORY;
					if (pTarget->OnCreateAggregates())
						sc = S_OK;
				}
			}
		}
		END_TRY

		// finish creation
		if (sc == S_OK)
		{
			DWORD dwRef = 1;
			if (pUnkOuter != NULL)
			{
				// return inner unknown instead of IUnknown
				*ppvObject = &pTarget->m_xInnerUnknown;
			}
			else
			{
				// query for requested interface
				sc = pTarget->InternalQueryInterface(&riid, ppvObject);
				if (sc == S_OK)
				{
					dwRef = pTarget->InternalRelease();
					ASSERT(dwRef != 0);
				}
			}
			if (dwRef != 1)
				TRACE1("Warning: object created with reference of %ld\n", dwRef);
		}

		// cleanup in case of errors
		if (sc != S_OK)
			delete pTarget;

		if( sc == S_OK )
		{
			pThis->m_arObjects.Add( pTarget );
			pTarget->SetFactory( pThis );

		}

		return sc;

	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CAlbomClassFactory::XClassFactory2::LockServer( BOOL fLock)
{
	_try_nested(CAlbomClassFactory, ClassFactory2, LockServer)
	{
		
		if( fLock )
			pThis->m_dwLockCount++;
		else
			pThis->m_dwLockCount--;

		
		return S_OK;
	}
	_catch_nested;
}


void CAlbomClassFactory::OnObjectKill( CAlbomDB* pObject )
{
	int nIndexFound = -1;
	for( int i=0;i<m_arObjects.GetSize();i++ )
	{
		if( pObject == m_arObjects[i] )
			nIndexFound = i;
	}

	ASSERT( nIndexFound != -1 );

	if( nIndexFound != -1 )
		m_arObjects.RemoveAt(nIndexFound);

}

/////////////////////////////////////////////////////////////////////////////
// CAlbomDB

//IMPLEMENT_DYNCREATE(CAlbomDB, CCmdTarget)


#define ALBOM_SECTION		"\\Album"
#define FILE_NAME			"DBase File name"

/////////////////////////////////////////////////////////////////////////////
CAlbomDB::CAlbomDB( )
{
	EnableAutomation();	
	m_pClassFactory = 0;
	
	OpenLastDBaseDoc();
}

/////////////////////////////////////////////////////////////////////////////
void CAlbomDB::SetFactory( CAlbomClassFactory* pClassFactory )
{
	m_pClassFactory = pClassFactory;
}

/////////////////////////////////////////////////////////////////////////////
CAlbomDB::~CAlbomDB()
{
}

/////////////////////////////////////////////////////////////////////////////
void CAlbomDB::OnFinalRelease()
{
	if( m_pClassFactory )
		m_pClassFactory->OnObjectKill( this );

	CCmdTarget::OnFinalRelease();
}

/////////////////////////////////////////////////////////////////////////////
bool CAlbomDB::IsAppValid()
{
	if( !CheckInterface( ::GetAppUnknown(), IID_INamedData ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString CAlbomDB::GetDBFileName()
{
	return ::GetValueString( ::GetAppUnknown(), ALBOM_SECTION, FILE_NAME, "" ); 
}

/////////////////////////////////////////////////////////////////////////////
//Init
bool CAlbomDB::OpenLastDBaseDoc()
{
	if( !IsAppValid() )
		return false;

	CString strFileName;

	strFileName = GetDBFileName();

	//test if file exist
	{
		CFileFind ff;
		if( !ff.FindFile( strFileName ) )
		{
			CString strFormat;
			strFormat.LoadString( IDS_WARNING_CANT_START_ALBOM );
			CString strReport;
			strReport.Format( strFormat, strFileName );
			
			AfxMessageBox( strReport, MB_OK | MB_ICONSTOP );
			return false;
		}
	}

	ExecuteAction( "FileOpen", CString( "\"" ) + strFileName + CString( "\"" ) );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
//helpers
IDBaseDocumentPtr CAlbomDB::GetDBaseDoc( bool bReportError /* = false */ )
{	
	CString strFileName = GetDBFileName();
	strFileName.MakeLower();

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{			
		LONG_PTR	lPosTemplate = 0;
		
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		
		while( lPosTemplate )
		{
			LONG_PTR	lPosDoc = 0;

			sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );				
				
				IDocumentSitePtr ptrDS;
				if( punkDoc )
				{
					ptrDS = punkDoc;					
					punkDoc->Release();
				}

				if( ptrDS )
				{
					BSTR bstrFileName = 0;
					ptrDS->GetPathName( &bstrFileName );
					CString strTest = bstrFileName;

					if( bstrFileName )
						::SysFreeString( bstrFileName );

					strTest.MakeLower();

					if( strTest == strFileName && CheckInterface( ptrDS, IID_IDBaseDocument ) )
					{
						return ptrDS;
					}
				}
			}
			
			sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
		}
	}

	if( bReportError )
	{
		ReportCantFindDBaseDoc();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
ISelectQueryPtr CAlbomDB::GetQuery( bool bReportError /* = false */ )
{
	

	IDBaseDocumentPtr ptrDBDoc = GetDBaseDoc( bReportError );
	if( ptrDBDoc )
	{
		IUnknown* punk = 0;
		ptrDBDoc->GetActiveQuery( &punk );
		if( punk )
		{
			ISelectQueryPtr ptrQ( punk );
			punk->Release();			

			return ptrQ;
		}
	}

	
	if( bReportError )
		ReportCantFindQuery();

	return 0;

}

/////////////////////////////////////////////////////////////////////////////
//Report error
void CAlbomDB::ReportCantFindDBaseDoc()
{
	AfxMessageBox( IDS_WARNING_ALBOM_CANT_FIND_DOC, MB_OK | MB_ICONSTOP );
}

/////////////////////////////////////////////////////////////////////////////
void CAlbomDB::ReportCantFindQuery()
{
	AfxMessageBox( IDS_WARNING_ALBOM_CANT_FIND_QUERY, MB_OK | MB_ICONSTOP );	
}

/////////////////////////////////////////////////////////////////////////////
bool CAlbomDB::IsQueryOpen( ISelectQueryPtr ptrQ )
{
	if( ptrQ == 0 )
		return false;

	BOOL bOpen = FALSE;
	ptrQ->IsOpen( &bOpen );

	return bOpen == TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CAlbomDB::BringApp2Top()
{
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp== 0 )
		return;

	HWND hWnd = 0;
	ptrApp->GetMainWindowHandle( &hWnd );

	if( !::IsWindow( hWnd ) )
		return;

	//!!!!!
	::SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
	::SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
	::SetWindowPos( hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );

}



BEGIN_MESSAGE_MAP(CAlbomDB, CCmdTarget)
	//{{AFX_MSG_MAP(CAlbomDB)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAlbomDB, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CAlbomDB)
	DISP_FUNCTION(CAlbomDB, "GetFieldType", GetFieldType, VT_I4, VTS_I4)
	DISP_FUNCTION(CAlbomDB, "GetFieldName", GetFieldName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CAlbomDB, "GetFieldCount", GetFieldCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "GetFieldValue", GetFieldValue, VT_VARIANT, VTS_I4)
	DISP_FUNCTION(CAlbomDB, "ShowQueryDialog", ShowQueryDialog, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CAlbomDB, "SetFieldValue", SetFieldValue, VT_BOOL, VTS_I4 VTS_VARIANT)
	DISP_FUNCTION(CAlbomDB, "GetRecordsCount", GetRecordsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "GetCurrentRecordNum", GetCurrentRecordNum, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "MoveFirst", MoveFirst, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "MoveNext", MoveNext, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "MovePrev", MovePrev, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "MoveLast", MoveLast, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "AddRecord", AddRecord, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "DeleteCurrentRecord", DeleteCurrentRecord, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "SetVisible", SetVisible, VT_BOOL, VTS_BOOL)
	DISP_FUNCTION(CAlbomDB, "IsVisible", IsVisible, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "PasteField", PasteField, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CAlbomDB, "OpenLastShape", OpenLastShape, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "OpenFile", OpenFile, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CAlbomDB, "UpdateView", UpdateView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAlbomDB, "GetActiveField", GetActiveField, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAlbomDB to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .ODL file.


static const IID IID_IAlbomDB =
{ 0xcf1154a4, 0xbe94, 0x4665, { 0x9d, 0xe2, 0x15, 0x9e, 0x0, 0xc, 0xe5, 0x45 } };

BEGIN_INTERFACE_MAP(CAlbomDB, CCmdTarget)
	INTERFACE_PART(CAlbomDB, IID_IAlbomDB, Dispatch)
END_INTERFACE_MAP()

/*
#ifndef _DBDAOINT_H_

typedef enum DataTypeEnum {
    dbBoolean = 1,
    dbByte = 2,
    dbInteger = 3,
    dbLong = 4,
    dbCurrency = 5,
    dbSingle = 6,
    dbDouble = 7,
    dbDate = 8,
    dbBinary = 9,
    dbText = 10,
    dbLongBinary = 11,
    dbMemo = 12,
    dbGUID = 15,
    dbBigInt = 16,
    dbVarBinary = 17,
    dbChar = 18,
    dbNumeric = 19,
    dbDecimal = 20,
    dbFloat = 21,
    dbTime = 22,
    dbTimeStamp = 23
} DataTypeEnum;


#endif
  */

const short dbImage = 100;
const short dbOLEObj = 101;

const short dbDigit = 7;
const short dbString = 10;
const short dbDataTime = 23;


/////////////////////////////////////////////////////////////////////////////
long CAlbomDB::GetFieldType(long iFieldNum) 
{
	IDBaseDocumentPtr	ptrDBDoc = GetDBaseDoc( );
	if( ptrDBDoc == 0 )
		return 0;

	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return 0;

	if( !IsQueryOpen(ptrQ) )
		return 0;

	IQueryObjectPtr ptrQO( ptrQ );	
	int nCount = 0;
	ptrQO->GetFieldsCount( &nCount );

	if( nCount < 1 )
		return 0;

	if( iFieldNum < 0 || iFieldNum >= nCount )
		return 0;

	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;
	ptrQO->GetField( iFieldNum, 0, &bstrTable, &bstrField );


	FieldType ft = ::GetFieldType( ptrDBDoc, bstrTable, bstrField );	
	CString strVTObjectType = ::GetFieldVTObjectType( ptrDBDoc, bstrTable, bstrField );

	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;

	if( ft == ftDigit )
		return dbDigit;
	else
	if( ft == ftString )
		return dbString;
	else
	if( ft == ftDateTime )
		return dbDataTime;
	else
	if( ft == ftVTObject && strVTObjectType == szTypeImage )
		return dbImage;
	else
	if( ft == ftVTObject && strVTObjectType == szTypeOleItem )
		return dbOLEObj;

	return 0;
}

BSTR CAlbomDB::GetFieldName(long iFieldNum) 
{
	CString strResult;

	IDBaseDocumentPtr	ptrDBDoc = GetDBaseDoc( );
	if( ptrDBDoc == 0 )
		return strResult.AllocSysString();

	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return strResult.AllocSysString();

	if( !IsQueryOpen(ptrQ) )
		return strResult.AllocSysString();

	IQueryObjectPtr ptrQO( ptrQ );	
	int nCount = 0;
	ptrQO->GetFieldsCount( &nCount );

	if( nCount < 1 )
		return strResult.AllocSysString();

	if( iFieldNum < 0 || iFieldNum >= nCount )
		return strResult.AllocSysString();

	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;
	ptrQO->GetField( iFieldNum, 0, &bstrTable, &bstrField );

	strResult.Format( "%s.%s", (const char*)CString(bstrTable), (const char*)CString(bstrField) );

	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;
	
	return strResult.AllocSysString();
}

long CAlbomDB::GetFieldCount() 
{
	IQueryObjectPtr ptrQO( GetQuery() );
	if( ptrQO == 0 )
		return 0;

	int nCount = 0;
	ptrQO->GetFieldsCount( &nCount );

	return nCount;
}


BOOL CAlbomDB::ShowQueryDialog(long hWndParent)
{
	//Not Implement!!!
	ASSERT( false );	

	return FALSE;
}


long CAlbomDB::GetRecordsCount() 
{	
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return 0;

	if( !IsQueryOpen(ptrQ) )
		return 0;

	long lCount = 0;
	ptrQ->GetRecordCount( &lCount );

	return lCount;
}

long CAlbomDB::GetCurrentRecordNum() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return 0;

	if( !IsQueryOpen(ptrQ) )
		return 0;

	long lRecNum = 0;
	ptrQ->GetCurrentRecord( &lRecNum );

	return lRecNum;
}

BOOL CAlbomDB::MoveFirst() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->MoveFirst() == S_OK;
}

BOOL CAlbomDB::MoveNext() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->MoveNext() == S_OK;
}

BOOL CAlbomDB::MovePrev() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->MovePrev() == S_OK;
}

BOOL CAlbomDB::MoveLast() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->MoveLast() == S_OK;
}

BOOL CAlbomDB::AddRecord() 
{
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->Insert() == S_OK;
}

BOOL CAlbomDB::DeleteCurrentRecord() 
{	
	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	return ptrQ->Delete() == S_OK;
}

BOOL CAlbomDB::SetVisible(BOOL bVisible) 
{
	return TRUE;
}

BOOL CAlbomDB::IsVisible() 
{
	return TRUE;
}

long CAlbomDB::GetActiveField() 
{
	IQueryObjectPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	int nActiveField = -1;

	ptrQ->GetActiveField( &nActiveField );

	return ( (long)nActiveField );
}

void CAlbomDB::UpdateView() 
{
	
}

BOOL CAlbomDB::OpenFile(LPCTSTR szPathName)
{
	//Not Implement!!!
	ASSERT( false );	

	return FALSE;
}

BOOL CAlbomDB::OpenLastShape()
{
	//Not Implement!!!
	ASSERT( false );	

	return FALSE;
}

BOOL CAlbomDB::PasteField(long iFieldNum)
{
	IDBaseDocumentPtr	ptrDBDoc = GetDBaseDoc( );
	if( ptrDBDoc == 0 )
		return FALSE;

	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	IQueryObjectPtr ptrQO( ptrQ );	
	int nCount = 0;
	ptrQO->GetFieldsCount( &nCount );

	if( nCount < 1 )
		return FALSE;

	if( iFieldNum < 0 || iFieldNum >= nCount )
		return FALSE;

	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;
	ptrQO->GetField( iFieldNum, 0, &bstrTable, &bstrField );

	CString strTable, strField;
	
	strTable = bstrTable;
	strField = bstrField;

	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;


	FieldType ft = ::GetFieldType( ptrDBDoc, strTable, strField );	
	CString strVTObjectType = ::GetFieldVTObjectType( ptrDBDoc, strTable, strField );


	_bstr_t bstrFieldFullName;	
	bstrFieldFullName += (LPCSTR)strTable;
	bstrFieldFullName += ".";
	bstrFieldFullName += (LPCSTR)strField;


	_bstr_t bstrFieldFullPath( SECTION_DBASEFIELDS );
	bstrFieldFullPath += "\\";
	bstrFieldFullPath += (LPCSTR)bstrFieldFullName;


	long lType = GetFieldType( iFieldNum );

	
	if( lType == dbOLEObj && ft == ftVTObject && strVTObjectType == szTypeOleItem )
	{
		IUnknown* punkOLE = ::CreateTypedObject( szTypeOleItem );
		IUnknownPtr ptrObj = punkOLE;
		if( punkOLE ) 
		{
			punkOLE->Release();	punkOLE = 0;
		}

		IOleObjectDataPtr ptrOle( ptrObj );
		if( ptrOle == NULL )
			return FALSE;

		if( S_OK != ptrOle->CreateFromClipboard() )
			return FALSE;
		

		INamedObject2Ptr ptrNO2( ptrObj );
		ptrNO2->SetName( bstrFieldFullName );		


		INamedDataPtr ptrND( ptrDBDoc );
		if( ptrND->SetValue( bstrFieldFullPath, _variant_t( (IUnknown*)ptrObj ) ) != S_OK )
			return FALSE;

		BringApp2Top();

		return TRUE;

	}	

	return FALSE;

}



//the most 
VARIANT CAlbomDB::GetFieldValue(long iFieldNum)
{
	_variant_t var;

	//Not Implement!!!
	ASSERT( false );
	
	return var;
}


BOOL CAlbomDB::SetFieldValue(long iFieldNum, const VARIANT FAR& var)
{
	IDBaseDocumentPtr	ptrDBDoc = GetDBaseDoc( );
	if( ptrDBDoc == 0 )
		return FALSE;

	ISelectQueryPtr ptrQ = GetQuery();
	if( ptrQ == 0 )
		return FALSE;

	if( !IsQueryOpen(ptrQ) )
		return FALSE;	

	IQueryObjectPtr ptrQO( ptrQ );	
	int nCount = 0;
	ptrQO->GetFieldsCount( &nCount );

	if( nCount < 1 )
		return FALSE;

	if( iFieldNum < 0 || iFieldNum >= nCount )
		return FALSE;

	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;
	ptrQO->GetField( iFieldNum, 0, &bstrTable, &bstrField );

	CString strTable, strField;
	
	strTable = bstrTable;
	strField = bstrField;

	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;


	FieldType ft = ::GetFieldType( ptrDBDoc, strTable, strField );	
	CString strVTObjectType = ::GetFieldVTObjectType( ptrDBDoc, strTable, strField );


	_bstr_t bstrFieldFullName;	
	bstrFieldFullName += (LPCSTR)strTable;
	bstrFieldFullName += ".";
	bstrFieldFullName += (LPCSTR)strField;


	_bstr_t bstrFieldFullPath( SECTION_DBASEFIELDS );
	bstrFieldFullPath += "\\";
	bstrFieldFullPath += (LPCSTR)bstrFieldFullName;


	long lType = GetFieldType( iFieldNum );
	
	if( lType == dbImage && ft == ftVTObject && strVTObjectType == szTypeImage )
	{
		_variant_t varData( var );

		if( varData.vt != (VT_UI1|VT_ARRAY) )
			return FALSE;
		
		if( varData.parray == NULL )
			return FALSE;

		ULONG uSize = varData.parray->rgsabound[0].cElements;
		BYTE* pSrc = (BYTE*)varData.parray->pvData;
		if( pSrc == NULL )
			return FALSE;

		LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)pSrc;		

		IUnknown* punkImage = ::CreateTypedObject( szTypeImage );
		IUnknownPtr ptrObj = punkImage;
		if( punkImage ) 
		{
			punkImage->Release();
			punkImage = 0;
		}
		

		IClipboardPtr sptrClip( ptrObj );
		if( sptrClip == 0 )
			return FALSE;

		if( sptrClip->Paste( CF_DIB, (HANDLE)lpbih ) != S_OK )
			return FALSE;


		INamedObject2Ptr ptrNO2( ptrObj );
		ptrNO2->SetName( bstrFieldFullName );


		INamedDataPtr ptrND( ptrDBDoc );
		if( ptrND->SetValue( bstrFieldFullPath, _variant_t( (IUnknown*)ptrObj ) ) != S_OK )
			return FALSE;

		BringApp2Top();

		return TRUE;
	}
	else if( lType == dbDigit || lType == dbDataTime )
	{
		_variant_t varData( var );

		INamedDataPtr ptrND( ptrDBDoc );
		if( ptrND->SetValue( bstrFieldFullPath, varData ) != S_OK )
			return FALSE;
	}
	else
	if( lType == dbString )
	{
		
		if( var.vt != VT_BSTR )
			return FALSE;

		CString str = var.pbVal;
		
		_variant_t varData( _bstr_t( (LPCSTR)str ) );

		INamedDataPtr ptrND( ptrDBDoc );
		if( ptrND->SetValue( bstrFieldFullPath, varData ) != S_OK )
			return FALSE;

	}
	else 
	{
		ASSERT( false );
	}


	return FALSE;
}
