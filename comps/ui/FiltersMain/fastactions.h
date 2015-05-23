#ifndef __fastactions_h__
#define __fastactions_h__

//[ag]1. classes

class CActionFastGradient : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFastGradient)
	GUARD_DECLARE_OLECREATE(CActionFastGradient)

public:
	CActionFastGradient();
	virtual ~CActionFastGradient();

public:
	virtual bool InvokeFilter();
};

class CActionFastUnsharpenMask : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFastUnsharpenMask)
	GUARD_DECLARE_OLECREATE(CActionFastUnsharpenMask)

public:
	CActionFastUnsharpenMask();
	virtual ~CActionFastUnsharpenMask();

public:
	virtual bool InvokeFilter();
};

class CActionFastAverage : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFastAverage)
	GUARD_DECLARE_OLECREATE(CActionFastAverage)

public:
	CActionFastAverage();
	virtual ~CActionFastAverage();

public:
	virtual bool InvokeFilter();
};


#endif //__fastactions_h__
