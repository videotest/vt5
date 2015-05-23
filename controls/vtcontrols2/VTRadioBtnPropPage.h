#pragma once
#include "afxwin.h"
#include "atlstr.h"
#include "Resource.h"

// Move
#define	MOVE_UP		1
#define	MOVE_DOWN	2

// VTRadioBtnPropPage.h : Declaration of the CVTRadioBtnPropPage property page class.


// CVTRadioBtnPropPage : See VTRadioBtnPropPage.cpp.cpp for implementation.

class CVTRadioBtnPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTRadioBtnPropPage)
	DECLARE_OLECREATE_EX(CVTRadioBtnPropPage)

// Constructor
public:
	CVTRadioBtnPropPage();

	virtual BOOL OnInitDialog( );
	
// Dialog Data
	enum { IDD = IDD_PROPPAGE_VTRADIOBTN };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()

	bool	m_bIsCreated;

	IDispatch*	GetRBInstance( );
public:
	afx_msg void OnBnClickedButton1();
	// list of strings
	CListBox m_list;
	CEdit m_edit;
	afx_msg void OnLbnSelchangeList2();
	// UP button
	CButton m_buttonUp;
	// DOWN Button
	CButton m_buttonDown;
	// DELETE Button
	CButton m_buttonDel;
	// UPDATE Button
	CButton m_buttonUpdate;
	// ADD Button
	CButton m_buttonAdd;

	void	MoveListString( int move );
	void	UpdateEdit( );
	void	UpdateBtns( );
	void	UpdateUpDownBtns( int iSelIndex );
	bool	LoadFromControl( );	
	
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};



// CVTRadioBtnPropPageTwo
class CVTRadioBtnPropPageTwo : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTRadioBtnPropPageTwo)
	DECLARE_OLECREATE_EX(CVTRadioBtnPropPageTwo)

// Constructor
public:
	CVTRadioBtnPropPageTwo();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VTRADIOBTN_TWO };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()

	BOOL	m_bEnable,	m_bVisible,	m_bEvenlySpaced;
public:
	
};
