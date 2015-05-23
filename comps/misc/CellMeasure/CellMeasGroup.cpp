#include "stdafx.h"
#include "CellMeasure.h"
#include "CellMeasGroup.h"
#include "Params.h"
#include "units_const.h"
#include "alloc.h"
#include "windows.h"
#include "\vt5\common2\misc_calibr.h"
#include "MarkovParam.h"
#include "ContourApi.h"

#include <math.h>

IMPLEMENT_DYNCREATE(CCellMeasGroup, CMeasParamGroupBase)


CCellMeasGroup::CCellMeasGroup()
{
	_OleLockApp( this );

	m_sName = "CellMeas";
}

CCellMeasGroup::~CCellMeasGroup()
{
	_OleUnlockApp( this );
}

// {BB31C680-AD2E-11D5-96D6-008048FD42FE}	
IMPLEMENT_GROUP_OLECREATE(CCellMeasGroup, "CellMeasure.CellMeasGroup",
0xBB31C680, 0xAD2E, 0x11D5, 0x96, 0xD6, 0x00, 0x80, 0x48, 0xFD, 0x42, 0xFE );

/////////////////////////////////////////////////////////////////////////////
// CCellMeasGroup message handlers

static int CalcSegments(byte **buf, int cx, int cy, int nSegColor, int nMinSegArea)
{ //возвращает, сколько сегментов нашли
	if(cx<3 || cy<3) return(0); //на такой фигне делать нечего...

	smart_alloc(ind, byte, 256);
	for(int i=0; i<256; i++) ind[i]=i;
	//индексы для объединения нескольких областей в одну
	
	smart_alloc(cnt_buf, byte, cx*cy);
	smart_cnt_buf.zero();
	
	smart_alloc(cnt, byte*, cy);
	for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;
	
	int cur_ind;
	
	cnt[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1]) cur_ind++;
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
				if(cur_ind>255) cur_ind=255;
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
	
	smart_alloc(area, int, 256);
	smart_area.zero();
	
	smart_alloc(min_x, int, 256);
	smart_alloc(max_x, int, 256);
	smart_alloc(min_y, int, 256);
	smart_alloc(max_y, int, 256);
	for(i=cur_ind; i>=0; i--)
	{
		min_x[i]=cx; max_x[i]=0;
		min_y[i]=cy; max_y[i]=0;
	}
	

	//FILE *f=fopen("d:\\vt5\\out.log","w");
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			byte c=ind[cnt[y][x]];
			//fputc(c+'A',f);
			if(buf[y][x]==nSegColor) area[c]++;
			if(x<min_x[c]) min_x[c]=x;
			if(x>max_x[c]) max_x[c]=x;
			if(y<min_y[c]) min_y[c]=y;
			if(y>max_y[c]) max_y[c]=y;
		}
		//fputs("\n",f);
	}
	//fclose(f);
	
	int count=0;
	for(i=1; i<=cur_ind; i++)
	{
		if(area[i]>=nMinSegArea) count++;
	}
	
	//CString s;
	//s.Format("Areas: %d (%d)", count,cur_ind);
	//char s[200];
	//sprintf(s,"Areas: %d (%d)", count,cur_ind);
	//MessageBox(0,s,"FindCells/DeleteSmallSegments",0);
	
	return(count);
}

static double CalcSegments2(byte **buf, int cx, int cy, double* pS1=0, double* pS2=0)
{	// возвращает число, характеризующее тенденцию к распадению на сегменты (при сегментации по уровню nSegColor)
	// *pS1 = площадь сегментов
	// *pS2 = сумма квадратов площадей сегментов
	if(cx<3 || cy<3) return(0); //на такой фигне делать нечего...

	smart_alloc(ind, byte, 256);
	for(int i=0; i<256; i++) ind[i]=i;
	//индексы для объединения нескольких областей в одну
	
	smart_alloc(cnt_buf, byte, cx*cy);
	smart_cnt_buf.zero();
	
	smart_alloc(cnt, byte*, cy);
	for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;
	
	int cur_ind;
	
	cnt[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1]) cur_ind++;
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
				if(cur_ind>255) cur_ind=255;
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
	
	smart_alloc(area, int, 256);
	smart_area.zero();

	//FILE *f=fopen("d:\\vt5\\out.log","w");
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			byte c=ind[cnt[y][x]];
			//fputc(c+'A',f);
			if(buf[y][x]) area[c]++;
		}
		//fputs("\n",f);
	}
	//fclose(f);
	
	double sum=0, sum2=0;
	for(i=1; i<=cur_ind; i++)
	{
		double d=area[i];
		sum += d;
		sum2 += d*d;
	}
	double ratio = max(1,sum);
	ratio = sum2 / (ratio*ratio);
	
	if(pS1) *pS1 = sum;
	if(pS2) *pS2 = sum2;
	
	return(ratio);
}

static void ErodeSegments(byte **buf, int cx, int cy, int nSegColor)
{  // требует, чтобы край был =0
	if(cx<3 || cy<3) return; //на такой фигне делать нечего...

	for(int y=0; y<cy-1; y++)
	{
		for(int x=0; x<cx-1; x++)
		{
			if(buf[y][x]==nSegColor)
			{
				if(buf[y][x+1]!=nSegColor || buf[y+1][x]!=nSegColor)
					buf[y][x]=0;
			}
		}
	}

	for(int y=cy-1; y>0; y--)
	{
		for(int x=cx-1; x>0; x--)
		{
			if(buf[y][x]==nSegColor)
			{
				if(buf[y][x-1]!=nSegColor || buf[y-1][x]!=nSegColor)
					buf[y][x]=0;
			}
		}
	}
}

#define SET(key, value)	{if (ParamIsEnabledByKey(key)) ptrCalc->SetValue(key, value);}

static double sqr(double x) {return x*x;}
static double root3(double x) {return x>=0 ? pow(x,0.3333333333) : -pow(-x,0.3333333333);}

