#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "DocView5.h"
#include "misc_utils.h"
#include "Stringer.h"
#include "array.h"
#include "Object5.h"
#include "measure5.h"
#include "misc_calibr.h"
#include "misc_new.h"
#include "params.h"
#include "MetallParams.h"
#include "class_utils.h"
#include "fpoint.h"
#include <stdio.h>
#include <alloc.h>
#include <math.h>


const char *g_pszDebugName = "c:\\stringers.log";

static int g_nDebug = -1;

static void dprintf(char * szFormat, ...)
{
	bool bFirstly = false;
	if (g_nDebug == -1)
	{
		g_nDebug = GetValueInt(GetAppUnknown(), "\\Stringers", "Debug", 0);
		bFirstly = true;
	}
	if (g_nDebug == 0)
		return;
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    vsprintf(buf, szFormat, va);
    va_end(va);
	HFILE file;
	OFSTRUCT of;
	if (bFirstly || OpenFile(g_pszDebugName,&of,OF_EXIST)==HFILE_ERROR)
		file = OpenFile(g_pszDebugName,&of,OF_CREATE|OF_WRITE);
	else
		file = OpenFile(g_pszDebugName,&of,OF_WRITE);
	_llseek(file,0,FILE_END);
	_lwrite(file,buf,lstrlen(buf));
	_lclose(file); 
	OutputDebugString(buf);
}


///////////////////////////////////////////////////////////////////////////
// class CStringerAxis
///////////////////////////////////////////////////////////////////////////


CStringerAxis::CStringerAxis()
{
	punkParent = NULL;
	m_dwFlags = nofHasData|nofStoreByParent;
}

CStringerAxis::~CStringerAxis()
{
}

IUnknown *CStringerAxis::DoGetInterface(const IID &iid)
{
	if (iid == IID_IStringerAxis)
		return (IStringerAxis *)this;
	else if (iid == IID_IDrawObject || iid == IID_IDrawObject2)
		return (IDrawObject2 *)this;
	else
		return CObjectBase::DoGetInterface(iid);
}

GuidKey CStringerAxis::GetInternalClassID()
{
	return clsidStringerAxis;
}

HRESULT CStringerAxis::Load( IStream *pStream, SerializeParams *pparams )
{
	DWORD dwRead;
	BYTE byVer;
	pStream->Read(&byVer, sizeof(byVer), &dwRead);
	pStream->Read(&m_pt1, sizeof(m_pt1), &dwRead);
	pStream->Read(&m_pt2, sizeof(m_pt2), &dwRead);
	pStream->Read(&m_dLength, sizeof(m_dLength), &dwRead);
	pStream->Read(&m_dWidth, sizeof(m_dWidth), &dwRead);
	pStream->Read(&m_dSumLenght, sizeof(m_dSumLenght), &dwRead);
	return S_OK;
}

HRESULT CStringerAxis::Store( IStream *pStream, SerializeParams *pparams )
{
	DWORD dwWritten;
	BYTE byVer = 0;
	pStream->Write(&byVer, sizeof(byVer), &dwWritten);
	pStream->Write(&m_pt1, sizeof(m_pt1), &dwWritten);
	pStream->Write(&m_pt2, sizeof(m_pt2), &dwWritten);
	pStream->Write(&m_dLength, sizeof(m_dLength), &dwWritten);
	pStream->Write(&m_dWidth, sizeof(m_dWidth), &dwWritten);
	pStream->Write(&m_dSumLenght, sizeof(m_dSumLenght), &dwWritten);
	return S_OK;
}

HRESULT CStringerAxis::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = FALSE;
	return S_OK;
}

HRESULT CStringerAxis::GetParent( IUnknown **ppunkParent )
{
	*ppunkParent = punkParent;
	return S_OK;
}

HRESULT CStringerAxis::SetParent(IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/)
{
	if (this->punkParent)
	{
		INamedDataObject2Ptr sptrNDOParent(this->punkParent);
		sptrNDOParent->RemoveChild(Unknown());
	}
	this->punkParent = punkParent;
	if (this->punkParent)
	{
		INamedDataObject2Ptr sptrNDOParent(this->punkParent);
		sptrNDOParent->AddChild(Unknown());
	}
	return S_OK;
}

void CStringerAxis::SetCoords(POINT pt1, POINT pt2)
{
	m_pt1 = pt1;
	m_pt2 = pt2;
}

BOOL CStringerAxis::GetCoords(POINT *ppt1, POINT *ppt2)
{
	*ppt1 = m_pt1;
	*ppt2 = m_pt2;
	return TRUE;
}

void CStringerAxis::SetParameter(long lParam, double dValue)
{
	if (lParam == KEY_PARAM_STRINGER_LENGHT)
		m_dLength = dValue;
	else if (lParam == KEY_PARAM_STRINGER_SUM_LENGHT)
		m_dSumLenght = dValue;
	else if (lParam == KEY_PARAM_STRINGER_WIDTH)
		m_dWidth = dValue;
	else if (lParam == KEY_PARAM_STRINGER_WIDTH_ASTM45)
		m_dWidthAstm45 = dValue;
}

void CStringerAxis::GetParameter(long lParam, double *pdValue)
{
	if (lParam == KEY_PARAM_STRINGER_LENGHT)
		*pdValue = m_dLength;
	else if (lParam == KEY_PARAM_STRINGER_SUM_LENGHT)
		*pdValue = m_dSumLenght;
	else if (lParam == KEY_PARAM_STRINGER_WIDTH)
		*pdValue = m_dWidth;
	else if (lParam == KEY_PARAM_STRINGER_WIDTH_ASTM45)
		*pdValue = m_dWidthAstm45;
}

HRESULT CStringerAxis::Paint(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache)
{
	if (m_pt1.x == m_pt2.x && m_pt1.y == m_pt2.y) return S_OK;
	ICalcObjectPtr sptrCO(punkParent);
	COLORREF clr = get_class_color(get_object_class(sptrCO));
	double	fZoom;
	_point	ptScroll;
	IScrollZoomSitePtr psite(punkTarget);
	psite->GetZoom(&fZoom);
	psite->GetScrollPos(&ptScroll);
	_pen pen(::CreatePen(PS_SOLID, 0, clr));
	HPEN pOld = (HPEN)::SelectObject(hDC, pen.handle);
	_point pt1((int)(fZoom*(m_pt1.x+0.5)-ptScroll.x),(int)(fZoom*(m_pt1.y+0.5)-ptScroll.y));
	_point pt2((int)(fZoom*(m_pt2.x+0.5)-ptScroll.x),(int)(fZoom*(m_pt2.y+0.5)-ptScroll.y));
	::MoveToEx(hDC, pt1.x, pt1.y, NULL);
	::LineTo(hDC, pt2.x, pt2.y);
	::SelectObject(hDC, pOld);
	return S_OK;
}

HRESULT CStringerAxis::GetRect( RECT *prect )
{
	if( prect )ZeroMemory( prect, sizeof( RECT ) );
	return E_NOTIMPL;
}

HRESULT CStringerAxis::SetTargetWindow( IUnknown *punkTarget )
{
	return E_NOTIMPL;
}

HRESULT CStringerAxis::PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions)
{
	return Paint(hDC, rectDraw, punkTarget, pbiCache, pdibCache);
}

///////////////////////////////////////////////////////////////////////////
// Search of the stringers: old mechanism (GOST and ASTM 1122).
///////////////////////////////////////////////////////////////////////////

class CPhasesCount
{
	int Data[256];
public:
	CPhasesCount()
	{
		memset(Data, 0, 256*sizeof(int));
	}
	CPhasesCount(IImage4 *pimg)
	{
		memset(Data, 0, 256*sizeof(int));
		InitByImage(pimg);
	}
	int &operator[](int n) {return Data[n];}
	operator int* (){return Data;};
	void Row(byte *pRowMask, int cx)
	{
		for (int x = 0; x < cx; x++)
			Data[pRowMask[x]]++;
	}
	void InitByImage(IImage4 *pimg)
	{
		int cxObj,cyObj;
		pimg->GetSize(&cxObj,&cyObj);
		for (int y = 0; y < cyObj; y++)
		{
			byte *pRowMask;
			pimg->GetRowMask(y, &pRowMask);
			Row(pRowMask, cxObj);
		}
	}
	double Sulfide()
	{ return Data[128]+Data[129]?double(Data[129])/double(Data[128]+Data[129]):0.;}
};

static int nMaxDistX = 0;
static int nMaxDistY = 0;
static double dDeltaAngle = PI/180.;
static double dAngleScope = PI/9.;
static double dCostCoef = 0.01;
static double dRatioThresh = 2.;
static double dAngleThreshold = PI/4.;
static double dThickThreshB = 0., dOvrThreshB = 0., dThickThreshC = 0., dOvrThreshC = 0.;
static BOOL bASTM;
static bool IsElongated(double dAspectRatio) {return bASTM?dAspectRatio>=5.:dAspectRatio>=2;}
static int nIgnoreSmall = 1;
static int nMinObjectSize;

static double dCalibr;
static double McmToPoints(double dMeterPerPixel, double Mcms)
{
	return (Mcms/(dMeterPerPixel*1000000.));
}

static double PointsToMcm(double dMeterPerPixel, double dPoints)
{
	return dPoints*(dMeterPerPixel*1000000.);
}



struct CObjInfo
{
	IUnknown *m_punkObj;
	IImage4 *m_pimg;
	_rect m_rectOrig;
	_rect m_rectRotated; // rotated to m_dAngle
	FRECT m_rectRotOrient; // rotated to m_dParticleOrientation
	double m_dArea;
	int m_nStringer;
	double m_dAngle; // Stringer angle (not particle's !!!)
	double m_dCoef;
	int m_nScanLine;
	int m_x1,m_x2;
	int m_nStringerType;  // 1 - if long particle (b/a > 5) >= 50% by area, 0 - in other case
	double m_dPartLength; // Particle length for the stringer (excluding thin tips)
	double m_dPartWidth;  // Particle width for the stringer (excluding thin tips)
	double m_dPartArea;   // Particla area excluding thin tips
	double m_dAspectRatio; // Particle aspect ratio (using local angle)
	double m_dParticleOrientation; // Particle angle
	bool   m_bSulfideParticle; // true if sulfide part > 50%
	bool m_bDisabled; // Perpendicular particles
	double m_dPhase128,m_dPhase129,m_dPhase130; // three phases
	double m_dRoughness; // parameter 150026
	double m_dXCoord,m_dYCoord;
	CObjInfo()
	{
		m_punkObj = NULL;
		m_pimg = NULL;
		m_dArea = 0;
		m_nStringer = -1;
		m_dCoef = 0.;
		m_dPartLength = m_dPartWidth = 0.;
		m_nStringerType = -1;
		m_bSulfideParticle = false;
		m_bDisabled = false;
		m_dPhase128 = m_dPhase129 = m_dPhase130 = 0.;
		m_dXCoord = m_dYCoord = 0;
	}
};

typedef _CArray<CObjInfo,50,1> _CObjArray;

POINT rotatepoint(POINT ptOrg, POINT pt, double angle)
{
	_point pt1(pt.x-ptOrg.x,pt.y-ptOrg.y);
	double x = pt1.x*cos(angle)-pt1.y*sin(angle);
	double y = pt1.x*sin(angle)+pt1.y*cos(angle);
	_point ptRet(int(x)+ptOrg.x,int(y)+ptOrg.y);
	return ptRet;
}

FPOINT rotatepoint(FPOINT ptOrg, FPOINT pt, double angle)
{
	FPOINT pt1(pt.x-ptOrg.x,pt.y-ptOrg.y);
	double x = pt1.x*cos(angle)-pt1.y*sin(angle);
	double y = pt1.x*sin(angle)+pt1.y*cos(angle);
	FPOINT ptRet(x+ptOrg.x,y+ptOrg.y);
	return ptRet;
}

FRECT CalcRotatedRect(CObjInfo &ObjCur, double dAngle, FPOINT ptCenter)
{
	bool bFirstPoint = true;
	FRECT rcRotated;
	int nContours;
	ObjCur.m_pimg->GetContoursCount(&nContours);
	for (int iContour = 0; iContour < nContours; iContour++)
	{
		Contour *pCont;
		ObjCur.m_pimg->GetContour(iContour, &pCont);
		if (pCont->lFlags & cfExternal)
		{
			for (int iPoint = 0; iPoint < pCont->nContourSize; iPoint++)
			{
				ContourPoint point = pCont->ppoints[iPoint];
				FPOINT ptRot = rotatepoint(ptCenter, FPOINT(point.x, point.y), dAngle);
				if (bFirstPoint)
				{
					rcRotated.left = ptRot.x;
					rcRotated.right = ptRot.x;
					rcRotated.top = ptRot.y;
					rcRotated.bottom = ptRot.y;
					bFirstPoint = false;
				}
				else
				{
					rcRotated.left = min(rcRotated.left,ptRot.x);
					rcRotated.right = max(rcRotated.right,ptRot.x+1);
					rcRotated.top = min(rcRotated.top,ptRot.y);
					rcRotated.bottom = max(rcRotated.bottom,ptRot.y+1);
				}
			}
		}
	}
	if (bFirstPoint)
	{
		FPOINT pt1 = rotatepoint(ptCenter, FPOINT(ObjCur.m_rectOrig.left, ObjCur.m_rectOrig.top), dAngle);
		FPOINT pt2 = rotatepoint(ptCenter, FPOINT(ObjCur.m_rectOrig.left, ObjCur.m_rectOrig.bottom), dAngle);
		FPOINT pt3 = rotatepoint(ptCenter, FPOINT(ObjCur.m_rectOrig.right, ObjCur.m_rectOrig.top), dAngle);
		FPOINT pt4 = rotatepoint(ptCenter, FPOINT(ObjCur.m_rectOrig.right, ObjCur.m_rectOrig.bottom), dAngle);
		rcRotated.left = min(min(pt1.x,pt2.x),min(pt3.x,pt4.x));
		rcRotated.right = max(max(pt1.x,pt2.x),max(pt3.x,pt4.x));
		rcRotated.top = min(min(pt1.y,pt2.y),min(pt3.y,pt4.y));
		rcRotated.bottom = max(max(pt1.y,pt2.y),max(pt3.y,pt4.y));
	}
	return rcRotated;
}

struct _FINTERVAL
{
	double t,b;
	bool init;
};

static bool _Lesse(int x, int x2, int dx)
{
	return dx>0?x<=x2:x>=x2;
}

void CalcRealWidthAndHeight(CObjInfo &ObjCur, FPOINT ptCenter, double dMinWidth,
	double &dRealLength, double &dRealAvrWidth, double &dArea)
{
	double dAngle = ObjCur.m_dParticleOrientation;
	FRECT rc = CalcRotatedRect(ObjCur, dAngle, ptCenter);
	int l = (int)(ceil(rc.left));
	int r = (int)(floor(rc.right));
	_CArray<_FINTERVAL,10,10> arr;
	arr.SetSize(r-l+1);
	int nContours;
	ObjCur.m_pimg->GetContoursCount(&nContours);
	for (int iContour = 0; iContour < nContours; iContour++)
	{
		Contour *pCont;
		ObjCur.m_pimg->GetContour(iContour, &pCont);
		if (pCont->lFlags & cfExternal)
		{
			ContourPoint point = pCont->ppoints[pCont->nContourSize-1];
			FPOINT ptRotPrev = rotatepoint(ptCenter, FPOINT(point.x, point.y), dAngle);
			for (int iPoint = 0; iPoint < pCont->nContourSize; iPoint++)
			{
				point = pCont->ppoints[iPoint];
				FPOINT ptRot = rotatepoint(ptCenter, FPOINT(point.x, point.y), dAngle);
				int x1,x2,dx;
				if (ptRot.x >= ptRotPrev.x)
				{
					x1 = (int)ceil(ptRotPrev.x);
					x2 = (int)floor(ptRot.x);
					dx = 1;
				}
				else
				{
					x1 = (int)floor(ptRotPrev.x);
					x2 = (int)ceil(ptRot.x);
					dx = -1;
				}
				if (ptRot.x == ptRotPrev.x)
				{
					if (x1 == x2)
					{
						int xx = x1-l;
						if (xx >= 0 && xx < arr.GetSize())
						{
							if (arr.GetPtrAt(xx).init)
							{
								if (ptRotPrev.y < arr.GetPtrAt(xx).t) arr.GetPtrAt(xx).t = ptRotPrev.y;
								if (ptRotPrev.y > arr.GetPtrAt(xx).b) arr.GetPtrAt(xx).b = ptRotPrev.y;
							}
							else
							{
								arr.GetPtrAt(xx).t = ptRotPrev.y;
								arr.GetPtrAt(xx).b = ptRotPrev.y;
							}
						}
					}
				}
				else
				{
					for (int x = x1; _Lesse(x,x2,dx); x+= dx)
					{
						if (ptRot.x==ptRotPrev.x)
							ptRot.x=ptRotPrev.x;
						double y = ptRotPrev.y+(ptRot.y-ptRotPrev.y)*(double(x)-ptRotPrev.x)/
							(ptRot.x-ptRotPrev.x);
						int xx = x-l;
						if (xx < 0 || xx >= arr.GetSize()) continue;
						if (arr.GetPtrAt(xx).init)
						{
							if (y < arr.GetPtrAt(xx).t) arr.GetPtrAt(xx).t = y;
							if (y > arr.GetPtrAt(xx).b) arr.GetPtrAt(xx).b = y;
						}
						else
						{
							arr.GetPtrAt(xx).t = arr.GetPtrAt(xx).b = y;
							arr.GetPtrAt(xx).init = true;
						}
					}
				}
				ptRotPrev = ptRot;
			}
		}
	}
	double dPrev = arr.GetPtrAt(0).init?arr.GetPtrAt(0).b-arr.GetPtrAt(0).t:0;
	bool bInterval = dPrev>=dMinWidth;
	double dLen = bInterval?dPrev==0.?double(l-rc.left):(l-rc.left)*(dPrev-dMinWidth)/dPrev:0.;
	double dRealArea = bInterval?dLen*(dPrev+dMinWidth)/2.:0.;
	for (int i = 1; i < arr.GetSize(); i++)
	{
		double d = arr.GetPtrAt(i).init?arr.GetPtrAt(i).b-arr.GetPtrAt(i).t:0.;
		if (d >= dMinWidth)
		{
			if (!bInterval)
			{
				double dd = (d-dMinWidth)/(d-dPrev);
				dLen += dd;
				dRealArea += dd*(d+dMinWidth)/2.;
			}
			else
			{
				dLen += 1.;
				dRealArea += (d+dPrev)/2.;
			}
			bInterval = true;
		}
		else
		{
			if (bInterval)
			{
				double dd = (dPrev-dMinWidth)/(dPrev-d);
				dLen += dd;
				dRealArea += dd*(d+dMinWidth)/2.;
			}
			bInterval = false;
		}
		dPrev = d;
	}
	if (bInterval)
	{
		double dd = dPrev==0.?double(rc.right-r):(rc.right-r)*(dPrev-dMinWidth)/dPrev;
		dLen += dd;
		dRealArea += dd*(dPrev+dMinWidth)/2.;
	}
	dRealLength = dLen;
	dRealAvrWidth = dLen>0?dRealArea/dLen:0.;
	dArea = dRealArea;
}

