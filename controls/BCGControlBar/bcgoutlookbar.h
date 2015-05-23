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

#if !defined(AFX_BCGOUTLOOKBAR_H__D92D40D4_BD73_11D1_A63F_00A0C93A70EC__INCLUDED_)
#define AFX_BCGOUTLOOKBAR_H__D92D40D4_BD73_11D1_A63F_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// bcgoutlookbar.h : header file
//

#include "BCGToolbar.h"
#include "BCGToolbarImages.h"
#include "BCGOutlookButton.h"
#include "BCGButton.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGOutlookBar window

class BCGCONTROLBARDLLEXPORT CBCGOutlookBar : public CBCGToolBar
{
	DECLARE_DYNAMIC(CBCGOutlookBar)

// Construction
public:
	CBCGOutlookBar();
	virtual ~CBCGOutlookBar();

// Operations
public:
	BOOL AddButton (UINT uiImage, LPCTSTR lpszLabel, UINT iIdCommand);
	BOOL AddButton (UINT uiImage, UINT uiLabel, UINT iIdCommand);
	BOOL RemoveButton (UINT iIdCommand);

	void SetTextColor (COLORREF clrRegText, COLORREF clrSelText = 0/* Obsolete*/);
	void SetTransparentColor (COLORREF color);
	void SetBackImage (UINT uiImageID);
	void SetBackColor (COLORREF color);

protected:
	void ScrollUp ();
	void ScrollDown ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGOutlookBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
	//}}AFX_VIRTUAL

	virtual CSize CalcFixedLayout (BOOL bStretch, BOOL bHorz);
	virtual void AdjustLocations ();
	virtual void DoPaint(CDC* pDC);
	virtual int HitTest(CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGOutlookBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
public:
	COLORREF GetRegularColor () const
	{
		return m_clrRegText;
	}

	BOOL IsDrawShadedHighlight () const
	{
		return m_bDrawShadedHighlight;
	}

	BOOL IsBackgroundTexture () const
	{
		return m_bmpBack.GetCount () != 0;
	}

protected:
	COLORREF				m_clrRegText;
	COLORREF				m_clrTransparentColor;
	COLORREF				m_clrBackColor;

	CSize					m_csImage;
	CBCGToolBarImages		m_bmpBack;
	UINT					m_uiBackImageId;

	CBCGButton				m_btnUp;
	CBCGButton				m_btnDown;
	int						m_iScrollOffset;
	int						m_iFirstVisibleButton;
	BOOL					m_bScrollDown;

	CBCGToolBarImages		m_Images;
	BOOL					m_bDrawShadedHighlight;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGOUTLOOKBAR_H__D92D40D4_BD73_11D1_A63F_00A0C93A70EC__INCLUDED_)
