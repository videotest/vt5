#include "stdafx.h"
#include "ChromSegm.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "aam_utils.h"
#include <limits.h>

_ainfo_base::arg CChromSegmInfo::s_pargs[] =
{
	{"InvertRatio",    szArgumentTypeInt, "0", true, false },
		//1 - как первый проход в Joyce-Loebl (обгрызает), 1 - нормально
	{"PaneNum",    szArgumentTypeInt, "0", true, false }, // по которой пане работать
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CChromSegm::CChromSegm()
{
}

/////////////////////////////////////////////////////////////////////////////
CChromSegm::~CChromSegm()
{
	
}


void CChromSegm::LoadIni()
{
	char section[]="ChromSegm";
	
	//m_SmoothSize=GetValueInt(GetAppUnknown(),section, "SmoothSize", 5);
}

static void RefineBin(color **src, byte **bin, int cx, int cy)
{
	smart_alloc(grad_buf, color, cx*cy);
	smart_grad_buf.zero();
	smart_alloc(grad, color *, cy);
	for(int y=0; y<cy; y++) grad[y]=grad_buf+y*cx;
	
	//посчитаем градиент
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        {
			double c=_hypot(
				src[y][x+1]-src[y][x-1],
				src[y+1][x]-src[y-1][x]
				)/3;
			grad[y][x]=int(c);
		}
	}
	
	//smooth gradient
	for(y=0; y<cy-1; y++)
	{
        for(int x=0; x<cx-1; x++)
        {
			grad[y][x]=(
				grad[y][x] + grad[y][x+1]+
				grad[y+1][x] + grad[y+1][x+1])/4;
		}
	}
	for(y=cy-1; y>0; y--)
	{
        for(int x=cx-1; x>0; x--)
        {
			grad[y][x]=(
				grad[y][x] + grad[y][x-1]+
				grad[y-1][x] + grad[y-1][x-1])/4;
		}
	}
	
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        {
			if(grad[y][x]>1.1*grad[y][x-1])
				bin[y][x]&=bin[y][x-1];
			if(grad[y][x]>1.1*grad[y-1][x+1])
				bin[y][x]&=bin[y-1][x+1];
			if(grad[y][x]>1.1*grad[y-1][x])
				bin[y][x]&=bin[y-1][x];
			if(grad[y][x]>1.1*grad[y-1][x-1])
				bin[y][x]&=bin[y-1][x-1];
		}
	}
	
	for(y=cy-2; y>0; y--)
	{
        for(int x=cx-2; x>0; x--)
        {
			if(grad[y][x]>1.1*grad[y][x+1])
				bin[y][x]&=bin[y][x+1];
			if(grad[y][x]>1.1*grad[y+1][x-1])
				bin[y][x]&=bin[y+1][x-1];
			if(grad[y][x]>1.1*grad[y+1][x])
				bin[y][x]&=bin[y+1][x];
			if(grad[y][x]>1.1*grad[y+1][x+1])
				bin[y][x]&=bin[y+1][x+1];
		}
	}
	
}

static void RefineBin2(color **src, byte **bin, int cx, int cy)
{
	{for(int y=8; y<cy-8; y++)
	{
        for(int x=8; x<cx-8; x++)
        {
			int m=max(
				abs(src[y][x+5]-src[y][x-5]),
				abs(src[y+5][x]-src[y-5][x]));
			int c=src[y][x];
			int d;
			
			d=abs(src[y][x+1+5]-src[y][x+1-5]);
			if(d<m)	{ m=d; bin[y][x]=bin[y][x+1]; }
			d=abs(src[y+1+5][x]-src[y+1-5][x]);
			if(d<m)	{ m=d; bin[y][x]=bin[y+1][x]; }
			d=abs(src[y][x-1+5]-src[y][x-1-5]);
			if(d<m)	{ m=d; bin[y][x]=bin[y][x-1]; }
			d=abs(src[y-1+5][x]-src[y-1-5][x]);
			if(d<m)	{ m=d; bin[y][x]=bin[y-1][x]; }
		}
	}}
	
	/*
	{for(int y=cy-3; y>1; y--)
	{
	for(int x=cx-3; x>1; x--)
	{
	}
	}}
	*/	
}

static void BinMedian(byte **bin, int cx, int cy)
{
	smart_alloc(dst_buf, byte, cx*cy);
	smart_alloc(dst, byte*, cy);
	for(int y=0; y<cy; y++) dst[y]=dst_buf+y*cx;
	
	//посчитаем градиент
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        { //можно соптимизировать... наверное :-) - может, и компилятор сдюжит
			int c=0;
			if(bin[y-1][x-1]) c++;
			if(bin[y-1][x]) c++;
			if(bin[y-1][x+1]) c++;
			if(bin[y][x-1]) c++;
			if(bin[y][x]) c++;
			if(bin[y][x+1]) c++;
			if(bin[y+1][x-1]) c++;
			if(bin[y+1][x]) c++;
			if(bin[y+1][x+1]) c++;
			dst[y][x]=(c>=5)?255:0;
		}
	}
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        { //можно соптимизировать
			bin[y][x]=dst[y][x];
		}
	}
	
}
static void BinMedian2(byte **bin, int cx, int cy)
{
	smart_alloc(dst_buf, byte, cx*cy);
	smart_alloc(dst, byte*, cy);
	for(int y=0; y<cy; y++) dst[y]=dst_buf+y*cx;
	
	//посчитаем градиент
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        { //можно соптимизировать... наверное :-) - может, и компилятор сдюжит
			int c1=0;
			if(bin[y][x]) c1++;
			int c2=c1, c3=c1;
			if(bin[y-1][x]) c2++;
			if(bin[y+1][x]) c2++;
			if(bin[y][x-1]) c2++;
			if(bin[y][x+1]) c2++;
			if(bin[y-1][x-1]) c3++;
			if(bin[y-1][x+1]) c3++;
			if(bin[y+1][x-1]) c3++;
			if(bin[y+1][x+1]) c3++;
			if(c2>=3) c1++;
			if(c3>=3) c1++;
			dst[y][x]=(c1>=2)?255:0;
		}
	}
	for(y=1; y<cy-1; y++)
	{
        for(int x=1; x<cx-1; x++)
        { //можно соптимизировать
			bin[y][x]=dst[y][x];
		}
	}
	
}

