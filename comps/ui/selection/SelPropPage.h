//{{AFX_INCLUDES()
#include "pushbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_SELPROPPAGE_H__5309AD02_5942_4731_B2BD_8CB62F9D1E27__INCLUDED_)
#define AFX_SELPROPPAGE_H__5309AD02_5942_4731_B2BD_8CB62F9D1E27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelPropPage dialog

class CSelPropPage : public CPropertyPageBase, CMsgListenerImpl
{
  PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CSelPropPage);
	GUARD_DECLARE_OLECREATE(CSelPropPage);
	ENABLE_MULTYINTERFACE();
	bool m_bAttached;
	CBrush m_brush;
	CToolTipCtrl m_ToolTip;
// Construction
public:
	CSelPropPage(CWnd* pParent = NULL);   // standard constructor


  virtual void DoLoadSettings();

// Dialog Data
	//{{AFX_DATA(CSelPropPage)
	enum { IDD = IDD_PROPPAGE_SELECTION };
	CSpinButtonCtrl	m_spinSmooth;
	CSpinButtonCtrl	m_spinMagick;
	CPushButton	m_pbModePlus;
	CPushButton	m_pbModeMinus;
	CPushButton	m_pbType1;
	CPushButton	m_pbType2;
	BOOL	m_bDynamic;
	long	m_nBright;
	long	m_nSmooth;
	//}}AFX_DATA
	bool	m_bBrightValid;
	bool	m_bSmoothValid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void _ManageTypeButtons();

	// Generated message map functions
	//{{AFX_MSG(CSelPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickPbModePlus();
	afx_msg void OnClickPbModeMinus();
	afx_msg void OnClickPbType1();
	afx_msg void OnClickPbType2();
	afx_msg void OnDynamicBrightness();
	afx_msg void OnChangeEditMagick();
	afx_msg void OnChangeEditSmooth();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusMagic();
	afx_msg void OnKillfocusSmooth();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()

long nnn;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );
};


/////////////////////////////////////////////////////////////////////////////
// CFixedSelectPage dialog

class CFixedSelectPage : public CPropertyPageBase
{
// Construction
public:
	CFixedSelectPage( UINT nID );   // standard constructor
	~CFixedSelectPage();

// Dialog Data
	//{{AFX_DATA(CFixedSelectPage)
	enum { IDD = IDD_PROPPAGE_FIXED_SELECTION };
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedSelectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFixedSelectPage)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeSizes();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual const char *GetSection() const;
	virtual const char *GetWidthEntry() const = 0;
	virtual const char *GetHeightEntry() const = 0;
	virtual const char *GetCalibrEntry() const {return "UseCalibration";}
protected:
	void _UpdateEditors();
	void _UpdateValues();

protected:
	long	m_nWidth;
	long	m_nHeight;
	bool	m_bUseCalibration;
};


class CFixedSelRectPropPage : public CFixedSelectPage
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CFixedSelRectPropPage);
	GUARD_DECLARE_OLECREATE(CFixedSelRectPropPage);

	CFixedSelRectPropPage();

	virtual const char *GetWidthEntry() const;
	virtual const char *GetHeightEntry() const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELPROPPAGE_H__5309AD02_5942_4731_B2BD_8CB62F9D1E27__INCLUDED_)
