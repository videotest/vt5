#include "stdafx.h"
#include "Actions.h"
#include "MeasManual.h"
#include "ManualObj.h"
#include "Resource.h"

#include "Defs.h"
#include "NameConsts.h"
#include "ClassBase.h"
#include "\vt5\common2\class_utils.h"

#include <math.h>

class _RestrictDraw
{
	HRGN hRgn;
	HDC hDC;
public:
	_RestrictDraw(CDC &dc, IUnknown *punkTarget)
	{
		hRgn = ::CreateRectRgn(0,0,0xFFFF,0xFFFF);
		int r = ::GetClipRgn(dc.m_hDC, hRgn);
		if (r == -1)
		{
			::DeleteObject(hRgn);
			hRgn = 0;
		}
		hDC = dc.m_hDC;
		IScrollZoomSitePtr sptrSZS(punkTarget);
		SIZE sz;
		sptrSZS->GetClientSize(&sz);
		sz = ::ConvertToWindow(punkTarget, CSize(sz));
		dc.IntersectClipRect(0, 0, sz.cx, sz.cy);
	}
	~_RestrictDraw()
	{
		::SelectClipRgn(hDC, hRgn);
		::DeleteObject(hRgn);
	};
};


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionMeasParallelLines, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasAngle2Line,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasAngle3Point, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasCycle3,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasCycle2,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasBrokenLine,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasLine,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasCount,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasSpline,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasFreeLine,	CCmdTargetEx);

//olecreate

// {93B277EC-7E47-4a6d-BEF7-A616F9E74949}
GUARD_IMPLEMENT_OLECREATE(CActionMeasParallelLines, "MeasManual.MeasParallelLines",
0x93b277ec, 0x7e47, 0x4a6d, 0xbe, 0xf7, 0xa6, 0x16, 0xf9, 0xe7, 0x49, 0x49);
// {0ADFC80C-3753-4dfc-8242-A1F2B09E0D41}
GUARD_IMPLEMENT_OLECREATE(CActionMeasAngle2Line, "MeasManual.MeasAngle2Line",
0xadfc80c, 0x3753, 0x4dfc, 0x82, 0x42, 0xa1, 0xf2, 0xb0, 0x9e, 0xd, 0x41);
// {A01DDBED-1187-4ada-B02A-F2826E8AB8F5}
GUARD_IMPLEMENT_OLECREATE(CActionMeasAngle3Point, "MeasManual.MeasAngle3Point",
0xa01ddbed, 0x1187, 0x4ada, 0xb0, 0x2a, 0xf2, 0x82, 0x6e, 0x8a, 0xb8, 0xf5);
// {83BEE39E-3FBC-4929-BFF8-043C1261CC2C}
GUARD_IMPLEMENT_OLECREATE(CActionMeasCycle3, "MeasManual.MeasCycle3",
0x83bee39e, 0x3fbc, 0x4929, 0xbf, 0xf8, 0x4, 0x3c, 0x12, 0x61, 0xcc, 0x2c);
// {1E388635-9552-49a9-B1C5-C1657AE7953A}
GUARD_IMPLEMENT_OLECREATE(CActionMeasCycle2, "MeasManual.MeasCycle2",
0x1e388635, 0x9552, 0x49a9, 0xb1, 0xc5, 0xc1, 0x65, 0x7a, 0xe7, 0x95, 0x3a);
// {CE498DCD-099F-40bf-85F8-474D857CB4DD}
GUARD_IMPLEMENT_OLECREATE(CActionMeasBrokenLine, "MeasManual.MeasBrokenLine",
0xce498dcd, 0x99f, 0x40bf, 0x85, 0xf8, 0x47, 0x4d, 0x85, 0x7c, 0xb4, 0xdd);
// {E84C7CC9-EA17-4ba2-8574-E8D0119BFD81}
GUARD_IMPLEMENT_OLECREATE(CActionMeasLine, "MeasManual.MeasLine",
0xe84c7cc9, 0xea17, 0x4ba2, 0x85, 0x74, 0xe8, 0xd0, 0x11, 0x9b, 0xfd, 0x81);
// {45E65AE0-65BE-4125-9741-AC464EE83592}
GUARD_IMPLEMENT_OLECREATE(CActionMeasCount, "MeasManual.MeasCount",
0x45e65ae0, 0x65be, 0x4125, 0x97, 0x41, 0xac, 0x46, 0x4e, 0xe8, 0x35, 0x92);
// {D15D49E7-EE2F-4ea8-920E-A00112144767}
GUARD_IMPLEMENT_OLECREATE(CActionMeasSpline, "MeasManual.MeasSpline",
0xd15d49e7, 0xee2f, 0x4ea8, 0x92, 0xe, 0xa0, 0x1, 0x12, 0x14, 0x47, 0x67);
// {A6DD019D-B880-40b3-A38B-2563F44AA1E6}
GUARD_IMPLEMENT_OLECREATE(CActionMeasFreeLine, "MeasManual.MeasFreeLine",
0xa6dd019d, 0xb880, 0x40b3, 0xa3, 0x8b, 0x25, 0x63, 0xf4, 0x4a, 0xa1, 0xe6);

//guidinfo

// {326D0A10-826A-4a10-A27B-A33356BEED61}
static const GUID guidMeasParallelLines =
{ 0x326d0a10, 0x826a, 0x4a10, { 0xa2, 0x7b, 0xa3, 0x33, 0x56, 0xbe, 0xed, 0x61 } };
// {87F39186-D9AF-4709-98AF-97B9EF1090F4}
static const GUID guidMeasAngle2Line =
{ 0x87f39186, 0xd9af, 0x4709, { 0x98, 0xaf, 0x97, 0xb9, 0xef, 0x10, 0x90, 0xf4 } };
// {67BA05B7-B5A3-4c6e-A105-C9B7899C2A06}
static const GUID guidMeasAngle3Point =
{ 0x67ba05b7, 0xb5a3, 0x4c6e, { 0xa1, 0x5, 0xc9, 0xb7, 0x89, 0x9c, 0x2a, 0x6 } };
// {0A381C32-9FAC-4182-8B4B-7B4C14C43B58}
static const GUID guidMeasCycle3 =
{ 0xa381c32, 0x9fac, 0x4182, { 0x8b, 0x4b, 0x7b, 0x4c, 0x14, 0xc4, 0x3b, 0x58 } };
// {E39C3DA5-71CF-4e4b-AB83-5F0E2E05F103}
static const GUID guidMeasCycle2 =
{ 0xe39c3da5, 0x71cf, 0x4e4b, { 0xab, 0x83, 0x5f, 0xe, 0x2e, 0x5, 0xf1, 0x3 } };
// {9FE5FF44-79CC-4b35-ACB5-92CFB25A5957}
static const GUID guidMeasBrokenLine =
{ 0x9fe5ff44, 0x79cc, 0x4b35, { 0xac, 0xb5, 0x92, 0xcf, 0xb2, 0x5a, 0x59, 0x57 } };
// {4EDB13D8-85F1-4c35-9783-87F0DBAB8E7E}
static const GUID guidMeasLine =
{ 0x4edb13d8, 0x85f1, 0x4c35, { 0x97, 0x83, 0x87, 0xf0, 0xdb, 0xab, 0x8e, 0x7e } };
// {C24B388F-8FBC-47a0-B201-4C4577AFD3A1}
static const GUID guidMeasCount =
{ 0xc24b388f, 0x8fbc, 0x47a0, { 0xb2, 0x1, 0x4c, 0x45, 0x77, 0xaf, 0xd3, 0xa1 } };
// {3C8CE435-F450-4477-B087-5E31FE1FA896}
static const GUID guidMeasSpline =
{ 0x3c8ce435, 0xf450, 0x4477, { 0xb0, 0x87, 0x5e, 0x31, 0xfe, 0x1f, 0xa8, 0x96 } };
// {E9386E0D-3B6D-4bf1-815B-87BD088CDF4E}
static const GUID guidMeasFreeLine =
{ 0xe9386e0d, 0x3b6d, 0x4bf1, { 0x81, 0x5b, 0x87, 0xbd, 0x8, 0x8c, 0xdf, 0x4e } };


//[ag]6. action info

ACTION_INFO_FULL(CActionMeasParallelLines, IDS_ACTION_MMPARALLEL,	IDS_MENU_MEAS, IDS_TB_DATA, guidMeasParallelLines);
ACTION_INFO_FULL(CActionMeasAngle2Line, IDS_ACTION_MMANGLE2LINE,	IDS_MENU_MEAS, IDS_TB_DATA, guidMeasAngle2Line);
ACTION_INFO_FULL(CActionMeasAngle3Point,IDS_ACTION_MMANGLE3POINT,	IDS_MENU_MEAS, IDS_TB_DATA, guidMeasAngle3Point);
ACTION_INFO_FULL(CActionMeasCycle3,		IDS_ACTION_MMCYCLE3,		IDS_MENU_MEAS, IDS_TB_DATA, guidMeasCycle3);
ACTION_INFO_FULL(CActionMeasCycle2,		IDS_ACTION_MMCYCLE2,		IDS_MENU_MEAS, IDS_TB_DATA, guidMeasCycle2);
ACTION_INFO_FULL(CActionMeasBrokenLine, IDS_ACTION_MMBROKENLINE,	IDS_MENU_MEAS, IDS_TB_DATA, guidMeasBrokenLine);
ACTION_INFO_FULL(CActionMeasLine,		IDS_ACTION_MMLINE,			IDS_MENU_MEAS, IDS_TB_DATA, guidMeasLine);
ACTION_INFO_FULL(CActionMeasCount,		IDS_ACTION_MMCOUNT,			IDS_MENU_MEAS, IDS_TB_DATA, guidMeasCount);
ACTION_INFO_FULL(CActionMeasSpline,		IDS_ACTION_MMSPLINE,		IDS_MENU_MEAS, IDS_TB_DATA, guidMeasSpline);
ACTION_INFO_FULL(CActionMeasFreeLine,	IDS_ACTION_MMFREELINE,		IDS_MENU_MEAS, IDS_TB_DATA, guidMeasFreeLine);

