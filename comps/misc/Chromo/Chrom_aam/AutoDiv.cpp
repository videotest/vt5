// AutoDiv.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoDiv.h"
#include "misc_utils.h"
#include <math.h>
#include "measure5.h"
#include "alloc.h"
#include "Test1func.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "chrom_aam_utils.h"
#include "CrossDiv.h"

#define ADDALLOC 10000

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              AutoDiv
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CAutoDivInfo::s_pargs[] =
{
	//{"Calc",       szArgumentTypeInt, "1", true, false }, //обсчитывать ли объекты
	{"Source",     szArgumentTypeObjectList, 0, true, true },
	{"AutoDiv",    szArgumentTypeObjectList, 0, false, true },
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

HRESULT CAutoDivFilter::DoUndo()
{
	HRESULT result=CFilter::DoUndo();

	if(result==S_OK)
	{
		long	pos = m_undo_images.head();
		while( pos ) 
		{
			long prevPos=pos;
			IUnknown *punk_image=m_undo_images.next(pos);
			IImage3Ptr image(punk_image);

			if(image==0) continue; //мало ли что...

			INamedDataObject2Ptr imageNDO(image);
			imageNDO->StoreData(sdfCopyLocalData);
			imageNDO->StoreData(sdfAttachParentData);
		}
	}

	return result;
}

HRESULT CAutoDivFilter::DoRedo()
{
	long	pos = m_undo_images.head();
	while( pos ) 
	{
		long prevPos=pos;
		IUnknown *punk_image=m_undo_images.next(pos);
		IImage3Ptr image(punk_image);

		if(image==0) continue; //мало ли что...

		INamedDataObject2Ptr imageNDO(image);
		imageNDO->StoreData(sdfCopyParentData | sdfClean); //перекинуть в детишек из parent'a, испортив его
	}

	HRESULT result=CFilter::DoRedo();

	return result;
}


#define C100 1000
#define C200 2000

int invoke_nN; //for debug purposes

bool CAutoDivFilter::InvokeFilter()
{
	INamedDataObject2Ptr objectsIn(GetDataArgument());
	INamedDataObject2Ptr objectsOut(GetDataResult());
	//bool bCalc = GetArgLong("Calc")!=0;
	
	if(objectsOut==0 || objectsIn==0) return false;
	
	long nCountIn; objectsIn->GetChildsCount(&nCountIn);
	if(!nCountIn) return false;
	
	int nN=0;
	StartNotification(nCountIn);
	
	CopyParams(objectsIn, objectsOut);
	
	//FinishNotification();
    //return true; //debug

	long pos; objectsIn->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		invoke_nN=nN; //for debug purposes

		IUnknownPtr sptr;
		objectsIn->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		
		IUnknownPtr sptr2;
		object->GetImage(&sptr2);
		IImage3Ptr image(sptr2);

		_bstr_t	bstrCC;
		int nPanes;
		{
			IUnknown *punkCC = 0;
			image->GetColorConvertor( &punkCC );
			IColorConvertor2Ptr	ptrCC( punkCC );
			punkCC->Release();

			nPanes = ::GetImagePaneCount( image );

			BSTR	bstr;
			ptrCC->GetCnvName( &bstr );
			bstrCC = bstr;
			::SysFreeString( bstr ); 
		}
		
		_rect cr;
		{
			_size size;
			image->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			image->GetOffset(&ptOffset);
			cr=_rect(ptOffset,size);
		}
		
		int cx = cr.right-cr.left;
		int cy = cr.bottom-cr.top;
		smart_alloc(pimg, byte, cx*cy);
		smart_pimg.zero();
		
		int nContour; image->GetContoursCount(&nContour);
		smart_alloc(pContrs, _point*, nContour);
		smart_alloc(pContrsSize, int, nContour);
		//копируем контура в свой буфер - со сдвигом
		for (int i=0;i<nContour;i++)
		{
			Contour* pContour; image->GetContour(i,&pContour);
			int Size = pContour->nContourSize;
			pContrsSize[i]=Size;
			pContrs[i]=new _point[Size];
			for (int j=0;j<Size;j++)
			{
				pContrs[i][j].x=pContour->ppoints[j].x-cr.left;
				pContrs[i][j].y=pContour->ppoints[j].y-cr.top;
			}
		}
		
		// Копируем маску объекта в буфер
		for (int y=0;y<cy;y++)
		{
			byte *pimgy=pimg+y*cx;
			byte *pbyte; image->GetRowMask(y,&pbyte);
			for (int x=0;x<cx;x++)
			{
				if (*pbyte++==255)
					*pimgy++=255;
				else
					*pimgy++=0;
			}
		}
		
		/////////////////////////////////////////////////////
		byte *pOutImg[C100];
		_point OffSet[C100];
        int pcx[C100];
		int pcy[C100];
		for (i=0;i<C100;i++)
		{
			pOutImg[i]=0;pcx[i]=0;pcy[i]=0;
		}
		int CountOutImg=0;
		DivideChrom(pimg,cx,cy,nContour,pContrs,pContrsSize,pOutImg,pcx,pcy,OffSet,&CountOutImg);
		dbg_assert(CountOutImg<C100);
		////////////////////////////////////////////////////////////////////////

		DWORD dwFlags;
		image->GetImageFlags(&dwFlags);
		bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

		bool bCrossNonVirtual = GetValueInt(GetAppUnknown(), "\\Chromos\\CrossDiv", "CrossNonVirtual", 0) != 0;
		//надо ли переводить виртуальные кресты в невиртуальные, затирая под ними

		// если разделили объект на несколько - затрем под ним изображение
		// и будем делать его не виртуальным
		if(bIsVirtual && (CountOutImg>1) && bCrossNonVirtual)
		{
			bool bIsCross=false;
			int nDuplicatePixels=0;

			smart_alloc(cnt,int,cx*cy);
			smart_cnt.zero();
			for(int i=0; i<CountOutImg; i++)
			{
				int xos=max(0,OffSet[i].x); //OffSet[i].x;
				int yos=max(0,OffSet[i].y); //OffSet[i].y;
				int cx1=min(OffSet[i].x+pcx[i],cx)-xos; //pcx[i]
				int cy1=min(OffSet[i].y+pcy[i],cy)-yos; //pcy[i];

				for (int y=0;y<cy1;y++)
				{
					int *pcnt=cnt+(y+yos)*cx+xos;
					byte *pimgy=pOutImg[i]+(y+yos-OffSet[i].y)*pcx[i]+xos-OffSet[i].x;
					for (int x=0;x<cx1;x++,pimgy++,pcnt++)
					{
						if(*pimgy) (*pcnt)++;
					}
				}
			}
			for (int y=0;y<cy;y++)
			{
				int *pcnt=cnt+y*cx;
				for (int x=0;x<cx;x++,pcnt++)
				{
					if(*pcnt>1)
					{
						nDuplicatePixels++;
					}
				}
			}
			bIsCross=(nDuplicatePixels>16);

			/* debug - output mask to file
			FILE *f=fopen("e:\\mylog.log","a");
			fputs("\n\r",f);
			for (y=0;y<cy;y++)
			{
				int *pcnt=cnt+y*cx;
				for (int x=0;x<cx;x++)
				{
					char c='0'+*pcnt;
					if(c=='0') c='.';
					fputc(c,f);
					*pcnt++;
				}
				fputs("\n\r",f);
			}
			fputs("\n\r",f);
			fclose(f);
			*/

			if(bIsCross)
			{
				INamedDataObject2Ptr imageNDO(image);
				imageNDO->StoreData(sdfCopyParentData | sdfClean);
				image->AddRef(); m_undo_images.insert(image);
				bIsVirtual=false;
			}
		}

		for(i=0;i<CountOutImg;i++)
		{ //создаем новые объекты из полученных масок
			int xos=max(0,OffSet[i].x); //OffSet[i].x;
			int yos=max(0,OffSet[i].y); //OffSet[i].y;
			int cx1=min(OffSet[i].x+pcx[i],cx)-xos; //pcx[i]
			int cy1=min(OffSet[i].y+pcy[i],cy)-yos; //pcy[i];
			if(cx1<=0)
			{
				xos=OffSet[i].x;
				cx1=pcx[i];
			}
			if(cy1<=0)
			{
				yos=OffSet[i].y;
				cy1=pcy[i];
			}
			if(cx1<=0 || cy1<=0) continue; //глючных image не создаем

			IUnknown *punk2 = ::CreateTypedObject(_bstr_t (szTypeImage));
			IImage3Ptr imgOut(punk2);
			punk2->Release();
			
			_point pt_offset;
			pt_offset.x=cr.left+xos; //+OffSet[i].x;
			pt_offset.y=cr.top+yos; //OffSet[i].y;

			if(bIsVirtual)
			{
				_rect rect=_rect(pt_offset,_size(cx1,cy1));

				imgOut->CreateVirtual( rect );
				imgOut->InitRowMasks();

				INamedDataObject2Ptr imgOutNDO(imgOut);
				INamedDataObject2Ptr imageNDO(image);

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
				imgOut->CreateImage(cx1,cy1,bstrCC, ::GetImagePaneCount( image ) );
				imgOut->SetOffset(pt_offset,false);
				imgOut->InitRowMasks();
				imgOut->InitContours();
			}
			
			if(!bIsVirtual)
			{ //скопировать данные
				for(int pane=0; pane<nPanes; pane++)
				{
					for (y=max(0,yos);y<min(cy,yos+cy1);y++)
					{
						color *pcol_out;
						imgOut->GetRow(y-yos,pane,&pcol_out);
						pcol_out-=min(0,xos);
						
						color *pcol_in;
						image->GetRow(y,pane,&pcol_in);
						pcol_in+=max(0,xos);
						
						int C=min(cx,xos+cx1)-max(0,xos);
						CopyMemory(pcol_out,pcol_in,C*sizeof(color));
					}
				}
			}
			////////////////////////////////////////////////////////////////////////
			// задать маску
			for (y=0;y<cy1;y++)
			{
				byte *pbyte; imgOut->GetRowMask(y,&pbyte);
				byte *pimgy=pOutImg[i]+(y+yos-OffSet[i].y)*pcx[i]+xos-OffSet[i].x;
				for (int x=0;x<cx1;x++)
				{
					if (*pimgy++==0)
						*pbyte++=0;
					else
						*pbyte++=255;
				}
			}
			
			imgOut->FreeContours();
			imgOut->InitContours();
			
			IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
			IMeasureObjectPtr objectOut(punk );
			punk->Release();
			
			objectOut->SetImage( imgOut );
			
			if(1)
			{
				INamedDataObject2Ptr ptrObject(objectOut);
				DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
				if(!bIsVirtual)
				{
					ptrObject->GetObjectFlags(&dwFlags);
				}
				ptrObject->SetParent(objectsOut, dwFlags);
			}
			// long pos1=objectsOut->AddChild(objectOut);
			//if(bCalc)
			//{
				//::ConditionalCalcObject(objectOut, 0, true, 0 );
			//}
		}
		
		for (i=0;i<C100;i++)
		{
			if(pOutImg[i]!=0 && pcx[i]!=0 && pcy[i]!=0) delete pOutImg[i];
		}
		for (i=0;i<nContour;i++)
		{
			if (pContrsSize[i]!=0) delete pContrs[i];
		}
		Notify(nN++);
	}

	GUID guidNewBase;
	objectsIn->GetBaseKey(&guidNewBase);
	objectsOut->SetBaseKey(&guidNewBase);

	FinishNotification();

	return true;
}

CAutoDivFilter::CAutoDivFilter()
{
	m_MinContourSize=20;
}

CAutoDivFilter::~CAutoDivFilter()
{
}

