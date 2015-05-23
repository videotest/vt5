#include "stdafx.h"
#include "action_undo.h"
#include "misc_utils.h"
#include "com_main.h"
#include "core5.h"
#include "data5.h"
#include "misc5.h"


//IUndoneableAction

CUndoneableActionImpl::CUndoneableActionImpl()
{
	m_bOldState = false;
	m_bFirstCall = true;
}

HRESULT CUndoneableActionImpl::StoreState()
{
	IUnknown	*punkTarget = _GetTarget();
	m_guidTarget = ::GetKey( punkTarget );
	punkTarget->Release();
	return S_OK;
}

HRESULT CUndoneableActionImpl::Undo()
{
	PrepareTarget( true );
	HRESULT hr = DoUndo();
	SetModified( false ); // 15.11.2004 build 72
	PrepareTarget( false );
	return hr;
}

HRESULT CUndoneableActionImpl::Redo()
{
	PrepareTarget( true );
	HRESULT hr = DoRedo();
	SetModified( true ); // 15.11.2004 build 72
	PrepareTarget( false );
	return hr;
}

HRESULT CUndoneableActionImpl::GetTargetKey( GUID *plKey )
{
	*plKey = m_guidTarget;
	return S_OK;
}

bool CUndoneableActionImpl::PrepareTarget( bool bSet )
{
	//get the target manager
	IUnknown	*punk = 0;

	if( bSet )
	{
		IUnknown *punkTargetMan = 0;
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		ptrApp->GetTargetManager( &punkTargetMan );

		if( !punkTargetMan )
			return false;

		IActionTargetManagerPtr	ptrTargetMan( punkTargetMan );
		punkTargetMan->Release();


		
		ptrTargetMan->GetTargetByKey( m_guidTarget, &punk );

		if( !punk )
			return false;
	}

	IActionPtr	ptrA( this );
	ptrA->AttachTarget( punk );
	if( punk )punk->Release();

	return true;

}

HRESULT CUndoneableActionImpl::DoUndo()
{
	IActionPtr	ptrA( this );
	IUnknown	*punkTarget;
	ptrA->GetTarget( &punkTarget );

	CDocHelper::DoUndo( punkTarget );

	if( punkTarget )punkTarget->Release();

	return S_OK;
}

HRESULT CUndoneableActionImpl::DoRedo()
{
	IActionPtr	ptrA( this );
	IUnknown	*punkTarget;
	ptrA->GetTarget( &punkTarget );

	CDocHelper::DoRedo( punkTarget );

	if( punkTarget )punkTarget->Release();

	return S_OK;
}



void CUndoneableActionImpl::SetModified( bool bSet )
{
	IUnknown *punkTargetMan = 0;

	{
		IApplicationPtr ptrApp( App::application() );
		ptrApp->GetTargetManager( &punkTargetMan );
	}

	if( !punkTargetMan )
		return;

	IActionTargetManagerPtr	sptrM( punkTargetMan );
	punkTargetMan->Release();

	IUnknown	*punkTarget = 0;
	sptrM->GetTargetByKey( m_guidTarget, &punkTarget );

	if( !punkTarget )
		return;

	//SetModifiedFlag( punkTarget, true );
	bool	bNewFlag = bSet?true:m_bOldState;

	IFileDataObject2Ptr	ptrFileObject( punkTarget );
	if( ptrFileObject != 0 )
	{
		if( m_bFirstCall )
		{
			m_bFirstCall = false;
			BOOL	bDirty = false;
			ptrFileObject->IsDirty( &bDirty );
			m_bOldState = bDirty != 0;
		}
		ptrFileObject->SetModifiedFlag( bNewFlag );
	}
	punkTarget->Release();
}

void CUndoneableActionImpl::OnAttachTarget( IUnknown *punkTarget )
{
	if( punkTarget )
		m_guidTarget = ::GetKey( punkTarget );
}

IUnknown *CUndoneableActionImpl::_GetTarget()
{
	IUnknown	*punkTarget = 0;
	IAction	*punkA = 0;
	QueryInterface( IID_IAction, (void**)&punkA );
	punkA->GetTarget( &punkTarget );
	punkA->Release();

	return punkTarget;
}


/////////////////////////////////////////////////////////////////////////////////////////
//helper for undo/redo for each object
ObjectData::ObjectData( IUnknown *punkObject, bool bObjectAdded, bool bMapDataToParent )
{
	m_object = punkObject;
	m_bObjectIsAdded = bObjectAdded;
	m_bMap = bMapDataToParent;

	IUnknown	*punkParent = 0;
	m_object->GetParent( &punkParent );
	m_lParentKey = ::GetKey( punkParent );
	if( punkParent )
		punkParent->Release();

	DWORD	dwObjectFlags = 0;
	m_object->GetObjectFlags( &dwObjectFlags );

	m_bObjectIsVirtual = (dwObjectFlags & nofHasData) != nofHasData;
	m_bChangeDocData = (dwObjectFlags & nofStoreByParent ) != nofStoreByParent;
}

void ObjectData::DoUndo( IUnknown *punkDocData )
{
	if( m_bObjectIsAdded )
		RemoveObject( punkDocData );
	else
		AddObject( punkDocData );
}

void ObjectData::DoRedo( IUnknown *punkDocData, bool  bCleanVirtualsBack )
{
	if( !m_bObjectIsAdded )
		RemoveObject( punkDocData, bCleanVirtualsBack );
	else
		AddObject( punkDocData );
}

