#ifndef __cmdtargeex_h__
#define __cmdtargeex_h__

#include "utils.h"

#include "afxctl.h"
#include "\vt5\awin\misc_map.h"

#define ROUTE_OLE_VIRTUALS()														 \
	HRESULT GetClassID(LPCLSID pclsid)												 \
	{	*pclsid = guid;		return S_OK;	}										 \
	virtual LPCONNECTIONPOINT GetConnectionHook( REFIID iid )						 \
	{	return COleEventSourceImpl::GetConnectionHook( iid );	}					 \
	virtual BOOL GetExtraConnectionPoints( CPtrArray* pConnPoints )					 \
	{	return COleEventSourceImpl::GetExtraConnectionPoints( pConnPoints );	}	 \
	virtual BOOL GetDispatchIID(IID* pIID)											 \
	{	return COleEventSourceImpl::GetDispatchIID( pIID ); }


///////////////////////////////////////////////////////////////////////////////
//interface info cache
struct interface_info
{
	GUID			guid;
	IUnknown*		punk;
};

inline void free_interface_info( void* pdata )
{
	delete (interface_info*)pdata;
}

inline long cmp_interface_guid( const GUID *p1, const GUID *p2 )
{
	return memcmp( p1, p2, sizeof( GUID ) );
}

class std_dll CMapInterfaceImpl
{
public:
	CMapInterfaceImpl();
	virtual ~CMapInterfaceImpl();
	
	virtual IUnknown* raw_get_interface( const GUID* piid ) = 0;
	virtual IUnknown* get_fast_interface( const GUID* piid );	
	_map_t<interface_info*, const GUID*, cmp_interface_guid, free_interface_info>	m_map_interface;
	
	bool	m_benable_map;
	bool	m_binside_hook;
};

class std_dll CCmdTargetEx : public CCmdTarget,
							public CMapInterfaceImpl
{
	DECLARE_DYNCREATE(CCmdTargetEx);
public:
	CCmdTargetEx();
	virtual ~CCmdTargetEx();
public:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	

	virtual void DeInit();
	virtual void OnFinalRelease();

	const char *GetClassName();

	static CPtrList	g_listRuntime;
	static void Dump();

	//cache interface
	virtual IUnknown* GetInterfaceHook( const void* p );
	virtual IUnknown* raw_get_interface( const GUID* piid );

};

//class implements required interfaces for OLE Event mechanism
//simply call FireEvent for use it!

/*derived class should:
1. insert in constructor
	EnableConnections(); 
2. insert two interface parts in interface map
	INTERFACE_PART(xxx, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(xxx, IID_IProvideClassInfo, ProvideClassInfo)
3. add next lines to the class declaration
	DECLARE_EVENT_MAP()
	DECLARE_OLETYPELIB(xxx)
4.	add definition of type library clsid (IID_ITypeLibID)to the main file (different for debug and release versions); 
	check its in odl file
5. insert event maps
enum {
	//{{AFX_DISP_ID(xxx)
	//}}AFX_DISP_ID
	};
	//{{AFX_EVENT(xxx)
	//}}AFX_EVENT
6. implement oel typelib
	IMPLEMENT_OLETYPELIB(CAButtonDispatch, IID_ITypeLibID, 1, 0)
7. generate two guid - 
	m_piidEvents = &IID_IAButtonEvents;
	m_piidPrimary = &IID_IAButtonDispatch;
	assign it in constructor of object
8. insert event map
BEGIN_EVENT_MAP(xxx, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(xxx)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()
9. Add to .odl file  disp
	#ifndef _DEBUG
	[ uuid(xxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx) ]
	#else
	[ uuid(xxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx) ]
	#endif //_DEBUG
	dispinterface IxxxxxxxEvents
	{
		properties:
		methods:
			// NOTE - ClassWizard will maintain event information here.
			//    Use extreme caution when editing this section.
			//{{AFX_ODL_EVENT(xxxxx)
			//}}AFX_ODL_EVENT
	};

	coclass Xxxxxxx
	{
		...
		[default, source] dispinterface IxxxxxxxEvents;
	}
10. Add ROUTE_OLE_VIRTUALS() macros to the class definition
11. Inherite class from COleEventSourceImpl implementation
12. Change project properties for build .tlb into .exe (.dll) directory
13. Remove the .clw file and re-create it
14. Make sure INTERFACE_PART(xxxx, IID_Ixxxx, Dispatch) has a correct iid (primary)
15. Add AfxOleRegisterTypeLib(AfxGetInstanceHandle(), IID_ITypeLibID, _T("project name.tlb") );
	and AfxOleUnregisterTypeLib( IID_ITypeLibID ); to the DllRegisterServer and DllUnregisterServer
*/

class std_dll COleEventSourceImpl : public CImplBase
{
public:
	COleEventSourceImpl();           
	virtual ~COleEventSourceImpl();
//operations
public:
	void AFX_CDECL FireEvent(DISPID dispid, BYTE* pbParams, ...);
//implementation
protected:
	virtual const AFX_EVENTMAP* GetEventMap() const;
#if _MSC_VER >= 1300
	#ifdef _AFXDLL
		static const AFX_EVENTMAP* PASCAL GetThisEventMap();
	#endif
#endif
	const AFX_EVENTMAP_ENTRY* GetEventMapEntry(LPCTSTR pszName,	DISPID* pDispid) const;
	void FireEventV(DISPID dispid, BYTE* pbParams, va_list argList);

	// Connection point container
	virtual LPCONNECTIONPOINT GetConnectionHook(REFIID iid);
	virtual BOOL GetExtraConnectionPoints(CPtrArray* pConnPoints);
//COM interfaceced
	// Connection point for events - from COleControl
	class std_dll XEventConnPt : public CConnectionPoint 
	{
	public:
		XEventConnPt()
		{ 
			#if _MSC_VER >= 1300			
			m_nOffset = 0;
			#else
			m_nOffset = offsetof(COleEventSourceImpl, m_xEventConnPt);
			#endif
		}
		virtual void OnAdvise(BOOL bAdvise);
		virtual REFIID GetIID();
		virtual LPUNKNOWN QuerySinkInterface(LPUNKNOWN pUnkSink);
	END_CONNECTION_PART(EventConnPt)

		// IProvideClassInfo2
	BEGIN_INTERFACE_PART_EXPORT(ProvideClassInfo, IProvideClassInfo2)
		INIT_INTERFACE_PART(COleControl, ProvideClassInfo)
		STDMETHOD(GetClassInfo)(LPTYPEINFO* ppTypeInfo);
		STDMETHOD(GetGUID)(DWORD dwGuidKind, GUID* pGUID);
	END_INTERFACE_PART(ProvideClassInfo)
protected:
	static const AFX_DATA AFX_EVENTMAP_ENTRY _eventEntries[];
	static const AFX_DATA AFX_EVENTMAP eventMap;

public:
	virtual HRESULT GetClassID(LPCLSID pclsid) = 0;
	virtual BOOL GetDispatchIID(IID* pIID);

	const IID* m_piidEvents;            // IID for control events
	const IID* m_piidPrimary;			  // IID for primary idispatch
};

#endif //__cmdtargeex_h__