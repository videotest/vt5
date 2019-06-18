// syncview.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "syncview.h"
#include "Utils.h"
#include "\vt5\common2\misc_calibr.h"

/////////////////////////////////////////////////////////////////////////////
// CSyncviewApp

double GetViewCalibration( IUnknown *punkV )
{
	double	fCalibr;
	::GetDefaultCalibration( &fCalibr, 0 );
	IViewPtr	ptrView( punkV );
	if( ptrView == 0 )return fCalibr;
	long	lpos;

	ptrView->GetFirstVisibleObjectPosition( &lpos );

	if( !lpos )return fCalibr;

	IUnknown	*punkObject = 0;
	ptrView->GetNextVisibleObject( &punkObject, &lpos );
	::GetCalibration( punkObject, &fCalibr, 0 );
	if(punkObject) punkObject->Release();
	return fCalibr;
}

class CSyncviewApp : public CWinApp
{
public:
	CSyncviewApp();
	virtual BOOL InitInstance();
};


CSyncviewApp::CSyncviewApp()
{
}


CSyncviewApp theApp;

BOOL CSyncviewApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	COleObjectFactory::RegisterAll();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CCompRegistrator	ra( szPluginApplication );
	ra.RegisterComponent( "syncview.SyncManager" );

	CActionRegistrator	r;
	r.RegisterActions();

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	ra( szPluginApplication );
	ra.UnRegisterComponent( "syncview.SyncManager" );

	CActionRegistrator	r;
	r.RegisterActions( false );

	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}






IMPLEMENT_DYNCREATE(CSyncManager, CCmdTargetEx);

// {DB4A61C3-42DF-415f-B4C6-ABD6CAD0CC62}
GUARD_IMPLEMENT_OLECREATE(CSyncManager, "syncview.SyncManager", 
0xdb4a61c3, 0x42df, 0x415f, 0xb4, 0xc6, 0xab, 0xd6, 0xca, 0xd0, 0xcc, 0x62);


CSyncManager::CSyncManager() : m_pointCurrentScroll(0, 0), m_pointPrevScroll(0, 0), m_sizeRelative(0, 0)
{
	_OleLockApp( this );
	m_bSyncScroll = false;
	m_bSyncZoom = false;
//	EnableAggregation();
	m_fCurrentZoom = 1;
	m_bLock = false;
	m_bRelative = false;
	m_bSyncContext = false;
	m_fCurrentCalibr = 1;
}

CSyncManager::~CSyncManager()
{
	_OleUnlockApp( this );
}

BEGIN_INTERFACE_MAP(CSyncManager, CCmdTargetEx)
	INTERFACE_PART(CSyncManager, IID_ISyncManager, Sync)
	INTERFACE_PART(CSyncManager, IID_IEventListener, EvList)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CSyncManager, Sync);


HRESULT CSyncManager::XSync::SetSyncContext( BOOL bSync )
{
	METHOD_PROLOGUE_EX(CSyncManager, Sync)
	pThis->m_bSyncContext = (bSync!=FALSE);
	if( pThis->m_bSyncContext )pThis->_SyncContexts();
	return S_OK;
}
HRESULT CSyncManager::XSync::GetSyncContext( BOOL *pbSync )
{
	METHOD_PROLOGUE_EX(CSyncManager, Sync)
	*pbSync = pThis->m_bSyncContext;
	return S_OK;
}
HRESULT CSyncManager::XSync::SetSyncMode( BOOL bEnterSyncMode, BOOL bRelative, BOOL bSyncZoom, IUnknown *punkSyncTargetDoc )
{
	METHOD_PROLOGUE_EX(CSyncManager, Sync)

	pThis->m_bSyncScroll = bEnterSyncMode == TRUE;
	pThis->m_bSyncZoom = bSyncZoom == TRUE;
	pThis->m_bRelative = bRelative == TRUE;

	if( bEnterSyncMode && !bRelative )
	{
		IApplicationPtr	ptrApp( GetAppUnknown() );
		long	lposT = 0;

		ptrApp->GetFirstDocTemplPosition( &lposT );
		while( lposT )
		{
			long	lposD = 0;
			ptrApp->GetFirstDocPosition( lposT, &lposD );
			while( lposD )
			{
				IUnknown *punkD = 0;
				ptrApp->GetNextDoc( lposT, &lposD, &punkD );
				
				IDocumentSitePtr	ptrD( punkD );
				punkD->Release();

				IUnknown	*punkV;

				ptrD->GetActiveView( &punkV );
				if( punkV )
				{
					pThis->_UpdateScrollPosition( punkV );
					punkV->Release();
				}
			}
			ptrApp->GetNextDocTempl( &lposT, 0, 0 );
		}
	}

	if(bRelative)
	{
		pThis->_SetCurrentScroll(0);
		pThis->_CalcRelative();
	}
	return S_OK;
}
HRESULT CSyncManager::XSync::GetSyncMode( BOOL *pbSyncMode, BOOL *pbSyncZoom, BOOL *pbRelative )
{
	METHOD_PROLOGUE_EX(CSyncManager, Sync)

	*pbSyncMode = pThis->m_bSyncScroll;
	*pbSyncZoom = pThis->m_bSyncZoom;
	*pbRelative = pThis->m_bRelative;

	return S_OK;
}


