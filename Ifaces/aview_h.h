

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:42:54 2015
 */
/* Compiler settings for aview.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
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


#ifndef __aview_h_h__
#define __aview_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAView_FWD_DEFINED__
#define __IAView_FWD_DEFINED__
typedef interface IAView IAView;

#endif 	/* __IAView_FWD_DEFINED__ */


#ifndef __AView_FWD_DEFINED__
#define __AView_FWD_DEFINED__

#ifdef __cplusplus
typedef class AView AView;
#else
typedef struct AView AView;
#endif /* __cplusplus */

#endif 	/* __AView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AViewLib_LIBRARY_DEFINED__
#define __AViewLib_LIBRARY_DEFINED__

/* library AViewLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AViewLib;

#ifndef __IAView_INTERFACE_DEFINED__
#define __IAView_INTERFACE_DEFINED__

/* interface IAView */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IAView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E95D6413-7A15-40fe-9A56-F1B7E75D9EB5")
    IAView : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ScrollToClass( 
            /* [in] */ long nClass) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAView * This,
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
        
        HRESULT ( STDMETHODCALLTYPE *ScrollToClass )( 
            IAView * This,
            /* [in] */ long nClass);
        
        END_INTERFACE
    } IAViewVtbl;

    interface IAView
    {
        CONST_VTBL struct IAViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAView_ScrollToClass(This,nClass)	\
    ( (This)->lpVtbl -> ScrollToClass(This,nClass) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAView_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AView;

#ifdef __cplusplus

class DECLSPEC_UUID("129AB27A-6E73-4418-A3D1-CC1FD3985A31")
AView;
#endif
#endif /* __AViewLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


