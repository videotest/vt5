// DocBySel.cpp: implementation of the CActionDocBySel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FiltersMain.h"
#include "DocBySel.h"
#include "misc_utils.h"
#include "binimageint.h"
#include "binary.h"
#include "meas_utils.h"
#include "\vt5\common2\class_utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static bool GetActiveImage( IUnknown *punkC, IUnknown **ppunkI, IUnknown **ppunkS )
{
	(*ppunkI) = 0;
	(*ppunkS) = 0;

	IUnknown	*punkImage = 0;
	IUnknown	*punkOld = 0;

	if( CheckInterface( punkC, IID_IDocument ) )
		punkImage = GetActiveObjectFromDocument( punkC, szTypeImage );
	else
		punkImage = GetActiveObjectFromContext( punkC, szTypeImage );
	if( ::GetParentKey( punkImage ) != INVALID_KEY )
	{
		punkOld = punkImage;
		INamedDataObject2Ptr	ptrN( punkImage );
		ptrN->GetParent( &punkImage );
	}
	
	if( !punkImage )
	{
		if( punkOld )
			punkOld->Release();
		return false;
	}

	(*ppunkI) = punkImage;
	(*ppunkS) = punkOld;

	return true;
}

static IUnknownPtr CropImageBySel(IUnknown *pimg, IUnknown *pimgSel)
{
	IUnknownPtr punkImgNew(::CreateTypedObject("Image"), false);
	INamedDataObject2Ptr sptrNDO2Old(pimg);
	INamedDataObject2Ptr sptrNDO2NewObj(punkImgNew);
	GUID guidBaseKey;
	sptrNDO2Old->GetBaseKey(&guidBaseKey);
	sptrNDO2NewObj->SetBaseKey(&guidBaseKey);
	if (punkImgNew != 0)
	{
		CImageWrp imgNew(punkImgNew);
		CImageWrp imgSel(pimgSel);
		// Whether edit this image as color? (BT4911)
		IUnknown *punkCC;
		imgSel->GetColorConvertor( &punkCC );
		IColorConvertor6Ptr sptrCC = punkCC;
		punkCC->Release();
		int nColorMode;
		sptrCC->GetEditMode(imgSel, &nColorMode);
		COLORREF	dwColorBack = ::GetValueColor( GetAppUnknown(), "\\Image", "Background", RGB( 255, 255, 255 ) );
		if (nColorMode != 1)
			dwColorBack = Bright(GetBValue(dwColorBack), GetGValue(dwColorBack), GetRValue(dwColorBack));
//		DWORD dwColorBack = ::GetValueColor(GetAppUnknown(), "\\Editor",
//			nColorMode==1?"Back":"Back gray", RGB(0,0,0));
		CopyPropertyBag(pimg, punkImgNew, true);
		// Create image
		int cx = imgSel.GetWidth();
		int cy = imgSel.GetHeight();
		CString sCC = imgSel.GetColorConvertor();
		imgNew.CreateNew(cx, cy, sCC, imgSel.GetColorsCount());
		// Copy image data
		int nPanes = imgSel.GetColorsCount();
		for (int k = 0; k < nPanes; k++)
		{
			color nColorComp = ByteAsColor((byte)(dwColorBack>>(k*8)));
			for (int i = 0; i < cy; i++)
			{
				color *pDst = imgNew.GetRow(i, k);
				color *pSrc = imgSel.GetRow(i, k);
				byte  *pMsk = imgSel.GetRowMask(i);
				for (int j = 0; j < cx; j++)
				{
					if (pMsk[j] == 255)
						pDst[j] = pSrc[j];
					else
						pDst[j] = nColorComp;
				}
			}
		}
		CopyObjectNamedData(pimg, punkImgNew, _T(""), true);
		// Set name same as old object's name.
		CString str_name = ::GetObjectName( pimg );
		::SetObjectName( punkImgNew, str_name );
	}
	return punkImgNew;
}

