#if !defined(AFX_IMAGEPAGE_H__4F68052B_566B_4BE3_BF71_4638A844AE89__INCLUDED_)
#define AFX_IMAGEPAGE_H__4F68052B_566B_4BE3_BF71_4638A844AE89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImagePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImagePage dialog

class CImagePage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CImagePage);
	GUARD_DECLARE_OLECREATE(CImagePage);
// Construction
public:
	CImagePage();   // standard constructor
	~CImagePage();

// Dialog Data
	//{{AFX_DATA(CImagePage)
	enum { IDD = IDD_PAGE_IMAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImagePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPAGE_H__4F68052B_566B_4BE3_BF71_4638A844AE89__INCLUDED_)
