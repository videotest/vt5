#pragma once
#include "afxcmn.h"
#include "TestUIUtils.h"

// CTestsProcessDlg dialog

class CTestsProcessDlg : public CDialog, public CWindowImpl, public CDockWindowImpl,
	public CHelpInfoImpl, public CNamedObjectImpl, public CEventListenerImpl
{
	DECLARE_DYNCREATE(CTestsProcessDlg)
	GUARD_DECLARE_OLECREATE(CTestsProcessDlg)
	ENABLE_MULTYINTERFACE();

	bool m_bRun;
	CBitmap m_bmpStopRun;
	CTestInfoArray m_arrTests;
	void AddOrSetStatus(LPCTSTR lpstrName, LPCTSTR lpstr_info, int nCond, bool bPassed, bool bSuccess);
	void SetRunStopButton();
	void ClearDialog();

public:
	CTestsProcessDlg();   // standard constructor
	virtual ~CTestsProcessDlg();

// Dialog Data
	enum { IDD = IDD_TESTING_PROCESS };

	// Attributes
public:
	// For CWindowImpl
	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	// For CEventListener
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_INTERFACE_MAP()
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	CListCtrl m_TestList;
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonViewResults();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnLvnGetInfoTipListTestProcess(NMHDR *pNMHDR, LRESULT *pResult);
};
