#pragma once

class CActionShowIdioDBView : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowIdioDBView)
	GUARD_DECLARE_OLECREATE(CActionShowIdioDBView)
public:
	virtual CString GetViewProgID();
};

class CActionAddToIdioDB : public CActionBase
{
	DECLARE_DYNCREATE( CActionAddToIdioDB )
	GUARD_DECLARE_OLECREATE( CActionAddToIdioDB )
public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};

class CActionFindObject : public CActionBase
{
	DECLARE_DYNCREATE( CActionFindObject )
	GUARD_DECLARE_OLECREATE( CActionFindObject )
public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};

