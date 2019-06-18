#include "stdafx.h"
#include "SpermSegm.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "aam_utils.h"
#include <limits.h>
#include "\vt5\awin\misc_ptr.h"
#include "float.h"

_ainfo_base::arg CSpermSegmInfo::s_pargs[] =
{
	{"BackTolerance",    szArgumentTypeDouble , "6", true, false },
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, false, true },
	{"ForeMethod",    szArgumentTypeInt , "-1", true, false },
	// ForeMethod: 0 - темное, 1 - светлое, -1 - автомат (выбор 0/1), 2 - черный фон (0), 3 - белый фон (65535)
	{0, 0, 0, false, false },
};

_ainfo_base::arg CSegmObjectsInfo::s_pargs[] =
{
	{"Threshold", szArgumentTypeDouble , "0.5", true, false },
	{"Dilate", szArgumentTypeInt , "5", true, false },
	{"Src",       szArgumentTypeObjectList, 0, true, true },
	{"Res",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg CSegmLinesInfo::s_pargs[] =
{
	{"Src",       szArgumentTypeImage, 0, true, true },
	{"SrcParticles",       szArgumentTypeObjectList, 0, true, true },
	{"SrcStopZone",       szArgumentTypeImage, 0, true, true },
	{"ResParticles",       szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

#define LIMIT(x,minval,maxval) \
	if(x<minval) x=minval; \
	else if(x>maxval) x=maxval;

/////////////////////////////////////////////////////////////////////////////
CSpermSegm::CSpermSegm()
{
}

/////////////////////////////////////////////////////////////////////////////
CSpermSegm::~CSpermSegm()
{
}


void CSpermSegm::FirstPass(color **src, byte **bin, int cx, int cy, double bg_tolerance)
//первый проход сегментации, вычисление цвета фона
//bg_tolerance - во сколько раз увеличивать определенную ширину пика фона для сегментации
{
	long lBorder = ::GetValueInt( GetAppUnknown(), "\\SpermSettings", "Border", 20 );
	if(lBorder>cx/4-1) lBorder=cx/4-1;
	if(lBorder>cy/4-1) lBorder=cy/4-1;
	if(lBorder<0) lBorder=0;
	// подсчитали отступ от края, исключаемый при построении гистограммы.

	smart_alloc(src4hist, color *, cy-lBorder*2);
	for(int i=0; i<cy-lBorder*2; i++) src4hist[i] = src[i+lBorder]+lBorder;

	//поиск пика гистограммы (фон) и его ширины
	smart_alloc(hist, int, NC);
	CalcHist(src4hist, cx-lBorder*2, cy-lBorder*2, hist);
	int max_peak=0;
	for(int i=0; i<NC; i++)
	{
		if(hist[i]>hist[max_peak]) max_peak=i;
	}
	if(m_nForeMethod==2) max_peak=0;
	else if(m_nForeMethod==3) max_peak=NC-1;
	
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
    if(bg_tolerance>0) peak_width *= bg_tolerance;
    else peak_width = -bg_tolerance;

	int pw1 =  int(peak_width/(65536/NC));
	double fore_count1=0, fore_count0=0;
	for(i=0; i<NC; i++)
	{
		if(abs(i-max_peak1)<pw1) continue;
		fore_count0+=hist[i];
		fore_count1+=hist[i]*i;
	}
	if(fore_count0<1) fore_count0=1;
	int fore1 = int(fore_count1/fore_count0+0.5);
	int fore=fore1*(65536/NC);

	int threshold1=max_peak-int(peak_width);
	int threshold2=max_peak+int(peak_width);

	if(m_nForeMethod==0) threshold2 = 65536; // если у нас объекты темные - в фон включаем все светлое
	else if(m_nForeMethod==1) threshold1 = threshold1=-1; // если светлые - в фон все темное
	else
	{ // автомат
		if(fore<max_peak) threshold2 = 65536; // если у нас объекты темные - в фон включаем все светлое
		else threshold1=-1; // если светлые - в фон все темное
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

	// !!! debug - логарифм разницы между изображением и фоном
	/*
	peak_width = max(peak_width,1);
	for(int y=0; y<cy; y++)
	{
        for(int x=0; x<cx; x++)
        {
			double c = abs(src[y][x]-m_back)+peak_width;
			c = log(c/peak_width)/log(65536/peak_width)*65536;
			if(c<0) c=0;
			if(c>65535) c=65535;
			src[y][x] = int(c);
		}
	}
	*/
}

void CSpermSegm::FirstPass2(color **src, byte **bin, int cx, int cy, bool invert_ratio)
//первый проход сегментации - по двум пикам гистограммы
{
	smart_alloc(hist, int, NC);
	CalcHist(src, cx, cy, hist);
	
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
	
	th=(th*(65536/NC)) ^ xor_mask;
	
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


bool CSpermSegm::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument() );
	IBinaryImagePtr pBin( GetDataResult("Binary") );
	double fBackTolerance = GetArgDouble("BackTolerance");
	m_nForeMethod = GetArgLong("ForeMethod");

	if( pArg==NULL || pBin==NULL )
		return false;
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	if(size.cx<=0 || size.cy<=0) return false;

	int nPaneCount;
	/*IColorConvertor2* pIClrConv=0;
	pArg->GetColorConvertor((IUnknown**)&pIClrConv);
	if(pIClrConv==0) return false;
	pIClrConv->GetColorPanesCount(&nPaneCount);
	pIClrConv->Release();*/
	nPaneCount = ::GetImagePaneCount( pArg );


	StartNotification( size.cy, nPaneCount );
	
	pBin->CreateNew(size.cx, size.cy);
	pBin->SetOffset( ptOffset, TRUE);
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}
	
	smart_alloc(binRows, byte *, size.cy);
	for(int y=0; y<size.cy; y++)
	{
		pBin->GetRow(&binRows[y], y, false);
	}
	
	if(1) FirstPass(srcRows, binRows, size.cx, size.cy, fBackTolerance);
	if(0) FirstPass2(srcRows, binRows, size.cx, size.cy, 0);
	
	FinishNotification();
	
	return true;
}

///////////////////////////////////////////////////////////////////////

static bool create_compatible_ol( IUnknown* punk_src, IUnknown* punk_target, bool bMakeNewImages = true )
{
	if( !punk_src || !punk_target )	return 0;

	int nleave_virtual = 1;

	ICompatibleObjectPtr ptr_co( punk_target );
	if( ptr_co == 0 )	return 0;

	ptr_co->CreateCompatibleObject( punk_src, 0, 0 );

	long l1 = GetTickCount();
	long l2 = GetTickCount();

	//create new images

	INamedDataObject2Ptr ptr_list( ptr_co );
	if( ptr_list )
	{
		long lpos = 0;
		ptr_list->GetFirstChildPosition( &lpos );
		while( lpos )
		{
			IUnknown* punk_child = 0;
			ptr_list->GetNextChild( &lpos, &punk_child );
			
			if( !punk_child )			continue;

			IMeasureObjectPtr ptr_meas_obj = punk_child;
			punk_child->Release(); punk_child = 0;

			if( ptr_meas_obj == 0 )		continue;
		
			IUnknownPtr ptr_image = 0;
			ptr_meas_obj->GetImage( &ptr_image );
			if( ptr_image == 0 )		continue;

			IImage3Ptr ptr_src_image = ptr_image;
			if( ptr_src_image == 0 )	continue;

			IUnknownPtr ptr_new_image( ::CreateTypedObject( _bstr_t(szTypeImage) ), false );
			if( ptr_new_image == 0 )	continue;
			
			ImageCloneData icd = {0};
			icd.dwCopyFlags = ICD_COPY_MASK | ICD_COPY_CONTOURS | ICD_COPY_IMGDATA;

			ICompatibleObjectPtr ptr_comp_image = ptr_new_image;
			if( ptr_comp_image == 0 )	continue;

			ptr_comp_image->CreateCompatibleObject( ptr_image, &icd, sizeof(icd) );
			ptr_meas_obj->SetImage( ptr_comp_image );
			
			
			//Unvirtual image if need
			if( nleave_virtual == 1 )
				continue;

			INamedDataObject2Ptr ptr_ndo( ptr_comp_image );
			if( ptr_ndo )
			{
				IUnknownPtr ptr_parent;
				ptr_ndo->GetParent( &ptr_parent );
				if( ptr_parent )
					ptr_ndo->SetParent( 0, sdfCopyParentData );
			}
		}
	}

	return ptr_co;
}

static void DilateColor(BYTE **sel, int cx, int cy, int mask, byte cFore, byte cForeNew, byte cBack)
//обработка маски - дилатация
{
	int m=mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	smart_alloc(count, int, (cx+1)*(cy+1));
	smart_alloc(countRows, int*, cy+1);
	for(int y=0; y<=cy; y++) countRows[y] = count+(cx+1)*y;
	
	for(int x=0; x<=cx; x++) count[x]=0; //1-я строка =0
	
	for(y=1; y<=cy; y++)
	{
		countRows[y][0]=0;
		for(int x=1; x<=cx; x++)
		{
			countRows[y][x]=
				countRows[y-1][x]+countRows[y][x-1]-
				countRows[y-1][x-1];
			if (sel[y-1][x-1]==cFore)
				countRows[y][x]++;
		}
	}
	
	smart_alloc(y0, int, cy);
	smart_alloc(y1, int, cy);
	for(y=0; y<cy; y++)
	{
		y0[y]=max(y-m1,0);
		y1[y]=min(y+m2,cy);
	}
	
	smart_alloc(x0, int, cx);
	smart_alloc(x1, int, cx);
	for(x=0; x<cx; x++)
	{
		x0[x]=max(x-m1,0);
		x1[x]=min(x+m2,cx);
	}
	
	for(y=0; y<cy; y++)
	{
		int *c0=countRows[y0[y]];
		int *c1=countRows[y1[y]];
		for(int x=0; x<cx; x++)
		{
			int xx0=x0[x], xx1=x1[x];
			if(sel[y][x]==cBack && c1[xx1]-c1[xx0]-c0[xx1]+c0[xx0]>2)
				sel[y][x]=cForeNew;
		}
	}
}

static void FillSmallSegments(byte **buf, int cx, int cy, byte old_color, byte fill_color)
{
	if(cx<3 || cy<3) return; //на такой фигне делать нечего...

	//smart_alloc(ind, byte, 256);
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
	
	smart_alloc(area, int, cur_ind+1);
	smart_area.zero();
	smart_alloc(color, int, cur_ind+1);
	smart_color.zero();
	
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=ind[cnt[y][x]];
			area[c]++;
			color[c]=buf[y][x];
		}
	}

	// найдем максимум площади среди нужного нам цвета
	int max_area = -1;
	for(i=cur_ind; i>=0; i--)
	{
		if(color[i]==old_color && area[i]>max_area) max_area=area[i];
	}

	smart_alloc(del_it, BOOL, cur_ind+1);
	for(i=cur_ind; i>=0; i--)
	{
		del_it[i] = 0;
		if(color[i]==old_color && area[i]<max_area)
			del_it[i]=1;
	}
	
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=ind[cnt[y][x]];
			if(del_it[c])
				buf[y][x]=fill_color;
		}
	}
}

