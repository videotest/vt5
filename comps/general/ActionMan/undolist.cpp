#include "stdafx.h"
#include "undolist.h"
/*
EditLine
EditLine


ActionState.BeginGroupUndo "two lines"
EditLine
EditLine
ActionState.EndGroupUndo 1
*/

IMPLEMENT_DYNCREATE(CUndoList, CCmdTargetEx);

// {A909D947-FE6D-407e-8A8E-11AB47D0974A}
GUARD_IMPLEMENT_OLECREATE(CUndoList, "ActionMan.UndoList", 
0xa909d947, 0xfe6d, 0x407e, 0x8a, 0x8e, 0x11, 0xab, 0x47, 0xd0, 0x97, 0x4a);


BEGIN_INTERFACE_MAP(CUndoList, CCmdTargetEx)
	INTERFACE_PART(CUndoList, IID_IUndoList, UndoList)
	INTERFACE_PART(CUndoList, IID_IUndoList2, UndoList)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CUndoList, UndoList)

CUndoList::CUndoList()
{
	EnableAggregation();

	IUnknown	*punk = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	m_ptrActionManager = punk;
	punk->Release();

	_OleLockApp( this );

	m_pgroup		= 0;
	m_benable_undo	= true;
}

CUndoList::~CUndoList()
{
	ClearUndoList();
	ClearRedoList();

	_OleUnlockApp( this );
}

HRESULT CUndoList::XUndoList::DoUndo()
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	if( pThis->m_listUndoActions.count() == 0 )
		return S_FALSE;

	IUnknown	*punkCurrentActionInfo = 0;
	pThis->m_ptrActionManager->GetRunningInteractiveActionInfo( &punkCurrentActionInfo );
	IActionInfoPtr	ptrAIOld( punkCurrentActionInfo );
	if( punkCurrentActionInfo )
	{
		punkCurrentActionInfo->Release();
		pThis->m_ptrActionManager->TerminateInteractiveAction();
	}
	
	TPOS	lpos = pThis->m_listUndoActions.head();
	undo_info	*pinfo = pThis->m_listUndoActions.get( lpos );

	for (TPOS lpos_a = pinfo->actions.tail(); lpos_a; lpos_a = pinfo->actions.prev(lpos_a))
	{
		IUndoneableActionPtr	ptrUA = pinfo->actions.get( lpos_a );
		IActionPtr		ptrA = ptrUA;
		ptrUA->Undo();
	}
	
	if(ptrAIOld != 0)
	{
		BSTR	bstrActionName;
		ptrAIOld->GetCommandInfo( &bstrActionName,	0, 0, 0);
		pThis->m_ptrActionManager->ExecuteAction( bstrActionName, 0, 0 );
		::SysFreeString(bstrActionName);
	}

	pThis->m_listUndoActions.remove( lpos );
	pThis->m_listRedoActions.insert_after( pinfo, 0 );

	return S_OK;
}

HRESULT CUndoList::XUndoList::DoRedo()
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	if( pThis->m_listRedoActions.count() == 0 )
		return S_FALSE;

	IUnknown	*punkCurrentActionInfo = 0;
	pThis->m_ptrActionManager->GetRunningInteractiveActionInfo( &punkCurrentActionInfo );
	IActionInfoPtr	ptrAIOld( punkCurrentActionInfo );
	if( punkCurrentActionInfo )
	{
		punkCurrentActionInfo->Release();
		pThis->m_ptrActionManager->TerminateInteractiveAction();
	}
	
	TPOS	lpos = pThis->m_listRedoActions.head();
	undo_info	*pinfo = pThis->m_listRedoActions.get( lpos );


	for (TPOS lpos_a = pinfo->actions.head(); lpos_a; lpos_a = pinfo->actions.next(lpos_a))
	{
		IUndoneableActionPtr	ptrUA = pinfo->actions.get( lpos_a );
		IActionPtr		ptrA = ptrUA;
		ptrUA->Redo();
	}

	if(ptrAIOld != 0)
	{
		BSTR	bstrActionName;
		ptrAIOld->GetCommandInfo( &bstrActionName,	0, 0, 0);
		pThis->m_ptrActionManager->ExecuteAction( bstrActionName, 0, 0 );
		::SysFreeString(bstrActionName);
	}

	pThis->m_listRedoActions.remove( lpos );
	pThis->m_listUndoActions.insert_after( pinfo, 0 );

	return S_OK;
}

HRESULT CUndoList::XUndoList::GetUndoStepsCount( int *pnStepsCount )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	*pnStepsCount = pThis->m_listUndoActions.count();

	return S_OK;
}

HRESULT CUndoList::XUndoList::GetRedoStepsCount( int *pnStepsCount )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	*pnStepsCount = pThis->m_listRedoActions.count();

	return S_OK;
}