void CalcMaxAspectRatio(CObjInfo &ObjCur, double dDeltaAngle, FPOINT ptCenter)
{
	ObjCur.m_dAspectRatio = 0.;
	ObjCur.m_dPartLength = 0.;
	ObjCur.m_dPartWidth = 0.;
	ObjCur.m_dParticleOrientation = 0.;
#if 1
	ICalcObjectPtr sptrCO(ObjCur.m_punkObj);
	if (sptrCO->GetValue(KEY_ANGLE, &ObjCur.m_dParticleOrientation) != S_OK ||
		sptrCO->GetValue(KEY_NOUN, &ObjCur.m_dAspectRatio) != S_OK ||
		sptrCO->GetValue(KEY_WIDTH, &ObjCur.m_dPartWidth) != S_OK ||
		sptrCO->GetValue(KEY_LENGTH, &ObjCur.m_dPartLength) != S_OK)
	{
		ObjCur.m_bDisabled = true;
		return;
	}
	if (ObjCur.m_dParticleOrientation > PI/2.) ObjCur.m_dParticleOrientation -= PI;
	ObjCur.m_dPartWidth /= dCalibr;
	ObjCur.m_dPartLength /= dCalibr;

#else
	for (double dAngle = 0.; dAngle <= PI/2.; dAngle += dDeltaAngle)
	{
		FRECT rc = CalcRotatedRect(ObjCur, dAngle, ptCenter);
		double dAspectRatio1 = (rc.right-rc.left)/(rc.bottom-rc.top);
		if (dAspectRatio1 > ObjCur.m_dAspectRatio)
		{
			ObjCur.m_dAspectRatio = dAspectRatio1;
			ObjCur.m_dPartLength = (rc.right-rc.left);
			ObjCur.m_dPartWidth = (rc.bottom-rc.top);
			ObjCur.m_dParticleOrientation = dAngle;
		}
		double dAspectRatio2 = (rc.bottom-rc.top)/(rc.right-rc.left);
		if (dAspectRatio2 > ObjCur.m_dAspectRatio)
		{
			ObjCur.m_dAspectRatio = dAspectRatio2;
			ObjCur.m_dPartLength = (rc.bottom-rc.top);
			ObjCur.m_dPartWidth = (rc.right-rc.left);
			ObjCur.m_dParticleOrientation = dAngle-PI/2.;
		}
	}
#endif
	if (ObjCur.m_dPartLength < nMinObjectSize && nIgnoreSmall > 0 ||
		ObjCur.m_dAspectRatio > dRatioThresh && (ObjCur.m_dParticleOrientation > dAngleThreshold ||
		ObjCur.m_dParticleOrientation < -dAngleThreshold))
		ObjCur.m_bDisabled = true;
}

void CalcMaxAspectRatio(_CObjArray &arrObjs, double dDeltaAngle, POINT ptCen)
{
	FPOINT ptCenter(ptCen);
	for (int j = 0; j < arrObjs.GetSize(); j++)
	{
		CObjInfo &ObjCur = arrObjs.GetPtrAt(j);
		CalcMaxAspectRatio(ObjCur, dDeltaAngle, ptCenter);
	}
}

void MakeObjectsArray(_CObjArray &arrObjs, INamedDataObject2 *pObjList, int nObjects)
{
	long lPos;
	pObjList->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr punkObj;
		pObjList->GetNextChild(&lPos, &punkObj);
		CObjInfo ObjInfo;
		IRectObjectPtr sptrRectObj(punkObj);
		sptrRectObj->GetRect(&ObjInfo.m_rectOrig);
		ObjInfo.m_punkObj = punkObj;
		ICalcObjectPtr sptrCO(punkObj);
		ObjInfo.m_dArea = 0.;
		sptrCO->GetValue(KEY_AREA, &ObjInfo.m_dArea);
		IMeasureObjectPtr sptrMObj(punkObj);
		IUnknownPtr punkImage;
		sptrMObj->GetImage(&punkImage);
		IImage4Ptr sptrImg(punkImage);
		if (sptrImg != 0)
		{
			ObjInfo.m_pimg = sptrImg;
			CPhasesCount PhasesCount(ObjInfo.m_pimg);
			ObjInfo.m_bSulfideParticle = PhasesCount.Sulfide()>=0.5;
			arrObjs.Add(ObjInfo);
		}
	}
}

void RotateObjects(_CObjArray &arrObjs, double dAngle, POINT ptCenter)
{
	for (int j = 0; j < arrObjs.GetSize(); j++)
	{
		CObjInfo &ObjCur = arrObjs.GetPtrAt(j);
		bool bFirstPoint = true;
		int nContours;
		ObjCur.m_pimg->GetContoursCount(&nContours);
		for (int iContour = 0; iContour < nContours; iContour++)
		{
			Contour *pCont;
			ObjCur.m_pimg->GetContour(iContour, &pCont);
			if (pCont->lFlags & cfExternal)
			{
				for (int iPoint = 0; iPoint < pCont->nContourSize; iPoint++)
				{
					ContourPoint point = pCont->ppoints[iPoint];
					POINT ptRot = rotatepoint(ptCenter, _point(point.x, point.y), dAngle);
					if (bFirstPoint)
					{
						ObjCur.m_rectRotated.left = ptRot.x;
						ObjCur.m_rectRotated.right = ptRot.x;
						ObjCur.m_rectRotated.top = ptRot.y;
						ObjCur.m_rectRotated.bottom = ptRot.y;
						bFirstPoint = false;
					}
					else
					{
						ObjCur.m_rectRotated.left = min(ObjCur.m_rectRotated.left,ptRot.x);
						ObjCur.m_rectRotated.right = max(ObjCur.m_rectRotated.right,ptRot.x+1);
						ObjCur.m_rectRotated.top = min(ObjCur.m_rectRotated.top,ptRot.y);
						ObjCur.m_rectRotated.bottom = max(ObjCur.m_rectRotated.bottom,ptRot.y+1);
					}
				}
			}
		}
		if (bFirstPoint)
		{
			POINT pt1 = rotatepoint(ptCenter, _point(ObjCur.m_rectOrig.left, ObjCur.m_rectOrig.top), dAngle);
			POINT pt2 = rotatepoint(ptCenter, _point(ObjCur.m_rectOrig.left, ObjCur.m_rectOrig.bottom), dAngle);
			POINT pt3 = rotatepoint(ptCenter, _point(ObjCur.m_rectOrig.right, ObjCur.m_rectOrig.top), dAngle);
			POINT pt4 = rotatepoint(ptCenter, _point(ObjCur.m_rectOrig.right, ObjCur.m_rectOrig.bottom), dAngle);
			ObjCur.m_rectRotated.left = min(min(pt1.x,pt2.x),min(pt3.x,pt4.x));
			ObjCur.m_rectRotated.right = max(max(pt1.x,pt2.x),max(pt3.x,pt4.x));
			ObjCur.m_rectRotated.top = min(min(pt1.y,pt2.y),min(pt3.y,pt4.y));
			ObjCur.m_rectRotated.bottom = max(max(pt1.y,pt2.y),max(pt3.y,pt4.y));
		}
	}
}


int __cdecl _intsort(const void *p1, const void *p2)
{
	return *(int *)p1-*(int*)p2;
}

POINT FindCenterPoint(_CObjArray &arrObjs)
{
	int nSize = arrObjs.GetSize();
	if (nSize < 1) return _point(0,0);
	CObjInfo &ObjInfo = arrObjs.GetPtrAt(0);
	_rect rc(ObjInfo.m_rectOrig);
	for (int i = 0; i < nSize; i++)
	{
		CObjInfo &ObjInfo1 = arrObjs.GetPtrAt(0);
		if (ObjInfo1.m_rectOrig.left < rc.left) rc.left = ObjInfo1.m_rectOrig.left;
		if (ObjInfo1.m_rectOrig.right > rc.right) rc.right = ObjInfo1.m_rectOrig.right;
		if (ObjInfo1.m_rectOrig.top < rc.top) rc.top = ObjInfo1.m_rectOrig.top;
		if (ObjInfo1.m_rectOrig.bottom > rc.bottom) rc.bottom = ObjInfo1.m_rectOrig.bottom;
	}
	return _point((rc.left+rc.right)/2, (rc.top+rc.bottom)/2);
}

RECT FindObjectsRect(_CObjArray &arrObjs)
{
	int nSize = arrObjs.GetSize();
	if (nSize < 1) return _rect(0,0,0,0);
	CObjInfo &ObjInfo = arrObjs.GetPtrAt(0);
	_rect rc(ObjInfo.m_rectOrig);
	for (int i = 0; i < nSize; i++)
	{
		CObjInfo &ObjInfo1 = arrObjs.GetPtrAt(i);
		if (ObjInfo1.m_rectOrig.left < rc.left) rc.left = ObjInfo1.m_rectOrig.left;
		if (ObjInfo1.m_rectOrig.right > rc.right) rc.right = ObjInfo1.m_rectOrig.right;
		if (ObjInfo1.m_rectOrig.top < rc.top) rc.top = ObjInfo1.m_rectOrig.top;
		if (ObjInfo1.m_rectOrig.bottom > rc.bottom) rc.bottom = ObjInfo1.m_rectOrig.bottom;
	}
	return rc;
}

double CalcCoef(double dAngle, double dLen, double dArea)
{
	return (dAngleScope-dCostCoef*fabs(dAngle))*(dLen+sqrt(dArea));
}

void FindScanLines(_CObjArray &arrObjects, _CArray<int,1000,100> &ScanLines, SIZE sz)
{
	_CArray<int,1000,100> Borders;
	int nObjects = arrObjects.GetSize();
	for (int i = 0; i < nObjects; i++)
	{
		CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
		Borders.Add(ObjInfo.m_rectRotated.top-nMaxDistY);
		Borders.Add(ObjInfo.m_rectRotated.bottom+nMaxDistY);
	}
	qsort(Borders.GetData(), Borders.GetSize(), sizeof(int), _intsort);
	_CArray<int,1000,100> RealBorders;
	int prev = -1;
	int nBorders = Borders.GetSize();
	for (i = 0; i < nBorders; i++)
	{
		int n = Borders.GetAt(i);
		if (n != prev)
			RealBorders.Add(n);
		prev = n;
	}
	int nRealBorders = RealBorders.GetSize();
	for (int i = 0; i < nRealBorders-1; i++)
	{
		int n1 = RealBorders.GetAt(i);
		int n2 = RealBorders.GetAt(i+1);
		int n3 = (n1+n2)/2;
		ScanLines.Add(n3);
	}
}

void FindObjectsForScanLine(int yScanLine, _CArray<int,1000,100> &arrSLObjectsNums, _CObjArray &arrObjects,
	SIZE sz)
{
	int nObjects = arrObjects.GetSize();
	for (int i = 0; i < nObjects; i++)
	{
		CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
		if (yScanLine >= ObjInfo.m_rectRotated.top-nMaxDistY && yScanLine <= ObjInfo.m_rectRotated.bottom+nMaxDistY)
		{
//			if (double(ObjInfo.m_rectRotated.height())/double(ObjInfo.m_rectRotated.width()) < dRatioThresh)
			if (!ObjInfo.m_bDisabled)
				arrSLObjectsNums.Add(i);
		}
	}
}

int CenterScanLine(_CArray<int,100,100> &arrObjectsInStringer, _CObjArray &arrObjects)
{
	int iObject = arrObjectsInStringer.GetAt(0);
	CObjInfo &ObjInfo = arrObjects.GetPtrAt(iObject);
//	_rect rc = ObjInfo.m_rectRotated;
	int nMinBottom = ObjInfo.m_rectRotated.bottom;
	int nMaxTop = ObjInfo.m_rectRotated.top;
	int nObjects = arrObjectsInStringer.GetSize();
	for (int i = 1; i < nObjects; i++)
	{
		iObject = arrObjectsInStringer.GetAt(i);
		CObjInfo &ObjInfo1 = arrObjects.GetPtrAt(iObject);
		if (ObjInfo1.m_rectRotated.top > nMaxTop) nMaxTop = ObjInfo1.m_rectRotated.top;
		if (ObjInfo1.m_rectRotated.bottom < nMinBottom) nMinBottom = ObjInfo1.m_rectRotated.bottom;
//		if (ObjInfo1.m_rectRotated.top < rc.top) rc.top = ObjInfo1.m_rectRotated.top;
//		if (ObjInfo1.m_rectRotated.bottom > rc.bottom) rc.bottom = ObjInfo1.m_rectRotated.bottom;
	}
//	return (rc.top+rc.bottom)/2;
	return (nMinBottom+nMaxTop)/2;
}

struct _CBorder
{
	bool bStart;
	int nObject;
	int x;
	_CBorder(bool bStart, int nObject, int x)
	{
		this->bStart = bStart;
		this->nObject = nObject;
		this->x = x;
	}
};

int __cdecl _bordersort(const void *p1, const void *p2)
{
	return ((_CBorder*)p1)->x-((_CBorder*)p2)->x;
}

static bool IsValidStringer(double dAreaShort, double dAreaLong, bool bOxide, bool bSulphide, int nObjects)
{
	if (dAreaLong > dAreaShort)
	{
		// string with long particles. Can be A or C.
		return bOxide&&!bSulphide||!bOxide&&bSulphide;
	}
	else
		return nObjects>=3;
}

static void AddStringerToList(_CObjArray &arrObjects, _CArray<int,100,100> &arrObjectsInStringer,
	int nObjectsInStringer, _CArray<_CBorder,100,10> &arrBorders, double dAngle, double dAreaLong,
	double dAreaShort, double dCoef, int iBeg, _CBorder &prev)
{
	for (int j = 0; j < nObjectsInStringer; j++)
	{
		int iObj = arrObjectsInStringer.GetAt(j);
		int iStringerObj = arrObjects.GetPtrAt(iObj).m_nStringer;
		if (iStringerObj == -1) continue;
		int nTotalObjects = arrObjects.GetSize();
		for (int k = 0; k < nTotalObjects; k++)
			if (arrObjects.GetPtrAt(k).m_nStringer == iStringerObj)
				arrObjects.GetPtrAt(k).m_nStringer = -1;
	}
	// Find object with minimal number. It will be stringer number.
	int iStringerObj = arrObjectsInStringer.GetAt(0);
	for (j = 1; j < nObjectsInStringer; j++)
		if (arrObjectsInStringer.GetAt(j) < iStringerObj)
			iStringerObj = arrObjectsInStringer.GetAt(j);
	// Mark all objects in stringer by stringer number.
	int nCenterSL = CenterScanLine(arrObjectsInStringer, arrObjects);
	for (j = 0; j < nObjectsInStringer; j++)
	{
		int iObj = arrObjectsInStringer.GetAt(j);
		arrObjects.GetPtrAt(iObj).m_nStringer = iStringerObj;
		arrObjects.GetPtrAt(iObj).m_dAngle = dAngle;
		arrObjects.GetPtrAt(iObj).m_dCoef = dCoef;
		arrObjects.GetPtrAt(iObj).m_nScanLine = nCenterSL;
		arrObjects.GetPtrAt(iObj).m_x1 = arrBorders.GetPtrAt(iBeg).x;
		arrObjects.GetPtrAt(iObj).m_x2 = prev.x;
		arrObjects.GetPtrAt(iObj).m_nStringerType = dAreaLong>=dAreaShort?1:0;
		arrObjects.GetPtrAt(iObj).m_dPartLength = arrObjects.GetPtrAt(iObj).m_rectRotated.width();
		arrObjects.GetPtrAt(iObj).m_dPartWidth = arrObjects.GetPtrAt(iObj).m_rectRotated.height();
	}
}

