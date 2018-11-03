#ifndef __defs_h__
#define __defs_h__

#ifndef IMPLEMENT_UNKNOWN
#define IMPLEMENT_UNKNOWN( class_name, member )	\
STDMETHODIMP_(ULONG) class_name::X##member::AddRef()\
{\
	METHOD_PROLOGUE_EX(class_name, member)\
	return (ULONG)pThis->ExternalAddRef();\
}\
STDMETHODIMP_(ULONG) class_name::X##member::Release()\
{\
	METHOD_PROLOGUE_EX(class_name, member)\
	return (ULONG)pThis->ExternalRelease();	  \
}\
STDMETHODIMP class_name::X##member::QueryInterface(\
	REFIID iid, LPVOID* ppvObj)\
{								\
	METHOD_PROLOGUE_EX(class_name, member)\
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);\
}
#endif //IMPLEMENT_UNKNOWN


#ifndef __MULTYINTERFACE__
#define __MULTYINTERFACE__

#define METHOD_PROLOGUE_BASE(theClass, localClass) \
	theClass* pThis = \
		((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass))); \
		AFX_MANAGE_STATE(pThis->GetModuleState());\
		pThis;

#define IMPLEMENT_UNKNOWN_BASE( class_name, member )	\
STDMETHODIMP_(ULONG) class_name::X##member::AddRef()\
{\
	METHOD_PROLOGUE_BASE(class_name, member)\
	return (ULONG)pThis->AddRef();\
}\
STDMETHODIMP_(ULONG) class_name::X##member::Release()\
{\
	METHOD_PROLOGUE_BASE(class_name, member)\
	return (ULONG)pThis->Release();	  \
}\
STDMETHODIMP class_name::X##member::QueryInterface(\
	REFIID iid, LPVOID* ppvObj)\
{								\
	METHOD_PROLOGUE_BASE(class_name, member)\
	return (HRESULT)pThis->QueryInterface( iid, ppvObj);\
}

#define ENABLE_MULTYINTERFACE()	\
	ULONG __stdcall AddRef(){return ExternalAddRef();}\
	ULONG __stdcall Release(){return ExternalRelease();}\
	HRESULT __stdcall QueryInterface( REFIID iid, LPVOID* ppvObj )\
				{return ExternalQueryInterface( &iid, ppvObj );}\
	virtual AFX_MODULE_STATE *GetModuleState() {return m_pModuleState;}\
	IUnknown *GetOtherComponent( const IID iid ){return _GetOtherComponent( GetControllingUnknown(), iid );}\
	virtual CCmdTarget *GetCmdTarget(){return this;}



#define BEGIN_INTERFACE_PART_EXPORT(localClass, baseClass) \
	public:	\
	class std_dll X##localClass : public baseClass \
	{ \
	public: \
		STDMETHOD_(ULONG, AddRef)(); \
		STDMETHOD_(ULONG, Release)(); \
		STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj); \

#define BEGIN_INTERFACE_PART_EXPORT_DERIVED(localClass, baseClass) \
	public:	\
	class std_dll X##localClass : public X##baseClass 

#endif //__MULTYINTERFACE__


#ifndef com_call
#define	com_call virtual HRESULT __stdcall
#endif //com_call

#ifndef std_dll
#ifdef _COMMON_LIB
#define std_dll /*extern "C"*/ __declspec(dllexport)
#else
#define std_dll /*extern "C" */__declspec(dllimport)
#endif //_COMMON_DLL
#endif //std_dll


#ifndef export_data

#ifdef _COMMON_LIB
//#define	class std_dll class _declspec(dllexport)
#define	export_data __declspec(dllexport)
//#define	export_struct struct _declspec(dllexport)
#else
//#define	class std_dll class _declspec(dllimport)
#define	export_data __declspec(dllimport)
//#define	export_struct struct _declspec(dllimport)
#endif //_COMMON_DLL

#endif //class std_dll



#define declare_interface( _interface, _guid_string )						\
struct __declspec(uuid( _guid_string )) _interface;							\
typedef _com_IIID<_interface, &__uuidof( _interface )>	iiid_##_interface;	\
typedef _com_ptr_t<iiid_##_interface>	sptr##_interface;					\
const IID IID_##_interface	= __uuidof( _interface );						\
_COM_SMARTPTR_TYPEDEF(_interface, __uuidof(_interface));


#ifdef __AFX_H__
#define NORECT	CRect( 0, 0, 0, 0 )
#endif //__AFX_H__

#define PI		3.1415926535897932384626433832795
#define A90		(PI/2)
#define A180	(PI)
#define A270	(PI+PI/2)


#ifndef PROVIDE_GUID_INFO
#define PROVIDE_GUID_INFO( )	\
void GetClassID( CLSID* pClassID)\
{\
	memcpy( pClassID, &guid, sizeof(GUID) );\
}
#endif //PROVIDE_GUID_INFO

#ifndef PROVIDE_GUID_INFO_NULL
#define PROVIDE_GUID_INFO_NULL( )	\
void GetClassID( CLSID* pClassID)\
{\
}
#endif //PROVIDE_GUID_INFO_NULL

//cool!!! guid-keys
#define lo_guid(p)	((__int64*)p)[0]
#define hi_guid(p)	((__int64*)p)[1]
class GuidKey : public GUID
{	
public:
	GuidKey()
	{
		lo_guid(this) = 0;
		hi_guid(this) = 0;
	}
	GuidKey( const GuidKey &g )
	{
		lo_guid(this) = lo_guid( &g );
		hi_guid(this) = hi_guid( &g );
	}
	GuidKey( const GUID &guid )
	{
		__int64	*pl = (__int64*)&guid;
		lo_guid(this) = pl[0];
		hi_guid(this) = pl[1];
	}
	operator GUID&(){return *this;}

	inline bool operator==( const GuidKey &g )
	{	return lo_guid(this) == lo_guid(&g)&&hi_guid(this)==hi_guid(&g);}
	inline bool operator!=( const GuidKey &g )
	{	return lo_guid(this) != lo_guid(&g)||hi_guid(this)!=hi_guid(&g);}
	inline GuidKey &operator=( const GuidKey &g )
	{	lo_guid(this)=lo_guid(&g);	hi_guid(this)=hi_guid(&g);	return *this;}
	inline bool operator==( const GUID &guid )
	{	__int64	*pl = (__int64*)&guid;	return	lo_guid(this) == pl[0] &&hi_guid(this) == pl[1];}
	inline bool operator!=( const GUID &guid )
	{	__int64	*pl = (__int64*)&guid;	return	lo_guid(this) != pl[0] || hi_guid(this) != pl[1];}
	inline GuidKey &operator=( const GUID &guid )
	{	__int64	*pl = (__int64*)&guid;	lo_guid(this) = pl[0];	hi_guid(this) = pl[1];	return *this;}
};

//struct __POSITION {};
typedef struct __POSITION* POSITION;
typedef POSITION TPOS;
typedef LONG_PTR LPOS;


#define INVALID_KEY	GuidKey()

#define _FILE_OPEN_SIZE_ sizeof(OPENFILENAME)

//#define NOGUARD

#endif //__defs_h__
