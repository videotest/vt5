#include "stdafx.h"
#include "ObjMorph.h"
#include "resource.h"
#include "math.h"
#include "BinImageInt.h"
#include "misc_utils.h"
//#include "NameConsts.h"
//#include "objectlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CSize _GetImageSize(IUnknown *punkImage)
{
	IImagePtr sptrImage(punkImage);
	int cx,cy;
	sptrImage->GetSize(&cx, &cy);
	return CSize(cx,cy);
}

static void _MapObjectToBinary(IBinaryImage *pbinImage, IUnknown *punkObject, CSize szBin,
	int nExpand, BYTE byFill)
{
	IMeasureObjectPtr sptrMO(punkObject);
	IUnknownPtr punkObjImage;
	sptrMO->GetImage(&punkObjImage);
	IImage3Ptr sptrObjImage(punkObjImage);
	int cx,cy;
	sptrObjImage->GetSize(&cx, &cy);
	CPoint ptOffset;
	sptrObjImage->GetOffset(&ptOffset);
	ptOffset.x += nExpand;
	ptOffset.y += nExpand;
	for (int y = 0; y < cy; y++)
	{
		int Y = y+ptOffset.y;
		if (Y < 0 || Y >= szBin.cy)
			continue;
		byte *pRowDst,*pRowSrc;
		pbinImage->GetRow(&pRowDst, Y, TRUE);
		sptrObjImage->GetRowMask(y, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			int X = x+ptOffset.x;
			if (X < 0 || X >= szBin.cx)
				continue;
			pRowDst[X] = pRowSrc[x]==255?byFill:0;
		}
	}
}

static IBinaryImagePtr _CreateBinaryImageByObjectList(IUnknown *punkImage, IUnknown *punkObjList,
	IUnknownPtr &punkActiveObj, int nExpand = 0)
{
	CSize szBaseImage(_GetImageSize(punkImage));
	CSize szBin(szBaseImage.cx+2*nExpand, szBaseImage.cy+2*nExpand);
	IUnknownPtr punkBin(CreateTypedObject(szArgumentTypeBinaryImage), false);
	IBinaryImagePtr sptrBin(punkBin);
	sptrBin->CreateNew(szBin.cx, szBin.cy);
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	TPOS posActive; // Active object
	sptrNDO2Obj->GetActiveChild(&posActive);
	TPOS pos = 0;
	sptrNDO2Obj->GetFirstChildPosition(&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		bool bActive = pos==posActive;
		sptrNDO2Obj->GetNextChild(&pos, &sptr);
		if (!bActive)
			_MapObjectToBinary(sptrBin, sptr, szBin, nExpand, 0xFF);
	}
	return sptrBin;
};

static IBinaryImagePtr _CreateBinaryImageByActiveObject(IUnknown *punkImage, IUnknown *punkObjList,
	IUnknownPtr &punkActiveObj, int nExpand = 0)
{
	CSize szBaseImage(_GetImageSize(punkImage));
	CSize szBin(szBaseImage.cx+2*nExpand, szBaseImage.cy+2*nExpand);
	IUnknownPtr punkBin(CreateTypedObject(szArgumentTypeBinaryImage), false);
	IBinaryImagePtr sptrBin(punkBin);
	sptrBin->CreateNew(szBin.cx, szBin.cy);
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	TPOS posActive; // Active object
	IUnknownPtr sptr;
	sptrNDO2Obj->GetActiveChild(&posActive);
	if (posActive == 0)
		return sptrBin;
	sptrNDO2Obj->GetNextChild(&posActive, &sptr);
	_MapObjectToBinary(sptrBin, sptr, szBin, nExpand, 0xFF);
	punkActiveObj = sptr;
	return sptrBin;
};