static IUnknownPtr CropBinaryBySel(IUnknown *pimgBin, IUnknown *pimgSel)
{
	IUnknownPtr punkImgBinNew(::CreateTypedObject("Binary"), false);
	INamedDataObject2Ptr sptrNDO2Old(pimgBin);
	INamedDataObject2Ptr sptrNDO2NewObj(punkImgBinNew);
	GUID guidBaseKey;
	sptrNDO2Old->GetBaseKey(&guidBaseKey);
	sptrNDO2NewObj->SetBaseKey(&guidBaseKey);
	if (punkImgBinNew != 0)
	{
		// Get binary image sizes and rectangle in base image's coordinates.
		CBinImageWrp binNew(punkImgBinNew);
		CBinImageWrp binOld(pimgBin);
		CPoint ptBinOffset;
		binOld->GetOffset(&ptBinOffset);
		CSize szBin;
		int cxBin,cyBin;
		binOld->GetSizes(&cxBin, &cyBin);
		CRect rcBin(ptBinOffset, CSize(cxBin, cyBin));
		// Get selection sizes and rectangle in base image's coordinates.
		CImageWrp  imgSel(pimgSel);
		CPoint ptSelOffset = imgSel.GetOffset();
		int cx = imgSel.GetWidth();
		int cy = imgSel.GetHeight();
		CRect rcSel(ptSelOffset, CSize(cx,cy));
		// Make rectangle to copy data from binary (in base image coordinates !).
		CRect rcDest(max(rcBin.left,rcSel.left), max(rcBin.top, rcSel.top),
			min(rcBin.right, rcSel.right), min(rcBin.bottom, rcSel.bottom));
		CopyPropertyBag(pimgBin, punkImgBinNew, true);
		// Create image
		binNew->CreateNew(cx, cy);
		// Copy binary data
		for (int y = rcDest.top; y < rcDest.bottom; y++)
		{
			byte *pDst,*pSrc;
			binNew->GetRow(&pDst, y-rcSel.top, TRUE);
			binOld->GetRow(&pSrc, y-rcBin.top, TRUE);
			byte  *pMsk = imgSel.GetRowMask(y-rcSel.top);
			for (int x = rcDest.left; x < rcDest.right; x++)
			{
				if (pMsk[x-rcSel.left] == 255)
					pDst[x-rcSel.left] = pSrc[x-rcBin.left];
				else
					pDst[x-rcSel.left] = 0;
			}
		}
		CopyObjectNamedData(pimgBin, punkImgBinNew, _T(""), true);
		// Set name same as old object's name.
		CString str_name = ::GetObjectName( pimgBin );
		::SetObjectName( punkImgBinNew, str_name );
	}
	return punkImgBinNew;
}

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
{
	if( in == 0 || out == 0)
		return;

	long lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer	*pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

static bool TestRectanleInObject(CRect rcSel, CImageWrp &imgSel, CRect rcObj)
{
	CRect rcOver(max(rcObj.left,rcSel.left),max(rcObj.top,rcSel.top),min(rcObj.right,rcSel.right),
		min(rcObj.bottom,rcSel.bottom)); // Overlapped rectangle in old base image coordinates
	for (int y = rcOver.top; y < rcOver.bottom; y++)
	{
		byte *pmaskSel = imgSel.GetRowMask(y-rcSel.top);
		for (int x = rcOver.left; x < rcOver.right; x++)
		{
			if (pmaskSel[x-rcSel.left] == 0)
				return false;
		}
	}
	return true;
}


static IUnknownPtr CropObjectsListBySel(IUnknown *pimgOLOld, IUnknown *pimgSel, IUnknown *punkBaseImage)
{
	CObjectListWrp objectsIn(pimgOLOld);
	CImageWrp imgBase(punkBaseImage);
	// Make new object list
	IUnknownPtr punkObjectOut(::CreateTypedObject(szTypeObjectList), false);
	if (punkObjectOut == 0)
		return punkObjectOut;
	ICompositeObjectPtr cOld(objectsIn);
	long bComposite =0;
	if(cOld) cOld->IsComposite(&bComposite);
	CObjectListWrp objectsOut(punkObjectOut);
	// Set base key
	INamedDataObject2Ptr sptrNDO2Old(objectsIn);
	INamedDataObject2Ptr sptrNDO2NewObj(punkObjectOut);

	sptrNDO2NewObj->InitAttachedData();
	INamedDataPtr ndNew= sptrNDO2NewObj;
	INamedDataPtr ndOld= sptrNDO2Old;

	bool bPhased =false;
	variant_t var;
	bstr_t bstrPhases("Phases");
	ndOld->GetValue( bstrPhases, &var );
	ndNew->SetValue(bstrPhases, var );
	 
	GUID guidBaseKey;
	sptrNDO2Old->GetBaseKey(&guidBaseKey);
	sptrNDO2NewObj->SetBaseKey(&guidBaseKey);
	// Initialize OL
	CopyParams(objectsIn, objectsOut);
	CMeasureContext MeasCtx(true, objectsOut);
	// Get selection sizes and rectangle in base image's coordinates.
	CImageWrp  imgSel(pimgSel);
	CPoint ptSelOffset = imgSel.GetOffset();
	int cx = imgSel.GetWidth();
	int cy = imgSel.GetHeight();
	CRect rcSel(ptSelOffset, CSize(cx,cy));
	// Copy object inside selection into new object list.
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		// Get object.
		IUnknownPtr punkObj(objectsIn.GetNextObject(pos), false);
		ICalcObjectPtr sptrCalc(punkObj);
		IMeasureObjectPtr sptrMeas(punkObj);
		if(sptrCalc == 0)
			continue;
		// Make new objects
		CListLockUpdate lock(objectsOut);
		ICalcObjectPtr sptrCalcNew;
		// Get object image.
		IUnknownPtr punkImageOld;
		sptrMeas->GetImage(&punkImageOld);
		CImageWrp imgOld(punkImageOld);

		CImageWrp imgNew;
		bool bExist = false;
		if (punkImageOld == 0)
		{
			// Light object. New object alse will be light.
			CRect rcObj = GetObjectRectangle(punkObj, 1);
			if (rcObj.left < rcSel.left || rcObj.top < rcSel.top || rcObj.right > rcSel.right ||
				rcObj.bottom > rcSel.bottom)
				continue; // object is outside selection rectangle
			if (!TestRectanleInObject(rcSel, imgSel, rcObj))
				continue; // object is in selection rectangle, but outside selection
			IClonableObjectPtr sptrClone(punkObj);
			IUnknownPtr punkObjNew;
			sptrClone->Clone(&punkObjNew);
			if (punkObjNew != 0)
			{
				// Offset all manual measure objects.
				sptrCalcNew = punkObjNew ;
				INamedDataObject2Ptr sptrNDO2Obj(punkObjNew );
				long	lpos = 0;
				sptrNDO2Obj->GetFirstChildPosition(&lpos);
				while (lpos)
				{
					IUnknownPtr punkChild;
					sptrNDO2Obj->GetNextChild(&lpos, &punkChild);
					IManualMeasureObjectPtr sptrMMObj(punkChild);
					if (sptrMMObj != 0)
					{
						sptrMMObj->SetOffset(CPoint(-rcSel.left, -rcSel.top));
					}
				}
				bExist = true;
			}
		}
		else
		{
			// Get object image rect.
			CPoint ptObjOffset = imgOld.GetOffset();
			int cxObj = imgOld.GetWidth();
			int cyObj = imgOld.GetHeight();
			CRect rcObj(ptObjOffset, CSize(cxObj,cyObj));
			if (rcObj.right <= rcSel.left || rcObj.bottom <= rcSel.top || rcObj.left >= rcSel.right ||
				rcObj.top >= rcSel.bottom)
				continue; // object is outside selection
			// Object rectangle is overlapping with selection rectangle. Test for real overlapping.
			CRect rcOver(max(rcObj.left,rcSel.left),max(rcObj.top,rcSel.top),min(rcObj.right,rcSel.right),
				min(rcObj.bottom,rcSel.bottom)); // Overlapped rectangle in old base image coordinates
			CRect rcOvrN(rcOver); // Overlapped rectangle in new base image coordinates
			rcOvrN.OffsetRect(-rcSel.left,-rcSel.top);
			// Make new object's image.
			BOOL bVirtual = imgOld.IsVirtual();
			if (bVirtual)
			{
				IUnknownPtr punkNewImage(imgBase.CreateVImage(rcOvrN), false);;
				imgNew = punkNewImage;
			}
			else
			{
				CString sCC = imgOld.GetColorConvertor();
				imgNew.CreateNew(rcOvrN.Width(), rcOvrN.Height(), sCC);
				imgNew.SetOffset(rcOvrN.TopLeft());
				sptrINamedDataObject2	sptrN( imgNew );
				sptrN->SetParent( imgBase, apfNotifyNamedData );
			}
			imgNew.InitRowMasks();
			for (int y = rcOver.top; y < rcOver.bottom; y++)
			{
				byte *pmaskDst = imgNew.GetRowMask(y-rcOver.top);
				byte *pmaskSrc = imgOld.GetRowMask(y-rcObj.top);
				byte *pmaskSel = imgSel.GetRowMask(y-rcSel.top);
				for (int x = rcOver.left; x < rcOver.right; x++)
				{
					pmaskDst[x-rcOver.left] = pmaskSel[x-rcSel.left]<128?0:pmaskSrc[x-rcObj.left];
					if (pmaskDst[x-rcOver.left] >= 128)
						bExist = true;
				}
				if (!bVirtual)
				{
					for (int k = 0; k < imgNew.GetColorsCount(); k++)
					{
						color *pSrc = imgOld.GetRow(y-rcObj.top, k);
						color *pDst = imgNew.GetRow(y-rcOver.top, k);
						for (int x = rcOver.left; x < rcOver.right; x++)
							pDst[x-rcOver.left] = pSrc[x-rcObj.left];
					}
				}
			}
			if (!bExist)
				continue;
			imgNew.InitContours();
			CObjectWrp	objectNew(::CreateTypedObject(szTypeObject), false);
			objectNew.SetImage(imgNew);
			sptrCalcNew = objectNew;
			// Copy also manual measures
			INamedDataObject2Ptr sptrNDO2Obj(sptrCalc);
			long	lpos = 0;
			sptrNDO2Obj->GetFirstChildPosition(&lpos);
			while (lpos)
			{
				IUnknownPtr punkChild;
				sptrNDO2Obj->GetNextChild(&lpos, &punkChild);
				IManualMeasureObjectPtr sptrMMObj(punkChild);
				if (sptrMMObj != 0)
				{
					CRect rcMMObj = ::GetObjectRectangle(punkChild, 0);
					if (TestRectanleInObject(rcSel, imgSel, rcMMObj))
					{
						IClonableObjectPtr sptrCloned(punkChild);
						IUnknownPtr punkNewChild;
						sptrCloned->Clone(&punkNewChild);
						INamedDataObject2Ptr ptrNDO2NewChild(punkNewChild);
						IManualMeasureObjectPtr sptrNewMMObj(punkNewChild);
						if (sptrNewMMObj != 0)
							sptrNewMMObj->SetOffset(CPoint(-rcSel.left, -rcSel.top));
						if (ptrNDO2NewChild != 0)
							ptrNDO2NewChild->SetParent(sptrCalcNew, 0);
					}
				}
			}			

		}
		INamedDataObject2Ptr ptrObject(sptrCalcNew);
		if (ptrObject != 0) ptrObject->SetParent(objectsOut, 0);
		int nClass = get_object_class(sptrCalc);
		set_object_class(sptrCalcNew, nClass);
		if(bComposite)
		{
			ICompositeSupportPtr cs(sptrCalc);
			if(cs)
			{
				cs->CopyAllCompositeData(sptrCalcNew);
			}
		}
		MeasCtx.Calc(sptrCalcNew,punkImageOld == 0 ? NULL : imgNew);
		if(!imgOld.IsCountored())
			imgNew.ResetContours();
		
	}
	MeasCtx.DeInit();
//Composite Support
	ICompositeObjectPtr cNew(objectsOut);
	if(bComposite && cNew!=0)
		{
			cNew->SetCompositeFlag();
			long binCount;
			cOld->GetPlaneCount(&binCount);
			cNew->SetPlaneCount(binCount);
			cNew->RestoreTreeByGUIDs();
			cNew->CalculateComposite();
		}
//CS
	return punkObjectOut;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CActionDocBySel, CCmdTargetEx);

