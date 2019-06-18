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

// BCGToolbarButton.h: interface for the CBCGToolbarButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGTOOLBARBUTTON_H__E0D2D9D5_C494_11D1_A646_00A0C93A70EC__INCLUDED_)
#define AFX_BCGTOOLBARBUTTON_H__E0D2D9D5_C494_11D1_A646_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXOLE_H__
	#include <afxole.h>
#endif

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

class CBCGToolBarImages;
class CBCGToolBar;
class CBCGToolbarMenuButton;

#include "bcgcontrolbar.h"

#define TBBS_BREAK 0x20000000

class BCGCONTROLBARDLLEXPORT CBCGToolbarButton : public CObject
{
	DECLARE_SERIAL(CBCGToolbarButton)

public:
	CBCGToolbarButton();
	CBCGToolbarButton(UINT uiID, int iImage, LPCTSTR lpszText = NULL, 
		BOOL bUserButton = FALSE, BOOL bLocked = FALSE);
	virtual ~CBCGToolbarButton();

// Operations:
public:

	//--------------------
	// Drag and drop stuf:
	//--------------------
	static CLIPFORMAT GetClipboardFormat ();
	static CBCGToolbarButton* CreateFromOleData (COleDataObject* pDataObject);
	BOOL PrepareDrag (COleDataSource& srcItem);

	virtual BOOL CanBeDropped (CBCGToolBar* /*pToolbar*/)	{	return TRUE; }

	//-----------------------------------------------------------------
	//	Protected commands support. 
	//	Protected buttons will disabled in customization mode, so user 
	//	will be unable to drag/drop/change them.
	//-----------------------------------------------------------------
	static void SetProtectedCommands (const CList<UINT, UINT>& lstCmds);
	static const CList<UINT, UINT>& GetProtectedCommands ()	{	return m_lstProtectedCommands;	}

// Overrides:
	virtual void CopyFrom (const CBCGToolbarButton& src);
	virtual void Serialize (CArchive& ar);
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
						BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
						BOOL bHighlight = FALSE,
						BOOL bDrawBorder = TRUE,
						BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	//AY
	virtual BOOL OnPreClick(CWnd* pWnd){pWnd;return false;};
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE)		
	{	
		pWnd;
		bDelay;
		return FALSE;
	}
	
	virtual BOOL OnClickUp() {return FALSE;}  // By Alex Corazzin
	virtual void OnChangeParentWnd (CWnd* /*pWndParent*/) {}
	virtual BOOL ExportToMenuButton (CBCGToolbarMenuButton& menuButton) const;
	virtual void OnMove ()								{}
	virtual void OnSize (int /*iSize*/)					{}
	virtual HWND GetHwnd ()								{	return NULL;	}
	virtual BOOL CanBeStretched () const				{	return FALSE;	}
	virtual BOOL NotifyCommand (int /*iNotifyCode*/)	{	return FALSE;	}
	virtual void OnAddToCustomizePage ()		{}
	virtual HBRUSH OnCtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/)	{	return NULL;	}
	virtual void OnDblClick (CWnd* /*pWnd*/)			{}
	virtual BOOL CanBeStored () const					{	return TRUE;	}
	virtual BOOL HaveHotBorder () const					{	return TRUE;	}
	virtual void OnCancelMode ()						{}
	virtual BOOL IsEditable () const
	{
		return !IsStandardCommand (m_nID) && 
				(m_lstProtectedCommands.Find (m_nID) == NULL);
	}
	
	virtual BOOL OnContextHelp (CWnd* /* pWnd */)
	{
		return FALSE;
	}

	virtual int OnDrawOnCustomizeList (CDC* pDC, const CRect& rect, 
										BOOL bSelected);

	virtual BOOL IsDroppedDown () const
	{
		return FALSE;
	}

	virtual BOOL OnBeforeDrag () const
	{
		return TRUE;	// Drag is possible
	}

	virtual BOOL OnToolHitTest(const CWnd* pWnd, TOOLINFO* pTI);
	virtual void SaveBarState () {}

	virtual void OnShow (BOOL /*bShow*/)	{}

	virtual const CRect GetInvalidateRect () const
	{
		return m_rect;
	}

protected:
	void Initialize ();

// Attributes:
public:
	BOOL IsDrawText () const
	{
		return m_bText && !m_strText.IsEmpty ();
	}

	BOOL IsDrawImage () const
	{
		return m_bImage && GetImage () >= 0;
	}

	int GetImage () const
	{
		return m_bUserButton ? m_iUserImage : m_iImage;
	}

	int GetMainImage() const
	{
		return m_iImage;
	}

	virtual void SetImage (int iImage);

	BOOL IsLocked () const
	{
		return m_bLocked;
	}

	void SetRect (const CRect rect)
	{
		if (m_rect != rect)
		{
			m_rect = rect;
			OnMove ();
		}
	}

	const CRect& Rect () const
	{
		return m_rect;
	}

	void Show (BOOL bShow)
	{
		if (m_bIsHidden != !bShow)
		{
			m_bIsHidden = !bShow;
			OnShow (bShow);
		}
	}

	BOOL IsHidden () const
	{
		return m_bIsHidden;
	}

	BOOL	m_bUserButton;	// Is user-defined tool button?
	UINT	m_nID;			// Command ID that this button sends
	UINT	m_nStyle;		// TBBS_ styles
	CString	m_strText;		// Button text (for user-defined buttons only!)
	BOOL	m_bText;		// Draw text label
	BOOL	m_bImage;		// Draw image
	BOOL	m_bWrap;		// Wrap toolbar
	BOOL	m_bWholeText;	// Is whole text printed?
	BOOL	m_bTextBelow;	// Is text below image?

	// Run-time properties:
	BOOL	m_bDragFromCollection;	// Button is dragged from collection
	static CLIPFORMAT	m_cFormat;

protected:
	CRect	m_rect;			// Button location
	int		m_iImage;		// index into bitmap of this button's picture
	int		m_iUserImage;	// index into user's bitmap of this button's picture

	BOOL	m_bLocked;		// Is buttons placed on the "locked" toolbar
	BOOL	m_bIsHidden;	// Button rectangle is out of bar

	CSize	m_sizeText;		// Actual only if m_bTextBelow is TRUE

	static CList<UINT, UINT>	m_lstProtectedCommands;
									// Buttons are disabled in customization mode

// diagnostics:
public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
//!!!andy modification
	static bool	s_bInitProgMode;
//end
};

#endif // !defined(AFX_BCGTOOLBARBUTTON_H__E0D2D9D5_C494_11D1_A646_00A0C93A70EC__INCLUDED_)