static double HollowFactor(color **ppc, byte **ppm, int cx, int cy, int nScale=65535)
{
	if(cx<3 || cy<3) return 0;

	// сформируем карту расстояний от края
	smart_alloc(ppd,int*,cy); //буфер 
	smart_alloc(pdbuf,int,cy*cx);
	smart_pdbuf.zero();
	for(int y=0; y<cy; y++) ppd[y]=pdbuf+y*cx;
	for(int y=0; y<cy; y++) 
	{
		ppd[y][0] = ppm[y][0]<128 ? 0 : 1;
		ppd[y][cx-1] = ppm[y][cx-1]<128 ? 0 : 1;
	}
	for(int x=0; x<cx; x++) 
	{
		ppd[0][x] = ppm[0][x]<128 ? 0 : 1;
		ppd[cy-1][x] = ppm[cy-1][x]<128 ? 0 : 1;
	}

	for(int y=1; y<cy-1; y++) 
	{
		for(int x=1; x<cx-1; x++) 
		{
			ppd[y][x] = ppm[y][x]<128 ? 0 : 65535;
			if(ppd[y][x]>ppd[y][x-1]+1) ppd[y][x]=ppd[y][x-1]+1;
			if(ppd[y][x]>ppd[y-1][x]+1) ppd[y][x]=ppd[y-1][x]+1;
		}
	}
	for(int y=cy-2; y>0; y--) 
	{
		for(int x=cx-2; x>0; x--) 
		{
			if(ppd[y][x]>ppd[y][x+1]+1) ppd[y][x]=ppd[y][x+1]+1;
			if(ppd[y][x]>ppd[y+1][x]+1) ppd[y][x]=ppd[y+1][x]+1;
		}
	}

	double sum0=0, sum1=0, cnt0=0, cnt1=0;
	int cmax=0, cmin=65535;
	for(int y=0; y<cy; y++) 
	{
		for(int x=0; x<cx; x++) 
		{
			if(ppm[y][x]>=128)
			{ 
				int c=ppc[y][x];
				if(c>cmax) cmax=c;
				if(c<cmin) cmin=c;
				sum0 += c;
				cnt0 += 1;
				sum1 += double(c)*ppd[y][x];
				cnt1 += ppd[y][x];
			}
		}
	}
	sum0 /= max(cnt0,1);
	sum1 /= max(cnt1,1);

	//return sum1-sum0;
	return (sum1-sum0)/max(nScale,256);
    //return (sum1-sum0)/max(1,cmax-cmin);
}

static void CalcMoments(color **ppc, byte **ppm, int cx, int cy, int nScale=65535,
						double *fMoment1=0, double *fMoment2=0, double *fMoment3=0 )
{
	if(cx<3 || cy<3) return;

	double sum1=0.0, sum2=0.0, sum3=0.0;
	int count=0;
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(ppm[y][x]>=128)
			{
				double c=ppc[y][x];
				count++;
				sum1 += c; sum2 += c*c; sum3 += c*c*c;
			};
		}
	}

	count=max(1,count);
	sum1/=count; sum2/=count; sum3/=count;

	sum2 -= sum1*sum1;
	sum3 -= sum1*sum1*sum1 + 3*sum1*sum2;

	nScale = max(256,nScale);
	sum1 /= nScale;
	sum2 /= nScale;
	sum3 /= nScale;

	if(fMoment1) *fMoment1=sum1;
	if(fMoment2) *fMoment2=sum2;
	if(fMoment3) *fMoment3=sum3;
}

class CParamRecord
{
public:
	long m_nKey;
	double *m_pfVal;
	bool m_bEnabled;
	ICalcObject	*m_pCalc;
	CParamRecord(int nKey, double *pfVal, bool bEnabled, ICalcObject *pCalc)
	{ m_nKey=nKey; m_pfVal=pfVal; m_bEnabled=bEnabled; m_pCalc=pCalc; }
	~CParamRecord()
	{
		if(m_bEnabled && m_pCalc) m_pCalc->SetValue(m_nKey, *m_pfVal);
	}
};

#define PARAM2(name,key) \
	double fVal##name = 0.0; \
	bool bEnabled##name = ParamIsEnabledByKey(key); \
	CParamRecord param##name(key, &fVal##name, bEnabled##name, ptrCalc);\
	bThereIsParams |= bEnabled##name;

#define PARAM(key) PARAM2(key, key)

bool bThereIsParams = false;

