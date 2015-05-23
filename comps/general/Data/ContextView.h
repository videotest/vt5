#if !defined(AFX_CONTEXTVIEW_H__A07DB69A_6711_446C_88CE_C16FE2239753__INCLUDED_)
#define AFX_CONTEXTVIEW_H__A07DB69A_6711_446C_88CE_C16FE2239753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContextView.h : header file
//

#include "ContextWrp.h"
/////////////////////////////////////////////////////////////////////////////
// CContextView window

class CContextView : public CWnd
{
// Construction
public:
	CContextView();
	virtual ~CContextView();

// Attributes
public:
	CContextWrpType		m_TypeTree;
	CContextWrpObject	m_ObjTree;

// Operations
public:
/*
    void SetEditable(BOOL bEditable = TRUE);
    BOOL IsEditable() const;
    void EnableTracking(BOOL bEnable = TRUE);
    BOOL IsTracking() const;

    int  GetDefItemHeight() const;
    void SetDefItemHeight(int nHeight);
    int  GetDefSmallItemHeight() const;
    void SetDefSmallItemHeight(int nHeight);
    int  GetDefHeaderHeight() const;
    void SetDefHeaderHeight(int nHeight);
	int  GetIndent() const;
	void SetIndent(int nIndent);
	int	 GetMargin();
	void SetMargin(int nMargin);
	int	 GetBorder();
	void SetBorder(int nBorder);
	int	 GetIconSize();
	void SetIconSize(int nIconSize);
	int	 GetSmallIconSize();
	void SetSmallIconSize(int nIconSize);
	int	 GetNumberOffset();
	void SetNumberOffset(int nOffset);
	int	 GetImageOffset();
	void SetImageOffset(int nOffset);

    void EnableTitleTips(BOOL bEnable = TRUE);
    BOOL GetTitleTips()

	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT);
	COLORREF GetTitleTipBackClr();
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT);
	COLORREF GetTitleTipTextClr();

    void     SetTextColor(COLORREF clr);
    COLORREF GetTextColor() const;
    void     SetTextBkColor(COLORREF clr);
    COLORREF GetTextBkColor() const;

    COLORREF GetControlTextColor() const;
    void     SetControlTextColor(COLORREF clr);
    COLORREF GetControlBkColor() const;
    void     SetControlBkColor(COLORREF clr);
    COLORREF GetControlLightColor() const;
    void     SetControlLightColor(COLORREF clr);
    COLORREF GetControlHighlightColor() const;
    void     SetControlHighlightColor(COLORREF clr);
    COLORREF GetControlShadowColor() const;
    void     SetControlShadowColor(COLORREF clr);
    COLORREF GetControlDKShadowColor() const;
    void     SetControlDKShadowColor(COLORREF clr);
	COLORREF GetTrackColor();
	void	 SetTrackColor(COLORREF clr);

	bool RegisterToApp();
	bool UnregisterFromApp();
	
	bool RegisterToDocument();
	bool UnregisterFromDocument();

	void SetViewObj(bool bShow = false);
	void SetViewType(bool bShow = true);
	bool IsViewType()	{	return m_bViewType;		}
	bool IsViewObj()	{	return !m_bViewType;	}

	void SetNumeric();
	void RemoveNumeric();

	void Expand(bool bExpand = true);

	void EnableMenu(BOOL bEnable = TRUE);
	BOOL IsMenuEnabled();
*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextView)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CContextView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTVIEW_H__A07DB69A_6711_446C_88CE_C16FE2239753__INCLUDED_)
