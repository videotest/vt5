#include "stdafx.h"
#include "resource.h"
#include "trajectory.h"
#include "nameconsts.h"
#include "misc_utils.h"
#include "dpoint.h"
#include "alloc.h"
#include "math.h"
#include "class_utils.h"
#include "PropBag.h"
#include "float.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"
#include "stdio.h"

trajectory_info::trajectory_info() : CDataInfoBase( clsid_trajectory, sz_trajectory_type, szArgumentTypeObject, IDI_ICON_TRAJECTORY )
{}

trajectory::trajectory()
{
	m_nObjNum=-1;
	m_TrajectoryData.r_max=1e20;
}

HRESULT trajectory::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream) return E_INVALIDARG;
	ULONG nRead = 0;
	pStream->Read( &m_nObjNum, sizeof( m_nObjNum ), &nRead );
	return S_OK;
}

HRESULT trajectory::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream) return E_INVALIDARG;
	ULONG nWritten = 0;
	pStream->Write( &m_nObjNum, sizeof( m_nObjNum ), &nWritten );
	return S_OK;
}

HRESULT trajectory::GetObjNum(long * plNum)
{
	if(!plNum) return E_INVALIDARG;
	*plNum=m_nObjNum;
	return S_OK;
}

HRESULT trajectory::SetObjNum(long lNum)
{
	m_nObjNum=lNum;
	return S_OK;
}

HRESULT trajectory::GetTrajectoryDataPtr(CTrajectoryData **ppData)
{
	if(ppData==0) return E_INVALIDARG;
	*ppData = &m_TrajectoryData;
	return S_OK;
}

static void MakeSmoothAxis(_dpoint *axis, int len,
						   _dpoint *smooth_axis, _dpoint *vect_axis,
						   int nPointsAverage, int nStepAverage)
// На входе - ось axis и ее длина lAxisSize.
// На выходе - сглаженная ось smooth_axis
// орт ее направления в каждой точке vect_axis
{
	//посчитать вектора
	{ //посчитаем вектора направления
		for(int i=0; i<len; i++) 
		{
			int i0=max(i-5,0);
			int i1=min(i+5,len-1);
			double dx=axis[i1].x-axis[i0].x;
			double dy=axis[i1].y-axis[i0].y;
			double d=_hypot(dx,dy); d=max(d,1e-5);
			vect_axis[i].x=dx/d; vect_axis[i].y=dy/d;
		}
	}

	/*
	smart_alloc(shift, double, len); //сдвиг точки оси
	//сглаживание оси.
	for(int i=0; i<len; i++)
	{ //найдем, насколько точка оси сдвинута по перпендикуляру к оси относительно средней линии по ближайшим точкам
		double s=0;
		int n=0;
		int jmax=nPointsAverage/2;
		for(int j=-jmax; j<=jmax; j++)
		{
			int i1=i-j*nStepAverage;
			if(i1>=0 && i1<len)
			{
				s+=	(axis[i].x-axis[i1].x)*(vect_axis[i].y)-
					(axis[i].y-axis[i1].y)*(vect_axis[i].x);
				n++;
			}
		}
		shift[i]=s/n;
	}

	for(i=0; i<len; i++)
	{ //сдвинем точки перпендикулярно оси
		smooth_axis[i].x = axis[i].x - shift[i]*vect_axis[i].y;
		smooth_axis[i].y = axis[i].y + shift[i]*vect_axis[i].x;
	}
	*/

	for(int i=0; i<len; i++)
	{
		smooth_axis[i].x = axis[i].x;
		smooth_axis[i].y = axis[i].y;
	}

	double a=0.5;
	for(int j=0; j<5; j++)
	{
		for(int i=len-2; i>0; i--)
		{
			smooth_axis[i].x = a*smooth_axis[i].x + (1-a)*smooth_axis[i+1].x;
			smooth_axis[i].y = a*smooth_axis[i].y + (1-a)*smooth_axis[i+1].y;
		}

		for(i=1; i<len-1; i++)
		{
			smooth_axis[i].x = a*smooth_axis[i].x + (1-a)*smooth_axis[i-1].x;
			smooth_axis[i].y = a*smooth_axis[i].y + (1-a)*smooth_axis[i-1].y;
		}
	}
}

COLORREF trajectory::_get_class_color( long lClass )
{
	/*
	long	lpos = ms_class_colors.find( lClass );
	if( !lpos )
	{
		COLORREF cr = ::get_class_color( lClass );
		ms_class_colors.set( cr, lClass );
		return cr;
	}

	return ms_class_colors.get( lpos );
	*/
	return ::get_class_color( lClass );
}