static void _CorrectBinary(IBinaryImage *pBinImg, IBinaryImage *pBinImg2)
{
	byte *p0,*p1,*p2,*pRow;
	int cx,cy;
	pBinImg->GetSizes(&cx, &cy);
	pBinImg2->GetRow(&p0, 0, TRUE);
	pBinImg2->GetRow(&p1, 1, TRUE);
	pBinImg2->GetRow(&p2, 2, TRUE);
	pBinImg->GetRow(&pRow, 0, TRUE);
	if (pRow[0] == 0xFF && (p0[0]|p0[1]|p1[0]|p1[1]))
		pRow[0] = 0;
	if (pRow[cx-1] == 0xFF && (p0[cx-1]|p0[cx-2]|p1[cx-1]|p1[cx-2]))
		pRow[cx-1] = 0;
	for (int x = 1; x < cx-1; x++)
	{
		if (pRow[x] == 0xFF && (p0[x-1]|p0[x]|p0[x+1]|p1[x-1]|p1[x]|p1[x+1]))
			pRow[x] = 0;
	}
	for (int y = 1; y < cy-1; y++)
	{
		pBinImg->GetRow(&pRow, y, TRUE);
		if (pRow[0] == 0xFF && (p0[0]|p0[1]|p1[0]|p1[1]|p2[0]|p2[1]))
			pRow[0] = 0;
		if (pRow[cx-1] == 0xFF && (p0[cx-1]|p0[cx-2]|p1[cx-1]|p1[cx-2]|p2[cx-1]|p2[cx-2]))
			pRow[cx-1] = 0;
		for (int x = 1; x < cx-1; x++)
		{
			if (pRow[x]==0xFF && (p0[x-1]|p0[x]|p0[x+1]|p1[x-1]|p1[x]|p1[x+1]|p2[x-1]|p2[x]|p2[x+1]))
				pRow[x] = 0;
		}
		p0 = p1;
		p1 = p2;
		pBinImg2->GetRow(&p2, y+2, TRUE);
	}
	pBinImg->GetRow(&pRow, cy-1, TRUE);
	if (pRow[0] == 0xFF && (p0[0]|p0[1]|p1[0]|p1[1]))
		pRow[0] = 0;
	if (pRow[cx-1] == 0xFF && (p0[cx-1]|p0[cx-2]|p1[cx-1]|p1[cx-2]))
		pRow[cx-1] = 0;
	for (int x = 1; x < cx-1; x++)
	{
		if (pRow[x] == 0xFF && (p0[x-1]|p0[x]|p0[x+1]|p1[x-1]|p1[x]|p1[x+1]))
			pRow[x] = 0;
	}
};

static CRect _FindRect(IBinaryImage *pBinImg, BYTE byActiveObj)
{
	int cx,cy;
	pBinImg->GetSizes(&cx, &cy);
	bool bFound = false;
	CRect rc(0,0,0,0);
	for (int y = 0; y < cy; y++)
	{
		byte *pRow;
		pBinImg->GetRow(&pRow, y, TRUE);
		for (int x = 0; x < cx; x++)
		{
			if (pRow[x] == byActiveObj)
			{
				if (bFound)
				{
					rc.left = min(rc.left,x);
					rc.top = min(rc.top,y);
					rc.right = max(rc.right,x);
					rc.bottom = max(rc.bottom,y);
				}
				else
				{
					rc = CRect(x,y,x,y);
					bFound = true;
				}
			}
		}
	}
	return rc;
}

static CRect _MakeBorders(IBinaryImage *pBinImg, BYTE byActiveObj, int nBorders)
{
	int cx,cy;
	pBinImg->GetSizes(&cx, &cy);
	for (int y = 0; y < nBorders; y++)
	{
		byte *pRow;
		pBinImg->GetRow(&pRow, y, TRUE);
		memset(pRow, 0, cx);
	}
	for (; y < cx-nBorders; y++)
	{
		byte *pRow;
		pBinImg->GetRow(&pRow, y, TRUE);
		memset(pRow, 0, nBorders);
		memset(pRow+cx-nBorders, 0, nBorders);
	}
	for (; y < cx; y++)
	{
		byte *pRow;
		pBinImg->GetRow(&pRow, y, TRUE);
		memset(pRow, 0, cx);
	}
}

static void _MapMasksToObjImage(IImage3 *pimg, IBinaryImage *pBinImg, CRect rcObj, BYTE byAObj)
{
	for (int y = rcObj.top; y <= rcObj.bottom; y++)
	{
		byte *pSrc,*pDst;
		pimg->GetRowMask(y-rcObj.top,&pDst);
		pBinImg->GetRow(&pSrc,y,TRUE);
		for (int x = rcObj.left; x <= rcObj.right; x++)
			pDst[x-rcObj.left] = pSrc[x]==byAObj?0xFF:0;
	}
}

static void _MapImageToObjImage(IImage3 *pObjImg, IImage3 *pBaseImage, CRect rcObj, int nColors,
	BYTE byAObj)
{
	for (int nColor = 0; nColor < nColors; nColor++)
	{
		for (int y = rcObj.top; y <= rcObj.bottom; y++)
		{
			color *pSrc,*pDst;
			pObjImg->GetRow(y-rcObj.top,nColor,&pDst);
			pBaseImage->GetRow(y,nColor,&pSrc);
			for (int x = rcObj.left; x <= rcObj.right; x++)
				pDst[x-rcObj.left] = pSrc[x];
		}
	}
}

