#if !defined(AFX_TEXT_H__BC127B40_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_TEXT_H__BC127B40_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Text.h : Declaration of the CVTText ActiveX Control class.

#include "\vt5\common\ax_ctrl_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CVTText : See Text.cpp for implementation.

#include "jdraw\\types.h"


class CVTText : public COleControl
{
	DECLARE_DYNCREATE(CVTText)
	DECLARE_AX_DATA_SITE(CVTText)

// Constructor
public:
	CVTText();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTText)
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
	~CVTText();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTText)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTText)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTText)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTText)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTText)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTText)
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentBack();
	afx_msg void SetTransparentBack(BOOL bNewValue);
	afx_msg OLE_COLOR GetFontColor();
	afx_msg void SetFontColor(OLE_COLOR nNewValue);
	afx_msg short GetTextPosition();
	afx_msg void SetTextPosition(short nNewValue);
	afx_msg BSTR GetText();
	afx_msg void SetText(LPCTSTR lpszNewValue);
	afx_msg LPFONTDISP GetTextFont();
	afx_msg void SetTextFont(LPFONTDISP newValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg short GetDegree();
	afx_msg void SetDegree(short nNewValue);
	afx_msg short GetHorzAlign();
	afx_msg void SetHorzAlign(short nNewValue);
	afx_msg short GetVertAlign();
	afx_msg void SetVertAlign(short nNewValue);
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BSTR GetKey();
	afx_msg void SetKey(LPCTSTR lpszNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTText)
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


	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)


	IUnknownPtr	m_ptrSite;
	IUnknownPtr	m_ptrApp;


	CString				m_strName;
	CString				m_strKeyValue;


// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTText)
	dispidBackColor = 1L,
	dispidTransparentBack = 2L,
	dispidFontColor = 3L,
	dispidTextPosition = 4L,
	dispidText = 5L,
	dispidTextFont = 6L,
	dispidVisible = 7L,
	dispidDegree =8L,
	dispidHorzAlign =9L,
	dispidVertAlign =10L,
	dispidDisabled = 11L,
	dispidKey = 12L,
	dispidGetPropertyAXStatus = 13L,
	dispidSetPropertyAXStatus = 14L,
	//}}AFX_DISP_ID
	};
protected:
	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;
	
	PositionType	m_TextPositionType;	
	
	OLE_COLOR		m_BackColor;
	BOOL			m_bTransparentBack;
	
	OLE_COLOR		m_FontColor;
	CString			m_strText;

	double			m_fFontRatio;

	short			m_nDegree;
	AlignHorizontal m_horzAlign;
	AlignVertical	m_vertAlign;


	void DrawContext( HDC hDC, SIZE sizeVTPixel );
	void DrawContextGDI( HDC hDC, SIZE sizeVTPixel );
	void DrawContextGDIPlus( HDC hDC, SIZE sizeVTPixel );
	void InitDefaults();

	//added by akm 22_11_k5
	BOOL		m_PropertyAXStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXT_H__BC127B40_026C_11D4_8124_0000E8DF68C3__INCLUDED)
