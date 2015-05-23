// ActionScissors.cpp: implementation of the CActionScissors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CutArea.h"
#include "ActionScissors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CActionScissors, CActionCutArea);

GUARD_IMPLEMENT_OLECREATE(CActionScissors, "CutArea.Scissors", 
0x37c7746c, 0xb4f, 0x4ecf, 0xb7, 0xd, 0xf0, 0xf, 0x69, 0xff, 0xb0, 0x1e);

static const GUID CutAreaAction1 = 
{ 0xaee0e5a7, 0x69f6, 0x4c00, { 0x8c, 0x2f, 0x74, 0x67, 0xab, 0x98, 0xa8, 0x64 } };

ACTION_INFO_FULL(CActionScissors, IDS_MAKE_CUT_AREA1, -1, -1, CutAreaAction1);
ACTION_TARGET(CActionScissors, szTargetViewSite);

CActionScissors::CActionScissors()
{
	m_Type = 1;
}

CActionScissors::~CActionScissors()
{

}