void CAutoDivFilter::DivideChrom( byte *in,int cx ,int cy ,
								 int ContCount , _point **pContrs ,int *pContrsSize,
								 byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int *pCountImg)
{
	smart_alloc(out, byte, (cx+1)*(cy+1));
	_point pP1[C100];
	_point pP2[C100];
	int CountP=0;
	int width=0;
	CTest1Func divfunk;
	divfunk.DoWork(in,cx,cy,out,pP1,pP2,&CountP,&width);
	//поделили на зерна, концы разделяющих отрезков - в pP1 и pP2
	dbg_assert(CountP<C100);
	int CountP0=CountP;
	if(CountP>300 || divfunk.m_bWasFatalError)
	{
		//*pCountImg=0; //пока так
		//return;
		CountP=0;
	}
	
	for (int i1=0;i1<CountP;i1++)
	{
		if (max(abs(pP1[i1].x-pP2[i1].x),abs(pP1[i1].y-pP2[i1].y))<2)//<3
		{
			for (int j=i1;j<CountP-1;j++)
			{
				pP1[j]=pP1[j+1];
				pP2[j]=pP2[j+1];
			}
			CountP--;
			i1--;
		}
	} //исключили отрезки короче 2

	/*// debug - output to file
	{
		FILE *f=fopen("d:\\mylog.log","a");
		fprintf(f,"%3i CountP=%3i %3i, Width=%3i size=(%i,%i)\n",
			invoke_nN,CountP0, CountP, width,
			cx,cy);
		fclose(f);
	}*/

	if(CountP>100 || divfunk.m_bWasFatalError)
	{
		//*pCountImg=0; //пока так
		//return;
		CountP=0;
	}
	
	_point pt11;
	double r1,r2;
	for (i1=0;i1<CountP;i1++)
	{
		int r=10000;
		for (int i=0;i<ContCount;i++)
		{
			int Size=pContrsSize[i];
			if (Size<m_MinContourSize) Size=0;
			_point *pCPd=pContrs[i];
			for (int j=0;j<Size;j++)
				if (max(abs(pCPd[j].x-pP1[i1].x),abs(pCPd[j].y-pP1[i1].y))<r)
				{
					r=max(abs(pCPd[j].x-pP1[i1].x),abs(pCPd[j].y-pP1[i1].y));
					pt11=pCPd[j];
				}
		} //pt11 - точка контура, ближайшая к pP1[i1]
		r1=sqrt((pP1[i1].x-pP2[i1].x)*(pP1[i1].x-pP2[i1].x)+
			(pP1[i1].y-pP2[i1].y)*(pP1[i1].y-pP2[i1].y));
		r2=sqrt((pP1[i1].x-pt11.x)*(pP1[i1].x-pt11.x)+
			(pP1[i1].y-pt11.y)*(pP1[i1].y-pt11.y));
		if (r2<r1-1)
		{       //если точка на контуре хоть немного ближе,
			//чем другой конец отрезка - заменим этот конец
			pP1[i1]=pt11;
		}
		else
		{       //иначе удалим отрезок нафиг - вероятно, он внутренний
			for (int j=i1;j<CountP-1;j++)
			{
				pP1[j]=pP1[j+1];
				pP2[j]=pP2[j+1];
			}
			CountP--;
			i1--;
		}
	}
	
	for (i1=0;i1<CountP;i1++)
	{
		int r=10000;
		for (int i=0;i<ContCount;i++)
		{
			int Size=pContrsSize[i];
			if (Size<m_MinContourSize) Size=0;
			_point *pCPd=pContrs[i];
			for (int j=0;j<Size;j++)
				if (max(abs(pCPd[j].x-pP2[i1].x),abs(pCPd[j].y-pP2[i1].y))<r)
				{
					r=max(abs(pCPd[j].x-pP2[i1].x),abs(pCPd[j].y-pP2[i1].y));
					pt11=pCPd[j];
				}
		} //pt11 - точка контура, ближайшая к pP2[i1]
		r1=sqrt((pP1[i1].x-pP2[i1].x)*(pP1[i1].x-pP2[i1].x)+
			(pP1[i1].y-pP2[i1].y)*(pP1[i1].y-pP2[i1].y));
		r2=sqrt((pP2[i1].x-pt11.x)*(pP2[i1].x-pt11.x)+
			(pP2[i1].y-pt11.y)*(pP2[i1].y-pt11.y));
		if (r2<r1-1)
		{       //если точка на контуре хоть немного ближе,
			//чем другой конец отрезка - заменим этот конец
			pP2[i1]=pt11;
		}
		else
		{       //иначе удалим отрезок нафиг - вероятно, он внутренний
			for (int j=i1;j<CountP-1;j++)
			{
				pP1[j]=pP1[j+1];
				pP2[j]=pP2[j+1];
			}
			CountP--;
			i1--;
		}
	}
	
	ReSetTouchPoint(in,cx,cy,ContCount,pContrs,pContrsSize,
		pP1,pP2,CountP,width);
	FNeaLine(pP1,pP2,CountP,width);
	int pPixCount=0;
	_point pPix[C200];
    FindShot(in,cx,cy,ContCount,pContrs,pContrsSize,&pPixCount,pPix);
	dbg_assert(CountP<C100);
	dbg_assert(pPixCount<C200);
	
	byte pFlag[C100];
	SelectTrueDiv(in,cx,cy,ContCount,pContrs,pContrsSize,pP1,pP2,CountP,width,
		pPixCount,pPix,pFlag);
	//////////   кресты
	_point pPX11[C200];
	_point pPX12[C200];
	_point pPX21[C200];
	_point pPX22[C200];
	int FlagPX[C200];
	int CountPX=0;
	FindXDiv(in,cx,cy,ContCount,pContrs,pContrsSize,pP1,pP2,CountP,width,
		&CountPX,pPX11,pPX12,pPX21,pPX22,FlagPX);
	dbg_assert(CountPX<C200);
	
	// касания
	// кресты   FlagPX[i]
	int i;
	for (i=0;i<CountPX-1;i++)
	{
		for (int j=i+1;j<CountPX;j++)
		{
			if (TouchCross(pPX11[i],pPX12[i],pPX21[i],pPX22[i],
				pPX11[j],pPX12[j],pPX21[j],pPX22[j],width) == 1)
			{
				FlagPX[i]=255;
				FlagPX[j]=255;
			}
		}
	}
	// линии   FlagPX[i]
	for (i=0;i<CountP;i++)
	{
		if (pFlag[i]==1)
		{
			for (int j=0;j<CountPX;j++)
			{
				if (TouchLineCross(pP1[i],pP2[i],
					pPX11[j],pPX12[j],pPX21[j],pPX22[j],width) == 1)
				{
					pFlag[i]=255;
				}
			}
		}
	}
	for (i=0;i<CountP;i++)
	{
		if (pFlag[i]==0)
		{
			for (int j=0;j<CountPX;j++)
			{
				if (TouchLineCross(pP1[i],pP2[i],
					pPX11[j],pPX12[j],pPX21[j],pPX22[j],width/5) == 1)
				{
					pFlag[i]=255;
				}
			}
		}
	}
	/////////////////////////////////////////////////
	/////////////    разбиение
	CopyMemory(out,in,cx*cy);
	for (i=0;i<CountP;i++)
	{
		if (pFlag[i]<5)
		{
			DrawDivLine(in,cx,cy,pP1[i],pP2[i]);
			DrawDivLine(out,cx,cy,pP1[i],pP2[i]);
		}
	}
	ErroImg(in,cx,cy,1);
	int CountImg=Separate(in,cx,cy,pOutImg,pcx,pcy,OffSet);
	Restavr(out,cx,cy,pOutImg,pcx,pcy,OffSet,CountImg);
	
	ReSet4Point(ContCount,pContrs,pContrsSize,
		pPX11,pPX12,pPX21,pPX22,FlagPX,CountPX);
	
	for (i=0;i<CountPX;i++)
	{
		if(FlagPX[i]==0)
			SeparateRect(pOutImg,pcx,pcy,OffSet,pPX11[i],pPX12[i],pPX21[i],pPX22[i],&CountImg);
	}
	
	*pCountImg=CountImg;
	/////////////////////////////////////////////////////
	/*      int x,y;
	for (i=0;i<CountPX;i++)
	if(FlagPX[i]==0)
	{
	DrawDivLine(in,cx,cy,pPX11[i],pPX12[i]);
	DrawDivLine(in,cx,cy,pPX12[i],pPX21[i]);
	DrawDivLine(in,cx,cy,pPX21[i],pPX22[i]);
	DrawDivLine(in,cx,cy,pPX22[i],pPX11[i]);
	}
	
	  for (i=0;i<pPixCount;i++)
	  {
	  for(y=max(0,pPix[i].y-1);y<=min(cy-1,pPix[i].y+1);y++)
	  for(x=max(0,pPix[i].x-1);x<=min(cx-1,pPix[i].x+1);x++)
	  in[y*cx+x]=0;
	  }
	  
		for (i=0;i<CountP;i++)
		if (pFlag[i]<5)
		DrawDivLine(in,cx,cy,pP1[i],pP2[i]);
	////////////////////////////////////////////////////*/
	return;
}

void CAutoDivFilter::ReSetTouchPoint(byte *in1,int cx,int cy,int ContCount,
									 _point **pContrs,int *pContrsSize,
									 _point *pP1,_point *pP2,int CountP,
									 int width)
{
	smart_alloc(in, byte, cx*cy);
	CopyMemory(in,in1,cx*cy);
	int Far = width;
	int dF=10;
	int i,j,j1,j2,j3,J;
	_point pt;
	double Max;
	int NumLine;
	int PosLine[1000];
	int NumEnd[1000];
	
	for (i=0;i<CountP;i++)
	{
		in[pP1[i].y*cx+pP1[i].x]=254;
		in[pP2[i].y*cx+pP2[i].x]=253;
	}
	
	// поиск точек разделения на контуре
	for (i=0;i<ContCount;i++)
	{
		int Size=pContrsSize[i];
		_point *pCPd=pContrs[i];
		//              if (Size>Far*2)
		if (Size<m_MinContourSize) Size=0;
		for (j=0;j<Size;j++)
		{
			if (in[pCPd[j].y*cx+pCPd[j].x]==254 || in[pCPd[j].y*cx+pCPd[j].x]==253)
			{
				///////////////////////////////////////////////////////////
				NumLine=0;
				for (j1=0;j1<CountP;j1++)
				{
					if (pCPd[j]==pP1[j1])
					{
						PosLine[NumLine]=j1;
						NumEnd[NumLine]=1;
						NumLine++;
						Far=(int)sqrt((pP1[j1].x-pP2[j1].x)*(pP1[j1].x-pP2[j1].x)+
							(pP1[j1].y-pP2[j1].y)*(pP1[j1].y-pP2[j1].y))/2+1;
						if (Far>width) Far=width;
						if (Far>Size/3) Far=Size/3;
					}
				}
				for (j1=0;j1<CountP;j1++)
				{
					if (pCPd[j]==pP2[j1])
					{
						PosLine[NumLine]=j1;
						NumEnd[NumLine]=2;
						NumLine++;
						Far=(int)sqrt((pP1[j1].x-pP2[j1].x)*(pP1[j1].x-pP2[j1].x)+
							(pP1[j1].y-pP2[j1].y)*(pP1[j1].y-pP2[j1].y))/2+1;
						if (Far>width) Far=width;
						if (Far>Size/3) Far=Size/3;
					}
				}
				/////////////////////////////////////////////////////////
				pt=pCPd[j];
				Max=0;
				// поиск ближайших точек максимума
				J=j+Far;
				if (J>=Size) J=0;if (J<0) J=Size-1;
				for (j1=j-dF,j2=j,j3=j+dF;j2!=J;j1++,j2++,j3++)
				{
					if (j1>=Size) j1=0;if (j1<0) j1=Size-1;
					if (j2>=Size) j2=0;if (j2<0) j2=Size-1;
					if (j3>=Size) j3=0;if (j3<0) j3=Size-1;
					if (j2==J) break;
					
					if (FindD(pCPd[j1],pCPd[j3],pCPd[j2],&Max,in,cx)==1)
						pt=pCPd[j2];
					
					if (in[pCPd[j2].y*cx+pCPd[j2].x]!=255 && j2!=j)
						break;
					//                                      if (in[pCPd[j3].y*cx+pCPd[j3].x]!=255)
					//                                              break;
				}
				
				J=j-Far;
				if (J>=Size) J=0;if (J<0) J=Size-1;
				for (j1=j-dF,j2=j,j3=j+dF;j2!=J;j1--,j2--,j3--)
				{
					if (j1>=Size) j1=0;if (j1<0) j1=Size-1;
					if (j2>=Size) j2=0;if (j2<0) j2=Size-1;
					if (j3>=Size) j3=0;if (j3<0) j3=Size-1;
					if (j2==J) break;
					
					if (FindD(pCPd[j1],pCPd[j3],pCPd[j2],&Max,in,cx)==1)
						pt=pCPd[j2];
					
					if (in[pCPd[j2].y*cx+pCPd[j2].x]!=255 && j2!=j)
						break;
					//                                      if (in[pCPd[j1].y*cx+pCPd[j1].x]!=255)
					//                                              break;
				}
				for (j1=0;j1<NumLine;j1++)
				{
					if (NumEnd[j1]==1)
						pP1[PosLine[j1]]=pt;
					else
						pP2[PosLine[j1]]=pt;
				}
			}
		}
	}
	return;
}

