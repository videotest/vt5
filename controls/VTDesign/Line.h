#if !defined(AFX_LINE_H__BC127B31_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_LINE_H__BC127B31_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "jdraw\\types.h"
#include "\vt5\common\ax_ctrl_misc.h"

// Line.h : Declaration of the CVTLine ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTLine : See Line.cpp for implementation.

class CVTLine : public COleControl
{
	DECLARE_DYNCREATE(CVTLine)
	DECLARE_AX_DATA_SITE(CVTLine)

// Constructor
public:
	CVTLine();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTLine)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDrawMetafile(CDC* pDC, const CRect& rcBounds);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTLine();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTLine)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTLine)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTLine)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTLine)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTLine)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTLine)
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentBack();
	afx_msg void SetTransparentBack(BOOL bNewValue);
	afx_msg OLE_COLOR GetLineColor();
	afx_msg void SetLineColor(OLE_COLOR nNewValue);
	afx_msg short GetLineStyle();
	afx_msg void SetLineStyle(short nNewValue);
	afx_msg double GetLineWidth();
	afx_msg void SetLineWidth(double newValue);
	afx_msg BOOL GetRoundedLine();
	afx_msg void SetRoundedLine(BOOL bNewValue);
	afx_msg short GetStartArrow();
	afx_msg void SetStartArrow(short nNewValue);
	afx_msg short GetEndArrow();
	afx_msg void SetEndArrow(short nNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg short GetHorizontalFlip();
	afx_msg void SetHorizontalFlip(short nNewValue);
	afx_msg short GetVerticalFlip();
	afx_msg void SetVerticalFlip(short nNewValue);
	afx_msg short GetOrientation();
	afx_msg void SetOrientation(short nNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTLine)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTLine)
	dispidBackColor = 1L,
	dispidTransparentBack = 2L,
	dispidLineColor = 3L,
	dispidLineStyle = 4L,
	dispidLineWidth = 5L,
	dispidRoundedLine = 6L,
	dispidStartArrow = 7L,
	dispidEndArrow = 8L,
	dispidVisible = 9L,
	dispidHorizontalFlip = 10L,
	dispidVerticalFlip = 11L,
	dispidOrientation = 12L,
	dispidGetPropertyAXStatus = 13L,
	dispidSetPropertyAXStatus = 14L,
	//}}AFX_DISP_ID
	};


	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)				
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  );
		com_call FlipVertical(  );
		com_call SetPageInfo( int nCurPage, int nPageCount );
		com_call NeedUpdate( BOOL* pbUpdate );
	END_INTERFACE_PART(PrintCtrl)


protected:

	OLE_COLOR	m_BackColor;
	BOOL		m_bTransparentBack;

	OLE_COLOR	m_LineColor;
	DashStyle	m_LineStyle;

	long		m_nLineWidth;

	BOOL		m_bRoundedLine;
	
	ArrowType m_StartArrow;
	ArrowType m_EndArrow;

	FlipType	m_ftFlipHorizontal;
	FlipType	m_ftFlipVertical;

	short m_nOrientation;

	void InitDefaults();
	
	void DrawArrow(ArrowType arrowType, POINT p1, POINT p2, int arrowSize, CDC *,int, int , int, CBrush *);
	void DrawContext( HDC hDC, SIZE size );

	//added by akm 22_11_k5
	BOOL		m_PropertyAXStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINE_H__BC127B31_026C_11D4_8124_0000E8DF68C3__INCLUDED)
