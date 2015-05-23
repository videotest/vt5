#include "StdAfx.h"
#include "Cromos.h"
#include "ChildObjects.h"
#include "ChromParam.h"
#include "Object5.h"
#include "ParamsKeys.h"
#include "Settings.h"
#include "Debug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const char *CChromosome::c_szType = "ChromosomeAxisAndCentromere";

IMPLEMENT_DYNCREATE(CChromosome, CDataObjectBase);


BEGIN_INTERFACE_MAP(CChromosome, CDataObjectBase)
	INTERFACE_PART(CChromosome, IID_IDrawObject, Draw)
	INTERFACE_PART(CChromosome, IID_IDrawObject2, Draw)
	INTERFACE_PART(CChromosome, IID_IChromosome, Chromos)
	INTERFACE_PART(CChromosome, IID_IChromosome2, Chromos)
	INTERFACE_PART(CChromosome, IID_IClonableObject, Clone)
END_INTERFACE_MAP()

// {EF849B40-FA15-11D5-BA8D-0002B38ACE2C}
static const GUID clsidChromosome = 
{ 0xef849b40, 0xfa15, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {2D97AA80-FA11-11D5-BA8D-0002B38ACE2C}
GUARD_IMPLEMENT_OLECREATE(CChromosome, "Chromos.Chromosome", 
0x2d97aa80, 0xfa11, 0x11d5, 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c);

DATA_OBJECT_INFO(IDS_CHROMOSOME_TYPE, CChromosome, CChromosome::c_szType, clsidChromosome, IDI_THUMBNAIL_OBJECT);


IMPLEMENT_UNKNOWN(CChromosome, Chromos);
IMPLEMENT_UNKNOWN(CChromosome, Clone);

CChromosome::CChromosome()
{
	m_ptOffs = CPoint(0,0);
	m_bDrawLong = false;
	m_bShortAxisManual = m_bLongAxisManual = m_bCenManual = FALSE;
	m_clrAxis = -1;
	memset(&m_Rotate, 0, sizeof(m_Rotate));
	m_Rotate.bMirrorV = TRUE;
	m_Rotate.dAngle = 0.;
	m_Rotate.dAngleVisible = 0.;
	m_nManualClass = 0;
	m_bEqualizeFlag = FALSE;
	m_KaryoSeq.Init(NULL, _T("ChromoCnvEmpty:o;ChromoCnvRotate"));
	m_bRotateVer4 = false;
	m_bCentromere5 = false;
}

CChromosome::~CChromosome()
{
}

void CChromosome::DoDraw(CDC &dc)
{
	if (!g_ShowAxisCmere) return;
	// 1. Obtain global settings
	CPoint ptOffs(GetImgOffset());
	if (ptOffs == INVPOINT) return;
	double	fZoom = ::GetZoom(m_punkDrawTarget);
	CPoint	ptScroll = ::GetScrollPos(m_punkDrawTarget);
	DWORD dwFlags = m_bShortAxisManual&&!m_bLongAxisManual?
		AT_ShortAxis|AT_Centromere:
		m_LongAxis.GetType()==FloatAxis?AT_LongAxis|AT_Centromere:AT_ShortAxis|AT_Centromere;
	if (m_pOptions)
	{ // Karyo
		// [vanek] BT2000: 3680 - use mask
		if( m_pOptions->dwMask & domOffset )
			ptOffs = m_pOptions->ptOffset;

		if( m_pOptions->dwMask & domScroll )
			ptScroll = m_pOptions->ptScroll;
        
		if( (m_pOptions->dwMask & domZoom) && (m_pOptions->dZoom > 0.) )
			fZoom = m_pOptions->dZoom;
		
		if( m_pOptions->dwMask & domFlags )
			dwFlags = m_pOptions->dwFlags;
	}
	// 2. Draw axis
	if (dwFlags&(AT_ShortAxis|AT_LongAxis))
	{
		CChromoAxis *pAxis = NULL;
		CChromoAxis Axis1;
		// If old convertor (ErectCC)
		if (m_sptrChromoConvert != 0 && (dwFlags&AT_Original) == 0)
//		if (m_sptrChromoConvert != 0)
		{
			long nSize = 0;
			HRESULT hr = m_sptrChromoConvert->CCGetAxisSize(&m_xChromos, dwFlags, &nSize, NULL);
			if (hr == S_OK && nSize > 0)
			{
				Axis1.Init(FloatAxis, nSize);
				m_sptrChromoConvert->CCGetAxis(&m_xChromos, dwFlags|AT_AxisDouble, (POINT*)Axis1.m_fptAxis);
				pAxis = &Axis1;
			}
		}
		else
			pAxis = &AxisByFlags(dwFlags&(AT_ShortAxis|AT_LongAxis|AT_Rotated)|AT_Draw);
		if (pAxis) // draw
		{
			int nPoints = pAxis->m_nSize,i;
			if (nPoints)
			{
				int nColor = m_clrAxis==-1?g_AxisColor:m_clrAxis;
				CPen pen(PS_SOLID, 0, nColor);
				CPen *pOld = dc.SelectObject(&pen);
				POINT *pt = new POINT[nPoints];
				if (pAxis->m_Type == IntegerAxis)
				{
					POINT *pPoints = pAxis->m_iptAxis;
					for (i = 0; i < nPoints; i++)
					{
						pt[i].x = (int)(fZoom*(pPoints[i].x+ptOffs.x+0.5)-ptScroll.x);
						pt[i].y = (int)(fZoom*(pPoints[i].y+ptOffs.y+0.5)-ptScroll.y);
					}
				}
				else if (pAxis->m_Type == FloatAxis)
				{
					FPOINT *pPoints = pAxis->m_fptAxis;
					for (i = 0; i < nPoints; i++)
					{
						pt[i].x = (int)(fZoom*(pPoints[i].x+ptOffs.x)-ptScroll.x+.5);
						pt[i].y = (int)(fZoom*(pPoints[i].y+ptOffs.y)-ptScroll.y+.5);
					}
				}
				else
					memset(pt, 0, sizeof(POINT)*nPoints);
				dc.Polyline(pt, nPoints);
				if (fZoom > 1. &&  g_ShowNodes)
				{
					for (i = 0; i < nPoints; i++)
					{
						if ((dwFlags&AT_ShortAxis) || i%5==0 && fZoom >= 5.)
							dc.Ellipse(pt[i].x-2, pt[i].y-2, pt[i].x+2, pt[i].y+2);
						else if (fZoom >= 3.)
							dc.Ellipse(pt[i].x-1, pt[i].y-1, pt[i].x+1, pt[i].y+1);
					}
				}
				delete pt;
				dc.SelectObject(pOld);
			}
		}
	}
	// 3. Draw centromere  
	if (m_bCentromere5) ConvertCentromere();
	if ((dwFlags&AT_Centromere) && m_Cen.ptCen.x >= 0)
	{
		CENTROMERE Cen;
		HRESULT hr = S_FALSE;
		if (m_sptrChromoConvert != 0 && (dwFlags&AT_Original) == 0) // If old convertor
		{
			CPoint pt,apt[2];
			if (SUCCEEDED(m_sptrChromoConvert->CCGetCentromereCoord(&m_xChromos, dwFlags&AT_Rotated, &pt, apt, NULL)))
			{
				Cen.ptCen = (CFPoint)pt;
				Cen.aptCenDraw[0] = (CFPoint)apt[0];
				Cen.aptCenDraw[1] = (CFPoint)apt[1];
				Cen.nCentromere= -1;
				Cen.dAngle = 0.;
				Cen.ptOffset = CPoint(0,0);
			}
			else
				Cen = m_Cen;
		}
		else if (dwFlags&AT_Rotated) // if new convertor
			Cen = m_KaryoSeq.GetCentromereCoord(&m_xChromos,NULL);
		else
			Cen = m_Cen;
		int nColor = g_CentromereColor;
		CPen pen(PS_SOLID, 0, nColor);
		CPen *pOld = dc.SelectObject(&pen);
		if (Cen.aptCenDraw[0].x != Cen.aptCenDraw[1].x || Cen.aptCenDraw[0].y != Cen.aptCenDraw[1].y)
		{
			CPoint pt1((int)(fZoom*(Cen.aptCenDraw[0].x+ptOffs.x)-ptScroll.x),
				(int)(fZoom*(Cen.aptCenDraw[0].y+ptOffs.y)-ptScroll.y));
			CPoint pt2((int)(fZoom*(Cen.aptCenDraw[1].x+ptOffs.x)-ptScroll.x),
				(int)(fZoom*(Cen.aptCenDraw[1].y+ptOffs.y)-ptScroll.y));
			dc.MoveTo(pt1);
			dc.LineTo(pt2);
			if (g_ShowNodes)
			{
				int n = 4;
				CPoint pt1((int)(fZoom*(Cen.ptCen.x+ptOffs.x)-ptScroll.x-n),
					(int)(fZoom*(Cen.ptCen.y+ptOffs.y)-ptScroll.y-n));
				CPoint pt2((int)(fZoom*(Cen.ptCen.x+ptOffs.x)-ptScroll.x+n),
					(int)(fZoom*(Cen.ptCen.y+ptOffs.y)-ptScroll.y+n));
				dc.Ellipse(pt1.x, pt1.y, pt2.x, pt2.y);
			}
		}
		else if (!m_bCenManual)
		{
			int n = 4;
			CPoint pt1((int)(fZoom*(Cen.ptCen.x+ptOffs.x)-ptScroll.x-n),
				(int)(fZoom*(Cen.ptCen.y+ptOffs.y)-ptScroll.y-n));
			CPoint pt2((int)(fZoom*(Cen.ptCen.x+ptOffs.x)-ptScroll.x+n),
				(int)(fZoom*(Cen.ptCen.y+ptOffs.y)-ptScroll.y+n));
			dc.Ellipse(pt1.x, pt1.y, pt2.x, pt2.y);
		}
		dc.SelectObject(pOld);
	}
}

static void SerializeImage(CStreamEx& ar, IImage3Ptr &image, SerializeParams *pparams )
{
	if (ar.IsStoring())
	{
		BOOL bImageExists = ::CheckInterface(image, IID_INamedDataObject2) == true;
		ar << bImageExists;
		if (bImageExists)
		{
			GuidKey ParentKey = GetParentKey(image);
			long lFlags = GetObjectFlags(image);
			ar << ParentKey;
			ar << lFlags;
			::StoreDataObjectToArchive(ar, image, pparams);
		}

	}
	else
	{
		BOOL bImageExists = false;
		ar >> bImageExists;
		if (bImageExists)
		{
			long  lFlags;
			GuidKey	lParentKey;
			ar >> lParentKey;
			ar >> lFlags;
			IUnknown * punk = ::LoadDataObjectFromArchive(ar, pparams);
			image = punk;
			if (punk)
				punk->Release();
		}
	}
}

bool CChromosome::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if (!CDataObjectBase::SerializeObject(ar, pparams))
		return false;	

	IImage3Ptr imgDummy;
	CChromoAxis AxisDummy;
	CPoint ptDummy;
	if (ar.IsStoring())
	{
		long lVersion = 6;
		ar << lVersion;
		ar << m_Rotate.dAngle;
		ar << m_Rotate.ptCenter;
		ar << m_Rotate.ptOffs;
		ar << m_Rotate.ptOffsR;
		ar << m_Rotate.bManual;
		ar << m_ptOrig;
		m_ShortAxis.Serialize(ar);
		m_LongAxis.Serialize(ar);
		ar << m_bShortAxisManual;
		ar << m_bLongAxisManual;
		ar << m_Cen.nCentromere;
		ar << m_bCenManual;
		SerializeImage(ar, m_sptrImageOrg, pparams);
		// Version 3
		ar << m_Rotate.bMirrorH;
		ar << m_Rotate.bMirrorV;
		ar << m_Rotate.dAngleVisible; 
		BOOL bErectCC = m_sptrChromoConvert != 0;
		ar << bErectCC;
		ar << m_bEqualizeFlag;
	}
	else
	{
		long lVersion;
		ar >> lVersion;
		ar >> m_Rotate.dAngle;
		ar >> m_Rotate.ptCenter;
		ar >> m_Rotate.ptOffs;
		ar >> m_Rotate.ptOffsR;
		ar >> m_Rotate.bManual;
		ar >> m_ptOrig;
		m_ShortAxis.Serialize(ar);
		if (lVersion <= 3) AxisDummy.Serialize(ar);
		m_LongAxis.Serialize(ar);
		if (lVersion <= 3) AxisDummy.Serialize(ar);
		ar >> m_bShortAxisManual;
		ar >> m_bLongAxisManual;
		if (lVersion <= 5)
		{
			CPoint pt;
			ar >> pt;
			m_Cen.ptCen = (CFPoint)pt;
			ar >> pt;
			m_Cen.aptCenDraw[0] = (CFPoint)pt;
			ar >> pt;
			m_Cen.aptCenDraw[1] = (CFPoint)pt;
		}
		else
		{
			ar >> m_Cen.nCentromere;
			m_Cen.dAngle = INVANGLE;
			m_Cen.ptCen = m_Cen.aptCenDraw[0] = m_Cen.aptCenDraw[1] = INVFPOINT;
		}
		if (lVersion <= 3) ar >> ptDummy; //m_ptCenR;
		if (lVersion <= 3) ar >> ptDummy; //m_ptCenDraw1R;
		if (lVersion <= 3) ar >> ptDummy; //m_ptCenDraw2R;
		ar >> m_bCenManual;
		if (lVersion >= 2) SerializeImage(ar, m_sptrImageOrg, pparams);
		if (lVersion <= 3) SerializeImage(ar, imgDummy, pparams);
		if (lVersion <= 3)
		{
			SerializeImage(ar, imgDummy, pparams);
			m_bEqualizeFlag = imgDummy != 0;
		}
		if (lVersion <= 3) SerializeImage(ar, imgDummy, pparams);
		m_ptOffs = m_Rotate.ptOffs;
		if (lVersion >= 3)
		{
			ar >> m_Rotate.bMirrorH;
			ar >> m_Rotate.bMirrorV;
			ar >> m_Rotate.dAngleVisible; 
			BOOL bErectCC;
			ar >> bErectCC;
			if (bErectCC)
			{
				_bstr_t bstrProgId(pszErectCCProgID);
				m_sptrChromoConvert.CreateInstance((LPOLESTR)bstrProgId);
			}
		}
		if (lVersion <= 3) // In previous versions offset in axes was not used and (0,0).
		{
			m_ShortAxis.SetOffset((CFPoint)GetImgOffset());
			m_LongAxis.SetOffset((CFPoint)GetImgOffset());
		}
		if (lVersion >= 4) ar >> m_bEqualizeFlag;
		m_bRotateVer4 = lVersion <= 4;
		m_bCentromere5 = true;
	}

	return true;
}

