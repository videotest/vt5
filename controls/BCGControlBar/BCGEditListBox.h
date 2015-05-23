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

#if !defined(AFX_BCGEDITLISTBOX_H__80D80812_B943_11D3_A713_009027900694__INCLUDED_)
#define AFX_BCGEDITLISTBOX_H__80D80812_B943_11D3_A713_009027900694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCGEditListBox.h : header file
//

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "bcgcontrolbar.h"
#include "BCGButton.h"

#define BGCEDITLISTBOX_BTN_NEW		0x0001
#define BGCEDITLISTBOX_BTN_DELETE	0x0002
#define BGCEDITLISTBOX_BTN_UP		0x0004
#define BGCEDITLISTBOX_BTN_DOWN		0x0008

/////////////////////////////////////////////////////////////////////////////
// CBCGEditListBox window

class BCGCONTROLBARDLLEXPORT CBCGEditListBox : public CStatic
{
// Construction
public:
	CBCGEditListBox();

// Attributes
public:
	CListCtrl	m_wndList;	// Embedded list control

protected:
	CList<CBCGButton*, CBCGButton*>	m_lstButtons;
	CList<DWORD, DWORD>				m_lstKeyAccell;
	CSize							m_sizeButton;
	CRect							m_rectCaption;
	UINT							m_uiStandardBtns;
	BOOL							m_bNewItem;

// Operations
public:
	BOOL SetStandardButtons (UINT uiBtns =
		BGCEDITLISTBOX_BTN_NEW | BGCEDITLISTBOX_BTN_DELETE | BGCEDITLISTBOX_BTN_UP | BGCEDITLISTBOX_BTN_DOWN);

	BOOL AddButton (UINT uiImageResId, LPCTSTR lpszTooltip = NULL,
					WORD wKeyAccelerator = 0,
					BYTE fVirt = 0);

	int AddItem (const CString& strIext, DWORD dwData = 0, int iIndex = -1);
	BOOL RemoveItem (int iIndex);
	int GetCount () const;

	int GetSelItem () const;
	BOOL SelectItem (int iItem);

	CString GetItemText (int iIndex) const;
	DWORD GetItemData (int iIndex) const;
	void SetItemData (int iIndex, DWORD dwData);

	BOOL EditItem (int iIndex);

protected:
	void Init ();
	int GetStdButtonNum (UINT uiStdBtn) const;
	void AdjustLayout ();
	virtual void CreateNewItem ();

// Overrides
public:
	virtual void OnClickButton (int iButton);
	virtual void OnKey (WORD wKey, BYTE fFlags);
	virtual void OnEndEditLabel (LPCTSTR lpszLabel);
	virtual int OnGetImage (LV_ITEM* /*pItem*/)
	{
		return -1;
	}
	
	virtual void OnSelectionChanged () {}

	// "Standard" action overrides
	virtual BOOL OnBeforeRemoveItem (int /*iItem*/)	{	return TRUE;	}
	virtual void OnAfterAddItem (int /*iItem*/) {}
	virtual void OnAfterRenameItem (int /*iItem*/) {}
	virtual void OnAfterMoveItemUp (int /*iItem*/) {}
	virtual void OnAfterMoveItemDown (int /*iItem*/) {}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGEditListBox)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGEditListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGEditListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
#if _MSC_VER >= 1300
	afx_msg void OnKeyDown (NMHDR*pKeyDown, LRESULT* pResult);
#else
	afx_msg void OnKeyDown (LPNMLVKEYDOWN pKeyDown, LRESULT* pResult);
#endif


	afx_msg void OnDblclkList (NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGEDITLISTBOX_H__80D80812_B943_11D3_A713_009027900694__INCLUDED_)
