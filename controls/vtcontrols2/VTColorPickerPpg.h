#if !defined(AFX_VTCOLORPICKERPPG_H__B6114906_4D9B_4604_9C85_D3432336EA3E__INCLUDED_)
#define AFX_VTCOLORPICKERPPG_H__B6114906_4D9B_4604_9C85_D3432336EA3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTColorPickerPpg.h : Declaration of the CVTColorPickerPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTColorPickerPropPage : See VTColorPickerPpg.cpp.cpp for implementation.

class CVTColorPickerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTColorPickerPropPage)
	DECLARE_OLECREATE_EX(CVTColorPickerPropPage)

// Constructor
public:
	bool m_IsCreated;
	CVTColorPickerPropPage();

// Dialog Data
	//{{AFX_DATA(CVTColorPickerPropPage)
	enum { IDD = IDD_PROPPAGE_VTCOLORPICKER };
	BOOL	m_likeCombo;
	int		m_sizeHeight;
	int		m_sizeWidth;
	BOOL	m_BAW;
	BOOL	m_binary;
	BOOL	m_bEnable;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTColorPickerPropPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCOLORPICKERPPG_H__B6114906_4D9B_4604_9C85_D3432336EA3E__INCLUDED)
