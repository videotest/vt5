#if !defined(AFX_IMAGEPPG_H__BC127B47_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_IMAGEPPG_H__BC127B47_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ImagePpg.h : Declaration of the CVTImagePropPage property page class.
#include "jdraw\\ColourPicker.h"
#include "jdraw\\ArrowPicker.h"
#include "jdraw\\dashline.h"
#include "jdraw\\linePicker.h"

////////////////////////////////////////////////////////////////////////////
// CVTImagePropPage : See ImagePpg.cpp.cpp for implementation.

class CVTImagePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTImagePropPage)
	DECLARE_OLECREATE_EX(CVTImagePropPage)

// Constructor
public:
	CVTImagePropPage();

// Dialog Data
	//{{AFX_DATA(CVTImagePropPage)
	enum { IDD = IDD_PROPPAGE_VTDIMAGE };
	CColourPicker	m_transparentColorPicker;	
	CColourPicker	m_bkColorPicker;	
	CString	m_strFileName;	
	int		m_bPermanentImage;
	//}}AFX_DATA
	COLORREF m_transparentColor;	
	COLORREF m_bkColor;	
	BOOL	m_bLockUpdateControl;	// lock update data of CVTImage

// Implementation
protected:
	void UpdateCtrl();
	void UpdateDataWithOutUpdateAX();	// update data without updates data's AX control
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTImagePropPage)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnColorChange(WPARAM lParam, LPARAM wParam);
	afx_msg void OnLoadPermanent();
	afx_msg void OnBrowseFile();
	afx_msg void OnPermanent();
	afx_msg void OnLinkWithFile();
	afx_msg void OnRadio2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPPG_H__BC127B47_026C_11D4_8124_0000E8DF68C3__INCLUDED)
