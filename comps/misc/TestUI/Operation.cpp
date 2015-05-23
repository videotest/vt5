#include "StdAfx.h"
#include "Operation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void ClearArray(CPtrArray *parr)
{
	for (int i = 0; i < parr->GetSize(); i++)
		delete parr->GetAt(i);
	parr->RemoveAll();
}

CUndoRedoList::CUndoRedoList()
{
}

CUndoRedoList::~CUndoRedoList()
{
	ClearArray(&m_UndoList);
	ClearArray(&m_RedoList);
}

void CUndoRedoList::Make(COperationBase *pOperation)
{
	pOperation->Invoke();
	m_UndoList.Add(pOperation);
}

void CUndoRedoList::Undo()
{
	int n = m_UndoList.GetSize()-1;
	if (n >= 0)
	{
		COperationBase *pOperation = m_UndoList.GetAt(n);
		pOperation->DoUndo();
		m_UndoList.RemoveAt(n);
		m_RedoList.InsertAt(0, pOperation);
	}
}

void CUndoRedoList::Redo()
{
	if (m_RedoList.GetSize() >= 1)
	{
		COperationBase *pOperation = m_RedoList.GetAt(0);
		m_RedoList.RemoveAt(0);
		pOperation->DoRedo();
		m_UndoList.Add(pOperation);
	}
}

bool CUndoRedoList::CanUndo()
{
	return m_UndoList.GetSize()>=1;
}

bool CUndoRedoList::CanRedo()
{
	return m_RedoList.GetSize()>=1;
}


