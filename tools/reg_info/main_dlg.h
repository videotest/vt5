#ifndef __main_dlg_h__
#define __main_dlg_h__

#include "\vt5\awin\win_dlg.h"

#include <commctrl.h>
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_string.h"
#include "\vt5\awin\misc_map.h"

class clsid_entry
{
public:
	clsid_entry()
	{
		m_str_clsid		= "";
		m_str_file_name	= "";
		m_str_prog_id	= "";
	}
	virtual ~clsid_entry(){}

	_string		m_str_clsid;
	_string		m_str_file_name;
	_string		m_str_prog_id;
};

inline void free_clsid_entry( void* pdata )
{
	delete( (clsid_entry*)pdata ); 
}

class file_server
{
public:
	file_server()
	{
		m_str_file_name = "";
	}
	virtual ~file_server()
	{
		m_clsid_ref_list.clear();
	}

	_string	m_str_file_name;
	_list_t<clsid_entry*, 0>	m_clsid_ref_list;
};

inline void free_file_server( void* pdata )
{
	delete( (file_server*)pdata ); 
}

class server_dir
{
public:
	server_dir()
	{
		m_str_dir	= "";
	}

	virtual ~server_dir()
	{
		
	}

	_string						m_str_dir;
	_list_t<file_server*, free_file_server>	m_file_list;
};

inline void free_server_dir( void* pdata )
{
	delete( (server_dir*)pdata ); 
}

#define SERVER_DIR_IDX	0
#define FILE_NA_IDX		1
#define FILE_EX_IDX		2
#define CLSID_IDX		3

typedef _list_t<clsid_entry*, 0>	clsid_delete_list;

inline long _cmp_string( const char *psz1, const char *psz2 )
{	return stricmp( psz1, psz2 );}

	//////////////////////////////////////////////////////////////////////
//
//	class main_dlg
//
//////////////////////////////////////////////////////////////////////
class main_dlg : public dlg_impl
{
public:
	main_dlg();
	virtual ~main_dlg();

protected:
	//message handler
	virtual long	on_initdialog();
	virtual long	on_destroy();
	virtual long	on_command( uint cmd );

	bool			fill_clsid_list();
	bool			fill_control();

	bool			recursive_select( HTREEITEM hti, bool bselect );
	bool			recursive_gather_clsid( HTREEITEM hti, clsid_delete_list& list ); 

	_list_t<clsid_entry*, free_clsid_entry>	m_clsid_list;
	_list_t<server_dir*, free_server_dir>	m_server_list;
	
	_map_t<server_dir*, const char*, _cmp_string, 0>	m_map_server_dirs;
	_map_t<file_server*, const char*, _cmp_string, 0>	m_map_file_dirs;

	HWND			m_hwnd_status;
	HWND			m_hwnd_tree;
	HWND			m_hwnd_progress;
	
	HIMAGELIST		m_himage_list; 

	bool			m_bprogid;
	_string			m_str_progid;
	bool			m_bprogid_and;

	bool			m_bfile;
	_string			m_str_file;
	bool			m_bfile_and;

	bool			m_bna_file;
	bool			m_bna_file_and;

	bool			m_buse_filters;

	void			load_profile();
	void			save_profile();

};

//////////////////////////////////////////////////////////////////////
//
//	class confirm_dlg
//
//////////////////////////////////////////////////////////////////////
class confirm_dlg : public dlg_impl
{
public:
	confirm_dlg();
	virtual ~confirm_dlg();

protected:
	//message handler
	virtual long	on_initdialog();
	virtual long	on_notify( uint idc, NMHDR *pnmhdr );

public:
	_string				m_str;

	clsid_delete_list*	m_plist;
	HIMAGELIST			m_himage_list;
};


//////////////////////////////////////////////////////////////////////
//
//	class filter_dlg
//
//////////////////////////////////////////////////////////////////////
class filter_dlg : public dlg_impl
{
public:
	filter_dlg();
	virtual ~filter_dlg();

protected:
	//message handler
	virtual long	on_initdialog();
	virtual long	on_command( uint cmd );
	virtual void	on_ok();
	void			update_ui();
	void			fill_controls();
	void			fill_members();

public:
	bool			m_bprogid;
	_string			m_str_progid;
	bool			m_bprogid_and;

	bool			m_bfile;
	_string			m_str_file;
	bool			m_bfile_and;

	bool			m_bna_file;
	bool			m_bna_file_and;

	bool			m_buse_filters;

};

#endif//__main_dlg_h__