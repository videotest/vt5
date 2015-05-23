#pragma once
#include "afxcmn.h"
#include "supergridctrl.h"
#include "test_man.h"


// CRunTestsDlg dialog

class CRunTestsDlg : public CDialog, public ISuperGridSite
{
	DECLARE_DYNAMIC(CRunTestsDlg)

	CMap<CItemInfo*,CItemInfo*,ITestRunningData*,ITestRunningData*> m_mapItems;
	void SaveAllItemsStates();

	void InitializeList();
	void DoAddListItem(ITestManager *pTestManager, long lParentPos, CSuperGridCtrl::CTreeItem *pBase/*, BOOL *pbAnyUnsaved*/);
	void InsertColumn(int n, int cx, int id = -1);
public:
	CRunTestsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRunTestsDlg();

	virtual void OnCheckStateChanged(CItemInfo *pInfo);
	virtual void OnSelectionChanged(CItemInfo *pInfo);

// Dialog Data
	enum { IDD = IDD_RUN_TESTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrlEx m_TestsList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaveTest();
	afx_msg void OnBnClickedButtonTestSystem();


};
