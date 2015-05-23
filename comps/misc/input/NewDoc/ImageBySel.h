// ImageBySel.h: interface for the CActionImageBySel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEBYSEL_H__D9ED0343_67CC_4555_9F72_77D2CEF0C9F3__INCLUDED_)
#define AFX_IMAGEBYSEL_H__D9ED0343_67CC_4555_9F72_77D2CEF0C9F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"


class CActionImageBySel : public CActionBase
{
	DECLARE_DYNCREATE(CActionImageBySel)
	GUARD_DECLARE_OLECREATE(CActionImageBySel)
	ENABLE_UNDO();

public:
	CActionImageBySel();
	virtual ~CActionImageBySel();

public:
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();

	virtual bool IsAvaible();

	CObjectListUndoRecord m_changes;
};


class CActionRestoreSel : public CActionBase
{
	DECLARE_DYNCREATE(CActionRestoreSel)
	GUARD_DECLARE_OLECREATE(CActionRestoreSel)
	ENABLE_UNDO();

	CObjectListUndoRecord m_changes;

public:
	CActionRestoreSel();
	virtual ~CActionRestoreSel();

public:
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();
};


#endif // !defined(AFX_IMAGEBYSEL_H__D9ED0343_67CC_4555_9F72_77D2CEF0C9F3__INCLUDED_)
