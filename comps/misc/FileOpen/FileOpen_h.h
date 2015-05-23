

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:16 2015
 */
/* Compiler settings for FileOpen.odl:
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


#ifndef __FileOpen_h_h__
#define __FileOpen_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFileOpenDlg_FWD_DEFINED__
#define __IFileOpenDlg_FWD_DEFINED__
typedef interface IFileOpenDlg IFileOpenDlg;

#endif 	/* __IFileOpenDlg_FWD_DEFINED__ */


#ifndef __IOpenFileDialogEvents_FWD_DEFINED__
#define __IOpenFileDialogEvents_FWD_DEFINED__
typedef interface IOpenFileDialogEvents IOpenFileDialogEvents;

#endif 	/* __IOpenFileDialogEvents_FWD_DEFINED__ */


#ifndef __FileOpenDlg_FWD_DEFINED__
#define __FileOpenDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileOpenDlg FileOpenDlg;
#else
typedef struct FileOpenDlg FileOpenDlg;
#endif /* __cplusplus */

#endif 	/* __FileOpenDlg_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __FileOpen_LIBRARY_DEFINED__
#define __FileOpen_LIBRARY_DEFINED__

/* library FileOpen */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_FileOpen,0xF360E263,0x7AF1,0x11d3,0xA5,0x04,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef __IFileOpenDlg_DISPINTERFACE_DEFINED__
#define __IFileOpenDlg_DISPINTERFACE_DEFINED__

/* dispinterface IFileOpenDlg */
/* [uuid] */ 


DEFINE_GUID(DIID_IFileOpenDlg,0xB189049B,0x7180,0x11D3,0xA4,0xF6,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B189049B-7180-11D3-A4F6-0000B493A187")
    IFileOpenDlg : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IFileOpenDlgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileOpenDlg * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileOpenDlg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileOpenDlg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileOpenDlg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileOpenDlg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileOpenDlg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileOpenDlg * This,
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
    } IFileOpenDlgVtbl;

    interface IFileOpenDlg
    {
        CONST_VTBL struct IFileOpenDlgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileOpenDlg_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileOpenDlg_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileOpenDlg_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileOpenDlg_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFileOpenDlg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFileOpenDlg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFileOpenDlg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IFileOpenDlg_DISPINTERFACE_DEFINED__ */


#ifndef __IOpenFileDialogEvents_DISPINTERFACE_DEFINED__
#define __IOpenFileDialogEvents_DISPINTERFACE_DEFINED__

/* dispinterface IOpenFileDialogEvents */
/* [uuid] */ 


DEFINE_GUID(DIID_IOpenFileDialogEvents,0xF360E264,0x7AF1,0x11d3,0xA5,0x04,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F360E264-7AF1-11d3-A504-0000B493A187")
    IOpenFileDialogEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOpenFileDialogEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpenFileDialogEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpenFileDialogEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpenFileDialogEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOpenFileDialogEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOpenFileDialogEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOpenFileDialogEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOpenFileDialogEvents * This,
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
    } IOpenFileDialogEventsVtbl;

    interface IOpenFileDialogEvents
    {
        CONST_VTBL struct IOpenFileDialogEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpenFileDialogEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOpenFileDialogEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOpenFileDialogEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOpenFileDialogEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOpenFileDialogEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOpenFileDialogEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOpenFileDialogEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOpenFileDialogEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FileOpenDlg,0xB1890499,0x7180,0x11D3,0xA4,0xF6,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("B1890499-7180-11D3-A4F6-0000B493A187")
FileOpenDlg;
#endif
#endif /* __FileOpen_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