void CChromSegm::FirstPass(color **src, byte **bin, int cx, int cy, double bg_tolerance)
//первый проход сегментации, вычисление цвета фона
//bg_tolerance - во сколько раз увеличивать определенную ширину пика фона для сегментации
{
	//поиск пика гистограммы (фон) и его ширины
	smart_alloc(hist, int, NC);
	CalcHist(src, cx, cy, hist);
	int max_peak=0;
	for(int i=0; i<NC; i++)
	{
		if(hist[i]>hist[max_peak]) max_peak=i;
	}
	
	double th1_level=0.05;
	for(int peak_bound1=max_peak; peak_bound1>0; peak_bound1--)
	{
		if(hist[peak_bound1]<hist[max_peak]*th1_level) break;
	}
	for(int peak_bound2=max_peak; peak_bound2<NC-1; peak_bound2++)
	{
		if(hist[peak_bound2]<hist[max_peak]*th1_level) break;
	}
	
	double peak_count1=0, peak_count0=0;
	for(i=0; i<NC; i++)
	{
		peak_count0+=hist[i];
		peak_count1+=hist[i]*abs(i-max_peak);
	}
	double peak_width=peak_count1/max(peak_count0,1);
	
	//peak_width=max(peak_bound2-max_peak,max_peak-peak_bound1);
	
	
	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"peak=%d, width=%4.2f",max_peak, peak_width);
		s=s+s1;
		//MessageBox(0, s, "peak: ", MB_OK);
	}
	//------------------------------------
	
	int max_peak1=max_peak;
	max_peak*=(65536/NC);
	m_back=max_peak;
	peak_width*=(65536/NC);
	peak_width*=bg_tolerance;
	m_back_width=int(peak_width);
	int threshold1=max_peak-int(peak_width);
	int threshold2=max_peak+int(peak_width);
	
	//подкорректируем ширину фона - заведомо выкинув объекты
	peak_count1=0; peak_count0=0;
	for(i=0; i<NC; i++)
	{
		color c=i*(65536/NC);
		if(c>=threshold1 && c<=threshold2)
		{
			peak_count0+=hist[i];
			peak_count1+=hist[i]*abs(i-max_peak1);
		}
	}
	peak_width=peak_count1/max(peak_count0,1);
	peak_width*=(65536/NC);
	m_back_width=int(peak_width);
	
	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"peak=%d, width=%4.2f",max_peak1, peak_width/(65536/NC));
		s=s+s1;
		//MessageBox(0, s, "peak: ", MB_OK);
	}
	
	//сегментация, 1 проход
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			color c=src[y][x];
			if(c>=threshold1 && c<=threshold2)
				bin[y][x]=0;
			else
				bin[y][x]=255;
		}
	}
}

void CChromSegm::FirstPass2(color **src, byte **bin, int cx, int cy, bool invert_ratio)
//первый проход сегментации - по двум пикам гистограммы
{
	long lBorder = ::GetValueInt( GetAppUnknown(), "\\SegmSource", "Border", 20 );
	if(lBorder>cx/4-1) lBorder=cx/4-1;
	if(lBorder>cy/4-1) lBorder=cy/4-1;
	if(lBorder<0) lBorder=0;
	// подсчитали отступ от края, исключаемый при построении гистограммы.

	smart_alloc(src4hist, color *, cy-lBorder*2);
	for(int i=0; i<cy-lBorder*2; i++) src4hist[i] = src[i+lBorder]+lBorder;

	smart_alloc(hist, int, NC);
	//CalcHist(src, cx, cy, hist);
	CalcHist(src4hist, cx-lBorder*2, cy-lBorder*2, hist);
	
	//поиск пика гистограммы (фон)
	int max_peak=0;
	for(int i=0; i<NC; i++)
	{
		if(hist[i]>hist[max_peak]) max_peak=i;
	}
	
	//поиск второго пика
	int peak2=0;
	double h_max=0;
	for(i=0; i<NC; i++)
	{
		double h=double(hist[i])*(i-max_peak)*(i-max_peak);
		if(h>h_max)
		{
			peak2=i;
			h_max=h;
		}
	}

	char section[]="\\SegmSource\\Result";
	SetValue(GetAppUnknown(),section, "Background", long(max_peak*256.0/NC));
	SetValue(GetAppUnknown(),section, "Foreground", long(peak2*256.0/NC));

	SetValue(GetAppUnknown(),"\\ODensity", "Backgr", long(max_peak*256.0/NC));

	int xor_mask=(peak2>max_peak)?65535:0;
	
	int peak1=min(peak2,max_peak); 
	peak2=max(peak2,max_peak);
	
	int h_min=INT_MAX;
	int th;
	for(i=peak1; i<=peak2; i++)
	{
		int h=hist[peak1]+
			(hist[peak2]-hist[peak1])*(i-peak1)/(peak2-peak1);
		h=hist[i]-h;
		if(h<h_max)
		{
			th=i;
			h_max=h;
		}
	}
	
	if(abs(th-max_peak)>peak2-peak1) th=(peak1+peak2)/2; //th должен быть ближе к фону
	if(invert_ratio) th=peak1+peak2-th; //если выставлено - то наоборот...

	SetValue(GetAppUnknown(),section, "Threshold", long(th*256.0/NC));

	th=(th*(65536/NC)) ^ xor_mask;

	// подсчет плотности объектов
	int count0=0, count1=0;
	for(i=0; i<NC; i++)
	{
		color c=(i*(65536/NC)) ^ xor_mask;
		if(c>=th) count0 += hist[i];
		else  count1 += hist[i];
	}
	SetValue(GetAppUnknown(),section, "ObjectsPercent", count1/max(1.0, count1+count0));

	//сегментация, 1 проход
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			color c=src[y][x] ^ xor_mask;
			if(c>=th) bin[y][x]=0;
			else bin[y][x]=255;
		}
	}
}


void CChromSegm::SecondPass(color **src, byte **bin, int cx, int cy)
//второй проход сегментации, вычисление цвета объектов (максимальный пик)
{
	//поиск пика гистограммы (фон) и его ширины
	smart_alloc(hist, int, NC);
	CalcHistMasked(src, bin, 255, cx, cy, hist);
	int max_peak=0;
	int back=m_back/(65536/NC);
	for(int i=0; i<NC; i++)
	{
		hist[i]*=abs(i-back);
		hist[i]*=abs(i-back);
		if(hist[i]>hist[max_peak]) max_peak=i;
	}
	
	double peak_count1=0, peak_count0=0;
	for(i=0; i<NC; i++)
	{
		peak_count0+=hist[i];
		peak_count1+=double(hist[i])*abs(i-max_peak);
	}
	double peak_width=peak_count1/max(peak_count0,1);
	
	_bstr_t s("");
	char s1[200];
	sprintf(s1,"peak=%d, width=%4.2f",max_peak, peak_width);
	s=s+s1;
	MessageBox(0, s, "peak: ", MB_OK);
	//------------------------------------
	
	max_peak*=(65536/NC);
	m_fore=max_peak;
	peak_width*=(65536/NC);
	peak_width*=1.0;
	m_fore_width=int(peak_width);
	
	double ratio=0.5;
	if(m_fore_width+m_back_width!=0)
		ratio=double(m_fore_width)/(m_fore_width+m_back_width);
	double ratio2=
		double(m_fore_width+m_back_width)/
		max(abs(m_fore-m_back),1)*2;
	//ratio2<<1 - отдельные пики, можно делить пополам;
	//иначе пропорционально
	ratio2=min(ratio2,1);
	//ratio=ratio*ratio2+0.5*(1-ratio2);
	
	int th1=max(m_fore-m_fore_width,m_back);
	int th2=min(m_back+m_back_width,m_fore);
	
	//int threshold=int(m_back*ratio+m_fore*(1-ratio));
	int threshold=int((th1+th2)/2);
	
	//сегментация
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			color c=src[y][x];
			if(c>threshold)
				bin[y][x]=0;
			else
				bin[y][x]=255;
		}
	}
}

