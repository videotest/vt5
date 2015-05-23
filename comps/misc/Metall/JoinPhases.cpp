#include "StdAfx.h"
#include "joinphases.h"
#include "nameconsts.h"
#include "misc_classes.h"

_ainfo_base::arg CJoinPhasesInfo::s_pargs[] =
{
	{"Phase1Num",	szArgumentTypeInt, "128", true, false },
	{"Phase2Num",	szArgumentTypeInt, "129", true, false },
	{"Phase1",	szArgumentTypeBinaryImage, 0, true, true },
	{"Phase2",	szArgumentTypeBinaryImage, 0, true, true },
	{"Joint",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

CJoinPhases::CJoinPhases(void)
{
}

CJoinPhases::~CJoinPhases(void)
{
}

static void InsertPhase(IBinaryImagePtr ptrDst, IBinaryImagePtr ptrSrc, byte phase)
{ // Обязательно Dst должен целиком умещаться в Src!!!
	int cx1=0,cy1=0;
	ptrSrc->GetSizes(&cx1,&cy1);
	_point ptOffset1(0,0);
	ptrSrc->GetOffset(&ptOffset1);

	_point ptOffset(0,0);
	ptrDst->GetOffset(&ptOffset);

	int dx=ptOffset1.x-ptOffset.x;
	int dy=ptOffset1.y-ptOffset.y;

	for(int y1=0; y1<cy1; y1++)
	{
		byte *psrc, *pdst;
		ptrSrc->GetRow(&psrc, y1, false);
		ptrDst->GetRow(&pdst, y1+dy, false);
		pdst+=dx;
		for(int x1=0; x1<cx1; x1++)
		{
			if(psrc[x1]) pdst[x1]=phase;
		}
	}
}

bool CJoinPhases::InvokeFilter()
{
	IBinaryImagePtr	ptrPhase1( GetDataArgument("Phase1") );
	IBinaryImagePtr	ptrPhase2( GetDataArgument("Phase2") );
	IBinaryImagePtr ptrJoint( GetDataResult("Joint") );
	byte nPhase1 = byte(GetArgLong("Phase1Num"));
	byte nPhase2 = byte(GetArgLong("Phase2Num"));

	if(ptrPhase1==0 || ptrPhase2==0 || ptrJoint==0) return false;

	int cx1=0,cy1=0;
	ptrPhase1->GetSizes(&cx1,&cy1);
	_point ptOffset1(0,0);
	ptrPhase1->GetOffset(&ptOffset1);
	_rect rect1(ptOffset1.x,ptOffset1.y, ptOffset1.x+cx1, ptOffset1.y+cy1);

	int cx2=0,cy2=0;
	ptrPhase2->GetSizes(&cx2,&cy2);
	_point ptOffset2(0,0);
	ptrPhase2->GetOffset(&ptOffset2);
	_rect rect2(ptOffset2.x,ptOffset2.y, ptOffset2.x+cx2, ptOffset2.y+cy2);

	_rect rect = rect1;
	rect.merge(rect2);
	int cx=rect.width(), cy=rect.height();
	_point ptOffset(rect.top,rect.left);

	StartNotification( cy, 3 );

	ptrJoint->CreateNew(cx, cy);
	ptrJoint->SetOffset( ptOffset, TRUE);

	for(int y=0; y<cy; y++)
	{
		byte *pdst;
		ptrJoint->GetRow(&pdst, y, false);
		FillMemory(pdst,cx*sizeof(byte),0);
	}

	InsertPhase(ptrJoint, ptrPhase2, nPhase2);
	InsertPhase(ptrJoint, ptrPhase1, nPhase1);

	FinishNotification();

	return true;
}