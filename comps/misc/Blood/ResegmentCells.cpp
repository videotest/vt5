#include "stdafx.h"
#include "ResegmentCells.h"

#include "misc_utils.h"
#include <math.h>
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "object5.h"
#include "measure5.h"
#include "bytemorpho.h"

#define MaxColor color(-1)

_ainfo_base::arg        CResegmentCellsInfo::s_pargs[] = 
{       
	{"SeedThreshold",      szArgumentTypeDouble, "0.0", true, false },
	{"Threshold",      szArgumentTypeDouble, "0.3", true, false },
	{"MaxDilate",      szArgumentTypeDouble, "10", true, false },
	{"Cells",       szArgumentTypeObjectList, 0, true, true },
	{"Resegmented",    szArgumentTypeObjectList, 0, false, true },	
	{0, 0, 0, false, false },
};

static inline int round(double x)
{
	return int(floor(x+0.5));
}

/////////////////////////////////////////////////////////////////////////////
bool CResegmentCells::InvokeFilter()
{
	INamedDataObject2Ptr sptrSrc(GetDataArgument("Cells")); // Object list
	INamedDataObject2Ptr sptrDst(GetDataResult()); // Object list

	if(sptrSrc==NULL || sptrDst==NULL)
		return false;

	double fThreshold = GetArgDouble("Threshold");
	double fSeedThreshold = GetArgDouble("SeedThreshold");
	double fMaxDilate = GetArgDouble("MaxDilate");

	long lObjects;
	sptrSrc->GetChildsCount(&lObjects);
	StartNotification(lObjects,2);

	long pos = 0, i = 0;
	sptrSrc->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr ptr;
		sptrSrc->GetNextChild((long*)&pos, &ptr);

		IClonableObjectPtr sptrClone(ptr);
		if(sptrClone == 0) continue;

		IUnknownPtr ptr2;
		sptrClone->Clone(&ptr2);

		IMeasureObjectPtr sptrMO(ptr);
		IUnknownPtr ptrImage;
		if(sptrMO !=0 )
			sptrMO->GetImage(&ptrImage);

		IUnknownPtr ptrImage2( ::CreateTypedObject(_bstr_t(szTypeImage)), false);
		ICompatibleObjectPtr sptrCO2 = ptrImage2;
		if(sptrCO2!=0) 
			sptrCO2->CreateCompatibleObject(ptrImage,0,0);

		IMeasureObjectPtr sptrMO2(ptr2);
		if(sptrMO2!=0)
			sptrMO2->SetImage(ptrImage2);

		IImage3Ptr sptrImage(ptrImage);
		if(sptrImage==0) continue;

		IImage3Ptr sptrImage2(ptrImage2);
		if(sptrImage2==0) continue;

		// скопируем контура
		{
			int nContours=0;
			sptrImage->GetContoursCount(&nContours);
			sptrImage2->FreeContours();
			for(int i=0; i<nContours; i++)
			{
				Contour* pCont = 0;
				sptrImage->GetContour(i, &pCont);
				Contour* pCont2 = new Contour;
				*pCont2 = *pCont; // скопировали все числа, ссылаемся на старые контура
				pCont2->ppoints =  new ContourPoint[pCont2->nAllocatedSize];
				memcpy(pCont2->ppoints, pCont->ppoints, pCont2->nAllocatedSize*sizeof(ContourPoint));
				sptrImage2->AddContour(pCont2);
			}
		}


		// собственно сегментация
		int cx=0,cy=0;
		sptrImage2->GetSize(&cx,&cy);

		smart_alloc(ppc, color*, cy);
		smart_alloc(ppm, byte*, cy);
		smart_alloc(pDist, double, cx*cy);
		smart_alloc(ppDist, double*, cy);

		for(int y=0; y<cy; y++)
		{
			sptrImage2->GetRow(y,1,ppc+y);
			sptrImage2->GetRowMask(y, ppm+y );
			ppDist[y] = pDist+y*cx;
		}


		if(fThreshold>=0.0)
		{	// --------------- Пересегментация ядер -------------
			// найдем пороги для сегментации
			double c0=0, c1=0;
			double s0=0, s1=0;
			double color0=0, color1=65535;

			for(int pass=0; pass<4; pass++)
			{
				for(int y=0; y<cy; y++)
				{
					for(int x=0; x<cx; x++)
					{
						double w;
						switch(ppm[y][x])
						{
						case 255: // цитоплазма
							w = ppc[y][x]-color0;
							w = exp(w/(256*20));
							w = max(0,w);
							s1 += ppc[y][x]*w;
							c1 += w;
						case 254: // ядро
							w = color1-ppc[y][x];
							w = exp(w/(256*20));
							w = max(0,w);
							s0 += ppc[y][x]*w;
							c0 += w;
						}
					}
				}

				if(c0) color0 = s0/c0;
				if(c1) color1 = s1/c1;
			}
			// нашли color0 и color1 - примерный диапазон цветов клетки

			{
				int th0 = int(color0 + (color1-color0)*fSeedThreshold);
				int th1 = int(color0 + (color1-color0)*fThreshold);

				// сегментация зародышей
				for(int y=0; y<cy; y++)
				{
					for(int x=0; x<cx; x++)
					{
						ppDist[y][x]=1e30;
						if(ppm[y][x]>=254) // цитоплазма или ядро
						{
							if(ppc[y][x]>th0)
							{
								ppm[y][x]=255; // цитоплазма
							}
							else
							{
								ppm[y][x]=254; // ядро
								ppDist[y][x]=0;
							}
						}
					}
				}

				for(int pass=0; pass<2; pass++)
				{
					for(int y=1; y<cy-1; y++)
					{
						for(int x=1; x<cx-1; x++)
						{
							ppDist[y][x+1] = min(ppDist[y][x+1],ppDist[y][x]+1);
							ppDist[y+1][x] = min(ppDist[y+1][x],ppDist[y][x]+1);
							if(ppm[y][x]==254) // ядро
							{
								double th = th1 + (th0-th1)*ppDist[y][x]/fMaxDilate;
								if(ppc[y][x+1]<=th) ppm[y][x+1]=254;
								if(ppc[y+1][x]<=th) ppm[y+1][x]=254;
							}
						}
					}
					for(int y=cy-2; y>0; y--)
					{
						for(int x=cx-2; x>0; x--)
						{
							ppDist[y][x-1] = min(ppDist[y][x-1],ppDist[y][x]+1);
							ppDist[y-1][x] = min(ppDist[y-1][x],ppDist[y][x]+1);
							if(ppm[y][x]==254) // ядро
							{
								double th = th1 + (th0-th1)*ppDist[y][x]/fMaxDilate;
								if(ppc[y][x-1]<=th) ppm[y][x-1]=254;
								if(ppc[y-1][x]<=th) ppm[y-1][x]=254;
							}
						}
					}
				}

			}

			// морфология, как в FindCells - чтобы границы были поаккуратнее
			int nNucleiCloseSize=GetValueInt(GetAppUnknown(),"\\FindCells", "NucleiCloseSize",5);
			if(nNucleiCloseSize%2 == 0) nNucleiCloseSize = nNucleiCloseSize-1;
			nNucleiCloseSize = max(1,nNucleiCloseSize);
			SetValue(GetAppUnknown(),"\\FindCells", "NucleiCloseSize",long(nNucleiCloseSize));

			DilateColor(ppm, cx, cy, nNucleiCloseSize, 254,254,255); // дилатация ядра на фоне цитоплазмы
			DilateColor(ppm, cx, cy, 3, 0,255,254); // эрозия ядра - только те части, что примыкают к краю цитоплазмы
			DilateColor(ppm, cx, cy, nNucleiCloseSize, 255,255,254); // эрозия ядра
			DilateColor(ppm, cx, cy, nNucleiCloseSize, 255,255,254); // эрозия ядра
			DilateColor(ppm, cx, cy, nNucleiCloseSize, 254,254,255); // дилатация ядра

			DeleteSmallSegments(ppm, cx, cy, 12, 254);
		}	// --------------- Пересегментация ядер end -------------
		else if ( -1.0 == fThreshold )
		{	// --------------- Пересегментация цитоплазмы - "бублик" (слой вокруг ядра) -------------
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					if(ppm[y][x]!=254) // ядро
						ppm[y][x]=253;  // фон внутри квадрата
				}
			}
			DilateColor(ppm, cx, cy, int(fMaxDilate), 254,255,253); // расползаемся от ядра (254) цитоплазмой (255) на фон (253)
		}	// --------------- Пересегментация цитоплазмы end -------------
		else if ( -2.0 == fThreshold )
		{	// --------------- Пересегментация цитоплазмы - ограничение бубликом -------------
			DilateColor(ppm, cx, cy, int(fMaxDilate), 254,128,255); // расползаемся от ядра (254) маркером (128) по цитоплазме (255)
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					if(ppm[y][x]==255) // цитоплазма вне бублика
						ppm[y][x]=253;  // фон внутри квадрата
					else if(ppm[y][x]==128) // цитоплазма внутри бублика
						ppm[y][x]=255;  // цитоплазма
				}
			}
		}	// --------------- Пересегментация цитоплазмы end -------------
		else if ( -3.0 == fThreshold )
		{	// --------------- Эксперимент - эквалайз -------------
			smart_alloc(hist, double, 256);
			for(int nPane=0; nPane<3; nPane++)
			{
				for(int y=0; y<cy; y++)
					sptrImage2->GetRow(y,nPane,ppc+y); // Работаем по нужной пане
				for(int i=0; i<256; i++) hist[i] = 10; // имитация шума

				for(int y=0; y<cy; y++)
				{
					for(int x=0; x<cx; x++)
					{
						if(ppm[y][x]<254) continue; // фон и всякую фигню пропускаем
						int v = ppc[y][x] / 256;
						hist[v] ++;
					}
				}

				// размоем гистограмму
				for(int i=1; i<256; i++) hist[i] = hist[i] + (hist[i-1]-hist[i])*0.5;
				for(int i=254; i>=0; i--) hist[i] = hist[i] + (hist[i+1]-hist[i])*0.5;

				double s=0;
				for(int i=0; i<256; i++) s += hist[i];

				smart_alloc(ii, int, 256);
				double ss=0;
				int j=0;
				for(int i=0; i<256; i++)
				{
					ss += hist[i]/s;
					int j1 = round(ss*255);
					ii[i] = j1;
					//while(j<=j1) ii[j++] = i;
				}

				for(int y=0; y<cy; y++)
				{
					for(int x=0; x<cx; x++)
					{
						//if(ppm[y][x]<254) continue; // фон и всякую фигню пропускаем
						int v = ppc[y][x] / 256;
						ppc[y][x] = ii[v]*256;
					}
				}

				/*
				int i1=0, i5=0, i9=0;
				double ss=0;
				for(int i=0; i<256; i++)
				{
					ss += hist[i];
					if(ss<=s*0.1) i1 = i;
					if(ss<=s*0.5) i5 = i;
					if(ss<=s*0.9) i9 = i;
				}
				//double D = i1*i1*i2 + i2*i2*i3 + i1*i3*i3
				//	- i1*i1*i3 - i1*i2*i2 - i3*i3*i2;
				//double a = 0.1*i2 + 0.5*i3 + 0.9*
				double scale = (i9-i1) / (255*0.8);
				double shift = i1 - 0.1*255*scale;
				// i = i*scale + shift
				double center_shift = 255*0.5
				*/
			}
		}	// --------------- Эксперимент - эквалайз end -------------

		INamedDataObject2Ptr ptrObject(ptr2); 
		if(ptrObject != 0)
			ptrObject->SetParent(sptrDst, 0);

		Notify(i++);
	}
	FinishNotification();

	SetModified(true);

	return true;
}
