#if !defined(AFX_VTPUSHBUTTONPPG_H__916A9FBD_D9BF_4E70_8EA7_51B0BE3A9171__INCLUDED_)
#define AFX_VTPUSHBUTTONPPG_H__916A9FBD_D9BF_4E70_8EA7_51B0BE3A9171__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTKeyButtonPpg.h : Declaration of the CVTKeyButtonPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonPropPage : See VTPushButtonPpg.cpp.cpp for implementation.

class CVTKeyButtonPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTKeyButtonPropPage)
	DECLARE_OLECREATE_EX(CVTKeyButtonPropPage)

// Constructor
public:
	CVTKeyButtonPropPage();

// Dialog Data
	//{{AFX_DATA(CVTKeyButtonPropPage)
	enum { IDD = IDD_PROPPAGE_VTKEYBUTTON };
	long	m_lVirtKey;
	int		m_nAlignHorz;
	int		m_nAlignVert;
	short	m_ItemDeltaX;
	short	m_ItemDeltaY;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL PreTranslateMessage(MSG* pMsg);


// Message maps
protected:
	//{{AFX_MSG(CVTKeyButtonPropPage)
	afx_msg void OnBtnPick();
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool m_bProcessPick;
	long GetCurrentPressedKey();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTPUSHBUTTONPPG_H__916A9FBD_D9BF_4E70_8EA7_51B0BE3A9171__INCLUDED)
