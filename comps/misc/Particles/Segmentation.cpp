#include "stdafx.h"
#include "Segmentation.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"

_ainfo_base::arg CSegmSmallObjectsInfo::s_pargs[] =
{
	{"Image",       szArgumentTypeImage, 0, true, true },
	{"Binary",       szArgumentTypeBinaryImage, 0, true, true },
	{"Objects",    szArgumentTypeObjectList, 0, false, true },
	{"MinArea",    szArgumentTypeInt, "5", true, false },
	{"Border",    szArgumentTypeInt, "0", true, false },
	{"MinCX",    szArgumentTypeInt, "16", true, false },
	{"MinCY",    szArgumentTypeInt, "16", true, false },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CSegmSmallObjects::CSegmSmallObjects()
{
}

/////////////////////////////////////////////////////////////////////////////
CSegmSmallObjects::~CSegmSmallObjects()
{
}

//extract object name from Unknown
static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return _bstr_t("");

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr) 
		return  _bstr_t("");

	bstr_t bstr1(bstr);
	::SysFreeString(bstr);

	return bstr1;
}

IUnknown *CSegmSmallObjects::GetContextObject(_bstr_t cName, _bstr_t cType)
{ //Возвращает с AddRef

	IDataContext3Ptr ptrC = m_ptrTarget;
	_bstr_t	bstrType = cType;

	if( cName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	LONG_PTR lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == cName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

#define MAX_AREAS 2048

static int EnumAreas(byte **buf, int **num, _rect *rect, int *area, byte *val, int cx, int cy)
{
	if(cx<3 || cy<3) return(0); //на такой фигне делать нечего...

	smart_alloc(ind, int, MAX_AREAS);
	for(int i=0; i<MAX_AREAS; i++) ind[i]=i;
	//индексы для объединения нескольких областей в одну
	
	int cur_ind;
	
	num[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1]) cur_ind++;
		num[0][x]=cur_ind;
	} //проиндексировали первую строку
	
	for(int y=1; y<cy; y++)
	{
		for(int x=0; x<cx; )
		{ //идем слева направо по строке
			int i=-1;
			int xx=x;
			for(; x<cx && buf[y][x]==buf[y][xx]; x++)
			{	//если значения совпадают - продолжим текущую область
				//(если уже выбрана область правее - то ее)
				if(buf[y][x]==buf[y-1][x] && num[y-1][x]>i) i=num[y-1][x];
			}
			if(i<0)
			{ //попали в новую область
				cur_ind++;
				if(cur_ind>MAX_AREAS-1) cur_ind=MAX_AREAS-1;
				i=cur_ind;
			}
			for(; xx<x; xx++) num[y][xx]=i;
		}
	}
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(num[y-1][x]<num[y][x] && buf[y-1][x]==buf[y][x])
				ind[num[y-1][x]]=ind[num[y][x]];
		}
	}
	
	for(i=cur_ind; i>=0; i--)
		ind[i]=ind[ind[i]];

	smart_alloc(min_x, int, MAX_AREAS);
	smart_alloc(max_x, int, MAX_AREAS);
	smart_alloc(min_y, int, MAX_AREAS);
	smart_alloc(max_y, int, MAX_AREAS);
	//smart_alloc(val, int, MAX_AREAS);
	for(i=cur_ind; i>=0; i--)
	{
		rect[i].left=cx; rect[i].right=0;
		rect[i].top=cy; rect[i].bottom=0;
		area[i]=0;
	}
	
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int i=ind[num[y][x]];
			num[y][x]=i;
			if(x<rect[i].left) rect[i].left=x;
			if(x>=rect[i].right) rect[i].right=x+1;
			if(y<rect[i].top) rect[i].top=y;
			if(y>=rect[i].bottom) rect[i].bottom=y+1;
			area[i]++;
			val[i]=buf[y][x];
		}
	}

	return(cur_ind+1);
}

static void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}


