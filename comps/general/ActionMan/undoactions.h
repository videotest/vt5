#ifndef __undoactions_h__
#define __undoactions_h__


class CActionUndo : public CActionBase
{
	DECLARE_DYNCREATE(CActionUndo)
	GUARD_DECLARE_OLECREATE(CActionUndo)

public:
	CActionUndo();
	virtual ~CActionUndo();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionRedo : public CActionBase
{
	DECLARE_DYNCREATE(CActionRedo)
	GUARD_DECLARE_OLECREATE(CActionRedo)

public:
	CActionRedo();
	virtual ~CActionRedo();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};


#endif //__undoactions_h__
