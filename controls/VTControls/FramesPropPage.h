#if !defined(AFX_FRAMESPROPPAGE_H__BE0B02A8_A0AE_11D3_A538_0000B493A187__INCLUDED_)
#define AFX_FRAMESPROPPAGE_H__BE0B02A8_A0AE_11D3_A538_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramesPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage : Property page dialog

class CFramesPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CFramesPropPage)
	DECLARE_OLECREATE_EX(CFramesPropPage)

// Constructors
public:
	CFramesPropPage();

// Dialog Data
	//{{AFX_DATA(CFramesPropPage)
	enum { IDD = IDD_PROPPAGE_FRAMES };
	BOOL	m_bInnerRaised;
	BOOL	m_bInnerSunken;
	BOOL	m_bOuterRaised;
	BOOL	m_bOuterSunken;
	int		m_nTemplateNum;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CFramesPropPage)
	afx_msg void OnTemplateChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMESPROPPAGE_H__BE0B02A8_A0AE_11D3_A538_0000B493A187__INCLUDED_)