static void MakeObjectsNumsFromBorders(_CArray<int,100,100> &arrObjectsInStringer,
	_CArray<_CBorder,100,10> &arrBorders, int iBeg1, int iEnd1)
{
	for (int j = iBeg1; j <= iEnd1; j++)
		if (arrBorders.GetPtrAt(j).bStart)
			arrObjectsInStringer.Add(arrBorders.GetPtrAt(j).nObject);
}

static void FindStringerInSubsetForObject(_CObjArray &arrObjects, _CArray<_CBorder,100,10> &arrBorders,
	int iBeg, int iEnd, double dAngle)
{
	int nInsideObject = 1;
	_ptr_t2<bool> afOpenedObject(arrObjects.GetSize());
	int iObj = arrBorders.GetPtrAt(iBeg).nObject;
	afOpenedObject[iObj] = true;
	bool bSulphide = arrObjects.GetPtrAt(iObj).m_bSulfideParticle;
	bool bOxide = !bSulphide;
	double dCoef = 0.;
	double dAreaShort = 0.;
	double dAreaLong = 0.;
	if (IsElongated(arrObjects.GetPtrAt(iObj).m_dAspectRatio))
		dAreaLong += arrObjects.GetPtrAt(iObj).m_dArea;
	else
		dAreaShort += arrObjects.GetPtrAt(iObj).m_dArea;
	double dMaxCoef = arrObjects.GetPtrAt(iObj).m_dCoef;
	int nBorders = arrBorders.GetSize();
	_CArray<int,100,100> arrObjectsInStringer;
	arrObjectsInStringer.Add(iObj);
	for (int i = iBeg+1; i < iEnd; i++)
	{
		_CBorder &prev = arrBorders.GetPtrAt(i-1);
		_CBorder &next = arrBorders.GetPtrAt(i==nBorders?i-1:i);
		if (i==nBorders || prev.bStart == false && next.bStart == true && nInsideObject == 0 &&
			next.x - prev.x > nMaxDistX)
			break;
		if (next.bStart)
		{
			nInsideObject++;
			afOpenedObject[next.nObject] = true;
			arrObjectsInStringer.Add(next.nObject);
			if (IsElongated(arrObjects.GetPtrAt(next.nObject).m_dAspectRatio))
				dAreaLong += arrObjects.GetPtrAt(next.nObject).m_dArea;
			else
				dAreaShort += arrObjects.GetPtrAt(next.nObject).m_dArea;
			if (arrObjects.GetPtrAt(next.nObject).m_bSulfideParticle)
				bSulphide = true;
			else
				bOxide = true;
			int iStringerObject = arrObjects.GetPtrAt(next.nObject).m_nStringer;
			if (iStringerObject > -1)
			{
				double dMaxCoef1 = arrObjects.GetPtrAt(iStringerObject).m_dCoef;
				if (dMaxCoef1 > dMaxCoef)
					dMaxCoef = dMaxCoef1;
			}
		}
		else if (afOpenedObject[next.nObject] == true)
		{
			nInsideObject--;
			afOpenedObject[next.nObject] = false;
			if (nInsideObject == 0)
			{
				if (IsValidStringer(dAreaShort,dAreaLong,bOxide,bSulphide,arrObjectsInStringer.GetSize()))
				{
					double dLength = prev.x-arrBorders.GetPtrAt(iBeg).x;
					double dCoef = CalcCoef(dAngle,dLength,dAreaLong+dAreaShort);
					if (dCoef > dMaxCoef)
					{
						AddStringerToList(arrObjects, arrObjectsInStringer, arrObjectsInStringer.GetSize(),
							arrBorders, dAngle, dAreaLong, dAreaShort, dCoef, iBeg, next);
						dCoef = dMaxCoef;
					}
				}
			}
		}
	}
}

static void FindStringersInSubset(_CObjArray &arrObjects, _CArray<_CBorder,100,10> &arrBorders,
	int iBeg, int iEnd, double dAngle)
{
	for (int i = iBeg; i < iEnd; i++)
	{
		if (arrBorders.GetPtrAt(i).bStart)
			FindStringerInSubsetForObject(arrObjects, arrBorders, i, iEnd, dAngle);
	}
}

static double CalcMaxCoef(_CArray<int,100,100> &arrObjectsInStringer, int nObjectsInStringer,
	_CObjArray &arrObjects)
{
	double dMaxCoef = 0.;
	for (int j = 0; j < nObjectsInStringer; j++)
	{
		int iObj = arrObjectsInStringer.GetAt(j);
		int iStringerObj = arrObjects.GetPtrAt(iObj).m_nStringer;
		if (iStringerObj == -1) continue;
		if (arrObjects.GetPtrAt(iStringerObj).m_dCoef > dMaxCoef)
			dMaxCoef = arrObjects.GetPtrAt(iStringerObj).m_dCoef;
	}
	return dMaxCoef;
}

static void CalcStringerParameters(_CArray<int,100,100> &arrObjectsInStringer, int nObjectsInStringer,
	_CObjArray &arrObjects, double &dAreaShort, double &dAreaLong, bool &bOxide, bool &bSulphide)
{
	dAreaShort = 0.;
	dAreaLong = 0.;
	bOxide = bSulphide = false;
	for (int j = 0; j < nObjectsInStringer; j++)
	{
		int iObj = arrObjectsInStringer.GetAt(j);
		if (IsElongated(arrObjects.GetPtrAt(iObj).m_dAspectRatio))
			dAreaLong += arrObjects.GetPtrAt(iObj).m_dArea;
		else
			dAreaShort += arrObjects.GetPtrAt(iObj).m_dArea;
		if (arrObjects.GetPtrAt(iObj).m_bSulfideParticle)
			bSulphide = true;
		else
			bOxide = true;
	}
}

static bool StringerAlreadyExists(_CObjArray &arrObjects, _CArray<int,100,100> &arrObjectsInStringer,
	int nObjectsInStringer)
{
	int iObj = arrObjectsInStringer.GetAt(0);
	int iStringerObj = arrObjects.GetPtrAt(iObj).m_nStringer;
	for (int j = 1; j < nObjectsInStringer; j++)
	{
		int iObj = arrObjectsInStringer.GetAt(j);
		int iStringerObj1 = arrObjects.GetPtrAt(iObj).m_nStringer;
		if (iStringerObj1 != iStringerObj)
			return false;
	}
	return iStringerObj>=0;
}


void FindStringersForScanLine(int yScanLine, _CArray<int,1000,100> &arrSLObjectsNums, _CObjArray &arrObjects,
	double dAngle, SIZE sz)
{
	_CArray<_CBorder,100,10> arrBorders;
	int nObjects = arrSLObjectsNums.GetSize();
	for (int i = 0; i < nObjects; i++)
	{
		int iObject = arrSLObjectsNums.GetAt(i);
		CObjInfo &ObjInfo = arrObjects.GetPtrAt(iObject);
		_CBorder beg(true, iObject, ObjInfo.m_rectRotated.left);
		arrBorders.Add(beg);
		_CBorder end(false, iObject, ObjInfo.m_rectRotated.right);
		arrBorders.Add(end);
	}
	qsort(arrBorders.GetData(),arrBorders.GetSize(),sizeof(_CBorder),_bordersort);
	int nBorders = arrBorders.GetSize();
	int iBeg = 0;
	int nInsideObject = 1; // in arrBorders start of the object
	for (i = 1; i < nBorders+1; i++)
	{
		_CBorder &prev = arrBorders.GetPtrAt(i-1);
		_CBorder &next = arrBorders.GetPtrAt(i==nBorders?i-1:i);
		if (i==nBorders || prev.bStart == false && next.bStart == true && nInsideObject == 0 &&
			next.x - prev.x > nMaxDistX)
		{// end of stringer
			_CArray<int,100,100> arrObjectsInStringer;
			for (int j = iBeg; j < i; j++)
				if (arrBorders.GetPtrAt(j).bStart)
					arrObjectsInStringer.Add(arrBorders.GetPtrAt(j).nObject);
			double dLength = prev.x-arrBorders.GetPtrAt(iBeg).x;
			int nObjectsInStringer = arrObjectsInStringer.GetSize();
			if (!StringerAlreadyExists(arrObjects, arrObjectsInStringer, nObjectsInStringer))
			{
				double dMaxCoef = CalcMaxCoef(arrObjectsInStringer, nObjectsInStringer, arrObjects);
				double dAreaShort = 0., dAreaLong = 0.;
				bool bOxide,bSulphide;
				CalcStringerParameters(arrObjectsInStringer, nObjectsInStringer, arrObjects,
					dAreaShort, dAreaLong, bOxide, bSulphide);
				double dCoef = CalcCoef(dAngle,dLength,dAreaLong+dAreaShort);
				if (dCoef > dMaxCoef && // No preferrable stringers overlaps with it
					IsValidStringer(dAreaShort,dAreaLong,bSulphide,bOxide,nObjectsInStringer))
					AddStringerToList(arrObjects, arrObjectsInStringer, nObjectsInStringer,
						arrBorders, dAngle, dAreaLong, dAreaShort, dCoef, iBeg, prev);
				else if (nObjectsInStringer > 1)
					FindStringersInSubset(arrObjects, arrBorders, iBeg, i-1, dAngle); 
			}
			iBeg = i;
		}
		if (next.bStart)
			nInsideObject++;
		else
			nInsideObject--;
	}
}

void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
{
	RotateObjects(arrObjects, dAngle, ptOrg);
	_CArray<int,1000,100> ScanLines;
	_size sz(rc.right-rc.left,rc.bottom-rc.top);
	FindScanLines(arrObjects, ScanLines, sz);
	int nScanLines = ScanLines.GetSize();
	for (int i = 0; i < nScanLines; i++)
	{
		_CArray<int,1000,100> arrSLObjectsNums;
		FindObjectsForScanLine(ScanLines.GetAt(i), arrSLObjectsNums, arrObjects, sz);
		FindStringersForScanLine(ScanLines.GetAt(i), arrSLObjectsNums, arrObjects, dAngle, sz);
	}
}

void MapImage(IImage4 *pimgNew, IImage4 *pimgOld, int nPanes, int *pnPhasesHisto)
{
	int cxOld,cyOld;
	pimgOld->GetSize(&cxOld, &cyOld);
	POINT ptOffsNew,ptOffsOld;
	pimgNew->GetOffset(&ptOffsNew);
	pimgOld->GetOffset(&ptOffsOld);
	int dx = ptOffsOld.x-ptOffsNew.x;
	int dy = ptOffsOld.y-ptOffsNew.y;
	for (int y = 0; y < cyOld; y++)
	{
		byte *pRowMaskSrc,*pRowMaskDst;
		pimgOld->GetRowMask(y, &pRowMaskSrc);
		pimgNew->GetRowMask(y+dy, &pRowMaskDst);
		for (int x = 0; x < cxOld; x++)
			if (pRowMaskSrc[x])
				pRowMaskDst[dx+x] = pRowMaskSrc[x];
		for (int x = 0; x < cxOld; x++)
			pnPhasesHisto[pRowMaskSrc[x]]++;
		for (int iPane = 0; iPane < nPanes; iPane++)
		{
			color *pRowSrc,*pRowDst;
			pimgOld->GetRow(y, iPane, &pRowSrc);
			pimgNew->GetRow(y+dy, iPane, &pRowDst);
			memcpy(pRowDst+dx,pRowSrc,cxOld*sizeof(color));
		}
	}
}

struct CParticleThresh
{
	double dThin, dWide, dOversized;
public:
	void Init(double dCalibr, double d1, double d2, double d3)
	{
		dThin = McmToPoints(dCalibr, d1);
		dWide = McmToPoints(dCalibr, d2);
		dOversized = McmToPoints(dCalibr, d3);
	}
	int WidthClass(double dWidthMcm) {return dWidthMcm<dThin?-1:dWidthMcm<=dWide?0:dWidthMcm<=dOversized?1:2;}
};

class CParticleThresholds
{
	CParticleThresh m_ThreshClasses[4];
public:
	CParticleThresh &operator[](int i) {return m_ThreshClasses[i];}
	void Init(double dCalibr)
	{
		double dMin = GetValueDouble(GetAppUnknown(),"Stringers","MinObjectSizeMcm",2.);
		m_ThreshClasses[0].Init(dCalibr, dMin, 4., 12.);
		m_ThreshClasses[1].Init(dCalibr, dMin, 9., 15.);
		m_ThreshClasses[2].Init(dCalibr, dMin, 5., 12.);
		m_ThreshClasses[3].Init(dCalibr, dMin, 8., 13.);
	}
};

CParticleThresholds s_ThreshClasses;

int GetObjectType(int nStringerType, double dSulfide)
{
	int nClass;
	if (nStringerType >= 0)
	{ // Stringer
		if (nStringerType == 1) // elongated
			nClass = dSulfide>0.5?0:2; // A or C
		else // non-elongated
			nClass = 1; // already B
	}
	else
		nClass = 3; // D
	return nClass;
}

int CalculateWidthClassAC(double dAvrWidth, int nType)
{
	int nWidthClass = s_ThreshClasses[nType].WidthClass(dAvrWidth);
	return nWidthClass;
}

int CalculateWidthClassB(_CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
{
	double dSumLenByWidthClass[3];
	dSumLenByWidthClass[0] = dSumLenByWidthClass[1] = dSumLenByWidthClass[2] = 0.;
	for (int i = 0; i < nObjectsInStringer; i++)
	{
		CObjInfo *p = arrObj.GetAt(i);
		int nWidthClass = s_ThreshClasses[1].WidthClass(p->m_dPartWidth);
		if (nWidthClass >= 0 && nWidthClass < 3)
			dSumLenByWidthClass[nWidthClass] += p->m_dPartLength;
	}
	double dTotal = dSumLenByWidthClass[0]+dSumLenByWidthClass[1]+dSumLenByWidthClass[2];
	if (dSumLenByWidthClass[0]/dTotal >= 0.5)
		return 0; // thin
	else if (dSumLenByWidthClass[2]/dTotal >= 0.5)
		return 2; // oversized
	else
		return 1;
}

int CalculateWidthClass(double dAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer, int nType)
{
	if (nType == 0 || nType == 2)
		return CalculateWidthClassAC(dAvrWidth,nType);
	else if (nType == 1)
		return CalculateWidthClassB(arrObj,nObjectsInStringer);
	else
		return -1;
}

int ObjectClass(int nType, int nWidthClass)
{
	if (nType == -1 || nWidthClass == -1) return -1;
	return nType*3+nWidthClass;
}


bool MakeNewObject(_CArray<CObjInfo*,10,10> &arrObj, INamedDataObject2 *pObjList, POINT ptOrg,
   ICompManager *pGroupManager, char *szClassName)
{
	int nObjectsInStringer = arrObj.GetSize();
	CObjInfo *p = arrObj.GetAt(0);
	CPhasesCount PhasesCounts;
	// Calculate axis
	POINT pt1 = rotatepoint(ptOrg, _point(p->m_x1,p->m_nScanLine), -p->m_dAngle);
	POINT pt2 = rotatepoint(ptOrg, _point(p->m_x2,p->m_nScanLine), -p->m_dAngle);
	int nStringerType = p->m_nStringerType;
	double dStringerLenght = double(p->m_x2-p->m_x1)*dCalibr;
	double dRealLen = 0.,dRealAvrWidth = 0.;
	// Calculate rectangle
	POINT ptOffset;
	int cx,cy;
	p->m_pimg->GetOffset(&ptOffset);
	p->m_pimg->GetSize(&cx, &cy);
	_rect rc(ptOffset.x, ptOffset.y, ptOffset.x+cx, ptOffset.y+cy);
	IUnknownPtr punkCC;
	p->m_pimg->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC2(punkCC);
	BSTR bstrName;
	sptrCC2->GetCnvName(&bstrName);
	int nPanes;
	p->m_pimg->GetPanesCount(&nPanes);
//	double dPartLen,dParAvrWidth;
	CalcRealWidthAndHeight(*p, ptOrg, double(nMinObjectSize), p->m_dPartLength,
		p->m_dPartWidth, p->m_dPartArea);
	dRealLen += p->m_dPartLength;
	dRealAvrWidth += p->m_dPartWidth;
	int nNotSmall = p->m_dPartWidth>0.?1:0;
	for (int i = 1; i < nObjectsInStringer; i++)
	{
		p = arrObj.GetAt(i);
		p->m_pimg->GetOffset(&ptOffset);
		p->m_pimg->GetSize(&cx, &cy);
		if (ptOffset.x < rc.left) rc.left = ptOffset.x;
		if (ptOffset.y < rc.top) rc.top = ptOffset.y;
		if (ptOffset.x+cx > rc.right) rc.right = ptOffset.x+cx;
		if (ptOffset.y+cy > rc.bottom) rc.bottom = ptOffset.y+cy;
		CalcRealWidthAndHeight(*p, ptOrg, double(nMinObjectSize), p->m_dPartLength,
			p->m_dPartWidth, p->m_dPartArea);
		dRealLen += p->m_dPartLength;
		dRealAvrWidth += p->m_dPartWidth;
		if (p->m_dPartWidth > 0.) nNotSmall++;
	}
	if (nNotSmall) dRealAvrWidth /= nNotSmall;
	ptOffset.x = rc.left;
	ptOffset.y = rc.top;
	cx = rc.right-rc.left+1;
	cy = rc.bottom-rc.top+1;
	// Make new image and map data to it
	IUnknownPtr punkImage(CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	IImage4Ptr sptrImage(punkImage);
	sptrImage->CreateImage(cx, cy, bstrName, nPanes);
	sptrImage->InitRowMasks();
	sptrImage->SetOffset(ptOffset, FALSE);
	::SysFreeString(bstrName);
	for (int y = 0; y < cy; y++)
	{
		byte *pRowMask;
		sptrImage->GetRowMask(y, &pRowMask);
		memset(pRowMask, 0, cx);
	}
	for (i = 0; i < nObjectsInStringer; i++)
	{
		p = arrObj.GetAt(i);
		MapImage(sptrImage, p->m_pimg, nPanes, (int*)PhasesCounts);
	}
	if (bASTM && nObjectsInStringer == 1 && nStringerType == 0 && PhasesCounts.Sulfide() < 0.5)
		return false;
	sptrImage->InitContours();
	IUnknownPtr punkObject(CreateTypedObject(_bstr_t(szArgumentTypeObject)), false);
	// Make new object and add it to object list
	IMeasureObjectPtr sptrMO(punkObject);
	sptrMO->SetImage(sptrImage);
	// Calculate non-stringer parameters (if need)
	if (pGroupManager) 
	{
		int	nCount;
		pGroupManager->GetCount(&nCount);
		for (int idx = 0; idx < nCount; idx++)
		{
			IUnknownPtr punk;
			pGroupManager->GetComponentUnknownByIdx(idx, &punk);
			IMeasParamGroupPtr	ptrG(punk);
			if (ptrG == 0) continue;
			ptrG->CalcValues(punkObject, punkImage);
		}
	}
	// Set parameters
	ICalcObjectPtr sptrCO(punkObject);
	sptrCO->SetValue(KEY_PARAM_STRINGER_LENGHT, dStringerLenght);
	sptrCO->SetValue(KEY_PARAM_STRINGER_SUM_LENGHT, dRealLen*dCalibr);
	sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH, dRealAvrWidth*dCalibr);
	// Make child : axis
	IUnknownPtr punkAxis(CreateTypedObject(_bstr_t(szTypeStringerAxis)), false);
	if (punkAxis != 0)
	{
		sptrINamedDataObject2 sptrNO2(punkAxis);
		sptrNO2->SetParent(punkObject, 0);
		IStringerAxisPtr sptrAxis(punkAxis);
		sptrAxis->SetCoords(pt1, pt2);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_LENGHT, dStringerLenght);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_SUM_LENGHT, dRealLen*dCalibr);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_WIDTH, dRealAvrWidth*dCalibr);
	}
	// Classify it
	int nType = GetObjectType(nStringerType, PhasesCounts.Sulfide());
	int nWidthClass = CalculateWidthClass(dRealAvrWidth,arrObj,nObjectsInStringer,nType);
	int nClass = ObjectClass(nType, nWidthClass);
	set_object_class(sptrCO, nClass, szClassName);
	// Add object to list
	if (nClass > -1 || nIgnoreSmall != 2)
	{
		INamedDataObject2Ptr sptrNDO2(punkObject);
		sptrNDO2->SetParent(pObjList, 0);
	}
	return true;
}

