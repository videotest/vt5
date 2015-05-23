Быстрый (часто ~log(len)) метод поиска расстояний в заданном направлении до края маски:
1. Precalc - посчитать расстояния до края (можно упрощенно - по 4 соседям)
   (пробежать по всему изображению в 4 направлениях,
   dist[] = min(dist[],dist[сосед]+1) )
2. Идем в заданном направлении. Первый шаг равен dist[]/sqrt(2),
   далее берем dist/sqrt(2) в той точке, до которой дошли, и так далее.

Метод поиска рисок:
Идем по интересным точкам (точки скелета), в каждой ищем максимум расстояния
по направлениям. Запоминаем (направление и значение).


#include "stdafx.h"
#include "branchsplit.h"
#include "resource.h"
#include "binary.h"
#include "alloc.h"
#include "\vt5\awin\misc_dbg.h"

IMPLEMENT_DYNCREATE(CBranchSplit, CCmdTargetEx);

// {0FC59253-3DE9-4a06-9E79-B42BFB42D65E}
GUARD_IMPLEMENT_OLECREATE(CBranchSplit, "FiltersMain.CBranchSplit", 
0xfc59253, 0x3de9, 0x4a06, 0x9e, 0x79, 0xb4, 0x2b, 0xfb, 0x42, 0xd6, 0x5e);

// {E6884D6E-A94A-4e6a-BB64-7BCA83F4E588}
static const GUID guidBranchSplit = 
{ 0xe6884d6e, 0xa94a, 0x4e6a, { 0xbb, 0x64, 0x7b, 0xca, 0x83, 0xf4, 0xe5, 0x88 } };

ACTION_INFO_FULL(CBranchSplit, IDS_ACTION_BRANCH_SPLIT, IDS_MENU_FILTERS, IDS_TB_MAIN, guidBranchSplit);

ACTION_TARGET(CBranchSplit, szTargetAnydoc);

ACTION_ARG_LIST(CBranchSplit)
	ARG_BINIMAGE(_T("BinImage"))
	RES_BINIMAGE(_T("BinResult"))
	RES_IMAGE(_T("DbgResult"))
END_ACTION_ARG_LIST();

CBranchSplit::CBranchSplit(void)
{
}

CBranchSplit::~CBranchSplit(void)
{
}

#define round(x) int(floor((x)+0.5))

