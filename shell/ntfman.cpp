#include "stdafx.h"
#include "ntfman.h"

#include "shelldoc.h"
#include "shellview.h"

CNotifyManager	g_NtfManager;

BEGIN_INTERFACE_MAP(CNotifyManager, CCmdTargetEx)
	INTERFACE_PART(CNotifyManager, IID_IOperationManager, Man)
	INTERFACE_PART(CNotifyManager, IID_IOperationManager2, Man)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CNotifyManager, Man)




/////class support single connection between long operation and view
CLongOperationListener::CLongOperationListener( IUnknown *punkLongOperation, CTypedPtrList<CPtrList, IUnknown*>& listPermanents )
{
	m_ptrLongOperation = punkLongOperation;
	IUndoneableActionPtr	ptrUAction( punkLongOperation );

	GuidKey lTargetKey;
	
	if( ptrUAction != 0 )
		ptrUAction->GetTargetKey( &lTargetKey );


	//todo:scan documents here
	/*POSITION	posTempl = AfxGetApp()->GetFirstDocTemplatePosition();

	while( posTempl )
	{
		CDocTemplate	*ptempl = AfxGetApp()->GetNextDocTemplate( posTempl );

		POSITION posDoc = ptempl->GetFirstDocPosition();

		while( posDoc )
		{
			CShellDoc	*pdoc = (CShellDoc*)ptempl->GetNextDoc( posDoc );

			if( GetObjectKey( pdoc->GetControllingUnknown() ) != lTargetKey )
				continue;

			//POSITION	posView = pdoc->GetFirstViewPosition();

			//while( posView ) 
			{
				//CView	*pview = pdoc->GetNextView( posView );
				CView	*pview = pdoc->GetActiveView();

				_AddListenersToSite( pview->GetControllingUnknown() );
			}
		}
	}*/

	POSITION	pos = listPermanents.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = listPermanents.GetNext( pos );
		_AddListenersToSite( punk );
	}
}

void CLongOperationListener::_AddListenersToSite( IUnknown *punkSite )
{
	IFilterActionPtr	ptrFilter( m_ptrLongOperation );

	if( CheckInterface( punkSite, IID_IPreviewSite ) )
	{
		bool bFirstCallForThisView = true;
		IPreviewSitePtr	ptrPreview( punkSite );

		long	lPreviewObjectPos = 0;
		ptrFilter->GetFirstArgumentPosition( &lPreviewObjectPos );

		while( lPreviewObjectPos )
		{
			BOOL	bOut = false;
			IUnknown	*punkObject = 0;

			ptrFilter->GetArgumentInfo( lPreviewObjectPos, 0, 0, &bOut );
			ptrFilter->GetNextArgument( &punkObject, &lPreviewObjectPos );

			if( !punkObject )
				continue;

			if( !bOut )
			{
				punkObject->Release();
				continue;
			}

			BOOL	bCanDisplay = false;
			ptrPreview->CanThisObjectBeDisplayed( punkObject, &bCanDisplay );

			if( !bCanDisplay )
			{
				punkObject->Release();
				continue;
			}

			if( bFirstCallForThisView )
			{
				IUnknown	*punkView = ptrPreview;
				punkView->AddRef();
				m_listViews.AddTail( punkView );

				ptrPreview->InitPreview( m_ptrLongOperation );

				bFirstCallForThisView = false;
			}
			
			ptrPreview->AddPreviewObject( punkObject );


			punkObject->Release();
		}

		if( bFirstCallForThisView )
		{
			IUnknown	*punkView = ptrPreview;
			punkView->AddRef();
			m_listViews.AddTail( punkView );

			ptrPreview->InitPreview( m_ptrLongOperation );

			bFirstCallForThisView = false;
		}
	}
}

CLongOperationListener::~CLongOperationListener()
{
	POSITION	pos = m_listViews.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = m_listViews.GetNext( pos );

		IPreviewSitePtr	ptrSite( punk );
		punk->Release();

		ptrSite->DeInitPreview();
	}

	m_listViews.RemoveAll();
}

void CLongOperationListener::RouteTerminateMessage()
{
	int	nStage, nPosition;

	m_ptrLongOperation->GetCurrentPosition( &nStage, &nPosition, 0 );

	POSITION	pos = m_listViews.GetHeadPosition();

	while( pos )
	{
		IPreviewSitePtr	ptrPreview( m_listViews.GetNext( pos ) );
		ptrPreview->TerminatePreview();
	}
}

void CLongOperationListener::RouteMessage()
{
	int	nStage, nPosition;

	m_ptrLongOperation->GetCurrentPosition( &nStage, &nPosition, 0 );

	POSITION	pos = m_listViews.GetHeadPosition();

	while( pos )
	{
		IPreviewSitePtr	ptrPreview( m_listViews.GetNext( pos ) );
		ptrPreview->ProgressPreview( nStage, nPosition );
	}
}



