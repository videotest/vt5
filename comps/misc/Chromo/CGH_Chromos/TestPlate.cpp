#include "stdafx.h"
#include "testplate.h"
#include "resource.h"
#include "misc_utils.h"
#include "alloc.h"
#include "math.h"

IMPLEMENT_DYNCREATE(CTestPlate,	CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CTestPlate, "CGH_Chromos.TestPlate", 
0x95351b0b, 0x35cc, 0x4d8c, 0xa5, 0xe2, 0x49, 0x1f, 0x88, 0xf1, 0x9a, 0x19);

static const GUID guidTestPlate = 
{ 0xd871ad2f, 0x50a7, 0x4081, { 0xbf, 0xde, 0x4a, 0x51, 0x9c, 0x3b, 0x28, 0x59 } };

ACTION_INFO_FULL(CTestPlate, IDS_ACTION_TESTPLATE, -1, -1, guidTestPlate);
//ACTION_TARGET(CTestPlate, szTargetAnydoc );
ACTION_TARGET(CTestPlate, szTargetViewSite);

ACTION_ARG_LIST(CTestPlate)
	ARG_IMAGE( _T("Image") )
	ARG_OBJECT( _T("Objects") )
END_ACTION_ARG_LIST();

CTestPlate::CTestPlate(void)
{
}

CTestPlate::~CTestPlate(void)
{
}