bool CCellMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	CImageWrp imageCalc = punkSource;
	double	fCalibr = 0.0025;
	GUID guidC;
	::GetDefaultCalibration( &fCalibr, &guidC );

	ICalcObjectPtr	ptrCalc = punkCalcObject;
	ICalibrPtr	ptrCalibr = imageCalc;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	fCalibr = max(fCalibr, 1e-10);

	if (imageCalc.IsEmpty())
	{
//		ptrCalc->ClearValues();
		return true;
	}

	int cx=imageCalc.GetWidth();
	int cy=imageCalc.GetHeight();

	smart_alloc(ppM,byte*,cy);
	smart_alloc(ppC0,color*,cy);
	smart_alloc(ppC1,color*,cy);
	smart_alloc(ppC2,color*,cy);

	for(int y=0; y<cy; y++)
	{
		ppC0[y]=imageCalc.GetRow(y,0);
		ppC1[y]=imageCalc.GetRow(y,1);
		ppC2[y]=imageCalc.GetRow(y,2);
		ppM[y]=imageCalc.GetRowMask(y);
	}

	smart_alloc(ppM2,byte*,cy+2);
	smart_alloc(pM2buf,byte,(cy+2)*(cx+2));
	smart_pM2buf.zero();
	for(int y=0; y<cy+2; y++) ppM2[y]=pM2buf+y*(cx+2);
	for(int y=0; y<cy; y++) memcpy(ppM2[y+1]+1,ppM[y],cx*sizeof(byte));
	ErodeSegments(ppM2, cx+2, cy+2, 254);
	ErodeSegments(ppM2, cx+2, cy+2, 255);

	///////////////////////////////////////////////////////////////////////
	// подсчет параметров - интегралов по площади
	int count[2]={0,0};
	double r1[2]={0.0,0.0}, g1[2]={0.0,0.0}, b1[2]={0.0,0.0}; // интеграл первой степени R,G,B
	double r2[2]={0.0,0.0}, g2[2]={0.0,0.0}, b2[2]={0.0,0.0};
	double r3[2]={0.0,0.0}, g3[2]={0.0,0.0}, b3[2]={0.0,0.0};
	double grad[2]={0.0,0.0};
	int grad_count[2]={0,0};
	double exp_sum0, exp_sum_r, exp_sum_b, exp_sum_g; // суммы для вычисления фона - светлого в квадрате клетки
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int k=-1; //kind
			if(ppM2[y+1][x+1]==254) k=0; // ядро
			if(ppM2[y+1][x+1]==255) k=1; // цитоплазма
			// (опять неоптимально)
         	if(k>=0)
			{
				count[k]++;
				double r=ppC2[y][x], g=ppC1[y][x], b=ppC0[y][x];
				r1[k] += r; g1[k] += g; b1[k] += b;
				r2[k] += r*r; g2[k] += g*g; b2[k] += b*b;
				r3[k] += r*r*r; g3[k] += g*g*g; b3[k] += b*b*b; // можно пооптимальнее
				if(x>0 && ppM2[y+1][x]==ppM2[y+1][x+1])
				{
					grad[k] += sqr(ppC1[y][x]-ppC1[y][x-1]);
					grad_count[k]++;
				}
				if(y>0 && ppM2[y][x+1]==ppM2[y+1][x+1])
				{
					grad[k] += sqr(ppC1[y][x]-ppC1[y-1][x]);
					grad_count[k]++;
				}
			}
			else
			{
				double r=ppC2[y][x], g=ppC1[y][x], b=ppC0[y][x];
				double w = exp(g/(256*5));
				exp_sum0 += w;
				exp_sum_r += r*w;
				exp_sum_g += g*w;
				exp_sum_b += b*w;
			}
		}
	}

	double s0=max(count[0],1);
	double s1=max(count[1],1);

	r1[0]/=s0; r2[0]/=s0; r3[0]/=s0;
	g1[0]/=s0; g2[0]/=s0; g3[0]/=s0;
	b1[0]/=s0; b2[0]/=s0; b3[0]/=s0;
	r2[0] -= r1[0]*r1[0]; r3[0] -= r1[0]*r1[0]*r1[0] + 3*r1[0]*r2[0];
	g2[0] -= g1[0]*g1[0]; g3[0] -= g1[0]*g1[0]*g1[0] + 3*g1[0]*g2[0];
	b2[0] -= b1[0]*b1[0]; b3[0] -= b1[0]*b1[0]*b1[0] + 3*b1[0]*r2[0];

	double fC0 = max(256,ppC1[0][1]); 
	// для калибровки будем делить на зеленую составляющую фона
	// (она заранее [FindCells] прописана в левый верхний угол квадрата)

	if(exp_sum0>=1)
		fC0 = exp_sum_g/exp_sum0;


	r1[0]/=fC0; r2[0]=sqrt(r2[0])/fC0; r3[0]=root3(r3[0])/fC0;
	g1[0]/=fC0; g2[0]=sqrt(g2[0])/fC0; g3[0]=root3(g3[0])/fC0;
	b1[0]/=fC0; b2[0]=sqrt(b2[0])/fC0; b3[0]=root3(b3[0])/fC0;

	r1[1]/=s1; r2[1]/=s1; r3[1]/=s1;
	g1[1]/=s1; g2[1]/=s1; g3[1]/=s1;
	b1[1]/=s1; b2[1]/=s1; b3[1]/=s1;
	r2[1] -= r1[1]*r1[1]; r3[1] -= r1[1]*r1[1]*r1[1] + 3*r1[1]*r2[1];
	g2[1] -= g1[1]*g1[1]; g3[1] -= g1[1]*g1[1]*g1[1] + 3*g1[1]*g2[1];
	b2[1] -= b1[1]*b1[1]; b3[1] -= b1[1]*b1[1]*b1[1] + 3*b1[1]*r2[1];

	r1[1]/=fC0; r2[1]=sqrt(r2[1])/fC0; r3[1]=root3(r3[1])/fC0;
	g1[1]/=fC0; g2[1]=sqrt(g2[1])/fC0; g3[1]=root3(g3[1])/fC0;
	b1[1]/=fC0; b2[1]=sqrt(b2[1])/fC0; b3[1]=root3(b3[1])/fC0;

	grad[0]/=max(grad_count[0],1); grad[0]=sqrt(grad[0]*2)/fC0;
	grad[1]/=max(grad_count[1],1); grad[1]=sqrt(grad[1]*2)/fC0;

	///////////////////////////////////////////////////////////////////////
	// подсчет параметров формы

	///////////////////////////////////////////////////////////////////////
	// подсчет параметров - интегралов по площади
	smart_pM2buf.zero();
	for(int y=0; y<cy+2; y++) ppM2[y]=pM2buf+y*(cx+2);
	for(int y=0; y<cy; y++) memcpy(ppM2[y+1]+1,ppM[y],cx*sizeof(byte));

	count[0]=count[1]=0;
	double mx[2]={0.0,0.0}, my[2]={0.0,0.0};
	double mxx[2]={0.0,0.0}, mxy[2]={0.0,0.0}, myy[2]={0.0,0.0};
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int k=-1; //kind
			if(ppM2[y+1][x+1]==254) k=0; // ядро
			if(ppM2[y+1][x+1]==255) k=1; // цитоплазма
			// (опять неоптимально)
         	if(k>=0)
			{
				count[k]++;
				mx[k] += x; my[k] += y;
				mxx[k] += x*x; mxy[k]+=x*y; myy[k]+=y*y;
			}
		}
	}

	s0=max(count[0],1);
	s1=max(count[1],1);

	double area=count[0]+count[1];

	mx[1]+=mx[0]; my[1]+=my[0];
	mxx[1]+=mxx[0]; mxy[1]+=mxy[0]; myy[1]+=myy[0];

	mx[0]/=s0; my[0]/=s0;
	mxx[0]/=s0; mxy[0]/=s0; myy[0]/=s0;
	mxx[0] -= mx[0]*mx[0]; mxy[0] -= mx[0]*my[0]; myy[0] -= my[0]*my[0];

	mx[1]/=(s0+s1); my[1]/=(s0+s1);
	mxx[1]/=(s0+s1); mxy[1]/=(s0+s1); myy[1]/=(s0+s1);
	mxx[1] -= mx[1]*mx[1]; mxy[1] -= mx[1]*my[1]; myy[1] -= my[1]*my[1];

	smart_pM2buf.zero();
	for(int y=0; y<cy+2; y++) ppM2[y]=pM2buf+y*(cx+2);
	for(int y=0; y<cy; y++) memcpy(ppM2[y+1]+1,ppM[y],cx*sizeof(byte));

	int nn[256];
	double per_nucl=0,per_cell=0;
	double dp[5]={0.0,0.7,1.0,0.7,0.0};
	for(int y=0; y<=cy; y++)
	{
		for(int x=0; x<=cx; x++)
		{
			nn[255]=0; nn[254]=0;
			nn[ppM2[y][x]]++;
			nn[ppM2[y][x+1]]++;
			nn[ppM2[y+1][x]]++;
			nn[ppM2[y+1][x+1]]++;
			per_nucl += dp[nn[254]];
			per_cell += dp[nn[255]+nn[254]];
		}
	}

	double d;
	d = max(per_cell,1);
	double circ_cell = area / (d*d) * 4 * 3.14159;
	d = max(per_nucl,1);
	double circ_nucl = count[0] / (d*d) * 4 * 3.14159;

	d = mxx[0]-myy[0]; d = d*d+sqr(mxy[0])*4;
	double ellipcity_nucl = d / max(1e-6, sqr(mxx[0]+myy[0]));
	d = mxx[1]-myy[1]; d = d*d+sqr(mxy[1])*4;
	double ellipcity_cell = d / max(1e-6, sqr(mxx[1]+myy[1]));


	ErodeSegments(ppM2, cx+2, cy+2, 254);
	int nSeg1=CalcSegments(ppM2, cx+2, cy+2, 254, 5);
	ErodeSegments(ppM2, cx+2, cy+2, 254);
	int nSeg2=CalcSegments(ppM2, cx+2, cy+2, 254, 5);
	ErodeSegments(ppM2, cx+2, cy+2, 254);
	int nSeg3=CalcSegments(ppM2, cx+2, cy+2, 254, 5);
	ErodeSegments(ppM2, cx+2, cy+2, 254);
	int nSeg4=CalcSegments(ppM2, cx+2, cy+2, 254, 5);
	ErodeSegments(ppM2, cx+2, cy+2, 254);
	int nSeg5=CalcSegments(ppM2, cx+2, cy+2, 254, 5);

	SET(KEY_CELLAREA, area*fCalibr*fCalibr);
	SET(KEY_CELLNUCLEUSAREA, count[0]*fCalibr*fCalibr);

	SET(KEY_CELLPERIMETER, per_cell*fCalibr);
	SET(KEY_CELLNUCLEUSPERIMETER, per_nucl*fCalibr);

	SET(KEY_CELLCIRCULARITY, circ_cell);
	SET(KEY_CELLNUCLEUSCIRCULARITY, circ_nucl);
	SET(KEY_CELLELLIPTICITY, ellipcity_cell);
	SET(KEY_CELLNUCLEUSELLIPTICITY, ellipcity_nucl);
	SET(KEY_CELLNUCLEUSAREAPERCENT, count[0]/max(1.0,count[0]+count[1]));

	SET(KEY_CELLNUCLEUSMOMENT1R,r1[0]);
	SET(KEY_CELLNUCLEUSMOMENT2R,r2[0]);
	SET(KEY_CELLNUCLEUSMOMENT3R,r3[0]);
	SET(KEY_CELLCITOPLASMAMOMENT1R,r1[1]);
	SET(KEY_CELLCITOPLASMAMOMENT2R,r2[1]);
	SET(KEY_CELLCITOPLASMAMOMENT3R,r3[1]);

	SET(KEY_CELLNUCLEUSMOMENT1,g1[0]);
	SET(KEY_CELLNUCLEUSMOMENT2,g2[0]);
	SET(KEY_CELLNUCLEUSMOMENT3,g3[0]);
	SET(KEY_CELLCITOPLASMAMOMENT1,g1[1]);
	SET(KEY_CELLCITOPLASMAMOMENT2,g2[1]);
	SET(KEY_CELLCITOPLASMAMOMENT3,g3[1]);

	SET(KEY_CELLNUCLEUSMOMENT1B,b1[0]);
	SET(KEY_CELLNUCLEUSMOMENT2B,b2[0]);
	SET(KEY_CELLNUCLEUSMOMENT3B,b3[0]);
	SET(KEY_CELLCITOPLASMAMOMENT1B,b1[1]);
	SET(KEY_CELLCITOPLASMAMOMENT2B,b2[1]);
	SET(KEY_CELLCITOPLASMAMOMENT3B,b3[1]);

	SET(KEY_CELLNUCLEUSGRADIENT,grad[0]/fCalibr);
	SET(KEY_CELLCITOPLASMAGRADIENT,grad[1]/fCalibr);

	SET(KEY_CELLNUCLEISEGMENTS1,nSeg1<=1?0:1);
	SET(KEY_CELLNUCLEISEGMENTS2,nSeg2<=1?0:1);
	SET(KEY_CELLNUCLEISEGMENTS3,nSeg3<=1?0:1);
	SET(KEY_CELLNUCLEISEGMENTS4,nSeg4<=1?0:1);
	SET(KEY_CELLNUCLEISEGMENTS5,nSeg5<=1?0:1);

	long lBackgr = GetValueInt(GetAppUnknown(),"\\ODensity", "Backgr",255)*256;
	if(ParamIsEnabledByKey(KEY_HOLLOWFACTOR))
	{
		double fHollowFactor=HollowFactor(ppC1, ppM, cx, cy, lBackgr);
		ptrCalc->SetValue(KEY_HOLLOWFACTOR,fHollowFactor);
	}

	if(ParamIsEnabledByKey(KEY_BRIGHTNESSMOMENT3))
	{
		double fM1=0, fM2=0, fM3=0;
		CalcMoments(ppC1, ppM, cx, cy, lBackgr,
						&fM1, &fM2, &fM3 );
		ptrCalc->SetValue(KEY_BRIGHTNESSMOMENT3,fM3);
	}

	// пошли марковские параметры...
	CMarkovParamCalculator* pMarkovCalc=0;

	int nPhase = 0;
	if(m_nMarkovianEqualizeMethod==1) nPhase=0x7F80; // 1 - по всей зоне интереса (маска объекта)
	else if(m_nMarkovianEqualizeMethod==2) nPhase=0x01FE; // 2 - по всей клетке

	// Марковские параметры для всей клетки
