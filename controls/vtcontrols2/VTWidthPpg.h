#if !defined(AFX_VTWIDTHPPG_H__290FB5AB_FC70_4C18_90CC_52C14FB35DF9__INCLUDED_)
#define AFX_VTWIDTHPPG_H__290FB5AB_FC70_4C18_90CC_52C14FB35DF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTWidthPpg.h : Declaration of the CVTWidthPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTWidthPropPage : See VTWidthPpg.cpp.cpp for implementation.

class CVTWidthPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTWidthPropPage)
	DECLARE_OLECREATE_EX(CVTWidthPropPage)

// Constructor
public:
	bool m_IsCreated;
	CVTWidthPropPage();

// Dialog Data
	//{{AFX_DATA(CVTWidthPropPage)
	enum { IDD = IDD_PROPPAGE_VTWIDTH };
	long	m_Max;
	long	m_Min;
	long	m_Width;
	BOOL	m_orientation;
	BOOL	m_checked;
	int		m_sizeHeight;
	int		m_sizeWidth;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTWidthPropPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTWIDTHPPG_H__290FB5AB_FC70_4C18_90CC_52C14FB35DF9__INCLUDED)