//[ag]7. targets

ACTION_TARGET(CActionMeasParallelLines, szTargetViewSite);
ACTION_TARGET(CActionMeasAngle2Line,	szTargetViewSite);
ACTION_TARGET(CActionMeasAngle3Point,	szTargetViewSite);
ACTION_TARGET(CActionMeasCycle3,		szTargetViewSite);
ACTION_TARGET(CActionMeasCycle2,		szTargetViewSite);
ACTION_TARGET(CActionMeasBrokenLine,	szTargetViewSite);
ACTION_TARGET(CActionMeasLine,			szTargetViewSite);
ACTION_TARGET(CActionMeasCount,			szTargetViewSite);
ACTION_TARGET(CActionMeasSpline,		szTargetViewSite);
ACTION_TARGET(CActionMeasFreeLine,		szTargetViewSite);

//[ag]8. arguments


//[ag]9. implementation
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//CActionMeasParallelLines implementation

CActionMeasParallelLines::CActionMeasParallelLines()
{
	m_bFirstLineComplete = false;
	m_clDefColor = ::GetValueColor(::GetAppUnknown(false), szManualMeasure, szMeasureColor, RGB(64, 128, 255));
	m_nObjectCounter = 0;
}

CActionMeasParallelLines::~CActionMeasParallelLines()
{
}

bool CActionMeasParallelLines::Invoke()
{
	//re-assign the target key to the document
	sptrIView	sptrV(m_punkTarget);
	IUnknown* punkDocument = 0;
	sptrV->GetDocument(&punkDocument);
	if(punkDocument)
	{
		m_lTargetKey = ::GetObjectKey(punkDocument);
		punkDocument->Release();
	}


	return true;
}


bool CActionMeasParallelLines::IsAvaible()
{
	if (CheckInterface(m_punkTarget, IID_IImageView))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();
			return true;
		}
		return false;
	}
	else if (CheckInterface(m_punkTarget, IID_IGridView))
		return true;
	else
		return false;
}

bool CActionMeasParallelLines::IsCompatibleTarget()
{
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	return ((dwFlags & isfFilledImageExceptObjects) == isfFilledImageExceptObjects) ||
		   ((dwFlags & isfFilledOnlyObjects) == isfFilledOnlyObjects) ||
           ((dwFlags & isfContours) == isfContours) ||
           ((dwFlags & isfPseudoImage) == isfPseudoImage);
}

void CActionMeasParallelLines::DoActivateObjects( IUnknown *punkObject )
{
	if( !IsCompatibleTarget() || !IsAvaible() ||
		::GetObjectKey(m_ObjListWrp) != ::GetObjectKey(punkObject))
		CInteractiveActionBase::Terminate();
};

void CActionMeasParallelLines::Terminate()
{
	sptrIView	sptrV(m_punkTarget);
	IUnknown* punkDocument = 0;
	sptrV->GetDocument(&punkDocument);
	if(punkDocument)
	{
		m_changes.DoUndo(punkDocument);
		punkDocument->Release();
	}
	
	CInteractiveActionBase::Terminate();
}

void CActionMeasParallelLines::SetParamsKeys()
{
	int nLine = 0;
	POSITION pos = m_ObjListWrp.GetFirstObjectPosition();
	while(pos)
	{
		IUnknownPtr punkObj(m_ObjListWrp.GetNextObject(pos),false);
		INamedDataObject2Ptr sptrObj(punkObj);
		long lPosChild;
		sptrObj->GetFirstChildPosition(&lPosChild);
		while (lPosChild)
		{
			IUnknownPtr punkMM;
			sptrObj->GetNextChild(&lPosChild,&punkMM);
			IManualMeasureObject2Ptr sptrMMObj(punkMM);
			if (sptrMMObj != 0)
			{
				DWORD lType = 0;
				if (sptrMMObj->GetObjectType(&lType) == S_OK && lType == emotParallelLines)
				{
					sptrMMObj->SetParametersCount(m_arrParamKeys.GetSize()-1);
					for (long i = 0; i < m_arrParamKeys.GetSize()-1; i++)
						sptrMMObj->SetParamKey(i, m_arrParamKeys[i]);
					break;
				}
			}
		}
	}
}

void CActionMeasParallelLines::Finalize()
{
	SetParamsKeys();
	//_Draw();
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	if(pwnd)pwnd->Invalidate();

	CInteractiveActionBase::Finalize();
}


bool CActionMeasParallelLines::DoRedo()
{
	m_changes.DoRedo(m_punkTarget);

	long nObjectCount = m_ObjListWrp.GetCount();
	POSITION pos = m_ObjListWrp.GetFirstObjectPosition();
	long nCounter = -1;
	while(pos)
	{
		nCounter++;
		IUnknown* punkObject = m_ObjListWrp.GetNextObject(pos);
		if(!punkObject)
			continue;

		if(nCounter < nObjectCount)
		{
			ICalcObjectPtr sptrCalc = punkObject;
			punkObject->Release();
			sptrCalc->SetValue(m_arrParamKeys[nCounter], 0);
		}
	}

	return true;
}

bool CActionMeasParallelLines::DoUndo()
{
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionMeasParallelLines::Initialize()
{
	if( !__super::Initialize() )
		return false;
	CMouseImpl::LockMovement(false, m_rectLock);
/*	if( CheckInterface( m_punkTarget, IID_IImageView ) )
	{
		if( !IsCompatibleTarget())
			if(!SetAppropriateView(m_punkTarget, avtObject) || m_state == asTerminate )
				return false;
	}*/

	m_hcurActive = AfxGetApp()->LoadCursor(IDC_PARALLEL_MEAS);
	return true;

}

static int _DiapOverlapMode(int x11, int x12, int x21, int x22)
{
	int min1 = min(x11,x12);
	int max1 = max(x11,x12);
	int min2 = min(x21,x22);
	int max2 = max(x21,x22);
	if (max1 < min2 || min1 > max2) return 0; // No overlap
	else if (min1 >= min2 && max1 <= max2) return 3; // 1 inside 2
	else if (min1 < min2 && max1 >= min2) return 1; // Overlap low border
	else return 2;
}

bool CActionMeasParallelLines::_CheckCoords(CPoint pt, CPoint ptEnd)
{
	if (!m_bFirstLineComplete)
	{
		if (!m_rectLock.PtInRect(pt))
			return false;
	}
	else
	{
		if (ptEnd == CPoint(-1,-1))
			ptEnd = _GetParallelPoint(pt);
		if (m_rectLock.PtInRect(pt)||m_rectLock.PtInRect(ptEnd))
			return true;
		int nXMode = _DiapOverlapMode(pt.x, ptEnd.x, m_rectLock.left, m_rectLock.right);
		int nYMode = _DiapOverlapMode(pt.y, ptEnd.y, m_rectLock.top, m_rectLock.bottom);
		if (nXMode == 0 || nYMode == 0)
			return false;
		else if (nXMode == 3 || nYMode == 3) // MUST NOT to be!!!!
			return true; // 3 cases: nXMode==3&&nYMode==3, nXMode==3&&nYMode>0, nXMode>0&&nYMode==3
		// nXMode==1..2 && nYMode==1..2
		else if (m_rectLock.PtInRect(pt)||m_rectLock.PtInRect(ptEnd))
			return true;
		else if (nYMode == 1) // overlap left top or right top corner
		{
			int xLineOvr = pt.x+(ptEnd.x-pt.x)*(m_rectLock.top-pt.y)/(ptEnd.y-pt.y);
			return xLineOvr >= m_rectLock.left && xLineOvr <= m_rectLock.right;
		}
		else // nYMode == 2 - overlap left bottom or right bottom corner
		{
			int xLineOvr = pt.x+(ptEnd.x-pt.x)*(m_rectLock.bottom-pt.y)/(ptEnd.y-pt.y);
			return xLineOvr >= m_rectLock.left && xLineOvr <= m_rectLock.right;
		}
	}
	return true;
}


bool CActionMeasParallelLines::DoLButtonDown(CPoint pt)
{
	if (!_CheckCoords(pt))
		return false;
	if( IsAvaible() )
	{
		_Draw();

		long nSize = m_arrLinesCoords.GetSize();
		if(nSize == 1 && !m_bFirstLineComplete)
		{
			m_arrLinesCoords[nSize-1].BottomRight() = pt;

			if(!AddMMObject(nSize-1))
			{
				m_arrLinesCoords.RemoveAt(nSize-1);
				m_arrParamKeys.RemoveAt(nSize-1);
			}
		}

		CRect rc = NORECT;
		rc.TopLeft() = pt;
		rc.BottomRight() = pt;
		m_arrLinesCoords.Add(rc);

		if(m_bFirstLineComplete)
		{
			m_arrLinesCoords[nSize-1].TopLeft() = pt;
			m_arrLinesCoords[nSize-1].BottomRight() = _GetParallelPoint();

			if(!AddMMObject(nSize-1))
			{
				m_arrLinesCoords.RemoveAt(nSize-1);
				m_arrParamKeys.RemoveAt(nSize-1);
			}

			CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
			if(pwnd)
				pwnd->Invalidate();
		}


		if(nSize == 1 && !m_bFirstLineComplete)
			m_bFirstLineComplete = true;

		_Draw();
	}
	
	return true;
}

bool CActionMeasParallelLines::DoSetCursor(CPoint point)
{
	if (_CheckCoords(point))
		__super::DoSetCursor(point);
	else
		::SetCursor( m_hcurStop );
	return true;
}


CPoint CActionMeasParallelLines::_GetParallelPoint()
{
	CPoint pt(0, 0);
	long nSize = m_arrLinesCoords.GetSize();
	if(m_bFirstLineComplete)
	{
		CPoint ptBaseLineBegin = m_arrLinesCoords[0].TopLeft();
		CPoint ptBaseLineEnd = m_arrLinesCoords[0].BottomRight();
		
		CPoint ptNewLineBegin = m_arrLinesCoords[nSize-1].TopLeft();
		
		pt.x = ptNewLineBegin.x + (ptBaseLineEnd.x - ptBaseLineBegin.x);
		pt.y = ptNewLineBegin.y + (ptBaseLineEnd.y - ptBaseLineBegin.y);
	}

	return pt;
}

CPoint CActionMeasParallelLines::_GetParallelPoint(CPoint ptNewLineBegin)
{
	CPoint ptNewLineEnd(ptNewLineBegin);
	long nSize = m_arrLinesCoords.GetSize();
	if(m_bFirstLineComplete)
	{
		CPoint ptBaseLineBegin = m_arrLinesCoords[0].TopLeft();
		CPoint ptBaseLineEnd = m_arrLinesCoords[0].BottomRight();
		ptNewLineEnd.x = ptNewLineBegin.x + (ptBaseLineEnd.x - ptBaseLineBegin.x);
		ptNewLineEnd.y = ptNewLineBegin.y + (ptBaseLineEnd.y - ptBaseLineBegin.y);
	}
	return ptNewLineEnd;
}

bool CActionMeasParallelLines::DoMouseMove(CPoint pt)
{
	long nSize = m_arrLinesCoords.GetSize();
	if(nSize < 1)
		return true;

	_Draw();
	if(!m_bFirstLineComplete)
	{
		m_arrLinesCoords[nSize-1].BottomRight() = pt;
	}
	else
	{
		m_arrLinesCoords[nSize-1].TopLeft() = pt;
		m_arrLinesCoords[nSize-1].BottomRight() = _GetParallelPoint();
	}
	_Draw();

	return true;
}

bool CActionMeasParallelLines::DoRButtonUp(CPoint pt)
{ 
	Finalize();
	return true;
}

void CActionMeasParallelLines::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	long nSize = m_arrLinesCoords.GetSize();
	//for(long i = 0; i < nSize; i++)
	if(nSize > 0)
	{
		if (!_CheckCoords(m_arrLinesCoords[nSize-1].TopLeft(), m_arrLinesCoords[nSize-1].BottomRight()))
			return;
		CPoint pt1 = ::ConvertToWindow(m_punkTarget, m_arrLinesCoords[nSize-1].TopLeft());
		CPoint pt2 = ::ConvertToWindow(m_punkTarget, m_arrLinesCoords[nSize-1].BottomRight());

		dc.MoveTo(pt1.x - CROSS_OFFSET, pt1.y - CROSS_OFFSET);		//  "\ "
		dc.LineTo(pt1.x + CROSS_OFFSET, pt1.y + CROSS_OFFSET);		//  " \"
		dc.MoveTo(pt1.x + CROSS_OFFSET, pt1.y - CROSS_OFFSET);		//  " /"
		dc.LineTo(pt1.x - CROSS_OFFSET, pt1.y + CROSS_OFFSET);		//  "/ "

		if(pt2 != CPoint(0,0))
		{
			if (pt1 != pt2)
			{
				dc.MoveTo(pt2.x - CROSS_OFFSET, pt2.y - CROSS_OFFSET);		//  "\ "
				dc.LineTo(pt2.x + CROSS_OFFSET, pt2.y + CROSS_OFFSET);		//  " \"
				dc.MoveTo(pt2.x + CROSS_OFFSET, pt2.y - CROSS_OFFSET);		//  " /"
				dc.LineTo(pt2.x - CROSS_OFFSET, pt2.y + CROSS_OFFSET);		//  "/ "
			}

			dc.MoveTo(pt1);
			dc.LineTo(pt2);
		}
	}
}

