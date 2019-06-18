#include "stdafx.h"
#include "sewLI_object.h"
#include "\vt5\ifaces\sewLI_int.h"
#include "SewUtils.h"
#include "atltypes.h"
#include "clone.h"
#include "cloneData.h"
#include "ScriptNotifyInt.h"
#include "FindPos.h"
#include <shlobj.h>
#include <direct.h>
#include <math.h>
#include <\vt5\awin\trace.h>

static BOOL SerializeBSTR( BSTR* pbstr, IStream &ar, bool bIsStoring )
{
	if( !pbstr )
		return false;

	if( bIsStoring )
	{
		ULONG len1=0;
		UINT uiLen = SysStringByteLen( *pbstr );
		ar.Write(&uiLen, sizeof(uiLen), &len1);
		if(len1!=sizeof(uiLen)) return false;
		if( uiLen > 0 )
		{
			ar.Write( (BYTE*)(*pbstr), uiLen, &len1 );
			if(len1!=uiLen) return false;
		}
	}
	else
	{
		*pbstr = 0;
		ULONG len1=0;
		UINT uiLen = 0;
		ar.Read(&uiLen, sizeof(uiLen), &len1);
		if(len1!=sizeof(uiLen)) return false;
		if( uiLen > 0 )
		{	
			static BYTE buf_stat[255];
			if( uiLen < sizeof(buf_stat) )
			{
				ar.Read( buf_stat, uiLen, &len1 );
				*pbstr = SysAllocStringByteLen( (LPCSTR)buf_stat, uiLen );
			}
			else
			{
				BYTE *pBuf = new BYTE[uiLen];
				ar.Read( pBuf, uiLen, &len1 );
				*pbstr = SysAllocStringByteLen( (LPCSTR)pBuf, uiLen );
				delete [] pBuf;	pBuf = 0;
			}			
		}
		
	}

	return true;
}

static _bstr_t _GetTempDir()
{
	_bstr_t bstrTemp = GetValueString(GetAppUnknown(), "\\Sew Large Image", "TempPath", "");
	if (bstrTemp.length() == 0)
	{
		TCHAR szTempPath[_MAX_PATH];
		if (GetTempPath(_MAX_PATH, szTempPath) > 0)
		{
			bstrTemp = szTempPath;
			int nLen = bstrTemp.length();
			if (((const wchar_t *)bstrTemp)[nLen-1] != L'\\' && 
				((const wchar_t *)bstrTemp)[nLen-1] != L'/')
				bstrTemp += _T("\\");
			bstrTemp += _T("VT5_SLI");
		}
	}
	if (bstrTemp.length() == 0)
		return bstrTemp;
	int nLen = bstrTemp.length();
	if (((const wchar_t *)bstrTemp)[nLen-1] != L'\\' && 
		((const wchar_t *)bstrTemp)[nLen-1] != L'/')
		bstrTemp += _T("\\");
	return bstrTemp;
}

CFragmentImage::CFragmentImage()
{
	m_nZoom = 0;
	m_nLoadCount = 0;
	m_pFragment = NULL;
	m_bChanged = false;
	m_bSavingImg = false;
}

IUnknownPtr CFragmentImage::RebuildImage(IUnknown *punkImg11)
{
	IImagePtr imgSrc(punkImg11);
	int cx,cy;
	imgSrc->GetSize(&cx,&cy);
	IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	ICompatibleObjectPtr sptrComp(punkImgNew);
	int cx1 = cx/m_nZoom;
	int cy1 = cy/m_nZoom;
	ImageCloneData icd = {0};
	icd.rect = CRect(0,0,cx1,cy1);
	sptrComp->CreateCompatibleObject(punkImg11, &icd, sizeof(icd));
	IImagePtr imgNew(punkImgNew);
	int nPanes = ::GetImagePaneCount(punkImg11);
	for (int y = 0; y < cy1; y++)
	{
		for (int c = 0; c < nPanes; c++)
		{
			color *pRowSrc,*pRowDst;
			imgSrc->GetRow(y*m_nZoom, c, &pRowSrc);
			imgNew->GetRow(y, c, &pRowDst);
			for (int x = 0; x < cx1; x++)
				pRowDst[x] = pRowSrc[x*m_nZoom];
		}
	}
	return punkImgNew;
}

bool CFragmentImage::SaveImage(IUnknown *punkImg)
{
//	message<mc_DebugText>("start saving image\n");
	_bstr_t bstrPath = MakePath();
	if (bstrPath.length() > 0)
	{
		INamedDataObject2Ptr sptrNDO2List(m_pFragment->m_pList);
		IUnknownPtr punkND;
		sptrNDO2List->GetData(&punkND);
		if (!m_bSavingImg)
		{
			CSetBool sb(&m_bSavingImg,true);
			if (!DoSaveImage(bstrPath, punkImg, punkND))
				return false;
		}
	}
	return true;
//	message<mc_DebugText>("Image saved\n");
}

void CFragmentImage::DeleteImage()
{
	_bstr_t bstrPath = MakePath();
	if (bstrPath.length() > 0)
	{
		_bstr_t bstrUndoDir;
		_bstr_t bstrPathUndo = MakePath(&bstrUndoDir);
		if (bstrPathUndo.length() > 0)
		{
			WIN32_FIND_DATA fd;
			HANDLE h = FindFirstFile(bstrUndoDir, &fd);
			if (h == INVALID_HANDLE_VALUE)
				_wmkdir(bstrUndoDir);
			else
			{
				unlink(bstrPathUndo);
				FindClose(h);
			}
			if (!MoveFile(bstrPath, bstrPathUndo))
				unlink(bstrPath);
		}
		else
			unlink(bstrPath);
	}
}

void CFragmentImage::RestoreImage()
{
	_bstr_t bstrPath = MakePath();
	_bstr_t bstrUndoDir;
	_bstr_t bstrPathUndo = MakePath(&bstrUndoDir);
	if (bstrPath.length() > 0 && bstrPathUndo.length() > 0)
		MoveFile(bstrPathUndo, bstrPath);
}


_bstr_t CFragmentImage::MakePath(_bstr_t *pbstrUndo)
{
	_bstr_t bstrExt;
	m_pFragment->m_pList->GetFragmentsFormat(bstrExt.GetAddress());
	if (bstrExt.length() == 0)
		bstrExt = GetValueString(GetAppUnknown(), "\\Sew Large Image", "DefaultFragmentFormat", "jpg");
	TCHAR szBuff[256];
	_stprintf(szBuff, _T("%dz%d.%s"), m_pFragment->m_lFragNum, m_nZoom, (LPCTSTR)bstrExt);
	_bstr_t bstrPath;
	m_pFragment->m_pList->GetPath(bstrPath.GetAddress(), FALSE);
	if (bstrPath.length() > 0)
	{
		if (pbstrUndo)
		{
			*pbstrUndo = bstrPath;
			*pbstrUndo += _bstr_t("Undo");
			bstrPath += _bstr_t("Undo\\");
		}
		bstrPath += _bstr_t(szBuff);
	}
	return bstrPath;
}

bool CFragmentImage::CheckImageValid()
{
	IImagePtr sptrImage(m_ptrImage);
	if (sptrImage != 0)
	{
		int cx,cy;
		sptrImage->GetSize(&cx, &cy);
		SIZE szFrag;
		m_pFragment->GetSize(&szFrag);
		if (cx != szFrag.cx/m_nZoom || cy != szFrag.cy/m_nZoom)
		{
			dbg_assert(false, "");
			return false;
		}
	}
	return true;
}


