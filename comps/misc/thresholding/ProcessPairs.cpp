#include "stdafx.h"
#include "ProcessPairs.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "measure5.h"
#include "docview5.h"
#include "object5.h"
#include "class_utils.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_map.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CProcessPairsInfo::s_pargs[] = 
{
	{"MaxDistance",	szArgumentTypeInt, "2", true, false },
	{"SizeRatio",	szArgumentTypeDouble, "0.5", true, false },
	{"Flags",	szArgumentTypeInt, "3", true, false },
	{"Source",		szArgumentTypeObjectList, 0, true, true },
	{"ProcessPairs",szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
// —копировать все вычисл€емые параметры из старого списка в новый
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

bool CProcessPairs::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());
	int nMaxDistance = GetArgLong("MaxDistance");
	int nFlags = GetArgLong("Flags");
	double fSizeRatio = GetArgDouble("SizeRatio");

	if(objectsIn==0 || objectsOut==0) return false;

	long nCountIn = 0;

	objectsIn->GetChildsCount(&nCountIn);

	smart_alloc(objs, IMeasureObject2Ptr, nCountIn);
	smart_alloc(imgs, IImage3Ptr, nCountIn);
	smart_alloc(rects, _rect, nCountIn);
	smart_alloc(areas, int, nCountIn);
	smart_alloc(kills, bool, nCountIn);
	smart_alloc(centers, _point, nCountIn);

	smart_alloc(classes, long, nCountIn); // классы, присваиваемые объектам
	smart_alloc(classes0, long, nCountIn); // классы, которые были у объектов до акции

	int n=0; // реальное количество объектов

	_list_map_t<long, long, cmp_long> pair_map;
	// список пар объектов.
	// формат ключа: номер1<<16+номер2
	// значение - рассто€ние между объектами

	{
		// заполним массивы
		POSITION pos=0;
		objectsIn->GetFirstChildPosition(&pos);
		while(pos)
		{
			if(n>=nCountIn) break; // ну, такого быть не должно - но мало ли...
			IUnknownPtr ptrObj;
			objectsIn->GetNextChild(&pos, &ptrObj);
			objs[n] = ptrObj;
			if(objs[n]==0) continue;
			IUnknownPtr ptrImg;
			objs[n]->GetImage(&ptrImg);
			imgs[n] = ptrImg;
			if(imgs[n]==0) continue;
			_point offset(0,0);
			int cx=0, cy=0;
			imgs[n]->GetOffset(&offset);
			imgs[n]->GetSize(&cx, &cy);
			rects[n] = _rect(offset, _size(cx,cy));
			areas[n] = 0;
			kills[n] = false;
			ICalcObject2Ptr sptrCO(objs[n]);
			classes0[n] = classes[n] = get_object_class(sptrCO);

			n++; // удачно получили объект и image - можно увеличивать счетчик
		}
		if(n>0)
		{
			_rect rc=rects[0];
			for(int i=0; i<n; i++)
			{ // найдем пр€моугольник, в который влезут все объекты
				rc.left = min(rc.left, rects[i].left);
				rc.top = min(rc.top, rects[i].top);
				rc.right = max(rc.right, rects[i].right);
				rc.bottom = max(rc.bottom, rects[i].bottom);
			}
			rc.inflate(1); // расширим на 1 пиксел - так будет удобнее работать
			int cx = rc.width(), cy=rc.height();

			smart_alloc(mark, short, cx*cy);
			smart_alloc(dist, short, cx*cy);
			for(int j=0; j<cx*cy; j++)
			{
				mark[j]=-1;
				dist[j]=32000;
			}

			// заполн€ем объекты на карте, заодно находим их центры
			for(int i=0; i<n; i++)
			{
				int area=0;
				double sx=0, sy=0;
				int cx1 = rects[i].width(), cy1 = rects[i].height();
				int dx = rects[i].left-rc.left, dy = rects[i].top-rc.top;
				for(int y=0; y<cy1; y++)
				{
					byte* pm=0;
					imgs[i]->GetRowMask(y, &pm);
					if(pm==0) continue;
					short* pmark = mark + (y+dy)*cx+dx;
					short* pdist = dist + (y+dy)*cx+dx;
					for(int x=0; x<cx1; x++)
					{
						if(*pm>=128)
						{
							*pmark = i;
							*pdist = 0;
							area++;
							sx += x;
							sy += y;
						}
						pm++;
						pmark++;
						pdist++;
					}
				}
				areas[i] = area;
				centers[i].x = sx / max(1,area);
				centers[i].y = sy / max(1,area);
			}

			// построим карту рассто€ний
			for(int y=1; y<cy-1; y++)
			{
				for(int x=1; x<cx-1; x++)
				{
					int i = y*cx+x;
					if(dist[i-cx]+1<dist[i])
					{
						dist[i] = dist[i-cx]+1;
						mark[i] = mark[i-cx];
					}
					if(dist[i-1]+1<dist[i])
					{
						dist[i] = dist[i-1]+1;
						mark[i] = mark[i-1];
					}
				}
			}

			for(int y=cy-2; y>=1; y--)
			{
				for(int x=cx-2; x>=1; x--)
				{
					int i = y*cx+x;
					if(dist[i+cx]+1<dist[i])
					{
						dist[i] = dist[i+cx]+1;
						mark[i] = mark[i+cx];
					}
					if(dist[i+1]+1<dist[i])
					{
						dist[i] = dist[i+1]+1;
						mark[i] = mark[i+1];
					}
				}
			}

			//{ // !!! debug - отобразить карту рассто€ний
			//	IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
			//	IImage3Ptr      sptrI(ptrI);
			//	sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );
   //             for(int y=0; y<cy; y++)
			//	{
			//		color* p=0;
			//		sptrI->GetRow(y, 0, &p);
			//		for(int x=0; x<cx; x++)
			//		{
   //                     p[x]=min(65535, dist[y*cx+x]*256);
			//		}
			//	}
			//	// debug - вставка в документ вспомогательной картинки
			//	if (m_ptrTarget==0) return S_FALSE;
			//	IDocumentPtr doc(m_ptrTarget);
			//	if(doc==0) return S_FALSE;
			//	SetToDocData(doc, sptrI);
			//}

			// поиск пар объектов
			for(int y=1; y<cy; y++)
			{
				for(int x=1; x<cx; x++)
				{
					int i = y*cx+x;
					if(mark[i]!=mark[i-1])
					{
						int d = dist[i]+dist[i-1];
						int key = (max(mark[i],mark[i-1])<<16) + min(mark[i],mark[i-1]);
						TPOS pos = pair_map.find(key);
						if(pos)
						{
							int d1 = pair_map.get(pos);
							if(d<d1) pair_map.set(d, key);
						}
						else pair_map.set(d, key);
					}
					if(mark[i]!=mark[i-cx])
					{
						long d = dist[i]+dist[i-cx];
						int key = (max(mark[i],mark[i-cx])<<16) + min(mark[i],mark[i-cx]);
						TPOS pos = pair_map.find(key);
						if(pos)
						{
							long d1 = pair_map.get(pos);
							if(d<d1) pair_map.set(d, key);
						}
						else pair_map.set(d, key);
					}
				}
			}

			const int nMaxProfileLength=100;
			color profile[nMaxProfileLength];
			{ // проверка профил€ €ркости дл€ пар объектов
				for(TPOS pos=pair_map.head(); pos; pos=pair_map.next(pos))
				{
					int n = pair_map.get_key(pos);
					short i1 = n>>16, i2 = n & 0xFFFF;
					long d = pair_map.get(pos);
					if( d<=nMaxDistance )
					{
						double dx = centers[i2].x-centers[i1].x;
						double dy = centers[i2].y-centers[i1].y;
						double d = _hypot( dx, dy );
						int nLen = max(1, min(int(d+0.5), nMaxProfileLength-1));
						dx /= nLen; dy/=nLen;

						color c=65535;
						for( int j=0; j<=nLen; j++)
						{
							int x = int( centers[i1].x + dx*j + 0.5 );
							int y = int( centers[i1].y + dy*j + 0.5 );
							if( x>=rects[i1].left && x<rects[i1].right &&
								y>=rects[i1].top && y<=rects[i1].bottom )
							{
								int xx = x - rects[i1].left;
								int yy = y - rects[i1].top;
								color* p=0;
								imgs[i1]->GetRow(yy, 1, &p); // зеленый
								if(p) c = p[xx];
							}
							if( x>=rects[i2].left && x<rects[i2].right &&
								y>=rects[i2].top && y<=rects[i2].bottom )
							{
								int xx = x - rects[i1].left;
								int yy = y - rects[i1].top;
								color* p=0;
								imgs[i2]->GetRow(yy, 1, &p); // зеленый
								if(p) c = p[xx];
							}
							profile[j] = c;
						}
						// профиль заполнен - теперь собственно проверка
						double r1 = sqrt((double)areas[i1]);
						double r2 = sqrt((double)areas[i2]);
					}
				}
			}

			if(nFlags & 1)
			{ // поиск тех, кого надо убить
				for(TPOS pos=pair_map.head(); pos; pos=pair_map.next(pos))
				{
					long n = pair_map.get_key(pos);
					short i1 = n>>16, i2 = n & 0xFFFF;
					int d = pair_map.get(pos);
					if( i1>=0 && i2>=0 && d<=nMaxDistance )
					{
						if( areas[i2] < areas[i1]*fSizeRatio )
							kills[i2] = true;
						if( areas[i1] < areas[i2]*fSizeRatio )
							kills[i1] = true;
					}
				}
				//fclose(f);

			}

			if(nFlags & 2)
			{ // "распространение" классов на соседние объекты
				//FILE* f=fopen("\\processpairs.log", "wt");
				for(TPOS pos=pair_map.head(); pos; pos=pair_map.next(pos))
				{
					long n = pair_map.get_key(pos);
					short i1 = n>>16, i2 = n & 0xFFFF;
					if( kills[i1] || kills[i2] ) continue; // убитые не участвуют в обработке
					long d = pair_map.get(pos);
					//if( i1>=0 && i2>=0 )
					//	fprintf(f,"%i-%i: %i\n", i1, i2, pair_map.get(pos));
					if( i1>=0 && i2>=0 && d<=nMaxDistance )
					{
						if( classes[i2] < classes0[i1] && areas[i2] < areas[i1] )
							classes[i2] = classes0[i1];
						if( classes[i1] < classes0[i2] && areas[i1] < areas[i2]  )
							classes[i1] = classes0[i2];
					}
				}
				//fclose(f);

			}

		} // end if(n>0)
	}

	CopyParams(objectsIn, objectsOut);

	StartNotification(n);
	long nNotificator = 0;

	for(int i=0; i<n; i++)
	{
		if(kills[i]) continue;

		IClonableObjectPtr sptrClone(objs[i]);
		if(sptrClone != 0)
		{
			IUnknownPtr ptrCloned = 0;
			sptrClone->Clone(&ptrCloned);

			INamedDataObject2Ptr sptrObject(ptrCloned);
			if(sptrObject != 0)
				sptrObject->SetParent(objectsOut, 0);

			ICalcObject2Ptr sptr(sptrObject);
			set_object_class(sptr, classes[i]);
		}
		Notify(nNotificator++);
	}
	
	GUID guid={0};
	INamedDataObject2Ptr sptrNDO1(objectsIn);
	if(sptrNDO1 != 0)
	{
		if(S_OK==sptrNDO1->GetBaseKey(&guid))
		{
			INamedDataObject2Ptr sptrNDO(objectsOut);
			if(sptrNDO != 0) sptrNDO->SetBaseKey(&guid);
		}
	}

	FinishNotification();

	return true;
}

/////////////////////////////////////////////////////////////////////////////

