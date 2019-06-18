#include "stdafx.h"
#include "CutFrame.h"
#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "objectlist.h"
#include "float.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"
#include "units_const.h"
#include "ObjActions.h" // For CopyParams(...)

static void _SetNamedDataInt(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, long nValue)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return;
	INamedDataPtr sptrNDO(punk);
	if (sptrNDO == 0)
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}
	sptrNDO->SetupSection(bstrSect);
	sptrNDO->SetValue(bstrKey, _variant_t(long(nValue)));
}

void _SetNamedDataDouble(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, double dValue)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return;
	INamedDataPtr sptrNDO(punk);
	if (sptrNDO == 0)
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}
	sptrNDO->SetupSection(bstrSect);
	sptrNDO->SetValue(bstrKey, _variant_t(dValue));
}

static long _CalcObjArea(CImageWrp &sptrImage)
{
	long lPoints = 0;
	int cx,cy;
	sptrImage->GetSize(&cx,&cy);
	for (int y = 0; y < cy; y++)
	{
		byte *pRowMask = sptrImage.GetRowMask(y);
		for (int x = 0; x < cx; x++)
			if (pRowMask[x] >= 0x80)
				lPoints++;
	}
	return lPoints;
}



IMPLEMENT_DYNCREATE(CActionCutFrame, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFindBorderObjects, CCmdTargetEx);

// {B5810984-483B-469e-ADFB-128A0AF2E62C}
GUARD_IMPLEMENT_OLECREATE(CActionCutFrame, "FiltersMain.CutFrame", 
0xb5810984, 0x483b, 0x469e, 0xad, 0xfb, 0x12, 0x8a, 0xa, 0xf2, 0xe6, 0x2c);

// {26B1BEF4-132D-4940-AB8D-7401AD0B802D}
GUARD_IMPLEMENT_OLECREATE(CActionFindBorderObjects, "FiltersMain.FindBorderObjects", 
0x26b1bef4, 0x132d, 0x4940, 0xab, 0x8d, 0x74, 0x1, 0xad, 0xb, 0x80, 0x2d);

// {69CFE3C0-B609-4262-9F68-500BDC7F6219}
static const GUID guidCutFrame = 
{ 0x69cfe3c0, 0xb609, 0x4262, { 0x9f, 0x68, 0x50, 0xb, 0xdc, 0x7f, 0x62, 0x19 } };

ACTION_INFO_FULL(CActionCutFrame, IDS_ACTION_CUT_FRAME, -1, -1, guidCutFrame);

// {D838097C-3FCA-46ad-998C-60543136E984}
static const GUID guidFindBorderObjects = 
{ 0xd838097c, 0x3fca, 0x46ad, { 0x99, 0x8c, 0x60, 0x54, 0x31, 0x36, 0xe9, 0x84 } };

ACTION_INFO_FULL(CActionFindBorderObjects, IDS_ACTION_FIND_BORDER_OBJECTS, -1, -1, guidFindBorderObjects);

ACTION_TARGET(CActionCutFrame, szTargetAnydoc);
ACTION_TARGET(CActionFindBorderObjects, szTargetAnydoc);

//////////////////////////////////////////////////////////////////////
//CActionCutFrameBase implementation

static CRect _RectFromImage(CImageWrp &sptrFrame)
{
	CPoint ptOffs = sptrFrame.GetOffset();
	int cx,cy;
	sptrFrame->GetSize(&cx, &cy);
	return CRect(ptOffs.x, ptOffs.y, ptOffs.x+cx, ptOffs.y+cy);
}

static bool _RectByObject(IUnknown *punkObject, CRect &rc)
{
	IRectObjectPtr sptrRO(punkObject);
	if (sptrRO != 0)
	{
		rc = CRect(0,0,0,0);
		if (sptrRO->GetRect(&rc) == S_OK)
			return true;
	}
	INamedDataObject2Ptr sptrNDO2Obj(punkObject);
	if (sptrNDO2Obj == 0)
		return false;
	long	lpos = 0;
	sptrNDO2Obj->GetFirstChildPosition(&lpos);
	while (lpos)
	{
		IUnknownPtr punk;
		sptrNDO2Obj->GetNextChild(&lpos, &punk);
		if (_RectByObject(punk, rc))
			return true;
	}
	return false;
}

static IUnknownPtr CopyObjectToSecondObjectList(IUnknown *punkObjSrc, IUnknown *punkObjList)
{
	IUnknownPtr punkCloned;
	IClonableObjectPtr sptrClone(punkObjSrc);
	if(sptrClone != 0)
	{
		sptrClone->Clone(&punkCloned);
		INamedDataObject2Ptr ptrObject(punkCloned);
		if(ptrObject != 0)
			ptrObject->SetParent(punkObjList, 0);
	}
	return punkCloned;
}