static void RemakeActiveObjectFromBin(IBinaryImage *pBinImg, IImage3 *pBaseImage,
	IUnknown *punkObject, IUnknownPtr &punkSavedImage, BYTE byObj, int nExpand = 0)
{
	IMeasureObjectPtr sptrMO(punkObject);
	sptrMO->GetImage(&punkSavedImage);
	IUnknownPtr punkObjImage(::CreateTypedObject(szTypeImage), false);
	sptrMO->SetImage(punkObjImage);
	IImage3Ptr sptrObjImage(punkObjImage);
	CRect rcObj(_FindRect(pBinImg, byObj));
	IUnknownPtr punkCC;
    pBaseImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr = NULL;
	sptrCC->GetCnvName(&bstr);
	int nPanes = GetImagePaneCount(pBaseImage);
	sptrObjImage->CreateImage(rcObj.Width()+1, rcObj.Height()+1, bstr, nPanes);
	sptrObjImage->InitRowMasks();
	::SysFreeString(bstr);
	CPoint ptOffset = rcObj.TopLeft();
	ptOffset.x -= nExpand;
	ptOffset.y -= nExpand;
	sptrObjImage->SetOffset(ptOffset, FALSE);
	_MapMasksToObjImage(sptrObjImage, pBinImg, rcObj, byObj);
	rcObj.OffsetRect(-nExpand,-nExpand);
	_MapImageToObjImage(sptrObjImage, pBaseImage, rcObj, nPanes, byObj);
	sptrObjImage->InitContours();
}

static int _SafeErode(IBinaryImage *pBinImg, int nCount, int nMinObj)
{
	int cx,cy;
	pBinImg->GetSizes(&cx,&cy);
	byte *p0,*p1,*p2;
	pBinImg->GetRow(&p0, 0, TRUE);
	for (int x = 0; x < cx; x++)
		if (p0[x] == 0xFF) p0[x] = 0xFE;
	for (int y = 1; y < cy-1; y++)
	{
		pBinImg->GetRow(&p0, y, TRUE);
		if (p0[0] == 0xFF) p0[0] = 0xFE;
		if (p0[cx-1] == 0xFF) p0[cx-1] = 0xFE;
	}
	pBinImg->GetRow(&p0, cy-1, TRUE);
	for (x = 0; x < cx; x++)
		if (p0[x] == 0xFF) p0[x] = 0xFE;
	for (int i = 0; i < nCount; i++)
	{
		pBinImg->GetRow(&p0, 0, TRUE);
		pBinImg->GetRow(&p1, 1, TRUE);
		pBinImg->GetRow(&p2, 2, TRUE);
		int nRestPoints = 0;
		for (int y = 1; y < cy-1; y++)
		{
			for (int x = 1; x < cx-1; x++)
			{
				if (p1[x]==0xFF)
				{
					if ((p0[x-1]&p0[x]&p0[x+1]&p1[x-1]&p1[x+1]&p2[x-1]&p2[x]&p2[x+1])==0)
						p1[x]--;
					else
						nRestPoints++;
				}
			}
			p0 = p1;
			p1 = p2;
			pBinImg->GetRow(&p2, y+2, TRUE);
		}
		BYTE b = nRestPoints<nMinObj?0xFF:0;
		for (int y = 0; y < cy; y++)
		{
			pBinImg->GetRow(&p0, y, TRUE);
			for (int x = 0; x < cx; x++)
			{
				if (p0[x]==0xFE) p0[x] = b;
			}
		}
		if (nRestPoints < nMinObj)
			return i;
	}
	return nCount;
}

