#ifndef __toolactions_h__
#define __toolactions_h__

//[ag]1. classes

class CActionRunCommand : public CActionBase
{
	DECLARE_DYNCREATE(CActionRunCommand)
	GUARD_DECLARE_OLECREATE(CActionRunCommand)

	ENABLE_SETTINGS();
public:
	CActionRunCommand();
	virtual ~CActionRunCommand();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
protected:
	CString	m_strScriptText;
};

class CActionEnumActions : public CActionBase
{
	DECLARE_DYNCREATE(CActionEnumActions)
	GUARD_DECLARE_OLECREATE(CActionEnumActions)

public:
	CActionEnumActions();
	virtual ~CActionEnumActions();

public:
	virtual bool Invoke();
};


#endif //__toolactions_h__
