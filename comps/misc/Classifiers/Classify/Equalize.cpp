#include "StdAfx.h"
#include "Equalize.h"
#include "nameconsts.h"
#include "data5.h"
#include "DocView5.h"
#include "Classes5.h"
#include <math.h>
#include "Chromosome.h"
#include "../../Chromo/ChromosEx/consts.h"
#include "CarioInt.h"
#include "MessageException.h"

_bstr_t _GetCCName(IImage *pimg1);
int _GetPanesCount(IImage *pimg1);

class CEqualize
{
public:
	_array_t<double> m_BorderValues;
	int m_nPanes;
	_bstr_t m_bstrCC;
	int m_nHeight,m_nWidth;
	bool m_bContours;
	IImage3Ptr m_pImgOld;
	POINT m_ptOffs;
	CEqualize(IImage3 *pImgOld, bool bNotEqualize = false)
	{
		CalcBorderInfoValues(pImgOld,bNotEqualize);
	}
	bool IsContours(IImage3 *pImgOld)
	{
		int nContours;
		pImgOld->GetContoursCount(&nContours);
		return nContours > 0;
	}
	void CalcBorderInfoValues(IImage3 *pImgOld, bool bNotEqualize)
	{
		// Sizes of images, offset and row mask will be copied from old image
		pImgOld->GetOffset(&m_ptOffs);
		m_bstrCC = _GetCCName(pImgOld);
		m_nPanes = _GetPanesCount(pImgOld);
		pImgOld->GetSize(&m_nWidth,&m_nHeight);
		m_pImgOld = pImgOld;
		m_bContours = bNotEqualize?false:IsContours(pImgOld);
		if (!m_bContours)
			return;
		m_BorderValues.alloc(m_nPanes);
		for (int nPane = 0; nPane < m_nPanes; nPane++)
			m_BorderValues[nPane] = 0.;
		Contour *pCntr;
		pImgOld->GetContour(0, &pCntr);
		int nPoints = 0;
		for (int i = 0; i < pCntr->nContourSize; i++)
		{
			ContourPoint pt = pCntr->ppoints[i];
			pt.x = (short)(pt.x-m_ptOffs.x);
			pt.y = (short)(pt.y-m_ptOffs.y);
			for (int nPane = 0; nPane < m_nPanes; nPane++)
			{
				color *pdata;
				pImgOld->GetRow(pt.y, nPane, &pdata);
				if (pt.y >= 0 && pt.y < m_nHeight && pt.x >= 0 && pt.x < m_nWidth)
				{
					m_BorderValues[nPane] = m_BorderValues[nPane]+pdata[pt.x];
					nPoints++;
				}
			}
		}
		if (nPoints > 0)
			for (int nPane = 0; nPane < m_nPanes; nPane++)
			{
				m_BorderValues[nPane] /= nPoints;
				if (m_BorderValues[nPane] < 0.)
					m_BorderValues[nPane] = 0.;
				if (m_BorderValues[nPane] > 0xFFFF)
					m_BorderValues[nPane] = 0xFFFF;
			}
	}
	void Equalize(IImage3 *pImgNew, IImage3 *pImgOld, int nPane)
	{
		int y,x;
		long i;
		const long lHistSize = 0x10000;
		DWORD dwHist[lHistSize];
		DWORD dwVmin, dwHistSum;
		color bBufTabl[lHistSize];
		DWORD dwPoints = 1;
		memset(&dwHist[0], 0, lHistSize*sizeof(DWORD) );
		memset(&bBufTabl[0], 0, lHistSize*sizeof(color));
		for (y = 0; y < m_nHeight; y++)
		{
			color *psrc;
			pImgOld->GetRow(y, nPane, &psrc);
			BYTE *psrcmsk;
			pImgOld->GetRowMask(y, &psrcmsk);
			for (x = 0; x < m_nWidth; x++)
			{
				if (psrcmsk[x])
				{
					dwHist[psrc[x]]++;
					dwPoints++;
				}
			}
		}
		long nMax = 0xFFFF;
		long nMin = 0;
		DWORD b = 0;
		for (i = 0; i < lHistSize; i++)
		{
			nMin = i;
			b += dwHist[i];
			if (b > dwPoints/50)
				break;
		}
		b = 0;
		for (i = lHistSize-1; i >= 0; i++)
		{
			nMax = i;
			b += dwHist[i];
			if (b > dwPoints/50)
				break;
		}
		dwVmin = dwHist[0];
		for (i = 0; i < lHistSize; i++)
		{
			if (dwHist[i] > 0)
			{
				dwVmin = dwHist[i];
				break;
			}
		}
		bBufTabl[0] = 0;
		dwHistSum = dwHist[0];
		for (i = 1; i < lHistSize; i++)
		{
			dwHistSum += dwHist[i];
			bBufTabl[i] = (color)((dwHistSum - dwVmin)*(lHistSize - 1)/(dwPoints - dwVmin));
		}
		DWORD pix;
		if (nMax-m_BorderValues[nPane]<m_BorderValues[nPane]-nMin)
		{
			for (y = 0; y < m_nHeight; y++)
			{
				color *psrc,*pdst;
				pImgOld->GetRow(y, nPane, &psrc);
				pImgNew->GetRow(y, nPane, &pdst);
				BYTE *psrcmsk;
				pImgOld->GetRowMask(y, &psrcmsk);
				for (x = 0; x < m_nWidth; x++)
				{
//					if (psrcmsk[x])
					{
						pix = (DWORD)(sqrt((double)bBufTabl[psrc[x]])*12.6*16.);
						if (pix >= lHistSize)
							pix = lHistSize-1;
						pdst[x] = (color)pix;
					}
				}
			}
		}
		else
		{
			for (y = 0; y < m_nHeight; y++)
			{
				color *psrc,*pdst;
				pImgOld->GetRow(y, nPane, &psrc);
				pImgNew->GetRow(y, nPane, &pdst);
				BYTE *psrcmsk;
				pImgOld->GetRowMask(y, &psrcmsk);
				for (x = 0; x < m_nWidth; x++)
				{
//					if (psrcmsk[x])
					{
						pix = (DWORD)((lHistSize-1-sqrt((double)(lHistSize-1-bBufTabl[psrc[x]]))
							*15.85*16.)*0.8)+50*lHistSize/256;
						if (pix >= lHistSize)
							pix = lHistSize-1;
						pdst[x] = (color)pix;
					}
				}
			}
		}
	}
	IImage3Ptr ProcessImage()
	{
		// Create new image
		IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t("Image")), false);
		IImage3Ptr pImgNew(punkImgNew);
		pImgNew->CreateImage(m_nWidth, m_nHeight, m_bstrCC, m_nPanes );
		pImgNew->SetOffset(m_ptOffs, FALSE);
		// Copy rows and row masks
		pImgNew->InitRowMasks();
		if (m_bContours)
		{
			for (int nPane = 0; nPane < m_nPanes; nPane++)
				Equalize(pImgNew, m_pImgOld, nPane);
		}
		else
		{
			// Copy image data from old (every pane)
			for (int y = 0; y < m_nHeight; y++)
			{
				for (int nPane = 0; nPane < m_nPanes; nPane++)
				{
					color *psrc,*pdst;
					m_pImgOld->GetRow(y, nPane, &psrc);
					pImgNew->GetRow(y, nPane, &pdst);
					memcpy(pdst,psrc,m_nWidth*sizeof(color));
				}
			}
		}
		for (int y = 0; y < m_nHeight; y++)
		{
			// Copy row mask from old
			BYTE *psrcmsk,*pdstmsk;
			m_pImgOld->GetRowMask(y, &psrcmsk);
			pImgNew->GetRowMask(y, &pdstmsk);
			memcpy(pdstmsk,psrcmsk,m_nWidth*sizeof(BYTE));
		}
		// Contours will be created
		pImgNew->InitContours();
		return pImgNew;
	}
	IImage3Ptr Process(IUnknown *ptr)
	{
		IImage3Ptr pImgNew;
		IMeasureObjectPtr ptrMO(ptr);
		if (ptrMO != 0)
		{
			pImgNew = ProcessImage();
			ptrMO->SetImage(pImgNew);
		}
		return pImgNew;
	}
};

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	TPOS lPos = 0;
	sptrNDOParent->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrChild;
		sptrNDOParent->GetNextChild(&lPos, &sptrChild);
		if (bool(sptrChild))
		{
			if (::CheckInterface(sptrChild, guid))
				return sptrChild;
		}
	}
	return NULL;
}

