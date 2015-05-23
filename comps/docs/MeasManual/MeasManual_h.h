

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:22 2015
 */
/* Compiler settings for MeasManual.odl:
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


#ifndef __MeasManual_h_h__
#define __MeasManual_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOleEventProviderDisp_FWD_DEFINED__
#define __IOleEventProviderDisp_FWD_DEFINED__
typedef interface IOleEventProviderDisp IOleEventProviderDisp;

#endif 	/* __IOleEventProviderDisp_FWD_DEFINED__ */


#ifndef __IOleEventProviderEvents_FWD_DEFINED__
#define __IOleEventProviderEvents_FWD_DEFINED__
typedef interface IOleEventProviderEvents IOleEventProviderEvents;

#endif 	/* __IOleEventProviderEvents_FWD_DEFINED__ */


#ifndef __OleEventProvider_FWD_DEFINED__
#define __OleEventProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class OleEventProvider OleEventProvider;
#else
typedef struct OleEventProvider OleEventProvider;
#endif /* __cplusplus */

#endif 	/* __OleEventProvider_FWD_DEFINED__ */


#ifndef __IMeasManager_FWD_DEFINED__
#define __IMeasManager_FWD_DEFINED__
typedef interface IMeasManager IMeasManager;

#endif 	/* __IMeasManager_FWD_DEFINED__ */


#ifndef __MeasManager_FWD_DEFINED__
#define __MeasManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class MeasManager MeasManager;
#else
typedef struct MeasManager MeasManager;
#endif /* __cplusplus */

#endif 	/* __MeasManager_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MeasManual_LIBRARY_DEFINED__
#define __MeasManual_LIBRARY_DEFINED__

/* library MeasManual */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_MeasManual,0xF5B36DD2,0xE81C,0x4334,0x98,0xFB,0xFD,0x29,0x81,0x68,0x95,0x05);

#ifndef __IOleEventProviderDisp_DISPINTERFACE_DEFINED__
#define __IOleEventProviderDisp_DISPINTERFACE_DEFINED__

/* dispinterface IOleEventProviderDisp */
/* [uuid] */ 


DEFINE_GUID(DIID_IOleEventProviderDisp,0x4010031B,0x2B07,0x4721,0x84,0xD8,0x58,0x4D,0x76,0x56,0xF3,0xDF);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("4010031B-2B07-4721-84D8-584D7656F3DF")
    IOleEventProviderDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOleEventProviderDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleEventProviderDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleEventProviderDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleEventProviderDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOleEventProviderDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOleEventProviderDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOleEventProviderDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOleEventProviderDisp * This,
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
    } IOleEventProviderDispVtbl;

    interface IOleEventProviderDisp
    {
        CONST_VTBL struct IOleEventProviderDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleEventProviderDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOleEventProviderDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOleEventProviderDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOleEventProviderDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOleEventProviderDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOleEventProviderDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOleEventProviderDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOleEventProviderDisp_DISPINTERFACE_DEFINED__ */


#ifndef __IOleEventProviderEvents_DISPINTERFACE_DEFINED__
#define __IOleEventProviderEvents_DISPINTERFACE_DEFINED__

/* dispinterface IOleEventProviderEvents */
/* [uuid] */ 


DEFINE_GUID(DIID_IOleEventProviderEvents,0xFB940961,0xF89A,0x4807,0xAC,0x94,0x00,0x2A,0xC0,0x88,0xDC,0x48);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FB940961-F89A-4807-AC94-002AC088DC48")
    IOleEventProviderEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOleEventProviderEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleEventProviderEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleEventProviderEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleEventProviderEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOleEventProviderEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOleEventProviderEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOleEventProviderEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOleEventProviderEvents * This,
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
    } IOleEventProviderEventsVtbl;

    interface IOleEventProviderEvents
    {
        CONST_VTBL struct IOleEventProviderEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleEventProviderEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOleEventProviderEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOleEventProviderEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOleEventProviderEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOleEventProviderEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOleEventProviderEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOleEventProviderEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOleEventProviderEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_OleEventProvider,0x43B1743F,0xB5C4,0x4c68,0xB5,0xBA,0xD2,0x1E,0x9F,0x2A,0x6D,0x00);

#ifdef __cplusplus

class DECLSPEC_UUID("43B1743F-B5C4-4c68-B5BA-D21E9F2A6D00")
OleEventProvider;
#endif

#ifndef __IMeasManager_DISPINTERFACE_DEFINED__
#define __IMeasManager_DISPINTERFACE_DEFINED__

/* dispinterface IMeasManager */
/* [uuid] */ 


DEFINE_GUID(DIID_IMeasManager,0x8DBC9EE3,0xA4EA,0x4FA8,0xBA,0x47,0x08,0xEE,0x47,0x93,0x2C,0xA8);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8DBC9EE3-A4EA-4FA8-BA47-08EE47932CA8")
    IMeasManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMeasManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMeasManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMeasManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMeasManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMeasManager * This,
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
    } IMeasManagerVtbl;

    interface IMeasManager
    {
        CONST_VTBL struct IMeasManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMeasManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMeasManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMeasManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMeasManager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MeasManager,0x986021C1,0x0B2A,0x4C8C,0x84,0x3E,0x7F,0xF6,0x86,0x0B,0x4B,0xD3);

#ifdef __cplusplus

class DECLSPEC_UUID("986021C1-0B2A-4C8C-843E-7FF6860B4BD3")
MeasManager;
#endif
#endif /* __MeasManual_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