IUnknown* CTestPlate::GetContextObject(CString cName, CString cType)
{ //Возвращает с AddRef

	IDataContext3Ptr ptrC = m_punkTarget;
	_bstr_t	bstrType = cType;

	if( cName.IsEmpty() )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	long lPos = 0;
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

bool CTestPlate::Invoke()
//bool CTestPlate::InvokeFilter()
{
	if (!m_punkTarget) return false;

	CString strImage=GetArgumentString( _T( "Image" ) );
	CString strObjectList=GetArgumentString( _T( "ObjectList" ) );

	IUnknown *punkImage = GetContextObject(strImage, szTypeImage);
	IImage4Ptr sptrImage(punkImage);
	if(punkImage) punkImage->Release();

	IUnknown *punkObjects = GetContextObject(strObjectList, szTypeObjectList);
	INamedDataObject2Ptr sptrObjects(punkObjects);
	if(punkObjects) punkObjects->Release();

	//IImage4Ptr sptrImage(GetDataArgument("Image"));
	//INamedDataObject2Ptr sptrObjects(GetDataArgument("Objects"));

	if(sptrImage==0 || sptrObjects==0) return false;

	int cx=0, cy=0;
	sptrImage->GetSize( &cx, &cy );
	CPoint ptOffset(0,0);
	sptrImage->GetOffset(&ptOffset);
	CRect rect(ptOffset, CSize(cx,cy));
	int nPanes = GetImagePaneCount(sptrImage);

	long nObjects=0;
	sptrObjects->GetChildsCount(&nObjects);

	// создадим карту счетчиков - для подсчета числа пересечений
	smart_alloc(pMaskCounters, byte, cx*cy);
	if(cx*cy) ZeroMemory(pMaskCounters, cx*cy*sizeof(byte)); // обнулим счетчики

	long nMaxOversectArea=0;
	long nTotalOversectArea=0;
	long nOversectCount=0;

	// пробежимся по объектам
	long pos=0; sptrObjects->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr ptrObj;
		sptrObjects->GetNextChild(&pos, &ptrObj);
		IMeasureObjectPtr sptrObj(ptrObj);
		if(sptrObj==0) continue;

		IUnknownPtr ptrImg;
		sptrObj->GetImage(&ptrImg);
		IImage4Ptr sptrImg(ptrImg);
		if(sptrImg==0) continue;

		int cx1=0, cy1=0; // для мелкого Img
		sptrImg->GetSize( &cx1, &cy1 );
		_point ptOffset1(0,0); // для мелкого Img
		sptrImg->GetOffset(&ptOffset1);
		CRect rect1(ptOffset1, CSize(cx1,cy1));

		CRect rcFill=NORECT;
		if( !rcFill.IntersectRect( rect, rect1 ) ) continue; // если rect объекта не пересекается с rect Image - ничего не делать

		int nOversectArea=0; // площадь пересечений для данной хромосомы

		for( int yy=rcFill.top; yy<rcFill.bottom; yy++ )
		{
			int y = yy - ptOffset.y;
			int y1 = yy - ptOffset1.y;
			byte *pmask = 0;
			sptrImg->GetRowMask( y1, &pmask );
			byte *pCounter = pMaskCounters + y*cx;

			for( int xx=rcFill.left; xx<rcFill.right; xx++ )
			{
				int x = xx - ptOffset.x;
				int x1 = xx - ptOffset1.x;

				int m=pmask[x1];
				if( m & 128 )
				{
					int counter = pCounter[x];
					if(counter) nOversectArea++; 
					if(counter!=255) counter++;
					pCounter[x] = counter;
				}
			}
		}
		nTotalOversectArea += nOversectArea;
		nMaxOversectArea = max(nMaxOversectArea,nOversectArea);
		if(nOversectArea) 	nOversectCount++;
	}

	// запишем результаты в shell.data
	::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", "TotalOversectArea", nTotalOversectArea);
	::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", "MaxOversectArea", nMaxOversectArea);
	::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", "OversectCount", nOversectCount);

	//------------------------------------------------------------------------------
	// Поиск центра масс маски
	double xSum=0, ySum=0;
	long nArea=0, nAreaOutside=0;
	byte *pCounter = pMaskCounters;
	for( int y=0; y<cy; y++ )
	{
		for( int x=0; x<cx; x++, pCounter++ )
		{
			if(*pCounter)
			{
				nArea++;
				xSum+=x;
				ySum+=y;
			}
			else
			{
				nAreaOutside++;
			}
		}
	}
	xSum /= max(nArea,1);
	ySum /= max(nArea,1);

	for(long nPane=0; nPane<nPanes; nPane++)
	{
		// Поиск центра масс паны и прочего
		double xSumPane=0, ySumPane=0, colorSum=0, colorSum2=0;
		double colorSumOutside=0;
		byte *pCounter = pMaskCounters;
		for( int y=0; y<cy; y++ )
		{
			color *pColor = 0;
			sptrImage->GetRow( y, nPane, &pColor );
			for( int x=0; x<cx; x++, pCounter++, pColor++ )
			{
				double c = *pColor;
				if(*pCounter)
				{
					xSumPane += x*c;
					ySumPane += y*c;
					colorSum += c;
					colorSum2 += c*c;
				}
				else
				{
					colorSumOutside += c;
				}
			}
		}
		xSumPane /= max(colorSum,1);
		ySumPane /= max(colorSum,1);
		double fDist = ::_hypot(xSumPane-xSum, ySumPane-ySum);

		colorSum /= max(nArea,1);
		colorSum2 /= max(nArea,1);
		colorSumOutside /= max(nAreaOutside,1);
		double fDynamicRange = 1000;
		if(colorSum<colorSumOutside*1000) fDynamicRange = colorSum/colorSumOutside;

		colorSum2 -= colorSum*colorSum; // переходим от среднего квадрата к квадрату СКО
		if(colorSum2<0) colorSum2=0; // просто на всякий случай, такого быть не может
		double fInsideObjectsDispersion = sqrt(colorSum2) / max(colorSum,1);

		// запишем результаты в shell.data
		CString strPaneName;
		strPaneName.Format("Pane%u_", unsigned(nPane));
		::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", strPaneName+"Offset", fDist );
		::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", strPaneName+"DynamicRange", fDynamicRange );
		::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", strPaneName+"InsideObjectsDispersion", fInsideObjectsDispersion );

        // снова пробежимся по объектам - для вычисления средних значений цвета каждого объекта
		double fObjectsSum=0, fObjectsSum2=0; // суммы средних яркостей объектов и их квадратов
		long nObjectsCount=0;
		long pos=0; sptrObjects->GetFirstChildPosition(&pos);
		while( pos ) //по всем объектам
		{
			IUnknownPtr ptrObj;
			sptrObjects->GetNextChild(&pos, &ptrObj);
			IMeasureObjectPtr sptrObj(ptrObj);
			if(sptrObj==0) continue;

			IUnknownPtr ptrImg;
			sptrObj->GetImage(&ptrImg);
			IImage4Ptr sptrImg(ptrImg);
			if(sptrImg==0) continue;

			int cx1=0, cy1=0; // для мелкого Img
			sptrImg->GetSize( &cx1, &cy1 );
			_point ptOffset1(0,0); // для мелкого Img
			sptrImg->GetOffset(&ptOffset1);
			CRect rect1(ptOffset1, CSize(cx1,cy1));

			CRect rcFill=NORECT;
			if( !rcFill.IntersectRect( rect, rect1 ) ) continue; // если rect объекта не пересекается с rect Image - ничего не делать

			long nObjectArea=0;
			double fObjectColorSum=0;

			for( int yy=rcFill.top; yy<rcFill.bottom; yy++ )
			{
				int y = yy - ptOffset.y;
				int y1 = yy - ptOffset1.y;
				byte *pmask = 0;
				sptrImg->GetRowMask( y1, &pmask );
				color *pColor = 0;
				sptrImage->GetRow( y, nPane, &pColor );

				for( int xx=rcFill.left; xx<rcFill.right; xx++ )
				{
					int x = xx - ptOffset.x;
					int x1 = xx - ptOffset1.x;

					int m=pmask[x1];
					if( m & 128 )
					{
						nObjectArea ++;
						fObjectColorSum += pColor[x];
					}
				}
			}
			if(nObjectArea)
			{
				fObjectColorSum /= nObjectArea;
				nObjectsCount++;
				fObjectsSum += fObjectColorSum; // возможно, стоит сюда приписать *nObjectArea
				fObjectsSum2 += fObjectColorSum*fObjectColorSum; // возможно, стоит сюда приписать *nObjectArea
			}
		}
		fObjectsSum /= max(nObjectsCount,1);
		fObjectsSum2 /= max(nObjectsCount,1);
		fObjectsSum2 -= fObjectsSum*fObjectsSum;
		if(fObjectsSum2<0) fObjectsSum2=0; // просто на всякий случай, такого быть не может
		double fObjectsDispersion = sqrt(fObjectsSum2) / max(fObjectsSum,1);
		::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", strPaneName+"ObjectsDispersion", fObjectsDispersion );

		// Теперь посчитаем granularity. Для этого нам прежде всего нужно создать размытое изображение
		double fGranSum2=0;
		smart_alloc(p_res, color, cx*cy); // буфер для размытого изображения
		{
			 // mask size
			int m=::GetValueInt(::GetAppUnknown(), CGH_ROOT "\\TestPlate", "GranularityMaskSize", 5 );
			int mm=m*m, m1=(m-1)/2, m2=m-m1;
			smart_alloc(buf, int, cx+m);
			ZeroMemory(buf, (cx+m)*sizeof(int));

			for (int iy=0; iy<m; iy++)
			{
				int *b=buf+m1;
				color *pColor = 0;
				sptrImage->GetRow( max(0,iy-m1), nPane, &pColor );
				for(int ix=0; ix<cx; ix++, b++)
					(*b) += pColor[ix];
			}
			for(int ix=0; ix<m1; ix++) buf[ix]=buf[m1];
			for(int ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];

			color *p_r=p_res;
			byte *pCounter = pMaskCounters;
			for(int iy=0; iy<cy; iy++)
			{
				color *pColor = 0;
				sptrImage->GetRow( iy, nPane, &pColor );
				int sum=0;
				for (int ix=0; ix<m; ix++) sum+=buf[ix];
				for (ix=0; ix<cx; ix++)
				{
					*(p_r++) = abs( pColor[ix] - sum/mm );
					int counter = *(pCounter++);
					if(counter)
					{
						double dc = pColor[ix] - sum/mm;
						fGranSum2 += dc*dc;
					}
					int x1=ix-m1, x2=x1+m;
					if(x1<0) x1=0;
					if(x2>=cx) x2=cx-1;
					sum -= buf[x1+m1];
					sum += buf[x2+m1];
				}

				int y1=iy-m1, y2=y1+m;
				if(y1<0) y1=0;
				if(y2>=cy) y2=cy-1;
				color *p_a1 = 0, *p_a2 = 0;
				sptrImage->GetRow( y1, nPane, &p_a1 );
				sptrImage->GetRow( y2, nPane, &p_a2 );
				int *b=buf+m1;
				for(ix=0; ix<cx; ix++, b++)
				{
					*b -= *p_a1++;
					*b += *p_a2++;
				}
				for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
				for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];
			}
		}
		fGranSum2 /= max(nArea,1);
		double fGranularity = sqrt(fGranSum2) / max(colorSum, 1);
		::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestPlate\\Result", strPaneName+"Granularity", fGranularity );
	}

	// прочитаtv из shell.data, на сколько пикселов объекты могут пересечься - и не быть заселекченными
	int nMaxAcceptableIntersection=::GetValueInt(::GetAppUnknown(), CGH_ROOT "\\TestPlate", "MaxAcceptableIntersection", 0 );

	// пометим те объекты, которые сильно пересекаются с другими
	IViewPtr sptrView(m_punkTarget);
	if(sptrView!=0)
	{
		IUnknownPtr ptrMultiFrame;
		sptrView->GetMultiFrame(&ptrMultiFrame, false);
		IMultiSelectionPtr sptrSelection(ptrMultiFrame);
		if(sptrSelection!=0)
		{
			sptrSelection->EmptyFrame();
			sptrObjects->SetActiveChild( 0 );
			// пробежимся по объектам
			long pos=0; sptrObjects->GetFirstChildPosition(&pos);
			while( pos ) //по всем объектам
			{
				IUnknownPtr ptrObj;
				sptrObjects->GetNextChild(&pos, &ptrObj);
				IMeasureObjectPtr sptrObj(ptrObj);
				if(sptrObj==0) continue;

				IUnknownPtr ptrImg;
				sptrObj->GetImage(&ptrImg);
				IImage4Ptr sptrImg(ptrImg);
				if(sptrImg==0) continue;

				int cx1=0, cy1=0; // для мелкого Img
				sptrImg->GetSize( &cx1, &cy1 );
				_point ptOffset1(0,0); // для мелкого Img
				sptrImg->GetOffset(&ptOffset1);
				CRect rect1(ptOffset1, CSize(cx1,cy1));

				CRect rcFill=NORECT;
				if( !rcFill.IntersectRect( rect, rect1 ) ) continue; // если rect объекта не пересекается с rect Image - ничего не делать

				int nOversectArea=0; // площадь пересечений для данной хромосомы

				// пройдемся по ректу объекта - посчитаем, сколько в нем точек пересечения
				for( int yy=rcFill.top; yy<rcFill.bottom; yy++ )
				{
					int y = yy - ptOffset.y;
					int y1 = yy - ptOffset1.y;
					byte *pmask = 0;
					sptrImg->GetRowMask( y1, &pmask );
					byte *pCounter = pMaskCounters + y*cx;

					for( int xx=rcFill.left; xx<rcFill.right; xx++ )
					{
						int x = xx - ptOffset.x;
						int x1 = xx - ptOffset1.x;

						int m=pmask[x1];
						if( m & 128 )
						{
							int counter = pCounter[x];
							if(counter>1) nOversectArea++; 
						}
					}
				}
                if(nOversectArea > nMaxAcceptableIntersection)
				{
					sptrSelection->SelectObject(sptrObj, true);
					//sptrSelection->SelectObject(sptrImg, true);
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	//StartNotification(1000);
	//Notify(999);
	//FinishNotification();

	return true;
}