//return key by request interface INumberedObect. If key can't be obtained, return -1 (NOKEY)
static GuidKey GetObjectKey( IUnknown *punk )
{
	GuidKey key;

	INumeredObject * punkNum = 0;
	if (!punk || FAILED(punk->QueryInterface(IID_INumeredObject, (void**)&punkNum)) || !punkNum)
		return key;

	punkNum->GetKey( &key );
	punkNum->Release();

	return key;
}

static _point _window(double x, double y, IScrollZoomSite *psite)
{
	INIT_ZOOM_SCROLL( psite );
	return _point( 
		int( x*fZoom+.5-pointScroll.x ),
		int( y*fZoom+.5-pointScroll.y ) );
}

HRESULT trajectory::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	CNamed2DArrays arrs;

	IUnknown *punk=0;
	GetParent(&punk);
	INamedDataObject2Ptr ptrO(punk);
	if(punk!=0) punk->Release();
	if(ptrO==0) return (E_FAIL);
	
	IUnknown *punk2=0;
	ptrO->GetParent(&punk2);
	INamedDataObject2Ptr ptrList(punk2);
	if(punk2!=0) punk2->Release();
	if(ptrList==0) return (E_FAIL);

	/*
	bool bActive = 0;
	long lPos=0;
	ptrList->GetActiveChild(&lPos);
	if(lPos!=0)
	{
		IUnknown *punkActive=0;
		ptrList->GetNextChild(&lPos,&punkActive);
		if(punkActive!=0)
		{
			bActive = ::GetObjectKey(punkActive) == ::GetObjectKey(ptrO);
			punkActive->Release();
		}
	}
	*/

	arrs.AttachArrays(ptrList);

	_byte_ptr<int> xTotal = arrs.GetExistingArray("x"); //координаты
	_byte_ptr<int> yTotal = arrs.GetExistingArray("y");
	_byte_ptr<byte> exist = arrs.GetExistingArray("exist"); //существует ли на соответствующем кадре
	_byte_ptr<float> pLineWidth(arrs.GetExistingArray("width")); // ширина

	if(xTotal==0 || yTotal==0 || exist==0) return S_OK; //на всякий пожарный

	int nObjectStep=arrs.GetObjectStep();
	int nFrameStep=arrs.GetFrameStep();
	int nFrames=::GetValueInt(ptrList, "\\particles", "FrameNo", 0);

	IScrollZoomSitePtr      ptrScrollZoom(punkTarget);
	
	COLORREF c_axis = GetValueColor(GetAppUnknown(), "\\Particles", "TrajectoryColor", RGB( 255, 0, 255 ));

	long	lclass = get_object_class( ICalcObjectPtr( ptrO ) );
	if(lclass!=-1) c_axis = _get_class_color(lclass);

	HPEN hPen = ::CreatePen( PS_SOLID, 0, c_axis );
	//HPEN hPen = ::CreatePen( PS_SOLID, 0, RGB( 255, 128, 128 ) );
	//::GetStockObject(WHITE_PEN)
	
	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen);
	int old_rop2=::GetROP2(hDC);

	if(old_rop2!=R2_XORPEN) ::SetROP2(hDC,R2_COPYPEN);

	//----------------
	int lDrawSmooth=-1000;
	INamedPropBagPtr bag(punkTarget);
	if(bag!=0)
	{
		_variant_t	var;
		bag->GetProperty(_bstr_t("DrawSmooth"), &var);
		if(var.vt != VT_EMPTY) lDrawSmooth=var.lVal;
	}
	if(lDrawSmooth==-1000)
	{
		lDrawSmooth = ::GetValueInt( GetAppUnknown(), "\\Particles", "TrajectoryDrawSmooth", 0 );
		if(bag!=0) bag->SetProperty(_bstr_t("DrawSmooth"),_variant_t(long(lDrawSmooth)));
	}

	if(lDrawSmooth && nFrames>2)
	{ //сглаженная траектория
		_ptr_t2<_dpoint> tr(nFrames);

		HPEN hPen1 = ::CreatePen( PS_SOLID, 0, 0xFF0000 );
		if(old_rop2!=R2_XORPEN) ::SelectObject(hDC, hPen1);

		int nRealFrames=0;
		int nRealFrames2=0;
		bool bFound=false;
		for(int nFrame=0; nFrame<nFrames; nFrame++)
		{
			int posT = nFrameStep*nFrame + nObjectStep*m_nObjNum;
			byte bFrameExist = exist[posT]; // текущий кадр задан
			if(bFrameExist)
			{
				bFound = true;
				nRealFrames2++;
			}
			if(!bFound) continue; //пропускаем пустые кадры в начале
			tr[nRealFrames].x = xTotal[posT];
			tr[nRealFrames].y = yTotal[posT];
			nRealFrames++;
		}
		if(nRealFrames>2)
		{
			// создадим сглаженную траекторию
			_ptr_t2<_dpoint> smooth_tr(nFrames);
			_ptr_t2<_dpoint> vect_tr(nFrames);
			MakeSmoothAxis(tr, nRealFrames, smooth_tr, vect_tr, 10, 2);

			int total=m_nObjNum;
			for(int nFrame=1; nFrame<nFrames; nFrame++)
			{
				_point pt1=_window(
					smooth_tr[nFrame].x, smooth_tr[nFrame].y, ptrScrollZoom);
				::MoveToEx(hDC, pt1.x,pt1.y, 0);
				_point pt2=_window(
					smooth_tr[nFrame-1].x, smooth_tr[nFrame-1].y, ptrScrollZoom);
				::LineTo(hDC, pt2.x,pt2.y);
			}
		}
		::SelectObject(hDC, hPen);
		::DeleteObject(hPen1);
	}
	//----------------

	//::SelectObject(hDC, hPen);

	int total=m_nObjNum;

	_point pt2=_point(-1000,-1000);
	for(int nFrame=0; nFrame<nFrames; nFrame++)
	{
		int posT = nFrameStep*nFrame + nObjectStep*total;
		if(!exist[posT]) continue;
		_point pt1=_window(_point(xTotal[posT],yTotal[posT]), ptrScrollZoom);

		if(pt2.x!=-1000)
		{
			::MoveToEx(hDC, pt1.x,pt1.y, 0);
			::LineTo(hDC, pt2.x,pt2.y);
			::LineTo(hDC, pt2.x+1,pt2.y); // чтобы нарисовало и последнюю точку - для случаев pt1==pt2
		}
		pt2=pt1;

		{	// отрисовка квадратиков
			int lDrawPoints=-1000;
			INamedPropBagPtr bag(punkTarget);
			if(bag!=0)
			{
				_variant_t	var;
				bag->GetProperty(_bstr_t("DrawPoints"), &var);
				if(var.vt != VT_EMPTY) lDrawPoints=var.lVal;
			}
			if(lDrawPoints==-1000)
			{
				lDrawPoints = ::GetValueInt( GetAppUnknown(), "\\Particles", "TrajectoryDrawPoints", 0 );
				if(bag!=0) bag->SetProperty(_bstr_t("DrawPoints"),_variant_t(long(lDrawPoints)));
			}
			if(lDrawPoints)
			{
				::MoveToEx(hDC, pt1.x-1,pt1.y-1, 0);
				::LineTo(hDC, pt1.x-1,pt1.y+1);
				::LineTo(hDC, pt1.x+1,pt1.y+1);
				::LineTo(hDC, pt1.x+1,pt1.y-1);
				::LineTo(hDC, pt1.x-1,pt1.y-1);
			}
		}
	}

	// нарисовать мускулистую часть =)
	if(pLineWidth!=0)
	{
		HPEN hPen2 = ::CreatePen( PS_SOLID, 0, 0xFF0000 );
		::SelectObject(hDC, hPen2);
		int posT = nFrameStep*0 + nObjectStep*total;
		_point pt1prev=_window(xTotal[posT],yTotal[posT], ptrScrollZoom);
		_point pt2prev=pt1prev; // начальные точки линий - от начала хвоста
		for(int nFrame=0; nFrame<nFrames; nFrame++)
		{
			int posT = nFrameStep*nFrame + nObjectStep*total;
			if(!exist[posT]) continue;
			double r=pLineWidth[posT];
			if(_isnan(r)) // !!! ловим багу с хвостами
			{
				int z = _fpclass(r);
				static bool bStopHere=false;
				if(bStopHere) MessageBox(0, "NaN found", "Warning", MB_OK|MB_ICONWARNING); //!!!debug
				r=-0.001; // подстраховка от баги с хвостами
			}
			if(r<0) r=-0.001;
			int nFrame0 = max(0,nFrame-1);
			int nFrame1 = min(nFrames-1,nFrame+1);
			int posT0 = nFrameStep*nFrame0 + nObjectStep*total;
			int posT1 = nFrameStep*nFrame1 + nObjectStep*total;
			if(exist[posT0] && exist[posT1])
			{
				double dx=xTotal[posT1]-xTotal[posT0];
				double dy=yTotal[posT1]-yTotal[posT0];
				double h=max(_hypot(dx,dy),0.01);
				dx/=h; dy/=h;
				_point pt1=_window(xTotal[posT]-dy*r,yTotal[posT]+dx*r, ptrScrollZoom);
				_point pt2=_window(xTotal[posT]+dy*r,yTotal[posT]-dx*r, ptrScrollZoom);
				if(nFrame!=0 && r>=0)
				{
					MoveToEx(hDC,pt1prev.x,pt1prev.y,0); LineTo(hDC,pt1.x,pt1.y);
					MoveToEx(hDC,pt2prev.x,pt2prev.y,0); LineTo(hDC,pt2.x,pt2.y);
				}
				pt1prev=pt1; pt2prev=pt2;
			}
			if(r<0) break;
		}
		::SelectObject(hDC, hPen);
		::DeleteObject(hPen2);
	}

	::SetROP2(hDC,old_rop2);
	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen);
	
	return S_OK;
}

