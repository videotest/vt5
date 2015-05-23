/////////////////////////////////////////////////////////////////////////////
// Rulers. Written By Stefan Ungureanu (stefanu@usa.net)
//

#if !defined(AFX_RULER_H__CC6B5F05_3281_11D2_B4F2_0000E8DB069D__INCLUDED_)
#define AFX_RULER_H__CC6B5F05_3281_11D2_B4F2_0000E8DB069D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "SomeDoc.h"

/////////////////////////////////////////////////////////////////////////////
// Ruler stuff

// ruler types
#define RT_VERTICAL					0x00000000
#define RT_HORIZONTAL				0x00000001

// hint information
#define VW_HSCROLL					0x00000001
#define VW_VSCROLL					0x00000002
#define VW_HPOSITION				0x00000003
#define VW_VPOSITION				0x00000004

#define DECLARE_FX_RULER(CLASSNAME)												\
protected:																		\
	CRulerSplitterWnd			m_##CLASSNAME##rulerSplitter;

#define IMPLEMENT_FX_RULER_SPLITTER(CLASSNAME, SPLITTER, ROW, COL, VIEWCLASS)	\
{																				\
	if (!m_##CLASSNAME##rulerSplitter.CreateStatic(&SPLITTER, 2, 2,				\
			WS_CHILD | WS_VISIBLE | WS_BORDER,									\
			SPLITTER.IdFromRowCol(ROW, COL)))									\
	{																			\
		TRACE0("Failed to create nested splitter\n");							\
		return FALSE;															\
	}																			\
																				\
	if (!m_##CLASSNAME##rulerSplitter.CreateView(0, 0,							\
		RUNTIME_CLASS(CRulerCornerView), CSize(0,0), pContext))					\
		return FALSE;															\
	if (!m_##CLASSNAME##rulerSplitter.CreateView(0, 1,							\
		RUNTIME_CLASS(CRulerView), CSize(0,0), pContext))						\
		return FALSE;															\
	if (!m_##CLASSNAME##rulerSplitter.CreateView(1, 0,							\
		RUNTIME_CLASS(CRulerView), CSize(0,0), pContext))						\
		return FALSE;															\
	if (!m_##CLASSNAME##rulerSplitter.CreateView(1, 1,							\
		VIEWCLASS, CSize(0,0), pContext))										\
		return FALSE;															\
																				\
	m_##CLASSNAME##rulerSplitter.SetColumnInfo(0, 16, 0);						\
	m_##CLASSNAME##rulerSplitter.SetRowInfo(0, 16, 0);							\
																				\
	((CRulerView*)m_##CLASSNAME##rulerSplitter.GetPane(0, 1))->					\
		SetRulerType(RT_HORIZONTAL);											\
	((CRulerView*)m_##CLASSNAME##rulerSplitter.GetPane(1, 0))->					\
		SetRulerType(RT_VERTICAL);												\
}

/////////////////////////////////////////////////////////////////////////////
// CRulerSplitterWnd window

class CRulerSplitterWnd : public CSplitterWnd
{
// Construction
public:
	CRulerSplitterWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulerSplitterWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRulerSplitterWnd();
	int HitTest(CPoint pt) const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRulerSplitterWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRulerView view

class CRulerView : public CView
{
protected:
	CRulerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRulerView)

// Attributes
public:
	UINT		m_rulerType;

// Attributes
public:
	//CSomeDoc* GetDocument();

// Operations
public:
	int m_scrollPos;
	void SetRulerType(UINT rulerType =RT_HORIZONTAL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulerView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRulerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	int m_lastPos;
	//{{AFX_MSG(CRulerView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/*
#ifndef _DEBUG  // debug version in SmartMorphView.cpp
inline CSomeDoc* CRulerView::GetDocument()
   { return (CSomeDoc*)m_pDocument; }
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView view

class CRulerCornerView : public CView
{
protected:
	CRulerCornerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRulerCornerView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulerCornerView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRulerCornerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRulerCornerView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H__CC6B5F05_3281_11D2_B4F2_0000E8DB069D__INCLUDED_)
