#ifndef __MiscActions_h__
#define __MiscActions_h__

//[ag]1. classes

class CActionToolsLoadSave : public CActionBase
{
	DECLARE_DYNCREATE(CActionToolsLoadSave)
	GUARD_DECLARE_OLECREATE(CActionToolsLoadSave)

	ENABLE_SETTINGS();
public:
	CActionToolsLoadSave();
	virtual ~CActionToolsLoadSave();

public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
protected:
	CString	m_strFileName;
};

class CActionToolsLoadSaveEx : public CActionBase
{
	DECLARE_DYNCREATE(CActionToolsLoadSaveEx)
	GUARD_DECLARE_OLECREATE(CActionToolsLoadSaveEx)

	ENABLE_SETTINGS();
public:
	CActionToolsLoadSaveEx();
	virtual ~CActionToolsLoadSaveEx();

public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
protected:
	CString	m_strFileName;
};

class CActionToolsStateSave : public CActionBase
{
	DECLARE_DYNCREATE(CActionToolsStateSave)
	GUARD_DECLARE_OLECREATE(CActionToolsStateSave)

	ENABLE_SETTINGS();
public:
	CActionToolsStateSave();
	virtual ~CActionToolsStateSave();

public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
protected:
	CString	m_strFileName;
};

// [vanek] - 16.07.2004
class CActionToolsStateSaveAs : public CActionBase
{
	DECLARE_DYNCREATE(CActionToolsStateSaveAs)
	GUARD_DECLARE_OLECREATE(CActionToolsStateSaveAs)

	ENABLE_SETTINGS();
public:
	CActionToolsStateSaveAs();
	virtual ~CActionToolsStateSaveAs();

public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
protected:
	CString	m_strFileName;
};

class CActionToolsCustomize: public CActionBase
{
	DECLARE_DYNCREATE(CActionToolsCustomize);
	GUARD_DECLARE_OLECREATE(CActionToolsCustomize);

	ENABLE_SETTINGS();
public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
};


#endif //__MiscActions_h__