HRESULT trajectory::GetRect( RECT *prect )
{
	CNamed2DArrays arrs;

	IUnknown *punk=0;
	GetParent(&punk);
	INamedDataObject2Ptr ptrO(punk);
	if(punk!=0) punk->Release();
	if(ptrO==0) return (E_FAIL);
	
	IUnknown *punk2=0;
	ptrO->GetParent(&punk2);
	INamedDataObject2Ptr ptrList(punk2);
	if(punk2!=0) punk2->Release();
	if(ptrList==0) return (E_FAIL);

	arrs.AttachArrays(ptrList);

	_byte_ptr<int> xTotal = arrs.GetExistingArray("x"); //координаты
	_byte_ptr<int> yTotal = arrs.GetExistingArray("y");
	_byte_ptr<byte> exist = arrs.GetExistingArray("exist"); //существует ли на соответствующем кадре
	if(xTotal==0 || yTotal==0 || exist==0)
	{
		*prect=NORECT;
		return E_FAIL; //на всякий пожарный
	}

	int nObjectStep=arrs.GetObjectStep();
	int nFrameStep=arrs.GetFrameStep();
	int nFrames=::GetValueInt(ptrList, "\\particles", "FrameNo", 0);

	int xmin=100000, xmax=-100000, ymin=100000, ymax=-100000;

	int total=m_nObjNum;
	_point pt2=_point(-1000,-1000);
	for(int nFrame=0; nFrame<nFrames; nFrame++)
	{
		int posT = nFrameStep*nFrame + nObjectStep*total;
		if(!exist[posT]) continue;
		_point pt1=_point(xTotal[posT],yTotal[posT]);

		if(pt1.x<xmin) xmin=pt1.x;
		if(pt1.x>xmax) xmax=pt1.x;
		if(pt1.y<ymin) ymin=pt1.y;
		if(pt1.y>ymax) ymax=pt1.y;		

		pt2=pt1;
	}

	if(xmax>=xmin && ymax>=ymin)
	{
		*prect=_rect(xmin-1,ymin-1,xmax+1,ymax+1);
		return S_OK;
	}
	else
	{
		*prect=NORECT;
		return S_OK;
	}


	return S_OK;
}

