

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:15 2015
 */
/* Compiler settings for shell.odl:
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


#ifndef __shell_h_h__
#define __shell_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IShell_FWD_DEFINED__
#define __IShell_FWD_DEFINED__
typedef interface IShell IShell;

#endif 	/* __IShell_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


#ifndef __ICommandManager_FWD_DEFINED__
#define __ICommandManager_FWD_DEFINED__
typedef interface ICommandManager ICommandManager;

#endif 	/* __ICommandManager_FWD_DEFINED__ */


#ifndef __CommandManager_FWD_DEFINED__
#define __CommandManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class CommandManager CommandManager;
#else
typedef struct CommandManager CommandManager;
#endif /* __cplusplus */

#endif 	/* __CommandManager_FWD_DEFINED__ */


#ifndef __ISettingsWindow_FWD_DEFINED__
#define __ISettingsWindow_FWD_DEFINED__
typedef interface ISettingsWindow ISettingsWindow;

#endif 	/* __ISettingsWindow_FWD_DEFINED__ */


#ifndef __SettingsWindow_FWD_DEFINED__
#define __SettingsWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class SettingsWindow SettingsWindow;
#else
typedef struct SettingsWindow SettingsWindow;
#endif /* __cplusplus */

#endif 	/* __SettingsWindow_FWD_DEFINED__ */


#ifndef __IShellDockBar_FWD_DEFINED__
#define __IShellDockBar_FWD_DEFINED__
typedef interface IShellDockBar IShellDockBar;

#endif 	/* __IShellDockBar_FWD_DEFINED__ */


#ifndef __ShellDockBar_FWD_DEFINED__
#define __ShellDockBar_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellDockBar ShellDockBar;
#else
typedef struct ShellDockBar ShellDockBar;
#endif /* __cplusplus */

#endif 	/* __ShellDockBar_FWD_DEFINED__ */


#ifndef __IMainFrame_FWD_DEFINED__
#define __IMainFrame_FWD_DEFINED__
typedef interface IMainFrame IMainFrame;

#endif 	/* __IMainFrame_FWD_DEFINED__ */


#ifndef __MainFrame_FWD_DEFINED__
#define __MainFrame_FWD_DEFINED__

#ifdef __cplusplus
typedef class MainFrame MainFrame;
#else
typedef struct MainFrame MainFrame;
#endif /* __cplusplus */

#endif 	/* __MainFrame_FWD_DEFINED__ */


#ifndef __IShellDocManagerDisp_FWD_DEFINED__
#define __IShellDocManagerDisp_FWD_DEFINED__
typedef interface IShellDocManagerDisp IShellDocManagerDisp;

#endif 	/* __IShellDocManagerDisp_FWD_DEFINED__ */


#ifndef __IShellDocManagerEvents_FWD_DEFINED__
#define __IShellDocManagerEvents_FWD_DEFINED__
typedef interface IShellDocManagerEvents IShellDocManagerEvents;

#endif 	/* __IShellDocManagerEvents_FWD_DEFINED__ */


#ifndef __ShellDocManagerDisp_FWD_DEFINED__
#define __ShellDocManagerDisp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellDocManagerDisp ShellDocManagerDisp;
#else
typedef struct ShellDocManagerDisp ShellDocManagerDisp;
#endif /* __cplusplus */

#endif 	/* __ShellDocManagerDisp_FWD_DEFINED__ */


#ifndef __IStatusBarMan_FWD_DEFINED__
#define __IStatusBarMan_FWD_DEFINED__
typedef interface IStatusBarMan IStatusBarMan;

#endif 	/* __IStatusBarMan_FWD_DEFINED__ */


#ifndef __StatusBarMan_FWD_DEFINED__
#define __StatusBarMan_FWD_DEFINED__

#ifdef __cplusplus
typedef class StatusBarMan StatusBarMan;
#else
typedef struct StatusBarMan StatusBarMan;
#endif /* __cplusplus */

#endif 	/* __StatusBarMan_FWD_DEFINED__ */


#ifndef __IShellViewDisp_FWD_DEFINED__
#define __IShellViewDisp_FWD_DEFINED__
typedef interface IShellViewDisp IShellViewDisp;

#endif 	/* __IShellViewDisp_FWD_DEFINED__ */


#ifndef __ShellView_FWD_DEFINED__
#define __ShellView_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellView ShellView;
#else
typedef struct ShellView ShellView;
#endif /* __cplusplus */

#endif 	/* __ShellView_FWD_DEFINED__ */


#ifndef __IAppDisp_FWD_DEFINED__
#define __IAppDisp_FWD_DEFINED__
typedef interface IAppDisp IAppDisp;

#endif 	/* __IAppDisp_FWD_DEFINED__ */


