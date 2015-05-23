#include "StdAfx.h"
#include "Cromos.h"
#include "ChromoCnv.h"
#include "Settings.h"
#include "Equalize.h"
#include "alloc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static void MirrorImage(IImage3 *pImg)
{
	CImageWrp img(pImg);
	int nWidth = img.GetWidth();
	int nHeight = img.GetHeight();
	int nPanes = img.GetColorsCount();
	{
		for (int y = 0; y < nHeight; y++)
		{
			byte  *pRowMask = img.GetRowMask(y);
			for (int x = 0; x < nWidth/2; x++)
			{
				byte by = pRowMask[x];
				pRowMask[x] = pRowMask[nWidth-x-1];
				pRowMask[nWidth-x-1] = by;
			}
			for (int c = 0; c < nPanes; c++)
			{
				color *pRow = img.GetRow(y,c);
				for (x = 0; x < nWidth/2; x++)
				{
					color c = pRow[x];
					pRow[x] = pRow[nWidth-1-x];
					pRow[nWidth-1-x] = c;
				}
			}
		}
	}
}


inline int min4(int i1, int i2, int i3, int i4) {return min(min(i1,i2),min(i3,i4));}
inline int max4(int i1, int i2, int i3, int i4) {return max(max(i1,i2),max(i3,i4));}

void Interp(CImageWrp &imgSrc, FPOINT pt, int c, color &clrDest, byte &maskDest)
{
	double dMask = 0., dData = 0.;
	int x1 = (int)floor(pt.x);
	int x2 = (int)ceil(pt.x);
	int y1 = (int)floor(pt.y);
	int y2 = (int)ceil(pt.y);
	if (y1 >= 0 && y1 < imgSrc.GetHeight())
	{
		byte *pMask = imgSrc.GetRowMask(y1);
		color *pData = imgSrc.GetRow(y1, c);
		if (x1 >= 0 && x1 < imgSrc.GetWidth() && pMask[x1])
		{
			dData += pData[x1]*(1.-pt.x+x1)*(1.-pt.y+y1);
			dMask += (1.-pt.x+x1)*(1.-pt.y+y1);
		}
		if (x2 != x1 && x2 >= 0 && x2 < imgSrc.GetWidth() && pMask[x2])
		{
			dData += pData[x2]*(1.-x2+pt.x)*(1.-pt.y+y1);
			dMask += (1.-x2+pt.x)*(1.-pt.y+y1);
		}
	}
	if (y2 != y1 && y2 >= 0 && y2 < imgSrc.GetHeight())
	{
		byte *pMask = imgSrc.GetRowMask(y2);
		color *pData = imgSrc.GetRow(y2, c);
		if (x1 >= 0 && x1 < imgSrc.GetWidth() && pMask[x1])
		{
			dData += pData[x1]*(1.-pt.x+x1)*(1.-y2+pt.y);
			dMask += (1.-pt.x+x1)*(1.-y2+pt.y);
		}
		if (x2 != x1 && x2 >= 0 && x2 < imgSrc.GetWidth() && pMask[x2])
		{
			dData += pData[x2]*(1.-x2+pt.x)*(1.-y2+pt.y);
			dMask += (1.-x2+pt.x)*(1.-y2+pt.y);
		}
	}
	if (dMask > 0.5)
	{
		double d = dData/dMask;
		ASSERT(dMask <= 1.);
		ASSERT(d >= 0. && d <= 0xFFFF);
		if (d > 0xFFFF)
			clrDest = 0xFFFF;
		else if (d < 0.)
			clrDest = 0;
		else
			clrDest = (color)d;
		maskDest = 0xFF;
	}
	else
		maskDest = 0;
}

