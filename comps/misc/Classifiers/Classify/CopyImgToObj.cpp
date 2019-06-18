#include "StdAfx.h"
#include "CopyImgToObj.h"
#include "nameconsts.h"
#include "data5.h"
#include "Classes5.h"



_bstr_t _GetCCName(IImage *pimg1)
{
	IUnknownPtr punkCC1;
	pimg1->GetColorConvertor(&punkCC1);
	if (punkCC1 == 0)
		return _bstr_t("");
	IColorConvertor2Ptr sptrCC1(punkCC1);
	if (sptrCC1 == 0)
		return _bstr_t("");
	BSTR bstr;
	sptrCC1->GetCnvName(&bstr);
	_bstr_t bstrN1(bstr);
	::SysFreeString(bstr);
	return bstrN1;
}

int _GetPanesCount(IImage *pimg1)
{
	return ::GetImagePaneCount( pimg1 );
}

static bool _IsSameCC(IImage *pimg1, IImage *pimg2, int *pnPanes)
{
	_bstr_t bstrCCName1 = _GetCCName(pimg1);
	_bstr_t bstrCCName2 = _GetCCName(pimg2);
	int nPanes1 = _GetPanesCount(pimg1);
	int nPanes2 = _GetPanesCount(pimg2);
	if (pnPanes)
		*pnPanes = nPanes1;
	if (bstrCCName1.length() > 0 && bstrCCName1 == bstrCCName2 && nPanes1 == nPanes2)
		return true;
	else
		return false;
}

static void _ImageFromBig(IUnknown *ptr, IImage3 *pImgBig, IImage3 *pImgOld, BSTR bstrCC, int nPanes)
{
	IMeasureObjectPtr ptrMO(ptr);
	if (ptrMO != 0)
	{
		// Sizes of images, offset and row mask will be copied from ald image
		POINT ptOffs;
		pImgOld->GetOffset(&ptOffs);
		int nHeight,nWidth;
		pImgOld->GetSize(&nWidth,&nHeight);
		// Create new image
		IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t("Image")), false);
		IImage3Ptr pImgNew(punkImgNew);
		pImgNew->CreateImage(nWidth, nHeight, bstrCC, nPanes );
		pImgNew->SetOffset(ptOffs, FALSE);
		// Copy rows and row masks
		pImgNew->InitRowMasks();
		for (int y = 0; y < nHeight; y++)
		{
			// Copy image data from big (every pane)
			for (int nPane = 0; nPane < nPanes; nPane++)
			{
				color *psrc,*pdst;
				pImgBig->GetRow(ptOffs.y+y, nPane, &psrc);
				pImgNew->GetRow(y, nPane, &pdst);
				memcpy(pdst,psrc+ptOffs.x,nWidth*sizeof(color));
			}
			// Copy row mask from old
			BYTE *psrcmsk,*pdstmsk;
			pImgOld->GetRowMask(y, &psrcmsk);
			pImgNew->GetRowMask(y, &pdstmsk);
			memcpy(pdstmsk,psrcmsk,nWidth*sizeof(BYTE));
		}
		// Contours will be created
		pImgNew->InitContours();
		ptrMO->SetImage(pImgNew);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionCopyImageToObjects
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionCopyImageToObjectsInfo::s_pargs[] = 
{
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"ObjOld",	szArgumentTypeObjectList, 0, true, true },	
	{"Obj",	szArgumentTypeObjectList, 0, false, true },	
	{0, 0, 0, false, false },
};

CActionCopyImageToObjects::CActionCopyImageToObjects()
{
}

bool CActionCopyImageToObjects::CanDeleteArgument( CFilterArgument *pa )
{
	if (pa->m_bInArgument && pa->m_bstrArgName == _bstr_t("ObjOld"))
		return true;
	else
		return false;
}

bool CActionCopyImageToObjects::InvokeFilter()
{
	IImage3Ptr sptrImg(GetDataArgument("Img"));
	int nPanes = _GetPanesCount(sptrImg);
	_bstr_t bstrCCName = _GetCCName(sptrImg);
	sptrINamedDataObject2 sptrObjIn(GetDataArgument("ObjOld")); // Object list
	sptrINamedDataObject2 sptrObjOut(GetDataResult()); // Object list
	long lObjects;
	sptrObjIn->GetChildsCount(&lObjects);
	StartNotification(lObjects);
	long pos = 0, i = 0;
	sptrObjIn->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrObjIn->GetNextChild((long*)&pos, &sptr);
		IClonableObjectPtr sptrClone(sptr);		
		if(sptrClone != 0)
		{
			IUnknownPtr ptr;
			sptrClone->Clone(&ptr);
			IMeasureObjectPtr ptrMO(ptr);
			if (ptrMO != 0)
			{
				IUnknownPtr punkImg;
				ptrMO->GetImage(&punkImg);
				IImage3Ptr sptrImg1(punkImg);
				if (sptrImg1 != 0)
					_ImageFromBig(ptr, sptrImg, sptrImg1, bstrCCName, nPanes);
				INamedDataObject2Ptr ptrObject(ptr); 
				if(ptrObject != 0)
					ptrObject->SetParent(sptrObjOut, 0);
			}
		}
		Notify(i++);
	}
	FinishNotification();
	return true;
}