IUnknown* CChromosome::Clone()
{
	IUnknown *punk = CreateTypedObject("ChromosomeAxisAndCentromere");
	IChromosome2Ptr sptr(punk);//punk will be returned and must not be released
	sptr->SetAxis2(AT_ShortAxis, &m_ShortAxis, m_bShortAxisManual);
	sptr->SetAxis2(AT_LongAxis, &m_LongAxis, m_bLongAxisManual);
	sptr->SetCentromereCoord2(AT_NotRecalc, &m_Cen, m_bCenManual);
/*	if (CPoint(m_Cen.aptCenDraw[0])==CPoint(-1,-1))
		sptr->SetCentromereCoord(AT_NotRecalc, m_Cen.ptCen, NULL, m_bCenManual);
	else
	{
		CPoint apt[2];
		apt[0] = m_Cen.aptCenDraw[0];
		apt[1] = m_Cen.aptCenDraw[1];
		sptr->SetCentromereCoord(AT_NotRecalc, m_Cen.ptCen, apt, m_bCenManual);
	}*/
	if (m_sptrImageOrg != 0)
		sptr->SetOriginalImage(m_sptrImageOrg);
	if (m_bRotateVer4) ConvertRotate();
	sptr->SetRotateParams(&m_Rotate);
	sptr->SetOrigin(m_ptOrig);
	sptr->SetOffset(m_ptOffs);
	sptr->SetColor(m_clrAxis, 0);
	return punk;
}