void ObjectData::AddObject( IUnknown *punkDocData )
{
	IUnknown	*punkParent = ::GetObjectByKeyEx( punkDocData, m_lParentKey );

	if( m_bObjectIsVirtual )
		m_object->SetParent( punkParent, sdfCopyLocalData|sdfAttachParentData );
	else
	{
		if( m_bMap )
			m_object->SetParent( punkParent, sdfCopyLocalData|sdfCopyParentData );
		else
			m_object->SetParent( punkParent, 0 );
	}

	if( punkParent )
		punkParent->Release();

	if( m_bChangeDocData )
		::SetValue( punkDocData, 0, 0, _variant_t( (IUnknown*)m_object ) );
}

void ObjectData::RemoveObject( IUnknown *punkDocData, bool  bCleanVirtualsBack)
{
	IUnknown* punkOldData = 0;
	IUnknown* punkParent = 0;
	m_object->GetParent( &punkParent );
	sptrINamedDataObject2 sptrND(punkParent);
	if(punkParent)
		punkParent->Release();
	if(sptrND !=0)
	{
		sptrND->GetData( &punkOldData );
		sptrND->AttachData(punkDocData);
	}
	if( m_bObjectIsVirtual )
		m_object->SetParent( 0, sdfCopyParentData|(bCleanVirtualsBack?sdfClean:0) );
	else
	{
		if( m_bMap )
			m_object->SetParent( 0, sdfCopyLocalData|sdfCopyParentData );
		else
			m_object->SetParent( 0, 0 );
	}

	if( m_bChangeDocData )
		::DeleteObject( punkDocData, m_object );

	if(sptrND !=0)
	{
		sptrND->AttachData(punkOldData);
	}
	if(punkOldData)
		punkOldData->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////
//helper for undo/redo for object list
CDocHelper::CDocHelper()
{
	m_bCleanVirtualsBack = false;
}

CDocHelper::~CDocHelper()
{
	DeInit();
}

void CDocHelper::DeInit()
{
	TPOS	lpos = head();
	while( lpos )delete next( lpos );

	deinit();
}

//add the object to the undo/redo list
ObjectData *CDocHelper::AddObject( IUnknown *punk, bool bObjectAdded, bool bMapData )
{
	ObjectData	*pour = new ObjectData( punk, bObjectAdded, bMapData );
	insert( pour );
	return pour;
}

void CDocHelper::DoUndo( IUnknown *punkDocData )
{
	TPOS lpos = head();

	while( lpos )
		next( lpos )->DoUndo( punkDocData );
}

void CDocHelper::DoRedo( IUnknown *punkDocData )
{
	TPOS lpos = head();

	while( lpos )
		next( lpos )->DoRedo( punkDocData, m_bCleanVirtualsBack );
}

//performs adding object to the named data
void CDocHelper::SetToDocData( IUnknown *punkDocData, IUnknown *punkObject )
{
	bool	bStoredInNamedData = (GetObjectFlags( punkObject )& nofStoreByParent ) != nofStoreByParent;

	//paul
	{
		INamedDataObjectPtr ptrNDO( punkObject );
		if( ptrNDO )
			ptrNDO->SetModifiedFlag( TRUE );
	}


	if( bStoredInNamedData )
		::SetValue( punkDocData, 0, 0, _variant_t( punkObject ) );
	else
	{
		//check the exist parent
		IUnknown	*punkParent = 0;
		sptrINamedDataObject2	sptrN( punkObject );
		sptrN->GetParent( &punkParent );

		//if parent exist, everything is ok
		if( !punkParent )
		{
			//parent doesn't exist - get its type
			BSTR	bstrParentType = 0;

			IUnknown	*punkType = 0;
			sptrN->GetDataInfo( &punkType );
			dbg_assert( punkType!= 0 );

			sptrINamedDataInfo	sptrTI( punkType );
			punkType->Release();

			sptrTI->GetContainerType( &bstrParentType );
			_bstr_t	strParentType = bstrParentType;
			::SysFreeString( bstrParentType );

			//get the active parent object
			punkParent = ::GetActiveObjectFromDocument( punkDocData, strParentType );

			//no valid parent objects exist - create a new one
			if( !punkParent )
			{
				punkParent = ::CreateTypedObject( strParentType );
				sptrN->SetParent( punkParent, 0 );
				SetToDocData( punkDocData, punkParent );
			}
			else
				sptrN->SetParent( punkParent, 0 );


			//attach the parent to the object
		}

		punkParent->Release();


		_bstr_t	strObjectName = ::GetName( punkObject );
		if( strObjectName.length() == 0 )
		{
			INamedObject2Ptr	sptrN( punkObject );
			_bstr_t	bstrBase = ::GetObjectType( punkObject );
			sptrN->GenerateUniqueName( bstrBase );
		}
	}
	ObjectData	*pr = AddObject( punkObject, true, false );
}

//preform removing object from named data
void CDocHelper::RemoveFromDocData( IUnknown *punkDocData, IUnknown *punkObject, bool bMapData, bool bCleanVirtualsBack )
{
	if( !punkObject )
		return;

	ObjectData	*pr = AddObject( punkObject, false, bMapData );

	sptrINamedDataObject2	sptrN( punkObject );


	m_bCleanVirtualsBack = bCleanVirtualsBack;

	if( pr->IsVirtual() )
		sptrN->SetParent( 0, sdfCopyParentData|(bCleanVirtualsBack?sdfClean:0) );
	else
	{
		if(!bMapData)
			sptrN->SetParent( 0, 0 );
		else
			sptrN->SetParent( 0, sdfCopyLocalData|sdfCopyParentData );
	}

	if( pr->IsDataChange() )
		::DeleteObject( punkDocData, punkObject );
}