#undef PARAM
#define PARAM(name) PARAM2(name,KEY_CELL_##name);
	{
		bThereIsParams = false;
		PARAM(INERTIA);
		PARAM(ENERGY);
		PARAM(ENTROPY);
		PARAM(CORRELATION);
		PARAM(HOMOGENITY);
		if(bThereIsParams)
		{
			if(!pMarkovCalc) pMarkovCalc = new CMarkovParamCalculator;
			if(m_nMarkovianEqualizeMethod==3) nPhase=0x01FE;
			if(pMarkovCalc->m_nLevelsPhases!=nPhase)
				pMarkovCalc->CalcLevels(ppC1, ppM, cx, cy, nPhase);
			pMarkovCalc->CalcMatrix(ppC1, ppM, cx, cy, 0x01FE);
			pMarkovCalc->CalcParams();
			fValINERTIA = pMarkovCalc->m_fInertia;
			fValENERGY = pMarkovCalc->m_fEnergy;
			fValENTROPY = pMarkovCalc->m_fEntropy;
			fValCORRELATION = pMarkovCalc->m_fCorrelation;
			fValHOMOGENITY = pMarkovCalc->m_fHomogenity;
		}
	}

#undef PARAM
#define PARAM(name) PARAM2(name,KEY_NUCLEUS_##name);
	try
	{ // Параметры "формы" (структуры) ядра
		bThereIsParams = false;
		PARAM(F0);
		PARAM(F1);
		PARAM(F2);
		PARAM(F3);
		PARAM(F4);
		PARAM(F5);
		PARAM(F6);
		PARAM(F7);
		PARAM(F8);
		if(bThereIsParams)
		{
			smart_alloc(ppM2, byte*, cy);
			smart_alloc(pM2buf, byte, cy*cx);
			for(int y=0; y<cy; y++) ppM2[y]=pM2buf+y*cx;

			double f0=0, f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7=0, f8=0, f9=0;
			// параметры - интегралы:
			// dA, B*dA, B'*dA, (B/A)*dA, (B/A/A)*dA
			// (B'/B)*dA
			double s1_prev=0, s2_prev=0;
			for(int col=0; col<=256; col++)
			{
				for(int y=0; y<cy; y++)
				{
					for(int x=0; x<cx; x++)
					{
						if(ppM[y][x]==254 && ppC1[y][x]<col*256)
							ppM2[y][x]=255;
						else
							ppM2[y][x]=0;
					}
				}
				double s1=0, s2=0;
				double ratio = 
					CalcSegments2(ppM2, cx, cy, &s1, &s2);
				double dA = s1-s1_prev, dB=s2-s2_prev;
				if(dA>=1)
				{
					f0 += dA;
					f1 += s2*dA;
					f2 += (s2-s2_prev)*dA;
					if(s1>=1) f3 += s2/s1*dA;
					if(s1>=1) f4 += s2/(s1*s1)*dA;
					if(s2>0) f5 += (dB/s2)*dA;
					s1_prev=s1;
					s2_prev=s2;
				}

				//FILE *f=fopen("d:\\vt5\\out.log","at");
				//fprintf( f, "%i\t%i\t%g\t%g\t%g\n", col, s1, s2, ratio );
				//fclose(f);
			}
			if(f0>=1)
			{ // приведем f1-f4 к безразмерному виду (реально f2 - 1/цвет)
				f1 /= f0*f0*f0;
				f2 /= f0*f0*f0;
				f3 /= f0*f0;
				f4 /= f0;
				f5 /= f0;
			}
			f9 = f1+f2+f3+f4+f5; // просто так.
			fValF1 = f1;
			fValF2 = f2;
			fValF3 = f3;
			fValF4 = f4;
			fValF5 = f5;
		}
	}
	catch(...)
	{
		// не получилось посчитать F1-F5
		// MessageBox(0,"Параметры \"формы\" (структуры) ядра","CellMeasGroup - Exception", 0);
	}

	// Марковские параметры для ядра