CImageWrp RotateImage(CImageWrp &imgSrc, CPoint ptCenterOCS, double dAngle)
{
	// Create temporary image - more than needed
	CPoint pt1 = rotatepoint(ptCenterOCS, CPoint(0,0), dAngle);
	CPoint pt2 = rotatepoint(ptCenterOCS, CPoint(imgSrc.GetWidth()-1,0), dAngle);
	CPoint pt3 = rotatepoint(ptCenterOCS, CPoint(imgSrc.GetWidth()-1,imgSrc.GetHeight()-1), dAngle);
	CPoint pt4 = rotatepoint(ptCenterOCS, CPoint(0,imgSrc.GetHeight()-1), dAngle);
	CRect rcNewImg(min4(pt1.x,pt2.x,pt3.x,pt4.x),min4(pt1.y,pt2.y,pt3.y,pt4.y),
		max4(pt1.x,pt2.x,pt3.x,pt4.x),max4(pt1.y,pt2.y,pt3.y,pt4.y));
	CPoint ptOffs = rcNewImg.TopLeft();
	CImageWrp imgTemp,imgNew;
	CRect rcValid(-1,-1,-1,-1);
	imgTemp.CreateNew(rcNewImg.Width()+1, rcNewImg.Height()+1, imgSrc.GetColorConvertor());
	imgTemp.InitRowMasks();
	for (int y = 0; y < rcNewImg.Height()+1; y++)
	{
		byte *pDstMask = imgTemp.GetRowMask(y);
		memset(pDstMask , 0, rcNewImg.Width()+1);
		for (int c = 0; c < imgTemp.GetColorsCount(); c++)
		{
			color *pDst = imgTemp.GetRow(y, c);
			memset(pDst, 0, (rcNewImg.Width()+1)*sizeof(color));
			for (int x = 0; x < rcNewImg.Width()+1; x++)
			{
				CPoint ptOCS(x+ptOffs.x,y+ptOffs.y); // in not-rotated OCS
				FPOINT ptNR = rotatepoint((CFPoint)ptCenterOCS, (CFPoint)ptOCS, -dAngle);
				Interp(imgSrc, ptNR, c, pDst[x], pDstMask[x]); 
				if (c == 0 && pDstMask[x])
				{
					if (rcValid.left == -1)
					{
						rcValid.left = rcValid.right = x;
						rcValid.top = rcValid.bottom = y;
					}
					else
					{
						rcValid.left = min(rcValid.left, x);
						rcValid.right = max(rcValid.right, x);
						rcValid.top = min(rcValid.top, y);
						rcValid.bottom = max(rcValid.bottom, y);
					}
				}
			}
		}
	}
	// Create new image
	ASSERT(rcValid.left != -1);
	imgNew.CreateNew(rcValid.Width()+1, rcValid.Height()+1, imgSrc.GetColorConvertor());
	imgNew.InitRowMasks();
	for (y = 0; y < rcValid.Height()+1; y++)
	{
		byte *pSrcMask = imgTemp.GetRowMask(y+rcValid.top)+rcValid.left;
		byte *pDstMask = imgNew.GetRowMask(y);
		memcpy(pDstMask, pSrcMask, rcValid.Width()+1);
		for (int c = 0; c < imgTemp.GetColorsCount(); c++)
		{
			color *pSrc = imgTemp.GetRow(y+rcValid.top, c)+rcValid.left;
			color *pDst = imgNew.GetRow(y, c);
			memcpy(pDst, pSrc, (rcValid.Width()+1)*sizeof(color));
		}
	}
	CPoint ptSrcOffs = imgSrc.GetOffset();
	imgNew.SetOffset(CPoint(ptSrcOffs.x+ptOffs.x+rcValid.left,ptSrcOffs.y+ptOffs.y+rcValid.top));
	return imgNew;
}

