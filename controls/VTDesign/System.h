#if !defined(AFX_SYSTEM_H__BC127B4F_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_SYSTEM_H__BC127B4F_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// System.h : Declaration of the CVTSystem ActiveX Control class.
#include "jdraw\\types.h"
#include "\vt5\common\ax_ctrl_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CVTSystem : See System.cpp for implementation.

class CVTSystem : public COleControl
{
	DECLARE_DYNCREATE(CVTSystem)
	DECLARE_AX_DATA_SITE(CVTSystem)

// Constructor
public:
	CVTSystem();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTSystem)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTSystem();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTSystem)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTSystem)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTSystem)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTSystem)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTSystem)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTSystem)
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentBack();
	afx_msg void SetTransparentBack(BOOL bNewValue);
	afx_msg OLE_COLOR GetFontColor();
	afx_msg void SetFontColor(OLE_COLOR nNewValue);
	afx_msg short GetDataType();
	afx_msg void SetDataType(short nNewValue);
	afx_msg LPFONTDISP GetTextFont();
	afx_msg void SetTextFont(LPFONTDISP newValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg short GetHorzAlign();
	afx_msg void SetHorzAlign(short nNewValue);
	afx_msg short GetVertAlign();
	afx_msg void SetVertAlign(short nNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTSystem)
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

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTSystem)
	dispidBackColor = 1L,
	dispidTransparentBack = 2L,
	dispidFontColor = 3L,
	dispidDataType = 4L,
	dispidTextFont = 5L,
	dispidVisible = 6L,
	dispidHorzAlign =7L,
	dispidVertAlign =8L,
	dispidGetPropertyAXStatus = 9L,
	dispidSetPropertyAXStatus = 10L,
	//}}AFX_DISP_ID
	};
protected:
	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;
	
	SystemType	m_DataType;	
	
	OLE_COLOR		m_BackColor;
	BOOL			m_bTransparentBack;
	
	OLE_COLOR		m_FontColor;

	AlignHorizontal m_horzAlign;
	AlignVertical	m_vertAlign;

	void DrawContext( HDC hDC, SIZE sizeVTPixel );
	void InitDefaults();

	int m_nCurPage;
	int m_nPageCount;

	double			m_fFontRatio;

	//added by akm 22_11_k5
	BOOL		m_PropertyAXStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEM_H__BC127B4F_026C_11D4_8124_0000E8DF68C3__INCLUDED)