#undef PARAM
#define PARAM(name) PARAM2(name,KEY_CELLNUCLEUS_##name);
	{
		bThereIsParams = false;
		PARAM(INERTIA);
		PARAM(ENERGY);
		PARAM(ENTROPY);
		PARAM(CORRELATION);
		PARAM(HOMOGENITY);
		//{
		//	char s[200];
		//	sprintf(s,"CalcValues %d %d %d %d %d (%d)", bEnabledINERTIA,bEnabledENERGY,
		//		bEnabledENTROPY,bEnabledCORRELATION,bEnabledHOMOGENITY,bThereIsParams);
		//	MessageBox(0,s,"CellMeasGroup", 0);
		//}

		if(bThereIsParams)
		{
			if(!pMarkovCalc) pMarkovCalc = new CMarkovParamCalculator;
			if(m_nMarkovianEqualizeMethod==3) nPhase=254;
			if(pMarkovCalc->m_nLevelsPhases!=nPhase)
				pMarkovCalc->CalcLevels(ppC1, ppM, cx, cy, nPhase);
			pMarkovCalc->CalcMatrix(ppC1, ppM, cx, cy, 254); // ядро
			pMarkovCalc->CalcParams();
			fValINERTIA = pMarkovCalc->m_fInertia;
			fValENERGY = pMarkovCalc->m_fEnergy;
			fValENTROPY = pMarkovCalc->m_fEntropy;
			fValCORRELATION = pMarkovCalc->m_fCorrelation;
			fValHOMOGENITY = pMarkovCalc->m_fHomogenity;
		}
	}

	// Марковские параметры для цитоплазмы
