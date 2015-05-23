#pragma once
#include "\vt5\awin\win_dlg.h"


class CInputMtdNameDlg : public dlg_impl
{
public:
	CString m_str_method_name,
			m_str_caption;

	BOOL	m_bcheck_unique; // проверять ли уникальность имени
	BOOL	m_ballow_overwrite; // если проверили и не уникально - переспросить юзера, хочет ли перезаписать

	BOOL	m_balready_exists;	// выходное значение: методика уже существовала? (надо стереть до продолжения работы)
						// возвращается, только если m_bcheck_unique (причем если нет m_ballow_overwrite - то всяко false)

	BOOL	m_bhide_static_text;
    
public:
	CInputMtdNameDlg();
	~CInputMtdNameDlg();

	virtual long	on_initdialog();
	virtual void	on_ok();

protected:
	virtual LRESULT on_command(uint cmd);
    
	// [vanek] SBT:985 - 18.05.2004
	void	update_ok( );
};