static int _SafeDilate(IBinaryImage *pBinImg, int nCount, IBinaryImage *pBinImg2)
{
	int cx,cy;
	pBinImg->GetSizes(&cx,&cy);
	byte *p0,*p1,*p2;
	for (int i = 0; i < nCount; i++)
	{
		pBinImg->GetRow(&p0, 0, TRUE);
		pBinImg->GetRow(&p1, 1, TRUE);
		pBinImg->GetRow(&p2, 2, TRUE);
		if (p0[0]==0 && (p0[1]|p1[0]|p1[1])==0xFF)
			p0[0] = 1;
		if (p0[cx-1]==0 && (p0[cx-2]|p1[cx-1]|p1[cx-2])==0xFF)
			p0[cx-1] = 1;
		for (int x = 1; x < cx-1; x++)
		{
			if (p0[x]==0 && (p0[x-1]|p0[x+1]|p1[x-1]|p1[x]|p1[x+1])==0xFF)
				p0[x] = 1;
		}
		for (int y = 1; y < cy-1; y++)
		{
			if (y == 584)
				y = 584;
			if (p1[0]==0 && (p0[0]|p0[1]|p1[1]|p2[0]|p2[1])==0xFF)
				p1[0] = 1;
			if (p1[cx-1]==0 && (p0[cx-1]|p0[cx-2]|p1[cx-2]|p2[cx-1]|p2[cx-2])==0xFF)
				p1[cx-1] = 1;
			for (int x = 1; x < cx-1; x++)
			{
				if (p1[x]==0)
				{
					if ((p0[x-1]|p0[x]|p0[x+1]|p1[x-1]|p1[x+1]|p2[x-1]|p2[x]|p2[x+1])==0xFF)
						p1[x]++;
				}
			}
			p0 = p1;
			p1 = p2;
			pBinImg->GetRow(&p2, y+2, TRUE);
		}
		if (p1[0]==0 && (p1[1]|p0[0]|p0[1])==0xFF)
			p1[0] = 1;
		if (p1[cx-1]==0 && (p1[cx-2]|p0[cx-1]|p0[cx-2])==0xFF)
			p1[cx-1] = 1;
		for (int x = 1; x < cx-1; x++)
		{
			if (p1[x]==0 && (p1[x-1]|p1[x+1]|p0[x-1]|p0[x]|p0[x+1])==0xFF)
				p1[x] = 1;
		}
		for (int y = 0; y < cy; y++)
		{
			pBinImg->GetRow(&p0, y, TRUE);
			for (int x = 0; x < cx; x++)
			{
				if (p0[x]==1) p0[x] = 0xFF;
			}
		}
		if (pBinImg2)
			_CorrectBinary(pBinImg, pBinImg2);
	}
	return nCount;
}



bool CObjectMorphBase::DoRedo()
{
	IMeasureObjectPtr sptrMO(m_punkObject);
	IUnknownPtr punk;
	sptrMO->GetImage(&punk);
	sptrMO->SetImage(m_punkSavedImage);
	m_punkSavedImage = punk;
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, m_punkObject, cncChange);
	return true;
}

bool CObjectMorphBase::DoUndo()
{
	return DoRedo();
}

IMPLEMENT_DYNCREATE(CObjectErode, CActionBase);

// {B9C0DBAC-4F52-4332-AE05-EA9A696563CF}
GUARD_IMPLEMENT_OLECREATE(CObjectErode, "ObjectMorphology.ErodeActiveObject", 
0xb9c0dbac, 0x4f52, 0x4332, 0xae, 0x5, 0xea, 0x9a, 0x69, 0x65, 0x63, 0xcf);


// {F16010DF-9996-4b37-9EFD-DBBC1F3286EA}
static const GUID guidObjectErode = 
{ 0xf16010df, 0x9996, 0x4b37, { 0x9e, 0xfd, 0xdb, 0xbc, 0x1f, 0x32, 0x86, 0xea } };

ACTION_INFO_FULL(CObjectErode, IDS_ACTION_ERODE_ACTIVE_OBJECT, -1, -1, guidObjectErode);

ACTION_TARGET(CObjectErode, szTargetAnydoc);

ACTION_ARG_LIST(CObjectErode)
	ARG_INT(_T("OperationsCount"), 1)
	ARG_INT(_T("MinObject"), 5)
END_ACTION_ARG_LIST()


bool CObjectErode::Invoke()
{
	int nCount = GetArgumentInt("OperationsCount");
	if (nCount<1) nCount = 1;
	int nMinObject = GetArgumentInt("MinObject");
	if (nMinObject<5) nMinObject = 5;
	IUnknownPtr punkImage(::GetActiveObjectFromDocument(m_punkTarget, szTypeImage), false);
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	IUnknownPtr punkActiveObj;
	IBinaryImagePtr sptrBinImage(_CreateBinaryImageByActiveObject(punkImage, punkObjList, punkActiveObj));
	IImage3Ptr sptrBaseImage(punkImage);
	_SafeErode(sptrBinImage, nCount, nMinObject);
	m_punkObject = punkActiveObj;
	RemakeActiveObjectFromBin(sptrBinImage, sptrBaseImage, punkActiveObj, m_punkSavedImage, 0xFF);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, punkActiveObj, cncChange);
	return true;
}


IMPLEMENT_DYNCREATE(CObjectDilate, CActionBase);

