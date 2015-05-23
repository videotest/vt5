

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:26 2015
 */
/* Compiler settings for DataUI.odl:
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


#ifndef __DataUI_h_h__
#define __DataUI_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Idata_context_ctrl_FWD_DEFINED__
#define __Idata_context_ctrl_FWD_DEFINED__
typedef interface Idata_context_ctrl Idata_context_ctrl;

#endif 	/* __Idata_context_ctrl_FWD_DEFINED__ */


#ifndef __qq_FWD_DEFINED__
#define __qq_FWD_DEFINED__

#ifdef __cplusplus
typedef class qq qq;
#else
typedef struct qq qq;
#endif /* __cplusplus */

#endif 	/* __qq_FWD_DEFINED__ */


#ifndef __Iimage_bar_FWD_DEFINED__
#define __Iimage_bar_FWD_DEFINED__
typedef interface Iimage_bar Iimage_bar;

#endif 	/* __Iimage_bar_FWD_DEFINED__ */


#ifndef __image_bar_FWD_DEFINED__
#define __image_bar_FWD_DEFINED__

#ifdef __cplusplus
typedef class image_bar image_bar;
#else
typedef struct image_bar image_bar;
#endif /* __cplusplus */

#endif 	/* __image_bar_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __DataUI_LIBRARY_DEFINED__
#define __DataUI_LIBRARY_DEFINED__

/* library DataUI */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_DataUI,0xFF424E72,0x6659,0x4201,0x84,0x48,0x01,0xF5,0x90,0xE0,0x62,0x87);

#ifndef __Idata_context_ctrl_DISPINTERFACE_DEFINED__
#define __Idata_context_ctrl_DISPINTERFACE_DEFINED__

/* dispinterface Idata_context_ctrl */
/* [uuid] */ 


DEFINE_GUID(DIID_Idata_context_ctrl,0x0A93A964,0xCCFE,0x4E2D,0x8E,0x6A,0x45,0xFD,0xB9,0x3E,0x18,0xA6);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0A93A964-CCFE-4E2D-8E6A-45FDB93E18A6")
    Idata_context_ctrl : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct Idata_context_ctrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Idata_context_ctrl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Idata_context_ctrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Idata_context_ctrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Idata_context_ctrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Idata_context_ctrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Idata_context_ctrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Idata_context_ctrl * This,
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
    } Idata_context_ctrlVtbl;

    interface Idata_context_ctrl
    {
        CONST_VTBL struct Idata_context_ctrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Idata_context_ctrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Idata_context_ctrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Idata_context_ctrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Idata_context_ctrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Idata_context_ctrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Idata_context_ctrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Idata_context_ctrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __Idata_context_ctrl_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_qq,0xD50FBA6E,0x4709,0x4F1C,0x86,0x6E,0x05,0x97,0xE6,0xD7,0x62,0x1A);

#ifdef __cplusplus

class DECLSPEC_UUID("D50FBA6E-4709-4F1C-866E-0597E6D7621A")
qq;
#endif

#ifndef __Iimage_bar_DISPINTERFACE_DEFINED__
#define __Iimage_bar_DISPINTERFACE_DEFINED__

/* dispinterface Iimage_bar */
/* [uuid] */ 


DEFINE_GUID(DIID_Iimage_bar,0xBBFE74A9,0x1C9A,0x4AC6,0x87,0x94,0x2D,0x4D,0x7D,0xF0,0x25,0x4C);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BBFE74A9-1C9A-4AC6-8794-2D4D7DF0254C")
    Iimage_bar : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct Iimage_barVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Iimage_bar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Iimage_bar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Iimage_bar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Iimage_bar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Iimage_bar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Iimage_bar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Iimage_bar * This,
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
    } Iimage_barVtbl;

    interface Iimage_bar
    {
        CONST_VTBL struct Iimage_barVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Iimage_bar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Iimage_bar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Iimage_bar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Iimage_bar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Iimage_bar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Iimage_bar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Iimage_bar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __Iimage_bar_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_image_bar,0x2D98D0CD,0x3FA2,0x4D08,0x84,0xCE,0x9E,0x07,0x33,0xF5,0xAD,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("2D98D0CD-3FA2-4D08-84CE-9E0733F5AD87")
image_bar;
#endif
#endif /* __DataUI_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


