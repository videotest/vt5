#include "stdafx.h"
#include "splitcircles.h"
#include "measure5.h"
#include "alloc.h"
#include "math.h"
#include "misc_utils.h"
#include "class_utils.h"

_ainfo_base::arg CSplitCirclesInfo::s_pargs[] = 
{       
	{"Objects",       szArgumentTypeObjectList, 0, true, true },
	{"SplitCircles",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

CSplitCircles::CSplitCircles(void)
{
}

CSplitCircles::~CSplitCircles(void)
{
}

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
// Скопировать все вычисляемые параметры из старого списка в новый
{
	if( in == 0 || out == 0)
		return;
	
	long lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer      *pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

static inline int mod(int a, int b)
{
	int c=a%b;
	return (c>=0) ? c : c+b ;
}

// точка под номером i; если i<0 или >=len - заворачиваем по кругу
#define pnt(i) (m_pcont[mod((i),m_len)])
//приращения площади и периметра на отрезке i1..i2
#define d_perim(i1,i2) _hypot(pnt(i2).x-pnt(i1).x, pnt(i2).y-pnt(i1).y)
#define d_area(i1,i2) (pnt(i1).x*pnt(i2).y - pnt(i2).x*pnt(i1).y)

IImage3Ptr CSplitCircles::CreateImageCopy(IImage3Ptr ptrSrc, _rect rect, bool bCopyMask)
{	// создать копию куска изображения
	DWORD dwFlags;
	ptrSrc->GetImageFlags(&dwFlags);
	bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

	_size size;
	ptrSrc->GetSize((int*)&size.cx,(int*)&size.cy);
	_point ptOffset;
	ptrSrc->GetOffset(&ptOffset);
	_rect rect_source;
	rect_source=_rect(ptOffset,size);

	if(rect==NORECT) rect=rect_source;

	rect.left = max( rect.left, rect_source.left );
	rect.top = max( rect.top, rect_source.top ); 
	rect.right = min( rect.right, rect_source.right );
	rect.bottom = min( rect.bottom, rect_source.bottom );

	if(rect.width()<1 || rect.height()<1) return 0;

	_bstr_t	bstrCC;
	int nPanes;
	{ // определим ColorConvertor и число пан
		IUnknown *punkCC = 0;
		ptrSrc->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( ptrSrc );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}

	IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr imgOut(punk2);
	punk2->Release();

	if(bIsVirtual)
	{
		imgOut->CreateVirtual( rect );
		imgOut->InitRowMasks();

		INamedDataObject2Ptr imgOutNDO(imgOut);
		INamedDataObject2Ptr imageNDO(ptrSrc);

		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		//imgOutNDO->GetObjectFlags( &dwFlags );
		imgOutNDO->SetParent( imageParent,  dwFlags);
	}
	else
	{
		imgOut->CreateImage(rect.width(),rect.height(),bstrCC, ::GetImagePaneCount( ptrSrc ) );
		imgOut->SetOffset(rect.top_left(),false);
	}

	imgOut->InitRowMasks();
	//imgOut->InitContours();

	int cx=rect.width();
	int cy=rect.height();
	int ofs_x=rect.left-ptOffset.x;
	int ofs_y=rect.top-ptOffset.y;
	// заполнение изображения image_obj
	if(!bIsVirtual)
	{ //скопировать данные
		for(int pane=0; pane<nPanes; pane++)
		{
			for (int y=0;y<cy;y++)
			{
				color *pcol_out;
				imgOut->GetRow(y,pane,&pcol_out);

				color *pcol_in;
				ptrSrc->GetRow(y+ofs_y,pane,&pcol_in);
				pcol_in+=ofs_x;

				CopyMemory(pcol_out,pcol_in,cx*sizeof(color));
			}
		}
	}

	if(bCopyMask)
	{
		for (int y=0;y<cy;y++)
		{
			byte *pmask_out;
			imgOut->GetRowMask(y,&pmask_out);

			byte *pmask_in;
			ptrSrc->GetRowMask(y+ofs_y,&pmask_in);
			pmask_in+=ofs_x;

			CopyMemory(pmask_out,pmask_in,cx*sizeof(byte));
		}
		imgOut->InitContours();
	}

	return imgOut;
}

static void Line(byte **dst, int x0, int y0, int x1, int y1, byte c, int cx, int cy)
{
	x0=min(max(x0,0),cx-1);
	x1=min(max(x1,0),cx-1);
	y0=min(max(y0,0),cy-1);
	y1=min(max(y1,0),cy-1);
	
	int L=max(abs(x1-x0),abs(y1-y0))*2+1;
	if(L==0) L=1;
	int dx=(x1-x0)*65536/L, dy=(y1-y0)*65536/L;
	int x=x0*65536, y=y0*65536;
	for(int i=0; i<=L; i++)
	{
		dst[y/65536][x/65536]=c;
		y+=dy; x+=dx;
	}
}

static void FillContourExtern(byte **dst, int cx, int cy, byte old_color, byte new_color)
{	// заполнить все, что снаружи от контура
	// оба цвета не должны совпадать с цветом контура!!!
	for(int x=0; x<cx; x++)
	{
		if(dst[0][x]==old_color) dst[0][x]=new_color;
		if(dst[cy-1][x]==old_color) dst[cy-1][x]=new_color;
	}
	for(int y=0; y<cy; y++)
	{
		if(dst[y][0]==old_color) dst[y][0]=new_color;
		if(dst[y][cx-1]==old_color) dst[y][cx-1]=new_color;
	}
	bool bProcess=true;
	while(bProcess)
	{
		bProcess=false;
		for(int y=1; y<cy; y++)
		{
			for(int x=1; x<cx; x++)
			{
				if(dst[y][x]==old_color && (dst[y][x-1]==new_color || dst[y-1][x]==new_color))
				{
					dst[y][x]=new_color;
					bProcess=true;
				}
			}
		}
		for(int y=cy-2; y>=0; y--)
		{
			for(int x=cx-1; x>=0; x--)
			{
				if(dst[y][x]==old_color && (dst[y][x+1]==new_color || dst[y+1][x]==new_color))
				{
					dst[y][x]=new_color;
					bProcess=true;
				}
			}
		}
	}

}

IImage3Ptr CSplitCircles::CreateImagePart(IImage3Ptr ptrSrc, int i1, int i2)
{
	// отрезать кусок объекта линией от точки i1 к точке i2
	// для отрезания второй половины - вызвать с параметрами i2, i1

	if(i2<i1) i2+=m_len;

	_rect rect(10000,10000,-10000,-10000);

	//определение размеров
	for(int i=i1; i<=i2; i++)
	{
		int x=pnt(i).x, y=pnt(i).y;
		rect.left = min( rect.left, x ); rect.top = min( rect.top, y ); 
		rect.right = max( rect.right, x ); rect.bottom = max( rect.bottom, y );
	}
	rect.right++; rect.bottom++;

	IImage3Ptr img = CreateImageCopy(ptrSrc, rect, true);
	img->InitRowMasks();

	_point ofs;
	img->GetOffset(&ofs);

	int cx=rect.width(), cy=rect.height();
	smart_alloc(mask,byte*,cy);
	for(int y=0; y<cy; y++)	img->GetRowMask( y, mask+y );

	for(int y=0; y<cy; y++)
	{
		FillMemory(mask[y],cx*sizeof(byte),255);
	}

	for(int i=i1; i<i2; i++)
	{
		Line(mask, pnt(i).x-ofs.x,pnt(i).y-ofs.y,pnt(i+1).x-ofs.x,pnt(i+1).y-ofs.y, 254, cx, cy);
	}
	Line(mask, pnt(i2).x-ofs.x,pnt(i2).y-ofs.y,pnt(i1).x-ofs.x,pnt(i1).y-ofs.y, 254, cx, cy);
	if(1)FillContourExtern(mask,cx,cy,255,0);
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(mask[y][x]==254) mask[y][x]=255;
		}
	}

	img->InitContours();

	return img;
}

static IMeasureObjectPtr ListInsertImage(INamedDataObject2Ptr ptrObjList, IImage3Ptr ptrImage)
{
	IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	IMeasureObjectPtr objectOut(punk );
	punk->Release();
	if(objectOut==0) return 0;
	objectOut->SetImage( ptrImage );

	INamedDataObject2Ptr ptrObject(objectOut);
	if(ptrObject==0) return 0;
	DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
	ptrObject->SetParent(ptrObjList, dwFlags);

	return objectOut;
}

double CSplitCircles::CalcAngle(int i, int step)
{
	step=max(step,1);
	double dx1 = pnt(i).x - pnt(i-step).x;
	double dy1 = pnt(i).y - pnt(i-step).y;
	double dx2 = pnt(i+step).x - pnt(i).x;
	double dy2 = pnt(i+step).y - pnt(i).y;
	double si = dx1*dy2 - dx2*dy1; // векторное произведение
	double co = dx1*dx2 + dy1*dy2; // скалярное произведение
	return atan2(si,co);
}

bool CSplitCircles::SplitImage(IImage3* image, IUnknown** ppDst1, IUnknown** ppDst2)
{ // разрезать на два, false - не удалось
	if(ppDst1==0 || ppDst2==0 || image==0) return false; //just for case

	*ppDst1 = 0;
	*ppDst2 = 0;

	_rect cr;
	{
		_size size;
		image->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		image->GetOffset(&ptOffset);
		cr=_rect(ptOffset,size);
	}

	int nContour=0;
	image->GetContoursCount(&nContour);
	if(nContour<=0) return false; // ошибочный объект
	Contour* pContour;
	image->GetContour(0, &pContour); //первый контур
	m_len = pContour->nContourSize;
	m_pcont = pContour->ppoints;
	if(m_len<50) return false; //мелочевку не трогаем

	smart_alloc(pangle,double,m_len);
	smart_alloc(parea_int,double,m_len*2); //интеграл, по которому быстро считаются площади
	smart_alloc(pperim_int,double,m_len*2); //интеграл, по которому быстро считаются периметры
	// по всем точкам контура - посчитаем угол при точке
	// и направление контура
	double s=0, p=0;
	int di=10;
	for(int i=0; i<m_len; i++)
	{
		parea_int[i] = s;
		pperim_int[i] = p;
		s += d_area(i,i+1);
		p += d_perim(i,i+1);
		pangle[i] = CalcAngle(i,di);
	}
	if(s<0)
	{ // если контур не в ту сторону
		for(int i=0; i<m_len; i++) pangle[i] = -pangle[i];
	}
	for(int i=0; i<m_len; i++)
	{
		parea_int[i+m_len] = parea_int[i] + s;
		pperim_int[i+m_len] = pperim_int[i] + s;
	}

	//перебор всех хорд, вычисление функции качества разреза, поиск наилучшей
	int best_i1=-1, best_i2=-1;
	double best_q=1.0; // "качество" разреза, чем меньше, тем лучше; это - граничный уровень
	double best_a1, best_a2;
	int nMaxDist=min(m_lMaxPerimeter,m_len);
	for(int i1=0; i1<m_len; i1+=5)
	{
		//for(int i2=i1+1; i2<m_len; i2+=5)
		for(int j2=5; j2<nMaxDist; j2+=5)
		{
			int i2 = mod(i1+j2,m_len);
#include "test_chord.inl"
		}
	}
	if(mod(best_i2-best_i1, m_len) >= m_lMaxPerimeter-5)
	{ // если поймали точку, соответствующую самой длинной допустимой дуге - отбросим, все равно надо больше
		best_i1=-1;
		best_i2=-1;
	}

	if(best_i1>=0)
	{
		int start_i1=best_i1;
		int start_i2=best_i2;
		for(int j1=-5; j1<5; j1++)
		{
			int i1 = mod(start_i1+j1,m_len);
			for(int j2=-5; j2<5; j2++)
			{
				int i2 = mod(start_i2+j2,m_len);
	#include "test_chord.inl"
			}
		}
	}

	// создание новых изображений/объектов
	double perim_int12 = d_perim(best_i1,best_i2);
	int a_step=int(perim_int12*m_fAngleDetectorChordCoeff)+m_lMinAngleDetector; //всегда не менее 3 пикселей
	
	double a1=CalcAngle(best_i1,a_step);
	double a2=CalcAngle(best_i2,a_step);
	if(best_i1>=0)
	{
		IImage3Ptr image1=CreateImagePart(image,best_i1,best_i2);
		if(image1!=0) image1->AddRef();
		*ppDst1 = image1;
		IImage3Ptr image2=CreateImagePart(image,best_i2,best_i1);
		if(image2!=0) image2->AddRef();
		*ppDst2 = image2;
		return true;
	}
	return false;
}


bool CSplitCircles::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());
	
	if(objectsOut==0 || objectsIn==0) return false;
	
	long nCountIn; objectsIn->GetChildsCount(&nCountIn);
	if(!nCountIn) return false;

	m_lMaxSplits = GetValueInt(GetAppUnknown(), "\\SplitCircles", "MaxSplits", 10);
	m_fMinArea = GetValueDouble(GetAppUnknown(), "\\SplitCircles", "MinArea", 30.0);
	m_lMaxPerimeter = GetValueInt(GetAppUnknown(), "\\SplitCircles", "MaxPerimeter", 1000);
	m_lMinAngleDetector = GetValueInt(GetAppUnknown(), "\\SplitCircles", "MinAngleDetector", 10);
	m_fAngleDetectorChordCoeff = GetValueDouble(GetAppUnknown(), "\\SplitCircles", "AngleDetectorChordCoef", 0.0);
	m_fAngleThreshold = GetValueDouble(GetAppUnknown(), "\\SplitCircles", "AngleThreshold", 12.0);
	m_fAngleThreshold = - m_fAngleThreshold * 3.14159/180;
	m_bstrClassification = GetValueString(GetAppUnknown(), "\\SplitCircles", "Classification", "splitted.ini");
	m_lClassWhole = GetValueInt(GetAppUnknown(), "\\SplitCircles", "ClassWhole", -2);
	m_lClassSplitted = GetValueInt(GetAppUnknown(), "\\SplitCircles", "ClassSplitted", -2);

	int nN=0;
	StartNotification(nCountIn);
	
	//CopyParams(objectsIn, objectsOut);

	int nSplitCountTotal=0;
	
	long pos; objectsIn->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		objectsIn->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		
		IUnknownPtr sptr2;
		object->GetImage(&sptr2);
		IImage3Ptr image(sptr2);

		_list_t2 <IImage3Ptr> split_query;

		int nSplitCount=0;
		long lClass = m_lClassWhole; // первоначально объект целый

		while(image!=0)
		{
			IUnknown *punk1=0, *punk2=0;
			bool bSplitted=false;
			if(nSplitCount<m_lMaxSplits) bSplitted = SplitImage(image, &punk1, &punk2);
			if(bSplitted)
			{
				split_query.insert(IImage3Ptr(punk1)); //добавим в хвост списка
				split_query.insert(IImage3Ptr(punk2));
				//ListInsertImage(objectsOut, IImage3Ptr(punk1));
				//ListInsertImage(objectsOut, IImage3Ptr(punk2));
				nSplitCount++;
			}
			else
			{
				IImage3Ptr image_out=CreateImageCopy(image);
				IMeasureObjectPtr obj = ListInsertImage(objectsOut, image_out);
				if(lClass != -2)
				{ // установить класс
					set_object_class(ICalcObjectPtr(obj), lClass, m_bstrClassification);
				}
			}
			if(punk1!=0) punk1->Release();
			if(punk2!=0) punk2->Release();

			long pos=split_query.head();
			if(pos)
			{
				image=split_query.get(pos);
				split_query.remove(pos);
			}
			else
			{
				image=0;
			}
			lClass = m_lClassSplitted; // на 2 и последующих проходах все объекты резаные
		}
		nSplitCountTotal += nSplitCount;

		Notify(nN++);
	}

	GUID guidNewBase;
	INamedDataObject2Ptr sptrNO;
	objectsIn->GetBaseKey(&guidNewBase);
	objectsOut->SetBaseKey(&guidNewBase);

	SetValue(GetAppUnknown(), "\\SplitCircles", "SplitCountTotal", long(nSplitCountTotal));

	FinishNotification();

	return true;
}
