// ActionHelper.cpp : implementation file
//

/*
  test script:
ActionManager.Clone
ActionManager.Negative
FormManager.ExecuteDockBar "BrightnessContrast"
*/

#include "stdafx.h"
#include "axform.h"
#include "ActionHelper.h"
#include "ActiveXSite.h"

#include "dbase.h"



CPreviewData::CPreviewData()
{
	EnableAggregation();
	m_ptrAggrData = 0;
	m_ptrAggrContext = 0;
}

void CPreviewData::Init()
{
	CLSID	clsidD, clsidC;
	_bstr_t	bstrD( szNamedDataProgID ), bstrC( szContextProgID );

	VERIFY( ::CLSIDFromProgID( bstrD, &clsidD ) == S_OK );
	VERIFY( ::CLSIDFromProgID( bstrC, &clsidC ) == S_OK );

	VERIFY( ::CoCreateInstance( clsidD, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_ptrAggrData ) == S_OK );
	VERIFY( ::CoCreateInstance( clsidC, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_ptrAggrContext ) == S_OK );

	/*INamedDataPtr	ptrN( GetControllingUnknown() );
	*/
	IDataContextPtr	ptrC( GetControllingUnknown() );
	ptrC->AttachData( GetControllingUnknown() );
}
CPreviewData::~CPreviewData()
{
	if( m_ptrAggrContext )m_ptrAggrContext->Release();
	if( m_ptrAggrData )m_ptrAggrData->Release();
}

BEGIN_INTERFACE_MAP(CPreviewData, CCmdTarget)
	INTERFACE_PART(CPreviewData, IID_INumeredObject, Num)
	INTERFACE_AGGREGATE(CPreviewData, m_ptrAggrData)
	INTERFACE_AGGREGATE(CPreviewData, m_ptrAggrContext)
END_INTERFACE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CActionHelper
bool	g_bTerminate = false;

IMPLEMENT_DYNCREATE(CActionHelper, CCmdTargetEx)

CActionHelper::CActionHelper()
{
	m_lExecuteActionCount = 0;
	m_pPreviewTarget = new CPreviewData();
	m_bInitialized = false;
	m_sName = "ActionHelper";
	m_bRunning = false;
	m_bTerminated =  false;
	m_bAllowTerminate = true;
	m_bApplyOnComplete = false;
	m_bRemoveOnComplete = false;
	m_bLockUpdate = false;
	m_pSiteButton = 0;
	m_psite = 0;
	m_bDelayEnableButtons = false;
	m_bHelperInGroup = false;
	m_nAction = -1; 
	m_bPtrCurrActionsChanged = false;
}

CActionHelper::~CActionHelper()
{
	if( m_bInitialized )
	{
		RegisterDocListener( false );
		UnRegister();
		DeActivatePreview();

		DoRemovePreview();
		DeInitArgs();
		delete_saved_args_arr();
	}

	if( m_pPreviewTarget )
	{
		{
			IDataContextPtr	ptrC( m_pPreviewTarget->GetControllingUnknown() );
			if( ptrC != 0 )ptrC->AttachData( 0 );
		}
		m_pPreviewTarget->GetControllingUnknown()->Release();
		m_pPreviewTarget = 0;
	}
	FreeSiteButtons();
}


BEGIN_INTERFACE_MAP(CActionHelper, CCmdTargetEx)
	INTERFACE_PART(CActionHelper, IID_IDispatch, Dispatch)
	INTERFACE_PART(CActionHelper, IID_IEventListener, EvList)
	INTERFACE_PART(CActionHelper, IID_INamedObject2, Name)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActionHelper, Dispatch);

/////////////////////////////////////////////////////////////////////////////
// CActionHelper message handlers

void CActionHelper::DeInit()
{
	if( !m_bInitialized )
		return;

	FreeSiteButtons();

	//if( m_ptrCurrentAction != 0 )
	if( !m_arrPtrCurrActions.IsEmpty() )
	{
		g_bTerminate = true;
		if( !m_bApplyOnComplete )
		{
			for( int i = 0; i < m_arrPtrCurrActions.GetSize(); i++ )
			{
				ILongOperationPtr	ptrL( m_arrPtrCurrActions.GetAt( i ) );
				ptrL->Abort();
			}

			
			if(0) if( m_arrPtrCurrActions == m_arrPtrWaitActions )
				m_arrPtrWaitActions.RemoveAll();

			for( int i = 0; i < m_arrPtrCurrActions.GetSize(); i++ )
			{
				IActionPtr	ptrA( m_arrPtrCurrActions.GetAt( i ) );
				ptrA->AttachTarget( m_ptrActiveDocument );
			}

			/*ILongOperationPtr	ptrL( m_ptrCurrentAction );
			ptrL->Abort();

			if( (IUnknown*)m_ptrCurrentAction == (IUnknown*)m_ptrWaitAction )
				m_ptrWaitAction = 0;

			IActionPtr	ptrA( m_ptrCurrentAction );
			ptrA->AttachTarget( m_ptrActiveDocument );*/

			m_bRemoveOnComplete = true;
		}

		FinalizeCurrentAction( false );
		//DoApplyPreview();
	}

	RegisterDocListener( false );
	DeActivatePreview();
	DoRemovePreview();
	DeInitArgs();

	m_ptrActionManagerDispatch = 0;
	m_ptrActionManager = 0;
	AttachPreviewView( 0 );

	UnRegister();
}

void CActionHelper::Init( CActiveXSite *psite )
{
	if( m_bInitialized )return;

	m_bInitialized = true;
	m_psite = psite;

	InitSiteButtons();
	m_pPreviewTarget->Init();

	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkAM = 0;
	ptrApp->GetActionManager( &punkAM );

	m_ptrActionManagerDispatch = punkAM;
	m_ptrActionManager = punkAM;

	punkAM->Release();

	IUnknown	*punkDocument = 0;
	ptrApp->GetActiveDocument( &punkDocument );

	if( punkDocument )
	{
		IDocumentSitePtr	ptrDocSite = punkDocument;
		punkDocument->Release();

		IUnknown	*punkView = 0;
		ptrDocSite->GetActiveView( &punkView );

		if( punkView )
		{
			AttachPreviewView( punkView );
			punkView->Release();
		}
	}

	Register();
	RegisterDocListener( true );

	if( m_ptrPreviewView )
		m_ptrPreviewView->AttachPreviewData( (*m_pPreviewTarget) );
}


///////////////////////////////////////////////////////////////////////////////
//IDispatch implementation. Handy made!!!
HRESULT CActionHelper::XDispatch::GetTypeInfoCount( UINT *pctinfo )
{
	return E_NOTIMPL;
}
        
HRESULT CActionHelper::XDispatch::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOTIMPL;
}

HRESULT CActionHelper::XDispatch::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, 
											UINT cNames, LCID lcid, DISPID *rgDispId )
{
	_try_nested(CActionHelper, Dispatch, GetIDsOfNames)
	{	
		if( !pThis->m_bInitialized )
		{
			AfxMessageBox( "You have to call Form.InitializePreview for use ActionHelper" );
			return E_INVALIDARG;
		}
		return pThis->m_ptrActionManagerDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId );
	}
	_catch_nested;
}

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

