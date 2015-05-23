#pragma once
#include "afxcmn.h"

#include "afxwin.h"
#include "input.h"
#include "BaseDialog.h"
#include "HistoDlg.h"


// CSettingsDialog dialog

class CSettingsDialog : public CBaseDialog, public IInputPreviewSite
{
	DECLARE_DYNAMIC(CSettingsDialog)

	CSize m_szButton;
	IInputPreview2Ptr m_sptrPrv;
	IDriver3Ptr m_sptrDrv;
	CHistoDlg m_HistoDlg;
	int m_nDev;

	CBaseDialog *m_pDialog;
	void AddDialog(CBaseDialog *p, int idd);
	void AddDialog(int n);
	virtual void OnLayoutChanged();
	void ShowHistogram(bool bShow);

public:
	CSettingsDialog(IUnknown *punk, int nDevice, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDialog();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDelayedRepaint(WPARAM wParam, LPARAM lParam);
public:
	// For IInputPreviewSite
	com_call Invalidate();
	com_call OnChangeSize();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}
public:
	CTabCtrl m_Tab;
	virtual BOOL OnInitDialog();
	CStatic m_StaticPeriod;
	CButton m_Ok;
	CButton m_Help;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
	afx_msg void OnDestroy();
};
