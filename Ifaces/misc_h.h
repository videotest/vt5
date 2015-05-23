

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:43:07 2015
 */
/* Compiler settings for misc.idl:
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


#ifndef __misc_h_h__
#define __misc_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INamedObject2_FWD_DEFINED__
#define __INamedObject2_FWD_DEFINED__
typedef interface INamedObject2 INamedObject2;

#endif 	/* __INamedObject2_FWD_DEFINED__ */


#ifndef __INumeredObject_FWD_DEFINED__
#define __INumeredObject_FWD_DEFINED__
typedef interface INumeredObject INumeredObject;

#endif 	/* __INumeredObject_FWD_DEFINED__ */


#ifndef __IRootedObject_FWD_DEFINED__
#define __IRootedObject_FWD_DEFINED__
typedef interface IRootedObject IRootedObject;

#endif 	/* __IRootedObject_FWD_DEFINED__ */


#ifndef __ICompManager_FWD_DEFINED__
#define __ICompManager_FWD_DEFINED__
typedef interface ICompManager ICompManager;

#endif 	/* __ICompManager_FWD_DEFINED__ */


#ifndef __ICompRegistrator_FWD_DEFINED__
#define __ICompRegistrator_FWD_DEFINED__
typedef interface ICompRegistrator ICompRegistrator;

#endif 	/* __ICompRegistrator_FWD_DEFINED__ */


#ifndef __IEventListener_FWD_DEFINED__
#define __IEventListener_FWD_DEFINED__
typedef interface IEventListener IEventListener;

#endif 	/* __IEventListener_FWD_DEFINED__ */


#ifndef __INotifyController_FWD_DEFINED__
#define __INotifyController_FWD_DEFINED__
typedef interface INotifyController INotifyController;

#endif 	/* __INotifyController_FWD_DEFINED__ */


#ifndef __INotifyController2_FWD_DEFINED__
#define __INotifyController2_FWD_DEFINED__
typedef interface INotifyController2 INotifyController2;

#endif 	/* __INotifyController2_FWD_DEFINED__ */


#ifndef __IHelpInfo_FWD_DEFINED__
#define __IHelpInfo_FWD_DEFINED__
typedef interface IHelpInfo IHelpInfo;

#endif 	/* __IHelpInfo_FWD_DEFINED__ */


#ifndef __IHelpInfo2_FWD_DEFINED__
#define __IHelpInfo2_FWD_DEFINED__
typedef interface IHelpInfo2 IHelpInfo2;

#endif 	/* __IHelpInfo2_FWD_DEFINED__ */


#ifndef __IPropertySheet_FWD_DEFINED__
#define __IPropertySheet_FWD_DEFINED__
typedef interface IPropertySheet IPropertySheet;

#endif 	/* __IPropertySheet_FWD_DEFINED__ */


#ifndef __IStatusBar_FWD_DEFINED__
#define __IStatusBar_FWD_DEFINED__
typedef interface IStatusBar IStatusBar;

#endif 	/* __IStatusBar_FWD_DEFINED__ */


#ifndef __IProvideColors_FWD_DEFINED__
#define __IProvideColors_FWD_DEFINED__
typedef interface IProvideColors IProvideColors;

#endif 	/* __IProvideColors_FWD_DEFINED__ */


#ifndef __IClipboard_FWD_DEFINED__
#define __IClipboard_FWD_DEFINED__
typedef interface IClipboard IClipboard;

#endif 	/* __IClipboard_FWD_DEFINED__ */


#ifndef __IStateObject_FWD_DEFINED__
#define __IStateObject_FWD_DEFINED__
typedef interface IStateObject IStateObject;

#endif 	/* __IStateObject_FWD_DEFINED__ */


#ifndef __IInitializeObjectVT_FWD_DEFINED__
#define __IInitializeObjectVT_FWD_DEFINED__
typedef interface IInitializeObjectVT IInitializeObjectVT;

#endif 	/* __IInitializeObjectVT_FWD_DEFINED__ */


/* header files for imported files */
#include "miscdefs.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MiscTypeLib_LIBRARY_DEFINED__
#define __MiscTypeLib_LIBRARY_DEFINED__