DWORD CChromosome::GetNamedObjectFlags()
{
	DWORD dw = CDataObjectBase::GetNamedObjectFlags();
	dw |= nofStoreByParent;
	return dw;
}

void CChromosome::ConvertRotate()
{
	CPoint ptImgOffs = GetImgOffset();
	if (ptImgOffs == INVPOINT) return;
	m_Rotate.ptOffs = CPoint(m_Rotate.ptOffs.x-ptImgOffs.x,m_Rotate.ptOffs.y-ptImgOffs.y);
	m_Rotate.ptOffsR = CPoint(m_Rotate.ptOffsR.x-ptImgOffs.x,m_Rotate.ptOffsR.y-ptImgOffs.y);
	m_Rotate.ptCenter = CPoint(m_Rotate.ptCenter.x-ptImgOffs.x,m_Rotate.ptCenter.y-ptImgOffs.y);
	m_bRotateVer4 = false;
}

void CChromosome::ConvertCentromere()
{
	if (IsInvPoint(m_Cen.ptCen))
	{
		if (m_Cen.nCentromere == -1) return;
		ASSERT(m_Cen.nCentromere >= -1);
		FPOINT fp = m_LongAxis.GetFPoint(m_Cen.nCentromere);
		m_Cen.ptCen = fp;
	}
	else if (m_Cen.nCentromere <= -1)
		m_Cen.nCentromere = _FindCentromereByPoint(m_Cen.ptCen, m_LongAxis);
	if ((IsInvPoint(m_Cen.aptCenDraw[0]) || IsInvPoint(m_Cen.aptCenDraw[1]) || IsInvAngle(m_Cen.dAngle)) &&
		m_Cen.nCentromere != -1)
	{
		IUnknownPtr punk;
		GetImage(&punk);
		if (!_calcdrawpoints(&m_Cen, &m_LongAxis, (CImageWrp)punk)) // calculates draw points and angle
			return;
	}
	m_Cen.ptOffset = CPoint(0,0);
	m_bCentromere5 = false;
}


