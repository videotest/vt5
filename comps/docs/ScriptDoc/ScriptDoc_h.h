

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:40 2015
 */
/* Compiler settings for ScriptDoc.odl:
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


#ifndef __ScriptDoc_h_h__
#define __ScriptDoc_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScriptDocument_FWD_DEFINED__
#define __IScriptDocument_FWD_DEFINED__
typedef interface IScriptDocument IScriptDocument;

#endif 	/* __IScriptDocument_FWD_DEFINED__ */


#ifndef __ScriptDocument_FWD_DEFINED__
#define __ScriptDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScriptDocument ScriptDocument;
#else
typedef struct ScriptDocument ScriptDocument;
#endif /* __cplusplus */

#endif 	/* __ScriptDocument_FWD_DEFINED__ */


#ifndef __IScriptDataObject_FWD_DEFINED__
#define __IScriptDataObject_FWD_DEFINED__
typedef interface IScriptDataObject IScriptDataObject;

#endif 	/* __IScriptDataObject_FWD_DEFINED__ */


#ifndef __ScriptDataObject_FWD_DEFINED__
#define __ScriptDataObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScriptDataObject ScriptDataObject;
#else
typedef struct ScriptDataObject ScriptDataObject;
#endif /* __cplusplus */

#endif 	/* __ScriptDataObject_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ScriptDoc_LIBRARY_DEFINED__
#define __ScriptDoc_LIBRARY_DEFINED__

/* library ScriptDoc */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_ScriptDoc,0x92FDEF6B,0x0C1A,0x11D3,0xA5,0xB6,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifndef __IScriptDocument_DISPINTERFACE_DEFINED__
#define __IScriptDocument_DISPINTERFACE_DEFINED__

/* dispinterface IScriptDocument */
/* [uuid] */ 


DEFINE_GUID(DIID_IScriptDocument,0xA66BA363,0x0F48,0x11D3,0xA5,0xBD,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A66BA363-0F48-11D3-A5BD-0000B493FF1B")
    IScriptDocument : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IScriptDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScriptDocument * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScriptDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScriptDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScriptDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScriptDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScriptDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScriptDocument * This,
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
    } IScriptDocumentVtbl;

    interface IScriptDocument
    {
        CONST_VTBL struct IScriptDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScriptDocument_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScriptDocument_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScriptDocument_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScriptDocument_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IScriptDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IScriptDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IScriptDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IScriptDocument_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ScriptDocument,0xA66BA365,0x0F48,0x11D3,0xA5,0xBD,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("A66BA365-0F48-11D3-A5BD-0000B493FF1B")
ScriptDocument;
#endif

#ifndef __IScriptDataObject_DISPINTERFACE_DEFINED__
#define __IScriptDataObject_DISPINTERFACE_DEFINED__

/* dispinterface IScriptDataObject */
/* [uuid] */ 


DEFINE_GUID(DIID_IScriptDataObject,0x39346386,0x2480,0x11D3,0xA5,0xD9,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("39346386-2480-11D3-A5D9-0000B493FF1B")
    IScriptDataObject : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IScriptDataObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScriptDataObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScriptDataObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScriptDataObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScriptDataObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScriptDataObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScriptDataObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScriptDataObject * This,
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
    } IScriptDataObjectVtbl;

    interface IScriptDataObject
    {
        CONST_VTBL struct IScriptDataObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScriptDataObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScriptDataObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScriptDataObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScriptDataObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IScriptDataObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IScriptDataObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IScriptDataObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IScriptDataObject_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ScriptDataObject,0x39346388,0x2480,0x11D3,0xA5,0xD9,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("39346388-2480-11D3-A5D9-0000B493FF1B")
ScriptDataObject;
#endif
#endif /* __ScriptDoc_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