bool CActionMeasParallelLines::AddMMObject(long nPos)
{
	CRect rcCoords = m_arrLinesCoords[nPos];
	m_arrParamKeys.SetSize(m_arrLinesCoords.GetSize());
	m_arrParamKeys[nPos] = -1;

	//1. create object list if it is absent
	if (m_ObjListWrp == 0)
	{	//create an object list
		IUnknown	*punkListObj = ::CreateTypedObject(szTypeObjectList);
		if (!punkListObj)
		{
			SetError(IDS_ERR_CANTCREATEOBJECTLIST);
			return false;
		}

		sptrIView	sptrV(m_punkTarget);
		IUnknown* punkDocument = 0;
		sptrV->GetDocument(&punkDocument);

		if(!punkDocument)
			return false;

		m_ObjListWrp.Attach(punkListObj, false);
		m_changes.SetToDocData(punkDocument, punkListObj);
		IDataContextPtr	ptrC = m_punkTarget;
		ptrC->SetActiveObject(0, m_ObjListWrp, aofActivateDepended);

		ASSERT(m_ObjListWrp != 0);

		IUnknownPtr sptrUnkIm(GetActiveImage(), false);
		INamedDataObject2Ptr sptrImage = sptrUnkIm;
		
		BOOL bBaseFlag = false;
		if (sptrImage != 0)
			sptrImage->IsBaseObject(&bBaseFlag);
		if (bBaseFlag) 
			SetBaseObject(m_ObjListWrp, sptrImage, punkDocument);

		punkDocument->Release();


		//sync table
		SyncViewData sd;
		::ZeroMemory( &sd, sizeof(SyncViewData) );

		sd.dwStructSize				= sizeof(SyncViewData);
		sd.punkObject2Activate		= (IUnknown*)m_ObjListWrp;
		sd.bCanChangeViewType		= false;
		sd.bSyncAllView				= false;
		sd.bOnlyViewInterfaceMatch	= true;
		memcpy( &(sd.iidViewMatch), &IID_IMeasureView, sizeof( IID ) );

		::SyncSplitterView( m_punkTarget, &sd );

	}

	//2. create object and add it to list
	IUnknown *punkObj = ::CreateTypedObject(szTypeObject);
	if(!punkObj)
		return false;

	INamedDataObject2Ptr sptrObj = punkObj;
	punkObj->Release();
	if (sptrObj == 0)
		return false;

	DWORD dwFlags = 0;
	sptrObj->GetObjectFlags(&dwFlags);
	sptrObj->SetParent(m_ObjListWrp, dwFlags);

	// add new object to undo/redo record
	sptrIView	sptrV(m_punkTarget);
	IUnknown* punkDocument = 0;
	sptrV->GetDocument(&punkDocument);
	if(!punkDocument)
		return false;
	IUnknownPtr sptrDocument(punkDocument);
	punkDocument->Release();
	m_changes.SetToDocData(sptrDocument, sptrObj);


	ICalibrPtr sptrCalibration;
	IUnknownPtr sptrUnkIm(GetActiveImage(), false);
	sptrCalibration = sptrUnkIm;
		

	CString strName;
	strName.Format("Line %d", ++m_nObjectCounter);

	INamedObject2Ptr sptrNO(sptrObj);
	sptrNO->SetUserName(_bstr_t(strName));

			
	//3. create mm object
	IUnknown	*punkMM = CreateTypedObject(szTypeManualMeasObject);
	ASSERT(punkMM);
	if (!punkMM)
		return false;
	
	dwFlags = 0;
	INamedDataObject2Ptr sptrMM = punkMM;
	punkMM->Release();
	if (sptrMM == 0)
		return false;
	
	IManualMeasureObjectPtr sptrMMObject = sptrMM;
	if (sptrMMObject == 0)
		return false;
	
	IUnknown* punkGroup = GetMeasManualGroup();
	IMeasParamGroupPtr sptrGroup(punkGroup);
	if(punkGroup) punkGroup->Release();
	
	long lpos = 0;
	long lNewKey = -1;
	ICalcObjectContainerPtr sptrCOC = m_ObjListWrp;
	if(sptrCOC != 0) 
	{
		sptrCOC->DefineParameter(-1, etLinear, sptrGroup, &lpos);
		ParameterContainer* pContainer;
		sptrCOC->GetNextParameter(&lpos, &pContainer);
		if(pContainer && pContainer->pDescr)
			lNewKey = pContainer->pDescr->lKey;
	}

	m_arrParamKeys[nPos] = lNewKey;
		
	DWORD dwParamType = (DWORD)etLinear;
	sptrMMObject->SetParamInfo(&lNewKey, &dwParamType);
//	sptrMMObject->SetObjectType((DWORD)emotLine);
	sptrMMObject->SetObjectType((DWORD)emotParallelLines);


	//4. exchange data with mm object
	TExchangeMeasData Data;
//	Data.dwObjectType = (DWORD)emotLine;
	Data.dwObjectType = (DWORD)emotParallelLines;
	Data.lNumPoints = 2;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(2 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	Data.pPoints[0].fX = rcCoords.TopLeft().x;
	Data.pPoints[0].fY = rcCoords.TopLeft().y;

	Data.pPoints[1].fX = rcCoords.BottomRight().x;
	Data.pPoints[1].fY = rcCoords.BottomRight().y;

	HRESULT hr = sptrMMObject->ExchangeData((DWORD*)&Data, FALSE);


	
	// set objetct to document and as child to parent meas object
	//recalc rectangel
	{
		CRect	rect;
		IRectObjectPtr	ptrRect( sptrMMObject );
		ptrRect->GetRect( &rect );
	}

	sptrMM->GetObjectFlags(&dwFlags);
	sptrMM->SetParent(sptrObj, dwFlags);
	m_changes.SetToDocData(sptrDocument, sptrMM);


	double fCalibration = 0;
	GUID guid;
	if (sptrCalibration == 0)
		fCalibration = 1.;
	else
		sptrCalibration->GetCalibration(&fCalibration, &guid);
	//5. process measure
	long nSize = m_arrParamKeys.GetSize() - 1;
	nSize = max(1, nSize);
	if(nSize > 1)
	{		
		long nCounter = -1;
		POSITION pos = m_ObjListWrp.GetFirstObjectPosition();

		double* pfVal = new double[nSize-1];
		ZeroMemory(pfVal, sizeof(double)*(nSize-1));

		while(pos)
		{
			nCounter++;

			IUnknown* punkObject = m_ObjListWrp.GetNextObject(pos);
			if(!punkObject)
				continue;
			
			if(nCounter < nSize-1)
			{
				ICalcObjectPtr sptrCalc = punkObject;
				pfVal[nCounter] = _GetDistance(nSize-1, nCounter)*fCalibration;
				sptrCalc->SetValue(m_arrParamKeys[nSize-1], pfVal[nCounter]);

				IMeasureObjectPtr sptrMeasObj = punkObject;
				sptrMeasObj->UpdateParent();
			}
			else
			{
				//ASSERT(FALSE);
			}

			punkObject->Release();

		}

		ICalcObjectPtr sptrCalc = sptrObj;
		for(long i = 0; i < nSize-1; i++)
		{
			sptrCalc->SetValue(m_arrParamKeys[i], pfVal[i]);
		}
		delete pfVal;
	}

	ICalcObjectPtr sptrCalc = sptrObj;
	sptrCalc->SetValue(m_arrParamKeys[nSize-1], 0);
	sptrCalibration = sptrMM;
	sptrCalibration->SetCalibration( fCalibration, &guid );

	IMeasureObjectPtr sptrMeasObj = sptrObj;
	sptrMeasObj->UpdateParent();

	return true;
}

double CActionMeasParallelLines::_GetDistance(long i, long j)
{
	CRect rc1 = m_arrLinesCoords[i];
	CRect rc2 = m_arrLinesCoords[j];

	double	x1_1, x2_1, y1_1, y2_1;
	double	a_1, b_1, c_1;

	x1_1 = rc1.left;
	x2_1 = rc1.right;
	y1_1 = rc1.top;
	y2_1 = rc1.bottom;

	a_1 = y2_1-y1_1;
	b_1 = x1_1-x2_1;
	c_1 = x1_1*a_1+y1_1*b_1;

	{
		double ct1 = a_1*x1_1+b_1*y1_1;
		double ct2 = a_1*x2_1+b_1*y2_1;
	}


	double	x1_2, x2_2, y1_2, y2_2;
	double	a_2, b_2, c_2;

	x1_2 = rc2.left;
	x2_2 = rc2.right;
	y1_2 = rc2.top;
	y2_2 = rc2.bottom;

	a_2 = y2_2-y1_2;
	b_2 = x1_2-x2_2;
	c_2 = x1_2*a_2+y1_2*b_2;

	{
		double ct = a_2*x2_2+b_2*y2_2;
	}

	double	a_p, b_p, c_p;
	a_p = b_1;
	b_p = -a_1;
	c_p = 0;

	double	dx_1, dy_1, d_1;
	dx_1 = c_1*b_p-c_p*b_1;
	dy_1 = a_1*c_p-a_p*c_1;
	d_1 = a_1*b_p-a_p*b_1;

	double	dx_2, dy_2, d_2;
	dx_2 = c_2*b_p-c_p*b_2;
	dy_2 = a_2*c_p-a_p*c_2;
	d_2 = a_2*b_p-a_p*b_2;


	double	xp_1, yp_1, xp_2, yp_2;

	xp_1 = dx_1/d_1;
	yp_1 = dy_1/d_1;
	xp_2 = dx_2/d_2;
	yp_2 = dy_2/d_2;

	return sqrt( (xp_1-xp_2)*(xp_1-xp_2)+(yp_1-yp_2)*(yp_1-yp_2) );

	/*double k1, k2, c1, c2, x, y;

	double dx = rc1.TopLeft().x-rc1.BottomRight().x;
	if(dx == 0) dx += 0.01;
	k1 = (rc1.TopLeft().y-rc1.BottomRight().y)*1./dx;
	c1 = rc1.TopLeft().y - k1*rc1.TopLeft().x;

	k2 = PI/2 + atan2(rc2.TopLeft().y - rc2.BottomRight().y, rc2.TopLeft().x - rc2.BottomRight().x);
	c2 = rc2.TopLeft().y - k2*rc2.TopLeft().x;
	
	x = (c1-c2)/(k2-k1);
	y = k1*x + c1;
	
	return sqrt(pow(x - rc2.TopLeft().x, 2) + pow(y - rc2.TopLeft().y, 2));*/
}

IUnknown * CActionMeasParallelLines::GetActiveImage()
{
	sptrINamedDataObject2 sptrN;

	IUnknown * punkImage = 0;

	if (CheckInterface(m_punkTarget, IID_IDocument))
		punkImage = ::GetActiveObjectFromDocument(m_punkTarget, szTypeImage);
	else
		punkImage = ::GetActiveObjectFromContext(m_punkTarget, szTypeImage);

	if (!punkImage)
		return 0;

	sptrN = punkImage;
	IUnknown * punkParent = 0;
	sptrN->GetParent(&punkParent);

	if (punkParent)
	{
		punkImage->Release();
		return punkParent;
	}
	return punkImage;
}


//CActionMeasAngle2Line implementation
CActionMeasAngle2Line::CActionMeasAngle2Line()
{
	m_point1 = m_point2 = m_point3 = m_point4 = CPoint(-1, -1);

//	m_fAngle = 0.;
	m_nMode = 0;
	m_bTracking = false;
//	m_dwParamType = etAngle;
	m_strPaneText = _T("Measure angle by two line");
}

CActionMeasAngle2Line::~CActionMeasAngle2Line()
{
}

bool CActionMeasAngle2Line::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 4;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(4 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	Data.pPoints[0].fX = m_point1.x;
	Data.pPoints[0].fY = m_point1.y;

	Data.pPoints[1].fX = m_point2.x;
	Data.pPoints[1].fY = m_point2.y;

	Data.pPoints[2].fX = m_point3.x;
	Data.pPoints[2].fY = m_point3.y;

	Data.pPoints[3].fX = m_point4.x;
	Data.pPoints[3].fY = m_point4.y;

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasAngle2Line::SupportType(DWORD dwType)
{
	if (dwType == etAngle)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasAngle2Line::GetMMObjectType()
{
	return (DWORD)emotAngle2Line;
}

DWORD CActionMeasAngle2Line::GetParamType()
{
	return (DWORD)etAngle;
}


bool CActionMeasAngle2Line::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ANGLE_MEAS);

	return true;
}

bool CActionMeasAngle2Line::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}


