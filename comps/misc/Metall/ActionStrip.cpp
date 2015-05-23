#include "StdAfx.h"
#include "actionstrip.h"
#include "misc_utils.h"
#include "nameconsts.h"
#include "math.h"
#include "alloc.h"
#include "docview5.h"

_ainfo_base::arg        CActionStripInfo::s_pargs[] =
{
	{"Img",         szArgumentTypeImage, 0, true, true },
	{0, 0, 0, false, false },
};

CActionStrip::CActionStrip(void)
{
	m_nStripSize=0;
	m_nDrawingState=0;
	m_ptrImage=0;
	m_ptrList=0;
	m_ptrObjectOld=0;
	m_ptLast=_point(0,0);
	m_nMinHeight=1;
	m_nLeft=m_nRight=0;
}

CActionStrip::~CActionStrip(void)
{
}

IUnknown* CActionStrip::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

HRESULT CActionStrip::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CActionStrip::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );

	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	if(!punkObj) return false;
	if(punkObj) punkObj->Release();
	
	return true;
}

bool CActionStrip::DoLButtonDown( _point point )
{
	int x = max(0, min(m_nStripSize-1, point.x));
	int y = point.y;
	if(y<(m_StripBottom[x]+m_StripTop[x]+1)/2)
		m_nDrawingState=1; // top line
	else
		m_nDrawingState=2; // bottom line

	SetPoint(x,y);

	::InvalidateRect(m_hwnd, 0, false); //лучше бы сделать нормально - чтоб не тормозило

	m_ptLast = _point(x,y);

	return true;
	//return __super::DoLButtonDown( point );
}

bool CActionStrip::DoRButtonDown( _point point )
{
	Finalize();
	return true;
}

bool CActionStrip::DoLButtonDblClick( _point point )
{
	Finalize();
	return true;
}

bool CActionStrip::DoLButtonUp( _point point )
{
	if(m_nDrawingState!=0)
	{
		m_nDrawingState=0; // конец рисовани€
		Finalize();
	}
	return true;
}

bool CActionStrip::DoMouseMove( _point point )
{
	return __super::DoMouseMove( point );
}

bool CActionStrip::SetPoint(int x, double y)
{
	double *pStrip;
	if(m_nDrawingState==1) pStrip=m_StripTop;
	else if(m_nDrawingState==2) pStrip=m_StripBottom;
	else return false;

	int cx=0,cy=0;
	m_ptrImage->GetSize(&cx, &cy);

	if(m_nDrawingState==1) y = max(0, min(cy-1-m_nMinHeight, y));
	if(m_nDrawingState==2) y = max(m_nMinHeight, min(cy-1, y));

	for(int i = -32; i<=32; i++)
	{
		int xx=x+i;
		if(xx<0 || xx>=m_nStripSize) continue;
		
		double max_dy=abs(i)+double(i*i)/5;

		if(pStrip[xx]>y+max_dy) pStrip[xx] = y+max_dy;
		if(pStrip[xx]<y-max_dy) pStrip[xx] = y-max_dy;
		if(m_nDrawingState==1)
		{
			m_StripBottom[xx] = max(m_StripBottom[xx], m_StripTop[xx]+m_nMinHeight);
			if(xx>0) m_StripBottom[xx-1] = max(m_StripBottom[xx-1], m_StripTop[xx]+2);
			if(xx>1) m_StripBottom[xx-2] = max(m_StripBottom[xx-2], m_StripTop[xx]+2);
			if(xx<m_nStripSize-1) m_StripBottom[xx+1] = max(m_StripBottom[xx+1], m_StripTop[xx]+2);
			if(xx<m_nStripSize-2) m_StripBottom[xx+2] = max(m_StripBottom[xx+2], m_StripTop[xx]+2);
		}
		if(m_nDrawingState==2)
		{
			m_StripTop[xx] = min(m_StripTop[xx], m_StripBottom[xx]-m_nMinHeight);
			if(xx>0) m_StripTop[xx-1] = min(m_StripTop[xx-1], m_StripBottom[xx]-2);
			if(xx>1) m_StripTop[xx-2] = min(m_StripTop[xx-2], m_StripBottom[xx]-2);
			if(xx<m_nStripSize-1) m_StripTop[xx+1] = min(m_StripTop[xx+1], m_StripBottom[xx]-2);
			if(xx<m_nStripSize-2) m_StripTop[xx+2] = min(m_StripTop[xx+2], m_StripBottom[xx]-2);
		}
	}

	if(x<m_nLeft) m_nLeft=x;
	if(x>m_nRight) m_nRight=x;

	for(int i=0; i<m_nStripSize; i++)
	{
		if(i<m_nLeft)
		{
			m_StripTop[i]=m_StripTop[m_nLeft];
			m_StripBottom[i]=m_StripBottom[m_nLeft];
		}
		if(i>m_nRight)
		{
			m_StripTop[i]=m_StripTop[m_nRight];
			m_StripBottom[i]=m_StripBottom[m_nRight];
		}
	}

	return true;
}

