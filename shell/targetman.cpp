#include "stdafx.h"
#include "shell.h"
#include "targetman.h"


//XTargetEntry
CTargetManager::XTargetEntry::XTargetEntry( const char *sz )
{
	m_strTypeName = sz;
}
CTargetManager::XTargetEntry::~XTargetEntry()
{
	POSITION pos = GetFirstTargetPosition();

	while( pos )
	{
		XUnk	*pxunk = GetNextTarget( pos );
		pxunk->punk->Release();
		delete pxunk;
	}
}

//return the active (head-pos)target, if exist
IUnknown *CTargetManager::XTargetEntry::GetActiveTarget()
{
	POSITION	pos = GetFirstTargetPosition();
	if( !pos )return 0;
	XUnk	*pxunk = GetNextTarget( pos );
	return pxunk->bEnable?pxunk->punk:0;
}

//walking target list
POSITION CTargetManager::XTargetEntry::GetFirstTargetPosition()
{
	return m_listTargets.GetHeadPosition();
}

//return IUnknown without AddRef
XUnk *CTargetManager::XTargetEntry::GetNextTarget( POSITION &pos )
{
	return (XUnk*)m_listTargets.GetNext( pos );
}

//find the specified terget in target list
POSITION CTargetManager::XTargetEntry::FindTargetPos( IUnknown *punk )
{
	POSITION	pos = GetFirstTargetPosition();

	while( pos )
	{
		POSITION	posPrev = pos;
		XUnk	*pxunk = GetNextTarget( pos );
		IUnknown *punkTest = pxunk->punk;
		if( ::GetObjectKey( punkTest ) == ::GetObjectKey( punk ) )
			return posPrev;
	}
	return 0;
}

bool CTargetManager::XTargetEntry::AddTarget( IUnknown *punk )
{
	ASSERT( !FindTargetPos( punk ) );
	XUnk	*pxunk = new XUnk;
	pxunk->bEnable = true;
	pxunk->punk = punk;
	pxunk->punk->AddRef();

	m_listTargets.AddHead( pxunk );
	return true;
}

bool CTargetManager::XTargetEntry::RemoveTarget( IUnknown *punk )
{
	POSITION	pos = FindTargetPos( punk );
	if( !pos )return false;

	XUnk	*pxunk = (XUnk*)m_listTargets.GetAt( pos );
	m_listTargets.RemoveAt( pos );
	pxunk->punk->Release();
	delete pxunk;

	return true;
}

bool CTargetManager::XTargetEntry::EnableTarget( IUnknown *punk, bool bEnable )
{
	POSITION	pos = FindTargetPos( punk );
	if( !pos )return false;

	XUnk	*pxunk = (XUnk*)m_listTargets.GetAt( pos );
	if( pxunk->bEnable == bEnable )return false;

	if( !bEnable )
	{
		m_listTargets.RemoveAt( pos );
		m_listTargets.AddTail( pxunk );
	}
	pxunk->bEnable = bEnable;
	return true;
}

