#include "stdafx.h"
#include "classifyactionsbase.h"
#include "object5.h"
#include "data5.h"

CClassifyActionsBase::CClassifyActionsBase(void)
{
	m_lClass = -1;
	m_manual_flag = false;
	m_undo_lClass = -1;
	m_undo_manual_flag = false;
}

CClassifyActionsBase::~CClassifyActionsBase(void)
{
}

// notify througth parent about object is changed 
void CClassifyActionsBase::NotifyParent()
{
	INamedDataObject2Ptr ptrNDO = m_ptrObject;
	if (ptrNDO == 0) return;

	// notify througth parent abouot object is changed 
	IUnknown *punkParent = 0;
	if( FAILED( ptrNDO->GetParent( &punkParent ) ) || !punkParent )
		return;

	IDataObjectListPtr ptrParent = punkParent;
	punkParent->Release();

	if (ptrParent) 
		ptrParent->UpdateObject( ptrNDO );
}

void CClassifyActionsBase::FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

HRESULT CClassifyActionsBase::DoUndo()
{
	set_object_class( m_ptrObject, m_undo_lClass, m_bstrClassFile);
	NotifyParent();	//просигналим, чтоб обновить вьюху
	return S_OK;
}

HRESULT CClassifyActionsBase::DoRedo()
{
	set_object_class( m_ptrObject, m_lClass, m_bstrClassFile);
	NotifyParent();	//просигналим, чтоб обновить вьюху
	return S_OK;
}