// {341432E9-E079-469a-8B4F-59DEDA52E761}
GUARD_IMPLEMENT_OLECREATE(CObjectDilate, "ObjectMorphology.DilateActiveObject", 
0x341432e9, 0xe079, 0x469a, 0x8b, 0x4f, 0x59, 0xde, 0xda, 0x52, 0xe7, 0x61);

// {7283AC13-ABD2-452a-AE38-7D2C7B7CF249}
static const GUID guidObjectDilate = 
{ 0x7283ac13, 0xabd2, 0x452a, { 0xae, 0x38, 0x7d, 0x2c, 0x7b, 0x7c, 0xf2, 0x49 } };

ACTION_INFO_FULL(CObjectDilate, IDS_ACTION_DILATE_ACTIVE_OBJECT, -1, -1, guidObjectDilate);

ACTION_TARGET(CObjectDilate, szTargetAnydoc);

ACTION_ARG_LIST(CObjectDilate)
	ARG_INT(_T("OperationsCount"), 1)
END_ACTION_ARG_LIST()


bool CObjectDilate::Invoke()
{
	int nCount = GetArgumentInt("OperationsCount");
	if (nCount<1) nCount = 1;
	IUnknownPtr punkImage(::GetActiveObjectFromDocument(m_punkTarget, szTypeImage), false);
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	IUnknownPtr punkActiveObj;
	IBinaryImagePtr sptrBinImage(_CreateBinaryImageByActiveObject(punkImage, punkObjList, punkActiveObj));
	IBinaryImagePtr sptrBinImage2(_CreateBinaryImageByObjectList(punkImage, punkObjList, punkActiveObj));
	IImage3Ptr sptrBaseImage(punkImage);
	_SafeDilate(sptrBinImage, nCount, sptrBinImage2);
	m_punkObject = punkActiveObj;
	RemakeActiveObjectFromBin(sptrBinImage, sptrBaseImage, punkActiveObj, m_punkSavedImage, 0xFF);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, punkActiveObj, cncChange);
	return true;
}

IMPLEMENT_DYNCREATE(CObjectOpen, CActionBase);

// {9B4789A2-537D-4c8a-B74E-FE862E5B0568}
GUARD_IMPLEMENT_OLECREATE(CObjectOpen, "ObjectMorphology.OpenActiveObject", 
0x9b4789a2, 0x537d, 0x4c8a, 0xb7, 0x4e, 0xfe, 0x86, 0x2e, 0x5b, 0x5, 0x68);

// {7D1E11D4-6802-40c4-8252-05A12008CF07}
static const GUID guidObjectOpen = 
{ 0x7d1e11d4, 0x6802, 0x40c4, { 0x82, 0x52, 0x5, 0xa1, 0x20, 0x8, 0xcf, 0x7 } };

ACTION_INFO_FULL(CObjectOpen, IDS_ACTION_OPEN_ACTIVE_OBJECT, -1, -1, guidObjectOpen);

ACTION_TARGET(CObjectOpen, szTargetAnydoc);

ACTION_ARG_LIST(CObjectOpen)
	ARG_INT(_T("OperationsCount"), 1)
	ARG_INT(_T("MinObject"), 5)
END_ACTION_ARG_LIST()


bool CObjectOpen::Invoke()
{
	int nCount = GetArgumentInt("OperationsCount");
	if (nCount<1) nCount = 1;
	int nMinObject = GetArgumentInt("MinObject");
	if (nMinObject<5) nMinObject = 5;
	IUnknownPtr punkImage(::GetActiveObjectFromDocument(m_punkTarget, szTypeImage), false);
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	IUnknownPtr punkActiveObj;
	IBinaryImagePtr sptrBinImage(_CreateBinaryImageByActiveObject(punkImage, punkObjList, punkActiveObj, nCount));
	IBinaryImagePtr sptrBinImage2(_CreateBinaryImageByObjectList(punkImage, punkObjList, punkActiveObj, nCount));
	IImage3Ptr sptrBaseImage(punkImage);
	int n = _SafeErode(sptrBinImage, nCount, nMinObject);
	_SafeDilate(sptrBinImage, n, NULL);
	_CorrectBinary(sptrBinImage, sptrBinImage2);
	m_punkObject = punkActiveObj;
	RemakeActiveObjectFromBin(sptrBinImage, sptrBaseImage, punkActiveObj, m_punkSavedImage, 0xFF, nCount);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, punkActiveObj, cncChange);
	return true;
}


IMPLEMENT_DYNCREATE(CObjectClose, CActionBase);

