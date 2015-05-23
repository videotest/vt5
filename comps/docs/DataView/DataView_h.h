

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:15 2015
 */
/* Compiler settings for DataView.odl:
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


#ifndef __DataView_h_h__
#define __DataView_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGridViewDisp_FWD_DEFINED__
#define __IGridViewDisp_FWD_DEFINED__
typedef interface IGridViewDisp IGridViewDisp;

#endif 	/* __IGridViewDisp_FWD_DEFINED__ */


#ifndef __ClassView_FWD_DEFINED__
#define __ClassView_FWD_DEFINED__

#ifdef __cplusplus
typedef class ClassView ClassView;
#else
typedef struct ClassView ClassView;
#endif /* __cplusplus */

#endif 	/* __ClassView_FWD_DEFINED__ */


#ifndef __MeasView_FWD_DEFINED__
#define __MeasView_FWD_DEFINED__

#ifdef __cplusplus
typedef class MeasView MeasView;
#else
typedef struct MeasView MeasView;
#endif /* __cplusplus */

#endif 	/* __MeasView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __DataView_LIBRARY_DEFINED__
#define __DataView_LIBRARY_DEFINED__

/* library DataView */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_DataView,0x3A7F1BE2,0x50A5,0x4888,0x9A,0x56,0xAF,0x7C,0xE9,0x14,0x3F,0xE0);

#ifndef __IGridViewDisp_DISPINTERFACE_DEFINED__
#define __IGridViewDisp_DISPINTERFACE_DEFINED__

/* dispinterface IGridViewDisp */
/* [uuid] */ 


DEFINE_GUID(DIID_IGridViewDisp,0x000C6422,0x057F,0x4152,0xA9,0x4D,0x3E,0x2F,0x6A,0xFB,0xA5,0xFA);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("000C6422-057F-4152-A94D-3E2F6AFBA5FA")
    IGridViewDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IGridViewDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGridViewDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGridViewDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGridViewDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGridViewDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGridViewDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGridViewDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGridViewDisp * This,
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
    } IGridViewDispVtbl;

    interface IGridViewDisp
    {
        CONST_VTBL struct IGridViewDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGridViewDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGridViewDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGridViewDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGridViewDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGridViewDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGridViewDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGridViewDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IGridViewDisp_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ClassView,0x03E6BDBA,0xE120,0x4f0b,0xB7,0xEF,0x01,0x21,0xC1,0xC8,0x87,0x7C);

#ifdef __cplusplus

class DECLSPEC_UUID("03E6BDBA-E120-4f0b-B7EF-0121C1C8877C")
ClassView;
#endif

DEFINE_GUID(CLSID_MeasView,0x6FDA8C15,0x0959,0x4F25,0x97,0x1B,0x87,0xFA,0xE6,0x72,0xA0,0x7A);

#ifdef __cplusplus

class DECLSPEC_UUID("6FDA8C15-0959-4F25-971B-87FAE672A07A")
MeasView;
#endif
#endif /* __DataView_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