HRESULT CSyncManager::XSync::SetLockSyncMode(BOOL bLock)
{
	METHOD_PROLOGUE_EX(CSyncManager, Sync)

	pThis->m_bLock = bLock == TRUE;
	if(bLock == FALSE)//unlock
	{
		//reset all - for relative mode only
		if(pThis->m_bRelative)
		{
			pThis->_SetCurrentScroll(0);
			pThis->_CalcRelative();
		}
	}
	
	return S_OK;
}



BOOL CSyncManager::OnCreateAggregates()
{
	Register( GetAppUnknown() );
	return CCmdTarget::OnCreateAggregates();
}

void CSyncManager::OnFinalRelease()
{
	UnRegister( GetAppUnknown() );
	delete this;
}

void CSyncManager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventActiveContextChange )||
		!strcmp( pszEvent, szEventActivateObject )||
		!strcmp( pszEvent, szEventActiveObjectChange ) )
	{
		//TRACE( "CSyncManager::OnNotify\n" );
		_SyncContexts();
		_UpdateScrollPosition( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppCreateView ) )
	{
		_AddView( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		_RemoveView( punkFrom );
	}
	else if( !strcmp( pszEvent, szEventScrollPosition ) )
	{
		if(m_bRelative)
		{
			if(!m_bLock)
				_UpdateScrollPosition( punkFrom );
		}
		else
			_UpdateScrollPosition( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppActivateView ) )
	{
		_AttachToView( punkFrom );
		if(m_bRelative && punkFrom && !m_bLock)
			_SetCurrentScroll(punkFrom);
	}

}

void CSyncManager::_AttachToView( IUnknown *punkView )
{
	if( GetObjectKey( punkView  )== GetObjectKey( m_ptrActiveView ) )
		return;

	if( m_ptrActiveView != 0 )
	{
		UnRegister( m_ptrActiveView );
		m_ptrActiveView = 0;
	}
	if( punkView != 0 )
	{
		m_ptrActiveView = punkView;
		Register( m_ptrActiveView );
		_SyncContexts();
	}
}

void CSyncManager::_AddView( IUnknown *punkView )
{
	if( m_ptrViews.Find( punkView ) )
		return;

	m_ptrViews.AddTail( punkView );
	_UpdatePositionForView( punkView );

	if( GetObjectKey( punkView ) != GetObjectKey( m_ptrActiveView ) )
		_AttachToView( punkView );
	
}

void CSyncManager::_RemoveView( IUnknown *punkView )
{
	if( GetObjectKey( punkView ) == GetObjectKey( m_ptrActiveView ) )
		_AttachToView( 0 );

	POSITION	pos = m_ptrViews.Find( punkView );

	if( !pos )return;
	m_ptrViews.RemoveAt( pos );
}

void CSyncManager::_UpdateScrollPosition( IUnknown *punkView )
{
	if( punkView && !CheckInterface( punkView, IID_IImageView ) )
		return;

	static bool bInside = false;

	if( bInside )return;
	bInside = true;

	if( punkView )
	{
		m_fCurrentZoom = ::GetZoom( punkView );
		sptrIScrollZoomSite2	sptrS( punkView );
		if( sptrS != 0 ) sptrS->GetFitDoc(&m_bCurrentFit);
		m_fCurrentCalibr = ::GetViewCalibration( punkView );

		if(m_bRelative )
		{
			IApplicationPtr	sptrA(GetAppUnknown());
			IUnknown* punkDoc = 0;
			sptrA->GetActiveDocument(&punkDoc);
			IDocumentSitePtr strDoc(punkDoc);
			if(punkDoc)
				punkDoc->Release();
			IUnknown* punkActiveView = 0;
			strDoc->GetActiveView(&punkActiveView);
			bool bActiveView = false;
			if(punkActiveView)
			{
				bActiveView = ::GetObjectKey(punkActiveView) == ::GetObjectKey(punkView);
				punkActiveView->Release();
			}

			if(bActiveView)
			{
				m_pointPrevScroll = m_pointCurrentScroll;
				m_pointCurrentScroll = ::GetScrollPos( punkView );
			}
		}
		else
		{
			m_pointCurrentScroll = ::GetScrollPos( punkView );
		}
	}
	
	if( !m_bSyncScroll )
	{
		bInside = false;
		return;
	}
	
	POSITION	pos = m_ptrViews.GetHeadPosition();


	GuidKey	lSrcKey, lDstKey;
	
	IUnknown *punkDoc = ::GetDocumentFromView( punkView );
	if( !punkDoc )
	{
		bInside = false;
		return;
	}
	ASSERT( punkDoc );
	lSrcKey = ::GetObjectKey( punkDoc );
	punkDoc->Release();



	while( pos )
	{
		IUnknown *punk = (IUnknown *)m_ptrViews.GetNext( pos );
		if( punkView == punk )
			continue;

		punkDoc = ::GetDocumentFromView( punk );
		ASSERT( punkDoc );
		lDstKey = ::GetObjectKey( punkDoc );
		punkDoc->Release();

		if( lDstKey != lSrcKey )
			continue;

		_UpdatePositionForView( punk );
	}
	bInside = false;
}