// {9BEFD80B-1FCC-40d6-ABD0-A93F3D912C59}
GUARD_IMPLEMENT_OLECREATE(CObjectClose, "ObjectMorphology.CloseActiveObject", 
0x9befd80b, 0x1fcc, 0x40d6, 0xab, 0xd0, 0xa9, 0x3f, 0x3d, 0x91, 0x2c, 0x59);

// {6DDEEE46-8342-4591-B2A7-30DB09D789E4}
static const GUID guidObjectClose = 
{ 0x6ddeee46, 0x8342, 0x4591, { 0xb2, 0xa7, 0x30, 0xdb, 0x9, 0xd7, 0x89, 0xe4 } };

ACTION_INFO_FULL(CObjectClose, IDS_ACTION_CLOSE_ACTIVE_OBJECT, -1, -1, guidObjectClose);

ACTION_TARGET(CObjectClose, szTargetAnydoc);

ACTION_ARG_LIST(CObjectClose)
	ARG_INT(_T("OperationsCount"), 1)
	ARG_INT(_T("MinObject"), 5)
END_ACTION_ARG_LIST()


bool CObjectClose::Invoke()
{
	int nCount = GetArgumentInt("OperationsCount");
	if (nCount<1) nCount = 1;
	int nMinObject = GetArgumentInt("MinObject");
	if (nMinObject<5) nMinObject = 5;
	IUnknownPtr punkImage(::GetActiveObjectFromDocument(m_punkTarget, szTypeImage), false);
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	IUnknownPtr punkActiveObj;
	IBinaryImagePtr sptrBinImage(_CreateBinaryImageByActiveObject(punkImage, punkObjList, punkActiveObj, nCount));
	if (punkActiveObj == 0)
		return false;
	IBinaryImagePtr sptrBinImage2(_CreateBinaryImageByObjectList(punkImage, punkObjList, punkActiveObj, nCount));
	IImage3Ptr sptrBaseImage(punkImage);
	_SafeDilate(sptrBinImage, nCount, NULL);
	_SafeErode(sptrBinImage, nCount, nMinObject);
	_CorrectBinary(sptrBinImage, sptrBinImage2);
	m_punkObject = punkActiveObj;
	RemakeActiveObjectFromBin(sptrBinImage, sptrBaseImage, punkActiveObj, m_punkSavedImage, 0xFF, nCount);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, punkActiveObj, cncChange);
	return true;
}


IMPLEMENT_DYNCREATE(CObjectFillHoles, CActionBase);

// {CC08EF4A-DA9D-4be5-A0A5-57F38F47C84C}
GUARD_IMPLEMENT_OLECREATE(CObjectFillHoles, "ObjectMorphology.FillHolesActiveObject", 
0xcc08ef4a, 0xda9d, 0x4be5, 0xa0, 0xa5, 0x57, 0xf3, 0x8f, 0x47, 0xc8, 0x4c);

// {7570C725-6695-4e27-972B-8C549478012C}
static const GUID guidObjectFillHoles = 
{ 0x7570c725, 0x6695, 0x4e27, { 0x97, 0x2b, 0x8c, 0x54, 0x94, 0x78, 0x1, 0x2c } };

ACTION_INFO_FULL(CObjectFillHoles, IDS_ACTION_FILL_HOLES, -1, -1, guidObjectFillHoles);

ACTION_TARGET(CObjectFillHoles, szTargetAnydoc);

ACTION_ARG_LIST(CObjectFillHoles)
	ARG_INT(_T("Size"), 0 )
END_ACTION_ARG_LIST()

const int BodyMask = 1<<7;

static void ContourFillAreaWithHoles2(CWalkFillInfo &info, byte byteRawBody,
	byte byteExtMark, byte byteFill, byte byteIntMark, byte byteIntMark2, CRect rect, int nSize)
{
	if( rect == NORECT )
		rect = info.GetRect();
	int	x, y;
	byte	bytePrev, byteCur;
	bool	bFill = false;
	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		byteCur = 0x00;
		for( x = rect.left-1; x < rect.right+1; x++)
		{
			bytePrev = byteCur;
			byteCur = info.GetPixel( x, y );
			if ((bytePrev == byteExtMark  && byteCur == byteRawBody) ||
				(bytePrev == byteIntMark && byteCur == byteRawBody) )
			{
				ASSERT( !bFill );
				bFill = true;
			}
			if ((bytePrev == byteRawBody && byteCur == byteExtMark) ||
				(byteCur == byteIntMark && bytePrev == byteRawBody))
			{
				ASSERT( bFill );
				bFill = false;
			}
			if (bFill && (bytePrev&BodyMask) == BodyMask && byteCur == 0)
			{ // Hole
				Contour *p = ::ContourWalk(info, x-1, y, 2);
				if( !p )
					continue;
				p->lFlags = cfInternal;
				CRect rc = ::ContourCalcRect(p);
				if (rc.Width() >= nSize || rc.Height() >= nSize)
				{
					::ContourExternalMark(info, p, byteIntMark, byteRawBody);
					bFill = false;
					byteCur = info.GetPixel( x, y );
					ASSERT( byteCur == byteIntMark );
				}
				else
				{
					::ContourExternalMark(info, p, byteIntMark2, byteRawBody);
					byteCur = info.GetPixel(x, y);
					ASSERT(byteCur == byteIntMark2);
				}
				info.AddContour(p);
			}
			if( bFill )
				info.SetPixel(x, y, byteFill);
		}
		ASSERT(!bFill);
	}
	for(int c = 0; c < info.GetContoursCount(); c++)
		::ContourMark( info, info.GetContour( c ), byteFill);
	info.ClearContours();
}


