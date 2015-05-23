#pragma once
#include "method_int.h"

///////////////////////////////////////////////////////////////////////
class CActionMtdList : public CActionBase
{
	DECLARE_DYNCREATE( CActionMtdList );
	GUARD_DECLARE_OLECREATE( CActionMtdList );

public:
	virtual bool Invoke();
	virtual bool IsAvaible();	
};

//////////////////////////////////////////////////////////////////////
//
//	Class CMtdChooser
//
//////////////////////////////////////////////////////////////////////
class CMtdChooser : public CCmdTargetEx, 
							public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE( CMtdChooser );
	long m_lCurSel;
	bool m_bVirtualFreeMode;
protected:
	CMtdChooser();
	virtual ~CMtdChooser();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART( Combo, IUIComboBox )
		com_call OnInitList( HWND hWnd );
		com_call OnSelChange();
		com_call OnDetachWindow();
	END_INTERFACE_PART( Combo )

	HWND m_hwnd_combo;

	IUnknown *m_punkExcludeIt; // без AddRef
public:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
};

////////////////////////////////////////////////////////////////////////
class CActionMtdRunStop : public CActionBase
{
	DECLARE_DYNCREATE( CActionMtdRunStop );
	GUARD_DECLARE_OLECREATE( CActionMtdRunStop );

public:
	virtual bool Invoke();

	virtual bool IsAvaible();	
	virtual bool IsChecked();	
};


/*////////////////////////////////////////////////////////////////////////
class CActionMtdExportMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdExportMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdExportMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};  


////////////////////////////////////////////////////////////////////////
class CActionMtdImportMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdImportMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdImportMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};
*/

////////////////////////////////////////////////////////////////////////
class CActionMtdCreateMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdCreateMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdCreateMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};


////////////////////////////////////////////////////////////////////////
class CActionMtdSaveMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdSaveMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdSaveMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

////////////////////////////////////////////////////////////////////////
class CActionMtdCopyMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdCopyMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdCopyMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

// [vanek] SBT:986 - 18.05.2004
////////////////////////////////////////////////////////////////////////
class CActionMtdDeleteMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdDeleteMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdDeleteMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

////////////////////////////////////////////////////////////////////////
class CActionMtdDeleteStep : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdDeleteStep );
	GUARD_DECLARE_OLECREATE( CActionMtdDeleteStep );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

class CActionMtdUndo : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdUndo );
	GUARD_DECLARE_OLECREATE( CActionMtdUndo );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

class CActionMtdRedo : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdRedo );
	GUARD_DECLARE_OLECREATE( CActionMtdRedo );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

class CActionMtdExportScript : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdExportScript );
	GUARD_DECLARE_OLECREATE( CActionMtdExportScript );
	ENABLE_SETTINGS();
public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};

// [vanek] - 22.10.2004
////////////////////////////////////////////////////////////////////////
class CActionMtdInsertMsgStep : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdInsertMsgStep );
	GUARD_DECLARE_OLECREATE( CActionMtdInsertMsgStep );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

// [vanek] - 25.10.2004
////////////////////////////////////////////////////////////////////////
class CActionMtdClearMethod : public CActionBase
{
    DECLARE_DYNCREATE( CActionMtdClearMethod );
	GUARD_DECLARE_OLECREATE( CActionMtdClearMethod );

public:
	virtual bool IsAvaible();	
	virtual bool Invoke();
};

