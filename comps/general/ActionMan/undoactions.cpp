#include "stdafx.h"
#include "undoactions.h"
#include "resource.h"
#include "actionmanager.h"
#include "undolist.h"


IMPLEMENT_DYNCREATE(CActionUndo, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRedo, CCmdTargetEx);

// {087126A7-4FD6-11d3-A620-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionUndo, "ActionMan.Undo",
0x87126a7, 0x4fd6, 0x11d3, 0xa6, 0x20, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {087126A3-4FD6-11d3-A620-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionRedo, "ActionMan.Redo",
0x87126a3, 0x4fd6, 0x11d3, 0xa6, 0x20, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// {087126A5-4FD6-11d3-A620-0090275995FE}
static const GUID guidUndo =
{ 0x87126a5, 0x4fd6, 0x11d3, { 0xa6, 0x20, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {087126A1-4FD6-11d3-A620-0090275995FE}
static const GUID guidRedo =
{ 0x87126a1, 0x4fd6, 0x11d3, { 0xa6, 0x20, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

ACTION_INFO_FULL(CActionUndo, IDS_ACTION_UNDO, IDS_MENU_EDIT, IDS_TB_MAIN, guidUndo);
ACTION_INFO_FULL(CActionRedo, IDS_ACTION_REDO, IDS_MENU_EDIT, IDS_TB_MAIN, guidRedo);

ACTION_TARGET(CActionUndo, "anydoc");
ACTION_TARGET(CActionRedo, "anydoc");


//////////////////////////////////////////////////////////////////////
//CActionUndo implementation
CActionUndo::CActionUndo()
{
}

CActionUndo::~CActionUndo()
{
}

bool CActionUndo::Invoke()
{
	IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	IActionManagerPtr	ptrAM( punkAM );
	punkAM->Release();
	ptrAM->TerminateInteractiveAction();

	IUndoListPtr	ptrUndo( m_punkTarget );
	if( ptrUndo == 0 )return false;
	return ptrUndo->DoUndo() == S_OK;
}

bool CActionUndo::IsAvaible()
{
	IUndoListPtr	ptrUndo( m_punkTarget );
	if( ptrUndo == 0 )return false;
	int	nCount = 0;
	ptrUndo->GetUndoStepsCount( &nCount );

	return nCount != 0;
}

//////////////////////////////////////////////////////////////////////
//CActionRedo implementation
CActionRedo::CActionRedo()
{
}

CActionRedo::~CActionRedo()
{
}

bool CActionRedo::Invoke()
{
	IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	IActionManagerPtr	ptrAM( punkAM );
	punkAM->Release();
	ptrAM->TerminateInteractiveAction();
	
	IUndoListPtr	ptrUndo( m_punkTarget );
	if( ptrUndo == 0 )return false;
	return ptrUndo->DoRedo() == S_OK;
}

bool CActionRedo::IsAvaible()
{
	IUndoListPtr	ptrUndo( m_punkTarget );
	if( ptrUndo == 0 )return false;
	int	nCount = 0;
	ptrUndo->GetRedoStepsCount( &nCount );

	return nCount != 0;
}
