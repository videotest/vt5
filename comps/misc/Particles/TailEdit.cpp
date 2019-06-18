// при таскании первой точки - направление от центра
// точки на длинных отрезках
#include "stdafx.h"
#include "tailedit.h"
#include "misc_utils.h"
#include <math.h>
#include <limits.h>
#include "measure5.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "alloc.h"
#include "aam_utils.h"

#include "docview5.h"
#include "Chromosome.h"
#include "core5.h"
#include "misc_utils.h"

#include "SpermSegm.h"

#define MaxColor color(-1)

_ainfo_base::arg	CTailEditInfo::s_pargs[] = 
{	
//	{"Img",		szArgumentTypeImage, 0, true, true },
//	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

CTailEdit::CTailEdit(void)
{
	m_nObjNum=-1; // нет выбранного объекта
	m_pOldData=0;
	m_bActionExecuted=false;
}

CTailEdit::~CTailEdit(void)
{
	if(m_pOldData!=0) delete[] m_pOldData;
}

HRESULT CTailEdit::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~(afEnabled | afChecked);
	return(r);
}

bool CTailEdit::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );

	IUnknownPtr sptr=0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &sptr);
	if(sptr==0) return false;
	
	sptr=0;
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &sptr);
	if(sptr==0) return false;

	_variant_t var = ::GetValue(sptr, "\\particles", "data", _variant_t(long(0)));
	sptr=0;

	if(var.vt!=VT_BITS || var.lVal==0) return false;

	
	return true;
}

IUnknown* CTailEdit::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

bool CTailEdit::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;

	IDataContextPtr	sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	m_ptrImage = punkObj;
	if(punkObj) punkObj->Release();

	IUnknown* punkObj1 = 0;	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj1);
	m_ptrList = INamedDataObject2Ptr (punkObj1);
	if(punkObj1) punkObj1->Release();
	if(m_ptrList!=0)
	{
		m_ptrList->SetActiveChild(0);
	}

	AttachArrays(m_ptrList);

	m_xTotal = GetExistingArray("x"); //координаты
	m_yTotal = GetExistingArray("y");
	m_exist = GetExistingArray("exist"); //существует ли на соответствующем кадре
	m_pLineIntensity = GetExistingArray("intensity"); // отличие по цвету от фона
	m_pLineWidth = GetExistingArray("width"); // ширина

	if(m_xTotal==0 || m_yTotal==0 || m_exist==0) return false; //на всякий пожарный

	m_nObjectStep=GetObjectStep();
	m_nFrameStep=GetFrameStep();
	GetArraySize(&m_nFrames, 0, 0);
	//m_nFrames=::GetValueInt(m_ptrList, "\\particles", "FrameNo", 0);

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSOREDITTAIL));
	m_hCurShiftActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSOREDITTAILSHIFT));
	m_hCurStop = ::LoadCursor(0, MAKEINTRESOURCE(IDC_NO));

	m_bActionExecuted=false;
	m_nOldMaxFrameNo = ::GetValueInt(m_ptrList, "\\particles", "MaxFrameNo", 0);
	m_nOldMaxObjectNo = ::GetValueInt(m_ptrList, "\\particles", "MaxObjectNo", 0);
	m_nOldMaxItemSize = ::GetValueInt(m_ptrList, "\\particles", "MaxItemSize", 0);
	_variant_t var = ::GetValue(m_ptrList, "\\particles", "data", _variant_t(long(0)));
	if(var.vt==VT_BITS && var.lVal!=0)
	{
		byte *pData = (byte *)var.lVal;
		long lSize = *(long*)pData; // m_nOldMaxFrameNo*m_nOldMaxObjectNo*m_nOldMaxItemSize;
		m_pOldData = new byte[sizeof( lSize )+lSize];
		memcpy(m_pOldData, pData, sizeof( lSize )+lSize);
	}

	return (m_ptrImage!=0) && (m_ptrList!=0);
}

void CTailEdit::DeInit()
{
	::InvalidateRect(m_hwnd, 0, false);
}

