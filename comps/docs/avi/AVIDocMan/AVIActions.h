#ifndef __AVIActions_H__
#define __AVIActions_H__

#include "common.h"

class CAVIActionBase : public CActionBase
{
public:
	IUnknown *_GetActiveContext( bool bAddRef = false );
};

//[ag]1. classes

class CActionAVIFirst : public CAVIActionBase
{
	DECLARE_DYNCREATE(CActionAVIFirst)
	GUARD_DECLARE_OLECREATE(CActionAVIFirst)

public:
	CActionAVIFirst();
	virtual ~CActionAVIFirst();

public:
	virtual bool IsAvaible();
	virtual bool Invoke();
};

class CActionAVIPrev : public CAVIActionBase
{
	DECLARE_DYNCREATE(CActionAVIPrev)
	GUARD_DECLARE_OLECREATE(CActionAVIPrev)

public:
	CActionAVIPrev();
	virtual ~CActionAVIPrev();

public:
	virtual bool IsAvaible();
	virtual bool Invoke();
};

class CActionAVINext : public CAVIActionBase
{
	DECLARE_DYNCREATE(CActionAVINext)
	GUARD_DECLARE_OLECREATE(CActionAVINext)

public:
	CActionAVINext();
	virtual ~CActionAVINext();

public:
	virtual bool IsAvaible();
	virtual bool Invoke();
};

class CActionAVILast : public CAVIActionBase
{
	DECLARE_DYNCREATE(CActionAVILast)
	GUARD_DECLARE_OLECREATE(CActionAVILast)

public:
	CActionAVILast();
	virtual ~CActionAVILast();

public:
	virtual bool IsAvaible();
	virtual bool Invoke();
};

class CActionAVICapture : public CAVIActionBase
{
	DECLARE_DYNCREATE(CActionAVICapture)
	GUARD_DECLARE_OLECREATE(CActionAVICapture)

public:
	CActionAVICapture();
	virtual ~CActionAVICapture();

public:
	virtual bool Invoke();
};


#endif //__AVIActions_H__
