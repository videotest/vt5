#if !defined(AFX_TABBEAM_H__5D7124A6_5B00_11D3_9999_0000B493A187__INCLUDED_)
#define AFX_TABBEAM_H__5D7124A6_5B00_11D3_9999_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabBeam.h : header file
//

class CTabEditor;

class CTab
{ 
public:
	long begPos;
	long endPos;
	CRgn rgnTab;
	CString strLabel;
	short nIndex;
};

#define CoveringNum			5
#define ScrollPosPerClick	8
#define MinTabWidth			40


/////////////////////////////////////////////////////////////////////////////
// CTabBeam window

class CTabBeam : public CWnd
{
// Construction
public:
	CTabBeam();

// Attributes
public:
	short m_nActiveTab;
	short m_numTabs;
	short m_nMaxTabIndex;
	short m_nCurLeftVisTab;
	long m_lLeftVisiblePos;
	long m_lTabsWidth;

	CFont m_fontActive;
	CFont m_fontInActive;
	CRect m_rc;
	bool m_partialRedraw;
#ifdef _DEBUG
	CMemoryState m_startMemState;
#endif
	CPtrArray m_arrTabs;
	CTabEditor* m_pEditor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabBeam)
	//}}AFX_VIRTUAL

// Implementation
public:
	long GetIndexByTab(long tab);
	void GetTabName(short idx, CString& strName);
	void RenameTab(short idx, CString newName);
	void DelTab(short Number, bool setActive = true);
	bool ScrollRight();
	bool ScrollLeft();
	void SetActiveTab(short newActive, bool setView = true);
	short GetActiveTab(){return m_nActiveTab;};
	bool NeedScrollRight();
	bool NeedScrollLeft();
	void AddTab(CString strLabel, bool setActive = true);
	virtual ~CTabBeam();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabBeam)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	bool	GetTabFromPoint(CPoint p, short& tab1, short& tab2);
	// [vanek] - 07.04.2004
	bool	GetTabFromPos(long lpos, short& ntab1, short& ntab2);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBEAM_H__5D7124A6_5B00_11D3_9999_0000B493A187__INCLUDED_)
