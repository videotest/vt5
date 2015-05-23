#ifndef __HelpActions_h__
#define __HelpActions_h__

//[ag]1. classes

class CActionHelpIndex : public CActionBase
{
	DECLARE_DYNCREATE(CActionHelpIndex)
	GUARD_DECLARE_OLECREATE(CActionHelpIndex)

public:
	CActionHelpIndex();
	virtual ~CActionHelpIndex();

public:
	virtual bool Invoke();
};

class CActionHelpAbout : public CActionBase
{
	DECLARE_DYNCREATE(CActionHelpAbout)
	GUARD_DECLARE_OLECREATE(CActionHelpAbout)

public:
	CActionHelpAbout();
	virtual ~CActionHelpAbout();

public:
	virtual bool Invoke();
};

class CActionHelpContext : public CActionBase
{
	DECLARE_DYNCREATE(CActionHelpContext)
	GUARD_DECLARE_OLECREATE(CActionHelpContext)
public:
	CActionHelpContext();
	virtual ~CActionHelpContext();

public:
	virtual bool Invoke();

	CString	m_strActionName;

	static bool m_bInside; // статическая переменная, указывающая, не находимся ли мы уже внутри акции
	// (чтобы избежать повторного вызова)
};

class CActionHelpContents : public CActionBase
{
	DECLARE_DYNCREATE(CActionHelpContents)
	GUARD_DECLARE_OLECREATE(CActionHelpContents)

public:
	CActionHelpContents();
	virtual ~CActionHelpContents();

public:
	virtual bool Invoke();
};


#endif //__HelpActions_h__
