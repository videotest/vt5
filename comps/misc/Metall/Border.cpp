#include "StdAfx.h"
#include "image5.h"
#include "misc_classes.h"
#include "misc_utils.h"
#include "Border.h"
#include "measure5.h"
#include "misc_new.h"
#include "MeasUtils.h"
#include "MetallParams.h"
#include <stdio.h>
#include <alloc.h>
#include <math.h>
#include "debug.h"

struct CDebugCtx
{
	int m_nDebug,m_nDebugStep;
	IUnknown *m_punkTarget;
	CDebugCtx(int nDebug, int nDebugStep, IUnknown *punkTarget)
	{
		m_nDebug = nDebug;
		m_nDebugStep = nDebugStep;
		m_punkTarget = punkTarget;
	}
};

static void SaveDebugImage(IUnknown *punkTarget, IUnknown *punkImage)
{
	::SetValue(punkTarget, 0, 0, _variant_t(punkImage));
}

static _rect RectByImage(IImage4 *pimg)
{
	_point ptOffset;
	pimg->GetOffset(&ptOffset);
	int cx,cy;
	pimg->GetSize(&cx,&cy);
	_rect rc(ptOffset, _size(cx,cy));
	return rc;
}

static _rect CalcObjectSize(INamedDataObject2 *pNDOObjLst)
{
	_rect rc(0,0,0,0);
	bool bInit = false;
	POSITION lPos;
	pNDOObjLst->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr punkObj;
		pNDOObjLst->GetNextChild(&lPos, &punkObj);
		IMeasureObjectPtr sptrMObj(punkObj);
		IUnknownPtr punkImage;
		sptrMObj->GetImage(&punkImage);
		IImage4Ptr sptrImg(punkImage);
		if (sptrImg != 0)
		{
			_rect rc1 = RectByImage(sptrImg);
			if (!bInit)
			{
				bInit = true;
				rc = rc1;
			}
			else
			{
				if (rc1.left < rc.left)
					rc.left = rc1.left;
				if (rc1.top < rc.top)
					rc.top = rc1.top;
				if (rc1.right > rc.right)
					rc.right = rc1.right;
				if (rc1.bottom > rc.bottom)
					rc.bottom = rc1.bottom;
			}
		}
	}
	return rc;
}

struct CImageData
{
	IImage4Ptr m_img;
	_size m_sz;
//	_rect m_rc;
	long m_Dirs[8];
	_point m_PtDirs[8];
	IImage4Ptr m_imgBase;
	_ptr_t2<IUnknown*> m_ObjectsIn;
	bool m_bInitContours;
	CImageData()
	{
		m_bInitContours = false;
	}
};

