#if !defined(AFX_EXTPAGE_H__69090736_8083_11D3_A510_0000B493A187__INCLUDED_)
#define AFX_EXTPAGE_H__69090736_8083_11D3_A510_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtPage dialog

class CExtPage : public CPropertyPage
{
// Construction
public:
	CExtPage();
	~CExtPage();

public:
	//attributes
	IUnknown *GetControllingPage()	const		{return m_sptrPage;}

	bool Init( IUnknown *punkPage );

	virtual BOOL OnSetActive();

// Dialog Data
	//{{AFX_DATA(CExtPage)
	enum { IDD = IDD_DIALOG_EMPTYPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CExtPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPage)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HWND m_hWndExt;
	sptrIOptionsPage m_sptrPage;
	CString m_strCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTPAGE_H__69090736_8083_11D3_A510_0000B493A187__INCLUDED_)