static int _DistToBorder(short* dist_map, CSize size, CPoint offset, CImageWrp& sptrImage)
{	// по заранее рассчитанной карте расстояний от края (числа >=0 внутри, <0 снаружи)
	// найти расстояние от объекта до края (min в мапе по точкам объекта)
	int nDist = 32768;
	CRect rc = _RectFromImage(sptrImage);

	for (int y = rc.top; y < rc.bottom; y++)
	{
		int y1 = y-offset.y;
		int dy = 0;
		if(y1<0) { dy=-y1; y1=0; };
		if(y1>=size.cy) { dy=y1-(size.cy-1); y1=size.cy-1; };
		byte *pRowMask = sptrImage.GetRowMask(y-rc.top);
		short *pdist = dist_map + size.cx*y1;
		for (int x = rc.left; x < rc.right; x++)
		{
			if( *(pRowMask++) & 128 )
			{
				int x1 = x-offset.x;
				int dx = 0;
				if(x1<0) { dx = -x1; x1=0; };
				if(x1>=size.cx) { dx = x1-(size.cx-1); x1=size.cx-1; };
				nDist = min(nDist, pdist[x1]-dx-dy);
			}
		}
	}
	return nDist;
}

static int _DistToBorder(short* dist_map, CSize size, CPoint offset, CRect rc)
{	// по заранее рассчитанной карте расстояний от края (числа >=0 внутри, <0 снаружи)
	// найти расстояние от объекта до края (min в мапе по точкам объекта)
	int nDist = 32768;

	for (int y = rc.top; y < rc.bottom; y++)
	{
		int y1 = y-offset.y;
		int dy = 0;
		if(y1<0) { dy=-y1; y1=0; };
		if(y1>=size.cy) { dy=y1-(size.cy-1); y1=size.cy-1; };
		short *pdist = dist_map + size.cx*y1;
		for (int x = rc.left; x < rc.right; x++)
		{
			int x1 = x-offset.x;
			int dx = 0;
			if(x1<0) { dx = -x1; x1=0; };
			if(x1>=size.cx) { dx = x1-(size.cx-1); x1=size.cx-1; };
			nDist = min(nDist, pdist[x1]-dx-dy);
		}
	}
	return nDist;
}


