// AviObj.h: interface for the CAviObj class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __aviobj_h__
#define __aviobj_h__

#include "avibase.h"
#include "\vt5\ifaces\avi_int.h"

#include "image5.h"
#include "misc_list.h"

#include <dshow.h>
//#include <qedit.h>
#include "\vt5\crypto\crypto.h"

class CAviObj : public CObjectBase,
				public IAviObject2,
				public IAviDibImage,
				//public IAviImage,				
				public IImage4,
				public _dyncreate_t<CAviObj>
{
	friend class XAviImageDisp;

	route_unknown();

public:
	virtual IUnknown *DoGetInterface( const IID &iid );

public:
	CAviObj();
	virtual ~CAviObj();

public:
	virtual void BeforeFinalRelease();

	
protected:	

	void InitDefaults();

	bool InitObject();
	void DeinitObject();

	void Free();
	
	//Initialization
	bool SetImageTypeInfo();

	//specified function
	//if pBuf == NULL, return buffer size
	bool		GetFrameInfo( long lFrame, BYTE* pBuf, long* plSize );
	bool		GetStreamInfo( AVISTREAMINFOW* pInfo );	
	long		GetFrameCount();
	//if pBuf == NULL, return buffer size
	bool		ReadNativeFrameData( long lFrame, BYTE* pBuf, long* plBufSize );


	long		m_lActiveFrame;
	bool		SetActiveFrame( long lFrame );	

	//some helpers
	HIC			FindDecompressor( BITMAPINFOHEADER* pbiIn, BITMAPINFOHEADER* pbiOut );
	bool		IsDecompressorCompatible( BITMAPINFOHEADER* pbiIn, BITMAPINFOHEADER* pbiOut, HIC hic );
	long		CalcBufSize( BITMAPINFOHEADER* pbi );	
		
	long		CalcFullDecompressBufSize( long lFrame );

	bool		FullDecompressFrame( long lFrame, BYTE* pDibBits );
	bool		DecompressFrame( long lFrame, BYTE* pDibBits, DWORD dwFlags );
	

	/////////////////////////////////////////////////
	//interface IAviObject
	com_call CreateLinkToFile( BSTR bstrPath );	
	/////////////////////////////////////////////////
	//interface IAviObject2
	com_call GetAviFileName( BSTR *pbstrPath );

	/////////////////////////////////////////////////
	//interface IAviDibImage
	com_call GetBitmapInfoHeader( long lFrame, long* plSize, BYTE* pBuf );
	com_call GetDibBitsSize( BYTE* pbi, long* plSize );
	com_call GetDibBits( long lFrame, BYTE* pDibBits, long* plPrevKeyFrame, long* plPrevFrame );
	com_call GetNearestKeyFrame( long lFrame, long* plKeyFrame );


	// [vanek] BT2000:3984 - 08.09.2004
	/////////////////////////////////////////////////
	//interface IAviImage
	class XAviImageDisp : public IAviImage
	{
		friend class CAviObj;
		CAviObj *m_pmainclass;

		ITypeLib*	m_pi_type_lib;
		ITypeInfo*	m_pi_type_info;
	public:
		XAviImageDisp( void );
		~XAviImageDisp( void );

		//Image disp:
		com_call GetWidth( VARIANT* pv_width );
		com_call GetHeight( VARIANT* pv_height );
		com_call CreateNew( long cx, long cy, BSTR bstr_color_system );
		com_call GetPosX( VARIANT* pv_x );
		com_call GetPosY( VARIANT* pv_y );
		com_call SetOffset( long cx, long cy, VARIANT_BOOL bMove);
		com_call GetColorSystem( VARIANT* pv_color_system );
		com_call GetColorsCount( VARIANT* pv_count );
		com_call GetCalibration( VARIANT* varCalibr, VARIANT* varGUID );

		//Avi disp:
		com_call get_TotalFrames( VARIANT * pv_count );
		com_call get_CurFrame( VARIANT * pv_frame );
		com_call MoveTo( long lFrame, long lflags );
		com_call get_FramePerSecond( VARIANT * pv_frames );

		com_call get_DoubleBuffering( VARIANT* pv_double_buf );//BOOL
		com_call put_DoubleBuffering( VARIANT_BOOL bDoubleBuffering );
		
		com_call get_EvenThenOdd( VARIANT* pv_EvenThenOdd );//BOOL
		com_call put_EvenThenOdd( VARIANT_BOOL bEvenThenOdd);

		com_call get_EvenStageRowOffset( VARIANT* pv_offset );//long
		com_call put_EvenStageRowOffset( long loffset );
		
		com_call get_OddStageRowOffset( VARIANT* pv_offset );//long
		com_call put_OddStageRowOffset( long loffset );				

		// VT5Object's common dispatch:
		com_call get_Name( BSTR* pVal);
		com_call put_Name( BSTR newVal);
        com_call GetType( /*[out, retval]*/ BSTR *pbstrType );
		com_call GetPrivateNamedData( /*[out, retval]*/ IDispatch **ppDisp );	
		com_call IsVirtual( /*[out, retval]*/ VARIANT_BOOL *pvbIsVirtual );
		com_call StoreData( /*[in]*/ long dwDirection );
		com_call GetParent( /*[out, retval]*/ IDispatch **ppDisp );
		com_call GetChildsCount( /*[out, retval]*/ long *plCount );
		com_call GetFirstChildPos( /*[in, out]*/ VARIANT *pvarPos );
		com_call GetLastChildPos( /*[in, out]*/ VARIANT *pvarPos );
		com_call GetNextChild( /*[in, out]*/ VARIANT *pvarPos, /*[out, retval]*/ IDispatch **ppDisp );
		com_call GetPrevChild( /*[in, out]*/ VARIANT *pvarPos, /*[out, retval]*/ IDispatch **ppDisp );
		com_call IsEmpty( /*[out, retval]*/ VARIANT_BOOL *pvbIsEmpty );
		com_call IsPrivateNamedDataExist( /*[out, retval]*/ VARIANT_BOOL *pvbExist );
        
		com_call ReloadSettings();


		com_call GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo);
		com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
		com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
		com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);

		// IUnknown
		com_call_ref AddRef();
		com_call_ref Release();
		com_call QueryInterface( const IID &iid, void **pret );
	};

	XAviImageDisp m_avidisp;    

	//Image creation helper
	bool		_CreateSrcImage( BYTE* bHeaderBuf, long lHeaderBufSize, BYTE* pbyteDIBBits, long lDibBitsBufSize );	
	void		copy_from_dib_to_img( BYTE* prgb_color, int cx, int nimg_row );

	bool		_InitImage( int cx, int cy );
	void		_DestroyImage();
	//image atributes
	_bstr_t		m_bstrCCName;
	color		**m_ppRows;		//"virtual" pointers to the image rows
	byte		**m_ppRowMasks;	//"virtual" pointers to the image masks
	color		*m_pcolors;		//"real" pointer to the image data
	color		*m_pcolors_src;
	byte		*m_pbytes;		//"real" pointer to the image data
	int			m_iPanesCount;
	long		m_nImageWidth;
	long		m_nImageHeight;

	IColorConvertor2Ptr m_ptrCC;
	bool		FireEventObjectChange();

	//double buffering
	short				m_nDoubleBufStage;

	bool				m_bEnableDoubleBuffering;
	bool				m_bEvenThenOdd;
	int					m_nEvenStageRowOffset;
	int					m_nOddStageRowOffset;
	