void CChromSegm::SecondPass2(color **src, byte **bin, int cx, int cy)
//второй проход сегментации, вычисление цвета объектов (максимальный пик)
//цвет объектов варьируется от места к месту
{
	//пересчитаем фон
	{
		double sum0=0, sum1=0;
		double sumb0=1, sumb1=m_back;
		for(int yy=0; yy<cy; yy++)
		{
			for(int xx=0; xx<cx; xx++)
			{
				if(bin[yy][xx])
				{
					sum0+=1;
					sum1+=src[yy][xx];
				}
				else
				{
					sumb0+=1;
					sumb1+=src[yy][xx];
				}
			}
		}
		sumb0=max(sumb0,1);
		m_back=int(sumb1/sumb0);
		sum0=max(sum0,1);
		m_fore=int(sum1/sum0);
	}
	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"avg fore=%4.2f, back=%4.2f",
			m_fore/256.0, m_back/256.0);
		s=s+s1;
		MessageBox(0, s, "Pass 2 start: ", MB_OK);
	}
	
	
	double foresum=0,forecount=0;
	double backsum=0,backcount=0;
	int mask_size=20;
	//сегментация
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			int x0=max(x-mask_size,0);
			int y0=max(y-mask_size,0);
			int x1=min(x+mask_size+1,cx);
			int y1=min(y+mask_size+1,cy);
			if(bin[y][x])
			{
				//calculate object's color
				double sum0=0, sum1=0;
				double sumb0=1, sumb1=m_back;
				for(int yy=y0; yy<y1; yy++)
				{
					for(int xx=x0; xx<x1; xx++)
					{
						if(bin[yy][xx])
						{
							double w=abs(src[yy][xx]-m_back);
							w*=w;
							sum0+=w;
							sum1+=w*src[yy][xx];
						}
						else
						{
							double w=abs(src[yy][xx]-m_back);
							sumb0+=1;
							sumb1+=src[yy][xx];
						}
					}
				}
				double fore=sum1/sum0;
				double back=sumb1/sumb0;
				back=(back*4+m_back*0)/4;
				fore=(fore*3+m_fore*1)/4;
				int th=abs(int(fore-back))/2;
				if(fabs(src[y][x]-back)<th) bin[y][x]=0;
				else bin[y][x]=255;
				foresum+=fore; forecount++;
				backsum+=back; backcount++;
			}
		}
	}
	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"avg fore=%4.2f, back=%4.2f",
			foresum/forecount/256,
			backsum/backcount/256);
		s=s+s1;
		MessageBox(0, s, "Pass 2: ", MB_OK);
	}
}

