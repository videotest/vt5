// SelectByAxis.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelectByAxis.h"
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
#include "chrom_aam_utils.h"
#include "core5.h"

#define MaxColor color(-1)

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	POSITION lPos = 0;
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

static IChromosome2Ptr CreateChildObjects(IUnknown *punkCalcObject)
{
	IChromosome2Ptr sptr;
	// Check whether the object already exists
	IUnknownPtr ptrObj = punkCalcObject;
	IUnknownPtr ptrFound = find_child_by_interface(ptrObj, IID_IChromosome2);
	bool bFound = ptrFound;
	if (bFound)
	{
		sptr = ptrFound;
		return sptr;
	}
	// Create child objects and add it to chromosome
//	CChromosome *pChromos = new CChromosome;
//	IUnknown *punk1 = pChromos->GetControllingUnknown();
	IUnknown *punk1 = CreateTypedObject(L"ChromosomeAxisAndCentromere");
	if( !punk1 )return 0;
	sptr =  punk1;
	sptrINamedDataObject2 sptrNO1 = punk1;
	punk1->Release();
	sptrNO1->SetParent(punkCalcObject, 0);
	return sptr;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		SelectByAxis
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CSelectByAxisInfo::s_pargs[] = 
{	
	{"AddToSource",		szArgumentTypeInt, "0", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

#define MAX_SPLINE_POINTS 50
CSelectByAxisFilter::CSelectByAxisFilter()
{
	m_SplinePoints=new _point[MAX_SPLINE_POINTS];
	m_SplinePointsNum=0;
}

CSelectByAxisFilter::~CSelectByAxisFilter()
{
	delete[] m_SplinePoints;
}

HRESULT CSelectByAxisFilter::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CSelectByAxisFilter::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	if(!punkObj) return false;
	if(punkObj) punkObj->Release();
	
	//sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	//if(!punkObj) return false;
	//if(punkObj) punkObj->Release();
	
	return true;
}

IUnknown* CSelectByAxisFilter::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

HRESULT CSelectByAxisFilter::Undo()
{
	HRESULT hr = __super::Undo();

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

	return hr;
}

static IUnknownPtr GetTargetByKey(GuidKey guid)
{
	// get app
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if(ptrApp==0) return 0;

	//get the target manager
	IUnknown *punkTargetMan = 0;
	ptrApp->GetTargetManager( &punkTargetMan );
	IActionTargetManagerPtr	ptrTargetMan( punkTargetMan );
	if(punkTargetMan) punkTargetMan->Release();
	if( ptrTargetMan==0 ) return 0;

	// get target
	IUnknown *punk = 0;
	ptrTargetMan->GetTargetByKey( guid, &punk );
	IUnknownPtr ptrResult(punk);
	if(punk!=0) punk->Release();

	return ptrResult;
}

HRESULT CSelectByAxisFilter::Redo()
{
	HRESULT hr = __super::Redo();

	if(m_ptrList!=0)
	{
		{
			IUnknownPtr ptrDoc = GetTargetByKey(m_guidTarget);

			INamedDataPtr	ptrData( ptrDoc );

			IUnknown	*punkContext = 0;
			if( ptrData )ptrData->GetActiveContext(&punkContext);
			IDataContextPtr	ptrContext( punkContext );
			if( punkContext )punkContext->Release();

			if( ptrContext )ptrContext->SetActiveObject( 0, m_ptrList, 0 );


			IDataObjectListPtr ptrObjectList(m_ptrList);
			if(ptrObjectList!=0)
			{
				ptrObjectList->UpdateObject(0);
			}
		}
	}

	return hr;
}

bool CSelectByAxisFilter::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;

	IDataContextPtr	sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	m_image = punkObj;
	if(punkObj) punkObj->Release();

	IUnknown* punkObj1 = 0;	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj1);
	m_ptrList = INamedDataObject2Ptr (punkObj1);
	if(punkObj1) punkObj1->Release();
	if(m_ptrList!=0)
	{
		m_ptrList->SetActiveChild(0);
	}

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORSELECTBYAXIS));
	m_hCurShiftActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORSELECTBYAXISSHIFT));

	m_action=0;

	return m_image != 0;
}

void CSelectByAxisFilter::DeInit()
{
	::InvalidateRect(m_hwnd, 0, false);
}

bool CSelectByAxisFilter::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	if( !CInteractiveAction::Finalize() )
		return false;

	return true;
}

bool CSelectByAxisFilter::DoStartTracking( _point point )
{
	//m_ptOrigOffset = point;
	return true;
}

bool CSelectByAxisFilter::DoTrack( _point point )
{
	//MoveTo(point.delta(m_ptOrigOffset));
	//m_ptOrigOffset = point;
	return true;
}


bool CSelectByAxisFilter::DoFinishTracking( _point point )
{
	//m_ptNewOffset = point;
	//Finalize();
	return true;
}

bool CSelectByAxisFilter::DoLButtonDblClick( _point point )
{
	if(!IsAvaible()) return(false);

	//DoLButtonDown(point);
	m_FirstPoint=point;
	m_action=1;
	Finalize();
	::InvalidateRect(m_hwnd, 0, false);

	return false;
}

bool CSelectByAxisFilter::DoRButtonDown( _point point )
{
	if(m_SplinePointsNum<1) return CInteractiveAction::DoRButtonDown(point); //если не начали выделять - по старому
	DoLButtonDown(point);
	return true; //не давать гадам обрабатывать
}

bool CSelectByAxisFilter::DoRButtonUp( _point point )
{
	if(m_SplinePointsNum<1) return CInteractiveAction::DoRButtonUp(point); //если не начали выделять - менюшка

	if(!IsAvaible()) return(false);

	m_FirstPoint=point;
	m_action=1;
	Finalize();
	::InvalidateRect(m_hwnd, 0, false);

	return true;
}

bool CSelectByAxisFilter::DoLButtonDown( _point point )
{
	if(!IsAvaible()) return(false);

	if(m_image==0) return false;
	int w,h;
	m_image->GetSize(&w,&h);
	if(point.x<0 || point.x>=w || point.y<0 || point.y>=h) return true; //за пределами image - ничего не делаем

	m_FirstPoint=point;

	if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	{
		m_action=2;
		Finalize();
	}

	if(m_SplinePointsNum<MAX_SPLINE_POINTS)
	{
		m_SplinePoints[m_SplinePointsNum]=point;
		m_SplinePointsNum++;
	}
	//StartTracking( point );
	::InvalidateRect(m_hwnd, 0, false);
	UpdateWindow(m_hwnd);
	return true;
}

bool CSelectByAxisFilter::DoMouseMove( _point point )
{
	//return true; //не давать гадам обрабатывать
	return CInteractiveAction::DoMouseMove(point);
}

bool CSelectByAxisFilter::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;

	if(m_image == 0)return false;

	return false;
}

