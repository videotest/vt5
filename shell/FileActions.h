#ifndef __FileActions_h__
#define __FileActions_h__

//simple file actions here
//file//exit for begin

class CActionFileExit: public CActionBase
{
	DECLARE_DYNCREATE(CActionFileExit);
	GUARD_DECLARE_OLECREATE(CActionFileExit);

	virtual bool Invoke();
};

class CActionFileOpen: public CActionBase
{
	DECLARE_DYNCREATE(CActionFileOpen);
	GUARD_DECLARE_OLECREATE(CActionFileOpen);

	ENABLE_SETTINGS();
public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};

class CActionFileNew: public CActionBase
{
	DECLARE_DYNCREATE(CActionFileNew);
	GUARD_DECLARE_OLECREATE(CActionFileNew);

	ENABLE_SETTINGS();
public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual void MakeModified()	{return;}
};

class CActionFileSave: public CActionBase
{
	DECLARE_DYNCREATE(CActionFileSave);
	GUARD_DECLARE_OLECREATE(CActionFileSave);

	ENABLE_SETTINGS();
public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
protected:
	virtual bool ShowDlgAnyway(){return false;}
	virtual UINT GetDialogTitleID(){return IDS_FILE_SAVE_DLG_TITLE;}
	bool IsNeedPrompt();
	virtual void MakeModified()	{m_pdoc->SetModifiedFlag( 0 );}
protected:
	CDocument	*m_pdoc;
	CString		m_strFileName;
};

class CActionFileSaveAs: public CActionFileSave
{
	DECLARE_DYNCREATE(CActionFileSaveAs);
	GUARD_DECLARE_OLECREATE(CActionFileSaveAs);
public:
protected:
	virtual bool ShowDlgAnyway(){return true;}
	virtual UINT GetDialogTitleID(){return IDS_FILE_SAVEAS_DLG_TITLE;}
};

class CActionFileClose: public CActionBase
{
	DECLARE_DYNCREATE(CActionFileClose);
	GUARD_DECLARE_OLECREATE(CActionFileClose);

	ENABLE_SETTINGS();
//pointer to document for close
	CDocument	*m_pdoc;
public:
	CActionFileClose();

	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};

//[ag]1. classes

class CActionFileCloseAll : public CActionBase
{
	DECLARE_DYNCREATE(CActionFileCloseAll)
	GUARD_DECLARE_OLECREATE(CActionFileCloseAll)

public:
	CActionFileCloseAll();
	virtual ~CActionFileCloseAll();

public:
	virtual bool Invoke();
};

class CActionFileSendEMail : public CActionBase
{
	DECLARE_DYNCREATE(CActionFileSendEMail)
	GUARD_DECLARE_OLECREATE(CActionFileSendEMail)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionFileSaveView : public CActionBase
{
	DECLARE_DYNCREATE(CActionFileSaveView)
	GUARD_DECLARE_OLECREATE(CActionFileSaveView)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
protected:
	bool save_to_file( const char *psz_filename );
};

class CActionFileSendView : public CActionFileSaveView
{
	DECLARE_DYNCREATE(CActionFileSendView)
	GUARD_DECLARE_OLECREATE(CActionFileSendView)
public:
	virtual bool Invoke();
};

class CActionDumpMem: public CActionBase
{
	DECLARE_DYNCREATE(CActionDumpMem);
	GUARD_DECLARE_OLECREATE(CActionDumpMem);

	virtual bool Invoke();
};



#endif //__FileActions_h__
