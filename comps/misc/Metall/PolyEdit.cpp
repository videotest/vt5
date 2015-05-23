#include "StdAfx.h"
#include "polyedit.h"
#include "misc_utils.h"
#include "nameconsts.h"
#include "math.h"
#include "alloc.h"
#include "docview5.h"
#include "limits.h"

_ainfo_base::arg CPolyEditInfo::s_pargs[] =
{
	{"Img",         szArgumentTypeImage, 0, true, true },
	{0, 0, 0, false, false },
};

IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName)
{
	_bstr_t strName(szName);
	_bstr_t strCompName;

	if (CheckInterface(punkThis, iid))
	{
		strCompName = ::GetName(punkThis);
		if (strCompName == strName)
		{
			punkThis->AddRef();
			return punkThis;
		}
	}

	IUnknown *punk = 0;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);

			if (punk)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
				punk->Release();
				punk = 0;
			}
		}
	}

	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknown	*punkParent = 0;

		sptrR->GetParent(&punkParent);

		punk = FindComponentByName(punkParent, iid, strName);

		if (punkParent)
			punkParent->Release();
	}

	return punk;
}

void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	if( !pGroupManager )
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
		if( !punk )return;
		ICompManagerPtr	ptrManager( punk );
		punk->Release();
		pGroupManager = ptrManager;
	}

	IDataObjectListPtr ptrObjectList;

	if( bSetParams )
	{
		INamedDataObject2Ptr ptrObject(punkCalc);
		IUnknown* punkParent = 0;
		ptrObject->GetParent(&punkParent);
		ptrObjectList = punkParent;
		if( punkParent )punkParent->Release();
		if( ptrObjectList == 0 )return;
	}

	IUnknownPtr sptr2;
	if(punkImage==0)
	{
		IMeasureObjectPtr ptrObject(punkCalc);
		if(ptrObject!=0)
		{
			ptrObject->GetImage(&sptr2);
			punkImage=sptr2;
		}
	}

	int	nCount;
	pGroupManager->GetCount( &nCount );
	for( int idx = 0; idx < nCount; idx++ )
	{
		IUnknown	*punk = 0;
		pGroupManager->GetComponentUnknownByIdx( idx, &punk );
		IMeasParamGroupPtr	ptrG( punk );
		if( punk )punk->Release();
		if( ptrG == 0 )continue;

		if( bSetParams )
			ptrG->InitializeCalculation( ptrObjectList );
		ptrG->CalcValues( punkCalc, punkImage );
		if( bSetParams )
			ptrG->FinalizeCalculation();
	}

	if( bSetParams )
		ptrObjectList->UpdateObject( punkCalc );
}

void ConditionalCalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if(bRecalc) CalcObject(punkCalc, punkImage, bSetParams, pGroupManager );
}

class CPointAndAngle
{
public:
	double x;
	double y;
	int index;
	double angle;
} POINTANDANGLE;

static int comparePointAndAngle( const void *arg1, const void *arg2 )
{
	double da = 
		((CPointAndAngle*)(arg1))->angle -
        ((CPointAndAngle*)(arg2))->angle;
	if(da<0) return -1;
	else if(da>0) return 1;
	else return 0;
}

static void SortPoints(POINT *pPoints, int nPoints)
{
	smart_alloc(tmp, CPointAndAngle, nPoints);
	double x0=0, y0=0;
    for(int i=1; i<nPoints; i++)
	{
		x0 += pPoints[i].x;
		y0 += pPoints[i].y;
	}
	if(nPoints>1)
	{
		x0 /= (nPoints-1);
		y0 /= (nPoints-1);
	}
	else
	{
		x0 = pPoints[0].x;
		y0 = pPoints[0].y;
	}

    for(int i=0; i<nPoints; i++)
	{
		tmp[i].x = pPoints[i].x - x0;
		tmp[i].y = pPoints[i].y - y0;
		tmp[i].angle = atan2(tmp[i].y,tmp[i].x);
		tmp[i].index = i;
	}
    for(int i=1; i<nPoints; i++)
	{
		if( tmp[i].angle <= tmp[0].angle )
			tmp[i].angle += 2*PI; // чтобы нулевая точка осталась нулевой
	}
	qsort(tmp, nPoints, sizeof(CPointAndAngle), comparePointAndAngle);

    for(int i=0; i<nPoints; i++)
	{
		pPoints[i].x = int(x0 + tmp[i].x + 0.5);
		pPoints[i].y = int(y0 + tmp[i].y + 0.5);
	}
}

