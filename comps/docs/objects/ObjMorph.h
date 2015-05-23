#if !defined(__ObjMorph_H__)
#define __ObjMorph_H__

#include "actionbase.h"
#include "CompManager.h"
#include "ObListWrp.h"
#include "resource.h"

class CObjectMorphBase : public CActionBase
{
	ENABLE_UNDO();
protected:
	IUnknownPtr m_punkSavedImage,m_punkObject;
	virtual bool DoRedo();
	virtual bool DoUndo(); 
};

class CObjectErode : public CObjectMorphBase
{
	DECLARE_DYNCREATE(CObjectErode);
	GUARD_DECLARE_OLECREATE(CObjectErode);
public:
	virtual bool Invoke();
};

class CObjectDilate : public CObjectMorphBase
{
	DECLARE_DYNCREATE(CObjectDilate);
	GUARD_DECLARE_OLECREATE(CObjectDilate);
public:
	virtual bool Invoke();
};

class CObjectOpen : public CObjectMorphBase
{
	DECLARE_DYNCREATE(CObjectOpen);
	GUARD_DECLARE_OLECREATE(CObjectOpen);
public:
	virtual bool Invoke();
};


class CObjectClose : public CObjectMorphBase
{
	DECLARE_DYNCREATE(CObjectClose);
	GUARD_DECLARE_OLECREATE(CObjectClose);
public:
	virtual bool Invoke();
};


class CObjectFillHoles : public CObjectMorphBase
{
	DECLARE_DYNCREATE(CObjectFillHoles);
	GUARD_DECLARE_OLECREATE(CObjectFillHoles);
public:
	virtual bool Invoke();
};

class CObjectSetZOrder : public CActionBase
{
	DECLARE_DYNCREATE(CObjectSetZOrder);
	GUARD_DECLARE_OLECREATE(CObjectSetZOrder);
public:
	virtual bool Invoke();
};

#endif