bool CSegmObjects::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());

	if( objectsIn == 0 || objectsOut == 0 )	return false;

	if( !create_compatible_ol( objectsIn, objectsOut ) )	return false;

	double dThreshold = GetArgDouble("Threshold");
	long nDilate = GetArgLong("Dilate");

	long nCount; objectsOut->GetChildsCount(&nCount);
	if(!nCount) return false;
	
	int nObject=0;
	StartNotification(nCount,1,1);

	long pos; objectsOut->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr ptr1;
		objectsOut->GetNextChild(&pos, &ptr1);
		IMeasureObjectPtr ptrO(ptr1);
		if(ptrO==0) continue;
        
		IUnknownPtr ptr2;
		ptrO->GetImage( &ptr2 );
		IImage3Ptr ptrI(ptr2);
		if(ptrI==0) continue;

		int cx=0,cy=0;
		ptrI->GetSize(&cx,&cy);

		double sum1=0; int sum0=0; // найдем средний цвет объекта
		for(int y=0; y<cy; y++)
		{
			byte *pm;
			ptrI->GetRowMask(y, &pm);
			color *pc;
			ptrI->GetRow( y, 0, &pc );
			for(int x=0; x<cx; x++)
			{
				if(pm[x]&128) { sum0++; sum1+=pc[x]; };
			}
		}
		if(sum0==0) continue;
		sum1/=sum0;

		double sum1H=0, sum0H=0; // найдем средний цвет светлых
		double sum1L=0, sum0L=0; // найдем средний цвет темных
		for(int y=0; y<cy; y++)
		{
			byte *pm;
			ptrI->GetRowMask(y, &pm);
			color *pc;
			ptrI->GetRow( y, 0, &pc );
			for(int x=0; x<cx; x++)
			{
				if(pm[x]&128)
				{
					double d=pc[x]-sum1;
					if(d<0) { sum0L-=d; sum1L-=d*pc[x]; }
					else { sum0H+=d; sum1H+=d*pc[x]; }
				};
			}
		}
		if(sum0H<1.0) continue;
		if(sum0L<1.0) continue;

		sum1H /= sum0H;
		sum1L /= sum0L;
		sum1 = sum1L+ (sum1H-sum1L)*dThreshold;

		for(int y=0; y<cy; y++)
		{
			byte *pm;
			ptrI->GetRowMask(y, &pm);
			color *pc;
			ptrI->GetRow( y, 0, &pc );
			for(int x=0; x<cx; x++)
			{
				if(pm[x]&128)
				{
					double d=pc[x]-sum1;
					if(d<0) pm[x]=254;
					else pm[x]=253;
				};
			}
		}

		// Morphological operations to delete edge
		smart_alloc(maskRows, byte *, cy);
		for(int y=0; y<cy; y++)
		{
			ptrI->GetRowMask(y, &maskRows[y]);
		}
		DilateColor(maskRows, cx,cy, nDilate, 254, 254, 253);
		DilateColor(maskRows, cx,cy, nDilate, 253, 253, 254);
		DilateColor(maskRows, cx,cy, nDilate, 254, 254, 253);
		DilateColor(maskRows, cx,cy, nDilate, 253, 253, 254);

		FillSmallSegments(maskRows, cx,cy, 253, 254);

		Notify(nObject++);
	}

	GUID guidNewBase;
	objectsIn->GetBaseKey(&guidNewBase);
	objectsOut->SetBaseKey(&guidNewBase);
	FinishNotification();

	return true;
}

