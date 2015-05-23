#if !defined(AFX_VTPUSHBUTTON_H__CFA1C4D0_428B_44D3_9262_AA6EA751CB23__INCLUDED_)
#define AFX_VTPUSHBUTTON_H__CFA1C4D0_428B_44D3_9262_AA6EA751CB23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTKeyButton.h : Declaration of the CVTKeyButton ActiveX Control class.





enum ImageAlignHorzEnum{
	iahLeft = 0,
	iahCenter = 1,
	iahRight = 2,
};

enum ImageAlignVertEnum{
	iavTop = 0,
	iavCenter = 1,
	iavBottom = 2,
};



/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton : See VTPushButton.cpp for implementation.

class CVTKeyButton : public COleControl
{
	DECLARE_DYNCREATE(CVTKeyButton)

// Constructor
public:
	CVTKeyButton();
	virtual void Serialize( CArchive& ar );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTKeyButton)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTKeyButton();

	DECLARE_OLECREATE_EX(CVTKeyButton)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTKeyButton)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTKeyButton)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTKeyButton)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTKeyButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTKeyButton)
	afx_msg long GetVirtKey();
	afx_msg void SetVirtKey(long nNewValue);
	afx_msg short GetImageWidth();
	afx_msg void SetImageWidth(short nNewValue);
	afx_msg short GetImageHeight();
	afx_msg void SetImageHeight(short nNewValue);
	afx_msg BSTR GetBmpFileName();
	afx_msg void SetBmpFileName(LPCTSTR strNewValue);
	afx_msg OLE_COLOR  GetImageMask();
	afx_msg void SetImageMask(OLE_COLOR nNewValue);	
	afx_msg void SetImageIndex(short nNewValue);
	afx_msg short GetImageIndex();
	afx_msg short GetImageAlignHorz();
	afx_msg void SetImageAlignHorz(short nNewValue);
	afx_msg short GetImageAlignVert();
	afx_msg void SetImageAlignVert(short nNewValue);
	afx_msg short GetImageDeltaX();
	afx_msg void SetImageDeltaX(short nNewValue);
	afx_msg short GetImageDeltaY();
	afx_msg void SetImageDeltaY(short nNewValue);	
	afx_msg BOOL SetImageList(LPCTSTR lpstrFileName, short cx, short cy, OLE_COLOR clrMask);
	afx_msg BOOL ReloadImageList();	
	afx_msg BSTR GetControlInfo();
	afx_msg void SetControlInfo(LPCTSTR strNewValue);
	afx_msg void SetIconDeltaX(short nNewValue);
	afx_msg short GetIconDeltaX();
	afx_msg void SetIconDeltaY(short nNewValue);
	afx_msg short GetIconDeltaY();
	afx_msg void SetTextDeltaX(short nNewValue);
	afx_msg short GetTextDeltaX();
	afx_msg void SetTextDeltaY(short nNewValue);
	afx_msg short GetTextDeltaY();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTKeyButton)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTKeyButton)
	dispidVirtKey = 1L,
	dispidImageWidth = 2L,
	dispidImageHeight = 3L,
	dispidBmpFileName = 4L,
	dispidImageMask = 5L,
	dispidImageIndex = 6L,
	dispidImageAlignHorz = 7L,
	dispidImageAlignVert = 8L,
	dispidImageDeltaX = 9L,
	dispidImageDeltaY = 10L,
	dispidSetImageList = 11L,
	dispidReloadImageList = 12L,	
	dispidControlInfo = 13L,	
	dispidIconDeltaX = 14L,
	dispidIconDeltaY = 15L,
	dispidTextDeltaX = 16L,
	dispidTextDeltaY = 17L,
	//}}AFX_DISP_ID
	};


protected:

	bool	m_bPressed;
	long	m_nVKey;
	char	m_szText[20];
	HICON	m_hIcon;

	UINT m_nTimerID;

	void InitDefaults();	

	CImageList	m_ImageList;
	short		m_nImageWidth;
	short		m_nImageHeight;
	OLE_COLOR	m_clrImageMask;

	short		m_nImageIndex;

	CString		m_strBmpFileName;

	ImageAlignHorzEnum m_imageAlignHorz;
	ImageAlignVertEnum m_imageAlignVert;
	
	short				m_imageDeltaX;
	short				m_imageDeltaY;

	short				m_iconDeltaX;
	short				m_iconDeltaY;

	short				m_textDeltaX;
	short				m_textDeltaY;



	void CreateDefaultImageList();

	CRect		m_rcAvailable;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTPUSHBUTTON_H__CFA1C4D0_428B_44D3_9262_AA6EA751CB23__INCLUDED)