bool CActionMeasAngle2Line::DoLButtonDown(CPoint pt)
{
//	 if (!CMMActionObjectBase::DoLButtonDown(CPoint pt))
//		 return false;

	_Draw();
	if (m_nMode == 0)
	{
		m_point1 = m_point2 = pt;
		m_nMode = 1;
	}
	else if (m_nMode == 1)
	{
		m_point2 = pt;
		m_nMode = 2;
	}
	else if (m_nMode == 2)
	{
		m_point3 = m_point4 = pt;
		m_nMode = 3;
	}
	else if (m_nMode == 3)
	{
		m_point4 = pt;
		m_nMode = 4;
		CalcValue();
	}
	_Draw();
	return true;
}

bool CActionMeasAngle2Line::DoLButtonUp(CPoint pt)
{
	if (m_nMode > 3)
		Finalize();

	return true;
}

bool CActionMeasAngle2Line::DoMouseMove(CPoint pt)
{
	if (m_nMode == 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();
	}
	else if (m_nMode == 3)
	{
		_Draw();
		m_point4 = pt;

		_Draw();

		CalcValue();
	}
	return true;
}

bool CActionMeasAngle2Line::DoStartTracking(CPoint pt)
{
	if (m_nMode == 0 || m_nMode == 2)
		return false;

	m_bTracking = true;
	return true;
}

bool CActionMeasAngle2Line::DoTrack(CPoint pt)
{
	if (!m_bTracking)
		return false;

	if (m_nMode == 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();
	}
	else if (m_nMode == 3)
	{
		_Draw();
		m_point4 = pt;
		_Draw();

		CalcValue();
	}
	return true;
}

bool CActionMeasAngle2Line::DoFinishTracking(CPoint pt)
{
	if (!m_bTracking)
		return false;

	if (m_nMode == 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();
	}
	else if (m_nMode == 3)
	{
		_Draw();
		m_point4 = pt;
		_Draw();

		CalcValue();
	}

	m_bTracking = false;
	return true;
}

void CActionMeasAngle2Line::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	CPoint pt1, pt2, pt3, pt4;

	if (m_nMode > 0)
	{
		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		pt2 = ::ConvertToWindow(m_punkTarget, m_point2);

		if (m_point1.x != -1 && m_point1.y != -1)
			DrawCross(&dc, pt1);

		if (pt1 != pt2)
			DrawCross(&dc, pt2);

		dc.MoveTo(pt1);
		dc.LineTo(pt2);

	}

	if (m_nMode > 1)
	{
		pt3 = ::ConvertToWindow(m_punkTarget, m_point3);
		if (m_point3.x != -1 && m_point3.y != -1)
			DrawCross(&dc, pt3);
	}
	
	if (m_nMode > 2)
	{
		pt4 = ::ConvertToWindow(m_punkTarget, m_point4);
		if (m_point3.x != -1 && m_point3.y != -1 && pt3 != pt4)
			DrawCross(&dc, pt4);

		dc.MoveTo(pt3);
		dc.LineTo(pt4);

	}
}


//////////////////////////////////////////////////////////////////////
//CActionMeasAngle3Point implementation
CActionMeasAngle3Point::CActionMeasAngle3Point()
{
	m_point1 = m_point2 = m_point3 = CPoint(-1, -1);
	m_fAngle = 0.;
	m_nMode = 0;
	m_bTracking = false;
//	m_dwParamType = etAngle;
	m_bRight = true;
	m_bFirst = true;
	m_strPaneText = _T("Measure angle by three points");
}

CActionMeasAngle3Point::~CActionMeasAngle3Point()
{
}

