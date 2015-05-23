#if !defined(AFX_CURVEBOX_H__2649CBD6_E387_11D3_9A3D_000000000000__INCLUDED_)
#define AFX_CURVEBOX_H__2649CBD6_E387_11D3_9A3D_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// curvebox.h : header file
//

#define WM_CURVECHANGE (WM_USER + 222)

const int g_nStateSize = 2124;

/////////////////////////////////////////////////////////////////////////////
// CCurveBox window

class CCurveBox : public CStatic
{
// Construction
	// Composite histogram data
	BYTE m_DataGray[256];
	BYTE m_DataBlue[256];
	BYTE m_DataGreen[256];
	BYTE m_DataRed[256];
	// Data for 2-point histogram
	BYTE m_xHisto2Low[4];
	BYTE m_xHisto2Up[4];
	// Data for 4-point histogram
	BYTE m_yData4[5][4];
	// Data for 8-point histogram
	BYTE m_yData8[9][4];
	// Data for manual histogram
	BYTE m_DataMan[256][4];
	bool m_bColorDoc; // Is current document color
	bool m_bCurveEdit; // Enable editing
	int  m_nCurveNum; // Current edited curve
	int  m_nMode; // Modes: 0..3
	int  m_nDragPoint; // Current dragged point
	int  m_xPrev; // For mode 3
	int  m_yPrev;
	COLORREF m_clrBack;
	bool m_bShowGrid;

	void DrawCurve(CDC *pDC, CRect &rcClient, BYTE *pData, COLORREF clr, int nCurveNum);
	void RecalcCompositeCurve();
public:
	CCurveBox();

// Attributes
public:

// Operations
public:
	void SetBoxType(bool bCurveEdit);
	void SetCurveNum(int nCurveNum);
	void SetColorDoc(bool bColorDoc);
	void SetCurveBCG(BYTE *pGray, BYTE *pBlue, BYTE *pGreen, BYTE *pRed);
	void GetCurveData(int nCurveNum, BYTE *pCurveData);
	void SetMode(int nMode, bool bInvalidate = true);
	void SetBackColor(COLORREF clr);
	void ShowGrid(bool bShowGrid, bool bInvalidate = true);

	void GetState(byte* pState);
	void SetState(byte* pState);
	void InitData();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCurveBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCurveBox)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEBOX_H__2649CBD6_E387_11D3_9A3D_000000000000__INCLUDED_)
