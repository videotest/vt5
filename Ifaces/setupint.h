#ifndef __setupInt_h__
#define __setupInt_h__


interface ISetup : public IUnknown
{
	virtual HRESULT __stdcall CanRegisterAction( GUID guidInner, BOOL* bCan )  = 0;
};

#define declare_interface__( _interface, _guid_string )						\
struct __declspec(uuid( _guid_string )) _interface;							\
typedef _com_IIID<_interface, &__uuidof( _interface )>	iiid_##_interface;	\
typedef _com_ptr_t<iiid_##_interface>	sptr##_interface;					\
const IID IID_##_interface	= __uuidof( _interface );						\
_COM_SMARTPTR_TYPEDEF(_interface, __uuidof(_interface));

declare_interface__( ISetup, "B2773FDB-2697-48e0-A754-7FBF45910B64" )


#endif __setupInt_h__