bool CActionMeasAngle3Point::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 4;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(4 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;

	Data.pPoints[0].fX = m_point1.x;
	Data.pPoints[0].fY = m_point1.y;

	Data.pPoints[1].fX = m_point2.x;
	Data.pPoints[1].fY = m_point2.y;

	Data.pPoints[2].fX = m_point3.x;
	Data.pPoints[2].fY = m_point3.y;

	Data.pPoints[3].fX = m_bRight ? 1. : -1.;
	Data.pPoints[3].fY = 0.;

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasAngle3Point::SupportType(DWORD dwType)
{
	if (dwType == etAngle)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasAngle3Point::GetMMObjectType()
{
	return (DWORD)emotAngle3Point;
}

DWORD CActionMeasAngle3Point::GetParamType()
{
	return (DWORD)etAngle;
}


bool CActionMeasAngle3Point::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ANGLE_MEAS);
	return true;
}

bool CActionMeasAngle3Point::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}

bool CActionMeasAngle3Point::DoLButtonDown(CPoint pt)
{
	_Draw();
	if (m_nMode == 0)
	{
		m_point1 = m_point2 = pt;
		m_nMode = 1;
	}
	else if (m_nMode == 1)
	{
		if (pt != m_point1)
		{
			m_point2 = m_point3 = pt;
			m_nMode = 2;
			m_bFirst = true;
		}
	}
	else if (m_nMode == 2)
	{
		if (pt != m_point2)
		{
			m_point3 = pt;
			m_nMode = 3;
			CalcValue();
			CheckDirection();
		}
	}
	_Draw();

	return true;
}

bool CActionMeasAngle3Point::DoLButtonUp(CPoint pt)
{
	if (m_nMode > 2)
		Finalize();

	return true;
}

bool CActionMeasAngle3Point::Invoke()
{
	if (m_nMode != 3 || m_point2 == m_point3)
		return false;
	return CMMActionObjectBase::Invoke();
}

bool CActionMeasAngle3Point::DoMouseMove(CPoint pt)
{
	_Draw();
	if (m_nMode == 1)
	{
		m_point2 = pt;
	}
	else if (m_nMode == 2)
	{
		m_point3 = pt;

		CalcValue();

		CheckDirection();
	}
	_Draw();

	return true;
}

void CActionMeasAngle3Point::CheckDirection()
{
	double dx1, dx2;
	double dy1, dy2;

	dx1 = (m_point1.x - m_point2.x);
	dy1 = -(m_point1.y - m_point2.y);
	dx2 = (m_point3.x - m_point2.x);
	dy2 = -(m_point3.y - m_point2.y);

	double fA1 = ::atan2(dy1, dx1);
	double fA2 = ::atan2(dy2, dx2);

	bool bright = true;
	if (fA1 > 0) // 1 || 2
	{
		if (fA2 > 0) // 1 || 2
			bright = (fA2 - fA1) <= 0.;
		else // 3 || 4
			bright = (fA1 - fA2) <= PI;
	}
	else // 3 || 4
	{
		if (fA2 > 0) // 1 || 2
			bright = (fA2 - fA1) >= PI;
		else // 3 || 4
			bright = (fA2 - fA1) <= 0;
	}
	
	double d = fA2 - fA1;
	if (d < 0) 
		d += 2 * PI;

	if (d > 2 * PI) 
		d -= 2 * PI;

	if (d > PI) 
		d = -2 * PI + d;

	if (m_bFirst)
	{
		m_bRight = bright;
		m_bFirst = false;
		m_fAngle = d;
	}
	else
	{
		double flim = PI / 6.;
		if (((m_fAngle * d) < 0) && ((fabs(d) <= (PI/2 - flim)) && (fabs(d) <= (PI / 2 + flim))))
		{
			if (bright != m_bRight)
				m_bRight = bright;
		}
		m_fAngle = d;
	}
}



bool CActionMeasAngle3Point::DoStartTracking(CPoint pt)
{
	m_bTracking = true;
	return true;
}

bool CActionMeasAngle3Point::DoTrack(CPoint pt)
{
	if (!m_bTracking)
		return false;

	_Draw();
	if (m_nMode == 2)
	{
		m_point2 = pt;
	}
	else if (m_nMode == 3)
	{
		m_point3 = pt;
		CalcValue();
		CheckDirection();
	}
	_Draw();
	return true;
}

bool CActionMeasAngle3Point::DoFinishTracking(CPoint pt)
{
	if (!m_bTracking)
		return false;

	_Draw();
	if (m_nMode == 2)
	{
		m_point2 = pt;
	}
	else if (m_nMode == 3)
	{
		m_point3 = pt;
	}
	_Draw();
	
	m_bTracking = false;
	return true;
}

void CActionMeasAngle3Point::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	CPoint pt1, pt2, pt3, ptc;
	
	if (!m_nMode)
		return;
	
	if (m_nMode < 2)
	{
		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		pt2 = ::ConvertToWindow(m_punkTarget, m_point2);

		DrawCross(&dc, pt1);

		if (pt1 != pt2)
			DrawCross(&dc, pt2);

		dc.MoveTo(pt1);
		dc.LineTo(pt2);
	}
	else if (m_nMode > 1)
	{
		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		pt2 = ::ConvertToWindow(m_punkTarget, m_point2);
		pt3 = ::ConvertToWindow(m_punkTarget, m_point3);

		DrawCross(&dc, pt1);
	
		if (pt1 != pt2)
			DrawCross(&dc, pt2);

		if (pt2 != pt3)
			DrawCross(&dc, pt3);
		
		dc.MoveTo(pt1);
		dc.LineTo(pt2);

		dc.MoveTo(pt2);
		dc.LineTo(pt3);

		DrawAngle(&dc);
	}
}

void CActionMeasAngle3Point::DrawAngle(CDC * pDC)
{
	if (!pDC)
		return;

	if (m_point1 == m_point2 || m_point2 == m_point3 || m_point3 == m_point1)
		return;
	
	CPoint pt1, pt2, pt3;

	pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
	pt2 = ::ConvertToWindow(m_punkTarget, m_point2);
	pt3 = ::ConvertToWindow(m_punkTarget, m_point3);

	double	d1, d2, d;
	int	x1, x2, x3, x4, y1, y2, y3, y4;
			
	d = CalcAngles(d1, d2);

	x1 = pt2.x + m_nArcSize;
	x2 = pt2.x - m_nArcSize;
	y1 = pt2.y + int(m_nArcSize );
	y2 = pt2.y - int(m_nArcSize );

	if (d > 5. / 180. * PI)	//Меньше 5 градусов не рисовать
	{
		if (m_bRight)
		{
			d = d1;
			d1 = d2;
			d2 = d;
		}

		x3 = pt2.x + int(m_nArcSize * cos(d1));
		y3 = pt2.y + int(m_nArcSize * sin(d1) );
		x4 = pt2.x + int(m_nArcSize * cos(d2));
		y4 = pt2.y + int(m_nArcSize * sin(d2) );

		CPoint ptB(x3, y3);
		CPoint ptE(x4, y4);

		if (ptB != ptE)
			pDC->Arc(x1, y1, x2, y2, x3, y3, x4, y4);
	}
}

double CActionMeasAngle3Point::CalcAngles(double & fA1, double & fA2)
{
	if (m_nMode < 2)
		return 0;

	double dx1, dx2;
	double dy1, dy2;

	dx1 = (m_point1.x - m_point2.x);
	dy1 = (m_point1.y - m_point2.y);
	dx2 = (m_point3.x - m_point2.x);
	dy2 = (m_point3.y - m_point2.y);

	fA1 = ::atan2(dy1, dx1);
	fA2 = ::atan2(dy2, dx2);

	double d = fA2 - fA1;

	if (d < 0) 
		d += 2 * PI;

	if (d > 2 * PI) 
		d -= 2 * PI;

	if (!m_bRight)
		d = 2 * PI - d;

//	TRACE ("angle : %d\n", (int)(d * 180 / PI));
	return d;
}

//////////////////////////////////////////////////////////////////////
//CActionMeasCycle3 implementation
CActionMeasCycle3::CActionMeasCycle3()
{
	m_point1 = CPoint(-1, -1);
	m_point2 = CPoint(-1, -1);
	m_point3 = CPoint(-1, -1);

	m_pointCenter = CPoint(-1, -1);
	m_fRadius = 0.;

	m_nMode = 0;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure radius of three-points cycle");
}

CActionMeasCycle3::~CActionMeasCycle3()
{
}

bool CActionMeasCycle3::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 3;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(3 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	Data.pPoints[0].fX = m_point1.x;
	Data.pPoints[0].fY = m_point1.y;

	Data.pPoints[1].fX = m_point2.x;
	Data.pPoints[1].fY = m_point2.y;

	Data.pPoints[2].fX = m_point3.x;
	Data.pPoints[2].fY = m_point3.y;

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasCycle3::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasCycle3::GetMMObjectType()
{
	return (DWORD)emotCycle3;
}

DWORD CActionMeasCycle3::GetParamType()
{
	return (DWORD)etLinear;
}



bool CActionMeasCycle3::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_RADIUS_MEAS);
	return true;
}

bool CActionMeasCycle3::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}

bool CActionMeasCycle3::DoLButtonDown(CPoint pt)
{
	_Draw();
	if (m_nMode == 0)
	{
		m_point1 = pt;
		m_point2 = m_point1;
		m_point3 = m_point1;
		m_nMode = 1;
	}
	else if (m_nMode == 1)
	{
		m_point2 = m_point3 = pt;
		m_nMode = 2;
	}
	else if (m_nMode == 2)
	{
		_Draw();
		m_point3 = pt;
		_Draw();
		CalcValue();
		m_nMode = 3;
	}
	_Draw();
	return true;
}

bool CActionMeasCycle3::DoLButtonUp(CPoint pt)
{
	if (m_nMode > 2)
		Finalize();

	return true;
}

