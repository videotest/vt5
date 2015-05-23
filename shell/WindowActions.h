#ifndef __WindowActions_h__
#define __WindowActions_h__

#include "MainFrm.h"

class CActionWndBase : public CActionBase
{
public:
	CMainFrame	*GetMainFrame();
};

//[ag]1. classes

class CActionWindowActivate : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowActivate)
	GUARD_DECLARE_OLECREATE(CActionWindowActivate)

public:
	CActionWindowActivate();
	virtual ~CActionWindowActivate();

	ENABLE_SETTINGS();
public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
protected:
	CMDIChildWnd	*m_pwndToActivate;
	
};

class CActionWindowNew : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowNew)
	GUARD_DECLARE_OLECREATE(CActionWindowNew)

public:
	CActionWindowNew();
	virtual ~CActionWindowNew();

public:
	virtual bool Invoke();
};

class CActionWindowTile : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowTile)
	GUARD_DECLARE_OLECREATE(CActionWindowTile)

public:
	CActionWindowTile();
	virtual ~CActionWindowTile();

public:
	virtual bool Invoke();
};

class CActionWindowCascade : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowCascade)
	GUARD_DECLARE_OLECREATE(CActionWindowCascade)

public:
	CActionWindowCascade();
	virtual ~CActionWindowCascade();

public:
	virtual bool Invoke();
};

class CActionWindowArrange : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowArrange)
	GUARD_DECLARE_OLECREATE(CActionWindowArrange)

public:
	CActionWindowArrange();
	virtual ~CActionWindowArrange();

public:
	virtual bool Invoke();
};

class CActionWindowClose : public CActionWndBase
{
	DECLARE_DYNCREATE(CActionWindowClose)
	GUARD_DECLARE_OLECREATE(CActionWindowClose)

public:
	CActionWindowClose();
	virtual ~CActionWindowClose();

	ENABLE_SETTINGS();
public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

	CMDIChildWnd	*m_pwndToActivate;
};


#endif //__WindowActions_h__
