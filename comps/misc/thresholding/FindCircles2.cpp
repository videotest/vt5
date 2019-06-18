#include "stdafx.h"
#include "FindCircles2.h"
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
_ainfo_base::arg	CFindCircles2Info::s_pargs[] = 
{
	{"Radius",		szArgumentTypeInt, "32", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Bin",		szArgumentTypeBinaryImage, 0, true, true },
	{"Objects",szArgumentTypeObjectList, 0, false, true },
	{"ShowDebugImage", szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

bool CFindCircles2::CanDeleteArgument( CFilterArgument *pa )
{
	if( pa->m_bstrArgName == _bstr_t("Img") )
		return false;

	return __super::CanDeleteArgument(pa);
}


static double inline sqr(double x)
{ return x*x; }

static int inline round(double x)
{ return int(floor(x+0.5)); }

double CFindCircles2::CheckCircle(int x, int y, int j)
{	// проверить точку (x,y) на наличие окружности радиуса rr[j]; возвращает "качество"
	// 1<=j<nr-2

	if( x<m_nRadius || x>=m_cx-m_nRadius || y<m_nRadius || y>=m_cy-m_nRadius )
		return 0;

	double s=0;
	for(int i=0; i<m_na; i++)
	{
		int xx = x + m_dx[i*m_nr+j];
		int yy = y + m_dy[i*m_nr+j];
		int xx1 = x + m_dx[i*m_nr+j+1];
		int yy1 = y + m_dy[i*m_nr+j+1];
		int c = m_ppSrc[yy][xx];
		int c1 = m_ppSrc[yy1][xx1];
		double d = c1-c;
		if(d<0) d=0;
		s += d*d;

		xx = x + m_dx[i*m_nr+j-1];
		yy = y + m_dy[i*m_nr+j-1];
		xx1 = x + m_dx[i*m_nr+j+2];
		yy1 = y + m_dy[i*m_nr+j+2];
		c = min(c, m_ppSrc[yy][xx]);
		c1 = max(c,m_ppSrc[yy1][xx1]);
		d = c1-c;
		if(d<0) d=0;
		s += d*d/4;
	}
	return sqrt(s/(m_na*1.25));
}

double CFindCircles2::FindSpot(int x, int y, int* jj)
{
	int j_best = 0;
	double h_best = 0;

	// ------------
	// Поиск по проценту площади маски внутри круга и на окружности
	if(1){
		double ss[m_nr];
		for( int j = 0; j<m_nr; j++)
		{
			int s = 0;
			for(int i=0; i<m_na; i++)
			{
				int xx = x+m_dx[i*m_nr+j];
				if(xx<0 || xx>=m_cx) continue;
				int yy = y+m_dy[i*m_nr+j];
				if(yy<0 || yy>=m_cy) continue;
				if(m_ppBin[yy][xx]>=128)
					s++;
			}
			ss[j] = double(s)/m_na;
		}

		double r_wanted = double(m_nRadius)/2;
		double d_best = 0;
		double s0=0,s1=0;
		for( int j = 1; j<m_nr; j++)
		{
			s0 += m_rr[j]*m_rr[j];
			s1 += ss[j]*m_rr[j]*m_rr[j];
			double s_avg = s1/s0;

			//double d1 = m_rr[j]/r_wanted;
			//if(d1>1) d1 = max(0,2.0-d1);
			double d1 = m_rr[j]/r_wanted;
			d1 = 1 - sqr(d1-1);
			d1 = max(0,d1);
			d1*=d1;

			for( int k = 1; k<6; k++)
			{
				if(j+k>=m_nr) break;
				double d = s_avg-ss[j+k];
				d /= sqrt((double)k);
				d *= d1;
				if(d>d_best)
				{
					d_best = d;
					j_best = j;
				}
			}
		}

		for(int i=0; i<m_na; i++)
		{
			jj[i]=j_best;
		}

		h_best = max( 0, min(1, d_best) );
		return h_best;
	}

	// ------------
	// поиск по наибольшему градиенту средней яркости между концентрическими окружностями
	if(0){
		double ss[m_nr];
		for( int j = 0; j<m_nr; j++)
		{
			double s = 0;
			for(int i=0; i<m_na; i++)
			{
				int xx = x+m_dx[i*m_nr+j];
				int yy = y+m_dy[i*m_nr+j];
				s += m_ppSrc[yy][xx];
			}
			ss[j] = s/m_na;
		}

		double r_wanted = double(m_nRadius)/2;
		double d_best = 0;
		for( int j = 1; j<m_nr; j++)
		{
			int c_dark = ss[j];
			int c_light = c_dark;
			//double d1 = m_rr[j]/r_wanted;
			//if(d1>1) d1 = 1.0/d1;
			double d1 = m_rr[j]/r_wanted;
			d1 = 1 - sqr(d1-1);
			d1 = max(0,d1);
			d1*=d1;

			for( int k = 1; k<6; k++)
			{
				if(j+k>=m_nr || j-k<0) break;

				c_dark = min(c_dark, ss[j-k]);
				c_light = max(c_light, ss[j+k]);

				double d = c_light-c_dark;
				d /= sqrt(double(k));
				d *= d1;
				if(d>d_best)
				{
					d_best = d;
					j_best = j;
				}
			}
		}

		for(int i=0; i<m_na; i++)
		{
			jj[i]=j_best;
		}

		h_best = min(1, d_best/32768);
		return h_best;
	}

	// ------------------
	// приближенный поиск эллипса на binary по карте расстояний от края

	for( int j = 3; j<=9; j++)
	{
		double s1=0, s2=0, s_co=0, s_si=0, s_co2=0, s_si2=0;
		double dd[m_na];
		for(int i=0; i<m_na; i++)
		{
			int i2 = (i*2) % m_na;
			int xx = x+m_dx[i*m_nr+j];
			int yy = y+m_dy[i*m_nr+j];
			double d = m_ppDist[yy][xx];
			dd[i]=d;
			s1 += d;
			s2 += d*d;
			s_co += d*m_co[i];
			s_si += d*m_si[i];
			s_co2 += d*m_co[i2];
			s_si2 += d*m_si[i2];
		}
		s1 /= m_na;
		s2 /= m_na;
		s_co /= m_na/2;
		s_si /= m_na/2;
		s_co2 /= m_na/2;
		s_si2 /= m_na/2;
		s2 -= s1*s1 
			+ (s_co*s_co + s_si*s_si) / 2
			+ (s_co2*s_co2 + s_si2*s_si2) / 2;

		if(s1>1) s2 += (s1-1)*(s1-1);
		if(s1<-1) s2 += (s1+1)*(s1+1);

		double h = max(0,1-s2);

		if(h>h_best)
		{
			h_best = h;
			j_best = j + round( s1*(m_nr-1)/(m_nRadius-2.0) );
			j_best = max(1, min(m_nr-1, j_best));
		}
	}

	for(int i=0; i<m_na; i++)
	{
		jj[i]=j_best;
	}


	return h_best;

	// ------------
	// поиск замкнутого контура вокруг точки

	for(int i=0; i<m_na; i++)
		jj[i]=1;
	for(int k=0; k<1000; k++)
	{
		i = rand() % m_na;
		int i1 = (i+m_na-1)%m_na;
		int i2 = (i+1)%m_na;
		int j = jj[i];
		if ( j <= (jj[i1]+jj[i2]+3)/2 && jj[i]<m_na-1 )
		{
			int xx = x+m_dx[i*m_nr+j];
			int yy = y+m_dy[i*m_nr+j];
			int xx1 = x+m_dx[i*m_nr+j+1];
			int yy1 = y+m_dy[i*m_nr+j+1];
			double d = m_ppDist[yy][xx];
			double d1 = m_ppDist[yy1][xx1];

			byte b = m_ppBin[yy][xx];
			byte b1 = m_ppBin[yy1][xx1];

			if(fabs(d1)<fabs(d)) jj[i] = j+1;
			//else if( (b>=128 || b1>=128) && d1>-3) jj[i] = j+1;
		}
	}

	double j_min = jj[0], j_max=jj[0];
	for(int i=0; i<m_na; i++)
	{
		if(jj[i]<j_min) j_min=jj[i];
		if(jj[i]>j_max) j_max=jj[i];
	}

	if(j_max>=m_nr-1) return 0;
	if(j_min<=1) return 0;

	return double(j_min)/j_max;
}

double CFindCircles2::FindSpot2(int x, int y, int* jj_max, int* jj_wanted, int* jj, double wanted_dev, int *px, int *py)
{	// поиск округлого пятна,
	// при этом по возможности выдерживается заданный радиус и не превышается максимальный

	// поиск по яркости
	double d_sum = 0;
	double dd[m_na];
	double j_avg[m_na];
	int seed0 = 0;
	if(m_ppSeed) seed0 = m_ppSeed[y][x];
	for(int i=0; i<m_na; i++)
	{
		double d_best = 0;
		int j_best=jj_wanted[i];
		double ds=0, js=0;
		int j_max = m_nr-1;
		for( int j = 1; j<m_nr; j++)
		{
			//double d1 = double(j)/jj_wanted[i];
			//if(d1>1) d1 = 1.0/d1;
			//d1 = 1 - (1-d1)/wanted_dev;

			double d1 = double(j)/jj_wanted[i];
			d1 = 1 - sqr((d1-1)/wanted_dev);
			d1 = max(0,d1);
			d1*=d1;

			// если wanted_dev=1 - не изменяется;
			// если wanted_dev стремится к 0 - пик сужается
			//d1 = max(0, d1*2-1);

			int ind = i*m_nr+j;
			int xx = x+m_dx[ind];
			int yy = y+m_dy[ind];

			if(xx<0 || xx>=m_cx || yy<0 || yy>=m_cy) break;

			int c_dark = m_ppSrc[yy][xx];
			int c_light = c_dark;

			if(m_ppSeed && m_ppSeed[yy][xx]!=seed0)
			{ // как вышли за свою зону - стоп
				j_max = min(j_max, j-1);
				d1 /= 1+sqrt(double(j-j_max));
			}

			for( int k = 1; k<4; k++)
			{
				if(j+k>=m_nr || j-k<0) break;

				int xx1 = x+m_dx[ind+k];
				int yy1 = y+m_dy[ind+k];
				if(xx1<0 || xx1>=m_cx || yy1<0 || yy1>=m_cy) break;
				int xx2 = x+m_dx[ind-k];
				int yy2 = y+m_dy[ind-k];

				c_dark = min(c_dark, m_ppSrc[yy2][xx2]);
				c_light = max(c_light, m_ppSrc[yy1][xx1]);

				double d = c_light - c_dark;
				d /= sqrt(double(k));
				d *= d1;
				if(d>d_best)
				{
					d_best = d;
					j_best = j;
				}
				ds += d;
				js += d*j;
			}
		}
		//jj[i] = min(j_best,j_max);
		jj[i] = j_best;
		if(j_best>j_max && j_best<=j_max-2)
			jj[i] = j_max;

		d_sum += d_best;
		dd[i] = d_best;
		j_avg[i] = js/max(ds,1e-5);
		
		//jj[i] = round(j_avg[i]);
	}
	d_sum /= m_na;

	// новое вычисление центра
	double sx=0, sy=0, s=0;
	for(int i=0; i<m_na/2; i++)
	{
		int i2 = i+m_na/2;
		double d = min(dd[i], dd[i2]);
		//d /= 1 + fabs(jj[i]-j_avg[i]);
		d*=d;
		sx += ( m_dx[i*m_nr+jj[i]] + m_dx[i2*m_nr+jj[i2]] ) * d;
		sy += ( m_dy[i*m_nr+jj[i]] + m_dy[i2*m_nr+jj[i2]] ) * d;
		s += d;
	}
	s = max(s,0.01);
	x += round(sx/s);
	y += round(sy/s);

	x = max(1, min(m_cx-2, x));
	y = max(1, min(m_cy-2, y));

	if(px) *px = x;
	if(py) *py = y;

	if(0)
	{
		m_ppDst[y][x] = 65535;
		m_ppDst[y+1][x] = 65535;
		m_ppDst[y-1][x] = 65535;
		m_ppDst[y][x+1] = 65535;
		m_ppDst[y][x-1] = 65535;
	}

	// обработка выбросов:
	if(0) for(int nPass=0; nPass<2; nPass++)
	{
		double dj[m_na];
		double ss=0;
		double dj_max=0;
		for(int i=0; i<m_na; i++)
		{
			double s=0;
			for(int di=-2; di<=2; di++)
			{
				int i1 = (i+di+m_na) % m_na;
				s += jj[i1];
			}
			dj[i] = jj[i] - s/5;
			ss += fabs(dj[i]);
			dj_max = max(dj_max, fabs(dj[i]));
		}
		ss /= m_na;

		for(int i=0; i<m_na; i++)
		{
			jj[i] -= round( dj[i]*fabs(dj[i])/dj_max );
		}
	}

	return d_sum/65535;

	// поиск по краю маски
	for(int i=0; i<m_na; i++)
	{
		int j_best = jj_wanted[i];
		double d_best=1e20;
		for(int j=0; i<m_nr-1; j++)
		{
			if(j>jj_max[i]) break;

			int xx = x+m_dx[i*m_nr+j];
			int yy = y+m_dy[i*m_nr+j];
			int xx1 = x+m_dx[i*m_nr+j+1];
			int yy1 = y+m_dy[i*m_nr+j+1];
			if(xx1<0 || xx1>=m_cx || yy1<0 || yy1>=m_cy) break;

			double d = fabs(m_ppDist[yy][xx]+1)*(1+abs(j-jj_wanted[i]));
			double d1 = fabs(m_ppDist[yy1][xx1]+1)*(1+abs(j+1-jj_wanted[i]));
			byte b = m_ppBin[yy][xx];
			byte b1 = m_ppBin[yy1][xx1];

			if(d<d_best && m_ppDist[yy][xx]<m_ppDist[yy1][xx1])
			{
				j_best = j;
				d_best = d;
			}
		}
		jj[i]=j_best;
	}

	return 1;
}

static void FillPoly(byte **dst, int cx, int cy, POINT *pPoints, int n, byte color)
{
	smart_alloc(x0, int, cy);
	for(int i=0; i<cy; i++) x0[i]=-1;

	int lastDir=0;
	int x1=pPoints[n-1].x, y1=pPoints[n-1].y;
	for(i=0; i<n; i++)
	{
		int xx1=x1*65536+32768, dxx;
		if(y1!=pPoints[i].y) dxx=(pPoints[i].x-x1)*65536/abs(pPoints[i].y-y1);
		while(y1!=pPoints[i].y)
		{
			x1=xx1/65536;
			int dy= pPoints[i].y>y1 ? 1 : -1;
			if(y1>=0 && y1<cy)
			{
				if(x0[y1]!=-1)
				{
					int xx1=min(x1,x0[y1]);
					int xx2=max(x1,x0[y1]);
					for(int fillx=xx1; fillx<=xx2; fillx++)
					{
						if(fillx>=0 && fillx<cx)
						{
							dst[y1][fillx] ^= color;
						}
					}
					x0[y1]=-1;
				}
				else x0[y1]=x1;
				if(dy!=lastDir && lastDir!=0)
				{
					if(x0[y1]==-1) x0[y1]=x1;
					else x0[y1]=-1;
				}
			}
			lastDir=dy;
			y1+=dy;
			xx1+=dxx;
		}
		x1=pPoints[i].x;
	}
}

double CFindCircles2::SmoothContour(int x, int y, int* jj, int nArcLength, int nHandleLength)
{
	// принцип работы: берем точки i1 и i2; в промежутке от i1 до i2
	// заменяем jj на параболу, проходящую через jj[i1] и jj[i2]
	// выпуклость - по методу наименьших квадратов по участкам за пределами i1..i2
	// Возвращает d - что-то типа среднего расстояния между исходным контуром и дугой,
	// но масштабированное с учетом градиентов на дуге и на исходном контуре
	// (если дуги не видно - d уменьшается)
	double s_worst = -1;
	int i1_worst = 0;
	double alpha_worst=0;
	double ratio_worst = 0;

	for(int i1=0; i1<m_na; i1++)
	{
		double s0=0, s1=0;
		int i2 = (i1+nArcLength) % m_na;
		double dj_di = double(jj[i2]-jj[i1])/nArcLength;
		for(int k=1; k<nHandleLength; k++)
		{
			int x1 = (i1+m_na-k)%m_na;
			int x2 = (i2+k)%m_na;
			double w = k*(k+nArcLength);
			double y1 = jj[i1] - k*dj_di;
			double y2 = jj[i2] + k*dj_di;
			s0 += w*w*2;
			s1 += w*(jj[x1]-y1 + jj[x2]-y2);
		}
		double alpha = s1/s0;

		double s2 = 0;
		double sd_exact=0;
		double sd_round=0;
		for(int k=0; k<=nArcLength; k++)
		{
			int i = (i1+k) % m_na;
			double j_predicted = jj[i1] + k*dj_di + alpha*k*(nArcLength-k);
			s2 += sqr(jj[i]-j_predicted);
			int j3 = round(j_predicted);
			double d_max_exact=0;
			double d_max_round=0;
			for(int dj=1; dj<3; dj++)
			{
				{
					int j3p = min(j3+dj, m_nr-1);
					int j3m = max(j3-dj, 1);
					int xp = x + m_dx[i*m_nr+j3p];
					int yp = y + m_dy[i*m_nr+j3p];
					int xm = x + m_dx[i*m_nr+j3m];
					int ym = y + m_dy[i*m_nr+j3m];
					xp = max(0,min(m_cx-1,xp));
					yp = max(0,min(m_cy-1,yp));
					xm = max(0,min(m_cx-1,xm));
					ym = max(0,min(m_cy-1,ym));
					int cp = m_ppSrc[yp][xp];
					int cm = m_ppSrc[ym][xm];
					double d = cp-cm;
					if(d<0) d=0;
					d /= sqrt((double)dj);
					d_max_round = max(d_max_round,d);
				}
				{
					int j3p = min(jj[i]+dj, m_nr-1);
					int j3m = max(jj[i]-dj, 1);
					int xp = x + m_dx[i*m_nr+j3p];
					int yp = y + m_dy[i*m_nr+j3p];
					int xm = x + m_dx[i*m_nr+j3m];
					int ym = y + m_dy[i*m_nr+j3m];
					xp = max(0,min(m_cx-1,xp));
					yp = max(0,min(m_cy-1,yp));
					xm = max(0,min(m_cx-1,xm));
					ym = max(0,min(m_cy-1,ym));
					int cp = m_ppSrc[yp][xp];
					int cm = m_ppSrc[ym][xm];
					double d = cp-cm;
					if(d<0) d=0;
					d /= sqrt((double)dj);
					d_max_exact = max(d_max_exact,d);
				}
			}
			sd_exact += d_max_exact;
			sd_round += d_max_round;
		}
		double ratio = sd_round/max(1,sd_exact);
		s2 = s2*ratio/nArcLength;
		if(s2>s_worst)
		{
			s_worst = s2;
			i1_worst = i1;
			alpha_worst = alpha;
			ratio_worst = ratio;
		}
	}

	if(ratio_worst<0.7)
		return s_worst;

	i1 = i1_worst;
	int i2 = (i1+nArcLength) % m_na;
	double dj_di = double(jj[i2]-jj[i1])/nArcLength;
	double alpha = alpha_worst;
	for(int k=0; k<=nArcLength; k++)
	{
		int i = (i1+k) % m_na;
		double j_predicted = jj[i1] + k*dj_di + alpha*k*(nArcLength-k);
		jj[i] = round(j_predicted);
	}

	return s_worst;
}

INamedDataObject2Ptr CFindCircles2::CreateObject(int x, int y, int* jj)
{	// выделить объект, исходя из его контура

	//-------------------- переведем контур в пары (x,y)
	smart_alloc(points, POINT, m_na);
	for(int i=0; i<m_na; i++)
	{
		points[i].x = x + m_dx[i*m_nr+jj[i]];
		points[i].y = y + m_dy[i*m_nr+jj[i]];
	}

	//-------------------- эксперимент: сильное сглаживание
	if(0){ // для x
		double s1=0, s_co=0, s_si=0, s_co2=0, s_si2=0;
		for(int i=0; i<m_na; i++)
		{
			int i2 = (i*2) % m_na;
			double d = points[i].x;
			s1 += d;
			s_co += d*m_co[i];
			s_si += d*m_si[i];
			s_co2 += d*m_co[i2];
			s_si2 += d*m_si[i2];
		}
		s1 /= m_na;
		s_co /= m_na/2;
		s_si /= m_na/2;
		s_co2 /= m_na/2;
		s_si2 /= m_na/2;

		for(int i=0; i<m_na; i++)
		{
			int i2 = (i*2) % m_na;
			points[i].x = round( s1 + s_co*m_co[i] + s_si*m_si[i] );
			//+
			//	s_co2*m_co[i2] + s_si2*m_si[i2] );
		}
	}
	if(0){ // для y
		double s1=0, s_co=0, s_si=0, s_co2=0, s_si2=0;
		for(int i=0; i<m_na; i++)
		{
			int i2 = (i*2) % m_na;
			double d = points[i].y;
			s1 += d;
			s_co += d*m_co[i];
			s_si += d*m_si[i];
			s_co2 += d*m_co[i2];
			s_si2 += d*m_si[i2];
		}
		s1 /= m_na;
		s_co /= m_na/2;
		s_si /= m_na/2;
		s_co2 /= m_na/2;
		s_si2 /= m_na/2;

		for(int i=0; i<m_na; i++)
		{
			int i2 = (i*2) % m_na;
			points[i].y = round( s1 + s_co*m_co[i] + s_si*m_si[i] +
				s_co2*m_co[i2] + s_si2*m_si[i2] );
		}
	}

	//-------------- эксперимент: замена наиболее подозрительных участков на интерполированные
	{
		for(int i=0; i<m_na; i++)
		{
		}
	}

	//-------------- эксперимент - двигаем точки к границе binary
	if(m_bCorrectByBin){
		for(int i=0; i<m_na; i++)
		{
			int x0 = points[i].x;
			int y0 = points[i].y;
			double dist0 = 1e20;
			for(int dy=-3; dy<=3; dy++)
			{
				y = points[i].y + dy;
				if(y<0 || y>=m_cy) continue;
				for(int dx=-3; dx<=3; dx++)
				{
					x = points[i].x + dx;
					if(x<0 || x>=m_cx) continue;
					double dist = m_ppDist[y][x];
					dist = fabs(dist-0.4);
					dist *= 1 + _hypot(dx,dy);
					double d2 = m_si[i]*dx - m_co[i]*dy;
					dist += sqr(d2); // чтобы старалось не двигать по окружности
					if(dist<dist0)
					{
						dist0 = dist;
						x0 = x;
						y0 = y;
					}
				}
			}
			points[i].x = x0;
			points[i].y = y0;
		}
	}

	//--------------

	//-------------------- rect
	_rect rc(x+1000,y+1000, x-1000,y-1000);
	for(int i=0; i<m_na; i++)
	{
		rc.left = min(rc.left, points[i].x);
		rc.top = min(rc.top, points[i].y);
		rc.right = max(rc.right, points[i].x);
		rc.bottom = max(rc.bottom, points[i].y);
	}
	rc.right++; rc.bottom++;
	
	rc.left = max(0,rc.left);
	rc.top = max(0,rc.top);
	rc.right = min(m_cx,rc.right);
	rc.bottom = min(m_cy,rc.bottom);
	if(rc.width()<=0 || rc.height()<=0) return 0;

	//-------------------- image
	IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
	IImage3Ptr      sptrI(ptrI);
	if(sptrI==0) return 0;
	INamedDataObject2Ptr sptrImg(ptrI);
	if(sptrImg==0) return 0;
	sptrI->CreateVirtual( rc );
	sptrI->InitRowMasks();
	//sptrI->InitContours();
	sptrImg->SetParent( m_sptrSrc,  0);

	//-------------------- object
	IUnknownPtr ptrO( ::CreateTypedObject(_bstr_t(szTypeObject)), false );
	IMeasureObjectPtr sptrO(ptrO);
	if(sptrO==0) return 0;

	sptrO->SetImage( ptrI );
	INamedDataObject2Ptr ptrObject(ptrO);
	ptrObject->SetParent(m_sptrList, 0);

	// приведем точки к внутренности прямоугольника
	int cx1 = rc.width(), cy1 = rc.height();
	for(int i=0; i<m_na; i++)
	{
		points[i].x -= rc.left;
		points[i].x = max(0,min(cx1-1,points[i].x));
		points[i].y -= rc.top;
		points[i].y = max(0,min(cy1-1,points[i].y));
	}
	smart_alloc(ppm, byte*, cy1);
	{
		for(int y1=0; y1<cy1; y1++)
		{
			sptrI->GetRowMask(y1,ppm+y1);
			ZeroMemory(ppm[y1],cx1);
		}
	}
	FillPoly(ppm, cx1, cy1, points, m_na, 255);
	//if(x>=rc.left && x<rc.right && y>=rc.top && y<rc.bottom)
	//	ppm[y-rc.top][x-rc.left] = 0; // пометить центр
	sptrI->InitContours();

	return sptrO;
}

// ---------------
class CSeed // зародыши (изначально - по одному в клетке)
{
public:
	int x,y; // координаты
	double h;
	double r;
	double c; // характерный цвет
	double s; // некая сумма для вычисления характерного цвета
};

// ------------- Point Processor для многофазного изображения -------------
class CFindCirclesPointProcessor
{
public:
	int m_seed_count; // количество зародышей
	CSeed* m_seeds; // координаты зародышей

	CFindCirclesPointProcessor()
	{
		m_ptrImage = 0;
		m_pRows = 0;
		m_pBinRows = 0;
		m_pSeedRows=0;
		m_seed_count = 0;
		m_seeds=0;
	};
	~CFindCirclesPointProcessor()
	{
		delete[] m_pRows;
		delete[] m_pBinRows;
		delete[] m_pSeedRows;
		delete[] m_pSeedBuf;
	};
	void SetupImages(IImage4* pImage, IBinaryImage* pBin)
	{
		m_ptrImage = pImage;
		m_ptrBin = pBin;
		pImage->GetSize(&m_cx,&m_cy);
		pImage->GetPanesCount(&m_nPanes);
		m_pRows = new color* [ m_nPanes * m_cy ];
		m_pBinRows = new byte* [ m_cy ];
		m_pSeedRows = new int* [m_cy]; 
		m_pSeedBuf = new int[m_cx*m_cy];
		for(int pane=0; pane<m_nPanes; pane++)
		{
			for(int y=0; y<m_cy; y++)
				pImage->GetRow(y, 0, & (m_pRows[pane*m_cy+y]) );
		}
		for(int y=0; y<m_cy; y++)
			pBin->GetRow( &(m_pBinRows[y]), y, false );

		for(int y=0; y<m_cy; y++)
			m_pSeedRows[y] = m_pSeedBuf + y*m_cx;
	};

public:
	inline double GetV(int x, int y, int x_from, int y_from)
	{
		int seed = m_pSeedRows[y_from][x_from];
		m_pSeedRows[y][x] = seed;
		double v = _hypot(x-m_seeds[seed].x,y-m_seeds[seed].y);
		v /= max(1,m_seeds[seed].r);
		v /= max(1,m_seeds[seed].r);

		double c = m_pRows[y][x];
		c -= m_pRows[y_from][x_from];
		//c -= m_seeds[seed].c;

		//v *= exp(c/4096);
		return v;
	};
	inline void OnAddPoint(int x, int y, double dist, double v)
	{
		int seed = m_pSeedRows[y][x];
		double c = m_pRows[y][x];
		double c0 = m_seeds[seed].c;
		double s = m_seeds[seed].s;
		if(c<c0)
		{
			m_seeds[seed].c = (c0*s + c*20)/(s+20);
			m_seeds[seed].s = s+20;
		}
		else
		{
			m_seeds[seed].c = (c0*s + c)/(s+1);
			m_seeds[seed].s = s+1;
		}
		//int phase = m_pBinRows[y][x];
	};
private:
	int m_cx,m_cy,m_nPanes;
	color **m_pRows;
	byte **m_pBinRows;
	IImage4Ptr m_ptrImage; // на всякий случай
	IBinaryImagePtr m_ptrBin; // на всякий случай
public:
	int **m_pSeedRows;
	int* m_pSeedBuf; // массив номеров ближайших зародышей
};

void CFindCircles2::CalcDistFromBinBound()
{	// Заполнение таблицы расстояний от края Binary
	for(int y=0; y<m_cy; y++)
	{
		for(int x=0; x<m_cx; x++)
			m_ppDist[y][x] = 1e20;
	}

	for(int y=1; y<m_cy-1; y++)
	{
		for(int x=1; x<m_cx-1; x++)
		{
			if(m_ppBin[y][x]!=m_ppBin[y-1][x-1]) m_ppDist[y][x]=0.7;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y-1][x-1]+1.4);
			if(m_ppBin[y][x]!=m_ppBin[y-1][x+1]) m_ppDist[y][x]=0.7;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y-1][x+1]+1.4);

			if(m_ppBin[y][x]!=m_ppBin[y-1][x]) m_ppDist[y][x]=0.5;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y-1][x]+1);
			if(m_ppBin[y][x]!=m_ppBin[y][x-1]) m_ppDist[y][x]=0.5;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y][x-1]+1);
		}
	}

	for(int y=m_cy-2; y>=1; y--)
	{
		for(int x=m_cx-2; x>=1; x--)
		{
			if(m_ppBin[y][x]!=m_ppBin[y+1][x-1]) m_ppDist[y][x]=0.7;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y+1][x-1]+1.4);
			if(m_ppBin[y][x]!=m_ppBin[y+1][x+1]) m_ppDist[y][x]=0.7;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y+1][x+1]+1.4);

			if(m_ppBin[y][x]!=m_ppBin[y+1][x]) m_ppDist[y][x]=0.5;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y+1][x]+1);
			if(m_ppBin[y][x]!=m_ppBin[y][x+1]) m_ppDist[y][x]=0.5;
			else m_ppDist[y][x]=min(m_ppDist[y][x],m_ppDist[y][x+1]+1);
		}
	}

	for(int y=1; y<m_cy-1; y++)
	{
		for(int x=1; x<m_cx-1; x++)
		{ // внутри объектов расстояние отрицательное
			if(m_ppBin[y][x] & 128) m_ppDist[y][x] = - m_ppDist[y][x];
		}
	}
}

