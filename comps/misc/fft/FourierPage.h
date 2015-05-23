#if !defined(AFX_FOURIERPAGE_H__34701AC2_C671_4C07_B496_0EBE6DEB8BA7__INCLUDED_)
#define AFX_FOURIERPAGE_H__34701AC2_C671_4C07_B496_0EBE6DEB8BA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FourierPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFourierPage dialog

class CFourierPage : public CPropertyPageBase
{
	DECLARE_DYNCREATE(CFourierPage)
	GUARD_DECLARE_OLECREATE(CFourierPage)
	PROVIDE_GUID_INFO()
// Construction
public:
	CFourierPage();
	~CFourierPage();

// Dialog Data
	//{{AFX_DATA(CFourierPage)
	enum { IDD = IDD_PAGE_FOURIER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFourierPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFourierPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOURIERPAGE_H__34701AC2_C671_4C07_B496_0EBE6DEB8BA7__INCLUDED_)