void CAutoDivFilter::FNeaLine(_point *pP1,_point *pP2,int CountP,int width)
{
	int i,j;
	double porog = 5;
	double r11,r12,r21,r22;
	double r1,r2;
	
	for (i=0;i<CountP;i++)
	{
		for (j=i+1;j<CountP;j++)
		{
			r1=sqrt((pP1[i].x-pP2[i].x)*(pP1[i].x-pP2[i].x)+
				(pP1[i].y-pP2[i].y)*(pP1[i].y-pP2[i].y));
			r2=sqrt((pP1[j].x-pP2[j].x)*(pP1[j].x-pP2[j].x)+
				(pP1[j].y-pP2[j].y)*(pP1[j].y-pP2[j].y));
			
			r11=sqrt((pP1[i].x-pP1[j].x)*(pP1[i].x-pP1[j].x)+
				(pP1[i].y-pP1[j].y)*(pP1[i].y-pP1[j].y));
			r12=sqrt((pP1[i].x-pP2[j].x)*(pP1[i].x-pP2[j].x)+
				(pP1[i].y-pP2[j].y)*(pP1[i].y-pP2[j].y));
			r21=sqrt((pP2[i].x-pP1[j].x)*(pP2[i].x-pP1[j].x)+
				(pP2[i].y-pP1[j].y)*(pP2[i].y-pP1[j].y));
			r22=sqrt((pP2[i].x-pP2[j].x)*(pP2[i].x-pP2[j].x)+
				(pP2[i].y-pP2[j].y)*(pP2[i].y-pP2[j].y));
			if (r2>r1)
			{
				if (r11<r12 && r11<r21 && r11<r22 && r11<porog)
				{
					pP1[i]=pP1[j];
					if(r22<porog)
						pP2[i]=pP2[j];
				}
				else
					if (r12<r21 && r12<r22 && r12<porog)
					{
						pP1[i]=pP2[j];
						if(r21<porog)
							pP2[i]=pP1[j];
					}
					else
						if (r21<r22 && r21<porog)
						{
							pP2[i]=pP1[j];
							if(r12<porog)
								pP1[i]=pP2[j];
						}
						else
							if (r22<porog)
							{
								pP2[i]=pP2[j];
								if(r12<porog)
									pP1[i]=pP1[j];
							}
			}
			else
			{
				if (r11<r12 && r11<r21 && r11<r22 && r11<porog)
				{
					pP1[j]=pP1[i];
					if(r22<porog)
						pP2[j]=pP2[i];
				}
				else
					if (r12<r21 && r12<r22 && r12<porog)
					{
						pP2[j]=pP1[i];
						if(r21<porog)
							pP1[j]=pP2[i];
					}
					else
						if (r21<r22 && r21<porog)
						{
							pP1[j]=pP2[i];
							if(r12<porog)
								pP2[j]=pP1[i];
						}
						else
							if (r22<porog)
							{
								pP2[j]=pP2[i];
								if(r12<porog)
									pP1[j]=pP1[i];
							}
			}
			
		}
	}
	return;
}

void CAutoDivFilter::FindShot(byte *in_img,int cx,int cy,int ContCount,
							  _point **pContrs,int *pContrsSize,
							  int *pPixCount,_point *pPix)
{
	int cx1=cx+2;
	int cy1=cy+2;
	int i,j,i1,x,y,flag;
	int PixCount=0;
	smart_alloc(image, DWORD, cx1*cy1);
	smart_image.zero();
	int NumPoint=1;
	int NPCountBeg;
	int a,a1;
	
	smart_alloc(in, byte, cx1*cy1);
	smart_in.zero();
	for(j=0;j<cy;j++)
		CopyMemory(in+(j+1)*cx1+1,in_img+j*cx,cx);
	
	for (i=0;i<ContCount;i++)
	{
		int Size=pContrsSize[i];
		_point *pCPd=pContrs[i];
		if (Size<m_MinContourSize) Size=0;
		NPCountBeg=NumPoint;
		for (j=0;j<Size;j++)
		{
			flag=0;
			for (y=pCPd[j].y;y<pCPd[j].y+3;y++)
				for (x=pCPd[j].x;x<pCPd[j].x+3;x++)
				{
					a=image[y*cx1+x];
					if (a>0)
					{
						if (a<NPCountBeg) // другой контур
							if (in[y*cx1+x]>100) {flag=1;a1=a;}
							if (NumPoint-a>m_MinContourSize && Size-j+a-NPCountBeg>m_MinContourSize) // этот контур
								if (in[y*cx1+x]>100) {flag=1;a1=a;}
					}
					image[y*cx1+x]=NumPoint;
				}
				if (flag==1)
				{
					pPix[PixCount]=pCPd[j];
					PixCount++;
					if (PixCount>199) PixCount=199;
					
					if (a1>=NPCountBeg)
					{
						pPix[PixCount]=pCPd[a1-NPCountBeg];
						PixCount++;
						if (PixCount>199) PixCount=199;
					}
					else
						for(i1=0;i1<=i;i1++)
						{
							if (pContrsSize[i1]>=m_MinContourSize)
								a1-=pContrsSize[i1];
							if (a1<0)
							{
								pPix[PixCount]=pContrs[i1][a1+pContrsSize[i1]];
								PixCount++;
								if (PixCount>199) PixCount=199;
								break;
							}
						}
				}
				NumPoint++;
		}
	}
	
	*pPixCount=PixCount;
	return;
}

void CAutoDivFilter::SelectTrueDiv(byte *in,int cx,int cy,int ContCount,_point **pContrs,
								   int *pContrsSize,_point *pP1,_point *pP2,
								   int CountP,int width,int pPixCount,_point *pPix,
								   byte *pFlag)
{
	int Far = 2*width;
	int FarShot = width/2;
	int i,j,j1,n;
	_point pP11[C100];
	_point pP12[C100];
	_point pP21[C100];
	_point pP22[C100];
	//////////////////////////////////////////////
	_point pP11s[C100];
	_point pP12s[C100];
	_point pP21s[C100];
	_point pP22s[C100];
	//////////////////////////////////////////////
	_point pDP11[C100];
	_point pDP12[C100];
	_point pDP21[C100];
	_point pDP22[C100];
	//////////////////////////////////////////////
	double pD11[C100];
	double pD12[C100];
	double pD21[C100];
	double pD22[C100];
	//////////////////////////////////////////////
	
	dbg_assert(CountP<C100);
	dbg_assert(pPixCount<C200);
	
	ZeroMemory(pFlag,C100*sizeof(byte));
	
    for (i=0;i<pPixCount;i++)
	{
		in[pPix[i].y*cx+pPix[i].x]=250;
		in[pPix[i].y*cx+pPix[i].x]=250;
	}
	
    for (i=0;i<CountP;i++)
	{
		in[pP1[i].y*cx+pP1[i].x]=254;
		in[pP2[i].y*cx+pP2[i].x]=254;
	}
	// поиск точек разделения на контуре
	for (i=0;i<ContCount;i++)
	{
		int Size=pContrsSize[i];
		_point *pCPd=pContrs[i];
		_point *pCPd1=pContrs[i];
		_point *pCPd2=pContrs[i];
		_point *pCPd3=pContrs[i];
		_point *pCPd4=pContrs[i];
		_point CPd5=*pCPd;
		_point CPd6=*pCPd;
		Far = 2*width;
		if (Far>Size/3) Far=Size/3;
		if (Size<m_MinContourSize) Size=0;
		for (j=0;j<Size;j++)
		{
			if (in[(*pCPd).y*cx+(*pCPd).x]==254)
			{
				// поиск боковых точек
				pCPd1=pCPd;n=j;
				for (j1=0;j1<Far;j1++)
				{
					pCPd1++;n++;
					if(n>=Size) {pCPd1=pContrs[i];n=0;}
					if (in[(*pCPd1).y*cx+(*pCPd1).x]!=255)
						break;
				}
				
				pCPd2=pCPd;n=j;
				for (j1=0;j1<Far;j1++)
				{
					pCPd2--;n--;
					if(n<0) {pCPd2=pContrs[i]+Size-1;n=Size-1;}
					if (in[(*pCPd2).y*cx+(*pCPd2).x]!=255)
						break;
				}
				
				//поиск макс отклонения
				pCPd3=pCPd;n=j;
				CPd5=*pCPd1;
				double maxd1=0;
				double o;
				ScanLine(in,cx,cy,*pCPd1,*pCPd,&o);
				if (o<0.333)
					for (j1=0;j1<Far;j1++)
					{
						pCPd3++;n++;
						if(n>=Size) {pCPd3=pContrs[i];n=0;}
						if (FindD1(*pCPd,*pCPd1,*pCPd3,&maxd1,in,cx)==1)
							CPd5=*pCPd3;
						if (in[(*pCPd3).y*cx+(*pCPd3).x]!=255)
							break;
					}
					
					pCPd4=pCPd;n=j;
					CPd6=*pCPd2;
					double maxd2=0;
					ScanLine(in,cx,cy,*pCPd2,*pCPd,&o);
					if (o<0.333)
						for (j1=1;j1<Far;j1++)
						{
							pCPd4--;n--;
							if(n<0) {pCPd4=pContrs[i]+Size-1;n=Size-1;}
							if (FindD1(*pCPd,*pCPd2,*pCPd4,&maxd2,in,cx)==1)
								CPd6=*pCPd4;
							if (in[(*pCPd4).y*cx+(*pCPd4).x]!=255)
								break;
						}
						
						for (j1=0;j1<CountP;j1++)
							if (*pCPd==pP1[j1])
							{
								pP11[j1]=*pCPd1;
								pP12[j1]=*pCPd2;
								pDP11[j1]=CPd5;
								pDP12[j1]=CPd6;
								if (Size>Far*2)
								{
									pD11[j1]=maxd1;
									pD12[j1]=maxd2;
								}
								else
								{
									pD11[j1]=-1000;
									pD12[j1]=-1000;
								}
							}
							for (j1=0;j1<CountP;j1++)
								if (*pCPd==pP2[j1])
								{
									pP21[j1]=*pCPd1;
									pP22[j1]=*pCPd2;
									pDP21[j1]=CPd5;
									pDP22[j1]=CPd6;
									if (Size>Far*2)
									{
										pD21[j1]=maxd1;
										pD22[j1]=maxd2;
									}
									else
									{
										pD21[j1]=-1000;
										pD22[j1]=-1000;
									}
								}
								
			}
			pCPd++;
		}
        }
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
        for (i=0;i<ContCount;i++)
        {
			int Size=pContrsSize[i];
			_point *pCPd=pContrs[i];
			_point *pCPd1=pContrs[i];
			_point *pCPd2=pContrs[i];
			FarShot = width/2;
			if (FarShot>Size/3) FarShot=Size/3;
			if (Size<m_MinContourSize) Size=0;
			for (j=0;j<Size;j++)
			{
				if (in[(*pCPd).y*cx+(*pCPd).x]==254)
				{
					// поиск боковых точек
					pCPd1=pCPd;n=j;
					for (j1=0;j1<FarShot;j1++)
					{
						pCPd1++;n++;
						if(n>=Size) {pCPd1=pContrs[i];n=0;}
						if (in[(*pCPd1).y*cx+(*pCPd1).x]!=255)
							break;
					}
					
					pCPd2=pCPd;n=j;
					for (j1=0;j1<FarShot;j1++)
					{
						pCPd2--;n--;
						if(n<0) {pCPd2=pContrs[i]+Size-1;n=Size-1;}
						if (in[(*pCPd2).y*cx+(*pCPd2).x]!=255)
							break;
					}
					
					for (j1=0;j1<CountP;j1++)
						if (*pCPd==pP1[j1])
						{
							pP11s[j1]=*pCPd1;
							pP12s[j1]=*pCPd2;
						}
                        for (j1=0;j1<CountP;j1++)
							if (*pCPd==pP2[j1])
							{
								pP21s[j1]=*pCPd1;
								pP22s[j1]=*pCPd2;
							}
				}
				pCPd++;
			}
        }
		
        double ang1,ang2,ang3,ang4;
        _point tt;
        for (i=0;i<CountP;i++)
        {
			int a=GetAng(pDP11[i],pP1[i],pP2[i],pDP21[i],&ang1);
			a=GetAng(pDP12[i],pP1[i],pP2[i],pDP22[i],&ang2);
			a=GetAng(pDP12[i],pP1[i],pP2[i],pDP21[i],&ang3);
			a=GetAng(pDP11[i],pP1[i],pP2[i],pDP22[i],&ang4);
			if (min(ang3,ang4)<min(ang1,ang2))
			{
				tt=pDP11[i];pDP11[i]=pDP12[i];pDP12[i]=tt;
				tt=pP11[i];pP11[i]=pP12[i];pP12[i]=tt;
				tt=pP11s[i];pP11s[i]=pP12s[i];pP12s[i]=tt;
			}
        }
		
        FindNoTouchDiv(pP1,pP2,
			pP11,pP12,pP21,pP22,
			pP11s,pP12s,pP21s,pP22s,
			pD11,pD12,pD21,pD22,
			pDP11,pDP12,pDP21,pDP22,
			pFlag,CountP,width,cx,cy);
        FindTouchDiv(pP1,pP2,
			pP11,pP12,pP21,pP22,
			pP11s,pP12s,pP21s,pP22s,
			pD11,pD12,pD21,pD22,
			pDP11,pDP12,pDP21,pDP22,
			pFlag,CountP,width,cx,cy);
		
		// Разделение
        for (i=0;i<cx*cy;i++)
			if(in[i]>200) in[i]=255;
			
			//      for (i=0;i<pPixCount;i++)
			//      {
			//              for(y=max(0,pPix[i].y-1);y<=min(cy-1,pPix[i].y+1);y++)
			//                      for(x=max(0,pPix[i].x-1);x<=min(cx-1,pPix[i].x+1);x++)
			//                              in[y*cx+x]=0;
			//
			//      }
			
			//      for (i=0;i<CountP;i++)
			//          if (pFlag[i]==0)
			//                      DrawDivLine(in,cx,cy,pP1[i],pP2[i]);
			
			//      for (i=0;i<cx*cy;i++)
			//              in[i]=0;
			
			for (i=0;i<min(CountP,99)*0;i++)
				//              if (pFlag[i]==0)
				//      i=1;
				//      if (i>CountP-1) i=CountP-1;
			{
                DrawLine(in,cx,cy,pP1[i],pP2[i]);
				
                DrawLine(in,cx,cy,pP1[i],pDP11[i]);
                DrawLine(in,cx,cy,pP1[i],pDP12[i]);
                DrawLine(in,cx,cy,pP2[i],pDP21[i]);
                DrawLine(in,cx,cy,pP2[i],pDP22[i]);
				
				//              DrawLine(in,cx,cy,pP1[i],pDP11[i]);
				//              DrawLine(in,cx,cy,pP1[i],pDP12[i]);
				
				//              DrawLine(in,cx,cy,pP2[i],pDP21[i]);
				//              DrawLine(in,cx,cy,pP2[i],pDP22[i]);
				
				//              DrawLine(in,cx,cy,pP1[i],pP11[i]);
				//              DrawLine(in,cx,cy,pP1[i],pP12[i]);
				
				//              DrawLine(in,cx,cy,pP2[i],pP21[i]);
				//              DrawLine(in,cx,cy,pP2[i],pP22[i]);
				
				//              DrawLine(in,cx,cy,pDP11[i],pP11[i]);
				//              DrawLine(in,cx,cy,pDP12[i],pP12[i]);
				
				//              DrawLine(in,cx,cy,pDP21[i],pP21[i]);
				//              DrawLine(in,cx,cy,pDP22[i],pP22[i]);
			}
			
			return;
}