bool CActionMeasCycle3::DoStartTracking(CPoint pt)
{
	if (m_nMode < 2)
		return false;

	m_bTracking = true;
	return true;
}

bool CActionMeasCycle3::DoMouseMove(CPoint pt)
{
	if (m_nMode > 1/* && !m_bTracking*/)
	{
		_Draw();
		m_point3 = pt;
		_Draw();
		CalcValue();
	}
	return true;
}

bool CActionMeasCycle3::DoTrack(CPoint pt)
{
	if (!m_bTracking)
		return false;

	if (m_nMode > 1)
	{
		_Draw();
		m_point3 = pt;
		_Draw();
		double fVal =  0;
		CalcValue();
	}
	return true;
}

bool CActionMeasCycle3::DoFinishTracking(CPoint pt)
{
	if (!m_bTracking)
		return false;

	_Draw();
	m_point3 = pt;
	_Draw();
	
	m_bTracking = false;
	return true;
}

void CActionMeasCycle3::DoDraw(CDC &dc)
{
	CPoint pt1, pt2, pt3, ptc;
	if (!m_nMode)
		return;

	_RestrictDraw rd(dc, m_punkTarget);
	if (m_nMode == 1)
	{
		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		DrawCross(&dc, pt1);
	}
	else if (m_nMode >= 2)
	{
		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		pt2 = ::ConvertToWindow(m_punkTarget, m_point2);
		pt3 = ::ConvertToWindow(m_punkTarget, m_point3);
		DrawCross(&dc, pt1);

		if (pt1 != pt2)
			DrawCross(&dc, pt2);

		if (pt2 != pt3 && pt1 != pt3)
			DrawCross(&dc, pt3);

		CRect	rect;
		if (!CalcCycle(rect))
			return;

		if( pt1 != pt2 && pt2 != pt3 && pt1 != pt3 )
		{
			rect = ::ConvertToWindow(m_punkTarget, rect);
			dc.Ellipse(rect);

			ptc = ::ConvertToWindow(m_punkTarget, m_pointCenter);
			
			DrawCross(&dc, ptc);
			dc.MoveTo(ptc);
			dc.LineTo(pt3);
		}
	}
}

bool CActionMeasCycle3::CalcCycle(CRect & rc)
{
	if (m_nMode < 2)
		return false;

	m_fRadius = 0; 
	m_pointCenter = CPoint(-1, -1);
	
	if (m_point1 == m_point2 || m_point2 == m_point3)
		return false;

	m_point1;
	m_point2;
	m_point3;

	double x1, x2, x3, y1, y2, y3; //Координаты в объекте
	double x1c, x2c, xc, y1c, y2c, yc; //Координаты центров хорд и окружности
	double a1, b1, c1, a2, b2, c2;  //параметры уравнений прямых
	double a1p, b1p, c1p, a2p, b2p, c2p;  //параметры уравнений перпендик. прямых
	double delta, deltaX, deltaY; //Определители
 
//Определение координат точек в объекте
	x1 = m_point1.x;
	x2 = m_point2.x;
	x3 = m_point3.x;
	y1 = m_point1.y;
	y2 = m_point2.y;
	y3 = m_point3.y;

	x1c = (x1 + x2) / 2.0;
	x2c = (x3 + x2) / 2.0;
	y1c = (y1 + y2) / 2.0;
	y2c = (y3 + y2) / 2.0;
//Определение уравнения прямых, проходящих через эти точки, вида ax+by+c = 0
	if (::fabs(x1 - x2) > fabs(y1 - y2))
	{
		b1 = 1.0;
		a1 = -(y2 - y1) / (x2 - x1);
		c1 = -a1 * x1 - b1 * y1;
	}
	else
	{
		a1 = 1.0;
		b1 = -(x2 - x1) / (y2 - y1);
		c1 = -a1 * x1 - b1 * y1;
	}
 
	if (::fabs(x3 - x2) > fabs(y3 - y2))
	{
		b2 = 1;
		a2 = -(y2 - y3) / (x2 - x3);
		c2 = -a2 * x3 - b2 * y3;
	}
	else
	{
		a2 = 1;
		b2 = -(x2 - x3) / (y2 - y3);
		c2 = -a2 * x3 - b2 * y3;
	}
//Определение уравнений прямых, проходящих через центры хорд и перпендикулярных им
	a1p = -b1;
	b1p = a1;
	c1p =  -(a1p * x1c + b1p * y1c);
 
	a2p = -b2;
	b2p = a2;
	c2p =  -(a2p * x2c + b2p * y2c);
//Центр окружности лежит на пересечении этих прямых => ищем точку пересечения
	delta = a1p * b2p - a2p * b1p;
	deltaX = -c1p * b2p + c2p * b1p;
	deltaY = -a1p * c2p + a2p * c1p;
 
	xc = deltaX / delta;
	yc = deltaY / delta;

//Это и есть центр окружности
	m_fRadius =  ::sqrt((xc - x1) * (xc - x1) + (yc - y1) * (yc - y1));
	m_pointCenter.x = int(xc);
	m_pointCenter.y = int(yc);
 
	rc.left = int(m_pointCenter.x - m_fRadius);
	rc.right = int(m_pointCenter.x + m_fRadius);
	rc.top = int(m_pointCenter.y - m_fRadius);
	rc.bottom = int(m_pointCenter.y + m_fRadius);
 
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionMeasCycle2 implementation
CActionMeasCycle2::CActionMeasCycle2()
{
	m_point1 = CPoint(-1, -1);
	m_point2 = CPoint(-1, -1);

	m_pointCenter = CPoint(-1, -1);
	m_nRadius = 0;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure radius of two-points cycle");
}

CActionMeasCycle2::~CActionMeasCycle2()
{
}

bool CActionMeasCycle2::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 2;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(2 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	Data.pPoints[0].fX = m_point1.x;
	Data.pPoints[0].fY = m_point1.y;

	Data.pPoints[1].fX = m_point2.x;
	Data.pPoints[1].fY = m_point2.y;
	
	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasCycle2::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasCycle2::GetMMObjectType()
{
	return (DWORD)emotCycle2;
}

DWORD CActionMeasCycle2::GetParamType()
{
	return (DWORD)etLinear;
}


bool CActionMeasCycle2::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_RADIUS_MEAS);
	return true;
}

bool CActionMeasCycle2::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}

void CActionMeasCycle2::DoDraw(CDC &dc)
{
	CPoint pt1, pt2, ptc;
	CRect	rect;

	_RestrictDraw rd(dc, m_punkTarget);
	m_pointCenter = CPoint((m_point1.x + m_point2.x) >> 1, (m_point1.y + m_point2.y) >> 1);
	double dx = (m_point2.x - m_point1.x);
	double dy = (m_point2.y - m_point1.y);
	double fRadius = ::sqrt(dx * dx + dy * dy) / 2.;
	m_nRadius = (int)fRadius;
	fRadius *= m_fZoom;

	pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
	pt2 = ::ConvertToWindow(m_punkTarget, m_point2);
	ptc = CPoint((pt1.x + pt2.x) >> 1, (pt1.y + pt2.y) >> 1);;

	if (m_point1.x != -1 && m_point1.y != -1)
		DrawCross(&dc, pt1);

	if (m_point2.x != -1 && m_point2.y != -1)
		DrawCross(&dc, pt2);

	if (pt1.x != -1 && pt1.y != -1)
	{
		rect.left = int(ptc.x - fRadius  + .5);
		rect.right = int(ptc.x + fRadius  + .5);
		rect.top = int(ptc.y - fRadius  + .5);
		rect.bottom = int(ptc.y + fRadius  + .5);

//		rect = ::ConvertToWindow(m_punkTarget, rect);
		dc.Ellipse(rect);
//		DrawCross(&dc, ptc);

//		dc.MoveTo(ptc);
//		dc.LineTo(pt2);
	}
}

bool CActionMeasCycle2::DoStartTracking(CPoint pt)
{
	_Draw();

	m_point1 = pt;
	m_point2 = pt;
	m_pointCenter = pt;
	m_nRadius = 0;
	
	_Draw();
	return true;
}

bool CActionMeasCycle2::DoTrack(CPoint pt)
{
	_Draw();
	
	m_point2 = pt;

	_Draw();
	double fVal =  0;
	CalcValue();
	return true;
}

bool CActionMeasCycle2::DoFinishTracking( CPoint pt )
{
	Finalize();
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionMeasBrokenLine implementation
CActionMeasBrokenLine::CActionMeasBrokenLine()
{
	m_pContour = 0;
	m_bTracking = false;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure length of broken line");
}

CActionMeasBrokenLine::~CActionMeasBrokenLine()
{
	::ContourDelete(m_pContour);
}

bool CActionMeasBrokenLine::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	if (!m_pContour || m_pContour->nContourSize < 2)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = m_pContour->nContourSize;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(m_pContour->nContourSize * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	for (int i = 0; i < m_pContour->nContourSize; i++)
	{
		Data.pPoints[i].fX = m_pContour->ppoints[i].x;
		Data.pPoints[i].fY = m_pContour->ppoints[i].y;
	}

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasBrokenLine::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasBrokenLine::GetMMObjectType()
{
	return (DWORD)emotBrokenLine;
}

DWORD CActionMeasBrokenLine::GetParamType()
{
	return (DWORD)etLinear;
}


bool  CActionMeasBrokenLine::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_BROKENLINE_MEAS);
	m_pContour = ::ContourCreate();
	return true;
}

bool CActionMeasBrokenLine::DoLButtonDown(CPoint pt)
{
	_Draw();
	
	if (m_pContour->nContourSize < 1)
		::ContourAddPoint(m_pContour, pt.x, pt.y);

	::ContourAddPoint(m_pContour, pt.x, pt.y);
	_Draw();

	CalcValue();
	return true;
}

bool CActionMeasBrokenLine::DoLButtonUp(CPoint pt)
{
	return true;
}

