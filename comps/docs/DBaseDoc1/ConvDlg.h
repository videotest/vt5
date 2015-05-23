#pragma once

#include "types.h"
#include "HelpDlgImpl.h"

/////////////////////////////////////////////////////////////////////////////
//
// CConvDlg dialog
//
/////////////////////////////////////////////////////////////////////////////
class CConvDlg :	public CDialog,
					public CHelpDlgImpl

{				  
	DECLARE_DYNAMIC(CConvDlg)
	IMPLEMENT_HELP( "DBConvert" );

public:
	CConvDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConvDlg();

// Dialog Data
	enum { IDD = IDD_CONVERT_VT4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:

	CArray<_CFieldInfo*, _CFieldInfo*> m_arr_src_fi;
	void					clear_src_ar_fi();
	
	CString					get_src_db();
	CString					get_target_db();

	ADO::_ConnectionPtr		m_conn_ptr;
	ADOX::_CatalogPtr		m_catalog_ptr;
	ADO::_RecordsetPtr		m_ptr_rs;

	void					set_ui_enable();
	bool					open_src_table();
	bool					analize_src_table();
	void					pack_field_info();
	bool					fill_list();
	bool					start_convert();

	bool					is_jpeg( BYTE* pbuf, int nsize );
	bool					is_raw_bmp( BYTE* pbuf, int nsize );
	long					find_vt4_prefix( BYTE* pbuf, int nsize );
	bool					is_aborted();

	IUnknownPtr				create_jpeg_filter();
	IUnknownPtr				create_vti_filter();
	IUnknownPtr				create_bmp_filter();
	
	CString					get_jpeg_tmp_path();
	CString					get_vti_tmp_path();

	void					analize_target_is_empty();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAnalize();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedVt4Browse();
	afx_msg void OnBnClickedVt5Browse();
	afx_msg void OnDestroy();
};
