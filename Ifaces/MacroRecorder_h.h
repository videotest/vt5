

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Aug 25 14:42:56 2018
 */
/* Compiler settings for MacroRecorder.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __MacroRecorder_h_h__
#define __MacroRecorder_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMacroManagerDisp_FWD_DEFINED__
#define __IMacroManagerDisp_FWD_DEFINED__
typedef interface IMacroManagerDisp IMacroManagerDisp;

#endif 	/* __IMacroManagerDisp_FWD_DEFINED__ */


#ifndef __IMacroHelperDisp_FWD_DEFINED__
#define __IMacroHelperDisp_FWD_DEFINED__
typedef interface IMacroHelperDisp IMacroHelperDisp;

#endif 	/* __IMacroHelperDisp_FWD_DEFINED__ */


#ifndef __CoMacroManager_FWD_DEFINED__
#define __CoMacroManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoMacroManager CoMacroManager;
#else
typedef struct CoMacroManager CoMacroManager;
#endif /* __cplusplus */

#endif 	/* __CoMacroManager_FWD_DEFINED__ */


#ifndef __CoMacroHelper_FWD_DEFINED__
#define __CoMacroHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoMacroHelper CoMacroHelper;
#else
typedef struct CoMacroHelper CoMacroHelper;
#endif /* __cplusplus */

#endif 	/* __CoMacroHelper_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMacroManagerDisp_INTERFACE_DEFINED__
#define __IMacroManagerDisp_INTERFACE_DEFINED__