// IRectObject
HRESULT trajectory::SetRect(RECT rc)
{
	return S_OK;
}

HRESULT trajectory::HitTest( POINT	point, long *plHitTest )
{
	CNamed2DArrays arrs;

	*plHitTest=0;

	IUnknown *punk=0;
	GetParent(&punk);
	INamedDataObject2Ptr ptrO(punk);
	if(punk!=0) punk->Release();
	if(ptrO==0) return (E_FAIL);
	
	IUnknown *punk2=0;
	ptrO->GetParent(&punk2);
	INamedDataObject2Ptr ptrList(punk2);
	if(punk2!=0) punk2->Release();
	if(ptrList==0) return (E_FAIL);

	arrs.AttachArrays(ptrList);

	_byte_ptr<int> xTotal = arrs.GetExistingArray("x"); //координаты
	_byte_ptr<int> yTotal = arrs.GetExistingArray("y");
	_byte_ptr<byte> exist = arrs.GetExistingArray("exist"); //существует ли на соответствующем кадре
	if(xTotal==0 || yTotal==0 || exist==0)
	{
		*plHitTest=0;
		return S_OK; //на всякий пожарный
	}

	int nObjectStep=arrs.GetObjectStep();
	int nFrameStep=arrs.GetFrameStep();
	int nFrames=::GetValueInt(ptrList, "\\particles", "FrameNo", 0);

	int total=m_nObjNum;
	_point pt2=_point(-1000,-1000);
	for(int nFrame=0; nFrame<nFrames; nFrame++)
	{
		int posT = nFrameStep*nFrame + nObjectStep*total;
		if(!exist[posT]) continue;
		_point pt1=_point(xTotal[posT],yTotal[posT]);
		
		if(abs(point.x-pt1.x)<5 && abs(point.y-pt1.y)<5)
		{
			*plHitTest=1;
			break;
		}

		pt2=pt1;
	}

	return S_OK;
}

