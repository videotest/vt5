#include "stdafx.h"
#include "ax_events.h"

ConnPtClient::ConnPtClient()
{
	m_pEvTarget = 0;
	m_pnext = 0;
	m_pfirst = 0;
	m_pEvTarget = 0;
	m_dwCookie = (DWORD)-1;
	m_iidDisp = GUID_NULL;
}

bool ConnPtClient::Init( CWinImpl *pwin, UINT nID, const IID &riid )
{
	m_pEvTarget = pwin;
	HWND	hwndControl = ::GetDlgItem( pwin->hwnd(), nID );
	IUnknown	*punk = (IUnknown	*)::SendMessage( hwndControl, WM_GETCONTROL, 0, 0 );
	if( !punk )return false;

	IConnectionPointContainerPtr	ptrConnPtCont( punk );
	m_ptrConnPt = punk;

	if( ptrConnPtCont == 0 )
		return false;

	if( ptrConnPtCont->FindConnectionPoint( riid, &m_ptrConnPt ) != S_OK )
		return false;

	if( m_ptrConnPt == 0 )
		return false;

	if( m_ptrConnPt->Advise( Unknown(), &m_dwCookie ) != S_OK )
		return false;

	return true;
}

void ConnPtClient::DeInit()
{
	if( m_ptrConnPt != 0 && m_dwCookie != (DWORD)-1 )
		m_ptrConnPt->Unadvise( m_dwCookie );
	m_ptrConnPt = 0;
	m_dwCookie = (DWORD)-1;

	while( m_pfirst )
	{
		EventEntry	*pcnt = m_pfirst->m_pnext;
		delete m_pfirst;
		m_pfirst = pcnt;
	}
}

ConnPtClient::~ConnPtClient()
{
	DeInit();
}

void ConnPtClient::AddEvent( UINT nDispID, FuncEventHandler	pfnEvent )
{
	EventEntry	*pe = new EventEntry;
	pe->m_pnext = m_pfirst;
	m_pfirst = pe;
	pe->nDispID = nDispID;
	pe->pfnEvent = pfnEvent;
}

IUnknown *ConnPtClient::DoGetInterface( const IID &iid )
{
	if( iid == IID_IDispatch )return (IDispatch*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT ConnPtClient::GetTypeInfoCount( 
    /* [out] */ UINT *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT ConnPtClient::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT ConnPtClient::GetIDsOfNames( 
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID *rgDispId)
{
	return E_NOTIMPL;
}

HRESULT ConnPtClient::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{
	EventEntry			*pe = m_pfirst;

	while( pe )
	{
		if( pe->nDispID == dispIdMember )
		{
			(m_pEvTarget->*pe->pfnEvent)();
			return S_OK;
		}
		pe = pe->m_pnext;
	}
	return DISP_E_MEMBERNOTFOUND;
	
}


/////////////////////////////////////////////////////////////////////////////////////////
//CAxEventMap
CAxEventMap::CAxEventMap()
{
	m_pfirst = 0;
}

CAxEventMap::~CAxEventMap()
{
	UnAdviseAll();
}

bool CAxEventMap::Advise( UINT nID, const IID &riid, UINT nEventID, FuncEventHandler pfn )
{
	ConnPtClient	*pcnt = m_pfirst;
	ConnPtClient	*pcntFound = 0;

	while( pcnt )
	{
		if( pcnt->iid() == riid && pcnt->id() == nID )
		{
			pcntFound = pcnt;
			break;
		}
		pcnt = pcnt->m_pnext;
	}

	if( !pcntFound )
	{
		pcntFound = new ConnPtClient();

		if( !pcntFound->Init( GetTarget(), nID, riid ) )
		{
			pcntFound->Release();
			return false;
		}

		pcntFound->m_pnext = m_pfirst;
		m_pfirst = pcntFound;
	}

	pcntFound->AddEvent( nEventID, pfn );

	return true;
}

void CAxEventMap::UnAdviseAll()
{
	while( m_pfirst )
	{
		ConnPtClient	*pcnt = m_pfirst->m_pnext;
		m_pfirst->DeInit();
		dbg_assert( m_pfirst->Release() == 0 );
		m_pfirst = pcnt;
	}
}