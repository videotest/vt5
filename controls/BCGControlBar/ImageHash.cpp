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

// ImageHash.cpp: implementation of the CImageHash class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHash.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//!!! andy modification - 2.06.99
__declspec( dllexport )
///end modification
CImageHash g_ImageHash;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageHash::CImageHash()
{
}
//****************************************************************************************
CImageHash::~CImageHash()
{
}
//****************************************************************************************
void CImageHash::Set (UINT uiCmd, int iImage, BOOL bUserImage)
{
	if (uiCmd == 0 || uiCmd == (UINT) -1)
	{
		return;
	}

	if (bUserImage)
	{
		if (Get (uiCmd, FALSE) < 0)
		{
			m_UserImages.SetAt (uiCmd, iImage);
		}
	}
	else
	{
		if (Get (uiCmd, TRUE) < 0)
		{
			m_StdImages.SetAt (uiCmd, iImage);
		}
	}
}
//****************************************************************************************
int CImageHash::Get (UINT uiCmd, BOOL bUserImage) const
{
	int iImage = -1;

	if (bUserImage)
	{
		if (!m_UserImages.Lookup (uiCmd, iImage))
		{
			return -1;
		}
	}
	else
	{
		if (!m_StdImages.Lookup (uiCmd, iImage))
		{
			return -1;
		}
	}
	
	return iImage;
}
//****************************************************************************************
int CImageHash::GetImageOfCommand (UINT uiCmd, BOOL bUserImage)
{
	return g_ImageHash.Get (uiCmd, bUserImage);
}
//***************************************************************************************
void CImageHash::Clear (UINT uiCmd)
{
	m_UserImages.RemoveKey (uiCmd);
}
//****************************************************************************************
void CImageHash::ClearAll ()
{
	m_UserImages.RemoveAll ();
	m_StdImages.RemoveAll ();
}
