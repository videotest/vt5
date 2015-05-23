

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:19 2015
 */
/* Compiler settings for AppMan.odl:
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


#ifndef __AppMan_h_h__
#define __AppMan_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAppWindow_FWD_DEFINED__
#define __IAppWindow_FWD_DEFINED__
typedef interface IAppWindow IAppWindow;

#endif 	/* __IAppWindow_FWD_DEFINED__ */


#ifndef __AppWindow_FWD_DEFINED__
#define __AppWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppWindow AppWindow;
#else
typedef struct AppWindow AppWindow;
#endif /* __cplusplus */

#endif 	/* __AppWindow_FWD_DEFINED__ */


#ifndef __IAppManager_FWD_DEFINED__
#define __IAppManager_FWD_DEFINED__
typedef interface IAppManager IAppManager;

#endif 	/* __IAppManager_FWD_DEFINED__ */


#ifndef __IAppManagerEvents_FWD_DEFINED__
#define __IAppManagerEvents_FWD_DEFINED__
typedef interface IAppManagerEvents IAppManagerEvents;

#endif 	/* __IAppManagerEvents_FWD_DEFINED__ */


#ifndef __AppManager_FWD_DEFINED__
#define __AppManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppManager AppManager;
#else
typedef struct AppManager AppManager;
#endif /* __cplusplus */

#endif 	/* __AppManager_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AppMan_LIBRARY_DEFINED__
#define __AppMan_LIBRARY_DEFINED__

/* library AppMan */
/* [version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_AppMan,0x53665021,0x669E,0x11d3,0xA6,0x4B,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifndef __IAppWindow_DISPINTERFACE_DEFINED__
#define __IAppWindow_DISPINTERFACE_DEFINED__

/* dispinterface IAppWindow */
/* [uuid] */ 


DEFINE_GUID(DIID_IAppWindow,0xE790124D,0x3A7D,0x11D3,0xA6,0x04,0x00,0x90,0x27,0x59,0x95,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E790124D-3A7D-11D3-A604-0090275995FE")
    IAppWindow : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAppWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppWindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAppWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAppWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAppWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAppWindow * This,
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
    } IAppWindowVtbl;

    interface IAppWindow
    {
        CONST_VTBL struct IAppWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppWindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppWindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppWindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppWindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAppWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAppWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAppWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAppWindow_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AppWindow,0xFD981A72,0x3ACE,0x11d3,0xA6,0x04,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifdef __cplusplus

class DECLSPEC_UUID("FD981A72-3ACE-11d3-A604-0090275995FE")
AppWindow;
#endif

#ifndef __IAppManager_DISPINTERFACE_DEFINED__
#define __IAppManager_DISPINTERFACE_DEFINED__

/* dispinterface IAppManager */
/* [uuid] */ 


DEFINE_GUID(DIID_IAppManager,0xB91F7CC6,0x3B46,0x11D3,0xA6,0x05,0x00,0x90,0x27,0x59,0x95,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B91F7CC6-3B46-11D3-A605-0090275995FE")
    IAppManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAppManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAppManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAppManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAppManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAppManager * This,
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
    } IAppManagerVtbl;

    interface IAppManager
    {
        CONST_VTBL struct IAppManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAppManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAppManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAppManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAppManager_DISPINTERFACE_DEFINED__ */


#ifndef __IAppManagerEvents_DISPINTERFACE_DEFINED__
#define __IAppManagerEvents_DISPINTERFACE_DEFINED__

/* dispinterface IAppManagerEvents */
/* [uuid] */ 


DEFINE_GUID(DIID_IAppManagerEvents,0x3A752EF1,0x668C,0x11d3,0xA6,0x4B,0x00,0x90,0x27,0x59,0x95,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3A752EF1-668C-11d3-A64B-0090275995FE")
    IAppManagerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAppManagerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppManagerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppManagerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAppManagerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAppManagerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAppManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAppManagerEvents * This,
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
    } IAppManagerEventsVtbl;

    interface IAppManagerEvents
    {
        CONST_VTBL struct IAppManagerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppManagerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppManagerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppManagerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppManagerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAppManagerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAppManagerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAppManagerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAppManagerEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AppManager,0xEFABC941,0x3B63,0x11d3,0xA6,0x05,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifdef __cplusplus

class DECLSPEC_UUID("EFABC941-3B63-11d3-A605-0090275995FE")
AppManager;
#endif
#endif /* __AppMan_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