// {8C34630D-6CA5-4b25-8DD7-151747BEA6B5}
GUARD_IMPLEMENT_OLECREATE(CActionDocBySel, "FiltersMain.DocBySel", 
0x8c34630d, 0x6ca5, 0x4b25, 0x8d, 0xd7, 0x15, 0x17, 0x47, 0xbe, 0xa6, 0xb5);

// {1169DE19-B814-4239-9A98-8A5A0DBA31E8}
static const GUID guidDocBySel = 
{ 0x1169de19, 0xb814, 0x4239, { 0x9a, 0x98, 0x8a, 0x5a, 0xd, 0xba, 0x31, 0xe8 } };

ACTION_INFO_FULL(CActionDocBySel, IDS_ACTION_DOC_BY_SEL, -1, -1, guidDocBySel);

ACTION_TARGET(CActionDocBySel, szTargetAnydoc);

ACTION_ARG_LIST(CActionDocBySel)
//	ARG_BOOL(_T("RestoreSel"), 0)
END_ACTION_ARG_LIST();


CActionDocBySel::CActionDocBySel()
{
}

CActionDocBySel::~CActionDocBySel()
{
}

bool CActionDocBySel::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionDocBySel::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionDocBySel::IsAvaible()
{
	sptrIDocumentSite sptrDS(m_punkTarget);
	if( sptrDS == 0 )
		return false;
	IUnknownPtr punkView;
	sptrDS->GetActiveView( &punkView );
	if( punkView == 0 )
		return false;
	IUnknownPtr punkImageSel,punkImage;
	if( GetActiveImage(punkView, &punkImage, &punkImageSel) )
		return punkImageSel != 0;
	return false;
}