static void MakeRegularPoly(POINT *pPoints, int nPoints)
{ // на основании первой точки (вершина) и нулевой (конец диаметра) построить правильный многоугольник
	double x0 = (pPoints[0].x+pPoints[1].x)*0.5;
	double y0 = (pPoints[0].y+pPoints[1].y)*0.5;
	double dx = pPoints[1].x - x0;
	double dy = pPoints[1].y - y0;
	if(nPoints % 2)
	{
		double co = cos(PI/nPoints/2);
		double si = sin(PI/nPoints/2);
		double tmp = dx - dy*si/co;
		dy = dx*si/co + dy;
		dx = tmp;
		x0 = pPoints[1].x - dx;
		y0 = pPoints[1].y - dy;
	}

	double co = cos(2*PI/nPoints);
	double si = sin(2*PI/nPoints);
	for(int i=0; i<nPoints; i++)
	{
		int j = (i+1) % nPoints;
		pPoints[j].x = int(x0 + dx + 0.5);
		pPoints[j].y = int(y0 + dy + 0.5);
		double tmp = dx*co+dy*si;
		dy = -dx*si + dy*co;
		dx = tmp;
	}
}

static void FillPoly(byte **dst, int cx, int cy, POINT *pPoints, int n, byte color)
{
	smart_alloc(x0, int, cy);
	for(int i=0; i<cy; i++) x0[i]=-1;

	int lastDir=0;
	int x1=pPoints[n-1].x, y1=pPoints[n-1].y;
	for(i=0; i<n; i++)
	{
		int xx1=x1*65536, dxx;
		if(y1!=pPoints[i].y) dxx=(pPoints[i].x-x1)*65536/abs(pPoints[i].y-y1);
		while(y1!=pPoints[i].y)
		{
			x1=xx1/65536;
			int dy= pPoints[i].y>y1 ? 1 : -1;
			if(y1>=0 && y1<cy)
			{
				if(x0[y1]!=-1)
				{
					int xx1=min(x1,x0[y1]);
					int xx2=max(x1,x0[y1]);
					for(int fillx=xx1; fillx<=xx2; fillx++)
					{
						if(fillx>=0 && fillx<cx)
						{
							dst[y1][fillx] ^= color;
						}
					}
					x0[y1]=-1;
				}
				else x0[y1]=x1;
				if(dy!=lastDir && lastDir!=0)
				{
					if(x0[y1]==-1) x0[y1]=x1;
					else x0[y1]=-1;
				}
			}
			lastDir=dy;
			y1+=dy;
			xx1+=dxx;
		}
		x1=pPoints[i].x;
	}
}

CPolyEdit::CPolyEdit(void)
{
	m_ptrImage=0;
	m_ptrList=0;
	m_nPoints=0;
	m_nMaxPoints=4;
	m_clLineColor1=0x00FF00; // цвет линии - основной
	m_clLineColor2=0xFF0000; // цвет линии - текущий отрезок
	m_nLineWidth=1; // толщина линии
}

CPolyEdit::~CPolyEdit(void)
{
}

IUnknown* CPolyEdit::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

HRESULT CPolyEdit::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CPolyEdit::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );

	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	if(!punkObj) return false;
	if(punkObj) punkObj->Release();
	
	return true;
}

bool CPolyEdit::DoLButtonDown( _point point )
{
	if(!IsAvaible()) return(false);

	if(m_ptrImage==0) return false;

	int w,h;
	m_ptrImage->GetSize(&w,&h);
	if(point.x<0 || point.x>=w || point.y<0 || point.y>=h) return true; //за пределами image - ничего не делаем

	if(m_nPoints==0)
	{
		m_nPoints++;
		m_Points[0] = point;
	}
	m_Points[0] = point;
	if(m_bRegularPoly)
	{
		if(m_nPoints>=2)
		{
			m_nPoints = m_nMaxPoints;
			MakeRegularPoly(m_Points, m_nPoints);
		}
	}
	else SortPoints(m_Points, m_nPoints);
	if(m_nPoints==m_nMaxPoints) Finalize();
	else 
	{
		m_Points[m_nPoints] = m_Points[0];
		m_nPoints++;
		_rect rect;
		GetRect(&rect);
		::InvalidateRect(m_hwnd, &rect, false);
	}
	return true;
}

bool CPolyEdit::DoFinishTracking( _point point )
{
	if(m_bRegularPoly)
	{
		m_Points[0] = point;
		if(m_nPoints>=2)
		{
			m_nPoints = m_nMaxPoints;
			MakeRegularPoly(m_Points, m_nPoints);
		}
		if(m_nPoints==m_nMaxPoints) Finalize();
	}
	return true;
}

bool CPolyEdit::DoLButtonUp( _point point )
{
	return true;
}