CSegmObjects::CSegmObjects()
{
}

CSegmObjects::~CSegmObjects()
{
}

bool CSegmLines::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument("Src") );
	INamedDataObject2Ptr pParticles(GetDataArgument("SrcParticles"));
	IImage3Ptr      pStopImg( GetDataArgument("SrcStopZone") );

	//IBinaryImagePtr	pBin(GetDataArgument("SrcBinStopZone"));
	IUnknown *punk4=::CreateTypedObject(_bstr_t(szTypeBinaryImage)); // создаем, но не включаем в контекст
	IBinaryImagePtr pBin = punk4;
	if (punk4!=0) punk4->Release();

	//IImage3Ptr      pRes( GetDataResult("Res") );
	IUnknown *punk5=::CreateTypedObject(_bstr_t(szTypeImage)); // создаем, но не включаем в контекст
	IImage3Ptr pRes = punk5;
	if (punk5!=0) punk5->Release();


	INamedDataObject2Ptr pResParticles(GetDataResult("ResParticles"));

	bool bStopZoneUsed=true;
	if(pStopImg==0)
	{
		pStopImg=pArg; //!!!
		bStopZoneUsed=false;
	}

	if( pArg==NULL || pRes==NULL || pParticles==NULL ||
		pResParticles==NULL || pBin==NULL || pStopImg==NULL)
		return false;

	if( !create_compatible_ol( pParticles, pResParticles ) )	return false;

	//long nThreshold = GetArgLong("Threshold");
	//double fMitoThreshold = GetArgDouble("MitoThreshold");
	long nThreshold = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "Threshold", 400 );
	nThreshold = max(nThreshold,10);
	double fMitoThreshold = ::GetValueDouble( GetAppUnknown(), "\\SegmLines", "MitoThreshold", 0.5 );
	//double fAngleTolerance = ::GetValueDouble( GetAppUnknown(), "\\SegmLines", "AngleTolerance", 20 );
	//fAngleTolerance = max(1,fAngleTolerance);

	int nCircleAngleStep = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "CircleAngleStep", 2 );
	LIMIT(nCircleAngleStep,1,64);

	int cx=0,cy=0;
	pArg->GetSize(&cx,&cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	{ // сверить размеры изображений
		int cx1=0,cy1=0;
		pStopImg->GetSize(&cx1,&cy1);
		if(cx1!=cx || cy1!=cy) return false;
		POINT ptOffset1;
		pStopImg->GetOffset(&ptOffset1);
		if(ptOffset1.x!=ptOffset.x && ptOffset1.y!=ptOffset.y) return false;
	}

	pBin->CreateNew(cx, cy);
	pBin->SetOffset( ptOffset, TRUE);

	pRes->CreateImage(cx, cy, _bstr_t("GRAY"),-1);
	pRes->SetOffset(ptOffset, false);

	for(int i=0; i<256; i++)
	{
		m_co[i]=cos(i*PI*2/256);
		m_si[i]=sin(i*PI*2/256);
	}

	long nCount; pResParticles->GetChildsCount(&nCount);
	if(!nCount) return false;

	//-------------------------------------------
	AttachArrays(pResParticles);

	int nMaxObjects=nCount;
	int nMaxFrames=80;

	GetArray("x",sizeof(int)); //координаты
	GetArray("y",sizeof(int));
	GetArray("exist",sizeof(byte)); //существует ли на соответствующем кадре
	GetArray("intensity",sizeof(int));
	GetArray("width",sizeof(float));
	SetArraySize(nMaxFrames,nMaxObjects,-1);

	_byte_ptr<int> pLineX(GetExistingArray("x")); //координаты
	_byte_ptr<int> pLineY(GetExistingArray("y"));
	_byte_ptr<byte> pLineExist(GetExistingArray("exist")); //существует ли на соответствующем кадре
	_byte_ptr<int> pLineIntensity(GetExistingArray("intensity")); // отличие по цвету от фона
	_byte_ptr<float> pLineWidth(GetExistingArray("width")); // ширина

	int nObjectStep=GetObjectStep();
	int nFrameStep=GetFrameStep();

	//обнулим массив видимости точек
	for(int nFrame=0; nFrame<nMaxFrames-1; nFrame++)
	{
		for(int nObject=0; nObject<nMaxObjects; nObject++)
		{
			int posT = nFrameStep*nFrame + nObjectStep*nObject;
			pLineExist[posT]=0; // не видна
			pLineX[posT]=nObject*10; // test - вертикальные полоски
			pLineY[posT]=nFrame*10;
		}
	}

	::SetValue(pResParticles, "\\particles", "FrameNo", long(nFrame));
	//-------------------------------------------

	int nAngles=16;

	int nPane=1;

	smart_alloc(srcRows, color *, cy);
	smart_alloc(stopRows, color *, cy);
	smart_alloc(dstRows, color *, cy);
	smart_alloc(binRows, byte *, cy);
	for(int y=0; y<cy; y++)
	{
		pArg->GetRow(y, nPane, &srcRows[y]);
		pStopImg->GetRow(y, nPane, &stopRows[y]);
		pRes->GetRow(y, 0, &dstRows[y]);
		pBin->GetRow(&binRows[y], y, 0);
	}

	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++) dstRows[y][x] = srcRows[y][x];//65535;
		for(int x=0; x<cx; x++) binRows[y][x] = 0;
	}

	// заполнение binary
	{
		long pos; pResParticles->GetFirstChildPosition(&pos);
		while( pos ) //по всем объектам
		{
			IUnknownPtr ptr1;
			pResParticles->GetNextChild(&pos, &ptr1);
			IMeasureObjectPtr ptrO(ptr1);
			if(ptrO==0) continue;

			IUnknownPtr ptr2;
			ptrO->GetImage( &ptr2 );
			IImage3Ptr ptrI(ptr2);
			if(ptrI==0) continue;

			int cx_obj=0,cy_obj=0;
			ptrI->GetSize(&cx_obj,&cy_obj);
			POINT ptOffset;
			ptrI->GetOffset(&ptOffset);

			for(int y=0; y<cy_obj; y++)
			{
				int yy=y+ptOffset.y;
				if(yy<0 || yy>=cy) continue;
				byte *row;
				ptrI->GetRowMask(y, &row);
				for(int x=0; x<cx_obj; x++)
				{
					int xx=x+ptOffset.x;
					if(xx<0 || xx>=cx) continue;
                    if(row[x]>=128) binRows[yy][xx]=255;
				}
			}
		}
	}

	m_len=10; m_w=4;
	int len2=m_len+m_w;

	int nObject=0;
	StartNotification(nCount,1,1);

	long pos; pResParticles->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr ptr1;
		pResParticles->GetNextChild(&pos, &ptr1);
		IMeasureObjectPtr ptrO(ptr1);
		if(ptrO==0) continue;
        
		IUnknownPtr ptr2;
		ptrO->GetImage( &ptr2 );
		IImage3Ptr ptrI(ptr2);
		if(ptrI==0) continue;

		int cx_obj=0,cy_obj=0;
		ptrI->GetSize(&cx_obj,&cy_obj);
		POINT ptOffset;
		ptrI->GetOffset(&ptOffset);

		smart_alloc(mskRows, byte*, cy_obj);
		{for(int y=0; y<cy_obj; y++)
		{
			ptrI->GetRowMask(y, mskRows+y);
		}}

		_point ptCenter(0,0);
		GetCenter(ptrI, &ptCenter);
		int x0 = ptCenter.x;
		int y0 = ptCenter.y;

		CLineTracer tracer;
		//tracer.m_fAngleTolerance = fAngleTolerance;
		tracer.m_cx=cx;
		tracer.m_cy=cy;
		tracer.m_srcRows = srcRows;
		tracer.m_stopRows = stopRows;
		//if(!bStopZoneUsed) tracer.m_stopRows = 0;
		tracer.m_binRows = binRows;

		int r=40;
		int nAngle0=0, cBest=0;
		int nEdgeIndent=8;
		for(int nAngle=0; nAngle<256; nAngle+=nCircleAngleStep)
		{

			for(int r1=0; r1<100; r1++)
			{
				int xx=ptCenter.x-ptOffset.x + int(r1*m_co[nAngle]);
				int yy=ptCenter.y-ptOffset.y + int(r1*m_si[nAngle]);
				if(xx<0 || xx>=cx_obj || yy<0 || yy>=cy_obj) break;
				if(mskRows[yy][xx]<128) break;
			}
			//r1=r1+nEdgeIndent;
			int x=x0 + int(r1*m_co[nAngle]), y=y0+int(r1*m_si[nAngle]);
			if ( x<len2 || x>=cx-len2 || y<len2 || y>=cy-len2 ) continue;
			//if ( x<0 || x>=cx || y<0 || y>=cy ) continue;

			tracer.m_px[0]=x;
			tracer.m_py[0]=y;
			tracer.m_pangle[0]=nAngle;
			tracer.m_pintensity[0]=1000; ///!!!

			tracer.m_binRows = 0;
			tracer.m_stopRows = 0;
			int c = tracer.TraceStep(1) * r1;// учтем форму - предпочтение отдаем хвостам, выходящим из дальних точек;
			tracer.m_stopRows = stopRows;
			tracer.m_binRows = binRows;
			if(c<cBest/2)
			{	// если точка выглядит хорошей - уточним еще две
				c += tracer.TraceStep(2) * r1;// учтем форму - предпочтение отдаем хвостам, выходящим из дальних точек;
				c += tracer.TraceStep(3) * r1;// учтем форму - предпочтение отдаем хвостам, выходящим из дальних точек;
				c /= 3;
			}

			if(c<cBest)
			{
				cBest=c;
				nAngle0=nAngle;
				r=r1;
			}
		}

		int x=x0 + int(r*m_co[nAngle0]), y=y0+int(r*m_si[nAngle0]);
		nAngle = nAngle0;

		tracer.m_px[0]=x;
		tracer.m_py[0]=y;
		tracer.m_pangle[0]=nAngle;
		tracer.m_pintensity[0]=1000; ///!!!

		for(int i=1; i<nMaxFrames-1; i++)
		{
			int cBest = tracer.TraceStep(i);
			if(cBest>-nThreshold && i>1) break; // ибо нефиг
			// но хоть один шаг всегда сделаем
		}
		tracer.StoreData(pLineX, pLineY, pLineExist,
			pLineIntensity, pLineWidth,
			nObjectStep*nObject, i, nFrameStep);

		double sumInt=0.0;
		bool bMitohondrial=true;
		for(int ii=0; ii<=i; ii++)
		{
			int posT = nFrameStep*ii + nObjectStep*nObject;
			double avgInt=sumInt/max(1,ii);
			if(pLineIntensity[posT]>avgInt*fMitoThreshold) bMitohondrial=false;
			pLineWidth[posT]=max(pLineWidth[posT],1);
			if(!bMitohondrial) pLineWidth[posT] = float( -1 - fabs(pLineWidth[posT]) );
			if(bMitohondrial)
			{
				//pLineX[posT] = px[ii] + int(-(pw2[ii]-pw1[ii])*m_si[pangle[ii]]);
				//pLineY[posT] = py[ii] + int( (pw2[ii]-pw1[ii])*m_co[pangle[ii]]);
			}

			//fprintf(f, ">> %3i:\t%i\n", ii, pLineIntensity[posT]);
			sumInt += pLineIntensity[posT];
		}

		// откорректируем нулевую точку
		{
			int nFrame=0;
			int posT = nFrameStep*nFrame + nObjectStep*nObject;
			pLineWidth[posT] = pLineWidth[posT+nFrameStep]; // сдублируем предыдущую точку
		}

		if(1)
		{
			ITrajectoryPtr ptrT=CreateTrajectory(ptrO);
			ptrT->SetObjNum(nObject);
		}

		/*
		for(int r=0; r<50; r++)
		{
			int x=x0 + int(r*m_co[nAngle0]), y=y0+int(r*m_si[nAngle0]);
			if ( x<0 || x>=cx || y<0 || y>=cy ) continue;
			dstRows[y][x]=0;
		}
		*/

		Notify(nObject++);
	}

	/*
	for(int nAngle=0; nAngle<nAngles; nAngle++)
	{
		Notify(nAngle);
		double alpha=nAngle*PI/nAngles;
		double si=sin(alpha), co=cos(alpha);

		for(int y0=len2; y0<cy-len2; y0++)
		{
			for(int x0=len2; x0<cx-len2; x0++)
			{
				int c=TestLine(x0,y0,nAngle*(128/nAngles),srcRows,cx,cy);
				c = max(0,min(65535,65535+c*4));
				dstRows[y0][x0] = min( c, dstRows[y0][x0] );
			}
		}
	}
	*/

	GUID guidNewBase;
	pParticles->GetBaseKey(&guidNewBase);
	pResParticles->SetBaseKey(&guidNewBase);

	FinishNotification();
	
	return true;
}

