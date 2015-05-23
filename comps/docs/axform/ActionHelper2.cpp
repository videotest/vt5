#include "stdafx.h"
#include "axform.h"
#include "ActiveXSite.h"
#include "ActionHelper2.h"
#include "ActiveXSite.h"
#include "UndoInt.h"

// -------------------------

static void TerminateCurrentOperation(long lTerminateFlag)
{
	IUnknownPtr	punk(_GetOtherComponent( GetAppUnknown(), IID_IOperationManager ), false);
	IOperationManager2Ptr sptrOM(punk);
	if (sptrOM != 0)
		sptrOM->TerminateCurrentOperation(lTerminateFlag);
}

void CViewsLock::AddView(IUnknown* pView)
{
	XViewLock lock;
	lock.sptrView = pView;
	lock.bPrevLock = false;
	lock.sptrView->GetLockUpdate(&lock.bPrevLock);
	lock.sptrView->LockUpdate(TRUE, FALSE); // дальше View залочено
	m_locks.push_back(lock);
};

void CViewsLock::Clear()
{
	list<XViewLock>::iterator p;
	for(p=m_locks.begin(); p!=m_locks.end(); ++p)
	{
		if(p->sptrView)
			p->sptrView->LockUpdate(p->bPrevLock, TRUE);
	}
	m_locks.clear();
}


// -------------------------
void CViewState::Push(IDataContext3Ptr sptrDC)
{
	if(sptrDC!=0)
	{
		m_SelectionList.clear();

		long nTypeCount = 0;
		sptrDC->GetObjectTypeCount(&nTypeCount);

		// пробежимся по всем типам и сложим все Selection в один список по порядку
		for (long nType = 0; nType < nTypeCount; nType++)
		{	// for all types
			_bstr_t bstrType;
			if (FAILED(sptrDC->GetObjectTypeName(nType, bstrType.GetAddress())) || !bstrType)
				continue;

			LONG_PTR lPos = 0;
			sptrDC->GetFirstSelectedPos( bstrType, &lPos );
			while( lPos )
			{
				IUnknownPtr ptr = 0;
				sptrDC->GetNextSelected( bstrType, &lPos, &ptr );
				m_SelectionList.push_back(ptr);
			}
		}
	}
}

void CViewState::Pop(IDataContext3Ptr sptrDC)
{
	if(sptrDC!=0)
	{
		long nTypeCount = 0;
		sptrDC->GetObjectTypeCount(&nTypeCount);

		// расселектим все
		for (long nType = 0; nType < nTypeCount; nType++)
		{	// for all types
			_bstr_t bstrType;
			if (FAILED(sptrDC->GetObjectTypeName(nType, bstrType.GetAddress())) || !bstrType)
				continue;
			sptrDC->UnselectAll(bstrType);
		}

		vector<IUnknownPtr>::iterator p;
		for( p=m_SelectionList.begin(); p!=m_SelectionList.end(); ++p)
		{
				sptrDC->SetObjectSelect(*p,true);
		}
	}
}

// -------------------------
void CDocumentState::CreateEmpty()
{
	m_sptrND.CreateInstance( szNamedDataProgID );
}

void CDocumentState::GetFrom(INamedDataPtr sptrND, IDataContext3Ptr sptrDC)
{
	if( m_sptrND!=0 && sptrND!=0 )
		CopyNamedData( m_sptrND, sptrND );

	IUndoList2Ptr	sptrUndoList( sptrND );
	if(sptrUndoList!=0) sptrUndoList->GetLastUndoAction( &m_ptrUndoPos );

	// Сохраним состояние вьюх
	TPOS pos = 0;
	IDocumentSitePtr sptrDoc(sptrND);
	if(sptrDoc!=0)
	{
		//m_ViewState.Push(sptrDC);
		m_ViewsStates.clear();
		sptrDoc->GetFirstViewPosition(&pos);
		while(pos)
		{
			IUnknownPtr ptrView;
			sptrDoc->GetNextView(&ptrView, &pos);
			CViewState state;
			state.Push(ptrView);
			m_ViewsStates.push_back(state);
		}
	}
}

void CDocumentState::PutTo(INamedDataPtr sptrND, IDataContext3Ptr sptrDC, bool bClearUndo)
{
	if( m_sptrND!=0 && sptrND!=0 )
		CopyNamedData( sptrND, m_sptrND );

	// Осторожно! Может почистить весь список Undo, если не содержит корректной ссылки!
	if(bClearUndo)
	{
		IUndoList2Ptr	sptrUndoList( sptrND );
		if(sptrUndoList!=0)
		{
			while(1)
			{
				IUnknownPtr ptrUndoPos;
				sptrUndoList->GetLastUndoAction( &ptrUndoPos );
				if(ptrUndoPos==0 || ptrUndoPos==m_ptrUndoPos)
					break;
				sptrUndoList->RemoveAction( ptrUndoPos );
			}
		}
	}

	// восстановим состояние вьюх
	IDocumentSitePtr sptrDoc(sptrND);
	if(sptrDoc!=0)
	{
		//m_ViewState.Pop(sptrDC);
		TPOS pos = 0;
		sptrDoc->GetFirstViewPosition(&pos);
		list<CViewState>::iterator pViewState = m_ViewsStates.begin();
		while(pos)
		{
			if(pViewState==m_ViewsStates.end()) break; // защита
			IUnknownPtr ptrView;
			sptrDoc->GetNextView(&ptrView, &pos);
			pViewState->Pop(ptrView);
		}
	}

}

