#include "stdafx.h"
#include "AverageObjects.h"
#include "misc_utils.h"
#include <math.h>
#include "measure5.h"
#include "DocView5.h"
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "Chromosome.h"
#include "dpoint.h"
#include "class_utils.h"
#include "aam_utils.h"
#include "cgh_int.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              AverageObjects
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CAverageObjectsInfo::s_pargs[] =
{
	{"AutoScaleX",    szArgumentTypeInt, "0", true, false },
	{"AutoScaleY",    szArgumentTypeInt, "1", true, false },
	{"Averaged",    szArgumentTypeObjectList, 0, false, true },
	{"Source0",    szArgumentTypeObjectList, 0, true, true },
	{"Source1",    szArgumentTypeObjectList, 0, true, true },
	{"Source2",    szArgumentTypeObjectList, 0, true, true },
	{"Source3",    szArgumentTypeObjectList, 0, true, true },
	{"Source4",    szArgumentTypeObjectList, 0, true, true },
	{"Source5",    szArgumentTypeObjectList, 0, true, true },
	{"Source6",    szArgumentTypeObjectList, 0, true, true },
	{"Source7",    szArgumentTypeObjectList, 0, true, true },
	{"Source8",    szArgumentTypeObjectList, 0, true, true },
	{"Source9",    szArgumentTypeObjectList, 0, true, true },
	{"Source10",    szArgumentTypeObjectList, 0, true, true },
	{"Source11",    szArgumentTypeObjectList, 0, true, true },
	{"Source12",    szArgumentTypeObjectList, 0, true, true },
	{"Source13",    szArgumentTypeObjectList, 0, true, true },
	{"Source14",    szArgumentTypeObjectList, 0, true, true },
	{"Source15",    szArgumentTypeObjectList, 0, true, true },
	{"Source16",    szArgumentTypeObjectList, 0, true, true },
	{"Source17",    szArgumentTypeObjectList, 0, true, true },
	{"Source18",    szArgumentTypeObjectList, 0, true, true },
	{"Source19",    szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

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

static IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	long lPos = 0;
	sptrNDOParent->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrChild;
		sptrNDOParent->GetNextChild(&lPos, &sptrChild);
		if (sptrChild!=0)
		{
			if (::CheckInterface(sptrChild, guid))
				return sptrChild;
		}
	}
	return NULL;
}

typedef _ptr_t2<int> int_array;

