#include "stdafx.h"

#include "data5.h"
#include "docview5.h"

#include "action_filter.h"
#include "misc_utils.h"
#include "com_main.h"
#include "aliaseint.h"

#include "map"
#include "string"

#include "\vt5\common\dump_util.h"

//using namespace std;

IUnknown *CFilter::DoGetInterface( const IID &iid )
{
	if( iid ==  IID_ILongOperation )return (ILongOperation*)this;
	else if( iid == IID_IFilterAction )return (IFilterAction*)this;
	else if( iid == IID_IFilterAction2 )return (IFilterAction2*)this;
	else if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}

HRESULT CFilter::GetFirstArgumentPosition( long *pnPos )
{
	*pnPos = m_arguments.head();
	return S_OK;
}

HRESULT CFilter::GetNextArgument( IUnknown **ppunkDataObject, long *pnPosNext )
{
	CFilterArgument	*parg = m_arguments.next( *pnPosNext );
	if( ppunkDataObject )
	{
		*ppunkDataObject = parg->m_punkArg;
		if( *ppunkDataObject )
			(*ppunkDataObject)->AddRef();
	}

	return S_OK;
}

HRESULT CFilter::SetArgument( long lPos, IUnknown *punkDataObject, BOOL bDataChanged )
{
	{//!!! защита от кривых lPos - был прецедент...
		long lPos1=m_arguments.head();
		bool bHere=false;
		while(lPos1)
		{
			if(lPos1==lPos) bHere=true;
			m_arguments.next(lPos1);
		}
		dbg_assert(bHere);
		if(!bHere)
			return E_FAIL;
	}

	CFilterArgument	*parg = m_arguments.next( lPos );

	if( parg->m_punkArg )
		parg->m_punkArg->Release();
	parg->m_punkArg = punkDataObject;
	parg->m_bNameDataChanged = bDataChanged == TRUE;
	if( parg->m_punkArg )
		parg->m_punkArg->AddRef();

	_SetArgumentName( parg );
	parg->m_lParentKey = ::GetParentKey( parg->m_punkArg );
	
	return S_OK;
}

HRESULT CFilter::CanDeleteArgument( IUnknown *punk, BOOL *pbCan )
{
	*pbCan = false;
	long pos = m_arguments.head();
	while( pos )
	{
		CFilterArgument	*pa = m_arguments.next( pos );
		if( pa->m_punkArg == punk )
		{
			*pbCan = CanDeleteArgument( pa );
			return S_OK;
		}
	}
	return S_OK;
}

HRESULT CFilter::NeedActivateResult( IUnknown *punk, BOOL *pbCan )
{
	*pbCan = true;
	long pos = m_arguments.head();
	while( pos )
	{
		CFilterArgument	*pa = m_arguments.next( pos );
		if( pa->m_punkArg == punk )
		{
			*pbCan = NeedActivateResult( pa );
			return S_OK;
		}
	}
	return S_OK;
}

HRESULT CFilter::GetArgumentInfo( long lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut )
{
	CFilterArgument	*parg = m_arguments.next( lPos );
	if( pbstrArgType )
		*pbstrArgType = parg->m_bstrArgType.copy();

	if( pbstrArgName )
		*pbstrArgName = parg->m_bstrArgName.copy();

	if( pbOut )
		*pbOut = !parg->m_bInArgument;


	return S_OK;
}

HRESULT CFilter::InitArguments()
{
	{
		return _Init()?S_OK:S_FALSE;
	}
}

HRESULT CFilter::LockDataChanging( BOOL bLock )
{
	{
		m_bLockChangeData = bLock != FALSE;
		if( !m_bLockChangeData && !m_bPreviewMode )
			SetModified( true );
		return S_OK;
	}
}

HRESULT CFilter::AttachTarget( IUnknown *punkTarget )
{
	HRESULT hr = CAction::AttachTarget( punkTarget );
	if( hr )return hr;

	OnAttachTarget( punkTarget );

	return S_OK;
}


HRESULT CFilter::SetPreviewMode( BOOL bEnter )
{
	{
		if( bEnter )
		{
			m_bPreviewMode = true;
			DoInitializePreview();
		}
		else
		{
			if( IsPreviewMode() )
			{
				DoFinalizePreview();
				m_bPreviewMode = false;
			}
		}
		return S_OK;
	}
}