CChromoAxis &CChromosome::AxisByFlags(int nFlags)
{
	if (nFlags&AT_Rotated)
		return m_KaryoSeq.GetAxis(&m_xChromos, nFlags, NULL);
	else
	{
		CChromoAxis *pAxis = (nFlags&AT_LongAxis)?&m_LongAxis:&m_ShortAxis;
		if ((nFlags&AT_Draw)&&(nFlags&AT_LongAxis))
		{
			if (m_DrawAxis.GetType() == NoData)
			{
				m_DrawAxis.Init(pAxis);
				IUnknownPtr punk;
				GetImage(&punk);
				SliceAxis(&m_DrawAxis, (CImageWrp)punk);
			}
			pAxis = &m_DrawAxis;
		}
		pAxis->SetOffset((CFPoint)GetImgOffset());
		return *pAxis;
	}
}

struct _RotateContext
{
	CPoint ptImgOffs;
	CPoint ptOffsR;
	CPoint ptCenter;
	CPoint ptOffs;
	_RotateContext(CChromosome *p)
	{
		if (p->m_bRotateVer4) p->ConvertRotate();
		ptImgOffs = p->GetImgOffset();
		ptOffsR = CPoint(ptImgOffs.x+p->m_Rotate.ptOffsR.x,ptImgOffs.y+p->m_Rotate.ptOffsR.y);
		ptCenter = CPoint(ptImgOffs.x+p->m_Rotate.ptCenter.x,ptImgOffs.y+p->m_Rotate.ptCenter.y);
		ptOffs = CPoint(ptImgOffs.x+p->m_Rotate.ptOffs.x,ptImgOffs.y+p->m_Rotate.ptOffs.y);
	}
};

CPoint CChromosome::Rot2Obj(CPoint pt)
{
	if (m_bRotateVer4) ConvertRotate();
	CPoint ptImgOffs = GetImgOffset();
	CPoint ptOffsR(ptImgOffs.x+m_Rotate.ptOffsR.x,ptImgOffs.y+m_Rotate.ptOffsR.y);
	CPoint ptCenter(ptImgOffs.x+m_Rotate.ptCenter.x,ptImgOffs.y+m_Rotate.ptCenter.y);
	CPoint ptOffs(ptImgOffs.x+m_Rotate.ptOffs.x,ptImgOffs.y+m_Rotate.ptOffs.y);
	CPoint ptDoc(pt.x+ptOffsR.x,pt.y+ptOffsR.y);
	CPoint ptRDoc = rotatepoint(ptCenter, ptDoc, -m_Rotate.dAngle);
	CPoint ptObj(ptRDoc.x-ptOffs.x,ptRDoc.y-ptOffs.y);
	return ptObj;
}

CPoint CChromosome::Obj2Rot(CPoint pt)
{
	if (m_bRotateVer4) ConvertRotate();
	CPoint ptImgOffs = GetImgOffset();
	CPoint ptOffsR(ptImgOffs.x+m_Rotate.ptOffsR.x,ptImgOffs.y+m_Rotate.ptOffsR.y);
	CPoint ptCenter(ptImgOffs.x+m_Rotate.ptCenter.x,ptImgOffs.y+m_Rotate.ptCenter.y);
	CPoint ptOffs(ptImgOffs.x+m_Rotate.ptOffs.x,ptImgOffs.y+m_Rotate.ptOffs.y);
	CPoint ptDoc(pt.x+ptOffs.x,pt.y+ptOffs.y);
	CPoint ptRDoc = rotatepoint(ptCenter, ptDoc, m_Rotate.dAngle);
	CPoint ptRot(ptRDoc.x-ptOffsR.x,ptRDoc.y-ptOffsR.y);
	return ptRot;
}

void CChromosome::Rot2Obj(CChromoAxis &Axis)
{
	_RotateContext ctx(this);
	Axis.Offset(CFPoint(ctx.ptOffsR)); // RDCS
	Axis.Rotate((CFPoint)ctx.ptCenter, -m_Rotate.dAngle); // DCS
	Axis.Offset(CFPoint(-ctx.ptOffs.x,-ctx.ptOffs.y)); // RDCS
/*	int nSize = Axis.m_nSize;
	for (int i = 0; i < nSize; i++)
	{
		CPoint pt1 = Axis.m_iptAxis[i];
		CPoint pt2 = Rot2Obj(pt1);
		Axis.m_iptAxis[i] = pt2;
	}*/
}

void CChromosome::Obj2Rot(CChromoAxis &Axis)
{
	int nSize = Axis.m_nSize;
	for (int i = 0; i < nSize; i++)
	{
		CPoint pt1 = Axis.m_iptAxis[i];
		CPoint pt2 = Obj2Rot(pt1);
		Axis.m_iptAxis[i] = pt2;
	}
}

