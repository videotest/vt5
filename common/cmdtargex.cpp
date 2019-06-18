#include "stdafx.h"
#include "cmdtargex.h"
#include "\vt5\AWIN\profiler.h"


/////////////////////////////////////////////////////////////////////////////////////////
CMapInterfaceImpl::CMapInterfaceImpl()
{
	m_benable_map	= true;
	m_binside_hook	= false;
}

/////////////////////////////////////////////////////////////////////////////////////////
CMapInterfaceImpl::~CMapInterfaceImpl()
{
}

LPUNKNOWN CMapInterfaceImpl::get_fast_interface( const GUID* piid )
{
	if( !m_benable_map )	return 0;
	if( m_binside_hook )	return 0;

	long lpos = m_map_interface.find( piid );
	if( lpos )
	{
		interface_info* pii = m_map_interface.get( lpos );
		return pii->punk;
	}		
	m_binside_hook = true;
	IUnknown* punk = raw_get_interface( piid );	
	m_binside_hook = false;
	if( punk )
	{
		interface_info* pii = new interface_info;
		memcpy( &pii->guid, piid, sizeof(GUID) );
		pii->punk = punk;
		m_map_interface.set( pii, &pii->guid );
	}
	return punk;
}


IMPLEMENT_DYNCREATE(CCmdTargetEx, CCmdTarget)

CPtrList	CCmdTargetEx::g_listRuntime;
extern CPtrList	g_listLock;

CCmdTargetEx::CCmdTargetEx()
{
}

CCmdTargetEx::~CCmdTargetEx()
{
	m_benable_map = false;
}


BOOL CCmdTargetEx::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return false;
}


const char *CCmdTargetEx::GetClassName()
{
	CRuntimeClass	*pclass = GetRuntimeClass();
	return pclass->m_lpszClassName;
}


void CCmdTargetEx::Dump()
{
	_OleTraceLeaks();
#ifdef _DEBUG
	POSITION	pos = g_listRuntime.GetHeadPosition();

	while( pos )
	{
		CCmdTargetEx	*pcmd = (CCmdTargetEx*)g_listRuntime.GetNext( pos );

		try{

			int	nRefCount = pcmd->GetControllingUnknown()->AddRef()-1;
			pcmd->GetControllingUnknown()->Release();
		

		CString	str = ::GetObjectName( pcmd->GetControllingUnknown() );

		if( str.IsEmpty() )
			str = "Unnamed object";

		TRACE( "Dump:%s (%s)    \tThe reference count is %d\n", pcmd->GetClassName(), (const char*)str, nRefCount );
		}catch( ... ){}
	}

#endif
}

void CCmdTargetEx::DeInit()
{
}

void CCmdTargetEx::OnFinalRelease()
{
	DeInit();
	delete this;
}


/////////////////////////////////////////////////////////////////////////////////////////
IUnknown* CCmdTargetEx::GetInterfaceHook( const void* p )
{
	return get_fast_interface( (const GUID*)p );
}
/////////////////////////////////////////////////////////////////////////////////////////
IUnknown* CCmdTargetEx::raw_get_interface( const GUID* piid )
{
	return GetInterface( piid );
}


/////////////////////////////////////////////////////////////////////////////////////////
//COleEventSourceImpl
const AFX_EVENTMAP COleEventSourceImpl::eventMap = { NULL, NULL };

const AFX_EVENTMAP* COleEventSourceImpl::GetEventMap() const
{
	return &eventMap;
}

#if _MSC_VER >= 1300
	#ifdef _AFXDLL
	const AFX_EVENTMAP* PASCAL COleEventSourceImpl::GetThisEventMap()
	{
		return &eventMap;
	}
	#endif
#endif


const AFX_EVENTMAP_ENTRY* COleEventSourceImpl::GetEventMapEntry(
		LPCTSTR pszName,
		DISPID* pDispid) const
{
	ASSERT(pszName != NULL);
	ASSERT(pDispid != NULL);

	const AFX_EVENTMAP* pEventMap = GetEventMap();
	const AFX_EVENTMAP_ENTRY* pEntry;
	DISPID dispid = MAKELONG(1, 0);

	while (pEventMap != NULL)
	{
		pEntry = pEventMap->lpEntries;

		// Scan entries in this event map

		if (pEntry != NULL)
		{
			while (pEntry->pszName != NULL)
			{
				if (_tcscmp(pEntry->pszName, pszName) == 0)
				{
					if (pEntry->dispid != DISPID_UNKNOWN)
						dispid = pEntry->dispid;

					*pDispid = dispid;
					return pEntry;
				}

				++pEntry;
				++dispid;
			}
		}

		// If we didn't find it, go to the base class's event map

#if _MSC_VER >= 1300
		pEventMap = pEventMap->pfnGetBaseMap();
#else
		pEventMap = pEventMap->lpBaseEventMap;
#endif
		dispid = MAKELONG(1, HIWORD(dispid)+1);
	}

	// If we reach here, the event isn't supported

	return NULL;
}