bool CBranchSplit::InvokeFilter()
{
	CBinImageWrp imgSrc(GetDataArgument());
	CBinImageWrp imgDst(GetDataResult("BinResult"));
	CImageWrp imgDbg(GetDataResult("DbgResult"));
	long cx = imgSrc.GetWidth(), cy = imgSrc.GetHeight();
	if( cy == 0 || cx == 0 )
		return false;
	CPoint	ptOffset;
	imgSrc->GetOffset(&ptOffset);

	if (!imgDst.CreateNew(cx, cy))
		return false;
	imgDst->SetOffset(ptOffset, true);

	imgDbg->CreateImage(cx, cy, _bstr_t("RGB"),-1);

	smart_alloc(srcRows, byte *, cy);
	smart_alloc(dstRows, byte *, cy);
	smart_alloc(tmpRows, byte *, cy);
	smart_alloc(tmpbuf, byte, cx*cy);

	smart_alloc(dbgRows0, color *, cy);
	smart_alloc(dbgRows1, color *, cy);
	smart_alloc(dbgRows2, color *, cy);


	{for(int y=0; y<cy; y++)
	{
		imgSrc->GetRow(&srcRows[y], y, false);
		imgDst->GetRow(&dstRows[y], y, false);
		dbgRows0[y] = imgDbg.GetRow(y, 0);
		dbgRows1[y] = imgDbg.GetRow(y, 1);
		dbgRows2[y] = imgDbg.GetRow(y, 2);
		tmpRows[y]=tmpbuf+cx*y;
		for(int x=0; x<cx; x++)
			tmpRows[y][x] = srcRows[y][x]>=128 ? 255 : 0;
	}}

	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y-1][x]+2);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y][x-1]+2);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y-1][x-1]+3);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y-1][x+1]+3);
		}
	}}
	{for(int y=cy-2; y>=1; y--)
	{
		for(int x=cx-2; x>=1; x--)
		{
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y+1][x]+2);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y][x+1]+2);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y+1][x-1]+3);
			tmpRows[y][x]=min(tmpRows[y][x],tmpRows[y+1][x+1]+3);
		}
	}}

	const int na=32;
	double co[na*7], si[na*7];
	double cc[na];
	double sa[7], sb[7], ss[7]; // коэф-ты разложения Фурье - по косинусу, синусу и квадрат модуля

	for(int i=0; i<na*7; i++)
	{
		co[i] = cos(i*2*PI/na);
		si[i] = sin(i*2*PI/na);
	}

	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			dbgRows0[y][x] = 0;
			dbgRows1[y][x] = tmpRows[y][x]*256;
			dbgRows2[y][x] = 0;
		}
	}}

	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			dbgRows0[y][x] = 0;
			dbgRows1[y][x] = tmpRows[y][x]*256;
			dbgRows2[y][x] = 0;

			//dstRows[y][x] = tmpRows[y][x]<3 && tmpRows[y][x]>0 ? 255 : 0;
			dstRows[y][x] = srcRows[y][x];
			int c = tmpRows[y][x];
			if( c>0	&& c>=tmpRows[y-1][x] && c>=tmpRows[y+1][x]
			&& c>=tmpRows[y][x-1] && c>=tmpRows[y][x+1] )
			{ // точка скелета - надо проверить ее
				double r = tmpRows[y][x] / 2.0;
				if(r>x) r=x;
				if(r>cx-1-x) r=cx-1-x;
				if(r>y) r=y;
				if(r>cy-1-y) r=cy-1-y;

				// проверка точек скелета - отсечение крайних и сомнительных
				double c_tst=0;
				double r2 = r/2;
				for(int i=0; i<na; i++)
				{
					int xx = x + round(r2*co[i]);
					int yy = y + round(r2*si[i]);
                    double c = tmpRows[yy][xx];
					if(c>c_tst) c_tst=c;
				}
				c_tst = c_tst/2.0; // радиус в самой "толстой" из соседних точек скелета

				if(c_tst>r) continue;
				//dstRows[y][x] = 0;

				// для удобства сложим точки с окружности в массив
				for(int i=0; i<na; i++)
				{
					int xx = x + round(r*co[i]);
					int yy = y + round(r*si[i]);
                    cc[i] = tmpRows[yy][xx];
				}

				for(int j=0; j<7; j++)
				{
					sa[j] = 0; sb[j] = 0;
					for(int i=0; i<na; i++)
					{
						sa[j] += cc[i]*co[i*j];
						sb[j] += cc[i]*si[i*j];
					}
					ss[j] = sa[j]*sa[j] + sb[j]*sb[j];
				}

				ss[6] = 0;

				//_trace_file( "BranchSplit.log", "(%i,%i)\t\t%4.0f\t%4.0f\t%4.0f\t%4.0f\t%4.0f\t%4.0f",
				//	x, y, ss[0], ss[1], ss[2], ss[3], ss[4], ss[5] );

				double s=0, a0=0, a2=0, b2=0; // энергия, коэф-ты 0 и 2 гармоник

				double c_max=0, c_min=1e100;

				for(int i=0; i<na; i++)
				{
					double c=cc[i];
					if(c>c_max) c_max=c;
					if(c<c_min) c_min=c;
					s += c*c;
					a0 += c;
					a2 += c*co[i*2];
					b2 += c*si[i*2];
				}
				s /= na; a0 /= na; a2 /= na; b2 /= na;
				double c0 = a0*a0, c2 = (a2*a2 + b2*b2) * 2;
				s -= c0;

				dbgRows0[y][x] = max(0,min(65535, s*256));
				//dbgRows0[y][x] = tmpRows[y][x];

				double t1 = sqrt(ss[1]+ss[3]*3*3+ss[4]*4*4+ss[5]*4*4)/6.5;
				double ratio = t1/r;
				dbgRows0[y][x] = max(0,min(65535, ratio*8192));

				if(ratio>3.2) dbgRows2[y][x] = 65535;
				if(ratio>3.2)
				{
					dstRows[y][x] = 0;
				}

				//if(ratio2>0.2 && a0>r*0.5) dstRows[y][x] = 0;
				//if(c_max<r*1.5) dstRows[y][x] = 0;
				//if(a0>r*0.5) dstRows[y][x] = 0;
			}
		}
	}}

	// дополнительная фильтрация - чтобы из нескольких точек оставлять только одну
	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			if(srcRows[y][x]<128 || dstRows[y][x]>=128) continue;
			// обрабатываем только проколы - то, что есть в src, но отсутствует в dst

			int c = tmpRows[y][x];
			int r = tmpRows[y][x] / 3; // в полтора раза меньше местного радиуса
			int y0 = max(0,y-r);
			int y1 = min(cy-1,y+r);
			int x0 = max(0,x-r);
			int x1 = min(cx-1,x+r);
			for(int yy=y0; yy<=y1; yy++)
			{
				for(int xx=x0; xx<=x1; xx++)
				{
					if(srcRows[yy][xx]<128 || dstRows[yy][xx]>=128) continue;
					if(tmpRows[yy][xx]<=c) dstRows[yy][xx] = 255; // убрать лишнее
					// причем из равных должен остаться только один
					// в цикле и себя зашибем - потом восстановим
				}
			}
			dstRows[y][x] = 0; // восстановление себя
		}
	}}


	// эксперимент - проведение разрезов
	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			if(srcRows[y][x]<128 || dstRows[y][x]) continue;
			// обрабатываем только проколы - то, что есть в src, но отсутствует в dst

			double r = tmpRows[y][x] / 2.0;
			if(r>x) r=x;
			if(r>cx-1-x) r=cx-1-x;
			if(r>y) r=y;
			if(r>cy-1-y) r=cy-1-y;

			// для удобства сложим точки с окружности в массив
			for(int i=0; i<na; i++)
			{
				int xx = x + round(r*co[i]);
				int yy = y + round(r*si[i]);
				cc[i] = tmpRows[yy][xx];
			}

			int i_min=0, i_max=0;
			for(int i=0; i<na; i++)
			{
				if(cc[i]<cc[i_min]) i_min=i;
				if(cc[i]>cc[i_max]) i_max=i;
			}

			// странный цикл - пройдемся по кругу, разобрав его на участки
			// по threshold и пометив минимум в каждом
			int cc_marks[na];

			int i_min2=i_max;
			int n_valleys = 0;
			for(int j=1; j<=na; j++)
			{
				int i = (j+i_max) % na;
				cc_marks[i] = 0;
				if( cc[i]-cc[i_min] >= (cc[i_max]-cc[i_min])*0.5 )
				{ // выскочили за пределы впадины - пометим лучшую точку в ней
					if( cc[i_min2]-cc[i_min] < (cc[i_max]-cc[i_min])*0.4 )
					{
						cc_marks[i_min2] = 1;
						n_valleys ++;
					}
					i_min2 = i; // начало следующего цикла
				}
				if(cc[i]<cc[i_min2]) i_min2=i;
			}

			if(n_valleys<2)
			{
				break; // Ахтунг! не смогли провести хотя бы два разреза!
			}

			for(int i=0; i<na; i++)
			{
				if(!cc_marks[i]) continue;
				if( cc[i]-cc[i_min] > (cc[i_max]-cc[i_min])*0.4 ) continue;

				double x1=x, y1=y;
				for(int k=0; ; k++)
				{
					int xx = round(x1);
					int yy = round(y1);
					if(xx<0 || xx>=cx || yy<0 || yy>=cy) break;
					if(srcRows[yy][xx]<128) break;
					dstRows[yy][xx] = 1; // не в 0 - чтобы не мешать последующей работе
					int x_min=xx, y_min=yy;
					if(xx>0 && xx<cx-1 && yy>0 && yy<cy-1)
					{
						for(int y2=yy-1; y2<=yy+1; y2++)
						{
							for(int x2=xx-1; x2<=xx+1; x2++)
							{
								if(tmpRows[y2][x2]<tmpRows[y_min][x_min])
								{
									y_min = y2;
									x_min = x2;
								}
							}
						}
					}
					//x1 += co[i];
					//y1 += si[i];
					//x1 = x_min;
					//y1 = y_min;
					x1 += co[i]*1.0 + (x_min-xx)*0.0;
					y1 += si[i]*1.0 + (y_min-yy)*0.0;
					//if(round(x1)==xx && round(y1)==yy) break;
				}
			}
		}
	}}

	StartNotification(cy);
	for (int y = 0; y < cy; y++)
	{
		Notify(y);
		for(int x=0; x<cx; x++)
		{
			dstRows[y][x] = dstRows[y][x]>=128 ? 255 : 0;
		}
	}
	return true;
}