static IUnknownPtr FindComponentByName(IUnknown *punkThis, const IID iid, _bstr_t strName)
{
	_bstr_t strCompName;
	if (CheckInterface(punkThis, iid))
	{
		strCompName = GetName(punkThis);
		if (strCompName == strName)
			return punkThis;
	}
	IUnknownPtr punk;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);
			if (punk != 0)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
			}
		}
	}
	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknownPtr punkParent;
		sptrR->GetParent(&punkParent);
		punk = FindComponentByName(punkParent, iid, strName);
	}
	return punk;
}



void MakeNewObjectsList(_CObjArray &arrObjects, INamedDataObject2 *pObjList, POINT ptOrg,
	BOOL bCalc, char *szClassName)
{
	// Prepare for parameter calculation (if need)
	ICompManagerPtr	sptrMeasMgr;
	if (bCalc)
	{
		sptrMeasMgr = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
		int	nCount;
		sptrMeasMgr->GetCount( &nCount );
		for( int idx = 0; idx < nCount; idx++ )
		{
			IUnknownPtr	punk;
			sptrMeasMgr->GetComponentUnknownByIdx(idx, &punk);
			IMeasParamGroupPtr	ptrG(punk);
			if( ptrG == 0 )continue;
			ptrG->InitializeCalculation(pObjList);
		}
	}
	// Make objects from stringers.
	int nObjects = arrObjects.GetSize();
	for (int i = 0; i < nObjects; i++)
	{
		CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
		if (ObjInfo.m_nStringer == i) // New stringer
		{
			int iStringerObj = ObjInfo.m_nStringer;
			_CArray<CObjInfo*,10,10> arrObj;
			arrObj.Add(&ObjInfo);
			for (int j = i+1; j < nObjects; j++)
			{
				CObjInfo &ObjInfo1 = arrObjects.GetPtrAt(j);
				if (ObjInfo1.m_nStringer == iStringerObj)
					arrObj.Add(&ObjInfo1);
			}
			if (!MakeNewObject(arrObj,pObjList,ptOrg, bCalc?(ICompManager *)sptrMeasMgr:NULL,szClassName))
				ObjInfo.m_nStringer = -1;
		}
	}
	// Copy unchanged objects
	for (int i = 0; i < nObjects; i++)
	{
		CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
		if (ObjInfo.m_nStringer < 0)
		{
			// Clone object
			IClonableObjectPtr sptrClone(ObjInfo.m_punkObj);
			IUnknownPtr punkCloned;
			if (sptrClone == 0) continue;
			sptrClone->Clone(&punkCloned);
			INamedDataObject2Ptr sptrObjectNDO2(punkCloned);
			if (sptrObjectNDO2 == 0) continue;
			int nClass = -1;
			if (!ObjInfo.m_bDisabled)
			{
				// calculate oxide and sulfide parts
				CPhasesCount PhasesCounts;
				int cxObj,cyObj;
				ObjInfo.m_pimg->GetSize(&cxObj,&cyObj);
				for (int y = 0; y < cyObj; y++)
				{
					byte *pRowMask;
					ObjInfo.m_pimg->GetRowMask(y, &pRowMask);
					PhasesCounts.Row(pRowMask, cxObj);
				}
				// Set values and classify object
				ICalcObjectPtr sptrCO(punkCloned);
				sptrCO->SetValue(KEY_PARAM_STRINGER_LENGHT, ObjInfo.m_dPartLength*dCalibr);
				sptrCO->SetValue(KEY_PARAM_STRINGER_SUM_LENGHT, ObjInfo.m_dPartLength*dCalibr);
				sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH, ObjInfo.m_dPartWidth*dCalibr);
				int nType = GetObjectType(-1, PhasesCounts.Sulfide());
				int nWidthClass = s_ThreshClasses[3].WidthClass(ObjInfo.m_dPartWidth);
				nClass = ObjectClass(nType, nWidthClass);
				set_object_class(sptrCO, nClass, szClassName);
				// Make dummy axis to store parameters
				IUnknownPtr punkAxis(CreateTypedObject(_bstr_t(szTypeStringerAxis)), false);
				if (punkAxis != 0)
				{
					sptrINamedDataObject2 sptrNO2(punkAxis);
					sptrNO2->SetParent(punkCloned, 0);
					IStringerAxisPtr sptrAxis(punkAxis);
					sptrAxis->SetCoords(_point(-1,-1), _point(-1,-1));
					sptrAxis->SetParameter(KEY_PARAM_STRINGER_LENGHT, ObjInfo.m_dPartLength*dCalibr);
					sptrAxis->SetParameter(KEY_PARAM_STRINGER_SUM_LENGHT, ObjInfo.m_dPartLength*dCalibr);
					sptrAxis->SetParameter(KEY_PARAM_STRINGER_WIDTH, ObjInfo.m_dPartWidth*dCalibr);
				}
			}
			if (nClass != -1 || nIgnoreSmall != 2)
				sptrObjectNDO2->SetParent(pObjList, 0);
		}
	}
	// Finalize calculation (if need)
	if (bCalc)
	{
		sptrMeasMgr = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
		int	nCount;
		sptrMeasMgr->GetCount( &nCount );
		for( int idx = 0; idx < nCount; idx++ )
		{
			IUnknownPtr	punk;
			sptrMeasMgr->GetComponentUnknownByIdx(idx, &punk);
			IMeasParamGroupPtr	ptrG(punk);
			if( ptrG == 0 )continue;
			ptrG->FinalizeCalculation();
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// Search of the stringers: new mechanism.
///////////////////////////////////////////////////////////////////////////

struct CStringerParams
{
	double dAreaShort;
	double dAreaLong;
	double dAreaSuperLong,dAreaSubLong;
	bool bOxide;
	bool bSulphide;
	int nObjectsInStringer;
	_CObjArray &arrObjects;
	_CArray<int,100,100> &arrObjectsInStringer;
	int yUpperObjectCenter,yLowerObjectCenter;
	RECT rcBounds;
	int nStringerType;
	double dPhase128,dPhase129,dPhase130,dPhase131;
	double dRough;
	double dBiggestObjArea,dBiggestGlobular;
	// For stringer center calculation
	double dSumAreaCen,dAxisPos;
	int nSuperLongCount;
	double dBiggestWidth;
	double dAreaOfSmallGlob;
	CStringerParams(_CObjArray &arrObjs, _CArray<int,100,100> &arrObjsInStringer) :
		arrObjects(arrObjs), arrObjectsInStringer(arrObjsInStringer)
	{
		nObjectsInStringer = arrObjectsInStringer.GetSize();
		bOxide = bSulphide = false;
		dAreaShort = dAreaLong = dAreaSuperLong = dAreaSubLong = 0.;
		yUpperObjectCenter = yLowerObjectCenter = -1;
		nStringerType = -1;
		dPhase128 = dPhase129 = dPhase130 = dPhase131 = 0.;
		dRough = 0.;
		dBiggestObjArea = 0.;
		dSumAreaCen = 0;
		dAxisPos = 0;
		dBiggestGlobular = 0.;
		nSuperLongCount = 0;
		dBiggestWidth = 0;
		dAreaOfSmallGlob = 0.;
	}
};

static void AddAxis(IUnknown *punkObject, POINT pt1, POINT pt2, double dStringerLength, double dSumLen,
	double dStringerWidth)
{
	IUnknownPtr punkAxis(CreateTypedObject(_bstr_t(szTypeStringerAxis)), false);
	if (punkAxis != 0)
	{
		sptrINamedDataObject2 sptrNO2(punkAxis);
		sptrNO2->SetParent(punkObject, 0);
		IStringerAxisPtr sptrAxis(punkAxis);
		sptrAxis->SetCoords(pt1, pt2);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_LENGHT, dStringerLength);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_SUM_LENGHT, dSumLen);
		sptrAxis->SetParameter(KEY_PARAM_STRINGER_WIDTH, dStringerWidth);
	}
}

class CWidthClasses
{
	double m_dThresholds[5][5];
public:
	int m_nTypes,m_nWidthClasses;
	CWidthClasses()
	{
		m_nTypes = 0;
		m_nWidthClasses = 0;
	}
	CWidthClasses(int nTypes, int nWidthClasses)
	{
		m_nTypes = nTypes;
		m_nWidthClasses = nWidthClasses;
	}
	void Init(int nTypes, int nWidthClasses)
	{
		m_nTypes = nTypes;
		m_nWidthClasses = nWidthClasses;
	}
	void Init(int nType, double d0, double d1, double d2 = 0., double d3 = 0., double d4 = 0.)
	{
		m_dThresholds[nType][0] = d0;
		m_dThresholds[nType][1] = d1;
		m_dThresholds[nType][2] = d2;
		m_dThresholds[nType][3] = d3;
		m_dThresholds[nType][4] = d4;
	}
	void ReadSettings(char *pszSection)
	{
		char szSection[256];
		for (int i = 0; i < m_nTypes; i++)
		{
			sprintf(szSection, "%s\\Thresholds\\%d", pszSection, i);
			for (int j = 0; j < m_nWidthClasses; j++)
			{
				char szKey[256];
				sprintf(szKey,"%d",j);
				m_dThresholds[i][j] = GetValueDouble(GetAppUnknown(), szSection, szKey, m_dThresholds[i][j]);
			}
		}
	}
	int Classify(int nType, double dWidth)
	{
		if (dWidth < m_dThresholds[nType][0])
			return -1;
		for (int i = 0; i < m_nWidthClasses-1; i++)
		{
			if(dWidth >= m_dThresholds[nType][i] && dWidth <= m_dThresholds[nType][i+1])
				return i;
		}
		return m_nWidthClasses-1;
	}
};

class CStandard
{
protected:
	char m_sName[20];
	void GetSectionName(char *pszBuff)
	{
		strcpy(pszBuff, "\\Stringers\\");
		strcat(pszBuff, m_sName);
	};
	double m_dMaxDistXMcmDef;
	double m_dMaxDistYMcmDef;
	double m_dMinObjectSizeMcmDef;
	double m_dElongatedRatio;
	_bstr_t m_bstrTypeClassifier,m_bstrWidthClassifier;
	BOOL m_bExcludePerpendicular;
	bool m_bAnyStringerAdded;
	bool m_bMessageThrown;
	BOOL m_bAxisMode,m_bCorrectAxis;

	CWidthClasses m_WidthClasses;
	double CalcObjectThickness(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dArea/(ObjInfo.m_dPartLength*dCalibr);
	}
	double m_dWidthThreshold;