CFilter::CFilter()
{
	m_bInitialized = false;
	m_bNotifyBegin = false;
	m_bNotifyFinished = false;
	m_bInsideInvokeFilter = false;
	m_bLockChangeData = false;
	m_bPreviewMode = false;

}
CFilter::~CFilter()
{
	_DeInit();
}

//action's virtuals
HRESULT CFilter::DoUndo()
{
	ContextNotification(ncEnterFilter);
	bool bRet = _PlaceArguments( false );
	ContextNotification(ncFilterUndo);
	return bRet?S_OK:S_FALSE;
}

HRESULT CFilter::DoRedo()
{
	ContextNotification(ncEnterFilter);
	bool bRet = _PlaceArguments();
	ContextNotification(ncFilterRedo);
	return bRet?S_OK:S_FALSE;
}

HRESULT CFilter::GetActionState( DWORD *pdwState )
{
	if( !_Init() )
		*pdwState = 0;
	else
		*pdwState = 1;

	return S_OK;
}

bool CFilter::_Init1()
{
	long pos = m_arguments.head();
	while (pos)
	{
		CFilterArgument	* parg = m_arguments.next( pos );
		if (parg->m_bInArgument)
		{
			//check arguments
			if (!parg->m_punkArg)
				return false;

			if (CanDeleteArgument(parg))
				parg->m_bNameDataChanged = true;
		}
	}

	return true;
}


void CFilter::ContextNotification(long lCod)
{
	IDocumentSitePtr sptrDS = m_ptrTarget;
	if (sptrDS == 0)
		return;

	long lPos = 0;
	sptrDS->GetFirstViewPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptr;
		sptrDS->GetNextView(&sptr, &lPos);
		IDataContextPtr ptrC = sptr;
		if (ptrC != 0)
			ptrC->Notify(lCod, Unknown(), 0, 0);
	}
	IDataContextPtr ptrC = m_ptrTarget;
	if (ptrC != 0)
		ptrC->Notify(lCod, Unknown(), 0, 0);
}