HRESULT CUndoList::XUndoList::AddAction( IUnknown *punkAction )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	pThis->ClearRedoList();

	if( !pThis->m_benable_undo )
		return S_OK;

	undo_info	*pinfo = pThis->m_pgroup;
	if( !pinfo )
	{
		pinfo = new undo_info;
		pinfo->caption = "internal";

		pThis->m_listUndoActions.insert_after( pinfo, 0 );
	}
	pinfo->actions.insert_before( punkAction, 0 );
	punkAction->AddRef();

	if( pThis->m_pgroup )return S_OK;

	int nUndoListMaxSize = GetValueInt( ::GetAppUnknown(), "ActionManager", "UndoListMaxSize", 5 );

	pThis->CheckUndoCount(); // the subroutine has an effect same as the previous code
	/*// vanek - 10.10.2003
	//if( pThis->m_listUndoActions.count() > nUndoListMaxSize )
	while( pThis->m_listUndoActions.count() > nUndoListMaxSize )
	{
		LONG_PTR	lpos_last = pThis->m_listUndoActions.tail();
		undo_info	*plast = pThis->m_listUndoActions.get( lpos_last );
		pThis->m_listUndoActions.remove( lpos_last );

		free_undo_info( plast );
	}*/

	return S_OK;
}

HRESULT CUndoList::XUndoList::RemoveAction( IUnknown *punkAction )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	TPOS	lpos, lpos_next, lpos_a, lpos_a_next;

	for( lpos = pThis->m_listUndoActions.head(); lpos; lpos = lpos_next )
	{
		lpos_next = pThis->m_listUndoActions.next( lpos );
		undo_info *pinfo = pThis->m_listUndoActions.get( lpos );

		for( lpos_a = pinfo->actions.head(); lpos_a; lpos_a = lpos_a_next )
		{
			lpos_a_next = pinfo->actions.next( lpos_a );
			IUnknown *punk = pinfo->actions.get( lpos_a );

			if( punk == punkAction )
			{
				pinfo->actions.remove( lpos_a );
				punk->Release();
			}
		}
		if( pinfo->actions.count() == 0 )
		{
			pThis->m_listUndoActions.remove( lpos );
			free_undo_info( pinfo );
		}
	}
	for( lpos = pThis->m_listRedoActions.head(); lpos; lpos = lpos_next )
	{
		lpos_next = pThis->m_listRedoActions.next( lpos );
		undo_info *pinfo = pThis->m_listRedoActions.get( lpos );

		for( lpos_a = pinfo->actions.head(); lpos_a; lpos_a = lpos_a_next )
		{
			lpos_a_next = pinfo->actions.next( lpos_a );
			IUnknown *punk = pinfo->actions.get( lpos_a );

			if( punk == punkAction )
			{
				pinfo->actions.remove( lpos_a );
				punk->Release();
			}
		}
		if( pinfo->actions.count() == 0 )
		{
			pThis->m_listRedoActions.remove( lpos );
			free_undo_info( pinfo );
		}
	}
	return S_OK;
}

HRESULT CUndoList::XUndoList::GetLastUndoAction( IUnknown **ppunkAction )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	*ppunkAction = 0;

	undo_info	*pinfo = 0;
	TPOS	lpos = pThis->m_listUndoActions.head();
	if( lpos )pinfo = pThis->m_listUndoActions.get( lpos );

	if( !pinfo )
		return S_OK;

	lpos = pinfo->actions.tail();
	if( lpos )
	{
		*ppunkAction = pinfo->actions.get( lpos );
		(*ppunkAction)->AddRef();
	}

	return S_OK;
}

HRESULT CUndoList::XUndoList::DeleteLastUndoAction( )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
	
	IUnknown *punkAction=0;
	GetLastUndoAction( &punkAction );
	if(punkAction)
	{
		RemoveAction( punkAction );
		punkAction->Release();
	}
	
	return S_OK;
}

HRESULT CUndoList::XUndoList::ClearUndoRedoList( )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
 
	pThis->ClearUndoList();
	pThis->ClearRedoList();

	return S_OK;
}

HRESULT CUndoList::XUndoList::ClearRedoList( )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
 
	pThis->ClearRedoList();

	return S_OK;
}


HRESULT CUndoList::XUndoList::BeginGroup( BSTR bstr )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	if( pThis->m_pgroup )return S_OK;
	
	pThis->m_pgroup = new undo_info;
	pThis->m_pgroup->caption = bstr;

	return S_OK;
}

HRESULT CUndoList::XUndoList::EndGroup( long code )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
	
	if( !pThis->m_pgroup )return S_OK;

	if( code == GROUP_CODE_OK && pThis->m_benable_undo )
		pThis->m_listUndoActions.insert_after( pThis->m_pgroup, 0 );
	else
		free_undo_info( pThis->m_pgroup );

	pThis->m_pgroup = 0;

	pThis->CheckUndoCount(); // AM BT 4957

	return S_OK;
}

