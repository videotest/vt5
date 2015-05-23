#ifndef __syncactions_h__
#define __syncactions_h__

//[ag]1. classes

class CActionSyncContext : public CActionBase
{
	DECLARE_DYNCREATE(CActionSyncContext)
	GUARD_DECLARE_OLECREATE(CActionSyncContext)

public:
	CActionSyncContext();
	virtual ~CActionSyncContext();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	ISyncManagerPtr	m_ptrSyncManager;
};

class CActionSyncContextAll : public CActionSyncContext
{
	DECLARE_DYNCREATE(CActionSyncContextAll)
	GUARD_DECLARE_OLECREATE(CActionSyncContextAll)

	virtual bool IsAvaible();
};

class CActionSyncScrollRelative : public CActionBase
{
	DECLARE_DYNCREATE(CActionSyncScrollRelative)
	GUARD_DECLARE_OLECREATE(CActionSyncScrollRelative)

public:
	CActionSyncScrollRelative();
	virtual ~CActionSyncScrollRelative();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	ISyncManagerPtr	m_ptrSyncManager;
};

class CActionSyncZoom : public CActionBase
{
	DECLARE_DYNCREATE(CActionSyncZoom)
	GUARD_DECLARE_OLECREATE(CActionSyncZoom)

public:
	CActionSyncZoom();
	virtual ~CActionSyncZoom();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();

	ISyncManagerPtr	m_ptrSyncManager;
};

class CActionSyncScroll : public CActionBase
{
	DECLARE_DYNCREATE(CActionSyncScroll)
	GUARD_DECLARE_OLECREATE(CActionSyncScroll)

public:
	CActionSyncScroll();
	virtual ~CActionSyncScroll();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();

	ISyncManagerPtr	m_ptrSyncManager;
};


#endif //__syncactions_h__