IChromosomePtr CreateChildObjects(IUnknown *punkCalcObject)
{
	IChromosomePtr sptr;
	// Check whether the object already exists
	IUnknownPtr ptrObj = punkCalcObject;
	IUnknownPtr ptrFound = find_child_by_interface(ptrObj, IID_IChromosome);
	bool bFound = ptrFound;
	if (bFound)
	{
		sptr = ptrFound;
		return sptr;
	}
	// Create child objects and add it to chromosome
	IUnknown *punk1 = CreateTypedObject(_bstr_t("ChromosomeAxisAndCentromere"));
	sptr =  punk1;
	sptrINamedDataObject2 sptrNO1 = punk1;
	punk1->Release();
	sptrNO1->SetParent(punkCalcObject, 0);
	return sptr;
};

bool _IsObjectEqualized(IUnknown *ptr)
{
	IUnknownPtr punkChr = find_child_by_interface(IUnknownPtr(ptr), IID_IChromosome);
	if (punkChr == 0)
		return false;
	IChromosomePtr sptrChr(punkChr);
	if (sptrChr != 0)
	{
		IUnknownPtr punkImgEqu;
		sptrChr->GetEqualizedImage(FALSE, &punkImgEqu);
		if (punkImgEqu != 0)
			return true;
	}
	return false;
}

