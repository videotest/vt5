#pragma once
#include "afxcmn.h"

#include "afxwin.h"
#include "input.h"
#include "SettingsBase.h"


// CSettingsDialog dialog

class CSettingsDialog : public CSettingsBase
{
	DECLARE_DYNAMIC(CSettingsDialog)

public:
	CSettingsDialog(IUnknown *punk, int nDevice, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDialog();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