HRESULT CFilter::DoInvoke()
{
	ContextNotification(ncEnterFilter);
	try
	{
//		CTimeTest	test( true, "Filter takes ", true );

		//m_bInitialized = false;

		if( !_Init() )
		{
			ContextNotification(ncCancelFilter);
			return S_FALSE;
		}
		if( !_Init1() )
		{
			ContextNotification(ncCancelFilter);
			return S_FALSE;
		}
		
		m_bInsideInvokeFilter = true;
		if( !InvokeFilter() )
		{
			if(m_bNotifyBegin)
			{
				DoResetData();
				TerminateNotification();
			}
			ContextNotification(ncCancelFilter);
			return S_FALSE;
		}
		m_bInsideInvokeFilter = false;

		//if( !_PlaceArguments() )
		//	return false;

		// find base key
		_bstr_t strBaseParamName = GetBaseKeyParamName();
		CFilterArgument	* pa = 0;
		long pos = m_arguments.head();

		while (pos)
		{
			pa = m_arguments.next(pos);
			if (!pa || !pa->m_punkArg)
				continue;

			if( !pa->m_bInArgument )
				continue;

			// save base object key and type
			if (strBaseParamName == pa->m_bstrArgName ||  // if param name is given
				(strBaseParamName.length() == 0 && m_lBaseKey == INVALID_KEY)) // or first appropriate
			{
				INamedDataObject2Ptr sptrObj = pa->m_punkArg;
				if (sptrObj == 0)
					continue;

				// for virtual objects only 
				if (pa->m_lParentKey != INVALID_KEY || pa->m_bVirtual) /// virtual Obj
				{	
					// get parent
					IUnknownPtr sptrParent;
					if (pa->m_lParentKey != INVALID_KEY)
						sptrParent.Attach(::GetObjectByKey(m_ptrTarget, pa->m_lParentKey), false);
					sptrObj = sptrParent;
				}

				// check object is exists
				if (sptrObj == 0)
					continue;

				BOOL bBase = false;
				// if object is base
				//sptrObj->IsBaseObject(&bBase);
				//if (bBase)
				{
					// get base key
					sptrObj->GetBaseKey(&m_lBaseKey);
					// and break
					break;
				}
			}
		}
		//ASSERT (m_lBaseKey != INVALID_KEY);

		// set base keys for result argument 
		// and change base and source keys for other needed objects in Data (m_ptrTarget)
		if (m_lBaseKey != INVALID_KEY)
		{
			//find new base objects
			CFilterArgument	* pa = 0;
			long pos = m_arguments.head();
			
			while (pos)
			{
				// get next argument
				pa = m_arguments.next(pos);

				// check it's out arg && it's has Unknown && type of arg is same as needed
				if (pa->m_bInArgument || !pa->m_punkArg || pa->m_bVirtual || pa->m_lParentKey != INVALID_KEY)
					continue;

				// check it's NamedDataObject
				INamedDataObject2Ptr sptr = pa->m_punkArg;
				if (sptr == 0)
					continue;

				sptr->SetBaseKey(&m_lBaseKey);
			}
		}

		// 15.11.2004 build 72
		SetModified(true);


		FinishNotification();
		ContextNotification(ncPassFilter);

		
		return S_OK;

	}
	catch( CFilterErrorInfo *pe )
	{
		delete pe;
		ContextNotification(ncCancelFilter);
		return S_FALSE;
	}
	catch( TerminateLongException *pe )
	{
		pe->Delete();
		DoResetData();
		ContextNotification(ncCancelFilter);
		return S_FALSE;
	}
	catch( ... )
	{
		if(m_bNotifyBegin)
		{
			DoResetData();
			TerminateNotification();
		}
		ContextNotification(ncCancelFilter);
		bool bDebug =  ::GetValueInt( ::GetAppUnknown(), DEBUGGER_SECTION, DEBUGGER_ENABLE, 0 ) != 0;	
		if(bDebug)
		{
			if( m_ptrAI )
			{
				_bstr_t bstr;
				m_ptrAI->GetCommandInfo( bstr.GetAddress(), 0, 0, 0 );

				::MessageBox( 0, (char *)_bstr_t( _bstr_t( "Unhandled exception in filter " ) + bstr ), "Warning", MB_OK );
				write_minidump( ::GetCurrentProcessId(), ::GetCurrentThreadId() );
			}
		}
		return S_FALSE;
	}
}
//filter's virtuals - should be overrided in derived classes
bool CFilter::InvokeFilter()	//here filter should preform work
{
	MessageBox( 0, "Filter's body required", "Error", MB_OK );
	return false;
}

_bstr_t CFilter::GetBaseKeyParamName()
{
	return "";
}

IUnknown *CFilter::GetDataArgument( const char *szArgName )
{
	bool bReturnFirst = szArgName == 0;

	long	pos = m_arguments.head();

	CFilterArgument	*pa = 0, *pargRes = 0;

    while( pos )
	{
		pa = m_arguments.next( pos );
		if( pa->m_bInArgument )
		{
			if( bReturnFirst || !strcmp( pa->m_bstrArgName, szArgName ) )
			{
				pargRes = pa;
				break;
			}
		}
	}

	if( !pargRes )
		return 0;

	if( !pargRes->m_punkArg )
		return 0;

	if( CanDeleteArgument( pargRes ) )
		pargRes->m_bNameDataChanged = true;

	return pargRes->m_punkArg;
}

IUnknown *CFilter::GetDataResult( const char *szArgName )
{
	bool bReturnFirst = szArgName == 0;

	long	pos = m_arguments.head();

	CFilterArgument	*pa = 0, *pargRes = 0;

	while( pos )
	{
		pa = m_arguments.next( pos );

		if( !pa->m_bInArgument )
		{
			if( bReturnFirst || !strcmp( pa->m_bstrArgName, szArgName ) )
			{
				pargRes = pa;
				break;
			}
		}
	}

	if( !pargRes )
		return 0;

	if( !pargRes->m_punkArg )
	{
		_CreateArgument( pargRes );
//if no notification begins, don't place the argument in NamedData, because it isn't initialized
//if notification allredy begun, we can't do anytthing with init and can't preform delayed transpher to NamedData
/*		if( m_bNotifyBegin )
		{
			_variant_t	var( pargRes->m_punkArg );
			::SetValue( m_ptrTarget, 0, 0, var );

			pargRes->m_bNameDataChanged = true;
		}*/
		

	}
	return pargRes->m_punkArg;
}