bool CActionStrip::DoTrack( _point point )
{
	int cx=0,cy=0;
	m_ptrImage->GetSize(&cx, &cy);

	int x0 = max(0, min(m_nStripSize-1, point.x));
	int y0 = max(0, min(cy-1, point.y));

	int len=abs(x0-m_ptLast.x);
	len=max(len,1);

	for(int i=len-1; i>=0; i--)
	{
		int x = x0 + (m_ptLast.x-x0)*i/len;
		double y = y0 + double(m_ptLast.y-y0)*i/len;
		SetPoint(x,y);
	}

	::InvalidateRect(m_hwnd, 0, false); //лучше бы сделать нормально - чтоб не тормозило

	m_ptLast = _point(x0,y0);

	return true;
}

bool CActionStrip::DoStartTracking( _point point )
{
	return __super::DoStartTracking(point);

}

bool CActionStrip::DoFinishTracking( _point point )
{
	Finalize();
	return true;
}

void CActionStrip::EnsureVisible( _point point )
{
	IScrollZoomSitePtr ptrScrollZoom(m_ptrTarget);
	if(ptrScrollZoom==0) return;
	ptrScrollZoom->EnsureVisible(_rect(point,_size(1,1)));
}

LRESULT	CActionStrip::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	LRESULT bResult = false;

	if( nMsg == WM_SETCURSOR )
	{
		::SetCursor(m_hCurActive);
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

bool CActionStrip::Initialize()
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

	if(m_ptrList!=0)
	{
		POSITION pos = 0;
		m_ptrList->GetFirstChildPosition(&pos);
		if( pos )
		{
			IUnknownPtr sptr;
			m_ptrList->GetNextChild(&pos, &sptr);
			m_ptrObjectOld = IMeasureObjectPtr(sptr);
		}
	}

	int cx=0,cy=0;
	m_ptrImage->GetSize(&cx, &cy);
	m_nStripSize=cx;
	m_StripTop.alloc(m_nStripSize);
	m_StripBottom.alloc(m_nStripSize);
	for(int i=0; i<m_nStripSize; i++)
	{
		m_StripTop[i]=cy+1;
		m_StripBottom[i]=-1;
	}

	m_nLeft=cx-1;
	m_nRight=0; // выставили наоборот - ни одного готового пиксела

	if(m_ptrObjectOld!=0)
	{	// вз€ть маску из объекта
		IUnknown *punk2=0;
		m_ptrObjectOld->GetImage( &punk2 );
		IImage4Ptr      ptrI(punk2);
		if(punk2!=0) punk2->Release();
		if(ptrI!=0)
		{
			_point ptOffset2(0,0);
			ptrI->GetOffset(&ptOffset2);
			int cx2=0, cy2=0;
			ptrI->GetSize(&cx2, &cy2);
			_rect rect(ptOffset2,_size(cx2,cy2));
			for(int y=rect.top; y<rect.bottom; y++)
			{
				byte *pm;
				ptrI->GetRowMask(y-rect.top, &pm);
				for(int x=rect.left; x<rect.right; x++)
				{
					if(x>=0 && x<m_nStripSize)
					{
						if(*pm>=128)
						{ // не оптимально, и фиг с ним
							m_StripTop[x] = min(m_StripTop[x],y);
							m_StripBottom[x] = max(m_StripBottom[x],y);
							m_nLeft = min(m_nLeft,x);
							m_nRight = max(m_nRight,x);
						}
					}
					pm++;
				}
			}
		}
		INamedDataObject2Ptr ptrObjectOldNDO(m_ptrObjectOld);
		if(ptrObjectOldNDO!=0)
			ptrObjectOldNDO->SetParent( 0,  apfNotifyNamedData);
	}

	for(int i=0; i<m_nStripSize; i++)
	{
		if(m_StripTop[i]>cy) m_StripTop[i]=-10; // не попавшие в объект - наверх...
	}

	for(int i=0; i<m_nStripSize; i++)
	{
		if(m_StripTop[i]==cx+1) m_StripTop[i]=0;
		if(m_StripBottom[i]==-1) m_StripBottom[i]=0;
	}

	//m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORCROSSDIV));

	m_nMinHeight = GetValueInt(GetAppUnknown(), "\\Metall\\Strip", "MinHeight", 10);
	m_clHilightColor = GetValueColor(GetAppUnknown(), "\\Metall\\Strip", "HilightColor", 0x00FF00);
	m_nHilightPercent = GetValueInt(GetAppUnknown(), "\\Metall\\Strip", "HilightPercent", 25);

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORSTRIP));

	::InvalidateRect(m_hwnd, 0, false);
	
	return true;
}