CSegmLines::CSegmLines()
{
}

CSegmLines::~CSegmLines()
{
}

//////////////////////////////////////////////
CLineTracer::CLineTracer()
{
	m_len=10; m_w=4;
	m_nStep=8;
	m_nStepsBack=4;
	m_fAngleTolerance=1000;

	m_fAngleTolerance = ::GetValueDouble( GetAppUnknown(), "\\SegmLines", "AngleTolerance", m_fAngleTolerance );
	LIMIT(m_fAngleTolerance,1,1000);
	m_nStep = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "Step", m_nStep );
	LIMIT(m_nStep,3,50);
	m_nStepsBack = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "StepsBack", m_nStepsBack );
	LIMIT(m_nStepsBack,1,10);
	m_len = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "DetectorLength", m_len );
	LIMIT(m_len,5,50);
	m_w = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "DetectorWidth", m_w );
	LIMIT(m_w,1,50);

	m_nTestAngles=7;
	m_nTestAnglesStep=8;
	m_nTraceAngles=19;
	m_nTraceAnglesStep=4;

	m_nTestAngles = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "TestAngles", m_nTestAngles );
	LIMIT(m_nTestAngles,1,256);
	m_nTestAnglesStep = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "TestAnglesStep", m_nTestAnglesStep );
	LIMIT(m_nTestAnglesStep,1,64);
	m_nTraceAngles = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "TraceAngles", m_nTraceAngles );
	LIMIT(m_nTraceAngles,1,256);
	m_nTraceAnglesStep = ::GetValueInt( GetAppUnknown(), "\\SegmLines", "TraceAnglesStep", m_nTraceAnglesStep );
	LIMIT(m_nTraceAnglesStep,1,64);

	for(int i=0; i<256; i++)
	{
		m_co[i]=cos(i*PI*2/256);
		m_si[i]=sin(i*PI*2/256);
	}
}