bool CActionCutFrameBase::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());

	bool bComplexFrame = 0 != ::GetValueInt(GetAppUnknown(), "\\CutFrame", "ComplexFrame", 0);

	long nCountIn = objectsIn.GetCount();
	if (!nCountIn) return false;
	CopyParams(objectsIn, objectsOut);
	IUnknownPtr punkImage(GetActiveObjectFromDocument(m_punkTarget, szArgumentTypeImage), false);
	CImageWrp sptrFrame(punkImage);
	if (sptrFrame == 0) return false;
	CRect rcFrame(_RectFromImage(sptrFrame));

	int cx = sptrFrame.GetWidth();
	int cy = sptrFrame.GetHeight();
	smart_alloc(dist, short, bComplexFrame ? cx*cy : 0); // выделим память, если надо

	if(bComplexFrame)
	{  // если сложная рамка - создадим карту расстояний до нее
		smart_alloc(dist_empty, short, cx);
		for(int x=0; x<cx; x++) dist_empty[x]=-16384;

		short *p1 = dist_empty;
		for(int y=0; y<cy; y++)
		{
			short *p = dist+y*cx;
			int v1 = -16384; // предыдущее значение в строке
			byte *pm = sptrFrame.GetRowMask(y);
			for(int x=0; x<cx; x++)
			{
				int v = *p;
				if((*pm) & 128)
				{
					int vv = min(v1,*p1);
					vv = max(-1,vv);
					v = vv+1;
				}
				else
				{
					int vv = max(v1,*p1);
					vv = min(0,vv);
					v = vv-1;
				}
				*p = v1 = v;
				p++; p1++; pm++;
			}
			p1 = p-cx;
		}

		p1 = dist_empty+cx-1;
		for(int y=cy-1; y>=0; y--)
		{
			short *p = dist + y*cx + cx-1;
			int v1 = -16384; // предыдущее значение в строке
			byte *pm = sptrFrame.GetRowMask(y)+cx-1;
			for(int x=cx-1; x>=0; x--)
			{
				int v = *p;
				if((*pm) & 128)
				{
					int vv = min(v1,*p1);
					vv = max(-1,vv);
					v = min(v,vv+1);
				}
				else
				{
					int vv = max(v1,*p1);
					vv = min(0,vv);
					v = max(v,vv-1);
				}
				*p = v1 = v;
				p--; p1--; pm--;
			}
			p1 = p+cx;
		}

	}


	long lBorderWidth = GetValueInt(GetAppUnknown(), "\\CutFrame", "BorderWidth", 3);
	rcFrame.InflateRect(-lBorderWidth,-lBorderWidth);
	StartNotification(nCountIn);
	long nNotificator = 0;
	bool bCalcArea = CalcCutArea();
	long lTotalArea,lCutArea = 0, lObjListArea = 0;
	if (bCalcArea)
		lTotalArea = _CalcObjArea(sptrFrame);
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		IMeasureObjectPtr sptrMeas(punkObj);
		if (punkObj)
			punkObj->Release();
		if (sptrMeas == 0)
		{
			Notify(nNotificator++);
			continue;
		}
		bool bDelete = false;
		IUnknownPtr punkObjImg;
		sptrMeas->GetImage(&punkObjImg);
		CImageWrp sptrObjImg(punkObjImg);
		if (sptrObjImg == 0)
		{
			CRect rc;
			if (_RectByObject(sptrMeas, rc))
			{
				if (rc.left <= rcFrame.left || rc.top <= rcFrame.top ||
					rc.right >= rcFrame.right || rc.bottom >= rcFrame.bottom)
					bDelete = true;
				if(bComplexFrame && !bDelete) // для сложной рамки дополнительно проверим по карте
				{
					if( _DistToBorder(dist, CSize(cx,cy), sptrFrame.GetOffset(), rc) <= lBorderWidth )
					bDelete = true;
				}
			}
			else
				bDelete = true;
		}
		else
		{
			CRect rc = _RectFromImage(sptrObjImg);
			if (rc.left <= rcFrame.left || rc.top <= rcFrame.top ||
				rc.right >= rcFrame.right || rc.bottom >= rcFrame.bottom)
				bDelete = true;
			if(bComplexFrame && !bDelete) // для сложной рамки дополнительно проверим по карте
			{
				if( _DistToBorder(dist, CSize(cx,cy), sptrFrame.GetOffset(), sptrObjImg) <= lBorderWidth )
				bDelete = true;
			}
		}

		if (bCalcArea && sptrObjImg != 0)
		{
			long nArea = _CalcObjArea(sptrObjImg);
			lObjListArea += nArea;
			if (bDelete) lCutArea += nArea;
		}
		OnProcessObject(bDelete,sptrMeas,objectsOut);
		Notify(nNotificator++);
	}
	ICompositeObjectPtr coIn(objectsIn);
	if(coIn)
	{
		long bC;
		coIn->IsComposite(&bC);
		if(bC)
		{
			long binCount=0;
			coIn->GetPlaneCount(&binCount);
			ICompositeObjectPtr coOut(objectsOut);
			if(coOut)
			{
				coOut->SetCompositeFlag();
				coOut->SetPlaneCount(binCount);
				CCompositeInfo* ci;
				coIn->GetCompositeInfo(&ci);
				coOut->SetCompositeInfo(ci);
				coOut->RestoreTreeByGUIDs();	
			}

		}
	}
	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);
	if (bCalcArea)
	{
		long lBackgroundArea = lTotalArea-lObjListArea;
		double dCut = double(lCutArea)/double(lObjListArea);
		double dBackgroundAreaCut = lBackgroundArea*dCut;
		char szBuff[60];
		_itoa(BASE_PARAM_CUT_AREA, szBuff, 10);
		_SetNamedDataDouble(objectsOut, _bstr_t(BASE_PARAMS_SECTION), _bstr_t(szBuff),
			double(lCutArea)+dBackgroundAreaCut);
	}
	FinishNotification();
	return true;
}

ACTION_ARG_LIST(CActionCutFrame)
	ARG_OBJECT(_T("SrcList"))
	RES_OBJECT(_T("DstList"))
END_ACTION_ARG_LIST();


//////////////////////////////////////////////////////////////////////
//CActionCutFrame implementation
CActionCutFrame::CActionCutFrame()
{
}

CActionCutFrame::~CActionCutFrame()
{
}

void CActionCutFrame::OnProcessObject(bool bContact, IUnknown *punkObjSrc, IUnknown *punkObjLstDst)
{
	if (!bContact)
		CopyObjectToSecondObjectList(punkObjSrc, punkObjLstDst);
}



ACTION_ARG_LIST(CActionFindBorderObjects)
	ARG_OBJECT(_T("SrcList"))
	RES_OBJECT(_T("DstList"))
END_ACTION_ARG_LIST();

//////////////////////////////////////////////////////////////////////
//CActionFindBorderObjects implementation
CActionFindBorderObjects::CActionFindBorderObjects()
{
}

CActionFindBorderObjects::~CActionFindBorderObjects()
{
}

void CActionFindBorderObjects::OnProcessObject(bool bContact, IUnknown *punkObjSrc, IUnknown *punkObjLstDst)
{
	IUnknownPtr punkObjDst = CopyObjectToSecondObjectList(punkObjSrc, punkObjLstDst);
	if (bContact)
	{
		ICalcObjectPtr sptrCO(punkObjDst);
		set_object_class(sptrCO, -1);
	}
}

