#pragma once
#include "\vt5\awin\win_dlg.h"


class CInputMsgDlg : public dlg_impl
{
public:
	CString m_str_msg,
			m_str_caption;

public:
	CInputMsgDlg();
	~CInputMsgDlg();

	virtual long	on_initdialog();
	virtual void	on_ok();

protected:
	virtual LRESULT on_command(uint cmd);
};