#undef PARAM
#define PARAM(name) PARAM2(name,KEY_CELLCITOPLASMA_##name);
	{
		bThereIsParams = false;
		PARAM(INERTIA);
		PARAM(ENERGY);
		PARAM(ENTROPY);
		PARAM(CORRELATION);
		PARAM(HOMOGENITY);
		if(bThereIsParams)
		{
			if(!pMarkovCalc) pMarkovCalc = new CMarkovParamCalculator;
			if(m_nMarkovianEqualizeMethod==3) nPhase=255;
			if(pMarkovCalc->m_nLevelsPhases!=nPhase)
				pMarkovCalc->CalcLevels(ppC1, ppM, cx, cy, nPhase);
			pMarkovCalc->CalcMatrix(ppC1, ppM, cx, cy, 255); // цитоплазма=255
			pMarkovCalc->CalcParams();
			fValINERTIA = pMarkovCalc->m_fInertia;
			fValENERGY = pMarkovCalc->m_fEnergy;
			fValENTROPY = pMarkovCalc->m_fEntropy;
			fValCORRELATION = pMarkovCalc->m_fCorrelation;
			fValHOMOGENITY = pMarkovCalc->m_fHomogenity;
		}
	}

	// покончили с марковскими параметрами
	if(pMarkovCalc)
	{
		delete pMarkovCalc;
		pMarkovCalc = 0;
	}

	// параметры для "многоугольника"
#undef PARAM
#define PARAM(name) PARAM2(name,KEY_ANGLES_##name);
	{
		bThereIsParams = false;
		PARAM(PLUS_N);
		PARAM(MINUS_N);
		PARAM(N);
		PARAM(RATIO_N);
		PARAM(SUM_PLUS_N);
		PARAM(SUM_MINUS_N);
		PARAM(SUM_RATIO_N);
		PARAM(SUM_RATIO2_N);

		if(bThereIsParams)
		{

			int step=10; // длина "рычага" для поиска углов

			CWalkFillInfo wfi( _rect(0,0, cx,cy) );
			for(int y=0; y<cy; y++) // заполним маски для walkfillinfo
			{
				for(int x=0; x<cx; x++)
				{
					if( ppM[y][x]>=m_nNucleiPhaseMin &&
						ppM[y][x]<=m_nNucleiPhaseMax) // ядро
					{
						wfi.SetPixel(x,y, 255); 
					}
				}
			}
			wfi.InitContours(); // создадим контура

			int n = wfi.GetContoursCount();

			fValPLUS_N = 0;
			fValMINUS_N = 0;
			fValSUM_PLUS_N = 0;
			fValSUM_MINUS_N = 0;

			double sum2=0, sum_len=0;

			for(int i=0; i<n; i++)
			{
				Contour* pcont = wfi.GetContour(i);
				int len=pcont->nContourSize;

				if(len<step*4) continue; // совсем мелкие контура не считаем
				sum_len += len;

				// посчитаем углы
				smart_alloc(angle, double, len);
				for(int j=0; j<len; j++)
				{
					int j1 = (j+len-step) % len; // назад по контуру
					int j2 = (j+step) % len; // вперед по контуру
					angle[j] = atan2( // можно бы и без арктангенса, ну да ладно
						(double)(pcont->ppoints[j2].y - pcont->ppoints[j1].y),
						(double)(pcont->ppoints[j2].x - pcont->ppoints[j1].x));
				}

				// посчитаем производную от угла
				smart_alloc(da, double, len);
				for(int j=0; j<len; j++)
				{
					int j1 = (j+1) % len ; // вперед по контуру
					da[j] = angle[j1]-angle[j];
					while(da[j]<=-PI) da[j] += PI*2;
					while(da[j]>=PI) da[j] -= PI*2;
				}

				double th=0; // threshold для пиков
				double d_max=0, d_min=0.01;

				int n_peaks=0; // число выпуклых углов
				int n_cavity=0; // число вогнутых углов

				smart_alloc(da2, double, len);
				smart_alloc(da3, double, len);
				{ // для поиска выпуклых углов - прогоним max filter
					for(int j=0; j<len; j++)
					{
						da2[j] = da[j];
						for(int k=-step; k<step; k++)
						{
							int j1 = (j+len+k) % len;
							da2[j] = max(da2[j], da[j1]);
						}
					}
				}
				{ // для вогнутых углов - прогоним min filter
					for(int j=0; j<len; j++)
					{
						da3[j] = da[j];
						for(int k=-step; k<step; k++)
						{
							int j1 = (j+len+k) % len;
							da3[j] = min(da3[j], da[j1]);
						}
					}
				}

				// пробежим 2 раза по кругу (1-й раз для "разгона") - ищем максимумы/минимумы
				{ // поиск пиков
					double d1=0,d2=0;
					for(int k=0; k<2; k++)
					{
						for(int j=0; j<len; j++)
						{
							double d = da2[j];
							if(d!=d1)
							{
								if(k && d1>d && d1>d2 && d1>th) n_peaks++;
								d2=d1;
								d1=d;
							}
						}
					}
				}

				{ // поиск впадин
					double d1=0,d2=0;
					for(int k=0; k<2; k++)
					{
						for(int j=0; j<len; j++)
						{
							double d = da3[j];
							if(d!=d1)
							{
								if(k && d1<d && d1<d2 && d1<-th) n_cavity++;
								d2=d1;
								d1=d;
							}
						}
					}
				}

				if( pcont->lFlags == cfExternal)
				{
					fValPLUS_N += n_peaks;
					fValMINUS_N += n_cavity;
					fValMINUS_N += 4;
				}
				if( pcont->lFlags == cfInternal)
				{
					fValPLUS_N += n_cavity;
					fValMINUS_N += n_peaks;
					fValPLUS_N += 4;
				}

				double sum_plus=0, sum_minus=0;
				{ // обсчет Ratio2 и иже с ним
					for(int j=0; j<len; j++)
					{
						double d = da[j];
						sum2 += d*d;
						if (d>0) sum_plus += d;
						else sum_minus += d;
					}
				}
				//sum_plus /= (step*2);
				//sum_minus /= (step*2);

				if( pcont->lFlags == cfExternal)
				{
					fValSUM_PLUS_N += sum_plus;
					fValSUM_MINUS_N += sum_minus;
				}
				if( pcont->lFlags == cfInternal)
				{
					fValSUM_PLUS_N -= sum_minus;
					fValSUM_MINUS_N -= sum_plus;
				}
			}

			fValMINUS_N -= 4; // на первом внешнем контуре было добавлено 4 лишних отрицательных угла - уберем их
			fValN = fValPLUS_N + fValMINUS_N;
			fValRATIO_N = fValMINUS_N / max(1,fValN);

			fValSUM_RATIO_N = -fValSUM_PLUS_N / max(1, fValSUM_PLUS_N-fValSUM_MINUS_N);
			fValSUM_RATIO2_N = sqrt( 4*PI*PI/(max(4*PI*PI,sum_len*sum2)) );
		}
	}

	return true;
}