// -------------------------

CActionHelper2::CActionHelper2(void)
{
	m_nFirstInvalidState = 0;
	m_psite = 0;
	m_bRunning = false;
	m_Action = ah2Continue;
}

CActionHelper2::~CActionHelper2(void)
{
}

void CActionHelper2::Init( CActiveXSite *psite, IUnknownPtr ptrDoc, int nStates )
{
	DeInit();

	m_psite = psite;
	m_sptrDoc = ptrDoc;

	m_States.alloc(nStates);
	for(int i=0; i<GetStatesCount(); i++)
	{
		m_States.ptr()[i].CreateEmpty();
	}
	m_nFirstInvalidState = 0; // ничего не готово, даже начальное состояние не запомнили
	m_Action = ah2Continue; // работать
}

void CActionHelper2::DeInit()
{
	m_ViewsLock.Clear(); // разлочим все вьюхи, если были залочены

	m_States.free();
	m_sptrDoc = 0;
}

bool CActionHelper2::SaveState(int index)
{
	if( index<0 || index>=GetStatesCount() )
		return false;

	IUnknownPtr ptrView = 0;
	if(m_sptrDoc!=0) m_sptrDoc->GetActiveView(&ptrView);

	m_States.ptr()[index].GetFrom(INamedDataPtr(m_sptrDoc), IDataContext3Ptr(ptrView));

	return true;
}

bool CActionHelper2::LoadState(int index, bool bClearUndo)
{
	if( index<0 || index>=GetStatesCount() )
		return false;

	IUnknownPtr ptrView = 0;
	if(m_sptrDoc!=0)
		m_sptrDoc->GetActiveView(&ptrView);

	m_States.ptr()[index].PutTo(INamedDataPtr(m_sptrDoc), IDataContext3Ptr(ptrView), bClearUndo);
	
	return true;
}

bool CActionHelper2::Invalidate(int index)
{ // пометить State как Invalid
	TRACE( "   CActionHelper2::Invalidate(%d)\n", index );
	if(m_nFirstInvalidState<index) return true; // ничего не надо делать - еще не дошли до index
	
	if(m_bRunning) // если что-то делаем - прервать
		TerminateCurrentOperation(1);

	m_nFirstInvalidState = max(index, 0);

	return true;
}

void CActionHelper2::LockAllViews()
{ // залочить все вьюхи - чтобы до следующего полного Update не моргало
	if(m_sptrDoc==0) return; // не проиничен
	TPOS pos = 0;
	m_sptrDoc->GetFirstViewPosition(&pos);
	while(pos)
	{
		IUnknownPtr ptrView;
		m_sptrDoc->GetNextView(&ptrView, &pos);
		m_ViewsLock.AddView(ptrView);
	}
}

void CActionHelper2::UnlockAllViews()
{	
	m_ViewsLock.Clear();
}


void CActionHelper2::Cancel()
{ // отменить выполнение
	if(m_bRunning) // если внутри UpdateStates/Process - то оборвем акцию и поставим пометку
		TerminateCurrentOperation(1);
	m_Action = ah2Cancel;
}

void CActionHelper2::Ok()
{ // завершить выполнение
	m_Action = ah2Ok;
}


bool CActionHelper2::UpdateStates()
{ // выполнить все Invalid States
	if(m_bRunning) return false; // нельзя вызывать один апдейт из другого
	XLock lock(&m_bRunning);

	if(m_sptrDoc==0) return false; // не проиничен

	if( m_nFirstInvalidState >= m_States.size() ) return false; // делать вообще нечего

	while( m_nFirstInvalidState < m_States.size() )
	{
		int nStage = m_nFirstInvalidState;

		if(nStage>0) // добавить еще проверку - вдруг не надо грузить (уже загружено)
			LoadState(nStage-1, true);
		m_nFirstInvalidState++;

		if(ah2Cancel==m_Action) // юзер где-то канцельнул
			break;

		if(nStage>0)
		{
			// собственно выполнение
			if(m_psite) m_psite->FireOnProcess(nStage);
			TerminateCurrentOperation(0); // Если юзер во время OnProcess сделал что-то для прерывания акций - вернем все в норму
		}

		// во время Process m_nFirstInvalidState мог быть сброшен
		if(nStage>=m_nFirstInvalidState) continue;

		SaveState(nStage);
	}

	if(ah2Cancel==m_Action) // вышли из цикла по Cancel - восстановить начальное состояние
	{
		if(m_nFirstInvalidState>0) // на всякий случай
			LoadState(0, true);
	}

	if(ah2Continue!=m_Action) // не надо продолжать - сбросим все
	{
		m_States.free();
	}

	UnlockAllViews(); // Разлочим вьюхи (надо обновить)
	LockAllViews(); // залочить все вьюхи - чтобы до следующего полного Update не моргало

	return true;
}
