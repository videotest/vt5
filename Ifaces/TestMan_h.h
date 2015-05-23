

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:43:14 2015
 */
/* Compiler settings for TestMan.idl:
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

#ifndef __TestMan_h_h__
#define __TestMan_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITestManagerDisp_FWD_DEFINED__
#define __ITestManagerDisp_FWD_DEFINED__
typedef interface ITestManagerDisp ITestManagerDisp;

#endif 	/* __ITestManagerDisp_FWD_DEFINED__ */


#ifndef __ITestItemDisp_FWD_DEFINED__
#define __ITestItemDisp_FWD_DEFINED__
typedef interface ITestItemDisp ITestItemDisp;

#endif 	/* __ITestItemDisp_FWD_DEFINED__ */


#ifndef __ITestDataArrayDisp_FWD_DEFINED__
#define __ITestDataArrayDisp_FWD_DEFINED__
typedef interface ITestDataArrayDisp ITestDataArrayDisp;

#endif 	/* __ITestDataArrayDisp_FWD_DEFINED__ */


#ifndef __CoTestManager_FWD_DEFINED__
#define __CoTestManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoTestManager CoTestManager;
#else
typedef struct CoTestManager CoTestManager;
#endif /* __cplusplus */

#endif 	/* __CoTestManager_FWD_DEFINED__ */


#ifndef __CoTestItem_FWD_DEFINED__
#define __CoTestItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoTestItem CoTestItem;
#else
typedef struct CoTestItem CoTestItem;
#endif /* __cplusplus */

#endif 	/* __CoTestItem_FWD_DEFINED__ */


#ifndef __CoTestDataArray_FWD_DEFINED__
#define __CoTestDataArray_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoTestDataArray CoTestDataArray;
#else
typedef struct CoTestDataArray CoTestDataArray;
#endif /* __cplusplus */

#endif 	/* __CoTestDataArray_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITestManagerDisp_INTERFACE_DEFINED__
#define __ITestManagerDisp_INTERFACE_DEFINED__

