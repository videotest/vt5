#if !defined(AFX_MEASPAGES_H__30BEE8A4_8653_4991_9FE2_42629C47AE20__INCLUDED_)
#define AFX_MEASPAGES_H__30BEE8A4_8653_4991_9FE2_42629C47AE20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasPages.h : header file
//

#include "ActionBase.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasPage dialog

class CMeasPage : public CPropertyPageBase,public CEventListenerImpl
{
	DECLARE_DYNCREATE(CMeasPage);
	GUARD_DECLARE_OLECREATE(CMeasPage);
	PROVIDE_GUID_INFO()
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

// Construction
public:
	CMeasPage(UINT nID = CMeasPage::IDD);   // standard constructor
	~CMeasPage();

	virtual bool OnSetActive();
	virtual void DoLoadSettings();
	virtual void DoStoreSettings();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

// Dialog Data
	//{{AFX_DATA(CMeasPage)
	enum { IDD = IDD_MEAS_PAGE };
	CString	m_strParamName;
	CString	m_strValue;
	CString	m_strUnit;
	CString	m_strCalibr;
	//}}AFX_DATA
	void OnBtnClick( long dir );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeasPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickHorz();
	afx_msg void OnClickVert();
	afx_msg void OnClickDiagonal();
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASPAGES_H__30BEE8A4_8653_4991_9FE2_42629C47AE20__INCLUDED_)
