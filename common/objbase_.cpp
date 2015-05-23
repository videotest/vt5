#include "stdafx.h"
#include "objbase_.h"
#include "utils.h"
#include "misc5.h"
#include "nameconsts.h"


/////////////////////////////////////////////////////////////////////////////////////////
//IDrawObject implemetation
CDrawObjectImpl::CDrawObjectImpl()
{
	m_punkDrawTarget = 0;
	m_pbi = 0;
	m_pdib = 0;
	m_pOptions = NULL;
	m_rcInvalid = NORECT;
}

CDrawObjectImpl::~CDrawObjectImpl()
{
}

//iunterface
IMPLEMENT_UNKNOWN_BASE(CDrawObjectImpl, Draw)

HRESULT CDrawObjectImpl::XDraw::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	METHOD_PROLOGUE_BASE(CDrawObjectImpl, Draw)

	CDC	*pDC = CDC::FromHandle( hDC );

	pThis->m_rcInvalid = rectDraw;

	pThis->m_punkDrawTarget = punkTarget;
	pThis->m_pbi = pbiCache;
	pThis->m_pdib = pdibCache;

	pThis->_Draw( pDC );

	pThis->m_pbi = 0;
	pThis->m_pdib = 0;
	
	return S_OK;
}

HRESULT CDrawObjectImpl::XDraw::PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions)
{
	METHOD_PROLOGUE_BASE(CDrawObjectImpl, Draw)

	CDC	*pDC = CDC::FromHandle( hDC );

	pThis->m_rcInvalid = rectDraw;

	pThis->m_punkDrawTarget = punkTarget;
	pThis->m_pbi = pbiCache;
	pThis->m_pdib = pdibCache;
	pThis->m_pOptions = pOptions;

	pThis->_Draw( pDC );

	pThis->m_pbi = 0;
	pThis->m_pdib = 0;
	pThis->m_pOptions = 0;
	
	return S_OK;
}


HRESULT CDrawObjectImpl::XDraw::GetRect( RECT *prect )
{
	_try_nested_base(CDrawObjectImpl, Draw, GetRect)
	{
		CRect	rect = pThis->DoGetRect();

		memcpy( prect, &rect, sizeof( rect ) );

		return S_OK;

	}
	_catch_nested;
}

HRESULT CDrawObjectImpl::XDraw::SetTargetWindow( IUnknown *punkTarget )
{
	_try_nested_base(CDrawObjectImpl, Draw, GetRect)
	{
		pThis->m_punkDrawTarget = punkTarget;
		return S_OK;

	}
	_catch_nested;
}

/*HRESULT CDrawObjectImpl::XDraw::SetInvalidRect(RECT rect)
{
	_try_nested_base(CDrawObjectImpl, Draw, SetInvalidRect)
	{
		pThis->m_rcInvalid = rect;
		return S_OK;
	}
	_catch_nested;
}*/

void CDrawObjectImpl::DoDraw( CDC &dc )
{
}

CRect CDrawObjectImpl::DoGetRect()
{
	return GetTargetRect();
}

CRect CDrawObjectImpl::GetTargetRect()
{
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkDrawTarget );
	if( !pwnd )
		return NORECT;

	CRect	rect;
	pwnd->GetClientRect( &rect );

	return rect;
}

void CDrawObjectImpl::_Draw( CDC *pdc )
{
	DoDraw( *pdc );
}

/////////////////////////////////////////////////////////////////////////////////////////
//helper for undo/redo for each object
CObjectUndoRecord::CObjectUndoRecord( IUnknown *punkObject, bool bObjectAdded, bool bMapDataToParent )
{
	SetObject( punkObject );
	m_bObjectIsAdded = bObjectAdded;
	m_bMap = bMapDataToParent;
}

void CObjectUndoRecord::SetObject( IUnknown *punkObject )
{
	m_object = punkObject;
	if( !punkObject )return;
	IUnknown	*punkParent = 0;
	m_object->GetParent( &punkParent );
	m_lParentKey = ::GetObjectKey( punkParent );
	if( punkParent )
		punkParent->Release();

	DWORD	dwObjectFlags = 0;
	m_object->GetObjectFlags( &dwObjectFlags );

	m_bObjectIsVirtual = (dwObjectFlags & nofHasData) != nofHasData;
	m_bChangeDocData = (dwObjectFlags & nofStoreByParent ) != nofStoreByParent;
}


void CObjectUndoRecord::DoUndo( IUnknown *punkDocData )
{
	if( m_bObjectIsAdded )
		RemoveObject( punkDocData );
	else
		AddObject( punkDocData );
}

