#pragma once
#include "afxcmn.h"
#include "TestUIUtils.h"
#include "afxwin.h"

// CTestingResults dialog

class CTestingResults : public CDialog
{
	DECLARE_DYNAMIC(CTestingResults)

	CBitmap m_bmpExport,m_bmpCmpWithSmp,m_bmpCmpVals;
	int m_nSelectedItem;
	CTestInfoArray m_arrTests;
	void InitializeList();
	void InitRows(int nFilter);
public:
	CTestingResults(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestingResults();

// Dialog Data
	enum { IDD = IDD_DIALOG_TESTING_RESULTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_Results;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CComboBox m_ComboFilter;
	afx_msg void OnCbnSelchangeComboFiltration();
//	afx_msg void OnHdnItemclickListTestsResults(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListTestsResults(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnExportTest();
	afx_msg void OnCompareValues();
	afx_msg void OnCompareWithSample();
};
