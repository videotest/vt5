#include "stdafx.h"
#include "monsteractions.h"
#include "resource.h"
#include "MonsterWindow.h"
#include "MessageWindow.h"
#include "textdlg.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionMonsterMessage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMonsterMove, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMonsterShow, CCmdTargetEx);

// olecreate 

// {A0C1FDAB-70FD-11d3-A65B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionMonsterMessage, "ScriptDoc.MonsterMessage",
0xa0c1fdab, 0x70fd, 0x11d3, 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {A0C1FDA7-70FD-11d3-A65B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionMonsterMove, "ScriptDoc.MonsterMove",
0xa0c1fda7, 0x70fd, 0x11d3, 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {A0C1FDA3-70FD-11d3-A65B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionMonsterShow, "ScriptDoc.MonsterShow",
0xa0c1fda3, 0x70fd, 0x11d3, 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

//guidinfo 

// {A0C1FDA9-70FD-11d3-A65B-0090275995FE}
static const GUID guidMonsterMessage =
{ 0xa0c1fda9, 0x70fd, 0x11d3, { 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {A0C1FDA5-70FD-11d3-A65B-0090275995FE}
static const GUID guidMonsterMove =
{ 0xa0c1fda5, 0x70fd, 0x11d3, { 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {A0C1FDA1-70FD-11d3-A65B-0090275995FE}
static const GUID guidMonsterShow =
{ 0xa0c1fda1, 0x70fd, 0x11d3, { 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };



//[ag]6. action info

ACTION_INFO_FULL(CActionMonsterMove, IDS_ACTION_MONSTERMOVE, -1, -1, guidMonsterMove);
ACTION_INFO_FULL(CActionMonsterMessage, IDS_ACTION_DISPLAYMESSAGE, -1, IDS_MONSTER_MENU, guidMonsterMessage);
ACTION_INFO_FULL(CActionMonsterShow, IDS_ACTION_SHOWMONSTER, -1, IDS_MONSTER_MENU, guidMonsterShow);

//[ag]7. targets
//ACTION_TARGET(CActionMonsterMessage, szTargetMainFrame);


//[ag]8. arguments

ACTION_ARG_LIST(CActionMonsterMessage)
	ARG_STRING("Message", 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionMonsterMove)
	ARG_INT("X", 0)
	ARG_INT("Y", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionMonsterShow)
	ARG_INT("Show", -1)
END_ACTION_ARG_LIST()

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionMonsterMessage implementation
CActionMonsterMessage::CActionMonsterMessage()
{
	m_pwndMessage = 0;
}

CActionMonsterMessage::~CActionMonsterMessage()
{
	if( m_pwndMessage )
		m_pwndMessage->DestroyWindow();
}

bool CActionMonsterMessage::Initialize()
{
	if( !CInteractiveActionBase::Initialize() )return false;

	m_pwndMessage = new CMessageWindow( this );
	m_pwndMessage->Create( CMonsterWindow::s_pMonsterWindow, m_strMessage );
	return true;
}

bool CActionMonsterMessage::ExecuteSettings( CWnd *pwndParent )
{
	m_strMessage = GetArgumentString( _T("Message") );

	if( m_strMessage.IsEmpty() ) 
	{
		CTextDlg	dlg( pwndParent );

		if( dlg.DoModal() != IDOK )
		{
			LeaveMode();
			return false;
		}
		m_strMessage = dlg.m_strText;
	}

	SetArgument( _T("Message"), _variant_t((const char*)m_strMessage));

	return true;
}

bool CActionMonsterMessage::Invoke()
{
	LeaveMode();
	return true;
}

void CActionMonsterMessage::Finalize()
{
	CInteractiveActionBase::Finalize();
	m_pwndMessage = 0;
}

//////////////////////////////////////////////////////////////////////
//CActionMonsterMove implementation
CActionMonsterMove::CActionMonsterMove()
{
}

CActionMonsterMove::~CActionMonsterMove()
{
}

bool CActionMonsterMove::Invoke()
{
	int	xPos = GetArgumentInt( _T("X") );
	int	yPos = GetArgumentInt( _T("Y") );

	if( xPos != -1 && yPos != -1 )
	{
		CRect	rc;
		CMonsterWindow::s_pMonsterWindow->GetWindowRect( &rc );
		CMonsterWindow::s_pMonsterWindow->MoveWindow( xPos, yPos, rc.Width(), rc.Height() );
	}
	return true;
}

//extended UI
bool CActionMonsterMove::IsAvaible()
{
	if( !CMonsterWindow::s_pMonsterWindow )
		return false;
	return true;
}

bool CActionMonsterMove::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionMonsterShow implementation
bool CActionMonsterShow::Invoke()
{
	if( !CMonsterWindow::s_pMonsterWindow )
		return false;

	int	iShow = GetArgumentInt( "Show" );

	if( iShow == -1 )
	{
		if( IsChecked() )
			iShow = 0;
		else
			iShow = 1;
	}

	if( iShow == 0 )
	{
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 0, CMonsterWindow::stPuzz );
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 300, CMonsterWindow::stBlue );
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 600, CMonsterWindow::stInvisible );
	}
	else
	{
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 0, CMonsterWindow::stNormal );
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 300, CMonsterWindow::stFun );
		CMonsterWindow::s_pMonsterWindow->SheduleOperation( 600, CMonsterWindow::stNormal );
	}

	SetArgument( _T("Show"), _variant_t( (long)iShow ) );

	return true;
}

bool CActionMonsterShow::IsChecked()
{
	if( !CMonsterWindow::s_pMonsterWindow )
		return false;
	return CMonsterWindow::s_pMonsterWindow->IsWindowVisible() == TRUE;
}

