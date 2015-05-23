#include "stdafx.h"
#include "FillHoles.h"
#include "misc_utils.h"
#include <math.h>
#include "measure5.h"
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "Chromosome.h"
#include "dpoint.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              FillHoles
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CFillHolesInfo::s_pargs[] =
{
	{"MaxHoleSize",    szArgumentTypeInt, "1000", true, false },
	{"Source",    szArgumentTypeObjectList, 0, true, true },
	{"FillHoles",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
// Скопировать все вычисляемые параметры из старого списка в новый
{
	if( in == 0 || out == 0)
		return;
	
	LONG_PTR lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer      *pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

static IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	POSITION lPos = 0;
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

static int FillSmallSegments(byte **buf, int cx, int cy, byte fill_color, int del_size=10)
{ //возвращает, сколько сегментов залили
	if(cx<3 || cy<3) return(0); //на такой фигне делать нечего...

	smart_alloc(ind, byte, 256);
	for(int i=0; i<256; i++) ind[i]=i;
	//индексы для объединения нескольких областей в одну
	
	smart_alloc(cnt_buf, byte, cx*cy);
	smart_cnt_buf.zero();
	
	smart_alloc(cnt, byte*, cy);
	for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;
	
	int cur_ind;
	
	cnt[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1]) cur_ind++;
		cnt[0][x]=cur_ind;
	} //проиндексировали первую строку
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; )
		{ //идем слева направо по строке
			int i=-1;
			int xx=x;
			for(; x<cx && buf[y][x]==buf[y][xx]; x++)
			{	//если значения совпадают - продолжим текущую область
				//(если уже выбрана область правее - то ее)
				if(buf[y][x]==buf[y-1][x] && cnt[y-1][x]>i) i=cnt[y-1][x];
			}				
			if(i<0)
			{ //попали в новую область
				cur_ind++;
				if(cur_ind>255) cur_ind=255;
				i=cur_ind;
			}
			for(; xx<x; xx++) cnt[y][xx]=i;
		}
	}
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(cnt[y-1][x]<cnt[y][x] && buf[y-1][x]==buf[y][x])
				ind[cnt[y-1][x]]=ind[cnt[y][x]];
		}
	}
	
	for(i=cur_ind; i>=0; i--)
		ind[i]=ind[ind[i]];
	
	smart_alloc(area, int, 256);
	smart_area.zero();
	
	smart_alloc(min_x, int, 256);
	smart_alloc(max_x, int, 256);
	smart_alloc(min_y, int, 256);
	smart_alloc(max_y, int, 256);
	for(i=cur_ind; i>=0; i--)
	{
		min_x[i]=cx; max_x[i]=0;
		min_y[i]=cy; max_y[i]=0;
	}
	

	//FILE *f=fopen("d:\\vt5\\out.log","w");
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			byte c=ind[cnt[y][x]];
			//fputc(c+'A',f);
			area[c]++;
			if(x<min_x[c]) min_x[c]=x;
			if(x>max_x[c]) max_x[c]=x;
			if(y<min_y[c]) min_y[c]=y;
			if(y>max_y[c]) max_y[c]=y;
		}
		//fputs("\n",f);
	}
	//fclose(f);
	
	smart_alloc(del_it, BOOL, 256);
	for(i=cur_ind; i>=0; i--)
	{
		del_it[i]=
			max_x[i]-min_x[i]<=del_size && max_y[i]-min_y[i]<=del_size;
	}

	//приграничные области не трогаем
	{for (y=0;y<cy;y++)
	{
		{byte c=ind[cnt[y][0]]; del_it[c]=0;}
		{byte c=ind[cnt[y][cx-1]]; del_it[c]=0;}
	}}
	{for (x=0;x<cx;x++)
	{
		{byte c=ind[cnt[0][x]]; del_it[c]=0;}
		{byte c=ind[cnt[cy-1][x]]; del_it[c]=0;}
	}}
	
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx-1; x++)
		{
			byte c=ind[cnt[y][x]];
			if(del_it[c]) buf[y][x]=fill_color;
		}
	}
	
	int count=0;
	for(i=1; i<=cur_ind; i++)
	{
		if(area[i]>1) count++;
	}
	
	//CString s;
	//s.Format("Areas: %d (%d)", count,cur_ind);
	//char s[200];
	//sprintf(s,"Areas: %d (%d)", count,cur_ind);
	//MessageBox(0,s,"FindCells/DeleteSmallSegments",0);
	
	return(count);
}

static bool FillMaskHoles(byte **mask, int cx, int cy)
//возвращает, были ли дыры.
{
	bool bRet=false;
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(mask[y][x]<128)
			{
				mask[y][x]=255;
				bRet=true;
			}
		}
	}

	return bRet;
}