static bool g_bInsideInvoke = false;

HRESULT CActionHelper::XDispatch::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	_try_nested(CActionHelper, Dispatch, Invoke)
	{
		// 1. Если не инициализировали превью - вообще не работаем
		if( !pThis->m_bInitialized )
		{
			AfxMessageBox( "You have to call Form.InitializePreview for use ActionHelper" );
			return S_OK;
		}

		// 2. Предотвратим рекурсивный вызов
		if( g_bInsideInvoke )
			return S_OK;
		CLockerFlag	locker( &g_bInsideInvoke );

		// 3. Если уже работаем (находимся внутри таймера и ExecuteCurrentAction) - прервать текущую акцию
		if (pThis->m_bRunning)
			pThis->TerminateCurrentAction();

		// 4. Если мы не в процессе набора группы - убрать результаты прошлого прохода
		pThis->DoRemovePreview();

		// 5. get action info
		IUnknown	*punkActionInfo = 0;
		pThis->m_ptrActionManager->GetActionInfo( dispIdMember, &punkActionInfo );

		if( !punkActionInfo )
		{
			return E_INVALIDARG;
		}

		IActionInfoPtr	ptrInfo( punkActionInfo );
		punkActionInfo->Release();
//get target manager
		/*IApplicationPtr	ptrApp( GetAppUnknown() );

		IUnknown	*punkTargetMan = 0;
		ptrApp->GetTargetManager( &punkTargetMan );
		IActionTargetManagerPtr	ptrTargetMan( punkTargetMan );
		punkTargetMan->Release();
//get target for action
		BSTR	bstrType;
		ptrInfo->GetTargetName( &bstrType );
		IUnknown	*punkTarget = 0;
		ptrTargetMan->GetTarget( bstrType, &punkTarget );
		::SysFreeString( bstrType );

		if( !CheckInterface( punkTarget, IID_IDocumentSite ) )
		{
			punkTarget->Release();
			return S_FALSE;
		}
		pThis->m_ptrActiveDocument = punkTarget;
		punkTarget->Release();*/

		// 6. create the action object
		IUnknown	*punkAction = 0;
		ptrInfo->ConstructAction( &punkAction );
		if( !punkAction )
			return S_FALSE;
		IActionPtr	ptrAction( punkAction );
		punkAction->Release();

		// 7. Акция должна быть фильтром, иначе облом
		if( !CheckInterface( ptrAction, IID_IFilterAction ) )
		{
			return S_FALSE;
		}

		// 8. Проверим, есть ли документ
		if( pThis->m_ptrActiveDocument == 0 )
		{
			return S_FALSE;
		}
		
		// 9. init the filter action arguments
		IFilterActionPtr	ptrFilterAction( ptrAction );

		
//		ptrAction->AttachTarget( pThis->m_pPreviewTarget );
		ptrAction->AttachTarget( pThis->m_ptrActiveDocument );
		ptrFilterAction->LockDataChanging( true );
		ptrFilterAction->InitArguments();
		
		// 10. Акция всосала ссылки на аргументы из документа (?) - теперь перекинем ее на временный контекст
		ptrAction->AttachTarget( *pThis->m_pPreviewTarget );

		// 11. Если мы не в процессе набора группы - очистим от старья
		// ??? подозрительное место, сложно как-то все и не единообразно
		if( !pThis->m_bHelperInGroup )
		{
 			pThis->m_arrPtrCurrActions.RemoveAll();

			if( pThis->m_bRunning && pThis->m_lExecuteActionCount )
			{
				pThis->save_args_arr(); // переложить все из m_argsArr в m_argsArrSaved
//				TRACE("Removing action from array, case 1 from %d to %d\n", pThis->m_lExecuteActionCount, pThis->m_argsArr.GetCount() - pThis->m_lExecuteActionCount);
			}
			else
			{
				if( pThis->m_arrPtrCurrComleteActions.GetSize() > 0 )
				{
					pThis->_delete_args( pThis->m_arrPtrCurrComleteActions.GetSize(), pThis->m_argsArr.GetCount() - pThis->m_arrPtrCurrComleteActions.GetSize()  );
//					TRACE("Removing action from array, case 2 from %d to %d\n", pThis->m_arrPtrCurrComleteActions.GetSize(), pThis->m_argsArr.GetCount() - pThis->m_arrPtrCurrComleteActions.GetSize());
				}
				else
				{
					pThis->DoRemovePreview();
					pThis->delete_args_arr();
//					TRACE("Removing action from array, case 3\n");
				}
			}
//			TRACE("Removing action from array...\n");
		}
		
		// 12. Втыкаем в m_argsArr новую запись
		{
			ArgInfoArr *pargs = new ArgInfoArr;
			pThis->m_argsArr.Add( pargs );
		}

		_map_t<ArgInfo*, CString, cmp_cstring> name2pargInfo;

		long	nInCount, nOutCount;
		nInCount = nOutCount = 0;

		long	lArgumentPosition = 0;
		ptrFilterAction->GetFirstArgumentPosition( &lArgumentPosition );

		// 13. Пошли по всем data-аргументам акции
		while( lArgumentPosition )
		{
			BSTR		bstrArgType = 0;
			BSTR		bstrArgName = 0;
			BOOL		bOut = 0;
			BOOL		bRemove = false;
			ptrFilterAction->GetArgumentInfo( lArgumentPosition, &bstrArgType, &bstrArgName, &bOut );

			ArgInfo	*pargInfo = 0;

			// получаем очередной аргумент из нашего массива (из последней записи в m_argsArr, 
			if( bOut )
				pargInfo = pThis->GetArgDescription( bOut==TRUE, bstrArgType, bstrArgName, nOutCount++ );
			else
				pargInfo = pThis->GetArgDescription( bOut==TRUE, bstrArgType, bstrArgName, nInCount++ );

			if( pargInfo )
			{
				pargInfo->m_lArgumentPosition = lArgumentPosition;

				// debug features:
				pargInfo->m_punkActionInfo = punkActionInfo;
				pargInfo->m_punkAction = punkAction;
//				TRACE("added argingfo, punkAction = %p\n", punkAction);
				BSTR bstrName;
				ptrInfo->GetCommandInfo(&bstrName,0,0,0);
				pargInfo->m_bstrActionName = bstrName;
				::SysFreeString(bstrName);
			}

			pargInfo->m_bstr_original_name = bstrArgName;

			// засунем указатель в мапу, чтоб потом проще было
			name2pargInfo.set( pargInfo, bstrArgName );

			::SysFreeString( bstrArgType );
			::SysFreeString( bstrArgName );

			if( pargInfo->m_ptrObject==0 )
			{
				//return S_FALSE;	//argument not found
			}

			//ptrFilterAction->SetArgument( lArgumentPosition, pargInfo->m_ptrObject, pargInfo->m_bHaveToRemove );

			ptrFilterAction->GetNextArgument( 0, &lArgumentPosition );

			if( !bOut )
			{
				ptrFilterAction->CanDeleteArgument( pargInfo->m_ptrObject, &bRemove );
				pargInfo->m_bHaveToRemove = (bRemove == TRUE);
			}
		}

		//attach arguments to action
		int	iInArgCount = 0;
		int	iParamsCounter = 0;

		ptrInfo->GetArgsCount( &iInArgCount );

		// [vanek] BT2000:3842 использование IActionInfo2 (как в ActionManager::PreExecute) - 17.06.2004
		IActionInfo2Ptr	sptrInfo2 = ptrInfo;

		int	nArgCount = 0;

		if( pDispParams )
			nArgCount = pDispParams->cArgs;

		// 14. ??? Пройдемся по всем аргументам акции, засунем в них переданные нам в диспатч
		for( int i = 0; i < iInArgCount; i++ )
		{
			_variant_t var;
			BSTR	bstrName = 0, bstrDefVal = 0;

			if( sptrInfo2 == 0 )
			{
				ptrInfo->GetArgInfo( i, &bstrName, 0, &bstrDefVal );

				if( iParamsCounter < int( nArgCount ) )
					var = pDispParams->rgvarg[nArgCount-iParamsCounter-1];
				else
				{
					if( bstrDefVal )
						var = bstrDefVal;
				}
			}
			else
			{
				int nNumber = 0;
				sptrInfo2->GetArgInfo2( i, &bstrName, 0, &bstrDefVal, &nNumber );

				if( nNumber < int( nArgCount ) )
					var = pDispParams->rgvarg[nArgCount-nNumber-1];
				else
				{
					if( bstrDefVal )
					{
						var = _bstr_t(bstrDefVal);
					}
				}
			}

			ptrAction->SetArgument( bstrName, &var );

			// если данный аргумент был в списке входных параметров фильтра - проставим там m_bstr_given_name
			{
				long	lpos = name2pargInfo.find(bstrName);
				if(lpos)
				{
					ArgInfo* pargInfo = name2pargInfo.get(lpos);
					_variant_t var1;
					var1.ChangeType(VT_BSTR, &var);
					if( pargInfo && var1.vt==VT_BSTR )
					{
						pargInfo->m_bstr_given_name = _bstr_t(var1);
					}
				}
			}

			iParamsCounter++;

			::SysFreeString( bstrName );
			::SysFreeString( bstrDefVal );

		}


		//process output args
		int	iOutArgCount = 0;
		int	iOutParamsCounter = 0;

		ptrInfo->GetOutArgsCount( &iOutArgCount );

		for( int i = 0; i < iOutArgCount; i++ )
		{
			_variant_t var;
			BSTR	bstrName = 0, bstrDefVal = 0;

			if( sptrInfo2 == 0 )
			{
				ptrInfo->GetOutArgInfo( i, &bstrName, 0 );

				if( iOutParamsCounter < int( nArgCount ) )
					var = pDispParams->rgvarg[nArgCount-iParamsCounter-1];
			}
			else
			{
				int nNumber = 0;
				sptrInfo2->GetOutArgInfo2( i, &bstrName, 0, &nNumber );

				if( nNumber < int( nArgCount ) )
					var = pDispParams->rgvarg[nArgCount-nNumber-1];
				else
				{
					if( bstrDefVal )
						var = bstrDefVal;
				}
			}

			ptrAction->SetArgument( bstrName, &var );

			// если данный аргумент был в списке входных параметров фильтра - проставим там m_bstr_given_name
			{
				long	lpos = name2pargInfo.find(bstrName);
				if(lpos)
				{
					ArgInfo* pargInfo = name2pargInfo.get(lpos);
					_variant_t var1;
					var1.ChangeType(VT_BSTR, &var);
					if( pargInfo && var1.vt==VT_BSTR )
					{
						pargInfo->m_bstr_given_name = _bstr_t(var1);
					}
				}
			}

			iParamsCounter++;

			::SysFreeString( bstrName );
			::SysFreeString( bstrDefVal );

		}

//		TRACE("Invoke, SetActionToExecuteList \n");
		pThis->SetActionToExecuteList( ptrAction );
		return S_OK;
	}
	_catch_nested;
}

