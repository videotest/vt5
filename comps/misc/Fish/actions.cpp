#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "\vt5\awin\misc_ptr.h"
#include "alloc.h"
#include "math.h"
#include "BinImageInt.h"
#include "PropBag.h"
#include "fish.h"
#include "color.h"
#include "Settings.h"

IMPLEMENT_DYNCREATE(CFishByClustering,	CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CFishByClustering, "FISH.FishByClustering", 
0x254b39f2, 0x9afa, 0x4ea6, 0xaa, 0xf5, 0x3c, 0xd6, 0xa8, 0x5d, 0x64, 0xee);

static const GUID guidFishByClustering = 
{ 0x68df200d, 0x8dc5, 0x4db9, { 0xbd, 0xba, 0xd4, 0x76, 0x1c, 0xe3, 0x81, 0xdb } };

ACTION_INFO_FULL(CFishByClustering, IDS_FISHBYCLUSTERING, -1, -1, guidFishByClustering);
ACTION_TARGET(CFishByClustering, szTargetAnydoc );

#define szStopOnStage	"StopOnStage"
#define szSourceImage	"Image"
#define szSourceBinary	"Binary"
#define szFishByClustering	"FishByClustering"

ACTION_ARG_LIST(CFishByClustering)
	ARG_INT( szStopOnStage, 0 )
	ARG_IMAGE( szSourceImage )
	ARG_BINIMAGE( szSourceBinary )
	RES_IMAGE( szFishByClustering )
END_ACTION_ARG_LIST();

CFishByClustering::CFishByClustering(void)
{
}

CFishByClustering::~CFishByClustering(void)
{
}

static int nCompareMethod=2;
static int nAverageMethod=1;
static double fGradLimit=0.2;

inline double VectorDist(double* a, double* b, long nPanes)
{	// ����������
	// nCompareMethod==1: VectorDist = 1 - cos(angle)  (angle - ���� ����� ���������)
	// nCompareMethod==2: VectorDist = ���������� �� b �� ����� a, ������� �� ����� a
	double aa=0.0, ab=0.0, bb=0.0;
	for(int i=0; i<nPanes; i++)
	{
		aa += a[i]*a[i];
		ab += a[i]*b[i];
		bb += b[i]*b[i];
	}
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
	else return 1; // �� ��������� ���������� �������
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
{	// nAverageMethod==0 - ��� ���� ����� 1
	// nAverageMethod==1 - ��� ����� ������������ ��������
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


/*
	for(int y=1; y<cy; y++)
	{
		for(int x=1; x<cx; x++)
		{
			d1 = distance( pt(x,y), cluster(x-1,y) );
			d2 = distance( pt(x,y), cluster(x,y-1) );
			if(d1<t || d2<t)
			{
				if(d1<d2) cluster(x-1,y) += pt(x,y);
				else      cluster(x,y-1) += pt(x,y);
				if(d1<t && d2<t && �������� ������)
				{
					������� �� 2-� ��������� += �������
				}
			}
			else ������� ������� (x,y)
		}
	}
*/

static void MarkSegments(color **img, byte **mask, color **cnt, int cx, int cy, int nPanes)
{
	if(cx<3 || cy<3) return; //�� ����� ����� �� ��������

	_ptr_t<int> smart_ind(256); // _ptr_t - �� awin, �� ����� �������������
	int *ind = smart_ind.ptr();

	_ptr_t<CCluster> smart_clusters(256); // _ptr_t - �� awin, �� ����� �������������
	CCluster *clusters = smart_clusters.ptr();

	int cur_ind=0;
	for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
	//������� ��� ����������� ���������� �������� � ����

	CCluster pt;
	pt.m_nPoints=1;
	pt.m_nPanes=nPanes;

	int cnt_free=1;

	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(mask[y][x]<128) continue;
			for(int pane=0; pane<nPanes; pane++)
				pt.m_center[i] = img[pane*cy+y][x];

			double d1=1e30, d2=1e30;
			if(x>0 && mask[y][x-1])
			{
				d1 = 0; // d1 = distance( pt(x,y), cluster(x-1,y) );
			}
			if(y>0 && mask[y-1][x])
			{
				d2 = 0; // d2 = distance( pt(x,y), cluster(x,y-1) );
			}

			if(d1<0.5 || d2<0.5)
			{
				int n;
				n = (d1<d2) ? cnt[y][x-1] : cnt[y-1][x]; // �������, � �������� ������������ �����
				cnt[y][x] = n;
				clusters[n] += pt.m_center; // ��������� � �������� �����
				if(d1<0.5 && d2<0.5 && 1/*�������� ������*/)
				{
					// ������� �� 2-� ��������� += �������
				}
			}
			else
			{
				// ������� ������� (x,y)
				cnt[y][x] = cnt_free;
				//if(cnt_free >= 
				cnt_free++;
			}
		}
	}
}

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