bool CActionDocBySel::Invoke()
{
	sptrIDocumentSite sptrDS(m_punkTarget);
	if( sptrDS == 0 )
		return false;
	IUnknownPtr punkView;
	sptrDS->GetActiveView( &punkView );
	if( punkView == 0 )
		return false;
	IUnknownPtr punkImageSel,punkImage;
	if( !GetActiveImage(punkView, &punkImage, &punkImageSel) || punkImageSel == 0 )
		return false;
	IUnknownPtr punkBinOld(GetActiveObjectFromContext(punkView, szTypeBinaryImage), false);
	IUnknownPtr punkObjectsOld(GetActiveObjectFromContext(punkView, szTypeObjectList), false);

	m_changes.RemoveFromDocData( m_punkTarget, punkImageSel );
	IUnknownPtr punkImgNew = CropImageBySel(punkImage, punkImageSel);
	if (punkImgNew != 0)
	{
		m_changes.RemoveFromDocData( m_punkTarget, punkImage );
		m_changes.SetToDocData( m_punkTarget, punkImgNew, UF_NOT_GENERATE_UNIQ_NAME );
	}
	if (punkBinOld != 0)
	{
		IUnknownPtr punkBinNew = CropBinaryBySel(punkBinOld, punkImageSel);
		if (punkBinNew != 0)
		{
			m_changes.RemoveFromDocData( m_punkTarget, punkBinOld );
			m_changes.SetToDocData( m_punkTarget, punkBinNew, UF_NOT_GENERATE_UNIQ_NAME );
		}
	}
	if (punkObjectsOld != 0)
	{
		IUnknownPtr punkObjectsNew = CropObjectsListBySel(punkObjectsOld, punkImageSel, punkImgNew);
		if (punkObjectsNew != 0)
		{
			m_changes.RemoveFromDocData( m_punkTarget, punkObjectsOld );
			m_changes.SetToDocData( m_punkTarget, punkObjectsNew, UF_NOT_GENERATE_UNIQ_NAME );
		}
	}
	return true;
}