CLineTracer::~CLineTracer()
{
}

int CLineTracer::LineMedian(int x, int y, int nAngle)
{
	/*
	static int hist[256];
	double si=m_si[nAngle], co=m_co[nAngle];
	for(int i=0; i<256; i++) hist[i]=0;
	for(int d=-m_len; d<=m_len; d++)
	{
		int cc = rows[int(y+d*si)][int(x+d*co)];
		hist[cc/256]++;
	}
	int sum=m_len+1;
	for(int i=0; sum>0; i++) sum-=hist[i];
	return i*256;
	*/
	const int nSegm=5;
	int sum[nSegm]={0,0,0,0,0};
	int count[nSegm]={0,0,0,0,0};
	int len2=m_len*2+1;
	double si=m_si[nAngle], co=m_co[nAngle];
	for(int d=-m_len; d<=m_len; d++)
	{
		int j=(d+m_len)*nSegm/len2;
		count[j]++;
		int xx = min(m_cx-1,max(0,int(x+d*co)));
		int yy = min(m_cy-1,max(0,int(y+d*si)));
		sum[j] += m_srcRows[yy][xx];
	}

	for(int i=0; i<nSegm; i++) sum[i] /= count[i];

	// тупая сортировка максимумом - на маленьком массиве не хуже qsort
	for(int i=nSegm-1; i>0; i--)
	{
		for(int j=0; j<i; j++)
		{
			if(sum[j]>sum[i])
			{
				int tmp=sum[j];
				sum[j]=sum[i];
				sum[i]=tmp;
			}
		}
	}

	return int(sum[nSegm/2]);
}

