#ifndef __com_defs_h__
#define __com_defs_h__

#ifndef com_call
#define com_call		virtual HRESULT __stdcall
#endif //com_call

#ifndef com_call_ref
#define com_call_ref	virtual ULONG  __stdcall
#endif //com_call_ref

#define dll_export __declspec( dllexport )

#define route_unknown()																		\
public:																						\
com_call_ref AddRef()									{return _addref();}					\
com_call_ref Release()									{return _release();}				\
com_call QueryInterface( const IID &iid, void **pret )	{return _qi( iid, pret );}


#define dummy_unknown()																		\
public:																						\
com_call_ref AddRef()									{return 1;}							\
com_call_ref Release()									{return 1;}							\
com_call QueryInterface( const IID &iid, void **pret )	{return E_NOINTERFACE;}




//struct __POSITION {};
typedef __POSITION* POSITION;
typedef POSITION TPOS;
typedef LONG_PTR LPOS;

#pragma pack(push, guard_component_info, 1)
struct __GuardComponentInfo
{
	int		nSize;
	TPOS lpos;
	BSTR	bstrName;
	GUID	guid;
	bool	bIsAction;
};
#pragma pack(pop, guard_component_info)

#define ACTION_NAME_PREFIX "VideoTesT5Action."

bool NeedComponentInfo(const CLSID &rclsid, const IID &iid);
HRESULT GetComponentInfo(void ** ppv);

#define implement_com_exports()																\
STDAPI DllCanUnloadNow()																	\
{																							\
	return App::instance() != 0;															\
}																							\
																							\
STDAPI DllGetClassObject( const CLSID &rclsid, const IID &iid, void **ppClassFactory )		\
{																							\
	if (NeedComponentInfo(rclsid, iid)) 													\
		return GetComponentInfo(ppClassFactory);											\
																							\
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();				\
	pfactory->SetCLSID( rclsid );															\
																							\
	HRESULT hResult = 																		\
	pfactory->QueryInterface( iid, ppClassFactory );										\
	pfactory->Release();																	\
																							\
	return hResult;																			\
}																							\
																							\
STDAPI DllRegisterServer()																	\
{																							\
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();				\
	pfactory->RegisterAll( true );															\
	pfactory->Release();																	\
	return 0;																				\
}																							\
																							\
STDAPI DllUnregisterServer()																\
{																							\
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();				\
	pfactory->RegisterAll( false );															\
	pfactory->Release();																	\
	return 0;																				\
}




typedef unsigned int uint;
typedef unsigned int dword;
typedef ULONG  ulong;

/*//classes
class Object
{
public:
	Object()	{}
	~Object()	{}
};*/



#endif //__com_defs_h__