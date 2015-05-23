

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:30 2015
 */
/* Compiler settings for NewDoc.odl:
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


#ifndef __NewDoc_h_h__
#define __NewDoc_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IInputDriverManager_FWD_DEFINED__
#define __IInputDriverManager_FWD_DEFINED__
typedef interface IInputDriverManager IInputDriverManager;

#endif 	/* __IInputDriverManager_FWD_DEFINED__ */


#ifndef __InputDriverManager_FWD_DEFINED__
#define __InputDriverManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class InputDriverManager InputDriverManager;
#else
typedef struct InputDriverManager InputDriverManager;
#endif /* __cplusplus */

#endif 	/* __InputDriverManager_FWD_DEFINED__ */


#ifndef __IDriverEmpty_FWD_DEFINED__
#define __IDriverEmpty_FWD_DEFINED__
typedef interface IDriverEmpty IDriverEmpty;

#endif 	/* __IDriverEmpty_FWD_DEFINED__ */


#ifndef __DriverEmpty_FWD_DEFINED__
#define __DriverEmpty_FWD_DEFINED__

#ifdef __cplusplus
typedef class DriverEmpty DriverEmpty;
#else
typedef struct DriverEmpty DriverEmpty;
#endif /* __cplusplus */

#endif 	/* __DriverEmpty_FWD_DEFINED__ */


#ifndef __IIPreviewView_FWD_DEFINED__
#define __IIPreviewView_FWD_DEFINED__
typedef interface IIPreviewView IIPreviewView;

#endif 	/* __IIPreviewView_FWD_DEFINED__ */


#ifndef __PreviewView_FWD_DEFINED__
#define __PreviewView_FWD_DEFINED__

#ifdef __cplusplus
typedef class PreviewView PreviewView;
#else
typedef struct PreviewView PreviewView;
#endif /* __cplusplus */

#endif 	/* __PreviewView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __NewDoc_LIBRARY_DEFINED__
#define __NewDoc_LIBRARY_DEFINED__

/* library NewDoc */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_NewDoc,0x3F277086,0xC8EB,0x11D3,0x99,0xF5,0x00,0x00,0x00,0x00,0x00,0x00);

#ifndef __IInputDriverManager_DISPINTERFACE_DEFINED__
#define __IInputDriverManager_DISPINTERFACE_DEFINED__

/* dispinterface IInputDriverManager */
/* [uuid] */ 


DEFINE_GUID(DIID_IInputDriverManager,0x394913EA,0x2CEC,0x463B,0xAE,0x00,0x13,0x88,0xA2,0xFD,0x6F,0x3D);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("394913EA-2CEC-463B-AE00-1388A2FD6F3D")
    IInputDriverManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IInputDriverManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInputDriverManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInputDriverManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInputDriverManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IInputDriverManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IInputDriverManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IInputDriverManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IInputDriverManager * This,
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
    } IInputDriverManagerVtbl;

    interface IInputDriverManager
    {
        CONST_VTBL struct IInputDriverManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInputDriverManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInputDriverManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInputDriverManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInputDriverManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IInputDriverManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IInputDriverManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IInputDriverManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IInputDriverManager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_InputDriverManager,0xBB2A4DD0,0x9D8C,0x4F9D,0x91,0x4A,0xB8,0x92,0x2D,0x24,0x7C,0xA9);

#ifdef __cplusplus

class DECLSPEC_UUID("BB2A4DD0-9D8C-4F9D-914A-B8922D247CA9")
InputDriverManager;
#endif

#ifndef __IDriverEmpty_DISPINTERFACE_DEFINED__
#define __IDriverEmpty_DISPINTERFACE_DEFINED__

/* dispinterface IDriverEmpty */
/* [uuid] */ 


DEFINE_GUID(DIID_IDriverEmpty,0xFDE17BB1,0x7E51,0x4C52,0xA8,0x7A,0xEB,0x2E,0xCC,0x24,0x10,0xB5);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FDE17BB1-7E51-4C52-A87A-EB2ECC2410B5")
    IDriverEmpty : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDriverEmptyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDriverEmpty * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDriverEmpty * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDriverEmpty * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDriverEmpty * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDriverEmpty * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDriverEmpty * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDriverEmpty * This,
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
    } IDriverEmptyVtbl;

    interface IDriverEmpty
    {
        CONST_VTBL struct IDriverEmptyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDriverEmpty_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDriverEmpty_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDriverEmpty_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDriverEmpty_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDriverEmpty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDriverEmpty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDriverEmpty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDriverEmpty_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DriverEmpty,0x6A6CD424,0x8DD2,0x4283,0x9A,0xC2,0x30,0x0E,0x27,0x82,0x16,0x66);

#ifdef __cplusplus

class DECLSPEC_UUID("6A6CD424-8DD2-4283-9AC2-300E27821666")
DriverEmpty;
#endif

#ifndef __IIPreviewView_DISPINTERFACE_DEFINED__
#define __IIPreviewView_DISPINTERFACE_DEFINED__

/* dispinterface IIPreviewView */
/* [uuid] */ 


DEFINE_GUID(DIID_IIPreviewView,0xC0D5CB20,0x6ABF,0x4DCC,0xAD,0xBA,0x9E,0x9C,0x99,0x89,0x65,0xA8);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C0D5CB20-6ABF-4DCC-ADBA-9E9C998965A8")
    IIPreviewView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IIPreviewViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIPreviewView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIPreviewView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIPreviewView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIPreviewView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIPreviewView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIPreviewView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIPreviewView * This,
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
    } IIPreviewViewVtbl;

    interface IIPreviewView
    {
        CONST_VTBL struct IIPreviewViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIPreviewView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIPreviewView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIPreviewView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIPreviewView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIPreviewView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIPreviewView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIPreviewView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IIPreviewView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_PreviewView,0xE3C7484B,0x3415,0x4982,0x89,0xB2,0xC4,0x92,0x0A,0xB7,0x39,0x1F);

#ifdef __cplusplus

class DECLSPEC_UUID("E3C7484B-3415-4982-89B2-C4920AB7391F")
PreviewView;
#endif
#endif /* __NewDoc_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