bool CActionStrip::Finalize()
{
	if(m_ptrList!=0 && m_ptrObjectOld!=0)
	{
		INamedDataObject2Ptr ptrObjectOldNDO(m_ptrObjectOld);
		if(ptrObjectOldNDO!=0)
			ptrObjectOldNDO->SetParent( m_ptrList,  apfNotifyNamedData);
	}
	::InvalidateRect(m_hwnd, 0, false);
	
	if( !CInteractiveAction::Finalize() )
		return false;
	
	return true;
}

HRESULT CActionStrip::Paint( HDC hDC, RECT rectDraw,
							IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache,
							BYTE *pdibCache )
{
	IScrollZoomSitePtr      ptrScrollZoom(punkTarget);
	if(ptrScrollZoom == 0) return E_FAIL;

	double fZoom = 1.;
	_point ptScroll(0, 0);
	if(ptrScrollZoom != 0) 
	{
		ptrScrollZoom->GetZoom(&fZoom);
		ptrScrollZoom->GetScrollPos(&ptScroll);
	}

	int bi_w34=(pbiCache->biWidth*3+3)/4*4;

	{ // подсветка
		smart_alloc(y0,int,pbiCache->biWidth);
		smart_alloc(y1,int,pbiCache->biWidth);
		for(int x=0; x<pbiCache->biWidth; x++)
		{
			int x_client = int( (x+rectDraw.left+ptScroll.x)/fZoom );
			if(x_client<0 || x_client>=m_nStripSize)
			{
				y0[x]=-1; y1[x]=-1;
				continue;
			}
			double y_client = m_StripTop[x_client];
            y0[x]=int( y_client*fZoom+.5-ptScroll.y ) - rectDraw.top;
			y_client = m_StripBottom[x_client];
            y1[x]=int( y_client*fZoom+.5-ptScroll.y ) - rectDraw.top;
		}
		int r=GetRValue(m_clHilightColor);
		int g=GetGValue(m_clHilightColor);
		int b=GetBValue(m_clHilightColor);
		int alpha = (100-m_nHilightPercent)*256/100;
		for(int y=0; y<pbiCache->biHeight; y++)
		{
			byte *pb = pdibCache + bi_w34*(pbiCache->biHeight-1-y);
			for(int x=0; x<pbiCache->biWidth; x++)
			{
				if(y>=y0[x] && y<=y1[x])
				{
					int bright = max(max(pb[0],pb[1]),pb[2]);
					pb[0] = b + (pb[0]-r) * alpha / 256;
					pb[1] = g + (pb[1]-g) * alpha / 256;
					pb[2] = r + (pb[2]-b) * alpha / 256;
				}
				pb+=3;
			}
		}
	}

	COLORREF cLine = GetValueColor(GetAppUnknown(), "\\Metall\\Strip", "TopLineColor", RGB( 255, 128, 128 ));
	int nWidth = GetValueInt(GetAppUnknown(), "\\Metall\\Strip", "TopLineWidth", 1);

	HPEN hPen = ::CreatePen( PS_SOLID, nWidth, cLine );
	
	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen);
	int old_rop2=::GetROP2(hDC);

	::SetROP2(hDC,R2_COPYPEN);

	_point pt=_window(_point(0,int(m_StripTop[0]+0.5)), ptrScrollZoom);
	::MoveToEx(hDC, pt.x,pt.y, 0);
	for(int i=1; i<m_nStripSize; i++)
	{
		_point pt=_window(_point(i,int(m_StripTop[i]+0.5)), ptrScrollZoom);
		::LineTo(hDC, pt.x,pt.y);
	}

	COLORREF cLine2 = GetValueColor(GetAppUnknown(), "\\Metall\\Strip", "BottomLineColor", RGB( 255, 128, 128 ));
	int nWidth2 = GetValueInt(GetAppUnknown(), "\\Metall\\Strip", "BottomLineWidth", 1);

	HPEN hPen2 = ::CreatePen( PS_SOLID, nWidth2, cLine2 );
	
	::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	::SelectObject(hDC, hPen2);

	pt=_window(_point(0,int(m_StripBottom[0]+0.5)), ptrScrollZoom);
	::MoveToEx(hDC, pt.x,pt.y, 0);
	for(int i=1; i<m_nStripSize; i++)
	{
		_point pt=_window(_point(i,int(m_StripBottom[i]+0.5)), ptrScrollZoom);
		::LineTo(hDC, pt.x,pt.y);
	}

	::SetROP2(hDC,old_rop2);
	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen);
	::DeleteObject(hPen2);
	
	return S_OK;
}

