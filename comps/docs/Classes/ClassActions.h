#ifndef __CLASS_ACTIONS_H__
#define __CLASS_ACTIONS_H__

#include "ObListWrp.h"
#include "Classes5.h"

//[ag]1. classes

class CActionSetActiveClass : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetActiveClass)
	GUARD_DECLARE_OLECREATE(CActionSetActiveClass)

	ENABLE_SETTINGS();
public:
	CActionSetActiveClass();
	virtual ~CActionSetActiveClass();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

protected:
	CString			m_strClassName;
	CObjectListWrp	m_listClasses;
};

class CActionObjectSetClassDirect : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectSetClassDirect)
	GUARD_DECLARE_OLECREATE(CActionObjectSetClassDirect)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionObjectSetClassDirect();
	virtual ~CActionObjectSetClassDirect();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

	void NotifyParent();

protected:
	long	m_lPrevClassKey, m_lNewClassKey;
	ICalcObjectPtr			m_sptrObj;
};

class CActionClassDelete : public CActionBase
{
	DECLARE_DYNCREATE(CActionClassDelete)
	GUARD_DECLARE_OLECREATE(CActionClassDelete)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionClassDelete();
	virtual ~CActionClassDelete();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	CObjectListUndoRecord	m_changes;	//undo
	IUnknown*				m_punkObj;
	CString					m_strClassName;
};

class CActionClassCreate : public CActionBase
{
	DECLARE_DYNCREATE(CActionClassCreate)
	GUARD_DECLARE_OLECREATE(CActionClassCreate)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionClassCreate();
	virtual ~CActionClassCreate();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	CObjectListUndoRecord	m_changes;	//undo
	CObjectListWrp			m_listWrp;
	CString					m_strClassName;

};

class CActionClassRecolor : public CActionBase
{
	DECLARE_DYNCREATE(CActionClassRecolor)
	GUARD_DECLARE_OLECREATE(CActionClassRecolor)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionClassRecolor();
	virtual ~CActionClassRecolor();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	COLORREF		m_clOld;
	COLORREF		m_clNew;
	IClassObjectPtr	m_sptrClass;

};

class CActionClassRename : public CActionBase
{
	DECLARE_DYNCREATE(CActionClassRename)
	GUARD_DECLARE_OLECREATE(CActionClassRename)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionClassRename();
	virtual ~CActionClassRename();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:

	CString					m_strOldName;
	CString					m_strNewName;
	INamedDataObject2Ptr	m_sptrObject;
};

#include "AfxTempl.h"

class CActionObjectSetClass : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectSetClass)
	GUARD_DECLARE_OLECREATE(CActionObjectSetClass)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionObjectSetClass();
	virtual ~CActionObjectSetClass();

public:
	IUnknown*	GetActiveList(LPCTSTR szObjectType);
	long	GetClassKey(LPCTSTR szSelectedClassName);
	void		Clear();

	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();

protected:
	long			m_lClassKey;
	CString			m_strClassName;
	CObjectListWrp	m_listClasses;
	CObjectListWrp	m_listObjects;

	CObjectListUndoRecord		m_changes;	//undo
	CArray <long, long>			m_arrKeys;
	CPtrArray					m_arrObjects;
};

class CActionCalcStat : public CActionBase
{
	DECLARE_DYNCREATE(CActionCalcStat)
	GUARD_DECLARE_OLECREATE(CActionCalcStat)

	ENABLE_SETTINGS();
public:
	CActionCalcStat();
	virtual ~CActionCalcStat();

public:
	IUnknown* GetActiveList(LPCTSTR szObjectType);
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};


#endif// __CLASS_ACTIONS_H__