static int AddImg(IImage3Ptr image,
				   int *out_img, int *out_msk,
				   int out_cx, int out_cy, int out_nPanes,
				   double *sum1, double *sum2, // массивы [out_cx] сумм значений и квадратов значений графика 3 паны
				   double &avgsum1, double &avgsum2, // суммы средних по оси значений графика 3 паны и их квадратов
				   double &sigmasum1, double &sigmasum2, // суммы СКО (по оси) значений графика 3 паны и их квадратов
				   _bstr_t out_ccname)
{	//возвращаем, сколько раз прибавили
	//узнаем ColorConverter и число пан в объектах
	int nPanes=0;
	_bstr_t	bstrCC;
	IUnknown *punkCC = 0;
	image->GetColorConvertor( &punkCC );
	IColorConvertor2Ptr	ptrCC( punkCC );
	punkCC->Release();
	
	nPanes = ::GetImagePaneCount( image );

	
	BSTR bstr;
	ptrCC->GetCnvName( &bstr );
	bstrCC = bstr;
	::SysFreeString( bstr ); 

	if(bstrCC!=out_ccname) return 0; //не совпадает color convertor
	if(nPanes!=out_nPanes) return 0; //ну, это уже перестраховка

	int cx,cy;
	image->GetSize(&cx,&cy);

	if(cx<1 || cy<1 || out_cx<1 || out_cy<1) return 0; //не связываемся с мелочевкой

	smart_alloc(srcMask, byte*, cy);
	{for(int y=0; y<cy; y++)
	{
		image->GetRowMask(y, srcMask+y);
	}}

	smart_alloc(srcRows, color*, cy*nPanes);
	{for(int pane=0; pane<nPanes; pane++)
	{
		for(int y=0; y<cy; y++)
		{
			image->GetRow(y, pane, srcRows+pane*cy+y);
		}
	}}
	
	double out_x0=(out_cx-1)/2.0, out_y0=(out_cy-1)/2.0;
	double x0=(cx-1)/2.0, y0=(cy-1)/2.0;
	double scale_x=double(cx)/out_cx;
	double scale_y=double(cy)/out_cy;

	smart_alloc(row_x0,int,cy);
	smart_alloc(row_x1,int,cy);
	smart_alloc(row_dx,int,cy);
	int dx=int(x0-out_x0), dy=int(y0-out_y0);
	{
		for(int iy=0; iy<cy; iy++)
		{
			row_x0[iy]=cx; row_x1[iy]=0;
			for(int ix=0; ix<cx; ix++)
			{
				if(srcMask[iy][ix]>=128)
				{ //медленно. можно быстрее
					if(row_x0[iy]>ix) row_x0[iy]=ix;
					if(row_x1[iy]<ix) row_x1[iy]=ix;
				}
			}
			if(row_x0[iy]>row_x1[iy]) row_x0[iy]=row_x1[iy]=cx/2; //если не было точек - на середину
			row_dx[iy]=(row_x0[iy]+row_x1[iy]-(cx-1))/2;
		}
	}
	//неплохо б еще median filter для порядку... или хотя бы края поправить

	{ //blur filter
		const int L=8;
		smart_alloc(row_dx1,int,cy);
		for(int i=0; i<cy; i++) row_dx1[i]=row_dx[i];
		if(0) for(i=L; i<cy-L; i++)
		{
			int S=L;
			for(int j=-L; j<L; j++) S+=row_dx1[i+j];
			row_dx[i]=S/(L+L+1);
		}
		for(i=0; i<L; i++) row_dx[i]=row_dx[L];
		for(i=0; i<L; i++) row_dx[cy-1-i]=row_dx[cy-1-L];
	}

	double v1sum1=0, v1sum2=0;
	int v1count=0;
	for(int y=0; y<out_cy; y++)
	{
		//int iy=y+dy;
		int iy=int(y*scale_y+0.5);
		if(iy<0 || iy>=cy) continue;

		int v0=0, v1=0;

		for(int x=0; x<out_cx; x++)
		{
			//int ix=x+dx;
			//int ix=int(x*scale_x+0.5);
			int ix;
			ix=int(x*scale_x+0.5+row_dx[iy]);

			if(ix>0 && ix<cy)	//поправить бы - а то за пределами прямоугольников почернеет
			{
				if(nPanes>3 && srcMask[iy][ix]>=128)
				{ // если есть 3 пана (ratio) - посчитать среднее по строке
					int c=srcRows[3*cy+iy][ix];
					if(c==0) c=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
					v0++;
					v1+=c;
				}

				if(srcMask[iy][ix]>=128) out_msk[y*out_cx+x]++;
				for(int pane=0; pane<nPanes; pane++)
				{ //можно бы билинейную фильтрацию сделать
					int c=srcRows[pane*cy+iy][ix];
					if(pane==3 && c==0) c=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
					out_img[(pane*out_cy+y)*out_cx+x]+=c;
				}
			}

			//ix=cx-1-ix;
			ix=int(cx-1-x*scale_x+0.5+row_dx[iy]);

			if(ix>0 && ix<cy)	//поправить бы - а то за пределами прямоугольников почернеет
			{
				if(srcMask[iy][ix]>=128) out_msk[y*out_cx+x]++;
				for(int pane=0; pane<nPanes; pane++)
				{ //можно бы билинейную фильтрацию сделать
					int c=srcRows[pane*cy+iy][ix];
					if(pane==3 && c==0) c=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
					out_img[(pane*out_cy+y)*out_cx+x]+=c;
				}
			}
		}
		if(v0>0) v1/=v0;
		else v1=128*256;
		sum1[y] += v1*2;
		sum2[y] += double(v1)*v1*2;
		v1sum1 += v1;
		v1sum2 += v1*v1;
		v1count++;
	}
	v1sum1 /= max(v1count,1);
	v1sum2 /= max(v1count,1);
	
	avgsum1 += v1sum1*2;
	avgsum2 += v1sum1*v1sum1*2;

	double sigma = v1sum2 - v1sum1*v1sum1;
	sigma = sqrt(max(sigma,0));

	sigmasum1 += sigma*2;
	sigmasum2 += sigma*sigma*2;

	return 2;
}

