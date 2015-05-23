

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:57 2015
 */
/* Compiler settings for macros.odl:
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


#ifndef __macros_h_h__
#define __macros_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMacroRecorder_FWD_DEFINED__
#define __IMacroRecorder_FWD_DEFINED__
typedef interface IMacroRecorder IMacroRecorder;

#endif 	/* __IMacroRecorder_FWD_DEFINED__ */


#ifndef __MacroRecorder_FWD_DEFINED__
#define __MacroRecorder_FWD_DEFINED__

#ifdef __cplusplus
typedef class MacroRecorder MacroRecorder;
#else
typedef struct MacroRecorder MacroRecorder;
#endif /* __cplusplus */

#endif 	/* __MacroRecorder_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Macros_LIBRARY_DEFINED__
#define __Macros_LIBRARY_DEFINED__

/* library Macros */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Macros,0x8E8E8339,0x6B33,0x11D3,0xA6,0x52,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifndef __IMacroRecorder_DISPINTERFACE_DEFINED__
#define __IMacroRecorder_DISPINTERFACE_DEFINED__

/* dispinterface IMacroRecorder */
/* [uuid] */ 


DEFINE_GUID(DIID_IMacroRecorder,0x8E8E8346,0x6B33,0x11D3,0xA6,0x52,0x00,0x90,0x27,0x59,0x95,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8E8E8346-6B33-11D3-A652-0090275995FE")
    IMacroRecorder : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMacroRecorderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMacroRecorder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMacroRecorder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMacroRecorder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMacroRecorder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMacroRecorder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMacroRecorder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMacroRecorder * This,
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
    } IMacroRecorderVtbl;

    interface IMacroRecorder
    {
        CONST_VTBL struct IMacroRecorderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMacroRecorder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMacroRecorder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMacroRecorder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMacroRecorder_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMacroRecorder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMacroRecorder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMacroRecorder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMacroRecorder_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MacroRecorder,0x8E8E8347,0x6B33,0x11D3,0xA6,0x52,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifdef __cplusplus

class DECLSPEC_UUID("8E8E8347-6B33-11D3-A652-0090275995FE")
MacroRecorder;
#endif
#endif /* __Macros_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


