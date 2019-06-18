#include "stdafx.h"
#include "GlueObj.h"
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
#include "ContourApi.h"
#include "chrom_aam_utils.h"

#include "docview5.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              GlueObj
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CGlueObjInfo::s_pargs[] =
{
	{"Img",         szArgumentTypeImage, 0, true, true },
	{"MeasObjects",    szArgumentTypeObjectList, 0, true, true },
	{"MeasObjectsRes",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

CGlueObjFilter::CGlueObjFilter()
{
	m_ObjectUnk=0;
	m_Object1Unk=0;
	m_Object2Unk=0;
	//m_Image=0;
	m_List=0;
}

CGlueObjFilter::~CGlueObjFilter()
{
	m_ObjectUnk=0;
	m_Object1Unk=0;
	m_Object2Unk=0;
	//m_Image=0;
	m_List=0;
}

IUnknown* CGlueObjFilter::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

bool CGlueObjFilter::Initialize()
{
	if(!CInteractiveAction::Initialize()) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORGLUEOBJ));
	
	return true;
}

void CGlueObjFilter::DeInit()
{
}

bool CGlueObjFilter::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	
	if( !CInteractiveAction::Finalize() )
		return false;
	
	return true;
}

HRESULT CGlueObjFilter::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CGlueObjFilter::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	
	//sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	//if(!punkObj) return false;
	//if(punkObj) punkObj->Release();
	//не провер€ем наличи€ image
	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	if(!punkObj) return false;
	// [max] bt: 3561
	if( punkObj )
	{
		INamedDataObject2Ptr sptrL = punkObj;
		punkObj->Release();

		long lCount = 0;
        sptrL->GetChildsCount( &lCount );

		if( !lCount )
			return false;
	}

	
	return true;
}

bool CGlueObjFilter::DoStartTracking( _point point )
{
	return true;
}

bool CGlueObjFilter::DoTrack( _point point )
{
	return true;
}


bool CGlueObjFilter::DoFinishTracking( _point point )
{
	return true;
}

bool CGlueObjFilter::DoLButtonDblClick( _point point )
{
	Finalize();
	
	return true;
}

/*
bool CGlueObjFilter::DoRButtonDown( _point point )
{
	
	return false;
}

bool CGlueObjFilter::DoRButtonUp( _point point )
{
	//Finalize();
	return false;
}
*/

bool CGlueObjFilter::DoLButtonDown( _point point )
{
	if(IsAvaible()) GlueTouch(point);
	return true;
}

LRESULT	CGlueObjFilter::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	int bResult = false;

	if( nMsg == WM_SETCURSOR )
	{
		::SetCursor(m_hCurActive);
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

bool CGlueObjFilter::DoMouseMove( _point point )
{
	//return true; //не давать гадам обрабатывать
	return CInteractiveAction::DoMouseMove(point);
}

bool CGlueObjFilter::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;
	
	return false;
}