bool CFilter::StartNotification( int nCYCount, int nStageCount, int nNotifyFreq )
{
	if( m_bNotifyBegin )
		return true;
	m_bNotifyBegin = true;

	if( !CLongOperationImpl::StartNotification( nCYCount, nStageCount, nNotifyFreq ) )
		return false;

	if( m_bLockChangeData )
		return true;

	//now every arguments and results chould be created, but no data was changed
	//detach the virtual arguments now
	long	pos = m_arguments.head();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
			continue;
		if( (::GetObjectFlags( pa->m_punkArg )& nofHasData) == 0 )
			pa->m_bVirtual = true;

		if( pa->m_bInArgument )
		{
			INamedDataObject2Ptr	sptr( pa->m_punkArg );
			if( pa->m_bNameDataChanged )
			{
				::DeleteObject( m_ptrTarget, pa->m_punkArg );
				if( pa->m_bVirtual )
					sptr->SetParent( 0, sdfCopyParentData|apfNotifyNamedData );
				else
					sptr->SetParent( 0, apfNotifyNamedData );
			}
			else
			{
				if( pa->m_bVirtual )
					sptr->StoreData( sdfCopyParentData );
			}
		}
	}

	//place all the initialized result's to the NamedData
	return true;
}

bool CFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return GetValueInt( GetAppUnknown(), "General", "DeleteFilterArgs", 1 ) !=0;
}

bool CFilter::NeedActivateResult( CFilterArgument *pa )
{
	return GetValueInt( GetAppUnknown(), "General", "ActivateFilterResult", 1 );
}

IUnknown *CFilter::GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext )
{
	long	nActiveContextObjectCount = 0;
	_bstr_t	bstrType = szType;
	IUnknown	*punkDataObject = 0;
	
	IDataContext3Ptr	ptrC( punkContext );

	// -->> paul 26.10.2000
	if( ptrC == NULL )
		return NULL;
	// <<-- paul 26.10.2000

	ptrC->GetSelectedCount( bstrType, &nActiveContextObjectCount );
	//punkContext->GetTotalObjectCount( bstrType, &nActiveContextObjectCount );

	if( nCurrentPos >= nActiveContextObjectCount )
		return 0;

	//punkContext->GetTotalObject( bstrType, nCurrentPos, &punkDataObject );
	long	lPosSelected = 0;
	ptrC->GetFirstSelectedPos( bstrType, &lPosSelected );
	while( lPosSelected )
	{
		ptrC->GetNextSelected( bstrType, &lPosSelected, &punkDataObject );

		if( !nCurrentPos )return punkDataObject;
		if( punkDataObject )punkDataObject->Release();
		nCurrentPos--;
	}

	return 0;
}

