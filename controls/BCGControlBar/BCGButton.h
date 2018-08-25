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

#if !defined(AFX_BCGBUTTON_H__C159C4C6_B79B_11D3_A712_009027900694__INCLUDED_)
#define AFX_BCGBUTTON_H__C159C4C6_B79B_11D3_A712_009027900694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCGButton.h : header file
//

//#include "MenuImages.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGButton window

class BCGCONTROLBARDLLEXPORT CBCGButton : public CButton
{
	DECLARE_DYNAMIC(CBCGButton)

// Construction
public:
	CBCGButton();

// Attributes
public:
	enum FlatStyle
	{
		BUTTONSTYLE_3D,
		BUTTONSTYLE_FLAT,
		BUTTONSTYLE_SEMIFLAT,
		BUTTONSTYLE_NOBORDERS
	};

	FlatStyle		m_nFlatStyle;
	BOOL			m_bRighImage;

	CToolTipCtrl& GetToolTipCtrl ()
	{
		return m_wndToolTip;
	}

public:
	BOOL IsPressed () const
	{
		return m_bPushed && m_bHighlighted;
	}

protected:
	BOOL			m_bPushed;
	BOOL			m_bHighlighted;
	BOOL			m_bCaptured;
	BOOL			m_bHover;

	CSize			m_sizeImage;
	HICON			m_hIcon;
	HBITMAP			m_hBitmap;
	HICON			m_hIconHot;
	HBITMAP			m_hBitmapHot;
	BOOL			m_bAutoDestroyImage;

	CMenuImages::IMAGES_IDS		m_nStdImageId;

	CToolTipCtrl	m_wndToolTip;

	HCURSOR			m_hCursor;

	CSize			m_sizePushOffset;

// Operations
public:
	void SetImage (HICON hIcon, BOOL bAutoDestroy = TRUE, HICON hIconHot = NULL);
	void SetImage (HBITMAP hBitmap, BOOL bAutoDestroy = TRUE, HBITMAP hBitmapHot = NULL);
	void SetImage (UINT uiBmpResId, UINT uiBmpHotResId = 0);
	void SetStdImage (CMenuImages::IMAGES_IDS id);

	void SetTooltip (LPCTSTR lpszToolTipText);	// lpszToolTip == NULL: disable tooltip

	void SetMouseCursor (HCURSOR hcursor);
	void SetMouseCursorHand ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGButton();
	virtual void CleanUp ();

	virtual CSize SizeToContent (BOOL bCalcOnly = FALSE);

protected:
	virtual void OnDrawFocusRect (CDC* pDC, const CRect& rectClient);
	virtual void OnDraw (CDC* pDC, const CRect& rect, UINT uiState);

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGBUTTON_H__C159C4C6_B79B_11D3_A712_009027900694__INCLUDED_)