void CGlueObjFilter::GlueTouch(_point pt)
{
	m_ObjectUnk=0;
	m_Object1Unk=0;
	m_Object2Unk=0;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	
	//sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	//m_Image = punkObj;
	//if(punkObj)     punkObj->Release();
	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	m_List = punkObj;
	if(punkObj)     punkObj->Release();
	
	long pos = 0;
	m_List->GetChildsCount(&pos);//
	m_List->GetFirstChildPosition(&pos);
	_point pt_best1, pt_best2;

	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		m_List->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr ptrO(sptr);
		m_ObjectUnk=IUnknownPtr(sptr); //!!!вы€снить - сбрасываетс€ ли счетчик у старого значени€ при присваивании
		IUnknownPtr sptr2;
		ptrO->GetImage(&sptr2);
		IImage3Ptr ptrI(sptr2);
		
		_rect cr;
		{
			_size size;
			ptrI->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			ptrI->GetOffset(&ptOffset);
			cr=_rect(ptOffset,size);
		}
		
		int r_min1=10000,r_min2=10000;
		//проверить, зацепили ли текущий объект
		int r_min=10000; //наименьшее рассто€ние от объекта
		_point pt_best;
		int x0=max(pt.x-10,cr.left), y0=max(pt.y-10,cr.top);
		int x1=min(pt.x+10,cr.right-1), y1=min(pt.y+10,cr.bottom-1);
		//ищем в пределах 10 точек от нужной, внутри объектов
		if(x1>=x0 && y1>=y0)
		{ //не выполн€ть лишней работы
			for(int y=y0; y<=y1; y++)
			{
				BYTE *pbyte;
				ptrI->GetRowMask(y-cr.top,&pbyte);
				pbyte-=cr.left;
				for(int x=x0; x<=x1; x++)
				{
					if(pbyte[x])
					{
						int r=(x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y);
						if(r<r_min) 
						{
							r_min=r;
							pt_best.x=x; pt_best.y=y;
						}
					}
				}
			}
		}
		if(r_min<5*5)
		{
			if(m_Object1Unk==0) 
			{
				m_Object1Unk=m_ObjectUnk;
				pt_best1=pt_best;
			}
			else if(m_Object2Unk==0) 
			{
				m_Object2Unk=m_ObjectUnk;
				pt_best2=pt_best;
			}
		}
	}

	IUnknownPtr ptrDoc = get_document();
	if(m_Object2Unk!=0 || m_Object2Unk==m_Object1Unk)
	{
		GlueTwoObj(pt,m_Object1Unk,m_Object2Unk,pt_best1,pt_best2);
		if(m_Object1Unk!=0) RemoveFromDocData( ptrDoc, m_Object1Unk );
		if(m_Object2Unk!=0) RemoveFromDocData( ptrDoc, m_Object2Unk );
		Finalize();
	}
	else
	{
		//Do nothing, if there is less than 2 objects
		//GlueTwoObj(pt,m_Object1Unk,m_Object1Unk,pt_best1,pt_best1);
	}
	
}

static bool GlueOneImage(IImage3Ptr ptrI, IImage3Ptr ptrI1, byte mask1, bool bFillImage)
//вклеить ptrI1 поверх ptrI
//ptrI заведомо должен полностью перекрывать ptrI1!!!
//если изображени€ несовместимы или еще кака€ б€ка - возвращает false
{
	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		ptrI->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( ptrI );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}

	_bstr_t	bstrCC1;
	{
		IUnknown *punkCC = 0;
		ptrI1->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC1 = bstr;
		::SysFreeString( bstr ); 
	}

	if(bstrCC1!=bstrCC) return(false);

	_rect cr1;
	{
		_size size;
		ptrI1->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		ptrI1->GetOffset(&ptOffset);
		cr1=_rect(ptOffset,size);
	}

	_rect cr;
	{
		_size size;
		ptrI->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		ptrI->GetOffset(&ptOffset);
		cr=_rect(ptOffset,size);
	}

	int cx=cr1.width(), cy=cr1.height();
	int ox=cr1.left-cr.left, oy=cr1.top-cr.top;

	if(bFillImage)
	{
		for(int pane=0; pane<nPanes; pane++)
		{ //заполним паны
			for(int y=0; y<cy; y++)
			{
				byte *p,*p1;
				color *c,*c1;
				ptrI->GetRowMask(y+oy,&p); p+=ox;
				ptrI1->GetRowMask(y,&p1);
				ptrI->GetRow(y+oy, pane, &c); c+=ox;
				ptrI1->GetRow(y, pane, &c1);
				for(int x=0; x<cx; x++)
				{
					if(*p==0) *c=*c1; //если не было заполнено - ухватим из нового
					p++; p1++; c++; c1++;
				}
			}
		}
	}
	for(int y=0; y<cy; y++)
	{
		byte *p,*p1;
		ptrI->GetRowMask(y+oy,&p); p+=ox;
		ptrI1->GetRowMask(y,&p1);
		for(int x=0; x<cx; x++)
		{
			if(*p1>=128) *p|=mask1; //точки нового изображени€ метим маской
			p++; p1++;
		}
	}


	return true;
}

