#if !defined(AFX_ACTIONMANAGER_H__C0B1BAC9_F95E_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_ACTIONMANAGER_H__C0B1BAC9_F95E_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CActionManager command target
struct CacheAIRecord
{
	IUnknown	*punkAI;
	int			idx;
	CString		strName;
};

typedef CList<DWORD, DWORD&> CDWordList;

class CActionManager : public CCmdTargetEx,
	public CNamedObjectImpl,
	public CCompManagerImpl,
	public CRootedObjectImpl,
	public CMsgListenerImpl,
	public CEventListenerImpl
{
	DECLARE_DYNCREATE(CActionManager)
	GUARD_DECLARE_OLECREATE(CActionManager)
public:
	CActionManager();
	virtual ~CActionManager();

	virtual void OnFinalRelease();
	virtual void OnChangeParent();

	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE()


	BEGIN_INTERFACE_PART(ActionManager, IActionManager2)
		com_call ExecuteAction( BSTR bstrActionName, BSTR bstrExecuteParams, DWORD dwExecuteFlags );
		com_call GetActionFlags( BSTR bstrActionName, DWORD *pdwFlags );
		com_call GetCurrentExecuteFlags( DWORD *pdwFlags );
		com_call SetScriptRunningFlag( BOOL bSet );
		com_call SetInteractiveRunningFlag( BOOL bSet );
		com_call IsScriptRunning();
		com_call TerminateInteractiveAction();
		com_call EnableManager(BOOL bEnable);
		com_call ExecuteDelayedActions();
		com_call InitDelayExecuting();
		com_call GetActionInfo( UINT nDispID, IUnknown **ppunkInfo );
		com_call AddCompleteActionToUndoList( IUnknown *punkAction );
		com_call GetActionInfo(BSTR bstrActionName, IUnknown **ppunkInfo);
		com_call GetRunningInteractiveActionInfo(IUnknown **ppunkInfo);
		com_call GetRunningInteractiveAction(IUnknown **ppunkAction);
		com_call ChangeInteractiveAction(IUnknown *punkInfo);
		com_call SetActionFlags( BSTR bstrActionName, DWORD dwState );//override default action flags. The main purpose is setup custom states for actions VBScript
		com_call ResetActionFlags( BSTR bstrActionName );
		com_call ResetAllActionsFlags();
		com_call LockInitInteractive( BOOL bLock, BOOL *pbOldLock );
		com_call FinalizeInteractiveAction();
		com_call ObfuscateName(BSTR* pbstrName); // "изувечить" им€
		com_call DeobfuscateName(BSTR* pbstrName); // перевести "изувеченное" им€ в нормальное; если не получаетс€ - вернуть как есть
	END_INTERFACE_PART(ActionManager);
	BEGIN_INTERFACE_PART(Disp, IDispatch)
        com_call GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo);
        
        com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
	END_INTERFACE_PART(Disp);

	//implememntation

	bool ExecuteActionCommand( DISPID dispIdMember, DISPPARAMS *pDispParams, bool bFromScript );

	//initialize Action object
	bool PreExecute( IUnknown *punkAction, DISPPARAMS *pDispParams = 0 );

	//begin interactive Action object
	bool InitInteractive();
	//finalize interactive Action object
	bool FinalizeInteractive();
	//terminate the interactive mode
	void TerminateInteractive(bool Finalize = false);

	bool Execute( IUnknown *punkAction );

	DWORD	CheckActionState( const char *pszAction );

public:
	static DISPPARAMS	*PhraseArguments( const char *pszArgString );
protected:
//actions execute
	IUnknown *GetActionInfoByName( const char *szName, int *pidx = 0 );	//without addref
	DISPID GetDispID( const char *pszName );
//interactive actions addition
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
//dynamic action's support
	virtual void OnAddComponent( int idx );
	virtual void OnRemoveComponent( int idx );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	bool InitUndoInfo( IUnknown *punkAction );		//init undo info
	bool StoreToUndo( IUnknown *punkAction );		//store the action to the UNDO list
	bool DoUndoRedo( IUnknown *punkAction, bool bUndo );	
													//execute "Undo" or "Redo" members
	void ExecuteNext();
	void AddToDelayList( const char *szAction, const char *szActionParams, DWORD dwFlags, bool bClearList );
protected:
	void _FillCache();
	void _FlushCache();
	void _CacheAction( IUnknown *punkAI, int idx );
	void _UncacheAction( IUnknown *punkAI );
	IUnknown	*_LookupCacheGetAction( const char *szActionName );
	int			_LookupCacheGetActionNo( const char *szActionName );
	void		_SetStatusCurrentAction();
	CString _ObfuscateName(CString strName);
protected:
	BOOL		m_bEnabled;		//show whether enable action execute
	bool		m_bCacheEmpty;
/////
	DWORD		m_dwCurrentExecuteFlags;
	bool		m_bWriteToLogFlag;
//interactive actions support
	sptrIInteractiveAction	m_sptrCurrentAction;
	sptrIActionInfo			m_sptrCurrentActionInfo;
	IUnknownPtr				m_sptrCurrentTarget;
	sptrIActionInfo			m_sptrChangeActionInfo;
	CMapStringToPtr			m_cacheByName;

	// дл€ обфускатора
	CMapStringToString	m_NameToObfuscated; // переводит нормальное им€ акции в искаженное
	CMapStringToString	m_ObfuscatedToName; // переводит искаженное им€ акции в нормальное

	CStringList				m_listDelayActions;
	CStringList				m_listDelayActionsParams;
	CDWordList				m_listDelayExecFlags;
	long					m_nRunningCount;
	long					m_nInitRunningCount;
	IActionTargetManagerPtr	m_ptrTargetManager;	
	CMap<CString, LPCSTR, DWORD, DWORD&>
							m_mapOverridedStates;
	HICON					m_hCurrentIcon;
	BOOL					m_bLockInitInteractive;

	BOOL					m_bold_enable, m_bneed_set_enable_flag;
public:
	static CActionManager	*s_pActionManager;	//the global pointer 

	//paul 18.04.2003
	bool					m_binside_interactive_loop;
	IActionPtr				m_ptr_running_action;

};

#endif // !defined(AFX_ACTIONMANAGER_H__C0B1BAC9_F95E_11D2_A596_0000B493FF1B__INCLUDED_)