bool CTailEdit::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	if( !CInteractiveAction::Finalize() )
		return false;

	return true;
}

bool CTailEdit::DoStartTracking( _point point )
{
	return true;
}

bool CTailEdit::DoTrack( _point point )
{
	IScrollZoomSitePtr	ptrScrollZoom(m_ptrTarget);
	if(ptrScrollZoom==0) return false;

	long nThreshold = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "Threshold", 400 );
	double fMitoThreshold = ::GetValueDouble( GetAppUnknown(), "\\SegmLines", "MitoThreshold", 0.5 );
	//double fAngleTolerance = ::GetValueDouble( GetAppUnknown(), "\\SegmLines", "AngleTolerance", 20 );
	//fAngleTolerance = max(1,fAngleTolerance);

	int w=0,h=0;
	m_ptrImage->GetSize(&w,&h);
	_point ptOffset(0,0);
	m_ptrImage->GetOffset(&ptOffset);
	_rect rc(ptOffset, _size(w,h));
	if(	point.x>=rc.left && point.x<rc.right && point.y>=rc.top && point.y<rc.bottom)
	{
		::SetCursor(m_hCurActive);
	}
	else
	{
		::SetCursor(m_hCurStop);
		return false;
	}
	int nPane=1;
	smart_alloc(srcRows, color *, h);
	for(int y=0; y<h; y++)
	{
		m_ptrImage->GetRow(y, nPane, &srcRows[y]);
	}


	if(m_nObjNum>=0)
	{
		int posT = m_nFrameStep*m_nFrame + m_nObjectStep*m_nObjNum;
		_rect rc(
			m_xTotal[posT], m_yTotal[posT],
			m_xTotal[posT], m_yTotal[posT] );

		m_xTotal[posT]=point.x;
		m_yTotal[posT]=point.y;

		for(int nFrame1=m_nFrame+1; nFrame1<m_nFrames; nFrame1++)
		{
			int posT1 = m_nFrameStep*nFrame1 + m_nObjectStep*m_nObjNum;
			m_exist[posT1] = 0; // остаток хвоста убьем
		}

		CLineTracer tracer;
		//tracer.m_fAngleTolerance = fAngleTolerance;
		tracer.m_cx=w;
		tracer.m_cy=h;
		tracer.m_srcRows = srcRows;
		tracer.m_stopRows = 0;
		tracer.m_binRows = 0;
		tracer.LoadData(
			m_xTotal, m_yTotal, m_exist,
			m_pLineIntensity, m_pLineWidth,
			m_nObjectStep*m_nObjNum, m_nFrame+1, m_nFrameStep);

		if(m_nFrame==0)
		{	// начало линии - надо выставить угол
			IMeasureObjectPtr ptrO(m_ptrObject);
			if(ptrO!=0)
			{
				IUnknownPtr ptr2;
				ptrO->GetImage(&ptr2);
				IImage3Ptr ptrI(ptr2);
				if(ptrI!=0)
				{
					_point ptCenter(0,0);
					GetCenter(ptrI, &ptCenter);
					double a=atan2(
						double(m_yTotal[posT]-ptCenter.y),
						double(m_xTotal[posT]-ptCenter.x));
					a+=PI*2;
					tracer.m_pangle[0] = int(a*256/2/PI+0.5) & 255;
				}
			}
		}

		int cBest=0;
		double fdx =  tracer.m_si[ tracer.m_pangle[m_nFrame] ];
		double fdy = -tracer.m_co[ tracer.m_pangle[m_nFrame] ];
		if(m_nFrame==0)
		{ // Для начала хвоста - практически не даем корректировать координаты
			fdx /= 8;
			fdy /= 8;
		}
		for(int dn=-3; dn<=3; dn++)
		{
			int x = int(point.x + dn*fdx + 0.5);
			int y = int(point.y + dn*fdy + 0.5);
			int c = tracer.TestLine(x,y,tracer.m_pangle[m_nFrame]);
			if(c<cBest/2)
			{	// если точка выглядит хорошей - уточним еще две
				int tmp = tracer.m_nStepsBack;
				tracer.m_nStepsBack = min(1,tmp);
				c += tracer.TraceStep(m_nFrame+1);
				tracer.m_nStepsBack = min(2,tmp);
				c += tracer.TraceStep(m_nFrame+2);
				tracer.m_nStepsBack = tmp;
				c /= 3;
			}
			if(c<cBest)
			{
				cBest=c;
				tracer.m_px[m_nFrame] = x;
				tracer.m_py[m_nFrame] = y;
			}
		}

		if(m_nFrame==0)
		{	// начало линии - надо откорректировать, чтобы попало на границу
			IMeasureObjectPtr ptrO(m_ptrObject);
			if(ptrO!=0)
			{
				IUnknownPtr ptr2;
				ptrO->GetImage(&ptr2);
				IImage3Ptr ptrI(ptr2);
				if(ptrI!=0)
				{
					_point ptCenter(0,0);
					GetCenter(ptrI, &ptCenter);
					_point ptEdge(tracer.m_px[m_nFrame],tracer.m_py[m_nFrame]);
					CorrectEdge(ptrI, &ptCenter, &ptEdge);
					double r=_hypot(ptEdge.x-ptCenter.x,ptEdge.y-ptCenter.y);
					tracer.m_px[m_nFrame] = ptEdge.x;
					tracer.m_py[m_nFrame] = ptEdge.y;
				}
			}
		}


		/*
		int tmp = tracer.m_nStepsBack;
		tracer.m_nStepsBack = 1;
		double tmp2 = tracer.m_fAngleTolerance;
		tracer.m_fAngleTolerance = 1;
		if(m_nFrame>0)
		{
			double a=atan2(
				m_yTotal[posT]-m_yTotal[posT-m_nFrameStep],
				m_xTotal[posT]-m_xTotal[posT-m_nFrameStep]);
			a+=PI*2;
			tracer.m_pangle[m_nFrame]=int(a*256/2/PI+0.5) & 255;
		}
		int cBest = tracer.TraceStep(m_nFrame);
		tracer.m_nStepsBack = tmp;
		tracer.m_fAngleTolerance=tmp2;
		*/

		DWORD dwTime1 = GetTickCount();
		for(int i=m_nFrame+1; i<m_nFrames; i++)
		{
			int tmp = tracer.m_nStepsBack;
			tracer.m_nStepsBack = min(i-m_nFrame,tmp);
			int cBest = tracer.TraceStep(i);
			tracer.m_nStepsBack = tmp;
			if(cBest>-nThreshold && i>1) break; // ибо нефиг
			// но хоть один шаг от головы всегда сделаем
			if(GetTickCount()-dwTime1>100)
			{
				int nSteps = i;
				tracer.StoreData(
					m_xTotal, m_yTotal, m_exist,
					m_pLineIntensity, m_pLineWidth,
					m_nObjectStep*m_nObjNum, nSteps, m_nFrameStep);
				for(int j=m_nFrame; j<nSteps; j++)
				{
					int posT1 = m_nFrameStep*j + m_nObjectStep*m_nObjNum;
					if(m_exist[posT1])
					{
						float w = m_pLineWidth[posT1];
						if(w<0) w = -(w+1);
						m_pLineWidth[posT1] = -1 - w; // за пределами средней части
					}
				}
				::InvalidateRect(m_hwnd, 0, false);
				::UpdateWindow(m_hwnd);
				MSG	msg;
				if( ::PeekMessage( &msg, 0, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE | PM_QS_INPUT ) )
				{
					break;
				}
				dwTime1 = GetTickCount();
			}
		}
		int nFrame2 = tracer.ExpandStep(m_nFrame);
		int nSteps = i + nFrame2 - m_nFrame;

		if(nSteps > m_nFrames-5)
		{
			SetArraySize(nSteps+15,-1,-1);
			m_xTotal = GetExistingArray("x"); //координаты
			m_yTotal = GetExistingArray("y");
			m_exist = GetExistingArray("exist"); //существует ли на соответствующем кадре
			m_pLineIntensity = GetExistingArray("intensity"); // отличие по цвету от фона
			m_pLineWidth = GetExistingArray("width"); // ширина

			if(m_xTotal==0 || m_yTotal==0 || m_exist==0) return false; //на всякий пожарный

			m_nObjectStep=GetObjectStep();
			m_nFrameStep=GetFrameStep();
			GetArraySize(&m_nFrames, 0, 0);
		}

		tracer.StoreData(
			m_xTotal, m_yTotal, m_exist,
			m_pLineIntensity, m_pLineWidth,
			m_nObjectStep*m_nObjNum, nSteps, m_nFrameStep);
		for(int j=m_nFrame; j<nSteps; j++)
		{
			int posT1 = m_nFrameStep*j + m_nObjectStep*m_nObjNum;
			if(m_exist[posT1])
			{
				float w = m_pLineWidth[posT1];
				if(w<0) w = -(w+1);
				m_pLineWidth[posT1] = -1 - w; // за пределами средней части
			}
		}

		rc.merge( _rect(
			m_xTotal[posT], m_yTotal[posT],
			m_xTotal[posT], m_yTotal[posT] ) );

		int posT1=posT-m_nFrameStep;
		if(m_nFrame<=0) posT1=posT;
		if(!m_exist[posT1]) posT1=posT;

		rc.merge( _rect(
			m_xTotal[posT1], m_yTotal[posT1],
			m_xTotal[posT1], m_yTotal[posT1] ) );

		_rect rc2 = _window(rc, ptrScrollZoom);

		rc2.inflate(42,42);

		::InvalidateRect(m_hwnd, &rc2, false);
		::InvalidateRect(m_hwnd, 0, false);
	}
	return true;
}

