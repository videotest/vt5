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
	lock.sptrView->LockUpdate(TRUE, FALSE); // ������ View ��������
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

		// ���������� �� ���� ����� � ������ ��� Selection � ���� ������ �� �������
		for (long nType = 0; nType < nTypeCount; nType++)
		{	// for all types
			_bstr_t bstrType;
			if (FAILED(sptrDC->GetObjectTypeName(nType, bstrType.GetAddress())) || !bstrType)
				continue;

			long lPos = 0;
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

		// ����������� ���
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

	// �������� ��������� ����
	long pos=0;
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

	// ���������! ����� ��������� ���� ������ Undo, ���� �� �������� ���������� ������!
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

	// ����������� ��������� ����
	IDocumentSitePtr sptrDoc(sptrND);
	if(sptrDoc!=0)
	{
		//m_ViewState.Pop(sptrDC);
		long pos=0;
		sptrDoc->GetFirstViewPosition(&pos);
		list<CViewState>::iterator pViewState = m_ViewsStates.begin();
		while(pos)
		{
			if(pViewState==m_ViewsStates.end()) break; // ������
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
	m_nFirstInvalidState = 0; // ������ �� ������, ���� ��������� ��������� �� ���������
	m_Action = ah2Continue; // ��������
}

void CActionHelper2::DeInit()
{
	m_ViewsLock.Clear(); // �������� ��� �����, ���� ���� ��������

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
{ // �������� State ��� Invalid
	TRACE( "   CActionHelper2::Invalidate(%d)\n", index );
	if(m_nFirstInvalidState<index) return true; // ������ �� ���� ������ - ��� �� ����� �� index
	
	if(m_bRunning) // ���� ���-�� ������ - ��������
		TerminateCurrentOperation(1);

	m_nFirstInvalidState = max(index, 0);

	return true;
}

void CActionHelper2::LockAllViews()
{ // �������� ��� ����� - ����� �� ���������� ������� Update �� �������
	if(m_sptrDoc==0) return; // �� ���������
	long pos=0;
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
{ // �������� ����������
	if(m_bRunning) // ���� ������ UpdateStates/Process - �� ������� ����� � �������� �������
		TerminateCurrentOperation(1);
	m_Action = ah2Cancel;
}

void CActionHelper2::Ok()
{ // ��������� ����������
	m_Action = ah2Ok;
}


bool CActionHelper2::UpdateStates()
{ // ��������� ��� Invalid States
	if(m_bRunning) return false; // ������ �������� ���� ������ �� �������
	XLock lock(&m_bRunning);

	if(m_sptrDoc==0) return false; // �� ���������

	if( m_nFirstInvalidState >= m_States.size() ) return false; // ������ ������ ������

	while( m_nFirstInvalidState < m_States.size() )
	{
		int nStage = m_nFirstInvalidState;

		if(nStage>0) // �������� ��� �������� - ����� �� ���� ������� (��� ���������)
			LoadState(nStage-1, true);
		m_nFirstInvalidState++;

		if(ah2Cancel==m_Action) // ���� ���-�� ����������
			break;

		if(nStage>0)
		{
			// ���������� ����������
			if(m_psite) m_psite->FireOnProcess(nStage);
			TerminateCurrentOperation(0); // ���� ���� �� ����� OnProcess ������ ���-�� ��� ���������� ����� - ������ ��� � �����
		}

		// �� ����� Process m_nFirstInvalidState ��� ���� �������
		if(nStage>=m_nFirstInvalidState) continue;

		SaveState(nStage);
	}

	if(ah2Cancel==m_Action) // ����� �� ����� �� Cancel - ������������ ��������� ���������
	{
		if(m_nFirstInvalidState>0) // �� ������ ������
			LoadState(0, true);
	}

	if(ah2Continue!=m_Action) // �� ���� ���������� - ������� ���
	{
		m_States.free();
	}

	UnlockAllViews(); // �������� ����� (���� ��������)
	LockAllViews(); // �������� ��� ����� - ����� �� ���������� ������� Update �� �������

	return true;
}