bool CFilter::_Init()
{
	if( m_bInitialized )
		return true;

	if( m_ptrAI == 0 )
		return false;

	if( m_ptrTarget == 0 )
		return false;

	IUnknown *punkContext = _GetActiveContext();
	if( !punkContext )
		return false;

	sptrIDataContext2	sptrC( punkContext );

	if( sptrC == 0 )
		return false;

	int	iArgCount;

	sptrIActionInfo2	sptrA(m_ptrAI);

	sptrA->GetArgsCount( &iArgCount );

	typedef std::map<std::string, long>	str2long;
	typedef std::map<std::string, GUID>	str2guid;

	str2long		mapTypesToUsedObject;
	str2guid		mapTypesToObjectParents;
	/*
	CMap<CString, LPCTSTR, long, long&>
		mapTypesToUsedObject;
	CMap<CString, LPCTSTR, GUID, GUID&>
		mapTypesToObjectParents;
	*/

	int i;
	for( i = 0; i < iArgCount; i++ )
	{
		BSTR	bstrName;
		BSTR	bstrType;

		sptrA->GetArgInfo( i, &bstrName, &bstrType, 0 );

		_bstr_t strArgument = bstrName;
		_bstr_t strType = bstrType;

		::SysFreeString( bstrName );
		::SysFreeString( bstrType );

		//it is "typed" argument
		BOOL	bDataObject = FALSE;
		sptrA->IsArgumentDataObject( i, true, &bDataObject );

		if( bDataObject )
		{
			IUnknown *punkDataObject = 0;

			long nCount = 0;
			str2long::iterator r;
			
			r = mapTypesToUsedObject.find( (const char *)strType );
			if( r != mapTypesToUsedObject.end() )
				nCount = (*r).second;

			//mapTypesToUsedObject.Lookup( strType, nCount );

///////////////////////
			if( !m_bLockChangeData )
			{
				_bstr_t	strObject = GetArgString( strArgument );
		
				if( strObject.length() !=0 )
					punkDataObject = ::GetObjectByName( m_ptrTarget, strObject, strType );
				if( !punkDataObject )
					punkDataObject = GetObjectArgument( strType, nCount, sptrC );

				if( !punkDataObject )
				{
					if(IsAllArgsRequired())
						return false;
					else
						break;
				}

			}
///////////////////////

			mapTypesToUsedObject[(const char*)strType] = nCount+1;

			CFilterArgument	*parg = new CFilterArgument();
			parg->m_bInArgument = true;
			parg->m_punkArg = punkDataObject;
			parg->m_bstrArgName = (const char *)strArgument;
			parg->m_bstrArgType = (const char *)strType;
			parg->m_lParentKey = GetParentKey( punkDataObject );

			mapTypesToObjectParents[(const char*)strType] = parg->m_lParentKey;

			m_arguments.insert( parg );		
		}
	}


	sptrA->GetOutArgsCount( &iArgCount );

	for( i = 0; i < iArgCount; i++ )
	{
		BSTR	bstrName;
		BSTR	bstrType;

		sptrA->GetOutArgInfo( i, &bstrName, &bstrType );

		_bstr_t strArgument = bstrName;
		_bstr_t strType = bstrType;

		::SysFreeString( bstrName );
		::SysFreeString( bstrType );

		//it is "typed" argument
		BOOL	bDataObject = FALSE;
		sptrA->IsArgumentDataObject( i, false, &bDataObject );

		if( bDataObject )
		{
			CFilterArgument	*parg = new CFilterArgument();
			parg->m_bInArgument = false;
			parg->m_punkArg = 0;
			parg->m_bstrArgName = (const char *)strArgument;
			parg->m_bstrArgType = (const char *)strType;
			parg->m_lParentKey = INVALID_KEY;

			//extract parent key
			str2guid::iterator	ig = mapTypesToObjectParents.find( (const char*)strType );
			if( ig != mapTypesToObjectParents.end() )
				parg->m_lParentKey = (*ig).second;

//special case - it is single out argument
			m_arguments.insert( parg );		
		}
	}

	m_bInitialized = true;
	return true;
}

bool CFilter::_DeInit()
{
	long	lpos = m_arguments.head();
	while( lpos )
	{
		CFilterArgument*	p = m_arguments.next( lpos );
		delete	p;
	}
	m_arguments.deinit();

	m_bInitialized = false;

	return true;
}


