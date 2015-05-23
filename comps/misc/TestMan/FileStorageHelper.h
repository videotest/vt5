#pragma once
#include <atlstr.h>

// CFileStorageHelper - обеспечивает копирование каталога в compound-файл и, наоборот, копирование папок и файлов
// из compound-файла
class CFileStorageHelper
{
	IStoragePtr	m_sptr_storage;
	CString		m_str_path;
public:
	CFileStorageHelper(void);
	~CFileStorageHelper(void);

	HRESULT	CreateStorage( LPCTSTR lpFileName );

    HRESULT	OpenStorage( LPCTSTR lpFileName );
    HRESULT	CloseStorage( );

	HRESULT	FillByPath( LPCTSTR lpPath );	// заполнение содержимого с диска
	HRESULT	FlushContent( LPCTSTR lpFlushTo );	// запись содержимого на диск

	static HRESULT	dir2storage( IStorage *pstg, LPCTSTR lpdir_path );
	static HRESULT	file2stream( IStream *pstm, LPCTSTR lpfile_path );

	static HRESULT	storage2dir( IStorage *pstg, LPCTSTR lpdir_path );
	static HRESULT	stream2file( IStream *pstm, LPCTSTR lpfile_path );
};