static bool s_bInInvoke = false;
void CActionHelper::TerminateCurrentAction()
{
	if (m_bRunning)
	{
//		TRACE("TerminateCurrentAction\n");
		m_bTerminateFilterGroup = true;
		IUnknownPtr	punk(_GetOtherComponent( GetAppUnknown(), IID_IOperationManager ), false);
		IOperationManager2Ptr sptrOM(punk);
		if (sptrOM != 0 && s_bInInvoke)
		{
//			TRACE("TerminateCurrentAction, flag set\n");
			sptrOM->TerminateCurrentOperation(2);
		}
	}
}


ArgInfo *CActionHelper::GetArgDescription( bool bOut, BSTR bstrArgType, BSTR bstrArgName, int nPosition )
{
	ArgInfoArr *pArgs = m_argsArr.GetAt( m_argsArr.GetCount() - 1 );

	for( int i = 0; i < pArgs->GetSize(); i++ )
	{
		ArgInfo	*pargInfo = pArgs->GetAt( i );

		if( pargInfo->m_bInput == bOut )
			continue;

		if( !nPosition )
			return pargInfo;
		nPosition--;
	}

	// Если нужного номера аргумента не было - воткнем новый, проинитим переданными данными и все такое
	ArgInfo	*pargInfo = new ArgInfo;

	pargInfo->m_bInput = !bOut;
	pargInfo->m_bstrType = bstrArgType;
	pargInfo->m_nContextIndex = nPosition;
	pargInfo->m_bNamedDataChanged = false;
	pargInfo->m_bHaveToRemove = true;
	pargInfo->m_bWasVirtual = false;
	
	
	/*	// перенесено в PrepareArgDescriptions, которая вызывается неепосредственно перед выполнением акции,
		// т.к. входными аргументами текущей акции могут быть выходные аргументы предыдущей акции (группа акций)

	IUnknown	*punkDataObject = 0;
	
	if( bOut )
	{
		punkDataObject = ::CreateTypedObject( pargInfo->m_bstrType );

		CString	str( bstrArgName );
		SetObjectName( punkDataObject, ::GenerateNameForArgument( str, m_ptrActiveDocument ) );
	}
	else
	{
		//if (m_bHelperInGroup)
		if( m_argsArr.GetSize() > 1 )
			punkDataObject = GetObject( pargInfo, m_pPreviewTarget->GetControllingUnknown() );
		if (punkDataObject == 0)
			punkDataObject = GetObject( pargInfo, m_ptrActiveDocument );

		//change the status of no-virtual objects
		if( punkDataObject )
		{
			INamedDataObject2Ptr	ptrN( punkDataObject );
			DWORD dwFlags;
			ptrN->GetObjectFlags( &dwFlags );

			if( (dwFlags & nofHasData) == 0 )
			{
				ptrN->StoreData( sdfCopyParentData );
				pargInfo->m_bWasVirtual = true;
			}
		}
	}

	pargInfo->m_ptrObject = punkDataObject;*/
	pargInfo->m_ptrObject = 0;

	/*if( punkDataObject )
		punkDataObject->Release();*/

	pArgs->Add( pargInfo );

	return pargInfo;
}