void CAutoDivFilter::FindXDiv(byte *in,int cx,int cy,int ContCount,_point **pContrs,
							  int *pContrsSize,_point *pP1,_point *pP2,
							  int CountP,int width,
							  int *pCountPX,_point *pPX11,_point *pPX12,_point *pPX21,
							  _point *pPX22,int *FlagPX)
{
	int i,j;
	smart_alloc(img, byte, cx*cy+1);
	CopyMemory(img,in,cx*cy);
	
	//  скелетизация
	int ItNum=Skelet(img,cx,cy);
	DelShortLine(img,cx,cy,ItNum,width);
	_point pPX1[C200];
	_point pPX2[C200];
	ZeroMemory(FlagPX,C200*sizeof(byte));
	int CountPX=0;
    FindXX(img,cx,cy,ItNum,width,pPX1,pPX2,&CountPX);
	dbg_assert(CountPX<C200);
	for (i=0;i<CountPX-1;i++)
		for (j=i+1;j<CountPX;j++)
		{
			if(pPX1[i]==pPX1[j] || pPX2[i]==pPX1[j] || pPX1[i]==pPX2[j] || pPX2[i]==pPX2[j])
			{
				FlagPX[i]=255;
				FlagPX[j]=255;
			}
		}
		
        FindNP(img,in,cx,cy,ItNum,width,pPX1,pPX2,
			pPX11,pPX12,pPX21,pPX22,
			FlagPX,CountPX);
		
		
        *pCountPX=CountPX;
        return;
}

int CAutoDivFilter::TouchCross(_point pPX11,_point pPX12,_point pPX13,_point pPX14,
							   _point pPX21,_point pPX22,_point pPX23,_point pPX24,int wide)
{
	double r=(double)wide/4;
	double r1 =sqrt((pPX21.x-pPX11.x)*(pPX21.x-pPX11.x)+(pPX21.y-pPX11.y)*(pPX21.y-pPX11.y));
	double r2 =sqrt((pPX21.x-pPX12.x)*(pPX21.x-pPX12.x)+(pPX21.y-pPX12.y)*(pPX21.y-pPX12.y));
	double r3 =sqrt((pPX21.x-pPX13.x)*(pPX21.x-pPX13.x)+(pPX21.y-pPX13.y)*(pPX21.y-pPX13.y));
	double r4 =sqrt((pPX21.x-pPX14.x)*(pPX21.x-pPX14.x)+(pPX21.y-pPX14.y)*(pPX21.y-pPX14.y));
	double r5 =sqrt((pPX22.x-pPX11.x)*(pPX22.x-pPX11.x)+(pPX22.y-pPX11.y)*(pPX22.y-pPX11.y));
	double r6 =sqrt((pPX22.x-pPX12.x)*(pPX22.x-pPX12.x)+(pPX22.y-pPX12.y)*(pPX22.y-pPX12.y));
	double r7 =sqrt((pPX22.x-pPX13.x)*(pPX22.x-pPX13.x)+(pPX22.y-pPX13.y)*(pPX22.y-pPX13.y));
	double r8 =sqrt((pPX22.x-pPX14.x)*(pPX22.x-pPX14.x)+(pPX22.y-pPX14.y)*(pPX22.y-pPX14.y));
	double r9 =sqrt((pPX23.x-pPX11.x)*(pPX23.x-pPX11.x)+(pPX23.y-pPX11.y)*(pPX23.y-pPX11.y));
	double r10=sqrt((pPX23.x-pPX12.x)*(pPX23.x-pPX12.x)+(pPX23.y-pPX12.y)*(pPX23.y-pPX12.y));
	double r11=sqrt((pPX23.x-pPX13.x)*(pPX23.x-pPX13.x)+(pPX23.y-pPX13.y)*(pPX23.y-pPX13.y));
	double r12=sqrt((pPX23.x-pPX14.x)*(pPX23.x-pPX14.x)+(pPX23.y-pPX14.y)*(pPX23.y-pPX14.y));
	double r13=sqrt((pPX24.x-pPX11.x)*(pPX24.x-pPX11.x)+(pPX24.y-pPX11.y)*(pPX24.y-pPX11.y));
	double r14=sqrt((pPX24.x-pPX12.x)*(pPX24.x-pPX12.x)+(pPX24.y-pPX12.y)*(pPX24.y-pPX12.y));
	double r15=sqrt((pPX24.x-pPX13.x)*(pPX24.x-pPX13.x)+(pPX24.y-pPX13.y)*(pPX24.y-pPX13.y));
	double r16=sqrt((pPX24.x-pPX14.x)*(pPX24.x-pPX14.x)+(pPX24.y-pPX14.y)*(pPX24.y-pPX14.y));
	if (r1<r || r2<r || r3<r || r4<r || r5<r || r6<r || r7<r || r8<r ||
		r9<r || r10<r || r11<r || r12<r || r13<r || r14<r || r15<r || r16<r)
		return 1;
	else
		return 0;
	
}

int CAutoDivFilter::TouchLineCross(_point pP1,_point pP2,
								   _point pPX1,_point pPX2,_point pPX3,_point pPX4,int wide)
{
	double r=(double)wide/2;
	double r1=sqrt((pP1.x-pPX1.x)*(pP1.x-pPX1.x)+(pP1.y-pPX1.y)*(pP1.y-pPX1.y));
	double r2=sqrt((pP1.x-pPX2.x)*(pP1.x-pPX2.x)+(pP1.y-pPX2.y)*(pP1.y-pPX2.y));
	double r3=sqrt((pP1.x-pPX3.x)*(pP1.x-pPX3.x)+(pP1.y-pPX3.y)*(pP1.y-pPX3.y));
	double r4=sqrt((pP1.x-pPX4.x)*(pP1.x-pPX4.x)+(pP1.y-pPX4.y)*(pP1.y-pPX4.y));
	double r5=sqrt((pP2.x-pPX1.x)*(pP2.x-pPX1.x)+(pP2.y-pPX1.y)*(pP2.y-pPX1.y));
	double r6=sqrt((pP2.x-pPX2.x)*(pP2.x-pPX2.x)+(pP2.y-pPX2.y)*(pP2.y-pPX2.y));
	double r7=sqrt((pP2.x-pPX3.x)*(pP2.x-pPX3.x)+(pP2.y-pPX3.y)*(pP2.y-pPX3.y));
	double r8=sqrt((pP2.x-pPX4.x)*(pP2.x-pPX4.x)+(pP2.y-pPX4.y)*(pP2.y-pPX4.y));
	if (r1<r || r2<r || r3<r || r4<r || r5<r || r6<r || r7<r || r8<r)
		return 1;
	else
		return 0;
}

void CAutoDivFilter::DrawDivLine(byte* in,int cx,int cy,_point pt1,_point pt2)
{
	if (pt1.x<0 || pt2.x<0 || pt1.y<0 || pt2.y<0)
		return;
	if (pt1.x>=cx || pt2.x>=cx || pt1.y>=cy || pt2.y>=cy)
		return;
	
	int x,x1,x2,y,y1,y2;
	y1=pt1.y;
	x1=pt1.x;
	y2=pt2.y;
	x2=pt2.x;
	double rad = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	
    if (rad>0)
	{
        double kx = (double)(x2-x1)/rad;
        double ky = (double)(y2-y1)/rad;
        int i;
        for (i=-1;i<=(int)(rad+1);i++)
		{
			y=y1+(int)(ky*i);
			x=x1+(int)(kx*i);
			*(in+y*cx+x)=0;
		}
	}
}

void CAutoDivFilter::ErroImg(byte *img,int cx,int cy,int N)
{
	byte *imgy;
	int ItNum=0;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	int x,y,i,j,flag,M_i;
	
	int bufsize=max(cx*cy/4,C100);
	bufsize=max(bufsize, (cx+cy)*4);

	smart_alloc(pC1, DWORD, bufsize+ADDALLOC);
	smart_alloc(pC2, DWORD, bufsize+ADDALLOC);
	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
			if (*imgy<100)
				*imgy++=0;
			else
				*imgy++=255;
	}
	
	for (y=0;y<cy;y++)
	{
		img[y*cx]=0;img[y*cx+cx-1]=0;
	}
	for (x=0;x<cx;x++)
	{
		img[x]=0;img[(cy-1)*cx+x]=0;
	}
	
	pC1d=pC1;pC2d=pC2;
	Count1=0;Count2=0;
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==255)
				for(i=0;i<4;i++)
					if(*(imgy+pd[i])==0)
					{
						pC1d[Count1++]=imgy-img;
						break;
					}
					imgy++;
		}
	}
	for (i=0;i<Count1;i++)
		*(img+pC1d[i])=0;
	int n=0;
	do
	{
		flag=0;
		if (n-((int)((int)n/5))*5==1 || n-((int)((int)n/5))*5==3 )
			M_i=8; else M_i=4;
		for(i=0;i<Count1;i++)
			for(j=0;j<M_i;j++)
			{
				imgy=img+pC1d[i]+pd[j];
				if(*imgy==255)
				{    // можно ли удалять эту точку
					
					pC2d[Count2++]=imgy-img;
					*imgy=0;
					flag=1;
				}
			}
			
			d_q=pC1d;pC1d=pC2d;pC2d=d_q;
			Count1=Count2;Count2=0;
			n++;
	}while(flag==1 && n<N);
	
	return;
}

int CAutoDivFilter::Separate(byte *img,int cx,int cy,
							 byte **pOutImg,int *pcx,int *pcy,_point *OffSet)
{
	byte *imgy;
	byte *imgy1;
	byte *out;
	byte *outy;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	
	int x,y,x1,y1,i,j,flag,xmax,xmin,ymax,ymin;
	int CountImg=0;
	int cx1,cy1;

	// debug - output mask to file
	/*
	FILE *f=fopen("e:\\mylog.log","a");
	fputs("\n",f);
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			char c='.';
			if(*imgy>=50) c='*';
			if(*imgy>=100) c='#';
			fputc(c,f);
			imgy++;
		}
		fputs("\n",f);
	}
	fputs("\n",f);
	fclose(f);
	/**/

	int bufsize=max(cx*cy/4,C100);
	bufsize=max(bufsize, (cx+cy)*4);

	smart_alloc(pC1, DWORD, bufsize+ADDALLOC);
	smart_alloc(pC2, DWORD, bufsize+ADDALLOC);
	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==255)
			{
				ymin=10000;xmin=10000;
				xmax=0;ymax=0;
				pC1d=pC1;pC2d=pC2;
				Count1=0;Count2=0;
				pC1d[Count1++]=imgy-img;
				*imgy=100;
				do
				{
					flag=0;
					for(i=0;i<Count1;i++)
						for(j=0;j<8;j++)
						{
							imgy1=img+pC1d[i]+pd[j];
							if(*imgy1==255)
							{    // можно ли удалять эту точку
								pC2d[Count2++]=imgy1-img;
								*imgy1=100;
								y1=(int)((imgy1-img)/cx);x1=imgy1-img-y1*cx;
								if(x1<xmin) xmin=x1;if(y1<ymin) ymin=y1;
								if(x1>xmax) xmax=x1;if(y1>ymax) ymax=y1;
								flag=1;
							}
						}
						d_q=pC1d;pC1d=pC2d;pC2d=d_q;
						Count1=Count2;Count2=0;
				}while(flag==1);
				if (xmax-xmin>2 && ymax-ymin>2)
				{
					pcx[CountImg]=xmax-xmin+15;pcy[CountImg]=ymax-ymin+15;
					OffSet[CountImg].x=xmin-7;OffSet[CountImg].y=ymin-7;
					pOutImg[CountImg]=new byte[pcx[CountImg]*pcy[CountImg]];
					out=pOutImg[CountImg];
					cx1=pcx[CountImg];cy1=pcy[CountImg];
					ZeroMemory(out,cx1*cy1*sizeof(byte));
					for (y1=ymin;y1<=ymax;y1++)
					{
						imgy1=img+y1*cx+xmin;
						outy=out+(y1-OffSet[CountImg].y)*cx1+7;
						for (x1=xmin;x1<=xmax;x1++)
							if(*imgy1==100)
							{*imgy1++=0;*outy++=255;}
							else
							{imgy1++;*outy++=0;}
					}
					CountImg++;
				}
			}
			imgy++;
		}
	}
	
	return CountImg;
}

