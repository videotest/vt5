//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// ImageEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>
#include "bcgbarres.h"
#include "globals.h"
#include "ImageEditDlg.h"
#include "bcglocalres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGImageEditDlg dialog


#pragma warning (disable : 4355)

CBCGImageEditDlg::CBCGImageEditDlg(CBitmap* pBitmap, CWnd* pParent /*=NULL*/)
	: CDialog(CBCGImageEditDlg::IDD, pParent),
	m_pBitmap (pBitmap),
	m_pWndPaletteBarDummyFrame (NULL),
	m_wndLargeDrawArea (this)
{
	ASSERT_VALID (m_pBitmap);

	BITMAP bmp;
	m_pBitmap->GetBitmap (&bmp);

	m_sizeImage = CSize (bmp.bmWidth, bmp.bmHeight);

	//{{AFX_DATA_INIT(CBCGImageEditDlg)
	//}}AFX_DATA_INIT
}

#pragma warning (default : 4355)

void CBCGImageEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCGImageEditDlg)
	DDX_Control(pDX, IDC_BCGBARRES_PALETTE, m_wndPaletteBarLocation);
	DDX_Control(pDX, IDC_BCGBARRES_PREVIEW_AREA, m_wndPreview);
	DDX_Control(pDX, IDC_BCGBARRES_DRAW_AREA, m_wndLargeDrawArea);
	DDX_Control(pDX, IDC_BCGBARRES_COLORS, m_wndColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCGImageEditDlg, CDialog)
	//{{AFX_MSG_MAP(CBCGImageEditDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BCGBARRES_COLORS, OnColors)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGImageEditDlg message handlers