class CWalkFillInfo2 : public CWalkFillInfo
{
public:
	CWalkFillInfo2(const CRect rect) : CWalkFillInfo(rect)
	{
	}
	bool FillObjects(int nSize)
	{
		CRect rectBounds = m_rect;
		CPoint pointStart = rectBounds.TopLeft();
		bool	bFound = false;
		for (int j = pointStart.y; j < rectBounds.bottom; j++)
			for (int i = rectBounds.left; i < rectBounds.right; i++)
				if (GetPixel(i, j) == m_byteRawBody)
				{
					Contour	*p = ::ContourWalk(*this, i, j, 6);
					if( !p )
						continue;
					bFound = true;
					p->lFlags = cfExternal;
					CRect	rect = ::ContourCalcRect(p);
					::ContourExternalMark(*this, p, m_byteExternalMark, m_byteContourMark);
					::ContourFillAreaWithHoles2(*this, m_byteContourMark,
						m_byteExternalMark, m_byteFillBody, m_byteInternalMark,
						0x03, rect, nSize);
					//kill external mark
					::ContourExternalMark(*this, p, 0, 0);
					::ContourDelete(p);
				}
		return bFound;
	}
};

static IUnknownPtr _FindImageOnActiveObject(IUnknown *punkObjList, IUnknownPtr &punkActiveObject)
{
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	TPOS posActive; // Active object
	IUnknownPtr punkObject;
	sptrNDO2Obj->GetActiveChild(&posActive);
	if (posActive == 0)
		return IUnknownPtr();
	sptrNDO2Obj->GetNextChild(&posActive, &punkActiveObject);
	IMeasureObjectPtr sptrMO(punkActiveObject);
	IUnknownPtr punkObjImage;
	sptrMO->GetImage(&punkObjImage);
	return punkObjImage;
}

static void _FillHoles(IBinaryImage *pBinImg, int nSize, IBinaryImage *pBinImg2)
{
	int cx,cy;
	pBinImg->GetSizes(&cx, &cy);
	CPoint ptOffset;
	pBinImg->GetOffset(&ptOffset);
	CRect rcBound(CPoint(0,0), CSize(cx, cy));
	byte *pbin;
	CWalkFillInfo2* pwfi = new CWalkFillInfo2(rcBound);
	for(long y = 0; y < cy; y++ )
	{
		pBinImg->GetRow(&pbin, y, FALSE);
		byte* pmaskdest = pwfi->GetRowMask(y);
		for(long x = 0; x < cx; x++, pmaskdest++)
		{
			if(pbin[x]!= 0)
				*pmaskdest = 255;
			else
				*pmaskdest = 0;
		}
	}
	pwfi->FillObjects(nSize);
	for (long y = 0; y < cy; y++)
	{
		byte* pmasksrc = pwfi->GetRowMask(y);
		pBinImg->GetRow(&pbin, y, FALSE);
		for(long x = 0; x < cx; x++)
			pbin[x] = pmasksrc[x]&BodyMask?255:0;
	}
	delete pwfi;
	if (pBinImg2)
		_CorrectBinary(pBinImg, pBinImg2);
}

