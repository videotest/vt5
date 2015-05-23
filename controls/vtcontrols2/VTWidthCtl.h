#if !defined(AFX_VTWIDTHCTL_H__EE96F0BE_0363_4EE2_BD7B_55CCAEBA6C0F__INCLUDED_)
#define AFX_VTWIDTHCTL_H__EE96F0BE_0363_4EE2_BD7B_55CCAEBA6C0F__INCLUDED_

#include "VTButton.h"	// Added by ClassView
#include "WidthDlg.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTWidthCtl.h : Declaration of the CVTWidthCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTWidthCtrl : See VTWidthCtl.cpp for implementation.

class CVTWidthCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTWidthCtrl)

// Constructor
public:
	CVTWidthCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTWidthCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTWidthCtrl();

	DECLARE_OLECREATE_EX(CVTWidthCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTWidthCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTWidthCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTWidthCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTWidthCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonWidthClicked();
	afx_msg void OnButtonUpClicked();
	afx_msg void OnButtonDownClicked();
	afx_msg void OnButtonSliderClicked();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
   
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTWidthCtrl)
	BOOL m_visible;
	afx_msg void OnVisibleChanged();
	OLE_COLOR m_colorText;
	afx_msg void OnColorTextChanged();
	OLE_COLOR m_colorTextHighlight;
	afx_msg void OnColorTextHighlightChanged();
	OLE_COLOR m_colorSlider;
	afx_msg void OnColorSliderChanged();
	OLE_COLOR m_colorSliderArrows;
	afx_msg void OnColorSliderArrowsChanged();
	OLE_COLOR m_colorTextScale;
	afx_msg void OnColorTextScaleChanged();
	OLE_COLOR m_colorSliderArrowsHighlight;
	afx_msg void OnColorSliderArrowsHighlightChanged();
	afx_msg long GetWidth();
	afx_msg void SetWidth(long nNewValue);
	afx_msg BOOL GetChecked();
	afx_msg void SetChecked(BOOL bNewValue);
	afx_msg long GetMin();
	afx_msg void SetMin(long nNewValue);
	afx_msg long GetMax();
	afx_msg void SetMax(long nNewValue);
	afx_msg BOOL GetOrientation();
	afx_msg void SetOrientation(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CVTWidthCtrl)
	void FireOnChange()
		{FireEvent(eventidOnChange,EVENT_PARAM(VTS_NONE));}
	void FireClick()
		{FireEvent(DISPID_CLICK,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	COLORREF GetColorSliderArrowsHighlight();
	void Check(bool bNewVal);
	COLORREF GetColorTextScale();
	COLORREF GetColorSliderArrows();
	COLORREF GetColorSlider();
	COLORREF GetColorTextHighlight();
	COLORREF GetColorText();

	CFont m_Font;
	BOOL m_Orientation;
	UINT m_Edge;
	COLORREF m_Brush;
	enum {
	//{{AFX_DISP_ID(CVTWidthCtrl)
	dispidWidth = 8L,
	dispidChecked = 9L,
	dispidMin = 10L,
	dispidMax = 11L,
	dispidOrientation = 12L,
	dispidVisible = 1L,
	dispidColorText = 2L,
	dispidColorTextHighlight = 3L,
	dispidColorSlider = 4L,
	dispidColorSliderArrows = 5L,
	dispidColorTextScale = 6L,
	dispidColorSliderArrowsHighlight = 7L,
	eventidOnChange = 1L,
	//}}AFX_DISP_ID
	};
	long m_Max;
	long m_Min;
	long m_Width;

private:
	BOOL m_IsFocused;
	BOOL m_IsSliderMoved;
	void InstallSlider();
	CVTButton m_ButtonSlider;
	void SelectBrushAndEdge();
	void SetMouseOver(BOOL bNewVal);
	BOOL m_IsCreated;
	CVTButton m_ButtonDown;
	CVTButton m_ButtonUp;
	void DrawLineWidth(CDC* pdc, CRect rcBounds);

	CWidthDlg m_WidthDlg;
	CVTButton m_ButtonWidth;
	BOOL m_Checked;
	BOOL m_IsMouseOver;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTWIDTHCTL_H__EE96F0BE_0363_4EE2_BD7B_55CCAEBA6C0F__INCLUDED)
