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

// BCGUserTool.cpp: implementation of the CBCGUserTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGUserTool.h"
#include "globals.h"
#include "bcgbarres.h"
#include "bcglocalres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGUserTool, CObject, VERSIONABLE_SCHEMA | 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGUserTool::CBCGUserTool()
{
	m_uiCmdId = 0;
	m_hIcon = NULL;
}
//*****************************************************************************************
CBCGUserTool::~CBCGUserTool()
{
}
//*****************************************************************************************
void CBCGUserTool::Serialize (CArchive& ar)
{
	CObject::Serialize (ar);

	if (ar.IsLoading ())
	{
		ar >> m_strLabel;
		
		CString strCmd;
		ar >> strCmd;
		SetCommand (strCmd);

		ar >> m_strArguments;
		ar >> m_strInitialDirectory;
		ar >> m_uiCmdId;
	}
	else
	{
		ar << m_strLabel;
		ar << m_strCommand;
		ar << m_strArguments;
		ar << m_strInitialDirectory;
		ar << m_uiCmdId;
	}
}
//*****************************************************************************************
BOOL CBCGUserTool::Invoke ()
{
	if (m_strCommand.IsEmpty ())
	{
		TRACE(_T("Empty command in user-defined tool: %d\n"), m_uiCmdId);
		return FALSE;
	}

	if (::ShellExecute (AfxGetMainWnd()->GetSafeHwnd (), NULL, m_strCommand,
		m_strArguments, m_strInitialDirectory, 
		SW_SHOWNORMAL) < (HINSTANCE) 32)
	{
		TRACE(_T("Can't invoke command: %s\n"), m_strCommand);
		return FALSE;
	}

	return TRUE;
}
//******************************************************************************
void CBCGUserTool::DrawToolIcon (CDC* pDC, const CRect& rectImage)
{
	ASSERT_VALID (pDC);

	int cx = ::GetSystemMetrics (SM_CXSMICON);
	int cy = ::GetSystemMetrics (SM_CYSMICON);

	int x = max (0, (rectImage.Width () - cx) / 2);
	int y = max (0, (rectImage.Height () - cy) / 2);

	::DrawIconEx (pDC->GetSafeHdc (),
		rectImage.left + x, rectImage.top + y, m_hIcon,
		0, 0, 0, NULL, DI_NORMAL);
}
//******************************************************************************
void CBCGUserTool::SetCommand (LPCTSTR lpszCmd)
{
	ASSERT (lpszCmd != NULL);

	if (m_strCommand != lpszCmd)
	{
		m_strCommand = lpszCmd;
		m_hIcon = SetToolIcon ();
	}
}
//*****************************************************************************
HICON CBCGUserTool::SetToolIcon ()
{
	CString strPath = m_strCommand;

	//--------------------------------------------
	// If the image path is not defined, found it:
	//--------------------------------------------
	if (strPath.Find (_T("\\")) == -1 &&
		strPath.Find (_T("/")) == -1 &&
		strPath.Find (_T(":")) == -1)
	{
		TCHAR lpszPath [MAX_PATH];
		CString strFileName = m_strCommand;

		if (::SearchPath (NULL, strFileName, NULL, MAX_PATH, 
			lpszPath, NULL) == 0)
		{
			return LoadDefaultIcon ();
		}

		strPath = lpszPath;
	}

	//----------------------------------------
	// Try to obtain a default icon from file:
	//----------------------------------------
	SHFILEINFO sfi;
	if (::SHGetFileInfo (strPath, 0, &sfi, sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SHELLICONSIZE | SHGFI_SMALLICON))
	{
		return sfi.hIcon;
	}

	return LoadDefaultIcon ();
}
//******************************************************************************
HICON CBCGUserTool::LoadDefaultIcon ()
{
	if (globalData.m_hiconTool == NULL)
	{
		CBCGLocalResource locaRes;

		globalData.m_hiconTool = (HICON) ::LoadImage (
			AfxGetResourceHandle (),
			MAKEINTRESOURCE (IDI_BCGRES_TOOL),
			IMAGE_ICON,
			::GetSystemMetrics (SM_CXSMICON),
			::GetSystemMetrics (SM_CYSMICON),
			LR_SHARED);
	}

	return globalData.m_hiconTool;
}