void COleEventSourceImpl::FireEventV(DISPID dispid, BYTE* pbParams,
	va_list argList)
{
	COleDispatchDriver driver;

	POSITION pos = m_xEventConnPt.GetStartPosition();
	LPDISPATCH pDispatch;
	while (pos != NULL)
	{
		pDispatch = (LPDISPATCH)m_xEventConnPt.GetNextConnection(pos);
		if( !pDispatch )	continue;
		ASSERT(pDispatch != NULL);
		driver.AttachDispatch(pDispatch, FALSE);
		TRY
			driver.InvokeHelperV(dispid, DISPATCH_METHOD, VT_EMPTY, NULL,
				pbParams, argList);
		END_TRY
		driver.DetachDispatch();
	}
}

void AFX_CDECL COleEventSourceImpl::FireEvent(DISPID dispid, BYTE* pbParams, ...)
{
	va_list argList;
	va_start(argList, pbParams);
	FireEventV(dispid, pbParams, argList);
	va_end(argList);
}

void COleEventSourceImpl::XEventConnPt::OnAdvise(BOOL bAdvise)
{
	_try_nested_base(COleEventSourceImpl, EventConnPt, OnAdvise)
	{
	//pThis->GetCmdTarget()->OnEventAdvise(bAdvise);
	//TRACE("Event conn pt advise\n");
	}
	_catch_nested_noreturn;
}

REFIID COleEventSourceImpl::XEventConnPt::GetIID()
{
	_try_nested_base(COleEventSourceImpl, EventConnPt, GetIID)
	{
	if (pThis->m_piidEvents != NULL)
		return *(pThis->m_piidEvents);
		
	}
	_catch_nested_noreturn;
	return GUID_NULL;
}

LPUNKNOWN COleEventSourceImpl::XEventConnPt::QuerySinkInterface(LPUNKNOWN pUnkSink)
{
	_try_nested_base(COleEventSourceImpl, EventConnPt, QuerySinkInterface)
	{

	// First, QI for control-specific IID; failing that, QI for IDispatch
	LPUNKNOWN pUnkReturn = NULL;
	if (FAILED(pUnkSink->QueryInterface(*(pThis->m_piidEvents),
		reinterpret_cast<void**>(&pUnkReturn))))
	{
		pUnkSink->QueryInterface(IID_IDispatch,
			reinterpret_cast<void**>(&pUnkReturn));
	}
	return pUnkReturn;
	}
	_catch_nested_noreturn;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// COleControl::GetConnectionHook - override the connection map

LPCONNECTIONPOINT COleEventSourceImpl::GetConnectionHook(REFIID iid)
{
	if ((m_piidEvents != NULL) && IsEqualIID(iid, *m_piidEvents))
		return (LPCONNECTIONPOINT)((char*)&m_xEventConnPt +
				offsetof(CConnectionPoint, m_xConnPt));
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// COleControl::GetExtraConnectionPoints - override the connection map

BOOL COleEventSourceImpl::GetExtraConnectionPoints(CPtrArray* pConnPoints)
{
	pConnPoints->Add((char*)&m_xEventConnPt +
		offsetof(CConnectionPoint, m_xConnPt));
	return TRUE;
}

BOOL COleEventSourceImpl::GetDispatchIID(IID* pIID)
{
	if (m_piidPrimary != NULL)
		*pIID = *m_piidPrimary;

	return (m_piidPrimary != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// COleControl::XProvideClassInfo
IMPLEMENT_UNKNOWN_BASE(COleEventSourceImpl, ProvideClassInfo)

COleEventSourceImpl::COleEventSourceImpl()
{
	m_piidPrimary = 0;
	m_piidEvents = 0;
}

COleEventSourceImpl::~COleEventSourceImpl()
{
}

STDMETHODIMP COleEventSourceImpl::XProvideClassInfo::GetClassInfo(
	LPTYPEINFO* ppTypeInfo)
{
	_try_nested_base(COleEventSourceImpl, ProvideClassInfo, GetClassInfo)
	{

	CLSID clsid;
	pThis->GetClassID(&clsid);

	HRESULT hr = pThis->GetCmdTarget()->GetTypeInfoOfGuid(GetUserDefaultLCID(), clsid, ppTypeInfo);

	if( hr )return hr;

	return S_OK;
	}
	_catch_nested;
}

STDMETHODIMP COleEventSourceImpl::XProvideClassInfo::GetGUID(DWORD dwGuidKind,
	GUID* pGUID)
{
	_try_nested_base(COleEventSourceImpl, ProvideClassInfo, GetGUID)
	{
	TRACE("GetGUID in provideclassinfo\n");
	
	if (dwGuidKind == GUIDKIND_DEFAULT_SOURCE_DISP_IID)
	{
		*pGUID = *pThis->m_piidEvents;
		return NOERROR;
	}
	else
	{
		*pGUID = GUID_NULL;
		return E_INVALIDARG;
	}
	}
	_catch_nested;
}