bool CObjectFillHoles::Invoke()
{
	int	nSize = GetArgumentInt( "Size" );
	if (nSize == 0) nSize = 1000000;
	IUnknownPtr punkImage(::GetActiveObjectFromDocument(m_punkTarget, szTypeImage), false);
	IUnknownPtr punkObjList(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	IUnknownPtr punkActiveObj;
	IBinaryImagePtr sptrBinImage(_CreateBinaryImageByActiveObject(punkImage, punkObjList, punkActiveObj, 1));
	if (punkActiveObj == 0)
		return false;
	IBinaryImagePtr sptrBinImage2(_CreateBinaryImageByObjectList(punkImage, punkObjList, punkActiveObj, 1));
	IImage3Ptr sptrBaseImage(punkImage);
	_FillHoles(sptrBinImage, nSize, sptrBinImage2);
	m_punkObject = punkActiveObj;
	RemakeActiveObjectFromBin(sptrBinImage, sptrBaseImage, punkActiveObj, m_punkSavedImage, 0xFF, 1);
	::FireEventNotify(m_punkTarget, szEventChangeObjectList, punkObjList, punkActiveObj, cncChange);
	return true;
}
/**********************************************************************************************/
IMPLEMENT_DYNCREATE(CObjectSetZOrder, CActionBase);

// {018AF9CC-5A0D-4d6c-A6BA-7C364263CF5C}
GUARD_IMPLEMENT_OLECREATE(CObjectSetZOrder, "Objects.SetZOrder", 
0x18af9cc, 0x5a0d, 0x4d6c, 0xa6, 0xba, 0x7c, 0x36, 0x42, 0x63, 0xcf, 0x5c);

// {867F41F9-505A-4939-B4D6-16BCC17D9822}
static const GUID guidObjectSetZOrder = 
{ 0x867f41f9, 0x505a, 0x4939, { 0xb4, 0xd6, 0x16, 0xbc, 0xc1, 0x7d, 0x98, 0x22 } };


ACTION_INFO_FULL(CObjectSetZOrder, IDS_ACTION_SET_Z_ORDER, -1, -1, guidObjectSetZOrder);

ACTION_TARGET(CObjectSetZOrder, szTargetViewSite );

ACTION_ARG_LIST(CObjectSetZOrder)
	ARG_INT(_T("ParamKey"), 1 )
	ARG_INT(_T("Direction"), 0 )
END_ACTION_ARG_LIST()



#include "\vt5\awin\misc_list.h"

struct XItem
{
	IUnknownPtr sptrChild;
	POSITION lPos;
	
	XItem()
	{
		sptrChild = 0;
		lPos = 0;
	}

	XItem( const XItem &item )
	{
		*this = item;
	}

	void operator=( const XItem &item )
	{
		sptrChild = item.sptrChild;
		lPos = item.lPos;
	}
};


bool CObjectSetZOrder::Invoke()
{
	int	nKey = GetArgumentInt( "ParamKey" );
	int	nDir = GetArgumentInt( "Direction" );

	IUnknown *punkList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );

	INamedDataObject2Ptr sptrND = punkList;

	_list_t< XItem > list;

	POSITION lPos = 0;
	sptrND->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		XItem item;

		item.lPos = lPos;
		
		IUnknown *punkChild = 0;
		sptrND->GetNextChild( &lPos, &punkChild );

		item.sptrChild = punkChild;

		if( punkChild )
			punkChild->Release();
	}

	if( punkList )
		punkList->Release();

	int lZOrder = 0;
	for (TPOS lPos = list.head(); lPos; lPos = list.next(lPos))
	{
		XItem &item = list.get( lPos );

		if( item.sptrChild )
		{
			double fValue = 0;

			ICalcObjectPtr sptrCalc = item.sptrChild;

			if( sptrCalc )
				sptrCalc->GetValue( nKey, &fValue );



			TPOS _min_lp = lPos;

			for (TPOS lPos2 = list.next(lPos); lPos2; lPos2 = list.next(lPos2))
			{
				XItem &item2 = list.get( lPos2 );

				if( item2.sptrChild )
				{
					double fValue2 = 0;

					ICalcObjectPtr sptrCalc2 = item2.sptrChild;

					if( sptrCalc2 )
						sptrCalc2->GetValue( nKey, &fValue2 );

					if( !nDir )
					{
						if( fValue2 < fValue )
						{
							fValue = fValue2;
							_min_lp = lPos2;
						}
					}
					else
					{
						if( fValue2 > fValue )
						{
							fValue = fValue2;
							_min_lp = lPos2;
						}
					}

				}
			}

			{
				XItem item = list.get( lPos );
				
				XItem &_item = list.get( lPos );
				_item = list.get( _min_lp );
				
				XItem &__item = list.get( _min_lp );
				__item = item;

				IMeasureObject2Ptr sptrO = _item.sptrChild;

				if( sptrO )
					sptrO->SetZOrder( lZOrder );

				lZOrder++;
			}
		}
	}

	int nZOrder = 0;

	return true;
}

