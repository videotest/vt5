#if !defined(AFX_3DBAR_H__BC127B4A_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_3DBAR_H__BC127B4A_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 3DBar.h : Declaration of the CVT3DBar ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVT3DBar : See 3DBar.cpp for implementation.

#include "jdraw\\types.h"
#include "\vt5\common\ax_ctrl_misc.h"


class CVT3DBar : public COleControl
{
	DECLARE_DYNCREATE(CVT3DBar)
	DECLARE_AX_DATA_SITE(CVT3DBar)

// Constructor
public:
	CVT3DBar();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVT3DBar)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();	
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVT3DBar();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVT3DBar)    // Class factory and guid
	DECLARE_OLETYPELIB(CVT3DBar)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVT3DBar)     // Property page IDs
	DECLARE_OLECTLTYPE(CVT3DBar)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVT3DBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVT3DBar)
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentBack();
	afx_msg void SetTransparentBack(BOOL bNewValue);
	afx_msg OLE_COLOR GetFontColor();
	afx_msg void SetFontColor(OLE_COLOR nNewValue);
	afx_msg short GetTextPosition();
	afx_msg void SetTextPosition(short nNewValue);
	afx_msg short GetTextAlign();
	afx_msg void SetTextAlign(short nNewValue);
	afx_msg BSTR GetText();
	afx_msg void SetText(LPCTSTR lpszNewValue);
	afx_msg LPFONTDISP GetTextFont();
	afx_msg void SetTextFont(LPFONTDISP newValue);
	afx_msg short GetBorderType();
	afx_msg void SetBorderType(short nNewValue);
	afx_msg double GetLineWidth();
	afx_msg void SetLineWidth(double newValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVT3DBar)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()


	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)				
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  );
		com_call FlipVertical(  );
		com_call SetPageInfo( int nCurPage, int nPageCount );
		com_call NeedUpdate( BOOL* pbUpdate );
	END_INTERFACE_PART(PrintCtrl)


protected:

	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;

	BorderType		m_BorderType;
	PositionType	m_TextPositionType;
	AlignType		m_TextAlignType;
	
	OLE_COLOR		m_BackColor;
	BOOL			m_bTransparentBack;
	
	OLE_COLOR		m_FontColor;
	CString			m_strText;

	long			m_nLineWidth;

	double			m_fFontRatio;

	BOOL			m_PropertyAXStatus; // BT 5109

	void DrawContext( HDC hDC, SIZE sizeVTPixel );
	void InitDefaults();

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVT3DBar)
	dispidBackColor = 1L,
	dispidTransparentBack = 2L,
	dispidFontColor = 3L,
	dispidTextPosition = 4L,
	dispidTextAlign = 5L,
	dispidText = 6L,
	dispidTextFont = 7L,
	dispidBorderType = 8L,
	dispidLineWidth = 9L,
	dispidVisible = 10L,
	dispidGetPropertyAXStatus = 11L,
	dispidSetPropertyAXStatus = 12L,
	//}}AFX_DISP_ID
	};
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DBAR_H__BC127B4A_026C_11D4_8124_0000E8DF68C3__INCLUDED)