void CAutoDivFilter::Restavr(byte *img,int cx,int cy,
							 byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int CountImg)
{
	
	byte *out;
	byte *outy;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	
	int x,y,i,j,flag,k,x1,y1;
	int cx1,cy1;
	int N=4;
	
	int bufsize=max(cx*cy/4,C100);
	bufsize=max(bufsize, (cx+cy)*4);
	smart_alloc(pC1, DWORD, bufsize+ADDALLOC);
	smart_alloc(pC2, DWORD, bufsize+ADDALLOC);

	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	_point os;
	for (k=0;k<CountImg;k++)
	{
		pC1d=pC1;pC2d=pC2;
		Count1=0;Count2=0;
		out=pOutImg[k];
		cx1=pcx[k];cy1=pcy[k];
		os=OffSet[k];
		pd[0]=-cx1; pd[1]=1; pd[2]=cx1; pd[3]=-1; pd[4]=-cx1+1; pd[5]=cx1+1; pd[6]=cx1-1; pd[7]=-cx1-1;
		for (y=1;y<cy1-1;y++)
		{
			outy=out+y*cx1+1;
			for (x=1;x<cx1-1;x++)
			{
				if(*outy==0)
					for(i=0;i<4;i++)
						if(*(outy+pd[i])==255)
						{
							pC1d[Count1++]=outy-out;
							break;
						}
						outy++;
			}
		}
		for (i=0;i<Count1;i++)
			*(out+pC1d[i])=255;
		int n=0;
		do
		{
			flag=0;
			for(i=0;i<Count1;i++)
				for(j=0;j<4;j++)
				{
					outy=out+pC1d[i]+pd[j];
					if(*outy==0)
					{    // можно ли удалять эту точку
						y1=(int)((outy-out)/cx1);x1=outy-out-y1*cx1;
						x1+=os.x;y1+=os.y;
						if (x1<cx && x1>-1 && y1<cy && y1>-1)
							if(*(img+y1*cx+x1)==255)
							{
								pC2d[Count2++]=outy-out;
								*outy=255;
								flag=1;
							}
					}
				}
				d_q=pC1d;pC1d=pC2d;pC2d=d_q;
				Count1=Count2;Count2=0;
				n++;
		}while(flag==1 && n<N);
	}
	
	return;
}

void CAutoDivFilter::ReSet4Point(int nContour,_point **pContrs,int *pContrsSize,
								 _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22,
								 int *FlagPX,int CountPX)
{
	int i,i1,j,n;
	_point pt;
	for (i1=0;i1<CountPX;i1++)
		if(FlagPX[i1]==0)
		{
			double Rad=0;
			//////////////////////////////////////////////
			double r=100000;
			for(i=0;i<nContour;i++)
			{
				int Size=pContrsSize[i];
				_point *pCPd=pContrs[i];
				if (Size<40) Size=0;
				for (j=0;j<Size;j++)
					if((pPX11[i1].x-pCPd[j].x)*(pPX11[i1].x-pCPd[j].x)+
						(pPX11[i1].y-pCPd[j].y)*(pPX11[i1].y-pCPd[j].y) < r)
					{
						r=(pPX11[i1].x-pCPd[j].x)*(pPX11[i1].x-pCPd[j].x)+
							(pPX11[i1].y-pCPd[j].y)*(pPX11[i1].y-pCPd[j].y);
						pt=pCPd[j];
						n=i;
					}
			}
			pPX11[i1]=pt;
			if (r>Rad) Rad=r;
			//////////////////////////////////////////////////
			r=100000;
			for(i=0;i<nContour;i++)
			{
				int Size=pContrsSize[i];
				_point *pCPd=pContrs[i];
				if (Size<20) Size=0;
				for (j=0;j<Size;j++)
					if((pPX12[i1].x-pCPd[j].x)*(pPX12[i1].x-pCPd[j].x)+
						(pPX12[i1].y-pCPd[j].y)*(pPX12[i1].y-pCPd[j].y) < r)
					{
						r=(pPX12[i1].x-pCPd[j].x)*(pPX12[i1].x-pCPd[j].x)+
							(pPX12[i1].y-pCPd[j].y)*(pPX12[i1].y-pCPd[j].y);
						pt=pCPd[j];
						n=i;
					}
			}
			pPX12[i1]=pt;
			if (r>Rad) Rad=r;
			/////////////////////////////////////////////////////////
			r=100000;
			for(i=0;i<nContour;i++)
			{
				int Size=pContrsSize[i];
				_point *pCPd=pContrs[i];
				if (Size<20) Size=0;
				for (j=0;j<Size;j++)
					if((pPX21[i1].x-pCPd[j].x)*(pPX21[i1].x-pCPd[j].x)+
						(pPX21[i1].y-pCPd[j].y)*(pPX21[i1].y-pCPd[j].y) < r)
					{
						r=(pPX21[i1].x-pCPd[j].x)*(pPX21[i1].x-pCPd[j].x)+
							(pPX21[i1].y-pCPd[j].y)*(pPX21[i1].y-pCPd[j].y);
						pt=pCPd[j];
						n=i;
					}
			}
			pPX21[i1]=pt;
			if (r>Rad) Rad=r;
			/////////////////////////////////////////////////////////
			r=100000;
			for(i=0;i<nContour;i++)
			{
				int Size=pContrsSize[i];
				_point *pCPd=pContrs[i];
				if (Size<20) Size=0;
				for (j=0;j<Size;j++)
					if((pPX22[i1].x-pCPd[j].x)*(pPX22[i1].x-pCPd[j].x)+
						(pPX22[i1].y-pCPd[j].y)*(pPX22[i1].y-pCPd[j].y) < r)
					{
						r=(pPX22[i1].x-pCPd[j].x)*(pPX22[i1].x-pCPd[j].x)+
							(pPX22[i1].y-pCPd[j].y)*(pPX22[i1].y-pCPd[j].y);
						pt=pCPd[j];
						n=i;
					}
			}
			pPX22[i1]=pt;
			if (r>Rad) Rad=r;
			/////////////////////////////////////////////////////////
			if (Rad>10)
				FlagPX[i1]=-1;
		}
}

void CAutoDivFilter::SeparateRect(byte **pOutImg,int *pcx,int *pcy,_point *OffSet,
								  _point pPX11,_point pPX12,_point pPX21,_point pPX22,
								  int *pCountImg)
{
	int cx,cy,i,x1,y1,res1,res2;
	_point PX11,PX12,PX21,PX22;
    int CountImg=*pCountImg;
	int CountImg1=*pCountImg;
	byte *in;
	for (i=0;i<CountImg1;i++)
	{
		PX11=pPX11-OffSet[i];PX12=pPX12-OffSet[i];
		PX21=pPX21-OffSet[i];PX22=pPX22-OffSet[i];
		_point pt4[4] = {PX11,PX12,PX22,PX21};
		Sort4Points(pt4);
		int nAreaSqr = SignedContourArea(pt4,4);
		PX11=pt4[0]; PX12=pt4[1]; PX21=pt4[2]; PX22=pt4[3];
		
		if (pcx[i]>0 && pcy[i]>0 && pOutImg[i]!=0)
		{
			//10.08.2002 - ставлю проверку на наличие слишком близких точек креста
			//(баг на G-4_qq1.image - крест удваивается вместо деления - 
			//все координаты x=1 y=2)
			//лучше б разобраться, откуда такие координаты
			double r=1e5;
			r=min(r,_hypot(pPX12.x-pPX11.x,pPX12.y-pPX11.y));
			r=min(r,_hypot(pPX22.x-pPX12.x,pPX22.y-pPX12.y));
			r=min(r,_hypot(pPX21.x-pPX22.x,pPX21.y-pPX22.y));
			r=min(r,_hypot(pPX11.x-pPX21.x,pPX11.y-pPX21.y));

			r=min(r,_hypot(pPX22.x-pPX11.x,pPX22.y-pPX11.y)); //диагонали
			r=min(r,_hypot(pPX21.x-pPX12.x,pPX21.y-pPX12.y));

			if (PX11.x>0      && PX11.y>0      &&
				PX11.x<pcx[i] && PX11.y<pcy[i] &&
				PX12.x>0      && PX12.y>0      &&
				PX12.x<pcx[i] && PX12.y<pcy[i] &&
				PX21.x>0      && PX21.y>0      &&
				PX21.x<pcx[i] && PX21.y<pcy[i] &&
				PX22.x>0      && PX22.y>0      &&
				PX22.x<pcx[i] && PX22.y<pcy[i] &&
				r>0)
			{
				in=pOutImg[i];cx=pcx[i];cy=pcy[i];
				x1=(PX11.x+PX12.x+PX21.x+PX22.x)/4;
				y1=(PX11.y+PX12.y+PX21.y+PX22.y)/4;
				if (in[y1*cx+x1]==255)
				{
					int nArea=0;
					int nCrossArea1=0, nCrossArea2=0;
					{
						for (int y=0;y<cy;y++)
						{
							byte * imgy=in+y*cx;
							for (int x=0;x<cx;x++)
							{
								if(*imgy) nArea++;
								imgy++;
							}
						}
					}

					DrawDivLine(in,cx,cy,PX11,PX12);
					DrawDivLine(in,cx,cy,PX21,PX22);
					res1=FillCross(pOutImg,pcx,pcy,OffSet,&CountImg,i,x1,y1, &nCrossArea1);
					// debug - output mask to file
					/*
					{
						FILE *f=fopen("e:\\mylog.log","a");
						fputs("\n",f);
						for (int y=0;y<cy;y++)
						{
							byte * imgy=in+y*cx;
							for (int x=0;x<cx;x++)
							{
								char c='.';
								if(*imgy>=50) c='*';
								if(*imgy>=100) c='#';
								fputc(c,f);
								imgy++;
							}
							fputs("\n",f);
						}
						fputs("\n",f);
						fclose(f);
					}
					/**/
					DrawLine(in,cx,cy,PX11,PX12);
					DrawLine(in,cx,cy,PX21,PX22);
					
					DrawDivLine(in,cx,cy,PX12,PX21);
					DrawDivLine(in,cx,cy,PX22,PX11);
					res2=FillCross(pOutImg,pcx,pcy,OffSet,&CountImg,i,x1,y1,&nCrossArea2);
					// debug - output mask to file
					/*
					{
						FILE *f=fopen("e:\\mylog.log","a");
						fputs("\n",f);
						for (int y=0;y<cy;y++)
						{
							byte * imgy=in+y*cx;
							for (int x=0;x<cx;x++)
							{
								char c='.';
								if(*imgy>=50) c='*';
								if(*imgy>=100) c='#';
								fputc(c,f);
								imgy++;
							}
							fputs("\n",f);
						}
						fputs("\n",f);
						fclose(f);
					}
					/**/
					DrawLine(in,cx,cy,PX12,PX21);
					DrawLine(in,cx,cy,PX22,PX11);
					if (res1==0 && res2==0)
					{ // оба куска больше 10x10, оба в списке
						if(nCrossArea1+nCrossArea2-nArea < nAreaSqr*2)
						{ // куски пересекаются по квадрату или типа того
							delete pOutImg[i];pOutImg[i]=0;
							pcx[i]=0;pcy[i]=0;
						}
						else
						{ // куски сильно пересекаются - больше 2 квадратов общая часть (кольцо)
							CountImg--;
							delete pOutImg[CountImg];pOutImg[CountImg]=0;
							pcx[CountImg]=0;pcy[CountImg]=0;
							CountImg--;
							delete pOutImg[CountImg];pOutImg[CountImg]=0;
							pcx[CountImg]=0;pcy[CountImg]=0;
						}
					}
					else
						if(res1+res2>-2)
						{ // один из кусков больше 10x10
							CountImg--;
							delete pOutImg[CountImg];pOutImg[CountImg]=0;
							pcx[CountImg]=0;pcy[CountImg]=0;
						}
				}
			}
		}
	}
	*pCountImg=CountImg;
	return;
}

int CAutoDivFilter::FindD(_point pt1,_point pt2,_point pt3,double *maxd,byte *in,int cx)
{
    double A=sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y));
	double B=sqrt((pt1.x-pt3.x)*(pt1.x-pt3.x)+(pt1.y-pt3.y)*(pt1.y-pt3.y));
	double C=sqrt((pt3.x-pt2.x)*(pt3.x-pt2.x)+(pt3.y-pt2.y)*(pt3.y-pt2.y));
	double cos;
	if (A>=1 && B>=1)
	{
		cos=(A*A+B*B-C*C)/(2*A*B);
		if (cos>0.9999)
			cos=0.9999;
	}
	else
		cos=0.9999;
	double h=B*sqrt(1-cos*cos);
	
	_point ptm = pt1+pt2;
	ptm.x=ptm.x/2;
	ptm.y=ptm.y/2;
	
	if (h>*maxd && in[ptm.y*cx+ptm.x]<100)
	{
		*maxd=h;
		return 1;
	}
	else
		return 0;
}