HRESULT CActionStrip::GetRect( RECT *prect )
{
	if(prect)
	{
		prect->left=0;
		prect->right=16383;
		prect->top=0;
		prect->bottom=16383;
		return S_OK;
	}
	
	return E_INVALIDARG;
}

IUnknownPtr CActionStrip::get_document()
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

HRESULT CActionStrip::DoInvoke()
{
	IUnknownPtr ptrDoc = get_document();
	OnAttachTarget( ptrDoc );
	
	if(m_ptrImage==0) return E_FAIL;

	int cx=0,cy=0;
	m_ptrImage->GetSize(&cx, &cy);
	if(cx!=m_nStripSize) return E_FAIL; // на вс€кий пожарный

	if(m_ptrObjectOld==0)
	{ // если верхн€€ лини€ не проведена
		double sum=0,sum2=0;
		color *pc;
		m_ptrImage->GetRow(0, 0, &pc);

		for(int x=0; x<cx; x++)
		{
			sum += pc[x];
			sum2 += double(pc[x])*pc[x];
		}
		sum /= max(cx,1);
		sum2 /= max(cx,1);
		sum2 -= sum*sum;
		sum2 = sqrt( max(sum2,1) );

		double fTolerance = GetValueDouble(GetAppUnknown(), "\\Metall\\Strip", "BackgroundTolerance", 4);
		int threshold = int(sum + sum2*fTolerance);

		for(int x=0; x<cx; x++) m_StripTop[x]=0; //m_StripBottom[x]-m_nMinHeight;

		for(int y=cy-1; y>=0; y--)
		{
			color *pc;
			m_ptrImage->GetRow(y, 0, &pc);
			for(int x=0; x<cx; x++)
			{
				if(y>=m_StripBottom[x]-m_nMinHeight) continue;
				if(x>0 && y>m_StripBottom[x-1]-2) continue;
				if(x<cx-1 && y>m_StripBottom[x+1]-2) continue;
				if(pc[x]>=threshold) m_StripTop[x]=y;
			}
		}
		smart_alloc(SmoothStrip,double,cx+1);
		SmoothStrip[0]=0;
		for(int x=0; x<cx; x++) SmoothStrip[x+1] = SmoothStrip[x]+m_StripTop[x];
		for(int x=0; x<cx; x++)
		{
			int x0=max(x-10,0);
			int x1=min(x+10,cx-1);
			m_StripTop[x] = (SmoothStrip[x1+1] - SmoothStrip[x0])/(x1-x0+1);
		}
	}

	int y_min=cy-1, y_max=0;
	for(int x=0; x<m_nStripSize; x++)
	{
		y_min = min(y_min,int(m_StripTop[x]));
		y_max = max(y_max,int(m_StripBottom[x]));
	}

	y_min=max(y_min,0);
	y_max=min(y_max,cy-1);
	if(y_min>y_max) return E_FAIL;

	_rect rect(0,y_min,cx,y_max+1);

	//-------------------- image
	IUnknown        *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr      ptrI(punk2);
	if(punk2!=0) punk2->Release();
	INamedDataObject2Ptr ptrImg(ptrI);
	ptrI->CreateVirtual( rect );
	ptrI->InitRowMasks();
	//ptrI->InitContours();
	ptrImg->SetParent( m_ptrImage,  0);

	for(int y=rect.top; y<rect.bottom; y++)
	{
		byte *pm;
		ptrI->GetRowMask(y-rect.top, &pm);
		for(int x=rect.left; x<rect.right; x++)
		{
			if(x>=0 && x<m_nStripSize)
			{
				if(y>=m_StripTop[x] && y<=m_StripBottom[x]) *pm=255;
				else *pm=0;
			}
			else *pm=0;
			pm++;
		}
	}
	ptrI->InitContours();

	//-------------------- object
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr ptrO(punk);
	if(punk!=0) punk->Release();

	ptrO->SetImage( ptrI );
	INamedDataObject2Ptr ptrObject(ptrO);

	INamedDataObject2Ptr ptrObjectOldNDO(m_ptrObjectOld);
	if(ptrObjectOldNDO!=0)
		ptrObjectOldNDO->SetParent( 0,  apfNotifyNamedData);
	SetToDocData(ptrDoc, ptrO);

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

	//if( ptrSrcImage == NULL) return false;
	
	return S_OK;
}
