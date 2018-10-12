#pragma once

#include "\vt5\awin\type_defs.h"
#include "\vt5\awin\win_dlg.h"
#include "\vt5\awin\misc_string.h"

#include "statistics.h"

//////////////////////////////////////////////////////////////////////
//
//	class CDeleteObjectDlg
//
//////////////////////////////////////////////////////////////////////
class CDeleteObjectDlg : public dlg_impl
{
public:
	CDeleteObjectDlg();
	virtual ~CDeleteObjectDlg();

protected:
//message handlers
	virtual LRESULT	on_initdialog();
	virtual LRESULT	on_command( uint cmd );
	virtual void	on_ok();	

	void			fill_combos();
	void			on_ctrls_change();

public:
	bool			m_bobject_list;
	_string			m_str_object_list;
	_string			m_str_image;

	IStatTablePtr	m_ptr_table;
};

struct col_info
{
	int		norder;
	char	sz_caption[255];
	bool	bvisible;
	LPARAM	lparam;
};

//////////////////////////////////////////////////////////////////////
//
//	class CCustomizeDlg
//
//////////////////////////////////////////////////////////////////////
class CCustomizeDlg : public dlg_impl
{
public:
	CCustomizeDlg();
	virtual ~CCustomizeDlg();

protected:
//message handlers
	virtual LRESULT	on_initdialog();
	virtual LRESULT	on_command(uint cmd);
	virtual void	on_ok();	
	virtual LRESULT	on_notify(uint idc, NMHDR *pnmhdr);

public:	
	void			set_col_info( col_info* parr, size_t size );
protected:
	
	col_info*		m_parr;
	size_t			m_size;

	void			update_controls( int idx );
	col_info*		get_item_data( int idx );
};