CPoint CChromosome::GetImgOffset()
{
	IUnknownPtr punkImg;
	sptrINamedDataObject2 sptrNO1(GetControllingUnknown());
	if( sptrNO1 == 0 )
		return INVPOINT;

	IUnknownPtr ptrPar;
	sptrNO1->GetParent(&ptrPar);
	if( ptrPar == 0 )
		return INVPOINT;

	IMeasureObjectPtr sptrM(ptrPar);
	if( sptrM == 0 )
		return INVPOINT;

	sptrM->GetImage(&punkImg);
	if( punkImg == 0 )
		return INVPOINT;

	IImage2Ptr sptrImg(punkImg);
	if (sptrImg == 0)
	{
		ASSERT(FALSE);
		return INVPOINT;
	}
	CPoint ptOffs;
	sptrImg->GetOffset(&ptOffs);
	return ptOffs;
}


HRESULT CChromosome::GetImage(IUnknown **ppunk, BOOL bOrig)
{
	if (m_sptrImageOrg != 0 && bOrig)
	{
		*ppunk = m_sptrImageOrg;
		(*ppunk)->AddRef();
		return S_OK;
	}
	sptrINamedDataObject2 sptrNO1(GetControllingUnknown());
	IUnknownPtr ptrPar;
	sptrNO1->GetParent(&ptrPar);
	IMeasureObjectPtr sptrM(ptrPar);
	if( sptrM == 0 )
	{
		*ppunk = NULL;
		return S_FALSE;
	}
	return sptrM->GetImage(ppunk);
}

bool CChromosome::IsMirrorV()
{
	if (m_Rotate.bMirrorV == FALSE)
		return false;
	if (::GetValueInt(::GetAppUnknown(), "Chromos", "MirrorInCaryo", 1) == 0)
		return false;
	sptrINamedDataObject2 sptrNO1(GetControllingUnknown());
	if (sptrNO1 == 0)
		return false;
	IUnknownPtr ptrPar;
	sptrNO1->GetParent(&ptrPar);
	ICalcObjectPtr sptrCO(ptrPar);
	if (sptrCO == 0)
		return false;
	double dValue;
	if (FAILED(sptrCO->GetValue(KEY_POLARITY,&dValue)))
		return false;
	if (dValue > 0.)
		return false;
	return true;
}

long CChromosome::OnGetBrightProfileSize(int nNum)
{
	return m_LongAxis.m_nSize;
}


typedef color (*GRAYVALFUNC)(IImage3 *pimg, int x, int y);

class _CGrayValue
{
	CImageWrp m_img;
	GRAYVALFUNC m_pGetGrayVal;
	static color _GetGVFromGray(IImage3 *pimg, int x, int y)
	{
		color *prow;
		pimg->GetRow(y,0,&prow);
		return prow[x];
	}
	static color _GetGVFromRGB(IImage3 *pimg, int x, int y)
	{
		color *pred,*pgreen,*pblue;
		pimg->GetRow(y,0,&pred);
		pimg->GetRow(y,1,&pgreen);
		pimg->GetRow(y,2,&pblue);
		return Bright(pblue[x],pgreen[x],pred[x]);
	}
	static color _GetGVFromCGH(IImage3 *pimg, int x, int y)
	{
		color *prow;
		pimg->GetRow(y,g_CGHPane,&prow);
		return prow[x];
	}
	static color _GetGVFromUnknown(IImage3 *, int, int)
	{
		return 0;
	}
public:
	_CGrayValue(CImageWrp &img)
	{
		m_img = img;
		CString s = img.GetColorConvertor();
		if (!s.CompareNoCase(_T("Gray")))
			m_pGetGrayVal = _GetGVFromGray;
		else if (!s.CompareNoCase(_T("RGB")))
			m_pGetGrayVal = _GetGVFromRGB;
		else if (!s.CompareNoCase(_T("CGH")))
			m_pGetGrayVal = _GetGVFromCGH;
		else
			m_pGetGrayVal = _GetGVFromGray;
//			m_pGetGrayVal = _GetGVFromUnknown;
	};
	color GetVal(int x, int y)
	{
		if (x >= 0 && x < m_img.GetWidth() && y >= 0 && y < m_img.GetHeight())
		{
			BYTE *pmask = m_img.GetRowMask(y);
			if (pmask)
				return (*m_pGetGrayVal)(m_img, x, y);
		}
		return 0;
	}
};

void CChromosome::OnGetBrightProfile(int nNum, long *plProfile)
{
	int nSize = OnGetBrightProfileSize(nNum);
	IUnknownPtr punkImg;
	GetImage(&punkImg);
	CImageWrp img(punkImg);
	if (img == 0) // No image in object - erroneous situalion !
	{
		ASSERT(FALSE);
		memset(plProfile, 0, nSize*sizeof(long));
		return;
	}
	_CGrayValue GV(img);
	if (nNum == 0)
	{
		for (int i = 0; i < nSize; i++)
		{
			CPoint pt(m_LongAxis.m_iptAxis[i]);
			plProfile[i] = GV.GetVal(pt.x, pt.y);
		}
	}
	else
	{
		CPoint pt0(m_LongAxis.m_iptAxis[0]);
		CPoint pt1(m_LongAxis.m_iptAxis[1]);
		long lPrev = GV.GetVal(pt0.x, pt0.y);
		plProfile[0] = GV.GetVal(pt1.x, pt1.y)-lPrev;
		for (int i = 1; i < nSize; i++)
		{
			CPoint pt(m_LongAxis.m_iptAxis[i]);
			long lCur = GV.GetVal(pt.x, pt.y);
			plProfile[i] = lCur-lPrev;
			lPrev = lCur;
		}
	}
}



HRESULT CChromosome::XClone::Clone(IUnknown** ppunkCloned)
{
	METHOD_PROLOGUE_EX(CChromosome, Clone)
	{
		if (!ppunkCloned)
			return E_INVALIDARG;
		*ppunkCloned = pThis->Clone();
		return S_OK;
	}
}