bool CPolyEdit::DoTrack( _point point )
{
	if(m_nPoints>0)
	{
		_rect rect1;
		GetRect(&rect1);
		::InvalidateRect(m_hwnd, &rect1, false);
		m_Points[0]=point;
		if(m_bRegularPoly)
		{
			if(m_nPoints>=2)
			{
				m_nPoints = m_nMaxPoints;
				MakeRegularPoly(m_Points, m_nPoints);
			}
		}
		else SortPoints(m_Points, m_nPoints);
		_rect rect2;
		GetRect(&rect2);
		::InvalidateRect(m_hwnd, &rect2, false);
	}
	return true;
}

bool CPolyEdit::DoMouseMove( _point point )
{
	if(m_nPoints>0) DoTrack(point);
	return __super::DoMouseMove( point );
}

void CPolyEdit::EnsureVisible( _point point )
{
	IScrollZoomSitePtr ptrScrollZoom(m_ptrTarget);
	if(ptrScrollZoom==0) return;
	ptrScrollZoom->EnsureVisible(_rect(point,_size(1,1)));
}

LRESULT	CPolyEdit::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	LRESULT bResult = false;

	if( nMsg == WM_SETCURSOR )
	{
		_point pt;
		::GetCursorPos( &pt );
		::ScreenToClient( m_hwnd, &pt );
		pt = ::_client( pt, m_ptrSite );
		int w=0,h=0;
		if(m_ptrImage!=0) m_ptrImage->GetSize(&w,&h);
		_point ptOffset(0,0);
		if(m_ptrImage!=0) m_ptrImage->GetOffset(&ptOffset);
		_rect rc(ptOffset, _size(w,h));
		if(	pt.x>=rc.left && pt.x<rc.right && pt.y>=rc.top && pt.y<rc.bottom)
			::SetCursor(m_hCurActive);
		else
			::SetCursor(m_hCurStop);
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

bool CPolyEdit::Initialize()
{
	if(!CInteractiveAction::Initialize()) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	if(sptrContext==0) return false;

	IUnknown* punkImage = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkImage);
	m_ptrImage = punkImage;
	if(punkImage) punkImage->Release();
	if(m_ptrImage==0) return false;

	IUnknown* punkList = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkList);
	m_ptrList = punkList;
	if(punkList) punkList->Release();

	::InvalidateRect(m_hwnd, 0, false);

	m_nMaxPoints = GetValueInt(GetAppUnknown(), "\\Metall\\PolyEdit", "MaxPoints", 4);
	m_nMaxPoints = min(19,max(3, m_nMaxPoints));
	m_bRegularPoly = GetValueInt(GetAppUnknown(), "\\Metall\\PolyEdit", "RegularPoly", 0) != 0;

	m_clLineColor1 = GetValueColor(GetAppUnknown(), "\\Metall\\PolyEdit", "LineColor", RGB( 255, 0, 0 ));
	m_clLineColor2 = GetValueColor(GetAppUnknown(), "\\Metall\\PolyEdit", "LineColor", RGB( 255, 0, 0 ));
	m_nLineWidth = GetValueInt(GetAppUnknown(), "\\Metall\\PolyEdit", "LineWidth", 1);

	static int cursors[] =
	{ IDC_POLY, IDC_POLY1, IDC_POLY2, IDC_POLY3, IDC_POLY4, IDC_POLY5, IDC_POLY6 };
	long nCursorNum = GetValueInt(GetAppUnknown(), "\\Metall\\PolyEdit", "Cursor", 0);
    if(nCursorNum<0 || nCursorNum>6) nCursorNum=0;
	m_hCurActive = ::LoadCursor(App::handle(), MAKEINTRESOURCE(cursors[nCursorNum]));
	m_hCurStop = ::LoadCursor(0, MAKEINTRESOURCE(IDC_NO));

	return true;
}

bool CPolyEdit::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	
	if( !CInteractiveAction::Finalize() )
		return false;
	
	return true;
}

HRESULT CPolyEdit::Paint( HDC hDC, RECT rectDraw,
							IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache,
							BYTE *pdibCache )
{
	IScrollZoomSitePtr ptrScrollZoom(punkTarget);
	if(ptrScrollZoom == 0) return E_FAIL;

	double fZoom = 1.;
	_point ptScroll(0, 0);
	ptrScrollZoom->GetZoom(&fZoom);
	ptrScrollZoom->GetScrollPos(&ptScroll);

	HPEN hPen1 = ::CreatePen( PS_SOLID, m_nLineWidth, m_clLineColor1 );
	HPEN hPen2 = ::CreatePen( PS_SOLID, m_nLineWidth, m_clLineColor2 );
	
	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen1);
	int old_rop2=::GetROP2(hDC);

	::SetROP2(hDC,R2_COPYPEN);

	if(m_nPoints)
	{
		_point points[20];
		for(int i=0; i<m_nPoints; i++)
			points[i] = _window(m_Points[i], ptrScrollZoom);
		::Polyline(hDC, points+1, m_nPoints-1);

		::SelectObject(hDC, hPen2);
		::MoveToEx(hDC, points[m_nPoints-1].x, points[m_nPoints-1].y, 0);
		::LineTo(hDC, points[0].x, points[0].y);
		::LineTo(hDC, points[1].x, points[1].y);
	}

	::SetROP2(hDC,old_rop2);
	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen1);
	::DeleteObject(hPen2);
	
	return S_OK;
}

