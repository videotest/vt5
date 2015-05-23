#pragma once
#include "resource.h"

#include "data_main.h"
#include "StorageObj_int.h"
#include "\vt5\common2\IDispatchImpl.h"
#include "thumbnail.h"
#include "\vt5\common2\misc_utils.h"
#include "misc_ptr.h"
#include "\vt5\awin\misc_list.h"

#define CURRENT_RECORD_NO_SET	-1
#define OBJECT_COUNT_NO_SET		CURRENT_RECORD_NO_SET


#define sNameInfoStream "Storage object info"
#define sDIBObjTypeName "Device-Independent Bitmap"

// storages names of storage of record
#define sObjectsStorage	"VideoTest's objects"
#define	sPrivateStorage	"Private" 
#define sPropBagsStorage "Properties bags"


inline BOOL StoreDIB( IStream  *ipStream, BYTE *pdib, DWORD dwSize )
{
	if( !pdib )
		return FALSE;

	if( FAILED( ipStream->Write( &dwSize, sizeof( dwSize ), 0 ) ) ) 
		return FALSE;

	if( dwSize < sizeof(BITMAPINFOHEADER) + ((BYTE)sizeof(TumbnailInfo)) )
		return FALSE;

	return SUCCEEDED( ipStream->Write( pdib, dwSize, 0 ) );
}

inline BOOL LoadDIB( IStream  *ipStream, BYTE **ppdib, DWORD *pdwSize = 0 )
{
    DWORD dwSize = -1;

	if( FAILED( ipStream->Read( &dwSize, sizeof( dwSize ), 0 ) ) ) 
		return FALSE;
		
	if( dwSize < 1 )
		return FALSE;

	if( pdwSize )
		*pdwSize = dwSize;

	*ppdib = new BYTE[ dwSize ];

	return SUCCEEDED( ipStream->Read( *ppdib, dwSize, 0 ) );
}

inline BOOL	FreeDIB( BYTE *pdib )
{
	if( !pdib)
		return FALSE;

	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return FALSE;

	if( FAILED( spThumbMan->ThumbnailFree( pdib ) ) )
		return FALSE;
	
	pdib = 0;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
//
// class storage_obj
//
//////////////////////////////////////////////////////////////////////
class storage_obj :	public CObjectBase,
					public IDispatchImpl< IStorageObject >,
					public IStorageObjectMisc,
					public _dyncreate_t<storage_obj>
{
	route_unknown();

	enum ObjectStates
	{
		osTemporaryFile		=	1 << 0,
		osSerializeContent	=	1 << 1,
	};

   	struct XStorageObjInfo
	{
		int		nVersion;
		ULONG	ulNextRecordNumber;

		XStorageObjInfo(void);
		~XStorageObjInfo(void);
        
		void	Reset(void);
		void	InitDefault(void);		
		ULONG	Serialize( IStream *ipStream, BOOL bIsStoring = TRUE );
	};

	struct XStreamObjInfo
	{
		int		nVersion;
		LPWSTR	lpwstrObjName;
		int		nSzObjName;
		bool	m_bIAllocText;
		
		XStreamObjInfo(void);
		~XStreamObjInfo(void);
        
		void	Reset(void);
		void	InitDefault(void);
		void	text_copy( LPCWSTR lpcwstr );
		ULONG	Serialize( IStream *ipStream, BOOL bIsStoring = TRUE );
	};
	
public:
	virtual IUnknown* DoGetInterface( const IID &iid );

public:
	storage_obj();
	virtual ~storage_obj();

	/////////////////////////////////////////////////
	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return CLSID_StorageObject; }

	/////////////////////////////////////////////////
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);

	/////////////////////////////////////////////////
	//IStorageObject
	com_call InitNew( /*[in]*/ BSTR bstr_file_name, VARIANT_BOOL bCreateTemp );
	com_call LoadFile( /*[in]*/ BSTR bstr_file_name );
	
	com_call InsertRecord();
	com_call DeleteRecord( /*[in]*/ long lrecord );

	com_call GetCurrentRecord( /*[out, retval]*/ long* plrecord );
	com_call SetCurrentRecord( /*[in]*/ long lrecord, DWORD dwFlags );
    com_call GetRecordCount( /*[out, retval]*/ long* plcount );

	com_call AddObject( /*[in] long plrecord,*/ /*[in]*/ IDispatch* pdisp_object );
    com_call GetObject( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type, /*[out, retval]*/ IDispatch** pdisp_object );
	com_call GetObjectName( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type, /*[out, retval]*/ BSTR* pbstr_name );
	com_call GetObjectCount( /*[in] long plrecord,*/ /*[out, retval]*/ long* plcount );
	com_call GetObjectType( /*[in] long plrecord,*/ /*[in]*/ long lidx, /*[out, retval]*/ BSTR* bstr_type );
	com_call DeleteObject( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type );

	com_call ExportRecord( /*[in] long lrecord,*/ /*[in]*/ IDispatch* pdisp_data );
	com_call ImportRecord( /*[in] long lrecord,*/ /*[in]*/ IDispatch* pdisp_data );
	com_call ClearDocument( /*[in] long lrecord,*/ /*[in]*/ IDispatch* pdisp_data );

	com_call GetExportRecord(  long *plrecord );

	com_call get_Name(BSTR *pbstr );
	com_call put_Name(BSTR bstr );

	// [vanek] 25.05.2004
	com_call get_SerializeContent(VARIANT_BOOL *pvbVal);
	com_call put_SerializeContent(VARIANT_BOOL vbNewVal);


	/////////////////////////////////////////////////
	//IStorageObjectMisc
	com_call GetRecordDib( /*[in] long pl_record,*/ /*[out]*/ BYTE** ppbi, /*[out]*/ DWORD* pdwSize );
	com_call GetObjectPropBag( /*[in]*/ BSTR bstr_type,  /*[out]*/ IUnknown **ppunkPropBag );

