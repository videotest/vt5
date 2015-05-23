#include "StdAfx.h"
#include "filestoragehelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CFileStorageHelper::CFileStorageHelper(void)
{
	m_str_path.Empty( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CFileStorageHelper::~CFileStorageHelper(void)
{
	CloseStorage( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::CreateStorage( LPCTSTR lpFileName )
{
	if( !lpFileName )
		return E_INVALIDARG;

    if( m_sptr_storage )
		CloseStorage( );
    
	HRESULT hr = S_FALSE;
	hr = ::StgCreateDocfile( _bstr_t( lpFileName ), 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, &m_sptr_storage );
	if( hr != S_OK )
		return hr;

    m_str_path = lpFileName;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::OpenStorage( LPCTSTR lpFileName )
{
    if( !lpFileName )
		return E_INVALIDARG;

	if( m_sptr_storage )
		CloseStorage( );

	HRESULT hr = S_FALSE;
	hr = ::StgOpenStorage( _bstr_t( lpFileName ), 0, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, 0, &m_sptr_storage );
	if( hr != S_OK )
		return hr;

	m_str_path = lpFileName;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::CloseStorage( )
{
	m_sptr_storage = 0;
	m_str_path.Empty( );
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::FillByPath( LPCTSTR lpPath )
{
	if( !lpPath )
		return E_INVALIDARG;

	if(  m_sptr_storage == 0 )
		return S_FALSE;

	return dir2storage( m_sptr_storage, lpPath );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::FlushContent( LPCTSTR lpFlushTo )
{
	if( !lpFlushTo )
		return E_INVALIDARG;

	if( m_sptr_storage == 0 )
		return S_FALSE;

	if( ::CreateDirectory( lpFlushTo, NULL ) )
		return storage2dir( m_sptr_storage, lpFlushTo );
	else
		return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::dir2storage( IStorage *pstg, LPCTSTR lpdir_path )
{
    if( !pstg || !lpdir_path )    
		return E_INVALIDARG;

	CString str_full_dir_path = lpdir_path;
	int nlength = str_full_dir_path.GetLength();
	if( !nlength )
		return E_INVALIDARG;

	if( str_full_dir_path[ nlength - 1 ] != _T('\\') )
		str_full_dir_path += _T("\\");
    
	HANDLE hfind = 0;
	WIN32_FIND_DATA st_found = {0};
	CString str_find(_T("*.*"));
	str_find = str_full_dir_path + str_find;
	hfind = ::FindFirstFile( str_find, &st_found );
	if( hfind == INVALID_HANDLE_VALUE )
		return S_FALSE;
	
	BOOL bcontinue = TRUE;
	do
	{
		if( st_found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{	// directory
			if( (_tcsncmp( st_found.cFileName, _T("."), 1 ) != 0) && (_tcsncmp( st_found.cFileName, _T(".."), 2 ) != 0) )
			{
				IStoragePtr sptr_dir_storage;
				if( S_OK == pstg->CreateStorage( _bstr_t( st_found.cFileName ), 
					STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0, &sptr_dir_storage ) )
				{
					HRESULT hr = S_FALSE;
                    hr = dir2storage( sptr_dir_storage, str_full_dir_path + st_found.cFileName );
					if( hr != S_OK )
						return hr;
				}
			}
		}
		else 
		{	// file
			IStreamPtr	sptr_file_stream;
            if( S_OK == pstg->CreateStream( _bstr_t( st_found.cFileName ), 
					STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0, &sptr_file_stream ) )
			{
				HRESULT hr = S_FALSE;
				hr = file2stream( sptr_file_stream, str_full_dir_path + st_found.cFileName );
				if( hr != S_OK )
					return hr;
			}

		}

		bcontinue = ::FindNextFile( hfind, &st_found );
	}
	while( bcontinue );
	::FindClose( hfind );	hfind = 0;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::file2stream( IStream *pstm, LPCTSTR lpfile_path )
{
    if( !pstm || !lpfile_path )    
		return E_INVALIDARG;

	// open file
    HANDLE hFile = 0;
	hFile = CreateFile( lpfile_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if( INVALID_HANDLE_VALUE == hFile )
		return S_FALSE;

	// get file size
    DWORD dwFileSize = 0;
	dwFileSize = GetFileSize(hFile, NULL);
	if( dwFileSize == INVALID_FILE_SIZE )
	{
		::CloseHandle( hFile );	hFile = 0;
		return S_FALSE;
	}

    LPVOID pvData = NULL;
    // alloc memory based on file size
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
    _ASSERTE(NULL != hGlobal);

    pvData = GlobalLock(hGlobal);
    if( pvData )
	{
		DWORD dwBytesRead = 0;
		// read file and store in global memory
		BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	}

	GlobalUnlock(hGlobal);
    CloseHandle(hFile);	hFile = 0;
    
	IStreamPtr sptr_stm;
    // create IStream* from global memory
    if( S_OK == CreateStreamOnHGlobal( hGlobal, TRUE, &sptr_stm ) )
	{
       	ULARGE_INTEGER size = {0};
		size.LowPart = dwFileSize;
		HRESULT hr = sptr_stm->CopyTo( pstm, size, 0, 0 );
		if( hr != S_OK )
			return hr;
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::storage2dir( IStorage *pstg, LPCTSTR lpdir_path )
{
	if( !pstg || !lpdir_path )
		return E_INVALIDARG;

	
	STATSTG	statstg = {0};    
	HRESULT hr = S_FALSE;

	/*
	// create directory for storage: lpdir_path + \\<storage_name>
	hr = pstg->Stat( &statstg, STATFLAG_DEFAULT );
	if( hr != S_OK )
		return hr;
	
	CString str_full_dir_path = lpdir_path;
	int nlength = str_full_dir_path.GetLength();
	if( !nlength )
		return E_INVALIDARG;

	if( str_full_dir_path[ nlength - 1 ] != _T('\\') )
		str_full_dir_path += _T("\\");

	str_full_dir_path = str_full_dir_path + (LPCTSTR)(_bstr_t(statstg.pwcsName));
	::CoTaskMemFree( statstg.pwcsName );
	statstg.pwcsName = NULL;
	
	if( !::CreateDirectory( str_full_dir_path, NULL ) )
		return S_FALSE;*/

	CString str_full_dir_path = lpdir_path;
	int nlength = str_full_dir_path.GetLength();
	if( !nlength )
		return E_INVALIDARG;

	if( str_full_dir_path[ nlength - 1 ] != _T('\\') )
		str_full_dir_path += _T("\\");

	// enum subelements   
	IEnumSTATSTGPtr sptr_enum;
   if( S_OK != pstg->EnumElements( NULL, NULL, NULL, &sptr_enum ) )
		return false;
			   
	hr = sptr_enum->Next( 1, &statstg, 0 );
	while( S_OK == hr )
	{
		switch (statstg.type)
		{
		case STGTY_STREAM:
			{
				IStreamPtr sptr_stream;
				if( S_OK == pstg->OpenStream( statstg.pwcsName, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, 
					&sptr_stream ) )
				{
					CString str_file_path = str_full_dir_path + (LPCTSTR)(_bstr_t( statstg.pwcsName ));
					stream2file( sptr_stream, str_file_path );
				}
			}
			break;

		case STGTY_STORAGE:
			{
                IStoragePtr sptr_stg;
				if( S_OK == pstg->OpenStorage( statstg.pwcsName, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, 0,
					&sptr_stg ) )
				{
					CString str_dir_path = str_full_dir_path + (LPCTSTR)(_bstr_t( statstg.pwcsName ));
					if( ::CreateDirectory( str_dir_path, NULL ) )
						storage2dir( sptr_stg, str_dir_path );
				}
			}
			break;
		}

		// Move on to the next element in the enumeration of this storage.
		::CoTaskMemFree( statstg.pwcsName );
		statstg.pwcsName = NULL;

		hr = sptr_enum->Next( 1, &statstg, 0 );
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT	CFileStorageHelper::stream2file( IStream *pstm, LPCTSTR lpfile_path )
{
	if( !pstm || !lpfile_path )
		return E_INVALIDARG;

	DWORD dwfile_size = 0;
	STATSTG stStat = {0};

	pstm->Stat( &stStat, STATFLAG_NONAME );
	dwfile_size = stStat.cbSize.LowPart;
	if( !dwfile_size )
		return S_FALSE;

	HANDLE hfile = 0;
	hfile = ::CreateFile( lpfile_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 0 );

	if( hfile == INVALID_HANDLE_VALUE )
		return S_FALSE;

	// move to begin
	LARGE_INTEGER liseek = {0};
	pstm->Seek( liseek, STREAM_SEEK_SET, NULL );
    
	BYTE *pbuffer = 0;
	pbuffer = new BYTE[ dwfile_size ];
	pstm->Read( pbuffer, dwfile_size, NULL);

	DWORD dwwritten = 0;
	::WriteFile( hfile, pbuffer, dwfile_size, &dwwritten, 0 );    

	if( pbuffer )
		delete[] pbuffer; pbuffer = 0;

	::CloseHandle( hfile ); hfile = 0;    
	return S_OK;
}