/* interface IMacroManagerDisp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMacroManagerDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77EB74DD-09B7-40df-94A2-E1DD518DCD24")
    IMacroManagerDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartRecord( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResumeRecord( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopRecord( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMacroScript( 
            /* [in] */ BSTR bstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMacroScript( 
            /* [retval][out] */ BSTR *pbstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteScript( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMacroManagerDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMacroManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMacroManagerDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMacroManagerDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMacroManagerDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMacroManagerDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMacroManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMacroManagerDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartRecord )( 
            IMacroManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResumeRecord )( 
            IMacroManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopRecord )( 
            IMacroManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMacroScript )( 
            IMacroManagerDisp * This,
            /* [in] */ BSTR bstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMacroScript )( 
            IMacroManagerDisp * This,
            /* [retval][out] */ BSTR *pbstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteScript )( 
            IMacroManagerDisp * This);
        
        END_INTERFACE
    } IMacroManagerDispVtbl;

    interface IMacroManagerDisp
    {
        CONST_VTBL struct IMacroManagerDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMacroManagerDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMacroManagerDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMacroManagerDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMacroManagerDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMacroManagerDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMacroManagerDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMacroManagerDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMacroManagerDisp_StartRecord(This)	\
    ( (This)->lpVtbl -> StartRecord(This) ) 

#define IMacroManagerDisp_ResumeRecord(This)	\
    ( (This)->lpVtbl -> ResumeRecord(This) ) 

#define IMacroManagerDisp_StopRecord(This)	\
    ( (This)->lpVtbl -> StopRecord(This) ) 

#define IMacroManagerDisp_SetMacroScript(This,bstrScript)	\
    ( (This)->lpVtbl -> SetMacroScript(This,bstrScript) ) 

#define IMacroManagerDisp_GetMacroScript(This,pbstrScript)	\
    ( (This)->lpVtbl -> GetMacroScript(This,pbstrScript) ) 

#define IMacroManagerDisp_ExecuteScript(This)	\
    ( (This)->lpVtbl -> ExecuteScript(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMacroManagerDisp_INTERFACE_DEFINED__ */


#ifndef __IMacroHelperDisp_INTERFACE_DEFINED__
#define __IMacroHelperDisp_INTERFACE_DEFINED__

/* interface IMacroHelperDisp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMacroHelperDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B0C7B4BE-6A1B-48b6-A1F5-B1BAC0810348")
    IMacroHelperDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEventsWindow( 
            /* [in] */ BSTR bstrWndClass) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MouseMove( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LButtonDown( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LButtonUp( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LButtonDblClk( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RButtonDown( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RButtonUp( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RButtonDblClk( 
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KeyDown( 
            /* [in] */ long lKeyCode,
            /* [in] */ long lIsExtendedKey,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KeyUp( 
            /* [in] */ long lKeyCode,
            /* [in] */ long lIsExtendedKey,
            /* [in] */ DWORD dwDelay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitMacroEnd( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScriptRunning( 
            /* [in] */ long lSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEventsWindow2( 
            /* [in] */ BSTR bstrWndClass,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ int nCX,
            /* [in] */ int nCY,
            /* [in] */ long lHwnd) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMacroHelperDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMacroHelperDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMacroHelperDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMacroHelperDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMacroHelperDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMacroHelperDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMacroHelperDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMacroHelperDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetEventsWindow )( 
            IMacroHelperDisp * This,
            /* [in] */ BSTR bstrWndClass);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MouseMove )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LButtonDown )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LButtonUp )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LButtonDblClk )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RButtonDown )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RButtonUp )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RButtonDblClk )( 
            IMacroHelperDisp * This,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KeyDown )( 
            IMacroHelperDisp * This,
            /* [in] */ long lKeyCode,
            /* [in] */ long lIsExtendedKey,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KeyUp )( 
            IMacroHelperDisp * This,
            /* [in] */ long lKeyCode,
            /* [in] */ long lIsExtendedKey,
            /* [in] */ DWORD dwDelay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitMacroEnd )( 
            IMacroHelperDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScriptRunning )( 
            IMacroHelperDisp * This,
            /* [in] */ long lSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetEventsWindow2 )( 
            IMacroHelperDisp * This,
            /* [in] */ BSTR bstrWndClass,
            /* [in] */ int nX,
            /* [in] */ int nY,
            /* [in] */ int nCX,
            /* [in] */ int nCY,
            /* [in] */ long lHwnd);
        
        END_INTERFACE
    } IMacroHelperDispVtbl;

    interface IMacroHelperDisp
    {
        CONST_VTBL struct IMacroHelperDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMacroHelperDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMacroHelperDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMacroHelperDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMacroHelperDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMacroHelperDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMacroHelperDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMacroHelperDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMacroHelperDisp_SetEventsWindow(This,bstrWndClass)	\
    ( (This)->lpVtbl -> SetEventsWindow(This,bstrWndClass) ) 

#define IMacroHelperDisp_MouseMove(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> MouseMove(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_LButtonDown(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> LButtonDown(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_LButtonUp(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> LButtonUp(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_LButtonDblClk(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> LButtonDblClk(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_RButtonDown(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> RButtonDown(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_RButtonUp(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> RButtonUp(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_RButtonDblClk(This,nX,nY,dwDelay)	\
    ( (This)->lpVtbl -> RButtonDblClk(This,nX,nY,dwDelay) ) 

#define IMacroHelperDisp_KeyDown(This,lKeyCode,lIsExtendedKey,dwDelay)	\
    ( (This)->lpVtbl -> KeyDown(This,lKeyCode,lIsExtendedKey,dwDelay) ) 

#define IMacroHelperDisp_KeyUp(This,lKeyCode,lIsExtendedKey,dwDelay)	\
    ( (This)->lpVtbl -> KeyUp(This,lKeyCode,lIsExtendedKey,dwDelay) ) 

#define IMacroHelperDisp_WaitMacroEnd(This)	\
    ( (This)->lpVtbl -> WaitMacroEnd(This) ) 

#define IMacroHelperDisp_SetScriptRunning(This,lSet)	\
    ( (This)->lpVtbl -> SetScriptRunning(This,lSet) ) 

#define IMacroHelperDisp_SetEventsWindow2(This,bstrWndClass,nX,nY,nCX,nCY,lHwnd)	\
    ( (This)->lpVtbl -> SetEventsWindow2(This,bstrWndClass,nX,nY,nCX,nCY,lHwnd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMacroHelperDisp_INTERFACE_DEFINED__ */



#ifndef __MacroRecordLib_LIBRARY_DEFINED__
#define __MacroRecordLib_LIBRARY_DEFINED__

/* library MacroRecordLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MacroRecordLib;

EXTERN_C const CLSID CLSID_CoMacroManager;

#ifdef __cplusplus

class DECLSPEC_UUID("8B8F28C2-6F8E-4d61-8212-1AEBE1C1C8CF")
CoMacroManager;
#endif

EXTERN_C const CLSID CLSID_CoMacroHelper;

#ifdef __cplusplus

class DECLSPEC_UUID("9ACE10DF-FB94-4239-8D15-B7D25CEDE85C")
CoMacroHelper;
#endif
#endif /* __MacroRecordLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


