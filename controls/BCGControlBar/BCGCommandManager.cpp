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

#include "stdafx.h"
#include "stdafx.h"
#include "BCGCommandManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// One global static CBCGCommandManager Object
//////////////////////////////////////////////////////////////////////
class _STATIC_CREATOR_
{
public:
	CBCGCommandManager s_TheCmdMgr;
};

static _STATIC_CREATOR_ STATIC_CREATOR;

BCGCONTROLBARDLLEXPORT CBCGCommandManager* GetCmdMgr()
{
	return &STATIC_CREATOR.s_TheCmdMgr;
}
//////////////////////////////////////////////////////////////////////


#ifndef _NO_BCG_LEGACY_
UINT CImageHash::GetImageOfCommand(UINT nID, BOOL bUser /*= false*/)
{
	return CMD_MGR.GetCmdImage(nID, bUser);
}
#endif

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

CBCGCommandManager::CBCGCommandManager()
{
}

CBCGCommandManager::~CBCGCommandManager()
{
}


//////////////////////////////////////////////////////////////////////
// ImageHash functions
//////////////////////////////////////////////////////////////////////

//****************************************************************************************
void CBCGCommandManager::SetCmdImage (UINT uiCmd, int iImage, BOOL bUserImage)
{
	if (uiCmd == 0 || uiCmd == (UINT) -1)
	{
		return;
	}

	if (bUserImage)
	{
		if (GetCmdImage (uiCmd, FALSE) < 0)
		{
			m_CommandIndexUser.SetAt (uiCmd, iImage);
		}
	}
	else
	{
		if (GetCmdImage (uiCmd, TRUE) < 0)
		{
			m_CommandIndex.SetAt (uiCmd, iImage);
		}
	}
}
//****************************************************************************************
int CBCGCommandManager::GetCmdImage (UINT uiCmd, BOOL bUserImage) const
{
	int iImage = -1;

	if (bUserImage)
	{
		if (!m_CommandIndexUser.Lookup (uiCmd, iImage))
		{
			return -1;
		}
	}
	else
	{
		if (!m_CommandIndex.Lookup (uiCmd, iImage))
		{
			return -1;
		}
	}
	
	return iImage;
}

//***************************************************************************************
void CBCGCommandManager::ClearCmdImage (UINT uiCmd)
{
	m_CommandIndexUser.RemoveKey (uiCmd);
}
//****************************************************************************************
void CBCGCommandManager::ClearAllCmdImages ()
{
	m_CommandIndex.RemoveAll ();
	m_CommandIndexUser.RemoveAll ();
}
