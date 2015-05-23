#pragma once
#include "test_man.h"
#include "afxwin.h"
#include "afxcmn.h"	
#include "\vt5\awin\misc_list.h"


// CCompareValuesDlg dialog

class CCompareValuesDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompareValuesDlg)

public:
	CCompareValuesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCompareValuesDlg();

// Dialog Data
	enum { IDD = IDD_COMPARE_VALUES };

	void SetTestParam( TEST_ERR_DESCR *pErrDescr );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	TEST_ERR_DESCR *m_pErrDescr;
	IUnknownPtr		m_sptrDocCmp,
					m_sptrDocSave;

	BOOL	fill_list( CString str_array_name );
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ctrlCombo;
	afx_msg void OnCbnSelchangeComboArrays();
	CListCtrl m_ctrlList;
    _list_t<long> m_err_items;
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
};
