

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:46 2015
 */
/* Compiler settings for ColorManAX.odl:
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


#ifndef __ColorManAX_h_h__
#define __ColorManAX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DColorManAX_FWD_DEFINED__
#define ___DColorManAX_FWD_DEFINED__
typedef interface _DColorManAX _DColorManAX;

#endif 	/* ___DColorManAX_FWD_DEFINED__ */


#ifndef ___DColorManAXEvents_FWD_DEFINED__
#define ___DColorManAXEvents_FWD_DEFINED__
typedef interface _DColorManAXEvents _DColorManAXEvents;

#endif 	/* ___DColorManAXEvents_FWD_DEFINED__ */


#ifndef __ColorManAX_FWD_DEFINED__
#define __ColorManAX_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColorManAX ColorManAX;
#else
typedef struct ColorManAX ColorManAX;
#endif /* __cplusplus */

#endif 	/* __ColorManAX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __COLORMANAXLib_LIBRARY_DEFINED__
#define __COLORMANAXLib_LIBRARY_DEFINED__

/* library COLORMANAXLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_COLORMANAXLib,0x5DB73BB3,0xF590,0x11D3,0xA0,0xBC,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef ___DColorManAX_DISPINTERFACE_DEFINED__
#define ___DColorManAX_DISPINTERFACE_DEFINED__

/* dispinterface _DColorManAX */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DColorManAX,0x5DB73BB4,0xF590,0x11D3,0xA0,0xBC,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5DB73BB4-F590-11D3-A0BC-0000B493A187")
    _DColorManAX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColorManAXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColorManAX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColorManAX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColorManAX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColorManAX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColorManAX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColorManAX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColorManAX * This,
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
    } _DColorManAXVtbl;

    interface _DColorManAX
    {
        CONST_VTBL struct _DColorManAXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColorManAX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColorManAX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColorManAX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColorManAX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColorManAX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColorManAX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColorManAX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColorManAX_DISPINTERFACE_DEFINED__ */


#ifndef ___DColorManAXEvents_DISPINTERFACE_DEFINED__
#define ___DColorManAXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DColorManAXEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DColorManAXEvents,0x5DB73BB5,0xF590,0x11D3,0xA0,0xBC,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5DB73BB5-F590-11D3-A0BC-0000B493A187")
    _DColorManAXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColorManAXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColorManAXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColorManAXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColorManAXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColorManAXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColorManAXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColorManAXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColorManAXEvents * This,
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
    } _DColorManAXEventsVtbl;

    interface _DColorManAXEvents
    {
        CONST_VTBL struct _DColorManAXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColorManAXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColorManAXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColorManAXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColorManAXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColorManAXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColorManAXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColorManAXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColorManAXEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ColorManAX,0x3871368D,0x87AC,0x4f8e,0xAE,0x3D,0x2F,0xC2,0xA3,0xDB,0x60,0xA8);

#ifdef __cplusplus

class DECLSPEC_UUID("3871368D-87AC-4f8e-AE3D-2FC2A3DB60A8")
ColorManAX;
#endif
#endif /* __COLORMANAXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