IMPLEMENT_DYNCREATE(CChromoCnvRotate,CCmdTargetEx);
// {6AD37CD6-16E2-4ADD-BC2A-B82F13C5002C}
GUARD_IMPLEMENT_OLECREATE(CChromoCnvRotate, _T("ChromoCnv.rotate"), 
0x6ad37cd6, 0x16e2, 0x4add, 0xbc, 0x2a, 0xb8, 0x2f, 0x13, 0xc5, 0x0, 0x2c);
// {5E091BEE-77A9-4736-923B-394944E2B846}
static const GUID clsidChromoCnvRotate = 
{ 0x5e091bee, 0x77a9, 0x4736, { 0x92, 0x3b, 0x39, 0x49, 0x44, 0xe2, 0xb8, 0x46 } };
DATA_OBJECT_INFO(IDS_CHROMOCNV_ROTATE, CChromoCnvRotate, "ChromoCnvRotate", clsidChromoCnvRotate, IDI_THUMBNAIL_OBJECT);
IMPLEMENT_DYNCREATE(CChromoCnvEqualize,CCmdTargetEx);
// {A6BB80F0-16B7-4AED-BF66-59D8B4C141E2}
GUARD_IMPLEMENT_OLECREATE(CChromoCnvEqualize, _T("ChromoCnv.equalize"), 
0xa6bb80f0, 0x16b7, 0x4aed, 0xbf, 0x66, 0x59, 0xd8, 0xb4, 0xc1, 0x41, 0xe2);
IMPLEMENT_DYNCREATE(CChromoCnvEmpty,CCmdTargetEx);
// {9CDB56D7-889C-448B-984C-1CE835388468}
static const GUID clsidChromoCnvEqualize = 
{ 0x9cdb56d7, 0x889c, 0x448b, { 0x98, 0x4c, 0x1c, 0xe8, 0x35, 0x38, 0x84, 0x68 } };
DATA_OBJECT_INFO(IDS_CHROMOCNV_EQUALIZE, CChromoCnvEqualize, "ChromoCnvEqualize", clsidChromoCnvEqualize, IDI_THUMBNAIL_OBJECT);
// {B66C20CE-B16F-4E74-A776-5DAE7C45D5C4}
GUARD_IMPLEMENT_OLECREATE(CChromoCnvEmpty, _T("ChromoCnv.empty"), 
0xb66c20ce, 0xb16f, 0x4e74, 0xa7, 0x76, 0x5d, 0xae, 0x7c, 0x45, 0xd5, 0xc4);
// {5BD36D45-154C-4E39-A806-56C73C45AD68}
static const GUID clsidChromoCnvEmpty = 
{ 0x5bd36d45, 0x154c, 0x4e39, { 0xa8, 0x6, 0x56, 0xc7, 0x3c, 0x45, 0xad, 0x68 } };
DATA_OBJECT_INFO(IDS_CHROMOCNV_EMPTY, CChromoCnvEmpty, "ChromoCnvEmpty", clsidChromoCnvEmpty, IDI_THUMBNAIL_OBJECT);

BEGIN_INTERFACE_MAP(CChromoCnvBase, CDataObjectBase)
END_INTERFACE_MAP()


LPUNKNOWN CChromoCnvBase::GetInterfaceHook(const void*p)
{
	if (*(IID*)p == IID_IUnknown || *(IID*)p == IID_IChromoConvert2)
		return (IChromoConvert2*)this;
	else
		return CDataObjectBase::GetInterfaceHook(p);
}

HRESULT CChromoCnvBase::Init(IChromoConvert2 *pCCPrev, LPCSTR lpszOpts)
{
	m_sptrCCPrev = pCCPrev;
	m_sOpts = lpszOpts;
	return S_OK;
}

HRESULT CChromoCnvBase::Recalc(IChromosome2 *pChromo)
{
	return S_OK;
}

double CChromoCnvRotate::CalcAngle(IChromosome2 *pChromo, ROTATEPARAMS *prp)
{
	BOOL bMirrorV;
	pChromo->IsMirrorV(&bMirrorV);
	pChromo->GetRotateParams(prp);
	BOOL bMirrorH = prp->bMirrorH;
	double dAngleR = bMirrorH?prp->dAngle-prp->dAngleVisible:prp->dAngle+prp->dAngleVisible;
	if (bMirrorV) dAngleR += PI;
	return dAngleR;
}