typedef _list_t2 <IImage3*, FreeReleaseUnknown> t_ImageList;

static int GetPaneCount(IImage3Ptr image1)
{
	//узнаем ColorConverter и число пан в объектах
	int nPanes1=0;
	IUnknown *punkCC1 = 0;
	image1->GetColorConvertor( &punkCC1 );
	IColorConvertor2Ptr	ptrCC1( punkCC1 );
	punkCC1->Release();
	
	nPanes1 = ::GetImagePaneCount( image1 );
	
	BSTR	bstr1;
	ptrCC1->GetCnvName( &bstr1 );
	bstr_t ccname1 = bstr1;
	::SysFreeString( bstr1 ); 

	return nPanes1;
}

static int isCompatible(IImage3Ptr image1, IImage3Ptr image2)
{
	//узнаем ColorConverter и число пан в объектах
	int nPanes1=0;
	IUnknown *punkCC1 = 0;
	image1->GetColorConvertor( &punkCC1 );
	IColorConvertor2Ptr	ptrCC1( punkCC1 );
	punkCC1->Release();
	
	nPanes1 = ::GetImagePaneCount( image1 );

	
	BSTR	bstr1;
	ptrCC1->GetCnvName( &bstr1 );
	bstr_t ccname1 = bstr1;
	::SysFreeString( bstr1 ); 

	int cx=0,cy=0;
	image1->GetSize(&cx, &cy);
	if(cx<=2 || cy<=2) return false; // слишком мелкие недопустимы 

	//узнаем ColorConverter и число пан в объектах
	int nPanes2=0;
	IUnknown *punkCC2 = 0;
	image2->GetColorConvertor( &punkCC2 );
	IColorConvertor2Ptr	ptrCC2( punkCC2 );
	punkCC2->Release();
	
	nPanes2 = ::GetImagePaneCount( image2 );
	
	BSTR	bstr2;
	ptrCC2->GetCnvName( &bstr2 );
	bstr_t ccname2 = bstr2;
	::SysFreeString( bstr2 );
	
	return (nPanes1==nPanes2 && ccname1==ccname2);
}

