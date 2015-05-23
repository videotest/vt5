#include "stdafx.h"
#include "segmserial.h"

#include "misc_utils.h"
#include "misc_math.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "docview5.h"
#include <queue>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CSegmSerialInfo::s_pargs[] = 
{
	{"dc", szArgumentTypeDouble, "40.0", true, false},
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Sequence",	szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CSegmSerial2Info::s_pargs[] = 
{
	{"Colors", szArgumentTypeInt, "10", true, false},
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Sequence",	szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

static bool CreateCompatible(IUnknownPtr ptrDst, IUnknownPtr ptrSrc)
{
	ICompatibleObjectPtr ptrCO( ptrDst );
	if( ptrCO == NULL ) return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
		return false;
	return true;
}

class color_comparer
{
public:
	vector<double> centers;
	vector<double> normals;
	vector<double> weights;
	int nClusters;
	int nPanes;
	double fDistMethod;

public:
	void init(int _nPanes, const char* szSection)
	{
		nPanes = _nPanes;
		// прочитаем инфу о кластерах
		nClusters = GetValueInt( GetAppUnknown( ), szSection, "Count", 2 );
		//ASSERT(nClusters>=1);

		centers.resize(nClusters*nPanes);
		normals.resize(nClusters*nPanes);
		weights.resize(nClusters);
		//phases.resize(nClusters);

		char sz[256];
		for(int cluster=0; cluster<nClusters; cluster++ )
		{
			sprintf(sz, "Color%u", cluster);
			_bstr_t bstrColor = GetValueString( GetAppUnknown( ), "\\SegmColorPhases", sz, "" );
			char *p = bstrColor;
			for(int nPane=0; nPane<nPanes; nPane++)
			{
				while(*p!=0 && !isdigit_ru(*p) && *p!='-') p++;
				double n=0;
				int sign=1;
				if(*p=='-') { sign=-1; p++; }
				while(isdigit_ru(*p))
				{
					n = n*10 + (*p-'0');
					p++;
				}
				n*=sign;
				centers[cluster*nPanes+nPane] = n*256;
			}
			sprintf(sz, "Weight%u", cluster);
			weights[cluster] = GetValueDouble( GetAppUnknown( ), "\\SegmColorPhases", sz, 1.0 );
			weights[cluster] *= weights[cluster];
			weights[cluster] = max(1e-10, min(1e10, weights[cluster] ));
			sprintf(sz, "Phase%u", cluster);
			//phases[cluster] = GetValueInt( GetAppUnknown( ), "\\SegmColorPhases", sz, 255 );
		}
	}

	inline double calc_dist(color* pColors, int cluster)
	{
		double r=0;
		for( int nPane=0; nPane<nPanes; nPane++ )
		{
			double d = centers[cluster*nPanes+nPane];
			if(d>=0)
				r += sqr( pColors[nPane] - d );
			else
				r += 64.0*64.0*256.0*256.0; // если задать -1 - то значение данной компоненты цвета не используется (для выделения объектов на неизвестном фоне и наоборот)
		}

		if(fDistMethod!=0.0) // если 0.0 - сэкономим время
		{
			double r2=0, r3=0;
			for( int nPane=0; nPane<nPanes; nPane++ )
			{
				color c = pColors[nPane];
				r2 += c*normals[cluster*nPanes+nPane];
				r3 += sqr(c);
			}
			r2 = r3 - sqr(r2);
			r = r2*fDistMethod + r*(1-fDistMethod);
			// Если fDistMethod=1 - то считаем расстояние до прямой, проходящей через центр кластера;
			// если 0 - до центра кластера
		}

		r /= weights[cluster];
		return sqrt(r);
	}

	inline int get_index(color* pColors)
	{
		int best=0;
		double r_best=1e20;
		for(int cluster=0; cluster<nClusters; cluster++)
		{
			double r = calc_dist(pColors, cluster);
			if(r<r_best)
			{
				best = cluster;
				r_best = r;
			}
		}
		return best;
	}
};

struct CPoint
{
	WORD x;
	WORD y;
	float c; // приоритет точки (чем меньше, тем раньше должна быть обработана)
	CPoint(WORD _x, WORD _y, int _c) : x(_x), y(_y), c(_c) {};
};

struct CPointLess : binary_function<CPoint, CPoint, bool>
{
	bool operator()(const CPoint& pt1, const CPoint& pt2) const
	{
		return pt1.c < pt2.c;
	}
};

struct CPointGreater : binary_function<CPoint, CPoint, bool>
{
	bool operator()(const CPoint& pt1, const CPoint& pt2) const
	{
		return pt1.c > pt2.c;
	}
};

class processor
{
public:
	processor() {};
	virtual ~processor()
	{
		if( distanceRows.size()>0 )
			delete distanceRows[0];
	}

	int cx;
	int cy;
	int nPanes;
	double m_dc;

	priority_queue<CPoint,vector<CPoint>, CPointGreater> q;
	vector<color*> srcRows;
	vector<color*> dstRows;
	vector<float*> distanceRows; // "distance" map

	vector<color> test;

	color_comparer cc;

	inline void expand(CPoint &pt, int dx, int dy)
	{
		if( dstRows[pt.y+dy][pt.x+dx] & 0x8000 ) return; // если точка уже обработана - выход

		int dc = int(m_dc*256);
		for(int pane=0; pane<nPanes; pane++) test[pane] = srcRows[pane*cy+pt.y+dy][pt.x+dx];
		int cluster = dstRows[pt.y][pt.x] & 0x7FFF;
		int cluster_prev = dstRows[pt.y+dy][pt.x+dx] & 0x7FFF;
		double dist = cc.calc_dist(&test[0], cluster);
		double dist_prev = cc.calc_dist(&test[0], cluster_prev);

		//bool bHasNeighbor = dstRows[pt.y+dy][pt.x+dx] & 0x4000; // в эту точку уже расползались
		//dstRows[pt.y+dy][pt.x+dx] |= 0x4000; // пометим, что в эту точку уже расползались

		dstRows[pt.y+dy][pt.x+dx] = dstRows[pt.y][pt.x]; // располземся

		double dist0 = pt.c;
		q.push( CPoint(pt.x+dx,pt.y+dy, max(dist0, dist-dc)) );
		//q.push( CPoint(pt.x+dx,pt.y+dy, dist0+(dist-dist0)*0.03 ));
	}

	inline void init(IImage4Ptr sptrSrc, IImage4Ptr sptrDst) // проинитить основные переменные и массивы
	{
		cx = 0; cy = 0;
		sptrSrc->GetSize(&cx,&cy);
		nPanes = ::GetImagePaneCount( sptrSrc );
		srcRows.resize(cy*nPanes);
		dstRows.resize(cy*nPanes);
		test.resize(nPanes);
		for(int pane=0; pane<nPanes; pane++)
		{
			for(int y=0; y<cy; y++)
			{
				sptrSrc->GetRow(y, pane, &srcRows[pane*cy+y]);
				sptrDst->GetRow(y, pane, &dstRows[pane*cy+y]);
			}
		}
		distanceRows.resize(cy);
		distanceRows[0] = new float[cx*cy];
		for(int y=1; y<cy; y++)
			distanceRows[y] = distanceRows[0] + y*cx;

		cc.init(nPanes, "\\SegmColorPhases");
	}

	inline void start() // собственно начать работу - заполнить начальные цвета
	{
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				for(int pane=0; pane<nPanes; pane++) test[pane] = srcRows[pane*cy+y][x];
				int cluster = cc.get_index(&test[0]);
				dstRows[y][x] = cluster; // предварительно пометим этим номером
				double dist = cc.calc_dist(&test[0], cluster);
				q.push( CPoint(x,y, dist) );
				//for(int pane=0; pane<nPanes; pane++) dstRows[pane*cy+y][x] = cc.centers[c*nPanes + pane];
			}
		}
	}

	inline void process()
	{
		int n=0;
		int cc = 0;
		double coeff = 65000.0/(cx*cy);
		while ( !q.empty() )
		{
			CPoint pt = q.top();
			q.pop();
			if(dstRows[pt.y][pt.x] & 0x8000) continue; // точка уже обработана

			// запихаем необработанных соседей в q - как более темных, чем надо
			if(pt.x>0) expand(pt, -1,0);
			if(pt.x<cx-1) expand(pt, 1,0);
			if(pt.y>0) expand(pt, 0,-1);
			if(pt.y<cy-1) expand(pt, 0,1);

			//dstRows[pt.y][pt.x] = 1 + (n++)*coeff;
			dstRows[pt.y][pt.x] |= 0x8000; // пометим как обработанную
		}
	}

	inline void finish()
	{
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int cluster = dstRows[y][x] & 0x7FFF;
				for(int pane=0; pane<nPanes; pane++) dstRows[pane*cy+y][x] = cc.centers[cluster*nPanes + pane];
			}
		}
	}
};