HRESULT CChromoCnvRotate::CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual)
{
	HRESULT hrPrev = m_sptrCCPrev->CCGetAxis(pChromo, dwFlags, ppAxis, pbIsManual);
	if (FAILED(hrPrev)) return hrPrev;
	IChromoAxis2 *pAxis = *ppAxis;
	FPOINT ptOffs = pAxis->GetOffset();
	pAxis->Offset(ptOffs); // in DCS now
	ROTATEPARAMS rp;
	double dAngle = CalcAngle(pChromo,&rp);
	bool bSmooth = pAxis->GetType()==IntegerAxis;
//	pAxis->Convert(FloatAxis);
	pAxis->Rotate(CFPoint(ptOffs.x+rp.ptCenter.x,ptOffs.y+rp.ptCenter.y), dAngle); // in RDCS
	if (bSmooth) pAxis->Smooth(g_AxisIterations, false);
	IUnknownPtr punk;
	pChromo->GetRotatedImage(AT_Original,&punk,NULL);
	CImageWrp ImgWrp(punk);
	if (ImgWrp.IsEmpty()) return E_FAIL;
	CPoint ptOffsRM = ImgWrp.GetOffset();
	pAxis->Offset(CFPoint(-ptOffsRM.x,-ptOffsRM.y)); // in DCS now
	if (rp.bMirrorH)
		pAxis->MirrorX(((CImageWrp)punk).GetWidth());
	SliceAxis(pAxis, ImgWrp);
	return S_OK;
}

HRESULT CChromoCnvRotate::CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual)
{
	HRESULT hrPrev = m_sptrCCPrev->CCGetCentromereCoord(pChromo, pCen, pbIsManual);
	if (FAILED(hrPrev)) return hrPrev;
	if (pCen->ptCen.x < 0) return S_OK;
	ROTATEPARAMS rp;
	double dAngle = CalcAngle(pChromo,&rp);
	CPoint ptImgOffs = pCen->ptOffset;
	IUnknownPtr punk;
	pChromo->GetRotatedImage(AT_Original,&punk,NULL);
	CImageWrp ImgWrp(punk);
	if (ImgWrp.IsEmpty()) return E_FAIL;
	CPoint ptOffsRM = ImgWrp.GetOffset();
	int nWidth = ImgWrp.GetWidth();
	int nHeight = ImgWrp.GetHeight();
	((CCentromere*)pCen)->Offset(pCen->ptOffset); // In DCS now
#if 1
	CFPoint ptCenter(rp.ptCenter.x+ptImgOffs.x,rp.ptCenter.y+ptImgOffs.y); // Center in DCS
	pCen->ptCen = rotatepoint(ptCenter, pCen->ptCen, dAngle);
	if (!IsInvAngle(pCen->dAngle))
	{
		pCen->dAngle += dAngle;
		pCen->aptCenDraw[0] = pCen->aptCenDraw[1] = INVFPOINT;
		if (rp.bMirrorH)
		{
			pCen->ptCen.x = 2*(ptOffsRM.x+ImgWrp.GetWidth()/2)-pCen->ptCen.x;
			pCen->dAngle = -pCen->dAngle;
		}
		_calcdrawpoints(pCen, NULL, ImgWrp);
	}
	else
	{
		pCen->aptCenDraw[0] = rotatepoint((CFPoint)ptCenter, pCen->aptCenDraw[0], dAngle);
		pCen->aptCenDraw[1] = rotatepoint((CFPoint)ptCenter, pCen->aptCenDraw[1], dAngle);
		if (rp.bMirrorH) ((CCentromere*)pCen)->MirrorX(ImgWrp.GetWidth()-1);
	}
	((CCentromere*)pCen)->Offset(CPoint(-ptOffsRM.x,-ptOffsRM.y));
#else
	CPoint ptCenter(rp.ptCenter.x+ptImgOffs.x,rp.ptCenter.y+ptImgOffs.y); // Center in DCS
	pCen->ptCen = rotatepoint(ptCenter, pCen->ptCen, dAndle);
	if (pCen->aptCenDraw[0].x > -10000)
	{
		pCen->aptCenDraw[0] = rotatepoint(ptCenter, pCen->aptCenDraw[0], dAndle);
		pCen->aptCenDraw[1] = rotatepoint(ptCenter, pCen->aptCenDraw[1], dAndle);
	}
	CPoint ptOffsRM = ImgWrp.GetOffset();
	((CCentromere*)pCen)->Offset(CPoint(-ptOffsRM.x,-ptOffsRM.y));
	if (rp.bMirrorH)
		((CCentromere*)pCen)->MirrorX(ImgWrp.GetWidth()-1);
#endif
	return S_OK;
}