#ifndef __AppDisp_FWD_DEFINED__
#define __AppDisp_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppDisp AppDisp;
#else
typedef struct AppDisp AppDisp;
#endif /* __cplusplus */

#endif 	/* __AppDisp_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Shell_LIBRARY_DEFINED__
#define __Shell_LIBRARY_DEFINED__

/* library Shell */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_Shell;

#ifndef __IShell_DISPINTERFACE_DEFINED__
#define __IShell_DISPINTERFACE_DEFINED__

/* dispinterface IShell */
/* [uuid] */ 


EXTERN_C const IID DIID_IShell;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C48CE9B7-F947-11D2-A596-0000B493FF1B")
    IShell : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShellVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShell * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShell * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShell * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShell * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShell * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShell * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShell * This,
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
    } IShellVtbl;

    interface IShell
    {
        CONST_VTBL struct IShellVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShell_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShell_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShell_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShell_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShell_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShell_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShell_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShell_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Document;

#ifdef __cplusplus

class DECLSPEC_UUID("C48CE9B5-F947-11D2-A596-0000B493FF1B")
Document;
#endif

#ifndef __ICommandManager_DISPINTERFACE_DEFINED__
#define __ICommandManager_DISPINTERFACE_DEFINED__

/* dispinterface ICommandManager */
/* [uuid] */ 


EXTERN_C const IID DIID_ICommandManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F49AE623-0446-11D3-A5A3-0000B493FF1B")
    ICommandManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICommandManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommandManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommandManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommandManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommandManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommandManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommandManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommandManager * This,
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
    } ICommandManagerVtbl;

    interface ICommandManager
    {
        CONST_VTBL struct ICommandManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommandManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommandManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommandManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommandManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommandManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommandManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommandManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICommandManager_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CommandManager;

#ifdef __cplusplus

class DECLSPEC_UUID("F49AE625-0446-11D3-A5A3-0000B493FF1B")
CommandManager;
#endif

#ifndef __ISettingsWindow_DISPINTERFACE_DEFINED__
#define __ISettingsWindow_DISPINTERFACE_DEFINED__

/* dispinterface ISettingsWindow */
/* [uuid] */ 


EXTERN_C const IID DIID_ISettingsWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("87F6BED3-11A3-11D3-A5BE-0000B493FF1B")
    ISettingsWindow : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ISettingsWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISettingsWindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISettingsWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISettingsWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISettingsWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISettingsWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISettingsWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISettingsWindow * This,
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
    } ISettingsWindowVtbl;

    interface ISettingsWindow
    {
        CONST_VTBL struct ISettingsWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISettingsWindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISettingsWindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISettingsWindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISettingsWindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISettingsWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISettingsWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISettingsWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ISettingsWindow_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SettingsWindow;

#ifdef __cplusplus

class DECLSPEC_UUID("87F6BED5-11A3-11D3-A5BE-0000B493FF1B")
SettingsWindow;
#endif

#ifndef __IShellDockBar_DISPINTERFACE_DEFINED__
#define __IShellDockBar_DISPINTERFACE_DEFINED__

/* dispinterface IShellDockBar */
/* [uuid] */ 


EXTERN_C const IID DIID_IShellDockBar;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("265274C4-1EFD-11D3-A5D3-0000B493FF1B")
    IShellDockBar : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShellDockBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDockBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDockBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDockBar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDockBar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDockBar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDockBar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDockBar * This,
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
    } IShellDockBarVtbl;

    interface IShellDockBar
    {
        CONST_VTBL struct IShellDockBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDockBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellDockBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellDockBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShellDockBar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShellDockBar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShellDockBar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShellDockBar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShellDockBar_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellDockBar;

#ifdef __cplusplus

class DECLSPEC_UUID("265274C6-1EFD-11D3-A5D3-0000B493FF1B")
ShellDockBar;
#endif

#ifndef __IMainFrame_DISPINTERFACE_DEFINED__
#define __IMainFrame_DISPINTERFACE_DEFINED__

/* dispinterface IMainFrame */
/* [uuid] */ 


EXTERN_C const IID DIID_IMainFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("05E1F7E3-38E6-11D3-A602-0090275995FE")
    IMainFrame : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMainFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMainFrame * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMainFrame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMainFrame * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMainFrame * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMainFrame * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMainFrame * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMainFrame * This,
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
    } IMainFrameVtbl;

    interface IMainFrame
    {
        CONST_VTBL struct IMainFrameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMainFrame_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMainFrame_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMainFrame_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMainFrame_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMainFrame_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMainFrame_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMainFrame_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMainFrame_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MainFrame;

#ifdef __cplusplus

class DECLSPEC_UUID("05E1F7E5-38E6-11D3-A602-0090275995FE")
MainFrame;
#endif

#ifndef __IShellDocManagerDisp_DISPINTERFACE_DEFINED__
#define __IShellDocManagerDisp_DISPINTERFACE_DEFINED__

/* dispinterface IShellDocManagerDisp */
/* [uuid] */ 


EXTERN_C const IID DIID_IShellDocManagerDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("4D6FBF33-39B6-11D3-A603-0090275995FE")
    IShellDocManagerDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShellDocManagerDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDocManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDocManagerDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDocManagerDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDocManagerDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDocManagerDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDocManagerDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDocManagerDisp * This,
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
    } IShellDocManagerDispVtbl;

    interface IShellDocManagerDisp
    {
        CONST_VTBL struct IShellDocManagerDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDocManagerDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellDocManagerDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellDocManagerDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShellDocManagerDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShellDocManagerDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShellDocManagerDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShellDocManagerDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShellDocManagerDisp_DISPINTERFACE_DEFINED__ */


#ifndef __IShellDocManagerEvents_DISPINTERFACE_DEFINED__
#define __IShellDocManagerEvents_DISPINTERFACE_DEFINED__

/* dispinterface IShellDocManagerEvents */
/* [uuid] */ 


EXTERN_C const IID DIID_IShellDocManagerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A23423C2-69D5-11d3-A64D-0090275995FE")
    IShellDocManagerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShellDocManagerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDocManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDocManagerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDocManagerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDocManagerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDocManagerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDocManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDocManagerEvents * This,
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
    } IShellDocManagerEventsVtbl;

    interface IShellDocManagerEvents
    {
        CONST_VTBL struct IShellDocManagerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDocManagerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellDocManagerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellDocManagerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShellDocManagerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShellDocManagerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShellDocManagerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShellDocManagerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShellDocManagerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellDocManagerDisp;

#ifdef __cplusplus

class DECLSPEC_UUID("A23423C4-69D5-11d3-A64D-0090275995FE")
ShellDocManagerDisp;
#endif

#ifndef __IStatusBarMan_DISPINTERFACE_DEFINED__
#define __IStatusBarMan_DISPINTERFACE_DEFINED__

/* dispinterface IStatusBarMan */
/* [uuid] */ 


EXTERN_C const IID DIID_IStatusBarMan;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("63F2E0D3-8AC1-11D3-A523-0000B493A187")
    IStatusBarMan : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IStatusBarManVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStatusBarMan * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStatusBarMan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStatusBarMan * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStatusBarMan * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStatusBarMan * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStatusBarMan * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStatusBarMan * This,
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
    } IStatusBarManVtbl;

    interface IStatusBarMan
    {
        CONST_VTBL struct IStatusBarManVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatusBarMan_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStatusBarMan_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStatusBarMan_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStatusBarMan_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStatusBarMan_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStatusBarMan_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStatusBarMan_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IStatusBarMan_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_StatusBarMan;

#ifdef __cplusplus

class DECLSPEC_UUID("63F2E0D4-8AC1-11D3-A523-0000B493A187")
StatusBarMan;
#endif

#ifndef __IShellViewDisp_DISPINTERFACE_DEFINED__
#define __IShellViewDisp_DISPINTERFACE_DEFINED__

/* dispinterface IShellViewDisp */
/* [uuid] */ 


EXTERN_C const IID DIID_IShellViewDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("28888205-51B2-47f2-9359-6EEA31DF0E47")
    IShellViewDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShellViewDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellViewDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellViewDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellViewDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellViewDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellViewDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellViewDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellViewDisp * This,
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
    } IShellViewDispVtbl;

    interface IShellViewDisp
    {
        CONST_VTBL struct IShellViewDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellViewDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellViewDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellViewDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShellViewDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShellViewDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShellViewDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShellViewDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShellViewDisp_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellView;

#ifdef __cplusplus

class DECLSPEC_UUID("E9DBCA3C-A3B7-40fd-8D44-BB0E0A130CB9")
ShellView;
#endif

#ifndef __IAppDisp_DISPINTERFACE_DEFINED__
#define __IAppDisp_DISPINTERFACE_DEFINED__

/* dispinterface IAppDisp */
/* [uuid] */ 


EXTERN_C const IID DIID_IAppDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BBC55473-AE6D-4864-AAF2-26969B0F00CB")
    IAppDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAppDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAppDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAppDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAppDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAppDisp * This,
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
    } IAppDispVtbl;

    interface IAppDisp
    {
        CONST_VTBL struct IAppDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAppDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAppDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAppDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAppDisp_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AppDisp;

#ifdef __cplusplus

class DECLSPEC_UUID("E52BCA9C-5664-46AB-A0A5-11BD9ACE9A7B")
AppDisp;
#endif
#endif /* __Shell_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