void CFragmentImage::MakeTransMap()
{
//	BOOL bSmoothTrans = GetValueInt(GetAppUnknown(), "\\Sew Large Image", "SmoothTransition", 1);
	if (m_ptrImage != 0 && (int)g_SmoothTransition)
	{
		IImagePtr sptrImage(m_ptrImage);
		int cx,cy;
		sptrImage->GetSize(&cx, &cy);
		POINT ptCur;
		m_pFragment->GetOffset(&ptCur);
		SIZE szCur;
		m_pFragment->GetSize(&szCur);
		m_DistMap.CreateNew(szCur.cx/m_nZoom,szCur.cy/m_nZoom);
		m_DistMap.AddRect(CRect(0,0,szCur.cx/m_nZoom,szCur.cy/m_nZoom));
	}
}


void CFragmentImage::LoadImage()
{
//	message<mc_DebugText>("Load image 0x%p (%d)", this, m_nLoadCount);
	if (m_nLoadCount++ >= 0)
	{
		if (m_ptrImage == 0)
		{
			bool bLoaded = false;
			_bstr_t bstrPath = MakePath();
			if (bstrPath.length())
			{
				if (!m_bSavingImg)
				{
					CSetBool sb(&m_bSavingImg,true);
					bLoaded = DoLoadImage(bstrPath, &m_ptrImage);
				}
				if (!CheckImageValid())
				{
					m_ptrImage = 0;
					bLoaded = false;
				}
			}
			if (bLoaded)
				MakeTransMap();
			m_bChanged = false;
		}
	}
}

void CFragmentImage::FreeImage()
{
	if (--m_nLoadCount == 0)
	{
		m_ptrImage = 0;
//		m_TranspMap.Free();
		m_DistMap.Destroy();
	}
//	message<mc_DebugText>("Free image 0x%p (%d)", this, m_nLoadCount);
}

CSewLIFragment::CSewLIFragment() :
	m_ptOffset(0,0),
	m_szSize(0,0),
	m_pList(NULL),
	m_bTerminal(true),
	m_bChanged(false),
	m_pImages(NULL),
	m_nImages(0)
//	m_bSavingImg(false)
{
}


CSewLIFragment::~CSewLIFragment()
{
	delete []m_pImages;
}

IUnknown *CSewLIFragment::DoGetInterface( const IID &iid )
{
	if( iid == IID_ISewFragment )return (ISewFragment*)this;
	else return CObjectBase::DoGetInterface( iid );
}

HRESULT CSewLIFragment::Load( IStream *pStream, SerializeParams *pparams )
{
	ULONG nRead = 0;
	pStream->Read( &m_ptOffset.x, sizeof(int), &nRead );
	pStream->Read( &m_ptOffset.y, sizeof(int), &nRead );
	pStream->Read( &m_szSize.cx, sizeof(long), &nRead );
	pStream->Read( &m_szSize.cy, sizeof(long), &nRead );
	pStream->Read( &m_lFragNum, sizeof(long), &nRead );
	return S_OK;
}

HRESULT CSewLIFragment::Store( IStream *pStream, SerializeParams *pparams )
{
	ULONG nWritten = 0;
	pStream->Write( &m_ptOffset.x , sizeof(int), &nWritten );
	pStream->Write( &m_ptOffset.y , sizeof(int), &nWritten );
	pStream->Write( &m_szSize.cx, sizeof(long), &nWritten );
	pStream->Write( &m_szSize.cy, sizeof(long), &nWritten );
	pStream->Write( &m_lFragNum, sizeof(long), &nWritten );
	return S_OK;
}

int CSewLIFragment::FindImage(int nZoom)
{
	for (int i = 0; i < m_nImages; i++)
	{
		if (m_pImages[i].m_nZoom >= nZoom)
			return i;
	}
	return 0;
}

HRESULT CSewLIFragment::GetSize(SIZE *psz)
{
	*psz = m_szSize;
	return S_OK;
}

HRESULT CSewLIFragment::SetSize(SIZE sz)
{
	m_szSize = _size(sz.cx,sz.cy);
	return S_OK;
}

HRESULT CSewLIFragment::GetOffset(POINT *ptOffset)
{
	*ptOffset = m_ptOffset;
	return S_OK;
}

HRESULT CSewLIFragment::SetOffset(POINT ptOffs)
{
	m_ptOffset = ptOffs;
	return S_OK;
}

HRESULT CSewLIFragment::GetImage(int *pnZoom, IUnknown **ppunkImage)
{
	CheckImages();
	int nImage = FindImage(pnZoom==NULL?1:*pnZoom);
	if (m_pImages[nImage].m_ptrImage != 0)
	{
		*ppunkImage = m_pImages[nImage].m_ptrImage;
		(*ppunkImage)->AddRef();
		if (pnZoom != NULL)
			*pnZoom = m_pImages[nImage].m_nZoom;
		return S_OK;
	}
	else
	{
		(*ppunkImage) = 0;
		return S_FALSE;
	}
}

void CSewLIFragment::CheckImages()
{
	if (m_pImages == NULL)
	{
		int nRangeCoef;
		m_pList->GetRangeCoef(&nRangeCoef);
		m_pImages = new CFragmentImage[g_nFragmentImages];
		m_nImages = g_nFragmentImages;
		int iZoom = 1;
		for (int i = 0; i < g_nFragmentImages; i++)
		{
			m_pImages[i].m_pFragment = this;
			m_pImages[i].m_nZoom = iZoom;
			iZoom *= nRangeCoef;
		}
	}
}

HRESULT CSewLIFragment::SetImage(IUnknown *punkImage)
{
	if (punkImage == NULL)
	{
		for (int i = 0; i < m_nImages; i++)
		{
			m_pImages[i].m_ptrImage = 0;
			m_pImages[i].DeleteImage();
		}
		return S_OK;
	}
//	m_bSavingImg = true;
	CheckImages();
	if (m_pImages[0].m_nLoadCount > 0)
	{
		m_pImages[0].m_ptrImage = punkImage;
		m_pImages[0].MakeTransMap();
	}

	if (!m_pImages[0].SaveImage(punkImage))
		return E_FAIL;
	for (int i = 1; i < m_nImages; i++)
	{
		IUnknownPtr punkImgZ = m_pImages[i].RebuildImage(punkImage);
		if (!m_pImages[i].SaveImage(punkImgZ))
			return E_FAIL;
		if (m_pImages[i].m_nLoadCount != 0)
		{
			m_pImages[i].m_ptrImage = punkImgZ;
			m_pImages[i].MakeTransMap();
		}
	}
//	m_bSavingImg = false;
	return S_OK;
}

HRESULT CSewLIFragment::RestoreImages()
{
	CheckImages();
	for (int i = 0; i < m_nImages; i++)
		m_pImages[i].RestoreImage();
	return S_OK;
}


/*HRESULT CSewLIFragment::GetTransparencyMap(int *pnZoom, ITransparencyMap **ppTranspMap)
{
	CheckImages();
	int nImage = FindImage(*pnZoom);
	if (!m_pImages[nImage].m_TranspMap.IsEmpty())
	{
		*ppTranspMap = &m_pImages[nImage].m_TranspMap;
		(*ppTranspMap)->AddRef();
		*pnZoom = m_pImages[nImage].m_nZoom;
		return S_OK;
	}
	else
	{
		(*ppTranspMap) = 0;
		return S_FALSE;
	}
	return S_OK;
}*/

