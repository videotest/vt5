#include "stdafx.h"
#include "filterbase.h"
#include "data5.h"
#include "docviewbase.h"
#include "utils.h"
#include "NameConsts.h"
#include "timetest.h"
#include "script_int.h"
#include "aliaseint.h"

#include "dump_util.h"

BEGIN_INTERFACE_MAP(CFilterBase, CActionBase)
	INTERFACE_PART(CFilterBase, IID_ILongOperation, Long)
	INTERFACE_PART(CFilterBase, IID_IFilterAction, Filter)
	INTERFACE_PART(CFilterBase, IID_IFilterAction2, Filter)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CFilterBase, Filter);



HRESULT CFilterBase::XFilter::GetFirstArgumentPosition(POSITION *pnPos)
{
	_try_nested_base( CFilterBase, Filter, GetFirstArgumentPosition )
	{
		
		*pnPos = pThis->m_arguments.GetHeadPosition();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::GetNextArgument(IUnknown **ppunkDataObject, POSITION *pnPosNext)
{
	_try_nested_base( CFilterBase, Filter, GetFirstArgumentPosition )
	{
		POSITION	pos = (POSITION)*pnPosNext;
		CFilterArgument	*parg = pThis->m_arguments.GetNext( pos );
		*pnPosNext = pos;

		if( ppunkDataObject )
		{
			*ppunkDataObject = parg->m_punkArg;
			if( *ppunkDataObject )
				(*ppunkDataObject)->AddRef();
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::SetArgument(POSITION lPos, IUnknown *punkDataObject, BOOL bDataChanged)
{
	_try_nested_base( CFilterBase, Filter, SetArgument)
	{
		POSITION	pos = (POSITION)lPos;
		CFilterArgument	*parg = pThis->m_arguments.GetNext( pos );

		if( parg->m_punkArg )
			parg->m_punkArg->Release();
		parg->m_punkArg = punkDataObject;
		parg->m_bNameDataChanged = bDataChanged == TRUE;
		if( parg->m_punkArg )
			parg->m_punkArg->AddRef();

		pThis->_SetArgumentName( parg );
		parg->m_lParentKey = ::GetParentKey( parg->m_punkArg );
		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::CanDeleteArgument( IUnknown *punk, BOOL *pbCan )
{
	_try_nested_base( CFilterBase, Filter, CanDeleteArgument)
	{
		*pbCan = false;
		POSITION pos = pThis->m_arguments.GetHeadPosition();
		while( pos )
		{
			CFilterArgument	*pa = pThis->m_arguments.GetNext( pos );
			if( pa->m_punkArg == punk )
			{
				*pbCan = pThis->CanDeleteArgument( pa );
				return S_OK;
			}
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::NeedActivateResult( IUnknown *punk, BOOL *pbCan )
{
	_try_nested_base( CFilterBase, Filter, NeedActivateResult)
	{
		*pbCan = true;
		POSITION pos = pThis->m_arguments.GetHeadPosition();
		while( pos )
		{
			CFilterArgument	*pa = pThis->m_arguments.GetNext( pos );
			if( pa->m_punkArg == punk )
			{
				*pbCan = pThis->NeedActivateResult( pa );
				return S_OK;
			}
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::GetArgumentInfo(POSITION lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut)
{
	_try_nested_base( CFilterBase, Filter, GetArgumentInfo)
	{
		POSITION	pos = (POSITION)lPos;
		CFilterArgument	*parg = pThis->m_arguments.GetNext( pos );
		if( pbstrArgType )
			*pbstrArgType = parg->m_strArgType.AllocSysString();

		if( pbstrArgName )
			*pbstrArgName = parg->m_strArgName.AllocSysString();

		if( pbOut )
			*pbOut = !parg->m_bInArgument;


		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::InitArguments()
{
	_try_nested_base( CFilterBase, Filter, GetArgumentInfo)
	{
		return pThis->_Init()?S_OK:S_FALSE;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::LockDataChanging( BOOL bLock )
{
	_try_nested_base( CFilterBase, Filter, LockDataChanging)
	{
		pThis->m_bLockChangeData = bLock != FALSE;
		if( !pThis->m_bLockChangeData )
			pThis->MakeModified();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFilterBase::XFilter::SetPreviewMode( BOOL bEnter )
{
	_try_nested_base( CFilterBase, Filter, LockDataChanging)
	{
		if( bEnter )
		{
			pThis->m_bPreviewMode = true;
			pThis->DoInitializePreview();
		}
		else
		{
			if( pThis->IsPreviewMode() )
			{
				pThis->DoFinalizePreview();
				pThis->m_bPreviewMode = false;
			}
		}
		return S_OK;
	}
	_catch_nested;
}

void CFilterBase::MakeModified()
{
	if( m_bPreviewMode )
		return;
	CActionBase::MakeModified();
}


CFilterBase::CFilterBase()
{
	m_bInitialized = false;
	m_bNotifyBegin = false;
	m_bNotifyFinished = false;
	m_bInsideInvokeFilter = false;
	m_bLockChangeData = false;
	m_bPreviewMode = false;

}
CFilterBase::~CFilterBase()
{
	_DeInit();
}

//action's virtuals
bool CFilterBase::DoUndo()
{
	ContextNotification(ncEnterFilter);
	bool bRet = _PlaceArguments( false );
	ContextNotification(ncFilterUndo);
	return bRet;
}

bool CFilterBase::DoRedo()
{
	ContextNotification(ncEnterFilter);
	bool bRet = _PlaceArguments();
	ContextNotification(ncFilterRedo);
	return bRet;
}

bool CFilterBase::IsAvaible()
{
	if( !_Init() )
		return false;
	return true;
}

bool CFilterBase::_Init1()
{
	POSITION pos = m_arguments.GetHeadPosition();
	while (pos)
	{
		CFilterArgument	* parg = m_arguments.GetNext( pos );
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


void CFilterBase::ContextNotification(long lCod)
{
	IDocumentSitePtr sptrDS = m_punkTarget;
	if (sptrDS == 0)
		return;

	TPOS lPos = 0;
	sptrDS->GetFirstViewPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptr;
		sptrDS->GetNextView(&sptr, &lPos);
		IDataContextPtr sptrC = sptr;
		if (sptrC != 0)
			sptrC->Notify(lCod, GetControllingUnknown(), 0, 0);
	}
	IDataContextPtr sptrC = m_punkTarget;
	if (sptrC != 0)
		sptrC->Notify(lCod, GetControllingUnknown(), 0, 0);
}

bool CFilterBase::Invoke()
{
	ContextNotification(ncEnterFilter);
	try
	{
//		CTimeTest	test( true, "Filter takes ", true );

		//m_bInitialized = false;

		if( !_Init() )
		{
			ContextNotification(ncCancelFilter);
			return false;
		}
		if( !_Init1() )
		{
			ContextNotification(ncCancelFilter);
			return false;
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
			return false;
		}
		m_bInsideInvokeFilter = false;

		//if( !_PlaceArguments() )
		//	return false;

		// find base key
		CString strBaseParamName = GetBaseKeyParamName();
		CFilterArgument	* pa = 0;
		POSITION pos = m_arguments.GetHeadPosition();

		while (pos)
		{
			pa = m_arguments.GetNext(pos);
			if (!pa || !pa->m_punkArg)
				continue;

			if( !pa->m_bInArgument )
				continue;

			// save base object key and type
			if (strBaseParamName == pa->m_strArgName ||  // if param name is given
				(strBaseParamName.IsEmpty() && m_lBaseKey == INVALID_KEY)) // or first appropriate
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
						sptrParent.Attach(::GetObjectByKey(m_punkTarget, pa->m_lParentKey), false);
					if (sptrParent != 0)
						sptrObj = sptrParent;
				}

				ASSERT(sptrObj != 0);

				// check object is exists
				if (sptrObj == 0)
					continue;

				BOOL bBase = false;
				// if object is base
				sptrObj->IsBaseObject(&bBase);
				if (bBase || strBaseParamName == pa->m_strArgName)
				{
					// get base key
					sptrObj->GetBaseKey(&m_lBaseKey);
					// and break
					break;
				}
			}
		}
		if(m_lBaseKey == INVALID_KEY)
		{
			POSITION pos = m_arguments.GetHeadPosition();
			while (pos)
			{
				pa = m_arguments.GetNext(pos);
				if (!pa || !pa->m_punkArg)
					continue;

				if( !pa->m_bInArgument )
					continue;

				// save base object key and type
				if (strBaseParamName == pa->m_strArgName ||  // if param name is given
					(strBaseParamName.IsEmpty() && m_lBaseKey == INVALID_KEY)) // or first appropriate
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
							sptrParent.Attach(::GetObjectByKey(m_punkTarget, pa->m_lParentKey), false);
						if (sptrParent != 0)
							sptrObj = sptrParent;
					}

					ASSERT(sptrObj != 0);

					// check object is exists
					if (sptrObj == 0)
						continue;

					// get base key
					sptrObj->GetBaseKey(&m_lBaseKey);
					// and break
					if(m_lBaseKey != INVALID_KEY)
						break;
				}
			}
		}
		ASSERT (m_lBaseKey != INVALID_KEY);

		// set base keys for result argument 
		// and change base and source keys for other needed objects in Data (m_punkTarget)
		if (m_lBaseKey != INVALID_KEY)
		{
			//find new base objects
			CFilterArgument	* pa = 0;
			POSITION pos = m_arguments.GetHeadPosition();
			
			while (pos)
			{
				// get next argument
				pa = m_arguments.GetNext(pos);

				// check it's out arg && it's has Unknown && type of arg is same as needed
				if (pa->m_bInArgument || !pa->m_punkArg || pa->m_bVirtual/* || pa->m_lParentKey != INVALID_KEY*/)
					continue;

				// check it's NamedDataObject
				INamedDataObject2Ptr sptr = pa->m_punkArg;
				if (sptr == 0)
					continue;

				sptr->SetBaseKey(&m_lBaseKey);
			}
		}

		FinishNotification();
		ContextNotification(ncPassFilter);
		
		return true;

	}
	catch( CFilterErrorInfo *pe )
	{
		delete pe;
		ContextNotification(ncCancelFilter);
		return false;
	}
	catch( CTerminateException *pe )
	{
		pe->Delete();

		DoResetData();
		ContextNotification(ncCancelFilter);
		return false;
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
		if( bDebug )
		{
			if( m_pActionInfo )
			{
				_bstr_t bstr;
				m_pActionInfo->GetCommandInfo( bstr.GetAddress(), 0, 0, 0 );

				::MessageBox( 0, CString( "Unhandled exception in filter " ) + (char *)bstr, "Warning", MB_OK );

				write_minidump( ::GetCurrentProcessId(), ::GetCurrentThreadId());
			}
		}
		return false;
	}
}
//filter's virtuals - should be overrided in derived classes
bool CFilterBase::InvokeFilter()	//here filter should preform work
{
	AfxMessageBox( "Filter's body required" );
	return false;
}

CString CFilterBase::GetBaseKeyParamName()
{
	return _T("");
}

IUnknown *CFilterBase::GetDataArgument( const char *szArgName )
{
	bool bReturnFirst = szArgName == 0;

	POSITION	pos = m_arguments.GetHeadPosition();

	CFilterArgument	*pa = 0, *pargRes = 0;

    while( pos )
	{
		pa = m_arguments.GetNext( pos );
		if( pa->m_bInArgument )
		{
			if( bReturnFirst || pa->m_strArgName == szArgName )
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

IUnknown *CFilterBase::GetDataResult( const char *szArgName )
{
	bool bReturnFirst = szArgName == 0;

	POSITION	pos = m_arguments.GetHeadPosition();

	CFilterArgument	*pa = 0, *pargRes;

	while( pos )
	{
		pa = m_arguments.GetNext( pos );

		if( !pa->m_bInArgument )
		{
			if( bReturnFirst || pa->m_strArgName == szArgName )
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
			::SetValue( m_punkTarget, 0, 0, var );

			pargRes->m_bNameDataChanged = true;
		}*/
		

	}
	return pargRes->m_punkArg;
}


bool CFilterBase::StartNotification( int nCYCount, int nStageCount, int nNotifyFreq )
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
	POSITION	pos = m_arguments.GetHeadPosition();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.GetNext( pos );

		if( !pa->m_punkArg )
			continue;
		if( (::GetObjectFlags( pa->m_punkArg )& nofHasData) == 0 && ::CheckInterface(pa->m_punkArg, IID_IImage))
			pa->m_bVirtual = true;

		if( pa->m_bInArgument )
		{
			INamedDataObject2Ptr	sptr( pa->m_punkArg );
			if( pa->m_bNameDataChanged )
			{
				::DeleteObject( m_punkTarget, pa->m_punkArg );
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

bool CFilterBase::CanDeleteArgument( CFilterArgument *pa )
{
	return GetValueInt( GetAppUnknown(), "General", "DeleteFilterArgs", 1 ) !=0;
}

bool CFilterBase::NeedActivateResult( CFilterArgument *pa )
{
	return GetValueInt( GetAppUnknown(), "General", "ActivateFilterResult", 1 );
}

IUnknown *CFilterBase::GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext )
{
	ASSERT(punkContext);

	long	lActiveContextObjectCount = 0;
	_bstr_t	bstrType = szType;
	IUnknown	*punkDataObject = 0;
	
	IDataContext3Ptr	ptrC( punkContext );

	// -->> paul 26.10.2000
	if( ptrC == NULL )
		return NULL;
	// <<-- paul 26.10.2000

	ptrC->GetSelectedCount( bstrType, &lActiveContextObjectCount );
	//punkContext->GetTotalObjectCount( bstrType, &lActiveContextObjectCount );

	if( nCurrentPos >= (int)lActiveContextObjectCount )
		return 0;

	//punkContext->GetTotalObject( bstrType, nCurrentPos, &punkDataObject );
	LONG_PTR	lPosSelected = 0;
	ptrC->GetFirstSelectedPos( bstrType, &lPosSelected );
	while( lPosSelected )
	{
		ptrC->GetNextSelected( bstrType, &lPosSelected, &punkDataObject );

    if(!IsObjectMatch(punkDataObject))
    {
      if( punkDataObject )punkDataObject->Release();
      continue;
    }

		if( !nCurrentPos )
				return punkDataObject;
		
		if( punkDataObject )punkDataObject->Release();
		nCurrentPos--;
	}

	return 0;
}

bool CFilterBase::_Init()
{
	if( m_bInitialized )
		return true;

	if( !m_pActionInfo )
		return false;

	if( !m_punkTarget )
		return false;

	IUnknown *punkContext = _GetActiveContext();
	if( !punkContext )
		return false;

	sptrIDataContext2	sptrC( punkContext );

	if( sptrC == 0 )
		return false;

	int	iArgCount;

	sptrIActionInfo2	sptrA(m_pActionInfo);

	sptrA->GetArgsCount( &iArgCount );

	CMap<CString, LPCTSTR, long, long&>
		mapTypesToUsedObject;
	CMap<CString, LPCTSTR, GUID, GUID&>
		mapTypesToObjectParents;

	for( int i = 0; i < iArgCount; i++ )
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
			mapTypesToUsedObject.Lookup( strType, nCount );

///////////////////////
			if( !m_bLockChangeData )
			{
				CString	strObject = GetArgumentString(strArgument);
		
				if( !strObject.IsEmpty() )
					punkDataObject = ::GetObjectByName( m_punkTarget, strObject, strType );
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

			mapTypesToUsedObject[strType] = nCount+1;

			CFilterArgument	*parg = new CFilterArgument();
			parg->m_bInArgument = true;
			parg->m_punkArg = punkDataObject;
			parg->m_strArgName = (const char *)strArgument;
			parg->m_strArgType = (const char *)strType;

			//get parent's key
			if( punkDataObject )
			{
				sptrINamedDataObject2	sptrN( punkDataObject );

				IUnknown	*punk = 0;
				sptrN->GetParent( &punk );

				parg->m_lParentKey = ::GetObjectKey( punk );

				if( punk ) punk->Release();

			}

			mapTypesToObjectParents[strType] = parg->m_lParentKey;

			m_arguments.AddTail( parg );		
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
			parg->m_strArgName = (const char *)strArgument;
			parg->m_strArgType = (const char *)strType;

			//extract parent key
			GUID	lKey;
			if( mapTypesToObjectParents.Lookup( strType, lKey ) )
				parg->m_lParentKey = lKey;

//special case - it is single out argument
			m_arguments.AddTail( parg );		
		}
	}

	m_bInitialized = true;
	return true;
}

bool CFilterBase::_DeInit()
{
	POSITION	pos = m_arguments.GetHeadPosition();
	while( pos )delete m_arguments.GetNext( pos );

	m_arguments.RemoveAll();

	m_bInitialized = false;

	return true;
}

void CFilterBase::_MakeFilterArgumentsArray(CArray<POSITION,POSITION&> &arrArgs)
{
	POSITION	pos = m_arguments.GetHeadPosition();
	while( pos )
	{
		POSITION pos1 = pos;
		CFilterArgument	*pa = m_arguments.GetNext(pos);
		CString sName = ::GetObjectName(pa->m_punkArg);
		bool bParentOfSomeObject = false;
		for (int i = 0; i < arrArgs.GetSize(); i++)
		{
			POSITION posTest = arrArgs.GetAt(i);
			CFilterArgument	*paTest = m_arguments.GetNext(posTest);
			CString sName1 = ::GetObjectName(paTest->m_punkArg);
			if (/*::GetParentKey(paTest->m_punkArg)*/paTest->m_lParentKey == ::GetObjectKey(pa->m_punkArg))
			{
				bParentOfSomeObject = true;
				break;
			}

		}
		if (bParentOfSomeObject)
			arrArgs.InsertAt(0, pos1);
		else
			arrArgs.Add(pos1);
	}
}

bool CFilterBase::_PlaceArguments( bool bDirection, bool bRemoveOnly )
{
	if( m_bLockChangeData )
		return true;

	ASSERT( m_punkTarget );

	POSITION	pos = m_arguments.GetHeadPosition();

	while( pos )
	{
		CFilterArgument	*pa = m_arguments.GetNext( pos );

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

			if( !pa->m_bNameDataChanged )
				continue;
		
			sptrINamedDataObject2	sptr( pa->m_punkArg );

			DWORD	dwFlags;
			sptr->GetObjectFlags( &dwFlags );

			pa->m_bVirtual = (dwFlags & nofHasData) == 0;
			pa->m_lParentKey = ::GetParentKey( pa->m_punkArg );

			::DeleteObject( m_punkTarget, pa->m_punkArg );

			if( pa->m_bVirtual )
				sptr->SetParent( 0, sdfCopyParentData|apfNotifyNamedData );
			else
				sptr->SetParent( 0, apfNotifyNamedData );
		}
	}

	if( bRemoveOnly )
		return true ;

	CArray<POSITION,POSITION&> arrArgs;
	_MakeFilterArgumentsArray(arrArgs);
	for (int i = 0; i < arrArgs.GetSize(); i++)
	{
		pos = arrArgs.GetAt(i);
		CFilterArgument *pa = m_arguments.GetNext(pos);
		CString sName = ::GetObjectName(pa->m_punkArg);
//	pos = m_arguments.GetHeadPosition();
//	pos = m_arguments.GetTailPosition();
	
//	while( pos )
//	{
//		CFilterArgument *pa = m_arguments.GetNext( pos );
//		CFilterArgument *pa = m_arguments.GetPrev( pos );

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

			//[ay] - 05.06.2002 - не трогать объекты которые не удалялись
			if( !pa->m_bNameDataChanged )
				continue;
			
			if( pa->m_lParentKey != INVALID_KEY )
			{
				IUnknown *punk = ::GetObjectByKey( m_punkTarget, pa->m_lParentKey );
				ASSERT( punk );
				if( pa->m_bVirtual )
					sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );
				else
					sptr->SetParent( punk, apfNotifyNamedData );
				if (punk)
					punk->Release();
			}

			_variant_t	var( pa->m_punkArg );
			::SetValue( m_punkTarget, 0, 0, var );
		}
	}
	

	return true;
}

IUnknown *CFilterBase::_GetActiveContext( bool bAddRef )
{
	IUnknown *punkView = 0;


	/*if( CheckInterface( m_punkTarget, IID_IDataContext ) )
	{
		if( bAddRef )m_punkTarget->AddRef();
		return m_punkTarget;
	} */

	sptrIDocumentSite	sptrS( m_punkTarget );
	if( sptrS )
		sptrS->GetActiveView( &punkView );

	if( !punkView )
	{
		if( CheckInterface( m_punkTarget, IID_IDataContext ) )
		{
			m_punkTarget->AddRef();
			punkView = m_punkTarget;
		}
	}

	if( !punkView )
		return 0;

	if( !bAddRef )
		punkView->Release();

	return punkView;
}


IUnknown *CFilterBase::_CreateArgument( CFilterArgument *parg )
{
	/*XArgumentIterator	i =  m_arguments.begin();

	CFilterArgument	*pa = 0;

    for (i =  m_arguments.begin(); i != m_arguments.end(); ++i)
		if( pa->m_bInArgument && pa->m_strArgType == szType && pa->m_punkArg && ::HasParentObject( pa->m_punkArg )  )
			return ::CloneObject( pa->m_punkArg );*/

	IUnknown	*punk = ::CreateTypedObject( parg->m_strArgType );
	parg->m_punkArg = punk;
	parg->m_bNameDataChanged = true;

	if( !punk )return 0;

	_SetArgumentName( parg );
	

	return punk;
}

void CFilterBase::_SetArgumentName( CFilterArgument *parg )
{
	if( parg->m_bInArgument )
		return;
	CString	strArgName = GetArgumentString( parg->m_strArgName );

	if( strArgName.IsEmpty() )
	{
		int			nCountOfArgsThisType = 0;
		CString		strSingleArgName;
		strArgName = parg->m_strArgName;
		
		POSITION pos = m_arguments.GetHeadPosition();
		while( pos )
		{
			CFilterArgument *pa = m_arguments.GetNext( pos );
			if( !pa->m_bInArgument )
				continue;

			if( pa->m_strArgType != parg->m_strArgType )
				continue;

			if( !CanDeleteArgument( pa ) )
				continue;


			if( nCountOfArgsThisType == 0 )
			{
				strSingleArgName = GetObjectName( pa->m_punkArg );
				nCountOfArgsThisType++;
			}
		}

		if( nCountOfArgsThisType == 1 && !strSingleArgName.IsEmpty() )
			strArgName = strSingleArgName;
		else
		{
			_bstr_t bstrArgName = parg->m_strArgName;
			if( m_pActionInfo )
			{
				_bstr_t bstrActionName;
				m_pActionInfo->GetCommandInfo( bstrActionName.GetAddress(), 0, 0, 0 );
				IAliaseMan2Ptr	ptrMan( GetAppUnknown() );
				if( ptrMan != 0 )
				{
					_bstr_t bstrA;
					if( ptrMan->GetActionArgumentString( bstrActionName, _bstr_t(parg->m_strArgName), bstrA.GetAddress() ) == S_OK )
					{
						if(bstrA.length()!=0) bstrArgName = bstrA;
					}
				}
			}

			strArgName = GenerateNameForArgument( (char*)(bstrArgName), m_punkTarget );
		}
	}

	SetObjectName( parg->m_punkArg, strArgName );
}

POSITION CFilterBase::_FindArgumentByKey(GuidKey key, bool bInArgs, bool bOutArgs)
{
	POSITION	pos = m_arguments.GetHeadPosition();

	while( pos )
	{
		POSITION pos1 = pos;
		CFilterArgument *pa = m_arguments.GetNext( pos );
		if (!pa->m_punkArg)
			continue;
		if (pa->m_bInArgument && !bInArgs)
			continue;
		if (!pa->m_bInArgument && !bOutArgs)
			continue;
		GuidKey keyTest = ::GetObjectKey(pa->m_punkArg);
		if (keyTest == key)
			return pos1;
	}
	return NULL;
}


bool CFilterBase::FinishNotification()
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



	POSITION	pos = m_arguments.GetHeadPosition();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.GetNext( pos );

		if( !pa->m_punkArg )
			continue;

		if( pa->m_bInArgument && pa->m_bNameDataChanged )
		{
			//only no-virtual arguments here. 
			if( !pa->m_bVirtual )
			{
				::DeleteObject( m_punkTarget, pa->m_punkArg );

				INamedDataObject2Ptr	sptr( pa->m_punkArg );

				//pa->m_bVirtual = true;
				sptr->SetParent( 0, apfNotifyNamedData );
			}
		}

		if( !pa->m_bInArgument )
		{
//			if( pa->m_lParentKey != -1 )
//			{
//				IUnknown *punk = ::GetObjectByKey( m_punkTarget, pa->m_lParentKey );
//				
//				sptrINamedDataObject2	sptr( pa->m_punkArg );
//				if( pa->m_bVirtual )
//					sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );
//				else
//					sptr->SetParent( punk, apfNotifyNamedData );
//				punk->Release();
//			} 

			// AM fix SBT1668, also BT4652.
			if( pa->m_bVirtual && CheckInterface(pa->m_punkArg, IID_IImage))
			{
				INamedDataObject2Ptr sptr(pa->m_punkArg);
				IUnknownPtr punkParent;
				if (sptr != 0)
					sptr->GetParent(&punkParent);
				if (punkParent != 0)
				{
					GuidKey keyParent = ::GetObjectKey(punkParent);
					if (_FindArgumentByKey(keyParent, true, false))
						sptr->SetParent(0, sdfCopyParentData);
				}
			}

			_variant_t	var( pa->m_punkArg );
			::SetValue( m_punkTarget, 0, 0, var );		

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
						::FireEvent(punkData, szNeedUpdateView, punkData, pa->m_punkArg);
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

bool CFilterBase::DoResetData()
{
	if( m_bLockChangeData )
		return true;

	POSITION	pos = m_arguments.GetHeadPosition();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.GetNext( pos );

		//paul 10.04.2002. Reason: when filter terminate & filter not create new object-> delete in arg
		/*
		if( !pa->m_punkArg )
			continue;

		if( pa->m_bVirtual )
		{
			IUnknown *punk = ::GetObjectByKey( m_punkTarget, pa->m_lParentKey );
			sptrINamedDataObject2	sptr( pa->m_punkArg );
			sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );

			_variant_t	var( pa->m_punkArg );
			::SetValue( m_punkTarget, 0, 0, var );

			if( punk )
				punk->Release();
		}
		*/

		if( !pa->m_punkArg )
			continue;

		if( !pa->m_bInArgument )
			continue;

		if( !pa->m_bNameDataChanged )
			continue;

		if( pa->m_bVirtual )
		{
			IUnknown *punk = ::GetObjectByKey( m_punkTarget, pa->m_lParentKey );
			sptrINamedDataObject2	sptr( pa->m_punkArg );
			sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );

			if( punk )
				punk->Release();
		}

		_variant_t	var( pa->m_punkArg );
		::SetValue( m_punkTarget, 0, 0, var );

/* Maxim - activate after ESC during long operation */
		INamedDataPtr	ptrData = m_punkTarget;

		IUnknown	*punkContext = 0;
		if( ptrData )
			ptrData->GetActiveContext(&punkContext);

		IDataContextPtr	ptrContext = punkContext;
		if( ptrContext )
			ptrContext->SetActiveObject( 0, pa->m_punkArg, 0 );
		if( punkContext )
			punkContext->Release();
/**/

	}

	return true;
}


void CFilterBase::DoFinalizePreview()
{
}


void CFilterBase::DoInitializePreview()
{
}