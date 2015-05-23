

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:06:01 2015
 */
/* Compiler settings for AciveColorsView.odl:
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


#ifndef __AciveColorsView_h_h__
#define __AciveColorsView_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DAciveColorsView_FWD_DEFINED__
#define ___DAciveColorsView_FWD_DEFINED__
typedef interface _DAciveColorsView _DAciveColorsView;

#endif 	/* ___DAciveColorsView_FWD_DEFINED__ */


#ifndef ___DAciveColorsViewEvents_FWD_DEFINED__
#define ___DAciveColorsViewEvents_FWD_DEFINED__
typedef interface _DAciveColorsViewEvents _DAciveColorsViewEvents;

#endif 	/* ___DAciveColorsViewEvents_FWD_DEFINED__ */


#ifndef __AciveColorsView_FWD_DEFINED__
#define __AciveColorsView_FWD_DEFINED__

#ifdef __cplusplus
typedef class AciveColorsView AciveColorsView;
#else
typedef struct AciveColorsView AciveColorsView;
#endif /* __cplusplus */

#endif 	/* __AciveColorsView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ACIVECOLORSVIEWLib_LIBRARY_DEFINED__
#define __ACIVECOLORSVIEWLib_LIBRARY_DEFINED__

/* library ACIVECOLORSVIEWLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_ACIVECOLORSVIEWLib,0x0C1F6A88,0x9DB3,0x11D3,0xA5,0x36,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef ___DAciveColorsView_DISPINTERFACE_DEFINED__
#define ___DAciveColorsView_DISPINTERFACE_DEFINED__

/* dispinterface _DAciveColorsView */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DAciveColorsView,0x0C1F6A89,0x9DB3,0x11D3,0xA5,0x36,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0C1F6A89-9DB3-11D3-A536-0000B493A187")
    _DAciveColorsView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAciveColorsViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAciveColorsView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAciveColorsView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAciveColorsView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAciveColorsView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAciveColorsView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAciveColorsView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAciveColorsView * This,
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
    } _DAciveColorsViewVtbl;

    interface _DAciveColorsView
    {
        CONST_VTBL struct _DAciveColorsViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAciveColorsView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAciveColorsView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAciveColorsView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAciveColorsView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAciveColorsView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAciveColorsView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAciveColorsView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAciveColorsView_DISPINTERFACE_DEFINED__ */


#ifndef ___DAciveColorsViewEvents_DISPINTERFACE_DEFINED__
#define ___DAciveColorsViewEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAciveColorsViewEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DAciveColorsViewEvents,0x0C1F6A8A,0x9DB3,0x11D3,0xA5,0x36,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0C1F6A8A-9DB3-11D3-A536-0000B493A187")
    _DAciveColorsViewEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAciveColorsViewEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAciveColorsViewEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAciveColorsViewEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAciveColorsViewEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAciveColorsViewEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAciveColorsViewEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAciveColorsViewEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAciveColorsViewEvents * This,
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
    } _DAciveColorsViewEventsVtbl;

    interface _DAciveColorsViewEvents
    {
        CONST_VTBL struct _DAciveColorsViewEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAciveColorsViewEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAciveColorsViewEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAciveColorsViewEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAciveColorsViewEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAciveColorsViewEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAciveColorsViewEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAciveColorsViewEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAciveColorsViewEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AciveColorsView,0x0C1F6A8B,0x9DB3,0x11D3,0xA5,0x36,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("0C1F6A8B-9DB3-11D3-A536-0000B493A187")
AciveColorsView;
#endif
#endif /* __ACIVECOLORSVIEWLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