HRESULT CSewLIFragment::GetDistanceMap(int *pnZoom, IDistanceMap **ppDistanceMap)
{
	CheckImages();
	int nImage = FindImage(*pnZoom);
	if (!m_pImages[nImage].m_DistMap.IsEmpty())
	{
		*ppDistanceMap = &m_pImages[nImage].m_DistMap;
		(*ppDistanceMap)->AddRef();
		*pnZoom = m_pImages[nImage].m_nZoom;
		return S_OK;
	}
	else
	{
		(*ppDistanceMap) = 0;
		return S_FALSE;
	}
	return S_OK;
}


/*HRESULT CSewLIFragment::RebuildTransparencyMaps()
{
	for (int i = 0; i < m_nImages; i++)
	{
		if (!m_pImages[i].m_TranspMap.IsEmpty())
			m_pImages[i].MakeTransMap();
	}
	return S_OK;
}*/

static bool s_bLoading = false;

HRESULT CSewLIFragment::LoadImage(int nZoom)
{
//	if (m_bSavingImg)
//		return S_FALSE;
//	CSetBool sbSavingImg(&m_bSavingImg,true);
	CSetBool sbLoading(&s_bLoading,true);
	CheckImages();
	int nImage = FindImage(nZoom);
	m_pImages[nImage].LoadImage();
	return S_OK;
}

HRESULT CSewLIFragment::FreeImage(int nZoom)
{
	CheckImages();
	int nImage = FindImage(nZoom);
	m_pImages[nImage].FreeImage();
	return S_OK;
}

HRESULT CSewLIFragment::SetListPtr(ISewImageList *pList)
{
	m_pList = pList;
	return S_OK;
}

HRESULT CSewLIFragment::SetFragNum(long lNum)
{
	m_lFragNum = lNum;
	return S_OK;
}

HRESULT CSewLIFragment::GetFragNum(long *plNum)
{
	*plNum = m_lFragNum;
	return S_OK;
}


CSewLIFragment *CSewLIFragment::Make()
{
	CSewLIFragment *pFrag = new CSewLIFragment;
	return pFrag;
}

CTotalImage::CTotalImage()
{
}

CTotalImage::~CTotalImage()
{
}

void CTotalImage::Load(IStream *pStream, SerializeParams *pparams)
{
	if (m_ptrImage == 0)
	{
		IUnknownPtr punk(::CreateTypedObject(_bstr_t(szTypeImage)),false);
		m_ptrImage = punk;
	}
	BYTE byImageExist;
	ULONG nRead;
	pStream->Read(&byImageExist, sizeof(byImageExist), &nRead);
	if (byImageExist > 0)
	{
		ISerializableObjectPtr sptrSOImg(m_ptrImage);
		sptrSOImg->Load(pStream,pparams);
	}
}

void CTotalImage::Save(IStream *pStream, SerializeParams *pparams)
{
	BYTE byImageExist = m_ptrImage!= 0;
	ULONG nWritten;
	pStream->Write(&byImageExist, sizeof(byImageExist), &nWritten);
	if (byImageExist)
	{
		ISerializableObjectPtr sptrSOImg(m_ptrImage);
		sptrSOImg->Store(pStream,pparams);
	}
}

bool CTotalImage::IsEmpty()
{
	CSize sz = GetSize();
	return sz.cx<1||sz.cy<1;
}

CSize CTotalImage::GetSize()
{
	if (m_ptrImage == 0)
		return CSize(0,0);
	int cx,cy;
	m_ptrImage->GetSize(&cx,&cy);
	return CSize(cx,cy);
}

IImagePtr CTotalImage::GetImage()
{
	return m_ptrImage;
}

static int _calc_tot_zoom(int nImg, int nThumb)
{
	int nZoom = 1;
	do
	{
		if (nImg <= nThumb)
			break;
		nZoom *= 2;
		nImg /= 2;
	}
	while(1);
	return nZoom;
}

void CTotalImage::Resize(CRect rcCoordSpacePrev, CRect rcCoordSpaceNew)
{
	if (rcCoordSpacePrev.Width() == rcCoordSpaceNew.Width() &&
		rcCoordSpacePrev.Height() == rcCoordSpaceNew.Height())
		return;
	if (rcCoordSpaceNew.IsRectEmpty())
	{
		m_ptrImage = 0;
		return;
	}
	int nZoomX = _calc_tot_zoom(rcCoordSpaceNew.Width(), 256);
	int nZoomY = _calc_tot_zoom(rcCoordSpaceNew.Height(), 256);
	int nZoom = max(nZoomX, nZoomY);
	if (m_ptrImage == 0)
	{
		IUnknownPtr punkNewImage(::CreateTypedObject(_bstr_t(szTypeImage)),false);
		m_ptrImage = punkNewImage;
		if (m_ptrImage == 0)
			return;
		int cx = rcCoordSpaceNew.Width()/nZoom;
		int cy = rcCoordSpaceNew.Height()/nZoom;
		m_ptrImage->CreateImage(cx,cy,_bstr_t("RGB"),-1);
		return;
	}
	int cxPrev = 0,cyPrev = 0;
	m_ptrImage->GetSize(&cxPrev,&cyPrev);
	int nZoomPrev = rcCoordSpacePrev.Width()/cxPrev;
//	if (nZoomPrev == nZoom)
//		return;
	IUnknownPtr punkCC;
	m_ptrImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC2(punkCC);
	if (sptrCC2 == 0) return;
	_bstr_t bstrCnvName;
	sptrCC2->GetCnvName(bstrCnvName.GetAddress());
	int nPanes = GetImagePaneCount(m_ptrImage);
	IUnknownPtr punkNewImage(::CreateTypedObject(_bstr_t(szTypeImage)),false);
	IImagePtr ptrNewImage(punkNewImage);
	if (ptrNewImage == 0)
		return;
	int cx = rcCoordSpaceNew.Width()/nZoom;
	int cy = rcCoordSpaceNew.Height()/nZoom;
	ptrNewImage->CreateImage(cx,cy,bstrCnvName,nPanes);
	_ptr_t2<int> axSrc(cx);
	for (int x = 0 ; x < cx; x++)
	{
		int xSrc = (x*nZoom+rcCoordSpaceNew.left-rcCoordSpacePrev.left)/nZoomPrev;
		axSrc[x] = xSrc;
	}
	CRect rcOver;
	if (!rcOver.IntersectRect(rcCoordSpaceNew,rcCoordSpacePrev))
		return;
	for (int y = 0; y < cy; y++)
	{
		for (int c = 0; c < nPanes; c++)
		{
			color *pSrc;
			ptrNewImage->GetRow(y, c, &pSrc);
			for (int x = 0 ; x < cx; x++)
			{
				pSrc[x] = 0xFFFF;
			}
		}
	}
	for (y = 0; y < cy; y++)
	{
		int ySrc = (y*nZoom+rcCoordSpaceNew.top-rcCoordSpacePrev.top)/nZoomPrev;
		if (ySrc < 0 || ySrc >= cyPrev) continue;
		for (int c = 0; c < nPanes; c++)
		{
			color *pSrc,*pDst;
			m_ptrImage->GetRow(ySrc, c, &pSrc);
			ptrNewImage->GetRow(y, c, &pDst);
			for (x = 0; x < cx; x++)
			{
				int xSrc = axSrc[x];
				if (xSrc >= 0 && xSrc < cxPrev)
					pDst[x] = pSrc[xSrc];
			}
		}
	}
	m_ptrImage = punkNewImage;
}

CRect CTotalImage::CalcDstRect(CRect rcCoordSpace, CRect rcSrc)
{
	CSize sz = GetSize();
	double dZoomX = double(sz.cx)/double(rcCoordSpace.Width());
	double dZoomY = double(sz.cy)/double(rcCoordSpace.Height());
	CRect rc((int)floor((rcSrc.left-rcCoordSpace.left)*dZoomX),
		(int)floor((rcSrc.top-rcCoordSpace.top)*dZoomY),
		(int)floor((rcSrc.right-rcCoordSpace.left)*dZoomX),
		(int)floor((rcSrc.bottom-rcCoordSpace.top)*dZoomY));
	return rc;
}

