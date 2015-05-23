

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Dec 28 23:57:29 2005
 */
/* Compiler settings for .\monster.odl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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


#ifndef __monster_h_h__
#define __monster_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMonsterWindow_FWD_DEFINED__
#define __IMonsterWindow_FWD_DEFINED__
typedef interface IMonsterWindow IMonsterWindow;
#endif 	/* __IMonsterWindow_FWD_DEFINED__ */


#ifndef __MonsterWindow_FWD_DEFINED__
#define __MonsterWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class MonsterWindow MonsterWindow;
#else
typedef struct MonsterWindow MonsterWindow;
#endif /* __cplusplus */

#endif 	/* __MonsterWindow_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __Monster_LIBRARY_DEFINED__
#define __Monster_LIBRARY_DEFINED__

/* library Monster */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Monster,0x223D1DB4,0x70AD,0x11D3,0xA6,0x5B,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifndef __IMonsterWindow_DISPINTERFACE_DEFINED__
#define __IMonsterWindow_DISPINTERFACE_DEFINED__

/* dispinterface IMonsterWindow */
/* [uuid] */ 


DEFINE_GUID(DIID_IMonsterWindow,0x223D1DC1,0x70AD,0x11D3,0xA6,0x5B,0x00,0x90,0x27,0x59,0x95,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("223D1DC1-70AD-11D3-A65B-0090275995FE")
    IMonsterWindow : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMonsterWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMonsterWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMonsterWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMonsterWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMonsterWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMonsterWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMonsterWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMonsterWindow * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IMonsterWindowVtbl;

    interface IMonsterWindow
    {
        CONST_VTBL struct IMonsterWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMonsterWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMonsterWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMonsterWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMonsterWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMonsterWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMonsterWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMonsterWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMonsterWindow_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MonsterWindow,0x223D1DC2,0x70AD,0x11D3,0xA6,0x5B,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifdef __cplusplus

class DECLSPEC_UUID("223D1DC2-70AD-11D3-A65B-0090275995FE")
MonsterWindow;
#endif
#endif /* __Monster_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