protected:
	_bstr_t				m_bstrAviFile;
	AviLocation			m_aviLocation;

	IAVIFile*			m_pAVIFile;
	IAVIStream*			m_pVideoStream;
	HIC					m_hicDecompressor;

	BYTE*				m_pDibBits;
	long				m_lDibSize;

	long				m_lPrevKeyFrame;
	long				m_lPrevFrame;

	bool				open_vfw();

//Direct Show Media Detector
protected:

	IMediaDet*			m_pi_det;
	double				m_fmd_frame_rate;
	double				m_fmd_stream_len;
	long				m_lmd_frame_count;	
	AM_MEDIA_TYPE		m_mt;

	bool				open_media_detector();
	bool				read_md_frame( long lframe );

	
	/////////////////////////////////////////////////
	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidAviObj; }

	/////////////////////////////////////////////////
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);



	//IImage
	com_call CreateImage( int cx, int cy, BSTR bstrCCNamed, int nPaneNum );
	com_call FreeImage();

	com_call GetColorConvertor( IUnknown **ppunk );

	com_call GetRow( int nRow, int nPane, color **ppcolor );
	com_call GetSize( int *pnCX, int *pnCY );

	//IImage2
	com_call CreateVirtual( RECT rect );
	com_call InitRowMasks();
	com_call GetRowMask( int nRow, BYTE **ppByte );

	com_call GetOffset( POINT *pt );
	com_call SetOffset( POINT pt, BOOL bMoveImage );
	com_call GetImageFlags( DWORD *pdwFlags );

	//IImage3
	com_call InitContours();
	com_call FreeContours();

	com_call GetContoursCount( int *piCount );
	com_call GetContour( int nPos, Contour **ppContour );
	com_call SetContour( int nPos, Contour *pContour );
	com_call AddContour( Contour *pContour );

	//IImage4
	com_call AddPane(int nPane );
	com_call RemovePane( int nPane );
	com_call GetPanesCount(int *nSz  );





	INamedDataObject2Ptr	m_ptrParent;
	IUnknownPtr				m_ptrNamedData;
	_list_t2<IUnknown*>		m_listChilds;	
	long					m_posActiveChild;
	long					m_posInParent;

	TPOS					FindChildPos( IUnknown* punkChild );
	bool					m_bDummyStub;



	com_call		AttachData( IUnknown *punkNamedData );	
	com_call		GetObjectFlags( DWORD *pdwObjectFlags );

	com_call CreateChild( IUnknown **ppunkChild );
	com_call GetParent( IUnknown **ppunkParent );
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ );
	