void CTotalImage::MapImage(CRect rcDst, IImage *pimgSrc)
{
	int cx,cy;
	pimgSrc->GetSize(&cx, &cy);
	if (rcDst.Width() == 0 || rcDst.Height() == 0 || m_ptrImage == 0)
		return;
	double kx = ((double)cx)/((double)rcDst.Width());
	double ky = ((double)cy)/((double)rcDst.Height());
//	int nx = cx/rcDst.Width();
//	int ny = cy/rcDst.Height();
	IUnknownPtr punkCC;
	m_ptrImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC2Dst(punkCC);
	if (sptrCC2Dst == 0) return;
	pimgSrc->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC2Src(punkCC);
	if (sptrCC2Src == 0) return;
	int cxDst = rcDst.Width();
	int cyDst = rcDst.Height();
	_ptr_t2<int> axSrc(cxDst);
	for (int x = 0; x < cxDst; x++)
	{
		int xSrc = (int)(x*kx);
		if (xSrc >= cx)
			xSrc = cx-1;
		axSrc[x] = xSrc;
	}
	int nPanes = GetImagePaneCount(pimgSrc);
	int nPanesDst = GetImagePaneCount(m_ptrImage);
	_ptr_t2<byte> aRGBBuff(cxDst*3);
	_ptr_t2<color> aClrBuff(cxDst*nPanes);
	_ptr_t2<color*> apClr(nPanes);
	_ptr_t2<color*> apClrDst(nPanesDst);
	for (int y = 0; y < cyDst; y++)
	{
		int ySrc = ((int)(y*ky));
		for (int c = 0; c < nPanes; c++)
		{
			color *pRow;
			pimgSrc->GetRow(ySrc, c, &pRow);
			apClr[c] = &aClrBuff[c*cxDst];
			for(int x = 0; x < cxDst; x++)
				aClrBuff[c*cxDst+x] = pRow[axSrc[x]];
		}
		sptrCC2Src->ConvertImageToDIB(apClr, aRGBBuff, cxDst, true);
		for (c = 0; c < nPanesDst; c++)
		{
			color *pRow;
			m_ptrImage->GetRow(y+rcDst.top, c, &pRow);
			apClrDst[c] = pRow+rcDst.left;
		}
		sptrCC2Dst->ConvertDIBToImage(aRGBBuff, apClrDst, cxDst, true);
	}
}

void CTotalImage::ClearRect(CRect rcDst)
{
	if (m_ptrImage == 0)
		return;
	int cx,cy;
	m_ptrImage->GetSize(&cx, &cy);
	rcDst.left = Range(rcDst.left, 0, cx);
	rcDst.right = Range(rcDst.right, 0, cx);
	rcDst.top = Range(rcDst.top, 0, cy);
	rcDst.bottom = Range(rcDst.bottom, 0, cy);
	if (rcDst.IsRectEmpty())
		return;
	int cxDst = rcDst.Width();
	int cyDst = rcDst.Height();
	int nPanes = GetImagePaneCount(m_ptrImage);
	for (int y = 0; y < cyDst; y++)
	{
		for (int c = 0; c < nPanes; c++)
		{
			color *pRow;
			m_ptrImage->GetRow(y+rcDst.top, c, &pRow);
			for (int x = rcDst.left; x < rcDst.right; x++)
			{
				pRow[x] = 0xFFFF;
			}
		}
	}
}

static int g_nObjectCount = 0;

CSewLIBase::CSewLIBase()
{
	m_dwFlags = nofHasData;
	m_pinfo = 0;
	m_bModified = false;
	m_keyBase = m_key;
	m_lFragNum = m_l1stFragNum = 0;
	m_nRangeCoef = g_nRangeCoef;
	m_lPosActive = 0;
	m_rcTotal = CRect(0,0,0,0);
	m_nOvrPerc = -1;
	m_bSaving = false;
	m_bDirInited = false;
	g_nObjectCount++;
}

CSewLIBase::~CSewLIBase()
{
//	ClearUndo();
	g_nObjectCount--;
	if (g_nObjectCount == 0)
	{
		_bstr_t bstrTempDir = _GetTempDir();
		RemoveDirContent(bstrTempDir, false, false);
	}
	else
	{
		if (m_bstrFragmentDir.length() > 0) // Not saved. Remove directory.
		{
			RemoveDirContent(m_bstrFragmentDir, false, true);
			wchar_t szPath[_MAX_PATH];
			wcscpy(szPath,(const wchar_t *)m_bstrFragmentDir);
			int nLen = wcslen(szPath);
			if (szPath[nLen-1] == L'/' || szPath[nLen-1] == L'\\')
				szPath[nLen-1] = 0;
			_wrmdir(m_bstrFragmentDir);
			m_bstrFragmentDir = "";
		}
	}
	if( m_pinfo )
		m_pinfo->Release();
}	

//IPersist helper
//////////////////////////////////////////////////////////////////////
GuidKey CSewLIBase::GetInternalClassID()
{
	return clsidSewLIBase;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CSewLIBase::DoGetInterface( const IID &iid )
{
	if (iid == IID_ISewImageList)
		return (ISewImageList*)this;
	else if (iid == IID_INotifyObject)
		return (INotifyObject*)this;
	else if (iid == IID_INotifyPlace)
		return (INotifyPlace *)this;
	else 
		return CObjectBase::DoGetInterface( iid );
}


//////////////////////////////////////////////////////////////////////
//basic functionality
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
HRESULT CSewLIBase::GetObjectFlags( DWORD *pdwObjectFlags )
{
	CObjectBase::GetObjectFlags( pdwObjectFlags );
	if( pdwObjectFlags )
		*pdwObjectFlags |= nofCantSendTo;
	return S_OK;
}


HRESULT CSewLIBase::OnActivateObject()
{
/*	bool bChanged = false;
	if (CheckFragmentDirValid(&bChanged) && bChanged)
	{
		IFileDataObject2Ptr sptrFDO2(m_punkNamedData);
		sptrFDO2->SetModifiedFlag(TRUE);
	}*/
	return S_OK;
}


void CSewLIBase::ClearUndo()
{
	_bstr_t bstrPath;
	GetPath(bstrPath.GetAddress(), FALSE);
	if (bstrPath.length() == 0)
		return;
	CString sUndoPath = (const char *)bstrPath;
	sUndoPath += "Undo";
	CString sUndoPath1 = sUndoPath + "\\";
	CString sUndoPathWC = sUndoPath + "\\*";
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(sUndoPathWC, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(fd.cFileName,".") == 0 || strcmp(fd.cFileName,"..") == 0)
				continue;
			CString s = sUndoPath1+fd.cFileName;
			unlink(s);
		}
		while(FindNextFile(h, &fd));
		FindClose(h);
		unlink(sUndoPath);
	}
}

