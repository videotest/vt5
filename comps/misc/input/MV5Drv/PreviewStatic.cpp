// PreviewStatic.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MV500Grab.h"
#include "PreviewStatic.h"
#include "StdProfile.h"
#include "misc_templ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic

CPreviewStatic::CPreviewStatic()
{
}

CPreviewStatic::~CPreviewStatic()
{
}


BEGIN_MESSAGE_MAP(CPreviewStatic, CStatic)
	//{{AFX_MSG_MAP(CPreviewStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic message handlers

void CPreviewStatic::OnPaint() 
{
	if (CMV500Grab::s_pGrab->IsOverlay())
	{
		CPaintDC dc(this); // device context for painting
		CRect rc1;
		GetClientRect(rc1);
		dc.FillSolidRect(rc1, RGB(128,128,0));
		CRect rc;
		GetWindowRect(rc);
		CMV500Grab::s_pGrab->Draw(rc);
		return;
	}
	CPaintDC dc(this); // device context for painting
	BYTE *pData = CMV500Grab::s_pGrab->GetRealData(m_nDevice);
	BOOL bHorzMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
	bool bIndirect = bHorzMirror&&CMV500Grab::s_pGrab->GetBoardType()==CMV500Grab::Board_MV510;
	if (pData && CMV500Grab::s_pGrab->CanDraw())
	{
		CSize sz = CMV500Grab::s_pGrab->GetSize();
		if (CMV500Grab::s_pGrab->m_bFields)
			sz.cy /= 2;
		BITMAPINFO256 bi;
		memset(&bi, 0, sizeof(BITMAPINFO256));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = sz.cx;
		if (!bIndirect&&CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE))
			bi.bmiHeader.biHeight = -sz.cy;
		else
			bi.bmiHeader.biHeight = sz.cy;
		bi.bmiHeader.biBitCount = CMV500Grab::s_pGrab->GetBitsPerSample();
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biClrImportant = bi.bmiHeader.biClrUsed = bi.bmiHeader.biBitCount==8?256:0;
		if (bi.bmiHeader.biClrImportant)
		{
			for (int i = 0; i < 256; i++)
			{
				bi.bmiColors[i].rgbBlue = bi.bmiColors[i].rgbGreen = bi.bmiColors[i].rgbRed = i;
			}
		}
		if (bIndirect)
		{
			_ptr_t2<byte> buf(((sz.cx*bi.bmiHeader.biBitCount/8+3)/4*4)*sz.cy);
			CMV500Grab::s_pGrab->CopyGrabbedImage((byte*)buf, (WORD*)pData, false);
			if (CMV500Grab::s_pGrab->m_bFields)
				StretchDIBits(dc.m_hDC,0,0,sz.cx,sz.cy*2,0,0,sz.cx,sz.cy,(byte*)buf,(BITMAPINFO*)&bi,DIB_RGB_COLORS,SRCCOPY);
			else
				SetDIBitsToDevice(dc.m_hDC,0,0,sz.cx,sz.cy,0,0,0,sz.cy,(byte*)buf,(BITMAPINFO *)&bi,DIB_RGB_COLORS);
		}
		else if (CMV500Grab::s_pGrab->m_bFields)
			StretchDIBits(dc.m_hDC,0,0,sz.cx,sz.cy*2,0,0,sz.cx,sz.cy,pData,(BITMAPINFO*)&bi,DIB_RGB_COLORS,SRCCOPY);
		else
			SetDIBitsToDevice(dc.m_hDC,0,0,sz.cx,sz.cy,0,0,0,sz.cy,pData,(BITMAPINFO *)&bi,DIB_RGB_COLORS);
	}
	else
	{
		CRect rc;
		GetClientRect(rc);
		dc.FillSolidRect(rc, RGB(0,0,0));
	}
	// Do not call CStatic::OnPaint() for painting messages
}



BOOL CPreviewStatic::OnEraseBkgnd(CDC* pDC) 
{
	if (CMV500Grab::s_pGrab->IsOverlay())
	{
		CRect rc;
		GetClientRect(rc);
		pDC->FillSolidRect(rc, RGB(128,128,0));
		return TRUE;
	}
	return CStatic::OnEraseBkgnd(pDC);
}