/* library MiscTypeLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MiscTypeLib;

#ifndef __INamedObject2_INTERFACE_DEFINED__
#define __INamedObject2_INTERFACE_DEFINED__

/* interface INamedObject2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_INamedObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67A8-2220-11d3-A5D6-0000B493FF1B")
    INamedObject2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            BSTR *pbstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserName( 
            BSTR *pbstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetName( 
            BSTR bstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetUserName( 
            BSTR bstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GenerateUniqueName( 
            BSTR bstrBase) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INamedObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INamedObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INamedObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INamedObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            INamedObject2 * This,
            BSTR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE *GetUserName )( 
            INamedObject2 * This,
            BSTR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE *SetName )( 
            INamedObject2 * This,
            BSTR bstr);
        
        HRESULT ( STDMETHODCALLTYPE *SetUserName )( 
            INamedObject2 * This,
            BSTR bstr);
        
        HRESULT ( STDMETHODCALLTYPE *GenerateUniqueName )( 
            INamedObject2 * This,
            BSTR bstrBase);
        
        END_INTERFACE
    } INamedObject2Vtbl;

    interface INamedObject2
    {
        CONST_VTBL struct INamedObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INamedObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INamedObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INamedObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INamedObject2_GetName(This,pbstr)	\
    ( (This)->lpVtbl -> GetName(This,pbstr) ) 

#define INamedObject2_GetUserName(This,pbstr)	\
    ( (This)->lpVtbl -> GetUserName(This,pbstr) ) 

#define INamedObject2_SetName(This,bstr)	\
    ( (This)->lpVtbl -> SetName(This,bstr) ) 

#define INamedObject2_SetUserName(This,bstr)	\
    ( (This)->lpVtbl -> SetUserName(This,bstr) ) 

#define INamedObject2_GenerateUniqueName(This,bstrBase)	\
    ( (This)->lpVtbl -> GenerateUniqueName(This,bstrBase) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INamedObject2_INTERFACE_DEFINED__ */


#ifndef __INumeredObject_INTERFACE_DEFINED__
#define __INumeredObject_INTERFACE_DEFINED__

/* interface INumeredObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_INumeredObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67A9-2220-11d3-A5D6-0000B493FF1B")
    INumeredObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AssignKey( 
            GUID Key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKey( 
            GUID *pKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCreateNum( 
            long *plNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GenerateNewKey( 
            GUID *pKey) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INumeredObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INumeredObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INumeredObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INumeredObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *AssignKey )( 
            INumeredObject * This,
            GUID Key);
        
        HRESULT ( STDMETHODCALLTYPE *GetKey )( 
            INumeredObject * This,
            GUID *pKey);
        
        HRESULT ( STDMETHODCALLTYPE *GetCreateNum )( 
            INumeredObject * This,
            long *plNum);
        
        HRESULT ( STDMETHODCALLTYPE *GenerateNewKey )( 
            INumeredObject * This,
            GUID *pKey);
        
        END_INTERFACE
    } INumeredObjectVtbl;

    interface INumeredObject
    {
        CONST_VTBL struct INumeredObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INumeredObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INumeredObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INumeredObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INumeredObject_AssignKey(This,Key)	\
    ( (This)->lpVtbl -> AssignKey(This,Key) ) 

#define INumeredObject_GetKey(This,pKey)	\
    ( (This)->lpVtbl -> GetKey(This,pKey) ) 

#define INumeredObject_GetCreateNum(This,plNum)	\
    ( (This)->lpVtbl -> GetCreateNum(This,plNum) ) 

#define INumeredObject_GenerateNewKey(This,pKey)	\
    ( (This)->lpVtbl -> GenerateNewKey(This,pKey) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INumeredObject_INTERFACE_DEFINED__ */


#ifndef __IRootedObject_INTERFACE_DEFINED__
#define __IRootedObject_INTERFACE_DEFINED__