void CSewLIBase::ClearOrphantImages()
{
	_bstr_t bstrPath;
	GetPath(bstrPath.GetAddress(), FALSE);
	if (bstrPath.length() == 0)
		return;
	long lMaxFragNum = -1;
	long lpos;
	GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		long lFragNum;
		sptrFrag->GetFragNum(&lFragNum);
		if (lFragNum > lMaxFragNum)
			lMaxFragNum = lFragNum;
	}
	if (lMaxFragNum == -1)
		return;
	_ptr_t2<byte> frags(lMaxFragNum+1);
	frags.zero();
	GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		long lFragNum;
		sptrFrag->GetFragNum(&lFragNum);
		frags[lFragNum] = 1;
	}
	CString sPath1 = CString((LPCTSTR)bstrPath) + "\\";
	CString sPathWC = sPath1 + "*";
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(sPathWC, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(fd.cFileName,".") == 0 || strcmp(fd.cFileName,"..") == 0)
				continue;
			if (!_istdigit(fd.cFileName[0]))
				continue;
			int n = _ttoi(fd.cFileName);
			if (n > 0 && n < min(lMaxFragNum,m_l1stFragNum) && frags[n] == 0)
			{
				CString s = sPath1+fd.cFileName;
				unlink(s);
			}
		}
		while(FindNextFile(h, &fd));
		FindClose(h);
	}
}


IUnknownPtr CSewLIBase::MakeFragment(IUnknown *punkImage, int nRange, POINT ptOffs, SIZE szImage)
{
	CSewLIFragment *pFrag = CSewLIFragment::Make();
	IUnknownPtr punkFrag(pFrag);
	pFrag->SetListPtr(this);
	pFrag->SetFragNum(m_lFragNum++);
	if (punkImage)
		pFrag->SetImage(punkImage);
	pFrag->SetOffset(ptOffs);
	pFrag->SetSize(szImage);
	pFrag->Release();
	return punkFrag;
}

void CSewLIBase::RebuildTotalImage(int nRects, RECT *pRects)
{
	for (int i = 0; i < nRects; i++)
		m_TotalImage.ClearRect(m_TotalImage.CalcDstRect(m_rcTotal,pRects[i]));
	long lpos;
	GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		POINT pt;
		sptrFrag->GetOffset(&pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		CRect rcFrag(pt,sz);
		bool bIntersect = false;
		for (int i = 0; i < nRects; i++)
		{
			CRect rcOvr;
			if (rcOvr.IntersectRect(rcFrag,&pRects[i]))
			{
				bIntersect = true;
				break;
			}
		}
		if (bIntersect)
		{
			IUnknownPtr punkImg;
			int nRangeCoef;
			GetRangeCoef(&nRangeCoef);
			int iZoom = 1;
			for (i = 0; i < g_nFragmentImages; i++)
			{
				int iZoom1 = iZoom;
				sptrFrag->GetImage(&iZoom1, &punkImg);
				if (punkImg != 0)
					break;
				iZoom *= nRangeCoef;
			}
			CLoadFragment lf;
			if (punkImg == 0)
			{
				int iZoom1 = iZoom/nRangeCoef;
				lf.Load(sptrFrag, iZoom1);
				sptrFrag->GetImage(&iZoom1, &punkImg);
			}
			IImagePtr ptrImg(punkImg);
			if (ptrImg != 0)
				m_TotalImage.MapImage(m_TotalImage.CalcDstRect(m_rcTotal,rcFrag),ptrImg);
		}
	}
}

void CSewLIBase::RebuildTotalImage(CRect rcCoordSpace)
{
	RebuildTotalImage(1, &rcCoordSpace);
}

void CSewLIBase::RecalcTotalRect()
{
	m_rcTotal = CRect(0,0,0,0);
	bool bFirst = true;
	long lpos;
	GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		POINT pt;
		sptrFrag->GetOffset(&pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		if (bFirst)
			m_rcTotal = CRect(pt, sz);
		else
		{
			m_rcTotal.left = min(m_rcTotal.left,pt.x);
			m_rcTotal.right = max(m_rcTotal.right, pt.x+sz.cx);
			m_rcTotal.top = min(m_rcTotal.top,pt.y);
			m_rcTotal.bottom = max(m_rcTotal.bottom,pt.y+sz.cy);
		}
		bFirst = false;
	}
}

void CSewLIBase::InitDir()
{
	if (!m_bDirInited)
	{
		IDocumentSite2Ptr sptsDoc(m_punkNamedData);
		if (sptsDoc != 0)
		{
			_bstr_t bstrPathName;
			sptsDoc->GetPathName2(bstrPathName.GetAddress(), NULL);
			if (bstrPathName.length() > 0)
			{
				wchar_t szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
				_wsplitpath(bstrPathName,szDrive,szDir,szFName,NULL);
				_wmakepath(szPath,szDrive,szDir,szFName,NULL);
				_bstr_t bstrObjName;
				INamedObject2Ptr sptrNO2(Unknown());
				sptrNO2->GetName(bstrObjName.GetAddress());
				if (bstrObjName.length() != 0)
				{
					wcscat(szPath,L"_");
					wcscat(szPath,bstrObjName);
				}
				_wmkdir(szPath);
				wcscat(szPath,L"\\");
				m_bstrFragmentDir = szPath;
			}
		}
		m_bDirInited = true;
	}
};

int CSewLIBase::CalcOverPercentByRect(CRect rcOvr, long lPosSkip)
{
	int nRes = -2;
	long lpos;
	GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		long lposPrev = lpos;
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		if (lposPrev == lPosSkip)
			continue;
		POINT pt;
		sptrFrag->GetOffset(&pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		if (pt.x < rcOvr.right && pt.x+sz.cx >= rcOvr.left &&
			pt.y < rcOvr.bottom && pt.y+sz.cy >= rcOvr.top)
		{
			int x1 = max(rcOvr.left,pt.x);
			int x2 = min(rcOvr.right,pt.x+sz.cx);
			int y1 = max(rcOvr.top,pt.y);
			int y2 = min(rcOvr.bottom,pt.y+sz.cy);
			int d = (x2-x1)*(y2-y1)*100/(rcOvr.Width()*rcOvr.Height());
			if (d > nRes)
				nRes = d;
		}
		else
			if (nRes < 0)
				nRes = 0;
	}
	return nRes;
}


