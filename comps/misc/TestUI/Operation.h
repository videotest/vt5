#pragma once

class COperationBase
{
public:
	virtual void Invoke() = 0;
	virtual void DoUndo() = 0;
	virtual void DoRedo() = 0;
};

class CUndoRedoList
{
public:
	CTypedPtrArray<CPtrArray, COperationBase*> m_UndoList;
	CTypedPtrArray<CPtrArray, COperationBase*> m_RedoList;
	CUndoRedoList();
	~CUndoRedoList();
	void Make(COperationBase *pOperation);
	void Undo();
	void Redo();
	bool CanUndo();
	bool CanRedo();
};

