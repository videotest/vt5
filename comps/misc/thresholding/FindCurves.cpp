#include "stdafx.h"
#include "FindCurves.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "docview5.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "\vt5\awin\misc_ptr.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CFindCurvesInfo::s_pargs[] = 
{
	//{"MaxSplit", szArgumentTypeInt, "4", true, false },
	//{"Tolerance", szArgumentTypeDouble, "1.0", true, false },
	{"r_min", szArgumentTypeDouble, "10.0", true, false},
	{"r_max", szArgumentTypeDouble, "40.0", true, false},
	{"threshold", szArgumentTypeDouble, "0.5", true, false},
	{"Bin",	szArgumentTypeBinaryImage, 0, true, true },
	{"Curves",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

static inline int iround(double x)
{
	return int(floor(x+0.5));
}

void CFindCurves::FindCurve()
{
	//double fx=492, fy=375;
	double fx=481, fy=391;
	double dx=0, dy=1;
	double fx1=fx-dx, fy1=fy-dy;
	double omega = 0;
	int count_bad = 0;
	int count_good = 0;
	for(int i=0; i<100; i++)
	{
		int x0 = iround(fx);
		int y0 = iround(fy);
		double sx=0, sy=0, s=0;
		for(int yy=-1; yy<=1; yy++)
		{
			for(int xx=-1; xx<=1; xx++)
			{
				if(src[y0+yy][x0+xx])
				{
					double w = 3 - xx*xx - yy*yy + xx*dx + yy*dy;
					w = max(0,w);
					int x=x0+xx, y=y0+yy;
					s += w;
					sx += w*x; sy+=w*y;
				}
			}
		}
		if(s>0)
		{
			sx /= s;
			sy /= s;

			double omega1 = (sy-fy)*dx - (sx-fx)*dy;
			omega = omega*0.9 + omega1*0.1*s/6;
			omega = max(-0.1, min(0.1, omega));

			fx1=fx; fy1=fy;
			fx = sx; fy = sy;
		}
		if(src[y0][x0])
		{
			count_bad=0;
			count_good++;
			dst[y0][x0] = 255;
		}
		else
		{
			count_bad++;
			dst[y0][x0] = 128;
		}
		{ // следующий шаг
			fx += dx;
			fy += dy;
			double tmp = omega*dx;
			dx -= omega*dy;
			dy += tmp;
			double h = _hypot(dx, dy);
			dx /= h;
			dy /= h;
		}
	}
}

class CRadiusDetector
{
public:
	CRadiusDetector(byte** ppData, int cx, int cy, double r_detector);
	//~CRadiusDetector(byte** ppData, int cx, int cy);
	double GetVal(int x, int y); // получить значение счетчика в точке
	double GetVal(double r); // получить значение счетчика, соответствующее радиусу
	byte ** m_ppData;
	int m_cx, m_cy;
	double m_r_detector;
	int m_na;
	_ptr_t2<double> m_dx;
	_ptr_t2<double> m_dy;
	_ptr_t2<double> m_rr;

};

CRadiusDetector::CRadiusDetector(byte** ppData, int cx, int cy, double r_detector)
{
	m_ppData = ppData;
	m_cx = cx;
	m_cy = cy;
    m_r_detector = r_detector;

	int r=int(ceil(r_detector));
	m_na = iround(r_detector*8);

	m_dx.alloc(m_na*2);
	m_dy.alloc(m_na*2);
	m_rr.alloc(m_na*2);

	for(int i=0; i<m_na*2; i++)
	{
		m_dx[i] = iround(r_detector*cos(i*2*PI/m_na));
		m_dy[i] = iround(r_detector*sin(i*2*PI/m_na));
		m_rr[i] = r_detector / max(0.0001, cos(PI*i/m_na)) / 2;
	}

}

double CRadiusDetector::GetVal(int x, int y)
{ // получить значение счетчика в точке
	int cnt=0, cnt_max=0;
	int i_best = 0;

	for(int i=0; i<m_na*2; i++)
	{
		int y2 = y + iround(m_dy[i]);
		int x2 = x + iround(m_dx[i]);
		if(x2<0 || x2>=m_cx || y2<0 || y2>=m_cy)
			continue; // при выходе за пределы не вполне корректно, но обработаем
		if(m_ppData[y2][x2])
			cnt++;
		else
			cnt=0;
		if(cnt>cnt_max)
		{
			cnt_max = cnt;
			i_best = i;
		};
	}
	return min(m_na, cnt_max);
}

double CRadiusDetector::GetVal(double r)
{ // получить значение счетчика, соответствующее радиусу
	return acos( m_r_detector / (2*r) ) * m_na / PI;
}

bool CFindCurves::InvokeFilter()
{
	IBinaryImagePtr	ptrSrc( GetDataArgument() );
	IBinaryImagePtr	ptrDst( GetDataResult() );

	if( ptrSrc == NULL || ptrDst==NULL )
		return false;

	double r_min = GetArgDouble("r_min");
	double r_max = GetArgDouble("r_max");
	double threshold = GetArgDouble("threshold");

	int cx, cy;
	ptrSrc->GetSizes(&cx,&cy);
	ptrDst->CreateNew(cx,cy);
	POINT point;
	ptrSrc->GetOffset(&point);
	ptrDst->SetOffset(point,true);

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, byte*, cy);
		smart_alloc(dstRows, byte*, cy);
		src = srcRows;
		dst = dstRows;

		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(&srcRows[y],y,FALSE);
			ptrDst->GetRow(&dstRows[y],y,FALSE);
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] = 0;
			}
		}

		IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		IImage3Ptr      sptrI(ptrI);
		sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );

		smart_alloc(grayRows, color *, cy);
		for(int y=0; y<cy; y++)
		{
			sptrI->GetRow(y, 0, grayRows+y);
			for(int x=0; x<cx; x++)
				grayRows[y][x] = 0;
		}

		//if(bShowDebugImage)
		if(0) { // debug - вставка в документ вспомогательной картинки
			if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
			if(doc==0) return S_FALSE;
			SetToDocData(doc, sptrI);
		}

		//FindCurve();

		CRadiusDetector det(srcRows, cx, cy, r_min); // для меньшего радиуса берем handle = r_min
		double th1 = det.GetVal( r_min );
		CRadiusDetector det2(srcRows, cx, cy, r_max);
		double th2 = det2.GetVal( r_max );

		for(int y1=1; y1<cy-1; y1++)
		{
			for(int x1=1; x1<cx-1; x1++)
			{
				if(!srcRows[y1][x1]) continue; // работаем только внутри маски
				if(	srcRows[y1][x1-1] && srcRows[y1][x1+1] && 
					srcRows[y1-1][x1] && srcRows[y1+1][x1] )
					continue; // работаем только на краях маски
				int cnt_max = det.GetVal(x1,y1);
				int cnt_max2 = det2.GetVal(x1,y1);

				//grayRows[y1][x1] = cnt_max*65535/na;
				if(cnt_max>=th1 && cnt_max2<=th2)
				{
					dstRows[y1][x1] = 255;
				}
				else
				{
					dstRows[y1][x1] = max(128, dstRows[y1][x1]);
				}
			}
		}

		{ // ----------- выделение областей, удаление убогих
			byte **buf = srcRows;

			_ptr_t<int> smart_ind(256); // _ptr_t - из awin, он умеет реаллокейтить
			int *ind = smart_ind.ptr();

			int cur_ind=0;
			for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
			//индексы для объединения нескольких областей в одну

			smart_alloc(cnt_buf, int, cx*cy);
			smart_cnt_buf.zero();

			smart_alloc(cnt, int*, cy);
			for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;

			cnt[0][0]=cur_ind=0;
			for(int x=1; x<cx; x++)
			{
				if(buf[0][x]!=buf[0][x-1])
				{
					cur_ind++;
					if(cur_ind>=smart_ind.size())
					{
						smart_ind.alloc(smart_ind.size()*2);
						ind = smart_ind.ptr();
						for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
					}
				}
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
						if(cur_ind>=smart_ind.size())
						{
							smart_ind.alloc(smart_ind.size()*2);
							ind = smart_ind.ptr();
							for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
						}
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

			smart_alloc(s255, int, cur_ind+1);
			smart_alloc(s128, int, cur_ind+1);
			smart_alloc(area, int, cur_ind+1);
			smart_s255.zero();
			smart_s128.zero();
			smart_area.zero();

			for(y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int c=ind[cnt[y][x]];
					area[c]++;
					if(dstRows[y][x]==255)
						s255[c]++;
					else if(dstRows[y][x]==128)
						s128[c]++;
				}
			}

			smart_alloc(del_it, BOOL, cur_ind+1);
			smart_del_it.zero();
			for(i=cur_ind; i>=0; i--)
			{
				if( s255[i] <= (s128[i]+s255[i])*threshold )
					del_it[i] = TRUE;
			}

			for(y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int c=ind[cnt[y][x]];
					if(srcRows[y][x])
					{
						if(del_it[c]) dstRows[y][x]=0;
						else dstRows[y][x] = 255;
					}
				}
			}


		} // ----------- end выделение областей, удаление убогих

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////