int CLineTracer::TestLine(int x, int y, int nAngle)
{
	int result=0;
	for(int nn=1; nn<=m_nTestAngles; nn++)
	{
		int dAngle=(nn/2)*m_nTestAnglesStep;
		if(nn&1) dAngle = -dAngle;
		int nAngle1 = nAngle + dAngle;
		nAngle1 &= 255;
		double si=m_si[nAngle1], co=m_co[nAngle1];
		int c0=LineMedian(x,y, nAngle1);
		int c1=LineMedian(x-int(m_w*si),y+int(m_w*co), nAngle1);
		int c2=LineMedian(x+int(m_w*si),y-int(m_w*co), nAngle1);
		result = min( result, c0-(c1+c2)/2 );
	}
	return result;
}

int CLineTracer::TraceStep(int i)
{	// проследить, куда пойдет n точка; могут также меняться предшествующие
	int len2=m_len+m_w;

	if(i>0)
	{
		m_pangle[i]=m_pangle[i-1];
		m_px[i]=m_px[i-1];
		m_pprev[i]=i-1;
	}
	m_pintensity[i]=1000000;
	m_pLineWidth[i]=0;

	for(int nn=1; nn<=m_nTraceAngles; nn++)
	{
		int dAngle=(nn/2)*m_nTraceAnglesStep;
		if(nn&1) dAngle = -dAngle;
		for(int nStep=1; nStep<=m_nStepsBack; nStep++)
		{
			if(nStep>i) continue;
			int nAngle1 = (m_pangle[i-nStep] + dAngle)&255;
			int nAngle2 = (m_pangle[i-nStep] + dAngle/2)&255;

			int x1 = m_px[i-nStep] + int(m_nStep*nStep*m_co[nAngle2]);
			int y1 = m_py[i-nStep] + int(m_nStep*nStep*m_si[nAngle2]);
			if ( x1<len2 || x1>=m_cx-len2 || y1<len2 || y1>=m_cy-len2 ) continue;
			//if ( x1<0 || x1>=m_cx || y1<0 || y1>=m_cy ) continue;
			bool bBad=false;
			if(m_binRows)
			{
				for(int zz=1; zz<=10; zz++)
				{
					int x2 = x1 + (m_px[i-nStep]-x1)*zz/10;
					int y2 = y1 + (m_py[i-nStep]-y1)*zz/10;
					if ( x2<len2 || x2>=m_cx-len2 || y2<len2 || y2>=m_cy-len2 )
					//if ( x1<0 || x1>=m_cx || y1<0 || y1>=m_cy )
					{
						bBad=true;
						break;
					}
					if (m_binRows[y2][x2]&128)
					{	// наехали на запрещенную область
						bBad=true;
						break;
					}
				}
			}
			if(bBad) continue;
			int c = int( TestLine(x1,y1,nAngle1) / (1.0+abs(dAngle)/m_fAngleTolerance) );
			if(m_stopRows) c = int( c * (m_stopRows[y1][x1]/65535.0) );
			if(c<m_pintensity[i])
			{
				m_pintensity[i]=c;
				m_pangle[i]=nAngle1;
				m_px[i]=x1;
				m_py[i]=y1;
				m_pprev[i]=i-nStep;
			}
		}
	}

	{
		int nAngle=m_pangle[i];
		double w1=0.0, w2=0.0;
		const double fWidthStep=0.25;
		const int nMaxWidth=40;
		int c_buf[nMaxWidth*2+1];
		int *pc=c_buf+nMaxWidth;
		for(int j=-nMaxWidth; j<=nMaxWidth; j++)
		{
			pc[j]=0;
			for(int k=-3; k<=3; k++)
			{
				int x1=m_px[i] + int(k*m_co[nAngle] - j*fWidthStep*m_si[nAngle]);
				int y1=m_py[i] + int(k*m_si[nAngle] + j*fWidthStep*m_co[nAngle]);
				x1=max(0,min(m_cx-1,x1));
				y1=max(0,min(m_cy-1,y1));
				pc[j]+=m_srcRows[y1][x1];
			}
			pc[j]/=7;
		}

		int cMax1=-65536, cMax2=-65536, cMin=65536, cAvg=0;
		for(int j=0; j<=nMaxWidth; j++)
		{
			cMax1 = max(cMax1, pc[j]);
			cMax2 = max(cMax1, pc[-j]);
			cMin = min(cMin, pc[j]);
			cMin = min(cMin, pc[-j]);
			cAvg += pc[j] + pc [-j];
		}
		cAvg /= (nMaxWidth+1)*2;
		double cThr = cAvg + (cMin-cAvg)*0.75;// + (min(cMax1,cMax2)-cAvg)*0.5;

		double dcMax1=0, dcMax2=0;
		for(int j=nMaxWidth-1; j>=0; j--)
		{
			if(pc[j]>cThr) w1=(j-0.5)*fWidthStep;
			if(pc[-j]>cThr) w2=(j-0.5)*fWidthStep;
		}

		m_pLineWidth[i] = float((w1+w2)/2);
		if(m_pLineWidth[i]<1) m_pLineWidth[i] = 1;
		m_pw1[i]=w1; m_pw2[i]=w2;

		if(1)//if(_hypot(px[i]-xStep1,py[i]-yStep1)>step)
		{
			for(int j=m_pprev[i]+1; j<i; j++)
			{
				double a = double(i-j)/(i-m_pprev[i]);
				double x = m_px[i]*(1-a) + m_px[m_pprev[i]]*a;
				double y = m_py[i]*(1-a) + m_py[m_pprev[i]]*a;
				//if(_hypot(px[i]-x,py[i]-y)>step/4.0)
				{
					m_px[j] = int( m_px[i]*(1-a) + m_px[m_pprev[i]]*a );
					m_py[j] = int( m_py[i]*(1-a) + m_py[m_pprev[i]]*a );
				}
				m_pintensity[j] = int( m_pintensity[i]*(1-a) + m_pintensity[m_pprev[i]]*a );
				double w0 = m_pLineWidth[i], w1 = m_pLineWidth[m_pprev[i]];
				if(w0<0) w0 = -(w0+1);
				if(w1<0) w1 = -(w1+1);
				double w = w0*(1-a) + w1*a;
				if(m_pLineWidth[i]<0 || m_pLineWidth[m_pprev[i]]<0)
					w = -1 - w;
				m_pLineWidth[j] = w;
			}
		}
	}

	if(i==1)
	{
		m_pLineWidth[0] = m_pLineWidth[1]; // чтобы 0 точку прописало...
		m_pintensity[0] = m_pintensity[1]; // чтобы 0 точку прописало...
	}

	if(i>=199) return 0; // если добрались до конца - нефиг дальше работать
	return m_pintensity[i];
}