bool CFilter::_PlaceArguments( bool bDirection, bool bRemoveOnly )
{
	if( m_bLockChangeData )
		return true;

	//ASSERT( m_ptrTarget );

	long	pos = m_arguments.head();

	while( pos )
	{
		CFilterArgument	*pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
				continue;

		if( pa->m_bInArgument == bDirection )
		{
			if( !pa->m_bNameDataChanged && pa->m_bVirtual )
			{
				INamedDataObject2Ptr	sptr( pa->m_punkArg );
				sptr->StoreData( sdfCopyParentData );
				continue;
			}

			//paul 11.04.2002
			if( !pa->m_bNameDataChanged )
				continue;
		
			sptrINamedDataObject2	sptr( pa->m_punkArg );

			DWORD	dwFlags;
			sptr->GetObjectFlags( &dwFlags );

			pa->m_bVirtual = (dwFlags & nofHasData) == 0;
			pa->m_lParentKey = ::GetParentKey( pa->m_punkArg );

			::DeleteObject( m_ptrTarget, pa->m_punkArg );

			if( pa->m_bVirtual )
				sptr->SetParent( 0, sdfCopyParentData|apfNotifyNamedData );
			else
				sptr->SetParent( 0, apfNotifyNamedData );
		}
	}

	if( bRemoveOnly )
		return true ;

	pos = m_arguments.head();
	
	while( pos )
	{
		CFilterArgument *pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
			continue;

		if( pa->m_bInArgument != bDirection )
		{
			INamedDataObject2Ptr	sptr( pa->m_punkArg );

			if( !pa->m_bNameDataChanged && pa->m_bVirtual )
			{
				if( pa->m_bVirtual )
					sptr->StoreData( sdfCopyLocalData|sdfAttachParentData );
				continue;
			}
			bool	bSetValue = true;
			
			if( pa->m_lParentKey != INVALID_KEY )
			{
				IUnknown *punk = ::GetObjectByKey( m_ptrTarget, pa->m_lParentKey );

				if( !punk )	//may be we remove it from data - find it in arguuments
				{
					long	pos1 = pos;
					while( pos1 )
					{
						CFilterArgument *pa1 = m_arguments.next( pos1 );
						if( pa1->m_bInArgument != bDirection )
							if( pa1->m_bNameDataChanged )
								if( ::GetKey( pa1->m_punkArg ) == pa->m_lParentKey )
								{
									punk = pa1->m_punkArg;
									punk->AddRef();
									bSetValue = false;
									break;
								}
					}
				}
				if( punk )
				{
					if( pa->m_bVirtual )
						sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );
					else
						sptr->SetParent( punk, apfNotifyNamedData );
					punk->Release();
				}
			}

			if( bSetValue )
			{
				_variant_t	var( pa->m_punkArg );
				::SetValue( m_ptrTarget, 0, 0, var );
			}
		}
	}
	

	return true;
}

IUnknown *CFilter::_GetActiveContext( bool bAddRef )
{
	IUnknown *punkView = 0;


	/*if( CheckInterface( m_ptrTarget, IID_IDataContext ) )
	{
		if( bAddRef )m_ptrTarget->AddRef();
		return m_ptrTarget;
	} */

	sptrIDocumentSite	sptrS( m_ptrTarget );

//	if( sptrS == 0 )
//		return 0;
	if( sptrS != 0 )
		sptrS->GetActiveView( &punkView );

	if( !punkView )
	{
		if( CheckInterface( m_ptrTarget, IID_IDataContext ) )
		{
			m_ptrTarget->AddRef();
			punkView = m_ptrTarget;
		}
	}

	if( !punkView )
		return 0;

	if( !bAddRef )
		punkView->Release();

	return punkView;
}


IUnknown *CFilter::_CreateArgument( CFilterArgument *parg )
{
	/*XArgumentIterator	i =  m_arguments.begin();

	CFilterArgument	*pa = 0;

    for (i =  m_arguments.begin(); i != m_arguments.end(); ++i)
		if( pa->m_bInArgument && pa->m_bstrArgType == szType && pa->m_punkArg && ::HasParentObject( pa->m_punkArg )  )
			return ::CloneObject( pa->m_punkArg );*/

	IUnknown	*punk = ::CreateTypedObject( parg->m_bstrArgType );
	parg->m_punkArg = punk;
	parg->m_bNameDataChanged = true;

	if( !punk )return 0;

	_SetArgumentName( parg );
	

	return punk;
}

void CFilter::_SetArgumentName( CFilterArgument *parg )
{
	if( parg->m_bInArgument )
		return;
	_bstr_t strArgName = GetArgString( parg->m_bstrArgName );

	if( strArgName.length()==0 )
	{
		int			nCountOfArgsThisType = 0;
		_bstr_t		strSingleArgName;
		strArgName = parg->m_bstrArgName;
		
		long pos = m_arguments.head();
		while( pos )
		{
			CFilterArgument *pa = m_arguments.next( pos );
			if( !pa->m_bInArgument )
				continue;

			if( pa->m_bstrArgType != parg->m_bstrArgType )
				continue;

			if( !CanDeleteArgument( pa ) )
				continue;


			if( nCountOfArgsThisType == 0 )
			{
				strSingleArgName = ::GetName( pa->m_punkArg );
				nCountOfArgsThisType++;
			}
		}

		if( nCountOfArgsThisType == 1 && strSingleArgName.length() != 0 )
			strArgName = strSingleArgName;
		else
		{
			_bstr_t bstrArgName = parg->m_bstrArgName;
			if( m_ptrAI )
			{
				_bstr_t bstrActionName;
				m_ptrAI->GetCommandInfo( bstrActionName.GetAddress(), 0, 0, 0 );
				IAliaseMan2Ptr	ptrMan( GetAppUnknown() );
				if( ptrMan != 0 )
				{
					_bstr_t bstrA;
					if( ptrMan->GetActionArgumentString( bstrActionName, parg->m_bstrArgName, bstrA.GetAddress() ) == S_OK )
					{
						if(bstrA.length()!=0) bstrArgName = bstrA;
					}
				}
			}

			strArgName = GenerateNameForArgument( bstrArgName, m_ptrTarget );
		}
	}

	SetName( parg->m_punkArg, strArgName );
}