void CObjectUndoRecord::DoRedo( IUnknown *punkDocData, bool  bCleanVirtualsBack )
{
	if( !m_bObjectIsAdded )
		RemoveObject( punkDocData, bCleanVirtualsBack );
	else
		AddObject( punkDocData );
}

void CObjectUndoRecord::AddObject( IUnknown *punkDocData )
{
	IUnknown	*punkParent = ::GetObjectByKeyEx( punkDocData, m_lParentKey );

//	CString sName = ::GetObjectName(m_object);

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
	{
		::SetValue( punkDocData, 0, 0, _variant_t( (IUnknown*)m_object ) );

		INamedDataPtr	ptrData( punkDocData );
		IUnknown	*punkContext = 0;
		if( ptrData )ptrData->GetActiveContext(&punkContext);
		IDataContextPtr	ptrContext( punkContext );
		// A.M. fix BT
		if( ptrContext )ptrContext->SetActiveObject( 0, (IUnknown*)m_object, aofActivateDepended );
//		if( ptrContext )ptrContext->SetActiveObject( 0, (IUnknown*)m_object, 0 );
		if( punkContext )punkContext->Release();
	}
}

void CObjectUndoRecord::RemoveObject( IUnknown *punkDocData, bool  bCleanVirtualsBack)
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
CObjectListUndoRecord::CObjectListUndoRecord()
{
	m_bCleanVirtualsBack = false;
}

CObjectListUndoRecord::~CObjectListUndoRecord()
{
	DeInit();
}

void CObjectListUndoRecord::DeInit()
{
	POSITION	pos = GetHeadPosition();

	while( pos ) 
		delete GetNext( pos );

	RemoveAll();
}

//add the object to the undo/redo list
CObjectUndoRecord *CObjectListUndoRecord::AddObject( IUnknown *punk, bool bObjectAdded, bool bMapData )
{
	CObjectUndoRecord	*pour = new CObjectUndoRecord( punk, bObjectAdded, bMapData );
	AddTail( pour );
	return pour;
}

void CObjectListUndoRecord::DoUndo( IUnknown *punkDocData )
{
	POSITION	pos = GetTailPosition();

	while( pos )
		GetPrev( pos )->DoUndo( punkDocData );
}

void CObjectListUndoRecord::DoRedo( IUnknown *punkDocData )
{
	POSITION	pos = GetHeadPosition();

	while( pos )
		GetNext( pos )->DoRedo( punkDocData, m_bCleanVirtualsBack );
}

//performs adding object to the named data
void CObjectListUndoRecord::SetToDocData( IUnknown *punkDocData, IUnknown *punkObject, DWORD dwFlags )
{
	if(punkObject==0)
		return; // 29.09.2005 build 91 - аварийное

	bool	bStoredInNamedData = (GetObjectFlags( punkObject )& nofStoreByParent ) != nofStoreByParent;

	// vanek BT:618 04.11.2003
	BOOL	bActivateObject = (dwFlags & UF_NOT_ACTIVATE_OBJECT) != UF_NOT_ACTIVATE_OBJECT;
	BOOL	bActivateDependent = (dwFlags & UF_ACTIVATE_DEPENDENT) == UF_ACTIVATE_DEPENDENT;


	//<<--Paul 21.12.2000		
	{
		INamedDataObjectPtr ptrNDO( punkObject );
		if( ptrNDO )
			ptrNDO->SetModifiedFlag( TRUE );
	}
	//>>--Paul 21.12.2000


	if( bStoredInNamedData )
	{	
		INamedDataPtr	ptrData = punkDocData;

		if( !( dwFlags & UF_NOT_GENERATE_UNIQ_NAME ) )
			::GenerateUniqueNameForObject( punkObject, punkDocData );

		IUnknown	*punkContext = 0;
		if( ptrData )
			ptrData->GetActiveContext(&punkContext);

		// vanek BT:618
		IDataContextMiscPtr sptrContextMisc( punkContext );
		BOOL bOldLockActivate = FALSE;
		if( sptrContextMisc != 0 )
			sptrContextMisc->GetLockObjectActivate( &bOldLockActivate );

		if( !bOldLockActivate && !bActivateObject )
			sptrContextMisc->SetLockObjectActivate( TRUE );

		::SetValue( punkDocData, 0, 0, _variant_t( punkObject ) );

		if( !bOldLockActivate && !bActivateObject )
			sptrContextMisc->SetLockObjectActivate( bOldLockActivate );
		
		if( bActivateObject )
		{
			IDataContextPtr	ptrContext( punkContext );
			if( ptrContext )
				ptrContext->SetActiveObject( 0, punkObject, bActivateDependent?aofActivateDepended:0 );
		}
		if( punkContext )
			punkContext->Release(); punkContext = 0;
	}
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
			ASSERT(punkType);

			sptrINamedDataInfo	sptrTI( punkType );
			punkType->Release();

			sptrTI->GetContainerType( &bstrParentType );
			CString	strParentType = bstrParentType;
			::SysFreeString( bstrParentType );

			//get the active parent object
			punkParent = ::GetActiveObjectFromDocument( punkDocData, strParentType );

			//no valid parent objects exist - create a new one
			bool	bParentToData = false;
			if( !punkParent )
			{
				punkParent = ::CreateTypedObject( strParentType );
				bParentToData = true;
			}

			//attach the parent to the object
			sptrN->SetParent( punkParent, 0 );

			if( bParentToData )
				SetToDocData( punkDocData, punkParent, bActivateObject ? 0 : UF_NOT_ACTIVATE_OBJECT ); 
		}

		//<<--Paul 21.12.2000		
		{
			INamedDataObjectPtr ptrNDO( punkParent );
			if( ptrNDO )
				ptrNDO->SetModifiedFlag( TRUE );
		}
		//>>--Paul 21.12.2000



		punkParent->Release();


		
	}
	CObjectUndoRecord	*pr = AddObject( punkObject, true, false );
}

