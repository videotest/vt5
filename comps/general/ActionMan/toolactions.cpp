#include "stdafx.h"
#include "toolactions.h"
#include "resource.h"

#include "dialogs.h"


IMPLEMENT_DYNCREATE(CActionRunCommand, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEnumActions, CCmdTargetEx);

// {82E22CF3-77EF-11d3-A665-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionRunCommand, "ActionMan.RunCommand",
0x82e22cf3, 0x77ef, 0x11d3, 0xa6, 0x65, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {ED4D8813-5C4B-11d3-A638-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionEnumActions, "ActionMan.EnumActions",
0xed4d8813, 0x5c4b, 0x11d3, 0xa6, 0x38, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// {82E22CF1-77EF-11d3-A665-0090275995FE}
static const GUID guidRunCommand =
{ 0x82e22cf1, 0x77ef, 0x11d3, { 0xa6, 0x65, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {ED4D8811-5C4B-11d3-A638-0090275995FE}
static const GUID guidEnumActions =
{ 0xed4d8811, 0x5c4b, 0x11d3, { 0xa6, 0x38, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };



ACTION_INFO_FULL(CActionRunCommand, IDS_ACTION_RUN, -1, -1, guidRunCommand);
ACTION_INFO_FULL(CActionEnumActions, IDS_ACTION_ENUMACTIONS, -1, -1, guidEnumActions);



//////////////////////////////////////////////////////////////////////
//CActionRunCommand implementation
CActionRunCommand::CActionRunCommand()
{
}

CActionRunCommand::~CActionRunCommand()
{
}

bool CActionRunCommand::ExecuteSettings( CWnd *pwndParent )
{
	CScriptDlg	dlg;

	if( dlg.DoModal() != IDOK )
		return false;
	m_strScriptText = dlg.m_strScriptText;

	if( m_strScriptText.IsEmpty() )
		return false;

	return true;
}

bool CActionRunCommand::Invoke()
{
	_bstr_t	bstrScript = m_strScriptText;

	static int count = 0;
	char sz_buf[1024];
	sprintf( sz_buf, "RunCmd%d", count );

	::ExecuteScript( bstrScript, sz_buf );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEnumActions implementation
CActionEnumActions::CActionEnumActions()
{
}

CActionEnumActions::~CActionEnumActions()
{
}

bool CActionEnumActions::Invoke()
{
	CEnumDialog	dlg;
	dlg.DoModal();

	return true;
}