HRESULT CPolyEdit::GetRect( RECT *prect )
{
	if(!prect) return E_INVALIDARG;
	if(m_nPoints==0)
	{
		*prect = NORECT;
		return S_OK;
	}
	_rect rect(m_Points[0],_size(0,0));
	for(int i=1; i<m_nPoints; i++)
	{
		rect.left = min(rect.left, m_Points[i].x);
		rect.right = max(rect.right, m_Points[i].x);
		rect.top = min(rect.top, m_Points[i].y);
		rect.bottom = max(rect.bottom, m_Points[i].y);
	}
	rect.inflate(1);
	IScrollZoomSitePtr      ptrScrollZoom(m_ptrTarget);
	rect = _window(rect, ptrScrollZoom);
	rect.inflate(m_nLineWidth/2+1);
	*prect = rect;
	return S_OK;
}

IUnknownPtr CPolyEdit::get_document()
{
	IViewPtr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return 0;
	
	IUnknownPtr ptrDoc;
	{
		IUnknown* punk = 0;
		ptrView->GetDocument( &punk );
		if( !punk       )
			return 0;
		
		ptrDoc = punk;
		punk->Release();        punk = 0;
	}
	
	return ptrDoc;
}

HRESULT CPolyEdit::DoInvoke()
{
	IUnknownPtr ptrDoc = get_document();
	OnAttachTarget( ptrDoc );

	if(m_ptrImage==0) return E_FAIL;
	int cx=0,cy=0;
	m_ptrImage->GetSize(&cx, &cy);

	_rect rect(m_Points[0],_size(0,0));
	for(int i=1; i<m_nPoints; i++)
	{
		rect.left = min(rect.left, m_Points[i].x);
		rect.right = max(rect.right, m_Points[i].x);
		rect.top = min(rect.top, m_Points[i].y);
		rect.bottom = max(rect.bottom, m_Points[i].y);
	}
	rect.right++;
	rect.bottom++;
	{
		_rect rect1(0,0,cx,cy);
		_rect rect2;
		::IntersectRect(&rect2, &rect, &rect1);
		rect = rect2;
	}

	//-------------------- image
	IUnknown        *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr      ptrI(punk2);
	if(punk2!=0) punk2->Release();
	INamedDataObject2Ptr ptrImg(ptrI);
	ptrI->CreateVirtual( rect );
	ptrI->InitRowMasks();
	ptrImg->SetParent( m_ptrImage,  0);

	for(int i=0; i<m_nPoints; i++)
		m_Points[i] -= rect.top_left(); // приведем точки к новому изображению

	smart_alloc(pm,byte*,rect.height());

	for(int y=0; y<rect.height(); y++)
	{
		ptrI->GetRowMask(y, pm+y);
		ZeroMemory(pm[y], rect.width()*sizeof(byte));
	}

	FillPoly(pm, rect.width(),rect.height(), m_Points, m_nPoints, 255);
	ptrI->InitContours();

	//-------------------- object
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr ptrO(punk);
	if(punk!=0) punk->Release();

	ptrO->SetImage( ptrI );
	INamedDataObject2Ptr ptrObject(ptrO);

	SetToDocData(ptrDoc, ptrO);
	::ConditionalCalcObject(ptrO, 0, true, 0 );

	if(m_ptrList==0)
	{
		INamedDataObject2Ptr ptrSrcImageNamed = INamedDataObject2Ptr(m_ptrImage);

		IUnknown *punk;
		INamedDataObject2Ptr ptrObj(ptrO);
		ptrObj->GetParent(&punk);
		INamedDataObject2Ptr ptrList(punk);
		if(punk!=0) punk->Release(); punk=0;

		if(ptrSrcImageNamed!=0 && ptrList!=0)
		{
			GUID guidNewBase;
			ptrSrcImageNamed->GetBaseKey(&guidNewBase);
			ptrList->SetBaseKey(&guidNewBase);
		}
		m_ptrList=ptrList;
	}

	{
		IUnknown *punk;
		INamedDataObject2Ptr ptrObj(ptrO);
		ptrObj->GetParent(&punk);
		INamedDataObject2Ptr ptrList(punk);
		if(punk!=0) punk->Release(); punk=0;
		IDataContextPtr sptrContext( m_ptrTarget );
		if(sptrContext!=0)
		{
			sptrContext->SetActiveObject(_bstr_t(szTypeObjectList), ptrList, aofActivateDepended);
		}
	}

	return S_OK;
}
