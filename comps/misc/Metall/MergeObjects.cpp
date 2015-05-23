#include "StdAfx.h"
#include "MergeObjects.h"
#include "nameconsts.h"
#include "misc_classes.h"
#include "misc_utils.h"
#include "class_utils.h"
#include "alloc.h"

_ainfo_base::arg CMergeObjectsInfo::s_pargs[] =
{
	{"JoinClass",	szArgumentTypeInt, "-1", true, false },
	{"Source",	szArgumentTypeObjectList, 0, true, true },
	{"Borders",	szArgumentTypeObjectList, 0, true, true },
	{"MergeObjects",	szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

CMergeObjects::CMergeObjects(void)
{
}

CMergeObjects::~CMergeObjects(void)
{
}

static IMeasureObjectPtr ListInsertImage(INamedDataObject2Ptr ptrObjList, IImage3Ptr ptrImage)
{
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr objectOut(punk );
	punk->Release();
	if(objectOut==0) return 0;
	objectOut->SetImage( ptrImage );

	INamedDataObject2Ptr ptrObject(objectOut);
	if(ptrObject==0) return 0;
	DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
	ptrObject->SetParent(ptrObjList, dwFlags);

	return objectOut;
}

bool CMergeObjects::InvokeFilter()
{
	INamedDataObject2Ptr ptrObjects(GetDataArgument("Source"));
	INamedDataObject2Ptr ptrBorders(GetDataArgument("Borders"));
	INamedDataObject2Ptr ptrResult(GetDataResult("MergeObjects"));
	if(ptrObjects==0 || ptrBorders==0 || ptrResult==0) return false;

	int nJoinClass = GetArgLong("JoinClass");

	long nObjects; ptrObjects->GetChildsCount(&nObjects);
	long nBorders; ptrBorders->GetChildsCount(&nBorders);
	long nDst=0;

	smart_alloc(Src, IMeasureObjectPtr, nObjects);
	smart_alloc(SrcImages, IImage3Ptr, nObjects);
	smart_alloc(Borders, ICalcObjectPtr, nBorders);

	{	// заполним массив объектов
		int nObject=0;
		POSITION posObject = 0; ptrObjects->GetFirstChildPosition(&posObject);
		while( posObject && nObject<nObjects ) //по всем объектам
		{
			IUnknownPtr sptr;
			ptrObjects->GetNextChild(&posObject, &sptr);
			Src[nObject] = sptr;
			if(Src[nObject]==0) return false;

			IUnknownPtr sptr2;
			Src[nObject]->GetImage(&sptr2);
			SrcImages[nObject] = sptr2;
			if(SrcImages[nObject]==0) return false;

			nObject++;
		}
	}

	{	// заполним массив границ
		int nBorder=0;
		POSITION posBorder = 0; ptrBorders->GetFirstChildPosition(&posBorder);
		while( posBorder && nBorder<nBorders ) //по всем границам
		{
			IUnknownPtr sptr;
			ptrBorders->GetNextChild(&posBorder, &sptr);
			Borders[nBorder] = sptr;
			if(Borders[nBorder]==0) return false;

			nBorder++;
		}
	}

	// массивы для результата - объединенных объектов
	// для каждого объекта - храним номер первого в той же цепочке, последнего и следующего за ним
	smart_alloc(FirstObject, int, nObjects);
	smart_alloc(LastObject, int, nObjects);
	smart_alloc(NextObject, int, nObjects);
	smart_alloc(Rectangles, _rect, nObjects);
	smart_alloc(Rectangles0, _rect, nObjects);
	for(int i=0; i<nObjects; i++)
	{
		FirstObject[i]=i;
		LastObject[i]=i;
		NextObject[i]=-1;
		_point ptOffset(0,0);
		int cx=0,cy=0;
		SrcImages[i]->GetOffset(&ptOffset);
		SrcImages[i]->GetSize(&cx, &cy);
		Rectangles[i]=Rectangles0[i]=_rect(ptOffset,_size(cx,cy));
	}

	char szClassifier[256];
	strncpy(szClassifier, short_classifiername(""), 255);
	for(int nBorder=0; nBorder<nBorders; nBorder++)
	{
		int nClass = get_object_class(Borders[nBorder], szClassifier);
		if(nClass!=nJoinClass) continue; // не наш класс - не сливаем
		int i1=-1, i2=-1;
		double fi1=-1, fi2=-1;
		Borders[nBorder]->GetValue(KEY_PARAM_OBJECT1, &fi1); i1=int(fi1)-1;
		Borders[nBorder]->GetValue(KEY_PARAM_OBJECT2, &fi2); i2=int(fi2)-1;
		if(i1<0 || i1>=nObjects || i2<0 || i2>=nObjects) continue; // ошибочные номера
		if(FirstObject[i1]==FirstObject[i2]) continue; // уже соединены
		int OldLast1=LastObject[i1], OldFirst2=FirstObject[i2];
		for(int j1=FirstObject[i1]; j1!=-1; j1=NextObject[j1])
			LastObject[j1]=LastObject[i2]; // единый последний - из 2-го списка
		for(int j2=FirstObject[i2]; j2!=-1; j2=NextObject[j2])
			FirstObject[j2]=FirstObject[i1]; // единый первый - из первого списка
		NextObject[OldLast1] = OldFirst2; // свяжем списки
	}

	for(int i=0; i<nObjects; i++)
	{
		Rectangles[FirstObject[i]].merge( Rectangles[i] );
	} // теперь ректанглы первых объектов включают в себя все объекты

	StartNotification( nObjects, 1, 1 );
	for(int i=0; i<nObjects; i++)
	{
		if(i!=FirstObject[i]) continue; // только для первых объектов в списках
		INamedDataObject2Ptr imageNDO(SrcImages[i]);
		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;
		if(imageParent==0) continue; // при ошибке
		
		IUnknownPtr ptrImageUnk(CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
		IImage4Ptr ptrImage(ptrImageUnk);
		if(ptrImage==0) continue; // при ошибке
		INamedDataObject2Ptr ptrNDO2Image(ptrImageUnk);
		if(ptrNDO2Image==0) continue; // при ошибке
		ptrImage->CreateVirtual( Rectangles[i] );
		ptrNDO2Image->SetParent(imageParent, 0);
		ptrImage->InitRowMasks();
		for(int y=0; y<Rectangles[i].height(); y++)
		{
			byte *pmask_out;
			ptrImage->GetRowMask(y, &pmask_out);
			ZeroMemory(pmask_out, Rectangles[i].width()*sizeof(byte));
		}

		int cnt=128;
		for(int j=i; j!=-1; j=NextObject[j])
		{
			int dx=Rectangles0[j].left-Rectangles[i].left; // смещение исходного по отношению к результату
			int dy=Rectangles0[j].top-Rectangles[i].top;
			for(int y=0; y<Rectangles0[j].height(); y++)
			{
				byte *pmask_in;
				byte *pmask_out;
				SrcImages[j]->GetRowMask(y, &pmask_in);
				ptrImage->GetRowMask(y+dy, &pmask_out);
				pmask_out += dx;
				for(int x=0; x<Rectangles0[j].width(); x++)
				{
					if(pmask_in[x]) pmask_out[x]=cnt;
				}
			}
			cnt++; if(cnt>255) cnt=128;
		}

		int cx=Rectangles[i].width(), cy=Rectangles[i].height();
		int mask=128;
		int val=64;
		for(int d=2; d>0; d--)
		{
			for(int y=d; y<cy-d; y++)
			{
				byte *p0,*p_m,*p_p;
				ptrImage->GetRowMask(y, &p0);
				ptrImage->GetRowMask(y-d, &p_m);
				ptrImage->GetRowMask(y+d, &p_p);
				for(int x=d; x<cx-d; x++)
				{
					if( (p0[x]&mask) == 0 )
					{
						if( (p0[x-d]&mask) && (p0[x+d]&mask) && p0[x-d]!=p0[x+d])
							p0[x]=val;
						if( (p_m[x]&mask) && (p_p[x]&mask) && p_m[x]!=p_p[x])
							p0[x]=val;
					}
				}
			}
			mask |= val;
			val <<= 1;
		}
		for(int y=0; y<cy; y++)
		{
			byte *p0;
			ptrImage->GetRowMask(y, &p0);
			for(int x=0; x<cx; x++)
			{
				if(p0[x]) p0[x]=255;
			}
		}

		ptrImage->InitContours();

		IMeasureObjectPtr obj = ListInsertImage(ptrResult, ptrImage);
	}

	FinishNotification();

	return true;
}