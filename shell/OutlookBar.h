#if !defined(AFX_OUTLOOKBAR_H__39AE2BD0_4EB5_458A_9AA3_C3E4773AFF95__INCLUDED_)
#define AFX_OUTLOOKBAR_H__39AE2BD0_4EB5_458A_9AA3_C3E4773AFF95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutlookBar.h : header file
//
#include "Outlook\\GfxOutBarCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COutlookBar window

class COutlookBar : public CGfxOutBarCtrl
{
// Construction
public:
	DECLARE_DYNAMIC(COutlookBar);

	COutlookBar();

// Attributes
public:
	CBCGToolBar *AddFolderBar( const char *szName ); 
	void		DeleteFolderBar( CBCGToolBar *pbar );
	void		DeleteAllFolderBars();

	CString	GetFolderName( int nFolder );
	CWnd	*GetFolderWindow( int nFolder );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutlookBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutlookBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutlookBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTLOOKBAR_H__39AE2BD0_4EB5_458A_9AA3_C3E4773AFF95__INCLUDED_)