bool CFishByClustering::InvokeFilter()
{
	IImage4Ptr sptrSrc(GetDataArgument(szSourceImage));
	IBinaryImagePtr sptrBin(GetDataArgument(szSourceBinary));
	IImage4Ptr sptrDst(GetDataResult(szFishByClustering) );

	if(sptrSrc==0 || sptrBin==0 || sptrDst==0) return false;

	int nDebugState = GetArgumentInt(szStopOnStage);
	// ��� 0 - ���������� �������� � �������-������

	int nPixelOrder = ::GetValueInt(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "PixelOrder", 3);
	// �������, � ������� ��������� ������� ����������� ��� �������������
	// 0 - �� �������
	// 1 - � ��������� �������
	// 2 - � ������� �������� DAPI
	// 3 - � ������� �������� ������������ �������� MFISH

	nCompareMethod = ::GetValueInt(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "CompareMethod", 2);
	// ����� ��������� ����� � ��������� (����������� �� ��������)
	// 1 - ���������� ���� (������, 1-cos)
	// 2 - ���������� ���������� �� ����� �� ���� �� 0 � ��������, ������� �� ����� ��������

	nAverageMethod = ::GetValueInt(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "AverageMethod", 1);
	// ����� ���������� �����, �������� � �������
	// 0 - ������ �������
	// 1 - ����� ����� ����� ����� ������������

	fGradLimit = ::GetValueDouble(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "GradLimit", 0.2);
	// ������ ���������� ��� ������������� ����� � ��������
	// ��. nCompareMethod

	double fSameValuesForAllClasses = ::GetValueDouble(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "SameValuesForAllClasses", 0.0);
	// ���� 0 - �� ������� ��� ��� ������� ������ ������������ ����������
	// ���� 1 - �� ���������, ��� ������ ��� ���� ���� � ����� ��� ���� ����
	// ������������� �������� - ��� � ���� �������������...

	double fAutoAdjustClasses = ::GetValueDouble(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "AutoAdjustClasses", 1.0);
	// ���� 0 - �� ������ ������� ������������ � ����� ������
	// ���� 1 - �� ����� ���������� ����������� ����� ��������� k-means
	// ������������� �������� - ��� � ���� �������������... ���� ���� �������� ����������

	double fAutoAdjustClassesWeight = ::GetValueDouble(::GetAppUnknown(), FISH_ROOT "\\FishByClustering", "AutoAdjustClassesWeight", 0.0);
	// �� ����� ������� ����� ����������� ���� �������
	// 0 - ��� ���� ���������
	// 1 - ����������� ��� ��� �������, ��� ���������� �����, ��������� - ��� �������
	fAutoAdjustClassesWeight = max(0, min(5,fAutoAdjustClassesWeight));

	int cx=0,cy=0;
	sptrSrc->GetSize(&cx,&cy);
	POINT ptOffset;
	sptrSrc->GetOffset(&ptOffset);
	int nPanes0=0; // ����� ���������� ���
	sptrSrc->GetPanesCount(&nPanes0);

	int nPanes = nPanes0; // ���� ���; ����� �������� ���������� ����������� ���

	{
		int cx1=0, cy1=0;
		sptrBin->GetSizes(&cx1,&cy1);
		if(cx1!=cx || cy1!=cy) return false;
	}

	//sptrRes->CreateCompatibleImage( sptrSrc );
	sptrDst->CreateImage(cx, cy, _bstr_t("RGB"),-1);
	sptrDst->SetOffset(ptOffset, false);

	smart_alloc(srcRows, color*, cy*nPanes);
	for(int pane=0; pane<nPanes; pane++ )
	{
		for(int y=0; y<cy; y++)
		{
			sptrSrc->GetRow(y, pane, &srcRows[pane*cy+y]);
		}
	}

	smart_alloc(dstRows, color*, cy*3);
	for(int pane=0; pane<3; pane++ )
	{
		for(int y=0; y<cy; y++)
		{
			sptrDst->GetRow(y, pane, &dstRows[pane*cy+y]);
		}
	}

	smart_alloc(binRows, byte *, cy);
	for(int y=0; y<cy; y++)
	{
		sptrBin->GetRow(&binRows[y], y, false);
	}

	smart_alloc(counters, int, 256);
	for(int i=0; i<256; i++) counters[i]=0;
	// ��������: ��, ��� �� ��������� �����, � ���� = 65535,
	// ��������� - ��������� ����� 0..255, �� ������� ����� �����������
	// (� �������� ��������, ������� � ������� = 255)
	for(int x=0; x<cx; x++) dstRows[0][x]=65535;
	for(int y=1; y<cy-1; y++)
	{
		dstRows[y][0]=65535;
		for(int x=1; x<cx-1; x++)
		{
			if(binRows[y][x]<128) dstRows[y][x] = 65535;
			else
			{
				int n=0; // nPixelOrder==0 - �� �������, ������ ��� � ���� ������
				if(nPixelOrder==1) n = rand()%256; // � ��������� �������
				else if(nPixelOrder==2) n = srcRows[y+cy][x]/256; // �� DAPI
				else if(nPixelOrder==3)
				{
					for(int pane=2; pane<nPanes; pane++ )
					{
						int c = srcRows[pane*cy+y][x]/256;
						if(n<c) n=c;
					}
				}
				dstRows[y][x] = n;
				counters[n]++;
			}
		}
		dstRows[y][cx-1]=65535;
	}
	for(int x=0; x<cx; x++) dstRows[cy-1][x]=65535;

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
			int n=dstRows[y][x];
			if(n<256)
			{
				dstRows[y][x]=0; // ������� - ����� ��� �� �����
				start_points[n].ptr()[counters[n]] = _point(x,y);
                counters[n]++;
			}
		}
	}


	int nClusters=1; // ������� ������� ������
	_ptr_t<CCluster> clusters(256);
	for(int n=255; n>=0; n--)
	{
		for(int j=0; j<counters[n]; j++)
		{
			_point *ppt=start_points[n].ptr()+j;
			if(dstRows[ppt->y][ppt->x]==0)
			{
				MarkCluster(srcRows+cy*2, dstRows, cx,cy, nPanes-2, ppt->x,ppt->y, nClusters, clusters.ptr()[nClusters]);
				nClusters++;
				if( nClusters>=clusters.size() ) clusters.alloc(nClusters*2);
			}
			//if(nClusters>1000) - ����� ���������������, ������� ������� �����

		}
	}

	if(nDebugState&1)
	{
		for(int pane=2; pane<nPanes; pane++ )
		{
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int n = dstRows[y][x];
					if(n==65535) n=0;
					if(n==0) srcRows[pane*cy+y][x] = 0;
					else srcRows[pane*cy+y][x] = color(clusters.ptr()[n].m_center[pane-2]);
				}
			}
		}
	}

	// 2-3 ���� - ������������� ���������� ��������, 
	CString strFilePath = GetValueString(::GetAppUnknown(), "\\KaryoOptions\\MultiFISHScheme","Scheme");
	sptrIFileDataObject	sptrF;
	sptrF.CreateInstance( szNamedDataProgID );

	if( sptrF != 0 )
	{
		if( sptrF->LoadFile( _bstr_t(strFilePath) ) != S_OK ) sptrF=0;
	}

	unsigned uPanesUsed = 0;
	int nCombos=g_bDefColors?(1<<nPanes):0;
	if( sptrF != 0 )
	{
		nCombos = ::GetValueInt(sptrF, "\\MFISH\\Combinations", "Count", 0);
	}

	smart_alloc(arrComboIdx, unsigned long, nCombos);
	smart_alloc(arrComboR, byte, nCombos);
	smart_alloc(arrComboG, byte, nCombos);
	smart_alloc(arrComboB, byte, nCombos);
	char szName[128];
	for(int i=0; i<nCombos; i++)
	{
		arrComboIdx[i] = i%(1<<nPanes);
		arrComboIdx[i] &= uPanesUsed;
		COLORREF clr = 0x808080;
		if( sptrF != 0 )
		{
			_variant_t	var;
			sprintf(szName, "Combination%u", i+1);
			arrComboIdx[i] = ::GetValueInt(sptrF, "\\MFISH\\Combinations", szName, arrComboIdx[i]);
			sprintf(szName, "Color%u", i+1);
			clr = ::GetValueColor(sptrF, "\\MFISH\\Combinations", szName, clr);
		}
		arrComboR[i] = GetRValue(clr);
		arrComboG[i] = GetGValue(clr);
		arrComboB[i] = GetBValue(clr);
	}

	smart_alloc(big_clusters, CCluster, nCombos);
	smart_alloc(big_clusters0, CCluster, nCombos);
	smart_alloc(best_big_cluster, int, nClusters);

	if(nCombos!=0)
	{ // ���� ���� ���������� � ������� - �� �������� �������� � ���, ��������� �� �����
		// ��������� ������� ������� ��� ���
		double init_dark[MAXPANES];
		double init_light[MAXPANES];
		for(int pane=2; pane<nPanes; pane++)
		{
			double cnt_light=0, cnt_dark=0;
			for(int n=0; n<nCombos; n++)
			{
				if(arrComboIdx[n] & (1<<pane)) cnt_light++;
				else cnt_dark++;
			}
			double c_theor = cnt_light / max(1, cnt_light+cnt_dark);
			int hist[256] = {0};
			for(int n=1; n<nClusters; n++)
			{
				int c = clusters.ptr()[n].m_center[pane-2];
				c = max(0, min(255, c/256));
				hist[c] += clusters.ptr()[n].m_nPoints;
			}
			for(int i=1; i<256; i++) hist[i]+=hist[i-1];
			double lim = c_theor*hist[255];
			for(int i=0; i<256; i++) if(hist[i]>=lim) break;
			lim = i*256; // ������� � �����. ��� ���� - �������, ������ - ������ (��������������)

			double s0_dark=0, s1_dark=0;
			double s0_light=0, s1_light=0;
			for(int n=1; n<nClusters; n++)
			{
				int c = clusters.ptr()[n].m_center[pane-2];
				if(c>lim)
				{
					s0_light += clusters.ptr()[n].m_nPoints;
					s1_light += clusters.ptr()[n].m_center[pane-2] * clusters.ptr()[n].m_nPoints;
				}
				else
				{
					s0_dark += clusters.ptr()[n].m_nPoints;
					s1_dark += clusters.ptr()[n].m_center[pane-2] * clusters.ptr()[n].m_nPoints;
				}
			}
			if(s0_dark) s1_dark /= s0_dark;
			else s1_dark = 0;
			if(s0_light) s1_light /= s0_light;
			else s1_light = 65535;

			char szName[128];
			sprintf(szName, "Threshold%u", pane);
			int th = ::GetValueInt(sptrF, "\\MFISH\\Combinations", szName, 128);

			init_dark[pane-2] = s1_dark*th/128;
			init_light[pane-2] = s1_light*th/128;
		}

		for(int i=0; i<nCombos; i++)
		{
			big_clusters[i].m_nPoints = 0;
			big_clusters[i].m_nPanes = nPanes-2;
			big_clusters[i].m_fWeightSum = 0;
			for(int pane=2; pane<nPanes; pane++)
				big_clusters[i].m_center[pane-2] =
				// (arrComboIdx[i] & (1<<pane)) ? 65535 : 0;
				(arrComboIdx[i] & (1<<pane)) ? init_light[pane-2] : init_dark[pane-2];
		}

		smart_alloc(big_clusters_weight, double, nCombos);
		for(int n=0; n<nCombos; n++) big_clusters_weight[n]=1;

		for(int steps=0; steps<100; steps++)
		{
			for(int i=0; i<nCombos; i++)
			{	// ������� ����� ��� ��������� � �������� ����
				// � ������� �������� ��� �������
				big_clusters0[i] = big_clusters[i];
				big_clusters[i].m_nPoints = 0;
				big_clusters[i].m_fWeightSum = 0;
			}
			for(int n=1; n<nClusters; n++)
			{			
				int best_cluster = 0;
				double best_dist = 1e31;
				for(int i=0; i<nCombos; i++)
				{
					double d = big_clusters0[i].dist(clusters.ptr()[n].m_center);
					d /= big_clusters_weight[i];
					if(d<best_dist)
					{
						best_cluster = i;
						best_dist = d;
					}
				}
				big_clusters[best_cluster] += clusters.ptr()[n];
				best_big_cluster[n] = best_cluster;
			}

			double s0_dark[MAXPANES], s1_dark[MAXPANES];
			double s0_light[MAXPANES], s1_light[MAXPANES];
			for(int pane=0; pane<nPanes; pane++)
			{
				s0_dark[pane]=0; s1_dark[pane]=0;
				s0_light[pane]=0; s1_light[pane]=0;
			}

			// ���������� ������� ����� ������� � �������� �� �����
			for(int n=0; n<nCombos; n++)
			{
				for(int pane=2; pane<nPanes; pane++)
				{
					if(arrComboIdx[n] & (1<<pane))
					{
						s0_light[pane-2] += big_clusters[n].m_nPoints;
						s1_light[pane-2] += big_clusters[n].m_center[pane-2] * big_clusters[n].m_nPoints;
					}
					else
					{
						s0_dark[pane-2] += big_clusters[n].m_nPoints;
						s1_dark[pane-2] += big_clusters[n].m_center[pane-2] * big_clusters[n].m_nPoints;
					}
				}
			}

			// ���������� �����...
			for(int pane=2; pane<nPanes; pane++)
			{
				s1_dark[pane-2] /= max(1,s0_dark[pane-2]);
				s1_light[pane-2] /= max(1,s0_light[pane-2]);
			}

			// �������������� ����� ���������
			for(int n=0; n<nCombos; n++)
			{
				for(int pane=2; pane<nPanes; pane++)
				{
					if(arrComboIdx[n] & (1<<pane))
					{
						big_clusters[n].m_center[pane-2] += 
							(s1_light[pane-2] - big_clusters[n].m_center[pane-2])
							* fSameValuesForAllClasses;
					}
					else
					{
						big_clusters[n].m_center[pane-2] += 
							(s1_dark[pane-2] - big_clusters[n].m_center[pane-2])
							* fSameValuesForAllClasses;
					}
				}
			}

			// ��������� ������ 2.1 - ����� ������� ������ ���� �������, ������� �����
			double count = 0;
			for(int n=0; n<nCombos; n++) count += big_clusters[n].m_nPoints;
			count = max(count,1);
			for(int n=0; n<nCombos; n++)
			{
				double ratio = big_clusters[n].m_nPoints / (count/nCombos); // 0..inf
				double c = ratio>1 ? 1-1/ratio : ratio-1; // ��������� �� -1 �� 1
				if(ratio>1) big_clusters_weight[n] *= 1 - c*fAutoAdjustClassesWeight/10;
				double d = 1+fAutoAdjustClassesWeight; // ������ ��������� ��� ����
				big_clusters_weight[n] = max(1/d, min(d, big_clusters_weight[n]));
			}

			for(int n=0; n<nCombos; n++)
			{
				for(int pane=2; pane<nPanes; pane++)
				{
					big_clusters[n].m_center[pane-2] = 
						big_clusters[n].m_center[pane-2]*fAutoAdjustClasses +
						big_clusters0[n].m_center[pane-2]*(1-fAutoAdjustClasses) ;
				}
			}
			
		}
	} // end if(nCombos!=0)

	// ��������� ����������
	{
		smart_alloc(r, color, nClusters);
		smart_alloc(g, color, nClusters);
		smart_alloc(b, color, nClusters);
		for(int n=0; n<nClusters; n++) r[n] = rand()%40000 + 16384;
		for(int n=0; n<nClusters; n++) g[n] = rand()%40000 + 16384;
		for(int n=0; n<nClusters; n++) b[n] = rand()%40000 + 16384;

		if(nCombos!=0)
		{
			for(int n=1; n<nClusters; n++) r[n] = arrComboR[best_big_cluster[n]]*256;
			for(int n=1; n<nClusters; n++) g[n] = arrComboG[best_big_cluster[n]]*256;
			for(int n=1; n<nClusters; n++) b[n] = arrComboB[best_big_cluster[n]]*256;
		}

		r[0]=0; g[0]=0; b[0]=0;

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int c = dstRows[y][x];
				if(c==65535) c=0;
				dstRows[y][x] = r[c];
				dstRows[y+cy][x] = g[c];
				dstRows[y+cy*2][x] = b[c];
			}
		}
	}

	StartNotification(1000);
	Notify(999);
	FinishNotification();

	return true;
}