bool CTailEdit::DoFinishTracking( _point point )
{
	m_bActionExecuted=true;
	Finalize();
	return true;
}

bool CTailEdit::DoLButtonDblClick( _point point )
{
	return false;
}

bool CTailEdit::DoRButtonDown( _point point )
{
	return false; //не давать гадам обрабатывать
}

bool CTailEdit::DoRButtonUp( _point point )
{
	return false;
}

static IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	long lPos = 0;
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

bool CTailEdit::DoLButtonUp( _point point )
{
	m_bActionExecuted=true;
	Finalize();
	return true;
}

bool CTailEdit::DoLButtonDown( _point point )
{
	if(!IsAvaible()) return(false);

	if(m_ptrImage==0 || m_ptrList==0) return false;
	int w,h;
	m_ptrImage->GetSize(&w,&h);
	if(point.x<0 || point.x>=w || point.y<0 || point.y>=h) return true; //за пределами image - ничего не делаем

	double bestDistance=1e20; //наиболее подходящий объект
	long bestObjPos = 0;
	long bestObjNum = -1;
	int best_x, best_y;
	int best_nFrame=0;

	long objPos = 0;
	m_ptrList->GetFirstChildPosition(&objPos);
	while( objPos ) //по всем объектам
	{
		IUnknownPtr ptr;
		long objPos1=objPos;
		m_ptrList->GetNextChild(&objPos, &ptr);
		IMeasureObjectPtr ptrO(ptr);
		if(ptrO==0) continue;

		// Find axis
		IUnknownPtr ptr3 = find_child_by_interface(ptrO, IID_ITrajectory);
		ITrajectoryPtr ptrT(ptr3);
		if(ptrT==0) continue;

		IUnknownPtr ptr2;
		ptrO->GetImage(&ptr2);
		IImage3Ptr ptrI(ptr2);
		if(ptrI==0) continue;

		_point ptOffset;
		ptrI->GetOffset(&ptOffset);
		int cx,cy;
		ptrI->GetSize(&cx,&cy);

		//if(point.x<ptOffset.x-20) continue; //скипаем заведомо ненужные объекты
		//if(point.y<ptOffset.y-20) continue;
		//if(point.x>ptOffset.x+cx+20) continue;
		//if(point.y>ptOffset.y+cy+20) continue;

		long nObjNum=0;
		if(ptrT->GetObjNum(&nObjNum)!=S_OK) continue;

		for(int nFrame=0; nFrame<m_nFrames; nFrame++)
		{
			int posT = m_nFrameStep*nFrame + m_nObjectStep*nObjNum;
			byte bFrameExist = m_exist[posT]; // текущий кадр задан
			if(bFrameExist)
			{
				double r = _hypot(point.x-m_xTotal[posT], point.y-m_yTotal[posT]);
				if(r<bestDistance)
				{
					bestDistance = r;
					bestObjPos = objPos1;
					bestObjNum = nObjNum;
					best_x = m_xTotal[posT];
					best_y = m_yTotal[posT];
					best_nFrame = nFrame;
				}
			}
		}
	}

	if(bestDistance<10)   // нашли объект, не дальше 10 пикселей от точки
	{
		IUnknownPtr ptr;
		long objPos=bestObjPos;
		m_ptrList->GetNextChild(&objPos, &ptr);
		m_ptrObject = ptr;
		m_nObjNum = bestObjNum;
		m_nFrame = best_nFrame;

		if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
		{	// нажат shift - двигаем границу средней части
			for(int nFrame1=0; nFrame1<m_nFrames; nFrame1++)
			{
				int posT1 = m_nFrameStep*nFrame1 + m_nObjectStep*bestObjNum;
				if(m_exist[posT1])
				{
					float w = m_pLineWidth[posT1];
					if(w<0) w = -(w+1);
					if(nFrame1<=m_nFrame) m_pLineWidth[posT1] = w;
					else m_pLineWidth[posT1] = -1 - w;
				}
			}
		}
		else
		{	// без шифта - режем хвост, при таскании - трассировка хвоста
			if(best_nFrame!=0)
			{	// если щелкнули по первой точке - не режем.
				int posT = m_nFrameStep*best_nFrame + m_nObjectStep*bestObjNum;
				m_xTotal[posT]=point.x;
				m_yTotal[posT]=point.y;

				for(int nFrame1=m_nFrame+1; nFrame1<m_nFrames; nFrame1++)
				{
					int posT1 = m_nFrameStep*nFrame1 + m_nObjectStep*bestObjNum;
					m_exist[posT1] = 0; // остаток хвоста убьем
				}
			}
		}
	}

	::InvalidateRect(m_hwnd, 0, false);
	UpdateWindow(m_hwnd);
	return true;
}

