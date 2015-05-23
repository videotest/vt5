#if !defined(AFX_VIEWAXSIZEPROPPAGE_H__D2FBDC50_ABFB_45E8_AD24_F233ED9293B6__INCLUDED_)
#define AFX_VIEWAXSIZEPROPPAGE_H__D2FBDC50_ABFB_45E8_AD24_F233ED9293B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewAXSizePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage : Property page dialog

class CViewAXSizePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CViewAXSizePropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXSizePropPage)
private:
	void HideUnusedControls();
// Constructors
public:
	CViewAXSizePropPage();

// Dialog Data
	//{{AFX_DATA(CViewAXSizePropPage)
	enum { IDD = IDD_PROPPAGE_SIZE };
	int		m_nTransformation;
	double	m_fDPI;
	double	m_fZoom;
	BOOL	m_bAutoBuild;
	BOOL	m_bUseObjectDPI;
	double	m_fObjectDPI;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	sptrIViewCtrl GetViewAXControl();

	virtual BOOL OnInitDialog();
	virtual BOOL OnEditProperty(DISPID dispid);
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	virtual BOOL OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );

// Message maps
protected:
	//{{AFX_MSG(CViewAXSizePropPage)
	afx_msg void OnSelchangeTransformation();
	afx_msg void OnUseObjectDpi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetOnOf( BOOL bUpdate = TRUE );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWAXSIZEPROPPAGE_H__D2FBDC50_ABFB_45E8_AD24_F233ED9293B6__INCLUDED_)