void CActionHelper::MoveNewArgsToData()
{
	XLock lock( this );

	//TRACE( "MoveNewArgsToData begin, nAction = %d, ArgsCount = %d\n", m_nAction, m_argsArr.GetCount() );

	IDataContextPtr	ptrC( m_pPreviewTarget->GetControllingUnknown() );

	if ( m_nAction < m_argsArr.GetCount() && m_nAction >= 0 )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( m_nAction );

		for( int i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pargInfo = pArgs->GetAt( i );

			if( !pargInfo->m_bNamedDataChanged ) // Глючит !!!
			{
				if( !pargInfo->m_bInput && pargInfo->m_ptrObject != 0 ) // vanek
				{
					//TRACE( "	MoveNewArgsToData SetToDocData, i = %d\n", i );
					m_changesPreview.SetToDocData( (*m_pPreviewTarget), pargInfo->m_ptrObject );
					ptrC->SetActiveObject( pargInfo->m_bstrType, pargInfo->m_ptrObject, 0 );

					pargInfo->m_bNamedDataChanged = true;
				}			
			}
		}
	}
	///!!!! we have to notify preview view
	if( m_ptrPreviewView )
		m_ptrPreviewView->PreviewReady();

	//TRACE( "MoveNewArgsToData end, nAction = %d\n", m_nAction );
}

void CActionHelper::ApplyPreview()
{
	if( m_bRemoveOnComplete )
		return;
	//if( m_ptrCurrentCompleteAction != 0 )
	if( !m_arrPtrCurrComleteActions.IsEmpty() )
		DoApplyPreview();
	else
	{
		//if( m_ptrCurrentAction == 0 )
		if( m_arrPtrCurrActions.IsEmpty() )
			return;	//nohing to apply

		m_bAllowTerminate = false;
		m_bApplyOnComplete = true;

		if( !m_bRunning )
			ExecuteCurrentAction();
	}

}

void CActionHelper::RemovePreview()
{
	if( m_bApplyOnComplete )
		return;
	if( m_bRunning )
	{
		//ASSERT( m_ptrCurrentAction != 0 );
		ASSERT( !m_arrPtrCurrActions.IsEmpty() );

		/*ILongOperationPtr	ptrLong( m_ptrCurrentAction );
		ptrLong->Abort();*/
		for( int i = 0; i < m_arrPtrCurrActions.GetSize(); i++ )
		{
			ILongOperationPtr	ptrLong( m_arrPtrCurrActions.GetAt( i ) );
			ptrLong->Abort();
		}

		/*if( (IUnknown*)m_ptrCurrentAction == (IUnknown*)m_ptrWaitAction )
			m_ptrWaitAction = 0;*/
		if(0) if( m_arrPtrCurrActions == m_arrPtrWaitActions )
			m_arrPtrWaitActions.RemoveAll();

		//m_ptrCurrentAction = 0;
		m_arrPtrCurrActions.RemoveAll();
		m_bRemoveOnComplete = true;
	}
	else
	{
		DoRemovePreview();
		UpdatePreviewView( true );
	}
}