static void MakeImage(_size sz, CImageData &ImageData)
{
	// Create intermediate image itself
	IUnknownPtr punkImage(CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	IImage4Ptr sptrImage(punkImage);
	sptrImage->CreateImage(sz.cx, sz.cy, _bstr_t("Gray"), -1);
	for (int i = 1; i < sz.cy-1; i++)
	{
		color *pRow;
		sptrImage->GetRow(i, 0, &pRow);
		memset(pRow, 0xFF, sz.cx*sizeof(color));
	}
	ImageData.m_img = sptrImage;
	// Initialize direction points for calculations
	color *pRow1,*pRow2;
	sptrImage->GetRow(0, 0, &pRow1);
	sptrImage->GetRow(1, 0, &pRow2);
	long lRowDiff = (long)(pRow2-pRow1);
	long lColDiff = (long)1;
	ImageData.m_Dirs[0] = -lRowDiff;
	ImageData.m_PtDirs[0] = _point(0,-1);
	ImageData.m_Dirs[1] = lColDiff;
	ImageData.m_PtDirs[1] = _point(1,0);
	ImageData.m_Dirs[2] = lRowDiff;
	ImageData.m_PtDirs[2] = _point(0,1);
	ImageData.m_Dirs[3] = -lColDiff;
	ImageData.m_PtDirs[3] = _point(-1,0);
	ImageData.m_Dirs[4] = -lRowDiff+lColDiff;
	ImageData.m_PtDirs[4] = _point(1,-1);
	ImageData.m_Dirs[5] = lRowDiff+lColDiff;
	ImageData.m_PtDirs[5] = _point(1,1);
	ImageData.m_Dirs[6] = lRowDiff-lColDiff;
	ImageData.m_PtDirs[6] = _point(-1,1);
	ImageData.m_Dirs[7] = -lRowDiff-lColDiff;
	ImageData.m_PtDirs[7] = _point(-1,-1);
}

// Color for map:
// 0-0xFFF0 - object
// 0xFFFC, 0xFFFD - borders
// 0xFFFE - image borders
// 0xFFFF - background

static void MapObjectsToImage(CImageData &ImageData, INamedDataObject2 *pNDOObjLstIn, int nObjects, _point ptTopLeft, bool bSequence)
{
	IImage4 *pimgMap = ImageData.m_img;
	color clrObject = 0;
	POSITION lPos;
	if (bSequence)
	{
		ImageData.m_ObjectsIn.alloc(nObjects);
		ImageData.m_ObjectsIn.zero();
	}
	pNDOObjLstIn->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr punkObj;
		pNDOObjLstIn->GetNextChild(&lPos, &punkObj);
		if (bSequence)
		{
			if (ImageData.m_ObjectsIn[clrObject] == 0)
				ImageData.m_ObjectsIn[clrObject] = (IUnknown*)punkObj;
		}
		IMeasureObjectPtr sptrMObj(punkObj);
		IUnknownPtr punkImage;
		sptrMObj->GetImage(&punkImage);
		IImage4Ptr sptrImg(punkImage);
		if (sptrImg != 0)
		{
			_point ptOffset;
			sptrImg->GetOffset(&ptOffset);
			int cx,cy;
			sptrImg->GetSize(&cx,&cy);
			for (int y = 0; y < cy; y++)
			{
				byte *pRowMaskSrc;
				sptrImg->GetRowMask(y, &pRowMaskSrc);
				color *pRowDst;
				pimgMap->GetRow(y+ptOffset.y,0,&pRowDst);
				for (int x = 0; x < cx; x++)
				{
					if (pRowMaskSrc[x]>=128)
						pRowDst[x+ptOffset.x] = clrObject;
				}
			}
		}
		if (bSequence)
		{
			clrObject++;
			if (clrObject >= 0xFFF0)
				clrObject = 0;
		}
		else
		{
			if (clrObject < 0xFEF0)
				clrObject+=0x100;
			else
				clrObject = (clrObject&0xFF)+1;
		}
	}
	color *pRow;
	pimgMap->GetRow(0, 0, &pRow);
	for (int x = 0; x < ImageData.m_sz.cx; x++)	pRow[x] = 0xFFFE;
	pimgMap->GetRow(ImageData.m_sz.cy-1, 0, &pRow);
	for (x = 0; x < ImageData.m_sz.cx; x++)	pRow[x] = 0xFFFE;
	for (int y = 1; y < ImageData.m_sz.cy-1; y++)
	{
		pimgMap->GetRow(y, 0, &pRow);
		pRow[0] = 0xFFFE;
		pRow[ImageData.m_sz.cx-1] = 0xFFFE;
	}
}

static void MakeObjectBorderArray(CImageData &ImageData, color **parray, int &nNodes)
{
	IImage4 *pimgMap = ImageData.m_img;
	long *pDirs = ImageData.m_Dirs;
	nNodes = 0;
	for (int y = 1; y < ImageData.m_sz.cy-1; y++)
	{
		color *pRow;
		pimgMap->GetRow(y,0,&pRow);
		pRow++;
		for (int x = 1; x < ImageData.m_sz.cx-1; x++,pRow++)
		{
			if (*pRow < 0xFFF0)
			{
				for (int i = 0; i < 8; i++)
					if (*(pRow+pDirs[i])==0xFFFF)
						break;
				if (i < 8)
				{
					*parray = pRow;
					parray++;
					nNodes++;
				}
			}
		}
	}
}

static void WidenStep(CImageData &ImageData, color **parrayIn, int nNodesIn, color **parrayOut, int &nNodesOut)
{
	color **parr1 = parrayIn;
	color **parr2 = parrayOut;
	long *pDir = ImageData.m_Dirs;
	for(int i = 0; i < nNodesIn; i++,parr1++)
	{
		color obj = **parr1;
		for (int j = 0; j < 8; j++)
		{
			color *p = *parr1+pDir[j];
			if (*p == 0xFFFF)
			{
				*p = obj;
				*parr2 = p;
				parr2++;
				nNodesOut++;
			}
		}
	}
}

