#pragma once

//////////////////////////////////////////////////////////////////////
//
//	class stream_file
//
//////////////////////////////////////////////////////////////////////
class stream_file : public IStream
{
public:
	stream_file();
	virtual ~stream_file();

	HRESULT	open(	const char* psz_file, DWORD dw_desired_access = GENERIC_READ|GENERIC_WRITE, 
					DWORD dw_share_mode = FILE_SHARE_READ, 
					DWORD dw_creation_disposition = OPEN_ALWAYS );
	HRESULT	close();

protected:
	HANDLE	m_hfile;

public:


    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if( riid == IID_IStream )
		{
			*ppvObject = (void*)(IStream*)this;
			return S_OK;
		}
		return E_NOINTERFACE;
	}
    
	virtual ULONG STDMETHODCALLTYPE AddRef(){ return 1L;}
	virtual ULONG STDMETHODCALLTYPE Release(){ return 1L;}
	//IStream
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [length_is][size_is][out] */ void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbRead);
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [size_is][in] */ const void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbWritten);

        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition);
        
        virtual HRESULT STDMETHODCALLTYPE SetSize( 
            /* [in] */ ULARGE_INTEGER libNewSize)
		{		return E_NOTIMPL;		}
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [unique][in] */ IStream *pstm,
            /* [in] */ ULARGE_INTEGER cb,
            /* [out] */ ULARGE_INTEGER *pcbRead,
            /* [out] */ ULARGE_INTEGER *pcbWritten)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ DWORD grfCommitFlags)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE Revert( void)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE LockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            /* [out] */ STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag)
		{		return E_NOTIMPL;		}
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IStream **ppstm)
		{		return E_NOTIMPL;		}

};

//////////////////////////////////////////////////////////////////////
inline stream_file::stream_file()
{
	m_hfile = 0;
}

//////////////////////////////////////////////////////////////////////
inline stream_file::~stream_file()
{
	close();
}

//////////////////////////////////////////////////////////////////////
inline HRESULT	stream_file::open(	const char* psz_file, DWORD dw_desired_access,
							DWORD dw_share_mode, 
							DWORD dw_creation_disposition )
{
	if( !psz_file )
		return E_INVALIDARG;

	HANDLE h = ::CreateFile( psz_file, dw_desired_access, dw_share_mode, 0, dw_creation_disposition, 0, 0 );
	if( h == INVALID_HANDLE_VALUE )
		return GetLastError();

	m_hfile = h;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
inline HRESULT stream_file::close()
{
	if( m_hfile )
		::CloseHandle( m_hfile );	m_hfile = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
inline HRESULT stream_file::Read( 
            /* [length_is][size_is][out] */ void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbRead)
{
	if( !m_hfile )
		return E_FAIL;	
	if( !ReadFile( m_hfile, pv, cb, pcbRead, 0 ) )
		return GetLastError();

	return S_OK;
}
        
//////////////////////////////////////////////////////////////////////
inline HRESULT stream_file::Write( 
            /* [size_is][in] */ const void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbWritten)
{
	if( !m_hfile )
		return E_FAIL;

	if( !WriteFile( m_hfile, pv, cb, pcbWritten, 0 ) )
		return GetLastError();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
inline HRESULT stream_file::Seek( 
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition)
{
	return E_NOTIMPL;
}

/*
DWOR dw_size = 0;
HANDLE h = ::CreateFile( char, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 )

HANDLE hmap = CreateFileMapping( hfile, 0, PAGE_READWRITE, 0, 0, 0 );
BYTE* pbuf = (BYTE*)::MapViewOfFile( hmap, FILE_MAP_ALL_ACCESS, 0, 0, dw_file_size );
UnMapViewOfFile( pbuf );
CloseHandle( hmap );
CloseHandle( hfile );
*/





