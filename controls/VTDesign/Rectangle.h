#if !defined(AFX_RECTANGLE_H__BC127B36_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_RECTANGLE_H__BC127B36_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Rectangle.h : Declaration of the CVTRectangle ActiveX Control class.


#include "jdraw\types.h"
#include "\vt5\common\ax_ctrl_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CVTRectangle : See Rectangle.cpp for implementation.

class CVTRectangle : public COleControl
{
	DECLARE_DYNCREATE(CVTRectangle)	

	DECLARE_AX_DATA_SITE(CVTRectangle)
	
// Constructor
public:
	CVTRectangle();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTRectangle)
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
	~CVTRectangle();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTRectangle)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTRectangle)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTRectangle)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTRectangle)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTRectangle)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTRectangle)
	afx_msg OLE_COLOR GetFillColor();
	afx_msg void SetFillColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentFill();
	afx_msg void SetTransparentFill(BOOL bNewValue);
	afx_msg OLE_COLOR GetLineColor();
	afx_msg void SetLineColor(OLE_COLOR nNewValue);
	afx_msg short GetLineStyle();
	afx_msg void SetLineStyle(short nNewValue);
	afx_msg double GetLineWidth();
	afx_msg void SetLineWidth(double newValue);
	afx_msg BOOL GetRoundedLine();
	afx_msg void SetRoundedLine(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTRectangle)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTRectangle)
	dispidFillColor = 1L,
	dispidTransparentFill = 2L,
	dispidLineColor = 3L,
	dispidLineStyle = 4L,
	dispidLineWidth = 5L,
	dispidRoundedLine = 6L,
	dispidVisible = 7L,
	dispidGetPropertyAXStatus = 8L,
	dispidSetPropertyAXStatus = 9L,
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
	OLE_COLOR	m_FillColor;
	BOOL		m_bTransparentFill;

	OLE_COLOR	m_LineColor;
	DashStyle	m_LineStyle;

	long		m_nLineWidth;

	BOOL		m_bRoundedLine;


	void InitDefaults();

	void DrawContext( HDC hDC, SIZE size );	

	//added by akm 22_11_k5
	BOOL		m_PropertyAXStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECTANGLE_H__BC127B36_026C_11D4_8124_0000E8DF68C3__INCLUDED)
