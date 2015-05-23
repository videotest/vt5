// AxDocContainer.cpp: implementation of the CAxDocContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AxDocContainer.h"
#include "ole_utils.h"
#include "resource.h"
#include "misc_utils.h"
#include "constant.h"
#include "oleitem.h"

#include "data5.h"
#include "misc5.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define szObjStreamName	"\003DocObjInfo"



static int dwCounter = 0;
CAxDocContainer::CAxDocContainer( COleItemData* pOleItemData )
{	

	DWORD dw = ::GetTickCount( ) + dwCounter;
	dwCounter++;

	m_bstrStreamName  = szObjStreamName;
	char buf[255];
	_ui64toa(dw, buf, 10);
	m_bstrStreamName += buf;	

	//m_bstrStreamName += "e82375712534123";


	m_pOleItemData = pOleItemData;

	InitDefaults();
	
	m_pSite = NULL;
	m_pSite = new CAxDocSite( this );
}

//////////////////////////////////////////////////////////////////////
CAxDocContainer::~CAxDocContainer()
{
	if( m_pSite )
	{
		m_pSite->Release();
		m_pSite = NULL;
	}

	Free();
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::InitDefaults()
{
	m_hwndMain				= NULL;
	m_hWndClient			= NULL;
	m_hWndObj				= NULL;



	m_lpLockBytes			= NULL;
	m_pIStorage				= NULL;
	m_pIStream				= NULL;


	m_pOleData				= NULL;
	m_uOleDataLength		= 0;

	
	::ZeroMemory( &m_bwIP, sizeof(BORDERWIDTHS) );
	m_pIOleIPActiveObject	= NULL;
	

	m_bDocObj				= FALSE;

	m_pObj					= NULL;
	m_pIOleObject			= NULL;
	m_pIOleIPObject			= NULL;
	m_pIOleDocView			= NULL;
	m_lpViewObject			= NULL;

	m_bCreateOleObject		= false;

	m_itemState				= iaNotDefined;

	m_bWasFullActivated		= false;
	m_bWasDeactivateUI		= false;

	m_hImageList			= NULL;
	m_nIconNumber			= -1;


	ClearMenu();

	m_bNeedFireObjectChange	= false;

}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::Free()
{
	Deactivate( m_hWndClient );
	DestroyOleUI();
	DestroyOleObject( );
	DestroyStorage();
	DestroyOleDataBuf();	

	DestroyImageList();	
}


//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::CreateOleObject( )
{
	DestroyOleUI();
	DestroyOleObject( );
	DestroyStorage();


    HRESULT             hr=E_FAIL;
    LPUNKNOWN           pObj = NULL;

	if( !CreateStorage( IsDataBufOK() ) )
		return false;

	if( !m_pIStorage || !m_pIStream )
		return false;

	OleCreationType oct = m_pOleItemData->m_creationType;
	bstr_t bstrFileName = m_pOleItemData->m_bstrFileName;
	GUID guidApp		= m_pOleItemData->m_appGuid;

	bool bNeedCreateIImageList = true;
	bool bNeedStoreToBuf = false;

	if( IsDataBufOK() )//Create from storage
	{
		hr = ::OleLoad( m_pIStorage, IID_IUnknown, (IOleClientSite*)m_pSite, (void **)&pObj);
		bNeedCreateIImageList = false;
	}
	else
	if( oct == octLinkToFile )
	{										 
		hr = ::OleCreateFromFile(CLSID_NULL, bstrFileName, IID_IUnknown
			, OLERENDER_DRAW, NULL, (IOleClientSite*)m_pSite, m_pIStorage, (void **)&pObj);
	}
	else
	if( oct == octInsertFromFile )
	{
		hr = ::OleCreateFromFile(CLSID_NULL, bstrFileName, IID_IUnknown
			, OLERENDER_DRAW, NULL, (IOleClientSite*)m_pSite, m_pIStorage, (void **)&pObj);
		
		bNeedStoreToBuf = true;
	}
	else
	if( oct == octCreateNewItem )
	{
		hr = ::OleCreate( guidApp, IID_IUnknown
			, OLERENDER_DRAW, NULL, (IOleClientSite*)m_pSite, m_pIStorage, (void **)&pObj);
		
		bNeedStoreToBuf = true;
	}
	else
	if( oct == octCreateFromClipboard )
	{

		IDataObject* pIDataObject = 0;
		SCODE sc = ::OleGetClipboard( &pIDataObject );
		if( sc == S_OK )
		{
			hr = ::OleCreateFromData( pIDataObject, IID_IUnknown, OLERENDER_DRAW, NULL,
					(IOleClientSite*)m_pSite, m_pIStorage, (void **)&pObj );			
			
			bNeedStoreToBuf = true;
		}

		if( pIDataObject )
			pIDataObject->Release();

		
	}
	else
		return false;

	
	m_bCreateOleObject = ( FAILED(hr) == FALSE );

	if( m_bCreateOleObject )
	{
		m_pObj = pObj;

		hr = m_pObj->QueryInterface(IID_IOleObject, (void **)&m_pIOleObject);

		if( m_pIOleObject )
		{
			//DWORD dwConnection;
			//m_pIOleObject->Advise( (IAdviseSink*)m_pSite, &dwConnection );
			hr = m_pIOleObject->QueryInterface(IID_IViewObject2, (void **)&m_lpViewObject );
			
			if( m_lpViewObject )
			{
				//hr = m_lpViewObject->SetAdvise( DVASPECT_CONTENT, 0, (IAdviseSink*)m_pSite );
			}
			

			BSTR bstrObjName = 0;
			m_pOleItemData->GetName( &bstrObjName );

		    m_pIOleObject->SetHostNames( OLESTR("VT5"), bstrObjName );

			::SysFreeString( bstrObjName );	bstrObjName = 0;			

			::OleSetContainedObject( m_pIOleObject, TRUE );

		}

		if( bNeedStoreToBuf )
		{
			StoreStreamToBuf( true );
		}

	}


	if( bNeedCreateIImageList )//Try create Image List	
		CreateImageList();


	m_itemState = iaLoaded;

	SetModifiedServerData( true );	
	

	return m_bCreateOleObject;
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::DestroyOleObject( )
{	

	if( m_lpViewObject )
	{
		m_lpViewObject->Release();	
		m_lpViewObject = 0;
	}

	if( m_pIOleObject )
	{
		m_pIOleObject->Release();
		m_pIOleObject = 0;
	}

	if( m_pObj )
	{
		m_pObj->Release();
		m_pObj = 0;
	}    
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::DestroyOleUI()
{

	if( m_lpViewObject )
	{
		//HRESULT hr = m_lpViewObject->SetAdvise( DVASPECT_CONTENT, 0, 0 );
	}


    //OnInPlaceDeactivate releases this pointer.
    if (NULL!=m_pIOleIPObject)
	{
		m_pIOleIPObject->UIDeactivate();
        m_pIOleIPObject->InPlaceDeactivate();		
		m_pIOleIPObject = 0;
	}

    if (NULL!=m_pIOleObject)
    {
        m_pIOleObject->Close( OLECLOSE_SAVEIFDIRTY );		
    }

	if( m_pIOleDocView )
	{		
		m_pIOleDocView->Release();
		m_pIOleDocView = 0;
	}    


}

//////////////////////////////////////////////////////////////////////
OleObjectType CAxDocContainer::GetOleObjectType()
{
	if( !m_pIOleObject )
		return ootNotDefined;

	if( m_pIOleDocView )
		return ootAxDocument;

	if( m_pIOleIPObject )
		return ootInPlaceActivate;

	return ootOleObject;
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::OnResizeClient( int cx, int cy )
{

	if( !m_hWndClient )
		return;

	OleObjectType oot = GetOleObjectType();

    RECT    rc;

	if( oot == ootAxDocument && m_pIOleDocView )
	{
	    GetClientRect( m_hWndClient, &rc);
		m_pIOleDocView->SetRect( &rc );
	}
	else
	if( oot == ootInPlaceActivate && m_pIOleIPObject )
	{
		RECT rcPos;
		RECT rcClip;
		SetRect( &rcPos, 0, 0, cx, cy );
		SetRect( &rcClip, 0, 0, 0, 0 );
		m_pIOleIPObject->SetObjectRects( &rcPos, &rcPos );
	}
	else
	if( oot == ootOleObject && m_pIOleIPActiveObject )
	{

	}

}


//////////////////////////////////////////////////////////////////////
void CAxDocContainer::DestroyOleDataBuf()
{	
	if( m_pOleData )
	{
		delete m_pOleData;
		m_pOleData = 0;
	}

	m_uOleDataLength = 0;		
}





//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::CreateStorage( bool bInitFromOleData )
{
	DestroyStorage();

	if( bInitFromOleData )
	{		
		if( m_uOleDataLength <= 0 || !m_pOleData )
			return false;		

		HGLOBAL hStorage = ::GlobalAlloc( GMEM_SHARE|GMEM_MOVEABLE, m_uOleDataLength );
		if (hStorage == NULL)
			return false;

		LPVOID lpBuf = ::GlobalLock( hStorage );
		memcpy( lpBuf, m_pOleData, m_uOleDataLength );
		::GlobalUnlock( hStorage );

		SCODE sc = CreateILockBytesOnHGlobal(hStorage, TRUE, &m_lpLockBytes);
		if( sc != S_OK || !m_lpLockBytes )
		{
			::GlobalFree(hStorage);
			return false;
		}
		
		
		sc = ::StgOpenStorageOnILockBytes( m_lpLockBytes, NULL,
			STGM_SHARE_EXCLUSIVE|STGM_READWRITE, NULL, 0, &m_pIStorage );
		if( sc != S_OK )
		{
			m_lpLockBytes->Release();
			m_lpLockBytes = NULL;
			return false;
		}

		if( !m_pIStorage )
			return false;
		
	}
	else
	{

		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &m_lpLockBytes);
		if (sc != S_OK)
			return false;

		sc = ::StgCreateDocfileOnILockBytes( m_lpLockBytes,
			STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &m_pIStorage );
		if (sc != S_OK)
		{
			m_lpLockBytes->Release();
			m_lpLockBytes = NULL;
		}

		if( !m_pIStorage )
			return false;
	}


	//create child stream
    OLECHAR  szwName[32];	
	//Create stream for the object; name is irrelevant
	DWORD dwMode=STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
	MultiByteToWideChar(CP_ACP, 0, m_bstrStreamName/*szObjStreamName*/, -1, szwName, 32);    

    HRESULT hr = m_pIStorage->CreateStream(szwName, dwMode, 0, 0, &m_pIStream );
    //If we failed to create a stream in the file, do it in memory
	if( FAILED(hr) )
	{
		dbg_assert( false );
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &m_pIStream )))
		    return false;		
	}// OLE_E_BLANK

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::DestroyStorage()
{	
	//at first destroy stream
	if( m_pIStorage && m_pIStream )
	{
		OLECHAR  szwName[32];			
		MultiByteToWideChar(CP_ACP, 0, m_bstrStreamName/*szObjStreamName*/, -1, szwName, 32);    
		HRESULT hr = m_pIStorage->DestroyElement(szwName);		
		if( hr != S_OK )
		{
			dbg_assert( false );
		}

		m_pIStream->Release();
		m_pIStream = 0;
	}


	if( m_pIStorage )
	{		
		m_pIStorage->Release();
		m_pIStorage = 0;
	}		


	if( m_lpLockBytes )
	{
		m_lpLockBytes->Release();
		m_lpLockBytes = 0;
	}


}


//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::LoadDataBuf( IStream *pStream )
{
	DestroyOleDataBuf();

	if(!pStream)
		return false;

	ULONG nRead = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	pStream->Read( &m_uOleDataLength, sizeof(UINT), &nRead );

	if( m_uOleDataLength > 0 )
	{
		m_pOleData = new BYTE[m_uOleDataLength];
		pStream->Read( m_pOleData, sizeof(BYTE)*m_uOleDataLength, &nRead );
	}

	SetModifiedServerData( false );	

	return true;

}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::StoreDataBuf( IStream *pStream )
{	
	//if( m_bWasFullActivated )
	StoreStreamToBuf();

	
	ULONG nWritten = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	pStream->Write( &m_uOleDataLength, sizeof(UINT), &nWritten );

	if( m_uOleDataLength > 0 && m_pOleData )
	{
		pStream->Write( m_pOleData, sizeof(BYTE)*m_uOleDataLength, &nWritten );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::StoreStreamToBuf( bool bSameStream )
{
	if( !m_pIStorage )
		return false;
	
	if( !m_pObj )
		return false;

	OleCreationType oct = m_pOleItemData->m_creationType;

	if( oct == octLinkToFile )
	{

		DestroyOleDataBuf();

		char szWarning[255];
		char szFormat[255];		
		
		HMODULE	hInst = App::handle();
		if( !::LoadString( hInst, IDS_WARNING_OLE_ITEM_IS_LINK, szFormat, 255 ) ) 
			return false;
		
		wsprintf( szWarning, szFormat, (LPCSTR)m_pOleItemData->m_bstrFileName );		

		if( VTMessageBox( szWarning, szDllName, MB_YESNO | MB_ICONQUESTION ) != IDYES )
			return true;

		LPPERSISTSTORAGE    pIPS = NULL;

		if( NULL == m_pIStorage || m_pObj == NULL )
			return false;

		m_pObj->QueryInterface(IID_IPersistStorage, (void **)&pIPS);
		if( !pIPS )
			return false;

		m_bNeedFireObjectChange = ( S_OK == pIPS->IsDirty() );

		OleSave( pIPS, m_pIStorage, TRUE);
		pIPS->SaveCompleted(NULL);
		pIPS->Release();	pIPS = 0;

		m_pIStorage->Commit(STGC_DEFAULT);

		return true;			
	}


	IPersistStorage* pPersistStorage = NULL;	
	HRESULT hRes = m_pObj->QueryInterface( IID_IPersistStorage, (void**)&pPersistStorage );
	if( hRes != S_OK || pPersistStorage == 0 )
		return false;

	// save the OLE object to its storage first

	bool bStorageDirty = ( S_OK == pPersistStorage->IsDirty() );

	SetModifiedServerData( bStorageDirty );

	m_bNeedFireObjectChange = bStorageDirty;	

	if( !bStorageDirty )
	{
		pPersistStorage->Release();	pPersistStorage = 0;
		return true;
	}
	
	
	SCODE sc = ::OleSave( pPersistStorage, m_pIStorage, bSameStream ? TRUE : FALSE );
	if( sc != S_OK ) 
	{
		__trace( "OleSave failed..." );
		dbg_assert( false );
	}

	
	hRes = pPersistStorage->SaveCompleted( NULL );
	if( hRes != S_OK )
	{
		dbg_assert( false );
	}

	//hRes = pPersistStorage->SaveCompleted(NULL);
	
	pPersistStorage->Release();	pPersistStorage = 0;

	hRes = m_pIStorage->Commit(STGC_DEFAULT);	
	if( hRes != S_OK )
	{
		dbg_assert( false );
	}


	HGLOBAL hStorage;
	sc = ::GetHGlobalFromILockBytes(m_lpLockBytes, &hStorage);
	if (sc != S_OK)
		return false;
		

	// first write a byte count
	STATSTG statstg;
	sc = m_lpLockBytes->Stat(&statstg, STATFLAG_NONAME);
	if (sc != S_OK)
		return false;

	DestroyOleDataBuf();

	m_uOleDataLength = statstg.cbSize.LowPart;	
	m_pOleData = new BYTE[m_uOleDataLength];

	// write the contents of the block
	LPVOID lpBuf = GlobalLock( hStorage );
	memcpy( m_pOleData, lpBuf, m_uOleDataLength );
	::GlobalUnlock(hStorage);


	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::ExecVerb( int iVerb )
{
	if( !m_pIOleObject )
		return false;

    RECT        rc;            
    GetClientRect( m_hWndClient, &rc);
    HRESULT hRes = m_pIOleObject->DoVerb( iVerb, NULL, (IOleClientSite*)m_pSite, 0
        , m_hWndClient, &rc);

	dbg_assert( hRes == S_OK );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::Activate( HWND hWndMain, HWND hWndClient )
{
	if( !m_bCreateOleObject )
		return false;

	if( !::IsWindow( hWndMain ) || !::IsWindow( hWndClient ) )
		return false;

	m_hwndMain = hWndMain;
	m_hWndClient = hWndClient;
	

	OleObjectType ot = GetOleObjectType();

	if( ot == ootNotDefined )
		return false;

	HWND hwndFrameActivateWhere = 0;

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF )
		ptrOF->ActivateOleObject( m_pOleItemData->Unknown(), &hwndFrameActivateWhere );
	
	if( m_pIOleObject )
	{
		OleRun( m_pIOleObject );

		DWORD dw;
		m_pIOleObject->SetClientSite( (IOleClientSite*)m_pSite );
		m_pIOleObject->Advise( (IAdviseSink*)m_pSite, &dw);
	}	

	if( m_lpViewObject )
	{
		//HRESULT hr = m_lpViewObject->SetAdvise( DVASPECT_CONTENT, ADVF_PRIMEFIRST, (IAdviseSink*)m_pSite );
	}

	if( ot == ootOleObject )
		ptrOF->CreateStdOLEMenu( m_pOleItemData->Unknown(), m_pIOleObject );
	
	ExecVerb( OLEIVERB_SHOW );


	m_itemState = iaFullActivate;
	m_bWasFullActivated = true;	

	//Force repaint to show "have object" message
	InvalidateRect( m_hWndClient, NULL, TRUE );
	UpdateWindow( m_hWndClient );	

/*	
	RECT rc;
	::GetClientRect( m_hWndClient, &rc);

	m_pOleItemData->OnFrameResizeBorder( rc );
	*/


	if( m_pIOleIPActiveObject )
	{
		//m_pIOleIPActiveObject->OnFrameWindowActivate( true );
		m_pIOleIPActiveObject->OnDocWindowActivate( true );
		
	}

	if( hwndFrameActivateWhere )
	{
		HWND hWndParent = ::GetParent( hwndFrameActivateWhere );
		if( hWndParent )
		{
			RECT rc;
			::GetWindowRect( hwndFrameActivateWhere, &rc );

			POINT pt1;
			POINT pt2;

			pt1.x = rc.left;
			pt1.y = rc.top;

			pt2.x = rc.right;
			pt2.y = rc.bottom;
			
			
			::ScreenToClient( hWndParent, &pt1 );
			::ScreenToClient( hWndParent, &pt2 );

			::SendMessage( hwndFrameActivateWhere, WM_SIZE, 0, MAKELPARAM( pt2.x - pt1.x + 1, pt2.y - pt1.y + 1 ) );
			::SendMessage( hwndFrameActivateWhere, WM_SIZE, 0, MAKELPARAM( pt2.x - pt1.x - 1, pt2.y - pt1.y - 1 ) );
			//SetWindowPos( hwndFrameActivateWhere, 0, pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y, 
			//	SWP_NOMOVE|SWP_NOZORDER );
		}
	}


	FireEventStateChange();

	return true;
}


//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::IsModifiedServerData()
{		
	return m_bWasModifiedServerData;
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::SetModifiedServerData( bool bModified )
{
	m_bWasModifiedServerData = bModified;
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::Deactivate( HWND hWndView )
{	
	if( !::IsWindow( hWndView ) )
		return false;

	if( hWndView != m_hWndClient )
		return false;

	m_bNeedFireObjectChange = false;

	if( m_bWasFullActivated )
	{
		m_bWasFullActivated = false;
		StoreStreamToBuf();
	}

	DestroyOleUI();

	
    SetRect( &m_bwIP, 0, 0, 0, 0 );
	InvalidateRect( m_hWndClient, NULL, TRUE );
	UpdateWindow( m_hWndClient );

	m_hwndMain				= NULL;
	m_hWndClient			= NULL;

	m_bWasFullActivated		= false;
	m_bWasDeactivateUI		= false;

	m_itemState = iaFullDeactivate;

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF )
		ptrOF->DeactivateOleObject( m_pOleItemData->Unknown() );

	FireEventStateChange();

	if( m_bNeedFireObjectChange )//determine on StoreStreamToBuf
		m_pOleItemData->FireEventObjectWasChange();


	m_bNeedFireObjectChange = false;

	return true;	
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::ActivateUI( HWND hWndView  )
{
	if( !m_bWasFullActivated || hWndView != m_hWndClient || !m_bWasDeactivateUI )
		return false;


	OleObjectType ot = GetOleObjectType();

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF )
		ptrOF->ActivateOleObject( m_pOleItemData->Unknown(), 0 );

	if( m_pIOleIPActiveObject )
		m_pIOleIPActiveObject->OnDocWindowActivate( true );

	if( ot == ootOleObject )
		ptrOF->CreateStdOLEMenu( m_pOleItemData->Unknown(), m_pIOleObject );



	m_bWasDeactivateUI = false;

	//AddVerbMenu();

	//if( m_hWndObj )
	//	::SetFocus( m_hWndObj );

	FireEventStateChange();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::DeactivateUI( HWND hWndView )
{
	if( !m_bWasFullActivated || hWndView != m_hWndClient || m_bWasDeactivateUI )
		return false;


	if( m_pIOleIPActiveObject )
		m_pIOleIPActiveObject->OnDocWindowActivate( false );

	//if( m_pIOleIPObject )
	//	m_pIOleIPObject->UIDeactivate();

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF )
		ptrOF->DeactivateOleObject( m_pOleItemData->Unknown() );

	

	m_bWasDeactivateUI = true;
	
	FireEventStateChange();

	return true;
}


//////////////////////////////////////////////////////////////////////
HRESULT	CAxDocContainer::ForceUpdate()
{
	if( !m_bWasFullActivated )
		return S_FALSE;
	
	StoreStreamToBuf();
	
	return S_OK;
	
}

//////////////////////////////////////////////////////////////////////
HRESULT CAxDocContainer::DoVerb( UINT uiVerb )
{
	if( !m_bWasFullActivated )
		return E_FAIL;

	bool bRes = ExecVerb( uiVerb );
	return ( S_OK ? bRes : S_FALSE );
}
/*
//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::AddVerbMenu()
{
	if( !( m_bWasFullActivated || m_bWasDeactivateUI ) ) 
		return false;

	if( !m_pIOleObject )
		return false;


	if( !::IsWindow( m_hwndMain ) )
		return false;


	HMENU hMenu = ::GetMenu( m_hwndMain );
	if( !hMenu )
		return false;

	HMENU hMenuVerb = 0;
	if( !::OleUIAddVerbMenu( m_pIOleObject, "NULL", hMenu, 1, IDM_VERB0, 0, FALSE, 0, &hMenuVerb ) )
		dbg_assert( false );

	return true;

}
*/

//////////////////////////////////////////////////////////////////////
IUnknown* CAxDocContainer::DoGetInterface( const IID &iid )
{
	if( iid == IID_IOleInPlaceFrame )
	{
		return (IOleInPlaceFrame*)this;
	}
	else
	if( iid == IID_IOleCommandTarget )
	{
		return (IOleCommandTarget*)this;
	}		
	else 
		return ComObjectBase::DoGetInterface( iid );
}




//////////////////////////////////////////////////////////////////////
void CAxDocContainer::ClearMenu()
{
	for( int i=0;i<OLE_MENU_SIZE;i++)
	{
		m_hFrameMenu[i] = NULL; 
		m_bstrMenuCaption[i] = ""; 
	}
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::CanProcessDrawig( HWND hWnd )
{
	OleObjectType oot = GetOleObjectType( );
	if( oot == ootNotDefined )
		return true;

	if( ( oot == ootInPlaceActivate || oot == ootAxDocument ) && hWnd == m_hWndClient )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAxDocContainer::DrawMeta( int nDrawAspect, HWND hWnd, HDC hdc, RECT rcTarget, POINT ptOffset, SIZE sizeClip )
{
	if( !m_lpViewObject )
		return E_FAIL;


	int nWidth, nHeight;

	nWidth	= rcTarget.right - rcTarget.left;
	nHeight	= rcTarget.bottom - rcTarget.top;

	

	if( nDrawAspect != DVASPECT_CONTENT )
	{
		nDrawAspect = DVASPECT_CONTENT;
		//??Wanna draw icon?
		if( m_hImageList && m_nIconNumber != -1 )
		{

			if( TRUE == ImageList_Draw( m_hImageList, m_nIconNumber, hdc, rcTarget.left, rcTarget.top, ILD_NORMAL ) )
				return S_OK;
		}
		
	}



	if( !m_lpViewObject )
		return E_FAIL;

	
	HDC hdcMem = ::CreateCompatibleDC( hdc );
	if( !hdcMem )
		return E_FAIL;

	HBITMAP hBmp = ::CreateCompatibleBitmap( hdc, nWidth, nHeight );
	if( !hBmp )
	{
		::DeleteDC( hdcMem );	hdcMem = 0;
		return E_FAIL;
	}
	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hdcMem, (HGDIOBJ)hBmp );

	POINT ptOldOrg;
	::SetWindowOrgEx( hdcMem, rcTarget.left, rcTarget.top, &ptOldOrg );
	


	double fZoomExtent = GetZoomExtent();	
	SIZEL sizel = GetViewObjectExtent();

	double fZoomX = double(rcTarget.right - rcTarget.left) / double( sizeClip.cx );
	double fZoomY = double(rcTarget.bottom - rcTarget.top) / double( sizeClip.cy );


	//::IntersectClipRect( hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom );

	//Define part do draw in object coordinates
	RECTL rclBounds;
	rclBounds.left = long( double(-ptOffset.x) * fZoomX ) + rcTarget.left;
	rclBounds.top = long( double(-ptOffset.y) * fZoomY ) + rcTarget.top;
	rclBounds.right = rclBounds.left + long( double(sizel.cx) * fZoomX );
	rclBounds.bottom = rclBounds.top + long( double(sizel.cy) * fZoomY );


	
	_brush brush( ::CreateSolidBrush( RGB(255,255,255) ) );
	::FillRect( hdcMem, &rcTarget, brush );
	


	SCODE sc = m_lpViewObject->Draw( nDrawAspect, -1, NULL,
		NULL, hdcMem, hdcMem,
		&rclBounds, NULL, NULL, 0);	

	dbg_assert( sc == S_OK );

	
	::BitBlt( hdc, rcTarget.left, rcTarget.top, nWidth, nHeight, hdcMem, rcTarget.left, rcTarget.top, SRCCOPY );

	::SelectObject( hdcMem, (HGDIOBJ)hOldBmp );

	::DeleteObject( hBmp );	hBmp = 0;

	::DeleteDC( hdcMem );	hdcMem = 0;
	

	return sc;
}

/*
	if( !m_lpViewObject )
		return E_FAIL;

	int nWidth, nHeight;

	nWidth	= rcTarget.right - rcTarget.left;
	nHeight	= rcTarget.bottom - rcTarget.top;

	

	if( nDrawAspect != DVASPECT_CONTENT )
	{
		nDrawAspect = DVASPECT_CONTENT;
		//??Wanna draw icon?
		if( m_hImageList && m_nIconNumber != -1 )
		{

			if( TRUE == ImageList_Draw( m_hImageList, m_nIconNumber, hdc, rcTarget.left, rcTarget.top, ILD_NORMAL ) )
				return S_OK;
		}
		
	}



	if( !m_lpViewObject )
		return E_FAIL;


	double fZoomExtent = GetZoomExtent();	
	SIZEL sizel = GetViewObjectExtent();

	double fZoomX = double(rcTarget.right - rcTarget.left) / double( sizeClip.cx );
	double fZoomY = double(rcTarget.bottom - rcTarget.top) / double( sizeClip.cy );


	//::IntersectClipRect( hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom );

	//Define part do draw in object coordinates
	RECTL rclBounds;
	rclBounds.left = long( double(-ptOffset.x) * fZoomX ) + rcTarget.left;
	rclBounds.top = long( double(-ptOffset.y) * fZoomY ) + rcTarget.top;
	rclBounds.right = rclBounds.left + long( double(sizel.cx) * fZoomX );
	rclBounds.bottom = rclBounds.top + long( double(sizel.cy) * fZoomY );


	SCODE sc = m_lpViewObject->Draw( nDrawAspect, -1, NULL,
		NULL, hdc, hdc,
		&rclBounds, NULL, NULL, 0);	


	return sc;
*/
//////////////////////////////////////////////////////////////////////
double CAxDocContainer::GetZoomExtent()
{
	return 20.0;
}

//////////////////////////////////////////////////////////////////////
SIZEL CAxDocContainer::GetViewObjectExtent()
{
	SIZEL sizel;
	sizel.cx = sizel.cy = 0;

	if( m_lpViewObject )
	{
		m_lpViewObject->GetExtent( DVASPECT_CONTENT, -1, NULL, &sizel );
		sizel.cx = long( double(sizel.cx) / GetZoomExtent() );
		sizel.cy = long( double(sizel.cy) / GetZoomExtent() );

	}

	return sizel;
}

//////////////////////////////////////////////////////////////////////
SIZE CAxDocContainer::GetScrollSize( HWND hWnd )
{
	SIZE size;
	size.cx = size.cy = 0;

	OleObjectType oot = GetOleObjectType( );
	if( oot == ootNotDefined )
		return size;

	if( ( oot == ootInPlaceActivate || oot == ootAxDocument ) && hWnd != NULL && hWnd == m_hWndClient )
		return size;

	SIZEL sizel = GetViewObjectExtent();

	size.cx = sizel.cx;
	size.cy = sizel.cy;


	return size;

}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::FireEventStateChange()
{	
	INotifyControllerPtr ptrNC( m_pOleItemData->m_ptrDocument );

	if( ptrNC == NULL )
		return;

	ptrNC->FireEvent( _bstr_t( szOleItemStateChange ), m_pOleItemData->Unknown(), ptrNC, &m_itemState, sizeof( m_itemState ) );		
}

//////////////////////////////////////////////////////////////////////
HICON CAxDocContainer::GetIconFromRegistry()
{
	if( !m_pIOleObject )
		return NULL;

	CLSID clsid;
	::ZeroMemory( &clsid, sizeof( CLSID ) );

	if( m_pIOleObject->GetUserClassID( &clsid) != S_OK )
		return NULL;

	return ::GetIconFromRegistry( App::handle(), clsid );
}

//////////////////////////////////////////////////////////////////////
bool CAxDocContainer::CreateImageList()
{
	DestroyImageList();

	HICON hIcon = GetIconFromRegistry();
	if( hIcon == NULL )
		return false;

	m_hImageList = ImageList_Create( 32, 32, ILC_MASK, 1, 0 );

	if( m_hImageList == 0 )
	{
		if( hIcon )
			::DestroyIcon( hIcon );	hIcon = 0;

		return false;
	}

	int nIndex = ImageList_AddIcon( m_hImageList, hIcon );

	m_nIconNumber = nIndex;

	::DestroyIcon( hIcon );

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::StoreImageList( IStream *pStream )
{
	ImageList_Write( m_hImageList, pStream );
	ULONG nWritten = 0;
	pStream->Write( &m_nIconNumber, sizeof(int), &nWritten );
}

//////////////////////////////////////////////////////////////////////
void CAxDocContainer::LoadImageList( IStream *pStream )
{
	DestroyImageList();
	m_hImageList = ImageList_Read( pStream );
	ULONG nRead = 0;		
	pStream->Read( &m_nIconNumber, sizeof(int), &nRead );
}


//////////////////////////////////////////////////////////////////////
void CAxDocContainer::DestroyImageList()
{
	if( m_hImageList )	
		ImageList_Destroy( m_hImageList );

	m_hImageList = 0;

	m_nIconNumber = -1;
}