HRESULT trajectory::Move( POINT point )
{
	return S_OK;
}


HRESULT trajectory::SetTargetWindow( IUnknown *punkTarget )
{
	return S_OK;
}


// ISmartSelector
HRESULT trajectory::SmartSelect( bool bSelect, IUnknown *punkSelection )
{
	IMultiSelectionPtr ptrSelection(punkSelection);
	if(ptrSelection==0) return E_FAIL;

	bool bGroup = ((short)GetKeyState(VK_CONTROL) < 0);

	INamedDataObject2Ptr ptrNDO = Unknown();
	if(ptrNDO)
	{
		IUnknownPtr ptrParent;
		ptrNDO->GetParent(&ptrParent);
		if(ptrParent!=0)
		{
			IMeasureObjectPtr ptrO(ptrParent);
			if(ptrO!=0)
			{
				IUnknownPtr ptr2;
				ptrO->GetImage(&ptr2);
				IImage3Ptr ptrI(ptr2);
				if(ptrI!=0)
				{
					ptrSelection->SelectObject(ptrParent, bGroup);
					ptrSelection->UnselectObject(Unknown());
					//ptrSelection->SelectObject(Unknown(), true);
					return S_OK;
				}
				else
				{
					ptrSelection->SelectObject( Unknown() , bGroup);
					return S_OK;
				}
			}
		}
	}
	return S_FALSE;
}

// INotifyObject
HRESULT trajectory::NotifyCreate()
{
	return S_OK;
}

HRESULT trajectory::NotifyDestroy()
{
	return S_OK;
}

HRESULT trajectory::NotifyActivate( bool bActivate )
{
	return S_OK;
}

HRESULT trajectory::NotifySelect( bool bSelect )
{
	if( bSelect )
	{
		IUnknown *punk1=0;
		GetParent(&punk1);
		INamedDataObject2Ptr ptrParent(punk1);
		if(punk1!=0) punk1->Release();

		if( ptrParent==0 )return S_FALSE;
		IUnknown	*punk = 0;
		ptrParent->GetParent( &punk );
		if( punk ==0 )return S_FALSE;
		ICalcObjectContainerPtr	ptrContainer( punk );
		punk->Release();
		if( ptrContainer==0 )return S_FALSE;

		long	lpos = 0;
		INamedDataObject2Ptr	ptrList( ptrContainer );
		ptrParent->GetObjectPosInParent( &lpos ); 
		ptrList->SetActiveChild( lpos );
	}
	return S_OK;
}

// ----------------------------------------------------------

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	long lPos = 0;
	sptrNDOParent->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrChild;
		sptrNDOParent->GetNextChild(&lPos, &sptrChild);
		if (sptrChild!=0)
		{
			if (::CheckInterface(sptrChild, guid))
				return sptrChild;
		}
	}
	return NULL;
}

ITrajectoryPtr CreateTrajectory(IUnknown *punkCalcObject)
{
	ITrajectoryPtr sptr;
	// Check whether the object already exists
	IUnknownPtr ptrObj = punkCalcObject;
	IUnknownPtr ptrFound = find_child_by_interface(ptrObj, IID_ITrajectory);
	bool bFound = ptrFound;
	if (bFound)
	{
		sptr = ptrFound;
		return sptr;
	}
	// Create child objects and add it to chromosome
//	CChromosome *pChromos = new CChromosome;
//	IUnknown *punk1 = pChromos->GetControllingUnknown();
	IUnknown *punk1 = CreateTypedObject(_bstr_t(sz_trajectory_type));
	if( !punk1 )return 0;
	sptr =  punk1;
	sptrINamedDataObject2 sptrNO1 = punk1;
	punk1->Release();
	sptrNO1->SetParent(punkCalcObject, 0);
	return sptr;
};

