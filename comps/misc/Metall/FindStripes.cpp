#include "StdAfx.h"
#include "findstripes.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "aam_utils.h"
#include <limits.h>
#include "\vt5\awin\misc_ptr.h"
namespace {
	int  round(double num) {
			return int((num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5));
	}
}

_ainfo_base::arg CFindStripesInfo::s_pargs[] =
{
	{"Pass",    szArgumentTypeInt, "0", true, false }, // нужно последовательно выполнить проходы 0, 1. А между ними - сглаживание
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Stripes",   szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CFindStripes::CFindStripes()
{
}

/////////////////////////////////////////////////////////////////////////////
CFindStripes::~CFindStripes()
{
}

//static inline int iround(double x)
//{
//	return int(floor(x+0.5));
//}

static int nCompareMethod=2;
static int nAverageMethod=1;
static double fGradLimit=0.2;

inline double VectorDist(double* a, double* b, long nPanes)
{	// расстояние
	// nCompareMethod==1: VectorDist = 1 - cos(angle)  (angle - угол между векторами)
	// nCompareMethod==2: VectorDist = расстояние от b до линии a, деленое на длину a

	double aa=0.0, ab=0.0, bb=0.0;
	for(int i=0; i<nPanes; i++)
	{
		aa += a[i]*a[i];
		ab += a[i]*b[i];
		bb += b[i]*b[i];
	}

	//!!!!!!!!!!!!!!!!!!!!
	return (aa+bb-2*ab)/(32768.0*32768.0);
	//!!!!!!!!!!!!!!!!!!!!

	if(nCompareMethod==1)
	{
		double d=aa*bb;
		if(d>0)
		{
			double d1=ab/sqrt(d);
			return 1.0 - ab/sqrt(d);
		}
		else return 1.0;
	}
	else if(nCompareMethod==2)
	{
		if(aa>0)
		{
			double d1=ab/aa;
			d1 = bb/aa - d1*d1;
			if(d1>=0) return sqrt(d1);
			else return 0;
		}
		else return 1.0;
	}
	else return 1; // по умолчанию расстояние большое
}

const int MAXPANES=32;

class CCluster
{
public:
	long m_nPoints;
	long m_nPanes;
	double m_fWeightSum;
	double m_center[MAXPANES];
	CCluster& operator+= (double *point);
	CCluster& operator+= (CCluster& c2);
	double weight(double *point);
	double dist(double *point) { return VectorDist(m_center, point, m_nPanes); };
};

inline double CCluster::weight(double *point)
{	// nAverageMethod==0 - все веса равны 1
	// nAverageMethod==1 - вес равен максимальной проекции
	if(nAverageMethod==0) return 1;
	double w=0;
	for(int i=0; i<m_nPanes; i++)
		if(point[i]>w) w=point[i];
	return w;
}

inline CCluster& CCluster::operator+= (double *point)
{
	double w1 = weight(point);
	double w = m_fWeightSum + w1;
	for(int i=0; i<m_nPanes; i++)
		m_center[i] = (m_center[i]*m_fWeightSum + point[i]*w1) / w;
    m_nPoints++;
	m_fWeightSum = w;
	return *this;
};

inline CCluster& CCluster::operator+= (CCluster& c2)
{
	double w = m_fWeightSum + c2.m_fWeightSum;
	for(int i=0; i<m_nPanes; i++)
		m_center[i] = (m_center[i]*m_fWeightSum + c2.m_center[i]*c2.m_fWeightSum) / w;
    m_nPoints += c2.m_nPoints;
	m_fWeightSum = w;
	return *this;
};

#define INSERTPOINT \
{ \
	cnt[y][x] = index; \
	cluster += col; \
	ps[nPoints] = _point(x,y); \
	nPoints++; \
	if(nPoints>=points.size()) \
		{ points.alloc(nPoints*2,1); ps=points.ptr(); } \
}

#define CHECK \
{ \
	for(int pane=0; pane<nPanes; pane++) \
		col[pane] = img[pane*cy+y][x]; \
	if(cnt[y][x]==0 && cluster.dist(col)<fGradLimit ) \
		INSERTPOINT \
}

static void MarkCluster(color **img, color **cnt, int cx, int cy, int nPanes,
						int x, int y, int index, CCluster &cluster )
{
	_ptr_t<_point> points(256);
	_point *ps=points.ptr();

	//CCluster cluster;
	cluster.m_nPoints=0;
	cluster.m_nPanes=nPanes;
	cluster.m_fWeightSum = 0;
	for(int pane=0; pane<nPanes; pane++) cluster.m_center[pane] = 0.0;

	double col[MAXPANES];
	for(int pane=0; pane<nPanes; pane++) col[pane] = img[pane*cy+y][x];

	long nPoints=0;
	long nProcessed=0;

	INSERTPOINT;

	//double gradLimit = 1-cos(10*PI/180);
	//double gradLimit = 0.2;

	while(nProcessed<nPoints)
	{
		int x=ps[nProcessed].x, y=ps[nProcessed].y;
		y--; CHECK; y++;
		x++; CHECK; x--;
		y++; CHECK; y--;
		x--; CHECK; x++;
		nProcessed++;
	}
}

bool CFindStripes::InvokeFilter()
{
	IImage3Ptr ptrSrc( GetDataArgument() );
	IImage3Ptr ptrDst( GetDataResult() );

	if( ptrSrc==0 || ptrDst==0 ) return false;

	int cx=0,cy=0;
	ptrSrc->GetSize(&cx,&cy);
	POINT ptOffset;
	ptrSrc->GetOffset(&ptOffset);

	StartNotification(cy);

	ptrDst->CreateImage(cx, cy, _bstr_t("RGB"),-1);
	ptrDst->SetOffset(ptOffset, false);

	int nPane = 0;

	smart_alloc(srcRows0, color *, cy);
	smart_alloc(srcRows1, color *, cy);
	smart_alloc(srcRows2, color *, cy);
	smart_alloc(dstRows0, color *, cy);
	smart_alloc(dstRows1, color *, cy);
	smart_alloc(dstRows2, color *, cy);
 	for(int y=0; y<cy; y++)
	{
		ptrSrc->GetRow(y, 0, &srcRows0[y]);
		ptrSrc->GetRow(y, 1, &srcRows1[y]);
		ptrSrc->GetRow(y, 2, &srcRows2[y]);
		ptrDst->GetRow(y, 0, &dstRows0[y]);
		ptrDst->GetRow(y, 1, &dstRows1[y]);
		ptrDst->GetRow(y, 2, &dstRows2[y]);
	}

 	for(int y=0; y<cy; y++)
	{
		Notify(y);
 		for(int x=0; x<cx; x++)
		{
			dstRows0[y][x] = 32768;
			dstRows1[y][x] = 32768;
			dstRows2[y][x] = 32768;
		}
	}

	int nPass = GetArgLong("Pass");

	if(0 == nPass)
	{
 		for(int y=1; y<cy-1; y++)
		{
			Notify(y);
 			for(int x=1; x<cx-1; x++)
			{
				double Re = ( double(srcRows1[y][x+1]) - srcRows1[y][x-1] )/65536.0;
				double Im = ( double(srcRows1[y+1][x]) - srcRows1[y-1][x] )/65536.0;
				double Re2 = Re*Re - Im*Im;
				double Im2 = 2*Re*Im;
				double h2 = _hypot(Re2,Im2);

				dstRows0[y][x] = max(0,min(65535,32768 + Re2*32768));
				dstRows1[y][x] = max(0,min(65535,32768 + Im2*32768));
				dstRows2[y][x] = max(0,min(65535,32768 + h2*32768));
			}
		}

		//for(int y=0; y<cy; y++)
		//{
		//	Notify(y);
		//	for(int x=0; x<cx; x++)
		//	{
		//		double Re = dstRows0[y][x]-32768;
		//		double Im = dstRows1[y][x]-32768;
		//		double h = _hypot(Re, Im);
		//		double a = atan2(Re, Im);

		//		dstRows0[y][x] = max(0,min(65535, 32768 + h*cos(a)));
		//		dstRows1[y][x] = max(0,min(65535, 32768 + h*cos(a+PI*2/3)));
		//		dstRows2[y][x] = max(0,min(65535, 32768 + h*cos(a-PI*2/3)));
		//	}
		//}
	}
	else if(1 == nPass)
	{
 		for(int y=0; y<cy; y++)
		{
 			for(int x=0; x<cx; x++)
			{
				dstRows0[y][x] = srcRows0[y][x];
				dstRows1[y][x] = srcRows1[y][x];
				dstRows2[y][x] = srcRows2[y][x];
			}
		}

		double alpha = 0.7;

		for(int y=1; y<cy; y++)
		{
			Notify(y);
 			for(int x=1; x<cx; x++)
			{
				double c2 = 32768+(dstRows2[y][x-1]-32768)*alpha;
				if(dstRows2[y][x]<c2)
				{
					dstRows0[y][x] = 32768+(dstRows0[y][x-1]-32768)*alpha;
					dstRows1[y][x] = 32768+(dstRows1[y][x-1]-32768)*alpha;
                    dstRows2[y][x] = c2;
				}
				c2 = 32768+(dstRows2[y-1][x]-32768)*alpha;
				if(dstRows2[y][x]<c2)
				{
					dstRows0[y][x] = 32768+(dstRows0[y-1][x]-32768)*alpha;
					dstRows1[y][x] = 32768+(dstRows1[y-1][x]-32768)*alpha;
                    dstRows2[y][x] = c2;
				}
			}
		}
		for(int y=cy-2; y>=0; y--)
		{
 			for(int x=cx-2; x>=0; x--)
			{
				double c2 = 32768+(dstRows2[y][x+1]-32768)*alpha;
				if(dstRows2[y][x]<c2)
				{
					dstRows0[y][x] = 32768+(dstRows0[y][x+1]-32768)*alpha;
					dstRows1[y][x] = 32768+(dstRows1[y][x+1]-32768)*alpha;
                    dstRows2[y][x] = c2;
				}
				c2 = 32768+(dstRows2[y+1][x]-32768)*alpha;
				if(dstRows2[y][x]<c2)
				{
					dstRows0[y][x] = 32768+(dstRows0[y+1][x]-32768)*alpha;
					dstRows1[y][x] = 32768+(dstRows1[y+1][x]-32768)*alpha;
                    dstRows2[y][x] = c2;
				}
			}
		}
	}
	else if(2 == nPass)
	{
 		for(int y=0; y<cy; y++)
		{
			Notify(y);
 			for(int x=0; x<cx; x++)
			{
				double Re = srcRows0[y][x]-32768;
				double Im = srcRows1[y][x]-32768;
				double h2 = srcRows2[y][x]-32768;
				//double h2 = _hypot(Re,Im);
				//h2 = 32767;
				double h = _hypot(Re, Im);
				double a = atan2(Re, Im);
				double Re_norm = Re / (h2+1.0);
				double Im_norm = Im / (h2+1.0);
				double h_norm = h / (h2+1.0);

				//dstRows0[y][x] = max(0,min(65535, 65535*h_norm));
				//dstRows1[y][x] = max(0,min(65535, 65535*h_norm));
				//dstRows2[y][x] = max(0,min(65535, 65535*h_norm));
				dstRows0[y][x] = max(0,min(65535, 32768*h_norm*(1+cos(a))));
				dstRows1[y][x] = max(0,min(65535, 32768*h_norm*(1+cos(a+PI*2/3))));
				dstRows2[y][x] = max(0,min(65535, 32768*h_norm*(1+cos(a-PI*2/3))));
			}
		}
	}
	else if(100 == nPass)
	{ // извратный вариант - прямой поиск направленных отрезков...
		const int na=16, nr=16;
		double co[na*2], si[na*2];
		for(int ia=0; ia<na*2; ia++)
		{
			co[ia] = cos(ia*2*PI/na);
			si[ia] = sin(ia*2*PI/na);
		}

 		for(int y=0; y<cy; y++)
		{
			Notify(y);
 			for(int x=0; x<cx; x++)
			{
				int c = srcRows1[y][x];
				double sx=0, sy=0;
				for(int ia=0; ia<na; ia++)
				{
					for(int ir=1; ir<nr; ir++)
					{
						int xx = x+round(ir*co[ia]);
						int yy = y+round(ir*si[ia]);
						if(xx<0 || xx>=cx || yy<0 || yy>=cy)
							break;
						if(abs(srcRows1[yy][xx]-c)>16384)
							break;
					}
					sx += ir*co[ia*2];
					sy += ir*si[ia*2];
				}
				sx /= na*nr; // от 0 до 1
				sy /= na*nr;
				double h2 = _hypot(sx,sy);

				dstRows0[y][x] = max(0,min(65535,32768 + sx*32768));
				dstRows1[y][x] = max(0,min(65535,32768 + sy*32768));
				dstRows2[y][x] = 32768; //max(0,min(65535,32768 + h2*32768));
			}
		}
	}
	else if(101 == nPass)
	{ // второй извратный вариант - считаем число пересечений на радиусах
		const int na=16, nr=32;
		double co[na*2], si[na*2];
		for(int ia=0; ia<na*2; ia++)
		{
			co[ia] = cos(ia*2*PI/na);
			si[ia] = sin(ia*2*PI/na);
		}

 		for(int y=0; y<cy; y++)
		{
			Notify(y);
 			for(int x=0; x<cx; x++)
			{
				int c = srcRows1[y][x];
				double sx=0, sy=0;
				for(int ia=0; ia<na; ia++)
				{
					int nBounds = 0;
					int c0 = c;
					for(int ir=1; ir<nr; ir++)
					{
						int xx = x+round(ir*co[ia]);
						int yy = y+round(ir*si[ia]);
						if(xx<0 || xx>=cx || yy<0 || yy>=cy)
							break;
						if( (srcRows1[yy][xx]<32768) ^ (c>32768) )
						{
							nBounds++;
							c = srcRows1[yy][xx];
						}
					}
					sx += nBounds*co[ia*2];
					sy += nBounds*si[ia*2];
				}
				sx /= na;
				sy /= na;
				double h2 = _hypot(sx,sy);

				dstRows0[y][x] = max(0,min(65535,32768 + sx*4096));
				dstRows1[y][x] = max(0,min(65535,32768 + sy*4096));
				dstRows2[y][x] = 32768; //max(0,min(65535,32768 + h2*32768));
			}
		}
	}
	else if(200 == nPass)
	{ // сегментация по областям
 		for(int y=0; y<cy; y++)
		{
 			for(int x=0; x<cx; x++)
			{
				dstRows0[y][x] = x; // координаты опорной точки области
				dstRows1[y][x] = y;
				dstRows2[y][x] = 0; // номер кластера
			}
		}

		int nPixelOrder=1;

		smart_alloc(counters, int, 256);
		for(int i=0; i<256; i++) counters[i]=0;
		// разметка: то, что за пределами маски, и края = 65535,
		// остальное - случайные числа 0..255, по которым будем сортировать
		// (и выделять кластеры, начиная с центров = 255)
		for(int x=0; x<cx; x++) dstRows2[0][x]=65535;
		for(int y=1; y<cy-1; y++)
		{
			dstRows2[y][0]=65535;
			for(int x=1; x<cx-1; x++)
			{
				int n=0; // nPixelOrder==0 - по порядку, свалим все в один массив
				if(nPixelOrder==1) n = rand()%256; // в случайном порядке
				else if(nPixelOrder==2) n = srcRows0[y+cy][x]/256; // по DAPI
				else if(nPixelOrder==3)
				{
					n = 255-srcRows0[y][x]/256;
				}
				dstRows2[y][x] = n;
				counters[n]++;
			}
			dstRows2[y][cx-1]=65535;
		}
		for(int x=0; x<cx; x++) dstRows2[cy-1][x]=65535;

		typedef _ptr_t<_point> _points;
		smart_alloc(start_points, _points, 256);
		for(int i=0; i<256; i++)
		{
			start_points[i].alloc(counters[i]);
			counters[i]=0;
		}
		for(int y=1; y<cy-1; y++)
		{
			for(int x=1; x<cx-1; x++)
			{
				int n=dstRows2[y][x];
				if(n<256)
				{
					dstRows2[y][x]=0; // обнулим - числа уже не нужны
					start_points[n].ptr()[counters[n]] = _point(x,y);
					counters[n]++;
				}
			}
		}

		int nClusters=1; // нулевой кластер пустой
		_ptr_t<CCluster> clusters(256);
		for(int n=255; n>=0; n--)
		{
			for(int j=0; j<counters[n]; j++)
			{
				_point *ppt=start_points[n].ptr()+j;
				if(dstRows2[ppt->y][ppt->x]==0)
				{
					MarkCluster(srcRows0, dstRows2, cx,cy, 1, ppt->x,ppt->y, nClusters, clusters.ptr()[nClusters]);
					nClusters++;
					if( nClusters>=clusters.size() ) clusters.alloc(nClusters*2);
				}
			}
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows0[y][x] = dstRows2[y][x]*27103;
				dstRows1[y][x] = dstRows2[y][x]*11103;
				dstRows2[y][x] = dstRows2[y][x]*43103;
			}
		}

	}
	else if(300 == nPass)
	{ // обработка квадратов
		int w = 32;

		int threshold = 16000;

		int nx = (cx+w-1)/w;
		int ny = (cy+w-1)/w;

		for(int yy=0; yy<ny-1; yy++)
		{
			for(int xx=0; xx<nx-1; xx++)
			{
				int x0 = xx*w, y0 = yy*w;
				int x1 = min(cx, x0+w), y1 = min(cy, y0+w);

				//double c0[4]={0,1,0,-1}, c1[4]={1,0,-1,0}; // вектора 4 точек для k-means
				double c0[4]={0,65535,65535,0}, c1[4]={65535,0,65535,0}; // вектора 4 точек для k-means
				double sw[4]={0,0,0,0}, s0[4]={0,0,0,0}, s1[4]={0,0,0,0};
				// сумма весов и суммы по двум панам (Re и Im)
				
				// найдем методом k-средних основные цвета
				int cnt = 0;
				for(int pass=0; pass<1; pass++)
				{
					cnt = 0;
					for(int k=0; k<4; k++)
					{
						sw[k]=0; s0[k]=0; s1[k]=0;
					}
 					for(int y=y0; y<y1; y++)
					{
 						for(int x=x0; x<x1; x++)
						{
							double w = srcRows2[y][x]-32768;
							w -= threshold;
							if(w<=0) continue; // неинтересные точки нах
							w = 1;

							cnt++;
							//double d_best=-1e20;
							double d_best=1e20;
							int k_best=0;
							for(int k=0; k<4; k++)
							{
								//double d = (srcRows0[y][x]-32768.0)*c0[k]+(srcRows1[y][x]-32768.0)*c1[k];
								double a0 = srcRows0[y][x] - c0[k], a1 = srcRows1[y][x] - c1[k];
								double d = a0*a0 + a1*a1;
								//if(d>d_best)
								if(d<d_best)
								{
									d_best=d;
									k_best=k;
								}
							}
							sw[k_best] += w;
							s0[k_best] += srcRows0[y][x]*w;
							s1[k_best] += srcRows1[y][x]*w;
						}
					}

					for(int k=0; k<4; k++)
					{
						if(sw[k]>0)
						{
							c0[k]=s0[k]/sw[k];
							c1[k]=s1[k]/sw[k];
							//double h = _hypot(c0[k], c1[k]);
							//c0[k] /= h; c1[k] /= h;
						}
					}
				}

				// найдем самый характерный цвет
				int k_best=0;
				for(int k=1; k<4; k++)
				{
					if(sw[k]>sw[k_best]) k_best=k;
				}

				color col0 = 32768, col1=32768, col2=32768;
				if(sw[k_best]>0)
				{
					double w = sw[k_best] / (sw[0] + sw[1] + sw[2] + sw[3]);
					w = w*w;
					//col0 = c0[k_best]*w*32767 + 32768;
					//col1 = c1[k_best]*w*32767 + 32768;
					//col2 = 65535 * w;
					
					double a = atan2(c0[k_best]-32768.0, c1[k_best]-32768.0);
					double h = _hypot(c0[k_best]-32768.0, c1[k_best]-32768.0);

					col0 = c0[k_best];
					col1 = c1[k_best];
					col2 = 32768+h;

					col0 = max(0,min(65535, h/2*(1 + cos(a))));
					col1 = max(0,min(65535, h/2*(1 + cos(a+PI*2/3))));
					col2 = max(0,min(65535, h/2*(1 + cos(a-PI*2/3))));

					//col2 = min( 65535, sw[k_best] / 400 );

					//col0 = max(0,min(65535, w*65535));
					//col1 = max(0,min(65535, w*65535));
					//col2 = max(0,min(65535, w*65535));
				}

 				for(int y=y0; y<y1; y++)
				{
 					for(int x=x0; x<x1; x++)
					{
						dstRows0[y][x] = col0;
						dstRows1[y][x] = col1;
						dstRows2[y][x] = col2;
					}
				}
			}
		}

	}
	else if(400 == nPass)
	{ // обработка квадратов - 8 направлений
		int w = 48;

		int threshold = 32768;
		double cd_threshold = w*16;

		int nx = (cx+w-1)/w;
		int ny = (cy+w-1)/w;

		int rr[] = {1, 0, -1, 0};
		int gg[] = {0, 1, 0, -1};
		int bb[] = {32768, 32768, 32768, 32768};

		for(int yy=0; yy<ny-1; yy++)
		{
			for(int xx=0; xx<nx-1; xx++)
			{
				int x0 = xx*w, y0 = yy*w;
				int x1 = min(cx, x0+w), y1 = min(cy, y0+w);

				int cnt[16] = {0};

				for(int y=y0+1; y<y1; y++)
				{
					for(int x=x0+1; x<x1; x++)
					{
						int bits=0;
						if(srcRows1[y-1][x-1]>=threshold) bits |= 1;
						if(srcRows1[y-1][x]>=threshold) bits |= 2;
						if(srcRows1[y][x-1]>=threshold) bits |= 4;
						if(srcRows1[y][x]>=threshold) bits |= 8;
						cnt[bits]++;
					}
				}

				// счетчики по 4 направлениям (каждое суммируется с противоположным - остается 4
				// чтобы были пропорциональны длине линии - умножаем на 2 или 3
				int cd[4];
				cd[0] = (cnt[3]+cnt[12])*3;
				cd[1] = (cnt[2]+cnt[11]+cnt[4]+cnt[13])*2;
				cd[2] = (cnt[10]+cnt[5])*3;
				cd[3] = (cnt[8]+cnt[14]+cnt[1]+cnt[7])*2;

				// теперь хитрый финт ушами - складываем по два соседних направления,
				// чтобы не было чувствительности к "вилянию" линий
				int tmp = cd[0];
				cd[0] = cd[0] + cd[1];
				cd[1] = cd[1] + cd[2];
				cd[2] = cd[2] + cd[3];
				cd[3] = cd[3] + tmp;

				int k_best=0;
				for(int k=1; k<4; k++)
					if(cd[k]>cd[k_best]) k_best=k;
				int k_worst = (k_best+2) % 4;
				double d = double(cd[k_best]) / max(1, cd[k_best]+cd[k_worst]);
				double d2 = max(0, min(65535, cd[k_best] / cd_threshold ));
				d = max(0, d*2-1);
				d *= d2;

				int c0 = 32768 + rr[k_best]*d*32767;
				int c1 = 32768 + gg[k_best]*d*32767;
				int c2 = 32768 + d*32767;

				double a = k_best*PI/2;

				c0 = max(0,min(65535, d*32767*(1 + cos(a))));
				c1 = max(0,min(65535, d*32767*(1 + cos(a+PI*2/3))));
				c2 = max(0,min(65535, d*32767*(1 + cos(a-PI*2/3))));
				
 				for(int y=y0; y<y1; y++)
				{
 					for(int x=x0; x<x1; x++)
					{
						dstRows0[y][x] = c0;
						dstRows1[y][x] = c1;
						dstRows2[y][x] = c2;
					}
				}
			}
		}
	}


	FinishNotification();

	return true;
}
