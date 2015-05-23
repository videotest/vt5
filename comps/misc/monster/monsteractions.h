#ifndef __monsteractions_h__
#define __monsteractions_h__

//[ag]1. classes
class CMessageWindow;

class CActionMonsterMessage : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionMonsterMessage)
	GUARD_DECLARE_OLECREATE(CActionMonsterMessage)

	ENABLE_SETTINGS();
public:
	CActionMonsterMessage();
	virtual ~CActionMonsterMessage();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
//update interface
protected:
	CString	m_strMessage;
	CMessageWindow	*m_pwndMessage;
};

class CActionMonsterMove : public CActionBase
{
	DECLARE_DYNCREATE(CActionMonsterMove)
	GUARD_DECLARE_OLECREATE(CActionMonsterMove)

public:
	CActionMonsterMove();
	virtual ~CActionMonsterMove();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionMonsterShow : public CActionBase
{
	DECLARE_DYNCREATE(CActionMonsterShow)
	GUARD_DECLARE_OLECREATE(CActionMonsterShow)

public:
	virtual bool Invoke();
	virtual bool IsChecked();
};


#endif //__monsteractions_h__