protected:
	IStorage				*m_ipSuperStorage;
	XStorageObjInfo			m_stSOInfo;
	_ptr_t<LPWSTR>			m_ptRecordsNames;
	SIZE					m_sizeDIB;
	DWORD					m_dwObjectState;
	LPWSTR					m_lpwstrFileName;

	// export records
	_list_t<long>			m_lExportRecords;	
	BOOL					m_bLockExportRecords;
	//
	
	// current record
	long					m_lCurrRecord;
	ULONG					m_ulObjectCount;
	//

	HRESULT	_attach( );

	BOOL	_init_storage_obj( );
	BOOL	_close_storage_obj( );
	BOOL	_commit( );
	void	_reset( );

	BOOL	_load_storage_info( );
	BOOL	_save_storage_info( );

	BOOL	_load_records_names( );
	void	_free_records_names( );
	BOOL	_reload_records_names( );
	
	HRESULT	_get_record_storage( long lrecord, IStorage** ppiStrorage, DWORD grfMode );
	HRESULT _get_create_sub_storage( LPWSTR lpwstrSubStorageName, IStorage* piStorage, IStorage** ppiSubStorage, DWORD grfMode, BOOL bCreateIfNoExist );
	
	BOOL	_calc_object_count( IStorage *piRecStorage = 0, IStorage *piObjectStorage = 0 );
	
	// DIB
	BOOL	_init_dib_size( );
	BOOL	_create_dib( IDispatch *pdisp_object, BYTE **ppdib, DWORD *pdwSize);
	// record
	HRESULT	_add_object( IStorage *ipRecordStorage, IStorage *ipObjectStorage, IDispatch* pdisp_object );
	HRESULT	_get_object(  IStorage *ipStorage, BSTR bstr_type, IDispatch** ppdisp_object, BOOL bOpenObjectsStorage = TRUE );
	HRESULT	_get_object_name(  IStorage *ipStorage, BSTR bstr_type, BSTR* pbstr_name, BOOL bOpenObjectsStorage = TRUE );
	
	// serialize content
	HRESULT	_serialize_content( IStream *pistream, BOOL bis_storing = TRUE  );
};

//////////////////////////////////////////////////////////////////////
//
// class storage_obj_info
//
//////////////////////////////////////////////////////////////////////
class storage_obj_info : public CDataInfoBase,
					public _dyncreate_t<storage_obj_info>
{
public:
	storage_obj_info()	: CDataInfoBase( CLSID_StorageObject, szTypeStorageObject, 0, IDI_STORAGE_OBJ )
	{	}
};