bool CAverageObjectsFilter::InvokeFilter()
{
	INamedDataObject2Ptr objectsOut(GetDataResult());
	
	if(objectsOut==0) return false;
	
	long nCountOut=::class_count();
	if(!nCountOut) return false;
	
	int nN=0;
	StartNotification(1000);


	{
		INamedDataPtr sptrNDO = objectsOut;
		if( sptrNDO == 0 ) objectsOut->InitAttachedData();
	}

	smart_alloc(in_images,t_ImageList,nCountOut);

	//CopyParams(objectsIn, objectsOut);
	
	//-----------------

	long nListCount=0;

	for(int nArg=0; nArg<20; nArg++)
	{
		char	sz[255];
		wsprintf( sz, "Source%d", nArg );
		INamedDataObject2Ptr objectsIn(GetDataArgument(sz));
		if( objectsIn==0 ) continue; // нет аргумента 

		nListCount++; //пересчитаем переданные листы

		long pos; objectsIn->GetFirstChildPosition(&pos);
		while( pos ) //по всем объектам
		{
			Notify(nN++);
			IUnknownPtr sptr;
			objectsIn->GetNextChild(&pos, &sptr);
			IMeasureObjectPtr object(sptr);
			if(object==0) continue;
			ICalcObjectPtr calcObj(sptr);
			if(calcObj==0) continue;

			int objClass=get_object_class(calcObj);
			if(objClass<0 || objClass>=nCountOut) continue;

			IImage3Ptr image;
			if(0)
			{
				//получим image объекта
				IUnknownPtr sptr2;
				object->GetImage(&sptr2);
				image=IImage3Ptr(sptr2);
				if(image==0) continue;
			}
			else
			{	//получим выпрямленный
				// Find axis
				IUnknownPtr ptr3 = find_child_by_interface(object, IID_IChromosome);
				IChromosomePtr ptrC(ptr3);
				if(ptrC==0) continue;

				IUnknownPtr ptrUnkErector(LPOLESTR(_bstr_t(pszErectCCProgID)));
				IChromoConvertPtr ptrErector=IChromoConvertPtr(ptrUnkErector);
				
				IUnknownPtr sptr2;
				ptrErector->CCGetImage(ptrC, &sptr2);

				image=IImage3Ptr(sptr2);
				if(image==0) continue;
			}
			image->AddRef();
			in_images[objClass].insert(image); // внесем в список
		}
	}

	//--------------------------------
	int x0=10, y0=10;
	for (int objClass=0; objClass<nCountOut; objClass++)
	{
		long lpos=in_images[objClass].head();
		if(lpos==0) continue;
		IImage3Ptr image0 = in_images[objClass].get(lpos); // первое изображение
		//узнаем ColorConverter и число пан в объектах
		int nPanes=0;
		IUnknown *punkCC = 0;
		image0->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();		
		nPanes = ::GetImagePaneCount( image0 );
		
		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		_bstr_t out_ccname = bstr;
		::SysFreeString( bstr ); 
		
		int cx,cy, cxsum=0, cysum=0, n=0;
		lpos=in_images[objClass].head();
		while(lpos)	// посчитаем размер результирующего изображения
		{
			IImage3Ptr image = in_images[objClass].next(lpos);
			if(! isCompatible(image, image0) ) continue;
			image->GetSize(&cx, &cy);
			cxsum+=cx; cysum+=cy; n++;
		}
		if(n==0) continue;
		cxsum/=n; cysum/=n;

		if(cxsum<3 || cysum<3) continue;

		smart_alloc(out_img, int, cxsum*cysum*nPanes); smart_out_img.zero();
		smart_alloc(out_msk, int, cxsum*cysum); smart_out_msk.zero();
		smart_alloc(r_sum1, double, cysum);
		smart_alloc(r_sum2, double, cysum);
		{
			for(int i=0; i<cysum; i++)
			{
				r_sum1[i]=0;
				r_sum2[i]=0;
			}
		}
		double ravg_sum1=0, ravg_sum2=0; // сумма средних значений графика ratio хромосом и их квадратов
		double rsigma_sum1=0, rsigma_sum2=0; // сумма СКО значений графика ratio хромосом и их квадратов

		int out_count=0;
		lpos=in_images[objClass].head();
		while(lpos)	// обсчитаем само изображение
		{
			IImage3Ptr image = in_images[objClass].next(lpos);
			if(! isCompatible(image, image0) ) continue;
			int nAdded = AddImg(image,
				out_img, out_msk,
				cxsum, cysum,
				nPanes,
				r_sum1, r_sum2,
				ravg_sum1, ravg_sum2,
				rsigma_sum1, rsigma_sum2,
				out_ccname);
			
			out_count+=nAdded;
		}

		if(out_count==0) continue;

		{
			for(int i=0; i<cysum; i++)
			{
				r_sum1[i]/=out_count;
				r_sum2[i]/=out_count;
				//r_sum2[i]-=r_sum1[i]*r_sum1[i];
			}
		}
		ravg_sum1/=out_count; // получили среднее значение среднего по профилю
		ravg_sum2/=out_count;	// и среднеквадратичное
		rsigma_sum1/=out_count; // получили среднее значение СКО по профилю
		rsigma_sum2/=out_count;	// и среднеквадратичное


		IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
		IImage3Ptr imgOut(punk2);
		punk2->Release();

		cx=cxsum, cy=cysum;
		int count=out_count;

		imgOut->CreateImage(cx,cy,out_ccname, ::GetImagePaneCount( image0 ) );
		imgOut->SetOffset(_point(-x0-cx,y0-cy),false);

		imgOut->InitRowMasks();

		int m1th=out_count*4/8;
		int bIsEmpty=1;
		for(int y=0; y<cy; y++)
		{
			byte *m; imgOut->GetRowMask(y,&m);
			int *m1=out_msk+y*cx;
			for(int x=0; x<cx; x++)
			{
				if(*m1>m1th)
				{
					*m=255;
					bIsEmpty=0; // проверка, не пустой ли image. Когда у AM будет заведомо работать на пустых - уберем
				}
				else *m=0;
				m++; m1++;
			}
		}
		if(bIsEmpty) continue; // проверка, не пустой ли image. Когда у AM будет заведомо работать на пустых - уберем

		for(int pane=0; pane<nPanes; pane++)
		{
			for(int y=0; y<cy; y++)
			{
				color *p; imgOut->GetRow(y,pane,&p);
				int *p1=out_img+(pane*cy+y)*cx;
				for(int x=0; x<cx; x++, p++, p1++)
				{
					*p=(*p1)/count;
				}
			}
		}

		imgOut->InitContours();
		x0+=cx+20;

		//create object
		IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
		IMeasureObjectPtr objOut(punk );
		punk->Release();
		
		objOut->SetImage( imgOut );

		ICalcObjectPtr calcObj(objOut);
		if(calcObj!=0) set_object_class(calcObj, objClass);

		{
			DWORD dwFlags;
			INamedDataObject2Ptr ptrObject(objOut);
			if(ptrObject!=0)
			{
				ptrObject->GetObjectFlags(&dwFlags);
				ptrObject->SetParent(objectsOut, dwFlags);
			}
		}

		{	//запишем info в NamedData листа
			GUID guidObj={0};
			{
				INumeredObjectPtr ptrNum(calcObj);
				if(ptrNum!=0) ptrNum->GetKey(&guidObj);
			}
			BSTR	bstr = 0;
			::StringFromCLSID( guidObj, &bstr );
			_bstr_t bstrGuid = bstr;	
			::CoTaskMemFree( bstr );	bstr = 0;

			char szClassNum[20];
			_itoa(objClass, szClassNum, 10);

			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghAveraged, long(out_count/2));

			smart_alloc(deviation, double, cy);
			{
				for(int i=0; i<cy; i++)
				{
					//deviation[i]=i*(cy-1.0-i)/cy/cy*0.5;
					double d=(r_sum2[i]-r_sum1[i]*r_sum1[i]);
					if(d<=0) d=0;
					deviation[i]=sqrt(d)/32768;
					if(deviation[i]>1)
					{
						deviation[i]=1;
					}
				}
			}
			double avg_deviation; // отклонение средних
			{
				double d = ravg_sum2 - ravg_sum1*ravg_sum1;
				if(d<=0) d=0; // на всякий случай
				avg_deviation = sqrt(d)/32768; // приведем к масштабу 1.
			}

			double sigma_mean; // среднее значение СКО профилей
			double sigma_deviation; // СКО от СКО ;-)
			{
				sigma_mean = rsigma_sum1/32768; // приведем к масштабу 1.
				double d = rsigma_sum2 - rsigma_sum1*rsigma_sum1;
				if(d<=0) d=0; // на всякий случай
				sigma_deviation = sqrt(d)/32768; // приведем к масштабу 1.
			}

			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghDeviationSize, long(cy));

			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghDeviation, (byte *)(deviation), sizeof(*deviation)*cy);

			// а теперь параметры для определения плохих
			// их дублируем - и под guid'ом, и под номером класса
			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghAvgDeviation, avg_deviation);
			::SetValue(objectsOut,
				_bstr_t(CGH_ROOT "\\" szCghObjects "\\") + szClassNum,
				szCghAvgDeviation, avg_deviation);

			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghSigmaMean, sigma_mean);
			::SetValue(objectsOut,
				_bstr_t(CGH_ROOT "\\" szCghObjects "\\") + szClassNum,
				szCghSigmaMean, sigma_mean);

			::SetValue(objectsOut,
				CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
				szCghSigmaDeviation, sigma_deviation);
			::SetValue(objectsOut,
				_bstr_t(CGH_ROOT "\\" szCghObjects "\\") + szClassNum,
				szCghSigmaDeviation, sigma_deviation);
		}

	}
	//--------------------------------
	
	if(nListCount==1)
	{
		for(int nArg=0; nArg<20; nArg++)
		{
			char	sz[255];
			wsprintf( sz, "Source%d", nArg );
			INamedDataObject2Ptr objectsIn(GetDataArgument(sz));
			if( objectsIn==0 ) continue; // нет аргумента 
			GUID guidNewBase;
			objectsIn->GetBaseKey(&guidNewBase);
			objectsOut->SetBaseKey(&guidNewBase);
		}
	}
	else
	{
		GUID guidNewBase;
		::CoCreateGuid(&guidNewBase);
		objectsOut->SetBaseKey(&guidNewBase);
	}

	FinishNotification();

	::SetValue(objectsOut, CGH_ROOT, szCghAveraged, nListCount);

	m_ActivateMe=objectsOut;

	/*
	IDataContextPtr sptrContext( m_ptrTarget );
	if(sptrContext)
	{
		sptrContext->SetActiveObject(_bstr_t(szTypeObjectList), objectsOut, 0);
	}
	*/
	
	return true;
}

