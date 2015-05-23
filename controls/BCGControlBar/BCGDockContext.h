//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#if !defined(AFX_BCGDOCKCONTEXT_H__A19352D0_7DE0_11D3_A9DB_005056800000__INCLUDED_)
#define AFX_BCGDOCKCONTEXT_H__A19352D0_7DE0_11D3_A9DB_005056800000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCGDockContext.h :
//

/////////////////////////////////////////////////////////////////////////////
// CBCGDockContext - by Erwin Tratar 
//andy
class BCGCONTROLBARDLLEXPORT CBCGDockContext : public CDockContext
{
public:
	CBCGDockContext(CControlBar* pBar);

	virtual void StartDrag(CPoint pt);
	void Stretch(CPoint pt);
	void StartResize(int nHitTest, CPoint pt);
	BOOL Track();

public:
	virtual ~CBCGDockContext();
};

/////////////////////////////////////////////////////////////////////////
// CBCGMiniDockFrameWnd class - by Erwin Tratar 
//andy
class BCGCONTROLBARDLLEXPORT CBCGMiniDockFrameWnd : public CMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CBCGMiniDockFrameWnd)

public:
	friend class CDockBar;

	CBCGMiniDockFrameWnd();
	virtual ~CBCGMiniDockFrameWnd();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGMiniDockFrameWnd)
	public:
	virtual BOOL Create(CWnd* pParent, DWORD dwBarStyle);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CBCGMiniDockFrameWnd)
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_BCGDOCKCONTEXT_H__A19352D0_7DE0_11D3_A9DB_005056800000__INCLUDED_
