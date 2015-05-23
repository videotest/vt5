#include "StdAfx.h"
#include "splitobjects.h"
#include "resource.h"
IMPLEMENT_DYNCREATE(CSplitObjects, CCmdTargetEx);

// {E3699FAE-16F7-4128-BD37-73D6DEA0301A}
GUARD_IMPLEMENT_OLECREATE(CSplitObjects, "Shape.SplitObjects", 
0xe3699fae, 0x16f7, 0x4128, 0xbd, 0x37, 0x73, 0xd6, 0xde, 0xa0, 0x30, 0x1a);

// {C4D89EBB-D9CE-42b5-BCCA-626D580583A3}
static const GUID guidSplitObjects = 
{ 0xc4d89ebb, 0xd9ce, 0x42b5, { 0xbc, 0xca, 0x62, 0x6d, 0x58, 0x5, 0x83, 0xa3 } };


ACTION_INFO_FULL(CSplitObjects,IDS_ACTION_SPLIT_OBJECTS, -1, -1, guidSplitObjects); 

ACTION_TARGET(CSplitObjects, szTargetAnydoc);

ACTION_ARG_LIST(CSplitObjects)
	ARG_INT("Class",-1)
	ARG_OBJECT(_T("ObjectsIn"))
	RES_OBJECT( _T("ObjectsOut"))
END_ACTION_ARG_LIST()

CSplitObjects::CSplitObjects(void)
{
}

CSplitObjects::~CSplitObjects(void)
{
}

bool CSplitObjects::InvokeFilter()
{
	IUnknown* pUnk =  GetDataArgument("ObjectsIn");
	if(pUnk==0) return false;

	CObjectListWrp ol(pUnk, false);
	int nClass = GetArgumentInt("Class");
	POSITION pos = ol.GetFirstObjectPosition();

	while(pos)
	{

	}

	return true;
}