static void WidenObjectImage(CImageData &ImageData, CDebugCtx DebugCtx)
{
	_ptr_t2<color*> arr1((ImageData.m_sz.cx-2)*(ImageData.m_sz.cy-2));
	color **parr1 = arr1;
	_ptr_t2<color*> arr2((ImageData.m_sz.cx-2)*(ImageData.m_sz.cy-2));
	color **parr2 = arr2;
	int nPoints1=0,nPoints2=0;
	MakeObjectBorderArray(ImageData,parr1,nPoints1);
	if (DebugCtx.m_nDebug == 2 && DebugCtx.m_nDebugStep == 0)
	{
		SaveDebugImage(DebugCtx.m_punkTarget, ImageData.m_img);
		return;
	}
	int nStep = 1;
	while (nPoints1)
	{
		nPoints2 = 0;
		WidenStep(ImageData,parr1,nPoints1,parr2,nPoints2);
		color **p = parr1;
		parr1 = parr2;
		parr2 = p;
		nPoints1 = nPoints2;
		if (DebugCtx.m_nDebug && DebugCtx.m_nDebugStep == nStep)
		{
			SaveDebugImage(DebugCtx.m_punkTarget, ImageData.m_img);
			return;
		}
		nStep++;
	}
	if (DebugCtx.m_nDebug == 2 && DebugCtx.m_nDebugStep == -1)
		SaveDebugImage(DebugCtx.m_punkTarget, ImageData.m_img);
}

struct CNodeEx
{
	short x,y;
	color *pRow;
};

static void TraceStep(CImageData &ImageData, _rect &rc, color obj1, color obj2, CNodeEx *pNodesIn,
	int nNodesIn, CNodeEx *pNodesOut, int &nNodesOut)
{
	for (int i = 0; i < nNodesIn; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			color *pRow1 = pNodesIn->pRow+ImageData.m_Dirs[j];
			if (*pRow1 == obj1)
			{
				for (int k = 0; k < 8; k++)
				{
					color *pRow2 = pRow1+ImageData.m_Dirs[k];
					if (*pRow2 == obj2)
						break;
				}
				if (k < 8)
				{
					*pRow1 = 0xFFFD;
					pNodesOut->pRow = pRow1;
					pNodesOut->x = pNodesIn->x+(short)(ImageData.m_PtDirs[j].x);
					pNodesOut->y = pNodesIn->y+(short)(ImageData.m_PtDirs[j].y);
					if (pNodesOut->x < rc.left) rc.left = pNodesOut->x;
					if (pNodesOut->x > rc.right) rc.right = pNodesOut->x;
					if (pNodesOut->y < rc.top) rc.top = pNodesOut->y;
					if (pNodesOut->y > rc.bottom) rc.bottom = pNodesOut->y;
					pNodesOut++;
					nNodesOut++;
					if (nNodesOut > 20)
					{
						dbg_assert(FALSE);
						return;
					}
				}
			}
		}
		pNodesIn++;
	}
}

static void TestPoint(color *pRow, long *pDir, color obj1)
{
	for (int i = 0; i < 8; i++)
		if (*(pRow+pDir[i]) == 0xFFFD)
			break;
	if (i < 8)
	{
		for (int j = 0; j < 8; j++)
		{
			color *pRow1 = pRow+pDir[j];
			if (*pRow1 < 0xFFF0 && *pRow1 != obj1)
				break;
		}
		if (j == 8)
			*pRow = 0xFFFC;
	}
}

static void WidenObject(CImageData &ImageData, _rect &rc, color obj1, color obj2, int n)
{
	for (int i = 0; i < n; i++)
	{
		// Calc new rect
		rc.left = max(rc.left-1,0);
		rc.top = max(rc.top-1,0);
		rc.right = min(rc.right+1,ImageData.m_sz.cx-1);
		rc.bottom = min(rc.bottom+1,ImageData.m_sz.cy-1);
		// Make expanded points as 0xFFFC
		for (int y = rc.top; y <= rc.bottom; y++)
		{
			color *pRow;
			ImageData.m_img->GetRow(y,0,&pRow);
			for (int x = rc.left; x <= rc.right; x++)
			{
				if (pRow[x] == obj1)
					TestPoint(pRow+x,ImageData.m_Dirs,obj1);
				if (pRow[x] == obj2)
					TestPoint(pRow+x,ImageData.m_Dirs,obj2);
			}
		}
		// Mark 0xFFFC as 0xFFFD
		for (int y = rc.top; y <= rc.bottom; y++)
		{
			color *pRow;
			ImageData.m_img->GetRow(y,0,&pRow);
			for (int x = rc.left; x <= rc.right; x++)
			{
				if (pRow[x] == 0xFFFC)
					pRow[x] = 0xFFFD;
			}
		}
	}
}

