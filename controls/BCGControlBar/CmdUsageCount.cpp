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

// CmdUsageCount.cpp: implementation of the CCmdUsageCount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "CmdUsageCount.h"
#include "BCGToolBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT CCmdUsageCount::m_nStartCount = 50;
UINT CCmdUsageCount::m_nMinUsagePercentage = 5;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdUsageCount::CCmdUsageCount() :
	m_nTotalUsage (0)
{
}
//*******************************************************************************
CCmdUsageCount::~CCmdUsageCount()
{
}
//*******************************************************************************
void CCmdUsageCount::Serialize (CArchive& ar)
{
	if (ar.IsLoading ())
	{
		ar >> m_nTotalUsage;
	}
	else
	{
		ar << m_nTotalUsage;
	}

	m_CmdUsage.Serialize (ar);
}
//*******************************************************************************
void CCmdUsageCount::AddCmd (UINT uiCmd)
{
	if (CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

	if ((uiCmd == 0 || uiCmd == (UINT) -1)	||	// Ignore submenus and separators,
		CBCGToolBar::IsBasicCommand (uiCmd)	||	// basic commands and
		IsStandardCommand (uiCmd))				// standard commands
	{
		return;
	}

	UINT uiCount = 0;
	if (!m_CmdUsage.Lookup (uiCmd, uiCount))
	{
		uiCount = 0;
	}

	m_CmdUsage.SetAt (uiCmd, ++uiCount);
	m_nTotalUsage ++;
}
//*******************************************************************************
void CCmdUsageCount::Reset ()
{
	m_CmdUsage.RemoveAll ();
	m_nTotalUsage = 0;
}
//*******************************************************************************
UINT CCmdUsageCount::GetCount (UINT uiCmd) const
{
	UINT uiCount = 0;
	m_CmdUsage.Lookup (uiCmd, uiCount);

	return uiCount;
}
//*******************************************************************************
BOOL CCmdUsageCount::IsFreqeuntlyUsedCmd (UINT uiCmd) const
{
	//-----------------------------------------------------
	// I say, that the specific command is frequently used,
	// if the command usage percentage  is more than 20%
	//-----------------------------------------------------
	if (m_nTotalUsage == 0)
	{
		return FALSE;
	}

	UINT uiCount = GetCount (uiCmd);
	UINT uiPercentage = uiCount * 100 / m_nTotalUsage;

	return uiPercentage > m_nMinUsagePercentage;
}
//*******************************************************************************
BOOL CCmdUsageCount::HasEnouthInformation () const
{
	return m_nTotalUsage > m_nStartCount;
}
//*******************************************************************************
BOOL CCmdUsageCount::SetOptions (UINT nStartCount, UINT nMinUsagePercentage)
{
	if (nMinUsagePercentage >= 100)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	m_nStartCount = nStartCount;
	m_nMinUsagePercentage = nMinUsagePercentage;

	return TRUE;
}