////////////////////////////////////////////////////////////////////////////////////////
//class manages connections list
CNotifyManager::CNotifyManager()
{
	m_lTerminateFlag = 0;
}
CNotifyManager::~CNotifyManager()
{
	ASSERT( m_listListeners.GetCount() == 0 );
	ASSERT( m_mapListeners.GetCount() == 0 );

	POSITION	pos = m_listPermanents.GetHeadPosition();
	while( pos ) m_listPermanents.GetNext( pos )->Release();
	m_listPermanents.RemoveAll();
}

void CNotifyManager::ProcessMessage( IUnknown *punkOperation, LongOperationAction loa )
{
	MSG	msg;

	DWORD dwStartTime = GetTickCount();
	while( GetTickCount() < dwStartTime+100 && ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_CHAR && msg.wParam == VK_ESCAPE || m_lTerminateFlag > 0)
		{
			ILongOperationPtr	ptrLong( punkOperation );
			ptrLong->Abort();

			// 05.04.2006 build 102
			if( msg.message == WM_CHAR && msg.wParam == VK_ESCAPE )
			{ // если прерываем по Esc - послать сообщение
				IApplicationPtr	ptrA( GetAppUnknown() );
				::FireEvent( ptrA, szEventAbortLongOperation );
			}

			if (m_lTerminateFlag == 2)
				m_lTerminateFlag = 0;
			TRACE("Terminated\n");
		}


		/*if( msg.message == WM_PAINT )
			continue;*/
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	if( loa == loaSetPos )
	{
		POSITION	pos = _GetOperationPos( punkOperation );
		if( pos )
		{
			CLongOperationListener	*plistener = m_listListeners.GetAt( pos );
			plistener->RouteMessage();
		}
	}
	else
	if( loa == loaStart)
		_AddOperation( punkOperation );
	else
	if( loa == loaTerminate || loa == loaComplete)
		_RemoveOperation( punkOperation, loa == loaComplete );

	
}

void CNotifyManager::_AddOperation( IUnknown *punkOperation )
{
	if( !CheckInterface( punkOperation, IID_IFilterAction ) )
		return;

	ASSERT( !_GetOperationPos( punkOperation ) );

	CLongOperationListener	*plistener = new CLongOperationListener( punkOperation, m_listPermanents );
	if( plistener->IsEmpty() )
	{
		delete plistener;
		return;
	}
	POSITION pos = 
		m_listListeners.AddTail( plistener );
	m_mapListeners[punkOperation] = pos;
}

void CNotifyManager::_RemoveOperation( IUnknown *punkOperation, bool bSuccess )
{
	POSITION	pos = _GetOperationPos( punkOperation );

	if( !pos )
		return;

	CLongOperationListener	*plistener = m_listListeners.GetAt( pos );

	if( !bSuccess )
		plistener->RouteTerminateMessage();

	m_listListeners.RemoveAt( pos );
	m_mapListeners.RemoveKey( punkOperation );

	delete plistener;
}

POSITION CNotifyManager::_GetOperationPos( IUnknown *punkOperation )
{
	void	*pos;

	if( !m_mapListeners.Lookup( punkOperation, (void*&)pos ) )
		return 0;
	
	return (POSITION)pos;
}

HRESULT CNotifyManager::XMan::RegisterPermanent( IUnknown *punk )
{
	_try_nested(CNotifyManager, Man, RegisterPermanent)
	{
		pThis->m_listPermanents.AddTail( punk );
		punk->AddRef();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CNotifyManager::XMan::UnRegisterPermanent( IUnknown *punk )
{
	_try_nested(CNotifyManager, Man, UnRegisterPermanent)
	{
		POSITION	pos = pThis->m_listPermanents.Find( punk );
		if( !pos )return E_INVALIDARG;

		pThis->m_listPermanents.RemoveAt( pos );

		POSITION	pos1 = pThis->m_listListeners.GetHeadPosition();
		
		while( pos1 )
		{
			CLongOperationListener	*plistener = pThis->m_listListeners.GetNext( pos1 );

			POSITION	pos2 = plistener->GetList().GetHeadPosition();

			while( pos2 )
			{
				POSITION	posS = pos2;
				IUnknown	*punkL = plistener->GetList().GetNext( pos2 );

				if( GetObjectKey( punk ) == GetObjectKey( punkL ) )
				{
					IPreviewSitePtr	ptrSite( punkL );
					ptrSite->DeInitPreview();
					punkL->Release();
					plistener->GetList().RemoveAt( posS );
				}
			}
		}

		punk->Release();
		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CNotifyManager::XMan::TerminateCurrentOperation(long lTerminateFlag)
{
	_try_nested(CNotifyManager, Man, TerminateCurrentOperation)
	{
		pThis->m_lTerminateFlag = lTerminateFlag;
	}
	_catch_nested;
	return S_OK;
}