void CChromSegm::SecondPass3(color **src, byte **bin, int cx, int cy, int sqr_size)
//второй проход сегментации, вычисление цвета объектов (максимальный пик)
//цвет объектов варьируется от места к месту
{
	//вычислим ratio
	
	double ratio;
	{
		//поиск пика гистограммы (фон) и его ширины
		smart_alloc(hist, int, NC);
		CalcHistMasked(src, bin, 255, cx, cy, hist);
		int max_peak=0;
		int back=m_back/(65536/NC);
		for(int i=0; i<NC; i++)
		{
			hist[i]*=abs(i-back);
			hist[i]*=abs(i-back);
			if(hist[i]>hist[max_peak]) max_peak=i;
		}
		
		double peak_count1=0, peak_count0=0;
		for(i=0; i<NC; i++)
		{
			peak_count0+=hist[i];
			peak_count1+=double(hist[i])*abs(i-max_peak);
		}
		double peak_width=peak_count1/max(peak_count0,1);
		
		{_bstr_t s("");
		char s1[200];
		sprintf(s1,"peak=%d, width=%4.2f",max_peak, peak_width);
		s=s+s1;
		MessageBox(0, s, "peak: ", MB_OK);}
		//------------------------------------
		
		max_peak*=(65536/NC);
		m_fore=max_peak;
		peak_width*=(65536/NC);
		peak_width*=1.0;
		m_fore_width=int(peak_width);
		
		ratio=0.5;
		if(m_fore_width+m_back_width!=0)
			ratio=double(m_fore_width)/(m_fore_width+m_back_width);
		double ratio2=
			double(m_fore_width+m_back_width)/
			max(abs(m_fore-m_back),1)*2;
		//ratio2<<1 - отдельные пики, можно делить пополам;
		//иначе пропорционально
		ratio2=min(ratio2,1);
		//ratio=ratio*ratio2+0.5*(1-ratio2);
		ratio=0.5;
	}
	{_bstr_t s("");
	char s1[200];
	sprintf(s1,"ratio=%1.3f", ratio);
	s=s+s1;
	//MessageBox(0, s, "SecondPass3: ", MB_OK);
	}
	
	smart_alloc(hist, int, NC); //гистограмма - для работы
	
	int nx=cx*2/sqr_size;	//вначале работаем с квадратами размера
	if(nx<1) nx=1;			//SqrSize/2
	int ny=cy*2/sqr_size;
	if(ny<1) ny=1;
	
	// координаты углов квадратов
	smart_alloc(sqr_x, int, nx+1);
	{for(int ix=0; ix<=nx; ix++) sqr_x[ix]=cx*ix/nx;}
	
	// координаты углов квадратов
	smart_alloc(sqr_y, int, ny+1);
	{for(int iy=0; iy<=ny; iy++) sqr_y[iy]=cy*iy/ny;}
	
	//уменьшенные изображения:
	
	//размеры - с запасом (на 1 квадрат за пределы)
	int wx=nx+2, wy=ny+2;
	
	smart_alloc(fore, int, wx*wy);
	smart_alloc(back, int, wx*wy);
	smart_alloc(fore_num, int, wx*wy);
	smart_alloc(back_num, int, wx*wy);
	
	//вычисление fore/back по квадратам
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			//построение гистограммы цветов для определения медианы
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];
			
			int fore_sum=0, back_sum=0;
			int fore_cnt=0, back_cnt=0;
			
			for(int y=y0; y<y1; y++) 
			{
				for(int x=x0; x<x1; x++)
				{
					int c=src[y][x];
					if(bin[y][x])
					{
						fore_sum+=c;
						fore_cnt++;
					}
					else
					{
						back_sum+=c;
						back_cnt++;
					}
					
				}
			}
			
			//коррекция для объектов - откинуть точки, близкие к фону)
			/*
			if(back_cnt>0)
			{
			fore_sum=0; fore_cnt=0;
			int back=back_sum/back_cnt;
			for(int y=y0; y<y1; y++) 
			{
			for(int x=x0; x<x1; x++)
			{
			int c=src[y][x];
			int w=abs(src[y][x]-back)/256;
			if(bin[y][x])
			{
			fore_sum+=c*w/256;
			fore_cnt+=w;
			}
			}
			}
			fore_cnt/=256;
			}
			*/
			
			back_num[(iy+1)*wx+ix+1]=back_cnt;
			back[(iy+1)*wx+ix+1] = back_sum;
			fore_num[(iy+1)*wx+ix+1]=fore_cnt;
			fore[(iy+1)*wx+ix+1] = fore_sum;
			
			//маленькая гистограммка
			if(back_cnt>0 && fore_cnt>0)
			{
				CalcHistMaskedSqr(src, bin, 255, cx, cy, hist, x0,y0,x1,y1);
				int max_peak=0;
				int b=m_back;
				if(back_cnt>0) b=back_sum/back_cnt;
				b/=(65536/NC);
				for(int i=0; i<NC; i++)
				{
					hist[i]*=abs(i-b);
					hist[i]*=abs(i-b);
					if(hist[i]>hist[max_peak]) max_peak=i;
				}
				if(hist[max_peak]) fore[(iy+1)*wx+ix+1]=max_peak*(65536/NC)*fore_cnt;
			}
		}
	}}
	
	//заполнение полей (квадраты за пределами image)
	{for(int ix=1; ix<=nx; ix++)
	{
		fore[0*wx+ix]=fore[1*wx+ix];
		fore[(ny+1)*wx+ix]=fore[ny*wx+ix];
		back[0*wx+ix]=back[1*wx+ix];
		back[(ny+1)*wx+ix]=back[ny*wx+ix];
		fore_num[0*wx+ix]=fore_num[1*wx+ix];
		fore_num[(ny+1)*wx+ix]=fore_num[ny*wx+ix];
		back_num[0*wx+ix]=back_num[1*wx+ix];
		back_num[(ny+1)*wx+ix]=back_num[ny*wx+ix];
	}}
	{for(int iy=0; iy<=ny+1; iy++)
	{
		fore[iy*wx+0]=fore[iy*wx+1];
		fore[iy*wx+nx+1]=fore[iy*wx+nx];
		back[iy*wx+0]=back[iy*wx+1];
		back[iy*wx+nx+1]=back[iy*wx+nx];
		fore_num[iy*wx+0]=fore_num[iy*wx+1];
		fore_num[iy*wx+nx+1]=fore_num[iy*wx+nx];
		back_num[iy*wx+0]=back_num[iy*wx+1];
		back_num[iy*wx+nx+1]=back_num[iy*wx+nx];
	}}
	
	// перейти от квадратов размера SqrSize/2 к пересекающимся
	// квадратам размара SqrSize
	{for(int iy=0; iy<=ny; iy++)
	{
		int n=iy*wx;
		for(int ix=0; ix<=nx; ix++,n++)
		{
			fore[n]=
				fore[n]+fore[n+wx]+
				fore[n+1]+fore[n+wx+1];
			fore_num[n]=
				fore_num[n]+fore_num[n+wx]+
				fore_num[n+1]+fore_num[n+wx+1];
			if(fore_num[n]) fore[n]/=fore_num[n];
			else fore[n]=32768;
			
			back[n]=
				back[n]+back[n+wx]+
				back[n+1]+back[n+wx+1];
			back_num[n]=
				back_num[n]+back_num[n+wx]+
				back_num[n+1]+back_num[n+wx+1];
			if(back_num[n]) back[n]/=back_num[n];
			else back[n]=32768;
		}
	}}
	
	//заполнение квадратов, где нет фона или объектов
	//пока кроме наружних...
	{for(int iy=1; iy<ny; iy++)
	{
		int n=iy*wx+1;
		for(int ix=1; ix<nx; ix++,n++)
		{
			if(back_num[n]==0)
			{
				if(back_num[n+1]) back[n]=back[n+1];
				else if(back_num[n+wx]) back[n]=back[n+wx];
				else if(back_num[n-wx]) back[n]=back[n-wx];
				else back[n]=back[n-1];
			}
			if(fore_num[n]==0)
			{
				if(fore_num[n+1]) fore[n]=fore[n+1];
				else if(fore_num[n+wx]) fore[n]=fore[n+wx];
				else if(fore_num[n-wx]) fore[n]=fore[n-wx];
				else fore[n]=fore[n-1];
			}
		}
	}}
	
	// интерполяция цветов фона и объектов по всему
	// изображению,
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];
			
			double b0=back[iy*wx+ix];
			double b1=back[iy*wx+ix+1];
			double db0_dy=double(back[(iy+1)*wx+ix]-b0)/(sqr_y[iy+1]-sqr_y[iy]);
			double db1_dy=double(back[(iy+1)*wx+ix+1]-b1)/(sqr_y[iy+1]-sqr_y[iy]);
			
			double f0=fore[iy*wx+ix];
			double f1=fore[iy*wx+ix+1];
			double df0_dy=double(fore[(iy+1)*wx+ix]-f0)/(sqr_y[iy+1]-sqr_y[iy]);
			double df1_dy=double(fore[(iy+1)*wx+ix+1]-f1)/(sqr_y[iy+1]-sqr_y[iy]);
			
			for(int y=y0; y<y1; y++) 
			{
				int b=int(b0*256);
				int db_dx=int((b1-b0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				b0+=db0_dy;
				b1+=db1_dy;
				
				int f=int(f0*256);
				int df_dx=int((f1-f0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				f0+=df0_dy;
				f1+=df1_dy;
				
				color *src1=src[y]+x0;
				byte  *bin1=bin[y]+x0;
				for(int x=x0; x<x1; x++)
				{
					int c=*src1;
					//if(abs(c-f/256)<abs(c-b/256)) *bin1=255;
					if(abs(c-f/256)*(1-ratio)<abs(c-b/256)*ratio) *bin1=255;
					else *bin1=0;
					//*src1=b/256;
					
					src1++; bin1++;
					
					b+=db_dx;
					f+=df_dx;
				}
			}
		}
		Notify( sqr_y[iy] );
	}}
	NextNotificationStage( );
}