//preform removing object from named data
static bool _HasImage(IUnknown *punkObj)
{
	IMeasureObjectPtr sptrMO(punkObj);
	if (sptrMO != 0)
	{
		IUnknownPtr punkImage;
		sptrMO->GetImage(&punkImage);
		if (punkImage != 0)
			return true;
	}
	return false;
}

void CObjectListUndoRecord::RemoveFromDocData( IUnknown *punkDocData, IUnknown *punkObject, bool bMapData, bool bCleanVirtualsBack )
{
	if( !punkObject )
		return;

	//<<--Paul 21.12.2000		
	{
		INamedDataObjectPtr ptrNDO( punkObject );
		if( ptrNDO )
			ptrNDO->SetModifiedFlag( TRUE );
	}

	//check the exist parent
	IUnknownPtr	ptrParent = 0;
	INamedDataObject2Ptr	ptrN( punkObject );
	ptrN->GetParent( &ptrParent );
	if( ptrParent )
	{
		INamedDataObjectPtr ptrNDO( ptrParent );
		if( ptrNDO )
			ptrNDO->SetModifiedFlag( TRUE );
	}
	//>>--Paul 21.12.2000

	CObjectUndoRecord	*pr = AddObject( punkObject, false, bMapData );


	DWORD	dwFlags =0;
	ptrN->GetObjectFlags( &dwFlags );


	m_bCleanVirtualsBack = bCleanVirtualsBack;

	if( pr->IsVirtual() )
		ptrN->SetParent( 0, sdfCopyParentData|(bCleanVirtualsBack?sdfClean:0) );
	else
	{
		if(!bMapData)
			ptrN->SetParent( 0, 0 );
		else
			ptrN->SetParent( 0, sdfCopyLocalData|sdfCopyParentData );
	}

	if( pr->IsDataChange() )
	{
		bool	bDelete = ::DeleteObject( punkDocData, punkObject );
		if( !bDelete )
		{
			punkObject = ::FindSourceObject( punkDocData, punkObject );
			if( punkObject )
			{
				if( ::DeleteObject( punkDocData, punkObject ) )
					pr->SetObject( punkObject );
				else
					pr->SetObject( 0 );

				punkObject->Release();
			}
		}
	}

	if( dwFlags & nofDefineParent )
	{
		// A.M. fix : SBT 252. Do not delete measure object during deletion of manual object
		// if measure object have another children.
		INamedDataObject2Ptr	ptrN( ptrParent );
		long lChildren = 0;
		ptrN->GetChildsCount(&lChildren);
		if (lChildren == 0 && !_HasImage(ptrParent)) // A.M. fix SBT 1762-1766
			RemoveFromDocData( punkDocData, ptrParent, bMapData, bCleanVirtualsBack );
	}
}


//
CModifiedUndoRecord::CModifiedUndoRecord()
{
	m_bOldState = true;
	m_bFirstCall = true;
}

void CModifiedUndoRecord::SetModified( IUnknown *punk, bool bSet )
{
	if( CheckInterface( punk, IID_IFileDataObject2 ) )
	{
		IFileDataObject2Ptr	ptrFileObject( punk );

		if( m_bFirstCall )
		{
			m_bFirstCall = false;
			BOOL	bDirty = false;
			ptrFileObject->IsDirty( &bDirty );
			m_bOldState = bDirty != 0;
		}
		ptrFileObject->SetModifiedFlag( bSet );
	}
}

void CModifiedUndoRecord::ResetModified( IUnknown *punk )
{
	if( !m_bFirstCall )
		SetModified( punk, m_bOldState );
}
