#pragma once

#include "_hook.h"

////////////////////////////////////////////////////////////////////////////
//line_info
class line_info
{
public:
	line_info()
	{
		m_laddress	= 0;
		m_lra		= 0;
		m_lsrc_line	= 0;
	}
	virtual ~line_info(){}
	static void free( void* p ){
		delete (line_info*)p;
	}

	long	m_laddress;				//address
	long	m_lra;					//relative address
	long	m_lsrc_line;			//12
	_string	m_str_image_file_name;	//c:\qq\debug\qq.exe
	_string	m_str_src_file_name;	//c:\qq\test.cpp
	_string	m_str_src_function;		//test_function
};

////////////////////////////////////////////////////////////////////////////
//pdb_engine
class pdb_engine
{
public:
	pdb_engine();
	virtual ~pdb_engine();

	bool	init();
	void	deinit();

	line_info*	get_line_info( DWORD dw_address );
	bool get_stack( unsigned* parr_stack, size_t size_in, size_t* psize_out );
protected:
	HANDLE	m_hprocess;
	_list_map_t<line_info*, long, cmp_long, line_info::free>	m_map;

};