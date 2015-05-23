#if !defined(AFX_VTSTATICTEXT_H__6029A462_0B66_4F73_9EA2_2473ACCB5C71__INCLUDED_)
#define AFX_VTSTATICTEXT_H__6029A462_0B66_4F73_9EA2_2473ACCB5C71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dib.h"


// VTStaticText.h : Declaration of the CVTStaticText ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTStaticText : See VTStaticText.cpp for implementation.

class CVTStaticText : public COleControl
{
	DECLARE_DYNCREATE(CVTStaticText)

// Constructor
public:
	CVTStaticText();
	virtual DWORD GetControlFlags();	
	virtual BOOL IsSubclassedControl();

	void InitDefaults();	
	virtual void OnDrawMetafile(CDC* pDC, const CRect& rcBounds);

protected:
	virtual void Serialize(CArchive& ar);


	OLE_COLOR	m_clrText;
	OLE_COLOR	m_clrBackText;
	OLE_COLOR	m_clrTransparent;

	OLE_COLOR	m_clrTransparentChangeToColor;

	CString		m_strCaption;
	CString		m_strFileName;

	//Text align
	BOOL		m_bTextAlignLeft;
	BOOL		m_bTextAlignRight;
	
	BOOL		m_bTextAlignTop;
	BOOL		m_bTextAlignBottom;

	BOOL		m_bTextAlignCenter;
	BOOL		m_bTextAlignMiddle;


	//Picture align
	BOOL		m_bPictAlignLeft;
	BOOL		m_bPictAlignRight;
	
	BOOL		m_bPictAlignTop;
	BOOL		m_bPictAlignBottom;

	//Text to Picture align
	BOOL		m_bText2PictAlignLeft;
	BOOL		m_bText2PictAlignRight;
	
	BOOL		m_bText2PictAlignTop;
	BOOL		m_bText2PictAlignBottom;


	CImageList	m_ImageList;
	int			m_nImageIndex;	
	

	BOOL		ReadPictureFromFile();	
	void		CreateDefaultImageList();
	

	void		DrawImage( CDC* pDC, CRect& rc);
	void		DrawText( CDC* pDC, CRect& rc);
	int			m_nImageWidth;
	int			m_nImageHeight;

	BOOL		m_bInnerRaised;
	BOOL		m_bInnerSunken;
	BOOL		m_bOuterRaised;
	BOOL		m_bOuterSunken;

	BOOL		m_bCentrImage;

	BOOL		m_bTransparencyText;

	BOOL		m_bUseSystemFont;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTStaticText)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnFontChanged();
	virtual void OnBackColorChanged();
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTStaticText();
	

	DECLARE_OLECREATE_EX(CVTStaticText)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTStaticText)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTStaticText)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTStaticText)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTStaticText)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTStaticText)
	afx_msg OLE_COLOR GetTextColor();
	afx_msg void SetTextColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetTextBackColor();
	afx_msg void SetTextBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetTransparentColor();
	afx_msg void SetTransparentColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetTextAlignLeft();
	afx_msg void SetTextAlignLeft(BOOL bNewValue);
	afx_msg BOOL GetTextAlignRight();
	afx_msg void SetTextAlignRight(BOOL bNewValue);
	afx_msg BOOL GetTextAlignTop();
	afx_msg void SetTextAlignTop(BOOL bNewValue);
	afx_msg BOOL GetTextAlignBottom();
	afx_msg void SetTextAlignBottom(BOOL bNewValue);

	afx_msg BOOL GetTextAlignCenter();
	afx_msg void SetTextAlignCenter(BOOL bNewValue);

	afx_msg BOOL GetTextAlignMiddle();
	afx_msg void SetTextAlignMiddle(BOOL bNewValue);

	afx_msg BOOL GetPictAlignLeft();
	afx_msg void SetPictAlignLeft(BOOL bNewValue);
	afx_msg BOOL GetPictAlignRight();
	afx_msg void SetPictAlignRight(BOOL bNewValue);
	afx_msg BOOL GetPictAlignTop();
	afx_msg void SetPictAlignTop(BOOL bNewValue);
	afx_msg BOOL GetPictAlignBottom();
	afx_msg void SetPictAlignBottom(BOOL bNewValue);
	afx_msg BOOL GetText2PictAlignLeft();
	afx_msg void SetText2PictAlignLeft(BOOL bNewValue);
	afx_msg BOOL GetText2PictAlignRight();
	afx_msg void SetText2PictAlignRight(BOOL bNewValue);
	afx_msg BOOL GetText2PictAlignTop();
	afx_msg void SetText2PictAlignTop(BOOL bNewValue);
	afx_msg BOOL GetText2PictAlignBottom();
	afx_msg void SetText2PictAlignBottom(BOOL bNewValue);
	afx_msg BSTR GetText();
	afx_msg void SetText(LPCTSTR lpszNewValue);
	afx_msg BSTR GetPictureFileName();
	afx_msg void SetPictureFileName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetInnerRaised();
	afx_msg void SetInnerRaised(BOOL bNewValue);
	afx_msg BOOL GetInnerSunken();
	afx_msg void SetInnerSunken(BOOL bNewValue);
	afx_msg BOOL GetOuterRaised();
	afx_msg void SetOuterRaised(BOOL bNewValue);
	afx_msg BOOL GetOuterSunken();
	afx_msg void SetOuterSunken(BOOL bNewValue);
	afx_msg BOOL GetCenterImage();
	afx_msg void SetCenterImage(BOOL bNewValue);
	afx_msg BOOL GetTransparencyText();
	afx_msg void SetTransparencyText(BOOL bNewValue);	
	afx_msg void SetCaption(LPCTSTR lpszNewValue);
	afx_msg BOOL ReadPicture(LPCTSTR pstrFileName);
	afx_msg BOOL SetAutoSize();
	afx_msg BOOL ReloadBitmap();
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg void SetTransparentChangeToColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetTransparentChangeToColor();
	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTStaticText)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTStaticText)
	dispidTextColor = 1L,
	dispidTextBackColor = 2L,
	dispidTransparentColor = 3L,
	dispidTextAlignLeft = 4L,
	dispidTextAlignRight = 5L,
	dispidTextAlignTop = 6L,
	dispidTextAlignBottom = 7L,
	dispidPictAlignLeft = 8L,
	dispidPictAlignRight = 9L,
	dispidPictAlignTop = 10L,
	dispidPictAlignBottom = 11L,
	dispidText2PictAlignLeft = 12L,
	dispidText2PictAlignRight = 13L,
	dispidText2PictAlignTop = 14L,
	dispidText2PictAlignBottom = 15L,
	dispidText = 16L,
	dispidPictureFileName = 17L,
	dispidInnerRaised = 18L,
	dispidInnerSunken = 19L,
	dispidOuterRaised = 20L,
	dispidOuterSunken = 21L,
	dispidCenterImage = 22L,
	dispidTransparencyText = 23L,	
	dispidReadPicture = 24L,
	dispidSetAutoSize = 25L,
	dispidReloadBitmap = 26L,
	dispidDisabled = 27L,
	dispidVisible = 28L,
	dispidSetFocus = 29L,
	dispidTransparentChangeToColor = 30L,
	dispidUseSystemFont = 31L,
	dispidTextAlignCenter = 32L,
	dispidTextAlignMiddle = 33L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSTATICTEXT_H__6029A462_0B66_4F73_9EA2_2473ACCB5C71__INCLUDED)