bool CSegmSerial::InvokeFilter()
{
	IImage4Ptr	ptrSrc( GetDataArgument("Img") );
	IImage4Ptr	ptrDst( GetDataResult("Sequence") );

	if( ptrSrc == NULL || ptrDst==NULL )
		return false;

	if( !CreateCompatible(ptrDst, ptrSrc) ) return false;

	m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 );

	int cx, cy;
	ptrSrc->GetSize(&cx,&cy);

	try
	{
		StartNotification( cy, 1 );

		processor pc;

		pc.init(ptrSrc, ptrDst);
		pc.m_dc = GetArgDouble("dc");
		pc.start();
		pc.process();
		pc.finish();

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

bool CSegmSerial2::InvokeFilter()
{
	IImage4Ptr	sptrSrc( GetDataArgument("Img") );
	IImage4Ptr	sptrDst( GetDataResult("Sequence") );

	if( sptrSrc == NULL || sptrDst==NULL )
		return false;

	if( !CreateCompatible(sptrDst, sptrSrc) ) return false;

	m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 );
	int nColors = GetArgLong("Colors");

	int cx, cy;
	sptrSrc->GetSize(&cx,&cy);

	try
	{
		//vector<color*> srcRows;
		//vector<color*> dstRows;
		//srcRows.resize(cy);
		//dstRows.resize(cy);
		smart_alloc(srcRows, color*, cy);
		smart_alloc(dstRows, color*, cy);
		smart_alloc(markRows, color*, cy);
		for(int y=0; y<cy; y++)
		{
			sptrSrc->GetRow(y, m_nWorkPane, &srcRows[y]);
			sptrDst->GetRow(y, 0, &dstRows[y]);
			sptrDst->GetRow(y, 1, &markRows[y]);
			for(int x=0; x<cx; x++)
				dstRows[y][x] = 65535;
		}

		priority_queue<CPoint,vector<CPoint>, CPointGreater> q;

		{
			int x=0, y=0;
			for(int i=0; i<nColors; i++)
			{
				double c0 = srcRows[y][x];
				int mark = i * 13789;
				int n=0;
				q.push( CPoint(x,y, 0) );
				while ( !q.empty() )
				{
					CPoint pt = q.top();
					q.pop();
					if(pt.c<dstRows[pt.y][pt.x])
					{
						n++;
						c0 += (srcRows[y][x]-c0)/n; // адаптивный цвет - средний по области
						x = pt.x;
						y = pt.y;
						dstRows[y][x] = pt.c;
						markRows[y][x] = mark;
						int cc = srcRows[y][x];
						if(x>0) q.push( CPoint(x-1,y, pt.c*0.9 + 0.01*abs(c0-srcRows[y][x-1]) ));
						if(x<cx-1) q.push( CPoint(x+1,y, pt.c*0.9 + 0.01*abs(c0-srcRows[y][x+1]) ));
						if(y>0) q.push( CPoint(x,y-1, pt.c*0.9 + 0.01*abs(c0-srcRows[y-1][x]) ));
						if(y<cy-1) q.push( CPoint(x,y+1, pt.c*0.9 + 0.01*abs(c0-srcRows[y+1][x]) ));					
					}
				}
				n = n+1;
			}
		}

		StartNotification( cy, 1 );

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