LRESULT	CSelectByAxisFilter::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	long bResult = false;

	if( nMsg == WM_KEYDOWN || nMsg == WM_KEYUP )
	{
		if(wParam==VK_SHIFT)
		{
			if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
				::SetCursor(m_hCurShiftActive);
			else
				::SetCursor(m_hCurActive);
		}
	}

	if( nMsg == WM_SETCURSOR )
	{
		if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
			::SetCursor(m_hCurShiftActive);
		else
			::SetCursor(m_hCurActive);
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

static void midpoint( // вычислить координату и производную в промежуточной точке
					 double y0, double v0, // координата и производная на одном конце
					 double y1, double v1, // координата и производная на втором конце
					 double *y, double *v  // найти
					 )
{
	*y = (y0+y1)/2+(v0-v1)/8;
	*v = ((y1-y0)*6-v0-v1)/4;
}

static void curve2( // Отобразить кривую
			HDC hDC,
            double x0, double u0, // координата X и производная на одном конце
            double y0, double v0, // координата Y и производная на одном конце
            double x1, double u1, // координата X и производная на втором конце
            double y1, double v1  // координата Y и производная на втором конце
			)
{
	double x,y,u,v;
	if(_hypot(x1-x0,y1-y0)>1) {
		midpoint(x0,u0,x1,u1, &x,&u);
		midpoint(y0,v0,y1,v1, &y,&v);
		curve2(hDC, x0,u0/2, y0,v0/2,  x,u/2, y,v/2);
		curve2(hDC, x,u/2, y,v/2,  x1,u1/2, y1,v1/2);
	}
	else
	{
		::MoveToEx(hDC, (int)x0, (int)y0, 0);
		::LineTo(hDC, (int)x1, (int)y1);
	}
}

static void MakeCurve2( // Создать кривую
			_dpoint **buf, int *bufsize, //буфер точек и остаток места в нем
            double x0, double u0, // координата X и производная на одном конце
            double y0, double v0, // координата Y и производная на одном конце
            double x1, double u1, // координата X и производная на втором конце
            double y1, double v1  // координата Y и производная на втором конце
			)
{
	double x,y,u,v;
	midpoint(x0,u0,x1,u1, &x,&u);
	midpoint(y0,v0,y1,v1, &y,&v);
	if(_hypot(x1-x0,y1-y0)>1) {
		MakeCurve2(buf,bufsize, x0,u0/2, y0,v0/2,  x,u/2, y,v/2);
		MakeCurve2(buf,bufsize, x,u/2, y,v/2,  x1,u1/2, y1,v1/2);
	}
	else
	{
		if(*bufsize)
		{
			**buf=_dpoint(x,y);
			(*buf)++;
			(*bufsize)--;
		}
	}
}

IUnknownPtr	CSelectByAxisFilter::get_document()
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

static _dpoint calc_d(_dpoint pt1, _dpoint pt2, _dpoint pt3)
{ //вычисление производных в точке pt2
#define tension 0.666
	double d0=_hypot(pt1.x-pt2.x,pt1.y-pt2.y);
	double d1=_hypot(pt3.x-pt2.x,pt3.y-pt2.y);
	//d0=1; d1=1;
	double d=max(d0+d1,1e-4);
	double dx=((pt2.x-pt1.x)*d1+(pt3.x-pt2.x)*d0)/d;
	double dy=((pt2.y-pt1.y)*d1+(pt3.y-pt2.y)*d0)/d;
	return(_dpoint(dx/tension,dy/tension));
}

static RECT ClipRect={0,0,1024,1024};

static void FillPoly(byte **dst, int *x, int*y, int n, byte color)
{
	smart_alloc(x0, int, ClipRect.bottom);
	for(int i=0; i<ClipRect.bottom; i++) x0[i]=-1;

	int lastDir=0;
	int x1=x[n-1], y1=y[n-1];
	for(i=0; i<n; i++)
	{
		int xx1=x1*65536, dxx;
		if(y1!=y[i]) dxx=(x[i]-x1)*65536/abs(y[i]-y1);
		while(y1!=y[i])
		{
			x1=xx1/65536;
			int dy= y[i]>y1 ? 1 : -1;
			if(x0[y1]!=-1)
			{
				int xx1=min(x1,x0[y1]);
				int xx2=max(x1,x0[y1]);
				for(int fillx=xx1; fillx<=xx2; fillx++)
				{
					if( y1>=ClipRect.top && y1<ClipRect.bottom &&
						fillx>=ClipRect.left && fillx<ClipRect.right)
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
			lastDir=dy;
			y1+=dy;
			xx1+=dxx;
		}
		x1=x[i];
	}
}

inline static void HLine(byte **dst,
		   int x1, int x2, int y,
		   byte color)
{
	if (x2<x1)
	{
		int i=x1; x1=x2; x2=i;
	}
	if(y<ClipRect.top || y>=ClipRect.bottom) return;
	if(x1<ClipRect.left) x1=ClipRect.left;
	if(x2>=ClipRect.right) x2=ClipRect.right-1;
	for(int x=x1; x<=x2; x++)
		dst[y][x] = color;
}

// отрисовка трапеции с горизонтальными основаниями
// (если длина одного из них = 0 - треугольник
static void FillTrap(byte **dst,
			  int x1_1, int x1_2, int y1, // верх
			  int x2_1, int x2_2, int y2, // низ
			  byte color)
{
  int i;
  int len;
  len=abs(y2-y1);
  if (len==0) len=1;
  for(i=0; i<=len; i++)
  {
    HLine(dst,
		x1_1+i*(x2_1-x1_1)/len, x1_2+i*(x2_2-x1_2)/len, y1+i,
		color);
  }

}

static void Line(byte **dst,
				 int x1, int y1,
				 int x2, int y2,
						 byte color)
{
	int len=max(abs(x1-x2),abs(y1-y2));
	len=max(len,1);
	double dx=double(x2-x1)/len;
	double dy=double(y2-y1)/len;
	double xx=x1, yy=y1;
	for(int i=0; i<=len; i++)
	{
		int x=int(xx), y=int(yy);
		if( y>=ClipRect.top && y<ClipRect.bottom &&
			x>=ClipRect.left && x<ClipRect.right)
		{
			dst[y][x] = color;
		}
		xx+=dx; yy+=dy;
	}
}

// Отрисовка треугольника
static void FillTriangle(byte **dst,
						 int x1, int y1,
						 int x2, int y2,
						 int x3, int y3,
						 byte color)
{
	/*dst[y1][x1]=color;
	dst[y2][x2]=color;
	dst[y3][x3]=color;*/
	/*
	Line(dst, x1,y1, x2,y2, color);
	Line(dst, x1,y1, x3,y3, color);
	Line(dst, x2,y2, x3,y3, color);
	return;
	*/
	int i;
	int x,y;
	// отсортируем вершины по вертикали
	if (y1>y2) {
		i=x1; x1=x2; x2=i;
		i=y1; y1=y2; y2=i;
	}
	if (y2>y3) {
		i=x2; x2=x3; x3=i;
		i=y2; y2=y3; y3=i;
	}
	if (y1>y2) {
		i=x1; x1=x2; x2=i;
		i=y1; y1=y2; y2=i;
	}
	// найдем координаты горизонтали, разделяющей треугольник на два
	x=x2; y=y2;
	if (y3>y1) {
		x=x1+(y-y1)*(x3-x1)/(y3-y1);
	}
	FillTrap(dst,  x1,x1,y1,  x2,x,y2,  color);
	FillTrap(dst,  x2,x,y2,  x3,x3,y3,  color);
}

static void OpenFilter(int *r, int point_num, int width)
{
	smart_alloc(r_buf, int, point_num);
	for(int i=0; i<point_num; i++)
	{
		int i1=max(i-width,0);
		int i2=min(i+width,point_num-1);
		int rr=r[i];
		for(int j=i1; j<i2; j++)
		{
			if(r[j]<rr) rr=r[j];
		}
		r_buf[i]=rr;
	}
	for(i=0; i<point_num; i++)
	{
		int i1=max(i-width,0);
		int i2=min(i+width,point_num-1);
		int rr=r_buf[i];
		for(int j=i1; j<i2; j++)
		{
			if(r_buf[j]>rr) rr=r_buf[j];
		}
		r[i]=rr;
	}
}

IImage3Ptr	CSelectByAxisFilter::CreateObjectImage(_dpoint *curve, int point_num, int &i0, int &i1)
//возвращает картинку и индекс первой (i0) и последней (i1) точки хромосомы
{
	//-------------------- image
	_bstr_t s2(szTypeImage);
	IUnknown        *punk2 = ::CreateTypedObject(s2);
	IImage3Ptr      ptrI(punk2);
	punk2->Release();
	INamedDataObject2Ptr ptrImg(ptrI);

	int w,h;
	m_image->GetSize(&w,&h);
	smart_alloc(srcRows, color *, h);
	for(int y=0; y<h; y++)
	{
		m_image->GetRow(y, 1, &srcRows[y]);
	}

	//1. найти max, min вокруг сплайна, постр. гистограмму
	smart_alloc(hist, int, NC);
	smart_hist.zero();
	smart_alloc(hist1, int, NC);

	int rcal=100;
	for(int j=0; j<2000; j++)
	{
		int i=rand()%point_num;
		int x=int(curve[i].x+rand()%(rcal*2)-rcal);
		int y=int(curve[i].y+rand()%(rcal*2)-rcal);
		if(x>=0 && x<w && y>=0 && y<h)
		{
			int c=srcRows[y][x];
			hist[c/(65536/NC)]++;
		}
	}
	//сгладим гистограмму
	{for(int i=1; i<NC; i++)
		hist[i]=hist[i]+int((hist[i-1]-hist[i])*hist_smooth_level);
	for(i=NC-2; i>=0; i--)
		hist[i]=hist[i]+int((hist[i+1]-hist[i])*hist_smooth_level);}

	//поиск пика гистограммы (фон)
	int max_peak=0;
	for(int i=0; i<NC; i++)
	{
		if(hist[i]>hist[max_peak]) max_peak=i;
	}

	//поиск второго пика
	int peak2=0;
	double h_max=0;
	for(i=0; i<NC; i++)
	{
		double h=double(hist[i])*(i-max_peak)*(i-max_peak);
		if(h>h_max)
		{
			peak2=i;
			h_max=h;
		}
	}

	int axis_sum=0, axis_cnt=0;
	for(i=0; i<point_num; i++)
	{
		int x=int(curve[i].x);
		int y=int(curve[i].y);
		if(x>=0 && x<w && y>=0 && y<h)
		{
			int c=srcRows[y][x];
			axis_sum+=c;
			axis_cnt++;
		}
	}
	axis_sum/=max(axis_cnt,1);
	
	int xor_mask=(peak2>max_peak);
	int peak1=min(peak2,max_peak); 
	peak2=max(peak2,max_peak);

	int h_min=INT_MAX;
	int th;
	for(i=peak1; i<=peak2; i++)
	{
		int h=hist[peak1]+
			(hist[peak2]-hist[peak1])*(i-peak1)/max(1,peak2-peak1);
		h=hist[i]-h;
		if(h<h_max)
		{
			th=i;
			h_max=h;
		}
	}

	peak1*=(65536/NC);
	peak2*=(65536/NC);
	max_peak*=(65536/NC);
	th*=(65536/NC);

	int minContrast=GetValueInt(GetAppUnknown(),
			"\\Chromos\\SelectByAxis", "MinContrast", 10)*256;

	int threshold=(max_peak+axis_sum)/2;
	//int threshold=(peak1+peak2)/2;
	//int threshold=th;

	if(abs(max_peak-axis_sum)<minContrast)
	{
		return(0);
	}

	//threshold=avgc;
	smart_alloc(r, int, point_num);
	smart_alloc(r1, int, point_num);
	smart_alloc(r2, int, point_num);
	for(i=0; i<point_num; i++)
	{
		int x=int(curve[i].x);
		int y=int(curve[i].y);
		if(x<0 || y<0 || x>=w || y>=h) continue;
		int c0=srcRows[y][x];
		c0=(c0*3+axis_sum)/4; //на случай, если ось по фону прошла
		int threshold=(max_peak+c0)/2;
		int i1=max(i-1,0);
		int i2=min(i+1,point_num-1);
		double dx=curve[i2].x-curve[i1].x;
		double dy=curve[i2].y-curve[i1].y;
		double d=max(_hypot(dx,dy),0.01);
		dx/=d; dy/=d;
		int err=0;
		for(j=0; j<50; j++)
		{
			int xx=int(curve[i].x-j*dy);
			int yy=int(curve[i].y+j*dx);
			if(xx<0 || xx>=w || yy<0 || yy>=h) break;
			int c=srcRows[yy][xx];
			if((c>threshold) ^ xor_mask)
			{
				err++;
				if(err>3) break;
			}
		}
		j-=err-1;
		r1[i]=j;
		err=0;
		for(j=0; j<50; j++)
		{
			int xx=int(curve[i].x+j*dy);
			int yy=int(curve[i].y-j*dx);
			if(xx<0 || xx>=w || yy<0 || yy>=h) break;
			int c=srcRows[yy][xx];
			if((c>threshold) ^ xor_mask)
			{
				err++;
				if(err>3) break;
			}
		}
		j-=err-1;
		r2[i]=j;
	}

	//01.07.2002 - ограничим ширину хромосомы
	//см. shell.data
	int nEstimatedWidth=GetValueInt(GetAppUnknown(),
			"\\Chromos\\SelectByAxis", "EstimatedWidth", 25);

	int chr_width=
		OrderValue(r1, point_num, point_num/2, 0, 50)+
		OrderValue(r2, point_num, point_num/2, 0, 50);
	chr_width=min(chr_width, nEstimatedWidth);

	//отсечем сильно асимметричные хвосты
	//29.05.2002 - увеличил допустимую асимметрию с 2 до 2.5
	//29.05.2002 - не в 2.5 раза, а 1 + половина ширины хромосомы
	//29.05.2002 - и в 2.5 раза тоже...

	for(i=0; i<point_num; i++)
	{
		if(r1[i]>r2[i]*2.5) r1[i]=r2[i];
		if(r2[i]>r1[i]*2.5) r2[i]=r1[i];
	}
	for(i=0; i<point_num; i++)
	{
		if(r1[i]>r2[i]+chr_width/2) r1[i]=max(r2[i],chr_width/2);
		if(r2[i]>r1[i]+chr_width/2) r2[i]=max(r1[i],chr_width/2);
	}

	//отсечем хвосты - то, что больше медианы толщины
	//29.05.2002 - вместо медианы - n*2/3 по порядку значение
	int max_r1=OrderValue(r1, point_num, point_num*2/3, 0, 50);
	int max_r2=OrderValue(r2, point_num, point_num*2/3, 0, 50);
	max_r1=min(max_r1, nEstimatedWidth/2);
	max_r2=min(max_r2, nEstimatedWidth/2);
	for(i=0; i<point_num; i++)
	{
		if(r1[i]>max_r1*3/2) r1[i]=max_r1;
		if(r2[i]>max_r2*3/2) r2[i]=max_r2;
	}

	//посчитаем ширину для симметричного случая...
	for(i=0; i<point_num; i++)
	{
		r[i]=min(r1[i],r2[i]);
	}


	if(0) 
	{ //так убирать кресты не будем
		OpenFilter(r, point_num, 9);
		OpenFilter(r1, point_num, 9);
		OpenFilter(r2, point_num, 9);
	}

	//сгладим край
	//29.04.2002 - усилил сглаживание
	//29.04.2002 - увеличиваю точность (fixed point 24.8);
	{for(int i=0; i<point_num; i++) r1[i]*=256; }
	{for(int i=0; i<point_num; i++) r2[i]*=256; }
	for(int smooth_cnt=0; smooth_cnt<5; smooth_cnt++)
	{
		{for(int i=1; i<point_num; i++) if(r1[i]>1 && r1[i-1]>1) r1[i]=r1[i]+(r1[i-1]-r1[i])/2; }
		{for(int i=point_num-2; i>=0; i--) if(r1[i]>1 && r1[i+1]>1)  r1[i]=r1[i]+(r1[i+1]-r1[i])/2; }
		{for(int i=1; i<point_num; i++) if(r2[i]>1 && r2[i-1]>1) r2[i]=r2[i]+(r2[i-1]-r2[i])/2; }
		{for(int i=point_num-2; i>=0; i--) if(r2[i]>1 && r2[i+1]>1) r2[i]=r2[i]+(r2[i+1]-r2[i])/2; }
	}
	{for(int i=0; i<point_num; i++) r1[i]=(r1[i]+128)/256; }
	{for(int i=0; i<point_num; i++) r2[i]=(r2[i]+128)/256; }

	double minx=1e20, maxx=-1e20, miny=1e20, maxy=-1e20;

	//double r=10;
	
	for(i=0; i<point_num; i++)
	{
		if(curve[i].x-r1[i]<minx) minx=curve[i].x-r1[i];
		if(curve[i].x+r1[i]>maxx) maxx=curve[i].x+r1[i];
		if(curve[i].y-r1[i]<miny) miny=curve[i].y-r1[i];
		if(curve[i].y+r1[i]>maxy) maxy=curve[i].y+r1[i];

		if(curve[i].x-r2[i]<minx) minx=curve[i].x-r2[i];
		if(curve[i].x+r2[i]>maxx) maxx=curve[i].x+r2[i];
		if(curve[i].y-r2[i]<miny) miny=curve[i].y-r2[i];
		if(curve[i].y+r2[i]>maxy) maxy=curve[i].y+r2[i];
	}

	_rect rect(
		int(floor(minx)),
		int(floor(miny)),
		int(ceil(maxx)),
		int(ceil(maxy)));
	_rect rp;
	{
		_size size;
		m_image->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		m_image->GetOffset(&ptOffset);
		rp=_rect(ptOffset,size);
	}
	rect.left=min(max(rect.left,rp.left),rp.right);
	rect.top=min(max(rect.top,rp.top),rp.bottom);
	rect.right=min(max(rect.right,rp.left),rp.right);
	rect.bottom=min(max(rect.bottom,rp.top),rp.bottom);

	ptrI->CreateVirtual( rect );
	ptrI->InitRowMasks();
	DWORD dwFlags = 0;
	ptrImg->GetObjectFlags( &dwFlags );
	ptrImg->SetParent( m_image,  dwFlags);
	
	ClipRect=rect;
	smart_alloc(maskRows,byte*,rect.bottom);
	for(y=rect.top; y<rect.bottom; y++)
	{
		ptrI->GetRowMask( y-rect.top, maskRows+y );
		maskRows[y]-=rect.left;
	}

	smart_alloc(poly_x, int, point_num*2+1);
	smart_alloc(poly_y, int, point_num*2+1);

	int *p1x=poly_x, *p2x=poly_x+(point_num*2-1);
	int *p1y=poly_y, *p2y=poly_y+(point_num*2-1);
	for(i=0; i<point_num; i++)
	{
		int i1=max(i-1,0);
		int i2=min(i+1,point_num-1);
		double dx=curve[i2].x-curve[i1].x;
		double dy=curve[i2].y-curve[i1].y;
		double d=max(_hypot(dx,dy),0.01);
		dx/=d; dy/=d;
		*p1x=int(curve[i].x-r1[i]*dy);
		*p1y=int(curve[i].y+r1[i]*dx);
		*p2x=int(curve[i].x+r2[i]*dy);
		*p2y=int(curve[i].y-r2[i]*dx);
		p1x++; p1y++; p2x--; p2y--;
	}
	for(y=rect.top; y<rect.bottom; y++)
	{
		for(int x=rect.left; x<rect.right; x++)
		{
			maskRows[y][x]=0;
		}
	}
	poly_x[point_num*2]=poly_x[0];
	poly_y[point_num*2]=poly_y[0];

	i0=0, i1=point_num-1;
	while(r1[i0]+r2[i0]<=2 && i0<=i1) i0++;
	while(r1[i1]+r2[i1]<=2 && i0<=i1) i1--;

	for(i=i0+1; i<=i1; i++)
	{
		/*
		Line(maskRows,
			poly_x[i],poly_y[i],
			poly_x[point_num*2-1-i],poly_y[point_num*2-1-i],
			255);
			*/
		FillTriangle(maskRows, 
			poly_x[i],poly_y[i],
			poly_x[i-1],poly_y[i-1],
			poly_x[point_num*2-1-i],poly_y[point_num*2-1-i],
			255);
		FillTriangle(maskRows, 
			poly_x[point_num*2-i],poly_y[point_num*2-i],
			poly_x[i-1],poly_y[i-1],
			poly_x[point_num*2-1-i],poly_y[point_num*2-1-i],
			255);
	}

	// 30.05.2002 - сглаживание масок
	{ //медиана 3x3 - чтобы шеф порадовался за ровные края
		int cx,cy;
		ptrI->GetSize(&cx,&cy);
		int wx=cx+2, wy=cy+2;
		smart_alloc(buf,byte,wx*wy); smart_buf.zero(); //на 1 пиксел шире
		for(int y=0; y<cy; y++)
		{
			byte *p;
			byte *m=buf+(y+1)*wx+1;
			ptrI->GetRowMask(y, &p);
			for(int x=0; x<cx; x++)
			{
				if(*p)
				{
					m[-wx-1]++; m[-wx]++; m[-wx+1]++;
					m[-1]++; m[0]++; m[1]++;
					m[wx-1]++; m[wx]++; m[wx+1]++;
				}
				p++; m++;
			}
		}
		for(y=0; y<cy; y++)
		{
			byte *p;
			byte *m=buf+(y+1)*wx+1;
			ptrI->GetRowMask(y, &p);
			for(int x=0; x<cx; x++)
			{
				*p = (*m>=5) ? 255 : 0;
				p++; m++;
			}
		}
	}

	return(ptrI);
}

static IUnknown *CreateAxis(_dpoint *buf, int i0, int i1, int x0, int y0,
				IMeasureObjectPtr ptrO)
{
	//создание оси и центромеры
	IChromosome2Ptr ptrC(CreateChildObjects(ptrO));
	if(ptrC!=0)
	{
		IUnknown *punk;;
		ptrO->GetImage( &punk );
		IImage3Ptr ptrI(punk);
		if(punk) punk->Release();

		_point ptOfs;
		ptrI->GetOffset(&ptOfs);
		int cx,cy;
		ptrI->GetSize(&cx,&cy);
		
		{
			ROTATEPARAMS r;
			memset(&r, 0, sizeof(r));
			r.bMirrorV = TRUE;
			r.bManual=true;
			double dx=buf[i1].x-buf[i0].x;
			double dy=buf[i1].y-buf[i0].y;
			double x0=(buf[i1].x+buf[i0].x)/2-ptOfs.x;
			double y0=(buf[i1].y+buf[i0].y)/2-ptOfs.y;
			if(fabs(dx)<0.1 && fabs(dy)<0.1) dy=0.1;
			double dd=_hypot(dx,dy);
			r.dAngle=atan(dx/dy);
			r.ptCenter=_point(int(x0),int(y0)); //центр прямоугольника
			r.ptCenter=_point(cx/2,cy/2); //центр прямоугольника
			r.ptOffs=_point(0,0); //лев верхн угол
			r.ptOffsR=_point(-10000,-10000); //лев верхн угол

			//r.dAngle=0;
			ptrC->SetRotateParams(&r);
			ptrC->SetOffset(r.ptOffs);
		}
		
		{
			smart_alloc(ax,_point,i1-i0+1);
			smart_alloc(fax,FPOINT,i1-i0+1);

			int j=0;
			for(int i=i0; i<i1; i+=6, j++)
			{
				fax[j].x=buf[i].x-ptOfs.x;
				fax[j].y=buf[i].y-ptOfs.y;
				ax[j]=_point(int(buf[i].x-ptOfs.x+0.5),int(buf[i].y-ptOfs.y+0.5));
			}
			i=i1;
			fax[j].x=buf[i].x-ptOfs.x;
			fax[j].y=buf[i].y-ptOfs.y;
			ax[j]=_point(int(buf[i].x-ptOfs.x+0.5),int(buf[i].y-ptOfs.y+0.5));
			j++;

			if(fax[j-1].y<fax[0].y)
			{ // sort axis by Y
				for(int j1=0; j1<j/2; j1++)
				{
					FPOINT tmp = fax[j1];
					fax[j1] = fax[j-1-j1];
					fax[j-1-j1] = tmp;
				}
			}

			if(0) ptrC->SetAxis(AT_ShortAxis, j, ax, true);
			if(1) ptrC->SetAxis(AT_ShortAxis | AT_AxisDouble, j, (POINT *)fax, true); //!!! warning: явное преобразование типов - см. реализацию SetAxis
			if(0) ptrC->SetAxis(AT_LongAxis | AT_AxisDouble, j, (POINT *)fax, true); //!!! warning: явное преобразование типов - см. реализацию SetAxis
		}
		
		double rc=1e20;
		int ic=(i0+i1)/2;
		for(int i=i0; i<=i1; i++)
		{
			double r=_hypot(buf[i].x-x0,buf[i].y-y0);
			if(r<rc)
			{
				ic=i;
				rc=r;
			}
		}
		
		_point ptCen=_point(int(buf[ic].x)-ptOfs.x,int(buf[ic].y)-ptOfs.y); //точка на оси

		smart_alloc(maskRows, byte*, cy);
		{for(int y=0; y<cy; y++)
		{
			ptrI->GetRowMask(y, &maskRows[y]);
		}}

		int ic1=max(ic-1,i0);
		int ic2=min(ic+1,i1);
		double dx=buf[ic2].x-buf[ic1].x;
		double dy=buf[ic2].y-buf[ic1].y;
		double d=max(_hypot(dx,dy),0.01);
		dx/=d; dy/=d;

		_point ptDraw[2];

		ptDraw[0]=ptCen;
		ptDraw[1]=ptCen;
		{for(int j=2; j<50; j++)
		{
			int xx=int(ptCen.x-j*dy);
			int yy=int(ptCen.y+j*dx);
			if(xx<0 || xx>=cx || yy<0 || yy>=cy) break;
			if(maskRows[yy][xx]<128) break;
			ptDraw[0]=_point(xx,yy);
		}}
		{for(int j=2; j<50; j++)
		{
			int xx=int(ptCen.x+j*dy);
			int yy=int(ptCen.y-j*dx);
			if(xx<0 || xx>=cx || yy<0 || yy>=cy) break;
			if(maskRows[yy][xx]<128) break;
			ptDraw[1]=_point(xx,yy);
		}}

		ptrC->SetCentromereCoord(0, ptCen, ptDraw, false);
		//центромеру ставим так, чтобы при пересчете уползла, куда надо
	}
	//::ConditionalCalcObject( ptrO, 0, true, 0 );
	IUnknown *punkC=ptrC;
	if(punkC!=0) punkC->AddRef();
	return(punkC);
}

bool CSelectByAxisFilter::SetCentromereClick(INamedDataObject2Ptr ptrList, int x0, int y0, IUnknownPtr ptrDoc)
{
	if(ptrList==0) return(false);
	
	double bestDistance=1e20; //наиболее подходящий объект
	POSITION bestObjPos = 0;
	int best_x, best_y;
	int best_i;

	POSITION objPos = 0;
	ptrList->GetFirstChildPosition(&objPos);
	while( objPos ) //по всем объектам
	{
		IUnknownPtr ptr;
		POSITION objPos1 = objPos;
		ptrList->GetNextChild(&objPos, &ptr);
		IMeasureObjectPtr ptrO(ptr);
		if(ptrO==0) continue;

		// Find axis
		IUnknownPtr ptr3 = find_child_by_interface(ptrO, IID_IChromosome2);
		IChromosome2Ptr ptrC(ptr3);
		if(ptrC==0) continue;

		IUnknownPtr ptr2;
		ptrO->GetImage(&ptr2);
		IImage3Ptr ptrI(ptr2);
		if(ptrI==0) continue;

		_point ptOffset;
		ptrI->GetOffset(&ptOffset);
		int cx,cy;
		ptrI->GetSize(&cx,&cy);

		if(x0<ptOffset.x-20) continue; //скипаем заведомо ненужные объекты
		if(y0<ptOffset.y-20) continue;
		if(x0>ptOffset.x+cx+20) continue;
		if(y0>ptOffset.y+cy+20) continue;

        //длина оси
        long lAxisSize=0;
        BOOL bIsManual;
        ptrC->GetAxisSize(AT_LongAxis, &lAxisSize, &bIsManual);

        //получим саму ось
        smart_alloc(axis, _point, lAxisSize);
        ptrC->GetAxis(AT_LongAxis, axis);

        for(int i=0; i<lAxisSize; i++)
		{
			int x=axis[i].x+ptOffset.x;
			int y=axis[i].y+ptOffset.y;
			double r=_hypot(x0-x,y0-y);
			if(r<bestDistance)
			{
				bestDistance=r;
				bestObjPos = objPos1;
				best_x=axis[i].x;
				best_y=axis[i].y;
				best_i=i;
			}
		}
	}
	if(bestDistance<20)
	{
		IUnknownPtr ptr;
		POSITION objPos = bestObjPos;
		ptrList->GetNextChild(&objPos, &ptr);
		IMeasureObjectPtr ptrO(ptr);

		// Find axis
		IUnknownPtr ptr3 = find_child_by_interface(ptrO, IID_IChromosome2);
		IChromosome2Ptr ptrC(ptr3);

		IUnknownPtr ptr2;
		ptrO->GetImage(&ptr2);
		IImage3Ptr ptrI(ptr2);

		_point ptOffset;
		ptrI->GetOffset(&ptOffset);
		int cx,cy;
		ptrI->GetSize(&cx,&cy);

        //длина оси
        long lAxisSize=0;
        BOOL bIsManual;
        ptrC->GetAxisSize(AT_LongAxis, &lAxisSize, &bIsManual);

        //получим саму ось
        smart_alloc(axis, _point, lAxisSize);
        ptrC->GetAxis(AT_LongAxis, axis);

		_point ptCen=_point(best_x,best_y); //точка на оси
		_point ptDraw[2];


		//посчитаем перпендикуляр к оси
		int i0=max(best_i-5,0);
		int i1=min(best_i+5,lAxisSize-1);
		double dx=axis[i1].x-axis[i0].x;
		double dy=axis[i1].y-axis[i0].y;
		double d=max(_hypot(dx,dy),0.1);
		dx/=d; dy/=d;
		ptDraw[0]=_point(ptCen.x+int(5*dy),ptCen.y-int(5*dx));
		ptDraw[1]=_point(ptCen.x-int(5*dy),ptCen.y+int(5*dx));

		{
			for(int i=2; i<20; i++)
			{
				int x,y;
				x=ptCen.x+int(i*dy); y=ptCen.y-int(i*dx);
				if(x>=0 && y>=0 && x<cx && y<cy)
				{
					byte *m;
					ptrI->GetRowMask(y, &m);
					if(m[x]>=128) ptDraw[0]=_point(x,y);
				}
				x=ptCen.x-int(i*dy); y=ptCen.y+int(i*dx);
				if(x>=0 && y>=0 && x<cx && y<cy)
				{
					byte *m;
					ptrI->GetRowMask(y, &m);
					if(m[x]>=128) ptDraw[1]=_point(x,y);
				}
			}
		}

		//создадим новый объект хромосомы
		IUnknown *punk1 = CreateTypedObject(L"ChromosomeAxisAndCentromere");
		// if( !punk1 )return 0; //проверку
		IChromosome2Ptr ptrC_new(punk1);

		::ConditionalCalcObject( ptrO, 0, true, 0 );

		sptrINamedDataObject2 sptrNO1(ptrC_new);
		if(punk1!=0) punk1->Release();
		sptrNO1->SetParent(ptrO, 0);
		/*
		sptrINamedDataObject2 sptrNO2(ptrC);
		sptrNO2->SetParent(0, 0);
		*/

		if(ptrC!=0) RemoveFromDocData(ptrDoc, ptrC);
		if(ptrC_new!=0) SetToDocData( ptrDoc, ptrC_new );

		{
			ROTATEPARAMS r;
			ptrC->GetRotateParams(&r);

			//длина оси
			long lAxisSize=0;
			BOOL bIsManual;
			ptrC->GetAxisSize(AT_ShortAxis, &lAxisSize, &bIsManual);

			//получим саму ось
			smart_alloc(faxis, FPOINT, lAxisSize);
			ptrC->GetAxis(AT_ShortAxis, (POINT*)faxis);

			ptrC_new->SetRotateParams(&r);
			ptrC_new->SetAxis(AT_ShortAxis, lAxisSize, (POINT*)faxis, bIsManual);
		}

		{
			//длина оси
			long lAxisSize=0;
			BOOL bIsManual;
			ptrC->GetAxisSize(AT_LongAxis, &lAxisSize, &bIsManual);

			//получим саму ось
			smart_alloc(faxis, FPOINT, lAxisSize);
			ptrC->GetAxis(AT_LongAxis, (POINT*)faxis);

			ptrC_new->SetAxis(AT_LongAxis, lAxisSize, (POINT*)faxis, bIsManual);
		}

		if(best_i<=0 || best_i>=lAxisSize-1)
		{
			ptDraw[0]=ptCen;
			ptDraw[1]=ptCen;
		}

		CENTROMERE cen;
		cen.ptCen = _fpoint(ptCen);
		cen.aptCenDraw[0] = _fpoint(ptDraw[0]);
		cen.aptCenDraw[1] = _fpoint(ptDraw[1]);
		cen.ptOffset = INVPOINT;
		cen.nCentromere = best_i;
		cen.dAngle = INVANGLE;

		ptrC_new->SetCentromereCoord(0, ptCen, ptDraw, true);
		//ptrC_new->SetCentromereCoord2(0, &cen, true);

		::ConditionalCalcObject( ptrO, 0, true, 0 );

		{
			IDataObjectListPtr ptrObjectList(ptrList);
			if(ptrObjectList!=0)
			{
				ptrObjectList->UpdateObject(ptrO);
			}
		}

		POSITION _lpos = 0;
		ptrList->GetChildPos( ptrO, &_lpos );
		ptrList->SetActiveChild( _lpos );

		IFileDataObject2Ptr	ptrFileObject(ptrDoc);
		if(ptrFileObject!=0) ptrFileObject->SetModifiedFlag( true );

		//::ExecuteAction("AddIdioBend2","",0);

		return true;
	}
	return false;
}

static bool StreamSeek( IStream* pStream, DWORD dwOrigin, long lOffset )
{
	LARGE_INTEGER	uiPos;
	::ZeroMemory( &uiPos, sizeof(LARGE_INTEGER) );
	uiPos.LowPart = lOffset;
	

	if( S_OK != pStream->Seek( uiPos, dwOrigin, 0 ) )
	{
		dbg_assert( 0 );
		return false;
	}

	return true;
}

//clone the data object
static IUnknown *CloneObject( IUnknown *punk )
{
//get object kind
	dbg_assert( CheckInterface( punk, IID_INamedDataObject ) );

	BSTR strObjectKind;
	INamedDataObjectPtr(punk)->GetType(&strObjectKind);
	IUnknown	*punkNew = ::CreateTypedObject( strObjectKind );

//if object support serializable interface, serialize old and deserialize new object

	if( CheckInterface( punk, IID_ISerializableObject ) )
	{
		dbg_assert( CheckInterface( punkNew, IID_ISerializableObject ) );
		//CMemFile	file;

		IStream* pStream = 0;
		::CreateStreamOnHGlobal( 0, TRUE, &pStream );
		if( !pStream )
			return 0;

		sptrISerializableObject	sptrO( punk );
		sptrISerializableObject	sptrN( punkNew );


		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );
		params.flags = sf_DetachParent;

//serialize
		{
			sptrO->Store( pStream, &params );

		}

		::StreamSeek( pStream, 0, 0 );

//de-serialize
		{
			sptrN->Load( pStream, &params );
		}

		pStream->Release();	pStream = 0;
	}
	
	return punkNew;
}

HRESULT CSelectByAxisFilter::DoInvoke()
{
	if(!IsAvaible()) return E_FAIL;

	IUnknownPtr ptrDoc = get_document();

	OnAttachTarget( ptrDoc );

	if(m_action==2)
	{
		IDataContextPtr sptrContext( m_ptrTarget );
		IUnknown* punkObj = 0;	
		sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
		INamedDataObject2Ptr ptrList(punkObj);
		if(punkObj) punkObj->Release();
		if(ptrList==0) return E_FAIL;

		bool bResult=SetCentromereClick(ptrList,m_FirstPoint.x,m_FirstPoint.y, ptrDoc);
		::InvalidateRect(m_hwnd, 0, false);
		if(!bResult) return E_FAIL;

		return S_OK;
	}

	IImage3Ptr	ptrSrcImage = m_image; //( GetDataArgument() );

	if( ptrSrcImage == NULL)
		return E_FAIL;

	int nPaneCount;
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	nPaneCount = ::GetImagePaneCount( ptrSrcImage );


	int buf_remain=4096; //max 4096 points
	smart_alloc(buf, _dpoint, buf_remain);
	_dpoint *buf2=buf;

	for(int i=0; i<m_SplinePointsNum-1; i++)
	{
		_dpoint pt0=m_SplinePoints[i];
		_dpoint pt1=m_SplinePoints[i+1];

		_dpoint ptm=pt0;
		if(i>0)
			ptm=m_SplinePoints[i-1];
		_dpoint pt2=pt1;
		if(i<m_SplinePointsNum-2)
			pt2=m_SplinePoints[i+2];

		/*
		curve2(hDC,
			pt0.x, (pt1.x-ptm.x)*0.66666666,
			pt0.y, (pt1.y-ptm.y)*0.66666666,
			pt1.x, (pt2.x-pt0.x)*0.66666666,
			pt1.y, (pt2.y-pt0.y)*0.66666666);
		*/

		_dpoint v0=calc_d(ptm,pt0,pt1);
		_dpoint v1=calc_d(pt0,pt1,pt2);
		MakeCurve2(&buf2, &buf_remain,
			pt0.x, v0.x,
			pt0.y, v0.y,
			pt1.x, v1.x,
			pt1.y, v1.y);
	}

	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			ptrSrcImage->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}
	

	if(buf2==buf) return E_FAIL;

	IDataContextPtr sptrContext( m_ptrTarget );
	
	{
		//проверить - может, не надо создавать новый объект
		bool bNewObject=true;
		POSITION bestPos = 0;

		int bCreateAxis=GetValueInt(GetAppUnknown(),
			"\\Chromos\\SelectByAxis", "CreateAxis", 1);


		//if(bCreateAxis) //разрешить редактировать оси, только если разрешено их создание
		{

			IUnknown* punkObj = 0;	
			sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
			INamedDataObject2Ptr ptrList(punkObj);
			if(punkObj) punkObj->Release();

			if(ptrList!=0)
			{
				double bestInRatio=0; //наиболее подходящий объект
				double sumInRatio=0; //сумма процентов кусков оси по хромосомам (если >0.8 - правим лучшую)
				POSITION objPos = 0;
				ptrList->GetFirstChildPosition(&objPos);
				while( objPos ) //по всем объектам
				{
					IUnknownPtr sptr;
					POSITION objPos1 = objPos;
					ptrList->GetNextChild(&objPos, &sptr);
					IMeasureObjectPtr ptrO(sptr);
					if(ptrO==0) continue;
					IUnknownPtr sptr2;
					ptrO->GetImage(&sptr2);
					IImage3Ptr ptrI(sptr2);
					if(ptrI==0) continue;

					_rect cr;
					_size size;
					ptrI->GetSize((int*)&size.cx,(int*)&size.cy);
					_point ptOffset;
					ptrI->GetOffset(&ptOffset);
					cr=_rect(ptOffset,size);

					smart_alloc(maskRows, byte*, size.cy);
					{for(int y=0; y<size.cy; y++)
					{
						ptrI->GetRowMask(y, &maskRows[y]);
					}}

					int inCount=0; //число точек оси, попавших внутрь объекта
					for(int i=0; i<buf2-buf; i++)
					{
						int x=int(buf[i].x-ptOffset.x);
						int y=int(buf[i].y-ptOffset.y);
						if(x>=0 && x<size.cx && y>=0 && y<size.cy)
						{
							if(maskRows[y][x]>=128) inCount++;
						}
					}
					double inRatio=double(inCount)/max(buf2-buf,1);
					sumInRatio+=inRatio;					
					if(inRatio>bestInRatio)
					{
						bestInRatio=inRatio;
						bestPos=objPos1;
					}
				}
				if(bestInRatio>0.5) bNewObject=false;
				//если >50% линии по одному объекту - правим его
			}
		}

		if(bNewObject)
		{
			int i0=0, i1=buf2-buf-1;
			IImage3Ptr ptrI(CreateObjectImage(buf,buf2-buf, i0,i1));

			if(ptrI==0) return E_FAIL; //если не вернули объект - значит, не судьба
			if(i1-i0<2) return E_FAIL; //не создаем пустых объектов

			// проверить новый объект на пересечение со старыми
			IUnknown* punkObj = 0;
			sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
			INamedDataObject2Ptr ptrList(punkObj);

			_size size;
			ptrI->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			ptrI->GetOffset(&ptOffset);
			_rect cr(ptOffset,size);

			int nArea=0;
			for(int y=0; y<size.cy; y++)
			{
				byte *pb;
				ptrI->GetRowMask(y, &pb);
				for(int x=0; x<size.cx; x++)
				{
					if(*pb>=128) nArea++;
					pb++;
				}
			}

			int nMaxIntersect=0;
			int nIntersectSum=0;
			if(ptrList!=0)
			{
				POSITION objPos = 0;
				ptrList->GetFirstChildPosition(&objPos);
				while( objPos ) //по всем объектам
				{
					IUnknownPtr sptr;
					POSITION objPos1 = objPos;
					ptrList->GetNextChild(&objPos, &sptr);
					IMeasureObjectPtr ptrO(sptr);
					if(ptrO==0) continue;
					IUnknownPtr sptr2;
					ptrO->GetImage(&sptr2);
					IImage3Ptr ptrI1(sptr2);
					if(ptrI1==0) continue;

					_size size1(0,0);
					ptrI1->GetSize((int*)&size1.cx,(int*)&size1.cy);
					_point ptOffset1(0,0);
					ptrI1->GetOffset(&ptOffset1);
					_rect cr1(_rect(ptOffset1,size1));

					_rect cr2( // пересечение ректов
						max(cr.left,cr1.left), max(cr.top,cr1.top),
						min(cr.right,cr1.right), min(cr.bottom,cr1.bottom) );

					int nIntersect=0;
					for(int y=cr2.top; y<cr2.bottom; y++)
					{
						byte *pb,*pb1;
						ptrI->GetRowMask(y-cr.top, &pb); pb += cr2.left-cr.left;
						ptrI1->GetRowMask(y-cr1.top, &pb1); pb1 += cr2.left-cr1.left;
						for(int x=cr2.left; x<cr2.right; x++)
						{
							if((*pb>=128) && (*pb1>=128)) nIntersect++;
							pb++; pb1++;
						}
					}
					if(nIntersect>nMaxIntersect) nMaxIntersect=nIntersect;
					nIntersectSum += nIntersect;
				}
			}

			//if(nMaxIntersect>nArea/2)
			//	return E_FAIL; // юзер дурак - провел так, что выделилась в основном часть имеющегося объекта
			if(nIntersectSum>nArea/2)
				return E_FAIL; // юзер дурак - провел так, что выделилась в основном часть имеющегося объекта



			//-------------------- object
			_bstr_t s(szTypeObject);
			IUnknown *punk = ::CreateTypedObject(s);
			IMeasureObjectPtr ptrO(punk);
			punk->Release(); punk=0;
			
			INamedDataObject2Ptr ptrObject(ptrO);
			ptrO->SetImage( ptrI );
			ptrI->InitContours();

			if(bCreateAxis)
			{
				CreateAxis(buf, i0, i1, m_FirstPoint.x, m_FirstPoint.y, ptrO);
			}

			//IDataContextPtr sptrContext( m_ptrTarget );
			IUnknown* punkObj1 = 0;
			
			sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj1);
			INamedDataObject2Ptr ptrOldList(punkObj1);
			if(punkObj1) punkObj1->Release();

			SetToDocData( ptrDoc, ptrO );

			if(ptrOldList==0)
			{
				INamedDataObject2Ptr ptrSrcImageNamed = INamedDataObject2Ptr(ptrSrcImage);

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

			if(bCreateAxis)
			{
				::ConditionalCalcObject( ptrO, 0, true, 0 );
			}

			{
				IUnknown *punk;
				INamedDataObject2Ptr ptrObj(ptrO);
				ptrObj->GetParent(&punk);
				INamedDataObject2Ptr ptrList(punk);
				if(punk!=0) punk->Release(); punk=0;
				IDataContextPtr sptrContext( m_ptrTarget );
				sptrContext->SetActiveObject(_bstr_t(szTypeObjectList), ptrList, aofActivateDepended);
			}

			IFileDataObject2Ptr	ptrFileObject(ptrDoc);
			if(ptrFileObject!=0) ptrFileObject->SetModifiedFlag( true );
			//SetModified( true );
		}
		else
		{
			IDataContextPtr sptrContext( m_ptrTarget );
			IUnknown* punkObj = 0;
			
			sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
			INamedDataObject2Ptr ptrList(punkObj);
			if(punkObj) punkObj->Release();

			IUnknownPtr sptr;

			//ptrList->SetActiveChild(bestPos);
			ptrList->GetNextChild(&bestPos, &sptr);
			IMeasureObjectPtr ptrO(sptr);

			//IMeasureObjectPtr ptrClone(CloneObject(ptrO));

			//удалим старую ось, если есть
			IUnknownPtr ptrFound = find_child_by_interface(ptrO, IID_IChromosome2);
			if(ptrFound)
			{
				RemoveFromDocData(ptrDoc, ptrFound);
			}

			//создание оси и центромеры
			IUnknown *punkC=CreateAxis(buf, 0, buf2-buf-1, m_FirstPoint.x, m_FirstPoint.y, ptrO);
			IChromosome2Ptr ptrC(punkC);
			if(punkC!=0) punkC->Release();
	
			if(ptrC!=0) SetToDocData( ptrDoc, ptrC );

			::ConditionalCalcObject( ptrO, 0, true, 0 );

			POSITION _lpos = 0;
			ptrList->GetChildPos( ptrO, &_lpos );
			ptrList->SetActiveChild( _lpos );

			//::ExecuteAction("AddIdioBend2","",0);

			IFileDataObject2Ptr	ptrFileObject(ptrDoc);
			if(ptrFileObject!=0) ptrFileObject->SetModifiedFlag( true );
			//SetModified( true );
		}
	}

	return S_OK;
}

HRESULT CSelectByAxisFilter::Paint( HDC hDC, RECT rectDraw, 
						  IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, 
						  BYTE *pdibCache )
{
	IScrollZoomSitePtr	ptrScrollZoom(punkTarget);

	COLORREF c_axis = GetValueColor(GetAppUnknown(), "\\Chromos\\SelectByAxis", "AxisColor", RGB( 0, 128, 255 ));
	HPEN hPen = ::CreatePen( PS_SOLID, 0, c_axis );
	//::GetStockObject(WHITE_PEN)

	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen);

	bool bPaintPoints=false;

	if(bPaintPoints || m_SplinePointsNum==1)
	{
		for(int i=0; i<m_SplinePointsNum; i++)
		{		
			_point pt=_window(m_SplinePoints[i], ptrScrollZoom);
			::Rectangle(hDC, pt.x-2,pt.y-2,pt.x+2,pt.y+2);
		}

		if(m_SplinePointsNum>0)
		{
			i=m_SplinePointsNum-1;
			_point pt=_window(m_SplinePoints[i], ptrScrollZoom);
			::Rectangle(hDC, pt.x-3,pt.y-3,pt.x+3,pt.y+3);
		}
	}

	int buf_remain=4096; //max 4096 points
	smart_alloc(buf, _dpoint, buf_remain);
	_dpoint *buf2=buf;

	for(int i=0; i<m_SplinePointsNum-1; i++)
	{
		_point pt0=_window(m_SplinePoints[i], ptrScrollZoom);
		_point pt1=_window(m_SplinePoints[i+1], ptrScrollZoom);

		_point ptm=pt0;
		if(i>0)
			ptm=_window(m_SplinePoints[i-1], ptrScrollZoom);
		_point pt2=pt1;
		if(i<m_SplinePointsNum-2)
			pt2=_window(m_SplinePoints[i+2], ptrScrollZoom);

		/*
		curve2(hDC,
			pt0.x, (pt1.x-ptm.x)*0.66666666,
			pt0.y, (pt1.y-ptm.y)*0.66666666,
			pt1.x, (pt2.x-pt0.x)*0.66666666,
			pt1.y, (pt2.y-pt0.y)*0.66666666);
		*/

		_dpoint v0=calc_d(ptm,pt0,pt1);
		_dpoint v1=calc_d(pt0,pt1,pt2);
		MakeCurve2(&buf2, &buf_remain,
			pt0.x, v0.x,
			pt0.y, v0.y,
			pt1.x, v1.x,
			pt1.y, v1.y);
	}

	for(i=0; i<buf2-buf; i++)
	{
		//::Rectangle(hDC, buf[i].x-1,buf[i].y-1,buf[i].x+1,buf[i].y+1);
		::MoveToEx(hDC, (int)buf[i].x,(int)buf[i].y, 0);
		::LineTo(hDC, (int)buf[i].x+1,(int)buf[i].y+1);
	}

	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen);

	return S_OK;
}

HRESULT CSelectByAxisFilter::GetRect( RECT *prect )
{
	if(prect)
	{
		//*prect = _rect(
			//m_FirstPoint.x-10,m_FirstPoint.y-10,
			//m_FirstPoint.x+10,m_FirstPoint.y+10);
		//return S_OK;
		*prect=_rect(INT_MIN,INT_MIN, INT_MAX,INT_MAX);
		return(NOERROR);
		//return(CInteractiveAction::GetRect(prect));
	}

	return E_INVALIDARG;
}