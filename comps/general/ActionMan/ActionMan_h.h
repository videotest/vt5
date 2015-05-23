

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:11 2015
 */
/* Compiler settings for ActionMan.odl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __ActionMan_h_h__
#define __ActionMan_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IActionLogWindow_FWD_DEFINED__
#define __IActionLogWindow_FWD_DEFINED__
typedef interface IActionLogWindow IActionLogWindow;

#endif 	/* __IActionLogWindow_FWD_DEFINED__ */


#ifndef __ActionLogWindow_FWD_DEFINED__
#define __ActionLogWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActionLogWindow ActionLogWindow;
#else
typedef struct ActionLogWindow ActionLogWindow;
#endif /* __cplusplus */

#endif 	/* __ActionLogWindow_FWD_DEFINED__ */


#ifndef __IActionStateEvents_FWD_DEFINED__
#define __IActionStateEvents_FWD_DEFINED__
typedef interface IActionStateEvents IActionStateEvents;

#endif 	/* __IActionStateEvents_FWD_DEFINED__ */


#ifndef __IActionState_FWD_DEFINED__
#define __IActionState_FWD_DEFINED__
typedef interface IActionState IActionState;

#endif 	/* __IActionState_FWD_DEFINED__ */


#ifndef __ActionState_FWD_DEFINED__
#define __ActionState_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActionState ActionState;
#else
typedef struct ActionState ActionState;
#endif /* __cplusplus */

#endif 	/* __ActionState_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ActionMan_LIBRARY_DEFINED__
#define __ActionMan_LIBRARY_DEFINED__

/* library ActionMan */
/* [version][uuid] */ 

typedef 
enum __MIDL___MIDL_itf_ActionMan_0000_0000_0001
    {
        nfAction	= 1,
        nfDocument	= 2,
        nfCurrentDocOnly	= 4
    } 	NotifyLevel;


DEFINE_GUID(LIBID_ActionMan,0x8069CA46,0x02B2,0x11D3,0xA5,0xA0,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifndef __IActionLogWindow_DISPINTERFACE_DEFINED__
#define __IActionLogWindow_DISPINTERFACE_DEFINED__

/* dispinterface IActionLogWindow */
/* [uuid] */ 


DEFINE_GUID(DIID_IActionLogWindow,0xA9386D07,0x221B,0x11D3,0xA5,0xD6,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A9386D07-221B-11D3-A5D6-0000B493FF1B")
    IActionLogWindow : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IActionLogWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionLogWindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionLogWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionLogWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActionLogWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActionLogWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActionLogWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActionLogWindow * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IActionLogWindowVtbl;

    interface IActionLogWindow
    {
        CONST_VTBL struct IActionLogWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionLogWindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActionLogWindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActionLogWindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActionLogWindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActionLogWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActionLogWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActionLogWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IActionLogWindow_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ActionLogWindow,0xA9386D08,0x221B,0x11D3,0xA5,0xD6,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("A9386D08-221B-11D3-A5D6-0000B493FF1B")
ActionLogWindow;
#endif

#ifndef __IActionStateEvents_DISPINTERFACE_DEFINED__
#define __IActionStateEvents_DISPINTERFACE_DEFINED__

/* dispinterface IActionStateEvents */
/* [uuid] */ 


DEFINE_GUID(DIID_IActionStateEvents,0x080118ED,0x98CC,0x47b6,0xB1,0xA2,0x7D,0x58,0x5C,0x69,0x8F,0xAA);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("080118ED-98CC-47b6-B1A2-7D585C698FAA")
    IActionStateEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IActionStateEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionStateEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionStateEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionStateEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActionStateEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActionStateEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActionStateEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActionStateEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IActionStateEventsVtbl;

    interface IActionStateEvents
    {
        CONST_VTBL struct IActionStateEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionStateEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActionStateEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActionStateEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActionStateEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActionStateEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActionStateEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActionStateEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IActionStateEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IActionState_DISPINTERFACE_DEFINED__
#define __IActionState_DISPINTERFACE_DEFINED__

/* dispinterface IActionState */
/* [uuid] */ 


DEFINE_GUID(DIID_IActionState,0xC416B1E1,0x27D9,0x49A9,0x99,0x5C,0xD2,0x98,0x68,0x71,0xC3,0x44);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C416B1E1-27D9-49A9-995C-D2986871C344")
    IActionState : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IActionStateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionState * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionState * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionState * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActionState * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActionState * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActionState * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActionState * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IActionStateVtbl;

    interface IActionState
    {
        CONST_VTBL struct IActionStateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionState_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActionState_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActionState_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActionState_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActionState_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActionState_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActionState_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IActionState_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ActionState,0x538D4938,0xAD2A,0x4D4B,0xBA,0x2F,0x37,0x47,0x7D,0xA9,0xFA,0x3F);

#ifdef __cplusplus

class DECLSPEC_UUID("538D4938-AD2A-4D4B-BA2F-37477DA9FA3F")
ActionState;
#endif
#endif /* __ActionMan_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


