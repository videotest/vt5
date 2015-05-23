#include "stdafx.h"
#include "AXAction.h"
#include "resource.h"
#include "FormManager.h"
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionFormPressOK, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFormPressCancel, CCmdTargetEx);

// olecreate 

// {351EB3BF-9EA0-4ef7-B73B-4E4736EE60DE}
GUARD_IMPLEMENT_OLECREATE(CActionFormPressOK, "AXForm.FormPressOK", 
0x351eb3bf, 0x9ea0, 0x4ef7, 0xb7, 0x3b, 0x4e, 0x47, 0x36, 0xee, 0x60, 0xde);

// {DD33AB91-7C44-4673-AB5B-002F96405464}
GUARD_IMPLEMENT_OLECREATE(CActionFormPressCancel, "AXForm.FormPressCancel", 
0xdd33ab91, 0x7c44, 0x4673, 0xab, 0x5b, 0x0, 0x2f, 0x96, 0x40, 0x54, 0x64);

// guidinfo 

// {1C7C552E-5FC4-42b6-B3CB-DBDA2F5C011F}
static const GUID guidFormPressOK = 
{ 0x1c7c552e, 0x5fc4, 0x42b6, { 0xb3, 0xcb, 0xdb, 0xda, 0x2f, 0x5c, 0x1, 0x1f } };

// {2B3885B7-16A1-4736-84EB-8D1226D4E832}
static const GUID guidFormPressCancel = 
{ 0x2b3885b7, 0x16a1, 0x4736, { 0x84, 0xeb, 0x8d, 0x12, 0x26, 0xd4, 0xe8, 0x32 } };

//[ag]6. action info

ACTION_INFO_FULL(CActionFormPressOK, IDS_ACTION_FORMPRESSOK, -1, -1, guidFormPressOK);
ACTION_INFO_FULL(CActionFormPressCancel, IDS_ACTION_FORMPRESSCANCEL, -1, -1, guidFormPressCancel);

//[ag]7. targets

ACTION_TARGET(CActionFormPressOK, "anydoc");
ACTION_TARGET(CActionFormPressCancel, "anydoc");


/////////////////////////////////////////////////////////////////////////////
bool CActionFormPressOK::Invoke()
{
	CFormManager::m_pFormManager->PressOK();
	return true;
}

bool CActionFormPressOK::IsAvaible()
{
	if( !CFormManager::m_pFormManager )
		return false;

	if( !CFormManager::m_pFormManager->IsPageRunning() )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionFormPressCancel::Invoke()
{
	CFormManager::m_pFormManager->PressCancel();
	return true;
}

bool CActionFormPressCancel::IsAvaible()
{
	if( !CFormManager::m_pFormManager )
		return false;

	if( !CFormManager::m_pFormManager->IsPageRunning() )
		return false;

	return true;
}