

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:06:07 2015
 */
/* Compiler settings for CCManager.odl:
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


#ifndef __CCManager_h_h__
#define __CCManager_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IColorConvertorManager_FWD_DEFINED__
#define __IColorConvertorManager_FWD_DEFINED__
typedef interface IColorConvertorManager IColorConvertorManager;

#endif 	/* __IColorConvertorManager_FWD_DEFINED__ */


#ifndef __ColorConvertorManager_FWD_DEFINED__
#define __ColorConvertorManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColorConvertorManager ColorConvertorManager;
#else
typedef struct ColorConvertorManager ColorConvertorManager;
#endif /* __cplusplus */

#endif 	/* __ColorConvertorManager_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CCManager_LIBRARY_DEFINED__
#define __CCManager_LIBRARY_DEFINED__

/* library CCManager */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_CCManager,0x6A1FC3B6,0x6749,0x11D3,0xA4,0xEB,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef __IColorConvertorManager_DISPINTERFACE_DEFINED__
#define __IColorConvertorManager_DISPINTERFACE_DEFINED__

/* dispinterface IColorConvertorManager */
/* [uuid] */ 


DEFINE_GUID(DIID_IColorConvertorManager,0x6A1FC3C9,0x6749,0x11D3,0xA4,0xEB,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6A1FC3C9-6749-11D3-A4EB-0000B493A187")
    IColorConvertorManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IColorConvertorManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IColorConvertorManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IColorConvertorManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IColorConvertorManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IColorConvertorManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IColorConvertorManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IColorConvertorManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IColorConvertorManager * This,
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
    } IColorConvertorManagerVtbl;

    interface IColorConvertorManager
    {
        CONST_VTBL struct IColorConvertorManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IColorConvertorManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IColorConvertorManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IColorConvertorManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IColorConvertorManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IColorConvertorManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IColorConvertorManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IColorConvertorManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IColorConvertorManager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ColorConvertorManager,0x6A1FC3CA,0x6749,0x11D3,0xA4,0xEB,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("6A1FC3CA-6749-11D3-A4EB-0000B493A187")
ColorConvertorManager;
#endif
#endif /* __CCManager_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


