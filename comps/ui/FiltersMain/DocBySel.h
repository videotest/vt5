#pragma once


class CActionDocBySel : public CActionBase
{
	DECLARE_DYNCREATE(CActionDocBySel)
	GUARD_DECLARE_OLECREATE(CActionDocBySel)
	ENABLE_UNDO();

public:
	CActionDocBySel();
	virtual ~CActionDocBySel();

public:
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();

	virtual bool IsAvaible();

	CObjectListUndoRecord m_changes;
};