BOOL CBCGImageEditDlg::OnInitDialog() 
{
	const int iBorderWidth = 10;
	const int iBorderHeight = 5;
	const int iPreviewBorderSize = 4;

	CDialog::OnInitDialog();

	m_wndLargeDrawArea.SetBitmap (m_pBitmap);

	//------------------------
	// Create the palette bar:
	//------------------------	
	{
		CBCGLocalResource locaRes;

		CRect rectPaletteBar;
		m_wndPaletteBarLocation.GetClientRect (&rectPaletteBar);
		m_wndPaletteBarLocation.MapWindowPoints (this, &rectPaletteBar);
		rectPaletteBar.DeflateRect (2, 2);

		m_wndPaletteBar.EnableLargeIcons (FALSE);
		m_wndPaletteBar.Create (this);
		m_wndPaletteBar.LoadToolBar (IDR_BCGRES_PALETTE, 0, 0, TRUE /* Locked bar */);

		m_wndPaletteBar.SetBarStyleEx(m_wndPaletteBar.GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY);
			
		m_wndPaletteBar.SetBarStyleEx (
			m_wndPaletteBar.GetBarStyle () & 
				~(CBRS_GRIPPER | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

		m_wndPaletteBar.SetBorders (iBorderWidth, iBorderHeight, 
									iBorderWidth, iBorderHeight);

		const int nButtonWidth = m_wndPaletteBar.GetButtonSize ().cx;
//		m_wndPaletteBar.WrapToolBar (rectPaletteBar.Width () - iBorderWidth);
		m_wndPaletteBar.WrapToolBar (nButtonWidth * 3);
		m_wndPaletteBar.MoveWindow (rectPaletteBar);

		m_wndPaletteBar.SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		m_pWndPaletteBarDummyFrame = new CPaletteCommandFrameWnd;
		m_pWndPaletteBarDummyFrame->Create (0, 
			CFrameWnd::rectDefault, this);

		m_wndPaletteBar.SetOwner (m_pWndPaletteBarDummyFrame);
	}
	
	//---------------------
	// Define preview area:
	//---------------------
	m_wndPreview.GetClientRect (&m_rectPreviewImage);
	m_wndPreview.MapWindowPoints (this, &m_rectPreviewImage);

	m_rectPreviewImage.left = (m_rectPreviewImage.left + m_rectPreviewImage.right - m_sizeImage.cx) / 2;
	m_rectPreviewImage.right = m_rectPreviewImage.left + m_sizeImage.cx;

	m_rectPreviewImage.top = (m_rectPreviewImage.top + m_rectPreviewImage.bottom - m_sizeImage.cy) / 2;
	m_rectPreviewImage.bottom = m_rectPreviewImage.top + m_sizeImage.cy;

	m_rectPreviewFrame = m_rectPreviewImage;
	m_rectPreviewFrame.InflateRect (iPreviewBorderSize, iPreviewBorderSize);

	m_wndLargeDrawArea.m_rectParentPreviewArea = m_rectPreviewImage;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//********************************************************************************
void CBCGImageEditDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.Draw3dRect (&m_rectPreviewFrame,
					globalData.clrBtnHilite,
					globalData.clrBtnShadow);

	CBitmap* pbmOld = NULL;
	CDC dcMem;
		
	dcMem.CreateCompatibleDC (&dc);
	pbmOld = dcMem.SelectObject (m_pBitmap);

	dc.BitBlt (m_rectPreviewImage.left, m_rectPreviewImage.top,
				m_sizeImage.cx, m_sizeImage.cy, &dcMem,
				0, 0, SRCCOPY);

	dcMem.SelectObject(pbmOld);
	dcMem.DeleteDC();
}
//********************************************************************************
void CBCGImageEditDlg::Clear() 
{
	CWindowDC	dc (this);
	CDC 		memDC;	

	memDC.CreateCompatibleDC (&dc);
	
	CBitmap* pOldBitmap = memDC.SelectObject (m_pBitmap);

	CRect rect (0, 0, m_sizeImage.cx, m_sizeImage.cy);
	memDC.FillRect (&rect, &globalData.brBtnFace);

	memDC.SelectObject (pOldBitmap);

	InvalidateRect (m_rectPreviewImage);
	m_wndLargeDrawArea.Invalidate ();
}
//********************************************************************************
void CBCGImageEditDlg::OnColors() 
{
	m_wndLargeDrawArea.SetColor (m_wndColor.GetColor ());
}
//********************************************************************************
void CBCGImageEditDlg::Copy() 
{
	CBCGLocalResource locaRes;

	if (m_pBitmap == NULL)
	{
		return;
	}

	try
	{
		CWindowDC dc (this);

		//----------------------
		// Create a bitmap copy:
		//----------------------
		CDC memDCDest;
		memDCDest.CreateCompatibleDC (NULL);
		
		CDC memDCSrc;
		memDCSrc.CreateCompatibleDC (NULL);
		
		CBitmap bitmapCopy;
		if (!bitmapCopy.CreateCompatibleBitmap (&dc, m_sizeImage.cx, m_sizeImage.cy))
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			return;
		}

		CBitmap* pOldBitmapDest = memDCDest.SelectObject (&bitmapCopy);
		CBitmap* pOldBitmapSrc = memDCSrc.SelectObject (m_pBitmap);

		memDCDest.BitBlt (0, 0, m_sizeImage.cx, m_sizeImage.cy,
						&memDCSrc, 0, 0, SRCCOPY);

		memDCDest.SelectObject (pOldBitmapDest);
		memDCSrc.SelectObject (pOldBitmapSrc);

		if (!OpenClipboard ())
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			return;
		}

		if (!::EmptyClipboard ())
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			::CloseClipboard ();
			return;
		}


		HANDLE hclipData = ::SetClipboardData (CF_BITMAP, bitmapCopy.Detach ());
		if (hclipData == NULL)
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			TRACE (_T("CBCGImageEditDlg::Copy() error. Error code = %x\n"), GetLastError ());
		}

		::CloseClipboard ();
	}
	catch (...)
	{
		CBCGLocalResource locaRes;
		AfxMessageBox (IDP_BCGBARRES_INTERLAL_ERROR);
	}
}
//********************************************************************************
void CBCGImageEditDlg::Paste() 
{
	CBCGLocalResource locaRes;

	COleDataObject data;
	if (!data.AttachClipboard ())
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		return;
	}

	if (!data.IsDataAvailable (CF_BITMAP))
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		return;
	}

	tagSTGMEDIUM dataMedium;
	if (!data.GetData (CF_BITMAP, &dataMedium))
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		return;
	}

	CBitmap* pBmpClip = CBitmap::FromHandle (dataMedium.hBitmap);
	if (pBmpClip == NULL)
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		return;
	}

	BITMAP bmp;
	pBmpClip->GetBitmap (&bmp);

	CDC memDCDst;
	CDC memDCSrc;

	memDCSrc.CreateCompatibleDC (NULL);
	memDCDst.CreateCompatibleDC (NULL);
	
	CBitmap* pSrcOldBitmap = memDCSrc.SelectObject (pBmpClip);
	if (pSrcOldBitmap == NULL)
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		return;
	}

	CBitmap* pDstOldBitmap = memDCDst.SelectObject (m_pBitmap);
	if (pDstOldBitmap == NULL)
	{
		AfxMessageBox (IDP_BCGBARRES_CANT_PASTE_BITMAP);
		
		memDCSrc.SelectObject (pSrcOldBitmap);
		return;
	}

	memDCDst.FillRect (CRect (0, 0, m_sizeImage.cx, m_sizeImage.cy), 
						&globalData.brBtnFace);

	int x = max (0, (m_sizeImage.cx - bmp.bmWidth) / 2);
	int y = max (0, (m_sizeImage.cy - bmp.bmHeight) / 2);

	memDCDst.BitBlt (x, y, m_sizeImage.cx, m_sizeImage.cy,
					&memDCSrc, 0, 0, SRCCOPY);

	memDCDst.SelectObject (pDstOldBitmap);
	memDCSrc.SelectObject (pSrcOldBitmap);

	InvalidateRect (m_rectPreviewImage);
	m_wndLargeDrawArea.Invalidate ();
}
//****************************************************************************************
LRESULT CBCGImageEditDlg::OnKickIdle(WPARAM, LPARAM)
{
	CControlBar* pBar = &m_wndPaletteBar;
	pBar->OnUpdateCmdUI (m_pWndPaletteBarDummyFrame, TRUE);

    return 0;
}
//****************************************************************************************
BOOL CBCGImageEditDlg::OnPickColor (COLORREF color)
{
	if (!m_wndColor.SelectColor (color))
	{
		return FALSE;
	}

	//-----------------------------------------
	// Move to the pen mode (not so good :-(!):
	//-----------------------------------------
	m_wndLargeDrawArea.SetMode (CImagePaintArea::IMAGE_EDIT_MODE_PEN);
	return TRUE;
}