void _EqualizeObject(IUnknown *ptr)
{
	// Equalize common image
	IMeasureObjectPtr ptrMO(ptr);
	if (ptrMO != 0)
	{
		IUnknownPtr punkImg;
		ptrMO->GetImage(&punkImg);
		IImage3Ptr sptrImg1(punkImg);
		if (sptrImg1 != 0)
		{
			CEqualize equ(sptrImg1);
			IImage3Ptr pImgNew = equ.Process(ptr);
			// Reset equalized rotated image (will be obtained from equalized by need).
			IUnknownPtr punkChr = CreateChildObjects(ptr);
			IChromosomePtr sptrChr(punkChr);
			if (sptrChr != 0)
			{
				sptrChr->SetOriginalImage(sptrImg1);
				sptrChr->SetRotatedImage(NULL);
				sptrChr->SetEqualizedImage(pImgNew);
				sptrChr->SetEqualizedRotatedImage(NULL);
			}
		}
	}
}

void _CopyImage(IUnknown *ptr)
{
	// Equalize common image
	IMeasureObjectPtr ptrMO(ptr);
	if (ptrMO != 0)
	{
		IUnknownPtr punkImg;
		ptrMO->GetImage(&punkImg);
		IImage3Ptr sptrImg1(punkImg);
		if (sptrImg1 != 0)
		{
			CEqualize equ(sptrImg1,true);
			equ.Process(ptr);
		}
	}
}

void _UnequalizeObject(IUnknown *ptr)
{
	// Unequalize common image
	IMeasureObjectPtr ptrMO(ptr);
	if (ptrMO != 0)
	{
		IUnknownPtr punkChr = CreateChildObjects(ptr);
		IChromosomePtr sptrChr(punkChr);
		if (sptrChr)
		{
			IUnknownPtr punkImg;
			ptrMO->GetImage(&punkImg);
			IImage2Ptr sptrImgCur(punkImg);
			_point pt(0,0);
			if (sptrImgCur != 0) sptrImgCur->GetOffset(&pt);
			sptrChr->GetOriginalImage(&punkImg);
			IImage2Ptr sptrImgOrg(punkImg);
			if (sptrImgOrg != 0) sptrImgOrg->SetOffset(pt, FALSE);
			ptrMO->SetImage(punkImg);
			sptrChr->SetRotatedImage(NULL);
			sptrChr->SetEqualizedImage(NULL);
			sptrChr->SetEqualizedRotatedImage(NULL);
		}
	}
}


static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
{
	if( in == 0 || out == 0)
		return;

	LONG_PTR lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer	*pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionChromoEqualize
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1
_ainfo_base::arg	CActionChromoEqualizeInfo::s_pargs[] = 
{
	{"Name", szArgumentTypeString, 0, true, true },	
	{0, 0, 0, false, false },
};

CActionChromoEqualize::CActionChromoEqualize()
{
}

bool CActionChromoEqualize::Process(IUnknown *punkObjList, bool bEqualize)
{
	sptrINamedDataObject2 sptrObjIn(punkObjList);
	TPOS pos = 0; long i = 0;
	sptrObjIn->GetFirstChildPosition(&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrObjIn->GetNextChild(&pos, &sptr);
		if (bEqualize)
		{
			if (!_IsObjectEqualized(sptr))
				_EqualizeObject(sptr);
		}
		else
		{
			if (_IsObjectEqualized(sptr))
				_UnequalizeObject(sptr);
		}
	}
	// Now all OK. Redraw document.
	INamedDataObject2Ptr NDO(m_punkObjList);
	IUnknown *punk = NULL;
	HRESULT hr = NDO->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		INotifyControllerPtr sptr(punk);
		sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
		punk->Release();
	}
	return true;
}



