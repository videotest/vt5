// BCGColorDialog.cpp : implementation file
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2000 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//

#include "stdafx.h"

#ifndef BCG_NO_COLOR

//#include "bcgcontrolbar.h"
#include "bcglocalres.h"
#include "BCGColorDialog.h"
#include "PropertySheetCtrl.h"
#include "VTColorPickerCtl.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGColorDialog dialog

CBCGColorDialog::CBCGColorDialog (COLORREF clrInit, DWORD /*dwFlags - reserved */, 
					CWnd* pParentWnd, HPALETTE hPal)
	: CDialog(CBCGColorDialog::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CBCGColorDialog)
	//}}AFX_DATA_INIT

	m_pPropSheet		= 0;

	m_pColourSheetOne	= 0;
	m_pColourSheetTwo	= 0;
	m_pColourSheetThree	= 0;

	m_CurrentColor = m_NewColor = clrInit;
	
	m_bIsMyPalette = TRUE;
	m_pPalette = NULL;
	
	if (hPal != NULL)
	{
		m_pPalette = CPalette::FromHandle (hPal);
		m_bIsMyPalette = FALSE;
	}
	m_bPalette = false;
}

CBCGColorDialog::~CBCGColorDialog ()
{
	Free();
}

void CBCGColorDialog::Free()
{
	if (m_pColourSheetOne != NULL)
		delete m_pColourSheetOne;	m_pColourSheetOne = 0;

	if (m_pColourSheetTwo != NULL)
		delete m_pColourSheetTwo;	m_pColourSheetTwo = 0;

	if (m_pColourSheetThree != NULL)
		delete m_pColourSheetThree;	m_pColourSheetThree = 0;

	if( m_pPropSheet )
		delete m_pPropSheet;	m_pPropSheet = 0;
}

void CBCGColorDialog::SetDrawPalette(COLORREF *pColor)
{
	m_bPalette = pColor != 0;
	if (pColor)
		memcpy(&m_Palette, pColor, sizeof(m_Palette));
}

void CBCGColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CBCGColorDialog)
	DDX_Control(pDX, IDC_BCGBARRES_COLOURPLACEHOLDER, m_wndColors);
	DDX_Control(pDX, IDC_BCGBARRES_STATICPLACEHOLDER, m_wndStaticPlaceHolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBCGColorDialog, CDialog)
	//{{AFX_MSG_MAP(CBCGColorDialog)
	ON_WM_DESTROY()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGColorDialog message handlers

