#if !defined(AFX_VTKEYBUTTONIMAGEPROPPAGE_H__959C425B_ABC3_476F_9A4F_877BD1B7A145__INCLUDED_)
#define AFX_VTKEYBUTTONIMAGEPROPPAGE_H__959C425B_ABC3_476F_9A4F_877BD1B7A145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTKeyButtonImagePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage : Property page dialog

class CVTKeyButtonImagePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTKeyButtonImagePropPage)
	DECLARE_OLECREATE_EX(CVTKeyButtonImagePropPage)

// Constructors
public:
	CVTKeyButtonImagePropPage();

// Dialog Data
	//{{AFX_DATA(CVTKeyButtonImagePropPage)
	enum { IDD = IDD_PROPPAGE_VTKEY_IMAGE };
	CString	m_strFileName;
	short	m_nImageIndex;
	short	m_nImageHeight;
	short	m_nImageWidth;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTKeyButtonImagePropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFileopen();
	afx_msg void OnButtonReload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTKEYBUTTONIMAGEPROPPAGE_H__959C425B_ABC3_476F_9A4F_877BD1B7A145__INCLUDED_)
