// StatGroupMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Classes.h"
#include "StatGroupMgr.h"
#include "ClassBase.h"
#include "NameConsts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatGroupMgr
IMPLEMENT_DYNCREATE(CStatGroupMgr, CParamGroupManBase)

// {941D678C-6F39-4e4f-81FA-D22E89C97FB4}
static const IID IID_IStatGroupMgrDisp =
{ 0x941d678c, 0x6f39, 0x4e4f, { 0x81, 0xfa, 0xd2, 0x2e, 0x89, 0xc9, 0x7f, 0xb4 } };
// {CE4AA513-3A74-4b10-AD36-A0D9678F1EB1}
GUARD_IMPLEMENT_OLECREATE(CStatGroupMgr, "Classes.StatGroupMgr", 
0xce4aa513, 0x3a74, 0x4b10, 0xad, 0x36, 0xa0, 0xd9, 0x67, 0x8f, 0x1e, 0xb1);


BEGIN_INTERFACE_MAP(CStatGroupMgr, CParamGroupManBase)
	INTERFACE_PART(CStatGroupMgr, IID_IStatGroupMgrDisp,	Dispatch)
END_INTERFACE_MAP()


CStatGroupMgr::CStatGroupMgr()
{
	m_bInConstructor = true;

	_OleLockApp( this );
	
	SetName(szStatistic);
	AttachComponentGroup(szPluginStatistic);
	SetSectionName(szPluginStatistic);

	EnableAutomation();
	EnableAggregation();
	Init();
	m_bInConstructor = false;
}

CStatGroupMgr::~CStatGroupMgr()
{
	_OleUnlockApp( this );
}


void CStatGroupMgr::OnFinalRelease()
{
	CParamGroupManBase::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CStatGroupMgr, CParamGroupManBase)
	//{{AFX_MSG_MAP(CStatGroupMgr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CStatGroupMgr, CParamGroupManBase)
	//{{AFX_DISPATCH_MAP(CStatGroupMgr)
	DISP_FUNCTION(CStatGroupMgr, "LoadState", LoadState, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CStatGroupMgr, "SaveState", SaveState, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatGroupMgr message handlers

void CStatGroupMgr::DeInit()
{
	if (!m_bInConstructor)
		SaveState();
	CParamGroupManBase::DeInit();
}

bool CStatGroupMgr::Init()
{
	return CParamGroupManBase::Init();
}

LPCTSTR CStatGroupMgr::GetGroupLoadParamString()
{
	return "";
}

void CStatGroupMgr::LoadState()
{
	LoadStateInner();
}

void CStatGroupMgr::SaveState()
{
	SaveStateInner();
}