/* interface ITestManagerDisp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ITestManagerDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("008D4C73-2889-43cd-98F2-D9FD2F84B6F1")
    ITestManagerDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFirstTestPos( 
            /* [in] */ long lParentPos,
            /* [retval][out] */ long *plPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNextTest( 
            /* [in] */ long lParentPos,
            /* [out] */ VARIANT *plPos,
            /* [retval][out] */ IDispatch **ppunk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddTest( 
            /* [in] */ long lParent,
            /* [in] */ IDispatch *punk,
            long *plpos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteTest( 
            /* [in] */ long lPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunTestCompare( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunTestSave( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopTesting( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRunningState( 
            /* [retval][out] */ long *plState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunTestSimple( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestManagerDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestManagerDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestManagerDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestManagerDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestManagerDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestManagerDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFirstTestPos )( 
            ITestManagerDisp * This,
            /* [in] */ long lParentPos,
            /* [retval][out] */ long *plPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNextTest )( 
            ITestManagerDisp * This,
            /* [in] */ long lParentPos,
            /* [out] */ VARIANT *plPos,
            /* [retval][out] */ IDispatch **ppunk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddTest )( 
            ITestManagerDisp * This,
            /* [in] */ long lParent,
            /* [in] */ IDispatch *punk,
            long *plpos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteTest )( 
            ITestManagerDisp * This,
            /* [in] */ long lPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunTestCompare )( 
            ITestManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunTestSave )( 
            ITestManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopTesting )( 
            ITestManagerDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRunningState )( 
            ITestManagerDisp * This,
            /* [retval][out] */ long *plState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunTestSimple )( 
            ITestManagerDisp * This);
        
        END_INTERFACE
    } ITestManagerDispVtbl;

    interface ITestManagerDisp
    {
        CONST_VTBL struct ITestManagerDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestManagerDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestManagerDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestManagerDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestManagerDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestManagerDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestManagerDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestManagerDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITestManagerDisp_GetFirstTestPos(This,lParentPos,plPos)	\
    ( (This)->lpVtbl -> GetFirstTestPos(This,lParentPos,plPos) ) 

#define ITestManagerDisp_GetNextTest(This,lParentPos,plPos,ppunk)	\
    ( (This)->lpVtbl -> GetNextTest(This,lParentPos,plPos,ppunk) ) 

#define ITestManagerDisp_AddTest(This,lParent,punk,plpos)	\
    ( (This)->lpVtbl -> AddTest(This,lParent,punk,plpos) ) 

#define ITestManagerDisp_DeleteTest(This,lPos)	\
    ( (This)->lpVtbl -> DeleteTest(This,lPos) ) 

#define ITestManagerDisp_RunTestCompare(This)	\
    ( (This)->lpVtbl -> RunTestCompare(This) ) 

#define ITestManagerDisp_RunTestSave(This)	\
    ( (This)->lpVtbl -> RunTestSave(This) ) 

#define ITestManagerDisp_StopTesting(This)	\
    ( (This)->lpVtbl -> StopTesting(This) ) 

#define ITestManagerDisp_GetRunningState(This,plState)	\
    ( (This)->lpVtbl -> GetRunningState(This,plState) ) 

#define ITestManagerDisp_RunTestSimple(This)	\
    ( (This)->lpVtbl -> RunTestSimple(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestManagerDisp_INTERFACE_DEFINED__ */


#ifndef __ITestItemDisp_INTERFACE_DEFINED__
#define __ITestItemDisp_INTERFACE_DEFINED__

/* interface ITestItemDisp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ITestItemDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37E9B6A7-5AA9-40bf-B3CF-08A8C1CE4F0A")
    ITestItemDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ BSTR bstrPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPath( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFirstCondPos( 
            /* [retval][out] */ long *lpPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNextCond( 
            /* [out] */ VARIANT *plPos,
            /* [retval][out] */ BSTR *pbstr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddCond( 
            /* [in] */ BSTR bstrCond) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteCond( 
            /* [in] */ long lPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteAllCond( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScript( 
            /* [in] */ BSTR bstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScript( 
            /* [retval][out] */ BSTR *pbstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadContent( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StoreContent( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRunTime( 
            /* [retval][out] */ DATE *pDate) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRunTime( 
            /* [in] */ DATE Date) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTestChecked( 
            /* [in] */ BOOL bSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTestChecked( 
            /* [retval][out] */ BOOL *pbSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KillThemselvesFromDisk( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ParseCond( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFirstSingleCond( 
            /* [retval][out] */ long *lpPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NextSingleCond( 
            /* [out] */ VARIANT *lpPos,
            /* [out] */ VARIANT *pVarName,
            /* [out] */ VARIANT *pVarPath,
            /* [out] */ VARIANT *pVarParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFirstMultyCond( 
            /* [retval][out] */ long *lpPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NextMultyCond( 
            /* [out] */ VARIANT *lpPos,
            /* [out] */ VARIANT *pVarName,
            /* [out] */ VARIANT *pVarPath,
            /* [out] */ VARIANT **pVarParams,
            /* [out] */ VARIANT *pVarFlag,
            /* [out] */ long *plCount,
            VARIANT *pVarNameMain,
            /* [out] */ VARIANT **pVarParamsMain,
            /* [retval][out] */ long *plCountMain) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTestSavedState( 
            /* [in] */ long bSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTestSavedState( 
            /* [retval][out] */ long *pbSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTestRunTime( 
            unsigned long dwTime) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTestRunTime( 
            unsigned long *pdwTime) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScriptFinal( 
            /* [in] */ BSTR bstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScriptFinal( 
            /* [retval][out] */ BSTR *pbstrScript) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Rename( 
            /* [in] */ BSTR bstrNewName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAdditionalInfo( 
            /* [retval][out] */ BSTR *pbstrInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAdditionalInfo( 
            /* [in] */ BSTR bstrInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSaveTime( 
            /* [retval][out] */ DATE *pDate) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSaveTime( 
            /* [in] */ DATE Date) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTestErrorState( 
            /* [retval][out] */ long *pbError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTestErrorState( 
            /* [in] */ long bError) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestItemDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestItemDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestItemDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestItemDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestItemDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestItemDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestItemDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestItemDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            ITestItemDisp * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFirstCondPos )( 
            ITestItemDisp * This,
            /* [retval][out] */ long *lpPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNextCond )( 
            ITestItemDisp * This,
            /* [out] */ VARIANT *plPos,
            /* [retval][out] */ BSTR *pbstr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddCond )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrCond);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteCond )( 
            ITestItemDisp * This,
            /* [in] */ long lPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteAllCond )( 
            ITestItemDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScript )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScript )( 
            ITestItemDisp * This,
            /* [retval][out] */ BSTR *pbstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadContent )( 
            ITestItemDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StoreContent )( 
            ITestItemDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRunTime )( 
            ITestItemDisp * This,
            /* [retval][out] */ DATE *pDate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRunTime )( 
            ITestItemDisp * This,
            /* [in] */ DATE Date);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTestChecked )( 
            ITestItemDisp * This,
            /* [in] */ BOOL bSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTestChecked )( 
            ITestItemDisp * This,
            /* [retval][out] */ BOOL *pbSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KillThemselvesFromDisk )( 
            ITestItemDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ParseCond )( 
            ITestItemDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFirstSingleCond )( 
            ITestItemDisp * This,
            /* [retval][out] */ long *lpPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NextSingleCond )( 
            ITestItemDisp * This,
            /* [out] */ VARIANT *lpPos,
            /* [out] */ VARIANT *pVarName,
            /* [out] */ VARIANT *pVarPath,
            /* [out] */ VARIANT *pVarParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFirstMultyCond )( 
            ITestItemDisp * This,
            /* [retval][out] */ long *lpPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NextMultyCond )( 
            ITestItemDisp * This,
            /* [out] */ VARIANT *lpPos,
            /* [out] */ VARIANT *pVarName,
            /* [out] */ VARIANT *pVarPath,
            /* [out] */ VARIANT **pVarParams,
            /* [out] */ VARIANT *pVarFlag,
            /* [out] */ long *plCount,
            VARIANT *pVarNameMain,
            /* [out] */ VARIANT **pVarParamsMain,
            /* [retval][out] */ long *plCountMain);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTestSavedState )( 
            ITestItemDisp * This,
            /* [in] */ long bSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTestSavedState )( 
            ITestItemDisp * This,
            /* [retval][out] */ long *pbSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTestRunTime )( 
            ITestItemDisp * This,
            unsigned long dwTime);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTestRunTime )( 
            ITestItemDisp * This,
            unsigned long *pdwTime);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScriptFinal )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScriptFinal )( 
            ITestItemDisp * This,
            /* [retval][out] */ BSTR *pbstrScript);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Rename )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrNewName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAdditionalInfo )( 
            ITestItemDisp * This,
            /* [retval][out] */ BSTR *pbstrInfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAdditionalInfo )( 
            ITestItemDisp * This,
            /* [in] */ BSTR bstrInfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSaveTime )( 
            ITestItemDisp * This,
            /* [retval][out] */ DATE *pDate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSaveTime )( 
            ITestItemDisp * This,
            /* [in] */ DATE Date);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTestErrorState )( 
            ITestItemDisp * This,
            /* [retval][out] */ long *pbError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTestErrorState )( 
            ITestItemDisp * This,
            /* [in] */ long bError);
        
        END_INTERFACE
    } ITestItemDispVtbl;

    interface ITestItemDisp
    {
        CONST_VTBL struct ITestItemDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestItemDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestItemDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestItemDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestItemDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestItemDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestItemDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestItemDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITestItemDisp_SetPath(This,bstrPath)	\
    ( (This)->lpVtbl -> SetPath(This,bstrPath) ) 

#define ITestItemDisp_GetPath(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetPath(This,pbstrPath) ) 

#define ITestItemDisp_GetFirstCondPos(This,lpPos)	\
    ( (This)->lpVtbl -> GetFirstCondPos(This,lpPos) ) 

#define ITestItemDisp_GetNextCond(This,plPos,pbstr)	\
    ( (This)->lpVtbl -> GetNextCond(This,plPos,pbstr) ) 

#define ITestItemDisp_AddCond(This,bstrCond)	\
    ( (This)->lpVtbl -> AddCond(This,bstrCond) ) 

#define ITestItemDisp_DeleteCond(This,lPos)	\
    ( (This)->lpVtbl -> DeleteCond(This,lPos) ) 

#define ITestItemDisp_DeleteAllCond(This)	\
    ( (This)->lpVtbl -> DeleteAllCond(This) ) 

#define ITestItemDisp_SetScript(This,bstrScript)	\
    ( (This)->lpVtbl -> SetScript(This,bstrScript) ) 

#define ITestItemDisp_GetScript(This,pbstrScript)	\
    ( (This)->lpVtbl -> GetScript(This,pbstrScript) ) 

#define ITestItemDisp_LoadContent(This)	\
    ( (This)->lpVtbl -> LoadContent(This) ) 

#define ITestItemDisp_StoreContent(This)	\
    ( (This)->lpVtbl -> StoreContent(This) ) 

#define ITestItemDisp_GetRunTime(This,pDate)	\
    ( (This)->lpVtbl -> GetRunTime(This,pDate) ) 

#define ITestItemDisp_SetRunTime(This,Date)	\
    ( (This)->lpVtbl -> SetRunTime(This,Date) ) 

#define ITestItemDisp_SetTestChecked(This,bSet)	\
    ( (This)->lpVtbl -> SetTestChecked(This,bSet) ) 

#define ITestItemDisp_GetTestChecked(This,pbSet)	\
    ( (This)->lpVtbl -> GetTestChecked(This,pbSet) ) 

#define ITestItemDisp_KillThemselvesFromDisk(This)	\
    ( (This)->lpVtbl -> KillThemselvesFromDisk(This) ) 

#define ITestItemDisp_ParseCond(This)	\
    ( (This)->lpVtbl -> ParseCond(This) ) 

#define ITestItemDisp_GetFirstSingleCond(This,lpPos)	\
    ( (This)->lpVtbl -> GetFirstSingleCond(This,lpPos) ) 

#define ITestItemDisp_NextSingleCond(This,lpPos,pVarName,pVarPath,pVarParams)	\
    ( (This)->lpVtbl -> NextSingleCond(This,lpPos,pVarName,pVarPath,pVarParams) ) 

#define ITestItemDisp_GetFirstMultyCond(This,lpPos)	\
    ( (This)->lpVtbl -> GetFirstMultyCond(This,lpPos) ) 

#define ITestItemDisp_NextMultyCond(This,lpPos,pVarName,pVarPath,pVarParams,pVarFlag,plCount,pVarNameMain,pVarParamsMain,plCountMain)	\
    ( (This)->lpVtbl -> NextMultyCond(This,lpPos,pVarName,pVarPath,pVarParams,pVarFlag,plCount,pVarNameMain,pVarParamsMain,plCountMain) ) 

#define ITestItemDisp_SetTestSavedState(This,bSet)	\
    ( (This)->lpVtbl -> SetTestSavedState(This,bSet) ) 

#define ITestItemDisp_GetTestSavedState(This,pbSet)	\
    ( (This)->lpVtbl -> GetTestSavedState(This,pbSet) ) 

#define ITestItemDisp_SetTestRunTime(This,dwTime)	\
    ( (This)->lpVtbl -> SetTestRunTime(This,dwTime) ) 

#define ITestItemDisp_GetTestRunTime(This,pdwTime)	\
    ( (This)->lpVtbl -> GetTestRunTime(This,pdwTime) ) 

#define ITestItemDisp_SetScriptFinal(This,bstrScript)	\
    ( (This)->lpVtbl -> SetScriptFinal(This,bstrScript) ) 

#define ITestItemDisp_GetScriptFinal(This,pbstrScript)	\
    ( (This)->lpVtbl -> GetScriptFinal(This,pbstrScript) ) 

#define ITestItemDisp_Rename(This,bstrNewName)	\
    ( (This)->lpVtbl -> Rename(This,bstrNewName) ) 

#define ITestItemDisp_GetAdditionalInfo(This,pbstrInfo)	\
    ( (This)->lpVtbl -> GetAdditionalInfo(This,pbstrInfo) ) 

#define ITestItemDisp_SetAdditionalInfo(This,bstrInfo)	\
    ( (This)->lpVtbl -> SetAdditionalInfo(This,bstrInfo) ) 

#define ITestItemDisp_GetSaveTime(This,pDate)	\
    ( (This)->lpVtbl -> GetSaveTime(This,pDate) ) 

#define ITestItemDisp_SetSaveTime(This,Date)	\
    ( (This)->lpVtbl -> SetSaveTime(This,Date) ) 

#define ITestItemDisp_GetTestErrorState(This,pbError)	\
    ( (This)->lpVtbl -> GetTestErrorState(This,pbError) ) 

#define ITestItemDisp_SetTestErrorState(This,bError)	\
    ( (This)->lpVtbl -> SetTestErrorState(This,bError) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestItemDisp_INTERFACE_DEFINED__ */


#ifndef __ITestDataArrayDisp_INTERFACE_DEFINED__
#define __ITestDataArrayDisp_INTERFACE_DEFINED__

/* interface ITestDataArrayDisp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ITestDataArrayDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("190D55A9-4888-4e61-AF0A-1F6C340BBE78")
    ITestDataArrayDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItem( 
            /* [in] */ VARIANT vData,
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItem( 
            /* [in] */ UINT nID,
            /* [retval][out] */ VARIANT *lpvRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemCount( 
            /* [retval][out] */ long *lplRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveItem( 
            /* [in] */ UINT nID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemName( 
            /* [in] */ UINT nID,
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItemCmpExpr( 
            /* [in] */ UINT nID,
            /* [in] */ BSTR bstrCmpExpr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemCmpExpr( 
            /* [in] */ UINT nID,
            /* [retval][out] */ BSTR *pbstrCmpExpr) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestDataArrayDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestDataArrayDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestDataArrayDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestDataArrayDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestDataArrayDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestDataArrayDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestDataArrayDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            ITestDataArrayDisp * This,
            /* [in] */ VARIANT vData,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT nID,
            /* [retval][out] */ VARIANT *lpvRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemCount )( 
            ITestDataArrayDisp * This,
            /* [retval][out] */ long *lplRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT nID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            ITestDataArrayDisp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemName )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT nID,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItemCmpExpr )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT nID,
            /* [in] */ BSTR bstrCmpExpr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemCmpExpr )( 
            ITestDataArrayDisp * This,
            /* [in] */ UINT nID,
            /* [retval][out] */ BSTR *pbstrCmpExpr);
        
        END_INTERFACE
    } ITestDataArrayDispVtbl;

    interface ITestDataArrayDisp
    {
        CONST_VTBL struct ITestDataArrayDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestDataArrayDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestDataArrayDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestDataArrayDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestDataArrayDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestDataArrayDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestDataArrayDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestDataArrayDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITestDataArrayDisp_AddItem(This,vData,bstrName)	\
    ( (This)->lpVtbl -> AddItem(This,vData,bstrName) ) 

#define ITestDataArrayDisp_GetItem(This,nID,lpvRetVal)	\
    ( (This)->lpVtbl -> GetItem(This,nID,lpvRetVal) ) 

#define ITestDataArrayDisp_GetItemCount(This,lplRet)	\
    ( (This)->lpVtbl -> GetItemCount(This,lplRet) ) 

#define ITestDataArrayDisp_RemoveItem(This,nID)	\
    ( (This)->lpVtbl -> RemoveItem(This,nID) ) 

#define ITestDataArrayDisp_RemoveAll(This)	\
    ( (This)->lpVtbl -> RemoveAll(This) ) 

#define ITestDataArrayDisp_GetItemName(This,nID,pbstrName)	\
    ( (This)->lpVtbl -> GetItemName(This,nID,pbstrName) ) 

#define ITestDataArrayDisp_SetItemCmpExpr(This,nID,bstrCmpExpr)	\
    ( (This)->lpVtbl -> SetItemCmpExpr(This,nID,bstrCmpExpr) ) 

#define ITestDataArrayDisp_GetItemCmpExpr(This,nID,pbstrCmpExpr)	\
    ( (This)->lpVtbl -> GetItemCmpExpr(This,nID,pbstrCmpExpr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestDataArrayDisp_INTERFACE_DEFINED__ */



#ifndef __TestManLib_LIBRARY_DEFINED__
#define __TestManLib_LIBRARY_DEFINED__

/* library TestManLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_TestManLib;

EXTERN_C const CLSID CLSID_CoTestManager;

#ifdef __cplusplus

class DECLSPEC_UUID("42F0E2B8-A151-45aa-891A-3A6E05981226")
CoTestManager;
#endif

EXTERN_C const CLSID CLSID_CoTestItem;

#ifdef __cplusplus

class DECLSPEC_UUID("10B89DC8-6DAF-4c20-9B9C-CF0B82C98AB0")
CoTestItem;
#endif

EXTERN_C const CLSID CLSID_CoTestDataArray;

#ifdef __cplusplus

class DECLSPEC_UUID("9C469E11-A284-40c4-A771-5271BCD77BAD")
CoTestDataArray;
#endif
#endif /* __TestManLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