static void EnhanceBin3(color **src, byte **bin_arg, byte **bin_res, int cx, int cy, int sqr_size, double threshold_ratio)
//второй проход сегментации - улучшение результата
//цвет объектов варьируется от места к месту
{
	smart_alloc(hist, int, NC); //гистограмма - для работы
	
	int nx=cx*2/sqr_size;	//вначале работаем с квадратами размера
	if(nx<1) nx=1;			//SqrSize/2
	int ny=cy*2/sqr_size;
	if(ny<1) ny=1;
	
	// координаты углов квадратов
	smart_alloc(sqr_x, int, nx+1);
	{for(int ix=0; ix<=nx; ix++) sqr_x[ix]=cx*ix/nx;}
	
	// координаты углов квадратов
	smart_alloc(sqr_y, int, ny+1);
	{for(int iy=0; iy<=ny; iy++) sqr_y[iy]=cy*iy/ny;}
	
	//уменьшенные изображения:
	
	//размеры - с запасом (на 1 квадрат за пределы)
	int wx=nx+2, wy=ny+2;
	
	smart_alloc(fore, int, wx*wy);
	smart_alloc(back, int, wx*wy);
	smart_alloc(fore_num, int, wx*wy);
	smart_alloc(back_num, int, wx*wy);
	smart_alloc(thre, int, wx*wy); //threshold
	
	double back_sum_total=0, fore_sum_total=0;
	int back_cnt_total=0, fore_cnt_total=0;
	//вычисление fore/back по квадратам
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			//построение гистограммы цветов для определения медианы
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];
			
			int fore_sum=0, back_sum=0;
			int fore_cnt=0, back_cnt=0;
			double fore_sum2=0, back_sum2=0;
			
			for(int y=y0; y<y1; y++) 
			{
				for(int x=x0; x<x1; x++)
				{
					int c=src[y][x];
					if(bin_arg[y][x])
					{
						fore_sum+=c;
						fore_sum2+=double(c)*c;
						fore_cnt++;
					}
					else
					{
						back_sum+=c;
						back_sum2+=double(c)*c;
						back_cnt++;
					}
					
				}
			}
			fore_sum_total+=fore_sum;
			back_sum_total+=back_sum;
			fore_cnt_total+=fore_cnt;
			back_cnt_total+=back_cnt;
			
			
			back_num[(iy+1)*wx+ix+1]=back_cnt;
			back[(iy+1)*wx+ix+1] = back_sum;
			fore_num[(iy+1)*wx+ix+1]=fore_cnt;
			fore[(iy+1)*wx+ix+1] = fore_sum;
			double b=double(back_sum)/max(back_cnt,1);
			back_sum2=back_sum2/max(back_cnt,1)-b*b;
			back_sum2=sqrt(back_sum2);
			double f=double(fore_sum)/max(fore_cnt,1);
			fore_sum2=fore_sum2/max(fore_cnt,1)-f*f;
			fore_sum2=sqrt(fore_sum2);
			double sum2=fore_sum2+back_sum2;
			double ratio=0.5;
			if(sum2>0) ratio=fore_sum2/sum2;
			if(ratio<0.5) ratio=0.5;
			if(ratio>1.0) ratio=1.0;
			ratio=threshold_ratio;
			
			thre[(iy+1)*wx+ix+1]=int(ratio*256); //временно в thre - ratio
			
			
			//маленькая гистограммка для фона
			if(back_cnt>0 && fore_cnt>0)
			{
				CalcHistMaskedSqr(src, bin_arg, 0, cx, cy, hist, x0,y0,x1,y1);
				int max_peak=0;
				for(int i=0; i<NC; i++)
				{
					if(hist[i]>hist[max_peak]) max_peak=i;
				}
				int sum0=0, sum1=0;
				for(i=0; i<NC; i++)
				{
					sum0+=hist[i];
					sum1+=hist[i]*abs(i-max_peak);
				}
				//if(hist[max_peak]) back[(iy+1)*wx+ix+1]=max_peak*(65536/NC)*back_cnt;
			}
			
			//маленькая гистограммка
			if(back_cnt>0 && fore_cnt>0)
			{
				CalcHistMaskedSqr(src, bin_arg, 255, cx, cy, hist, x0,y0,x1,y1);
				int max_peak=0;
				int b=0;
				if(back_cnt>0) b=back_sum/back_cnt;
				b/=(65536/NC);
				for(int i=0; i<NC; i++)
				{
					hist[i]*=abs(i-b);
					hist[i]*=abs(i-b);
					if(hist[i]>hist[max_peak]) max_peak=i;
				}
				//if(hist[max_peak]) fore[(iy+1)*wx+ix+1]=max_peak*(65536/NC)*fore_cnt;
			}
		}
	}}
	
	int fore_avg=int(fore_sum_total/max(fore_cnt_total,1));
	int back_avg=int(back_sum_total/max(back_cnt_total,1));
	
	//заполнение полей (квадраты за пределами image)
	{for(int ix=1; ix<=nx; ix++)
	{
		fore[0*wx+ix]=fore[1*wx+ix];
		fore[(ny+1)*wx+ix]=fore[ny*wx+ix];
		back[0*wx+ix]=back[1*wx+ix];
		back[(ny+1)*wx+ix]=back[ny*wx+ix];
		fore_num[0*wx+ix]=fore_num[1*wx+ix];
		fore_num[(ny+1)*wx+ix]=fore_num[ny*wx+ix];
		back_num[0*wx+ix]=back_num[1*wx+ix];
		back_num[(ny+1)*wx+ix]=back_num[ny*wx+ix];
		thre[0*wx+ix]=thre[1*wx+ix];
		thre[(ny+1)*wx+ix]=thre[ny*wx+ix];
	}}
	{for(int iy=0; iy<=ny+1; iy++)
	{
		fore[iy*wx+0]=fore[iy*wx+1];
		fore[iy*wx+nx+1]=fore[iy*wx+nx];
		back[iy*wx+0]=back[iy*wx+1];
		back[iy*wx+nx+1]=back[iy*wx+nx];
		fore_num[iy*wx+0]=fore_num[iy*wx+1];
		fore_num[iy*wx+nx+1]=fore_num[iy*wx+nx];
		back_num[iy*wx+0]=back_num[iy*wx+1];
		back_num[iy*wx+nx+1]=back_num[iy*wx+nx];
		thre[iy*wx+0]=thre[iy*wx+1];
		thre[iy*wx+nx+1]=thre[iy*wx+nx];
	}}
	
	double fmb=0, fmb_cnt=0;
	// перейти от квадратов размера SqrSize/2 к пересекающимся
	// квадратам размара SqrSize
	{for(int iy=0; iy<=ny; iy++)
	{
		for(int ix=0; ix<=nx; ix++)
		{
			int n=iy*wx+ix;
			fore[n]=
				fore[n]+fore[n+wx]+
				fore[n+1]+fore[n+wx+1];
			fore_num[n]=
				fore_num[n]+fore_num[n+wx]+
				fore_num[n+1]+fore_num[n+wx+1];
			if(fore_num[n]) fore[n]/=fore_num[n];
			else fore[n]=32768;
			
			back[n]=
				back[n]+back[n+wx]+
				back[n+1]+back[n+wx+1];
			back_num[n]=
				back_num[n]+back_num[n+wx]+
				back_num[n+1]+back_num[n+wx+1];
			if(back_num[n]) back[n]/=back_num[n];
			else back[n]=32768;
			
			thre[n]=
				(thre[n]+thre[n+wx]+
				thre[n+1]+thre[n+wx+1])/4;
			
			if(fore_num[n] && back_num[n])
			{
				fmb+=(fore[n]-back[n]);
				fmb_cnt++;
			}
			
		}
	}}
	fmb/=max(fmb_cnt,1); //<fore-back>
	int xor_val=(fmb>0)?0:255;
	int ifmb=int(fmb);
	
	//заполнение квадратов, где нет фона или объектов, вычисление threshold
	{for(int iy=0; iy<wy; iy++)
	{
		int n=iy*wx;
		for(int ix=0; ix<wx; ix++,n++)
		{
			if(back_num[n]==0)
				back[n]=fore[n]-ifmb;
			if(fore_num[n]==0) fore[n]=back[n]+ifmb;
			if(abs(fore[n]-back[n])<abs(ifmb)/2)
				fore[n]=back[n]+ifmb/2;
			//fore[n]=back[n]+fmb;
			thre[n]=(back[n]*thre[n]+fore[n]*(256-thre[n]))/256;
		}
	}}
	
	// интерполяция thre по всему изображению,
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];
			
			double t0=thre[iy*wx+ix];
			double t1=thre[iy*wx+ix+1];
			double dt0_dy=double(thre[(iy+1)*wx+ix]-t0)/(sqr_y[iy+1]-sqr_y[iy]);
			double dt1_dy=double(thre[(iy+1)*wx+ix+1]-t1)/(sqr_y[iy+1]-sqr_y[iy]);
			
			for(int y=y0; y<y1; y++) 
			{
				int t=int(t0*256);
				int dt_dx=int((t1-t0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				t0+=dt0_dy;
				t1+=dt1_dy;
				
				color *src1=src[y]+x0;
				byte  *bin1=bin_res[y]+x0;
				byte  *bin0=bin_arg[y]+x0;
				for(int x=x0; x<x1; x++)
				{
					int c=*src1;
					register int out=(c<t/256)?0:255;
					//*bin1=out^xor_val;
					if(out^xor_val)
					{
						if(*bin0) *bin1=*bin0;
						else *bin1=255;
					}

					//*src1=b/256;
					//*src1=f/256;
					
					src1++; bin1++; bin0++;
					
					t+=dt_dx;
				}
			}
		}
	}}
}


