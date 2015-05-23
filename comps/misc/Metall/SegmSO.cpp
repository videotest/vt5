#include "stdafx.h"
#include "SegmSO.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "aam_utils.h"
#include <limits.h>

_ainfo_base::arg CSegmSOInfo::s_pargs[] =
{
	{"method",    szArgumentTypeInt, "1", true, false }, // 0 - границы заданы (относительно фона); 1 - берем середины между пиками; 2 - между пиками ищем минимум; 3 - ищем минимум по 2 peaks
	{"th1",       szArgumentTypeDouble, "0.8", true, false },
	{"th2",       szArgumentTypeDouble, "0.4", true, false },
	{"CreateBinary", szArgumentTypeInt, "1", true, false }, // 0 - не создавать Binary, только прописать в shell.data
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CSegmSO::CSegmSO()
{
}

/////////////////////////////////////////////////////////////////////////////
CSegmSO::~CSegmSO()
{
}

static int find_threshold(int *hist, int peak1, int peak2, int method)
{
	if(method==1) return (peak1+peak2+1)/2;
	double h_min=INT_MAX;
	int th=(peak1+peak2)/2;
	for(int i=peak1; i<=peak2; i++)
	{
		double h=0;
		if(method==3)
			h = hist[peak1]+
			double(hist[peak2]-hist[peak1])*(i-peak1)/(peak2-peak1);
		h=hist[i]-h;
		if(h<h_min)
		{
			th=i;
			h_min=h;
		}
	}
	return th;
}

void CSegmSO::FindThresholds(color **src, int cx, int cy, double th1, double th2, int nMethod)
// th1, th2 - трешолды (доля от яркости фона, 0<th2<th1<1)
// min_peak - минимальная высота пика (меньше - игнорируем) (доля от высоты пика фона)
{
	long lBorder = ::GetValueInt( GetAppUnknown(), "\\SpermSettings", "Border", 20 );
	if(lBorder>cx/4-1) lBorder=cx/4-1;
	if(lBorder>cy/4-1) lBorder=cy/4-1;
	if(lBorder<0) lBorder=0;
	// подсчитали отступ от края, исключаемый при построении гистограммы.

	smart_alloc(src4hist, color *, cy-lBorder*2);
	for(int i=0; i<cy-lBorder*2; i++) src4hist[i] = src[i+lBorder]+lBorder;

	//поиск пика гистограммы (фон)
	smart_alloc(hist, int, NC);
	double tmp=hist_smooth_level;
	hist_smooth_level=0.8;
	CalcHist(src4hist, cx-lBorder*2, cy-lBorder*2, hist);
	hist_smooth_level=tmp;
	int max_peak=0;
	int count=0;
	for(int i=0; i<NC; i++)
	{
		count += hist[i];
		if(hist[i]>hist[max_peak]) max_peak=i;
	}
	
	m_back=max_peak*(65536/NC) + (65536/NC)/2;
	SetValue(GetAppUnknown(),"\\ODensity", "Backgr", long((m_back+128)/256));
	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Backgr", long((m_back+128)/256));

	int th1hist = int(max_peak*th1);
	int th2hist = int(max_peak*th2);

	int peak1 = (th1hist+th2hist)/2;
	int peak2 = th2hist/2;
	int max1=0, max2=0;
	int count1=0, count2=0;
	for(int i=0; i<=th2hist; i++)
	{
		count2 += hist[i];
		if(hist[i]>hist[peak2]) peak2=i;
	}

	// скипнем спуск от большого пика
	while(th1hist>th2hist && hist[th1hist]>hist[th1hist-1]) th1hist--;
	for(int i=th2hist; i<=th1hist; i++)
	{
		count1 += hist[i];
		if(hist[i]>hist[peak1]) peak1=i;
	}

	m_fore1=peak1*(65536/NC) + (65536/NC)/2;
	m_fore2=peak2*(65536/NC) + (65536/NC)/2;
	//m_fore1 = int( sum1/max(1,count1)*(65536/NC) );
	//m_fore2 = int( sum2/max(1,count2)*(65536/NC) );

	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore1", long((m_fore1+128)/256));
	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore2", long((m_fore2+128)/256));

	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore1Rel", double(m_fore1)/m_back);
	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore2Rel", double(m_fore2)/m_back);

	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore1Percent", double(count1)/max(count,1));
	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Fore2Percent", double(count2)/max(count,1));

	m_threshold1=int(m_back*th1);
	m_threshold2=int(m_back*th2);

	if(nMethod!=0)
	{
		int th1_min = find_threshold(hist, peak1, max_peak, nMethod);
		int th2_min = find_threshold(hist, peak2, peak1, nMethod);
		m_threshold1 = th1_min*(65536/NC) + (65536/NC)/2;
		m_threshold2 = th2_min*(65536/NC) + (65536/NC)/2;
	}

	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Threshold1", long((m_threshold1+128)/256));
	SetValue(GetAppUnknown(),"\\SegmSO\\Result", "Threshold2", long((m_threshold2+128)/256));
}

void CSegmSO::Segmentation(color **src, byte **bin, int cx, int cy)
{
	//сегментация
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			color c=src[y][x];
			if(c>m_threshold1)
				bin[y][x]=0;
			else if(c>m_threshold2)
				bin[y][x]=1;
			else
				bin[y][x]=2;
		}
	}
}

bool CSegmSO::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBin( GetDataResult() );
	long nMethod = GetArgLong("method");
	double th1 = GetArgDouble("th1");
	double th2 = GetArgDouble("th2");
	bool bCreateBinary = GetArgLong("CreateBinary")!=0;
	
	if( pArg==NULL || pBin==NULL )
		return false;
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	int nPaneCount;
	/*IColorConvertor2* pIClrConv;
	pArg->GetColorConvertor((IUnknown**)&pIClrConv);
	pIClrConv->GetColorPanesCount(&nPaneCount);
	pIClrConv->Release();*/
	nPaneCount = ::GetImagePaneCount( pArg );

	StartNotification( size.cy, nPaneCount );
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}
	
	FindThresholds(srcRows, size.cx, size.cy, th1, th2, nMethod);
	pBin->CreateNew(size.cx, size.cy);
	pBin->SetOffset( ptOffset, TRUE);
	smart_alloc(binRows, byte *, size.cy);
	for(int y=0; y<size.cy; y++)
	{
		pBin->GetRow(&binRows[y], y, false);
	}
	Segmentation(srcRows, binRows, size.cx, size.cy);
	if(bCreateBinary)
	{
		FinishNotification();
		return true;
	}
	return false;
}
