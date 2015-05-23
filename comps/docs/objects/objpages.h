#if !defined(AFX_OBJPAGES_H__84531A53_A0AD_11D3_A6A3_0090275995FE__INCLUDED_)
#define AFX_OBJPAGES_H__84531A53_A0AD_11D3_A6A3_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "actionobjectbase.h"
// objpages.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixedObjectPage dialog

class CFixedObjectPage : public CPropertyPageBase
{
// Construction
public:
	CFixedObjectPage( UINT nID );   // standard constructor
	~CFixedObjectPage();

// Dialog Data
	//{{AFX_DATA(CFixedObjectPage)
	enum { IDD = IDD_ELLIPSE_FIXED };
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedObjectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFixedObjectPage)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeSizes();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual const char *GetSection() const;
	virtual const char *GetWidthEntry() const = 0;
	virtual const char *GetHeightEntry() const = 0;
	virtual const char *GetCalibrEntry() const {return "UseCalibration";}
protected:
	void _UpdateEditors();
	void _UpdateValues();

protected:
	long	m_nWidth;
	long	m_nHeight;
	bool	m_bUseCalibration;
};

class CFixedEllipsePage : public CFixedObjectPage
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CFixedEllipsePage);
	GUARD_DECLARE_OLECREATE(CFixedEllipsePage);

	CFixedEllipsePage();

	virtual const char *GetWidthEntry() const;
	virtual const char *GetHeightEntry() const;
};

class CFixedRectPage : public CFixedObjectPage
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CFixedRectPage);
	GUARD_DECLARE_OLECREATE(CFixedRectPage);

	CFixedRectPage();

	virtual const char *GetWidthEntry() const;
	virtual const char *GetHeightEntry() const;

};
/////////////////////////////////////////////////////////////////////////////
// CMagickObjectPage dialog

class CMagickObjectPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CMagickObjectPage)
	GUARD_DECLARE_OLECREATE(CMagickObjectPage);

// Construction
public:
	CMagickObjectPage();
	~CMagickObjectPage();

// Dialog Data
	//{{AFX_DATA(CMagickObjectPage)
	enum { IDD = IDD_MAGICK_STICK };
	int		m_nBriInt;
	BOOL	m_bDynamicBrightness;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMagickObjectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMagickObjectPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeBriInt();
	afx_msg void OnDynamicBrightness();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
	afx_msg void OnKillFocusBriInt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	HWND  hTip;
	//---------  For tolltip -------------
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	//------------------------------------
	BOOL bEditTip;
};
/////////////////////////////////////////////////////////////////////////////
// CEditObjectPage dialog

class CEditObjectPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CEditObjectPage)
	GUARD_DECLARE_OLECREATE(CEditObjectPage);
// Construction
public:
	CEditObjectPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditObjectPage)
	enum { IDD = IDD_OBJECT_EDITOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditObjectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditObjectPage)
	afx_msg void OnClickWidth1Back();
	afx_msg void OnClickWidth2Back();
	afx_msg void OnClickWidth3Back();
	afx_msg void OnClickWidth4Back();
	afx_msg void OnClickWidth1();
	afx_msg void OnClickWidth2();
	afx_msg void OnClickWidth3();
	afx_msg void OnClickWidth4();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void _SetMergeWidth( int nWidth );
	void _SetBackWidth( int nWidth );
};
/////////////////////////////////////////////////////////////////////////////
// CObjectGeneralPage dialog

class CObjectGeneralPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CObjectGeneralPage);
	GUARD_DECLARE_OLECREATE(CObjectGeneralPage)
// Construction
public:
	CObjectGeneralPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectGeneralPage)
	enum { IDD = IDD_OBJECTS_GENERAL };
	int		m_nMinSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickClick();
	afx_msg void OnClickDblclick();
	afx_msg void OnClickModEadd();
	afx_msg void OnClickModeSep();
	afx_msg void OnClickModeSub();
	afx_msg void OnChangeMin();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void _Update();
protected:
	bool	m_bShowType;

	FreeMode	m_nFreeMode;
	DefineMode	m_nDefineMode;
};

class CObjectFreePage : public CObjectGeneralPage
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CObjectFreePage);
	GUARD_DECLARE_OLECREATE(CObjectFreePage);
public:
	CObjectFreePage();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_OBJPAGES_H__84531A53_A0AD_11D3_A6A3_0090275995FE__INCLUDED_)
 