	HRESULT COGetValue(ICalcObject *ptrCO, long lParamKey, double *pdValue)
	{
		HRESULT hr = ptrCO->GetValue(lParamKey, pdValue);
		if (hr != S_OK && !m_bMessageThrown)
		{
			m_bMessageThrown = true;
			char szBuff[512];
			sprintf(szBuff, "Parameter %d not found", lParamKey);
			VTMessageBox(szBuff, "vt5", MB_OK|MB_ICONEXCLAMATION);
		}
		return hr;
	}

public:
	CStandard()
	{
		strcpy(m_sName, "General");
		nIgnoreSmall = 1; // Whether ignore small particles
		m_dMinObjectSizeMcmDef = 2.;
		dRatioThresh = 2.; // Ignore particles with aspect ratio > dRatioThresh and angle > dAngleThreshold
		dAngleThreshold = PI/4.;
		m_dMaxDistXMcmDef = 40.;
		m_dMaxDistYMcmDef = 15.;
		m_dElongatedRatio = 5.; // Object with such or more ration will be classified as elongated
		m_bExcludePerpendicular = TRUE;
		m_bAnyStringerAdded = false;
		m_dWidthThreshold = 0.3;
		m_bMessageThrown = false;
		m_bAxisMode = false;
		m_bCorrectAxis = false;
	};
	virtual void InitThresholds(double dCalibr)
	{
		s_ThreshClasses.Init(dCalibr);
	}
	virtual char *GetName() {return m_sName;}
	virtual void ReadSettings()
	{
		char szSection[256];
		GetSectionName(szSection);
		// Setting will be stored in global variables. It would be better to store it in class
		// members, but common utilities uses global variables.
		dCostCoef = GetValueDouble(GetAppUnknown(),szSection,"CostCoefficient",0.01);
		dAngleThreshold = GetValueDouble(GetAppUnknown(),szSection,"IgnoredPariclesAngle",dAngleThreshold);
		dRatioThresh = GetValueDouble(GetAppUnknown(),szSection,"IgnoredPariclesAspectRatio",dRatioThresh);
		m_dElongatedRatio = GetValueDouble(GetAppUnknown(),szSection,"ElongatedParticlesAspectRatio",m_dElongatedRatio);
		nIgnoreSmall = GetValueInt(GetAppUnknown(),szSection,"IgnoreSmall",nIgnoreSmall);
		m_bExcludePerpendicular = GetValueInt(GetAppUnknown(),szSection,"IgnorePerpendicularParticles",m_bExcludePerpendicular);
		m_bAxisMode = GetValueInt(GetAppUnknown(),szSection,"AxisOnObjectCenters",m_bAxisMode);
		m_bCorrectAxis = GetValueInt(GetAppUnknown(),szSection,"CorrectAxis",m_bCorrectAxis);
		// Init angles and distances (some hardcoded)
		int nAngles = int(ceil(dAngleScope/dDeltaAngle)+1.);
		GuidKey guid;
		GetDefaultCalibration(&dCalibr, &guid);
		InitThresholds(dCalibr);
//		s_ThreshClasses.Init(dCalibr);
		double dMaxDistXMcm = GetValueDouble(GetAppUnknown(),szSection,"MaxDistXMcm",m_dMaxDistXMcmDef);
		double dMaxDistYMcm = GetValueDouble(GetAppUnknown(),szSection,"MaxDistYMcm",m_dMaxDistYMcmDef);
		nMaxDistX = (int)(McmToPoints(dCalibr,dMaxDistXMcm));
		nMaxDistY = (int)(McmToPoints(dCalibr,dMaxDistYMcm));
		double dMinObjectSizeMcm = GetValueDouble(GetAppUnknown(),szSection,"MinObjectSizeMcm",m_dMinObjectSizeMcmDef);
		nMinObjectSize = (int)(McmToPoints(dCalibr,dMinObjectSizeMcm));
		m_bstrTypeClassifier = GetValueString(GetAppUnknown(),szSection,"TypeClassifier","StringerType.class");
		m_bstrWidthClassifier = GetValueString(GetAppUnknown(),szSection,"WidthClassifier","StringerWidth.class");
	}
	// Utilities
	virtual bool IsElongated(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=m_dElongatedRatio;
	}
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		return (dAngleScope-dCostCoef*fabs(dAngle))*(dLen+sqrt(dArea));
	}
	virtual void CalcStringerParameters(CStringerParams &Params)
	{
		for (int j = 0; j < Params.nObjectsInStringer; j++)
		{
			int iObj = Params.arrObjectsInStringer.GetAt(j);
			CalcStringerParamsForObj(Params, iObj);
		}
	}
	virtual void CalcStringerParamsForObj(CStringerParams &Params, int nObj)
	{
		CObjInfo &ObjInfo(Params.arrObjects.GetPtrAt(nObj));
		if (IsElongated(Params.arrObjects.GetPtrAt(nObj)) && IsValidAngle(Params.arrObjects.GetPtrAt(nObj)))
			Params.dAreaLong += Params.arrObjects.GetPtrAt(nObj).m_dArea;
		else
			Params.dAreaShort += Params.arrObjects.GetPtrAt(nObj).m_dArea;
		if (Params.arrObjects.GetPtrAt(nObj).m_bSulfideParticle)
			Params.bSulphide = true;
		else
			Params.bOxide = true;
		Params.nObjectsInStringer = Params.arrObjectsInStringer.GetSize();
		if (m_bCorrectAxis)
		{
			Params.dSumAreaCen += ObjInfo.m_dArea*ObjInfo.m_dYCoord;
			Params.dAxisPos = Params.dSumAreaCen/(Params.dAreaShort+Params.dAreaLong);
		}
	}
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		if (Params.dAreaLong > Params.dAreaShort)
		{
			// string with long particles. Can be A or C.
			return Params.bOxide&&!Params.bSulphide||!Params.bOxide&&Params.bSulphide;
		}
		else
			return Params.nObjectsInStringer>=3;
	}
	virtual bool IsValidDistances(CStringerParams &Params)
	{
		for (int j = 0; j < Params.nObjectsInStringer; j++)
		{
			int iObj = Params.arrObjectsInStringer.GetAt(j);
			CObjInfo &ObjInfo(Params.arrObjects.GetPtrAt(iObj));
			if (!(Params.dAxisPos >= ObjInfo.m_rectRotated.top-nMaxDistY &&
				Params.dAxisPos <= ObjInfo.m_rectRotated.bottom+nMaxDistY))
				return false;
		}
		return true;
	}
	virtual bool CanBeStringer(int nObjectsInStringer, int nStringerType, CPhasesCount &PhasesCounts)
	{
		if (nObjectsInStringer == 1 && nStringerType == 0 && PhasesCounts.Sulfide() < 0.5)
			return false;
		return true;
	}
	virtual void AddStringerToList(_CObjArray &arrObjects, _CArray<int,100,100> &arrObjectsInStringer,
		int nObjectsInStringer, _CArray<_CBorder,100,10> &arrBorders, double dAngle, int nStringerType,
		double dCoef, int iBeg, _CBorder &prev, CStringerParams &Params)
	{
		for (int j = 0; j < nObjectsInStringer; j++)
		{
			int iObj = arrObjectsInStringer.GetAt(j);
			int iStringerObj = arrObjects.GetPtrAt(iObj).m_nStringer;
			if (iStringerObj == -1) continue;
			int nTotalObjects = arrObjects.GetSize();
			for (int k = 0; k < nTotalObjects; k++)
				if (arrObjects.GetPtrAt(k).m_nStringer == iStringerObj)
					arrObjects.GetPtrAt(k).m_nStringer = -1;
		}
		// Find object with minimal number. It will be stringer number.
		int iStringerObj = arrObjectsInStringer.GetAt(0);
		for (j = 1; j < nObjectsInStringer; j++)
			if (arrObjectsInStringer.GetAt(j) < iStringerObj)
				iStringerObj = arrObjectsInStringer.GetAt(j);
		// Mark all objects in stringer by stringer number.
		int nCenterSL = m_bCorrectAxis?
			(int)(Params.dSumAreaCen/(Params.dAreaShort+Params.dAreaLong)):
			CenterScanLine(arrObjectsInStringer, arrObjects);
		for (j = 0; j < nObjectsInStringer; j++)
		{
			int iObj = arrObjectsInStringer.GetAt(j);
			arrObjects.GetPtrAt(iObj).m_nStringer = iStringerObj;
			arrObjects.GetPtrAt(iObj).m_dAngle = dAngle;
			arrObjects.GetPtrAt(iObj).m_dCoef = dCoef;
			arrObjects.GetPtrAt(iObj).m_nScanLine = nCenterSL;
			arrObjects.GetPtrAt(iObj).m_x1 = arrBorders.GetPtrAt(iBeg).x;
			arrObjects.GetPtrAt(iObj).m_x2 = prev.x;
			arrObjects.GetPtrAt(iObj).m_nStringerType = nStringerType;//dAreaLong>=dAreaShort?1:0;
			arrObjects.GetPtrAt(iObj).m_dPartLength = arrObjects.GetPtrAt(iObj).m_rectRotated.width();
			arrObjects.GetPtrAt(iObj).m_dPartWidth = arrObjects.GetPtrAt(iObj).m_rectRotated.height();
		}
		m_bAnyStringerAdded = true;
	}
	virtual int GetStringerType(CStringerParams &StringerParameters)
		{return StringerParameters.dAreaLong>=StringerParameters.dAreaShort?1:0;}
	virtual bool IsValidAngle(CObjInfo &Obj)
	{
		return !(Obj.m_dParticleOrientation > dAngleThreshold ||
			Obj.m_dParticleOrientation < -dAngleThreshold);
	}
	// Preprocessing of objects
	virtual void PreprocessObject(CObjInfo &ObjCur, double dDeltaAngle, FPOINT ptCenter)
	{
		ObjCur.m_dAspectRatio = 0.;
		ObjCur.m_dPartLength = 0.;
		ObjCur.m_dPartWidth = 0.;
		ObjCur.m_dParticleOrientation = 0.;
		ICalcObjectPtr sptrCO(ObjCur.m_punkObj);
		if (COGetValue(sptrCO, KEY_ANGLE, &ObjCur.m_dParticleOrientation) != S_OK ||
			COGetValue(sptrCO, KEY_NOUN, &ObjCur.m_dAspectRatio) != S_OK ||
			COGetValue(sptrCO, KEY_WIDTH, &ObjCur.m_dPartWidth) != S_OK ||
			COGetValue(sptrCO, KEY_LENGTH, &ObjCur.m_dPartLength) != S_OK)
		{
			ObjCur.m_bDisabled = true;
			return;
		}
		if (ObjCur.m_dParticleOrientation > PI/2.) ObjCur.m_dParticleOrientation -= PI;
		ObjCur.m_dPartWidth /= dCalibr;
		ObjCur.m_dPartLength /= dCalibr;

		if (m_bAxisMode)
		{
			if (COGetValue(sptrCO, KEY_XCOORD, &ObjCur.m_dXCoord) != S_OK ||
				COGetValue(sptrCO, KEY_YCOORD, &ObjCur.m_dYCoord) != S_OK)
			{
				ObjCur.m_bDisabled = true;
				return;
			}
		}
		ObjCur.m_dXCoord /= dCalibr;
		ObjCur.m_dYCoord /= dCalibr;

		if (ObjCur.m_dPartLength < nMinObjectSize && nIgnoreSmall > 0 ||
			ObjCur.m_dAspectRatio > dRatioThresh && (ObjCur.m_dParticleOrientation > dAngleThreshold ||
			ObjCur.m_dParticleOrientation < -dAngleThreshold) && m_bExcludePerpendicular)
			ObjCur.m_bDisabled = true;
	}
	virtual void PreprocessObjects(_CObjArray &arrObjs, double dDeltaAngle, POINT ptCen)
	{
		FPOINT ptCenter(ptCen);
		dprintf("\n");
		for (int j = 0; j < arrObjs.GetSize(); j++)
		{
			CObjInfo &ObjCur = arrObjs.GetPtrAt(j);
			PreprocessObject(ObjCur, dDeltaAngle, ptCenter);
			if (g_nDebug == 1)
			{
				dprintf("%d (%d,%d)-(%d,%d), ", j, ObjCur.m_rectOrig.left, ObjCur.m_rectOrig.top,
					ObjCur.m_rectOrig.right, ObjCur.m_rectOrig.bottom);
				dprintf("m_dAspectRatio = %f, m_dParticleOrientation = %f, m_dArea = %g\n",
					ObjCur.m_dAspectRatio, ObjCur.m_dParticleOrientation, ObjCur.m_dArea);
			}
		}
	}

	// Stringer search
	virtual void FindStringerInSubsetForObject(_CObjArray &arrObjects, _CArray<_CBorder,100,10> &arrBorders,
		int iBeg, int iEnd, double dAngle)
	{
		_CArray<int,100,100> arrObjectsInStringer;
		int nInsideObject = 1;
		_ptr_t2<bool> afOpenedObject(arrObjects.GetSize());
		int iObj = arrBorders.GetPtrAt(iBeg).nObject;
		afOpenedObject[iObj] = true;
		CStringerParams StringerParameters(arrObjects,arrObjectsInStringer);
		CalcStringerParamsForObj(StringerParameters, iObj);
		double dCoef = 0.;
		double dMaxCoef = arrObjects.GetPtrAt(iObj).m_dCoef;
		int nBorders = arrBorders.GetSize();
		arrObjectsInStringer.Add(iObj);
		for (int i = iBeg+1; i <= iEnd; i++)
		{
			_CBorder &prev = arrBorders.GetPtrAt(i-1);
			_CBorder &next = arrBorders.GetPtrAt(i>=nBorders?i-1:i);
			if (i==nBorders || prev.bStart == false && next.bStart == true && nInsideObject == 0 &&
				next.x - prev.x > nMaxDistX)
				break;
			if (next.bStart)
			{
				nInsideObject++;
				afOpenedObject[next.nObject] = true;
				arrObjectsInStringer.Add(next.nObject);
				CalcStringerParamsForObj(StringerParameters, next.nObject);
				int iStringerObject = arrObjects.GetPtrAt(next.nObject).m_nStringer;
				if (iStringerObject > -1)
				{
					double dMaxCoef1 = arrObjects.GetPtrAt(iStringerObject).m_dCoef;
					if (dMaxCoef1 > dMaxCoef)
						dMaxCoef = dMaxCoef1;
				}
			}
			else if (afOpenedObject[next.nObject] == true)
			{
				nInsideObject--;
				afOpenedObject[next.nObject] = false;
				if (nInsideObject == 0)
				{
					if (g_nDebug == 1)
					{
						dprintf("Stringer (");
						for (int i = 0; i < arrObjectsInStringer.GetSize(); i++)
							dprintf("%d,", (int)arrObjectsInStringer.GetAt(i));
						dprintf(") l=%g,s=%g", StringerParameters.dAreaLong, StringerParameters.dAreaShort);
					}
					if (IsValidStringer(StringerParameters))
					{
						double dLength = prev.x-arrBorders.GetPtrAt(iBeg).x;
						double dCoef = CalcCoef(dAngle,dLength,StringerParameters.dAreaLong+StringerParameters.dAreaShort,StringerParameters);
						if (dCoef > dMaxCoef)
						{
							AddStringerToList(arrObjects, arrObjectsInStringer,
								arrObjectsInStringer.GetSize(),
								arrBorders, dAngle, GetStringerType(StringerParameters),
								dCoef, iBeg, next, StringerParameters);
							dMaxCoef = dCoef;
							dprintf(" added in subset\n");
						}
						else
							dprintf(" not added in subset, %g<%g\n", dCoef,dMaxCoef);
					}
					else
						dprintf(" not added in subset\n");
				}
			}
		}
	}
	virtual void FindStringersInSubset(_CObjArray &arrObjects, _CArray<_CBorder,100,10> &arrBorders,
		int iBeg, int iEnd, double dAngle)
	{
		for (int i = iBeg; i < iEnd; i++)
		{
			if (arrBorders.GetPtrAt(i).bStart)
				FindStringerInSubsetForObject(arrObjects, arrBorders, i, iEnd, dAngle);
		}
	}
	virtual void FindStringersForScanLine(int yScanLine, _CArray<int,1000,100> &arrSLObjectsNums, _CObjArray &arrObjects,
		double dAngle, SIZE sz)
	{
//		m_bAnyStringerAdded = false;
		_CArray<_CBorder,100,10> arrBorders;
		int nObjects = arrSLObjectsNums.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			int iObject = arrSLObjectsNums.GetAt(i);
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(iObject);
			_CBorder beg(true, iObject, ObjInfo.m_rectRotated.left);
			arrBorders.Add(beg);
			_CBorder end(false, iObject, ObjInfo.m_rectRotated.right);
			arrBorders.Add(end);
		}
		qsort(arrBorders.GetData(),arrBorders.GetSize(),sizeof(_CBorder),_bordersort);
		int nBorders = arrBorders.GetSize();
		int iBeg = 0;
		int nInsideObject = 1; // in arrBorders start of the object
		for (i = 1; i < nBorders+1; i++)
		{
			_CBorder &prev = arrBorders.GetPtrAt(i-1);
			_CBorder &next = arrBorders.GetPtrAt(i==nBorders?i-1:i);
			if (i==nBorders || prev.bStart == false && next.bStart == true && nInsideObject == 0 &&
				next.x - prev.x > nMaxDistX)
			{// end of stringer
				_CArray<int,100,100> arrObjectsInStringer;
				for (int j = iBeg; j < i; j++)
					if (arrBorders.GetPtrAt(j).bStart)
						arrObjectsInStringer.Add(arrBorders.GetPtrAt(j).nObject);
				double dLength = prev.x-arrBorders.GetPtrAt(iBeg).x;
				int nObjectsInStringer = arrObjectsInStringer.GetSize();
				if (!StringerAlreadyExists(arrObjects, arrObjectsInStringer, nObjectsInStringer))
				{
					double dMaxCoef = CalcMaxCoef(arrObjectsInStringer, nObjectsInStringer, arrObjects);
					CStringerParams StringerParams(arrObjects, arrObjectsInStringer);
					CalcStringerParameters(StringerParams);
					double dCoef = CalcCoef(dAngle,dLength,StringerParams.dAreaLong+StringerParams.dAreaShort,StringerParams);
					if (g_nDebug == 1)
					{
						dprintf("Stringer (");
						for (int i = 0; i < nObjectsInStringer; i++)
							dprintf("%d,", (int)arrObjectsInStringer.GetAt(i));
						dprintf(") Coefs %g,%g ", dCoef, dMaxCoef);
					}
					if (dCoef > dMaxCoef && // No preferrable stringers overlaps with it
						IsValidStringer(StringerParams))
					{
						dprintf("added\n");
						AddStringerToList(arrObjects, arrObjectsInStringer,
							nObjectsInStringer, arrBorders, dAngle,
							GetStringerType(StringerParams), dCoef, iBeg,
							prev, StringerParams);
					}
					else if (nObjectsInStringer > 1)
					{
						dprintf("Not added. Finding substrings...\n");
						FindStringersInSubset(arrObjects, arrBorders, iBeg, i-1, dAngle); 
					}
				}
				iBeg = i;
			}
			if (next.bStart)
				nInsideObject++;
			else
				nInsideObject--;
		}
	}
	virtual bool ObjectCanBeInStringer(CObjInfo &ObjInfo) {return true;}
	virtual void FindScanLines1(_CObjArray &arrObjects, _CArray<int,1000,100> &ScanLines, SIZE sz)
	{
		_CArray<int,1000,100> SLineTmp;
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			SLineTmp.Add((int)ObjInfo.m_dYCoord);
		}
		qsort(SLineTmp.GetData(), SLineTmp.GetSize(), sizeof(int), _intsort);
		int prev = -1;
		int nSLineTmp = SLineTmp.GetSize();
		for (i = 0; i < nSLineTmp; i++)
		{
			int n = SLineTmp.GetAt(i);
			if (n != prev)
				ScanLines.Add(n);
			prev = n;
		}
	}
	virtual void FindScanLines(_CObjArray &arrObjects, _CArray<int,1000,100> &ScanLines, SIZE sz)
	{
		if (m_bAxisMode)
			FindScanLines1(arrObjects, ScanLines, sz);
		else
			::FindScanLines(arrObjects, ScanLines, sz);

	}
	virtual void FindObjectsForScanLine(int yScanLine, _CArray<int,1000,100> &arrSLObjectsNums, _CObjArray &arrObjects,
		SIZE sz, bool bOnlyNotInStringer = false)
	{
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			if (yScanLine >= ObjInfo.m_rectRotated.top-nMaxDistY && yScanLine <= ObjInfo.m_rectRotated.bottom+nMaxDistY)
			{
				if (!ObjInfo.m_bDisabled && ObjectCanBeInStringer(ObjInfo) &&
					(!bOnlyNotInStringer  || ObjInfo.m_nStringer == -1))
					arrSLObjectsNums.Add(i);
			}
		}
	}
	virtual void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
	{
		RotateObjects(arrObjects, dAngle, ptOrg);
		_CArray<int,1000,100> ScanLines;
		_size sz(rc.right-rc.left,rc.bottom-rc.top);
		FindScanLines(arrObjects, ScanLines, sz);
		int nScanLines = ScanLines.GetSize();

		for (int i = 0; i < nScanLines; i++)
		{
			_CArray<int,1000,100> arrSLObjectsNums;
			FindObjectsForScanLine(ScanLines.GetAt(i), arrSLObjectsNums, arrObjects, sz);
			if (g_nDebug == 1)
			{
				dprintf("Scanline %d:", ScanLines.GetAt(i));
				for (int j = 0; j < arrSLObjectsNums.GetSize(); j++)
					dprintf("%d,",arrSLObjectsNums.GetAt(j));
				dprintf("\n");
			}
			FindStringersForScanLine(ScanLines.GetAt(i), arrSLObjectsNums, arrObjects, dAngle, sz);
/*			for (int k = 0; k < 10; k++)
			{
				if (g_nDebug == 1)
					dprintf("Try find additional stringers, iteralion %d\n", k);
				_CArray<int,1000,100> arrSLObjectsNums1;
				m_bAnyStringerAdded = false;
				FindObjectsForScanLine(ScanLines.GetAt(i), arrSLObjectsNums1, arrObjects, sz, true);
				if (g_nDebug == 1)
				{
					dprintf("Scanline %d (additional stringers, iteration %d):", ScanLines.GetAt(i), k);
					for (int j = 0; j < arrSLObjectsNums1.GetSize(); j++)
						dprintf("%d,",arrSLObjectsNums1.GetAt(j));
					dprintf("\n");
				}
				FindStringersForScanLine(ScanLines.GetAt(i), arrSLObjectsNums1, arrObjects, dAngle, sz);
				if (!m_bAnyStringerAdded)
					break;
			}*/
		}
		for (int k = 0; k < 10; k++)
		{
			m_bAnyStringerAdded = false;
			for (int i = 0; i < nScanLines; i++)
			{
				_CArray<int,1000,100> arrSLObjectsNums1;
				FindObjectsForScanLine(ScanLines.GetAt(i), arrSLObjectsNums1, arrObjects, sz, true);
				FindStringersForScanLine(ScanLines.GetAt(i), arrSLObjectsNums1, arrObjects, dAngle, sz);
			}
			if (!m_bAnyStringerAdded)
				break;
		}
	}
	// Object classification