bool CTailEdit::DoMouseMove( _point point )
{
	//return true; //не давать гадам обрабатывать
	return CInteractiveAction::DoMouseMove(point);
}

bool CTailEdit::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;

	if(m_ptrImage == 0)return false;

	return false;
}

LRESULT	CTailEdit::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	long bResult = false;

	if( nMsg == WM_KEYDOWN || nMsg == WM_KEYUP )
	{
		if(wParam==VK_SHIFT)
		{
			_point pt;
			::GetCursorPos( &pt );
			::ScreenToClient( m_hwnd, &pt );
			pt = ::_client( pt, m_ptrSite );
			int w=0,h=0;
			m_ptrImage->GetSize(&w,&h);
			_point ptOffset(0,0);
			m_ptrImage->GetOffset(&ptOffset);
			_rect rc(ptOffset, _size(w,h));
			if(	pt.x>=rc.left && pt.x<rc.right && pt.y>=rc.top && pt.y<rc.bottom)
			{
				if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
					::SetCursor(m_hCurShiftActive);
				else
					::SetCursor(m_hCurActive);
			}
			else
			{
				::SetCursor(m_hCurStop);
			}
			bResult=true;
		}
	}

	if( nMsg == WM_SETCURSOR )
	{
		_point pt;
		::GetCursorPos( &pt );
		::ScreenToClient( m_hwnd, &pt );
		pt = ::_client( pt, m_ptrSite );
		int w=0,h=0;
		m_ptrImage->GetSize(&w,&h);
		_point ptOffset(0,0);
		m_ptrImage->GetOffset(&ptOffset);
		_rect rc(ptOffset, _size(w,h));
		if(	pt.x>=rc.left && pt.x<rc.right && pt.y>=rc.top && pt.y<rc.bottom)
		{
			if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
				::SetCursor(m_hCurShiftActive);
			else
				::SetCursor(m_hCurActive);
		}
		else
		{
			::SetCursor(m_hCurStop);
		}
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

IUnknownPtr	CTailEdit::get_document()
{
	IViewPtr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return 0;

	IUnknownPtr ptrDoc;
	{
		IUnknown* punk = 0;
		ptrView->GetDocument( &punk );
		if( !punk	)
			return 0;

		ptrDoc = punk;		
		punk->Release();	punk = 0;
	}

	return ptrDoc;
}

HRESULT CTailEdit::DoInvoke()
{
	if(!IsAvaible()) return E_FAIL;

	IUnknownPtr ptrDoc = get_document();

	OnAttachTarget( ptrDoc );

	return S_OK;
}

HRESULT CTailEdit::Paint( HDC hDC, RECT rectDraw, 
						  IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, 
						  BYTE *pdibCache )
{
	IScrollZoomSitePtr	ptrScrollZoom(punkTarget);

	HPEN hPen = ::CreatePen( PS_SOLID, 0, 0x00FF00 );
	//::GetStockObject(WHITE_PEN)

	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen);

	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen);

	return S_OK;
}

