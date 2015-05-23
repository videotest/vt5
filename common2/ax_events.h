#ifndef __ax_events_h__
#define __ax_events_h__

#include "win_main.h"
#include "com_main.h"
#include "ax_host.h"

typedef void (CWinImpl::*FuncEventHandler)();

struct EventEntry
{
	FuncEventHandler	pfnEvent;
	DISPID		nDispID;
public:
	EventEntry	*m_pnext;
};

class ConnPtClient : public ComObjectBase, 
				public IDispatch
{
public:
	route_unknown();

	ConnPtClient();
	virtual ~ConnPtClient();
	virtual IUnknown *DoGetInterface( const IID &iid );

public:
	const IID &iid()	{return m_iidDisp;}
	const UINT id()		{return m_nIDControl;}
	bool Init( CWinImpl *pwin, UINT nID, const IID &iid );
	void DeInit();

	void AddEvent( UINT nDispID, FuncEventHandler );
public:
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

public:
	ConnPtClient		*m_pnext;
	EventEntry			*m_pfirst;
protected:
	CWinImpl			*m_pEvTarget;
	DWORD				m_dwCookie;
	IID					m_iidDisp;
	IConnectionPointPtr	m_ptrConnPt;
	UINT				m_nIDControl;
};

class CAxEventMap
{
public:
	CAxEventMap();
	virtual ~CAxEventMap();
public:
	virtual CWinImpl	*GetTarget() = 0;
	bool Advise( UINT nID, const IID &riid, UINT nEventID, FuncEventHandler pfn );
	void UnAdviseAll();
public:
	ConnPtClient	*m_pfirst;
};


#endif //__ax_events_h__