bool CActionMeasBrokenLine::DoLButtonDblClick(CPoint pt)
{
	_Draw();

	if (m_pContour && m_pContour->nContourSize > 0)
		m_pContour->nContourSize--;

	_Draw();

	Finalize();
	return true;
}

bool CActionMeasBrokenLine::DoMouseMove(CPoint pt)
{
	if (m_pContour->nContourSize > 0 && !m_bTracking)
	{
		_Draw();

		m_pContour->ppoints[m_pContour->nContourSize - 1].x = pt.x;
		m_pContour->ppoints[m_pContour->nContourSize - 1].y = pt.y;

		_Draw();
		CalcValue();
	}
	return true;
}


bool CActionMeasBrokenLine::DoStartTracking(CPoint pt)
{
	m_bTracking = true;

	return true;
}

bool CActionMeasBrokenLine::DoTrack(CPoint pt)
{
	ASSERT(m_pContour);
	ASSERT(m_pContour->nContourSize > 0);

	ContourPoint	cpt = m_pContour->ppoints[m_pContour->nContourSize - 1];

	if (cpt.x  == pt.x && cpt.y == pt.y)
		return true;


	if (m_pContour->nContourSize > 0)
	{
		_Draw();

		m_pContour->ppoints[m_pContour->nContourSize - 1].x = pt.x;
		m_pContour->ppoints[m_pContour->nContourSize - 1].y = pt.y;

		_Draw();

		CalcValue();
	}
	return true;
}

bool CActionMeasBrokenLine::DoFinishTracking(CPoint pt)
{
	ASSERT(m_pContour);
	ASSERT(m_pContour->nContourSize > 0);

//	ContourPoint	cpt = m_pContour->ppoints[m_pContour->nContourSize - 1];

//	if (cpt.x != pt.x && cpt.y != pt.y)
	{
		_Draw();
		m_pContour->ppoints[m_pContour->nContourSize - 1].x = pt.x;
		m_pContour->ppoints[m_pContour->nContourSize - 1].y = pt.y;
		_Draw();
	}
	CalcValue();

	m_bTracking = false;

	return true;
}

void CActionMeasBrokenLine::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	::ContourDraw(dc, m_pContour, m_punkTarget, 0);

	if (m_pContour->nContourSize > 0)
	{
		for (int nPoint = /*m_bTracking ? m_pContour->nContourSize - 1 : */0; nPoint < m_pContour->nContourSize; nPoint++)
		{
			CPoint	point;
			point.x = m_pContour->ppoints[nPoint].x;
			point.y = m_pContour->ppoints[nPoint].y;
			point = ::ConvertToWindow(m_punkTarget, point);

			DrawCross(&dc, point);
		}
	}
}

bool CActionMeasBrokenLine::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}


//////////////////////////////////////////////////////////////////////
//CActionMeasLine implementation
CActionMeasLine::CActionMeasLine()
{
	m_point1 = m_point2 = CPoint(-1, -1);
	m_nMode = 0;
	m_bTracking = false;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure length of line");
}

CActionMeasLine::~CActionMeasLine()
{
}

bool CActionMeasLine::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 2;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(2 * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	Data.pPoints[0].fX = m_point1.x;
	Data.pPoints[0].fY = m_point1.y;

	Data.pPoints[1].fX = m_point2.x;
	Data.pPoints[1].fY = m_point2.y;

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasLine::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;

	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasLine::GetMMObjectType()
{
	return (DWORD)emotLine;
}

DWORD CActionMeasLine::GetParamType()
{
	return (DWORD)etLinear;
}


bool CActionMeasLine::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_LENGTH_MEAS);
	return true;
}

bool CActionMeasLine::DoLButtonDown(CPoint pt)
{
	_Draw();
	if (m_nMode == 0)
	{
		m_point1 = m_point2 = pt;
		m_nMode = 1;
	}
	else if (m_nMode == 1)
	{
		m_point2 = pt;
		m_nMode = 2;
	}
	_Draw();

	return true;
}

bool CActionMeasLine::DoLButtonUp(CPoint pt)
{
	if (m_nMode > 1)
		Finalize();

	return true;
}

bool CActionMeasLine::DoMouseMove(CPoint pt)
{
	if (m_nMode == 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();
		CalcValue();
	}
	return true;
}

bool CActionMeasLine::DoStartTracking(CPoint pt)
{
	m_bTracking = true;

	return true;
}

bool CActionMeasLine::DoTrack(CPoint pt)
{
	if (m_nMode >= 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();

		CalcValue();
	}
	
	return true;
}

bool CActionMeasLine::DoFinishTracking(CPoint pt)
{
	if (m_nMode >= 1)
	{
		_Draw();
		m_point2 = pt;
		_Draw();
	}

	m_bTracking = false;

	return true;
}

void CActionMeasLine::DoDraw(CDC &dc)
{
	CPoint pt1, pt2;

	if (m_nMode > 0)
	{
		_RestrictDraw rd(dc, m_punkTarget);
		/*(IScrollZoomSitePtr sptrSZS(m_punkTarget);
		SIZE sz;
		sptrSZS->GetClientSize(&sz);
		sz = ::ConvertToWindow(m_punkTarget, CSize(sz));
		dc.IntersectClipRect(0, 0, sz.cx, sz.cy);*/

		pt1 = ::ConvertToWindow(m_punkTarget, m_point1);
		pt2 = ::ConvertToWindow(m_punkTarget, m_point2);

		if (m_point1.x != -1 && m_point1.y != -1)
			DrawCross(&dc, pt1);

		if (pt1 != pt2)
			DrawCross(&dc, pt2);

		dc.MoveTo(pt1);
		dc.LineTo(pt2);
	}
}

bool CActionMeasLine::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}



//////////////////////////////////////////////////////////////////////
//CActionMeasCount implementation
CActionMeasCount::CActionMeasCount()
{
	m_pContour = 0;
//	m_dwParamType = etCounter;
	m_bFinalizeOnClick = true;
	m_strPaneText = _T("Measure count");
}

CActionMeasCount::~CActionMeasCount()
{
	::ContourDelete(m_pContour);
}

bool CActionMeasCount::DoCalcParam()
{
	if (!m_pContour || !m_pContour->nContourSize)
		return false;

	if (m_ptrManualMeasObject == 0)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = m_pContour->nContourSize;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(m_pContour->nContourSize * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	for (int i = 0; i < m_pContour->nContourSize; i++)
	{
		Data.pPoints[i].fX = m_pContour->ppoints[i].x;
		Data.pPoints[i].fY = m_pContour->ppoints[i].y;
	}

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasCount::SupportType(DWORD dwType)
{
	if (dwType == etCounter)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasCount::GetMMObjectType()
{
	return (DWORD)emotCount;
}

DWORD CActionMeasCount::GetParamType()
{
	return (DWORD)etCounter;
}


bool CActionMeasCount::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_COUNT_MEAS);
	m_pContour = ::ContourCreate();

	// check meas manual object already exists
	// if so we not need to create new object 
	// and need to get its data and transfort it to point

	m_bFinalizeOnClick = !CheckPrevObject();
	return true;
}

void CActionMeasCount::Finalize()
{
	CMMActionObjectBase::Finalize();
}

bool CActionMeasCount::CheckPrevObject()
{
//	if (m_sptrPrevMMObject == 0)
//		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = 0;
	Data.pPoints = 0;

	//ASSERT( false );
	return false;
	/*
	// get object's data
	if (FAILED(m_sptrPrevMMObject->ExchangeData((DWORD*)&Data, TRUE)))
		return false;
	
	for (int i = 0; i < Data.lNumPoints; i++)
	{
		CPoint pt;
		pt.x = Data.pPoints[i].fX;
		pt.y = Data.pPoints[i].fY;

		::ContourAddPoint(m_pContour, pt.x, pt.y);
	}

	if (Data.pPoints)
		CoTaskMemFree((LPVOID)Data.pPoints);

	if (m_pContour->nContourSize > 0)
	{
		double fVal =  0;
		if (CalcValue(fVal))
		{
			CString str;
			str.Format("Count : %d", (int)fVal);
			SetPaneText(str);
		}
	}
	*/

	return true;
}

bool CActionMeasCount::DoUpdateSettings()
{
	return CMMActionObjectBase::DoUpdateSettings();
}
	
bool CActionMeasCount::DoLButtonDown(CPoint pt)
{
	::ContourAddPoint(m_pContour, pt.x, pt.y);
	CalcValue();

	_Draw();

	return true;
}

bool CActionMeasCount::DoLButtonUp(CPoint pt)
{
//	if (!m_bFinalizeOnClick)
//		UpdateObjectData();
	return true;
}

bool CActionMeasCount::DoRButtonUp(CPoint pt)
{
	DoCalcParam();
	Finalize();

	return true;
}

bool CActionMeasCount::Invoke()
{
	bool b = CMMActionObjectBase::Invoke();
	LeaveMode();
	return b;
}

void CActionMeasCount::DoDraw(CDC &dc)
{
	if (!m_pContour)
		return;


	dc.SetROP2( R2_COPYPEN );
	IDrawObjectPtr	ptrDraw( m_ptrManualMeasObject );

	COLORREF	clr = get_class_color( -1 );
		//::GetValueColor( GetAppUnknown(), szPluginClasses, szUnkClassColor, RGB(255, 255, 0) );
	CPen	pen( PS_SOLID, 0, clr );
	CPen	*poldPen = dc.SelectObject( &pen );
	dc.SetTextColor( clr );

	ptrDraw->Paint( dc, NORECT, m_punkTarget, 0, 0 );

	dc.SelectObject( poldPen );

	/*for (int nPoint = 0; nPoint < m_pContour->nContourSize; nPoint++)
	{
		CPoint point;
		point.x = m_pContour->ppoints[nPoint].x;
		point.y = m_pContour->ppoints[nPoint].y;
		point = ::ConvertToWindow(m_punkTarget, point);
		DrawCross(&dc, point);
	}*/
}


//////////////////////////////////////////////////////////////////////
//CActionMeasFreeLine implementation
CActionMeasFreeLine::CActionMeasFreeLine()
{
	m_pContour = 0;
	m_bTracking = false;
	m_nMouseMode = fmUndef;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure length of free line");
}

CActionMeasFreeLine::~CActionMeasFreeLine()
{
	::ContourDelete(m_pContour);
}

bool CActionMeasFreeLine::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	if (!m_pContour || m_pContour->nContourSize < 2)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = m_pContour->nContourSize;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(m_pContour->nContourSize * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	for (int i = 0; i < m_pContour->nContourSize; i++)
	{
		Data.pPoints[i].fX = m_pContour->ppoints[i].x;
		Data.pPoints[i].fY = m_pContour->ppoints[i].y;
	}

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasFreeLine::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasFreeLine::GetMMObjectType()
{
	return (DWORD)emotFreeLine;
}

DWORD CActionMeasFreeLine::GetParamType()
{
	return (DWORD)etLinear;
}


bool CActionMeasFreeLine::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_FREELINE_MEAS);
	m_pContour = ::ContourCreate();
	return true;
}