bool CChromSegm::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBin( GetDataResult("Binary") );
	bool invert_ratio = GetArgLong("InvertRatio")!=0;
	long lPaneNum = GetArgLong("PaneNum");
	if (GetValueInt(GetAppUnknown(), "\\SegmSource", "SavePaneNum", 1))
		SetValue(GetAppUnknown(), "\\SegmSource", "PaneNum", lPaneNum);
	
	if( pArg==NULL || pBin==NULL )
		return false;
	
	LoadIni();
	int nPaneCount;
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	
	nPaneCount = ::GetImagePaneCount( pArg );
	lPaneNum = max(0,min(nPaneCount-1,lPaneNum));

	StartNotification( size.cy, nPaneCount );
	
	pBin->CreateNew(size.cx, size.cy);
	pBin->SetOffset( ptOffset, TRUE);
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
			Notify(y);
		}
		NextNotificationStage();
	}
	
	smart_alloc(binRows, byte *, size.cy);
	for(int y=0; y<size.cy; y++)
	{
		pBin->GetRow(&binRows[y], y, false);
	}

	if(0) FirstPass(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy, 3);
	if(1) FirstPass2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy, invert_ratio);
	if(0) SecondPass(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
	if(0) SecondPass2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
	
	if(0) SecondPass3(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy, 64);
	if(0) SecondPass3(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy, 8);
	
	if(0)
	{
		RefineBin(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
		RefineBin(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
	}
	
	if(0)
	{
		RefineBin2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
		RefineBin2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
		RefineBin2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
		RefineBin2(srcRows+lPaneNum*size.cy, binRows, size.cx, size.cy);
	}
	
	
	FinishNotification();
	
	return true;
}


//-------------------------------------------------
//ChromSegmSecond - улучшалка выделения
_ainfo_base::arg CChromSegmSecondInfo::s_pargs[] =
{
	{"SquareSize", szArgumentTypeInt, "64", true, false }, //размер квадратов для интерполяции threshold
	{"ThresholdRatio", szArgumentTypeDouble, "0.5", true, false }, //<0.5 - объекты мельче, >0.5 - крупнее
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, true, true },
	{"Refined",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


CChromSegmSecond::CChromSegmSecond()
{}

CChromSegmSecond::~CChromSegmSecond()
{}

bool CChromSegmSecond::CanDeleteArgument( CFilterArgument *pa )
{
	if (pa->m_bstrArgName==_bstr_t("Img")) return(false);
	else return CFilter::CanDeleteArgument(pa);
};

bool CChromSegmSecond::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBinArg( GetDataArgument("Binary") );
	IBinaryImagePtr pBinRes( GetDataResult("Refined") );
	int square_size=GetArgLong( "SquareSize" );
	if(square_size<4) square_size=4;
	double threshold_ratio=GetArgDouble( "ThresholdRatio" );
	if(threshold_ratio<0.01) threshold_ratio=0.01;
	if(threshold_ratio>0.99) threshold_ratio=0.99;
	long lPaneNum = GetValueInt(GetAppUnknown(), "\\SegmSource", "PaneNum", 0);
	
	if( pArg==NULL || pBinArg==NULL || pBinRes==NULL)
		return false;

	/*
	char sz_buf[255];
	sprintf( sz_buf, "\nCChromSegmSecond::InvokeFilter -> %f", threshold_ratio );
	trace( sz_buf );
	*/	
	
	LoadIni();
	int nPaneCount = ::GetImagePaneCount( pArg );
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	
	StartNotification( size.cy, nPaneCount );
	
	pBinRes->CreateNew(size.cx, size.cy);
	pBinRes->SetOffset( ptOffset, TRUE);
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}
	
	smart_alloc(binRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pBinArg->GetRow(&binRows[y], y, false);
	}}
	
	smart_alloc(dstRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pBinRes->GetRow(&dstRows[y], y, false);
	}}
	
	EnhanceBin3(srcRows+lPaneNum*size.cy, binRows, dstRows, size.cx, size.cy, square_size, threshold_ratio);
	
	FinishNotification();

	/*
	sprintf( sz_buf, "\nCChromSegmSecond::DoneFilter -> %f", threshold_ratio );
	trace( sz_buf );
	*/
	

	return true;
}

//-------------------------------------------------
//StrangeAverage - осреднение по точкам, равноудаленным от границ
_ainfo_base::arg CStrangeAverageInfo::s_pargs[] =
{
	{"MaskSize", szArgumentTypeInt, "10", true, false }, //размер квадратов для подсчета расстояния от границы
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, true, true },
	{"StrangeAverage",    szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};


CStrangeAverage::CStrangeAverage()
{}

CStrangeAverage::~CStrangeAverage()
{}

bool CStrangeAverage::CanDeleteArgument( CFilterArgument *pa )
{
	return(false);
};