static IImage3Ptr CreateImageCopy(IImage3Ptr image)
{ //копирует image - цветовые данные и маску; контура не создает

	DWORD dwFlags;
	image->GetImageFlags(&dwFlags);
	bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

	_point offset;
	image->GetOffset(&offset);
	int cx,cy;
	image->GetSize(&cx,&cy);

	IImage3Ptr imgOut;

	if(bIsVirtual)
	{
		_rect rect=_rect(offset,_size(cx,cy));

		IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
		imgOut=IImage3Ptr(punk2);
		if (punk2!=0) punk2->Release();
		if(imgOut==0) return 0;

		imgOut->CreateVirtual( rect );

		INamedDataObject2Ptr imgOutNDO(imgOut);
		INamedDataObject2Ptr imageNDO(image);

		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		//imgOutNDO->GetObjectFlags( &dwFlags );
		imgOutNDO->SetParent( imageParent,  dwFlags);

		imgOut->InitRowMasks();

		//скопируем маску
		for(int y=0; y<cy; y++)
		{
			{ //copy mask
				byte *mSrc,*mDst;
				image->GetRowMask(y, &mSrc);
				imgOut->GetRowMask(y, &mDst);
				memcpy( mDst, mSrc, cx );
			}
		}
	}
	else
	{
		_bstr_t	bstrCC;
		int nPanes;
		{
			IUnknown *punkCC = 0;
			image->GetColorConvertor( &punkCC );
			IColorConvertor2Ptr	ptrCC( punkCC );
			punkCC->Release();

			nPanes = ::GetImagePaneCount( image );

			BSTR	bstr;
			ptrCC->GetCnvName( &bstr );
			bstrCC = bstr;
			::SysFreeString( bstr ); 
		}

		IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
		imgOut=IImage3Ptr(punk2);
		if (punk2!=0) punk2->Release();
		if(imgOut==0) return 0;

		imgOut->CreateImage(cx,cy,bstrCC, ::GetImagePaneCount( image ) );
		imgOut->InitRowMasks();

		imgOut->SetOffset(offset,false);

		//создали image, теперь заполним его

		for(int y=0; y<cy; y++)
		{
			{ //copy mask
				byte *mSrc,*mDst;
				image->GetRowMask(y, &mSrc);
				imgOut->GetRowMask(y, &mDst);
				memcpy( mDst, mSrc, cx );
			}
			for(int pane=0; pane<nPanes; pane++)
			{ //copy panes
				color *cSrc,*cDst;
				image->GetRow( y, pane, &cSrc );
				imgOut->GetRow( y, pane, &cDst );
				memcpy( cDst, cSrc, cx*sizeof(color) );
			}
		}
	}

	return(imgOut);
}

//собственно, главная функция - обрабатывает один объект.
//если с ним что-то не так, возвращает 0
static IMeasureObjectPtr ProcessObject(IMeasureObjectPtr object, long nMaxHoleSize)
{
	//получим image объекта
	IUnknownPtr sptr2;
	object->GetImage(&sptr2);
	IImage3Ptr image(sptr2);
	if(image==0) return 0;

	IImage3Ptr imgOut(CreateImageCopy(image));

	_point offset;
	imgOut->GetOffset(&offset);
	int cx,cy;
	imgOut->GetSize(&cx,&cy);

	smart_alloc(dstMask, byte*, cy);
	{for(int y=0; y<cy; y++)
	{
		imgOut->GetRowMask(y, dstMask+y);
	}}
	
	if(nMaxHoleSize>=0)
	{
		FillSmallSegments(dstMask, cx,cy, 255, nMaxHoleSize);
	}
	else
	{ // <0 - тест: превратить объекты в прямоугольники
		FillMaskHoles(dstMask, cx, cy);
	}
	//заполнить все кусочки маски размером <nMaxHoleSize цветом 255

	imgOut->InitContours();

	//create object
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr objOut(punk );
	punk->Release();
	
	objOut->SetImage( imgOut );

	return objOut;
}

bool CFillHolesFilter::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());
	
	if(objectsOut==0 || objectsIn==0) return false;
	
	long nCountIn; objectsIn->GetChildsCount(&nCountIn);
	if(!nCountIn) return false;
	
	int nN=0;
	StartNotification(nCountIn);
	
	//CopyParams(objectsIn, objectsOut);

	long nMaxHoleSize = GetArgLong("MaxHoleSize");
	
	POSITION pos; objectsIn->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		objectsIn->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		if(object==0) continue;

		IMeasureObjectPtr objOut(ProcessObject(object, nMaxHoleSize));
		{
			DWORD dwFlags;
			INamedDataObject2Ptr ptrObject(objOut);
			if(ptrObject!=0)
			{
				ptrObject->GetObjectFlags(&dwFlags);
				dwFlags=0;
				ptrObject->SetParent(objectsOut, dwFlags);
			}
		}

		Notify(nN++);
	}
	
	GUID guidNewBase;
	objectsIn->GetBaseKey(&guidNewBase);
	objectsOut->SetBaseKey(&guidNewBase);
	
	FinishNotification();

	{
		INamedDataObject2Ptr ptrList(objectsOut);
		IDataContextPtr sptrContext( m_ptrTarget );
		sptrContext->SetActiveObject(_bstr_t(szTypeObjectList), ptrList, aofActivateDepended);
	}
	
	return true;
}

CFillHolesFilter::CFillHolesFilter()
{
}

CFillHolesFilter::~CFillHolesFilter()
{
}
