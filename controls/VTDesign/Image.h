#if !defined(AFX_IMAGE_H__BC127B45_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_IMAGE_H__BC127B45_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Image.h : Declaration of the CVTImage ActiveX Control class.

#include "\vt5\common\ax_ctrl_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CVTImage : See Image.cpp for implementation.

class CVTImage : public COleControl
{
	DECLARE_DYNCREATE(CVTImage)
	DECLARE_AX_DATA_SITE(CVTImage)

// Constructor
public:
	CVTImage();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTImage)
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
	~CVTImage();
	virtual void Serialize(CArchive& ar);

	DECLARE_OLECREATE_EX(CVTImage)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTImage)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTImage)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTImage)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTImage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTImage)
	afx_msg OLE_COLOR GetTransparentColor();
	afx_msg void SetTransparentColor(OLE_COLOR nNewValue);
	afx_msg short GetPermanentImage();
	afx_msg void SetPermanentImage(short nNewValue);
	afx_msg BSTR GetFileName();
	afx_msg void SetFileName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);	
	afx_msg OLE_COLOR GetBackgroundColor();
	afx_msg void SetBackgroundColor(OLE_COLOR nNewValue);
	afx_msg BOOL ReloadImage();
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTImage)
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
	//{{AFX_DISP_ID(CVTImage)	
	dispidTransparentColor = 1L,
	dispidPermanentImage = 2L,
	dispidFileName = 3L,	
	dispidVisible = 4L,
	dispidBackgroundColor = 5L,
	dispidReloadImage = 6L,
	dispidGetPropertyAXStatus = 7L,
	dispidSetPropertyAXStatus = 8L,
	//}}AFX_DISP_ID
	};

protected:
	OLE_COLOR		m_TransparentColor;	
	OLE_COLOR		m_BackgroundColor;	

	BOOL			m_bUsePermanentImage;

	CString			m_strFileName;

	CImageList		m_ImageList;
	int				m_nImageIndex;

	BOOL			ReadPictureFromFile();	
	void			CreateDefaultImageList();	

	int m_nImageWidth, m_nImageHeight;

	void DrawContext( HDC hDC, SIZE sizeVTPixel );	
	void InitDefaults();

	//added by akm 22_11_k5
	BOOL		m_PropertyAXStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGE_H__BC127B45_026C_11D4_8124_0000E8DF68C3__INCLUDED)
