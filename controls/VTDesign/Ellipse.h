#if !defined(AFX_ELLIPSE_H__BC127B3B_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_ELLIPSE_H__BC127B3B_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Ellipse.h : Declaration of the CVTEllipse ActiveX Control class.
#include "jdraw\\types.h"
#include "\vt5\common\ax_ctrl_misc.h"
/////////////////////////////////////////////////////////////////////////////
// CVTEllipse : See Ellipse.cpp for implementation.

class CVTEllipse : public COleControl
{
	DECLARE_DYNCREATE(CVTEllipse)
	DECLARE_AX_DATA_SITE(CVTEllipse)

// Constructor
public:
	CVTEllipse();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTEllipse)
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
	~CVTEllipse();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTEllipse)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTEllipse)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTEllipse)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTEllipse)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTEllipse)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTEllipse)
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTransparentBack();
	afx_msg void SetTransparentBack(BOOL bNewValue);
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
	//{{AFX_EVENT(CVTEllipse)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTEllipse)
	dispidBackColor = 1L,
	dispidTransparentBack = 2L,
	dispidFillColor = 3L,
	dispidTransparentFill = 4L,
	dispidLineColor = 5L,
	dispidLineStyle = 6L,
	dispidLineWidth = 7L,
	dispidRoundedLine = 8L,
	dispidVisible = 9L,
	dispidGetPropertyAXStatus = 10L,
	dispidSetPropertyAXStatus = 11L,
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

#endif // !defined(AFX_ELLIPSE_H__BC127B3B_026C_11D4_8124_0000E8DF68C3__INCLUDED)
