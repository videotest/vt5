#include "stdafx.h"
#include "AddFrame.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "Params.h"

_ainfo_base::arg CParticlesAddFrameInfo::s_pargs[] =
{
	{"Current",    szArgumentTypeObjectList, 0, true, true },
	{"Total",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////

//extract object name from Unknown
static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return _bstr_t("");

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr) 
		return  _bstr_t("");

	bstr_t bstr1(bstr);
	::SysFreeString(bstr);

	return bstr1;
}

static void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

////////////////////////////////////////////////////////////////////////

CParticlesAddFrame::CParticlesAddFrame()
{
}

CParticlesAddFrame::~CParticlesAddFrame()
{
}

IUnknown *CParticlesAddFrame::GetContextObject(_bstr_t cName, _bstr_t cType)
{ //Возвращает с AddRef

	IDataContext3Ptr ptrC = m_ptrTarget;
	_bstr_t	bstrType = cType;

	if( cName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	LONG_PTR lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == cName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
// Скопировать все вычисляемые параметры из старого списка в новый
{
	if( in == 0 || out == 0)
		return;
	
	LONG_PTR lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer      *pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

HRESULT CParticlesAddFrame::DoInvoke()
{
	if (m_ptrTarget==0) return false;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return false;

	_bstr_t bstrCurrent=GetArgString( "Current" );
	_bstr_t bstrTotal=GetArgString( "Total" );

	IUnknown *punkCurrent = GetContextObject(bstrCurrent, szTypeObjectList);
	INamedDataObject2Ptr ptrCurrent(punkCurrent);
	if(punkCurrent!=0) punkCurrent->Release();
	if (ptrCurrent==0) return false;

	IUnknown *punkTotal = GetContextObject(bstrTotal, szTypeObjectList);
	INamedDataObject2Ptr ptrTotal(punkTotal);
	if(punkTotal!=0) punkTotal->Release();
	if (ptrTotal==0)
	{	// если итогового списка еще нет - создадим и воткнем в документ
		IUnknown *punk3=::CreateTypedObject(_bstr_t(szTypeObjectList));
		ptrTotal=punk3;
		if (punk3!=0) punk3->Release();
		if (ptrTotal==0) return false;
		if( bstrTotal != _bstr_t("")  )
		{
			INamedObject2Ptr	sptr( ptrTotal );
			if (sptr != 0)
			{
				HRESULT hr=sptr->SetName( bstrTotal );
			}
		}
		::SetValue( doc, 0, 0/*path*/, _variant_t((IUnknown*)ptrTotal));
		if (ptrTotal==0) return false;

		ICalcObjectContainerPtr	ptrT(ptrTotal);
		ICalcObjectContainerPtr	ptrC(ptrCurrent);
		if(ptrT!=0 && ptrC!=0)
		{
			CopyParams(ptrC, ptrT);
			//ptrT->DefineParameter(
		}
	}
	if (ptrTotal==0) return false;

	long lMaxFramesBack = ::GetValueInt( GetAppUnknown(), "\\Particles", "MaxFramesBack", 1 );
	long lMaxDistance = ::GetValueInt( GetAppUnknown(), "\\Particles", "MaxDistance", 16 );
	lMaxDistance=max(1,lMaxDistance);
	long lSearchSteps = ::GetValueInt( GetAppUnknown(), "\\Particles", "SearchSteps", 3 );

	bool bAdaptiveMaxDistance = ::GetValueInt( GetAppUnknown(), "\\Particles", "AdaptiveMaxDistanceOn", 1 ) != 0;

	long nCurrent=0; //число объектов в исходном листе.
	long nTotal=0; //число объектов в едином листе.
	ptrCurrent->GetChildsCount(&nCurrent);
	ptrTotal->GetChildsCount(&nTotal);

	smart_alloc(xCurrent,int,nCurrent); smart_xCurrent.zero();
	smart_alloc(yCurrent,int,nCurrent); smart_yCurrent.zero();
	smart_alloc(assignedCurrent,int,nCurrent); smart_assignedCurrent.zero();

	AttachArrays(ptrTotal);

	GetArray("x",sizeof(int)); //координаты
	GetArray("y",sizeof(int));
	GetArray("exist",sizeof(byte)); //существует ли на соответствующем кадре

	byte* xTotal = GetExistingArray("x"); //координаты
	byte* yTotal = GetExistingArray("y");
	byte* exist = GetExistingArray("exist"); //существует ли на соответствующем кадре

	//ссылки на объекты - в массив
	smart_alloc(objCurrent,IMeasureObjectPtr,nCurrent); //объекты
	TPOS lPosCurrent=0; //позиция объекта в листе
	ptrCurrent->GetFirstChildPosition(&lPosCurrent);
	for(int current=0; current<nCurrent; current++)
	{
		if(lPosCurrent==0) return false; //ошибка - объектов не хватает
		IUnknown *punk=0;
		ptrCurrent->GetNextChild(&lPosCurrent, &punk);
		IMeasureObjectPtr ptrO=punk;
		if(punk!=0) punk->Release();
		if(ptrO==0) return false; //ошибка - не объект в листе
		objCurrent[current]=ptrO;
	}

	//ссылки на объекты - в массив
	smart_alloc(objTotal,IMeasureObjectPtr,nTotal+nCurrent); //объекты
	smart_alloc(pTrajectoryDataTotal,PCTrajectoryData,nTotal+nCurrent); //данные траекторий
	smart_pTrajectoryDataTotal.zero();
	TPOS lPosTotal=0; //позиция объекта в листе
	ptrTotal->GetFirstChildPosition(&lPosTotal);
	for(int total=0; total<nTotal; total++)
	{
		if(lPosTotal==0) return false; //ошибка - объектов не хватает
		IUnknown *punk=0;
		ptrTotal->GetNextChild(&lPosTotal, &punk);
		IMeasureObjectPtr ptrO=punk;
		if(punk!=0) punk->Release();
		if(ptrO==0) return false; //ошибка - не объект в листе
		objTotal[total]=ptrO;

		// заодно получим Trajectory Data
		INamedDataObject2Ptr ptrNDO(ptrO);
		if(ptrNDO!=0)
		{
			TPOS lPos;
			ptrNDO->GetFirstChildPosition(&lPos);
			if(lPos!=0)
			{
				IUnknown *punk=0;
				ptrNDO->GetNextChild(&lPos, &punk);
				ITrajectoryDataPtr ptrTD=punk;
				if(punk!=0) punk->Release();
				if(ptrTD!=0)
				{
					ptrTD->GetTrajectoryDataPtr(&pTrajectoryDataTotal[total]);
					if(pTrajectoryDataTotal[total]!=0) 
					{
						pTrajectoryDataTotal[total]->r_max =
							min(pTrajectoryDataTotal[total]->r_max, lMaxDistance);
					}
				}
			}
		}
	}

	int xmax=1, ymax=1; //всяко не менее 1 пиксела
	// пробежим по всем объектам, посчитаем их координаты
	for(current=0; current<nCurrent; current++)
	{
		IMeasureObjectPtr ptrO=objCurrent[current];
		IUnknown *punk2;
		ptrO->GetImage( &punk2 );
		IImage3Ptr      ptrI(punk2);
		if(punk2!=0) punk2->Release();
		if(ptrI==0) return false; //ошибка - все объекты должны иметь изображения
		int cx=0, cy=0;
		ptrI->GetSize(&cx, &cy);
		_point ofs;
		ptrI->GetOffset(&ofs);
		double xsum=0, ysum=0;
		int count=0;
		for(int y=0; y<cy; y++)
		{
			byte *pmask;
			ptrI->GetRowMask(y,&pmask);
			for(int x=0; x<cx; x++)
			{
				if(*pmask>=128)
				{
					xsum+=x;
					ysum+=y;
					count++;
				}
				pmask++;
			} // for x
		} // for y
		if(count)	{ xsum=xsum/count; ysum=ysum/count; }
		else		 { xsum=(cx-1)/2.0; ysum=(cy-1)/2.0; }
		xCurrent[current]=int(xsum+0.5)+ofs.x;
		yCurrent[current]=int(ysum+0.5)+ofs.y;
		if(xCurrent[current]>xmax) xmax=xCurrent[current];
		if(yCurrent[current]>ymax) ymax=yCurrent[current];
	}

	smart_alloc(current2total,int,nCurrent);
	smart_alloc(total2current,int,nTotal+nCurrent);
	smart_alloc(rCurrent,double,nCurrent);
	smart_alloc(rTotal,double,nTotal+nCurrent);

	{	for(int i=0; i<nCurrent; i++) current2total[i]=-1; }
	{	for(int i=0; i<nTotal+nCurrent; i++) total2current[i]=-1; }
	{	for(int i=0; i<nCurrent; i++) rCurrent[i]=1e20; }
	{	for(int i=0; i<nTotal+nCurrent; i++) rTotal[i]=1e20; }

	int nObjectStep=GetObjectStep();
	int nFrameStep=GetFrameStep();
	int nFrame=::GetValueInt(ptrTotal, "\\particles", "FrameNo", 0);

	/*
	//для ускорения заведем хэш-массивы для current - чтобы просматривать только частицы, ближайшие к исследуемой
	int nCellsX=int(xmax/(lMaxDistance*2));
	nCellsX=max(1,min(16,nCellsX));
	int cxCell=max(lMaxDistance*2, xmax/nCellsX);

	int nCellsY=int(ymax/(lMaxDistance*2));
	nCellsY=max(1,min(16,nCellsY));
	int cyCell=max(lMaxDistance*2, ymax/nCellsY);

	smart_alloc(cellhashfirst, int, cxCell*cyCell); //указатели на первые объекты в клетке
	smart_cellhashfirst.zero();
	smart_alloc(cellhashnext, int, nCurrent); //указатели на следующие объекты в клетке
	smart_cellhashnext.zero();

	for(int current=0; current<nCurrent; current++)
	{
		double r=_hypot(xCurrent[current], yCurrent[current]);
	}
	*/

	for(int nFramesBack=1; nFramesBack<=lMaxFramesBack; nFramesBack++)
	{
		int lCurrentSearchSteps = (nFramesBack==1) ? lSearchSteps : 1;
		int nFrame1=nFrame-nFramesBack;
		if(nFrame1>=0)
		{
			for(int k=lCurrentSearchSteps-1; k>=0; k--)
			{ // повторим цикл несколько раз: на каждом проходе могут остаться необработанные
				double r_max1 = double(lMaxDistance)/(1<<k); // пойдем от 4 до 16, удваивая шаг
				r_max1 /= nFramesBack; //пересчитаем в расчете на 1 кадр
				for(int total=0; total<nTotal; total++) //если первый кадр - nTotal==0, цикла не будет - все Ok.
				{
					int posT = nFrameStep*(nFrame1) + nObjectStep*total;
					if(!*(byte *)(exist+posT)) continue; //если объекта не было на прошлом кадре - скипаем
					double r_max = r_max1;
					if(pTrajectoryDataTotal[total]!=0)
						r_max = min(r_max, pTrajectoryDataTotal[total]->r_max);
					for(int current=0; current<nCurrent; current++)
					{
						double r=_hypot(xCurrent[current]-*(int *)(xTotal+posT), yCurrent[current]-*(int *)(yTotal+posT));
						r /= nFramesBack; //пересчитаем в расчете на 1 кадр
						if(r>r_max) continue;
						if(r<rCurrent[current] && r<rTotal[total])
						{
							int current2=total2current[total];
							if(current2!=-1)
							{ // оборвем старую связь
								rTotal[total]=1e20;
								rCurrent[current2]=1e20;
								total2current[total]=-1;
								current2total[current2]=-1;
							}
							int total2=current2total[current];
							if(total2!=-1)
							{ // оборвем старую связь
								rTotal[total2]=1e20;
								rCurrent[current]=1e20;
								total2current[total2]=-1;
								current2total[current]=-1;
							}
							// теперь установим новую связь
							rCurrent[current]=r;
							rTotal[total]=r;
							current2total[current]=total;
							total2current[total]=current;
						}
					}
				}
			}
		}
	}
		

	int nUnassigned=0; //пересчитаем неприсоединенные объекты
	if(nTotal<1) 
	{
		for(current=0; current<nCurrent; current++)
		{
			if (current2total[current] == -1)
			{
				current2total[current] = nTotal+nUnassigned;
				total2current[nTotal+nUnassigned] = current;
				nUnassigned++;
			}
		}
	}

	//насоздаем недостающие объекты
	for(total=nTotal; total<nTotal+nUnassigned; total++)
	{
		IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
		IMeasureObjectPtr ptrO = punk;
		if(punk!=0) punk->Release();
		if(ptrO==0) return false; //ошибка - не создался объект
		objTotal[total]=ptrO;
		
		INamedDataObject2Ptr ptrObject(ptrO);
		if(ptrObject==0) return false;
		ptrObject->SetParent(ptrTotal, 0);

		ITrajectoryPtr ptrT=CreateTrajectory(ptrO);
		ptrT->SetObjNum(total);
	}

	int nMaxObjects=nTotal+nUnassigned;
	nMaxObjects=((nMaxObjects+99)/100)*100; //до ближайшего большего, кратного 100

	int nMaxFrames=nFrame+1;
	nMaxFrames=((nMaxFrames+49)/50)*50; //до ближайшего большего, кратного 50

	SetArraySize(nMaxFrames,nMaxObjects,-1); //100 кадров по 1000 частиц - потом добавим, если что
	xTotal = GetExistingArray("x"); //координаты
	yTotal = GetExistingArray("y");
	exist = GetExistingArray("exist"); //существует ли на соответствующем кадре

	//заполним очередной кадр в массиве
	for(total=0; total<nTotal+nUnassigned; total++) //если первый кадр - nTotal==0, цикла не будет - все Ok.
	{
		int posT = nFrameStep*nFrame + nObjectStep*total;
		int current=total2current[total];
		if(current!=-1)
		{
			*(int *)(xTotal+posT) = xCurrent[current];
			*(int *)(yTotal+posT) = yCurrent[current];
			*(byte *)(exist+posT) = 1;
		}
		else
		{
			if(nFrame>0)
			{
				*(int *)(xTotal+posT) = *(int *)(xTotal+posT-nFrameStep);
				*(int *)(yTotal+posT) = *(int *)(yTotal+posT-nFrameStep);
				*(byte *)(exist+posT) = 0;
			}
		}
	}

	::SetValue(ptrTotal, "\\particles", "FrameNo", long(nFrame+1));

	// обновим средние значения
	ICalcObjectContainerPtr	ptrC(ptrCurrent);
	if(ptrC!=0)
	{
		long nkeys=0;
		ptrC->GetParameterCount(&nkeys);
		_ptr_t2<long> keys(nkeys);	

		long nkeys2=0; //исключим ключи KEY_PARTICLES_xxx
		LONG_PTR lPos; ptrC->GetFirstParameterPos(&lPos);
		for (int i = 0; i < nkeys; i++)
		{
			ParameterContainer	*pc = 0;
			ptrC->GetNextParameter(&lPos, &pc);
			long key=pc->pDescr->lKey;
			if(key<KEY_BASE_PARTICLES || key>KEY_END_PARTICLES)
			{
				keys[nkeys2]=pc->pDescr->lKey;
				nkeys2++;
			}
		}

		for(total=0; total<nTotal+nUnassigned; total++) //если первый кадр - nTotal==0, цикла не будет - все Ok.
		{
			int posT = nFrameStep*nFrame + nObjectStep*total;
			int current=total2current[total];
			if(current!=-1)
			{
				if(bAdaptiveMaxDistance && pTrajectoryDataTotal[total]!=0 && rTotal[total]<1e10)
				{
					double rNew = rTotal[total]*2.5;
					double dr = rNew - pTrajectoryDataTotal[total]->r_max;
					if(dr>0)
					{// увеличиваем радиус относительно легко
						pTrajectoryDataTotal[total]->r_max += dr*0.2;
					}
					else
					{// уменьшаем радиус c ба-альшим скрипом
						pTrajectoryDataTotal[total]->r_max += dr*0.02;
					}
				}

				ICalcObjectPtr ptrOT=objTotal[total];
				ICalcObjectPtr ptrOC=objCurrent[current];
				if(ptrOT!=0 && ptrOC!=0)
				{
					int nFramesAveraged = nFrame; //сколько кадров уже включено в осреднение данного объекта
					double fFramesAveraged = 0; //сколько кадров уже включено в осреднение данного объекта
					ptrOT->GetValue(KEY_PARTICLES_NFRAMES,&fFramesAveraged);
					nFramesAveraged = int(fFramesAveraged+0.5);
					for(int i=0; i<nkeys2; i++)
					{
						double val=0;
						if(nFramesAveraged>0)
						{
							ptrOT->GetValue(keys[i],&val);
						}
						double val1=0;
						ptrOC->GetValue(keys[i],&val1);
						val = (val*nFramesAveraged+val1)/(nFramesAveraged+1);
						ptrOT->SetValue(keys[i],val);
					}
					ptrOT->SetValue(KEY_PARTICLES_NFRAMES,nFramesAveraged+1);
				}

			}
		}
	}
	return true;
}