/* interface IRootedObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IRootedObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67AB-2220-11d3-A5D6-0000B493FF1B")
    IRootedObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AttachParent( 
            IUnknown *punkParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            IUnknown **punkParent) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRootedObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRootedObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRootedObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRootedObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *AttachParent )( 
            IRootedObject * This,
            IUnknown *punkParent);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IRootedObject * This,
            IUnknown **punkParent);
        
        END_INTERFACE
    } IRootedObjectVtbl;

    interface IRootedObject
    {
        CONST_VTBL struct IRootedObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRootedObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRootedObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRootedObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRootedObject_AttachParent(This,punkParent)	\
    ( (This)->lpVtbl -> AttachParent(This,punkParent) ) 

#define IRootedObject_GetParent(This,punkParent)	\
    ( (This)->lpVtbl -> GetParent(This,punkParent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRootedObject_INTERFACE_DEFINED__ */


#ifndef __ICompManager_INTERFACE_DEFINED__
#define __ICompManager_INTERFACE_DEFINED__

/* interface ICompManager */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICompManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67AC-2220-11d3-A5D6-0000B493FF1B")
    ICompManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            int *piCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadComponents( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeComponents( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponentUnknownByIdx( 
            int idx,
            IUnknown **ppunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponentUnknownByIID( 
            const IID iid,
            IUnknown **ppunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponentUnknownByCLSID( 
            const CLSID clsid,
            IUnknown **ppunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddComponent( 
            IUnknown *punk,
            int *piIdx) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveComponent( 
            int idx) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            ICompManager * This,
            int *piCount);
        
        HRESULT ( STDMETHODCALLTYPE *LoadComponents )( 
            ICompManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *FreeComponents )( 
            ICompManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentUnknownByIdx )( 
            ICompManager * This,
            int idx,
            IUnknown **ppunk);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentUnknownByIID )( 
            ICompManager * This,
            const IID iid,
            IUnknown **ppunk);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentUnknownByCLSID )( 
            ICompManager * This,
            const CLSID clsid,
            IUnknown **ppunk);
        
        HRESULT ( STDMETHODCALLTYPE *AddComponent )( 
            ICompManager * This,
            IUnknown *punk,
            int *piIdx);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveComponent )( 
            ICompManager * This,
            int idx);
        
        END_INTERFACE
    } ICompManagerVtbl;

    interface ICompManager
    {
        CONST_VTBL struct ICompManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompManager_GetCount(This,piCount)	\
    ( (This)->lpVtbl -> GetCount(This,piCount) ) 

#define ICompManager_LoadComponents(This)	\
    ( (This)->lpVtbl -> LoadComponents(This) ) 

#define ICompManager_FreeComponents(This)	\
    ( (This)->lpVtbl -> FreeComponents(This) ) 

#define ICompManager_GetComponentUnknownByIdx(This,idx,ppunk)	\
    ( (This)->lpVtbl -> GetComponentUnknownByIdx(This,idx,ppunk) ) 

#define ICompManager_GetComponentUnknownByIID(This,iid,ppunk)	\
    ( (This)->lpVtbl -> GetComponentUnknownByIID(This,iid,ppunk) ) 

#define ICompManager_GetComponentUnknownByCLSID(This,clsid,ppunk)	\
    ( (This)->lpVtbl -> GetComponentUnknownByCLSID(This,clsid,ppunk) ) 

#define ICompManager_AddComponent(This,punk,piIdx)	\
    ( (This)->lpVtbl -> AddComponent(This,punk,piIdx) ) 

#define ICompManager_RemoveComponent(This,idx)	\
    ( (This)->lpVtbl -> RemoveComponent(This,idx) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompManager_INTERFACE_DEFINED__ */


#ifndef __ICompRegistrator_INTERFACE_DEFINED__
#define __ICompRegistrator_INTERFACE_DEFINED__

/* interface ICompRegistrator */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICompRegistrator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67AD-2220-11d3-A5D6-0000B493FF1B")
    ICompRegistrator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            int *piCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCompInfo( 
            int idx,
            BSTR *pbstr) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompRegistratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompRegistrator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompRegistrator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompRegistrator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            ICompRegistrator * This,
            int *piCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetCompInfo )( 
            ICompRegistrator * This,
            int idx,
            BSTR *pbstr);
        
        END_INTERFACE
    } ICompRegistratorVtbl;

    interface ICompRegistrator
    {
        CONST_VTBL struct ICompRegistratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompRegistrator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompRegistrator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompRegistrator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompRegistrator_GetCount(This,piCount)	\
    ( (This)->lpVtbl -> GetCount(This,piCount) ) 

#define ICompRegistrator_GetCompInfo(This,idx,pbstr)	\
    ( (This)->lpVtbl -> GetCompInfo(This,idx,pbstr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompRegistrator_INTERFACE_DEFINED__ */


#ifndef __IEventListener_INTERFACE_DEFINED__
#define __IEventListener_INTERFACE_DEFINED__

/* interface IEventListener */
/* [object][uuid] */ 


EXTERN_C const IID IID_IEventListener;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67AE-2220-11d3-A5D6-0000B493FF1B")
    IEventListener : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Notify( 
            BSTR szEventDesc,
            IUnknown *pHint,
            IUnknown *pFrom,
            void *pdata,
            long cbSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsInNotify( 
            BOOL *pbFlag) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEventListenerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEventListener * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEventListener * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEventListener * This);
        
        HRESULT ( STDMETHODCALLTYPE *Notify )( 
            IEventListener * This,
            BSTR szEventDesc,
            IUnknown *pHint,
            IUnknown *pFrom,
            void *pdata,
            long cbSize);
        
        HRESULT ( STDMETHODCALLTYPE *IsInNotify )( 
            IEventListener * This,
            BOOL *pbFlag);
        
        END_INTERFACE
    } IEventListenerVtbl;

    interface IEventListener
    {
        CONST_VTBL struct IEventListenerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventListener_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEventListener_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEventListener_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEventListener_Notify(This,szEventDesc,pHint,pFrom,pdata,cbSize)	\
    ( (This)->lpVtbl -> Notify(This,szEventDesc,pHint,pFrom,pdata,cbSize) ) 

#define IEventListener_IsInNotify(This,pbFlag)	\
    ( (This)->lpVtbl -> IsInNotify(This,pbFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEventListener_INTERFACE_DEFINED__ */


#ifndef __INotifyController_INTERFACE_DEFINED__
#define __INotifyController_INTERFACE_DEFINED__

/* interface INotifyController */
/* [object][uuid] */ 


EXTERN_C const IID IID_INotifyController;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51CE67B3-2220-11d3-A5D6-0000B493FF1B")
    INotifyController : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FireEvent( 
            BSTR szEventDesc,
            IUnknown *pHint,
            IUnknown *pFrom,
            void *pdata,
            long cbSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterEventListener( 
            BSTR szEventDesc,
            IUnknown *pListener) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegisterEventListener( 
            BSTR szEventDesc,
            IUnknown *pListener) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsRegisteredEventListener( 
            BSTR szEventDesc,
            IUnknown *pListener,
            BOOL *pbFlag) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INotifyControllerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INotifyController * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INotifyController * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INotifyController * This);
        
        HRESULT ( STDMETHODCALLTYPE *FireEvent )( 
            INotifyController * This,
            BSTR szEventDesc,
            IUnknown *pHint,
            IUnknown *pFrom,
            void *pdata,
            long cbSize);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterEventListener )( 
            INotifyController * This,
            BSTR szEventDesc,
            IUnknown *pListener);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterEventListener )( 
            INotifyController * This,
            BSTR szEventDesc,
            IUnknown *pListener);
        
        HRESULT ( STDMETHODCALLTYPE *IsRegisteredEventListener )( 
            INotifyController * This,
            BSTR szEventDesc,
            IUnknown *pListener,
            BOOL *pbFlag);
        
        END_INTERFACE
    } INotifyControllerVtbl;

    interface INotifyController
    {
        CONST_VTBL struct INotifyControllerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotifyController_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INotifyController_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INotifyController_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INotifyController_FireEvent(This,szEventDesc,pHint,pFrom,pdata,cbSize)	\
    ( (This)->lpVtbl -> FireEvent(This,szEventDesc,pHint,pFrom,pdata,cbSize) ) 

#define INotifyController_RegisterEventListener(This,szEventDesc,pListener)	\
    ( (This)->lpVtbl -> RegisterEventListener(This,szEventDesc,pListener) ) 

#define INotifyController_UnRegisterEventListener(This,szEventDesc,pListener)	\
    ( (This)->lpVtbl -> UnRegisterEventListener(This,szEventDesc,pListener) ) 

#define INotifyController_IsRegisteredEventListener(This,szEventDesc,pListener,pbFlag)	\
    ( (This)->lpVtbl -> IsRegisteredEventListener(This,szEventDesc,pListener,pbFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INotifyController_INTERFACE_DEFINED__ */


#ifndef __INotifyController2_INTERFACE_DEFINED__
#define __INotifyController2_INTERFACE_DEFINED__

/* interface INotifyController2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_INotifyController2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EEB71132-0F95-41f6-A851-8E985B8A7076")
    INotifyController2 : public INotifyController
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddSource( 
            IUnknown *punkSrc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveSource( 
            IUnknown *punkSrc) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INotifyController2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INotifyController2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INotifyController2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INotifyController2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *FireEvent )( 
            INotifyController2 * This,
            BSTR szEventDesc,
            IUnknown *pHint,
            IUnknown *pFrom,
            void *pdata,
            long cbSize);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterEventListener )( 
            INotifyController2 * This,
            BSTR szEventDesc,
            IUnknown *pListener);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterEventListener )( 
            INotifyController2 * This,
            BSTR szEventDesc,
            IUnknown *pListener);
        
        HRESULT ( STDMETHODCALLTYPE *IsRegisteredEventListener )( 
            INotifyController2 * This,
            BSTR szEventDesc,
            IUnknown *pListener,
            BOOL *pbFlag);
        
        HRESULT ( STDMETHODCALLTYPE *AddSource )( 
            INotifyController2 * This,
            IUnknown *punkSrc);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveSource )( 
            INotifyController2 * This,
            IUnknown *punkSrc);
        
        END_INTERFACE
    } INotifyController2Vtbl;

    interface INotifyController2
    {
        CONST_VTBL struct INotifyController2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotifyController2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INotifyController2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INotifyController2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INotifyController2_FireEvent(This,szEventDesc,pHint,pFrom,pdata,cbSize)	\
    ( (This)->lpVtbl -> FireEvent(This,szEventDesc,pHint,pFrom,pdata,cbSize) ) 

#define INotifyController2_RegisterEventListener(This,szEventDesc,pListener)	\
    ( (This)->lpVtbl -> RegisterEventListener(This,szEventDesc,pListener) ) 

#define INotifyController2_UnRegisterEventListener(This,szEventDesc,pListener)	\
    ( (This)->lpVtbl -> UnRegisterEventListener(This,szEventDesc,pListener) ) 

#define INotifyController2_IsRegisteredEventListener(This,szEventDesc,pListener,pbFlag)	\
    ( (This)->lpVtbl -> IsRegisteredEventListener(This,szEventDesc,pListener,pbFlag) ) 


#define INotifyController2_AddSource(This,punkSrc)	\
    ( (This)->lpVtbl -> AddSource(This,punkSrc) ) 

#define INotifyController2_RemoveSource(This,punkSrc)	\
    ( (This)->lpVtbl -> RemoveSource(This,punkSrc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INotifyController2_INTERFACE_DEFINED__ */


#ifndef __IHelpInfo_INTERFACE_DEFINED__
#define __IHelpInfo_INTERFACE_DEFINED__

/* interface IHelpInfo */
/* [object][uuid] */ 


EXTERN_C const IID IID_IHelpInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("911C6B01-5AEB-11d3-A635-0090275995FE")
    IHelpInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHelpInfo( 
            BSTR *pbstrHelpFile,
            BSTR *pbstrHelpTopic,
            DWORD *pdwFlags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IHelpInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHelpInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHelpInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHelpInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHelpInfo )( 
            IHelpInfo * This,
            BSTR *pbstrHelpFile,
            BSTR *pbstrHelpTopic,
            DWORD *pdwFlags);
        
        END_INTERFACE
    } IHelpInfoVtbl;

    interface IHelpInfo
    {
        CONST_VTBL struct IHelpInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHelpInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHelpInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHelpInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHelpInfo_GetHelpInfo(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags)	\
    ( (This)->lpVtbl -> GetHelpInfo(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHelpInfo_INTERFACE_DEFINED__ */


#ifndef __IHelpInfo2_INTERFACE_DEFINED__
#define __IHelpInfo2_INTERFACE_DEFINED__

/* interface IHelpInfo2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_IHelpInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AD1B4F26-AA9A-49df-815B-299509E2CBDA")
    IHelpInfo2 : public IHelpInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHelpInfo2( 
            BSTR *pbstrHelpFile,
            BSTR *pbstrHelpTopic,
            DWORD *pdwFlags,
            BSTR *pbstrHelpPrefix) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IHelpInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHelpInfo2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHelpInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHelpInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHelpInfo )( 
            IHelpInfo2 * This,
            BSTR *pbstrHelpFile,
            BSTR *pbstrHelpTopic,
            DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetHelpInfo2 )( 
            IHelpInfo2 * This,
            BSTR *pbstrHelpFile,
            BSTR *pbstrHelpTopic,
            DWORD *pdwFlags,
            BSTR *pbstrHelpPrefix);
        
        END_INTERFACE
    } IHelpInfo2Vtbl;

    interface IHelpInfo2
    {
        CONST_VTBL struct IHelpInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHelpInfo2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHelpInfo2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHelpInfo2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHelpInfo2_GetHelpInfo(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags)	\
    ( (This)->lpVtbl -> GetHelpInfo(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags) ) 


#define IHelpInfo2_GetHelpInfo2(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags,pbstrHelpPrefix)	\
    ( (This)->lpVtbl -> GetHelpInfo2(This,pbstrHelpFile,pbstrHelpTopic,pdwFlags,pbstrHelpPrefix) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHelpInfo2_INTERFACE_DEFINED__ */


#ifndef __IPropertySheet_INTERFACE_DEFINED__
#define __IPropertySheet_INTERFACE_DEFINED__

/* interface IPropertySheet */
/* [object][uuid] */ 


EXTERN_C const IID IID_IPropertySheet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("59C0F710-7FE5-11d3-A50C-0000B493A187")
    IPropertySheet : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IncludePage( 
            int nPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExcludePage( 
            int nPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPagesCount( 
            int *pNumPages) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPage( 
            int nPage,
            IUnknown **ppunkPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ActivatePage( 
            int nPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetActivePage( 
            int *pnPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetModified( 
            BOOL bSetModified) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPropertySheetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertySheet * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertySheet * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertySheet * This);
        
        HRESULT ( STDMETHODCALLTYPE *IncludePage )( 
            IPropertySheet * This,
            int nPage);
        
        HRESULT ( STDMETHODCALLTYPE *ExcludePage )( 
            IPropertySheet * This,
            int nPage);
        
        HRESULT ( STDMETHODCALLTYPE *GetPagesCount )( 
            IPropertySheet * This,
            int *pNumPages);
        
        HRESULT ( STDMETHODCALLTYPE *GetPage )( 
            IPropertySheet * This,
            int nPage,
            IUnknown **ppunkPage);
        
        HRESULT ( STDMETHODCALLTYPE *ActivatePage )( 
            IPropertySheet * This,
            int nPage);
        
        HRESULT ( STDMETHODCALLTYPE *GetActivePage )( 
            IPropertySheet * This,
            int *pnPage);
        
        HRESULT ( STDMETHODCALLTYPE *SetModified )( 
            IPropertySheet * This,
            BOOL bSetModified);
        
        END_INTERFACE
    } IPropertySheetVtbl;

    interface IPropertySheet
    {
        CONST_VTBL struct IPropertySheetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertySheet_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertySheet_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertySheet_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertySheet_IncludePage(This,nPage)	\
    ( (This)->lpVtbl -> IncludePage(This,nPage) ) 

#define IPropertySheet_ExcludePage(This,nPage)	\
    ( (This)->lpVtbl -> ExcludePage(This,nPage) ) 

#define IPropertySheet_GetPagesCount(This,pNumPages)	\
    ( (This)->lpVtbl -> GetPagesCount(This,pNumPages) ) 

#define IPropertySheet_GetPage(This,nPage,ppunkPage)	\
    ( (This)->lpVtbl -> GetPage(This,nPage,ppunkPage) ) 

#define IPropertySheet_ActivatePage(This,nPage)	\
    ( (This)->lpVtbl -> ActivatePage(This,nPage) ) 

#define IPropertySheet_GetActivePage(This,pnPage)	\
    ( (This)->lpVtbl -> GetActivePage(This,pnPage) ) 

#define IPropertySheet_SetModified(This,bSetModified)	\
    ( (This)->lpVtbl -> SetModified(This,bSetModified) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPropertySheet_INTERFACE_DEFINED__ */


#ifndef __IStatusBar_INTERFACE_DEFINED__
#define __IStatusBar_INTERFACE_DEFINED__

/* interface IStatusBar */
/* [object][uuid] */ 


EXTERN_C const IID IID_IStatusBar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1362B112-8AE6-11d3-A523-0000B493A187")
    IStatusBar : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IncludePane( 
            IUnknown *punkPane,
            int *pNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExcludePane( 
            int nNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveState( 
            IUnknown *punkNamedData,
            BSTR bstrSection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadState( 
            IUnknown *punkNamedData,
            BSTR bstrSection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandle( 
            HANDLE *phWnd) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStatusBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStatusBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStatusBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStatusBar * This);
        
        HRESULT ( STDMETHODCALLTYPE *IncludePane )( 
            IStatusBar * This,
            IUnknown *punkPane,
            int *pNum);
        
        HRESULT ( STDMETHODCALLTYPE *ExcludePane )( 
            IStatusBar * This,
            int nNum);
        
        HRESULT ( STDMETHODCALLTYPE *SaveState )( 
            IStatusBar * This,
            IUnknown *punkNamedData,
            BSTR bstrSection);
        
        HRESULT ( STDMETHODCALLTYPE *LoadState )( 
            IStatusBar * This,
            IUnknown *punkNamedData,
            BSTR bstrSection);
        
        HRESULT ( STDMETHODCALLTYPE *GetHandle )( 
            IStatusBar * This,
            HANDLE *phWnd);
        
        END_INTERFACE
    } IStatusBarVtbl;

    interface IStatusBar
    {
        CONST_VTBL struct IStatusBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatusBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStatusBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStatusBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStatusBar_IncludePane(This,punkPane,pNum)	\
    ( (This)->lpVtbl -> IncludePane(This,punkPane,pNum) ) 

#define IStatusBar_ExcludePane(This,nNum)	\
    ( (This)->lpVtbl -> ExcludePane(This,nNum) ) 

#define IStatusBar_SaveState(This,punkNamedData,bstrSection)	\
    ( (This)->lpVtbl -> SaveState(This,punkNamedData,bstrSection) ) 

#define IStatusBar_LoadState(This,punkNamedData,bstrSection)	\
    ( (This)->lpVtbl -> LoadState(This,punkNamedData,bstrSection) ) 

#define IStatusBar_GetHandle(This,phWnd)	\
    ( (This)->lpVtbl -> GetHandle(This,phWnd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStatusBar_INTERFACE_DEFINED__ */


#ifndef __IProvideColors_INTERFACE_DEFINED__
#define __IProvideColors_INTERFACE_DEFINED__

/* interface IProvideColors */
/* [object][uuid] */ 


EXTERN_C const IID IID_IProvideColors;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AB4386E1-FE60-11d3-A0C5-0000B493A187")
    IProvideColors : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetColorsInfo( 
            long *pnCount,
            DWORD **ppdwColors) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProvideColorsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProvideColors * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProvideColors * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProvideColors * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorsInfo )( 
            IProvideColors * This,
            long *pnCount,
            DWORD **ppdwColors);
        
        END_INTERFACE
    } IProvideColorsVtbl;

    interface IProvideColors
    {
        CONST_VTBL struct IProvideColorsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProvideColors_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProvideColors_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProvideColors_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProvideColors_GetColorsInfo(This,pnCount,ppdwColors)	\
    ( (This)->lpVtbl -> GetColorsInfo(This,pnCount,ppdwColors) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProvideColors_INTERFACE_DEFINED__ */


#ifndef __IClipboard_INTERFACE_DEFINED__
#define __IClipboard_INTERFACE_DEFINED__

/* interface IClipboard */
/* [object][uuid] */ 


EXTERN_C const IID IID_IClipboard;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D1EFC4A4-F6F0-4667-96B5-957BBCC2C4B8")
    IClipboard : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFirstFormatPosition( 
            long *pnPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextFormat( 
            UINT *pnFormat,
            long *pnPosNext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Copy( 
            UINT nFormat,
            HANDLE *pHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Paste( 
            UINT nFormat,
            HANDLE Handle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsSupportPaste( 
            BOOL *pboolSupportPaste) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IClipboardVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClipboard * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClipboard * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClipboard * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstFormatPosition )( 
            IClipboard * This,
            long *pnPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextFormat )( 
            IClipboard * This,
            UINT *pnFormat,
            long *pnPosNext);
        
        HRESULT ( STDMETHODCALLTYPE *Copy )( 
            IClipboard * This,
            UINT nFormat,
            HANDLE *pHandle);
        
        HRESULT ( STDMETHODCALLTYPE *Paste )( 
            IClipboard * This,
            UINT nFormat,
            HANDLE Handle);
        
        HRESULT ( STDMETHODCALLTYPE *IsSupportPaste )( 
            IClipboard * This,
            BOOL *pboolSupportPaste);
        
        END_INTERFACE
    } IClipboardVtbl;

    interface IClipboard
    {
        CONST_VTBL struct IClipboardVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClipboard_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClipboard_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClipboard_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClipboard_GetFirstFormatPosition(This,pnPos)	\
    ( (This)->lpVtbl -> GetFirstFormatPosition(This,pnPos) ) 

#define IClipboard_GetNextFormat(This,pnFormat,pnPosNext)	\
    ( (This)->lpVtbl -> GetNextFormat(This,pnFormat,pnPosNext) ) 

#define IClipboard_Copy(This,nFormat,pHandle)	\
    ( (This)->lpVtbl -> Copy(This,nFormat,pHandle) ) 

#define IClipboard_Paste(This,nFormat,Handle)	\
    ( (This)->lpVtbl -> Paste(This,nFormat,Handle) ) 

#define IClipboard_IsSupportPaste(This,pboolSupportPaste)	\
    ( (This)->lpVtbl -> IsSupportPaste(This,pboolSupportPaste) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClipboard_INTERFACE_DEFINED__ */


#ifndef __IStateObject_INTERFACE_DEFINED__
#define __IStateObject_INTERFACE_DEFINED__

/* interface IStateObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IStateObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("55049B03-6244-42d9-AFFA-51F751494F76")
    IStateObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StoreState( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStateObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStateObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStateObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStateObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *LoadState )( 
            IStateObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *StoreState )( 
            IStateObject * This);
        
        END_INTERFACE
    } IStateObjectVtbl;

    interface IStateObject
    {
        CONST_VTBL struct IStateObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStateObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStateObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStateObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStateObject_LoadState(This)	\
    ( (This)->lpVtbl -> LoadState(This) ) 

#define IStateObject_StoreState(This)	\
    ( (This)->lpVtbl -> StoreState(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStateObject_INTERFACE_DEFINED__ */


#ifndef __IInitializeObjectVT_INTERFACE_DEFINED__
#define __IInitializeObjectVT_INTERFACE_DEFINED__

/* interface IInitializeObjectVT */
/* [object][uuid] */ 


EXTERN_C const IID IID_IInitializeObjectVT;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F09F8272-2A8D-4c32-9517-A24524DEECC4")
    IInitializeObjectVT : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInitializeObjectVTVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInitializeObjectVT * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInitializeObjectVT * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInitializeObjectVT * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IInitializeObjectVT * This);
        
        END_INTERFACE
    } IInitializeObjectVTVtbl;

    interface IInitializeObjectVT
    {
        CONST_VTBL struct IInitializeObjectVTVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInitializeObjectVT_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInitializeObjectVT_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInitializeObjectVT_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInitializeObjectVT_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInitializeObjectVT_INTERFACE_DEFINED__ */

#endif /* __MiscTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