HRESULT CActionChromoEqualize::DoInvoke()
{
	try
	{
		// Find object list
		_bstr_t bstrName = GetArgString("Name");
		if (bstrName.length() == 0)
			m_punkObjList = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
		else
			m_punkObjList = IUnknownPtr(GetObjectByName(m_ptrTarget, bstrName,
				_bstr_t(szArgumentTypeObjectList)),false);
		Process(m_punkObjList,true);
		SetModified(true);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED);
	}
	return S_OK;
}


HRESULT CActionChromoEqualize::DoUndo()
{
	Process(m_punkObjList, false);
	SetModified(false);
	return S_OK;
}

HRESULT CActionChromoEqualize::DoRedo()
{
	Process(m_punkObjList, true);
	SetModified(true);
	return S_OK;
}

IUnknown *CActionChromoEqualize::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}


HRESULT CActionChromoEqualize::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;
	IUnknownPtr punk(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
	if (punk != 0)
	{
		sptrINamedDataObject2 sptrNDO2ObjList(punk);
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
		{
			IDocumentSitePtr ptrDocSite(m_ptrTarget);
			IUnknownPtr punkView;
			ptrDocSite->GetActiveView(&punkView);
			IMeasureViewPtr sptrMeasView(punkView);
			if (sptrMeasView == 0)
				return CAction::GetActionState(pdwState);
		}
	}
	return S_OK;
}
#else
_ainfo_base::arg	CActionChromoEqualizeInfo::s_pargs[] = 
{
	{"ObjOld",	szArgumentTypeObjectList, 0, true, true },	
	{"Obj",	szArgumentTypeObjectList, 0, false, true },	
	{0, 0, 0, false, false },
};

CActionChromoEqualize::CActionChromoEqualize()
{
}

bool CActionChromoEqualize::InvokeFilter()
{
	sptrINamedDataObject2 sptrObjIn(GetDataArgument("ObjOld")); // Object list
	sptrINamedDataObject2 sptrObjOut(GetDataResult()); // Object list
	CopyParams(sptrObjIn, sptrObjOut);
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
			if (_IsObjectEqualized(ptr))
				_CopyImage(ptr);
			else
				_EqualizeObject(ptr);
			INamedDataObject2Ptr ptrObject(ptr); 
			if(ptrObject != 0)
				ptrObject->SetParent(sptrObjOut, 0);
		}
		Notify(i++);
	}
	FinishNotification();
	SetModified(true);
	return true;
}

static void __ResetObjList(IUnknown *punkTarget, IUnknown *punkList)
{
	IDocumentSitePtr ptrDocSite(punkTarget);
	IUnknownPtr punkView;
	ptrDocSite->GetActiveView(&punkView);
	if (punkView != 0)
	{
		sptrIDataContext sptrDC(punkView);
		sptrDC->SetActiveObject(_bstr_t(szArgumentTypeObjectList), punkList, 0);
	}
}

HRESULT CActionChromoEqualize::DoInvoke()
{
	HRESULT hr = CFilter::DoInvoke();
	sptrINamedDataObject2 sptrObjOut(GetDataResult()); // Object list
	__ResetObjList(m_ptrTarget, sptrObjOut);
/*	IDocumentSitePtr ptrDocSite(m_ptrTarget);
	IUnknownPtr punkView;
	ptrDocSite->GetActiveView(&punkView);
	if (punkView != 0)
	{
		sptrIDataContext sptrDC(punkView);
		sptrDC->SetActiveObject(_bstr_t(szArgumentTypeObjectList), sptrObjOut, 0);
	}*/
	return hr;
}

HRESULT CActionChromoEqualize::DoUndo()
{
	HRESULT hr = CFilter::DoUndo();
	sptrINamedDataObject2 sptrObjIn(GetDataArgument("ObjOld")); // Object list
	__ResetObjList(m_ptrTarget, sptrObjIn);
	/*IDocumentSitePtr ptrDocSite(m_ptrTarget);
	IUnknownPtr punkView;
	ptrDocSite->GetActiveView(&punkView);
	if (punkView != 0)
	{
		sptrIDataContext sptrDC(punkView);
		sptrDC->SetActiveObject(_bstr_t(szArgumentTypeObjectList), sptrObjIn, 0);
	}*/
	return hr;
}

HRESULT CActionChromoEqualize::DoRedo()
{
	HRESULT hr = CFilter::DoRedo();
	sptrINamedDataObject2 sptrObjOut(GetDataResult()); // Object list
	__ResetObjList(m_ptrTarget, sptrObjOut);
	/*IDocumentSitePtr ptrDocSite(m_ptrTarget);
	IUnknownPtr punkView;
	ptrDocSite->GetActiveView(&punkView);
	if (punkView != 0)
	{
		sptrIDataContext sptrDC(punkView);
		sptrDC->SetActiveObject(_bstr_t(szArgumentTypeObjectList), sptrObjOut, 0);
	}*/
	return hr;
}