void CActionHelper::DoApplyPreview()
{
	XLock lock( this );
	int	i;
//[AY] 29.03.2002

	for( int z = 0; z < m_argsArr.GetCount(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( z );
		for( i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pargInfo = pArgs->GetAt( i );
			if( pargInfo->m_bInput )
			{
				if( pargInfo->m_bHaveToRemove)
					m_changes.RemoveFromDocData( m_ptrActiveDocument, pargInfo->m_ptrObject, false );
			}
		}
		for( i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pargInfo = pArgs->GetAt( i );
			if( !pargInfo->m_bInput )
			{
				SetObjectName( pargInfo->m_ptrObject, pargInfo->m_bstr_original_name );

				//paul 1.04.2002
				DWORD dwFlags = 0;
				if( CheckInterface( m_ptrActiveDocument, IID_IDBaseDocument ) )
					dwFlags = UF_NOT_GENERATE_UNIQ_NAME;

				m_changes.SetToDocData( m_ptrActiveDocument, pargInfo->m_ptrObject, dwFlags );
			}
		}
	}

	m_changes.DeInit();
	
	/*{
		/*IFilterActionPtr	ptrFilterAction( m_ptrCurrentCompleteAction );
		ptrFilterAction->SetPreviewMode( false );
		ptrFilterAction->LockDataChanging( false );
	}
	{
		IActionPtr	ptrA( m_ptrCurrentCompleteAction );
		ptrA->AttachTarget( m_ptrActiveDocument );
	}*/
	for( int j = 0; j < m_arrPtrCurrComleteActions.GetSize(); j++ )
	{
		IFilterActionPtr	ptrFilterAction( m_arrPtrCurrComleteActions.GetAt( j ) );
		ptrFilterAction->SetPreviewMode( false );
		ptrFilterAction->LockDataChanging( false );

		IActionPtr	ptrA = ptrFilterAction;
		ptrA->AttachTarget( m_ptrActiveDocument );
	}

	//m_ptrActionManager->AddCompleteActionToUndoList( m_ptrCurrentCompleteAction );
	for( int j = 0; j < m_arrPtrCurrComleteActions.GetSize(); j++ )
		m_ptrActionManager->AddCompleteActionToUndoList( m_arrPtrCurrComleteActions.GetAt( j ) );

	
	//::FireEvent( GetAppUnknown(), szEventAfterActionExecute, 0, m_ptrCurrentCompleteAction );
	for( int j = 0; j < m_arrPtrCurrComleteActions.GetSize(); j++ )
		::FireEvent( GetAppUnknown(), szEventAfterActionExecute, 0, 
					m_arrPtrCurrComleteActions.GetAt( j ) );
	
	//DeInitArgs();

	delete_args_arr();

	m_changesPreview.DeInit();

	/*m_ptrCurrentAction = 0;
	m_ptrCurrentCompleteAction = 0;*/
	m_arrPtrCurrActions.RemoveAll();
	m_arrPtrCurrComleteActions.RemoveAll();

	AttachActivesObjectFromContext( true );
	m_bApplyOnComplete = false;
	m_bAllowTerminate = true;
	m_psite->FireOnChangePreview();	
}

void CActionHelper::DoRemovePreview()
{
	if( m_bHelperInGroup ) 
		return;

	m_bRemoveOnComplete = false;

	for( int z = 0; z < m_argsArr.GetSize(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( z );
		for( int i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pargInfo = pArgs->GetAt( i );

			if( !pargInfo->m_bInput )
				::DeleteObject( (*m_pPreviewTarget), pargInfo->m_ptrObject );
			pargInfo->m_bNamedDataChanged = false;
		}
	}

	if( m_ptrPreviewView != 0 )
	{
		IDataContext2Ptr	ptrSrc( m_ptrPreviewView );
		IDataContext2Ptr	ptrDest( m_pPreviewTarget->GetControllingUnknown() );
		ptrDest->SynchronizeWithContext( ptrSrc );
	}

	return;
}

static CString GetActionName(IUnknown* punkAction)
{
	CString strName;
	IActionPtr sptrAction(punkAction);
	if(sptrAction != 0)
	{
		IUnknown* punkAI = 0;
		sptrAction->GetActionInfo(&punkAI);
		IActionInfoPtr sptrAI(punkAI);
		if(punkAI)
			punkAI->Release();
		BSTR	bstrActionName = 0;
		sptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0); 
		strName = bstrActionName;
		SysFreeString(bstrActionName);
	}
	return strName;
}

void CActionHelper::SetActionToExecuteList( IUnknown *punk )
{
	//m_ptrCurrentAction = punk;
	//m_ptrWaitAction = m_ptrCurrentAction;

	{ // debug
		CString s = "Action: ";
		s += GetActionName(punk);
 		TRACE( s+"\n" );
	}
	
	m_arrPtrCurrActions.Add( IActionPtr( punk ) );
	if(0) m_arrPtrWaitActions = m_arrPtrCurrActions;
	m_bPtrCurrActionsChanged = true;
}

void CActionHelper::ExecuteCurrentAction()
{
  	if( m_bHelperInGroup )	// нельзя выполнять акции, пока не заполнилась вся группа
		return;


	//if( m_ptrCurrentAction == 0 && m_bDelayEnableButtons )	//delayed enable buttons
	if( m_bDelayEnableButtons && m_arrPtrCurrActions.IsEmpty() )// && m_bDelayEnableButtons )	//delayed enable buttons
	{
		XSiteButton	*p = m_pSiteButton;

		while( p )
		{
			if( ::IsWindow( p->m_hwnd ) )
				::EnableWindow( p->m_hwnd, p->m_bOriginalEnabled );
			p = p->m_pnext;
		}
		m_bDelayEnableButtons = false;
	}

	/*if( !m_bRunning && m_ptrCurrentAction == 0 && m_ptrWaitAction != 0 )
	{
		m_ptrCurrentAction = f;
	}*/
	if(0) if( !m_bRunning && m_arrPtrCurrActions.IsEmpty() && !m_arrPtrWaitActions.IsEmpty() )
		m_arrPtrCurrActions = m_arrPtrWaitActions;
	
	//if( !m_bRunning && m_ptrCurrentAction != 0 )
	if( !m_bRunning && !m_arrPtrCurrActions.IsEmpty() )
	{		
//		TRACE("Begin of CActionHelper::ExecuteCurrentAction\n");
		//m_ptrCurrentCompleteAction = 0;
		
		CLockerFlag locker( &m_bRunning );
        {
			// [vanek] SBT: 664
			//_delete_args( 0, m_arrPtrCurrComleteActions.GetSize() );
			if( 0 < m_arrPtrCurrComleteActions.GetSize() )
				_delete_args( 0, m_arrPtrCurrComleteActions.GetSize() );

			m_arrPtrCurrComleteActions.RemoveAll();

			/*IFilterActionPtr	ptrFilterAction( m_ptrCurrentAction );
			ptrFilterAction->SetPreviewMode( TRUE );*/
			for( int i = 0; i < m_arrPtrCurrActions.GetSize(); i++ )
			{
				IFilterActionPtr	ptrFilterAction( m_arrPtrCurrActions.GetAt( i ) );
				ptrFilterAction->SetPreviewMode( TRUE );
			}
			
			//IActionPtr	ptrAction( m_ptrCurrentAction );
			XIActionPtrArray arrPtrActions;
			arrPtrActions = m_arrPtrCurrActions;

			EnableSiteButtons( false, false );

			m_lExecuteActionCount = arrPtrActions.GetSize();
			
			//ptrAction->Invoke();
			m_bPtrCurrActionsChanged = false;
//			TRACE("m_bPtrCurrActionsChanged = false\n");
			m_bTerminateFilterGroup = false;
			for( int i = 0; i < arrPtrActions.GetSize(); i++ )
			{
				m_nAction = i;
				PrepareArgDescriptions( i, IFilterActionPtr( arrPtrActions.GetAt( i ) ) );
				s_bInInvoke = true;
//				TRACE("Before invoke\n");
                arrPtrActions.GetAt( i )->Invoke();
//				TRACE("After invoke\n");
				s_bInInvoke = false;
				if (m_bTerminateFilterGroup) break;
			}
			m_bTerminateFilterGroup = false; // 17.10.2005 build 91 SBT 1506

			bool bPtrCurrActionsChanged = m_bPtrCurrActionsChanged;
			m_bPtrCurrActionsChanged = false;
			IUnknownPtr	punk(_GetOtherComponent( GetAppUnknown(), IID_IOperationManager ), false);
			IOperationManager2Ptr sptrOM(punk);
			if (sptrOM != 0)
				sptrOM->TerminateCurrentOperation(0);
			m_nAction = -1;
			
			/*if( (IUnknown*)ptrAction == (IUnknown*)m_ptrWaitAction )
				m_ptrWaitAction = 0;*/

			if(0) if( arrPtrActions == m_arrPtrWaitActions )
				m_arrPtrWaitActions.RemoveAll();


 			m_lExecuteActionCount = 0;

			EnableSiteButtons( true, false );

			if( g_bTerminate )
			{
				g_bTerminate = false;
				//return; // убрано - вызывало повторный вызов акции
				// 03.10.2005 build 91
			}

			// Если Invoke или BeginFilterGroup будет вызван во время Invoke какой-либо акции отсюда
			// то эта акция будет прервана и вновь добавлена в список. Ее надо снова запустить, список
			// акций может совпадать с тем, который был.
			if( m_arrPtrCurrActions == arrPtrActions && !bPtrCurrActionsChanged)
			{
//				TRACE("m_arrPtrCurrActions.RemoveAll() in ExecuteCurrentAction");
				m_arrPtrCurrActions.RemoveAll(); 
				m_arrPtrCurrComleteActions.RemoveAll();
				DoRemovePreview();
			}
			delete_saved_args_arr();
			FireEvent(::GetAppUnknown(), szEventChangePreview, m_ptrPreviewView);
		}
//		TRACE("End of CActionHelper::ExecuteCurrentAction\n");
 	}
}

void CActionHelper::FinalizeCurrentAction( bool bTerminate )
{
	if( !bTerminate )
	{
		MoveNewArgsToData();
		
		/*m_ptrCurrentCompleteAction = m_ptrCurrentAction;
		m_ptrCurrentAction = 0;*/
		if( m_nAction != -1 )
			TRACE( "CActionHelper::FinalizeCurrentAction: m_nAction = %d, m_lExecuteActionCount = %d\n", m_nAction, m_lExecuteActionCount );

		if( m_nAction >= 0 && m_nAction == m_lExecuteActionCount - 1 && !m_bPtrCurrActionsChanged)
		{	// все акции выполнились и во время выполнения не был вызван ActionHelper.Invoke
			m_arrPtrCurrComleteActions.RemoveAll();
			for( int z = 0; z < m_lExecuteActionCount;z++ )
			{
				if(0==m_arrPtrCurrActions.GetSize())
					break; // 19.11.2004 build 72 [SBT 1181] - ставим защиту, но надо разбираться...
				m_arrPtrCurrComleteActions.Add( m_arrPtrCurrActions.GetAt( 0 ) );
				m_arrPtrCurrActions.RemoveAt( 0 );
			}
			m_changesPreview.DeInit();
		}

		if( m_bApplyOnComplete )
			DoApplyPreview();
		if( m_bRemoveOnComplete )
			DoRemovePreview();

		UpdatePreviewView( false );
	}
	else
	{

		//m_ptrCurrentCompleteAction = 0;
		m_arrPtrCurrComleteActions.RemoveAll();

		bool	bRemoveOnComplete = m_bRemoveOnComplete;
		DoRemovePreview();
		if( bRemoveOnComplete )
			UpdatePreviewView( true );

	}
}

void CActionHelper::DeInitArgs()
{
	for( int z = 0; z < m_argsArr.GetSize(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( z );
		for( int i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pargInfo = (*pArgs)[i];
			if( pargInfo->m_bWasVirtual )
			{
				INamedDataObject2Ptr	ptrN( pargInfo->m_ptrObject );
				ptrN->StoreData( sdfAttachParentData );
			}
			delete pargInfo;
		}
		delete pArgs;
	}
	
	m_changesPreview.DoUndo( (*m_pPreviewTarget) );

	m_argsArr.RemoveAll();
	m_changesPreview.DeInit();
}

void CActionHelper::AddHeadArg(IUnknown* punkDataObject)
{
	ArgInfoArr *pargs = new ArgInfoArr;
	m_argsArr.Add( pargs );

	ArgInfoArr *pArgs = m_argsArr.GetAt( m_argsArr.GetCount() - 1 );

	ArgInfo	*pargInfo = new ArgInfo;

	pargInfo->m_bInput = true;
	pargInfo->m_bstrType = ::GetObjectKind(punkDataObject);
	pargInfo->m_nContextIndex = 0;
	pargInfo->m_bNamedDataChanged = false;
	pargInfo->m_bHaveToRemove = true;
	pargInfo->m_bWasVirtual = false;
	
	pargInfo->m_ptrObject = punkDataObject;

	pArgs->InsertAt(0, pargInfo);
}

void CActionHelper::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{	
	if( !m_bInitialized )
		return;
	if( m_bLockUpdate )
			return;
	if( !strcmp( pszEvent, szEventActiveObjectChange ) )
	{
		CheckObjects();
	}
	else if( !strcmp( pszEvent, szEventNoDataObject ) )
	{
	//may be, we remove the result of action. in this case, we have
		//to remove source data object.
		long	lAction = *(long*)pdata;

		if( lAction == ncRemoveObject )	
		{
			GUID	guid = ::GetObjectKey(punkHit);
			bool	bFound = false;

			for( int z = 0; z < m_argsArr.GetSize(); z++ )
			{
				ArgInfoArr *pArgs = m_argsArr.GetAt( z );
				for( int i = 0; i < pArgs->GetSize(); i++ )
				{
					ArgInfo	*pai = (*pArgs)[i];
					if( GetObjectKey( pai->m_ptrObject ) == guid &&
						pai->m_bInput == false )
					{
						
						bFound = true;
						break;
					}
				}
				if (bFound) break;
			}

			//deleted object is result - try to delete argument
			if( bFound )
			{
				CString	strType = (char*)::GetObjectType( punkHit );

				for( int z = 0; z < m_argsArr.GetSize(); z++ )
				{
					ArgInfoArr *pArgs = m_argsArr.GetAt( z );

					for( int i = 0; i < pArgs->GetSize(); i++ )
					{
						ArgInfo	*pai = (*pArgs)[i];
						if( pai->m_bInput ) 
						{
							IUnknown	*punkObject = pai->m_ptrObject;
							if( strType == (char*)::GetObjectType( punkObject ) )
							{
								::SetSourceObject( punkObject );
								return;
							}
						}
					}
				}
			}

		}
	}
	else if( !strcmp( pszEvent, szEventActivateObject ) )
	{
		DoRemovePreview();
		DeInitArgs();
		AttachActivesObjectFromContext( true );
		m_psite->FireOnChangePreview();	
	}
	else if( !strcmp( pszEvent, szAppActivateView ) )
	{
		if( m_ptrPreviewView == punkFrom )return;
		IUnknown	*punkOldDoc = GetDocumentFromView( m_ptrPreviewView );
		IUnknown	*punkNewDoc = GetDocumentFromView( punkFrom );

		//if( punkOldDoc == punkNewDoc || !punkOldDoc )
		{
			if( punkOldDoc != punkNewDoc )RegisterDocListener( false );
			DeActivatePreview();
			AttachPreviewView( punkFrom );
			CheckObjects();
			ActivatePreview();
			if( punkOldDoc != punkNewDoc )RegisterDocListener( true );
		}

		if( punkOldDoc )punkOldDoc->Release();
		if( punkNewDoc )punkNewDoc->Release();
	}
	else if( !strcmp( pszEvent, szAppCreateView ) )
	{
		IUnknown	*punkOldDoc = GetDocumentFromView( m_ptrPreviewView );
		IUnknown	*punkNewDoc = GetDocumentFromView( punkFrom );

		if( punkOldDoc == punkNewDoc || !punkOldDoc )
		{
			DeActivatePreview();
			AttachPreviewView( punkFrom );
			ActivatePreview();

			if( !punkOldDoc )
				RegisterDocListener( true );
			CheckObjects();
		}

		if( punkOldDoc )punkOldDoc->Release();
		if( punkNewDoc )punkNewDoc->Release();

	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		IUnknown	*punkOldDoc = GetDocumentFromView( m_ptrPreviewView );
		IUnknown	*punkNewDoc = GetDocumentFromView( punkFrom );

		if( punkFrom == m_ptrPreviewView )
		{
			RegisterDocListener( false );
			m_ptrPreviewView->AttachPreviewData( 0 );
			AttachPreviewView( 0 );
		}

		if( punkOldDoc )punkOldDoc->Release();
		if( punkNewDoc )punkNewDoc->Release();
	}
}

void CActionHelper::DeActivatePreview()
{
	if( m_ptrPreviewView == 0 )return;

	m_ptrPreviewView->AttachPreviewData( 0 );
	AttachPreviewView( 0 );
}



void CActionHelper::ActivatePreview()
{
	if( m_ptrPreviewView == 0 )return;
	m_ptrPreviewView->AttachPreviewData( (*m_pPreviewTarget) );
}

void CActionHelper::RegisterDocListener( bool bRegister )
{
	if( bRegister )
	{
		IUnknownPtr	ptrNewDoc( GetDocumentFromView( m_ptrPreviewView ), false );

		if( ptrNewDoc == m_ptrActiveDocument )
			return;

		DoRemovePreview();
		DeInitArgs();

		if( m_ptrActiveDocument )UnRegister( m_ptrActiveDocument );
		AttachActivesObjectFromContext( false );
		m_ptrActiveDocument = ptrNewDoc;
		if( m_ptrActiveDocument )Register( m_ptrActiveDocument );
		AttachActivesObjectFromContext( true );

		m_psite->FireOnChangePreview();
	}
	else
	{
		DoRemovePreview();
		DeInitArgs();

		if( m_ptrActiveDocument )UnRegister( m_ptrActiveDocument );
		AttachActivesObjectFromContext( false );
		m_ptrActiveDocument = 0;
	}
}

void CActionHelper::UpdatePreviewView( bool bReAttachObjects )
{
	if( m_ptrPreviewView )
	{
		if( bReAttachObjects )
		{
			m_ptrPreviewView->AttachPreviewData( 0 );
			m_ptrPreviewView->AttachPreviewData( (*m_pPreviewTarget) );
		}
		else
		{
			CWnd	*pwnd = ::GetWindowFromUnknown( m_ptrPreviewView );
			if( pwnd )pwnd->Invalidate();
		}
	}
}

void CActionHelper::CheckObjects()
{
	//CTimeTest	timeTest( true, "CheckObjects" );


	bool	bSame = true;
	if (m_argsArr.GetSize() == 0) return;
	if( m_arrPtrCurrComleteActions.GetSize() == 0 )
		bSame = false;
		
	if( bSame )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( /*0*/ m_arrPtrCurrComleteActions.GetSize() - 1 );
		for( int i = 0; i < pArgs->GetSize(); i++ )
		{
			ArgInfo	*pai = (*pArgs)[i];
			
			if( !pai->m_bInput )
				continue;
			IUnknownPtr ptr( GetObject( pai, m_ptrPreviewView ), false );

			if( GetObjectKey( ptr ) != ::GetObjectKey( pai->m_ptrObject ) )
			{
				bSame = false;
				break;
			}
		}
	}

	if( !bSame )
	{
		//CTimeTest	timeTest( true, "InsideCheckObjects" );
		DoRemovePreview();
		_delete_args( m_arrPtrCurrComleteActions.GetSize(), m_argsArr.GetCount() - m_arrPtrCurrComleteActions.GetSize()  );
		

		AttachActivesObjectFromContext( true );

//		DeInitArgs();

		m_arrPtrCurrActions.RemoveAll();
		if(0) m_arrPtrWaitActions.RemoveAll();

		m_psite->FireOnChangePreview();	
	}
}

IUnknown	*CActionHelper::GetObject( ArgInfo *pargInfo, IUnknown *punkFrom )
{
	if(pargInfo->m_bstr_given_name!=_bstr_t(""))
	{ // если дано имя - ищем по имени
		return GetObjectByName(punkFrom, pargInfo->m_bstr_given_name, pargInfo->m_bstrType);
		// ??? возможно, если не нашли - стоит попытаться взять активный?
	}
	//if( CheckInterface( punkFrom, IID_IDataContext ) )
	if( CheckInterface( punkFrom, IID_IDocumentSite) )
		return GetActiveObjectFromDocument( punkFrom, pargInfo->m_bstrType );
	else
		return GetActiveObjectFromContext( punkFrom, pargInfo->m_bstrType );
		
}

void CActionHelper::AttachActivesObjectFromContext( bool bAttach )
{
	::DeleteEntry( (*m_pPreviewTarget), 0 );




	if( m_ptrPreviewView != 0 && bAttach )
	{
		IDataContext2Ptr	ptrSrc( m_ptrPreviewView );
		IDataContext2Ptr	ptrDest( m_pPreviewTarget->GetControllingUnknown() );

		/*
		{
			IUnknownPtr	ptrUnk( GetActiveObjectFromContext( ptrSrc, szTypeImage ), false );
			IUnknownPtr	ptrUnk1( GetActiveObjectFromContext( ptrDest, szTypeImage ), false );
			TRACE( "%s %s\n", ::GetObjectName( ptrUnk ), ::GetObjectName( ptrUnk1 ) );
		}
		*/


		long	nTypes;
		ptrSrc->GetObjectTypeCount( &nTypes );

		for( long n = 0; n < nTypes; n++ )
		{
			BSTR	bstrType;
			ptrSrc->GetObjectTypeName( n, &bstrType );

			long	nCount = 0;
			ptrSrc->GetChildrenCount( bstrType, 0, &nCount );

			if (nCount)
			{
				long lPos = 0;
				ptrSrc->GetFirstChildPos(bstrType, 0, &lPos);
				while (lPos)
				{
					IUnknownPtr sptrObj;
					if (FAILED(ptrSrc->GetNextChild(bstrType, 0, &lPos, &sptrObj)) || sptrObj == 0)
						continue;

					CObjectListUndoRecord	tempo;
					tempo.SetToDocData( *m_pPreviewTarget, sptrObj );
				}
			}

			::SysFreeString( bstrType );
		}
		ptrDest->SynchronizeWithContext( ptrSrc );

		/*
		{
			IUnknownPtr	ptrUnk( GetActiveObjectFromContext( ptrSrc, szTypeImage ), false );
			IUnknownPtr	ptrUnk1( GetActiveObjectFromContext( ptrDest, szTypeImage ), false );
			TRACE( "%s %s\n", ::GetObjectName( ptrUnk ), ::GetObjectName( ptrUnk1 ) );
		}
		*/
	}

	//attach NamedData to objects
	{
		
		IUnknown			*punkDoc = m_ptrActiveDocument;//GetDocumentFromView( m_ptrPreviewView );
		IDataTypeManagerPtr	ptr( punkDoc );
		//punkDoc->Release();
		if( !punkDoc )return;

		long	nTypesCount = 0, nType;
		ptr->GetTypesCount( &nTypesCount );

		for( nType = 0; nType < nTypesCount; nType++ )
		{
			long lpos = 0;
			ptr->GetObjectFirstPosition( nType, &lpos );

			while( lpos )
			{
				IUnknown	*punkObj = 0;
				ptr->GetNextObject( nType, &lpos, &punkObj );

				if( !punkObj )continue;
				INamedDataObject2Ptr	ptrN( punkObj );
				punkObj->Release();
				ptrN->AttachData( ptr );
			}
		}
	}
}

void	CActionHelper::InitSiteButtons()
{
	FreeSiteButtons();

	HWND	hwndParent = ::GetParent( m_psite->GetSafeHwnd() );
	if( !hwndParent )return;

	hwndParent = ::GetParent( hwndParent );
	HWND	hwndChild = ::GetWindow( hwndParent, GW_CHILD );
	

	while( hwndChild ) 
	{
		char	szClassName[128];

		::GetClassName( hwndChild, szClassName, 128 );

		if( !strcmp( szClassName, "Button" ) )
			AddSiteButton( hwndChild );

		hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT );
	}

}