HRESULT CUndoList::XUndoList::EndGroup2( BSTR bstr, long code )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
	
	if( !pThis->m_pgroup )return S_OK;

	if( pThis->m_pgroup->caption != _bstr_t(bstr) )return S_OK;

	if( code == GROUP_CODE_OK && pThis->m_benable_undo )
		pThis->m_listUndoActions.insert_after( pThis->m_pgroup, 0 );
	else
		free_undo_info( pThis->m_pgroup );

	pThis->m_pgroup = 0;

	pThis->CheckUndoCount(); // AM BT 4957

	return S_OK;
}

HRESULT CUndoList::XUndoList::GetEnableUndo( BOOL* pbenable )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
	if( !pbenable )
		return E_POINTER;

	*pbenable = pThis->m_benable_undo;
	
	return S_OK;
}

HRESULT CUndoList::XUndoList::SetEnableUndo( BOOL benable )
{
	METHOD_PROLOGUE_EX(CUndoList, UndoList);
	pThis->m_benable_undo = benable;
	
	return S_OK;
}

void CUndoList::CheckUndoCount()
{
	int nUndoListMaxSize = GetValueInt( ::GetAppUnknown(), "ActionManager", "UndoListMaxSize", 5 );
	// vanek - 10.10.2003
	//if( pThis->m_listUndoActions.count() > nUndoListMaxSize )
	while( m_listUndoActions.count() > nUndoListMaxSize )
	{
		TPOS	lpos_last = m_listUndoActions.tail();
		undo_info	*plast = m_listUndoActions.get( lpos_last );
		m_listUndoActions.remove( lpos_last );

		free_undo_info( plast );
	}
}


void CUndoList::Check()
{
	for (TPOS lpos = m_listUndoActions.head(); lpos; lpos = m_listUndoActions.next(lpos))
	{
		undo_info *p = m_listUndoActions.get( lpos );
		for (TPOS lpos = p->actions.head(); lpos; lpos = p->actions.next(lpos))
		{
			IUnknown *punk = p->actions.get( lpos );
			IFilterActionPtr sptrFA(punk);
			if (sptrFA == 0) continue;
			{
				TPOS lpos = 0;
				sptrFA->GetFirstArgumentPosition(&lpos);
				while (lpos)
				{
					IUnknownPtr punkArg;
					sptrFA->GetNextArgument(&punkArg,&lpos);
					LONG l = punk->AddRef();
					if (l <= 1) DebugBreak();
					punk->Release();
				}
			}
		}
	}
}

void CUndoList::ClearUndoList()		//clear the undo list
{
	for (TPOS lpos = m_listUndoActions.head(); lpos; lpos = m_listUndoActions.next(lpos))
		free_undo_info( m_listUndoActions.get( lpos ) );
	m_listUndoActions.clear();

	free_undo_info( m_pgroup );
	m_pgroup = 0;
}

void CUndoList::ClearRedoList()		//clear the redo list
{
	for (TPOS lpos = m_listRedoActions.head(); lpos; lpos = m_listRedoActions.next(lpos))
		free_undo_info( m_listRedoActions.get( lpos ) );
	m_listRedoActions.clear();

//	free_undo_info( m_pgroup );
//	m_pgroup = 0;
}

HRESULT CUndoList::XUndoList::BeginGroup2( BSTR bstr, long lStepsBack )
{	// начать группу, включив в нее акции из lStepsBack последних групп
	METHOD_PROLOGUE_EX(CUndoList, UndoList);

	if( pThis->m_pgroup )return S_OK;
	
	pThis->m_pgroup = new undo_info;
	pThis->m_pgroup->caption = bstr;

	for( int i=0; i<lStepsBack; i++ )
	{
		TPOS	lpos = pThis->m_listUndoActions.head();
		if(!lpos) break;
		undo_info	*pinfo = pThis->m_listUndoActions.get( lpos );
		if(!pinfo) break;
		
		TPOS lActionPos = pinfo->actions.tail();
		while(lActionPos)
		{
			IUnknown* punkAction = pinfo->actions.get(lActionPos);
			if(punkAction)
			{
				pThis->m_pgroup->actions.add_head( punkAction );
				punkAction->AddRef();
			}
			lActionPos = pinfo->actions.prev(lActionPos);
		}

		pThis->m_listUndoActions.remove( lpos );
		free_undo_info( pinfo );
	}

	return S_OK;
}

HRESULT CUndoList::XUndoList::GetUndoAction( int nUndoStepPos, int nActionPos, IUnknown **ppunkAction )
{	// получить указатель на акцию в undo-листе
	// nUndoStepPos - номер шага Undo, 0 - последний
	// nActionPos - номер акции в шаге, 0 - последн€€
	return E_NOTIMPL;
}

HRESULT CUndoList::XUndoList::GetUndoActionName( int nUndoStepPos, int nActionPos, BSTR *pbstrActionName)
{	// получить им€ акции в undo-листе
	return E_NOTIMPL;
}
