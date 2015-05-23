#include "stdafx.h"
#include "segmserial3.h"

#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "docview5.h"
#include "FastMarch.h"
#include <queue>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CSegmSerial3Info::s_pargs[] = 
{
	{"Tolerance", szArgumentTypeDouble, "10.0", true, false},
	{"DistTolerance", szArgumentTypeDouble, "1.0", true, false},
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Sequence",	szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

CSegmSerial3::CSegmSerial3(void)
{
}

CSegmSerial3::~CSegmSerial3(void)
{
}

static bool CreateCompatible(IUnknownPtr ptrDst, IUnknownPtr ptrSrc)
{
	ICompatibleObjectPtr ptrCO( ptrDst );
	if( ptrCO == NULL ) return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
		return false;
	return true;
}


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


class CDarkPointProcessor
{ // обработка от темных точек к светлым
public:
	double m_fTolerance;
	double m_fDistTolerance;
	_fast_march_t<CDarkPointProcessor>* m_pMarcher;

	priority_queue<CPoint,vector<CPoint>, CPointGreater> q; // очередь точек для последовательного добавления - заменить на свою

	CDarkPointProcessor()
	{
		m_ptrImage = 0;
		m_pRows = 0;
		m_pSum = 0;
		m_pAvg = 0;
		m_count = 0.0;
		m_fTolerance = 1.0;
		m_fDistTolerance = 1.0;
		m_nWorkPane = 1;
		m_pMarcher = 0;
	};
	~CDarkPointProcessor()
	{
		delete[] m_pRows;
		delete[] m_pSum;
		delete[] m_pAvg;
	};
	void SetupImage(IImage4* pImage)
	{
		m_ptrImage = pImage;
		pImage->GetSize(&m_cx,&m_cy);
		pImage->GetPanesCount(&m_nPanes);
		m_nWorkPane = min(m_nWorkPane, m_nPanes-1);
		m_pRows = new color* [ m_nPanes * m_cy ];
		m_pSum = new double[m_nPanes];
		m_pAvg = new double[m_nPanes];
		for(int pane=0; pane<m_nPanes; pane++)
		{
			m_pSum[pane] = m_pAvg[pane] = 0.0;
			for(int y=0; y<m_cy; y++)
			{
				pImage->GetRow(y, pane, & (m_pRows[pane*m_cy+y]) );
			}
		}
		m_count = 0.0;

		for(int y=0; y<m_cy; y++)
		{
			color* p = m_pRows[m_nWorkPane*m_cy+y];
			if(y==118)
			{
				y += 0;
			}
			for(int x=0; x<m_cx; x++)
			{
				q.push( CPoint(x,y, p[x]) );
				if(p[x]<32768)
				{
					y += 0;
				}
			}
		}
	};

public:
	inline double GetV(int x, int y, int x_from, int y_from)
	{
		return exp(m_pRows[m_nWorkPane*m_cy+y][x]/m_fTolerance);
		/*
		double s = 0.0;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			double d = m_pRows[pane*m_cy+y][x];
			d -= m_pAvg[pane];
			s += exp(d/m_fTolerance);
		}
		return s;
		*/
	};
	inline void OnAddPoint(int x, int y, double dist, double v)
	{
		m_count += 1/v;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			m_pSum[pane] += m_pRows[pane*m_cy+y][x]/v;
			m_pAvg[pane] = m_pSum[pane] / m_count;
		}

		while(!q.empty())
		{
			CPoint pt = q.top();
			if(m_pMarcher && m_pMarcher->m_pDist[pt.y*m_cx+pt.x]<1e199)
			{
				q.pop();
				continue;
			}

			double v1 = GetV(pt.x, pt.y, x, y);
			//double d = m_pRows[m_nWorkPane*m_cy+pt.y][pt.x];
			//d -= m_pAvg[m_nWorkPane];
			////d -= m_pRows[m_nWorkPane*m_cy+y][x];

			if(v1>dist*m_fDistTolerance)
				break;

			q.pop();
			if(m_pMarcher)
				m_pMarcher->AddPoint(pt.x, pt.y, dist);
		}
	};
private:
	int m_cx,m_cy,m_nPanes;
	int m_nWorkPane;
	color **m_pRows;
	double *m_pSum, *m_pAvg; // счетчики 
	double m_count;
	IImage4Ptr m_ptrImage; // на всякий случай
};


static inline int iround5(double x)
{
	return int(floor(x + 0.5));
}

bool CSegmSerial3::InvokeFilter()
{
	IImage4Ptr	ptrSrc( GetDataArgument("Img") );
	IImage4Ptr	ptrDst( GetDataResult("Sequence") );

	if( ptrSrc == NULL || ptrDst==NULL )
		return false;

	if( !CreateCompatible(ptrDst, ptrSrc) ) return false;

	m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 );

	int cx=0, cy=0, nPanes=0;
	ptrSrc->GetSize(&cx,&cy);
	ptrSrc->GetPanesCount(&nPanes);

	try
	{
		StartNotification( cy, 1 );

		_fast_march_t<CDarkPointProcessor> marcher1;
		marcher1.m_pMarcher = &marcher1; // коряво, но пока так
		marcher1.SetupImage(ptrSrc);
		marcher1.Init(cx,cy);
		marcher1.m_fTolerance = GetArgDouble("Tolerance")*256;
		marcher1.m_fDistTolerance = GetArgDouble("DistTolerance")*256;

		int x0=359, y0=196; // заменить на самые темные точки
		marcher1.AddPoint( x0, y0 );
		marcher1.OnAddPoint( x0, y0, 0, 1);

		marcher1.Process();

		double dMax=1.0;

		for( int y = 2; y < cy-2; y++ )
			for( int x = 2; x < cx-2; x++ )
				dMax = max(dMax, marcher1.m_pDist[y*cx+x]);

		double scale = 65535.0/log(1+dMax);
		//scale *= 8;

		for( int y = 0; y < cy; y++ )
		{
			for( int nPane = 0; nPane<nPanes; nPane++)
			{
				color* p=0;
				ptrDst->GetRow(y, nPane, &p );
				for( int x = 0; x < cx; x++ )
					p[x] = iround5( min(65535.0, log(1+marcher1.m_pDist[y*cx+x]) * scale) );
			}
			Notify(y);
		}
		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}

	return true;
}
