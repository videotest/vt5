#ifndef __pfaction_h__
#define __pfaction_h__

#include "\vt5\ifaces\printfield.h"

struct	control_data
{
	print_field_params			params;
	unsigned	id, 
				id_static, 
				id_spin;

	CRect		rect_static, 
				rect_edit;

	CRect		rect_position;	//position on the report

	_bstr_t		caption;
	long		data_format;

	control_data	*next;

	long		ny_dlg;


};

struct group_data
{
	_bstr_t	caption;
	int		controls_count;
	CRect		rect;

	control_data	*first_control;
	group_data		*next;
};

class request_impl
{
public:
	request_impl();
	virtual ~request_impl();

	bool add_control( IUnknown *punk );
	bool add_control( print_field_params *p, BSTR caption );
	bool add_control( print_field_params *pparams, BSTR caption, BSTR groupCaption  );
	void layout( CWnd *pwnd );
	void layout_by_report_pos( CWnd *pwnd );
	bool check_data();


	CSize	total_size( int cols, int cx_min = 0 );	// [vanek]: учет минимального размера формы
public:
	group_data	*first_group;
	unsigned	current_id;
	CSize		m_max_extent, m_size;
	bool		m_bUseReportPosition;

	double GetDisplayPixelPMM();
};
//[ag]1. classes

class CActionRequestValuesForDB : public CActionBase, public request_impl
{
	DECLARE_DYNCREATE(CActionRequestValuesForDB)
	GUARD_DECLARE_OLECREATE(CActionRequestValuesForDB)

public:
	CActionRequestValuesForDB();
	virtual ~CActionRequestValuesForDB();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionRequestValuesForReport : public CActionBase, public request_impl
{
	DECLARE_DYNCREATE(CActionRequestValuesForReport)
	GUARD_DECLARE_OLECREATE(CActionRequestValuesForReport)

public:
	CActionRequestValuesForReport();
	virtual ~CActionRequestValuesForReport();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};
/**/
class CActionRequestValuesForReportSD : public CActionBase, public request_impl
{
	DECLARE_DYNCREATE(CActionRequestValuesForReportSD)
	GUARD_DECLARE_OLECREATE(CActionRequestValuesForReportSD)

public:
	CActionRequestValuesForReportSD();
	virtual ~CActionRequestValuesForReportSD();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};
/**/

class CActionStoreValuesForReport : public CActionBase, public request_impl
{
	DECLARE_DYNCREATE(CActionStoreValuesForReport)
	GUARD_DECLARE_OLECREATE(CActionStoreValuesForReport)

public:
	CActionStoreValuesForReport();
	virtual ~CActionStoreValuesForReport();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


#endif //__pfaction_h__