void CSewLIBase::RecalcOvrPercent(bool bFireEvent)
{
	long lpos;
	GetLastFragmentPosition(&lpos);
	if (lpos != 0)
	{
		long lPosSkip = lpos;
		ISewFragmentPtr sptrFrag;
		GetNextFragment(&lpos, &sptrFrag);
		POINT pt;
		sptrFrag->GetOffset(&pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		CRect rcOvr(pt, sz);
		m_nOvrPerc = CalcOverPercentByRect(rcOvr, lPosSkip);
	}
}


HRESULT CSewLIBase::AddImage(IUnknown *punkImage, POINT ptOffs, DWORD dwFlags)
{
	IImage3Ptr img(punkImage);
	if (img == 0)
		return E_FAIL;
	m_bSaving = true;
	if (dwFlags&addImageFindPos)
	{
		BOOL bGlobalSearch = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "EnableAfterInput", TRUE);
		ISewFragment *pPrev;
		if (CalcOrigFragOffset(&pPrev, &ptOffs) == S_OK)
		{
			if (bGlobalSearch)
				FindOptimalPosition1(this, pPrev, img, &ptOffs);
		}
	}
	int cx,cy;
	img->GetSize(&cx, &cy);
	if (cx == 0 || cy == 0)
		return E_FAIL;
	_size szImage(cx,cy);
	CRect rcTotalPrev(m_rcTotal);
	if (m_listTerm.head() == NULL)
		m_rcTotal = CRect(ptOffs, CSize(cx,cy));
	else
	{
		m_rcTotal.left = min(m_rcTotal.left,ptOffs.x);
		m_rcTotal.right = max(m_rcTotal.right, ptOffs.x+cx);
		m_rcTotal.top = min(m_rcTotal.top,ptOffs.y);
		m_rcTotal.bottom = max(m_rcTotal.bottom,ptOffs.y+cy);
	}
	IUnknownPtr punkTermFrag = MakeFragment(NULL/*punkImage*/, 0, ptOffs, szImage);
	ISewFragmentPtr pTermFrag(punkTermFrag);
	m_lPosActive = m_listTerm.insert(pTermFrag);
	m_TotalImage.Resize(rcTotalPrev, m_rcTotal);
	m_TotalImage.MapImage(m_TotalImage.CalcDstRect(m_rcTotal,CRect(ptOffs,CSize(cx,cy))),img);
	INotifyControllerPtr	sptrNC(m_punkNamedData);
	if (sptrNC != 0)
	{
		int nev = silevAddImage;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	if (FAILED(pTermFrag->SetImage(punkImage)))
	{
		m_bSaving = false;
		DeleteFragment(m_lPosActive);
		return E_FAIL;
	}
	if (sptrNC != 0)
	{
		int nev = silevImageChanged;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	RecalcOvrPercent(false);
	m_bSaving = false;
	SetModifiedFlag(TRUE);
	return S_OK;
}

HRESULT CSewLIBase::AddFragment(ISewFragment *pFragment)
{
	CPoint ptOffs;
	pFragment->GetOffset(&ptOffs);
	CSize sz;
	pFragment->GetSize(&sz);
	CRect rcTotalPrev(m_rcTotal);
	if (m_listTerm.head() == NULL)
		m_rcTotal = CRect(ptOffs, sz);
	else
	{
		m_rcTotal.left = min(m_rcTotal.left,ptOffs.x);
		m_rcTotal.right = max(m_rcTotal.right, ptOffs.x+sz.cx);
		m_rcTotal.top = min(m_rcTotal.top,ptOffs.y);
		m_rcTotal.bottom = max(m_rcTotal.bottom,ptOffs.y+sz.cy);
	}
	m_lPosActive = m_listTerm.insert(pFragment);
	m_TotalImage.Resize(rcTotalPrev, m_rcTotal);
	INotifyControllerPtr	sptrNC(m_punkNamedData);
	if (sptrNC != 0)
	{
		int nev = silevAddImage;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	IUnknownPtr punkImg;
	int nZoom = 1;
	pFragment->GetImage(&nZoom,&punkImg);
	CLoadFragment lf;
	if (punkImg == 0)
	{
		lf.Load(pFragment,1);
		pFragment->GetImage(&nZoom,&punkImg);
	}
	IImagePtr img(punkImg);
	m_TotalImage.MapImage(m_TotalImage.CalcDstRect(m_rcTotal,CRect(ptOffs,sz)),img);
	if (sptrNC != 0)
	{
		int nev = silevImageChanged;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	RecalcOvrPercent(false);
	SetModifiedFlag(TRUE);
	return S_OK;
}


HRESULT CSewLIBase::MoveFragment(ISewFragment *pFragment, POINT ptNewPos, DWORD dwFlags)
{
	POINT ptPrevPos;
	pFragment->GetOffset(&ptPrevPos);
	SIZE sz;
	pFragment->GetSize(&sz);
	if (dwFlags&moveFragCorrect)
	{
		if (GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Correction", "Enable", 1))
			CorrectPos(this,pFragment,ptNewPos,&ptNewPos);
	}
	pFragment->SetOffset(ptNewPos);
	CRect rcTotalPrev(m_rcTotal);
	RecalcTotalRect();
	if (dwFlags&moveFragFire)
		m_TotalImage.Resize(rcTotalPrev, m_rcTotal);
	CRect rcTI(min(ptPrevPos.x,ptNewPos.x),min(ptPrevPos.y,ptNewPos.y),
		max(ptPrevPos.x,ptNewPos.x)+sz.cx,max(ptPrevPos.y,ptNewPos.y)+sz.cy);
	if (dwFlags&moveFragFire)
		RebuildTotalImage(m_rcTotal);
	if (m_punkNamedData != 0)
	{
		INotifyControllerPtr	sptrNC(m_punkNamedData);
		int nev = silevMoveImage;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	if (dwFlags&moveFragCalcOvr)
		RecalcOvrPercent(true);
	if (dwFlags&moveFragFire)
		CheckOverlapPercent();
	SetModifiedFlag(TRUE);
	return S_OK;
}

HRESULT CSewLIBase::DeleteFragment(long lPosFrag)
{
	ISewFragment *pFrag = m_listTerm.get(lPosFrag);
	if (pFrag == NULL) return S_FALSE;
	if (lPosFrag == m_lPosActive)
	{
		long l = lPosFrag;
		m_listTerm.prev(l);
		if (l == NULL && m_listTerm.tail() != lPosFrag)
			l = m_listTerm.tail();
		m_lPosActive = l;
	}
	POINT ptPos;
	pFrag->GetOffset(&ptPos);
	SIZE sz;
	pFrag->GetSize(&sz);
	CRect rc(ptPos, sz);
//	pFrag->SetImage(NULL);
	m_listTerm.remove(lPosFrag);
	CRect rcTotalPrev(m_rcTotal);
	RecalcTotalRect();
	m_TotalImage.Resize(rcTotalPrev, m_rcTotal);
	RebuildTotalImage(CRect(max(m_rcTotal.left,rc.left),max(m_rcTotal.top,rc.top),
		min(m_rcTotal.right,rc.right),min(m_rcTotal.bottom,rc.bottom)));
	if (m_punkNamedData != 0)
	{
		INotifyControllerPtr	sptrNC(m_punkNamedData);
		int nev = silevDeleteImage;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	RecalcOvrPercent(false);
	SetModifiedFlag(TRUE);
	return S_OK;
}


HRESULT CSewLIBase::GetActiveFragmentPosition(long *plPos)
{
	*plPos = m_lPosActive;
	return S_OK;
}

HRESULT CSewLIBase::SetActiveFragmentPosition(long lPos)
{
	m_lPosActive = lPos;
	if (m_punkNamedData != 0)
	{
		INotifyControllerPtr	sptrNC(m_punkNamedData);
		int nev = silevActiveChanged;
		sptrNC->FireEvent(_bstr_t(szEventChangeObject), NULL, Unknown(), &nev, sizeof(int));
	}
	return S_OK;
}

/*
bool CSewLIBase::CheckFragmentDirValid(bool *pbChanged)
{
	*pbChanged = false;
	while (1)
	{
		CString sDir = (const char *)m_bstrFragmentDir;
		int n = sDir.GetLength();
		if (sDir[n-1] == '\\' || sDir[n-1] == '/')
			sDir = sDir.Left(n-1);
		WIN32_FIND_DATA fd;
		bool bFound = false;
		HANDLE h = FindFirstFile(sDir, &fd);
		if (h == INVALID_HANDLE_VALUE)
		{
			if (!PromptForDir())
			{
				m_bstrFragmentDir = L"";
				return false;
			}
			*pbChanged = true;
		}
		else
		{
			FindClose(h);
			sDir += "\\sli.vt5";
			if (!CheckSliVt5(sDir, m_guidDoc))
			{
				if (!PromptForDir())
				{
					m_bstrFragmentDir = L"";
					return false;
				}
				*pbChanged = true;
			}
			else
				bFound = true;
		}
		if (bFound)
			return true;
	}
}
*/

HRESULT CSewLIBase::CalcOrigFragOffset(ISewFragment **ppPrev, POINT *pptOffs)
{
	long lTailPos;
	if (m_lPosActive != 0)
		lTailPos = m_lPosActive;
	else if (m_listTerm.tail())
		lTailPos = m_listTerm.tail();
	else
		lTailPos = 0;
	if (lTailPos != 0)
	{
		CPoint ptOffs;
		ISewFragment *pPrev = m_listTerm.get(lTailPos);
		long lBef = lTailPos;
		m_listTerm.prev(lBef);
		ISewFragment *pPrev2 = lBef==0?NULL:m_listTerm.get(lBef);
		POINT ptPrev;
		pPrev->GetOffset(&ptPrev);
		SIZE szPrev;
		pPrev->GetSize(&szPrev);
		if (pPrev2 != NULL)
		{
			POINT ptPrev2;
			pPrev2->GetOffset(&ptPrev2);
			ptOffs.x = ptPrev.x+(ptPrev.x-ptPrev2.x);
			ptOffs.y = ptPrev.y+(ptPrev.y-ptPrev2.y);
		}
		else
		{
			ptOffs.y = ptPrev.y;
			ptOffs.x = ptPrev.x+szPrev.cx*9/10;
		}
		*pptOffs = ptOffs;
		*ppPrev = pPrev;
		if (*ppPrev)
			(*ppPrev)->AddRef();
		return S_OK;
	}
	return S_FALSE;
}

HRESULT CSewLIBase::IsSavingOrLoading(BOOL *pbSaving, BOOL *pbLoading)
{
	*pbSaving = m_bSaving;
	*pbLoading = s_bLoading;
	return S_OK;
}

HRESULT CSewLIBase::GetTotalImage(IUnknown **ppunk)
{
	*ppunk = m_TotalImage.GetImage();
	if (*ppunk)
		(*ppunk)->AddRef();
	return S_OK;
}


void CSewLIBase::MoveFragmentsDir()
{
	_bstr_t bstrPath,bstrSavingPath;
	IDocumentSite2Ptr sptsDoc(m_punkNamedData);
	if (sptsDoc == 0)
		return;
	sptsDoc->GetPathName2(bstrPath.GetAddress(), NULL);
	sptsDoc->GetSavingPathName(bstrSavingPath.GetAddress());
	if (bstrSavingPath == bstrPath)
		return;
	bool bTemp = bstrPath.length()==0;
	GetPath(bstrPath.GetAddress(), FALSE);
	GetPath(bstrSavingPath.GetAddress(), TRUE);
	wchar_t szPath[_MAX_PATH],szSavingPath[_MAX_PATH];
	wcscpy(szPath,bstrPath);
	int nLen = wcslen(szPath);
	if (szPath[nLen-1] == L'\\' || szPath[nLen-1] == L'/')
		szPath[nLen-1] = 0;
	wcscpy(szSavingPath,bstrSavingPath);
	nLen = wcslen(szSavingPath);
	if (szSavingPath[nLen-1] == L'\\' || szSavingPath[nLen-1] == L'/')
		szSavingPath[nLen-1] = 0;
	wchar_t szDrive1[_MAX_DRIVE],szDrive2[_MAX_DRIVE];
	_wsplitpath(szSavingPath,szDrive1,NULL,NULL,NULL);
	_wsplitpath(szPath,szDrive2,NULL,NULL,NULL);
	if (wcscmp(szDrive1,szDrive2)==0 && bTemp)
		MoveFileW(szPath, szSavingPath);
	else
	{
		CHourglass wc;
		RecursiveCopyDir(bstrPath,bstrSavingPath);
		if (bTemp)
		{
			RemoveDirContent(bstrPath,false,false);
			char szPath1[_MAX_PATH];
			strcpy(szPath1,bstrPath);
			int n = strlen(szPath1);
			if (szPath1[n-1] == '\\' || szPath1[n-1] == '/')
				szPath1[n-1] = 0;
			rmdir(szPath1);
		}
	}
	m_bstrFragmentDir = "";
}


//////////////////////////////////////////////////////////////////////
//interface ISerializableObject
//////////////////////////////////////////////////////////////////////
HRESULT CSewLIBase::Load( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;
	ULONG nRead;
	long lVersion;
	pStream->Read( &lVersion, sizeof(long), &nRead );
	pStream->Read( &m_lFragNum, sizeof(long), &nRead );
	SerializeBSTR(m_bstrFragmentDir.GetAddress(), *pStream, false);
	pStream->Read( (LPRECT)m_rcTotal, sizeof(RECT), &nRead );
	while (1)
	{
		long lFrag = 0;
		pStream->Read( &lFrag, sizeof(long), &nRead );
		if (lFrag == 0)
			break;
		IUnknownPtr sptr(MakeFragment(NULL, 0, _point(0,0), _size(0,0)));
		ISerializableObjectPtr sptrSOFrag(sptr);
		sptrSOFrag->Load(pStream, pparams);
		ISewFragmentPtr sptrSF(sptr);
		m_listTerm.insert(sptrSF);
	}
	m_lPosActive = m_listTerm.tail();
	m_TotalImage.Load(pStream,pparams);
	pStream->Read(&m_guidDoc, sizeof(m_guidDoc), &nRead);
	if (lVersion >= 1)
		SerializeBSTR(m_bstrFragmentFmt.GetAddress(), *pStream, false);
	else
		m_bstrFragmentFmt = GetValueString(GetAppUnknown(), "\\Sew Large Image", "DefaultFragmentFormat", "jpg");
	m_l1stFragNum = m_lFragNum;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewLIBase::Store( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( S_OK != hr )
		return hr;
	ULONG nWritten = 0;
	long lVersion = 1;
	pStream->Write( &lVersion, sizeof(long), &nWritten );
	pStream->Write( &m_lFragNum, sizeof(long), &nWritten );
	SerializeBSTR(&m_bstrFragmentDir.GetBSTR(), *pStream, true);
	pStream->Write( (LPRECT)m_rcTotal, sizeof(RECT), &nWritten );
	long lObj = 1;
	long lposTerm = m_listTerm.head();
	while (lposTerm)
	{
		ISewFragment *pFrag = m_listTerm.next(lposTerm);
		ISerializableObjectPtr sptrFrag(pFrag);
		if (sptrFrag != 0)
		{
			pStream->Write( &lObj, sizeof(long), &nWritten );
			sptrFrag->Store(pStream, pparams);
		}
	}
	lObj = 0;
	pStream->Write( &lObj, sizeof(long), &nWritten );
	m_TotalImage.Save(pStream,pparams);
	pStream->Write(&m_guidDoc, sizeof(m_guidDoc), &nWritten);
	SerializeBSTR(&m_bstrFragmentFmt.GetBSTR(), *pStream, true);
	MoveFragmentsDir();
	if (GetValueInt(GetAppUnknown(), "\\Sew Large Image", "DeleteOrphantImages", FALSE))
		ClearOrphantImages();
	return S_OK;
}

////////////////////////////////////////////////////////////////////////
// INotifyPlace
////////////////////////////////////////////////////////////////////////

HRESULT CSewLIBase::NotifyPutToData( IUnknown* punkObj )
{
	return E_FAIL;
}

HRESULT CSewLIBase::NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj )
{
	*punkUndoObj = NULL;
	CLockDocument ld(m_punkNamedData);
	CPoint ptPos(0,0);
	DWORD dwFlags = addImageFindPos;
	IDocumentSitePtr sptrDS(m_punkNamedData);
	if (sptrDS != 0)
	{
		IUnknownPtr punkView;
		sptrDS->GetActiveView(&punkView);
		ISewLIViewPtr sptrView(punkView);
		if (sptrView != 0)
		{
			BOOL bLiveVideo;
			sptrView->get_ShowLiveVideo(&bLiveVideo);
			if (bLiveVideo)
			{
				CRect rcLV;
				sptrView->GetLiveVideoRect((int *)&rcLV.left, (int *)&rcLV.top,
					(int *)&rcLV.right,	(int *)&rcLV.bottom);
				ptPos = rcLV.TopLeft();
				dwFlags = 0;
			}
		}
	}
	HRESULT hr = AddImage(punkObj, ptPos, dwFlags);
	if (SUCCEEDED(hr))
	{
		ISewFragment *p = m_listTerm.get(m_listTerm.tail());
		*punkUndoObj = p;
		(*punkUndoObj)->AddRef();
	}
	return S_OK;
}

HRESULT CSewLIBase::Undo( IUnknown* punkUndoObj )
{
	ISewFragmentPtr ptrFragToDel(punkUndoObj);
	if (ptrFragToDel != 0)
	{
		long lpos,lToDel=0;
		GetFirstFragmentPos(&lpos);
		while (lpos)
		{
			long lPrev = lpos;
			ISewFragmentPtr ptrFragCur;
			GetNextFragment(&lpos, &ptrFragCur);
			if (ptrFragCur == ptrFragToDel)
				lToDel = lPrev;
		}
		if (lToDel != 0)
			DeleteFragment(lToDel);
	}
	return S_OK;
}

HRESULT CSewLIBase::Redo( IUnknown* punkUndoObj )
{
	ISewFragmentPtr ptrFragRestore(punkUndoObj);
	if (ptrFragRestore != 0)
	{
//		ptrFragRestore->RestoreImages();
		AddFragment(ptrFragRestore);
	}
	return S_OK;
}



//////////////////////////////////////////////////////////////////////
//interface INamedDataObject2
//////////////////////////////////////////////////////////////////////
HRESULT CSewLIBase::IsBaseObject( BOOL * pbFlag )
{
	if( !pbFlag )	return E_POINTER;

	*pbFlag = TRUE;
	return S_OK;
}

/*bool CSewLIBase::PromptForDir()
{
	HWND hWndMain = NULL;
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp != 0 )
		ptrApp->GetMainWindowHandle( &hWndMain );
	IMalloc *pMalloc;
	SHGetMalloc(&pMalloc);
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.ulFlags = BIF_NEWDIALOGSTYLE|BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	char szPrompt[256];
	LoadString(app::handle(), IDS_BROWSE_FOR_BASE_FOLDER, szPrompt, 256);
	bi.lpszTitle = szPrompt;
	bi.hwndOwner = hWndMain;
	ITEMIDLIST *pItemIdList = SHBrowseForFolder(&bi);
	if (pItemIdList == NULL)
		return false;
	wchar_t szPath[_MAX_PATH];
    SHGetPathFromIDListW(pItemIdList, szPath);
	pMalloc->Free(pItemIdList);
	int n = wcslen(szPath);
	if (n > 0)
	{
		if (szPath[n] != L'\\' && szPath[n] != L'/')
			wcscat(szPath, L"\\");
		m_bstrFragmentDir = szPath;
		return true;
	}
	return false;
}*/


HRESULT CSewLIBase::NotifyCreate()
{
	::CoCreateGuid((GUID*)&m_guidDoc);
	m_bstrFragmentFmt = GetValueString(GetAppUnknown(), "\\Sew Large Image", "DefaultFragmentFormat", "jpg");
	_bstr_t bstrTemp = _GetTempDir();
	if (bstrTemp.length() == 0)
		return E_FAIL;
	wchar_t szWBuff[512];
	StringFromGUID2(m_guidDoc, szWBuff, 512);
	bstrTemp += szWBuff;
	if (CheckDirectory(bstrTemp, m_guidDoc))
	{
		bstrTemp += _T("\\");
		m_bstrFragmentDir = bstrTemp;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSewLIBase::NotifyDestroy()
{
	return S_OK;
}

HRESULT CSewLIBase::NotifyActivate( bool bActivate )
{
	return S_OK;
}

HRESULT CSewLIBase::NotifySelect( bool bSelect )
{
	InitDir();
	return S_OK;
}


HRESULT	CSewLIBase::GetType( BSTR *pbstrType )
{
	return CObjectBase::GetType( pbstrType );
}

HRESULT CSewLIBase::GetFirstFragmentPos(long *plPos)
{
	*plPos = m_listTerm.head();
	return S_OK;
}

HRESULT CSewLIBase::GetNextFragment(long *plPos, ISewFragment **ppFragment)
{
	*ppFragment = m_listTerm.next(*plPos);
	if (*ppFragment)
		(*ppFragment)->AddRef();
	return S_OK;
}

HRESULT CSewLIBase::GetPrevFragment(long *plPos, ISewFragment **ppFragment)
{
	*ppFragment = m_listTerm.prev(*plPos);
	if (*ppFragment)
		(*ppFragment)->AddRef();
	return S_OK;
}

HRESULT CSewLIBase::GetFragmentsCount(long *plCount)
{
	*plCount = m_listTerm.Count();
	return S_OK;
}


HRESULT CSewLIBase::GetPath(BSTR *pbstrPath, BOOL bSaving)
{
	*pbstrPath = 0;
	IDocumentSite2Ptr sptsDoc(m_punkNamedData);
	if (sptsDoc != 0)
	{
		_bstr_t bstrPathName;
		if (bSaving)
			sptsDoc->GetSavingPathName(bstrPathName.GetAddress());
		else
			sptsDoc->GetPathName2(bstrPathName.GetAddress(), NULL);
		if (bstrPathName.length() > 0)
		{
			wchar_t szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
			_wsplitpath(bstrPathName,szDrive,szDir,szFName,NULL);
			_wmakepath(szPath,szDrive,szDir,szFName,NULL);
			_bstr_t bstrObjName;
			INamedObject2Ptr sptrNO2(Unknown());
			sptrNO2->GetName(bstrObjName.GetAddress());
			if (bstrObjName.length() != 0)
			{
				wcscat(szPath,L"_");
				wcscat(szPath,bstrObjName);
			}
			wcscat(szPath,L"\\");
			*pbstrPath = SysAllocString(szPath);
		}
	}
	if (*pbstrPath == 0)
		*pbstrPath = m_bstrFragmentDir.copy();
	return S_OK;
}

HRESULT CSewLIBase::GetOrigin(POINT *pOrg)
{
	pOrg->x = m_rcTotal.left;
	pOrg->y = m_rcTotal.top;
	return S_OK;
}


HRESULT CSewLIBase::GetSize(SIZE *pSize)
{
	pSize->cx = m_rcTotal.Width();
	pSize->cy = m_rcTotal.Height();
	return S_OK;
}

HRESULT CSewLIBase::GetRangeCoef(int *pnRangeCoef)
{
	*pnRangeCoef = m_nRangeCoef;
	return S_OK;
}

HRESULT CSewLIBase::GetLastFragmentPosition(long *plPos)
{
	*plPos = m_listTerm.tail();
	return S_OK;
}

HRESULT CSewLIBase::GetOverlapPercent(int *pnOvrPerc)
{
	if (m_nOvrPerc == -1)
		RecalcOvrPercent(false);
	*pnOvrPerc = m_nOvrPerc;
	return S_OK;
}

HRESULT CSewLIBase::CheckOverlapPercent()
{
	if (m_nOvrPerc >= 0 && m_nOvrPerc < ::GetValueInt(::GetAppUnknown(),
		"\\Sew Large Image", "MinOverlapPercentForControl", 5))
	{
		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if (ptr_ss != 0)
			ptr_ss->Invoke(_bstr_t("SewLI_OverlapPercentTooSmall"),
				0, NULL, 0, fwAppScript);
	}
	return S_OK;
}

HRESULT CSewLIBase::GetFragmentsFormat(BSTR *pbstrFragFmt)
{
	*pbstrFragFmt = m_bstrFragmentFmt.copy();
	return S_OK;
}

