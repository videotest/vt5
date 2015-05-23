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

// MenuImages.cpp: implementation of the CMenuImages class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuImages.h"
#include "bcglocalres.h"
#include "bcgbarres.h"

static const COLORREF clrTransparent = RGB (255, 0, 255);
static const int iImageWidth = 9;
static const int iImageHeight = 9;

CBCGToolBarImages CMenuImages::m_Images;

BOOL CMenuImages::Initialize ()
{
	if (!m_Images.IsValid ())
	{
		CBCGLocalResource locaRes;
		m_Images.SetImageSize (CSize (iImageWidth, iImageHeight));
		if (!m_Images.Load (IDB_BCGBARRES_MENU_IMAGES))
		{
			TRACE(_T("CMenuImages. Can't load menu images %x\n"), IDB_BCGBARRES_MENU_IMAGES);
			return FALSE;
		}
		
		//[ay]
		m_Images.SetTransparentColor (clrTransparent);
		
		//-----------------------------------------
		// Addapt menu images to the system colors:
		//-----------------------------------------
		m_Images.MapTo3dColors ();
	}

	return TRUE;
}
//****************************************************************************************
void CMenuImages::Draw (CDC* pDC, IMAGES_IDS id, const CPoint& ptImage)
{
	if (!Initialize ())
	{
		return;
	}

	CBCGDrawState ds;
	m_Images.PrepareDrawImage (ds);
	m_Images.Draw (pDC, ptImage.x, ptImage.y, id);
	m_Images.EndDrawImage (ds);
}
