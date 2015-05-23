#if !defined(AFX_VNMDICLIENT_H__FC9EC8F2_8175_11D1_A16B_000000000000__INCLUDED_)
#define AFX_VNMDICLIENT_H__FC9EC8F2_8175_11D1_A16B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "BCGMainClientAreaWnd.h"

class render;

class CMdiClient : public CBCGMainClientAreaWnd
{
	DECLARE_DYNCREATE(CMdiClient)

// Construction
public:
	CMdiClient();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMdiClient)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMdiClient();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMdiClient)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
		//set label to window (default - "VideoTesT 5.0"). 
	void SetLabel( const char  *pszLabel );
	const char *GetLabel(){return m_strLabel;}

	render		*m_prender;
protected:
	CString		m_strLabel;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VNMDICLIENT_H__FC9EC8F2_8175_11D1_A16B_000000000000__INCLUDED_)