void CGlueObjFilter::GlueTwoObj(_point pt, IUnknownPtr obj1, IUnknownPtr obj2, _point ptConn1,  _point ptConn2)
//объекты могут быть одинаковыми
//объедин€ет объекты, убирает щели <=1 пикс, соедин€ет точки ptConn1 и ptConn2
{
	if(obj1==0 || obj2==0) return;

	IMeasureObjectPtr ptrO1(obj1);
	IUnknownPtr sptr1;
	ptrO1->GetImage(&sptr1);
	IImage3Ptr ptrI1(sptr1);
		
	_rect cr1;
	{
		_size size;
		ptrI1->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		ptrI1->GetOffset(&ptOffset);
		cr1=_rect(ptOffset,size);
	}
		
	IMeasureObjectPtr ptrO2(obj2);
	IUnknownPtr sptr2;
	ptrO2->GetImage(&sptr1);
	IImage3Ptr ptrI2(sptr1);

	DWORD dwFlags1;
	ptrI1->GetImageFlags(&dwFlags1);
	bool bIsVirtual1=(dwFlags1&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

	DWORD dwFlags2;
	ptrI1->GetImageFlags(&dwFlags2);
	bool bIsVirtual2=(dwFlags2&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

	bool bIsVirtual = bIsVirtual1 && bIsVirtual2; //оба виртуальные - результат виртуальный

	{
		INamedDataObject2Ptr NDO1(ptrI1);
		INamedDataObject2Ptr NDO2(ptrI2);

		IUnknown *punkParent1=0;
		IUnknown *punkParent2=0;

		NDO1->GetParent(&punkParent1);
		NDO2->GetParent(&punkParent2);

		bIsVirtual = bIsVirtual && (punkParent1==punkParent2);

		if(punkParent1!=0) punkParent1->Release();
		if(punkParent2!=0) punkParent2->Release();
	}

		
	_rect cr2;
	{
		_size size;
		ptrI2->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		ptrI2->GetOffset(&ptOffset);
		cr2=_rect(ptOffset,size);
	}

	//--------------------
	//объект-результат

	//размер
	_rect cr(cr1);
	cr.merge(cr2);
	int cx=cr.width(), cy=cr.height();

	IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr ptrI(punk2);
	punk2->Release();

	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		ptrI1->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( ptrI1 );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}
	
	_point pt_offset(cr.top_left());
	if(bIsVirtual)
	{
		_rect rect=_rect(pt_offset,_size(cx,cy));

		ptrI->CreateVirtual( rect );
		ptrI->InitRowMasks();

		INamedDataObject2Ptr imgOutNDO(ptrI);
		INamedDataObject2Ptr imageNDO(ptrI1);

		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		//imgOutNDO->GetObjectFlags( &dwFlags );
		imgOutNDO->SetParent( imageParent,  dwFlags);
	}
	else
	{
		ptrI->CreateImage(cx,cy,bstrCC, ::GetImagePaneCount( ptrI1 ) );
		ptrI->SetOffset(pt_offset,false);
	}
	ptrI->InitRowMasks();

	{for(int y=0; y<cy; y++) //очистим маску объекта
	{
		byte *p;
		ptrI->GetRowMask(y,&p);
		for(int x=0; x<cx; x++)
		{
			*p++=0;
		}
	}}

	bool bFillImage=!bIsVirtual;
	if(!GlueOneImage(ptrI, ptrI1, 1, bFillImage)) return;
	if(!GlueOneImage(ptrI, ptrI2, 2, bFillImage)) return;

	for(int y=2; y<cy-2; y++) //очистим маску объекта
	{
		byte *p,*pp,*pm;
		ptrI->GetRowMask(y-1,&pm);
		ptrI->GetRowMask(y,&p);
		ptrI->GetRowMask(y+1,&pp);
		byte *pd[9]={pm,pm+1,p+1,pp+1,pp,pp-1,p-1,pm-1,pm}; //маска в разных направлени€х
		for(int x=2; x<cx-2; x++)
		{
			if(p[x]==0)
			{
				int n=0; //число примыкающих к точке кусков объекта
				int ncol=0;
				for(int i=0; i<8; i++)
				{
					if(pd[i][x]&3)
					{
						ncol++;
						if((pd[i+1][x]&3)==0) n++;
					}
				}
				if(n>1) //точка линии, раздел€ющей 2 объекта
				{
					if(ncol>3)
					{
						p[x]|=128;
					}
				}
			}
			else p[x]|=128;
		}
	} //теперь маска =128 - заклеенна€ дырка, >128 - объекты
	
	if(bFillImage)
	{
			for(int pane=0; pane<nPanes; pane++)
		{
			for(int y=2; y<cy-2; y++)
			{
				byte *p,*pp,*pm;
				color *c,*cp,*cm;
				ptrI->GetRowMask(y-1,&pm);
				ptrI->GetRowMask(y,&p);
				ptrI->GetRowMask(y+1,&pp);
				ptrI->GetRow(y-1,pane,&cm);
				ptrI->GetRow(y,pane,&c);
				ptrI->GetRow(y+1,pane,&cp);
				for(int x=2; x<cx-2; x++)
				{
					if(p[x]==128) //заклеенна€ дырка
					{
						int col=0,ncol=0; //дл€ подсчета среднего цвета по окрестност€м
						if(pm[x-1]&3) {ncol++; col+=cm[x-1];}
						if(pm[x  ]&3) {ncol++; col+=cm[x  ];}
						if(pm[x+1]&3) {ncol++; col+=cm[x+1];}
						if(p [x-1]&3) {ncol++; col+=c [x-1];}
						if(p [x  ]&3) {ncol++; col+=c [x  ];}
						if(p [x+1]&3) {ncol++; col+=c [x+1];}
						if(pp[x-1]&3) {ncol++; col+=cp[x-1];}
						if(pp[x  ]&3) {ncol++; col+=cp[x  ];}
						if(pp[x+1]&3) {ncol++; col+=cp[x+1];}
						if(ncol>3)
						{
							c[x]=col/ncol;
						}
					}
				}
			}
		}
	}

	{for(int y=0; y<cy; y++) //приведем маску объекта к значени€м 0/255
	{
		byte *p;
		ptrI->GetRowMask(y,&p);
		for(int x=0; x<cx; x++)
		{
			*p= (*p==0) ? 0:255;
			p++;
		}
	}}

	{for(int pane=0; pane<nPanes; pane++)
	{
		int L=max(abs(ptConn1.x-ptConn2.x),abs(ptConn1.y-ptConn2.y));
		if(L>0)
		{
			int C0;
			color *c;
			ptrI->GetRow(ptConn1.y-pt_offset.y,pane,&c); C0=c[ptConn1.x-pt_offset.x];
			ptrI->GetRow(ptConn2.y-pt_offset.y,pane,&c); C0=(C0+c[ptConn2.x-pt_offset.x])/2;
			double x=ptConn1.x, y=ptConn1.y;
			double dx=(ptConn2.x-x)/L, dy=(ptConn2.y-y)/L;
			for(int i=0; i<=L; i++)
			{
				int x1=int(x)-pt_offset.x, y1=int(y)-pt_offset.y;
				for(int yy=y1-1; yy<=y1+1; yy++)
				{
					for(int xx=x1-1; xx<=x1+1; xx++)
					{
						if(xx>=0 && xx<cx && yy>=0 && yy<cy)
						{
							byte *p;
							color *c0;
							ptrI->GetRowMask(yy,&p);
							if(p[xx]<128)
							{
								p[xx]=255;
								if(bFillImage)
								{
									ptrI->GetRow(yy,pane,&c0); c0[xx]=C0;
								}
							}
						}
					}
				}
				x+=dx; y+=dy;
			}
		}
	}}

	ptrI->InitContours();

	//создаем собственно объект
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr ptrO(punk );
	punk->Release();
	
	ptrO->SetImage(ptrI);
	IUnknownPtr ptrDoc = get_document();
	SetToDocData(ptrDoc, ptrO);
	::ConditionalCalcObject(ptrO, 0, true, 0 );
}

IUnknownPtr     CGlueObjFilter::get_document()
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


HRESULT CGlueObjFilter::DoInvoke()
{
	IUnknownPtr ptrDoc = get_document();
	
	OnAttachTarget( ptrDoc );
	
	//IImage3Ptr    ptrSrcImage = m_image; //( GetDataArgument() );
	
	//if( ptrSrcImage == NULL) return false;
	
	return S_OK;
}