void CAutoDivFilter::ScanLine(byte* in,int cx,int cy,_point pt1,_point pt2,double *o)
{
	int x,x1,x2,y,y1,y2;
	int a=0;
	y1=pt1.y;
	x1=pt1.x;
	y2=pt2.y;
	x2=pt2.x;
	
	double rad = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	
    if (rad>0)
	{
        double kx = (double)(x2-x1)/rad;
        double ky = (double)(y2-y1)/rad;
        int i;
		
        for (i=0;i<=(int)(rad+0.5);i++)
		{
			y=y1+(int)(ky*i);
			x=x1+(int)(kx*i);
			if (*(in+y*cx+x)<100)
				a++;
		}
		
	}
	*o=a/rad;
	return;
}

int CAutoDivFilter::FindD1(_point pt1,_point pt2,_point pt3,double *maxd,byte *in,int cx)
{
    double A=sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y));
	double B=sqrt((pt1.x-pt3.x)*(pt1.x-pt3.x)+(pt1.y-pt3.y)*(pt1.y-pt3.y));
	double C=sqrt((pt3.x-pt2.x)*(pt3.x-pt2.x)+(pt3.y-pt2.y)*(pt3.y-pt2.y));
	double cos;
	if (A>=1 && B>=1)
	{
		cos=(A*A+B*B-C*C)/(2*A*B);
		if (cos>0.9999)
			cos=0.9999;
	}
	else
		cos=0.9999;
	double h=B*sqrt(1-cos*cos);
	
	_point ptm = pt1+pt2;
	ptm.x=ptm.x/2;
	ptm.y=ptm.y/2;
	
	if (h>*maxd && in[ptm.y*cx+ptm.x]>100 && h>3)
	{
		*maxd=h;
		return 1;
	}
	else
		return 0;
}

int CAutoDivFilter::GetAng(_point pt11,_point pt12,_point pt21,_point pt22,double *ang)
{
	_point pt1,pt2,pt3;
	if (pt12==pt21)
	{
		pt1=pt11;
		pt2=pt12;
        pt3=pt22;
	}
	else
	{
		pt1=pt11;
		pt2=pt12;
        pt3=pt22+pt12-pt21;
	}
	
    double A=sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y));
	double B=sqrt((pt2.x-pt3.x)*(pt2.x-pt3.x)+(pt2.y-pt3.y)*(pt2.y-pt3.y));
	double C=sqrt((pt3.x-pt1.x)*(pt3.x-pt1.x)+(pt3.y-pt1.y)*(pt3.y-pt1.y));
	double cos;
	if (A>=1 && B>=1)
	{
		cos=(A*A+B*B-C*C)/(2*A*B);
		if (cos>0.9999)
			cos=0.9999;
		if (cos<-0.9999)
			cos=-0.9999;
	}
	else
		cos=-0.9999;
	*ang=(double)acos(cos);
	
	return 1;
}

void CAutoDivFilter::FindTouchDiv(_point *pP1,_point *pP2,
								  _point *pP11,_point *pP12,_point *pP21,_point *pP22,
								  _point *pP11s,_point *pP12s,_point *pP21s,_point *pP22s,
								  double *pD11,double *pD12,double *pD21,double *pD22,
								  _point *pDP11,_point *pDP12,_point *pDP21,_point *pDP22,
								  byte *pFlag,int CountP,int width,int cx,int cy)
{
	int i,j,a1,a2,a3,a4,a;
	double ang1,ang2,ang3,ang4;
	
	for (i=0;i<CountP;i++)
		for (j=i+1;j<CountP;j++)
			if (pFlag[i]<5 && pFlag[j]<5)
			{
				a1=max(abs(pP1[i].x-pP1[j].x),abs(pP1[i].y-pP1[j].y));
				a2=max(abs(pP1[i].x-pP2[j].x),abs(pP1[i].y-pP2[j].y));
				a3=max(abs(pP2[i].x-pP1[j].x),abs(pP2[i].y-pP1[j].y));
				a4=max(abs(pP2[i].x-pP2[j].x),abs(pP2[i].y-pP2[j].y));
				if(a1<3)
				{
					a=GetAng(pP11s[i],pP1[i],pP1[i],pP2[i],&ang1);
					a=GetAng(pP12s[i],pP1[i],pP1[i],pP2[i],&ang2);
					a=GetAng(pP11s[j],pP1[j],pP1[j],pP2[j],&ang3);
					a=GetAng(pP12s[j],pP1[j],pP1[j],pP2[j],&ang4);
					if(min(ang1,ang2)<min(ang3,ang4))
						pFlag[i]=255;
					else
						pFlag[j]=255;
				}
				else
					if(a2<3)
					{
						a=GetAng(pP11s[i],pP1[i],pP1[i],pP2[i],&ang1);
						a=GetAng(pP12s[i],pP1[i],pP1[i],pP2[i],&ang2);
						a=GetAng(pP21s[j],pP2[j],pP2[j],pP1[j],&ang3);
						a=GetAng(pP22s[j],pP2[j],pP2[j],pP1[j],&ang4);
						if(min(ang1,ang2)<min(ang3,ang4))
							pFlag[i]=255;
						else
							pFlag[j]=255;
					}
					else
						if(a3<3)
						{
							a=GetAng(pP21s[i],pP2[i],pP2[i],pP1[i],&ang1);
							a=GetAng(pP22s[i],pP2[i],pP2[i],pP1[i],&ang2);
							a=GetAng(pP11s[j],pP1[j],pP1[j],pP2[j],&ang3);
							a=GetAng(pP12s[j],pP1[j],pP1[j],pP2[j],&ang4);
							if(min(ang1,ang2)<min(ang3,ang4))
								pFlag[i]=255;
							else
								pFlag[j]=255;
						}
						else
							if(a4<3)
							{
								a=GetAng(pP21s[i],pP2[i],pP2[i],pP1[i],&ang1);
								a=GetAng(pP22s[i],pP2[i],pP2[i],pP1[i],&ang2);
								a=GetAng(pP21s[j],pP2[j],pP2[j],pP1[j],&ang3);
								a=GetAng(pP22s[j],pP2[j],pP2[j],pP1[j],&ang4);
								if(min(ang1,ang2)<min(ang3,ang4))
									pFlag[i]=255;
								else
									pFlag[j]=255;
							}
							
			}
			
			return;
}

void CAutoDivFilter::FindNoTouchDiv(_point *pP1,_point *pP2,
									_point *pP11,_point *pP12,_point *pP21,_point *pP22,
									_point *pP11s,_point *pP12s,_point *pP21s,_point *pP22s,
									double *pD11,double *pD12,double *pD21,double *pD22,
									_point *pDP11,_point *pDP12,_point *pDP21,_point *pDP22,
									byte *pFlag,int CountP,int width,int cx,int cy)
{
	double ang_porog=3.1415128*2/3.5;
	double ang_porog1=3.1415128/2;
	double ang_porog2=3.1415128/4;
	double ang_porog3=0.60;
	double ang_porog4=1.3;
	int i,a;
	for (i=0;i<CountP;i++)
		pFlag[i]=255;
	double ang1,ang2,ang3,ang4,ang5,ang6;
	//  проверка -88888888888
	for (i=0;i<CountP;i++)
	{
		if(pP1[i].x<0 || pP1[i].x>=cx) pFlag[i]=128;
		if(pP2[i].x<0 || pP2[i].x>=cx) pFlag[i]=128;
		if(pP11[i].x<0 || pP11[i].x>=cx) pFlag[i]=128;
		if(pP12[i].x<0 || pP12[i].x>=cx) pFlag[i]=128;
		if(pP21[i].x<0 || pP21[i].x>=cx) pFlag[i]=128;
		if(pP22[i].x<0 || pP22[i].x>=cx) pFlag[i]=128;
		if(pDP11[i].x<0 || pDP11[i].x>=cx) pFlag[i]=128;
		if(pDP12[i].x<0 || pDP12[i].x>=cx) pFlag[i]=128;
		if(pDP21[i].x<0 || pDP21[i].x>=cx) pFlag[i]=128;
		if(pDP22[i].x<0 || pDP22[i].x>=cx) pFlag[i]=128;
		if(pP11s[i].x<0 || pP11s[i].x>=cx) pFlag[i]=128;
		if(pP12s[i].x<0 || pP12s[i].x>=cx) pFlag[i]=128;
		if(pP21s[i].x<0 || pP21s[i].x>=cx) pFlag[i]=128;
		if(pP22s[i].x<0 || pP22s[i].x>=cx) pFlag[i]=128;
		if(max(abs(pP1[i].x-pP11[i].x),abs(pP1[i].y-pP11[i].y))<4) pFlag[i]=128;
		if(max(abs(pP1[i].x-pP12[i].x),abs(pP1[i].y-pP12[i].y))<4) pFlag[i]=128;
		if(max(abs(pP2[i].x-pP21[i].x),abs(pP2[i].y-pP21[i].y))<4) pFlag[i]=128;
		if(max(abs(pP2[i].x-pP22[i].x),abs(pP2[i].y-pP22[i].y))<4) pFlag[i]=128;
		if(max(abs(pP1[i].x-pDP11[i].x),abs(pP1[i].y-pDP11[i].y))<4) pFlag[i]=128;
		if(max(abs(pP1[i].x-pDP12[i].x),abs(pP1[i].y-pDP12[i].y))<4) pFlag[i]=128;
		if(max(abs(pP2[i].x-pDP21[i].x),abs(pP2[i].y-pDP21[i].y))<4) pFlag[i]=128;
		if(max(abs(pP2[i].x-pDP22[i].x),abs(pP2[i].y-pDP22[i].y))<4) pFlag[i]=128;
		if(max(abs(pP1[i].x-pP11s[i].x),abs(pP1[i].y-pP11s[i].y))<3) pFlag[i]=128;
		if(max(abs(pP1[i].x-pP12s[i].x),abs(pP1[i].y-pP12s[i].y))<3) pFlag[i]=128;
		if(max(abs(pP2[i].x-pP21s[i].x),abs(pP2[i].y-pP21s[i].y))<3) pFlag[i]=128;
		if(max(abs(pP2[i].x-pP22s[i].x),abs(pP2[i].y-pP22s[i].y))<3) pFlag[i]=128;
	}
	
	double r1,r2,r3,r4,r;
	for (i=0;i<CountP;i++)
	{
		if (pFlag[i]==255)
		{
			a=GetAng(pDP11[i],pP1[i],pP2[i],pDP21[i],&ang1);
			a=GetAng(pDP12[i],pP1[i],pP2[i],pDP22[i],&ang2);
			
			r1=sqrt((pDP11[i].x-pDP21[i].x)*(pDP11[i].x-pDP21[i].x)+
				(pDP11[i].y-pDP21[i].y)*(pDP11[i].y-pDP21[i].y));
			r2=sqrt((pDP12[i].x-pDP22[i].x)*(pDP12[i].x-pDP22[i].x)+
				(pDP12[i].y-pDP22[i].y)*(pDP12[i].y-pDP22[i].y));
			r3=sqrt((pDP12[i].x-pDP21[i].x)*(pDP12[i].x-pDP21[i].x)+
				(pDP12[i].y-pDP21[i].y)*(pDP12[i].y-pDP21[i].y));
			r4=sqrt((pDP11[i].x-pDP22[i].x)*(pDP11[i].x-pDP22[i].x)+
				(pDP11[i].y-pDP22[i].y)*(pDP11[i].y-pDP22[i].y));
			r=sqrt((pP1[i].x-pP2[i].x)*(pP1[i].x-pP2[i].x)+
				(pP1[i].y-pP2[i].y)*(pP1[i].y-pP2[i].y));
			
			if (ang1>ang_porog2 && ang2>ang_porog2 && r<r1*2 && r<r2*2 && r<r3*2 && r<r4*2)
			{
				a=GetAng(pP11s[i],pP1[i],pP1[i],pP2[i],&ang3);
				a=GetAng(pP12s[i],pP1[i],pP1[i],pP2[i],&ang4);
				a=GetAng(pP21s[i],pP2[i],pP2[i],pP1[i],&ang5);
				a=GetAng(pP22s[i],pP2[i],pP2[i],pP1[i],&ang6);
				if (ang3>1.0 && ang4>1.0 && ang5>1.0 && ang6>1.0)
				{
					pFlag[i]=0;
				}
				else
					if (sqrt((pP1[i].x-pP2[i].x)*(pP1[i].x-pP2[i].x)+
						(pP1[i].y-pP2[i].y)*(pP1[i].y-pP2[i].y)) < width/1.5)
					{
						pFlag[i]=0;
					}
					else
					{
						a=GetAng(pDP11[i],pP1[i],pP1[i],pP2[i],&ang3);
						a=GetAng(pDP12[i],pP1[i],pP1[i],pP2[i],&ang4);
						a=GetAng(pDP21[i],pP2[i],pP2[i],pP1[i],&ang5);
						a=GetAng(pDP22[i],pP2[i],pP2[i],pP1[i],&ang6);
						if (ang3>1.3 && ang4>1.3 && ang5>1.3 && ang6>1.3)
						{
							pFlag[i]=0;
						}
					}
			}
		}
	}
	///////////////////////////////////////////////////////////////
	for (i=0;i<CountP;i++)
	{
		if (pFlag[i]==255)
		{
			a=GetAng(pDP11[i],pP1[i],pP1[i],pDP12[i],&ang1);
			a=GetAng(pDP21[i],pP2[i],pP2[i],pDP22[i],&ang2);
			
			a=GetAng(pDP11[i],pP1[i],pP2[i],pDP21[i],&ang3);
			a=GetAng(pDP12[i],pP1[i],pP2[i],pDP22[i],&ang4);
			
            if (ang3>2.1)
			{
				a=GetAng(pDP11[i],pP1[i],pP1[i],pP2[i],&ang5);
				a=GetAng(pP1[i],pP2[i],pP2[i],pDP21[i],&ang6);
				if (ang5>2.4 && ang6>2.4)
				{
					if (sqrt((pP1[i].x-pP2[i].x)*(pP1[i].x-pP2[i].x)+
						(pP1[i].y-pP2[i].y)*(pP1[i].y-pP2[i].y)) < width*2.0)
					{
						pFlag[i]=1;
					}
				}
			}
			else
				if (ang4>2.1)
				{
					a=GetAng(pDP12[i],pP1[i],pP1[i],pP2[i],&ang5);
					a=GetAng(pP1[i],pP2[i],pP2[i],pDP22[i],&ang6);
					if (ang5>2.4 && ang6>2.4)
					{
						if (sqrt((pP1[i].x-pP2[i].x)*(pP1[i].x-pP2[i].x)+
							(pP1[i].y-pP2[i].y)*(pP1[i].y-pP2[i].y)) < width*2.0)
						{
							pFlag[i]=1;
						}
					}
				}
		}
	}
	
	return;
}