bool CFindCircles2::InvokeFilter()
{
	IImage4Ptr	ptrSrc( GetDataArgument("Img") );
	IBinaryImagePtr	ptrBin( GetDataArgument("Bin") );
	//IImage4Ptr	ptrDst( GetDataResult("FindCircles2") );
	m_sptrList = GetDataResult("Objects");
	long nRadius = GetArgLong("Radius");
	m_nRadius = nRadius;
	bool bShowDebugImage = 0!=GetArgLong("ShowDebugImage");

	double fFindSpotThreshold = ::GetValueDouble( GetAppUnknown( ), "\\FindCircles2", "FindSpotThreshold", 0.1 ); 
	double fFindSpot2Threshold = ::GetValueDouble( GetAppUnknown( ), "\\FindCircles2", "FindSpot2Threshold", 0.06 ); 
	m_bSmoothContour = 0 != ::GetValueInt( GetAppUnknown( ), "\\FindCircles2", "SmoothContour", 1 ); 
	m_bCorrectByBin = 0 != ::GetValueInt( GetAppUnknown( ), "\\FindCircles2", "CorrectByBin", 0 ); 

	IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
	IImage4Ptr      ptrDst(ptrI);

	if( ptrSrc == NULL || ptrDst==NULL || ptrBin==NULL || m_sptrList==NULL )
		return false;

	m_sptrSrc = ptrSrc;

	int cx,cy;
	ptrSrc->GetSize(&cx,&cy);
	int nPanes = ::GetImagePaneCount( ptrSrc );

	{	
		ICompatibleObjectPtr ptrCO( ptrDst );
		if( ptrCO == NULL ) return false;
		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
			return false;
	}

	if(bShowDebugImage)
	{ // debug - вставка в документ вспомогательной картинки
		if (m_ptrTarget==0) return S_FALSE;
		IDocumentPtr doc(m_ptrTarget);
		if(doc==0) return S_FALSE;
		SetToDocData(doc, ptrDst);
	}

	try
	{
		// StartNotification(cy,nPanes);
		StartNotification(100000,1/*nPanes*/,10);
		{
			smart_alloc(srcRows, color *, cy);
			smart_alloc(dstRows, color *, cy);
			smart_alloc(binRows, byte *, cy);

			smart_alloc(dist_buf, float, cx*cy);
			smart_alloc(ppDist, float*, cy);

			// инициализация
			m_ppSrc = srcRows;
			m_ppDst = dstRows;
			m_ppBin = binRows;
			m_ppDist = ppDist;
			m_ppSeed = 0;
			m_cx = cx;
			m_cy = cy;
			int nPane=1;

			for(int y=0; y<cy; y++)
			{
				ptrSrc->GetRow(y, nPane, &srcRows[y]);
				ptrDst->GetRow(y, nPane, &dstRows[y]);
				ptrBin->GetRow(&binRows[y],y,FALSE);
				ZeroMemory(dstRows[y],cx*sizeof(color));
				ppDist[y] = dist_buf + y*cx;
				for(int x=0; x<cx; x++)
					ppDist[y][x] = 1e20;
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
				double r = j2r(j);
				m_rr[j]=r;
				for(int i=0; i<na; i++)
				{
					m_dx[i*nr+j] = round( r*m_co[i] );
					m_dy[i*nr+j] = round( r*m_si[i] );
				}
			}

			// Заполнение таблицы расстояний от края
			CalcDistFromBinBound();
			// (заполнили m_ppDist)

			// Собственно поиск

			int step=16;
			int nx = (cx+step-1)/step;
			int ny = (cy+step-1)/step;
			int step2 = 4;

			smart_alloc(seeds, CSeed, nx*ny);
			for(int i=0; i<nx*ny; i++) seeds[i].h=0;

			for(int start_y=0; start_y<cy; start_y++) for(int start_x=0; start_x<cx; start_x++)
			{
				int x = start_x, y = start_y;

				// работаем только из особых точек
				double d = ppDist[y][x];
				bool bGood = false;
				if(fabs(d)>3 && d<nRadius)
				{
					if(	d<=ppDist[y+1][x] && d<=ppDist[y-1][x] &&
						d<=ppDist[y][x+1] && d<=ppDist[y][x-1] )
					{
						bGood = true;
					}
					if(	d>=ppDist[y+1][x] && d>=ppDist[y-1][x] &&
						d>=ppDist[y][x+1] && d>=ppDist[y][x-1] )
					{
						bGood = true;
					}
				}
				if( y%step2==0 && x%step2==0) bGood = true; // на всякий случай добавим еще точек
				if(!bGood) continue;

				//dstRows[y][x] = 65535;

				for(int nPass=0; nPass<1; nPass++)
				{
					// проба - ищем пятно от точки
					int jj[na];
					double h = FindSpot(x,y, jj);
					if( (binRows[y][x] & 128) == 0) h /= 2; // для центров вне маски - качество в 2 раза ниже

					//dstRows[y][x] = max(0,min(65535,h*32768));
					int ix = x/step, iy=y/step;
					CSeed* pSeed = seeds+iy*nx+ix;
					if(h>pSeed->h)
					{
						pSeed->h = h;
						pSeed->x = x;
						pSeed->y = y;
						pSeed->r = m_rr[jj[0]]; // временно
						if(jj[0]<=2) pSeed->h = 0;
					}
				}
			}

			// уточнение координат центров
			for(int nPass=0; nPass<2; nPass++)
			{
				for(int iSeed=0; iSeed<nx*ny; iSeed++)
				{
					CSeed* pSeed = seeds+iSeed;
					double h0 = pSeed->h;
					if( h0<fFindSpotThreshold ) continue; // незачем
					h0 = max(h0,0.001);

					double h_max = h0;

					double s=0, xs=0, ys=0, rs=0;
					for (int dy=-6; dy<=6; dy++)
					{
						for (int dx=-6; dx<=6; dx++)
						{
							if(dx*dx+dy*dy>36) continue;

							int y = pSeed->y + dy;
							int x = pSeed->x + dx;
							if( x<0 || x>=cx || y<0 || y>=cy )
								continue;
							int jj[na];
							double h = FindSpot(x,y, jj);
							if( (binRows[y][x] & 128) == 0) h /= 2; // для центров вне маски - качество в 2 раза ниже

							double w = max(0, h/h0-0.5);
							xs += x*w;
							ys += y*w;
							rs += m_rr[jj[0]]*w;
							h_max = max(h_max,h);
							s += w;
							// надо бы еще коррекцию для радиуса придумать
						}
					}
					if(s>1)
					{
						pSeed->y = round(ys/s);
						pSeed->x = round(xs/s);
						pSeed->r = rs/s;
						pSeed->h = h_max;
					}
				}
			}

			// Первая попытка убить лишних - без учета FMM; можно провести уточнение радиусов
			if(1)
			{
				for(int iSeed=0; iSeed<nx*ny; iSeed++)
				{
					CSeed *pSeed = seeds+iSeed;
					double h = pSeed->h;
					if( h<=fFindSpotThreshold ) continue; // незачем

					int jj[na], jj_max[na], jj_wanted[na];
					for(int i=0; i<na; i++)
					{
						jj_wanted[i] = r2j(pSeed->r);
						jj_wanted[i] = max(0, min(nr-1,jj_wanted[i]));
						jj_max[i] = r2j(pSeed->r*2);
						jj_max[i] = max(0, min(nr-1,jj_max[i]));
					}

					int x = pSeed->x, y = pSeed->y; 

					//if( x<nRadius || x>=cx-nRadius || y<nRadius || y>=cy-nRadius )
					//	continue;

					h = FindSpot2(x,y, jj_max, jj_wanted, jj, 1, &x, &y );
					if( (binRows[y][x] & 128) == 0) h /= 2; // для центров вне маски - качество в 2 раза ниже


					if(h<fFindSpot2Threshold)
					{	// если тут фигня - убить
						pSeed->h = 0;
					}

					double s=0, s1=0;
					double r1 = m_rr[jj[na-1]];

					for(int i=0; i<na; i++)
					{
						int r = m_rr[jj[i]];
						s += r;
						s1 += fabs(r-r1);
					}
					double h_form = s1/max(s,1);
					if(h_form>0.3)
					{	// если тут фигня - убить
						pSeed->h = 0;
					}
				}
			}


			// прореживание сетки
			if(1) for(int iy=0; iy<ny; iy++)
			{
				for(int ix=0; ix<nx; ix++)
				{
					CSeed* pSeed = seeds+iy*nx+ix;
					double h = pSeed->h;

					if(h>0)
					{
						for(int ky=-2; ky<=2; ky++)
						{
							for(int kx=-2; kx<=2; kx++)
							{
								if(kx==0 && ky==0) continue;
								if(ix+kx<0 || ix+kx>=nx || iy+ky<0 || iy+ky>=ny)
									continue;

								CSeed *pSeed1 = pSeed + ky*nx+kx;
								if( h<=pSeed1->h )
								{
									double step1 = min(pSeed1->r,m_nRadius/2.0) / 2 +
										min(pSeed->r,m_nRadius/2.0) / 2;
									if( _hypot( pSeed1->x-pSeed->x, pSeed1->y-pSeed->y ) < step1 )
										pSeed->h = 0;
								}
							}
						}
					}
				}
			}

			/////////////////////////////////////////////////
			// Попробуем использовать Fast Marching Method //
			_fast_march_t<CFindCirclesPointProcessor> marcher1;
			for(int i=0; i<nx*ny;i++)
			{
				seeds[i].c = 65535;
				seeds[i].s = 1;
			}
			marcher1.m_seeds = seeds;
			marcher1.m_seed_count = nx*ny;
			marcher1.SetupImages(ptrSrc, ptrBin);
			marcher1.Init(cx,cy);

			// пробежимся по всем стартовым точкам
			for(int iy=0; iy<ny; iy++)
			{
				for(int ix=0; ix<nx; ix++)
				{
					int iSeed = iy*nx+ix;
					CSeed* pSeed = seeds+iSeed;
					double h = pSeed->h;

					if(h>fFindSpotThreshold)
					{
						marcher1.m_pSeedRows[pSeed->y][pSeed->x] = iSeed;
						marcher1.AddPoint( pSeed->x, pSeed->y );
						marcher1.OnAddPoint(  pSeed->x, pSeed->y, 0, 1);

						// нарисуем точку (debug)
						int x = pSeed->x;
						int y = pSeed->y;
						dstRows[y][x] = max(dstRows[y][x], 65535*h);
						dstRows[y+1][x] = max(dstRows[y+1][x], 65535*h);
						dstRows[y-1][x] = max(dstRows[y-1][x], 65535*h);
						dstRows[y][x+1] = max(dstRows[y][x+1], 65535*h);
						dstRows[y][x-1] = max(dstRows[y][x-1], 65535*h);
					}
				}
			}

			marcher1.Process();
			m_ppSeed = marcher1.m_pSeedRows;

			if(1)
			{ // нарисовать границы между зонами
				for(int y=1; y<cy-1; y++)
				{
					for(int x=1; x<cx-1; x++)
					{
						int n = marcher1.m_pSeedRows[y][x];
						if( n != marcher1.m_pSeedRows[y][x+1] ) m_ppDst[y][x] = max(m_ppDst[y][x], 32768);
						if( n != marcher1.m_pSeedRows[y][x-1] ) m_ppDst[y][x] = max(m_ppDst[y][x], 32768);
						if( n != marcher1.m_pSeedRows[y+1][x] ) m_ppDst[y][x] = max(m_ppDst[y][x], 32768);
						if( n != marcher1.m_pSeedRows[y-1][x] ) m_ppDst[y][x] = max(m_ppDst[y][x], 32768);
						//if( marcher1.m_pDist[y*cx+x]>0.5/*r_seed[n]/2*/ ) m_ppDst[y][x] = max(32768,m_ppDst[y][x]);
					}
				}
			}
			/////////////////////////////////////////////////

			for(int iSeed=0; iSeed<nx*ny; iSeed++)
			{
				CSeed* pSeed = seeds+iSeed;
				double h = pSeed->h;
				if( h<=fFindSpotThreshold ) continue; // незачем

				int jj[na], jj_max[na], jj_wanted[na];
				for(int i=0; i<na; i++)
				{
					jj_wanted[i] = r2j(pSeed->r);
					jj_wanted[i] = max(0, min(nr-1,jj_wanted[i]));
					jj_max[i] = r2j(pSeed->r*2);
					jj_max[i] = max(0, min(nr-1,jj_max[i]));
				}

				int x = pSeed->x, y = pSeed->y; 
				
				//if( x<nRadius || x>=cx-nRadius || y<nRadius || y>=cy-nRadius )
				//	continue;

				{ // эксперимент
					double d=0;
					d = SmoothContour(x,y, jj_wanted, 6, 3);
					d = SmoothContour(x,y, jj_wanted, 4, 3);
					d = SmoothContour(x,y, jj_wanted, 3, 3);
					d += 0;
				}

				h = FindSpot2(x,y, jj_max, jj_wanted, jj, 1.0, 0, 0 );
				if( (binRows[y][x] & 128) == 0) h /= 2; // для центров вне маски - качество в 2 раза ниже

				dstRows[y][x] = max(dstRows[y][x], 65535*h);
				dstRows[y+1][x] = max(dstRows[y+1][x], 65535*h);
				dstRows[y-1][x] = max(dstRows[y-1][x], 65535*h);
				dstRows[y][x+1] = max(dstRows[y][x+1], 65535*h);
				dstRows[y][x-1] = max(dstRows[y][x-1], 65535*h);

				if(h<fFindSpot2Threshold)
					continue; // если тут фигня - аминь
				// надо: поправить маски в Marcher и пересчитать соседние

				if(m_bSmoothContour)
				{ // эксперимент
					double d=0;
					d = SmoothContour(x,y, jj, 4, 3);
					d = SmoothContour(x,y, jj, 3, 3);
					d += 0;
				}

				CreateObject(x, y, jj);
			}
		}

		GUID guid={0};
		INamedDataObject2Ptr sptrNDO1(ptrSrc);
		if(sptrNDO1 != 0)
		{
			if(S_OK==sptrNDO1->GetBaseKey(&guid))
			{
				INamedDataObject2Ptr sptrNDO(m_sptrList);
				if(sptrNDO != 0) sptrNDO->SetBaseKey(&guid);
			}
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