/*	virtual int GetObjectType(int nStringerType, double dSulfide)
	{
		int nClass;
		if (nStringerType >= 0)
		{ // Stringer
			if (nStringerType == 1) // elongated
				nClass = dSulfide>0.5?0:2; // A or C
			else // non-elongated
				nClass = 1; // already B
		}
		else
			nClass = 3; // D
		return nClass;
	}*/
	virtual int GetObjectTypeForStringer(int nStringerType, double dSulfide, double dRealAvrWidth,
		_CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nClass;
		if (nStringerType == 1) // elongated
			nClass = dSulfide>0.5?0:2; // A or C
		else // non-elongated
			nClass = 1; // already B
		return nClass;
	}
	virtual int GetObjectTypeForSingleObject(int nStringerType, double dSulfide, CObjInfo &ObjInfo)
	{
		return 3; // D
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType = GetObjectTypeForStringer(nStringerType, PhasesCounts.Sulfide(), dRealAvrWidth,
			arrObj, nObjectsInStringer);
		int nWidthClass = CalculateWidthClass(dRealAvrWidth,arrObj,nObjectsInStringer,nType);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
//		int nClass = ObjectClass(nType, nWidthClass);
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType = GetObjectTypeForSingleObject(-1, PhasesCounts.Sulfide(), ObjInfo);
		int nWidthClass = s_ThreshClasses[nType].WidthClass(dRealAvrWidth);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
//		int nClass = ObjectClass(nType, nWidthClass);
//		return nClass;
	}
//	virtual void CalcWidthAndHeight()
//	{
//	}
	virtual bool MakeNewObject(_CArray<CObjInfo*,10,10> &arrObj, INamedDataObject2 *pObjList, POINT ptOrg,
		ICompManager *pGroupManager, char *szClassName)
	{
		int nObjectsInStringer = arrObj.GetSize();
		CObjInfo *p = arrObj.GetAt(0);
		CPhasesCount PhasesCounts;
		// Calculate axis
		POINT pt1 = rotatepoint(ptOrg, _point(p->m_x1,p->m_nScanLine), -p->m_dAngle);
		POINT pt2 = rotatepoint(ptOrg, _point(p->m_x2,p->m_nScanLine), -p->m_dAngle);
		int nStringerType = p->m_nStringerType;
		double dStringerLenght = double(p->m_x2-p->m_x1)*dCalibr;
		double dRealLen = 0.,dRealAvrWidth = 0.;
		// Calculate rectangle
		POINT ptOffset;
		int cx,cy;
		p->m_pimg->GetOffset(&ptOffset);
		p->m_pimg->GetSize(&cx, &cy);
		_rect rc(ptOffset.x, ptOffset.y, ptOffset.x+cx, ptOffset.y+cy);
		IUnknownPtr punkCC;
		p->m_pimg->GetColorConvertor(&punkCC);
		IColorConvertor2Ptr sptrCC2(punkCC);
		BSTR bstrName;
		sptrCC2->GetCnvName(&bstrName);
		int nPanes;
		p->m_pimg->GetPanesCount(&nPanes);
//		CalcRealWidthAndHeight(*p, ptOrg, double(nMinObjectSize),
//			p->m_dPartLength, p->m_dPartWidth, p->m_dPartArea);
		dRealLen += p->m_dPartLength;
		dRealAvrWidth += p->m_dPartWidth;
		int nNotSmall = p->m_dPartWidth>0.?1:0;
		for (int i = 1; i < nObjectsInStringer; i++)
		{
			p = arrObj.GetAt(i);
			p->m_pimg->GetOffset(&ptOffset);
			p->m_pimg->GetSize(&cx, &cy);
			if (ptOffset.x < rc.left) rc.left = ptOffset.x;
			if (ptOffset.y < rc.top) rc.top = ptOffset.y;
			if (ptOffset.x+cx > rc.right) rc.right = ptOffset.x+cx;
			if (ptOffset.y+cy > rc.bottom) rc.bottom = ptOffset.y+cy;
//			CalcRealWidthAndHeight(*p, ptOrg, double(nMinObjectSize),
//				p->m_dPartLength, p->m_dPartWidth, p->m_dPartArea);
			dRealLen += p->m_dPartLength;
			dRealAvrWidth += p->m_dPartWidth;
			if (p->m_dPartWidth > 0.) nNotSmall++;
		}
		if (nNotSmall) dRealAvrWidth /= nNotSmall;
		ptOffset.x = rc.left;
		ptOffset.y = rc.top;
		cx = rc.right-rc.left+1;
		cy = rc.bottom-rc.top+1;
		// Make new image and map data to it
		IUnknownPtr punkImage(CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
		IImage4Ptr sptrImage(punkImage);
		sptrImage->CreateImage(cx, cy, bstrName, nPanes);
		sptrImage->InitRowMasks();
		sptrImage->SetOffset(ptOffset, FALSE);
		::SysFreeString(bstrName);
		for (int y = 0; y < cy; y++)
		{
			byte *pRowMask;
			sptrImage->GetRowMask(y, &pRowMask);
			memset(pRowMask, 0, cx);
		}
		for (i = 0; i < nObjectsInStringer; i++)
		{
			p = arrObj.GetAt(i);
			MapImage(sptrImage, p->m_pimg, nPanes, (int*)PhasesCounts);
		}
		if (!CanBeStringer(nObjectsInStringer, nStringerType, PhasesCounts))
			return false;
		sptrImage->InitContours();
		IUnknownPtr punkObject(CreateTypedObject(_bstr_t(szArgumentTypeObject)), false);
		// Make new object and add it to object list
		IMeasureObjectPtr sptrMO(punkObject);
		sptrMO->SetImage(sptrImage);
		// Calculate non-stringer parameters (if need)
		if (pGroupManager) 
		{
			int	nCount;
			pGroupManager->GetCount(&nCount);
			for (int idx = 0; idx < nCount; idx++)
			{
				IUnknownPtr punk;
				pGroupManager->GetComponentUnknownByIdx(idx, &punk);
				IMeasParamGroupPtr	ptrG(punk);
				if (ptrG == 0) continue;
				ptrG->CalcValues(punkObject, punkImage);
			}
		}
		// Set parameters
		ICalcObjectPtr sptrCO(punkObject);
		sptrCO->SetValue(KEY_PARAM_STRINGER_LENGHT, dStringerLenght);
		sptrCO->SetValue(KEY_PARAM_STRINGER_SUM_LENGHT, dRealLen*dCalibr);
		sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH, dRealAvrWidth*dCalibr);
		// Classify it
		bool bClassified = ClassifyStringer(sptrCO, nStringerType, PhasesCounts, dRealAvrWidth, arrObj, nObjectsInStringer);
//		set_object_class(sptrCO, nClass, szClassName);
		// Add object to list
		if (bClassified/*nClass > -1*/ || nIgnoreSmall != 2)
		{
			// Make child : axis
			AddAxis(punkObject, pt1, pt2, dStringerLenght, dRealLen*dCalibr, dRealAvrWidth*dCalibr);
			INamedDataObject2Ptr sptrNDO2(punkObject);
			sptrNDO2->SetParent(pObjList, 0);
		}
		return true;
	}
	virtual void MakeNewObjectsList(_CObjArray &arrObjects, INamedDataObject2 *pObjList, POINT ptOrg,
		BOOL bCalc, char *szClassName)
	{
		// Prepare for parameter calculation (if need)
		ICompManagerPtr	sptrMeasMgr;
		if (bCalc)
		{
			sptrMeasMgr = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
			int	nCount;
			sptrMeasMgr->GetCount( &nCount );
			for( int idx = 0; idx < nCount; idx++ )
			{
				IUnknownPtr	punk;
				sptrMeasMgr->GetComponentUnknownByIdx(idx, &punk);
				IMeasParamGroupPtr	ptrG(punk);
				if( ptrG == 0 )continue;
				ptrG->InitializeCalculation(pObjList);
			}
		}
		// Make objects from stringers.
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			if (ObjInfo.m_nStringer == i) // New stringer
			{
				int iStringerObj = ObjInfo.m_nStringer;
				_CArray<CObjInfo*,10,10> arrObj;
				arrObj.Add(&ObjInfo);
				for (int j = i+1; j < nObjects; j++)
				{
					CObjInfo &ObjInfo1 = arrObjects.GetPtrAt(j);
					if (ObjInfo1.m_nStringer == iStringerObj)
						arrObj.Add(&ObjInfo1);
				}
				if (!MakeNewObject(arrObj,pObjList,ptOrg, bCalc?(ICompManager *)sptrMeasMgr:NULL,szClassName))
				{
					ObjInfo.m_nStringer = -1;
					for( int j = 0; j < arrObj.GetSize(); j++)
					{
						CObjInfo* p = arrObj.GetAt(j);
						p->m_nStringer = -1;
					}
				}
			}
		}
		// Copy unchanged objects
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			if (ObjInfo.m_nStringer < 0)
			{
				// Clone object
				IClonableObjectPtr sptrClone(ObjInfo.m_punkObj);
				IUnknownPtr punkCloned;
				if (sptrClone == 0) continue;
				sptrClone->Clone(&punkCloned);
				INamedDataObject2Ptr sptrObjectNDO2(punkCloned);
				if (sptrObjectNDO2 == 0) continue;
				bool bClassified = false;
				if (!ObjInfo.m_bDisabled)
				{
					// calculate oxide and sulfide parts
					CPhasesCount PhasesCounts;
					int cxObj,cyObj;
					ObjInfo.m_pimg->GetSize(&cxObj,&cyObj);
					for (int y = 0; y < cyObj; y++)
					{
						byte *pRowMask;
						ObjInfo.m_pimg->GetRowMask(y, &pRowMask);
						PhasesCounts.Row(pRowMask, cxObj);
					}
					// Set values and classify object
					ICalcObjectPtr sptrCO(punkCloned);
					sptrCO->SetValue(KEY_PARAM_STRINGER_LENGHT, ObjInfo.m_dPartLength*dCalibr);
					sptrCO->SetValue(KEY_PARAM_STRINGER_SUM_LENGHT, ObjInfo.m_dPartLength*dCalibr);
					sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH, ObjInfo.m_dPartWidth*dCalibr);
					bool bNeedAxis = false;
					bClassified = ClassifySingleObject(sptrCO, PhasesCounts, ObjInfo.m_dPartWidth,ObjInfo, bNeedAxis);
					// Make dummy axis to store parameters
					RECT rc = ObjInfo.m_rectRotated;
					int yCen = (rc.top+rc.bottom)/2;
					_point pt1 = bNeedAxis?_point(rc.left,yCen):_point(-1,-1);
					_point pt2 = bNeedAxis?_point(rc.right,yCen):_point(-1,-1);
					AddAxis(punkCloned, pt1, pt2, ObjInfo.m_dPartLength*dCalibr, ObjInfo.m_dPartLength*dCalibr, ObjInfo.m_dPartWidth*dCalibr);
				}
				if (bClassified || nIgnoreSmall != 2)
					sptrObjectNDO2->SetParent(pObjList, 0);
			}
		}
		// Finalize calculation (if need)
		if (bCalc)
		{
			sptrMeasMgr = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
			int	nCount;
			sptrMeasMgr->GetCount( &nCount );
			for( int idx = 0; idx < nCount; idx++ )
			{
				IUnknownPtr	punk;
				sptrMeasMgr->GetComponentUnknownByIdx(idx, &punk);
				IMeasParamGroupPtr	ptrG(punk);
				if( ptrG == 0 )continue;
				ptrG->FinalizeCalculation();
			}
		}
	}

	virtual double CalcStringerWidthASTM45(int nObjectsInStringer, _CArray<CObjInfo*,10,10> &arrObj)
	{
		double dSumLen = 0., dSumArea = 0.;
		for (int i = 0; i < nObjectsInStringer; i++)
		{
			CObjInfo*p = arrObj.GetAt(i);
			dSumLen += p->m_dPartLength;
			dSumArea += p->m_dPartArea;
		}
		if (dSumLen <= 0.)
			return 0.;
		else
		{
			double dPixels = dSumArea/dSumLen;
			return dPixels*dCalibr;
		}
		// Old algorythm, abandoned?
		/*double dMaxWidth = 0.;
		for(int i = 0; i < nObjectsInStringer; i++)
		{
			double d = CalcObjectThickness(*arrObj.GetAt(i));
			dMaxWidth = max(d,dMaxWidth);
		}
		return dMaxWidth;*/
	}
	virtual int ClassifyStringerASTM45(int nObjectsInStringer, _CArray<CObjInfo*,10,10> &arrObj, int nType)
	{
		_ptr_t2<double> adAreas(m_WidthClasses.m_nWidthClasses);
		for (int i = 0; i < m_WidthClasses.m_nWidthClasses; i++)
			adAreas[i] = 0.;
		double dSumArea = 0.;
		for (i = 0; i < nObjectsInStringer; i++)
		{
			CObjInfo*p = arrObj.GetAt(i);
			int nWidthClass = m_WidthClasses.Classify(nType, p->m_dPartWidth*dCalibr*1000000.);
			if (nWidthClass >= 0)
				adAreas[nWidthClass] += p->m_dPartArea;
			dSumArea += p->m_dPartArea;
		}
		double dBiggestArea = 0.;
		for (i = m_WidthClasses.m_nWidthClasses-1; i >= 0; i--)
		{
			dBiggestArea += adAreas[i];
			if (dBiggestArea >= dSumArea*m_dWidthThreshold)
				return i;
		}
		return -1;
	};
};

class CGost : public CStandard
{
public:
	CGost()
	{
		strcpy(m_sName, "GOST");
		m_dElongatedRatio = 2.;
	};
/*	virtual bool IsElongated(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=2.;
	}*/
	virtual bool CanBeStringer(int nObjectsInStringer, int nStringerType, CPhasesCount &PhasesCounts)
	{
		return true;
	}
};

class CAstm1122 : public CStandard
{
public:
	CAstm1122()
	{
		strcpy(m_sName, "ASTM 1122");
	};
};

