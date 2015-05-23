#pragma once
#include "stdafx.h"
#include "misc_classes.h"
#include "\vt5\awin\win_dlg.h"

class CSnapshotParamsDlg :
	public dlg_impl
{
public:
	CSnapshotParamsDlg(void);
	virtual ~CSnapshotParamsDlg(void);
	virtual long on_initdialog();
	virtual long on_destroy();
	bool m_bSelectTarget;
	bool m_bScrollWindow;
	char m_szResizeCX[20];
	char m_szResizeCY[20];
};
