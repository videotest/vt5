#pragma once

#include "MacroRecorder_int.h"
#include "idispatchimpl.h"
#include "impl_misc.h"
#include "impl_event.h"
#include "\vt5\awin\misc_list.h"


class CCachedMacroHelper
{
	IMacroHelper *m_phelper;
public:
	CCachedMacroHelper(void);
	~CCachedMacroHelper(void);
	BOOL	GetMacroHelper( IMacroHelper **ppmh );
};


class CMacroManager:	public ComObjectBase,
						public IMacroManager,
						public CNamedObjectImpl,
						public IDispatchImpl< IMacroManagerDisp >,
						public CEventListenerImpl
{
	BOOL	m_brecording,
			m_bexecuting,
			m_bstart_record;	// for process message

	int		m_nold_mousespeed;	// old mouse spped

	CString m_str_script;
public:
	CMacroManager(void);
	virtual ~CMacroManager(void);

	route_unknown( );
	IUnknown* DoGetInterface(const IID &iid );

	// IMacroManager Interface
	com_call ProcessMessageHandler( /*[in]*/ MSG *pMsg );	
	com_call IsRecording( /*[out]*/ BOOL *pbRecording );	
	com_call IsExecuting( /*[out]*/ BOOL *pbExecuting );
    
	// IMacroManagerDisp Interface
	com_call StartRecord( );
	com_call ResumeRecord( );
	com_call StopRecord( );
	com_call SetMacroScript( /*[in]*/ BSTR bstrScript );
	com_call GetMacroScript( /*[out, retval]*/ BSTR *pbstrScript );
	com_call ExecuteScript( );

	static CMacroManager *s_pMacroManager;

	void OnTimer( );

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	CCachedMacroHelper m_helper;

	void _init_for_rec(void);

	// [vanek] : double click
	int m_nlast_btndown;
	bool m_bwait_end_dblclick;

	// [vanek] : key's states
	DWORD	m_dwkeys_states;

	// [vanek] : snapshot's running flag
	bool	m_bsnapshot_running;
};
					

// CLockerFlag class
class CLockerFlag
{
public:
	CLockerFlag( BOOL *pbFlag ) { m_pbFlag = pbFlag; *m_pbFlag = TRUE; }
	~CLockerFlag()	{ *m_pbFlag = FALSE; }
	BOOL	*m_pbFlag;
};