void CSyncManager::_UpdatePositionForView( IUnknown *punkView )
{
	//if( m_bInsideSyncMode
	if( !CheckInterface( punkView, IID_IImageView ) )
		return;

	IScrollZoomSitePtr	ptrZoomScroll( punkView );

	if( m_bSyncZoom )
	{
		double	fCalibr = ::GetViewCalibration( punkView );
		double	fZoom = m_fCurrentZoom*max(fCalibr, 1e-10 )/max(m_fCurrentCalibr, 1e-10);

		ptrZoomScroll->SetZoom( fZoom );
		ptrZoomScroll->SetScrollPos( m_pointCurrentScroll );
	}
	else
	{
		CPoint	point;
		double	fCurrentZoom = ::GetZoom( ptrZoomScroll );

		CPoint pointScroll = ::GetScrollPos( ptrZoomScroll );

		if(m_bRelative)
		{
			bool bNegative = m_nActiveKey != ::GetObjectKey(punkView);	


			CSize size;
			if(bNegative)
				size = m_pointCurrentScroll - pointScroll;
			else
				size = pointScroll - m_pointCurrentScroll;
			
			CSize sizeOffset = m_pointPrevScroll-m_pointCurrentScroll;

			//TRACE("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%m_pointCurrentScroll=%d\n", m_pointCurrentScroll.y);
			//TRACE("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%pointScrollA=%d\n", pointScroll.y);
			//TRACE("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%sizeY=%d\n", size.cy);

			CSize sizeInsuff;
			if(bNegative)
				sizeInsuff = m_sizeRelative - size;
			else
				sizeInsuff = size - m_sizeRelative;

			if(size.cx == 0)
				sizeInsuff.cx = 0;
			if(size.cy == 0)
				sizeInsuff.cy = 0;
				
				
			
			pointScroll -= sizeOffset + sizeInsuff;
		}
		else
			pointScroll = m_pointCurrentScroll;

		point.x = (pointScroll.x*fCurrentZoom/m_fCurrentZoom);
		point.y = (pointScroll.y*fCurrentZoom/m_fCurrentZoom);

		ptrZoomScroll->SetScrollPos( point );
	}
}

/*bool CSyncManager::_IsImageView( IUnknown *punkView )
{
	IViewSitePtr	ptrView( punkView );
	IUnknown	*punk = 0;
	ptrView->
}*/


void CSyncManager::_SetCurrentScroll(IUnknown* punkView)
{
	bool bRelease = false;
	if(!punkView)
	{
		IApplicationPtr	sptrA(GetAppUnknown());
		IUnknown* punkDoc = 0;
		sptrA->GetActiveDocument(&punkDoc);
		IDocumentSitePtr strDoc(punkDoc);
		if(punkDoc)
			punkDoc->Release();
		strDoc->GetActiveView(&punkView);
		bRelease = true;
	}

	if(punkView)
	{
		m_pointCurrentScroll = ::GetScrollPos( punkView );
		if(bRelease)
			punkView->Release();
	}
}