bool CStrangeAverage::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBinArg( GetDataArgument("Binary") );
	int mask_size=GetArgLong( "MaskSize" );
	if(mask_size<1) mask_size=1;
	
	if( pArg==NULL || pBinArg==NULL)
		return false;

	IImage3Ptr pRes( GetDataResult("StrangeAverage") );

	if(pRes==NULL) return false;
	
	LoadIni();
	int nPaneCount;
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	int cx=size.cx, cy=size.cy;
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	
	nPaneCount = ::GetImagePaneCount( pArg );
	
	StartNotification( size.cy, nPaneCount );
	
	{	
		ICompatibleObjectPtr ptrCO( pRes );
		
		if( ptrCO == NULL )
			return false;
		
		if( S_OK != ptrCO->CreateCompatibleObject( pArg, NULL, 0 ) )
		{			
			return false;
		}		
	}
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}}
	
	smart_alloc(binRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pBinArg->GetRow(&binRows[y], y, false);
	}}
	
	smart_alloc(dstRows, color *, size.cy*nPaneCount);
	{
		for(int nPane=0;nPane<nPaneCount;nPane++ )
		{
			for(int y=0; y<size.cy; y++)
			{
				pRes->GetRow(y, nPane, &dstRows[nPane*size.cy+y]);
			}
		}
	}
	
	smart_alloc(x0, int, size.cx);
	smart_alloc(x1, int, size.cx);
	smart_alloc(y0, int, size.cy);
	smart_alloc(y1, int, size.cy);
	{for(int i=0; i<cx; i++) x0[i]=max(i-mask_size,0);}
	{for(int i=0; i<cx; i++) x1[i]=min(i+mask_size,cx-1);}
	{for(int i=0; i<cy; i++) y0[i]=max(i-mask_size,0);}
	{for(int i=0; i<cy; i++) y1[i]=min(i+mask_size,cy-1);}
	
	smart_alloc(cnt, int, cx*cy);
	smart_alloc(cntRows, int*, size.cy);
	{for(int y=0; y<size.cy; y++) cntRows[y]=cnt+y*cx;}

	
	/*
	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=0;
			{for(int yy=y0[y]; yy<=y1[y]; yy++)
			{
				for(int xx=x0[x]; xx<x1[x]; xx++)
				{
					if(binRows[yy][xx]) c++; 
				}
			}}
			cntRows[y][x]=c;
		}
	}}
	*/

	{for(int i=0; i<cx*cy; i++) cnt[i]=255;}
	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			if(binRows[y][x]!=binRows[y-1][x]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x]+1);
			if(binRows[y][x]!=binRows[y][x-1]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y][x-1]+1);
			//if(binRows[y][x]==binRows[y-1][x-1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x-1]+1);
			//if(binRows[y][x]==binRows[y-1][x+1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x+1]+1);
		}
	}}
	{for(int y=cy-2; y>=1; y--)
	{
		for(int x=cx-2; x>=1; x--)
		{
			if(binRows[y][x]!=binRows[y+1][x]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y+1][x]+1);
			if(binRows[y][x]!=binRows[y][x+1]) cntRows[y][x]=1;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y][x+1]+1);
			//if(binRows[y][x]==binRows[y+1][x-1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x-1]+1);
			//if(binRows[y][x]==binRows[y+1][x+1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x+1]+1);
		}
	}}
	
	//int max_cnt=(mask_size*2+1)*(mask_size*2+1);
	int max_dis=128; //максимальное измеряемое расстояние
	int max_cnt=max_dis*2+1;

	smart_alloc(c_avg, __int64, max_cnt+1);
	smart_alloc(c_cnt, int, max_cnt+1);
	smart_c_avg.zero();
	smart_c_cnt.zero();

	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=cntRows[y][x];
			c=min(c,max_dis);
			c=binRows[y][x]?max_dis-c:max_dis+1+c;
			c_avg[c]+=srcRows[y][x];
			c_cnt[c]++;
		}
	}}

	{for(int i=0; i<=max_cnt; i++) c_avg[i]/=max(c_cnt[i],1);}

	int c_avg_min=65535, c_avg_max=0;
	{for(int i=max_dis-mask_size; i<=max_dis+1+mask_size; i++)
	{
		if(c_cnt[i])
		{
			c_avg_min=int(min(c_avg_min,c_avg[i]));
			c_avg_max=int(max(c_avg_max,c_avg[i]));
		}
	}}

	int dilate_count=0;
	{
		int c0=(c_avg_min+c_avg_max)/2;
		int i0=max_dis; //по умолчанию - на середину
		for(int i=max_dis-mask_size; i<=max_dis+mask_size; i++)
		{
			if(c_cnt[i])
			{
				int c_min=int(min(c_avg[i],c_avg[i+1]));
				int c_max=int(max(c_avg[i],c_avg[i+1]));
				if(c0>=c_min && c0<=c_max) i0=i;
			}
		}
		dilate_count=i0-max_dis;
	}

	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"Erode/dilate %d times",dilate_count);
		s=s+s1;
		MessageBox(0, s, "Time: ", MB_OK);
	}


	{
		FILE *f=fopen("d:\\vt5\\ChromSegm.log","w");
		for(int i=0; i<=max_cnt; i++)
		{
			fprintf(f,"%5.1f\t%d\n",
				double(i-max_cnt/2.0),int(c_avg[i])/256);
		}
		fclose(f);
	}
	
	c_avg[0]=c_avg[1];
	c_avg[max_cnt]=c_avg[max_cnt-1];

	{
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int c=cntRows[y][x];
				c=min(c,max_dis);
				dstRows[y][x]=c*65535/max_dis;
			}
		}
	}
	
	FinishNotification();
	
	return true;
}

//-------------------------------------------------
//BoundDistance - подсчет расстояний до границы объекта
_ainfo_base::arg CBoundDistanceInfo::s_pargs[] =
{
	{"Binary",    szArgumentTypeBinaryImage, 0, true, true },
	{"BoundDistance",    szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};


CBoundDistance::CBoundDistance()
{}

CBoundDistance::~CBoundDistance()
{}

bool CBoundDistance::CanDeleteArgument( CFilterArgument *pa )
{
	return(false);
};

bool CBoundDistance::InvokeFilter()
{
	IBinaryImagePtr pArg( GetDataArgument("Binary") );
	IImage3Ptr pRes( GetDataResult("BoundDistance") );
	
	if( pArg==NULL || pRes==NULL)
		return false;
	
	SIZE size;
	pArg->GetSizes((int*)&size.cx,(int*)&size.cy);
	int cx=size.cx, cy=size.cy;
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	pRes->CreateImage(cx,cy,L"GRAY",-1);
	pRes->SetOffset(ptOffset,TRUE);
	
	StartNotification(size.cy, 1);
	
	smart_alloc(binRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pArg->GetRow(&binRows[y], y, false);
	}}
	
	smart_alloc(dstRows, color *, size.cy);
	{
		for(int y=0; y<size.cy; y++)
		{
			pRes->GetRow(y, 0, &dstRows[y]);
		}
	}

	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(binRows[y][x]!=0) dstRows[y][x]=0;
			else dstRows[y][x]=65535;
		}
	}}

	int m_size=8;
	smart_alloc(radbuf,int,(m_size*2+1)*(m_size*2+1));
	radbuf+=(m_size*2+1)*m_size+m_size;
	smart_alloc(rad,int*,(m_size*2+1));
	rad+=m_size;	//r указывает на центр квадрата
	{for(int i=-m_size; i<=m_size; i++) rad[i]=radbuf+(m_size*2+1)*i;}
	{for(int iy=-m_size; iy<=m_size; iy++)
	{
		for(int ix=-m_size; ix<=m_size; ix++)
		{
			rad[iy][ix]=int(ceil(_hypot(ix,iy)*16));
		}
	}}

	{for(int y=m_size; y<cy-m_size; y++)
	{
		for(int x=m_size; x<cx-m_size; x++)
		{
			int d=dstRows[y][x];
			for(int iy=-m_size; iy<=m_size; iy++)
			{
				for(int ix=-m_size; ix<=m_size; ix++)
				{
					int d1=dstRows[y+iy][x+ix]+rad[iy][ix];
					if(d1<d) d=d1;
				}
			}
			dstRows[y][x]=d;
		}
	}}

	{for(int y=cy-m_size-1; y>=m_size; y--)
	{
		for(int x=cx-m_size-1; x>=m_size; x--)
		{
			int d=dstRows[y][x];
			for(int iy=-m_size; iy<=m_size; iy++)
			{
				for(int ix=-m_size; ix<=m_size; ix++)
				{
					int d1=dstRows[y+iy][x+ix]+rad[iy][ix];
					if(d1<d) d=d1;
				}
			}
			dstRows[y][x]=d;
		}
	}}

	int max_dis=63*16;

	{
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int c=dstRows[y][x];
				c=min(c,max_dis);
				dstRows[y][x]=c*1024/16;//65535/max_dis;
			}
			Notify(y);
		}
	}
	
	FinishNotification();
	
	return true;
}


