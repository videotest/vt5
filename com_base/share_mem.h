#ifndef __sare_mem_h__
#define __sare_mem_h__

//////////////////////////////////////////////////////////////////////
//
//	class share_mem
//
//////////////////////////////////////////////////////////////////////
class share_mem
{
public:
	share_mem();
	~share_mem();

	bool	open_exist( const _char* psz_name, size_t size  );
	bool	create_new( const _char* psz_name, size_t size );
	
	void*	lock( DWORD dw_time_out = 0 );
	bool	unlock();
	
	size_t	get_size();	
	
	void	deinit();
protected:
	bool	get_mutex_name( const _char* psz_file_name, _char* psz_buf, size_t size_buf );

protected:
	HANDLE	m_file_map;
	BYTE*	m_map_data;
	size_t	m_size;
	HANDLE	m_mutex_data_free;
};

//////////////////////////////////////////////////////////////////////
inline share_mem::share_mem()
{
	m_file_map			= 0;
	m_map_data			= 0;
	m_size				= 0;
	m_mutex_data_free	= 0;
}

//////////////////////////////////////////////////////////////////////
inline share_mem::~share_mem()
{
	deinit();
}

//////////////////////////////////////////////////////////////////////
inline bool	share_mem::open_exist( const _char* psz_name, size_t size  )
{
	deinit();

	if( !psz_name )
		return false;

	_char sz_mutex_name[255];	sz_mutex_name[0] = 0;
	if( !get_mutex_name( psz_name, sz_mutex_name, sizeof(sz_mutex_name) ) )
		return false;
	
	//at first create mutex
	m_mutex_data_free = ::OpenMutex( MUTEX_ALL_ACCESS, FALSE, sz_mutex_name );
	if( !m_mutex_data_free )
		return false;

	//create file maping
	m_file_map = ::OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, psz_name );
	if( !m_file_map )//exist
		return false;

	m_map_data = (BYTE*)::MapViewOfFile( m_file_map, FILE_MAP_WRITE, 0, 0, size );
	if( !m_map_data )
		return false;

	m_size = size;

	return true;
}

//////////////////////////////////////////////////////////////////////
inline bool	share_mem::create_new( const _char* psz_name, size_t size )
{
	deinit();

	if( !psz_name )
		return false;

	_char sz_mutex_name[255];	sz_mutex_name[0] = 0;
	if( !get_mutex_name( psz_name, sz_mutex_name, sizeof(sz_mutex_name) ) )
		return false;
	
	//at first create mutex
	m_mutex_data_free = ::OpenMutex( MUTEX_ALL_ACCESS, FALSE, sz_mutex_name );
	if( m_mutex_data_free )//exist
		return false;

	m_mutex_data_free = ::CreateMutex( 0, FALSE, sz_mutex_name );
	if( !m_mutex_data_free )
		return false;

	//create file maping
	m_file_map = ::OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, psz_name );
	if( m_file_map )//exist
		return false;

	m_file_map = ::CreateFileMapping( (HANDLE)0xFFFFFFFF, 0, PAGE_READWRITE, 0, size, psz_name );
	if( !m_file_map )
		return false;

	m_map_data = (BYTE*)::MapViewOfFile( m_file_map, FILE_MAP_WRITE, 0, 0, size );
	if( !m_map_data )
		return false;

	m_size = size;

	return true;
}

//////////////////////////////////////////////////////////////////////
inline void* share_mem::lock( DWORD dw_time_out )
{
	if( !m_mutex_data_free )
		return 0;

	if( WAIT_OBJECT_0 != ::WaitForSingleObject( m_mutex_data_free, dw_time_out ) )
		return 0;

	return m_map_data;
}

//////////////////////////////////////////////////////////////////////
inline bool	share_mem::unlock()
{
	if( !m_mutex_data_free )
		return false;

	::ReleaseMutex( m_mutex_data_free );
	return true;
}

//////////////////////////////////////////////////////////////////////
inline size_t share_mem::get_size()
{
	return m_size;
}

//////////////////////////////////////////////////////////////////////
inline void share_mem::deinit()
{
	if( m_map_data )
		::UnmapViewOfFile( m_map_data );

	m_map_data = 0;

	if( m_file_map )
		verify( ::CloseHandle( m_file_map ) );
	
	m_file_map = 0;

	if( m_mutex_data_free )
		::CloseHandle( m_mutex_data_free );		m_mutex_data_free = 0;			

	m_size = 0;
}

//////////////////////////////////////////////////////////////////////
inline bool share_mem::get_mutex_name( const _char* psz_file_name, _char* psz_buf, size_t size_buf )
{
	if( !psz_file_name || !psz_buf )
		return false;

	if( size_buf < ___strlen(psz_file_name) + 10 )
		return false;

	___sprintf( psz_buf, ___T("mutex_%s"), psz_file_name );
	return true;

}

#endif// __sare_mem_h__