void CAutoDivFilter::DrawLine(byte* in,int cx,int cy,_point pt1,_point pt2)
{
	if (pt1.x<0 || pt2.x<0 || pt1.y<0 || pt2.y<0)
		return;
	if (pt1.x>=cx || pt2.x>=cx || pt1.y>=cy || pt2.y>=cy)
		return;
	
	int x,x1,x2,y,y1,y2;
	y1=pt1.y;
	x1=pt1.x;
	y2=pt2.y;
	x2=pt2.x;
	double rad = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	
    if (rad>0)
	{
        double kx = (double)(x2-x1)/rad;
        double ky = (double)(y2-y1)/rad;
        int i;
        for (i=-1;i<=(int)(rad+1);i++)
		{
			y=y1+(int)(ky*i);
			x=x1+(int)(kx*i);
			*(in+y*cx+x)=255;
		}
	}
}

int CAutoDivFilter::Skelet(byte *img,int cx,int cy)
{
	byte *imgy;
	int ItNum=0;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	int x,y,i,j,j1,flag,a,a1,M_i,k,k1;
	
	int bufsize=max(cx*cy/4,C100);
	bufsize=max(bufsize, (cx+cy)*4);
	smart_alloc(pC1, DWORD, bufsize+ADDALLOC);
	smart_alloc(pC2, DWORD, bufsize+ADDALLOC);
	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	DWORD DQ;
	
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
			if (*imgy<100)
				*imgy++=0;
			else
				*imgy++=255;
	}
	
	for (y=0;y<cy;y++)
	{
		img[y*cx]=0;img[y*cx+cx-1]=0;
	}
	for (x=0;x<cx;x++)
	{
		img[x]=0;img[(cy-1)*cx+x]=0;
	}
	
	pC1d=pC1;pC2d=pC2;
	Count1=0;Count2=0;
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==255)
				for(i=0;i<4;i++)
					if(*(imgy+pd[i])==0)
					{
						pC1d[Count1++]=imgy-img;
						break;
					}
					imgy++;
		}
	}
	for (i=0;i<Count1;i++)
		*(img+pC1d[i])=0;
	int n=0;
	do
	{
		flag=0;
		if (n-((int)((int)n/5))*5==1 || n-((int)((int)n/5))*5==3 )
			M_i=8; else M_i=4;
		for(i=0;i<Count1;i++)
			for(j=0;j<M_i;j++)
			{
				imgy=img+pC1d[i]+pd[j];
				if(*imgy==255)
				{    // можно ли удалять эту точку
					if(n>1)
					{
						a1=*(imgy+pd1[0]);
						k=0;k1=0;
						for(j1=1;j1<9;j1++)
						{
							a=*(imgy+pd1[j1]);
							if(a1==255)
							{
								k++;if(a!=255) k1++;
							}
							a1=a;
						}
						if(k1==1 && k>1 && k<6)//<7
						{
							pC2d[Count2++]=imgy-img;
							*imgy=0;
							flag=1;
						}
						if(k1==2 && k<6)//<5
						{
							if(*(imgy+pd1[0])==255 && *(imgy+pd1[1])==0 && *(imgy+pd1[2])==255)
							{
								pC2d[Count2++]=imgy-img;
								*imgy=0;
								flag=1;
							}
							else
								if(*(imgy+pd1[2])==255 && *(imgy+pd1[3])==0 && *(imgy+pd1[4])==255)
								{
									pC2d[Count2++]=imgy-img;
									*imgy=0;
									flag=1;
								}
								else
									if(*(imgy+pd1[4])==255 && *(imgy+pd1[5])==0 && *(imgy+pd1[6])==255)
									{
										pC2d[Count2++]=imgy-img;
										*imgy=0;
										flag=1;
									}
									else
										if(*(imgy+pd1[6])==255 && *(imgy+pd1[7])==0 && *(imgy+pd1[8])==255)
										{
											pC2d[Count2++]=imgy-img;
											*imgy=0;
											flag=1;
										}
						}
					}
					else
					{
						pC2d[Count2++]=imgy-img;
						*imgy=0;
						flag=1;
					}
				}
			}
			
			d_q=pC1d;pC1d=pC2d;pC2d=d_q;
			Count1=Count2;Count2=0;
			dbg_assert(Count1<bufsize);
			for (j1=0;j1<Count1/2;j1++)
			{
				DQ=pC1d[j1];pC1d[j1]=pC1d[Count1-1-j1];pC1d[Count1-1-j1]=DQ;
			}
			n++;
	}while(flag==1);
	
	ItNum=n;
	return ItNum;
}

void CAutoDivFilter::DelShortLine(byte *img,int cx,int cy,int ItNum,int width)
{
	
	int x,y,i,k,a1,a,flag,l;
	byte *imgy;
	byte *imgy1;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==255)
			{
				a1=*(imgy+pd1[0]);
				k=0;
				for(i=1;i<9;i++)
				{
					a=*(imgy+pd1[i]);
					if(a1==255 && a!=255)
						k++;
					a1=a;
				}
				if (k==1)  // вычисление размера отрезка
				{
                    imgy1=imgy;
					*imgy1=100;
					l=1;
					do
					{
						flag=0;
						for (i=0;i<8;i++)
							if (*(imgy1+pd[i])==255)
							{
								imgy1=imgy1+pd[i];break;
							}
							if (*imgy1==255)
							{
								a1=*(imgy1+pd1[0]);
								k=0;
								for(i=1;i<9;i++)
								{
									a=*(imgy1+pd1[i]);
									if(a1==255 && a!=255)
										k++;
									a1=a;
								}
								if (k==1)
								{
									*imgy1=100;
									flag=1;
									l++;
								}
							}
					}while(flag==1);
					if (l>width/2) ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						a=255;
					else
						a=0;
					imgy1=imgy;
					*imgy1=a;
					do
					{
						flag=0;
						for (i=0;i<8;i++)
							if (*(imgy1+pd[i])==100)
							{
								imgy1=imgy1+pd[i];
								*imgy1=a;flag=1;break;
							}
					}while(flag==1);
				}
			}
			imgy++;
		}
	}
	return;
}

void CAutoDivFilter::FindXX(byte *img,int cx,int cy,int ItNum,int width,
							_point *pPX1,_point *pPX2,int *pCountPX)
{
	int x,y,i,j,a,a1,k;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	byte *imgy;
	_point pPt[C200];
	int CountPt=0;int CountPX=0;
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==255)
			{
				a1=*(imgy+pd1[0]);
				k=0;
				for(i=1;i<9;i++)
				{
					a=*(imgy+pd1[i]);
					if(a1==255 && a!=255)
						k++;
					a1=a;
				}
				if (k==3)
				{
					pPt[CountPt].x=x;pPt[CountPt].y=y;CountPt++;
				}
				if (k==4)
				{
					pPX1[CountPX].x=x;pPX1[CountPX].y=y;
					pPX2[CountPX].x=x;pPX2[CountPX].y=y;CountPX++;
				}
			}
			imgy++;
		}
	}
	
	double r;a=-1;
	for (i=0;i<CountPt-1;i++)
	{
		r=10000;
		for (j=i+1;j<CountPt;j++)
		{
			if (sqrt((pPt[i].x-pPt[j].x)*(pPt[i].x-pPt[j].x)+
				(pPt[i].y-pPt[j].y)*(pPt[i].y-pPt[j].y)) < r)
			{
				r=sqrt((pPt[i].x-pPt[j].x)*(pPt[i].x-pPt[j].x)+
					(pPt[i].y-pPt[j].y)*(pPt[i].y-pPt[j].y));
				a=j;
			}
		}
		if ((int)r<width/2 && a!=-1)
		{
			pPX1[CountPX].x=pPt[i].x;pPX1[CountPX].y=pPt[i].y;
			pPX2[CountPX].x=pPt[a].x;pPX2[CountPX].y=pPt[a].y;CountPX++;
		}
	}
	
	*pCountPX=CountPX;
	return;
}

void CAutoDivFilter::FindNP(byte *img,byte *in,int cx,int cy,int ItNum,int width,
							_point *pPX1,_point *pPX2,
							_point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22,
							int *FlagPX,int CountPX)
{
	int i;
	
	for(i=0;i<CountPX;i++)
	{
		if(pPX1[i]==pPX2[i])
		{
			aaa1(img,in,cx,cy,ItNum,width,pPX1[i],
				&(pPX11[i]),&(pPX12[i]),&(pPX21[i]),&(pPX22[i]));
		}
		else
		{
			aaa2(img,in,cx,cy,ItNum,width,pPX1[i],pPX2[i],
				&(pPX11[i]),&(pPX12[i]),&(pPX21[i]),&(pPX22[i]));
		}
	}
	return;
}

int CAutoDivFilter::FillCross(byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int *pCountImg,
							  int N,int xp,int yp, int *pnCrossArea)
{
	byte *img;
	byte *imgy;
	byte *out;
	byte *outy;
	int x1,y1,i,j,flag,xmax,xmin,ymax,ymin,cx,cy;
	int CountImg=*pCountImg;
	int cx1,cy1;

	*pnCrossArea=0;
	
	img=pOutImg[N];
	cx=pcx[N];cy=pcy[N];
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	
	int bufsize=max(cx*cy/4,C100);
	bufsize=max(bufsize, (cx+cy)*4);
	smart_alloc(pC1, DWORD, bufsize+ADDALLOC);
	smart_alloc(pC2, DWORD, bufsize+ADDALLOC);

	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	
	imgy=img+cx*yp+xp;
	ymin=10000;xmin=10000;
	xmax=0;ymax=0;
	pC1d=pC1;pC2d=pC2;
	Count1=0;Count2=0;
	pC1d[Count1++]=imgy-img;
	*imgy=100; // пометили затравку
	do
	{ // заливаем от опорной точки цветом 100 там, где было 255; можно упростить
		flag=0;
		for(i=0;i<Count1;i++)
			for(j=0;j<4;j++)
			{
				imgy=img+pC1d[i]+pd[j];
				if(*imgy==255)
				{    // можно ли удалять эту точку
					pC2d[Count2++]=imgy-img;
					*imgy=100;
					y1=(int)((imgy-img)/cx);x1=imgy-img-y1*cx;
					if(x1<xmin) xmin=x1;if(y1<ymin) ymin=y1;
					if(x1>xmax) xmax=x1;if(y1>ymax) ymax=y1;
					flag=1;
				}
			}
			d_q=pC1d;pC1d=pC2d;pC2d=d_q;
			Count1=Count2;Count2=0;
	}while(flag==1);
	if (xmax-xmin>10 && ymax-ymin>10)
	{ // если размер залитой области больше 10x10
		int nArea100=0;
		pcx[CountImg]=xmax-xmin+15;pcy[CountImg]=ymax-ymin+15;
		OffSet[CountImg].x=xmin-7;OffSet[CountImg].y=ymin-7;
		//! todo: проверять, чтобы images не вылезали за пределы исходного
		pOutImg[CountImg]=new byte[pcx[CountImg]*pcy[CountImg]];
		out=pOutImg[CountImg];
		cx1=pcx[CountImg];cy1=pcy[CountImg];
		ZeroMemory(out,cx1*cy1*sizeof(byte));
		for (y1=ymin;y1<=ymax;y1++)
		{
			imgy=img+y1*cx+xmin;
			outy=out+(y1-OffSet[CountImg].y)*cx1+7;
			for (x1=xmin;x1<=xmax;x1++)
			{
				if(*imgy==100)
				{
					nArea100++;
					*imgy++=255;*outy++=255;
				}
				else
				{
					imgy++;*outy++=0;
				}
			}
		}
		*pnCrossArea=nArea100;
		OffSet[CountImg]+=OffSet[N];
		CountImg++;
		*pCountImg=CountImg;
		return 0;
	}
	else
	{
		return -1;
	}
}