void CActionHelper::AddSiteButton( HWND hwnd )
{
	XSiteButton	*p = new XSiteButton;
	p->m_pnext = m_pSiteButton;
	m_pSiteButton = p;
	p->m_hwnd = hwnd;
	p->m_bOriginalEnabled = ::IsWindowEnabled( hwnd ) != 0;
}

void	CActionHelper::EnableSiteButtons( bool bEnable, bool bForceEnable )
{
	if( !bForceEnable && bEnable )
	{
		m_bDelayEnableButtons = true;
		return;
	}

	XSiteButton	*p = m_pSiteButton;

	while( p )
	{
		if( ::IsWindow( p->m_hwnd ) )
		{
			if( bEnable )
				::EnableWindow( p->m_hwnd, p->m_bOriginalEnabled );
			else
				::EnableWindow( p->m_hwnd, false );
		}
		p = p->m_pnext;
	}
}

void	CActionHelper::FreeSiteButtons()
{
	EnableSiteButtons( true, true );

	while( m_pSiteButton )
	{
		XSiteButton	*p = m_pSiteButton->m_pnext;
		delete m_pSiteButton;
		m_pSiteButton = p;
	}
}


void CActionHelper::AttachPreviewView( IUnknown *punkView )
{
	if( m_ptrPreviewView == punkView )
		return;

	if( m_ptrPreviewView != 0 )
	{
		UnRegister( m_ptrPreviewView );
	}
	m_ptrPreviewView  = punkView;	
	if( m_ptrPreviewView != 0 )
	{
		Register( m_ptrPreviewView );
	}
}