bool CActionChromoEqualize::DoResetData()
{
	if( m_bLockChangeData )
		return true;

	long	pos = m_arguments.head();

	while( pos )
	{
		CFilterArgument *pa = m_arguments.next( pos );

		if( !pa->m_punkArg )
			continue;

		if( pa->m_bVirtual && pa->m_bInArgument)
		{
			IUnknown *punk = ::GetObjectByKey( m_ptrTarget, pa->m_lParentKey );
			sptrINamedDataObject2	sptr( pa->m_punkArg );
			sptr->SetParent( punk, sdfAttachParentData|sdfCopyLocalData|apfNotifyNamedData );

			_variant_t	var( pa->m_punkArg );
			::SetValue( m_ptrTarget, 0, 0, var );

			if( punk )
				punk->Release();
		}
	}
	return true;
}

HRESULT CActionChromoEqualize::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;
	IUnknownPtr punk(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
	if (punk != 0)
	{
		sptrINamedDataObject2 sptrNDO2ObjList(punk);
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
		{
			IDocumentSitePtr ptrDocSite(m_ptrTarget);
			IUnknownPtr punkView;
			ptrDocSite->GetActiveView(&punkView);
			IMeasureViewPtr sptrMeasView(punkView);
			if (sptrMeasView == 0)
				return CFilter::GetActionState(pdwState);
		}
	}
	return S_OK;
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionChromoEqualizeObj
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionChromoEqualizeObjInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CActionChromoEqualizeObj::CActionChromoEqualizeObj()
{
	m_hcurDefault = LoadCursor(app::handle(), MAKEINTRESOURCE(IDC_EQUALIZE));
}

IUnknown *CActionChromoEqualizeObj::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}


bool CActionChromoEqualizeObj::DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList )
{
	if (!_IsObjectEqualized(punkObj))
	{
		_EqualizeObject(punkObj);
		m_ptrObj = punkObj;
		SetModified(true);
	}
	return true;
}

HRESULT CActionChromoEqualizeObj::DoUndo()
{
	if (m_ptrObj != 0)
	{
		_UnequalizeObject(m_ptrObj);
		RedrawDocument(m_ptrTarget);
		SetModified(false);
	}
	return S_OK;
}

HRESULT CActionChromoEqualizeObj::DoRedo()
{
	if (m_ptrObj != 0)
	{
		_EqualizeObject(m_ptrObj);
		RedrawDocument(m_ptrTarget);
		SetModified(true);
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionChromoUnEqualizeObj
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionChromoUnEqualizeObjInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CActionChromoUnEqualizeObj::CActionChromoUnEqualizeObj()
{
	m_hcurDefault = LoadCursor(app::handle(), MAKEINTRESOURCE(IDC_UNEQUALIZE));
}

IUnknown *CActionChromoUnEqualizeObj::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}


bool CActionChromoUnEqualizeObj::DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList )
{
	if (_IsObjectEqualized(punkObj))
	{
		_UnequalizeObject(punkObj);
		m_ptrObj = punkObj;
		SetModified(true);
	}
	return true;
}

HRESULT CActionChromoUnEqualizeObj::DoUndo()
{
	if (m_ptrObj != 0)
	{
		_EqualizeObject(m_ptrObj);
		RedrawDocument(m_ptrTarget);
		SetModified(false);
	}
	return S_OK;
}

HRESULT CActionChromoUnEqualizeObj::DoRedo()
{
	if (m_ptrObj != 0)
	{
		_UnequalizeObject(m_ptrObj);
		RedrawDocument(m_ptrTarget);
		SetModified(true);
	}
	return S_OK;
}

HRESULT CActionChromoUnEqualizeObj::GetActionState(DWORD *pdwState)
{
	HRESULT hr = CObjIntActionBase::GetActionState(pdwState);
	if (SUCCEEDED(hr) && (*pdwState&afEnabled))
	{
		IUnknownPtr punk(::GetActiveObjectFromContext(m_ptrTarget, szTypeObjectList), false);
		sptrINamedDataObject2 sptrObjIn(punk);
		bool bFound = false;
		TPOS pos = 0;
		if( sptrObjIn )//paul
		{
			sptrObjIn->GetFirstChildPosition(&pos);
			while (pos)
			{
				IUnknownPtr sptr;
				sptrObjIn->GetNextChild(&pos, &sptr);
				if (_IsObjectEqualized(sptr))
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				*pdwState = 0;
		}
	}
	return hr;
};