HRESULT CChromoCnvRotate::CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk)
{
	HRESULT hrPrev = m_sptrCCPrev->CCGetImage(pChromo, ppunk);
	if (FAILED(hrPrev)) return hrPrev;
	CChromoSettings Settings;
	ROTATEPARAMS rp;
	double dAngle = CalcAngle(pChromo,&rp);
//	if (dAngle != 0)
	{
		CImageWrp ImgWrp,ImgWrp1;
		ImgWrp.Attach(*ppunk);
		if (ImgWrp.IsEmpty()) return E_FAIL;
#if 1
		CIntervalImage *pIntvl = new CIntervalImage;
		CIntervalImage *pIntvl1 = NULL;
		pIntvl->MakeByImage(ImgWrp);
		CPoint ptCenter;
		rottrans(ImgWrp, pIntvl, dAngle, ImgWrp1, pIntvl1, &ptCenter, g_BlackOnWhite!=0, false);
		delete pIntvl;
		delete pIntvl1;
#else
		ImgWrp1 = RotateImage(ImgWrp, rp.ptCenter, dAngle);
#endif
		// AAM: добавляю медианную фильтрацию для края изображения
		for(int nPass=0; nPass<3; nPass++)
		{
			int cx = ImgWrp1.GetWidth();
			int cy = ImgWrp1.GetHeight();
			int nColors = ImgWrp1.GetColorsCount();
			smart_alloc(msk,byte,(cx+2)*(cy+2));
			ZeroMemory(msk, (cx+2)*(cy+2));
			for(int y=0; y<cy; y++)
			{	//copy mask
				byte *p = ImgWrp1.GetRowMask(y);
				byte *pmsk = msk + (y+1)*(cx+2) + 1;
				CopyMemory(pmsk, p, cx*sizeof(byte));
			} // теперь копия маски - в массиве msk[cx+2,cy+2], c полями по 1 пикселу

			for(int y=0; y<cy; y++)
			{// сглаживание - медианой по 9 точкам
				byte *mSrc = msk + (y+1)*(cx+2) + 1;
				byte *mSrcM = mSrc - (cx+2), *mSrcP = mSrc + (cx+2);
				byte *mDst = ImgWrp1.GetRowMask(y);
				int yp1=min(y+1,cy-1), ym1=max(y-1,0);
				for(int x=0; x<cx; x++)
				{
					int n=0;
					if(mSrcM[-1]>=128) n++; if(mSrcM[0]>=128) n++; if(mSrcM[1]>=128) n++;
					if(mSrc[-1]>=128) n++; if(mSrc[0]>=128) n++; if(mSrc[1]>=128) n++;
					if(mSrcP[-1]>=128) n++; if(mSrcP[0]>=128) n++; if(mSrcP[1]>=128) n++;
					*mDst= (n>=5) ? 255 : 0;
					if((*mDst) && (mSrc[0]<128))
					{
						for(int npane=0; npane<nColors; npane++)
						{
							int c=0, n1=0;
							color *pSrcM = ImgWrp1.GetRow(y-1,npane) + x;
							color *pSrc = ImgWrp1.GetRow(y,npane) + x;
							color *pSrcP = ImgWrp1.GetRow(y+1,npane) + x;

							// куча дурацких строчек, сводящихся к тому, что учитываем лишь крайние точки
							if(mSrcM[-1]>=128 && (mSrc [-1]<128 || mSrcM[ 0]<128)) { c+=pSrcM[-1]; n1++; }
							if(mSrcM[ 0]>=128 && (mSrcM[-1]<128 || mSrcM[+1]<128)) { c+=pSrcM[ 0]; n1++; }
							if(mSrcM[+1]>=128 && (mSrc [+1]<128 || mSrcM[ 0]<128)) { c+=pSrcM[+1]; n1++; }
							if(mSrc [-1]>=128 && (mSrcM[-1]<128 || mSrcP[-1]<128)) { c+=pSrc [-1]; n1++; }
							if(mSrc [+1]>=128 && (mSrcM[+1]<128 || mSrcP[+1]<128)) { c+=pSrc [+1]; n1++; }
							if(mSrcP[-1]>=128 && (mSrc [-1]<128 || mSrcP[ 0]<128)) { c+=pSrcP[-1]; n1++; }
							if(mSrcP[ 0]>=128 && (mSrcM[-1]<128 || mSrcM[+1]<128)) { c+=pSrcP[ 0]; n1++; }
							if(mSrcP[+1]>=128 && (mSrc [+1]<128 || mSrcP[ 0]<128)) { c+=pSrcP[+1]; n1++; }
							pSrc[0] = c/max(n1,1);
						}
					}
					mDst++; mSrc++;	mSrcM++; mSrcP++;
				}
			}
		}

		(*ppunk)->Release();
		*ppunk = ImgWrp1;
		if (*ppunk) (*ppunk)->AddRef();
	}
	if (rp.bMirrorH)
		MirrorImage((CImageWrp)(*ppunk));
	return S_OK;
}

