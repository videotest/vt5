#pragma once
#include "\vt5\awin\win_dlg.h"


class CInputMtdNameDlg : public dlg_impl
{
public:
	CString m_str_method_name,
			m_str_caption;

	BOOL	m_bcheck_unique; // ��������� �� ������������ �����
	BOOL	m_ballow_overwrite; // ���� ��������� � �� ��������� - ������������ �����, ����� �� ������������

	BOOL	m_balready_exists;	// �������� ��������: �������� ��� ������������? (���� ������� �� ����������� ������)
						// ������������, ������ ���� m_bcheck_unique (������ ���� ��� m_ballow_overwrite - �� ����� false)

	BOOL	m_bhide_static_text;
    
public:
	CInputMtdNameDlg();
	~CInputMtdNameDlg();

	virtual long	on_initdialog();
	virtual void	on_ok();

protected:
	virtual long on_command( uint cmd );
    
	// [vanek] SBT:985 - 18.05.2004
	void	update_ok( );
};