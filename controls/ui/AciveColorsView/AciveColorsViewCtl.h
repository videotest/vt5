#if !defined(AFX_ACIVECOLORSVIEWCTL_H__0C1F6A99_9DB3_11D3_A536_0000B493A187__INCLUDED_)
#define AFX_ACIVECOLORSVIEWCTL_H__0C1F6A99_9DB3_11D3_A536_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dib.h"

// AciveColorsViewCtl.h : Declaration of the CAciveColorsViewCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl : See AciveColorsViewCtl.cpp for implementation.

class CAciveColorsViewCtrl : public COleControl
{
	DECLARE_DYNCREATE(CAciveColorsViewCtrl)

// Constructor
public:
	CAciveColorsViewCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAciveColorsViewCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CAciveColorsViewCtrl();

	DECLARE_OLECREATE_EX(CAciveColorsViewCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CAciveColorsViewCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CAciveColorsViewCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CAciveColorsViewCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	//{{AFX_MSG(CAciveColorsViewCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CAciveColorsViewCtrl)
	CString m_bMPFileName;
	afx_msg void OnBMPFileNameChanged();
	OLE_COLOR m_leftBtnTransparentColor;
	afx_msg void OnLeftBtnTransparentColorChanged();
	OLE_COLOR m_rightBtnTransparentColor;
	afx_msg void OnRightBtnTransparentColorChanged();
	OLE_COLOR m_leftBtnColor;
	afx_msg void OnLeftBtnColorChanged();
	OLE_COLOR m_rightBtnColor;
	afx_msg void OnRightBtnColorChanged();
	OLE_COLOR m_colorTransparent;
	afx_msg void OnColorTransparentChanged();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CAciveColorsViewCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CAciveColorsViewCtrl)
	dispidBMPFileName = 1L,
	dispidLeftBtnTransparentColor = 2L,
	dispidRightBtnTransparentColor = 3L,
	dispidLeftBtnColor = 4L,
	dispidRightBtnColor = 5L,
	dispidColorTransparent = 6L,
	//}}AFX_DISP_ID
	};

	
	HBITMAP		m_hBitmap;
	CDib		m_DIB;
	CString		m_strPrevFileName;
	CRect		m_rcModifiedArea;
	CSize		m_sizeOriginBitmap;
	BOOL		m_bStoredBMPInArchive;

	void		_MakeBitmaps();
	void		_FreeBMPs();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACIVECOLORSVIEWCTL_H__0C1F6A99_9DB3_11D3_A536_0000B493A187__INCLUDED)