BOOL CBCGColorDialog::OnInitDialog()
{
	CBCGLocalResource locaRes;

	CDialog::OnInitDialog();
	
	if (globalData.m_nBitsPerPixel < 8)	// 16 colors, call standard dialog
	{
		CColorDialog dlg (m_CurrentColor, CC_FULLOPEN | CC_ANYCOLOR);
		int nResult = dlg.DoModal ();
		m_NewColor = dlg.GetColor ();
		EndDialog (nResult);

		return TRUE;
	}

	if (m_pPalette == NULL)
	{
		m_pPalette = new CPalette ();
		RebuildPalette ();
	}

	m_wndColors.SetType (CBCGColorPickerCtrl::CURRENT);
	m_wndColors.SetPalette (m_pPalette);

	m_wndColors.SetOriginalColor (m_CurrentColor);
	m_wndColors.SetRGB (m_NewColor);

	if (m_bPalette)
		m_wndColors.SetDrawPalette(m_Palette);

	// Create property sheet.

	Free();
	m_pPropSheet = new CPropertySheet(_T(""), this);
	ASSERT(m_pPropSheet);

	m_pColourSheetOne = new CColorPage1;
	m_pColourSheetTwo = new CColorPage2;
	m_pColourSheetThree = new CColorPage3;

	// Set parent dialog.
	m_pColourSheetOne->m_pDialog = this;
	m_pColourSheetTwo->m_pDialog = this;
	m_pColourSheetThree->m_pDialog = this;

	if(m_bColor)
	{
		m_pPropSheet->AddPage (m_pColourSheetOne);
		m_pPropSheet->AddPage (m_pColourSheetTwo);
	}
	else
		m_pPropSheet->AddPage (m_pColourSheetThree);
	
	// Retrieve the location of the window
	CRect rectListWnd;
	m_wndStaticPlaceHolder.GetWindowRect(rectListWnd);
	ScreenToClient(rectListWnd);
	
	if (!m_pPropSheet->Create(this,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0))
	{
		TRACE0 ("CBCGColorDialog::OnInitDialog(): Can't create the property sheet.....\n");
	}

	m_pPropSheet->SetWindowPos(NULL, rectListWnd.left, rectListWnd.top, rectListWnd.Width(),
				rectListWnd.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

	if(m_bColor)
	{
		SetPageOne (GetRValue (m_CurrentColor), 
					GetGValue (m_CurrentColor),
					GetBValue (m_CurrentColor));

		SetPageTwo (GetRValue (m_CurrentColor),
					GetGValue (m_CurrentColor),
					GetBValue (m_CurrentColor));
	}
	else
	{
		SetPageThree (GetRValue (m_CurrentColor),
					  GetGValue (m_CurrentColor),
					  GetBValue (m_CurrentColor));
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBCGColorDialog::SetCurrentColor(COLORREF rgb)
{
	if(m_bColor)
	{
		m_CurrentColor = rgb;
	}
	else
	{
		m_CurrentColor = RGB(GetGValue(rgb),GetGValue(rgb),GetGValue(rgb));
	}
}

void CBCGColorDialog::SetPageOne(BYTE R, BYTE G, BYTE B)
{
	m_pColourSheetOne->m_hexpicker.SelectCellHexagon(R, G, B);
	m_pColourSheetOne->m_hexpicker_greyscale.SelectCellHexagon(R, G, B);
}

void CBCGColorDialog::SetPageTwo(BYTE R, BYTE G, BYTE B)
{
	m_pColourSheetTwo->Setup (R, G, B);
}

INT_PTR CBCGColorDialog::DoModal() 
{
	CBCGLocalResource locaRes;
	return CDialog::DoModal();
}

void CBCGColorDialog::OnDestroy() 
{
	if (m_bIsMyPalette && m_pPalette != NULL)
	{
		m_pPalette->DeleteObject ();
		delete m_pPalette;
		m_pPalette = NULL;
	}

	CDialog::OnDestroy();
}

void CBCGColorDialog::SetNewColor (COLORREF rgb)
{
	if (globalData.m_nBitsPerPixel == 8) // 256 colors
	{
		ASSERT (m_pPalette != NULL);

		UINT uiPalIndex = m_pPalette->GetNearestPaletteIndex (rgb);
		m_NewColor = PALETTEINDEX (uiPalIndex);
	}
	else
	{
		m_NewColor = rgb;
	}

	m_wndColors.SetRGB (m_NewColor);

	m_wndColors.Invalidate ();
	m_wndColors.UpdateWindow ();
}

void CBCGColorDialog::OnSysColorChange() 
{
	CDialog::OnSysColorChange();

	globalData.UpdateSysColors ();
	
	if (m_bIsMyPalette)
	{
		if (globalData.m_nBitsPerPixel < 8)	// 16 colors, call standard dialog
		{
			ShowWindow (SW_HIDE);

			CColorDialog dlg (m_CurrentColor, CC_FULLOPEN | CC_ANYCOLOR);
			int nResult = dlg.DoModal ();
			m_NewColor = dlg.GetColor ();
			EndDialog (nResult);
		}
		else
		{
			::DeleteObject (m_pPalette->Detach ());
			RebuildPalette ();

			Invalidate ();
			UpdateWindow ();
		}
	}
}

void CBCGColorDialog::RebuildPalette ()
{
	ASSERT (m_pPalette->GetSafeHandle () == NULL);

	// Create palette:
	CClientDC dc (this);

	int nColors = 256;	// Use 256 first entries
	UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
	LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

	pLP->palVersion = 0x300;
	pLP->palNumEntries = (USHORT) nColors;

	::GetSystemPaletteEntries (dc.GetSafeHdc (), 0, nColors, pLP->palPalEntry);

	m_pPalette->CreatePalette (pLP);

	delete[] pLP;
}

#endif // BCG_NO_COLOR

void CBCGColorDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	EndDialog(IDCANCEL);
//	CDialog::OnCancel();
}

void CBCGColorDialog::OnOK() 
{
	// TODO: Add extra validation here
	EndDialog(IDOK);
//	CDialog::OnOK();
}

void CBCGColorDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	//CDialog::OnActivate(nState, pWndOther, bMinimized);
	GetParent()->ShowWindow(SW_HIDE);

	return ;
	
	// TODO: Add your message handler code here
	
}

void CBCGColorDialog::SetPageThree(BYTE R, BYTE G, BYTE B)
{
	m_pColourSheetThree->m_hexgreyscale.SelectCellHexagon(G, G, G);
	
	double luminance;
	double saturation;
	double hue;
	RGBtoHSL((COLORREF)RGB(G, G, G), &hue, &saturation, &luminance);

	m_pColourSheetThree->m_luminance.SetHLS(hue, luminance, saturation);

	m_pColourSheetThree->m_L = G;
	m_pColourSheetThree->m_R = G;
	m_pColourSheetThree->m_G = G;
	m_pColourSheetThree->m_B = G;
	m_pColourSheetThree->UpdateData(FALSE);

}