//for internal usage - it is called only from framework
	com_call RemoveChild( IUnknown *punkChild );
	com_call AddChild( IUnknown *punkChild );

	com_call GetChildsCount( long *plCount );
	com_call GetFirstChildPosition( LONG_PTR *plPos );
	com_call GetNextChild( LONG_PTR *plPos, IUnknown **ppunkChild );
	com_call GetLastChildPosition(LONG_PTR *plPos);
	com_call GetPrevChild(LONG_PTR *plPos, IUnknown **ppunkChild);
	
	virtual void	_DetachChildParent(); //отвязать всех чайлдов	

	com_call SetActiveChild( LONG_PTR lPos );
	com_call GetActiveChild( LONG_PTR *plPos );

	com_call GetObjectPosInParent( LONG_PTR *plPos ); // return object position in parent's object child list
	com_call SetObjectPosInParent( LONG_PTR lPos ); 

	com_call GetChildPos(IUnknown *punkChild, LONG_PTR *plPos);

protected:
	vlong *m_pSignatures; // подписи всех кадров
};


/*
class CAviObjInfo : public CDataInfoBase,
					public _dyncreate_t<CAviObjInfo>
{
	route_unknown();
public:		
	CAviObjInfo( );
	com_call CreateObject( IUnknown **ppunk );	
};
*/

/*
class CAviObjInfo : public ComObjectBase, 
					public INamedDataInfo,
					public CNamedObjectImpl,
					public _dyncreate_t<CAviObjInfo>
{
	route_unknown();
public:
	CAviObjInfo( const CLSID &clsid, const char *pszTypeName, const char *pszContainerTypeName, UINT nIcon );
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	com_call CreateObject( IUnknown **ppunk );
	com_call GetName( BSTR *pbstrName );
	com_call GetContainerType( BSTR *pbstrTypeName );
	com_call GetProgID( BSTR *pbstrProgID );
	com_call CreateUniqueName(BSTR *pbstrName);
	com_call GetObjectIcon(HICON* phIcon);
	com_call GetUserName( BSTR *pbstr );
protected:
	_bstr_t	m_bstrContainer;
	long	m_nIconID;
	CLSID	m_clsid;
};
*/


#endif // __aviobj_h__
