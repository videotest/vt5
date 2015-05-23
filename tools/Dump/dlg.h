#pragma once

#include "_types.h"
#include "src_wnd.h"


#define ID_ADDR_TYPE	0
#define ID_ADDR_GROUP	1
#define ID_ADDR_HEAP	2
#define ID_ADDR_SIZE	3
#define ID_ADDR_POINTER	4

#define ID_STACK_ADDR	0
#define ID_STACK_RA		1
#define ID_STACK_MODULE	2
#define ID_STACK_FUNC	3
#define ID_STACK_SRC	4
#define ID_STACK_LINE	5

#define ID_CMP_TYPE		0
#define ID_CMP_FILE		1
#define ID_CMP_SIZE		2


#define ROW_TYPE_MEM	0
#define ROW_TYPE_FILE1	1
#define ROW_TYPE_FILE2	2

class heap_info_ex : public heap_info
{
public:
	heap_info_ex()
	{
		m_count			= 0;
		m_dw_full_size	= 0;
		m_ltype			= 0;
	}

	unsigned	m_count;
	DWORD		m_dw_full_size;
	long		m_ltype;

	static void	free( void* p ){
		delete (heap_info_ex*)p;}
};

long cmp_heap_stack( heap_info_ex* p1, heap_info_ex* p2 );


//////////////////////////////////////////////////////////////////////
//	class main_dlg
//////////////////////////////////////////////////////////////////////
class main_dlg : public dlg_impl
{
public:
	main_dlg();
	virtual ~main_dlg();

protected:
	//message handlers
	virtual long	on_initdialog( );
	virtual long	on_command( uint cmd );
	virtual long	on_notify( uint idc, NMHDR *pnmhdr );
	virtual long	on_destroy( );

	//data
	void			deinit_data( );
		
	void			grab_current_heap_map();
	void			grab_external_heap_map();
	void			fill_list( );
	void			clean_list( long ltype );

	//lists
	HWND			m_hwnd_addr, m_hwnd_stack, m_hwnd_cmp;

	//notify
	void			on_address_change( int nitem );
	void			on_stack_change( int nitem );
	void			on_browse_compare_file( int nitem );
	void			on_browse_stack( int nitem );

	//state
	void			on_state_change();
	void			set_state_to_ctrls();

	//save
	bool			save_dump( const char* psz_file );
	bool			load_dump( const char* psz_file, long ltype );

	_list_t<heap_info_ex*, heap_info_ex::free>						m_list;
	_list_map_t<heap_info_ex*, heap_info_ex*, cmp_heap_stack, 0>	m_map;

	char			m_sz_dump_file[MAX_PATH];

	//check state
	bool			m_bmem_check, m_bfile1_check, m_bfile2_check;
	bool			m_block_check;

	//colors
	COLORREF		m_clr_mem, m_clr_file1, m_clr_file2;

	//src wnd
	src_wnd			m_src_wnd;

	//ICO
	HICON			m_hicon_small, m_hicon_big;
};