HRESULT CChromosome::XChromos::GetAxisSize(int nAxis, long *plSize, BOOL *pbIsManual)
{
	_try_nested(CChromosome, Chromos, GetAxisSize)
	{
		if (pThis->m_sptrChromoConvert != 0 && (nAxis&AT_Original) == 0)
		{
			if (SUCCEEDED(pThis->m_sptrChromoConvert->CCGetAxisSize(&pThis->m_xChromos, nAxis, plSize, pbIsManual)))
				return S_OK;
		}
		CChromoAxis &Axis = pThis->AxisByFlags(nAxis);
		*plSize = Axis.m_nSize;
		if (pbIsManual) *pbIsManual = pThis->IsManualAxis(nAxis);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetAxis(int nAxis, POINT *pAxisPoints)
{
	_try_nested(CChromosome, Chromos, GetAxis)
	{
		if (pThis->m_sptrChromoConvert != 0 && (nAxis&AT_Original) == 0)
		{
			if (SUCCEEDED(pThis->m_sptrChromoConvert->CCGetAxis(&pThis->m_xChromos, nAxis, pAxisPoints)));
				return S_OK;
		}
		CChromoAxis &Axis = pThis->AxisByFlags(nAxis);
		if (nAxis&AT_AxisDouble)
		{
			FPOINT *pAxisPoints1 = (FPOINT *)pAxisPoints;
			if (Axis.m_Type == FloatAxis)
				memcpy(pAxisPoints1, Axis.m_fptAxis, Axis.m_nSize*sizeof(FPOINT));
			else
			{
				for (unsigned i = 0; i < Axis.m_nSize; i++)
					pAxisPoints1[i] = CFPoint(Axis.m_iptAxis[i].x, Axis.m_iptAxis[i].y);
			}
		}
		else
		{
			if (Axis.m_Type == IntegerAxis)
				memcpy(pAxisPoints, Axis.m_iptAxis, Axis.m_nSize*sizeof(POINT));
			else
			{
				for (unsigned i = 0; i < Axis.m_nSize; i++)
					pAxisPoints[i] = CPoint((int)Axis.m_fptAxis[i].x, (int)Axis.m_fptAxis[i].y);
			}
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetAxis(int nAxis, long lSize, POINT* pAxisPoints, BOOL bIsManual)
{
	_try_nested(CChromosome, Chromos, SetAxis)
	{
		CChromoAxis *pAxis = nAxis&AT_ShortAxis?&pThis->m_ShortAxis:&pThis->m_LongAxis;
		if (nAxis&AT_AxisDouble)
			pAxis->Init(FloatAxis, lSize, (FPOINT*)pAxisPoints);
		else
		{
			pAxis->Init(IntegerAxis, lSize, pAxisPoints);
			pAxis->Convert(FloatAxis);
		}
		pThis->IsManualAxis(nAxis) = bIsManual;
		if (nAxis&AT_Rotated)
			pThis->Rot2Obj(*pAxis);
		pAxis->SetOffset(CFPoint(pThis->GetImgOffset()));
		if (pThis->m_sptrChromoConvert != 0)
			pThis->m_sptrChromoConvert->Invalidate();
//			pThis->m_sptrChromoConvert->Recalc(&pThis->m_xChromos);
		pThis->m_DrawAxis.Init(NoData, 0);
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetAxis2(int nFlags /*ChromoAxisType*/, IChromoAxis2 *pAxis, BOOL *pbIsManual)
{
	_try_nested(CChromosome, Chromos, GetAxis2)
	{
		if (pThis->m_sptrChromoConvert != 0 && (nFlags&AT_Original) == 0)
		{
			long lSize;
			if (SUCCEEDED(pThis->m_sptrChromoConvert->CCGetAxisSize(&pThis->m_xChromos, nFlags, &lSize, pbIsManual)))
			{
				pAxis->Init(IntegerAxis, lSize);
				if (SUCCEEDED(pThis->m_sptrChromoConvert->CCGetAxis(&pThis->m_xChromos, nFlags, (POINT*)pAxis->GetData())))
					return S_OK;
			}
		}
		pAxis->Init(&pThis->AxisByFlags(nFlags));
		if (pbIsManual) *pbIsManual = pThis->IsManualAxis(nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetAxis2(int nAxis /*ChromoAxisType*/, IChromoAxis2 *p, BOOL bIsManual)
{
	_try_nested(CChromosome, Chromos, SetAxis2)
	{
		CChromoAxis *pAxis = nAxis&AT_ShortAxis?&pThis->m_ShortAxis:&pThis->m_LongAxis;
		pThis->IsManualAxis(nAxis) = bIsManual;
		pAxis->Init(p);
		Dump_Axis(_T("Rotated axis"), p);
		if (nAxis&AT_Rotated)
		{
			pThis->Rot2Obj(*pAxis);
			Dump_Axis(_T("Not-Rotated axis"), pAxis);
		}
		pThis->m_DrawAxis.Init(NoData, 0);
		if (pThis->m_sptrChromoConvert != 0)
			pThis->m_sptrChromoConvert->Invalidate();
//			pThis->m_sptrChromoConvert->Recalc(&pThis->m_xChromos);
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetCentromereCoord(int nFlags, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual)
{
	_try_nested(CChromosome, Chromos, GetCentromereCoord)
	{
		if (pThis->m_sptrChromoConvert != 0 && (nFlags&AT_Original) == 0)
		{
			if (SUCCEEDED(pThis->m_sptrChromoConvert->CCGetCentromereCoord(&pThis->m_xChromos,
				nFlags, pptCen, pptDraw, pbIsManual)))
				return S_OK;
		}
		if (pThis->m_bCentromere5) pThis->ConvertCentromere();
		if (pThis->m_Cen.ptCen.x < 0)
		{
			CChromoAxis &Axis = pThis->AxisByFlags((nFlags&~AT_ShortAxis)|AT_LongAxis);
			int n = Axis.GetSize();
			CPoint pt = n > 0 ? (CPoint)(CFPoint)Axis.GetFPoint(n/2) : CPoint(0,0);
			*pptCen = pt;
			if (pptDraw)
				pptDraw[0] = pptDraw[1] = pt;
		}
		else
		{
			CENTROMERE Cen;
			if (nFlags&AT_Rotated)
				Cen = pThis->m_KaryoSeq.GetCentromereCoord(&pThis->m_xChromos,NULL);
			else
				Cen = pThis->m_Cen;
			*pptCen = CPoint((int)Cen.ptCen.x, (int)Cen.ptCen.y);;
			if (pptDraw)
			{
				pptDraw[0] = (CPoint)(CFPoint)Cen.aptCenDraw[0];
				pptDraw[1] = (CPoint)(CFPoint)Cen.aptCenDraw[1];
			}
		}
		if (pbIsManual)	*pbIsManual = pThis->m_bCenManual;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetCentromereCoord(int nFlags, POINT ptCen, POINT *ptDraw, BOOL bIsManual)
{
	_try_nested(CChromosome, Chromos, SetCentromereCoord)
	{
		if (nFlags&AT_Rotated)
		{
			pThis->m_Cen.ptCen = (CFPoint)pThis->Rot2Obj(ptCen);
			if (ptDraw)
			{
				pThis->m_Cen.aptCenDraw[0] = (CFPoint)pThis->Rot2Obj(ptDraw[0]);
				pThis->m_Cen.aptCenDraw[1] = (CFPoint)pThis->Rot2Obj(ptDraw[1]);
			}
			else
				pThis->m_Cen.aptCenDraw[0] = pThis->m_Cen.aptCenDraw[1] = INVFPOINT;
		}
		else
		{
			pThis->m_Cen.ptCen = (CFPoint)ptCen;
			if (ptDraw)
			{
				pThis->m_Cen.aptCenDraw[0] = (CFPoint)ptDraw[0];
				pThis->m_Cen.aptCenDraw[1] = (CFPoint)ptDraw[1];
			}
			else
				pThis->m_Cen.aptCenDraw[0] = pThis->m_Cen.aptCenDraw[1] = INVFPOINT;
		}
		pThis->m_Cen.nCentromere = -1;
		pThis->m_Cen.ptOffset = CPoint(0,0);
		pThis->m_bCenManual = bIsManual;
		pThis->ConvertCentromere();
		if (pThis->m_sptrChromoConvert != 0)
			pThis->m_sptrChromoConvert->Invalidate();
//			pThis->m_sptrChromoConvert->Recalc(&pThis->m_xChromos);
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL *pbIsManual)
{
	_try_nested(CChromosome, Chromos, GetCentromereCoord2)
	{
		if (pThis->m_bCentromere5) pThis->ConvertCentromere();
		*pCen = pThis->m_Cen;
		pCen->ptOffset = pThis->GetImgOffset();
		return S_OK;
	}
	_catch_nested;

}

HRESULT CChromosome::XChromos::SetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL bIsManual)
{
	_try_nested(CChromosome, Chromos, SetCentromereCoord2)
	{
		if (pCen->nCentromere <= -1 && CFPoint(pCen->ptCen) == INVFPOINT) return E_INVALIDARG;
		pThis->m_Cen = (CCentromere&)*pCen;
		pThis->m_bCenManual = bIsManual;
		pThis->ConvertCentromere();
		if (pThis->m_sptrChromoConvert != 0)
			pThis->m_sptrChromoConvert->Invalidate();
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;

}


HRESULT CChromosome::XChromos::GetOriginalImage(IUnknown **ppunk)
{
	_try_nested(CChromosome, Chromos, GetOriginalImage)
	{
		return pThis->GetImage(ppunk,TRUE);
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetRotatedImage(int nFlags, IUnknown **ppunk, BOOL *pbOrigImage)
{
	_try_nested(CChromosome, Chromos, GetRotatedImage)
	{
		if (pThis->m_sptrChromoConvert != 0 && (nFlags&AT_Original) == 0)
//		if (pThis->m_sptrChromoConvert != 0)
		{
			HRESULT hr = pThis->m_sptrChromoConvert->CCGetImage(&pThis->m_xChromos, ppunk);
			if (hr == S_OK && *ppunk != 0)
				return S_OK;
		}
		*ppunk = pThis->m_KaryoSeq.GetImage(&pThis->m_xChromos);
		if ((*ppunk)) (*ppunk)->AddRef();
		if (pbOrigImage) *pbOrigImage = FALSE;
		return (*ppunk)?S_OK:E_UNEXPECTED;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetEqualizedImage(int nFlag, IUnknown **ppunk)
{
	_try_nested(CChromosome, Chromos, GetEqualizedImage)
	{
		if (pThis->m_bEqualizeFlag)
			pThis->GetImage(ppunk, FALSE);
		else
			*ppunk = NULL;
		return S_FALSE;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetEqualizedRotatedImage(int nFlags, IUnknown **ppunk, BOOL *pbOrigImage)
{
	return GetRotatedImage(nFlags, ppunk, pbOrigImage);
}

HRESULT CChromosome::XChromos::SetOriginalImage(IUnknown *punk)
{
	_try_nested(CChromosome, Chromos, SetOriginalImage)
	{
		pThis->m_sptrImageOrg = punk;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetRotatedImage(IUnknown *pImg)
{
	_try_nested(CChromosome, Chromos, SetRotatedImage)
	{
		pThis->ResetSequences();
//		pThis->m_sptrImageR = pImg;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetEqualizedImage(IUnknown *pImg)
{
	_try_nested(CChromosome, Chromos, SetEqualizedImage)
	{
		pThis->m_bEqualizeFlag = pImg != 0;
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetEqualizedRotatedImage(IUnknown *pImg)
{
	_try_nested(CChromosome, Chromos, SetEqualizedRotatedImage)
	{
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetRotateParams(ROTATEPARAMS *pRotateParams)
{
	_try_nested(CChromosome, Chromos, GetRotateParams)
	{
		if (pThis->m_bRotateVer4) pThis->ConvertRotate();
		*pRotateParams = pThis->m_Rotate;
		return S_OK;
	}
	_catch_nested;
}

void CChromosome::CalcOffsets()
{
	CChromoSettings Settings;
	IUnknownPtr punkImg;
	if (m_Rotate.ptOffs.x <= -10000)
	{
		if (GetImage(&punkImg) == S_OK)
		{
			CImageWrp ImgWrp(punkImg);
			m_Rotate.ptOffs = CPoint(0,0);
		}
	}
	if (m_Rotate.ptCenter.x <= -10000)
	{
		if (GetImage(&punkImg) == S_OK)
		{
			CImageWrp ImgWrp(punkImg);
			int nWidth = ImgWrp.GetWidth();
			int nHeight = ImgWrp.GetHeight();
			m_Rotate.ptCenter = CPoint(nWidth/2,nHeight/2);
		}
	}
	if (m_Rotate.ptOffsR.x <= -10000)
	{
		if (GetImage(&punkImg) == S_OK)
		{
			sptrINamedDataObject2 sptrNO1(GetControllingUnknown());
			IUnknownPtr ptrPar;
			sptrNO1->GetParent(&ptrPar);
			IMeasureObjectPtr sptrM(ptrPar);
			CChromParam Param;
			Param.Init(sptrM, &m_xChromos, punkImg);
			CImageWrp ImgWrp1;
			CIntervalImage *pIntvl1 = NULL;
			CPoint ptCenter;
			rottrans(Param.pImgWrp,Param.pIntvl,m_Rotate.dAngle,ImgWrp1,pIntvl1,&ptCenter);
			delete pIntvl1;
			CPoint ptImgOffs = GetImgOffset();
			CPoint ptImgOffsR = ImgWrp1.GetOffset();
			m_Rotate.ptOffsR = CPoint(ptImgOffsR.x-ptImgOffs.x,ptImgOffsR.y-ptImgOffs.y);
		}
	}
}


HRESULT CChromosome::XChromos::SetRotateParams(ROTATEPARAMS *pRotateParams)
{
	_try_nested(CChromosome, Chromos, SetRotateParams)
	{
		pThis->m_Rotate = *pRotateParams;
		pThis->CalcOffsets();
		if (pThis->m_sptrChromoConvert != 0)
			pThis->m_sptrChromoConvert->Invalidate();
//			pThis->m_sptrChromoConvert->Recalc(&pThis->m_xChromos);
		pThis->ResetSequences();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetOrigin(POINT *pptOrg)
{
	_try_nested(CChromosome, Chromos, GetOrigin)
	{
		*pptOrg = pThis->m_ptOrig;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetOrigin(POINT ptOrg)
{
	_try_nested(CChromosome, Chromos, SetOrigin)
	{
		pThis->m_ptOrig = ptOrg;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetOffset(POINT *pptOffs)
{
	_try_nested(CChromosome, Chromos, GetOffset)
	{
		*pptOffs = pThis->m_ptOffs;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetOffset(POINT ptOffs)
{
	_try_nested(CChromosome, Chromos, SetOffset)
	{
		pThis->m_ptOffs = ptOffs;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetBrightProfileSize(int iNum, long *plSize)
{
	_try_nested(CChromosome, Chromos, GetBrightProfileSize)
	{
		*plSize = pThis->OnGetBrightProfileSize(iNum);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetBrightProfile(int iNum, long *plBright)
{
	_try_nested(CChromosome, Chromos, GetBrightProfile)
	{
		pThis->OnGetBrightProfile(iNum,plBright);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetColor(COLORREF color, int iNum)
{
	_try_nested(CChromosome, Chromos, SetColor)
	{
		if (iNum == 0) pThis->m_clrAxis = color;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetManualClass(int *pnClass)
{
	_try_nested(CChromosome, Chromos, GetManualClass)
	{
		*pnClass = pThis->m_nManualClass;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetManualClass(int nClass)
{
	_try_nested(CChromosome, Chromos, SetManualClass)
	{
		pThis->m_nManualClass = nClass;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetChromoConvert(IChromoConvert **ppChromoConvert)
{
	_try_nested(CChromosome, Chromos, GetChromoConvert)
	{
		if (pThis->m_sptrChromoConvert == 0)
			*ppChromoConvert = 0;
		else
		{
			*ppChromoConvert = pThis->m_sptrChromoConvert;
			(*ppChromoConvert)->AddRef();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::SetChromoConvert(IChromoConvert *pChromoConvert)
{
	_try_nested(CChromosome, Chromos, SetChromoConvert)
	{
		pThis->m_sptrChromoConvert = pChromoConvert;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::IsMirrorV(BOOL *pbMirror)
{
	_try_nested(CChromosome, Chromos, IsMirrorV)
	{
		*pbMirror = pThis->IsMirrorV();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CChromosome::XChromos::GetKaryoSequence(IChromoConvert2 **ppSequence)
{
	_try_nested(CChromosome, Chromos, GetKaryoSequence)
	{
		*ppSequence = (IChromoConvert2 *)&pThis->m_KaryoSeq;
		if (*ppSequence) (*ppSequence)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

void CChromosome::ResetSequences()
{
	m_KaryoSeq.Recalc(NULL);
	if (m_sptrChromoConvert != 0)
		m_sptrChromoConvert->Invalidate();
}



