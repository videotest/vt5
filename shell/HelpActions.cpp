#include "stdafx.h"
#include "shell.h"
#include "mainfrm.h"
#include "misc.h"
#include "HelpActions.h"
#include "CommandManager.h"
#include "aboutdlg.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionHelpIndex, CCmdTarget);
IMPLEMENT_DYNCREATE(CActionHelpAbout, CCmdTarget);
IMPLEMENT_DYNCREATE(CActionHelpContext, CCmdTarget);
IMPLEMENT_DYNCREATE(CActionHelpContents, CCmdTarget);

//[ag]4. olecreate release

// {BFE2698F-468A-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionHelpIndex, "Shell.HelpIndex",
0xbfe2698f, 0x468a, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {BFE2698B-468A-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionHelpAbout, "Shell.HelpAbout",
0xbfe2698b, 0x468a, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {BFE26987-468A-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionHelpContext, "Shell.HelpContext",
0xbfe26987, 0x468a, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {BFE26983-468A-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionHelpContents, "Shell.HelpContents",
0xbfe26983, 0x468a, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

//[ag]5. guidinfo release

// {BFE2698D-468A-11d3-A614-0090275995FE}
static const GUID guidHelpIndex =
{ 0xbfe2698d, 0x468a, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {BFE26989-468A-11d3-A614-0090275995FE}
static const GUID guidHelpAbout =
{ 0xbfe26989, 0x468a, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {BFE26985-468A-11d3-A614-0090275995FE}
static const GUID guidHelpContext =
{ 0xbfe26985, 0x468a, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {BFE26981-468A-11d3-A614-0090275995FE}
static const GUID guidHelpContents =
{ 0xbfe26981, 0x468a, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

//[ag]6. action info

ACTION_INFO_FULL(CActionHelpIndex, IDS_HELP_INDEX, IDS_MENU_HELP, -1, guidHelpIndex);
ACTION_INFO_FULL(CActionHelpAbout, IDS_ACTION_HELP_ABOUT, IDS_MENU_HELP, -1, guidHelpAbout);
ACTION_INFO_FULL(CActionHelpContext, IDS_ACTION_HELP_CONTEXT, IDS_MENU_HELP, IDS_TB_HELP, guidHelpContext);
//ACTION_INFO_FULL(CActionHelpContents, -1, IDS_MENU_HELP, -1, guidHelpContents);

//[ag]7. targets


//[ag]8. arguments

ACTION_ARG_LIST(CActionHelpContext)
	ARG_STRING( "Topic", 0 )
END_ACTION_ARG_LIST()


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionHelpIndex implementation
CActionHelpIndex::CActionHelpIndex()
{
}

CActionHelpIndex::~CActionHelpIndex()
{
}

bool CActionHelpIndex::Invoke()
{
	::HelpDisplay( AfxGetMainWnd()->GetControllingUnknown() );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionHelpAbout implementation
CActionHelpAbout::CActionHelpAbout()
{
}

CActionHelpAbout::~CActionHelpAbout()
{
}

bool CActionHelpAbout::Invoke()
{
	CAboutDlg	dlg( AfxGetMainWnd() );
	dlg.DoModal();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionHelpContext implementation
CActionHelpContext::CActionHelpContext()
{
}

CActionHelpContext::~CActionHelpContext()
{
}

bool CActionHelpContext::m_bInside = false;

bool CActionHelpContext::Invoke()
{
	if(m_bInside) return false; // вложенный вызов - отказать
	m_bInside = true;

	m_strActionName = GetArgumentString( "Topic" );
	if( !m_strActionName.IsEmpty() ) 
	{
		CActionInfoWrp	*pinfo = g_CmdManager.GetActionInfo( m_strActionName );

		if( !pinfo )
		{
			m_bInside = false;
			return false;
		}
		::HelpDisplay( pinfo->m_pActionInfo );

		m_bInside = false;
		return true;
	}
	else
	{
		CMainFrame	*pMainFrame = (CMainFrame	*)AfxGetMainWnd();
		bool bRet = pMainFrame->HelpMode();
		m_bInside = false;
		return bRet;
	}
}


//////////////////////////////////////////////////////////////////////
//CActionHelpContents implementation
CActionHelpContents::CActionHelpContents()
{
}

CActionHelpContents::~CActionHelpContents()
{
}

bool CActionHelpContents::Invoke()
{
	return true;
}