void CActionHelper::BeginFilterGroup()
{
 	TRACE( "BeginFilterGroup\n" );

	// 1. Если не инициализировали превью - вообще не работаем
	//if( m_bInitialized )
	//{
	//	AfxMessageBox( "You have to call Form.InitializePreview for use ActionHelper" );
	//	return;
	//}

	// 2. Предотвратим рекурсивный вызов
	if( g_bInsideInvoke )
		return; // ошибка
	CLockerFlag	locker( &g_bInsideInvoke );

	// 3. Если уже работаем (находимся внутри таймера и ExecuteCurrentAction) - прервать текущую акцию
	if (m_bRunning)
		TerminateCurrentAction();

	// 4. Если мы не в процессе набора группы - убрать результаты прошлого прохода
	DoRemovePreview();


//		DoRemovePreview();
//		delete_args_arr();

	/*
	if( m_bRunning && m_bAllowTerminate )
	{
		for( int i = 0; i < m_arrPtrCurrActions.GetSize(); i++ )
		{
			ILongOperationPtr	ptrLong( m_arrPtrCurrActions.GetAt( i ) );
			ptrLong->Abort();
		}
	}
	*/
	if (m_bRunning)
		TerminateCurrentAction();

 	m_arrPtrCurrActions.RemoveAll();

	if( m_bRunning && m_lExecuteActionCount )
		save_args_arr();
//		_delete_args( m_lExecuteActionCount, m_argsArr.GetCount() - m_lExecuteActionCount  );
	else
	{
		if( m_arrPtrCurrComleteActions.GetSize() > 0 )
		{
			_delete_args( m_arrPtrCurrComleteActions.GetSize(), m_argsArr.GetCount() - m_arrPtrCurrComleteActions.GetSize()  );
		}
		else
		{
			DoRemovePreview();
			delete_args_arr();
		}
	}

	m_bHelperInGroup = true;
}