bool CAverageObjectsFilter::IsAvaible()
{
	for(int nArg=0; nArg<20; nArg++)
	{
		char	sz[255];
		wsprintf( sz, "Source%d", nArg );
		INamedDataObject2Ptr objectsIn(GetDataArgument(sz));
		if( objectsIn!=0 )
		{	//есть хоть один аргумент - можно работать
			long pos; objectsIn->GetFirstChildPosition(&pos);
			while( pos ) //по всем объектам
			{
				IUnknownPtr sptr;
				objectsIn->GetNextChild(&pos, &sptr);
				IMeasureObjectPtr object(sptr);
				if(object==0) continue;
				ICalcObjectPtr calcObj(sptr);
				if(calcObj==0) continue;

				int objClass=get_object_class(calcObj);
				if(objClass<0) continue;

				return true; //если есть хоть один объект с определенным классом - можно работать
			}
		}
	}
	return false;
}

HRESULT CAverageObjectsFilter::GetActionState( DWORD *pdwState )
{
	HRESULT r=CFilter::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

HRESULT CAverageObjectsFilter::DoUndo()
{
	if(m_PrevActive!=0)
	{
		// get active context
		IDataContext2Ptr sptrContext;
		
		IDocumentSitePtr sptrDoc = m_ptrTarget;
		if (sptrDoc != 0)
		{
			// get active view from document
			IUnknown * punkActiveView = 0;
			sptrDoc->GetActiveView(&punkActiveView);
			
			// and set it to context
			sptrContext = punkActiveView;
			if (punkActiveView)
				punkActiveView->Release();
			
		}
		if (sptrContext != 0)
			sptrContext->SetActiveObject( 0, m_PrevActive, aofActivateDepended);
	}
	HRESULT r = CFilter::DoUndo();
	return r;
}

HRESULT CAverageObjectsFilter::DoRedo()
{
	HRESULT r = CFilter::DoRedo();
	if(m_ActivateMe!=0)
	{
		// get active context
		IDataContext2Ptr sptrContext;
		
		IDocumentSitePtr sptrDoc = m_ptrTarget;
		if (sptrDoc != 0)
		{
			// get active view from document
			IUnknown * punkActiveView = 0;
			sptrDoc->GetActiveView(&punkActiveView);
			
			// and set it to context
			sptrContext = punkActiveView;
			if (punkActiveView)
				punkActiveView->Release();
			
		}
		if (sptrContext != 0)
		{
			sptrContext->SetActiveObject( 0, m_ActivateMe, aofActivateDepended);
			IUnknown *punk=0;
		}

	}
	return r;
}

HRESULT CAverageObjectsFilter::DoInvoke()
{
	HRESULT r = CFilter::DoInvoke();
	if(m_ActivateMe!=0)
	{
		// get active context
		IDataContext2Ptr sptrContext;
		
		IDocumentSitePtr sptrDoc = m_ptrTarget;
		if (sptrDoc != 0)
		{
			// get active view from document
			IUnknown * punkActiveView = 0;
			sptrDoc->GetActiveView(&punkActiveView);
			
			// and set it to context
			sptrContext = punkActiveView;
			if (punkActiveView)
				punkActiveView->Release();
			
		}
		if (sptrContext != 0)
		{
			IUnknown *punk=0;
			sptrContext->GetActiveObject( _bstr_t(szTypeObjectList), &punk);
			m_PrevActive=punk;
			if(punk!=0) punk->Release();
			sptrContext->SetActiveObject( 0, m_ActivateMe, aofActivateDepended);
		}

	}

	return r;
}


CAverageObjectsFilter::CAverageObjectsFilter()
{
	m_ActivateMe=0;
	m_PrevActive=0;
}

CAverageObjectsFilter::~CAverageObjectsFilter()
{
}