STracePoint1_(s_tpMkImage,"MkImg");
STracePoint1(s_tpMkImage1,"MkImg1", &s_tpMkImage);
STracePoint1(s_tpCont, "Contours", &s_tpMkImage1);
STracePoint1(s_tpMeas, "Meas", &s_tpCont);
STracePoint1(s_tpBP, "BP", &s_tpMeas);
STracePoint1(s_tpSP, "SP", &s_tpBP);
static void MakeBorderObject(CImageData &ImageData, _rect rc, INamedDataObject2 *pNDOObjLstOut,
	CMeasureContext &MeasCtx, IBorderParameters *pBP, color obj1, color obj2)
{
	StartTracePoint(s_tpMkImage);
	// Make object image
	IUnknownPtr punkImage(CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	IImage4Ptr sptrImage(punkImage);
	INamedDataObject2Ptr sptrNDO2Image(punkImage);
	sptrImage->CreateVirtual(rc);
	sptrImage->InitRowMasks();
	sptrNDO2Image->SetParent(ImageData.m_imgBase, 0);
	StartTracePoint(s_tpMkImage1);
	for (int y = 0; y < rc.height(); y++)
	{
		byte *pRowMask;
		sptrImage->GetRowMask(y,&pRowMask);
		color *pRow;
		ImageData.m_img->GetRow(y+rc.top,0,&pRow);
		pRow += rc.left;
		for (int x = 0; x < rc.width(); x++)
		{
			if (pRow[x] == 0xFFFD)
			{
				pRowMask[x] = 0xFF;
				pRow[x] = 0xFFFB;
			}
			else
				pRowMask[x] = 0;
		}
	}
	StartTracePoint(s_tpCont);
	if (ImageData.m_bInitContours)
		sptrImage->InitContours();
	// Make new object and add it to object list
	IUnknownPtr punkObject(CreateTypedObject(_bstr_t(szArgumentTypeObject)), false);
	IMeasureObjectPtr sptrMO(punkObject);
	sptrMO->SetImage(sptrImage);
	StartTracePoint(s_tpMeas);
	MeasCtx.Calc(punkObject,sptrImage);
	StartTracePoint(s_tpBP);
	if (pBP && ImageData.m_ObjectsIn.size())
	{
		ICalcObjectPtr sptrCO(punkObject);
		TPOS lPos;
		pBP->GetFirstPosEx(&lPos);
		ParameterDescriptorEx *pdescr;
		while (lPos)
		{
			pBP->GetNextParamEx(&lPos, &pdescr);
			if (pdescr->BorderParam.lKey >= 0 && !pdescr->bZombie)
			{
				if (pdescr->BorderParam.lKeyObj < 0)
					sptrCO->SetValue(pdescr->BorderParam.lKey, (double)pdescr->BorderParam.nObject==0?obj1+1:obj2+1);
				else
				{
					int nBase = pdescr->BorderParam.nObject==0?obj1:obj2;
					ICalcObjectPtr sptrCOBase(ImageData.m_ObjectsIn[nBase]);
					double dValue;
					if (sptrCOBase->GetValue(pdescr->BorderParam.lKeyObj, &dValue) == S_OK)
						sptrCO->SetValue(pdescr->BorderParam.lKey, dValue);
				}
			}
		}
	}
	StartTracePoint(s_tpSP);
	INamedDataObject2Ptr sptrNDO2(punkObject);
	sptrNDO2->SetParent(pNDOObjLstOut, 0);
	StopTracePoint(s_tpSP);
}

STracePoint1_(s_tpTrace,"Trace");
STracePoint1(s_tpWiden, "Widen", &s_tpTrace);
STracePoint1(s_tpMake, "Make", &s_tpWiden);
static void TraceObjectBorder(CImageData &ImageData, color obj1, color obj2, int x,
	int y, color *pRow, INamedDataObject2 *pNDOObjLstOut, CMeasureContext &MeasCtx,
	IBorderParameters *pBP)
{
	StartTracePoint(s_tpTrace);
	*pRow = 0xFFFD;
	_ptr_t2<CNodeEx> Nodes1(20);
	CNodeEx *p1 = Nodes1;
	_ptr_t2<CNodeEx> Nodes2(20);
	CNodeEx *p2 = Nodes2;
	int nNodes1 = 1, nNodes2 = 0;
	p1->x = x;
	p1->y = y;
	p1->pRow = pRow;
	_rect rc(x,y,x,y);
	while (nNodes1)
	{
		TraceStep(ImageData, rc, obj1, obj2, p1, nNodes1, p2, nNodes2);
		CNodeEx *p = p1;
		p1 = p2;
		p2 = p;
		nNodes1 = nNodes2;
		nNodes2 = 0;
	}
	StartTracePoint(s_tpWiden);
	WidenObject(ImageData,rc,obj1,obj2,1);
	StartTracePoint(s_tpMake);
	MakeBorderObject(ImageData,rc,pNDOObjLstOut,MeasCtx,pBP,obj1,obj2);
	StopTracePoint(s_tpMake);
}

static void TraceBorders(CImageData &ImageData, INamedDataObject2 *pNDOObjLstOut,
	CMeasureContext &MeasCtx, IBorderParameters *pBP)
{
	IImage4 *pimgMap = ImageData.m_img;
	long *pDirs = ImageData.m_Dirs;
	for (int y = 1; y < ImageData.m_sz.cy-1; y++)
	{
		color *pRow;
		pimgMap->GetRow(y,0,&pRow);
		pRow++;
		for (int x = 1; x < ImageData.m_sz.cx-1; x++,pRow++)
		{
			if (*pRow < 0xFFF0)
			{
				for (int i = 0; i < 8; i++)
					if (*(pRow+pDirs[i]) < 0xFFF0 && *(pRow+pDirs[i]) != *pRow)
					{
						TraceObjectBorder(ImageData,*pRow,*(pRow+pDirs[i]),x,y,pRow,
							pNDOObjLstOut,MeasCtx,pBP);
						break;
					}
			}
		}
	}
}

static IBorderParametersPtr FindMetallMeasGroup(CMeasureContext &MeasCtx)
{
	IBorderParametersPtr sptrBP;
	for(int nGroup = 0; nGroup < MeasCtx.GetMeasGroupCount(); nGroup++ )
	{
		IUnknownPtr	ptrG;
		MeasCtx.GetMeasMgr()->GetComponentUnknownByIdx(nGroup, &ptrG);
		IBorderParametersPtr sptrBPTest(ptrG);
		if (sptrBPTest != 0)
		{
			sptrBP = sptrBPTest;
			break;
		}
	}
	return sptrBP;
}

_ainfo_base::arg	CFindBorderInfo::s_pargs[] = 
{
	{"Calc", szArgumentTypeInt, "0", true, false },
	{"SourceImage", szArgumentTypeImage, 0, true, true},
	{"SourceObjList", szArgumentTypeObjectList, 0, true, true},
	{"Result", szArgumentTypeObjectList, 0, false, true},
	{"Debug", szArgumentTypeInt, "0", true, false },
	{"DebugStep", szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};



CFindBorder::CFindBorder()
{
}

bool CFindBorder::InvokeFilter()
{
	long bCalc = GetArgLong("Calc");
	CDebugCtx DebugCtx(GetArgLong("Debug"),GetArgLong("DebugStep"),m_ptrTarget);
	IImage4Ptr imageSource(GetDataArgument("SourceImage"));
	INamedDataObject2Ptr objectsIn(GetDataArgument("SourceObjList"));
	INamedDataObject2Ptr objectsOut(GetDataResult());
	if (objectsOut == 0 || objectsIn == 0) return false;
	long nCountIn; objectsIn->GetChildsCount(&nCountIn);
	if (!nCountIn) return false;
	StartNotification(50);
	_rect rc = RectByImage(imageSource);
	Notify(10);
	CImageData map;
	map.m_sz = rc.size();
	map.m_imgBase = imageSource;
	map.m_bInitContours = ::GetValueInt(GetAppUnknown(), "\\Metall\\FindBorder", "InitContours", 0) != 0;
	MakeImage(rc.size(), map);
	Notify(20);
	MapObjectsToImage(map, objectsIn, nCountIn, rc.top_left(), DebugCtx.m_nDebug==0||DebugCtx.m_nDebug>2);
	if (DebugCtx.m_nDebug == 1)
		SaveDebugImage(m_ptrTarget, map.m_img);
	else
	{
		Notify(30);
		WidenObjectImage(map, DebugCtx);
		Notify(40);
		if (DebugCtx.m_nDebug != 2)
		{
			CMeasureContext MeasCtx(bCalc?true:false,objectsOut);
			IBorderParametersPtr sptrBP(FindMetallMeasGroup(MeasCtx));
			if (sptrBP != 0)
				sptrBP->DefineBorderParameters(objectsOut, false);
			TraceBorders(map,objectsOut,MeasCtx,sptrBP);
			MeasCtx.DeInit();
		}
	}
	MsgBoxTracePoint(s_tpSP);
	MsgBoxTracePoint(s_tpMake);
	FinishNotification();
	return true;
}