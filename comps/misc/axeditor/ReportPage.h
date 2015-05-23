#if !defined(AFX_REPORTPAGE_H__CA2EA5D5_E37A_433E_9A21_2E2137A12F5C__INCLUDED_)
#define AFX_REPORTPAGE_H__CA2EA5D5_E37A_433E_9A21_2E2137A12F5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportPage dialog

class CReportPage : public COlePropertyPage
{	
	DECLARE_DYNCREATE(CReportPage);
	GUARD_DECLARE_OLECREATE(CReportPage);

// Construction
public:
	CReportPage();
	~CReportPage();

// Dialog Data
	//{{AFX_DATA(CReportPage)
	enum { IDD = IDD_PAGE_REPORT };
	CSpinButtonCtrl	m_spinZoom;
	CSpinButtonCtrl	m_spinDPI;
	CSpinButtonCtrl	m_spinWidth;
	CSpinButtonCtrl	m_spinTop;
	CSpinButtonCtrl	m_spinLeft;
	CSpinButtonCtrl	m_spinHeight;
	double	m_fHeight;
	double	m_fLeft;
	double	m_fWidth;
	double	m_fTop;
	BOOL	m_bHorzScale;
	BOOL	m_bVertScale;
	double	m_fDPI;
	double	m_fZoom;
	BOOL	m_bCloneObject;
	//}}AFX_DATA

	bool	CheckOwnUnknown( IUnknown *punk );
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CReportPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CReportPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(MyPropertyPage, IPropertyPage2)
		com_call SetPageSite(LPPROPERTYPAGESITE);
		com_call Activate(HWND, LPCRECT, BOOL);
		com_call Deactivate();
		com_call GetPageInfo(LPPROPPAGEINFO);
		com_call SetObjects(ULONG, LPUNKNOWN*);
		com_call Show(UINT);
		com_call Move(LPCRECT);
		com_call IsPageDirty();
		com_call Apply();
		com_call Help(LPCOLESTR);
		com_call TranslateAccelerator(LPMSG);
		com_call EditProperty(DISPID);
	END_INTERFACE_PART(MyPropertyPage)
protected:
	IUnknownPtr	m_ptrControl;
	bool		m_bActive;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPAGE_H__CA2EA5D5_E37A_433E_9A21_2E2137A12F5C__INCLUDED_)
