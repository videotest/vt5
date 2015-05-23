#if !defined(AFX_MEASPROPPAGE_H__4DD1C545_1C96_4ADC_9C0F_CCA1439942B4__INCLUDED_)
#define AFX_MEASPROPPAGE_H__4DD1C545_1C96_4ADC_9C0F_CCA1439942B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeasPropPage dialog

class CMeasPropPage : public CPropertyPageBase, public CEventListenerImpl
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CMeasPropPage);
	GUARD_DECLARE_OLECREATE(CMeasPropPage);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CMeasPropPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMeasPropPage)
	enum { IDD = IDD_MEASUREMENT };
	CString	m_strName;
	CString	m_strUnti;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasPropPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeasPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickHorz();
	afx_msg void OnClickVert();
	afx_msg void OnClickDiagonal();
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG

	void DoLoadSettings();
	void OnBtnClick( long dir );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASPROPPAGE_H__4DD1C545_1C96_4ADC_9C0F_CCA1439942B4__INCLUDED_)
