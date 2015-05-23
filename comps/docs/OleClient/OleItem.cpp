#include "stdafx.h"

#include "OleItem.h"

#include "Core5.h"
#include "misc_utils.h"
#include "ole_utils.h"
#include "com_main.h"
#include "resource.h"

#include "nameconsts.h"

//////////////////////////////////////////////////////////////////////
//
//	class COleItemDataInfo
//
//
HRESULT COleItemDataInfo::GetUserName( BSTR *pbstr )
{
	if( !pbstr ) 
		return E_POINTER;

	if( !m_bstrUserName.length() )
	{
		char sz_buf[256];
		sz_buf[0] = 0;

		::LoadString( App::handle(), IDS_OLE_OBJECT_TYPE_NAME, sz_buf, sizeof(sz_buf) );
		
		m_bstrUserName	= sz_buf;
	}

	*pbstr = m_bstrUserName.copy();

	return S_OK;//CDataInfoBase::GetUserName( pbstr );
}




//////////////////////////////////////////////////////////////////////
//
//	class COleItemData
//
//
COleItemData::COleItemData() : IDispatchImpl<IOleItemDataDisp>("OleClient.tlb")
{
	m_pContainer = NULL;
	m_pContainer = new CAxDocContainer( this );	
	InitDefaults();
}

//////////////////////////////////////////////////////////////////////
void COleItemData::InitDefaults()
{
	m_bstrFileName = "";
	m_bstrAppName =  "";
	m_creationType = octUndefine;
	::ZeroMemory( &m_appGuid, sizeof(GUID) );

	m_bEmpty = TRUE;
	
	m_bSucceededReadStorage = FALSE;
	m_bSucceededCreateServer = FALSE;	

	m_hwndView = NULL;
	
}

//////////////////////////////////////////////////////////////////////
COleItemData::~COleItemData()
{
	if( m_pContainer )
	{
		m_pContainer->Release();
		m_pContainer = NULL;
	}

	Free();
}

//////////////////////////////////////////////////////////////////////
void COleItemData::Free()
{
}


//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::IsModified( BOOL *pbModified )
{	
	BOOL bModified = FALSE;
	
	HRESULT hRes = CObjectBase::IsModified( &bModified );
	
	if( hRes != S_OK )
	{
		if( pbModified )
			*pbModified = bModified;

		return hRes;
	}

	if( !pbModified )
		return E_INVALIDARG;

	if( !bModified )
	{
		if( m_pContainer )
			bModified = ( m_pContainer->IsModifiedServerData() == true );
	}

	*pbModified = bModified;

	return S_OK;
}

HRESULT COleItemData::IsEmpty( /*[out, retval]*/ VARIANT_BOOL *pbIsEmpty )
{
	*pbIsEmpty = m_creationType==octUndefine?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}


//ISerializableObject
//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::Load( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;

	if(!pStream)
		return E_INVALIDARG;

	Free();
	InitDefaults();

	ULONG nRead = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	//Item properties
	{
		//File name				
		m_bstrFileName = ::LoadStringFromStream( pStream );		

		//App name				
		m_bstrAppName = ::LoadStringFromStream( pStream );		

		//Creation type
		short n = 0;
		pStream->Read( &n, sizeof(short), &nRead );
		m_creationType = (OleCreationType)n;

		//App guid
		pStream->Read( &m_appGuid, sizeof(GUID), &nRead );		

		//stote object
		m_pContainer->LoadDataBuf( pStream );

		if( nVersion >= 2 )
			m_pContainer->LoadImageList(  pStream );

		m_pContainer->CreateOleObject( );
	}

	SetModifiedFlag( FALSE );
	m_pContainer->SetModifiedServerData( false );

	return S_OK;
}

HRESULT COleItemData::Store( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( S_OK != hr )
		return hr;

	if(!pStream)
		return E_INVALIDARG;

	ULONG nWritten = 0;

	//At first version control
	long nVersion  = 2;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	//Item properties
	{	
		//File name
		::StoreStringToStream( pStream, m_bstrFileName );

		//App name
		::StoreStringToStream( pStream, m_bstrAppName );
		
		//Creation type
		short n = (short)m_creationType;
		pStream->Write( &n, sizeof(short), &nWritten );		

		//App guid
		pStream->Write( &m_appGuid, sizeof(GUID), &nWritten );		

		//stote object
		m_pContainer->StoreDataBuf( pStream );
		
		m_pContainer->StoreImageList(  pStream );		
	}
	m_pContainer->SetModifiedServerData( false );


	return S_OK;

}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::AttachData( IUnknown *punkNamedData )	//called from NamedData
{
	HRESULT hr = CObjectBase::AttachData( punkNamedData );
	m_ptrDocument = punkNamedData;

	return hr;
}
//INamedDataObject2
//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = TRUE;
	return S_OK;
}

