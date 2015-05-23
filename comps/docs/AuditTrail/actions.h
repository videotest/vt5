#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "auditint.h"

//[ag]1. classes

class CActionSetupAT : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetupAT)
	GUARD_DECLARE_OLECREATE(CActionSetupAT)

public:
	CActionSetupAT();
public:
	virtual bool Invoke();
};


class CActionShowTextAT : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowTextAT)
	GUARD_DECLARE_OLECREATE(CActionShowTextAT)

public:
	virtual CString GetViewProgID();
};

class CActionSetActiveAThumbnail : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetActiveAThumbnail)
	GUARD_DECLARE_OLECREATE(CActionSetActiveAThumbnail)

public:
	CActionSetActiveAThumbnail();
	virtual ~CActionSetActiveAThumbnail();

protected:
	IAuditTrailObject* GetAT();

public:
	virtual bool Invoke();
};

class CActionSetPrevAThumbnail : public CActionSetActiveAThumbnail
{
	DECLARE_DYNCREATE(CActionSetPrevAThumbnail)
	GUARD_DECLARE_OLECREATE(CActionSetPrevAThumbnail)

public:
	CActionSetPrevAThumbnail();
	virtual ~CActionSetPrevAThumbnail();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionSetNextAThumbnail : public CActionSetActiveAThumbnail
{
	DECLARE_DYNCREATE(CActionSetNextAThumbnail)
	GUARD_DECLARE_OLECREATE(CActionSetNextAThumbnail)

public:
	CActionSetNextAThumbnail();
	virtual ~CActionSetNextAThumbnail();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionShowSingleAT : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowSingleAT)
	GUARD_DECLARE_OLECREATE(CActionShowSingleAT)

public:
	virtual CString GetViewProgID();
};

class CActionShowAT : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowAT)
	GUARD_DECLARE_OLECREATE(CActionShowAT)
public:
	virtual CString GetViewProgID();
};

class CActionOnOffAT : public CActionBase
{
	DECLARE_DYNCREATE(CActionOnOffAT)
	GUARD_DECLARE_OLECREATE(CActionOnOffAT)

public:
	CActionOnOffAT();
	virtual ~CActionOnOffAT();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


#endif
