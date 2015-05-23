

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:27 2015
 */
/* Compiler settings for ChooseColor.odl:
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


#ifndef __ChooseColor_h_h__
#define __ChooseColor_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IColorChooseDlg_FWD_DEFINED__
#define __IColorChooseDlg_FWD_DEFINED__
typedef interface IColorChooseDlg IColorChooseDlg;

#endif 	/* __IColorChooseDlg_FWD_DEFINED__ */


#ifndef __ColorChooseDlg_FWD_DEFINED__
#define __ColorChooseDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColorChooseDlg ColorChooseDlg;
#else
typedef struct ColorChooseDlg ColorChooseDlg;
#endif /* __cplusplus */

#endif 	/* __ColorChooseDlg_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ChooseColor_LIBRARY_DEFINED__
#define __ChooseColor_LIBRARY_DEFINED__

/* library ChooseColor */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_ChooseColor,0xBE0B0294,0xA0AE,0x11D3,0xA5,0x38,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef __IColorChooseDlg_DISPINTERFACE_DEFINED__
#define __IColorChooseDlg_DISPINTERFACE_DEFINED__

/* dispinterface IColorChooseDlg */
/* [uuid] */ 


DEFINE_GUID(DIID_IColorChooseDlg,0x09A5EBF3,0xA0E0,0x11d3,0xA5,0x38,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("09A5EBF3-A0E0-11d3-A538-0000B493A187")
    IColorChooseDlg : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IColorChooseDlgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IColorChooseDlg * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IColorChooseDlg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IColorChooseDlg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IColorChooseDlg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IColorChooseDlg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IColorChooseDlg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IColorChooseDlg * This,
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
    } IColorChooseDlgVtbl;

    interface IColorChooseDlg
    {
        CONST_VTBL struct IColorChooseDlgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IColorChooseDlg_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IColorChooseDlg_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IColorChooseDlg_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IColorChooseDlg_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IColorChooseDlg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IColorChooseDlg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IColorChooseDlg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IColorChooseDlg_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ColorChooseDlg,0x09A5EBF2,0xA0E0,0x11d3,0xA5,0x38,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("09A5EBF2-A0E0-11d3-A538-0000B493A187")
ColorChooseDlg;
#endif
#endif /* __ChooseColor_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