bool CCellMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_CELLAREA, "Cell Area", "%0.1f");
	DefineParameter(KEY_CELLNUCLEUSAREA, "Nucleus Area", "%0.1f");
	DefineParameter(KEY_CELLPERIMETER, "Cell Perimeter", "%0.1f");
	DefineParameter(KEY_CELLNUCLEUSPERIMETER, "Nucleus Perimeter", "%0.1f");
	DefineParameter(KEY_CELLCIRCULARITY, "Cell Circularity", "%0.3f"); // 1 - круглое, >1 - нет
	DefineParameter(KEY_CELLNUCLEUSCIRCULARITY, "Nucleus Circularity", "%0.3f"); // 1 - ядро круглое, >1 - нет
	DefineParameter(KEY_CELLELLIPTICITY, "Cell Ellipticity", "%0.3f"); // 0 - круг, >0 - эллипс
	DefineParameter(KEY_CELLNUCLEUSELLIPTICITY, "Nucleus Ellipticity", "%0.3f"); // 0 - круг, >0 - эллипс
	DefineParameter(KEY_CELLNUCLEUSAREAPERCENT, "Nucleus Area Percent", "%0.2f"); // площадь ядра/площадь клетки

	DefineParameter(KEY_CELLNUCLEUSMOMENT1, "Nucleus Moment 1", "%0.3f"); // первый момент яркости
	DefineParameter(KEY_CELLNUCLEUSMOMENT2, "Nucleus Moment 2", "%0.3f"); // второй момент яркости
	DefineParameter(KEY_CELLNUCLEUSMOMENT3, "Nucleus Moment 3", "%0.3f"); // третий момент яркости
	DefineParameter(KEY_CELLCITOPLASMAMOMENT1, "Citoplasma Moment 1", "%0.3f"); // первый момент яркости
	DefineParameter(KEY_CELLCITOPLASMAMOMENT2, "Citoplasma Moment 2", "%0.3f"); // второй момент яркости
	DefineParameter(KEY_CELLCITOPLASMAMOMENT3, "Citoplasma Moment 3", "%0.3f"); // третий момент яркости

	DefineParameter(KEY_CELLNUCLEUSGRADIENT, "Nucleus Gradient", "%0.3f"); // среднеквадратичный градиент яркости
	DefineParameter(KEY_CELLCITOPLASMAGRADIENT, "Citoplasma Gradient", "%0.3f"); // среднеквадратичный градиент яркости


	DefineParameter(KEY_CELLNUCLEISEGMENTS1, "Nucleus Segm.1", "%0.0f"); // число сегментов в ядре после erode 1
	DefineParameter(KEY_CELLNUCLEISEGMENTS2, "Nucleus Segm.2", "%0.0f"); // число сегментов в ядре после erode 2
	DefineParameter(KEY_CELLNUCLEISEGMENTS3, "Nucleus Segm.3", "%0.0f"); // число сегментов в ядре после erode 3
	DefineParameter(KEY_CELLNUCLEISEGMENTS4, "Nucleus Segm.4", "%0.0f"); // число сегментов в ядре после erode 4
	DefineParameter(KEY_CELLNUCLEISEGMENTS5, "Nucleus Segm.5", "%0.0f"); // число сегментов в ядре после erode 5

	DefineParameter(KEY_CELLNUCLEUSMOMENT1R, "Nucleus Moment 1R", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUSMOMENT2R, "Nucleus Moment 2R", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUSMOMENT3R, "Nucleus Moment 3R", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT1R, "Citoplasma Moment 1R", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT2R, "Citoplasma Moment 2R", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT3R, "Citoplasma Moment 3R", "%0.3f"); 

	DefineParameter(KEY_CELLNUCLEUSMOMENT1B, "Nucleus Moment 1B", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUSMOMENT2B, "Nucleus Moment 2B", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUSMOMENT3B, "Nucleus Moment 3B", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT1B, "Citoplasma Moment 1B", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT2B, "Citoplasma Moment 2B", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMAMOMENT3B, "Citoplasma Moment 3B", "%0.3f"); 

	DefineParameter(KEY_HOLLOWFACTOR, "Hollow Factor", "%0.3f"); 
	DefineParameter(KEY_BRIGHTNESSMOMENT3, "3rd Moment of Brightness", "%0.3f"); 

	DefineParameter(KEY_CELLNUCLEUS_INERTIA, "Inertia_N", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUS_ENERGY, "Energy_N", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUS_ENTROPY, "Entropy_N", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUS_CORRELATION, "Correlation_N", "%0.3f"); 
	DefineParameter(KEY_CELLNUCLEUS_HOMOGENITY, "Homogenity_N", "%0.3f"); 

	DefineParameter(KEY_CELLCITOPLASMA_INERTIA, "Inertia_C", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMA_ENERGY, "Energy_C", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMA_ENTROPY, "Entropy_C", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMA_CORRELATION, "Correlation_C", "%0.3f"); 
	DefineParameter(KEY_CELLCITOPLASMA_HOMOGENITY, "Homogenity_C", "%0.3f"); 

	DefineParameter(KEY_CELL_INERTIA, "Inertia_C2", "%0.3f"); 
	DefineParameter(KEY_CELL_ENERGY, "Energy_C2", "%0.3f"); 
	DefineParameter(KEY_CELL_ENTROPY, "Entropy_C2", "%0.3f"); 
	DefineParameter(KEY_CELL_CORRELATION, "Correlation_C2", "%0.3f"); 
	DefineParameter(KEY_CELL_HOMOGENITY, "Homogenity_C2", "%0.3f"); 

	DefineParameter(KEY_NUCLEUS_F0, "F0_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F1, "F1_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F2, "F2_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F3, "F3_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F4, "F4_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F5, "F5_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F6, "F6_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F7, "F7_N", "%0.3f"); 
	DefineParameter(KEY_NUCLEUS_F8, "F8_N", "%0.3f"); 

	DefineParameter(KEY_ANGLES_PLUS_N, "AnglesPlus_N", "%0.0f"); 
	DefineParameter(KEY_ANGLES_MINUS_N, "AnglesMinus_N", "%0.0f"); 
	DefineParameter(KEY_ANGLES_N, "Angles_N", "%0.0f"); 
	DefineParameter(KEY_ANGLES_RATIO_N, "AnglesRatio_N", "%0.3f"); 
	DefineParameter(KEY_ANGLES_SUM_PLUS_N, "AnglesSumPlus_N", "%0.3f"); 
	DefineParameter(KEY_ANGLES_SUM_MINUS_N, "AnglesSumMinus_N", "%0.3f"); 
	DefineParameter(KEY_ANGLES_SUM_RATIO_N, "AnglesSumRatio_N", "%0.3f"); 
	DefineParameter(KEY_ANGLES_SUM_RATIO2_N, "AnglesSumRatio2_N", "%0.3f"); 

	return true;
}

