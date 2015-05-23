#pragma once

//#include "calc_int.h"
//#include "\vt5\comps\misc\Calculator\Calculator.h"
#include "DrawParam.h"


class CBlinkingCursor
{
	HDC m_dc;
	RECT m_rect;
	COLORREF m_clrBack, m_clrFore;
public:
	CBlinkingCursor(){};
	~CBlinkingCursor(){};

	void Draw(HDC dc, RECT rect);
	void Hide(HWND hwnd, RECT rect);
	void SetBkColor(COLORREF color);
	void SetForeColor(COLORREF color);

};


// DisplUserParamCtrl.h : Declaration of the CDisplUserParamCtrl ActiveX Control class.


// CDisplUserParamCtrl : See DisplUserParamCtrl.cpp for implementation.

class CDisplUserParamCtrl : public COleControl
{
	DECLARE_DYNCREATE(CDisplUserParamCtrl)

	CString	m_sErrorDescription;	
	UINT_PTR m_timer;
	BOOL m_bCursor;
	LOGFONT m_lf;
// Constructor
public:
	CDisplUserParamCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
protected:
	~CDisplUserParamCtrl();

	DECLARE_OLECREATE_EX(CDisplUserParamCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CDisplUserParamCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDisplUserParamCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CDisplUserParamCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		eventidContentChanged = 1L,			dispidInsert = 9,	dispidGetLastErrorCode = 8,		dispidRefresh = 7L,		dispidWorksMode = 6,		dispidLeftMargin = 5,		dispidExpression = 4,		dispidDraw = 3L,	dispidGetLastError = 2L, dispidDelete = 10l
	};
private:
	//IDrawExpressionPtr	m_iDrawPtr;
	
	CDrawParam			m_drawParam;
	long				m_lLeftMargin;  // отступ от левого края (5)
	//long				m_lWorksMode;	// режим работы 
	void	InitDefault();
	CScrollBar m_ScrollBar;
	BOOL m_SBVisible;
	int m_SBPosition;
	int m_SBMin, m_SBMax;
	int width, old_width;
	BOOL m_bRectsValid;
	RECT* m_pExprRects;
	TCHAR** m_pExprStr;
	int m_nRectCount;
	int m_nCursorPos;
	RECT m_rectCursor;
	CBlinkingCursor m_cursor;
	BOOL m_bFocused;
	ICursorPosition* get_cursor_interface();

protected:
	BSTR GetLastError(void);
	void Draw(void);
	BSTR GetExpression(void);
	void SetExpression(LPCTSTR newVal);
	int GetLastErrorCode(void);
	void Insert(LPCTSTR str);
	void DeleteActiveEntry();
public:
	virtual void Serialize(CArchive& ar);
protected:
	LONG GetLeftMargin(void);
	void SetLeftMargin(LONG newVal);
	LONG GetWorksMode(void);
	void SetWorksMode(LONG newVal);

	// vanek : refresh expression parameters infos - 29.03.2005
	void Refresh(void);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
protected:
	// удаление элемента выражения 
	void delete_entry();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:



	void FireContentChanged(void)
	{
		FireEvent(eventidContentChanged, EVENT_PARAM(VTS_NONE));
	}
public:
	virtual void OnFontChanged();
private:
	void set_expression(CString& str);
	void adjust_scrollbar(void);
};

