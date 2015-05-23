#ifndef __morphology_h__
#define __morphology_h__

#include "imgfilterbase.h"

//[ag]1. classes

class CActionBoundary : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionBoundary)
	GUARD_DECLARE_OLECREATE(CActionBoundary)

public:
	CActionBoundary();
	virtual ~CActionBoundary();

public:
	virtual bool InvokeFilter();
};

class CActionClose : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionClose)
	GUARD_DECLARE_OLECREATE(CActionClose)

public:
	CActionClose();
	virtual ~CActionClose();

public:
	virtual bool InvokeFilter();
};

class CActionOpen : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionOpen)
	GUARD_DECLARE_OLECREATE(CActionOpen)

public:
	CActionOpen();
	virtual ~CActionOpen();

public:
	virtual bool InvokeFilter();
};

class CActionDliate : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionDliate)
	GUARD_DECLARE_OLECREATE(CActionDliate)

public:
	CActionDliate();
	virtual ~CActionDliate();

public:
	virtual bool InvokeFilter();
};

class CActionErode : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionErode)
	GUARD_DECLARE_OLECREATE(CActionErode)

public:
	CActionErode();
	virtual ~CActionErode();

public:
	virtual bool InvokeFilter();
};


#endif //__morphology_h__