class CAstm45A : public CAstm1122
{
protected:
	double m_dRelativeDistance;
	double m_dB3Factor;
public:
	CAstm45A()
	{
		strcpy(m_sName, "ASTM 45 A");
		m_dMaxDistXMcmDef = 127.;
		dAngleThreshold = PI/9.;
		m_bExcludePerpendicular = FALSE;
		m_WidthClasses.Init(4, 3);
		m_WidthClasses.Init(0, 2., 6., 12.);
		m_WidthClasses.Init(1, 2., 9., 15.);
		m_WidthClasses.Init(2, 2., 5., 12.);
		m_WidthClasses.Init(3, 2., 10., 15.);
		m_dRelativeDistance = 0.1;
	};
	virtual void ReadSettings()
	{
		char szSection[256];
		GetSectionName(szSection);
		__super::ReadSettings();
		m_WidthClasses.ReadSettings(szSection);
		m_dRelativeDistance = GetValueDouble(GetAppUnknown(), szSection, "RelativeDistance", 0.1);
		m_dWidthThreshold = GetValueDouble(GetAppUnknown(), szSection, "ProcAreaThresholdForWidthClassification", m_dWidthThreshold);
	}
	virtual bool ObjectCanBeInStringer(CObjInfo &ObjInfo) {return !ObjInfo.m_bSulfideParticle;}
	virtual void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
	{
		m_dB3Factor = 0.;
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			m_dB3Factor += ObjInfo.m_dArea;
		}
		m_dB3Factor = sqrt(m_dB3Factor);
		m_dB3Factor += rc.right-rc.left;
		m_dB3Factor *= dAngleScope;
		__super::FindStringersForObjects(arrObjects, dAngle, ptOrg, rc);
	}
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		double d = __super::CalcCoef(dAngle,dLen,dArea,Params);
		if (Params.dAreaLong == 0. && Params.arrObjectsInStringer.GetSize() >= 3)
			d += m_dB3Factor;
		return d;
	}
	virtual void CalcStringerParamsForObj(CStringerParams &Params, int iObj)
	{
		__super::CalcStringerParamsForObj(Params,iObj);
		RECT rc = Params.arrObjects.GetPtrAt(iObj).m_rectRotated;
		int y = (rc.top+rc.bottom)/2;
		if (Params.yUpperObjectCenter == -1 && Params.yLowerObjectCenter == -1)
		{
			Params.yUpperObjectCenter = Params.yLowerObjectCenter = y;
			Params.rcBounds = rc;
		}
		else
		{
			Params.yUpperObjectCenter = min(Params.yUpperObjectCenter,y);
			Params.yLowerObjectCenter = max(Params.yLowerObjectCenter,y);
			Params.rcBounds.top = min(Params.rcBounds.top,rc.top);
			Params.rcBounds.left = min(Params.rcBounds.left,rc.left);
			Params.rcBounds.bottom = max(Params.rcBounds.bottom,rc.bottom);
			Params.rcBounds.right = max(Params.rcBounds.right,rc.right);
		}
	}
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		if (Params.nObjectsInStringer < 2)
			return false;
		int yCenterLine = (Params.rcBounds.top+Params.rcBounds.bottom)/2;
		int nMaxDist = max(abs(yCenterLine-Params.yUpperObjectCenter),
			abs(Params.yLowerObjectCenter-yCenterLine));
		if (nMaxDist > (int)((Params.rcBounds.right-Params.rcBounds.left)*m_dRelativeDistance))
			return false;
		return true;
	}
	virtual int GetStringerType(CStringerParams &StringerParameters)
	{
		return StringerParameters.dAreaLong>0.?1:0;
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType = nStringerType==1?2:1; // C or B
		double dMaxWidth = CalcStringerWidthASTM45(nObjectsInStringer, arrObj);
		int nWidthClass = ClassifyStringerASTM45(nObjectsInStringer, arrObj, nType);
		/*double dMaxWidth = 0.;
		for(int i = 0; i < nObjectsInStringer; i++)
		{
			double d = CalcObjectThickness(*arrObj.GetAt(i));
			dMaxWidth = max(d,dMaxWidth);
		}
		int nWidthClass = m_WidthClasses.Classify(nType, dMaxWidth*1000000.);*/
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dMaxWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, (double)nObjectsInStringer);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
//		int nClass = ObjectClass(nType, nWidthClass);
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType;
		if (PhasesCounts.Sulfide() > 0.5)
			nType = 0; //A
		else if (ObjInfo.m_dAspectRatio > 5.)
			nType = 2; //C
		else if (ObjInfo.m_dAspectRatio > 2.)
			nType = 1; //B
		else
			nType = 3; // D
		bNeedAxis = nType!=3;
		double dWidth = CalcObjectThickness(ObjInfo);
		int nWidthClass = m_WidthClasses.Classify(nType, dWidth*1000000.);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, 1.);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
	}
};

class CAstm45B : public CAstm45A
{
public:
	CAstm45B()
	{
		strcpy(m_sName, "ASTM 45 B");
	};
};

class CAstm45C : public CAstm45A
{
public:
	CAstm45C()
	{
		strcpy(m_sName, "ASTM 45 C");
		m_dMaxDistXMcmDef = 127.;
		m_dMaxDistXMcmDef = 15.;
		dAngleThreshold = PI/9.;
	};
	virtual void ReadSettings()
	{
		CStandard::ReadSettings();
	}
	virtual bool ObjectCanBeInStringer(CObjInfo &ObjInfo) {return CStandard::ObjectCanBeInStringer(ObjInfo);}
	virtual void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
	{
		CStandard::FindStringersForObjects(arrObjects, dAngle, ptOrg, rc);
	}
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		return CStandard::CalcCoef(dAngle,dLen,dArea,Params);
	}
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		int yCenterLine = (Params.rcBounds.top+Params.rcBounds.bottom)/2;
		int nMaxDist = max(abs(yCenterLine-Params.yUpperObjectCenter),
			abs(Params.yLowerObjectCenter-yCenterLine));
		if (nMaxDist > (Params.rcBounds.right-Params.rcBounds.left)/10)
			return false;
		return true;
	}
	virtual int GetStringerType(CStringerParams &StringerParameters)
	{
		return StringerParameters.dAreaLong>0.?1:0;
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType = nStringerType; 
		if (nType!= -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			return true;
		}
		else
			return false;
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType = ObjInfo.m_dAspectRatio > 5. ? 1 : 0;
		bNeedAxis = nType==1;
		if (nType!= -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			return true;
		}
		else
			return false;
	}
};

class CAstm45D : public CAstm45A
{
public:
	CAstm45D()
	{
		strcpy(m_sName, "ASTM 45 D");
		m_WidthClasses.Init(0, 2., 4., 6.);
		m_WidthClasses.Init(1, 2., 9., 15.);
		m_WidthClasses.Init(2, 2., 5., 9.);
		m_WidthClasses.Init(3, 2., 8., 12.);
	};
};

class CAstm45E : public CAstm45D
{
public:
	CAstm45E()
	{
		strcpy(m_sName, "ASTM 45 E");
		m_WidthClasses.Init(2, 2);
		m_WidthClasses.Init(0, 2., 13.);
		m_WidthClasses.Init(1, 2., 13.);
	};
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		if (nStringerType==1) return false;
		int nType = 0; // C or B
		double dMaxWidth = CalcStringerWidthASTM45(nObjectsInStringer, arrObj);
		int nWidthClass = ClassifyStringerASTM45(nObjectsInStringer, arrObj, nType);
		/*double dMaxWidth = 0.;
		for(int i = 0; i < nObjectsInStringer; i++)
		{
			double d = CalcObjectThickness(*arrObj.GetAt(i));
			dMaxWidth = max(d,dMaxWidth);
		}
		int nWidthClass = m_WidthClasses.Classify(nType, dMaxWidth*1000000.);*/
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dMaxWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, (double)nObjectsInStringer);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType;
		if (PhasesCounts.Sulfide() > 0.5)
			nType = -1; //A
		else if (ObjInfo.m_dAspectRatio > 5.)
			nType = -1; //C
		else if (ObjInfo.m_dAspectRatio > 2.)
			nType = 0; //B
		else
			nType = 1; // D
		if (nType == -1) return false;
		bNeedAxis = nType==0;
		double dWidth = CalcObjectThickness(ObjInfo);
		int nWidthClass = m_WidthClasses.Classify(nType, dWidth*1000000.);
		if (nWidthClass == 0) return false;
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, 1.);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
	}
};


class CIso4967 : public CStandard
{
//	CWidthClasses m_WidthClasses;
	double m_dB3Factor;
	/*double CalcObjectThickness(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dArea/(ObjInfo.m_dPartLength*dCalibr);
	}*/
public:
	CIso4967()
	{
		strcpy(m_sName, "ISO 4967");
		dAngleThreshold = PI/9.;
		m_dMaxDistYMcmDef = 5.;
		m_WidthClasses.Init(4, 3);
		m_WidthClasses.Init(0, 2., 4., 12.);
		m_WidthClasses.Init(1, 2., 9., 15.);
		m_WidthClasses.Init(2, 2., 5., 12.);
		m_WidthClasses.Init(3, 3., 8., 13.);
	};
	virtual void ReadSettings()
	{
		char szSection[256];
		GetSectionName(szSection);
		__super::ReadSettings();
		m_WidthClasses.ReadSettings(szSection);
		m_dWidthThreshold = GetValueDouble(GetAppUnknown(), szSection, "ProcAreaThresholdForWidthClassification", m_dWidthThreshold);
	}
	virtual void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
	{
		m_dB3Factor = 0.;
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			m_dB3Factor += ObjInfo.m_dArea;
		}
		m_dB3Factor = sqrt(m_dB3Factor);
		m_dB3Factor += rc.right-rc.left;
		m_dB3Factor *= dAngleScope;
		__super::FindStringersForObjects(arrObjects, dAngle, ptOrg, rc);
	}
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		double d = __super::CalcCoef(dAngle,dLen,dArea,Params);
		if (Params.dAreaLong == 0. && Params.arrObjectsInStringer.GetSize() >= 3)
			d += m_dB3Factor;
		return d;
	}
/*	virtual bool IsElongated(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=3.;
	}*/
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		if (Params.bSulphide)
		{
			if (Params.bOxide) return false;
		}
		else if (Params.bOxide)
		{
			if (Params.dAreaLong > 0.)
				return true;
			else
				return Params.nObjectsInStringer >= 3;
		}
		return false;
	}
	virtual int GetStringerType(CStringerParams &StringerParameters)
	{
		return StringerParameters.dAreaLong>0.?1:0;
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType;
		CObjInfo*p = arrObj.GetAt(0);
		if (p->m_bSulfideParticle)
			nType = 0; //A
		else if (p->m_nStringerType==1)
			nType = 2; //C
		else 
			nType = 1; //B
		double dMaxWidth = CalcStringerWidthASTM45(nObjectsInStringer, arrObj);
		int nWidthClass = ClassifyStringerASTM45(nObjectsInStringer, arrObj, nType);
		/*double dMaxWidth = 0.;
		for(int i = 0; i < nObjectsInStringer; i++)
		{
			double d = CalcObjectThickness(*arrObj.GetAt(i));
			dMaxWidth = max(d,dMaxWidth);
		}
		int nWidthClass = m_WidthClasses.Classify(nType, dMaxWidth*1000000.);*/
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dMaxWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, (double)nObjectsInStringer);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType;
		if (PhasesCounts.Sulfide() > 0.5)
			nType = 0; //A
		else if (IsElongated(ObjInfo) && IsValidAngle(ObjInfo))
			nType = 2; //C
		else
			nType = 3; // D
		bNeedAxis = nType!=3;
		double dWidth = CalcObjectThickness(ObjInfo);
		int nWidthClass = m_WidthClasses.Classify(nType, dWidth*1000000.);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dWidth);
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, 1.);
		if (nType!= -1 && nWidthClass != -1)
		{
			set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
			set_object_class(pCalcObject, nWidthClass, m_bstrWidthClassifier);
			return true;
		}
		else
			return false;
	}
};

class CDin50602 : public CAstm45A
{
	double m_dRatioSuperLong,m_dRatioSubLong;
	bool CanFormOAStringer(CStringerParams &Params)
	{
		if (Params.nObjectsInStringer >= 3 && Params.dAreaSuperLong == 0. &&
			Params.dAreaLong <= (Params.dAreaLong+Params.dAreaShort)/3.)
			return true;
		else
			return false;
	}
public:
	CDin50602()
	{
		strcpy(m_sName, "DIN 50602");
		m_dMaxDistXMcmDef = 40.;
		dAngleThreshold = PI/9.;
		m_dRatioSuperLong = 10.;
		m_dRatioSubLong = 3.;
	};
	virtual void ReadSettings()
	{
		CStandard::ReadSettings();
		char szSection[256];
		GetSectionName(szSection);
		m_dRatioSuperLong = GetValueDouble(GetAppUnknown(),szSection,"ElongatedParticles10AspectRatio",m_dRatioSuperLong);
		m_dRatioSubLong = GetValueDouble(GetAppUnknown(),szSection,"ElongatedParticles3AspectRatio",m_dRatioSubLong);
		m_dRelativeDistance = GetValueDouble(GetAppUnknown(), szSection, "RelativeDistance", 0.1);
	}
/*	virtual void FindStringersForObjects(_CObjArray &arrObjects, double dAngle, POINT ptOrg, RECT rc)
	{
		m_dB3Factor = 0.;
		int nObjects = arrObjects.GetSize();
		for (int i = 0; i < nObjects; i++)
		{
			CObjInfo &ObjInfo = arrObjects.GetPtrAt(i);
			m_dB3Factor += ObjInfo.m_dArea;
		}
		m_dB3Factor = sqrt(m_dB3Factor);
		m_dB3Factor += rc.right-rc.left;
		m_dB3Factor *= dAngleScope;
		__super::FindStringersForObjects(arrObjects, dAngle, ptOrg, rc);
	}*/
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		double d = CStandard::CalcCoef(dAngle,dLen,dArea,Params);
		if (CanFormOAStringer(Params))
//		if (Params.dAreaSubLong == 0. && Params.arrObjectsInStringer.GetSize() >= 3)
			d += m_dB3Factor;
		return d;
	}
	virtual bool IsSuperLong(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=m_dRatioSuperLong&&IsValidAngle(ObjInfo);
	}
	virtual bool IsSubLong(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=m_dRatioSubLong&&IsValidAngle(ObjInfo);
	}
	virtual void CalcStringerParamsForObj(CStringerParams &Params, int iObj)
	{
		__super::CalcStringerParamsForObj(Params,iObj);
		if (IsSuperLong(Params.arrObjects.GetPtrAt(iObj)))
			Params.dAreaSuperLong += Params.arrObjects.GetPtrAt(iObj).m_dArea;
		if (IsSubLong(Params.arrObjects.GetPtrAt(iObj)))
			Params.dAreaSubLong += Params.arrObjects.GetPtrAt(iObj).m_dArea;
	}
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		bool bValid = false;
		dprintf("spl=%g,sbl=%g,", Params.dAreaSuperLong, Params.dAreaSubLong);
		if (CanFormOAStringer(Params))
			bValid = true; // OA
		else if (Params.dAreaSubLong > 0.)
			bValid = true; // OS
		if (!bValid) return false;
		int yCenterLine = (Params.rcBounds.top+Params.rcBounds.bottom)/2;
		int nMaxDist = max(abs(yCenterLine-Params.yUpperObjectCenter),
			abs(Params.yLowerObjectCenter-yCenterLine));
		dprintf("mxdst=%d,rd=%d,", nMaxDist, (int)((Params.rcBounds.right-Params.rcBounds.left)*m_dRelativeDistance));
		if (nMaxDist > (int)((Params.rcBounds.right-Params.rcBounds.left)*m_dRelativeDistance))
			return false;
		return true;
	}
	virtual int GetStringerType(CStringerParams &Params)
	{
		if (CanFormOAStringer(Params))
//		if (Params.nObjectsInStringer >= 3 && Params.dAreaSuperLong == 0. &&
//			Params.dAreaLong <= (Params.dAreaLong+Params.dAreaShort)/3.)
			return 0;
		else
			return 1;
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType = nStringerType==1?2:1; // OS or OA
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, (double)nObjectsInStringer);
		set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
		return true;
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nType;
		if (PhasesCounts.Sulfide() > 0.5)
			nType = 0; //SS
		else if (IsSubLong(ObjInfo))
			nType = 2; // OS
		else
			nType = 3; // OG
		bNeedAxis = nType!=3;
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, 1.);
		set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
		return true;
	}
};

class CNF_A04_106 : public CStandard
{
public:
	CNF_A04_106()
	{
		strcpy(m_sName, "NF A04-106");
	};
};

