#pragma once
#include "resource.h"
////////////////////////////////////////////////////////////////////////
namespace MetodicsTabSpace
{
namespace Actions
{
class CActionAddPane : public CActionBase
{
	DECLARE_DYNCREATE( CActionAddPane );
	GUARD_DECLARE_OLECREATE( CActionAddPane );

	ENABLE_SETTINGS();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	

	virtual bool ExecuteSettings( CWnd *pwndParent );
};         
////////////////////////////////////////////////////////////////////////
class CActionRemoveMtdPane : public CActionBase
{
	DECLARE_DYNCREATE( CActionRemoveMtdPane );
	GUARD_DECLARE_OLECREATE( CActionRemoveMtdPane );

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};
////////////////////////////////////////////////////////////////////////
class CActionMtdLoadContent : public CActionBase
{
	DECLARE_DYNCREATE( CActionMtdLoadContent );
	GUARD_DECLARE_OLECREATE( CActionMtdLoadContent );

	ENABLE_SETTINGS();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	

	virtual bool ExecuteSettings( CWnd *pwndParent );
};
////////////////////////////////////////////////////////////////////////
class CActionMtdStoreContent : public CActionBase
{
	DECLARE_DYNCREATE( CActionMtdStoreContent );
	GUARD_DECLARE_OLECREATE( CActionMtdStoreContent );

	ENABLE_SETTINGS();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	

	virtual bool ExecuteSettings( CWnd *pwndParent );
};
////////////////////////////////////////////////////////////////////////
class CActionRemoveItem : public CActionBase
{
	DECLARE_DYNCREATE( CActionRemoveItem );
	GUARD_DECLARE_OLECREATE( CActionRemoveItem );

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};
//////////////////////////////////////////////////////////////////////
class CActionAddItem : public CActionBase
{
	DECLARE_DYNCREATE( CActionAddItem );
	GUARD_DECLARE_OLECREATE( CActionAddItem );

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};
//////////////////////////////////////////////////////////////////////
class CActionOpenPane : public CActionBase
{
	DECLARE_DYNCREATE( CActionOpenPane );
	GUARD_DECLARE_OLECREATE( CActionOpenPane );

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};


}
}