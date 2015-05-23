

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:42:53 2015
 */
/* Compiler settings for avi.idl:
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


#ifndef __avi_h_h__
#define __avi_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAviObject_FWD_DEFINED__
#define __IAviObject_FWD_DEFINED__
typedef interface IAviObject IAviObject;

#endif 	/* __IAviObject_FWD_DEFINED__ */


#ifndef __IAviObject2_FWD_DEFINED__
#define __IAviObject2_FWD_DEFINED__
typedef interface IAviObject2 IAviObject2;

#endif 	/* __IAviObject2_FWD_DEFINED__ */


#ifndef __IAviDibImage_FWD_DEFINED__
#define __IAviDibImage_FWD_DEFINED__
typedef interface IAviDibImage IAviDibImage;

#endif 	/* __IAviDibImage_FWD_DEFINED__ */


#ifndef __IAviImage_FWD_DEFINED__
#define __IAviImage_FWD_DEFINED__
typedef interface IAviImage IAviImage;

#endif 	/* __IAviImage_FWD_DEFINED__ */


#ifndef __AviImage_FWD_DEFINED__
#define __AviImage_FWD_DEFINED__

#ifdef __cplusplus
typedef class AviImage AviImage;
#else
typedef struct AviImage AviImage;
#endif /* __cplusplus */

#endif 	/* __AviImage_FWD_DEFINED__ */


#ifndef __IAviPlayPropPage_FWD_DEFINED__
#define __IAviPlayPropPage_FWD_DEFINED__
typedef interface IAviPlayPropPage IAviPlayPropPage;

#endif 	/* __IAviPlayPropPage_FWD_DEFINED__ */


#ifndef __IInteractiveAviPlayAction_FWD_DEFINED__
#define __IInteractiveAviPlayAction_FWD_DEFINED__
typedef interface IInteractiveAviPlayAction IInteractiveAviPlayAction;

#endif 	/* __IInteractiveAviPlayAction_FWD_DEFINED__ */


#ifndef __IAviGalleryPropPage_FWD_DEFINED__
#define __IAviGalleryPropPage_FWD_DEFINED__
typedef interface IAviGalleryPropPage IAviGalleryPropPage;

#endif 	/* __IAviGalleryPropPage_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AviLib_LIBRARY_DEFINED__
#define __AviLib_LIBRARY_DEFINED__

/* library AviLib */
/* [helpstring][version][uuid] */ 


enum AviLocation
    {
        alFile	= 0,
        alDocument	= 1
    } ;
#define	szTypeAviImage	( "AviImage" )


EXTERN_C const IID LIBID_AviLib;

#ifndef __IAviObject_INTERFACE_DEFINED__
#define __IAviObject_INTERFACE_DEFINED__

/* interface IAviObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAviObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3EE8C5C0-4144-4dbf-9AEF-12D97135FCE4")
    IAviObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateLinkToFile( 
            BSTR bstrPath) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLinkToFile )( 
            IAviObject * This,
            BSTR bstrPath);
        
        END_INTERFACE
    } IAviObjectVtbl;

    interface IAviObject
    {
        CONST_VTBL struct IAviObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviObject_CreateLinkToFile(This,bstrPath)	\
    ( (This)->lpVtbl -> CreateLinkToFile(This,bstrPath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviObject_INTERFACE_DEFINED__ */


#ifndef __IAviObject2_INTERFACE_DEFINED__
#define __IAviObject2_INTERFACE_DEFINED__

/* interface IAviObject2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAviObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("238609AA-9345-463b-BB13-77B3E858A7AC")
    IAviObject2 : public IAviObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAviFileName( 
            BSTR *pbstrPath) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLinkToFile )( 
            IAviObject2 * This,
            BSTR bstrPath);
        
        HRESULT ( STDMETHODCALLTYPE *GetAviFileName )( 
            IAviObject2 * This,
            BSTR *pbstrPath);
        
        END_INTERFACE
    } IAviObject2Vtbl;

    interface IAviObject2
    {
        CONST_VTBL struct IAviObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviObject2_CreateLinkToFile(This,bstrPath)	\
    ( (This)->lpVtbl -> CreateLinkToFile(This,bstrPath) ) 


#define IAviObject2_GetAviFileName(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetAviFileName(This,pbstrPath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviObject2_INTERFACE_DEFINED__ */


