#if !defined(AFX_VTCHECKBOXPPG_H__77E698B8_2CCC_437B_8583_17BF3F19C300__INCLUDED_)
#define AFX_VTCHECKBOXPPG_H__77E698B8_2CCC_437B_8583_17BF3F19C300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTCheckBoxPpg.h : Declaration of the CVTCheckBoxPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxPropPage : See VTCheckBoxPpg.cpp.cpp for implementation.

class CVTCheckBoxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTCheckBoxPropPage)
	DECLARE_OLECREATE_EX(CVTCheckBoxPropPage)

// Constructor
public:
	CVTCheckBoxPropPage();

// Dialog Data
	//{{AFX_DATA(CVTCheckBoxPropPage)
	enum { IDD = IDD_PROPPAGE_VTCHECKBOX };
	CString	m_strCaption;
	BOOL m_bNewDrawMode;
	BOOL	m_bAppearance3D;
	BOOL	m_bUseSystemFont;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTCheckBoxPropPage)
	afx_msg void OnNewDrawingMode();
	afx_msg void On3dappearance();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SetButtonsState();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCHECKBOXPPG_H__77E698B8_2CCC_437B_8583_17BF3F19C300__INCLUDED)
