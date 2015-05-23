#ifndef __scriptactions_h__
#define __scriptactions_h__

#include "activscp.h"
#include "scriptdocint.h"

class CActionScriptStart: public CActionBase
{
	DECLARE_DYNCREATE(CActionScriptStart);
	GUARD_DECLARE_OLECREATE(CActionScriptStart);

	CString GetScriptText();

	virtual bool Invoke();
	virtual bool IsAvaible();

	GuidKey	m_lScriptKey;
	CString m_strScriptName;
};

class CActionScriptTerminate: public CActionBase
{
	DECLARE_DYNCREATE(CActionScriptTerminate);
	GUARD_DECLARE_OLECREATE(CActionScriptTerminate);

	virtual bool Invoke();
	virtual bool IsAvaible();
};

//[ag]1. classes

class CActionStartDebugger : public CActionBase
{
	DECLARE_DYNCREATE(CActionStartDebugger)
	GUARD_DECLARE_OLECREATE(CActionStartDebugger)

public:
	CActionStartDebugger();
	virtual ~CActionStartDebugger();

public:
	virtual bool Invoke();
};

class CActionToggleBreakpoint : public CActionBase
{
	DECLARE_DYNCREATE(CActionToggleBreakpoint)
	GUARD_DECLARE_OLECREATE(CActionToggleBreakpoint)

public:
	CActionToggleBreakpoint();
	virtual ~CActionToggleBreakpoint();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionBreakAtNextStatement : public CActionScriptStart
{
	DECLARE_DYNCREATE(CActionBreakAtNextStatement)
	GUARD_DECLARE_OLECREATE(CActionBreakAtNextStatement)

public:
	CActionBreakAtNextStatement();

public:
	virtual bool Invoke();
};


class CCrystalTextBuffer;

interface IScriptChangeAction : public IUnknown 
{
	com_call GetObjectKey( long *plKey ) = 0;
	com_call GetTargetKey( long *plTargetKey ) = 0;
};

declare_interface(IScriptChangeAction, "E05D0A41-7022-11d3-A65A-0090275995FE");


class CActionScriptChange : public CActionBase
{
	DECLARE_DYNCREATE(CActionScriptChange)
	GUARD_DECLARE_OLECREATE(CActionScriptChange)

	DECLARE_INTERFACE_MAP();

	ENABLE_UNDO();
public:
	CActionScriptChange();
public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	CCrystalTextBuffer *GetCurrentTextBuffer();
protected:
	GuidKey m_lObjectKey;

	BEGIN_INTERFACE_PART(SAct, IScriptChangeAction)
		com_call GetObjectKey( long *plKey );
		com_call GetTargetKey( long *plTargetKey );
	END_INTERFACE_PART(SAct)
};

class CActionShowScript : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowScript)
	GUARD_DECLARE_OLECREATE(CActionShowScript)
protected:
	virtual CString GetViewProgID();
};



#endif //__scriptactions_h__