void CActionHelper::EndFilterGroup()
{
 	TRACE( "EndFilterGroup\n" );
	m_bHelperInGroup = false;
}

void CActionHelper::delete_args_arr()
{
	for( int z = 0; z < m_argsArr.GetCount(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( z );

		for( int i = 0; i < pArgs->GetSize(); i++ )
			delete (*pArgs)[i];

		delete pArgs;
	}
	m_argsArr.RemoveAll();
}

BOOL CActionHelper::PrepareArgDescriptions( int nAction, IFilterAction *pIFilterAction )
{
	if( nAction >= m_argsArr.GetSize() )
		return FALSE;

	ArgInfoArr * parrActionArgs = m_argsArr.GetAt( nAction );
	for( int i = 0; i < parrActionArgs->GetSize(); i ++ )
	{
        IUnknown	*punkDataObject = 0;
		ArgInfo		*pargInfo = parrActionArgs->GetAt( i );
		if( !pargInfo )
			continue;
	
		if( !pargInfo->m_bInput )
		{
			punkDataObject = ::CreateTypedObject( pargInfo->m_bstrType );

			CString	str;
			if(pargInfo->m_bstr_given_name==_bstr_t(""))
				str = (const char *)(pargInfo->m_bstr_original_name);
			else
				str = (const char *)(pargInfo->m_bstr_given_name);
			SetObjectName( punkDataObject, ::GenerateNameForArgument( str, m_ptrActiveDocument ) );
		}
		else
		{
			if( m_argsArr.GetSize() > 1 )
				punkDataObject = GetObject( pargInfo, m_pPreviewTarget->GetControllingUnknown() );
			if (punkDataObject == 0)
				punkDataObject = GetObject( pargInfo, m_ptrActiveDocument );

			//change the status of no-virtual objects
			if( punkDataObject )
			{
				INamedDataObject2Ptr	ptrN( punkDataObject );
				DWORD dwFlags;
				ptrN->GetObjectFlags( &dwFlags );

				if( (dwFlags & nofHasData) == 0 )
				{
					ptrN->StoreData( sdfCopyParentData );
					pargInfo->m_bWasVirtual = true;
				}
			}
		}

		pargInfo->m_ptrObject = punkDataObject;
		if( punkDataObject )
			punkDataObject->Release(); punkDataObject = 0;

		if( pIFilterAction )
		{
			// debug features
			IUnknownPtr sptrAction = pIFilterAction;
			IUnknown *punkAction = sptrAction;

			// защита...
			if( punkAction != pargInfo->m_punkAction )
			{
				TRACE( "Bad action, punkAction=%p, pargInfo->m_punkAction=%p\n", punkAction, pargInfo->m_punkAction );
				return false;
			}

			pIFilterAction->SetArgument( pargInfo->m_lArgumentPosition, pargInfo->m_ptrObject, pargInfo->m_bHaveToRemove );
			pargInfo->m_bstr_original_name = ::GetObjectName( pargInfo->m_ptrObject );
		}
	}

	return TRUE;
}

void CActionHelper::_delete_args( int nFrom, int nCount )
{
	for( int z = 0; z < nCount; z++ )
	{
		if( nFrom < m_argsArr.GetSize() )
		{
			ArgInfoArr *pArgs = m_argsArr.GetAt( nFrom );

			for( int i = 0; i < pArgs->GetSize(); i++ )
			{
				if( !(*pArgs)[i]->m_bInput )
					::DeleteObject( (*m_pPreviewTarget), (*pArgs)[i]->m_ptrObject );

				delete (*pArgs)[i];
			}

			delete pArgs;
			m_argsArr.RemoveAt( nFrom );
		}
	}
}

void CActionHelper::save_args_arr()
{
	for( int z = 0; z < m_argsArr.GetCount(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArr.GetAt( z );
		m_argsArrSaved.Add(pArgs);
	}
	m_argsArr.RemoveAll();
}


void CActionHelper::delete_saved_args_arr()
{
	for( int z = 0; z < m_argsArrSaved.GetCount(); z++ )
	{
		ArgInfoArr *pArgs = m_argsArrSaved.GetAt( z );

		for( int i = 0; i < pArgs->GetSize(); i++ )
			delete (*pArgs)[i];

		delete pArgs;
	}
	m_argsArrSaved.RemoveAll();
}