void CSyncManager::_CalcRelative()
{
	//relative between scrollbars of 2 views
	POSITION	pos = m_ptrViews.GetHeadPosition();

	IUnknown *punkV = 0;

	IApplicationPtr	sptrA(GetAppUnknown());
	IUnknown* punkDoc = 0;
	sptrA->GetActiveDocument(&punkDoc);
	IDocumentSitePtr	sptrD( punkDoc );
				
	IUnknown	*punkAV = 0;
	sptrD->GetActiveView( &punkAV );
	if( punkAV )
	{
		m_nActiveKey = ::GetObjectKey(punkAV);	
		punkAV->Release();
	}

	
	
	CPoint pointScroll[2];

	ZeroMemory(&pointScroll, sizeof(CPoint)*2);

	bool bFirstActive = false;

	int nIdx = 0;
	while(pos)
	{
		punkV = (IUnknown *)m_ptrViews.GetNext( pos );
		IUnknown* punkD = 0;
		IViewPtr sptrV(punkV);
		if(sptrV)
		{
			sptrV->GetDocument(&punkD);
			if(punkD == punkDoc)
			{
				if(nIdx == 0)
					bFirstActive = m_nActiveKey == ::GetObjectKey(punkV);	
				if(nIdx < 2)
				{
					pointScroll[nIdx++] = ::GetScrollPos(punkV);
				}
			}
			if(punkD)
				punkD->Release();
		}
	}

	if(!bFirstActive)
		m_sizeRelative = pointScroll[1] - pointScroll[0];
	else
		m_sizeRelative = pointScroll[0] - pointScroll[1];

	punkDoc->Release();
}

void SafeSelect( IDataContext3	*punkContext, BSTR bstrType, IUnknown *punkDataObject )
{
	IUnknown	*punkObjectSelected;
	punkContext->GetActiveObject( bstrType, &punkObjectSelected );

	if( ::GetObjectKey( punkObjectSelected ) != ::GetObjectKey( punkDataObject ) )
		punkContext->SetActiveObject( bstrType, punkDataObject, 0 );

	if( punkObjectSelected  )punkObjectSelected ->Release();
}

void CSyncManager::_SyncContexts()
{
	if( !m_bSyncContext )
		return;
	IDataContext3Ptr	ptrSourceContext( m_ptrActiveView );
	if( ptrSourceContext == 0 )return;

	static bool bInside = false;
	if( bInside )return;
	bInside = true;

	IUnknown	*punkDoc = ::GetDocumentFromView( m_ptrActiveView );
	ASSERT( punkDoc );
	IDataTypeManagerPtr	ptrMan( punkDoc );
	INamedDataPtr		ptrNamedData( punkDoc );
	punkDoc->Release();

	long	lTypesCount = 0;
	ptrMan->GetTypesCount( &lTypesCount );

	GuidKey	keyView = ::GetObjectKey( m_ptrActiveView );
	GuidKey	keyDoc = ::GetObjectKey( ptrMan );

	POSITION	pos = m_ptrViews.GetHeadPosition();
	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_ptrViews.GetNext( pos );

		if( keyView == GetObjectKey( punk ) )
			continue;

		IUnknown	*punkDoc = ::GetDocumentFromView( punk );
		GuidKey	keyDocTest = ::GetObjectKey( punkDoc );
		if( punkDoc )punkDoc->Release();

		if( keyDoc != keyDocTest )
			continue;

		//sync here
		IDataContext3Ptr	ptrDestContext( punk );
		IViewPtr			ptrView( punk );

		BOOL	bOldLock = 0;

		ptrDestContext->GetLockUpdate( &bOldLock );
		ptrDestContext->LockUpdate( true, false );

		for( int nType = 0; nType < lTypesCount; nType++ )
		{
			BSTR	bstrType;
			ptrMan->GetType( nType, &bstrType );

			DWORD	dwMatch = 0;
			DWORD	dwBaseMatch = 0;

			IUnknown	*punkSelectedObject = 0;
			IUnknown	*punkBaseObject = 0;

			ptrSourceContext->GetActiveObject( bstrType, &punkSelectedObject );

			if( punkSelectedObject )
			{
				CString sName = ::GetObjectName(punkSelectedObject );
				GuidKey	keyBase;
				INamedDataObject2Ptr	ptrNamedObject( punkSelectedObject );
				ptrNamedObject->GetBaseKey( &keyBase );

				ptrNamedData->GetBaseGroupBaseObject( &keyBase, &punkBaseObject );
			
				_bstr_t	bstrBaseType = ::GetObjectKind( punkBaseObject );
				CString sBaseName = ::GetObjectName(punkBaseObject);
				ptrView->GetMatchType( bstrType, &dwMatch );
				ptrView->GetMatchType( bstrBaseType, &dwBaseMatch );
				

				// A.M. BT4872
				/*if( dwMatch < dwBaseMatch )
				{
					ASSERT( punkBaseObject );
					SafeSelect( ptrDestContext, bstrBaseType, punkBaseObject );
				}*/
				SafeSelect( ptrDestContext, bstrType, punkSelectedObject );

				if( punkSelectedObject )punkSelectedObject->Release();
				if( punkBaseObject )punkBaseObject->Release();
			}
			else
				ptrDestContext->SetActiveObject( bstrType, 0, 0 );

			

			::SysFreeString( bstrType );
		}

		ptrDestContext->LockUpdate( bOldLock, true );
	}
	bInside = false;
}