//IOleObjectData
//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::CreateItem( OLEUIINSERTOBJECT* pio )
{
	Free();
	InitDefaults();

	m_pContainer->Free();

	if( pio )
	{
		memcpy( &m_appGuid, &pio->clsid, sizeof(GUID) );

		BSTR bstrAppName = 0;
		if( S_OK == ::ProgIDFromCLSID( m_appGuid, &bstrAppName ) )
		{
			m_bstrAppName = bstrAppName;
			::CoTaskMemFree( bstrAppName );
		}
	}
	
	if( pio->lpszFile )
	{
		m_bstrFileName = pio->lpszFile;

	}

	if( pio->dwFlags & IOF_SELECTCREATEFROMFILE )
	{
		if( pio->dwFlags & IOF_CHECKLINK )
			m_creationType = octLinkToFile;
		else
			m_creationType = octInsertFromFile;
	}
	else
		m_creationType = octCreateNewItem;

	SetModifiedFlag( TRUE );
	

	return (m_pContainer->CreateOleObject( ) ? S_OK : S_FALSE );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::CreateFromClipboard()
{
	Free();
	InitDefaults();

	m_creationType = octCreateFromClipboard;	

	SetModifiedFlag( TRUE );

	return (m_pContainer->CreateOleObject( ) ? S_OK : S_FALSE );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::GetItemInfo( BSTR* bstrFileName, short* pCreationType, BSTR* bstrAppProgID )
{
	if( bstrFileName )
		*bstrFileName = m_bstrFileName.copy();

	if( pCreationType )
		*pCreationType = (short)m_creationType;

	if( bstrAppProgID )
		*bstrAppProgID = m_bstrAppName.copy();
	
	return S_OK;
}



//IOleObjectUI
//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::Activate( HWND hWndView )
{
	CHourglass wait;

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF == 0 )
		return E_FAIL;

	HWND hWndMain = NULL;
	ptrOF->GetMainHwnd( &hWndMain );

	if( !hWndMain )
		return E_FAIL;
	
	if( !m_pContainer->Activate( hWndMain, hWndView ) )
		return E_FAIL;	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::Deactivate( HWND hWndView )
{
	return ( m_pContainer->Deactivate( hWndView ) ? S_OK : E_FAIL );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::ActivateUI( HWND hWndView )
{
	return ( m_pContainer->ActivateUI( hWndView ) ? S_OK : E_FAIL );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::DeactivateUI( HWND hWndView )
{
	return ( m_pContainer->DeactivateUI( hWndView ) ? S_OK : E_FAIL );	
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::OnSize( int cx, int cy )
{
	m_pContainer->OnResizeClient( cx, cy );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::IsObjectCreated( BOOL* pbCreated )
{
	*pbCreated = ( m_pContainer->m_bCreateOleObject == true );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::DrawMeta( int nDrawAspect, HWND hWnd, HDC hdc, RECT rcTarget, POINT ptOffset, SIZE sizeClip )
{		
	return m_pContainer->DrawMeta( nDrawAspect, hWnd, hdc, rcTarget, ptOffset, sizeClip );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::GetInPlaceEditHwnd( HWND* phWnd )
{
	*phWnd = NULL;
	if( m_pContainer->GetItemState() != iaFullActivate )
		return S_OK;

	*phWnd = m_pContainer->GetInplaceActiveHwnd();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::GetScrollSize( SIZE* pSize, HWND hWnd  )
{
	*pSize = m_pContainer->GetScrollSize( hWnd );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::GetExtent( SIZEL* psizel )
{
	psizel->cx = psizel->cy = 0;

	if( !m_pContainer->m_pSite )
		return S_OK;

	*psizel = m_pContainer->GetViewObjectExtent();

	return S_OK;

}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::GetState( short* pnState )
{
	*pnState = m_pContainer->m_itemState;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::CanProcessDrawig( HWND hWnd, BOOL* pbProcess )
{
	bool bRes = m_pContainer->CanProcessDrawig( hWnd );
	*pbProcess = ( bRes == true );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::OnFrameResizeBorder( RECT rcBorder )
{
	if( m_pContainer->GetItemState() != iaFullActivate )
		return S_OK;

	IOleInPlaceActiveObjectPtr ptrW( m_pContainer->m_pIOleIPObject );
	if( ptrW )
		ptrW->ResizeBorder( &rcBorder, (IOleInPlaceUIWindow*)m_pContainer, TRUE );		
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::OnFrameActivate( BOOL bActivate )
{
	if( m_pContainer->GetItemState() != iaFullActivate )
		return S_OK;


	IOleInPlaceActiveObjectPtr ptrW( m_pContainer->m_pIOleIPObject );
	if( ptrW )
		ptrW->OnFrameWindowActivate( bActivate );
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::DoVerb( UINT uiVerb )
{
	return m_pContainer->DoVerb( uiVerb );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleItemData::ForceUpdate()
{
	return m_pContainer->ForceUpdate( );
}

HRESULT COleItemData::OnOwnerActivate( )
{
	
	if( m_pContainer->m_itemState == iaFullActivate )
	{
		if( m_pContainer->m_pIOleIPActiveObject )
			m_pContainer->m_pIOleIPActiveObject->OnDocWindowActivate( true );		
	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool COleItemData::FireEventObjectWasChange()
{
	IUnknown* punkDoc = 0;
	GetData( &punkDoc );
	if( !punkDoc )
		return false;

	INotifyControllerPtr ptrNC( punkDoc );
	punkDoc->Release();	punkDoc = 0;

	if( ptrNC == 0 )
		return false;

	ptrNC->FireEvent( _bstr_t( szEventChangeObject ), 0, Unknown(), 0, 0 );

	return true;
}