HRESULT CChromoCnvEqualize::CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual)
{
	return m_sptrCCPrev->CCGetAxis(pChromo, dwFlags, ppAxis, pbIsManual);
}

HRESULT CChromoCnvEqualize::CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual)
{
	return m_sptrCCPrev->CCGetCentromereCoord(pChromo, pCen, pbIsManual);
}

HRESULT CChromoCnvEqualize::CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk)
{
	HRESULT hrPrev = m_sptrCCPrev->CCGetImage(pChromo, ppunk);
	if (FAILED(hrPrev)) return hrPrev;
	CImageWrp ImgWrp(*ppunk);
	CEqualize equ(ImgWrp);
	(*ppunk)->Release();
	*ppunk = equ.Process(ImgWrp);
	(*ppunk)->AddRef();
	return S_OK;
}

HRESULT CChromoCnvEmpty::CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual)
{
	return pChromo->GetAxis2(dwFlags&~AT_Rotated|AT_Original, *ppAxis, NULL);
}

HRESULT CChromoCnvEmpty::CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual)
{
	return pChromo->GetCentromereCoord2(AT_Original|AT_Centromere, pCen, pbIsManual);
}

HRESULT CChromoCnvEmpty::CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk)
{
	sptrINamedDataObject2 sptrNO1(pChromo);
	IUnknownPtr ptrPar;
	sptrNO1->GetParent(&ptrPar);
	IMeasureObjectPtr sptrM(ptrPar);
	if( sptrM == 0 )
		return E_UNEXPECTED;
	return sptrM->GetImage(ppunk);
}

CChromoCnvSeq::CChromoCnvSeq()
{
	m_nSize = 0;
	m_pCnvs = NULL;
	m_bAxisInited = m_bCenInited = m_bAxisManual = m_bCenManual = false;
}

HRESULT CChromoCnvSeq::Init(IChromoConvert2 *, LPCTSTR lpSeq)
{
	ReleaseAll();
	CString s(lpSeq);
	m_sInit = s;
	int n = s.Find(_T(';'));
	while (n != -1)
	{
		CString s1 = s.Left(n);
		s = s.Mid(n+1);
		m_saProgIDs.Add(s1);
		n = s.Find(_T(';'));
	}
	m_saProgIDs.Add(s);
	int nSize = m_saProgIDs.GetSize();
	IChromoConvert2 *pCCPrev = NULL;
	m_pCnvs = new IChromoConvert2*[nSize];
	memset(m_pCnvs, 0, nSize);
	m_nSize = 0;
	for (int i = 0; i < nSize; i++)
	{
		n = m_saProgIDs[i].Find(_T(':'));
		CString sName = n==-1?m_saProgIDs[i]:m_saProgIDs[i].Left(n);
		CString sOpts = n==-1?_T(""):m_saProgIDs[i].Mid(n+1);
		BOOL bOblig = sOpts.Find(_T('o'))!=-1;
		IUnknownPtr punkCC(::CreateTypedObject(sName),false);
		IChromoConvert2Ptr sptrCC(punkCC);
		if (sptrCC == 0)
		{
			m_nSize = i;
			ReleaseAll();
			return E_UNEXPECTED;
		}
		m_pCnvs[m_nSize] = sptrCC;
		m_pCnvs[m_nSize]->AddRef();
		m_pCnvs[m_nSize]->Init(pCCPrev,sOpts);
		pCCPrev = m_pCnvs[m_nSize];
		m_nSize++;
	}
	return S_OK;
}

