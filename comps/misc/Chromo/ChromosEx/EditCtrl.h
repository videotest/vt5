#pragma once


// CEditCtrl

class CEditCtrl : public CEdit
{
		bool m_bCtrlPressed;
// Construction		   
public:
	CEditCtrl() :m_x(0),m_y(0),m_bEscapeKey(FALSE) { m_bCtrlPressed = false; }
	void SetCtrlFont( CFont* pFont );

// Attributes
protected:
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	BOOL m_bEscapeKey;
	CFont m_Font;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	//}}AFX_VIRTUAL	
// Implementation
public:
	virtual ~CEditCtrl() {};			
	// Generated message map functions
protected:
	//{{AFX_MSG(CLVEdit)	
	afx_msg void OnKillfocus();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:	

	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	/*
public:
	vitual BOOL PreTranslateMessage(MSG* pMsg);
	*/
	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


