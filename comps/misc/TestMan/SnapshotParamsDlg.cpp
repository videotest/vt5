#include "StdAfx.h"
#include "snapshotparamsdlg.h"
#include "resource.h"

CSnapshotParamsDlg::CSnapshotParamsDlg(void) : dlg_impl( IDD_SNAPSHOTPARAMS )
{
}

CSnapshotParamsDlg::~CSnapshotParamsDlg(void)
{
}

LRESULT CSnapshotParamsDlg::on_initdialog()
{
	set_dlg_item_text(IDC_RESIZECX, m_szResizeCX);
	set_dlg_item_text(IDC_RESIZECY, m_szResizeCY);
	check_dlg_button(IDC_SELECTTARGET, m_bSelectTarget ? BST_CHECKED : BST_UNCHECKED );
	check_dlg_button(IDC_SCROLLWINDOW, m_bScrollWindow ? BST_CHECKED : BST_UNCHECKED );
	return __super::on_initdialog();
}

LRESULT CSnapshotParamsDlg::on_destroy()
{
	get_dlg_item_text(IDC_RESIZECX, m_szResizeCX, sizeof(m_szResizeCX));
	get_dlg_item_text(IDC_RESIZECY, m_szResizeCY, sizeof(m_szResizeCY));
	m_bSelectTarget = is_dlg_button_checked(IDC_SELECTTARGET) == BST_CHECKED;
	m_bScrollWindow = is_dlg_button_checked(IDC_SCROLLWINDOW) == BST_CHECKED;
	return __super::on_destroy();
}