bool CActionMeasFreeLine::DoLButtonDown(CPoint pt)
{
	_Draw();
	if (m_pContour->nContourSize < 1)
		::ContourAddPoint(m_pContour, pt.x, pt.y);
	::ContourAddPoint(m_pContour, pt.x, pt.y);
	_Draw();
	CalcValue();
	return false;
}

bool CActionMeasFreeLine::DoLButtonUp(CPoint pt)
{
	_Draw();
	::ContourAddPoint(m_pContour, pt.x, pt.y);
	_Draw();
	CalcValue();
	return true;
}

bool CActionMeasFreeLine::DoLButtonDblClick(CPoint pt)
{
	_Draw();

	if (m_pContour && m_pContour->nContourSize > 0)
		m_pContour->nContourSize--;

	_Draw();

	Finalize();
	return true;
}

bool CActionMeasFreeLine::DoMouseMove(CPoint pt)
{
	if (m_pContour->nContourSize > 0 && !m_bTracking)
	{
		_Draw();

		m_pContour->ppoints[m_pContour->nContourSize - 1].x = pt.x;
		m_pContour->ppoints[m_pContour->nContourSize - 1].y = pt.y;

		_Draw();
		CalcValue();
	}
	return true;
}

bool CActionMeasFreeLine::DoStartTracking(CPoint pt)
{
	m_bTracking = true;

	return true;
}

bool CActionMeasFreeLine::DoTrack(CPoint pt)
{
	ASSERT(m_pContour);
	ASSERT(m_pContour->nContourSize > 0);

	ContourPoint	cpt = m_pContour->ppoints[m_pContour->nContourSize - 1];

	if (cpt.x  == pt.x && cpt.y == pt.y)
		return true;

	_Draw();
	::ContourAddPoint(m_pContour, pt.x, pt.y);
	_Draw();

	CalcValue();
	return true;
}

bool CActionMeasFreeLine::DoFinishTracking(CPoint pt)
{
	ASSERT(m_pContour);
	ASSERT(m_pContour->nContourSize > 0);

	ContourPoint	cpt = m_pContour->ppoints[m_pContour->nContourSize - 1];

	if (cpt.x != pt.x && cpt.y != pt.y)
	{
		_Draw();
		::ContourAddPoint(m_pContour, pt.x, pt.y);
		_Draw();
	}

	m_bTracking = false;

	if (m_nMouseMode == fmClick)
		Finalize();

	return true;
}

void CActionMeasFreeLine::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	::ContourDraw(dc, m_pContour, m_punkTarget, 0);
	if (m_pContour->nContourSize > 0)
	{
		CPoint	point;
		point.x = m_pContour->ppoints[0].x;
		point.y = m_pContour->ppoints[0].y;
		point = ::ConvertToWindow(m_punkTarget, point);
		DrawCross(&dc, point);

		if (m_pContour->nContourSize > 1)
		{
			point.x = m_pContour->ppoints[m_pContour->nContourSize - 1].x;
			point.y = m_pContour->ppoints[m_pContour->nContourSize - 1].y;
			point = ::ConvertToWindow(m_punkTarget, point);
			DrawCross(&dc, point);
		}
	}
}

bool CActionMeasFreeLine::DoUpdateSettings()
{
	m_nMouseMode = (EMouseMode)GetValueInt(::GetAppUnknown(false), szManualMeasure, szMouseMode, (long)fmClick);
	return CMMActionObjectBase::DoUpdateSettings();
}




//////////////////////////////////////////////////////////////////////
//CActionMeasSpline implementation
CActionMeasSpline::CActionMeasSpline()
{
	m_pContour  = 0;
//	m_dwParamType = etLinear;
	m_strPaneText = _T("Measure length of spline line");
}

CActionMeasSpline::~CActionMeasSpline()
{
	::ContourDelete(m_pContour);
}

bool CActionMeasSpline::DoCalcParam()
{
	if (m_ptrManualMeasObject == 0)
		return false;

	if (!m_pContour || m_pContour->nContourSize < 2)
		return false;

	TExchangeMeasData Data;
	Data.dwObjectType = GetMMObjectType();
	Data.lNumPoints = m_pContour->nContourSize;
	Data.pPoints = (TMeasPoint*)CoTaskMemAlloc(m_pContour->nContourSize * sizeof(TMeasPoint));

	if (!Data.pPoints)
		return false;
 
	for (int i = 0; i < m_pContour->nContourSize; i++)
	{
		Data.pPoints[i].fX = m_pContour->ppoints[i].x;
		Data.pPoints[i].fY = m_pContour->ppoints[i].y;
	}

	HRESULT hr = m_ptrManualMeasObject->ExchangeData((DWORD*)&Data, FALSE);
	return (SUCCEEDED(hr)) ? true : false;
}

bool CActionMeasSpline::SupportType(DWORD dwType)
{
	if (dwType == etLinear)
		return true;
	else if (dwType == etUndefined)
		return true;

	return false;
}

DWORD CActionMeasSpline::GetMMObjectType()
{
	return (DWORD)emotSpline;
}

DWORD CActionMeasSpline::GetParamType()
{
	return (DWORD)etLinear;
}

bool CActionMeasSpline::Initialize()
{
	AddPropertyPageDescription(c_szCMeasPage);
	if( !CMMActionObjectBase::Initialize() )
		return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_SPLINE_MEAS);
	m_hCursourCycled = AfxGetApp()->LoadCursor(IDC_TARGET_MEAS);

	m_pContour  = ::ContourCreate();
	return true;
}

bool CActionMeasSpline::IsCycled()
{
	if (m_pContour->nContourSize < 3)
		return false;

	CPoint	point1;
	CPoint	point2;

	point1.x = m_pContour->ppoints[m_pContour->nContourSize - 1].x;
	point2.x = m_pContour->ppoints[0].x;
	point1.y = m_pContour->ppoints[m_pContour->nContourSize - 1].y;
	point2.y = m_pContour->ppoints[0].y;

	point1 = ::ConvertToWindow(m_punkTarget, point1);
	point2 = ::ConvertToWindow(m_punkTarget, point2);

	return ::abs(point1.x-point2.x) < 5 && abs(point1.y-point2.y) < 5;

}

void CActionMeasSpline::DoDraw(CDC &dc)
{
	_RestrictDraw rd(dc, m_punkTarget);
	dc.SelectStockObject(NULL_BRUSH);

/*	if (IsCycled())
	{
		m_pContour->nContourSize--;
		::ContourDraw(dc, m_pContour, m_punkTarget, cdfSpline|cdfClosed|cdfFill);
		m_pContour->nContourSize++;
	}
	else
*/	{
		::ContourDraw(dc, m_pContour, m_punkTarget, cdfSpline);
	}
	
	for (int nPoint = 0; nPoint < m_pContour->nContourSize; nPoint++)
	{
		CPoint	point;
		point.x = m_pContour->ppoints[nPoint].x;
		point.y = m_pContour->ppoints[nPoint].y;
		point = ::ConvertToWindow( m_punkTarget, point );

		DrawCross(&dc, point);
	}

	dc.SetROP2(R2_XORPEN);
	
}

bool CActionMeasSpline::DoLButtonDown(CPoint pt)
{
	_Draw();
/*
	if (IsCycled())
	{
		m_pContour->nContourSize--;

		_Draw();
		Finalize();
		return true;
	}
*/
	if (!m_pContour->nContourSize)
		::ContourAddPoint(m_pContour, pt.x, pt.y);

	::ContourAddPoint(m_pContour, pt.x, pt.y);
	
	_Draw();
	return true;
}

bool CActionMeasSpline::DoLButtonDblClick(CPoint pt)
{
	_Draw();

	if (m_pContour && m_pContour->nContourSize > 0)
		m_pContour->nContourSize--;

	_Draw();

	Finalize();

	return true;
}

bool CActionMeasSpline::DoSetCursor(CPoint point)
{
//	if (!IsCycled())
		return CMMActionObjectBase::DoSetCursor(point);
//	else
//		SetCursor(m_hCursourCycled);
	return true;
}

bool CActionMeasSpline::DoMouseMove(CPoint pt)
{
	if (m_pContour->nContourSize == 0 ||
		(m_pContour->ppoints[m_pContour->nContourSize - 1].x == pt.x &&
		  m_pContour->ppoints[m_pContour->nContourSize - 1].y == pt.y))
		  return true;
	_Draw();

	if (m_pContour->nContourSize > 0)
	{
		m_pContour->ppoints[m_pContour->nContourSize - 1].x = pt.x;
		m_pContour->ppoints[m_pContour->nContourSize - 1].y = pt.y;
	}

	_Draw();

	CalcValue();
	return true;
}

void CActionMeasSpline::Finalize()
{
	CMMActionObjectBase::Finalize();
}
