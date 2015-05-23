#pragma once

/////////////////////////////////////////////////////////////////////////////
class CActionFormPressOK : public CActionBase
{
	DECLARE_DYNCREATE(CActionFormPressOK)
	GUARD_DECLARE_OLECREATE(CActionFormPressOK)

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

/////////////////////////////////////////////////////////////////////////////
class CActionFormPressCancel : public CActionBase
{
	DECLARE_DYNCREATE(CActionFormPressCancel)
	GUARD_DECLARE_OLECREATE(CActionFormPressCancel)

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};
