#if !defined(AFX_COLORRANGECTRLCTL_H__DE2D8CF7_6D11_4919_926C_CB27814C3C12__INCLUDED_)
#define AFX_COLORRANGECTRLCTL_H__DE2D8CF7_6D11_4919_926C_CB27814C3C12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ColorRangeCtrlCtl.h : Declaration of the CColorRangeCtrl ActiveX Control class.

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl : See ColorRangeCtrlCtl.cpp for implementation.

class range_info
{
public:
	range_info()
	{
		m_flen	= 0.;
		m_color	= RGB(0,0,0);
			
	}
	
	double		m_flen;
	OLE_COLOR	m_color;
};

#define NORECT	CRect( 0, 0, 0, 0 )

class CColorRangeCtrl : public COleControl
{
	DECLARE_DYNCREATE(CColorRangeCtrl)

// Constructor
public:
	CColorRangeCtrl();	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorRangeCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CColorRangeCtrl();

	DECLARE_OLECREATE_EX(CColorRangeCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CColorRangeCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CColorRangeCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CColorRangeCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CColorRangeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CColorRangeCtrl)
	afx_msg double GetMinValue();
	afx_msg void SetMinValue(double newValue);
	afx_msg double GetMaxValue();
	afx_msg void SetMaxValue(double newValue);
	afx_msg long GetRangesCount();
	afx_msg void SetRangesCount(long nNewValue);
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetRangeColor(long nidx);
	afx_msg void SetRangeColor(long nidx, OLE_COLOR color);
	afx_msg double GetRangeLen(long nidx);
	afx_msg void SetRangeLen(long nidx, double flen);
	afx_msg void InvalidateCtrl();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	void			InitDefaults();
	virtual void	Serialize( CArchive& ar );

	double			m_fmin;
	double			m_fmax;
	OLE_COLOR		m_color_back;
	CArray<range_info*, range_info*>	m_ar_range;

//helpers	
	range_info*		get_range( int nidx );
	void			deinit_array();
	CRect			get_range_rect( int nidx );
	bool			get_drag_range( CPoint pt_cur, int* pleft_range, int* pright_range );
	double			get_unit_in_pixel( );
	CRect			get_image_rect( );

//ui
	HCURSOR			m_hcursor_normal;
	HCURSOR			m_hcursor_size;

//resize range
	bool			m_bdraging;
	int				m_ndrag_range_left;
	int				m_ndrag_range_right;
	CPoint			m_pt_drag_first;

	HICON			m_hicon_slider;

// Event maps
	//{{AFX_EVENT(CColorRangeCtrl)
	void FireRangeLenChange()
		{FireEvent(eventidRangeLenChange,EVENT_PARAM(VTS_NONE));}
	void FireRangeColorChange()
		{FireEvent(eventidRangeColorChange,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CColorRangeCtrl)
	dispidMinValue = 1L,
	dispidMaxValue = 2L,
	dispidRangesCount = 3L,
	dispidBackColor = 4L,
	dispidGetRangeColor = 5L,
	dispidSetRangeColor = 6L,
	dispidGetRangeLen = 7L,
	dispidSetRangeLen = 8L,
	dispidInvalidateCtrl = 9L,
	eventidRangeLenChange = 1L,
	eventidRangeColorChange = 2L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORRANGECTRLCTL_H__DE2D8CF7_6D11_4919_926C_CB27814C3C12__INCLUDED)