//-------------------------------------------------
//AutoAdjustBin - подстройка изображения на основе осреднения по точкам
_ainfo_base::arg CAutoAdjustBinInfo::s_pargs[] =
{
	{"MaskSize", szArgumentTypeInt, "10", true, false }, //размер квадратов для подсчета расстояния от границы
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, true, true },
	{"Adjusted",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


CAutoAdjustBin::CAutoAdjustBin()
{
}

CAutoAdjustBin::~CAutoAdjustBin()
{}

bool CAutoAdjustBin::CanDeleteArgument( CFilterArgument *pa )
{
	if (pa->m_bstrArgName==_bstr_t("Img")) return(false);
	else return CFilter::CanDeleteArgument(pa);
};

bool CAutoAdjustBin::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBinArg( GetDataArgument("Binary") );
	int mask_size=GetArgLong( "MaskSize" );
	if(mask_size<1) mask_size=1;
	
	if( pArg==NULL || pBinArg==NULL)
		return false;

	IBinaryImagePtr pBinRes( GetDataResult("Adjusted") );

	if(pBinRes==NULL) return false;
	
	LoadIni();
	int nPaneCount;
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	int cx=size.cx, cy=size.cy;
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	
	nPaneCount = ::GetImagePaneCount( pArg );
	
	StartNotification( size.cy, nPaneCount );
	

	pBinRes->CreateNew(size.cx, size.cy);
	pBinRes->SetOffset( ptOffset, TRUE);
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}}
	
	smart_alloc(binRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pBinArg->GetRow(&binRows[y], y, false);
	}}
	
	smart_alloc(dstBinRows, byte *, size.cy);

	{
		for(int y=0; y<size.cy; y++)
		{
			pBinRes->GetRow(&dstBinRows[y], y, false);
		}
	}
	
	smart_alloc(x0, int, size.cx);
	smart_alloc(x1, int, size.cx);
	smart_alloc(y0, int, size.cy);
	smart_alloc(y1, int, size.cy);
	{for(int i=0; i<cx; i++) x0[i]=max(i-mask_size,0);}
	{for(int i=0; i<cx; i++) x1[i]=min(i+mask_size,cx-1);}
	{for(int i=0; i<cy; i++) y0[i]=max(i-mask_size,0);}
	{for(int i=0; i<cy; i++) y1[i]=min(i+mask_size,cy-1);}
	
	smart_alloc(cnt, int, cx*cy);
	smart_alloc(cntRows, int*, size.cy);
	{for(int y=0; y<size.cy; y++) cntRows[y]=cnt+y*cx;}

	
	/*
	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=0;
			{for(int yy=y0[y]; yy<=y1[y]; yy++)
			{
				for(int xx=x0[x]; xx<x1[x]; xx++)
				{
					if(binRows[yy][xx]) c++; 
				}
			}}
			cntRows[y][x]=c;
		}
	}}
	*/

	{for(int i=0; i<cx*cy; i++) cnt[i]=255;}
	{for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			if(binRows[y][x]!=binRows[y-1][x]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x]+1);
			if(binRows[y][x]!=binRows[y][x-1]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y][x-1]+1);
			//if(binRows[y][x]==binRows[y-1][x-1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x-1]+1);
			//if(binRows[y][x]==binRows[y-1][x+1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x+1]+1);
		}
	}}
	{for(int y=cy-2; y>=1; y--)
	{
		for(int x=cx-2; x>=1; x--)
		{
			if(binRows[y][x]!=binRows[y+1][x]) cntRows[y][x]=0;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y+1][x]+1);
			if(binRows[y][x]!=binRows[y][x+1]) cntRows[y][x]=1;
			else cntRows[y][x]=min(cntRows[y][x],cntRows[y][x+1]+1);
			//if(binRows[y][x]==binRows[y+1][x-1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x-1]+1);
			//if(binRows[y][x]==binRows[y+1][x+1]) cntRows[y][x]=min(cntRows[y][x],cntRows[y-1][x+1]+1);
		}
	}}
	
	//int max_cnt=(mask_size*2+1)*(mask_size*2+1);
	int max_dis=128; //максимальное измеряемое расстояние
	int max_cnt=max_dis*2+1;

	smart_alloc(c_avg, __int64, max_cnt+1);
	smart_alloc(c_cnt, int, max_cnt+1);
	smart_c_avg.zero();
	smart_c_cnt.zero();

	{for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=cntRows[y][x];
			c=min(c,max_dis);
			c=binRows[y][x]?max_dis-c:max_dis+1+c;
			cntRows[y][x]=c;
			c_avg[c]+=srcRows[y][x];
			c_cnt[c]++;
		}
	}}

	{for(int i=0; i<=max_cnt; i++) c_avg[i]/=max(c_cnt[i],1);}

	int c_avg_min=65535, c_avg_max=0;
	{for(int i=max_dis-mask_size; i<=max_dis+1+mask_size; i++)
	{
		if(c_cnt[i])
		{
			c_avg_min=int(min(c_avg_min,c_avg[i]));
			c_avg_max=int(max(c_avg_max,c_avg[i]));
		}
	}}

	int dilate_count=0;
	{
		int c0=(c_avg_min+c_avg_max)/2;
		int i0=max_dis; //по умолчанию - на середину
		for(int i=max_dis-mask_size; i<=max_dis+mask_size; i++)
		{
			if(c_cnt[i])
			{
				int c_min=int(min(c_avg[i],c_avg[i+1]));
				int c_max=int(max(c_avg[i],c_avg[i+1]));
				if(c0>=c_min && c0<=c_max) i0=i;
			}
		}
		dilate_count=i0-max_dis;
	}

	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"Erode/dilate %d times",dilate_count);
		s=s+s1;
		MessageBox(0, s, "Time: ", MB_OK);
	}


	{
		FILE *f=fopen("d:\\vt5\\ChromSegm.log","w");
		for(int i=0; i<=max_cnt; i++)
		{
			fprintf(f,"%5.1f\t%d\n",
				double(i-max_cnt/2.0),int(c_avg[i])/256);
		}
		fclose(f);
	}
	
	c_avg[0]=c_avg[1];
	c_avg[max_cnt]=c_avg[max_cnt-1];

	{
		int th=max_dis+dilate_count;
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int c=cntRows[y][x];
				dstBinRows[y][x] = (c<=th) ? 255 : 0;
				//dstBinRows[y][x] = (x^y)&1;
			}
		}
	}

	if(1) BinMedian(dstBinRows, cx,cy);
	if(0) BinMedian2(dstBinRows, cx,cy);
	
	FinishNotification();
	
	return true;
}
