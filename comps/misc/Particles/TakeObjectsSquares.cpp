#include "stdafx.h"
#include "takeobjectssquares.h"

_ainfo_base::arg CTakeObjectsRectsInfo::s_pargs[] =
{
	{"cx", szArgumentTypeInt , "100", true, false },
	{"cy", szArgumentTypeInt , "100", true, false },
	{"Image",       szArgumentTypeImage, 0, true, true },
	{"Objects",    szArgumentTypeObjectList, 0, true, true },
	{"ObjectsWithRects",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

CTakeObjectsRects::CTakeObjectsRects(void)
{
}

CTakeObjectsRects::~CTakeObjectsRects(void)
{
}

bool CTakeObjectsRects::InvokeFilter()
{
	IImage3Ptr ptrImage( GetDataArgument("Image") );
	INamedDataObject2Ptr objectsIn(GetDataArgument("Objects"));
	INamedDataObject2Ptr objectsOut(GetDataResult());

	int nMinCX = GetArgLong("cx");
	int nMinCY = GetArgLong("cy");

	if(ptrImage==0 || objectsIn==0 || objectsOut==0) return false;

	long nCount; objectsIn->GetChildsCount(&nCount);
	if(!nCount) return false;

	StartNotification(nCount,1,1);

	m_ObjectPool.StartProcessing(objectsOut, ptrImage, nMinCX, nMinCY);
	long pos; objectsIn->GetFirstChildPosition(&pos);
	int nObj=0;
	while( pos ) //по всем объектам
	{
		Notify(nObj);
		nObj++;

		IUnknownPtr ptr1;
		objectsIn->GetNextChild(&pos, &ptr1);
		IMeasureObjectPtr ptrO(ptr1);
		if(ptrO==0) continue;
        
		IUnknownPtr ptr2;
		ptrO->GetImage( &ptr2 );
		IImage3Ptr ptrI(ptr2);
		if(ptrI==0) continue;

		int cx=0,cy=0;
		ptrI->GetSize(&cx,&cy);
		POINT ptOffset;
		ptrI->GetOffset(&ptOffset);
		_rect rect(ptOffset, _size(cx, cy));

		IMeasureObjectPtr ptrO2=m_ObjectPool.GetObject(rect);
		if(ptrO2==0)	continue;
		IUnknownPtr ptr3;
		ptrO2->GetImage( &ptr3 );
		IImage3Ptr ptrI2(ptr3);
		if(ptr2==0)	continue;

		int cx2,cy2;
		ptrI2->GetSize(&cx2, &cy2);
		_point ptOffset2;
		ptrI2->GetOffset(&ptOffset2);
		int dy = ptOffset.y - ptOffset2.y;
		int dx = ptOffset.x - ptOffset2.x;

		int xstart = max(0,dx);
		int xstop = min(dx+cx,cx2);

		for(int y2=0; y2<cy2; y2++)
		{
			byte *pmask2;
			ptrI2->GetRowMask(y2,&pmask2);
			ZeroMemory(pmask2,cx2);
			int y=y2-dy;
			if(y>=0 && y<cy)
			{
				byte *pmask;
				ptrI->GetRowMask(y,&pmask);
				CopyMemory(pmask2+xstart, pmask+xstart-dx, xstop-xstart);
			}
		} // for y2
		ptrI2->InitContours();
	}
	m_ObjectPool.StopProcessing();

	GUID guidNewBase;
	objectsIn->GetBaseKey(&guidNewBase);
	objectsOut->SetBaseKey(&guidNewBase);

	FinishNotification();

	return true;
}