bool CTargetManager::XTargetEntry::ActivateTarget( IUnknown *punk )
{
	POSITION	pos = FindTargetPos( punk );
	if( !pos )return false;

	XUnk	*pxunk = (XUnk*)m_listTargets.GetAt( pos );

	if( pos == m_listTargets.GetHeadPosition() && pxunk->bEnable )
		return false;

	m_listTargets.RemoveAt( pos );
	pxunk->bEnable = true;
	m_listTargets.AddHead( pxunk );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//some globals
CTargetManager	g_TargetManager;

/////////////////////////////////////////////////////////////////////////////////////////
//CTargetManager implementation
CTargetManager::CTargetManager()
{
}

CTargetManager::~CTargetManager()
{
}

//empty the current target state and register "app" terget
void CTargetManager::Init()
{
	DeInit();
	RegisterTarget( theApp.GetControllingUnknown(), szTargetApplication );
}

//de-initialize tagert manager and free memory
void CTargetManager::DeInit()
{
	POSITION pos = GetFisrtTargetEntryPosition();

	while( pos )
	{
		XTargetEntry *pe = GetNextTargetEntry( pos );
		delete pe;
	}

	m_listTargetEntries.RemoveAll();
	m_mapTargetCache.RemoveAll();
}

POSITION CTargetManager::GetFisrtTargetEntryPosition()
{
	return m_listTargetEntries.GetHeadPosition();
}

CTargetManager::XTargetEntry* 
CTargetManager::GetNextTargetEntry( POSITION &pos )
{
	return (XTargetEntry *)m_listTargetEntries.GetNext( pos );
}

CTargetManager::XTargetEntry* 
CTargetManager::GetTargetEntry( const char *szTargetName )
{
	POSITION	pos = GetFisrtTargetEntryPosition();

	while( pos )
	{
		XTargetEntry	*pe = GetNextTargetEntry( pos );
		if( !strcmp( pe->GetName(), szTargetName ) )
			return pe;
	}

	XTargetEntry	*pe = new XTargetEntry( szTargetName );

	m_listTargetEntries.AddTail( pe );

	return pe;
}

void CTargetManager::ActivateTarget( IUnknown *punk )
{
	if( !punk )
		return;

	bool	bChanged = false;
	m_mapTargetCache.RemoveAll();

	POSITION pos = GetFisrtTargetEntryPosition();

	while( pos )
	{
		XTargetEntry *pe = GetNextTargetEntry( pos );
		bChanged |= pe->ActivateTarget( punk );
	}

	if( bChanged )NotifyApplication( punk, tncActivate );
}

IUnknown *CTargetManager::GetTarget( const char *szName )
{
	IUnknown *punk = 0;

	void	*pvalRet = 0;
	if( !m_mapTargetCache.Lookup( szName, pvalRet ) )
	{
		XTargetEntry *pe = GetTargetEntry( szName );
		punk = pe->GetActiveTarget();
		m_mapTargetCache[szName] = punk;
	}
	else
		punk = (IUnknown*)pvalRet;

	if( punk )punk->AddRef();
	return punk;
}

IUnknown *CTargetManager::GetTarget( GuidKey nKey )
{
	POSITION	pos = GetFisrtTargetEntryPosition();				//TargetEntry list navigation

	while( pos )
	{
		XTargetEntry	*pe = GetNextTargetEntry( pos );
		POSITION posVE = pe->GetFirstTargetPosition();

		while( posVE )
		{
			XUnk	*pxunk = pe->GetNextTarget( posVE );
			IUnknown *punk = pxunk->punk;

			if( ::GetObjectKey( punk ) == nKey )
			{
				punk->AddRef();
				return punk;
			}
		}
	}
	return 0;
}

void CTargetManager::RegisterTarget( IUnknown *punk, const char *szTargName )
{
	CString	strTargets( szTargName );
	bool	bChanged = false;

	int idx;
	do
	{
		CString	strTarget;
		idx = strTargets.Find( "\n" );

		if( idx == -1 )
			strTarget = strTargets;
		else
		{
			strTarget = strTargets.Left( idx );
			strTargets = strTargets.Right( strTargets.GetLength()-idx-1 );
		}

		XTargetEntry *pe = GetTargetEntry( strTarget );
		bChanged |= pe->AddTarget( punk );
	}
	while( idx != -1 );

	if( bChanged )NotifyApplication( punk, tncAdd );
}

void CTargetManager::UnRegisterTarget( IUnknown *punk )
{
	POSITION pos = GetFisrtTargetEntryPosition();
	bool bChange = false;

	while( pos )
	{
		XTargetEntry *pe = GetNextTargetEntry( pos );
		bChange |= pe->RemoveTarget( punk );
	}
	m_mapTargetCache.RemoveAll();
	if( bChange )NotifyApplication( punk, tncRemove );
}

void CTargetManager::EnableTarget( IUnknown *punk, bool bEnable )
{
	POSITION pos = GetFisrtTargetEntryPosition();
	bool	bChange = false;

	while( pos )
	{
		XTargetEntry *pe = GetNextTargetEntry( pos );
		bChange |= pe->EnableTarget( punk, bEnable );
	}
	m_mapTargetCache.RemoveAll();
	if( bChange )NotifyApplication( punk, tncActivate );
}


BEGIN_INTERFACE_MAP(CTargetManager, CCmdTargetEx)
    INTERFACE_PART(CTargetManager, IID_IActionTargetManager, Target)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CTargetManager, Target);


HRESULT CTargetManager::XTarget::RegisterTarget( IUnknown *punkTarget, BSTR bstr )
{
	_try_nested(CTargetManager, Target, RegisterTarget)
	{
		CString	str( bstr );
		pThis->RegisterTarget( punkTarget, str );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CTargetManager::XTarget::UnRegisterTarget( IUnknown *punkTarget )
{
	_try_nested(CTargetManager, Target, UnRegisterTarget)
	{
		pThis->UnRegisterTarget( punkTarget );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CTargetManager::XTarget::ActivateTarget( IUnknown *punkTarget )
{
	_try_nested(CTargetManager, Target, ActivateTarget)
	{
		pThis->ActivateTarget( punkTarget );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CTargetManager::XTarget::GetTarget( BSTR bstr, IUnknown **punkTarget )
{
	//_try_nested(CTargetManager, Target, GetTarget)
	METHOD_PROLOGUE_EX(CTargetManager, Target)
	{
		CString	str( bstr );

		*punkTarget = pThis->GetTarget( str );

		return S_OK;
	}
	//_catch_nested;
}

HRESULT CTargetManager::XTarget::GetTargetByKey( GUID lKey, IUnknown **ppunk )
{
	//_try_nested(CTargetManager, Target, GetTarget)
	METHOD_PROLOGUE_EX(CTargetManager, Target)
	{
		*ppunk = pThis->GetTarget( lKey );
		return S_OK;
	}
	//_catch_nested;
}

void CTargetManager::NotifyApplication( IUnknown *punk, int nCode )
{
	::FireEventNotify( GetAppUnknown(), szActiveTargetChanged, punk, 0, nCode );
}