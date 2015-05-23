#ifndef __fpactions_h__
#define __fpactions_h__

//[ag]1. classes

class CActionThresholdTrace : public CFilterBase
{
	DECLARE_DYNCREATE(CActionThresholdTrace)
	GUARD_DECLARE_OLECREATE(CActionThresholdTrace)

public:
	CActionThresholdTrace();
	virtual ~CActionThresholdTrace();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
};

class CActionEluminateBackgr : public CFilterBase
{
	DECLARE_DYNCREATE(CActionEluminateBackgr)
	GUARD_DECLARE_OLECREATE(CActionEluminateBackgr)

public:
	CActionEluminateBackgr();
	virtual ~CActionEluminateBackgr();

public:
	virtual bool InvokeFilter();
};


#endif //__fpactions_h__
