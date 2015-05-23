#if !defined(AFX_VTSTATICTEXTPPG_H__EC31CB6B_4777_4E79_BE3C_D5705666CB0F__INCLUDED_)
#define AFX_VTSTATICTEXTPPG_H__EC31CB6B_4777_4E79_BE3C_D5705666CB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTStaticTextPpg.h : Declaration of the CVTStaticTextPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPage : See VTStaticTextPpg.cpp.cpp for implementation.

class CVTStaticTextPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTStaticTextPropPage)
	DECLARE_OLECREATE_EX(CVTStaticTextPropPage)

// Constructor
public:
	CVTStaticTextPropPage();

// Dialog Data
	//{{AFX_DATA(CVTStaticTextPropPage)
	enum { IDD = IDD_PROPPAGE_VTSTATICTEXT };
	BOOL	m_bUseSystemFont;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTStaticTextPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt : Property page dialog

class CVTStaticTextPropPageExt : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTStaticTextPropPageExt)
	DECLARE_OLECREATE_EX(CVTStaticTextPropPageExt)

// Constructors
public:
	CVTStaticTextPropPageExt();

// Dialog Data
	//{{AFX_DATA(CVTStaticTextPropPageExt)
	enum { IDD = IDD_PROPPAGE_VTSTATICTEXT_EXT };
	BOOL	m_bTextAlignTop;
	BOOL	m_bTextAlignRght;
	BOOL	m_bTextAlignLeft;
	BOOL	m_bTextAlignBottom;
	BOOL	m_bTextAlignCenter;
	BOOL	m_bTextAlignMiddle;
	BOOL	m_bPictAlignRight;
	BOOL	m_bPictAlignLeft;
	BOOL	m_bPictAlignTop;
	BOOL	m_bPictAlignBottom;
	BOOL	m_bText2PictBottom;
	BOOL	m_bText2PictLeft;
	BOOL	m_bText2PictRight;
	BOOL	m_bText2PictTop;
	CString	m_strPictureFileName;
	CString	m_strText;
	BOOL	m_bCenterImage;
	BOOL	m_bTransparencyText;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTStaticTextPropPageExt)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileopen();
	afx_msg void OnReloadBitmap();
	afx_msg void OnAutoSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSTATICTEXTPPG_H__EC31CB6B_4777_4E79_BE3C_D5705666CB0F__INCLUDED)