CChromoCnvSeq::~CChromoCnvSeq()
{
	ReleaseAll();
}

void CChromoCnvSeq::ReleaseAll()
{
	for (int i = 0; i < m_nSize; i++)
	{
		m_pCnvs[i]->Release();
		m_pCnvs[i] = NULL;
	}
	delete m_pCnvs;
	m_nSize = 0;
	m_pCnvs = NULL;
	m_saProgIDs.RemoveAll();
}

CChromoAxis& CChromoCnvSeq::GetAxis(IChromosome2 *pChromo, DWORD dwFlags, BOOL *pbIsManual)
{
	CChromoAxis *pAxis = (dwFlags&AT_ShortAxis)?&m_ShortAxis:&m_LongAxis;
	IChromoAxis2 *pIAxis = pAxis;
	if (!m_bAxisInited && m_pCnvs && m_nSize > 0)
	{
		GetImage(pChromo);
		HRESULT hr = m_pCnvs[m_nSize-1]->CCGetAxis(pChromo, dwFlags, &pIAxis, &m_bAxisManual);
		if (SUCCEEDED(hr))
			m_bAxisInited = true;
		else
			pAxis->Init(NoData, 0);
	}
	return *pAxis;
}

HRESULT CChromoCnvSeq::CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual)
{
	CChromoAxis& Axis = GetAxis(pChromo, dwFlags, pbIsManual);
	*ppAxis = &Axis;
	if (*ppAxis) (*ppAxis)->AddRef();
	return S_OK;
}


CENTROMERE CChromoCnvSeq::GetCentromereCoord(IChromosome2 *pChromo, BOOL *pbIsManual)
{
	if (!m_bCenInited && m_pCnvs && m_nSize > 0)
	{
		GetImage(pChromo);
		HRESULT hr = m_pCnvs[m_nSize-1]->CCGetCentromereCoord(pChromo, &m_Cen, pbIsManual);
		if (SUCCEEDED(hr))
		{
			m_bCenInited = true;
		}
		else
			m_Cen.ptCen = m_Cen.aptCenDraw[0] = m_Cen.aptCenDraw[1] = INVFPOINT;
	}
	return m_Cen;
}

HRESULT CChromoCnvSeq::CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual)
{
	CENTROMERE Cen = GetCentromereCoord(pChromo, pbIsManual);
	*pCen = Cen;
	return S_OK;
}

//static bool s_bInside = false;
IUnknownPtr CChromoCnvSeq::GetImage(IChromosome2 *pChromo)
{
	if (m_punkImg == 0 && m_pCnvs && m_nSize > 0)
	{
		if (FAILED(m_pCnvs[m_nSize-1]->CCGetImage(pChromo, &m_punkImg)))
			m_punkImg.Release();
	}
	return m_punkImg;
}

HRESULT CChromoCnvSeq::CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk)
{
	*ppunk = GetImage(pChromo);
	if (*ppunk) (*ppunk)->AddRef();
	return *ppunk?S_OK:E_FAIL;
}


HRESULT CChromoCnvSeq::Recalc(IChromosome2 *)
{
	m_punkImg = 0;
	m_bCenInited = m_bAxisInited = false;
	m_ShortAxis.Init(NoData, 0);
	m_LongAxis.Init(NoData, 0);
	return S_OK;
}

HRESULT CChromoCnvSeq::GetInitString(BSTR *pbstrInitStr)
{
	*pbstrInitStr = m_sInit.AllocSysString();
	return S_OK;
}