void CAutoDivFilter::aaa1(byte *img,byte *in,int cx,int cy,int ItNum,int width,
						  _point pPX1,
						  _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22)
{
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	int i,j,flag,a,a1,d;
	byte *imgy=img+cx*pPX1.y+pPX1.x;
	byte *pimgy[8];
	*imgy=100;
	a=0;
	for (i=0;i<8;i++)


		if(*(imgy+pd1[i])==255)
			pimgy[a++]=imgy+pd1[i];
		else
		{
			*pPX11=*pPX12=*pPX21=*pPX22=_point(0,0);
			return;
		}

        for (i=0;i<4;i++)
        {
			a1=0;


			do
			{
				*(pimgy[i])=100;
				flag=0;a=0;a1++;
				for (j=0;j<8;j++)
					if(*(pimgy[i]+pd1[j])==255)
					{
						a++;d=pd1[j];
					}
					if(a==1 && a1<=width*2)
					{
						flag=1;
						pimgy[i]=pimgy[i]+d;
					}
			}while(flag==1);

        }
        for (i=0;i<cx*cy;i++)
			if(img[i]==100) img[i]=255;
			
			_point pt1,pt2,pt3,pt4,pt;
			pt=pPX1;
			pt1.y=(pimgy[0]-img)/cx;pt1.x=(pimgy[0]-img)-pt1.y*cx;
			pt2.y=(pimgy[1]-img)/cx;pt2.x=(pimgy[1]-img)-pt2.y*cx;
			pt3.y=(pimgy[2]-img)/cx;pt3.x=(pimgy[2]-img)-pt3.y*cx;
			pt4.y=(pimgy[3]-img)/cx;pt4.x=(pimgy[3]-img)-pt4.y*cx;
			ScanCross(in,cx,cy,ItNum,width,
				pt,pt1,pt,pt2,pt,pt3,pt,pt4,
				pPX11,pPX12,pPX21,pPX22);
			return;
}

void CAutoDivFilter::aaa2(byte *img,byte *in,int cx,int cy,int ItNum,int width,
						  _point pPX1,_point pPX2,
						  _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22)
{
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	int i,j,flag,a,a1,a2,da,d;
	byte *imgy1=img+cx*pPX1.y+pPX1.x;
	byte *imgy2=img+cx*pPX2.y+pPX2.x;
	byte *pimgy[16];
	*imgy1=100; *imgy2=100;
	a2=0;
	for (i=0;i<8;i++)
	{
		if(*(imgy1+pd1[i])==255)
		{
			pimgy[a2++]=imgy1+pd1[i];
			*(imgy1+pd1[i])=100;
		}
	}
	da=a2;
	for (i=0;i<8;i++)
	{
		if(*(imgy2+pd1[i])==255)
		{
			pimgy[a2++]=imgy2+pd1[i];
			*(imgy2+pd1[i])=100;
		}
	}
	for (i=0;i<a2;i++)
	{
		a1=0;
		do
		{
			*(pimgy[i])=100;
			flag=0;a=0;a1++;
			for (j=0;j<8;j++)
				if(*(pimgy[i]+pd1[j])==255)
				{
					a++;d=pd1[j];
				}
				if(a==1 && a1<=width*2)
				{
					flag=1;
					pimgy[i]=pimgy[i]+d;
				}
		}while(flag==1);
	}
	for (i=0;i<cx*cy;i++)
		if(img[i]==100) img[i]=255;
		
        if(a2==5)
        {
			_point pt1,pt2,pt3,pt;
			pt=pPX2;
			pt1.y=(pimgy[0]-img)/cx;pt1.x=(pimgy[0]-img)-pt1.y*cx;
			pt2.y=(pimgy[1]-img)/cx;pt2.x=(pimgy[1]-img)-pt2.y*cx;
			pt3.y=(pimgy[2]-img)/cx;pt3.x=(pimgy[2]-img)-pt3.y*cx;
			if (max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt2.x-pt.x),abs(pt2.y-pt.y)) &&
				max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
				pimgy[0]=0;
			else
				if (max(abs(pt2.x-pt.x),abs(pt2.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
					pimgy[1]=0;
				else
					pimgy[2]=0;
        }
		
        if(a2==6)
        {
			_point pt1,pt2,pt3,pt;
			pt=pPX2;
			pt1.y=(pimgy[0]-img)/cx;pt1.x=(pimgy[0]-img)-pt1.y*cx;
			pt2.y=(pimgy[1]-img)/cx;pt2.x=(pimgy[1]-img)-pt2.y*cx;
			pt3.y=(pimgy[2]-img)/cx;pt3.x=(pimgy[2]-img)-pt3.y*cx;
			if (max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt2.x-pt.x),abs(pt2.y-pt.y)) &&
				max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
				pimgy[0]=0;
			else
				if (max(abs(pt2.x-pt.x),abs(pt2.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
					pimgy[1]=0;
				else
					pimgy[2]=0;
                //////////////////////////////////////////////////////////////////////
                pt=pPX1;
                pt1.y=(pimgy[3]-img)/cx;pt1.x=(pimgy[3]-img)-pt1.y*cx;
                pt2.y=(pimgy[4]-img)/cx;pt2.x=(pimgy[4]-img)-pt2.y*cx;
                pt3.y=(pimgy[5]-img)/cx;pt3.x=(pimgy[5]-img)-pt3.y*cx;
                if (max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt2.x-pt.x),abs(pt2.y-pt.y)) &&
					max(abs(pt1.x-pt.x),abs(pt1.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
					pimgy[3]=0;
                else
					if (max(abs(pt2.x-pt.x),abs(pt2.y-pt.y))<max(abs(pt3.x-pt.x),abs(pt3.y-pt.y)))
						pimgy[4]=0;
					else
						pimgy[5]=0;
        }
        for(i=0;i<a2;i++)
        {
			if(pimgy[i]==0)
			{
				for(j=i+1;j<a2;j++)
					pimgy[j-1]=pimgy[j];
				i--;a2--;
			}
        }
		
        _point pt11,pt12,pt21,pt22,pt1,pt2;
        pt1=pPX1;
        pt2=pPX2;
        pt11.y=(pimgy[0]-img)/cx;pt11.x=(pimgy[0]-img)-pt11.y*cx;
        pt12.y=(pimgy[1]-img)/cx;pt12.x=(pimgy[1]-img)-pt12.y*cx;
        pt21.y=(pimgy[2]-img)/cx;pt21.x=(pimgy[2]-img)-pt21.y*cx;
        pt22.y=(pimgy[3]-img)/cx;pt22.x=(pimgy[3]-img)-pt22.y*cx;
        ScanCross(in,cx,cy,ItNum,width,
			pt1,pt11,pt1,pt12,pt2,pt21,pt2,pt22,
			pPX11,pPX12,pPX21,pPX22);
        return;
}

void CAutoDivFilter::ScanCross(byte *in,int cx,int cy,int ItNum,int width,
							   _point pt11,_point pt12,_point pt21,_point pt22,
							   _point pt31,_point pt32,_point pt41,_point pt42,
							   _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22)
{
	double r1,r2,r3,r4;
	r1=sqrt((pt11.x-pt12.x)*(pt11.x-pt12.x)+(pt11.y-pt12.y)*(pt11.y-pt12.y));
	r2=sqrt((pt21.x-pt22.x)*(pt21.x-pt22.x)+(pt21.y-pt22.y)*(pt21.y-pt22.y));
	r3=sqrt((pt31.x-pt32.x)*(pt31.x-pt32.x)+(pt31.y-pt32.y)*(pt31.y-pt32.y));
	r4=sqrt((pt41.x-pt42.x)*(pt41.x-pt42.x)+(pt41.y-pt42.y)*(pt41.y-pt42.y));
	int k=0;
	if (r1<width/1.5) k++;
	if (r2<width/1.5) k++;
	if (r3<width/1.5) k++;
	if (r4<width/1.5) k++;
	if (k>1)
	{
		*pPX11=*pPX12=*pPX21=*pPX22=_point(0,0);
		return;
	}
	double ang1,ang2,ang3,ang;
	int a;
	_point p;
	ang1=0;ang2=0;ang3=0;
	a=GetAng(pt12,pt11,pt21,pt22,&ang);ang1=ang1+ang;
	a=GetAng(pt22,pt21,pt31,pt32,&ang);ang1=ang1+ang;
	a=GetAng(pt32,pt31,pt41,pt42,&ang);ang1=ang1+ang;
	a=GetAng(pt42,pt41,pt11,pt12,&ang);ang1=ang1+ang;
	///////////////////////////////////////////////////      2-3
	a=GetAng(pt12,pt11,pt31,pt32,&ang);ang2=ang2+ang;
	a=GetAng(pt32,pt31,pt21,pt22,&ang);ang2=ang2+ang;
	a=GetAng(pt22,pt21,pt41,pt42,&ang);ang2=ang2+ang;
	a=GetAng(pt42,pt41,pt11,pt12,&ang);ang2=ang2+ang;
	///////////////////////////////////////////////////      3-4
	a=GetAng(pt12,pt11,pt21,pt22,&ang);ang3=ang3+ang;
	a=GetAng(pt22,pt21,pt41,pt42,&ang);ang3=ang3+ang;
	a=GetAng(pt42,pt41,pt31,pt32,&ang);ang3=ang3+ang;
	a=GetAng(pt32,pt31,pt11,pt12,&ang);ang3=ang3+ang;
	///////////////////////////////////////////////////
	if (ang2<ang1 && ang2<ang3)
	{
        p=pt21;pt21=pt31;pt31=p;
		p=pt22;pt22=pt32;pt32=p;
	}
	else
		if (ang3<ang1 && ang3<ang2)
		{
			p=pt31;pt31=pt41;pt41=p;
			p=pt32;pt32=pt42;pt42=p;
		}
		
        int res1=ScanAng(in,cx,cy,pt11,pt12,pt21,pt22,pPX11);
        int res2=ScanAng(in,cx,cy,pt21,pt22,pt31,pt32,pPX12);
        int res3=ScanAng(in,cx,cy,pt31,pt32,pt41,pt42,pPX21);
        int res4=ScanAng(in,cx,cy,pt41,pt42,pt11,pt12,pPX22);
		
        if (res1+res2+res3+res4!=4)
        {
			*pPX11=*pPX12=*pPX21=*pPX22=_point(0,0);
        }
		
        return;
}

int CAutoDivFilter::ScanAng(byte *in,int cx,int cy,
							_point pt11,_point pt12,_point pt21,_point pt22,
							_point *pPX)
{
	double rd1=sqrt((pt11.x-pt12.x)*(pt11.x-pt12.x)+(pt11.y-pt12.y)*(pt11.y-pt12.y));
	double rd2=sqrt((pt21.x-pt22.x)*(pt21.x-pt22.x)+(pt21.y-pt22.y)*(pt21.y-pt22.y));
	if (rd1<1 || rd2<1)
		return -1;
	
	int x11,x21,x1,x2,y11,y21,y1,y2,x,y,i,f1,f2,flag;
	x11=pt11.x;x21=pt21.x;
	y11=pt11.y;y21=pt21.y;
	x1=pt11.x;x2=pt21.x;
	y1=pt11.y;y2=pt21.y;
	
    double kx1,kx2,ky1,ky2;
	kx1=(double)(pt12.x-pt11.x)/rd1;
	kx2=(double)(pt22.x-pt21.x)/rd2;
	ky1=(double)(pt12.y-pt11.y)/rd1;
	ky2=(double)(pt22.y-pt21.y)/rd2;
	_point pc;
	pc.x=(pt11.x+pt21.x)/2;
	pc.y=(pt11.y+pt21.y)/2;
	
	i=0;
	do
	{
		flag=0;f1=0;f2=0;
		x=int(x11+kx1*i); y=int(y11+ky1*i);
		if (x>=0 && y>=0 && x<cx && y<cy)
		{x1=x;y1=y;f1=1;}
		x=int(x21+kx2*i); y=int(y21+ky2*i);
		if (x>=0 && y>=0 && x<cx && y<cy)
		{x2=x;y2=y;f2=1;}
		
		if (f1+f2==0)
			flag=-1;
		else
			flag=ScanLine(in,cx,cy,x1,y1,x2,y2,pc.x,pc.y,pPX);
		
		i++;
	}while(flag==0);
	
	if (flag==1)
		return 1;
	else
		return -1;
}

int CAutoDivFilter::ScanLine(byte *in,int cx,int cy,int x1,int y1,int x2,int y2,
							 int xc,int yc,_point *pPX)
{
	int x,y;
	double i;
	int a=0;
	double rad = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	if (rad<1)
		return 0;
	
	double kx = (double)(x2-x1)/rad;
    double ky = (double)(y2-y1)/rad;
	double kx1,ky1,r1;
	double r=(sqrt((xc-x1)*(xc-x1)+(yc-y1)*(yc-y1))+
		sqrt((xc-x2)*(xc-x2)+(yc-y2)*(yc-y2)))/2;
    for (i=3;i<rad-3;i+=0.5)
	{
		y=int(y1+ky*i);
		x=int(x1+kx*i);
		r1=sqrt((xc-x)*(xc-x)+(yc-y)*(yc-y));
		if (r1>1)
		{
			kx1 = (double)(x-xc)/r1;
			ky1 = (double)(y-yc)/r1;
			y=int(yc+ky1*r);
			x=int(xc+kx1*r);
			if (*(in+y*cx+x)<100)
			{
				(*pPX).x=x;(*pPX).y=y;
				return 1;
			}
		}
	}
	
    return 0;
}

