#include "stdafx.h"
#include "FindCircles3.h"
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
#include "FastMarch.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CFindCircles3Info::s_pargs[] = 
{
	{"MinRadius",	szArgumentTypeInt, "16", true, false },
	{"MaxRadius",	szArgumentTypeInt, "32", true, false },
	{"MinDistance",	szArgumentTypeInt, "16", true, false },
	{"MaxDistortion",	szArgumentTypeDouble, "0.25", true, false },
	{"Threshold",	szArgumentTypeDouble, "0.5", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Objects",szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

bool CFindCircles3::CanDeleteArgument( CFilterArgument *pa )
{
	if( pa->m_bstrArgName == _bstr_t("Img") )
		return false;

	return __super::CanDeleteArgument(pa);
}


static double inline sqr(double x)
{ return x*x; }

static int inline iround(double x)
{ return int(floor(x+0.5)); }


class CChord
{
public:
	int x1,y1;
	int x2,y2;
	int c1,c2;
	int i1,i2;
};

//////////////////////////////////////////////////////////////////////////
// класс для уточнения параметров окружности методом наименьших квадратов
// (может использоваться как вспомогательный для реализации RANSAC)
class CCircle
{
public:
	CCircle();
	CCircle(double x, double y, double r);
	double x0, y0, r0;
	double a[12]; // матрица коэф-тов для поиска dx, dy и r0 методом наименьших квадратов
	// порядок коэф-тов: a[0]*r0 + a[1]*dx + a[2]*dy = a[3] (3 строки, первая для d/dr0, вторая для d/dx, третья для d/dy)

	void BeginUpdate(); // Старт метода наименьших квадратов
	void AddPoint(double x, double y, double w); // добавить точку к окружности (в данные метода наименьших квадратов), вес точки w
	void EndUpdate(); // Завершение метода наименьших квадратов - вычисление x0, y0, r0
};

CCircle::CCircle()
{
}

CCircle::CCircle(double x, double y, double r)
{
	x0=x; y0=y; r0=r;
}

void CCircle::BeginUpdate()
{
	for(int i=0; i<12; i++) a[i]=0;
}

void CCircle::AddPoint(double x, double y, double w)
{
	x -= x0; y-= y0;
	double r = _hypot(x,y);
	double co = x/r, si = y/r;
	//if(	fabs(r-test_r)<15 )
	{
		a[0] += w;    a[1] += w*co;    a[2] += w*si;    a[3] += w*r;
		a[4] += w*co; a[5] += w*co*co; a[6] += w*co*si; a[7] += w*x;
		a[8] += w*si; a[9] += w*co*si; a[10]+= w*si*si; a[11]+= w*y;
	}
}

void CCircle::EndUpdate()
{	// коррекция круга - вычисление координат и радиуса
	// тупо метод Гаусса без поиска главного элемента
	// можно проапдейтить - в случае деления на 0 окружность не найдена
	for(int i=0; i<3; i++)
	{
		double t = a[i*4+i];
		if(t<1e-6)
		{
			return; // ошибка - ну и не будем апдейтить окружность
		}

		for(int j=i; j<4; j++) a[i*4+j] /= t;
		for(int i1=i+1; i1<3; i1++)
		{
			double t = a[i1*4+i];
			for(int j=i; j<4; j++) a[i1*4+j] -= a[i*4+j]*t;
		}
	}
	r0 = a[3];
	x0 += a[7];
	y0 += a[11];
}
//////////////////////////////////////////////////////////////////////////


bool CFindCircles3::InvokeFilter()
{
	m_sptrSrc = GetDataArgument("Img");
	m_sptrList = GetDataResult("Objects");

	long nMinRadius = GetArgLong("MinRadius");
	long nMaxRadius = GetArgLong("MaxRadius");
	long nMinDistance = GetArgLong("MinDistance");
	double fMaxDistortion = GetArgDouble("MaxDistortion");
	double fThreshold = GetArgDouble("Threshold");

	int nStep = int(nMinRadius*fMaxDistortion);
	nStep = max(1,nStep); // на таком шаге ищем окружности толщиной 1 пиксел и больше

	int nStep2 = int(nMaxRadius-nMinRadius);
	// а на таком шаге все окружности радиуса от Min до Max укладываются в 1 пиксел

	// Концепция предварительного поиска:
	// 1. Max по маске Step*Step
	// 2. В каждом квадрате Step2*Step2 ищем максимум

	if( m_sptrSrc == NULL || m_sptrList==NULL )
		return false;

	int cx,cy;
	m_sptrSrc->GetSize(&cx,&cy);
	int nPanes = ::GetImagePaneCount( m_sptrSrc );

	try
	{
		// StartNotification(cy,nPanes);
		StartNotification(100000,1/*nPanes*/,10);
		{
			smart_alloc(srcRows, color *, cy);
			smart_alloc(dbgRows, color *, cy);
			smart_alloc(dbg2Rows, color *, cy);

			{ // !!! debug
				IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
				IImage3Ptr      sptrI(ptrI);
				sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );
                for(int y=0; y<cy; y++)
					sptrI->GetRow(y, 0, &dbgRows[y]);
				// debug - вставка в документ вспомогательной картинки
				if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
				if(doc==0) return S_FALSE;
				SetToDocData(doc, sptrI);
				for(int y=0; y<cy; y++)
					for(int x=0; x<cx; x++)
						dbgRows[y][x] = 0;
			}
			{ // !!! debug
				IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
				IImage3Ptr      sptrI(ptrI);
				sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );
                for(int y=0; y<cy; y++)
					sptrI->GetRow(y, 0, &dbg2Rows[y]);
				// debug - вставка в документ вспомогательной картинки
				if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
				if(doc==0) return S_FALSE;
				SetToDocData(doc, sptrI);
				for(int y=0; y<cy; y++)
					for(int x=0; x<cx; x++)
						dbg2Rows[y][x] = 0;
			}

			// инициализация
			m_ppSrc = srcRows;
			m_cx = cx;
			m_cy = cy;
			int nPane=1;

			for(int y=0; y<cy; y++)
			{
				m_sptrSrc->GetRow(y, nPane, &srcRows[y]);
			}

			// инициализация данных для обхода кругов
			const int na=m_na;
			const int nr=m_nr;
			for(int i=0; i<na; i++)
			{
				m_co[i] = cos(i*2*PI/na);
				m_si[i] = sin(i*2*PI/na);
			}

			for(int j=0; j<nr; j++)
			{
				double r = nMinRadius + double(j)*(nMaxRadius-nMinRadius)/nr;
				m_rr[j]=r;
				for(int i=0; i<na; i++)
				{
					m_dx[i*nr+j] = iround( r*m_co[i] );
					m_dy[i*nr+j] = iround( r*m_si[i] );
				}
			}

			// test - для поиска середин ярких линий
			for(int y=1; y<cy; y++)
			{
				for(int x=1; x<cx; x++)
				{
					int c = min(dbg2Rows[y-1][x],dbg2Rows[y][x-1]) + srcRows[y][x]/32;
					dbg2Rows[y][x] = min(srcRows[y][x], c);
				}
			}
			for(int y=cy-2; y>=0; y--)
			{
				for(int x=cx-2; x>=0; x--)
				{
					int c = min(dbg2Rows[y+1][x],dbg2Rows[y][x+1]) + srcRows[y][x]/32;
					dbg2Rows[y][x] = min(dbg2Rows[y][x], c);
				}
			}

			// ------ Эксперимент: в каждом квадрате MinDistance*MinDistance найдем
			// максимум картинки
			int nx = (cx+nMinDistance-1) / nMinDistance;
			int ny = (cy+nMinDistance-1) / nMinDistance;
			int nn=nx*ny;

			smart_alloc(xx, int, nx*ny); // ключевые точки
			smart_alloc(yy, int, nx*ny);
			smart_alloc(cc, int, nx*ny); // яркости в ключевых точках
			smart_alloc(cc_min, int, nx*ny); // минимум яркости по квадратам - для отсева фона

			for(int i=0; i<nn; i++)
			{
				cc[i] = -1;
				cc_min[i] = 65536;
			}

			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int iy = y/nMinDistance;
					int ix = x/nMinDistance;
					i = iy*nx+ix;

					int c = srcRows[y][x];
					if(c<cc_min[i]) cc_min[i] = c;

					if(i>nx)
					{
						double d = _hypot(x-xx[i-nx-1],y-yy[i-nx-1]);
						d = min(d, _hypot(x-xx[i-nx],y-yy[i-nx]));
						d = min(d, _hypot(x-xx[i-nx+1],y-yy[i-nx+1]));
						d = min(d, _hypot(x-xx[i-1],y-yy[i-1]));
						if(d<nMinDistance/2) continue; // избегаем слишком близких точек
					}

					if(c>cc[i])
					{
						cc[i] = srcRows[y][x];
						yy[i] = y; xx[i] = x;
					}
				}
			}

			int di[8] = {1, nx+1, nx, nx-1, -1, -nx-1, -nx, -nx+1};

			//for(int i=0; i<nn; i++)
			//	dbgRows[yy[i]][xx[i]] = cc[i];

			smart_alloc(chords, CChord, nn*8);
			int nChords = 0;

			for(int i=0; i<nn; i++)
			{
				int ix = i%nx, iy = i/nx;
				if(ix<=0 || ix>=nx-1 || iy<=0 || iy>=ny-1)
					continue;
				int x1 = xx[i], y1=yy[i], c1 = cc[i];
				if(!c1) continue;

				int cnt_white = 0;
				int min_max = 65536;
				if(cc_min[i]>c1*3/4) cnt_white+=4;
				for(int k=0; k<8; k+=2)
				{
					int i2 = i + di[k];
					if(cc_min[i2]>c1*3/4) cnt_white++;
					if(cc_min[i2]<min_max) min_max = cc_min[i2];
				}
				//if(cnt_white>=4) continue;

				double s_best = -1;
				double ss[8];
				for(int k=0; k<8; k+=2)
				{
					ss[k]=0;
					int i2 = i + di[k];
					int x2 = xx[i2], y2=yy[i2], c2 = cc[i2];
					if(!c2) continue;
					//if(c2>c1) continue; // попробуем пока вести линии только от более ярких к менее ярким

					int L = max(abs(x2-x1), abs(y2-y1));
					if(L<1)
						continue;
					double dx = double(x2-x1)/L;
					double dy = double(y2-y1)/L;
					double s=0;
					for(int j=0; j<=L; j++)
					{ // проверка линии между точками
						int x = iround(x1+dx*j); //можно в целых быстрее
						int y = iround(y1+dy*j); //можно в целых быстрее
						int c = srcRows[y][x];
						s += c;
					}
					s /= L+1;
					s = s/max(1,max(c1,c2));
					ss[k] = s;
					if(s>s_best) s_best = s;
				}

				int cnt=0;
				for(int k=0; k<8; k+=2)
				{
					if(ss[k]>=s_best*0.9) cnt++;
				}
				//if(cnt>2)
				//	continue;

				if(c1>50000)
				{
					c1=c1;
				}

				double s0=0,sx=0,sy=0,sxx=0,sxy=0,syy=0;
				for(int k=0; k<8; k+=2)
				{
					int i2 = i + di[k];
					int x2 = xx[i2], y2=yy[i2], c2 = cc[i2];
					double s = ss[k];
					int dx = x2-x1, dy=y2-y1;
					s0 += s;
					sx += s*dx; sy += s*dy;
					sxx += s*dx*dx; sxy += s*dx*dy; syy += s*dy*dy;
				}
				double d = sxx-syy; d = d*d+sqr(sxy)*4;
				double ellipcity = d / max(1e-6, sqr(sxx+syy));
				double d2 = max(1e-6,(sxx+syy)/2);
				sxx/=d2; sxy/=d2; syy/=d2;

				for(int k=0; k<8; k+=2)
				{
					int i2 = i + di[k];
					int x2 = xx[i2], y2=yy[i2], c2 = cc[i2];
					int dx = x2-x1, dy=y2-y1;
					double w = dx*sxx*dx + 2*dx*sxy*dy + dy*syy*dy;
					double h = dx*dx + dy*dy;
					if(h>0) w/=h;
					ss[k] *= w;
				}

				int k1=0, k2=0;
				for(int k=0; k<8; k+=2)
				{
					if(ss[k]>ss[k1])
					{
						k2 = k1;
						k1 = k;
					}
					else if(ss[k]>ss[k2]) k2=k;
				}


				//if(ellipcity<0.2) continue;

				for(int k=0; k<8; k+=2)
				{
					//if(k!=k1 && k!=k2) continue;
					int i2 = i + di[k];
					int x2 = xx[i2], y2=yy[i2], c2 = cc[i2];
					//if(c2>c1) continue; // попробуем пока вести линии только от более ярких к менее ярким
					if( c2>c1 || (c2==c1 && k<4) )
						continue;

					//if( c2<16384) continue;


					int L = max(abs(x2-x1), abs(y2-y1));
					if(L<1) continue;
					double dx = double(x2-x1)/L;
					double dy = double(y2-y1)/L;

					//if(ss[k]>0.75)
					{
						// воткнем в список хорд
						//if(c1>c2 || (c1==c2 && k<4)) // каждая хорда входит 1 раз, причем с1>=c2
						{
							chords[nChords].x1 = x1;
							chords[nChords].y1 = y1;
							chords[nChords].x2 = x2;
							chords[nChords].y2 = y2;
							chords[nChords].c1 = c1;
							chords[nChords].c2 = c2;
							nChords++;
						}
						int c = min(c1,c2);

						//c = max(0, c - min_max);
						//double dc = double(c2-c1)/L;
						for(int j=0; j<=L; j++)
						{ // провести линию между точками
							int x = iround(x1+dx*j); //можно в целых быстрее
							int y = iround(y1+dy*j); //можно в целых быстрее
							//int c = iround(c1+dc*j); //можно в целых быстрее
							dbgRows[y][x] = max(c,dbgRows[y][x]);
						}
					}
				}
			}

			nChords += 0;

			const int nc=100;
			CCircle rings[nc];
			for(int j=0; j<nc; j++)
			{
				CCircle* pr = rings+j;
				pr->x0 = rand() % cx;
				pr->y0 = rand() % cy;
				pr->r0 = 30;
			}

			for(int nPass=0; nPass<100; nPass++)
			{
				for(int j=0; j<nc; j++)
				{
					CCircle* pr = rings+j;
					pr->BeginUpdate();
				}
				for(int i=0; i<nChords; i++)
				{
					CChord* pc = chords+i;
					int c = min(pc->c1,pc->c2);

					if(c>32768)
					{
						for(int j=0; j<nc; j++)
						{
							CCircle* pr = rings+j;
							// коррекция круга - подсчет сумм
							double r1 = _hypot( pc->x1 - pr->x0, pc->y1 - pr->y0 );
							double r2 = _hypot( pc->x2 - pr->x0, pc->y2 - pr->y0 );
							double len = _hypot( pc->x2 - pc->x1, pc->y2 - pc->y1 );
							double r = (r1+r2)/2;

							//if(	fabs(r-test_r)<15 )
							{
								double w = sqr(c/65536.0);
								double angle_d = (r2-r1)/max(len,1.0);
								w *= exp( -sqr(angle_d/0.5));
								w *= exp( -sqr((r - pr->r0)/4) );
								pr->AddPoint((pc->x1 + pc->x2)/2.0, (pc->y1 + pc->y2)/2.0, w);
							}
						}
					}
				}

				double s=0;
				for(int j=0; j<nc; j++)
				{
					CCircle* pr = rings+j;
					s += pr->a[0]; // сумма весов
				}
				s /= nc; // средняя сумма весов по всем кругам

				for(int j=0; j<nc; j++)
				{
					CCircle* pr = rings+j;
					if(pr->a[0]>s || nPass>80)
						pr->EndUpdate();
					else
					{ // убогих (с малой суммой весов) переинитим
						pr->x0 = rand() % cx;
						pr->y0 = rand() % cy;
					}						
				}
			}

			for(int j=0; j<nc; j++)
			{
				CCircle* pr = rings+j;
				for(int k=0; k<256; k++)
				{
					int x = iround(pr->x0 + pr->r0*cos(k*PI/128));
					int y = iround(pr->y0 + pr->r0*sin(k*PI/128));
					if(x>=0 && x<cx && y>=0 && y<cy)
						dbg2Rows[y][x]=65535;
				}
			}

			// ------ собственно работа
			/*
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int s_max = 0;
					for(int j=0; j<nr; j++)
					{
						int s = 0;
						for(int i=0; i<na; i++)
						{
							int x1 = x+m_dx[i*nr+j], y1 = y+m_dy[i*nr+j];
							if( x1>=0 && y1>=0 && x1<cx && y1<cy )
							{
								int n = srcRows[y1][x1];
								s += n;
							}
						}
						s_max = max(s,s_max);
					}
					dbgRows[y][x] = color(s_max/na);
				}
				Notify(y);
			}
			*/

		}
		FinishNotification();
	}
	catch( ... )
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

