#pragma once

//UNICODE support
#include "unicode.h"


//base define
#ifndef com_call
#define	com_call	virtual HRESULT STDMETHODCALLTYPE
#endif//com_call
#define	com_ref		virtual ULONG STDMETHODCALLTYPE

static const GUID INVALID_GUID = 
{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

#define	zero_guid	INVALID_GUID
#define	zero_clsid	INVALID_GUID

//[ay]
#ifdef assert
#undef assert
#endif //assert

#ifdef verify
#undef verify
#endif //verify

#ifdef trace
#undef trace
#endif //trace


// Diagnostic support
#ifdef _DEBUG
#define trace( string )		OutputDebugString(string);
#define assert(f)																	\
if( !(f) )																			\
{																					\
	_char sz_file[MAX_PATH]; sz_file[0] = 0;										\
	::GetModuleFileName( 0, sz_file, SIZEOF_CHARS(sz_file) );						\
	_char sz_buf[512];	sz_buf[0] = 0;												\
	___sprintf( sz_buf, "assert failed!!!\n\nProgram:%s\nFile:%s\nLine:%d",			\
				sz_file, __FILE__, __LINE__ );										\
	int nres = MessageBox(	0, sz_buf, _T("Error"), 								\
							MB_ICONSTOP|MB_TASKMODAL|MB_ABORTRETRYIGNORE );			\
	if( nres == IDABORT )															\
		exit( -1 );																	\
	else if( nres == IDRETRY )														\
		DebugBreak();																\
}																					\

#define verify(f)			assert(f)
#else// _DEBUG
#define trace( string )		((void)0)
#define assert(f)			((void)0)
#define verify(f)			((void)(f))
#endif

//interface definition
#ifndef declare_interface
#define declare_interface( _interface, _guid_string )									\
struct __declspec(uuid( _guid_string )) _interface;										\
typedef _com_IIID<_interface, &__uuidof( _interface )>	iiid_##_interface;				\
typedef _com_ptr_t<iiid_##_interface>	sptr##_interface;								\
const IID IID_##_interface	= __uuidof( _interface );									\

#endif//declare_interface

//_COM_SMARTPTR_TYPEDEF(_interface, __uuidof(_interface));//<-to process auto list member in IDE

//unknown implement 
#define implement_unknown()																\
	com_ref AddRef()																	\
	{	return _add_ref();	}															\
	com_ref Release()																	\
	{	return _release();	}															\
	com_call QueryInterface( REFIID riid, void **pp )									\
	{	return _query_interface( riid, pp );	}										\


//create implement
#define implement_self_create(class_name)												\
static void* CreateInstance(){															\
	return new class_name;																\
}																						\

//application init / deinit
#define init_app( hInstance )															\
new com_app;																			\
{																						\
	com_app* papp = com_app::GetApp();													\
	if( papp )																			\
		papp->Init( (HINSTANCE)hInstance );												\
}																						\

#define deinit_app()																	\
{																						\
	com_app* papp = com_app::GetApp();													\
	if( papp )																			\
	{																					\
		papp->Deinit();																	\
		papp->Release();																\
	}																					\
}																						\

//object factory
#define add_factory(class_name, object_clsid, object_progid)							\
{																						\
	com_app* papp = com_app::GetApp();													\
	if( papp )																			\
		papp->AddFactory( class_name::CreateInstance, object_clsid, object_progid );	\
}																						\

//perform additional registration
HRESULT leaveDllRegisterServer();
HRESULT leaveDllUnregisterServer();


//com exports
#define implement_app_exports()																		\
STDAPI DllCanUnloadNow()																			\
{																									\
	com_app*	papp = com_app::GetApp();															\
	if( !papp )																						\
		return E_UNEXPECTED;																		\
																									\
	return ( ( papp->GetObjectsCount() == 0 && papp->GetServerLocks() == 0 ) ? S_OK : S_FALSE );	\
}																									\
																									\
STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID FAR* ppv )							\
{																									\
	com_factory* pfactory = new com_factory;														\
																									\
	pfactory->SetCLSID( rclsid );																	\
	HRESULT hres = pfactory->QueryInterface( riid, ppv );											\
	pfactory->Release();																			\
																									\
	return hres;																					\
}																									\
																									\
STDAPI DllRegisterServer()																			\
{																									\
	com_app*	papp = com_app::GetApp();															\
	if( !papp )																						\
		return E_UNEXPECTED;																		\
																									\
	if( !papp->RegisterAll() )																		\
		return SELFREG_E_CLASS;																		\
																									\
	HRESULT hr = leaveDllRegisterServer();															\
	if( hr != S_OK ) return hr;																		\
																									\
	return S_OK;																					\
}																									\
																									\
STDAPI DllUnregisterServer()																		\
{																									\
	com_app*	papp = com_app::GetApp();															\
	if( !papp )																						\
		return E_UNEXPECTED;																		\
																									\
	if( !papp->UnregisterAll() )																	\
		return SELFREG_E_CLASS;																		\
																									\
	HRESULT hr = leaveDllUnregisterServer();														\
	if( hr != S_OK ) return hr;																		\
																									\
	return S_OK;																					\
}

#define implement_app_exports_full()																\
HRESULT leaveDllRegisterServer()	{	return S_OK; }												\
																									\
HRESULT leaveDllUnregisterServer()	{	return S_OK; }																									\

