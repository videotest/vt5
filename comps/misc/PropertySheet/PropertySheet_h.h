

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:34 2015
 */
/* Compiler settings for PropertySheet.odl:
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


#ifndef __PropertySheet_h_h__
#define __PropertySheet_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPropertySheetMan_FWD_DEFINED__
#define __IPropertySheetMan_FWD_DEFINED__
typedef interface IPropertySheetMan IPropertySheetMan;

#endif 	/* __IPropertySheetMan_FWD_DEFINED__ */


#ifndef __PropertySheetMan_FWD_DEFINED__
#define __PropertySheetMan_FWD_DEFINED__

#ifdef __cplusplus
typedef class PropertySheetMan PropertySheetMan;
#else
typedef struct PropertySheetMan PropertySheetMan;
#endif /* __cplusplus */

#endif 	/* __PropertySheetMan_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PropertySheet_LIBRARY_DEFINED__
#define __PropertySheet_LIBRARY_DEFINED__

/* library PropertySheet */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_PropertySheet,0xFD74A2C9,0x7FAC,0x11D3,0xA5,0x0C,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef __IPropertySheetMan_DISPINTERFACE_DEFINED__
#define __IPropertySheetMan_DISPINTERFACE_DEFINED__

/* dispinterface IPropertySheetMan */
/* [uuid] */ 


DEFINE_GUID(DIID_IPropertySheetMan,0xFD74A2D9,0x7FAC,0x11D3,0xA5,0x0C,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FD74A2D9-7FAC-11D3-A50C-0000B493A187")
    IPropertySheetMan : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IPropertySheetManVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertySheetMan * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertySheetMan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertySheetMan * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPropertySheetMan * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPropertySheetMan * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPropertySheetMan * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPropertySheetMan * This,
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
    } IPropertySheetManVtbl;

    interface IPropertySheetMan
    {
        CONST_VTBL struct IPropertySheetManVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertySheetMan_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertySheetMan_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertySheetMan_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertySheetMan_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPropertySheetMan_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPropertySheetMan_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPropertySheetMan_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IPropertySheetMan_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_PropertySheetMan,0xFD74A2D8,0x7FAC,0x11D3,0xA5,0x0C,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("FD74A2D8-7FAC-11D3-A50C-0000B493A187")
PropertySheetMan;
#endif
#endif /* __PropertySheet_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


