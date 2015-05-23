#if !defined(AFX_PICKERCTRL_H__D25927B4_51E7_11D4_AEE6_0000E8DF68C3__INCLUDED_)
#define AFX_PICKERCTRL_H__D25927B4_51E7_11D4_AEE6_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PickerCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPickerListBox window

class CPickerListBox : public CListBox
{
// Construction
public:
	CPickerListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickerListBox)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPickerListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPickerListBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	HWND m_hWndReciever;
public:
	void SetReciver( HWND hWndReciever ){ m_hWndReciever = hWndReciever;}
};


/////////////////////////////////////////////////////////////////////////////
// CPickerPopupWnd window

class CPickerCtrl;
class CPickerPopupWnd : public CWnd
{
// Construction
public:
	CPickerPopupWnd( );	
	CFont m_Font;
	CPickerCtrl *m_pPickerCtrl;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickerPopupWnd)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPickerPopupWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPickerPopupWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CPickerListBox m_ctrlListBox;
};




/////////////////////////////////////////////////////////////////////////////
// CPickerCtrl window

enum PickerState{
	psUnpressed = 0,
	psPressed = 1
};

class CPickerCtrl : public CWnd
{
// Construction
public:
	CPickerCtrl();
	CSize GetMinDimension(){return CSize(20,20);}

	void ResetContext(){}
	void AddString(CString strAdd){}

	void SetPopupInitRect( CRect rcPopupInit) { m_rcPopupInit = rcPopupInit;}
	void SetPickerOldRect( CRect rcPickerOldRect) { m_PickerOldRect = rcPickerOldRect;}

	void SetDisabled( BOOL bDisabled ) { m_bDisabled = bDisabled; }
	BOOL GetDisabled( ) { return m_bDisabled; }

// Attributes
public:
	bool m_bClickedFromPopup;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickerCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPickerCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPickerCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	PickerState m_pickerState;	 
	BOOL		m_bDisabled;
			
	PickerState GetState();
	void SetState( PickerState state );	


	CPickerPopupWnd* m_pPopupWnd;

	CRect m_rcPopupInit;
	CRect m_PickerOldRect;


	
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#define ID_LIST_BOX_PICKER 201

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICKERCTRL_H__D25927B4_51E7_11D4_AEE6_0000E8DF68C3__INCLUDED_)
