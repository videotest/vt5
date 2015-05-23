#if !defined(AFX_VTCOLORPICKERCTL_H__E4B0F463_3110_44BD_9224_CAC60DBA919E__INCLUDED_)
#define AFX_VTCOLORPICKERCTL_H__E4B0F463_3110_44BD_9224_CAC60DBA919E__INCLUDED_

	// Added by ClassView
#include "ColorBtnDlg.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTColorPickerCtl.h : Declaration of the CVTColorPickerCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerCtrl : See VTColorPickerCtl.cpp for implementation.

class CVTColorPickerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTColorPickerCtrl)

// Constructor
public:
	CVTColorPickerCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTColorPickerCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTColorPickerCtrl();

	DECLARE_OLECREATE_EX(CVTColorPickerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTColorPickerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTColorPickerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTColorPickerCtrl)		// Type name and misc status

	// Subclassed control support

// Message maps
	//{{AFX_MSG(CVTColorPickerCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTColorPickerCtrl)
	BOOL m_likeCombo;
	afx_msg void OnLikeComboChanged();
	BOOL m_blackAndWhite;
	afx_msg void OnBlackAndWhiteChanged();
	BOOL m_binary;
	afx_msg void OnBinaryChanged();
	afx_msg long GetColor();
	afx_msg void SetColor(long nNewValue);
	afx_msg OLE_COLOR GetBinaryColor(short index);
	afx_msg void SetBinaryColor(short index, long color);
	afx_msg short GetActiveBinaryIndex();
	afx_msg short GetBinaryColorsCount();
	afx_msg void SetActiveBinaryIndex(short nIdx);
	afx_msg BSTR GetBinaryColorRGB( short index );
	afx_msg void SetBinaryColorRGB( short index, LPCTSTR strRGBColor  );
	afx_msg void SetColorRGB( LPCTSTR strRGBColor  );
	afx_msg BSTR GetColorRGB();
	afx_msg void SetPaletteEntry(short index, long color);
	afx_msg void InitializePicker(LPUNKNOWN punkView, LPUNKNOWN punkImage);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CVTColorPickerCtrl)
	void FireOnChange()
		{FireEvent(eventidOnChange,EVENT_PARAM(VTS_NONE));}
	void FireClick()
		{FireEvent(DISPID_CLICK,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	BOOL IsBinary();
	BOOL IsBlackAndWhite();
	CColorBtnDlg m_dlg;
	void Serialize( CArchive& ar );
	COLORREF currentcolor;
	BOOL m_IsFocused;
	UINT m_Edge;
	COLORREF m_Brush;
	BOOL m_IsMouseOver;
	BOOL m_Checked;
	COLORREF m_Palette[256];
	bool m_bPalette;

	enum {
	//{{AFX_DISP_ID(CVTColorPickerCtrl)
	dispidColor = 4L,
	dispidLikeCombo = 1L,
	dispidBlackAndWhite = 2L,
	dispidBinary = 3L,
	dispidGetBinaryColor = 5L,
	dispidSetBinaryColor = 6L,
	dispidGetActiveBinaryIndex = 7L,
	dispidGetBinaryColorsCount = 8L,
	dispidSetActiveBinaryIndex = 9L,
	eventidOnChange = 1L,
	dispidGetBinaryColorRGB = 10L,
	dispidSetBinaryColorRGB = 11L,
	dispidSetColorRGB = 12L,
	dispidGetColorRGB = 13L,
	dispidSetPaletteEntry = 14L,
	dispidInitializePicker = 15L,
	dispidSetVisible = 16L,
	dispidGetVisible = 17L,
	//}}AFX_DISP_ID
	};
private:
	void SelectBrushAndEdge();
	CString _color_to_str( long color );
	long _str_to_color( CString color );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTCOLORPICKERCTL_H__E4B0F463_3110_44BD_9224_CAC60DBA919E__INCLUDED)
