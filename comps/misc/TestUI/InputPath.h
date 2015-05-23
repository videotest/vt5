#pragma once

// CInputPath dialog

class CInputPathDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputPathDlg)

public:
	CInputPathDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputPathDlg();

// Dialog Data
	enum { IDD = IDD_INPUT_PATH };

	CEdit		m_ctrl_edit_path;
	CComboBox	m_ctrl_combo_types;
	CString		m_str_object_name,
				m_str_type,
				m_str_path;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCbnSelchangeComboTypes();

protected:
	void update_ok();
public:
	afx_msg void OnEnChangeEditPath();
};