int CLineTracer::ExpandStep(int i)
{	// преобразовать участок i-1..i в такое количество, чтобы каждый был не длиннее step
	// возвращает, в какой шаг преобразовался шаг i
	if(i<=0) return 0;
	int nLen = int(
		(_hypot(m_px[i]-m_px[i-1], m_py[i]-m_py[i-1]) + m_nStep-1) / m_nStep );
	nLen = max(1,nLen);
	// надо вставить nLen-1 записей
	for(int j=200-nLen; j>=i; j--)
	{
		int j1 = j+nLen-1;
		m_px[j1] = m_px[j];
		m_py[j1] = m_py[j];
		m_pangle[j1] = m_pangle[j];
		if(m_pprev[j1]>=i) m_pprev[j1] += nLen-1;
		m_pintensity[j1] = m_pintensity[j];
		m_pLineWidth[j1] = m_pLineWidth[j];
		m_pw1[j1] = m_pw1[j];
		m_pw2[j1] = m_pw2[j];
	}
	
	// теперь проинтерполируем промежуток
	int i0 = i-1;
	int i1 = i+nLen-1;
	for(int j=i; j<i1; j++)
	{
		double a = double(j-i0)/nLen;
		m_px[j] = int(0.5 + m_px[i0]*(1-a) + m_px[i1]*a);
		m_py[j] = int(0.5 + m_py[i0]*(1-a) + m_py[i1]*a);
		m_pangle[j] = m_pangle[i1];
		m_pprev[j] = j-1;
		m_pintensity[j] = int(0.5 + m_pintensity[i0]*(1-a) + m_pintensity[i1]*a);

		double w0 = m_pLineWidth[i0], w1 = m_pLineWidth[i1];
		if(w0<0) w0 = -(w0+1);
		if(w1<0) w1 = -(w1+1);
		double w = float( w0*(1-a) + w1*a );
		if(m_pLineWidth[i0]<0 || m_pLineWidth[i1]<0)
			w = -1 - w;
		m_pLineWidth[j] = w;
		m_pw1[j] = m_pw1[i0]*(1-a) + m_pw1[i1]*a;
		m_pw2[j] = m_pw2[i0]*(1-a) + m_pw2[i1]*a;
	}
	return i1;
}