bool CFilter::FinishNotification()
{
	if( !m_bNotifyBegin )
		return false;
	if( m_bNotifyFinished )
		return true;
	if( m_bInsideInvokeFilter )
		return false;

	m_bNotifyFinished = true;

	if( m_bLockChangeData )
		return CLongOperationImpl::FinishNotification();

	
	long	pos = m_arguments.head();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
			continue;

		if( pa->m_bInArgument && pa->m_bNameDataChanged )
		{
			//only no-virtual arguments here. 
			if( !pa->m_bVirtual )
			{
				::DeleteObject( m_ptrTarget, pa->m_punkArg );

				INamedDataObject2Ptr	sptr( pa->m_punkArg );

				//pa->m_bVirtual = true;
				sptr->SetParent( 0, apfNotifyNamedData );
			}
		}

		if( !pa->m_bInArgument )
		{
//			if( pa->m_lParentKey != -1 )
//			{
//				IUnknown *punk = ::GetObjectByKey( m_ptrTarget, pa->m_lParentKey );
//				
//				sptrINamedDataObject2	sptr( pa->m_punkArg );
//				if( pa->m_bVirtual )
//					sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );
//				else
//					sptr->SetParent( punk, apfNotifyNamedData );
//				punk->Release();
//			} 
			_variant_t	var( pa->m_punkArg );
			::SetValue( m_ptrTarget, 0, 0, var );		

			if( pa->m_bVirtual && CheckInterface(pa->m_punkArg, IID_IImage))
			{
				INamedDataObject2Ptr	ptrObject(pa->m_punkArg);
				IUnknown* punkParent = 0;
				if(ptrObject != 0)
					ptrObject->GetParent(&punkParent);
				INamedDataObject2Ptr	ptrParent(punkParent);
				if(punkParent)
					punkParent->Release();
				if(ptrParent != 0)
				{
					IUnknown	*punkData = 0;
					ptrParent->GetData(&punkData);
					if(punkData != 0)
					{
						INotifyControllerPtr	sptr(punkData);
						if(sptr != 0)
							sptr->FireEvent(_bstr_t("NeedUpdateView"), punkData, pa->m_punkArg, 0, 0 );
						
						punkData->Release();
					}
				}

			}
		}
	}
	
//	_PlaceArguments();



	return CLongOperationImpl::FinishNotification();
}

//this is called from InvokeFilter
//the old arguments are in data, except virtual
//new arguments are not in data

bool CFilter::DoResetData()
{
	if( m_bLockChangeData )
		return true;

	long	pos = m_arguments.head();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
			continue;

		if( !pa->m_punkArg )
			continue;

		if( !pa->m_bInArgument )
			continue;

		if( !pa->m_bNameDataChanged )
			continue;

		if( pa->m_bVirtual )
		{
			IUnknown *punk = ::GetObjectByKey( m_ptrTarget, pa->m_lParentKey );
			sptrINamedDataObject2	sptr( pa->m_punkArg );
			sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );

			if( punk )
				punk->Release();
		}

		_variant_t	var( pa->m_punkArg );
		::SetValue( m_ptrTarget, 0, 0, var );
	}
	return true;
}


void CFilter::DoFinalizePreview()
{
}


void CFilter::DoInitializePreview()
{
}


bool IsPaneEnable( int nPane )
{
	char szPane[20];
	sprintf( szPane, "Pane%d", nPane );	
	long nEnable = ::GetValueInt( GetAppUnknown(), "Filters", szPane, 1 );

	return (nEnable == 1);
}