HRESULT CTailEdit::GetRect( RECT *prect )
{
	if(prect)
	{
		*prect=_rect(INT_MIN,INT_MIN, INT_MAX,INT_MAX);
		return(NOERROR);
	}

	return E_INVALIDARG;
}

void CTailEdit::UndoRedo()
{
	int tmp_nOldMaxFrameNo = ::GetValueInt(m_ptrList, "\\particles", "MaxFrameNo", 0);
	int tmp_nOldMaxObjectNo = ::GetValueInt(m_ptrList, "\\particles", "MaxObjectNo", 0);
	int tmp_nOldMaxItemSize = ::GetValueInt(m_ptrList, "\\particles", "MaxItemSize", 0);
	_variant_t var = ::GetValue(m_ptrList, "\\particles", "data", _variant_t(long(0)));

	::SetValue(m_ptrList, "\\particles", "MaxFrameNo", long(m_nOldMaxFrameNo));
	::SetValue(m_ptrList, "\\particles", "MaxObjectNo", long(m_nOldMaxObjectNo));
	::SetValue(m_ptrList, "\\particles", "MaxItemSize", long(m_nOldMaxItemSize));
	_variant_t var1((long)m_pOldData);
	var1.vt = VT_BITS;
	::SetValue(m_ptrList, "\\particles", "data", var1);

	tmp_nOldMaxFrameNo = tmp_nOldMaxFrameNo;
	tmp_nOldMaxObjectNo = tmp_nOldMaxObjectNo ;
	tmp_nOldMaxItemSize = tmp_nOldMaxItemSize;
	m_pOldData = (byte *)var.lVal;
}

HRESULT CTailEdit::Undo()
{
	if(m_bActionExecuted) UndoRedo();
	if(m_ptrList!=0)
	{
		{
			IDataObjectListPtr ptrObjectList(m_ptrList);
			if(ptrObjectList!=0)
			{
				ptrObjectList->UpdateObject(0);
			}
		}
	}
	return S_OK;
}

HRESULT CTailEdit::Redo()
{
	if(m_bActionExecuted) UndoRedo();
	if(m_ptrList!=0)
	{
		{
			IDataObjectListPtr ptrObjectList(m_ptrList);
			if(ptrObjectList!=0)
			{
				ptrObjectList->UpdateObject(0);
			}
		}
	}
	return S_OK;
}

HRESULT CTailEdit::TerminateInteractive()
{
	if(!m_bActionExecuted) UndoRedo();
	return __super::TerminateInteractive();
}