bool CLineTracer::StoreData(_byte_ptr<int> pLineX, _byte_ptr<int> pLineY,
							_byte_ptr<byte> pLineExist,
							_byte_ptr<int> pLineIntensity,
							_byte_ptr<float> pLineWidth,
							int nFrameStart,
							int nFrames,
							int nFrameStep
							)
{
	for(int j=0; j<nFrames; j++)
	{
		int nFrame=j;
		if(j<200)
		{
			int posT = nFrameStep*nFrame + nFrameStart;
			pLineExist[posT]=1; // видна
			pLineX[posT]=m_px[j];
			pLineY[posT]=m_py[j];
			pLineIntensity[posT] = m_pintensity[j];
			pLineWidth[posT] = float(m_pLineWidth[j]);
			if(_isnan(pLineWidth[posT])) // !!! debug
			{
				int z = _fpclass(pLineWidth[posT]);
				static bool bStopHere=false;
				if(bStopHere) MessageBox(0, "NaN found", "Warning", MB_OK|MB_ICONWARNING); //!!!debug
			}
		}
	}
	return true;
}

bool CLineTracer::LoadData(_byte_ptr<int> pLineX, _byte_ptr<int> pLineY,
							_byte_ptr<byte> pLineExist,
							_byte_ptr<int> pLineIntensity,
							_byte_ptr<float> pLineWidth,
							int nFrameStart,
							int nFrames,
							int nFrameStep
							)
{
	for(int j=0; j<nFrames; j++)
	{
		int nFrame=j;
		if(j<200)
		{
			int posT = nFrameStep*nFrame + nFrameStart;
			m_px[j] = pLineX[posT];
			m_py[j] = pLineY[posT];
			m_pintensity[j] = pLineIntensity[posT];
			m_pLineWidth[j] = pLineWidth[posT];
			if(j>0)
			{
				double a=atan2(
					double(pLineY[posT]-pLineY[posT-nFrameStep]),
					double(pLineX[posT]-pLineX[posT-nFrameStep]));
				a+=PI*2;
				m_pangle[j]=int(a*256/2/PI+0.5) & 255;
			}
		}
	}
	m_pangle[0]=m_pangle[1];
	return true;
}

void GetCenter(IImage3Ptr ptrI, POINT *ptCenter)
{
	if(ptrI==0 || ptCenter==0) return;

	int cx_obj=0,cy_obj=0;
	ptrI->GetSize(&cx_obj,&cy_obj);
	POINT ptOffset={0,0};
	ptrI->GetOffset(&ptOffset);

	ptCenter->x = ptOffset.x + cx_obj/2; // центр прямоугольника - лучше бы центр маски
	ptCenter->y = ptOffset.y + cy_obj/2;

	double xsum=0, ysum=0;
	int area=0;
	for(int yy=0; yy<cy_obj; yy++)
	{
		byte* pMask=0;
		ptrI->GetRowMask(yy, &pMask);
		if(pMask)
		{
			for(int xx=0; xx<cx_obj; xx++)
			{
				if(pMask[xx]&128)
				{
					area++;
					xsum+=xx;
					ysum+=yy;
				}
			}
		}
	}
	if(area>0)
	{
		ptCenter->x = ptOffset.x + int(xsum/area);
		ptCenter->y = ptOffset.y + int(ysum/area);
	}
}

void CorrectEdge(IImage3Ptr ptrI, POINT *ptCenter, POINT *ptEdge)
{	// передвинуть точку ptEdge на край объекта
	if(ptrI==0 || ptCenter==0 || ptEdge==0) return;

	double dx = ptEdge->x - ptCenter->x;
	double dy = ptEdge->y - ptCenter->y;
	int nLen = max(int(ceil(_hypot(dx,dy))), 1);
	dx /= nLen; dy /= nLen;

	int cx_obj=0,cy_obj=0;
	ptrI->GetSize(&cx_obj,&cy_obj);
	POINT ptOffset={0,0};
	ptrI->GetOffset(&ptOffset);

	for(int r1=0; r1<100; r1++)
	{
		int xx = ptCenter->x - ptOffset.x + int(r1*dx);
		int yy = ptCenter->y - ptOffset.y + int(r1*dy);
		if(xx<0 || xx>=cx_obj || yy<0 || yy>=cy_obj) break;
		byte* pMask=0;
		ptrI->GetRowMask(yy, &pMask);
		if(!pMask) break;
		if(pMask[xx]<128) break;
	}
	ptEdge->x = ptCenter->x + int(r1*dx);
	ptEdge->y = ptCenter->y + int(r1*dy);
}