HRESULT CSegmSmallObjects::DoInvoke()
{
	if (m_ptrTarget==0) return S_FALSE;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return S_FALSE;

	_bstr_t bstrImage = GetArgString("Image");
	_bstr_t bstrBinary = GetArgString("Binary");
	_bstr_t bstrObjectList=GetArgString("Objects");
	int nMinArea = GetArgLong("MinArea");
	int nBorder = GetArgLong("Border");
	int nMinCX = GetArgLong("MinCX");
	nMinCX = max(4,nMinCX);
	int nMinCY = GetArgLong("MinCY");
	nMinCY = max(4,nMinCY);

	long lBorderWidth = ::GetValueInt( GetAppUnknown(), "\\SpermSettings", "Border", 50 );

	IUnknown *punk2 = GetContextObject(bstrImage, szTypeImage);
	IImage3Ptr ptrImage(punk2);
	if (punk2!=0) punk2->Release();
	if (ptrImage==0) return S_FALSE;

	IUnknown *punkB = GetContextObject(bstrBinary, szTypeBinaryImage);
	IBinaryImagePtr ptrBinary(punkB);
	if (punkB!=0) punkB->Release();
	if (ptrBinary==0) return S_FALSE;

	IUnknown *punk = GetContextObject(bstrObjectList, szTypeObjectList);
	INamedDataObject2Ptr ptrObjectList(punk);
	if (punk!=0) punk->Release();
	if (ptrObjectList==0)
	{
		IUnknown *punk3=::CreateTypedObject(_bstr_t(szTypeObjectList));
		ptrObjectList=punk3;
		if (punk3!=0) punk3->Release();
		if(ptrObjectList!=0)
		{
			GUID guidNewBase;
			INamedDataObject2Ptr ptrNDOImage(ptrImage);
			if(ptrNDOImage!=0)
			{
				ptrNDOImage->GetBaseKey(&guidNewBase);
				ptrObjectList->SetBaseKey(&guidNewBase);
			}

			::SetValue( doc, 0, 0/*path*/, _variant_t((IUnknown*)ptrObjectList));
			if( bstrObjectList != _bstr_t("")  )
			{
				INamedObject2Ptr	sptr( ptrObjectList );
				if (sptr != 0) sptr->SetName( bstrObjectList );
			}
		}
	}

	if(ptrObjectList!=0)
		::DeleteObject( doc, ptrObjectList);
	
	if( ptrImage==NULL || ptrObjectList==NULL) return S_FALSE;
	
	int nPaneCount;
	
	int cx,cy;
	ptrImage->GetSize(&cx, &cy);
	POINT ptOffset;
	ptrImage->GetOffset(&ptOffset);
	
	/*IColorConvertor2* pIClrConv;
	ptrImage->GetColorConvertor((IUnknown**)&pIClrConv);
	pIClrConv->GetColorPanesCount(&nPaneCount);
	pIClrConv->Release();*/
	nPaneCount = ::GetImagePaneCount( ptrImage );

	
	/*
	for(int i=1; i<1000000; i++)
	{
		IImage3Ptr z=pArg;
		ICompatibleObjectPtr zz=z;
	}
	*/

	smart_alloc(srcRows, color *, cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<cy; y++)
		{
			ptrImage->GetRow(y, nPane, &srcRows[nPane*cy+y]);
		}
	}

	smart_alloc(binRows, byte *, cy);
	for(int y=0; y<cy; y++)
	{
		ptrBinary->GetRow(&binRows[y], y, 0);
	}
	
	smart_alloc(num_buf,int,cx*cy);
	smart_alloc(num,int *,cy);
	for(y=0; y<cy; y++) num[y]=num_buf+y*cx;

	smart_alloc(rects,_rect,MAX_AREAS);
	smart_alloc(areas,int,MAX_AREAS);
	smart_alloc(vals,byte,MAX_AREAS);
	for(int i=0; i<MAX_AREAS; i++) vals[i]=127;

	int nAreas=EnumAreas(binRows, num, rects, areas, vals, cx, cy);

	m_ObjectPool.StartProcessing(ptrObjectList, ptrImage, nMinCX, nMinCY);

	for(i=0; i<nAreas; i++)
	{
		if(vals[i]<128)	continue;	//только объекты, фон не нужен
		rects[i].left=max(min(rects[i].left,cx),0);
		rects[i].right=max(min(rects[i].right,cx),0);
		rects[i].top=max(min(rects[i].top,cy),0);
		rects[i].bottom=max(min(rects[i].bottom,cy),0);
		if(rects[i].right<=rects[i].left || rects[i].bottom<rects[i].top) continue; // должен быть хоть 1 пиксел

		int xc = (rects[i].left+rects[i].right+1)/2;
		int yc = (rects[i].top+rects[i].bottom+1)/2;

		if( xc<nBorder || yc<nBorder ||
			xc>=cx-nBorder || yc>=cy-nBorder) continue; // выкидываем те, что за бордюром или пересекают его

		if(areas[i]<nMinArea) continue; //мелочь уберем

		IMeasureObjectPtr ptrO=m_ObjectPool.GetObject(rects[i]);
		if(ptrO==0)	continue;
		IUnknown *punk2;
		ptrO->GetImage( &punk2 );
		IImage3Ptr ptrI;
		ptrI=punk2;
		if(punk2!=0) punk2->Release();
		if(ptrI==0)	continue;
		int cx1,cy1;
		ptrI->GetSize(&cx1, &cy1);
		_point ofs;
		ptrI->GetOffset(&ofs);
		for(int y=0; y<cy1; y++)
		{
			byte *pmask;
			ptrI->GetRowMask(y,&pmask);
			for(int x=0; x<cx1; x++)
			{
				if(num[ofs.y+y][ofs.x+x]==i)
					*pmask=255;
				else
					*pmask=0;
				pmask++;
			} // for x
		} // for y
		ptrI->InitContours();
	}

	/*
	for(int i=0; i<1000; i++)
	{
		_rect rect;
		rect.left = rand()%(cx-16);
		rect.top = rand()%(cy-16);
		rect.right = rect.left+10+rand()%7;
		rect.bottom = rect.top+10+rand()%7;
		if(rand()%50==0)
		{
			rect.right = rect.left+10+rand()%70;
			rect.bottom = rect.top+10+rand()%70;
		}
		
		rect.left=max(min(rect.left,cx),0);
		rect.right=max(min(rect.right,cx),0);
		rect.top=max(min(rect.top,cy),0);
		rect.bottom=max(min(rect.bottom,cy),0);

		IMeasureObjectPtr ptrO=m_ObjectPool.GetObject(rect);
	}
	*/
	m_ObjectPool.StopProcessing();
	
	if(ptrObjectList!=0)
		::SetValue( doc, 0, 0/*path*/, _variant_t((IUnknown*)ptrObjectList));

	{ //просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(ptrObjectList);
		IUnknown *punk = NULL;
		HRESULT hr = NDO->GetData(&punk);
		if (SUCCEEDED(hr) && punk)
		{
			long l = cncReset;
			::FireEvent(punk, szEventChangeObjectList, ptrObjectList, NULL, &l, 0);
			punk->Release();
		}
	}
	
	return S_OK;
}