class CGost177870 : public CAstm45A // We will use specific criterion - 1/10 of the stringer length
{
	double m_dRatioSuperLong,m_dOSMaxSizeMcm;
	double m_dThreshRoughness;
	double m_dLongShareForOxid;
	double m_dNotSuperLongShareForSP;
	int    m_nRoughnessKey;
	double m_dBiggestObjArea,m_dBiggestGlobArea;
	int m_nMinObjectsForOS;
	int m_nSLongThresh;
	double m_dMinGlobForSMcm;
	int m_nMinNotSuperLong;
	// 0 - 128, 1 - 129, 2 - 130, 3 - 131
	int GetPhaseType(double dPhase128, double dPhase129, double dPhase130, double dPhase131)
	{
		if (dPhase128 >= dPhase129)
		{
			if (dPhase130 >= dPhase131)
				return dPhase128>=dPhase130?0:2;
			else
				return dPhase128>=dPhase131?0:3;
		}
		else
		{
			if (dPhase130 >= dPhase131)
				return dPhase129>=dPhase130?1:2;
			else
				return dPhase129>=dPhase131?1:3;
		}
	}
	bool CanFormS(CStringerParams &Params, int nPhaseType, bool *pbMarkAsUnknown)
	{
		if (Params.dAreaOfSmallGlob >= (Params.dAreaLong+Params.dAreaShort)*0.5 &&
			pbMarkAsUnknown != NULL)
			*pbMarkAsUnknown = true;
		return nPhaseType==2;
	}
	bool CanFormOS(CStringerParams &Params, int nPhaseType)
	{
		if (nPhaseType == 0 && Params.nObjectsInStringer >= m_nMinObjectsForOS &&
			Params.dAreaSubLong == 0. && Params.dAreaSuperLong == 0. &&
			Params.dAreaLong <= (Params.dAreaLong+Params.dAreaShort)*m_dLongShareForOxid &&
			Params.dBiggestObjArea < (Params.dAreaLong+Params.dAreaShort)*m_dBiggestObjArea)
			return true;
		else
			return false;
	}
	bool CanFormSH(CStringerParams &Params, int nPhaseType)
	{
		if ((nPhaseType == 1 || nPhaseType == 0) && Params.dAreaSuperLong > 0. &&
			((Params.dAreaShort+Params.dAreaLong-Params.dAreaSuperLong) >
			(Params.dAreaShort+Params.dAreaLong) * m_dNotSuperLongShareForSP ||
			Params.dRough >= Params.dAreaShort+Params.dAreaLong-Params.dRough) ||
			Params.nObjectsInStringer >= 3 && Params.dAreaLong > 0.)
			return true;
		else
			return false;
	}
	bool CanFormSP(CStringerParams &Params, int nPhaseType)
	{
		if ((nPhaseType == 1 || nPhaseType == 0) && Params.dAreaSuperLong > 0. &&
			(Params.dAreaShort+Params.dAreaLong-Params.dAreaSuperLong) <=
			Params.dAreaSuperLong * m_dNotSuperLongShareForSP &&
			Params.dRough < Params.dAreaShort+Params.dAreaLong-Params.dRough)
			return true;
		else
			return false;
	}
public:
	CGost177870()
	{
		strcpy(m_sName, "GOST 1778-70");
		m_dMaxDistXMcmDef = 40.;
		m_dMaxDistYMcmDef = 25.;
		m_dElongatedRatio = 2.5;
		m_dRatioSuperLong = 3.5;
		m_dOSMaxSizeMcm = 30.;
		m_dThreshRoughness = 3.5;
		m_dLongShareForOxid = 0.33;
		m_dNotSuperLongShareForSP = 0.1;
		m_nRoughnessKey = 150026;
		m_dBiggestObjArea = 0.5;
		m_nMinObjectsForOS = 4;
		m_dBiggestGlobArea = 0.7;
		m_bAxisMode = true;
		m_bCorrectAxis = true;
		m_nSLongThresh = 1;
		m_dMinGlobForSMcm = 1.;
		m_nMinNotSuperLong = 4;
	};
	virtual void ReadSettings()
	{
		CStandard::ReadSettings();
		char szSection[256];
		GetSectionName(szSection);
		m_dRelativeDistance = GetValueDouble(GetAppUnknown(), szSection, "RelativeDistance", 0.1);
		m_dRatioSuperLong = GetValueDouble(GetAppUnknown(),szSection,"ElongatedParticles35AspectRatio",m_dRatioSuperLong);
		m_dOSMaxSizeMcm = GetValueDouble(GetAppUnknown(),szSection,"OSMaxSizeMcm",m_dOSMaxSizeMcm);
		m_dThreshRoughness = GetValueDouble(GetAppUnknown(),szSection,"RoughnessThreshold",m_dThreshRoughness);
		m_dLongShareForOxid = GetValueDouble(GetAppUnknown(),szSection,"LongShareForOxide",m_dLongShareForOxid);
		m_dNotSuperLongShareForSP = GetValueDouble(GetAppUnknown(),szSection,"NotSuperLongShareForSP",m_dNotSuperLongShareForSP);
		m_nRoughnessKey = GetValueInt(GetAppUnknown(),szSection,"RoughnessKey",m_nRoughnessKey);
		m_dBiggestObjArea = GetValueDouble(GetAppUnknown(),szSection,"BiggestObjArea",m_dBiggestObjArea);
		m_nMinObjectsForOS = GetValueInt(GetAppUnknown(),szSection,"MinObjectsForOS",m_nMinObjectsForOS);
		m_dBiggestGlobArea = GetValueDouble(GetAppUnknown(),szSection,"BiggestGlobularObjArea",m_dBiggestGlobArea);
		m_nSLongThresh = GetValueInt(GetAppUnknown(),szSection,"SuperLongCountThreshold",m_nSLongThresh);
		m_dMinGlobForSMcm = GetValueDouble(GetAppUnknown(),szSection,"MinGlobForSMcm",m_dMinGlobForSMcm);
		m_nMinNotSuperLong = GetValueInt(GetAppUnknown(),szSection,"MinNotSuperLongParticles",m_nMinNotSuperLong);
	}
	virtual bool CanBeStringer(int nObjectsInStringer, int nStringerType, CPhasesCount &PhasesCounts)
	{
		return true;
	}
	virtual bool ObjectCanBeInStringer(CObjInfo &ObjInfo) {return CStandard::ObjectCanBeInStringer(ObjInfo);}
	virtual void PreprocessObject(CObjInfo &ObjCur, double dDeltaAngle, FPOINT ptCenter)
	{
		__super::PreprocessObject(ObjCur, dDeltaAngle, ptCenter);
		if (!ObjCur.m_bDisabled)
		{
			ICalcObjectPtr sptrCO(ObjCur.m_punkObj);
			if (COGetValue(sptrCO, KEY_PHASE128AREA, &ObjCur.m_dPhase128) != S_OK ||
				COGetValue(sptrCO, KEY_PHASE129AREA, &ObjCur.m_dPhase129) != S_OK ||
				COGetValue(sptrCO, KEY_PHASE130AREA, &ObjCur.m_dPhase130) != S_OK ||
				COGetValue(sptrCO, m_nRoughnessKey, &ObjCur.m_dRoughness) != S_OK)
				ObjCur.m_bDisabled = true;
		}
	}
	virtual bool IsSuperLong(CObjInfo &ObjInfo)
	{
		return ObjInfo.m_dAspectRatio>=m_dRatioSuperLong&&IsValidAngle(ObjInfo);
	}
	virtual void CalcStringerParamsForObj(CStringerParams &Params, int iObj)
	{
		__super::CalcStringerParamsForObj(Params,iObj);
		CObjInfo &ObjCur = Params.arrObjects.GetPtrAt(iObj);
		if (IsSuperLong(ObjCur))
		{
			Params.dAreaSuperLong += ObjCur.m_dArea;
			Params.nSuperLongCount++;
			if (ObjCur.m_dRoughness > m_dThreshRoughness)
				Params.dRough += ObjCur.m_dArea;
		}
		else
			Params.dRough += ObjCur.m_dArea;
		Params.dPhase128 += ObjCur.m_dPhase128;
		Params.dPhase129 += ObjCur.m_dPhase129;
		Params.dPhase130 += ObjCur.m_dPhase130;
		Params.dPhase131 += ObjCur.m_dArea-ObjCur.m_dPhase128-ObjCur.m_dPhase129-ObjCur.m_dPhase130;
		if (ObjCur.m_dPartLength > m_dOSMaxSizeMcm/1000000./dCalibr)
			Params.dAreaSubLong += ObjCur.m_dArea;
		if (ObjCur.m_dArea > Params.dBiggestObjArea)
			Params.dBiggestObjArea = ObjCur.m_dArea;
		if (!IsElongated(ObjCur) || !IsValidAngle(ObjCur))
		{
			if (ObjCur.m_dArea > Params.dBiggestGlobular)
				Params.dBiggestGlobular = ObjCur.m_dArea;
		}
		if (ObjCur.m_dPartWidth > Params.dBiggestWidth)
			Params.dBiggestWidth = ObjCur.m_dPartWidth;
		if ((ObjCur.m_dPartWidth+ObjCur.m_dPartLength)/2. < m_dMinGlobForSMcm/1000000./dCalibr)
			Params.dAreaOfSmallGlob += ObjCur.m_dArea;
	}
	virtual bool IsValidStringer(CStringerParams &Params)
	{
		if (Params.nObjectsInStringer < 2) return false;
		int yCenterLine = (Params.rcBounds.top+Params.rcBounds.bottom)/2;
		int nMaxDist = max(abs(yCenterLine-Params.yUpperObjectCenter),
			abs(Params.yLowerObjectCenter-yCenterLine));
//		if (nMaxDist > (Params.rcBounds.right-Params.rcBounds.left)/10)
//			return false;
		if (nMaxDist > (int)((Params.rcBounds.right-Params.rcBounds.left)*m_dRelativeDistance))
			return false;
		if (Params.dBiggestGlobular >= (Params.dAreaLong+Params.dAreaShort)*m_dBiggestGlobArea)
			return false;
		if (Params.dAreaSuperLong == 0. && Params.nObjectsInStringer < m_nMinNotSuperLong)
			return false;
		if (Params.nSuperLongCount > 0 &&
			Params.nSuperLongCount <= m_nSLongThresh &&
			(Params.nObjectsInStringer - Params.nSuperLongCount < 2 ||
			Params.dAreaSuperLong < (Params.dAreaLong+Params.dAreaShort)*0.5))
			return false;
		if (Params.nObjectsInStringer - Params.nSuperLongCount <= 2 &&
			Params.dAreaSuperLong < (Params.dAreaLong+Params.dAreaShort)*0.5)
		{
			for (int j = 0; j < Params.nObjectsInStringer; j++)
			{
				int iObj = Params.arrObjectsInStringer.GetAt(j);
				CObjInfo &ObjInfo(Params.arrObjects.GetPtrAt(iObj));
				if (ObjInfo.m_dPartWidth < 0.3*Params.dBiggestWidth)
					return false;
			}
		}

		Params.nStringerType = -1;
		int nPhaseType = GetPhaseType(Params.dPhase128, Params.dPhase129, Params.dPhase130,
			Params.dPhase131);
		bool bMarkAsUnknown = false;
		if (CanFormS(Params, nPhaseType, &bMarkAsUnknown))
			Params.nStringerType = bMarkAsUnknown?-1:5;
		else if (CanFormOS(Params, nPhaseType))
			Params.nStringerType = 0;
		else if (CanFormSH(Params, nPhaseType))
			Params.nStringerType = 2;
		else if (CanFormSP(Params, nPhaseType))
			Params.nStringerType = 3;
		return Params.nStringerType >= 0 || bMarkAsUnknown;
	}
	virtual int GetStringerType(CStringerParams &Params)
	{
		return Params.nStringerType;
	}
	virtual bool ClassifyStringer(ICalcObject *pCalcObject, int nStringerType, CPhasesCount &PhasesCounts,
		double dRealAvrWidth, _CArray<CObjInfo*,10,10> &arrObj, int nObjectsInStringer)
	{
		int nType = nStringerType;
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, (double)nObjectsInStringer);
		set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
		return true;
	}
	virtual bool ClassifySingleObject(ICalcObject *pCalcObject, CPhasesCount &PhasesCounts, double dRealAvrWidth, CObjInfo &ObjInfo, bool &bNeedAxis)
	{
		int nPhase = GetPhaseType(ObjInfo.m_dPhase128, ObjInfo.m_dPhase129, ObjInfo.m_dPhase130,
			ObjInfo.m_dArea-ObjInfo.m_dPhase128-ObjInfo.m_dPhase129-ObjInfo.m_dPhase130);
		int nType;
		if (nPhase == 2) // sulphides
		{
			if ((ObjInfo.m_dPartWidth+ObjInfo.m_dPartLength)/2 < m_dMinGlobForSMcm/1000000./dCalibr)
				nType = -1;
			else
				nType = 5; // S
		}
		else if (nPhase == 0 && ObjInfo.m_dPartLength <= m_dOSMaxSizeMcm*1000000./dCalibr) // oxide
			nType = 1; //OT
		else if ((nPhase == 0 || nPhase == 1) && IsSuperLong(ObjInfo))
			nType = ObjInfo.m_dRoughness > m_dThreshRoughness ? 2 : 3; // SH or ST
		else
			nType = 4; // SN
		bNeedAxis = false;//nType==2||nType==3;
		pCalcObject->SetValue(KEY_PARAM_STRINGER_OBJECTS, 1.);
		set_object_class(pCalcObject, nType, m_bstrTypeClassifier);
		return true;
	}
	virtual double CalcCoef(double dAngle, double dLen, double dArea, CStringerParams &Params)
	{
		return CStandard::CalcCoef(dAngle,dLen,dArea,Params);
	}
};


CStandard *MakeStandard(int nStandard)
{
	if (nStandard == 1)
		return new CGost;
	else if (nStandard == 2)
		return new CAstm1122;
	else if (nStandard == 3)
		return new CAstm45A;
	else if (nStandard == 4)
		return new CAstm45B;
	else if (nStandard == 5)
		return new CAstm45C;
	else if (nStandard == 6)
		return new CAstm45D;
	else if (nStandard == 7)
		return new CAstm45E;
	else if (nStandard == 8)
		return new CIso4967;
	else if (nStandard == 9)
		return new CDin50602;
	else if (nStandard == 10)
		return new CNF_A04_106;
	else if (nStandard == 11)
		return new CGost177870;
	else
		return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class CStringer
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CStringerInfo::s_pargs[] = 
{
	{"Calc", szArgumentTypeInt, "1", true, false },
	{"Source", szArgumentTypeObjectList, 0, true, true},
	{"Result", szArgumentTypeObjectList, 0, false, true},
	{0, 0, 0, false, false },
};

CStringer::CStringer()
{
}

static void CopyParams(ICalcObjectContainerPtr in, ICalcObjectContainerPtr out)
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

bool CStringer::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());
	if (objectsOut == 0 || objectsIn == 0) return false;
	long nCountIn; objectsIn->GetChildsCount(&nCountIn);
	if (!nCountIn) return false;
	BOOL bCalc = GetArgLong("Calc");
	GUID guid;
	objectsIn->GetBaseKey(&guid);
	objectsOut->SetBaseKey(&guid);
	CopyParams(objectsIn, objectsOut);
	int nStandard = GetValueInt(GetAppUnknown(),"Stringers","Standard",0);
	if (nStandard == 0) // old algorythm
	{
		// Read settings from shell.data
		dCostCoef = GetValueDouble(GetAppUnknown(),"Stringers","CostCoefficient",0.01);
		dAngleThreshold = GetValueDouble(GetAppUnknown(),"Stringers","IgnoredPariclesAngle",dAngleThreshold);
		dRatioThresh = GetValueDouble(GetAppUnknown(),"Stringers","IgnoredPariclesAspectRatio",2.);
		bASTM = GetValueInt(GetAppUnknown(),"Stringers","ASTM",TRUE);
		nIgnoreSmall = GetValueInt(GetAppUnknown(),"Stringers","IgnoreSmall",nIgnoreSmall);
		// Init angles and distances (some hardcoded)
		int nAngles = int(ceil(dAngleScope/dDeltaAngle)+1.);
		GetDefaultCalibration(&dCalibr, &guid);
		s_ThreshClasses.Init(dCalibr);
		double dMaxDistXMcm = GetValueDouble(GetAppUnknown(),"Stringers","MaxDistXMcm",40.);
		double dMaxDistYMcm = GetValueDouble(GetAppUnknown(),"Stringers","MaxDistYMcm",15.);
		nMaxDistX = (int)(McmToPoints(dCalibr,dMaxDistXMcm));
		nMaxDistY = (int)(McmToPoints(dCalibr,dMaxDistYMcm));
		double dMinObjectSizeMcm = GetValueDouble(GetAppUnknown(),"Stringers","MinObjectSizeMcm",2.);
		nMinObjectSize = (int)(McmToPoints(dCalibr,dMinObjectSizeMcm));
		_CObjArray arrObjects;
		StartNotification(nAngles*10);
		MakeObjectsArray(arrObjects,objectsIn,nCountIn); // Cache for object information
		RECT rc = FindObjectsRect(arrObjects);
		_point ptOrg((rc.left+rc.right)/2,(rc.top+rc.bottom)/2);
		CalcMaxAspectRatio(arrObjects,dDeltaAngle,ptOrg);
		FindStringersForObjects(arrObjects,0.,ptOrg,rc);
		// Main cycle
		int nNotify = 0;
		/*for (double dAngle = 0; dAngle <= dAngleScope; dAngle += dDeltaAngle, nNotify += 10)
		{
			FindStringersForObjects(arrObjects,dAngle,ptOrg,rc);
			FindStringersForObjects(arrObjects,-dAngle,ptOrg,rc);
			Notify(nNotify);
		}*/
		MakeNewObjectsList(arrObjects,objectsOut,ptOrg,bCalc,"stringers.class");
		FinishNotification();
	}
	else // New algorythm. 8 standards are supported.
	{
		CStandard *pStandard = MakeStandard(nStandard);
		if (!pStandard)
			return false;
		pStandard->ReadSettings();
		_CObjArray arrObjects;
		StartNotification(1);
		MakeObjectsArray(arrObjects,objectsIn,nCountIn); // Cache for object information
		RECT rc = FindObjectsRect(arrObjects);
		_point ptOrg((rc.left+rc.right)/2,(rc.top+rc.bottom)/2);
		pStandard->PreprocessObjects(arrObjects,0.,ptOrg);// Second parameter doesn't used now
		pStandard->FindStringersForObjects(arrObjects,0.,ptOrg,rc);
		// Some objects from stringer candidates rejected for better candidates can form stringer itself.
		// Make second iteration.
		pStandard->MakeNewObjectsList(arrObjects,objectsOut,ptOrg,bCalc,"stringers.class");
		FinishNotification();
		delete pStandard;
	}
	SetModified(true);
	return true;
}