#ifndef __IAviDibImage_INTERFACE_DEFINED__
#define __IAviDibImage_INTERFACE_DEFINED__

/* interface IAviDibImage */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAviDibImage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E20D3187-D2E6-458a-8BAB-AF5F61241B58")
    IAviDibImage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBitmapInfoHeader( 
            long lFrame,
            long *plSize,
            BYTE *pBuf) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDibBitsSize( 
            BYTE *pbi,
            long *plSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDibBits( 
            long lFrame,
            BYTE *pDibBits,
            long *plPrevKeyFrame,
            long *plPrevFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNearestKeyFrame( 
            long lFrame,
            long *plKeyFrame) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviDibImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviDibImage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviDibImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviDibImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBitmapInfoHeader )( 
            IAviDibImage * This,
            long lFrame,
            long *plSize,
            BYTE *pBuf);
        
        HRESULT ( STDMETHODCALLTYPE *GetDibBitsSize )( 
            IAviDibImage * This,
            BYTE *pbi,
            long *plSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetDibBits )( 
            IAviDibImage * This,
            long lFrame,
            BYTE *pDibBits,
            long *plPrevKeyFrame,
            long *plPrevFrame);
        
        HRESULT ( STDMETHODCALLTYPE *GetNearestKeyFrame )( 
            IAviDibImage * This,
            long lFrame,
            long *plKeyFrame);
        
        END_INTERFACE
    } IAviDibImageVtbl;

    interface IAviDibImage
    {
        CONST_VTBL struct IAviDibImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviDibImage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviDibImage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviDibImage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviDibImage_GetBitmapInfoHeader(This,lFrame,plSize,pBuf)	\
    ( (This)->lpVtbl -> GetBitmapInfoHeader(This,lFrame,plSize,pBuf) ) 

#define IAviDibImage_GetDibBitsSize(This,pbi,plSize)	\
    ( (This)->lpVtbl -> GetDibBitsSize(This,pbi,plSize) ) 

#define IAviDibImage_GetDibBits(This,lFrame,pDibBits,plPrevKeyFrame,plPrevFrame)	\
    ( (This)->lpVtbl -> GetDibBits(This,lFrame,pDibBits,plPrevKeyFrame,plPrevFrame) ) 

#define IAviDibImage_GetNearestKeyFrame(This,lFrame,plKeyFrame)	\
    ( (This)->lpVtbl -> GetNearestKeyFrame(This,lFrame,plKeyFrame) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviDibImage_INTERFACE_DEFINED__ */


#ifndef __IAviImage_INTERFACE_DEFINED__
#define __IAviImage_INTERFACE_DEFINED__

/* interface IAviImage */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IAviImage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D6EA332A-BC7A-422b-A73C-5C8D7CF51626")
    IAviImage : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [retval][out] */ VARIANT *pv_width) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [retval][out] */ VARIANT *pv_height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateNew( 
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ BSTR bstr_color_system) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPosX( 
            /* [retval][out] */ VARIANT *pv_x) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPosY( 
            /* [retval][out] */ VARIANT *pv_y) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOffset( 
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ VARIANT_BOOL bMove) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorSystem( 
            /* [retval][out] */ VARIANT *pv_color_system) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorsCount( 
            /* [retval][out] */ VARIANT *pv_count) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCalibration( 
            /* [ref][out] */ VARIANT *varCalibr,
            /* [ref][out] */ VARIANT *varGUID) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TotalFrames( 
            /* [retval][out] */ VARIANT *pv_count) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CurFrame( 
            /* [retval][out] */ VARIANT *pv_frame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveTo( 
            /* [in] */ long lframe,
            /* [in] */ long lflags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FramePerSecond( 
            /* [retval][out] */ VARIANT *pv_frames) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DoubleBuffering( 
            /* [retval][out] */ VARIANT *pv_double_buf) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DoubleBuffering( 
            /* [in] */ VARIANT_BOOL bDoubleBuffering) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EvenThenOdd( 
            /* [retval][out] */ VARIANT *pv_EvenThenOdd) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EvenThenOdd( 
            /* [in] */ VARIANT_BOOL bEvenThenOdd) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EvenStageRowOffset( 
            /* [retval][out] */ VARIANT *pv_offset) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EvenStageRowOffset( 
            /* [in] */ long loffset) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_OddStageRowOffset( 
            /* [retval][out] */ VARIANT *pv_offset) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_OddStageRowOffset( 
            /* [in] */ long loffset) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ BSTR *pbstrType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrivateNamedData( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsVirtual( 
            /* [retval][out] */ VARIANT_BOOL *pvbIsVirtual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StoreData( 
            /* [in] */ long dwDirection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChildsCount( 
            /* [retval][out] */ long *plCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstChildPos( 
            /* [out][in] */ VARIANT *pvarPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastChildPos( 
            /* [out][in] */ VARIANT *pvarPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextChild( 
            /* [out][in] */ VARIANT *pvarPos,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrevChild( 
            /* [out][in] */ VARIANT *pvarPos,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsEmpty( 
            /* [retval][out] */ VARIANT_BOOL *pvbIsEmpty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsPrivateNamedDataExist( 
            /* [retval][out] */ VARIANT_BOOL *pvbExist) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReloadSettings( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviImage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAviImage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAviImage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAviImage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAviImage * This,
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
        
        HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_width);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeight )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_height);
        
        HRESULT ( STDMETHODCALLTYPE *CreateNew )( 
            IAviImage * This,
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ BSTR bstr_color_system);
        
        HRESULT ( STDMETHODCALLTYPE *GetPosX )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_x);
        
        HRESULT ( STDMETHODCALLTYPE *GetPosY )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_y);
        
        HRESULT ( STDMETHODCALLTYPE *SetOffset )( 
            IAviImage * This,
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ VARIANT_BOOL bMove);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorSystem )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_color_system);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorsCount )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_count);
        
        HRESULT ( STDMETHODCALLTYPE *GetCalibration )( 
            IAviImage * This,
            /* [ref][out] */ VARIANT *varCalibr,
            /* [ref][out] */ VARIANT *varGUID);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TotalFrames )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_count);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurFrame )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_frame);
        
        HRESULT ( STDMETHODCALLTYPE *MoveTo )( 
            IAviImage * This,
            /* [in] */ long lframe,
            /* [in] */ long lflags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FramePerSecond )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_frames);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_DoubleBuffering )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_double_buf);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_DoubleBuffering )( 
            IAviImage * This,
            /* [in] */ VARIANT_BOOL bDoubleBuffering);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_EvenThenOdd )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_EvenThenOdd);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_EvenThenOdd )( 
            IAviImage * This,
            /* [in] */ VARIANT_BOOL bEvenThenOdd);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_EvenStageRowOffset )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_offset);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_EvenStageRowOffset )( 
            IAviImage * This,
            /* [in] */ long loffset);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_OddStageRowOffset )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT *pv_offset);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_OddStageRowOffset )( 
            IAviImage * This,
            /* [in] */ long loffset);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IAviImage * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IAviImage * This,
            /* [in] */ BSTR newVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IAviImage * This,
            /* [retval][out] */ BSTR *pbstrType);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrivateNamedData )( 
            IAviImage * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        HRESULT ( STDMETHODCALLTYPE *IsVirtual )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT_BOOL *pvbIsVirtual);
        
        HRESULT ( STDMETHODCALLTYPE *StoreData )( 
            IAviImage * This,
            /* [in] */ long dwDirection);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IAviImage * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        HRESULT ( STDMETHODCALLTYPE *GetChildsCount )( 
            IAviImage * This,
            /* [retval][out] */ long *plCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstChildPos )( 
            IAviImage * This,
            /* [out][in] */ VARIANT *pvarPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastChildPos )( 
            IAviImage * This,
            /* [out][in] */ VARIANT *pvarPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextChild )( 
            IAviImage * This,
            /* [out][in] */ VARIANT *pvarPos,
            /* [retval][out] */ IDispatch **ppDisp);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrevChild )( 
            IAviImage * This,
            /* [out][in] */ VARIANT *pvarPos,
            /* [retval][out] */ IDispatch **ppDisp);
        
        HRESULT ( STDMETHODCALLTYPE *IsEmpty )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT_BOOL *pvbIsEmpty);
        
        HRESULT ( STDMETHODCALLTYPE *IsPrivateNamedDataExist )( 
            IAviImage * This,
            /* [retval][out] */ VARIANT_BOOL *pvbExist);
        
        HRESULT ( STDMETHODCALLTYPE *ReloadSettings )( 
            IAviImage * This);
        
        END_INTERFACE
    } IAviImageVtbl;

    interface IAviImage
    {
        CONST_VTBL struct IAviImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviImage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviImage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviImage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviImage_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAviImage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAviImage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAviImage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAviImage_GetWidth(This,pv_width)	\
    ( (This)->lpVtbl -> GetWidth(This,pv_width) ) 

#define IAviImage_GetHeight(This,pv_height)	\
    ( (This)->lpVtbl -> GetHeight(This,pv_height) ) 

#define IAviImage_CreateNew(This,cx,cy,bstr_color_system)	\
    ( (This)->lpVtbl -> CreateNew(This,cx,cy,bstr_color_system) ) 

#define IAviImage_GetPosX(This,pv_x)	\
    ( (This)->lpVtbl -> GetPosX(This,pv_x) ) 

#define IAviImage_GetPosY(This,pv_y)	\
    ( (This)->lpVtbl -> GetPosY(This,pv_y) ) 

#define IAviImage_SetOffset(This,cx,cy,bMove)	\
    ( (This)->lpVtbl -> SetOffset(This,cx,cy,bMove) ) 

#define IAviImage_GetColorSystem(This,pv_color_system)	\
    ( (This)->lpVtbl -> GetColorSystem(This,pv_color_system) ) 

#define IAviImage_GetColorsCount(This,pv_count)	\
    ( (This)->lpVtbl -> GetColorsCount(This,pv_count) ) 

#define IAviImage_GetCalibration(This,varCalibr,varGUID)	\
    ( (This)->lpVtbl -> GetCalibration(This,varCalibr,varGUID) ) 

#define IAviImage_get_TotalFrames(This,pv_count)	\
    ( (This)->lpVtbl -> get_TotalFrames(This,pv_count) ) 

#define IAviImage_get_CurFrame(This,pv_frame)	\
    ( (This)->lpVtbl -> get_CurFrame(This,pv_frame) ) 

#define IAviImage_MoveTo(This,lframe,lflags)	\
    ( (This)->lpVtbl -> MoveTo(This,lframe,lflags) ) 

#define IAviImage_get_FramePerSecond(This,pv_frames)	\
    ( (This)->lpVtbl -> get_FramePerSecond(This,pv_frames) ) 

#define IAviImage_get_DoubleBuffering(This,pv_double_buf)	\
    ( (This)->lpVtbl -> get_DoubleBuffering(This,pv_double_buf) ) 

#define IAviImage_put_DoubleBuffering(This,bDoubleBuffering)	\
    ( (This)->lpVtbl -> put_DoubleBuffering(This,bDoubleBuffering) ) 

#define IAviImage_get_EvenThenOdd(This,pv_EvenThenOdd)	\
    ( (This)->lpVtbl -> get_EvenThenOdd(This,pv_EvenThenOdd) ) 

#define IAviImage_put_EvenThenOdd(This,bEvenThenOdd)	\
    ( (This)->lpVtbl -> put_EvenThenOdd(This,bEvenThenOdd) ) 

#define IAviImage_get_EvenStageRowOffset(This,pv_offset)	\
    ( (This)->lpVtbl -> get_EvenStageRowOffset(This,pv_offset) ) 

#define IAviImage_put_EvenStageRowOffset(This,loffset)	\
    ( (This)->lpVtbl -> put_EvenStageRowOffset(This,loffset) ) 

#define IAviImage_get_OddStageRowOffset(This,pv_offset)	\
    ( (This)->lpVtbl -> get_OddStageRowOffset(This,pv_offset) ) 

#define IAviImage_put_OddStageRowOffset(This,loffset)	\
    ( (This)->lpVtbl -> put_OddStageRowOffset(This,loffset) ) 

#define IAviImage_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IAviImage_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IAviImage_GetType(This,pbstrType)	\
    ( (This)->lpVtbl -> GetType(This,pbstrType) ) 

#define IAviImage_GetPrivateNamedData(This,ppDisp)	\
    ( (This)->lpVtbl -> GetPrivateNamedData(This,ppDisp) ) 

#define IAviImage_IsVirtual(This,pvbIsVirtual)	\
    ( (This)->lpVtbl -> IsVirtual(This,pvbIsVirtual) ) 

#define IAviImage_StoreData(This,dwDirection)	\
    ( (This)->lpVtbl -> StoreData(This,dwDirection) ) 

#define IAviImage_GetParent(This,ppDisp)	\
    ( (This)->lpVtbl -> GetParent(This,ppDisp) ) 

#define IAviImage_GetChildsCount(This,plCount)	\
    ( (This)->lpVtbl -> GetChildsCount(This,plCount) ) 

#define IAviImage_GetFirstChildPos(This,pvarPos)	\
    ( (This)->lpVtbl -> GetFirstChildPos(This,pvarPos) ) 

#define IAviImage_GetLastChildPos(This,pvarPos)	\
    ( (This)->lpVtbl -> GetLastChildPos(This,pvarPos) ) 

#define IAviImage_GetNextChild(This,pvarPos,ppDisp)	\
    ( (This)->lpVtbl -> GetNextChild(This,pvarPos,ppDisp) ) 

#define IAviImage_GetPrevChild(This,pvarPos,ppDisp)	\
    ( (This)->lpVtbl -> GetPrevChild(This,pvarPos,ppDisp) ) 

#define IAviImage_IsEmpty(This,pvbIsEmpty)	\
    ( (This)->lpVtbl -> IsEmpty(This,pvbIsEmpty) ) 

#define IAviImage_IsPrivateNamedDataExist(This,pvbExist)	\
    ( (This)->lpVtbl -> IsPrivateNamedDataExist(This,pvbExist) ) 

#define IAviImage_ReloadSettings(This)	\
    ( (This)->lpVtbl -> ReloadSettings(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviImage_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AviImage;

#ifdef __cplusplus

class DECLSPEC_UUID("9590F8A9-C444-4f85-B85F-D6B88C42231F")
AviImage;
#endif

#ifndef __IAviPlayPropPage_INTERFACE_DEFINED__
#define __IAviPlayPropPage_INTERFACE_DEFINED__

/* interface IAviPlayPropPage */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAviPlayPropPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("488B155A-0AF8-45c1-914E-822470502CE9")
    IAviPlayPropPage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AttachAction( 
            IUnknown *punk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdatePosition( 
            long lFrame,
            long lFrameCount,
            double fFramesPerSecond) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateCtrls( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviPlayPropPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviPlayPropPage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviPlayPropPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviPlayPropPage * This);
        
        HRESULT ( STDMETHODCALLTYPE *AttachAction )( 
            IAviPlayPropPage * This,
            IUnknown *punk);
        
        HRESULT ( STDMETHODCALLTYPE *UpdatePosition )( 
            IAviPlayPropPage * This,
            long lFrame,
            long lFrameCount,
            double fFramesPerSecond);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateCtrls )( 
            IAviPlayPropPage * This);
        
        END_INTERFACE
    } IAviPlayPropPageVtbl;

    interface IAviPlayPropPage
    {
        CONST_VTBL struct IAviPlayPropPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviPlayPropPage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviPlayPropPage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviPlayPropPage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviPlayPropPage_AttachAction(This,punk)	\
    ( (This)->lpVtbl -> AttachAction(This,punk) ) 

#define IAviPlayPropPage_UpdatePosition(This,lFrame,lFrameCount,fFramesPerSecond)	\
    ( (This)->lpVtbl -> UpdatePosition(This,lFrame,lFrameCount,fFramesPerSecond) ) 

#define IAviPlayPropPage_UpdateCtrls(This)	\
    ( (This)->lpVtbl -> UpdateCtrls(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviPlayPropPage_INTERFACE_DEFINED__ */


#ifndef __IInteractiveAviPlayAction_INTERFACE_DEFINED__
#define __IInteractiveAviPlayAction_INTERFACE_DEFINED__

/* interface IInteractiveAviPlayAction */
/* [object][uuid] */ 


EXTERN_C const IID IID_IInteractiveAviPlayAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B2339221-D99B-4681-A9AD-ADED32C84F11")
    IInteractiveAviPlayAction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentFrame( 
            long lFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentFrame( 
            long *plFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsPlay( 
            BOOL *pbPlay) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAviImage( 
            IUnknown **punkAvi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoom( 
            double fZoom) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInteractiveAviPlayActionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInteractiveAviPlayAction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInteractiveAviPlayAction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInteractiveAviPlayAction * This);
        
        HRESULT ( STDMETHODCALLTYPE *Play )( 
            IInteractiveAviPlayAction * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IInteractiveAviPlayAction * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IInteractiveAviPlayAction * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentFrame )( 
            IInteractiveAviPlayAction * This,
            long lFrame);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentFrame )( 
            IInteractiveAviPlayAction * This,
            long *plFrame);
        
        HRESULT ( STDMETHODCALLTYPE *IsPlay )( 
            IInteractiveAviPlayAction * This,
            BOOL *pbPlay);
        
        HRESULT ( STDMETHODCALLTYPE *GetAviImage )( 
            IInteractiveAviPlayAction * This,
            IUnknown **punkAvi);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoom )( 
            IInteractiveAviPlayAction * This,
            double fZoom);
        
        END_INTERFACE
    } IInteractiveAviPlayActionVtbl;

    interface IInteractiveAviPlayAction
    {
        CONST_VTBL struct IInteractiveAviPlayActionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInteractiveAviPlayAction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInteractiveAviPlayAction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInteractiveAviPlayAction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInteractiveAviPlayAction_Play(This)	\
    ( (This)->lpVtbl -> Play(This) ) 

#define IInteractiveAviPlayAction_Pause(This)	\
    ( (This)->lpVtbl -> Pause(This) ) 

#define IInteractiveAviPlayAction_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IInteractiveAviPlayAction_SetCurrentFrame(This,lFrame)	\
    ( (This)->lpVtbl -> SetCurrentFrame(This,lFrame) ) 

#define IInteractiveAviPlayAction_GetCurrentFrame(This,plFrame)	\
    ( (This)->lpVtbl -> GetCurrentFrame(This,plFrame) ) 

#define IInteractiveAviPlayAction_IsPlay(This,pbPlay)	\
    ( (This)->lpVtbl -> IsPlay(This,pbPlay) ) 

#define IInteractiveAviPlayAction_GetAviImage(This,punkAvi)	\
    ( (This)->lpVtbl -> GetAviImage(This,punkAvi) ) 

#define IInteractiveAviPlayAction_SetZoom(This,fZoom)	\
    ( (This)->lpVtbl -> SetZoom(This,fZoom) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInteractiveAviPlayAction_INTERFACE_DEFINED__ */


#ifndef __IAviGalleryPropPage_INTERFACE_DEFINED__
#define __IAviGalleryPropPage_INTERFACE_DEFINED__

/* interface IAviGalleryPropPage */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAviGalleryPropPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("020513FF-DB67-4106-9131-0592EEAE164F")
    IAviGalleryPropPage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReadInfo( 
            IUnknown *punkAvi) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAviGalleryPropPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAviGalleryPropPage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAviGalleryPropPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAviGalleryPropPage * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReadInfo )( 
            IAviGalleryPropPage * This,
            IUnknown *punkAvi);
        
        END_INTERFACE
    } IAviGalleryPropPageVtbl;

    interface IAviGalleryPropPage
    {
        CONST_VTBL struct IAviGalleryPropPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAviGalleryPropPage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAviGalleryPropPage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAviGalleryPropPage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAviGalleryPropPage_ReadInfo(This,punkAvi)	\
    ( (This)->lpVtbl -> ReadInfo(This,punkAvi) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAviGalleryPropPage_INTERFACE_DEFINED__ */

#endif /* __AviLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