bool CCellMeasGroup::OnInitCalculation()
{
	m_nPointsAverage = ::GetValueInt( ::GetAppUnknown(), "\\CellMeasGroup", "PointsAverage", 5);
	m_nStepAverage = ::GetValueInt( ::GetAppUnknown(), "\\CellMeasGroup", "StepAverage", 2);
	m_nMarkovianEqualizeMethod = ::GetValueInt( ::GetAppUnknown(), "\\CellMeasGroup", "MarkovianEqualizeMethod", 2);
	m_nNucleiPhaseMin = ::GetValueInt( ::GetAppUnknown(), "\\CellMeasGroup", "NucleiPhaseMin", 254);
	m_nNucleiPhaseMax = ::GetValueInt( ::GetAppUnknown(), "\\CellMeasGroup", "NucleiPhaseMax", 254);
	return CMeasParamGroupBase::OnInitCalculation();
}

bool CCellMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	switch (lKey)
	{
	case KEY_CELLAREA:
	case KEY_CELLNUCLEUSAREA:
		{
			CString	strUnitName, str;

			str = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
			fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
			strUnitName.Format( "%s*%s", (const char*)str, (const char*)str );

			fMeterPerUnit*=fMeterPerUnit;
			bstrType = strUnitName;
			break;
		}
	case KEY_CELLPERIMETER:
	case KEY_CELLNUCLEUSPERIMETER:
		{
			bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
			fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
			break;
		}
	case KEY_CELLCIRCULARITY:
	case KEY_CELLNUCLEUSCIRCULARITY:
	case KEY_CELLELLIPTICITY:
	case KEY_CELLNUCLEUSELLIPTICITY:
	case KEY_CELLNUCLEUSAREAPERCENT:
		{
			bstrType = GetValueString( GetAppUnknown(), szRelativeUnits, szUnitName, szDefRelativeUnits );
			fMeterPerUnit = GetValueDouble( GetAppUnknown(), szRelativeUnits, szUnitCoeff, 1 );
			break;
		}
	default:
		{
			bstrType = "-";
			fMeterPerUnit = 1;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CCellMeasGroup::GetClassID( CLSID* pClassID )
{
	if(pClassID==0) return; //так, на всякий пожарный
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
