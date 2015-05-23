// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// Auxiliary System/Screen metrics

struct GLOBAL_DATA
{
	// solid brushes with convenient gray colors and system colors
	HBRUSH hbrBtnHilite, hbrBtnShadow;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;
	COLORREF clrBtnDkShadow, clrBtnLight;
	COLORREF clrGrayedText;
	COLORREF clrHilite;
	COLORREF clrTextHilite;
	COLORREF clrMenu;
	COLORREF clrMenuText;

	HCURSOR	m_hcurStretch;

	// Toolbar and menu fonts:
	CFont	fontRegular;
	CFont	fontBold;
	CFont	fontVert;

	BOOL	bIsWindowsNT;

// Implementation
	GLOBAL_DATA();
	~GLOBAL_DATA();

	void UpdateSysColors();
public:
	CBrush brBtnFace;
	int m_nBitsPerPixel;
};

extern GLOBAL_DATA globalData;

void AFXAPI AfxDeleteObject(HGDIOBJ* pObject);

/////////////////////////////////////////////////////////////////////////////
