#if !defined(AFX_SHELLSTATUSBAR_H__3FB99C88_9385_476B_9E96_FCCAE43131BE__INCLUDED_)
#define AFX_SHELLSTATUSBAR_H__3FB99C88_9385_476B_9E96_FCCAE43131BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "statusint.h"
// ShellStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShellStatusBar window

class CShellStatusBar : public CStatusBar,
						public CWindow2Impl
{
	ENABLE_MULTYINTERFACE()
// Construction
public:
	struct PaneInfo
	{
		DWORD	dwFlags;
		int		cx;
		HWND	hwnd;
		GUID	guidPane;
		long	nRefCounter;
		long	nID;
		HICON	hIcon;

		PaneInfo()		{hIcon = 0;	}
		~PaneInfo()		{if( hIcon )::DestroyIcon( hIcon );	}
	};

public:
	CShellStatusBar();
	virtual ~CShellStatusBar();
// Attributes
public:
	void RecalcLayout();
	long GetPanesCount()				{return m_panes.GetSize();}
	PaneInfo *GetPaneInfo( int idx )	{return (PaneInfo*)m_panes[idx];}


	TPOS SetXPBar(HWND hwndXPBar);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellStatusBar)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	virtual CWnd *GetWindow()		{return this;}
	//{{AFX_MSG(CShellStatusBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnSetPaneText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(Status, INewStatusBar)
		com_call AddPane( const GUID &guidPane, int cxWidth, HWND hwnd, DWORD dwStyle );
		com_call RemovePane( const GUID &guidPane );
		com_call GetPaneID( const GUID &guidPane, long *pnID );
		com_call SetPaneIcon( const GUID &guidPane, HICON hIcon );
	END_INTERFACE_PART(Status)


protected:
	PaneInfo	*_Find( const GUID &guid );

	long				m_nPercent;
	CPtrArray	m_panes;


	HWND		m_hwndXPBar;
	TPOS		lposStatus_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLSTATUSBAR_H__3FB99C88_9385_476B_9E96_FCCAE43131BE__INCLUDED_)
