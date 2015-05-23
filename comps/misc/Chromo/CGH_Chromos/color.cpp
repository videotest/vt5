#include "stdafx.h"
#include "color.h"
#include <math.h>
#include "resource.h"
#include "ccmacro.h"
#include "alloc.h"
#include "PropBag.h"


int nInterpolationMethod=-1; //метод интерполяции при увеличении
// 0 - по ближайшим соседям, 1 - билинейная
// -1 - прочитать значение из shell.data
int nMaskInterpolationMethod=-1; //метод интерполяции масок при увеличении
// 0 - квадратами, 1 - под углами 90 и 45 градусов


#define NotUsed 0 
/////


#define ScaleDoubleToColor(clr) ((color)max( 0, min( 65535,(((clr)+0.5)*65535) ) ))
#define ScaleDoubleToByte(clr)  ((byte)max(0, min(255,(((clr)+0.5)*255))))
//#define ScaleDoubleToColor(clr) ((color) (((clr)+0.5)*65535)) 
//#define ScaleDoubleToByte(clr)  ((byte)(((clr)+0.5)*255))

#define ScaleColorToDouble(clr) ((double)(((clr)/65535.0)-0.5))
#define ScaleByteToDouble(clr) ((double)(((clr)/255.0)-0.5))


static IUnknown	*GetActiveFrom(IUnknown *source, const char *kind)
{
	if( CheckInterface( source, IID_IDocument ) )
		return ::GetActiveObjectFromDocument( source, kind );
	else
		return ::GetActiveObjectFromContext( source, kind );
}

//---------------------

IMPLEMENT_DYNCREATE(CColorConvertorCGH, CCmdTargetEx);


// {4B1DFDE4-A014-4f32-A3DD-CBA630672B8B}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorCGH, "CGH_Chromos.ColorConvertorCGH",
0x4b1dfde4, 0xa014, 0x4f32, 0xa3, 0xdd, 0xcb, 0xa6, 0x30, 0x67, 0x2b, 0x8b);


/////////////////////////////////////////////////////////////////
//color convertor CGH
/////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CColorConvertorCGH, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorCGH, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorCGH, IID_IProvideHistColors, Hist)
	INTERFACE_PART(CColorConvertorCGH, IID_IEventListener, EvList)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorCGH, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorCGH, Hist)

HRESULT CColorConvertorCGH::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorCGH, Hist);

	if( nPaneNo >= 4 )
		return S_FALSE;

	if(nPaneNo==CGH_RATIO)
	{
		*pcrCurve=0x808080;
		for( int i = 0; i < 256; i++, pcrArray++ )
			*pcrArray = RGB( pThis->m_pal_r[i], pThis->m_pal_g[i], pThis->m_pal_b[i] );
	}
	else
	{
		*pcrCurve=pThis->m_PaneColor[nPaneNo];
		int r=GetRValue(pThis->m_PaneColor[nPaneNo]);
		int g=GetGValue(pThis->m_PaneColor[nPaneNo]);
		int b=GetBValue(pThis->m_PaneColor[nPaneNo]);
		for( int i = 0; i < 256; i++, pcrArray++ )
			*pcrArray = RGB( r*i/256, g*i/256, b*i/256 );
	}

	return S_OK;
}

boolean CalcRatioPane(IImage3Ptr img, INamedDataObject2Ptr obj,
					  long nProcessImage=2, //0 - не трогать, 1 - обсчитать, 2 - забить 0
					  long nProcessObjects=1,
					  long nProcessObjectsRects=2)
{
	if(img==0) return false;
	if(obj==0) return false; //??

	_bstr_t	bstrCC;
	{
		IUnknown *punkCC = 0;
		img->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}
	if(bstrCC!=_bstr_t("CGH")) return(false);

	int cx,cy;
	img->GetSize(&cx,&cy);
	POINT ptOffset;
	img->GetOffset(&ptOffset);

	smart_alloc(bin,byte,cx*cy); smart_bin.zero();

	//построение маски для определения фона
	POSITION pos; obj->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		obj->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		if(object==0) continue;
		ICalcObjectPtr calcObj(sptr);
		if(calcObj==0) continue;

		IImage3Ptr img_obj;
		//получим image объекта
		IUnknownPtr sptr2;
		object->GetImage(&sptr2);
		img_obj=IImage3Ptr(sptr2);
		if(img_obj==0) continue;

		POINT offset_obj;
		img_obj->GetOffset(&offset_obj);
		int cx_obj,cy_obj;
		img_obj->GetSize(&cx_obj,&cy_obj);

		int x0=offset_obj.x-ptOffset.x, x1=x0+cx_obj;
		int y0=offset_obj.y-ptOffset.y, y1=y0+cy_obj;
		x0=max(x0,0); y0=max(y0,0);
		x1=min(x1,cx); y1=min(y1,cy);

		for(int y=y0; y<y1; y++)
		{
			byte *m=bin+cx*y;
			byte *m_obj;
			img_obj->GetRowMask(y-offset_obj.y, &m_obj);
			m_obj=m_obj+x0-offset_obj.x;
			for(int x=x0; x<x1; x++)
			{
				if(*m_obj>=128) m[x]=255;
				m_obj++;
			}
		}
	}

	//гистограмма фона
	smart_alloc(hist1_bg, int, 256); smart_hist1_bg.zero();
	smart_alloc(hist2_bg, int, 256); smart_hist2_bg.zero();
	for(int y=0; y<cy; y++)
	{
		byte *m=bin+cx*y;
		color *p1,*p2;
		img->GetRow(y,1,&p1);
		img->GetRow(y,2,&p2);
		for(int x=0; x<cx; x++)
		{
			if(m[x]==0)
			{
				int c1=p1[x], c2=p2[x];
				int d=c2-c1;
				if(d<300 && d>-300) c2=c1; // чтобы не замечать погрешность в младшем бите
				hist1_bg[c1>>8]++;
				hist2_bg[c2>>8]++;
			}			
		}
	}

	//гистограмма объектов
	smart_alloc(hist1_fg, int, 256); smart_hist1_fg.zero();
	smart_alloc(hist2_fg, int, 256); smart_hist2_fg.zero();

	obj->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		obj->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		if(object==0) continue;
		ICalcObjectPtr calcObj(sptr);
		if(calcObj==0) continue;

		IImage3Ptr img_obj;
		//получим image объекта
		IUnknownPtr sptr2;
		object->GetImage(&sptr2);
		img_obj=IImage3Ptr(sptr2);
		if(img_obj==0) continue;

		POINT offset_obj;
		img_obj->GetOffset(&offset_obj);
		int cx_obj,cy_obj;
		img_obj->GetSize(&cx_obj,&cy_obj);

		_bstr_t	bstrCC;
		{
			IUnknown *punkCC = 0;
			img_obj->GetColorConvertor( &punkCC );
			if(punkCC==0) continue; //no color convertor - как такое может быть?
			IColorConvertor2Ptr	ptrCC( punkCC );
			punkCC->Release();

			BSTR	bstr;
			ptrCC->GetCnvName( &bstr );
			bstrCC = bstr;
			::SysFreeString( bstr ); 
		}
		if(bstrCC!=_bstr_t("CGH")) continue; //не CGH - скипаем нафиг

		for(int y=0; y<cy_obj; y++)
		{
			byte *m_obj;
			img_obj->GetRowMask(y, &m_obj);
			color *p1,*p2;
			img_obj->GetRow(y,1,&p1);
			img_obj->GetRow(y,2,&p2);
			for(int x=0; x<cx_obj; x++)
			{
				if(m_obj[x]>=128) 
				{
					int c1=p1[x], c2=p2[x];
					int d=c2-c1;
					if(d<300 && d>-300) c2=c1; // чтобы не замечать погрешность в младшем бите
					hist1_fg[c1>>8]++;
					hist2_fg[c2>>8]++;
				}
			}
		}
	}
	

	//поиск медиан
	color med1_bg, med2_bg, med1_fg, med2_fg;
	{
		int s=0;
		for(int i=0; i<256; i++) s+=hist1_bg[i]; //сумма hist2 = сумма hist1
		int s1=s/2, s2=s/2;
		for(int i1=0; s1>0 && i1<256; i1++) s1-=hist1_bg[i1];
		for(int i2=0; s2>0 && i2<256; i2++) s2-=hist2_bg[i2];
		med1_bg = min(max(i1*256-128,0),65535);
		med2_bg = min(max(i2*256-128,0),65535);

		med1_bg = ::GetValueInt(img, KARYO_ACTIONROOT, CGH_PANE1 "\\BackGroundMedian", med1_bg);
		med2_bg = ::GetValueInt(img, KARYO_ACTIONROOT, CGH_PANE2 "\\BackGroundMedian", med2_bg);
	}
	{
		int s=0;
		for(int i=0; i<256; i++) s+=hist1_fg[i]; //сумма hist2 = сумма hist1
		int s1=s/2, s2=s/2;
		for(int i1=0; s1>0 && i1<256; i1++) s1-=hist1_fg[i1];
		for(int i2=0; s2>0 && i2<256; i2++) s2-=hist2_fg[i2];
		med1_fg = min(max(i1*256,0),65535);
		med2_fg = min(max(i2*256,0),65535);
	}
	
	if(nProcessImage)
	{	//обсчет image или заполнение 0
		for(y=0; y<cy; y++)
		{
			color *p1,*p2,*p3;
			img->GetRow(y,1,&p1);
			img->GetRow(y,2,&p2);
			img->GetRow(y,3,&p3);
			for(int x=0; x<cx; x++)
			{ //пока double - потом сделаю int
				if(nProcessImage==1)
				{
					int cc1=p1[x], cc2=p2[x];
					int d=cc2-cc1;
					if(d<300 && d>-300) cc2=cc1; // чтобы не замечать погрешность в младшем бите
					
					double c1=(cc1-med1_bg)/double(med1_fg-med1_bg);
					double c2=(cc2-med2_bg)/double(med2_fg-med2_bg);
					c1 +=0.05;
					c2 +=0.05;
					double c;
					if(c1<=c2) c=c1/c2;
					else c=2-c2/c1;
					if(c<0) c=0;
					if(c>2) c=2;
					p3[x]=int(c*(127*256)+256); //0 в старшем байте никогда не получаем
				}
				else
				{
					p3[x]=0;
				}
			}
		}
	}

	if(nProcessObjects || nProcessObjectsRects)
	{	//заполнение objects.pane_ratio
		obj->GetFirstChildPosition(&pos);
		while( pos ) //по всем объектам
		{
			IUnknownPtr sptr;
			obj->GetNextChild(&pos, &sptr);
			IMeasureObjectPtr object(sptr);
			if(object==0) continue;
			ICalcObjectPtr calcObj(sptr);
			if(calcObj==0) continue;
			
			IImage3Ptr img_obj;
			//получим image объекта
			IUnknownPtr sptr2;
			object->GetImage(&sptr2);
			img_obj=IImage3Ptr(sptr2);
			if(img_obj==0) continue;
			
			POINT offset_obj;
			img_obj->GetOffset(&offset_obj);
			int cx_obj,cy_obj;
			img_obj->GetSize(&cx_obj,&cy_obj);
			
			_bstr_t	bstrCC;
			{
				IUnknown *punkCC = 0;
				img_obj->GetColorConvertor( &punkCC );
				IColorConvertor2Ptr	ptrCC( punkCC );
				punkCC->Release();
				
				BSTR	bstr;
				ptrCC->GetCnvName( &bstr );
				bstrCC = bstr;
				::SysFreeString( bstr ); 
			}
			if(bstrCC!=_bstr_t("CGH")) continue; //не CGH - скипаем нафиг
			
			for(int y=0; y<cy_obj; y++)
			{
				byte *m_obj;
				img_obj->GetRowMask(y, &m_obj);
				color *p1,*p2,*p3;
				img_obj->GetRow(y,1,&p1);
				img_obj->GetRow(y,2,&p2);
				img_obj->GetRow(y,3,&p3);
				for(int x=0; x<cx_obj; x++)
				{
					int nCountMethod = (m_obj[x]>=128) ? nProcessObjects : nProcessObjectsRects;
					//
					if(nCountMethod==0) ;
					else if(nCountMethod==1)
					{ //пока double - потом сделаю int
						int cc1=p1[x], cc2=p2[x];
						int d=cc2-cc1;
						if(d<300 && d>-300) cc2=cc1; // чтобы не замечать погрешность в младшем бите
						
						double c1=(cc1-med1_bg)/double(med1_fg-med1_bg);
						double c2=(cc2-med2_bg)/double(med2_fg-med2_bg);
						c1 +=0.05;
						c2 +=0.05;
						double c;
						if(c1<=c2) c=c1/c2;
						else c=2-c2/c1;
						if(c<0) c=0;
						if(c>2) c=2;
						p3[x]=int(c*(127*256)+256); //0 в старшем байте никогда не получаем
					}
					else // if(nCountMethod==2)
					{
						p3[x]=0;
					}
				}
			}
		}
	}


	return(true);
}

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionCalcRatioPane, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {F4608860-DC80-11d6-96DA-0000004442FE}
GUARD_IMPLEMENT_OLECREATE(CActionCalcRatioPane, "CGH.CalcRatioPaneD", 
0xf4608860, 0xdc80, 0x11d6, 0x96, 0xda, 0x0, 0x0, 0x0, 0x44, 0x42, 0xfe);

//[ag]3. guidinfo debug

// {F4608861-DC80-11d6-96DA-0000004442FE}
static const GUID guidActionCalcRatioPane = 
{ 0xf4608861, 0xdc80, 0x11d6, { 0x96, 0xda, 0x0, 0x0, 0x0, 0x44, 0x42, 0xfe } };

#else
//[ag]4. olecreate release

// {F4608862-DC80-11d6-96DA-0000004442FE}
GUARD_IMPLEMENT_OLECREATE(CActionCalcRatioPane, "CGH.CalcRatioPane", 
0xf4608862, 0xdc80, 0x11d6, 0x96, 0xda, 0x0, 0x0, 0x0, 0x44, 0x42, 0xfe);

//[ag]5. guidinfo release

// {F4608863-DC80-11d6-96DA-0000004442FE}
static const GUID guidActionCalcRatioPane = 
{ 0xf4608863, 0xdc80, 0x11d6, { 0x96, 0xda, 0x0, 0x0, 0x0, 0x44, 0x42, 0xfe } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionCalcRatioPane,IDS_ACTION_CALCRATIOPANE, -1, -1, guidActionCalcRatioPane);

//[ag]7. targets

ACTION_TARGET(CActionCalcRatioPane,szTargetViewSite);

//[ag]8. arguments

ACTION_ARG_LIST(CActionCalcRatioPane)
	ARG_INT(_T("ProcessImage"), 2)
	ARG_INT(_T("ProcessObjects"), 1)
	ARG_INT(_T("ProcessObjectsRects"), 0)
	ARG_STRING("Image", 0)
	ARG_STRING("ObjectList", 0)
END_ACTION_ARG_LIST();

//[ag]9. implementation

CActionCalcRatioPane::CActionCalcRatioPane()
{
	m_undo_Image=0;
	m_undo_List=0;
}

CActionCalcRatioPane::~CActionCalcRatioPane()
{
}

bool CActionCalcRatioPane::IsAvaible()
{
	CString strImage=GetArgumentString( _T( "Image" ) );
	CString strObjectList=GetArgumentString( _T( "ObjectList" ) );

	//IUnknown *punk2 = GetActiveFrom(m_punkTarget, szTypeImage);
	IUnknown *punk2 = GetContextObject(strImage, szTypeImage);
	if (!punk2) return false;
	IImage3Ptr ptrImage(punk2);
	punk2->Release();
	if (ptrImage==0) return false;

	//IUnknown *punk = GetActiveFrom(m_punkTarget, szTypeObjectList);
	IUnknown *punk = GetContextObject(strObjectList, szTypeObjectList);
	if (!punk) return false;
	INamedDataObject2Ptr ptrList(punk);
	punk->Release();
	if (ptrList==0) return false;

	IImage3Ptr ptrImageRoot=0;
	{
		IUnknown *punk;
		INamedDataObject2Ptr ptrNDO(ptrImage);
		if(ptrNDO!=0)
		{
			ptrNDO->GetParent(&punk);
			ptrImageRoot=IImage3Ptr(punk);
			if(punk!=0) punk->Release();
		}
	}
	if(ptrImageRoot==0) ptrImageRoot=ptrImage;

	int state0=0, state1=0, state2=0, state3=0;
	if (ptrImageRoot!=0)
	{
		int bit=1;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE0, 0)) state0|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE1, 0)) state1|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE2, 0)) state2|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE3, 0)) state3|=bit;
	}

	if (ptrList!=0)
	{
		int bit=2;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE0, 0)) state0|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE1, 0)) state1|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE2, 0)) state2|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE3, 0)) state3|=bit;
	}

	return(state1!=0 && state2!=0);
}


IUnknown *CActionCalcRatioPane::GetContextObject(CString cName, CString cType)
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

bool CActionCalcRatioPane::DoUndo()
{
	if(m_undo_Image)
	{
		long tmp = ::GetValueInt(m_undo_Image, KARYO_ACTIONROOT, CGH_PANE3, 0);
		::SetValue(m_undo_Image, KARYO_ACTIONROOT, CGH_PANE3, long(m_undo_bImagePane3));
		m_undo_bImagePane3=tmp;
	}
	if(m_undo_List)
	{
		long tmp = ::GetValueInt(m_undo_List, KARYO_ACTIONROOT, CGH_PANE3, 0);
		::SetValue(m_undo_List, KARYO_ACTIONROOT, CGH_PANE3, long(m_undo_bListPane3));
		m_undo_bListPane3=tmp;
	}

	TPOS	posImage = m_undoImages.tail();
	TPOS	posData = m_undoData.tail();
	while( posImage && posData ) 
	{
		IImage3Ptr image=m_undoImages.get(posImage);
		color *data=m_undoData.get(posData);
		posImage=m_undoImages.prev(posImage);
		posData=m_undoData.prev(posData);

		if(image==0) continue; //мало ли что...

		int cx,cy;
		image->GetSize(&cx,&cy);
		if(cx>0 && cy>0)
		{
			smart_alloc(tmp,color,cx);
			for(int y=0; y<cy; y++)
			{
				color *p3;
				image->GetRow(y,3,&p3);
				MoveMemory(tmp, p3, cx*sizeof(color));
				MoveMemory(p3, data+cx*y, cx*sizeof(color));
				MoveMemory(data+cx*y, tmp, cx*sizeof(color));
			}
		}
	}

	{
		sptrIDocument	sptrD( m_punkTarget );
		if(sptrD!=0)
		{
			//просигналим, чтоб обновить вьюху
			INamedDataObject2Ptr NDO(m_undo_List);
			if(NDO!=0)
			{
				IUnknown *punk = NULL;
				HRESULT hr = NDO->GetData(&punk);
				if (SUCCEEDED(hr) && punk)
				{
					long l = cncReset;
					::FireEvent(punk, szEventChangeObjectList, m_undo_List, NULL, &l, 0);
					punk->Release();
				}
			}
			if(m_undo_Image) ::UpdateDataObject( sptrD, m_undo_Image );
			if(m_undo_List) ::UpdateDataObject( sptrD, m_undo_List );
		}
	}

	return true;
}

bool CActionCalcRatioPane::DoRedo()
{
	if(m_undo_Image)
	{
		long tmp = ::GetValueInt(m_undo_Image, KARYO_ACTIONROOT, CGH_PANE3, 0);
		::SetValue(m_undo_Image, KARYO_ACTIONROOT, CGH_PANE3, long(m_undo_bImagePane3));
		m_undo_bImagePane3=tmp;
	}
	if(m_undo_List)
	{
		long tmp = ::GetValueInt(m_undo_List, KARYO_ACTIONROOT, CGH_PANE3, 0);
		::SetValue(m_undo_List, KARYO_ACTIONROOT, CGH_PANE3, long(m_undo_bListPane3));
		m_undo_bListPane3=tmp;
	}

	TPOS	posImage = m_undoImages.head();
	TPOS	posData = m_undoData.head();
	while( posImage && posData ) 
	{
		IImage3Ptr image=m_undoImages.get(posImage);
		color *data=m_undoData.get(posData);
		posImage=m_undoImages.next(posImage);
		posData=m_undoData.next(posData);

		if(image==0) continue; //мало ли что...

		int cx,cy;
		image->GetSize(&cx,&cy);
		if(cx>0 && cy>0)
		{
			smart_alloc(tmp,color,cx);
			for(int y=0; y<cy; y++)
			{
				color *p3;
				image->GetRow(y,3,&p3);
				MoveMemory(tmp, p3, cx*sizeof(color));
				MoveMemory(p3, data+cx*y, cx*sizeof(color));
				MoveMemory(data+cx*y, tmp, cx*sizeof(color));
			}
		}
	}

	{
		sptrIDocument	sptrD( m_punkTarget );
		if(sptrD!=0)
		{
			//просигналим, чтоб обновить вьюху
			INamedDataObject2Ptr NDO(m_undo_List);
			if(NDO!=0)
			{
				IUnknown *punk = NULL;
				HRESULT hr = NDO->GetData(&punk);
				if (SUCCEEDED(hr) && punk)
				{
					long l = cncReset;
					::FireEvent(punk, szEventChangeObjectList, m_undo_List, NULL, &l, 0);
					punk->Release();
				}
			}
			if(m_undo_Image) ::UpdateDataObject( sptrD, m_undo_Image );
			if(m_undo_List) ::UpdateDataObject( sptrD, m_undo_List );
		}
	}
	
	return true;
}


bool CActionCalcRatioPane::Invoke()
{
	if (!m_punkTarget)
		return false;

	CString strImage=GetArgumentString( _T( "Image" ) );
	CString strObjectList=GetArgumentString( _T( "ObjectList" ) );

	//IUnknown *punk2 = GetActiveFrom(m_punkTarget, szTypeImage);
	IUnknown *punk2 = GetContextObject(strImage, szTypeImage);
	if (!punk2) return false;
	IImage3Ptr ptrImage(punk2);
	punk2->Release();
	if (ptrImage==0) return false;

	//IUnknown *punk = GetActiveFrom(m_punkTarget, szTypeObjectList);
	IUnknown *punk = GetContextObject(strObjectList, szTypeObjectList);
	if (!punk) return false;
	INamedDataObject2Ptr ptrList(punk);
	punk->Release();
	if (ptrList==0) return false;

	long nProcessImage = GetArgumentInt("ProcessImage");
	long nProcessObjects = GetArgumentInt("ProcessObjects");
	long nProcessObjectsRects = GetArgumentInt("ProcessObjectsRects");

	m_undo_bImagePane3 = ::GetValueInt(ptrImage, KARYO_ACTIONROOT, CGH_PANE3, 0);
	m_undo_bListPane3 = ::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE3, 0);
	if(nProcessImage) m_undo_Image = ptrImage;
	if(nProcessObjects || nProcessObjectsRects) m_undo_List = ptrList;

	//для undo - старые данные с image
	if(m_undo_Image!=0)
	{
		int cx,cy;
		ptrImage->GetSize(&cx,&cy);
		color* data = (color*) malloc(cx*cy*sizeof(color));

		m_undoImages.add_tail(ptrImage);
		m_undoData.add_tail(data);

		if(cx>0 && cy>0)
		{
			for(int y=0; y<cy; y++)
			{
				color *p3;
				ptrImage->GetRow(y,3,&p3);
				MoveMemory(data+cx*y, p3, cx*sizeof(color));
			}
		}
	}

	//для undo - старые данные с list
	if(m_undo_List!=0)
	{
		POSITION pos; ptrList->GetFirstChildPosition(&pos);
		while( pos ) //по всем объектам
		{
			IUnknownPtr sptr;
			ptrList->GetNextChild(&pos, &sptr);
			IMeasureObjectPtr object(sptr);
			if(object==0) continue;
			ICalcObjectPtr calcObj(sptr);
			if(calcObj==0) continue;
			
			IImage3Ptr img_obj;
			//получим image объекта
			IUnknownPtr sptr2;
			object->GetImage(&sptr2);
			img_obj=IImage3Ptr(sptr2);
			if(img_obj==0) continue;
			
			int cx,cy;
			img_obj->GetSize(&cx,&cy);
			color* data = (color*) malloc(cx*cy*sizeof(color));
			
			m_undoImages.add_tail(img_obj);
			m_undoData.add_tail(data);
			
			if(cx>0 && cy>0)
			{
				for(int y=0; y<cy; y++)
				{
					color *p3;
					img_obj->GetRow(y,3,&p3);
					MoveMemory(data+cx*y, p3, cx*sizeof(color));
				}
			}
		}
	}

	int zzz=1;
	
	CalcRatioPane(ptrImage, ptrList,
		nProcessImage, nProcessObjects, nProcessObjectsRects);

	::SetValue(ptrImage, KARYO_ACTIONROOT, CGH_PANE3, long(1));
	::SetValue(ptrList, KARYO_ACTIONROOT, CGH_PANE3, long(1));


	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		if(sptrV!=0)
		{
			sptrV->GetDocument( &punkDoc );
			sptrIDocument	sptrD( punkDoc );
			if(sptrD!=0) m_lTargetKey = ::GetObjectKey( sptrD );		
		}
		if(punkDoc) punkDoc->Release();		
		::UpdateDataObject( punkDoc, ptrImage );
		::UpdateDataObject( punkDoc, ptrList );
		//просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(ptrList);
		if(NDO!=0)
		{
			IUnknown *punk = NULL;
			HRESULT hr = NDO->GetData(&punk);
			if (SUCCEEDED(hr) && punk)
			{
				long l = cncReset;
				::FireEvent(punk, szEventChangeObjectList, ptrList, NULL, &l, 0);
				punk->Release();
			}
		}
	}


	//CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
	//if( pwnd ) pwnd->InvalidateRect(0);

	return true;
}

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {C7B7F338-1CD1-4860-B0BA-0546B2027077}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane, "CGH.SwitchCGHPaneD", 
0xc7b7f338, 0x1cd1, 0x4860, 0xb0, 0xba, 0x5, 0x46, 0xb2, 0x2, 0x70, 0x77);

//[ag]3. guidinfo debug

// {E3471E78-A17D-464b-89F2-05B33BFFD7AC}
static const GUID guidActionSwitchCGHPane = 
{ 0xe3471e78, 0xa17d, 0x464b, { 0x89, 0xf2, 0x5, 0xb3, 0x3b, 0xff, 0xd7, 0xac } };

#else
//[ag]4. olecreate release

// {4B580D74-781A-4c70-A63B-8174B7088D63}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane, "CGH.SwitchCGHPane", 
0x4b580d74, 0x781a, 0x4c70, 0xa6, 0x3b, 0x81, 0x74, 0xb7, 0x8, 0x8d, 0x63);

//[ag]5. guidinfo release

// {C5EF929B-D017-4746-84E0-08B7AF921699}
static const GUID guidActionSwitchCGHPane = 
{ 0xc5ef929b, 0xd017, 0x4746, { 0x84, 0xe0, 0x8, 0xb7, 0xaf, 0x92, 0x16, 0x99 } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane,IDS_ACTION_SWITCHCGHPANE, -1, -1, guidActionSwitchCGHPane);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane)
	ARG_INT(_T(szPaneNum), CGH_COMPOSED)
	ARG_BOOL(_T(szMonochrome), 0)
	ARG_BOOL(_T(szNegative), 0)
	ARG_BOOL(_T(szRatioOver), 0)
END_ACTION_ARG_LIST();

//[ag]9. implementation

CActionSwitchCGHPane::CActionSwitchCGHPane()
{
}

CActionSwitchCGHPane::~CActionSwitchCGHPane()
{
}

bool CActionSwitchCGHPane::IsAvaible()
{
	if (!m_punkTarget)
		return false;

	IViewPtr view(m_punkTarget);
	if(view==0) return false;

	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;

	return true;
}

bool CActionSwitchCGHPane::IsChecked()
{
	return false;
}

IUnknown *CActionSwitchCGHPane::GetContextObject(CString cName, CString cType)
{

	IDataContext3Ptr ptrC = m_punkTarget;
	_bstr_t	bstrType = cType;

	if( cName.IsEmpty() )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		if( punkImage )
			punkImage->Release();

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
				if( punkImage )
					punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}


bool CActionSwitchCGHPane::Invoke()
{
	if (!m_punkTarget)
		return false;

	IViewPtr view(m_punkTarget);
	if(view==0) return false;

	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;

	long lPaneNum=GetArgumentInt(szPaneNum);
	long lMonochrome = GetArgumentInt(szMonochrome);
	long lNegative = GetArgumentInt(szNegative);
	long lRatioOver = GetArgumentInt(szRatioOver);

	bag->SetProperty(_bstr_t(szPaneNum),_variant_t(lPaneNum));
	bag->SetProperty(_bstr_t(szMonochrome),_variant_t(lMonochrome));
	bag->SetProperty(_bstr_t(szNegative),_variant_t(lNegative));
	bag->SetProperty(_bstr_t(szRatioOver),_variant_t(lRatioOver));

	IUnknown *punk2 = GetActiveFrom(m_punkTarget, szTypeImage);
	IImage3Ptr ptrImage(punk2);
	if (punk2) punk2->Release();

	IUnknown *punk3 = GetActiveFrom(m_punkTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punk3);
	if (punk3) punk3->Release();

	if(ptrImage)
	{
		::SetValue( ptrImage, CGH_ROOT, szPaneNum, long(lPaneNum) );
		::SetValue( ptrImage, CGH_ROOT, szMonochrome, long(lMonochrome) );
		::SetValue( ptrImage, CGH_ROOT, szNegative, long(lNegative) );
		::SetValue( ptrImage, CGH_ROOT, szRatioOver, long(lRatioOver) );
	}

	if(ptrList)
	{
		{
			INamedDataPtr sptrNDO = ptrList;
			if( sptrNDO == 0 ) ptrList->InitAttachedData();
		}
		::SetValue( ptrList, CGH_ROOT, szPaneNum, long(lPaneNum) );
		::SetValue( ptrList, CGH_ROOT, szMonochrome, long(lMonochrome) );
		::SetValue( ptrList, CGH_ROOT, szNegative, long(lNegative) );
		::SetValue( ptrList, CGH_ROOT, szRatioOver, long(lRatioOver) );
	}

	//CWnd * pwnd = GetWindowFromUnknown(view);
	//if( pwnd ) pwnd->InvalidateRect(0);


	return true;
}

//--------- (0)
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane0, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {F6105538-F530-4a74-A14F-5591D83F02DA}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane0, "CGH.SwitchCGHPane0D", 
0xf6105538, 0xf530, 0x4a74, 0xa1, 0x4f, 0x55, 0x91, 0xd8, 0x3f, 0x2, 0xda);

//[ag]3. guidinfo debug

// {B40B9870-D248-4cee-9A3D-DA9ABBEE579E}
static const GUID guidActionSwitchCGHPane0 = 
{ 0xb40b9870, 0xd248, 0x4cee, { 0x9a, 0x3d, 0xda, 0x9a, 0xbb, 0xee, 0x57, 0x9e } };

#else
//[ag]4. olecreate release

// {88975F69-AA27-4780-8169-77F6D545357B}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane0, "CGH.SwitchCGHPane0", 
0x88975f69, 0xaa27, 0x4780, 0x81, 0x69, 0x77, 0xf6, 0xd5, 0x45, 0x35, 0x7b);

//[ag]5. guidinfo release

// {E5006748-180A-4084-B6B8-ACFBC9FDFED2}
static const GUID guidActionSwitchCGHPane0 = 
{ 0xe5006748, 0x180a, 0x4084, { 0xb6, 0xb8, 0xac, 0xfb, 0xc9, 0xfd, 0xfe, 0xd2 } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane0,IDS_ACTION_SWITCHCGHPANE0, -1, -1, guidActionSwitchCGHPane0);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane0,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane0)
END_ACTION_ARG_LIST();

//[ag]9. implementation


CActionSwitchCGHPane0::CActionSwitchCGHPane0()
{
	m_lPaneToSwitch=0;
}

CActionSwitchCGHPane0::~CActionSwitchCGHPane0()
{
}

bool CActionSwitchCGHPane0::IsAvaible()
{
	if(!CActionSwitchCGHPane::IsAvaible()) return false;

	if (!m_punkTarget)
		return false;

	IViewPtr view(m_punkTarget);
	if(view==0) return false;

	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;


	IUnknown *punk2 = GetActiveFrom(m_punkTarget, szTypeImage);
	IImage3Ptr ptrImage(punk2);
	if (punk2) punk2->Release();

	IUnknown *punk3 = GetActiveFrom(m_punkTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punk3);
	if (punk3) punk3->Release();

	IImage3Ptr ptrImageRoot=0;
	{
		IUnknown *punk;
		INamedDataObject2Ptr ptrNDO(ptrImage);
		if(ptrNDO!=0)
		{
			ptrNDO->GetParent(&punk);
			ptrImageRoot=IImage3Ptr(punk);
			if(punk!=0) punk->Release();
		}
	}
	if(ptrImageRoot==0) ptrImageRoot=ptrImage;

	int state0=0, state1=0, state2=0, state3=0;
	if (ptrImageRoot!=0)
	{
		int bit=1;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE0, 0)) state0|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE1, 0)) state1|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE2, 0)) state2|=bit;
		if(::GetValueInt(ptrImageRoot, KARYO_ACTIONROOT, CGH_PANE3, 0)) state3|=bit;
	}

	_bstr_t	bstrCC;
	{
		IUnknown *punkCC = 0;
		if(ptrImageRoot) ptrImageRoot->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		if(punkCC) punkCC->Release();
		if(ptrCC)
		{
			BSTR	bstr;
			ptrCC->GetCnvName( &bstr );
			bstrCC = bstr;
			::SysFreeString( bstr );
		}
	}
	if(bstrCC!=_bstr_t("CGH"))
	{
		state0 &= ~1;
		state1 &= ~1;
		state2 &= ~1;
		state3 &= ~1;
	}

	if (ptrList!=0)
	{
		int bit=2;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE0, 0)) state0|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE1, 0)) state1|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE2, 0)) state2|=bit;
		if(::GetValueInt(ptrList, KARYO_ACTIONROOT, CGH_PANE3, 0)) state3|=bit;
	}

	switch(m_lPaneToSwitch)
	{
	case CGH_DAPI: return(state0!=0);
	case CGH_FITC: return(state1!=0);
	case CGH_TRITC: return(state2!=0);
	case CGH_RATIO: return(state3!=0);
	case CGH_COMPOSED: return(state0!=0 || state1!=0 || state2!=0);
	}

	return true;
}

bool CActionSwitchCGHPane0::IsChecked()
{
	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;

	_variant_t	var;
	bag->GetProperty(_bstr_t(szPaneNum), &var);
	if(var.vt != VT_EMPTY) return var.lVal==m_lPaneToSwitch;

	return false;
}


bool CActionSwitchCGHPane0::Invoke()
{
	if (!m_punkTarget)
		return false;

	IViewPtr view(m_punkTarget);
	if(view==0) return false;

	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;

	long lPaneNum=m_lPaneToSwitch;

	bag->SetProperty(_bstr_t(szPaneNum),_variant_t(lPaneNum));

	IUnknown *punk2 = GetActiveFrom(m_punkTarget, szTypeImage);
	IImage3Ptr ptrImage(punk2);
	if (punk2) punk2->Release();

	IUnknown *punk3 = GetActiveFrom(m_punkTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punk3);
	if (punk3) punk3->Release();

	if(ptrImage)
	{
		::SetValue( ptrImage, CGH_ROOT, szPaneNum, long(lPaneNum) );
	}

	if(ptrList)
	{
		{
			INamedDataPtr sptrNDO = ptrList;
			if( sptrNDO == 0 ) ptrList->InitAttachedData();
		}
		::SetValue( ptrList, CGH_ROOT, szPaneNum, long(lPaneNum) );
	}

	//CWnd * pwnd = GetWindowFromUnknown(view);
	//if( pwnd ) pwnd->InvalidateRect(0);


	return true;
}
//---------

//--------- (1)
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane1, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {03584B8A-8251-4a96-9848-E3EC0A06EC8A}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane1, "CGH.SwitchCGHPane1D",
0x3584b8a, 0x8251, 0x4a96, 0x98, 0x48, 0xe3, 0xec, 0xa, 0x6, 0xec, 0x8a);

//[ag]3. guidinfo debug

// {FC941E01-9939-405c-A05B-AA1EAC07290C}
static const GUID guidActionSwitchCGHPane1 =
{ 0xfc941e01, 0x9939, 0x405c, { 0xa0, 0x5b, 0xaa, 0x1e, 0xac, 0x7, 0x29, 0xc } };

#else
//[ag]4. olecreate release

// {CE79E760-3DA4-4c95-829E-0BF44CDC50EB}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane1, "CGH.SwitchCGHPane1",
0xce79e760, 0x3da4, 0x4c95, 0x82, 0x9e, 0xb, 0xf4, 0x4c, 0xdc, 0x50, 0xeb);

//[ag]5. guidinfo release

// {0AE396A6-4AA2-4816-AF73-4F683072FFAA}
static const GUID guidActionSwitchCGHPane1 =
{ 0xae396a6, 0x4aa2, 0x4816, { 0xaf, 0x73, 0x4f, 0x68, 0x30, 0x72, 0xff, 0xaa } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane1,IDS_ACTION_SWITCHCGHPANE1, -1, -1, guidActionSwitchCGHPane1);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane1,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane1)
END_ACTION_ARG_LIST();

//[ag]9. implementation
CActionSwitchCGHPane1::CActionSwitchCGHPane1()
{
	m_lPaneToSwitch=1;
}
//-----

//--------- (2)
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane2, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {02DCFEC7-C1A2-465d-90AF-0633B2D90E66}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane2, "CGH.SwitchCGHPane2D",
0x2dcfec7, 0xc1a2, 0x465d, 0x90, 0xaf, 0x6, 0x33, 0xb2, 0xd9, 0xe, 0x66);

//[ag]3. guidinfo debug

// {B72B0BC9-1BAB-434d-BF4C-7BAF6E1B93BF}
static const GUID guidActionSwitchCGHPane2 =
{ 0xb72b0bc9, 0x1bab, 0x434d, { 0xbf, 0x4c, 0x7b, 0xaf, 0x6e, 0x1b, 0x93, 0xbf } };

#else
//[ag]4. olecreate release

// {B204EB8E-8E6D-484b-931A-D58776DC5B0C}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane2, "CGH.SwitchCGHPane2",
0xb204eb8e, 0x8e6d, 0x484b, 0x93, 0x1a, 0xd5, 0x87, 0x76, 0xdc, 0x5b, 0xc);

//[ag]5. guidinfo release

// {7D4C06E6-6117-4e00-8255-8C7EA8DB570B}
static const GUID guidActionSwitchCGHPane2 =
{ 0x7d4c06e6, 0x6117, 0x4e00, { 0x82, 0x55, 0x8c, 0x7e, 0xa8, 0xdb, 0x57, 0xb } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane2,IDS_ACTION_SWITCHCGHPANE2, -1, -1, guidActionSwitchCGHPane2);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane2,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane2)
END_ACTION_ARG_LIST();

//[ag]9. implementation
CActionSwitchCGHPane2::CActionSwitchCGHPane2()
{
	m_lPaneToSwitch=2;
}
//-----

//--------- (3)
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane3, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {7F9CD0AE-6BE0-4b80-B77F-22C1F7EB013C}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane3, "CGH.SwitchCGHPane3D",
0x7f9cd0ae, 0x6be0, 0x4b80, 0xb7, 0x7f, 0x22, 0xc1, 0xf7, 0xeb, 0x1, 0x3c);

//[ag]3. guidinfo debug

// {B556807C-6F54-4993-B392-B4A92CE92116}
static const GUID guidActionSwitchCGHPane3 =
{ 0xb556807c, 0x6f54, 0x4993, { 0xb3, 0x92, 0xb4, 0xa9, 0x2c, 0xe9, 0x21, 0x16 } };

#else
//[ag]4. olecreate release

// {AC01E0EF-2812-4d89-8525-31FAFE893CEB}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane3, "CGH.SwitchCGHPane3",
0xac01e0ef, 0x2812, 0x4d89, 0x85, 0x25, 0x31, 0xfa, 0xfe, 0x89, 0x3c, 0xeb);

//[ag]5. guidinfo release

// {CB88A03B-E174-4ba6-B778-16FBC05B7F41}
static const GUID guidActionSwitchCGHPane3 =
{ 0xcb88a03b, 0xe174, 0x4ba6, { 0xb7, 0x78, 0x16, 0xfb, 0xc0, 0x5b, 0x7f, 0x41 } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane3,IDS_ACTION_SWITCHCGHPANE3, -1, -1, guidActionSwitchCGHPane3);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane3,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane3)
END_ACTION_ARG_LIST();

//[ag]9. implementation
CActionSwitchCGHPane3::CActionSwitchCGHPane3()
{
	m_lPaneToSwitch=3;
}
//-----

//--------- (4)
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSwitchCGHPane4, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {3A17BD9C-0464-476c-8FF6-5190BB808208}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane4, "CGH.SwitchCGHPane4D",
0x3a17bd9c, 0x464, 0x476c, 0x8f, 0xf6, 0x51, 0x90, 0xbb, 0x80, 0x82, 0x8);

//[ag]3. guidinfo debug

// {99DEC595-DEC3-47ea-BB89-397E05B1CE95}
static const GUID guidActionSwitchCGHPane4 =
{ 0x99dec595, 0xdec3, 0x47ea, { 0xbb, 0x89, 0x39, 0x7e, 0x5, 0xb1, 0xce, 0x95 } };

#else
//[ag]4. olecreate release

// {8F9AE9EB-66C0-4dd3-90D3-2D6C5CC6F578}
GUARD_IMPLEMENT_OLECREATE(CActionSwitchCGHPane4, "CGH.SwitchCGHPane4",
0x8f9ae9eb, 0x66c0, 0x4dd3, 0x90, 0xd3, 0x2d, 0x6c, 0x5c, 0xc6, 0xf5, 0x78);

//[ag]5. guidinfo release

// {9B652701-CBE8-4ca8-858C-56F7C98EDE73}
static const GUID guidActionSwitchCGHPane4 =
{ 0x9b652701, 0xcbe8, 0x4ca8, { 0x85, 0x8c, 0x56, 0xf7, 0xc9, 0x8e, 0xde, 0x73 } };

#endif //_DEBUG

//[ag]6. action info

ACTION_INFO_FULL(CActionSwitchCGHPane4,IDS_ACTION_SWITCHCGHPANE4, -1, -1, guidActionSwitchCGHPane4);

//[ag]7. targets

ACTION_TARGET(CActionSwitchCGHPane4,"view site");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSwitchCGHPane4)
END_ACTION_ARG_LIST();

//[ag]9. implementation
CActionSwitchCGHPane4::CActionSwitchCGHPane4()
{
	m_lPaneToSwitch=4;
}
//-----

HRESULT CColorConvertorCGH::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *punkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorCGH, CnvEx);

	byte *pal_r=pThis->m_pal_r; // лучше б забирать через функцию и проверять на 0
	byte *pal_g=pThis->m_pal_g;
	byte *pal_b=pThis->m_pal_b;

	if(pThis->m_fRatioMin<0)
	{
		pThis->m_PaneColor[0]=::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_PANE0_COLOR, RGB( 0, 0, 255 ) );
		pThis->m_PaneColor[1]=::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_PANE1_COLOR, RGB( 0, 255, 0 ) );
		pThis->m_PaneColor[2]=::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_PANE2_COLOR, RGB( 255, 0, 0 ) );

		pThis->m_fRatioMin = ::GetValueDouble( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOMIN, 0.75 );
		pThis->m_fRatioMid = ::GetValueDouble( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOMID, 1.0 );
		pThis->m_fRatioMax = ::GetValueDouble( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOMAX, 1.25 );

		COLORREF cl0 = ::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_RATIO_COLOR_MID, RGB( 0, 0, 255 ) );
		COLORREF cl1min = ::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_RATIO_COLOR_MIN, RGB( 255, 0, 0 ) );
		COLORREF cl1max= ::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_RATIO_COLOR_MAX, RGB( 0, 255, 0 ) );
		COLORREF clColor0 = ::GetValueColor( ::GetAppUnknown(), CGH_ROOT, CGH_RATIO_COLOR_BACKGROUND, RGB( 0, 0, 0 ) );

		int nRatioSmoothPalette=::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_RATIO_SMOOTH_PALETTE, 1 );

		for(int i=0; i<256; i++)
		{
			int b=255-abs(i-128)*8;
			int g=(i-(128+16))*8;
			int r=((128-16)-i)*8;
			pal_r[i]=max(0,min(255,r));
			pal_b[i]=max(0,min(255,b));
			pal_g[i]=max(0,min(255,g));

			double c=i/128.0;
			COLORREF cl1;
				
			if(c<pThis->m_fRatioMid)
			{
				cl1 = cl1min;
				c=(1-c)/max(pThis->m_fRatioMid-pThis->m_fRatioMin, 0.001);
			}
			else
			{
				cl1=cl1max;
				c=(c-1)/max(pThis->m_fRatioMax-pThis->m_fRatioMid, 0.001);
			}
			if(c>1) c=1;
			if(c<0.0) c=0;
			if(nRatioSmoothPalette==0 && c<1) c=0; //если не плавная палитра - то все внутри диапазона одного цвета

			pal_r[i]=int(GetRValue(cl1)*c+GetRValue(cl0)*(1-c));
			pal_g[i]=int(GetGValue(cl1)*c+GetGValue(cl0)*(1-c));
			pal_b[i]=int(GetBValue(cl1)*c+GetBValue(cl0)*(1-c));

			/*
			double c=i/128.0;
			if(c<=0.8)
			{
				pal_r[i]=255;
				pal_g[i]=0;
				pal_b[i]=0;
			}
			else if(c>=1.2)
			{
				pal_r[i]=0;
				pal_g[i]=255;
				pal_b[i]=0;
			}
			else
			{
				pal_r[i]=0;
				pal_g[i]=0;
				pal_b[i]=255;
			}
			*/
		}
		pal_r[0]=int(GetRValue(clColor0));
		pal_g[0]=int(GetGValue(clColor0));
		pal_b[0]=int(GetBValue(clColor0));
	}

	byte	*pLookupTableR = pThis->m_pLookupTableR;
	byte	*pLookupTableG = pThis->m_pLookupTableG;
	byte	*pLookupTableB = pThis->m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);																	\
		byte	fillG = GetGValue(dwFillColor);																	\
		byte	fillB = GetBValue(dwFillColor);																	\

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}

	int lPaneNum=-1000, lNegative=-1000, lMonochrome=-1000, lRatioOver=-1000;
	INamedPropBagPtr bag(punkParams);
	if(bag!=0)
	{
		_variant_t	var;
		bag->GetProperty(_bstr_t(szPaneNum), &var);
		if(var.vt != VT_EMPTY) lPaneNum=var.lVal;
		bag->GetProperty(_bstr_t(szMonochrome), &var);
		if(var.vt != VT_EMPTY) lMonochrome=var.lVal;
		bag->GetProperty(_bstr_t(szNegative), &var);
		if(var.vt != VT_EMPTY) lNegative=var.lVal;
		bag->GetProperty(_bstr_t(szRatioOver), &var);
		if(var.vt != VT_EMPTY) lRatioOver=var.lVal;
	}
	if(lPaneNum==-1000)
	{
		lPaneNum = ::GetValueInt( pimage, CGH_ROOT, szPaneNum, -1000 );
		if(lPaneNum ==-1000)
		{
			IViewPtr view(punkParams);
			if(view!=0)
			{
				IUnknown *punk2 = GetActiveFrom(view, szTypeObjectList);
				lPaneNum = ::GetValueInt( punk2, CGH_ROOT, szPaneNum, -1000 );
				if (punk2) punk2->Release();
			}
		}
		if(lPaneNum ==-1000) lPaneNum=CGH_COMPOSED;
		if(bag!=0) bag->SetProperty(_bstr_t(szPaneNum),_variant_t(long(lPaneNum)));
	}
	if(lPaneNum ==-1000) lPaneNum=CGH_COMPOSED;
	if(lMonochrome==-1000)
	{
		lMonochrome = ::GetValueInt( pimage, CGH_ROOT, szMonochrome, -1000 );
		if(lMonochrome ==-1000)
		{
			IViewPtr view(punkParams);
			if(view!=0)
			{
				IUnknown *punk2 = GetActiveFrom(view, szTypeObjectList);
				lMonochrome = ::GetValueInt( punk2, CGH_ROOT, szMonochrome, -1000 );
				if (punk2) punk2->Release();
			}
		}
		if(lMonochrome ==-1000) lMonochrome=0;
		if(bag!=0) bag->SetProperty(_bstr_t(szMonochrome),_variant_t(long(lMonochrome)));
	}
	if(lMonochrome ==-1000) lMonochrome=0;
	if(lNegative==-1000)
	{
		lNegative = ::GetValueInt( pimage, CGH_ROOT, szNegative, -1000 );
		if(lNegative ==-1000)
		{
			IViewPtr view(punkParams);
			if(view!=0)
			{
				IUnknown *punk2 = GetActiveFrom(view, szTypeObjectList);
				lNegative = ::GetValueInt( punk2, CGH_ROOT, szNegative, -1000 );
				if (punk2) punk2->Release();
			}
		}
		if(lNegative ==-1000) lNegative=0;
		if(bag!=0) bag->SetProperty(_bstr_t(szNegative),_variant_t(long(lNegative)));
	}
	if(lNegative ==-1000) lNegative=0;
	if(lRatioOver==-1000)
	{
		lRatioOver = ::GetValueInt( pimage, CGH_ROOT, szRatioOver, -1000 );
		if(lRatioOver ==-1000)
		{
			IViewPtr view(punkParams);
			if(view!=0)
			{
				IUnknown *punk2 = GetActiveFrom(view, szTypeObjectList);
				lRatioOver = ::GetValueInt( punk2, CGH_ROOT, szRatioOver, -1000 );
				if (punk2) punk2->Release();
			}
		}
		if(lRatioOver ==-1000) lRatioOver=0;
		if(bag!=0) bag->SetProperty(_bstr_t(szRatioOver),_variant_t(long(lRatioOver)));
	}
	if(lRatioOver ==-1000) lRatioOver=0;

	DWORD dwViewType=lPaneNum;
	color xor0=0, xor1=0, xor2=0, xor3=0;
	if(lMonochrome) dwViewType|=16;
	if(lNegative)
	{
		xor0=65535; xor1=65535; xor2=65535; xor3=65535;
	}
	else
	{
		xor0=0; xor1=0; xor2=0; xor3=0;
	}

	int Rm[3]={0,0,0}, Gm[3]={0,0,0}, Bm[3]={0,0,0};
	{
		for(int i=0; i<3; i++)
		{
			if(lPaneNum==CGH_COMPOSED || lPaneNum==i)
			{
				Rm[i]=GetRValue(pThis->m_PaneColor[i]);
				Gm[i]=GetGValue(pThis->m_PaneColor[i]);
				Bm[i]=GetBValue(pThis->m_PaneColor[i]);
			}
			if(lPaneNum==i && lMonochrome)
			{
				Rm[i]=255;
				Gm[i]=255;
				Bm[i]=255;
			}
		}
	}

	int nMaskedThreshold0=int(pThis->m_fRatioMin*127*256);
	int nMaskedThreshold1=int(pThis->m_fRatioMax*127*256);

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		const unsigned uPanesUsed=0x0F;
		CONVERTTODIB_PART1
			;
		switch(dwViewType)
		{
		case CGH_RATIO:	//ratio
			if(pan3<128 || (pan3<4096 && G<=B+int(4096))) // поправка для случайных точек за пределами объектов после jpeg compression.
				pan3=0;									  
			else
				pan3^=xor3;
			Rbyte = pal_r[pan3>>8];
			Gbyte = pal_g[pan3>>8];
			Bbyte = pal_b[pan3>>8];
			break;
		case CGH_RATIO | 0x10: //ratio mono
			if(pan3<128 || (pan3<4096 && G<=B+int(4096)))
				pan3=0;
			else
				pan3^=xor3;
			Rbyte = pLookupTableR?pLookupTableR[pan3]:(pan3>>8);
			Gbyte = pLookupTableG?pLookupTableG[pan3]:(pan3>>8);
			Bbyte = pLookupTableB?pLookupTableB[pan3]:(pan3>>8);
			break;
		default:
			R^=xor0; G^=xor1; B^=xor2;
			Rbyte = min(255, (Rm[0]*R + Rm[1]*G + Rm[2]* B)/65536);
			Gbyte = min(255, (Gm[0]*R + Gm[1]*G + Gm[2]* B)/65536);
			Bbyte = min(255, (Bm[0]*R + Bm[1]*G + Bm[2]* B)/65536);
			break;
		}
		if(lRatioOver || dwViewType==CGH_RATIO)
		{
			if(pan3!=0)
			{
				if(pan3<=nMaskedThreshold0)
				{
					if(pan3>4096 || G>B+int(4096)) // workaround случайных точек за пределами объектов после jpeg compression.
					{
						Rbyte = pal_r[1];
						Gbyte = pal_g[1];
						Bbyte = pal_b[1];
					}
				} else 	if(pan3>nMaskedThreshold1)
				{
					Rbyte = pal_r[255];
					Gbyte = pal_g[255];
					Bbyte = pal_b[255];
				}
			}				
		}
			
		CONVERTTODIB_PART2
	}

	if( ptemp )delete ptemp;

	return S_OK;
}

CColorConvertorCGH::CColorConvertorCGH()
{
	_OleLockApp( this );
	m_fRatioMin = -1;
	m_fRatioMid = -1;
	m_fRatioMax = -1;
	Register( ::GetAppUnknown() );
}

CColorConvertorCGH::~CColorConvertorCGH()
{
	_OleUnlockApp( this );
}

void CColorConvertorCGH::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp(pszEvent, szEventNewSettings) )
	{
		m_fRatioMin = -1;
		m_fRatioMid = -1;
		m_fRatioMax = -1;
	}
	if( !strcmp( szBeforeAppTerminate, pszEvent ) )
	{
		UnRegister( ::GetAppUnknown() );
	}

}

const char *CColorConvertorCGH::GetCnvName()
{
	return _T("CGH");
}

int CColorConvertorCGH::GetColorPanesCount()
{
	return 4;
/*
Паны:
Исходное изображение DAPI, FITC, TRITC. О них никто не знает.
Для расчета коэф. флюоресценции и еще чего-нибудь.
DAPI, FITC, TRITC – для пользователя, их можно модифицировать.
Для их отображения применяется цветокодирование. DAPI – голубой, FITC – зеленый, TRITC – красный.
цветокодированное изображения. Делеция – красный цвет (умолч.< 0.5),
амплификация  (умолч.>1.5) – зеленый цвет

получаем:
DAPI user
FITC user
TRITC user
Color Coding
DAPI src(*)
FITC src(*)
TRITC src(*)
*/
}

const char *CColorConvertorCGH::GetPaneName(int numPane)
{
	//if (numPane == 0) return _T(::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE0 PANENAME, CGH_PANE0));
	//if (numPane == 1) return _T(::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE1 PANENAME, CGH_PANE1));
	//if (numPane == 2) return _T(::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE2 PANENAME, CGH_PANE2));
	//if (numPane == 3) return _T(::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE3 PANENAME, CGH_PANE3));
	if (numPane == 0) return _T(CGH_PANE0);
	if (numPane == 1) return _T(CGH_PANE1);
	if (numPane == 2) return _T(CGH_PANE2);
	if (numPane == 3) return _T(CGH_PANE3);

	return _T("");
}

const char *CColorConvertorCGH::GetPaneShortName( int numPane )
{
	// [vanek]: return valid pointer - 02.04.2004
	static CString s_res;
	switch( numPane )
	{
	case 0:
		s_res = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE0 PANESHORTNAME, "0");
		break;

	case 1:
        s_res = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE1 PANESHORTNAME, "1");
		break;

	case 2:
		s_res = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE2 PANESHORTNAME, "2");
		break;
	case 3:
		s_res = ::GetValueString(::GetAppUnknown(), CGH_ROOT, CGH_PANE3 PANESHORTNAME, "3");
		break;

	default:
		s_res = "";
	}


	return s_res;
}

color CColorConvertorCGH::ConvertToHumanValue( color colorInternal, int numPane )
{
	return color( double(colorInternal) * 256.0 / double(colorMax) );
}

DWORD CColorConvertorCGH::GetPaneFlags(int numPane)
{
	return pfOrdinary|pfGray;
}

void CColorConvertorCGH::ConvertRGBToImage( byte *_pRGB, color **_ppColor, int _cx )
{
	color	*p0, *p1, *p2;
	p0 = _ppColor[0];
	p1 = _ppColor[1];
	p2 = _ppColor[2];
	byte	*pRGB = _pRGB;
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		p2[n] = ByteAsColor( _pRGB[c++] );
		p1[n] = ByteAsColor( _pRGB[c++] );
		p0[n] = ByteAsColor( _pRGB[c++] );
	}
#if 0
	__asm
	{
			mov	ebx, _pRGB
			mov	eax, _ppColor
			mov	ecx, _cx


			push	ebp
			push	esi
			push	edi

			mov		ebp, DWORD PTR [eax+0]		//_ppColor[0]
			mov		esi, DWORD PTR [eax+4]		//_ppColor[0]
			mov		edi, DWORD PTR [eax+8]		//_ppColor[0]
			xor		eax, eax
$loop:
			mov		ah, BYTE PTR [ebx]
			mov		WORD PTR [edi], ax
			inc		ebx
			mov		ah, BYTE PTR [ebx]
			mov		WORD PTR [esi], ax
			inc		ebx
			mov		ah, BYTE PTR [ebx]
			mov		WORD PTR [ebp], ax
			inc		ebx

			add		ebp, 2
			add		esi, 2
			add		edi, 2

			dec		ecx
			jne		$loop

			pop		edi
			pop		esi
			pop		ebp
			
	}
#endif
}

void CColorConvertorCGH::ConvertImageToRGB( color **_ppColor, byte *_pRGB, int _cx )
{
	color	*p0, *p1, *p2;
	p0 = _ppColor[0];
	p1 = _ppColor[1];
	p2 = _ppColor[2];
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		_pRGB[c++] = ColorAsByte( p2[n] );
		_pRGB[c++] = ColorAsByte( p1[n] );
		_pRGB[c++] = ColorAsByte( p0[n] );
	}
#if 0
	__asm
	{
			mov	ebx, _pRGB
			mov	eax, _ppColor
			mov	ecx, _cx


			push	ebp
			push	esi
			push	edi

			mov		ebp, DWORD PTR [eax+0]		//_ppColor[0]
			mov		esi, DWORD PTR [eax+4]		//_ppColor[1]
			mov		edi, DWORD PTR [eax+8]		//_ppColor[2]
			xor		eax, eax
			xor		edx, edx
$loop:
			mov		ax, WORD PTR [edi]
			mov		BYTE PTR [ebx], ah
			inc		ebx
			mov		ax, WORD PTR [esi]
			mov		BYTE PTR [ebx], ah
			inc		ebx
			mov		ax, WORD PTR [ebp]
			mov		BYTE PTR [ebx], ah
			inc		ebx

			inc		ebp
			inc		ebp
			inc		esi
			inc		esi
			inc		edi
			inc		edi

			dec		ecx
			jne		$loop

			pop		edi
			pop		esi
			pop		ebp
			
	}
#endif
}


void CColorConvertorCGH::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];
	for( long n = 0; n < cx; n++ )
	{
		p2[n] = ByteAsColor( pGray[n] );
		p1[n] = ByteAsColor( pGray[n] );
		p0[n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorCGH::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	for( long n = 0; n < cx; n++ )
	{
		pGray[n] = Bright( ColorAsByte( p0[n] ), 
						ColorAsByte( p1[n] ), 
						ColorAsByte( p2[n] ) );
	}
}

void CColorConvertorCGH::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CGH));
}

void CColorConvertorCGH::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ;
    double RR, GG, BB;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		L = pLab[c++];
		a = pLab[c++];
		b = pLab[c++];

		fY = pow((L + 16.0) / 116.0, 3.0);
	    if (fY < 0.008856)
	       fY = L / 903.3;
	    Y = fY;

	    if (fY > 0.008856)
			fY = pow(fY, 1.0/3.0);
		else
			fY = 7.787 * fY + 16.0/116.0;

		fX = a / 500.0 + fY;
		if (fX > 0.206893)
			X = pow(fX, 3.0);
		else
			X = (fX - 16.0/116.0) / 7.787;

		fZ = fY - b /200.0;
		if (fZ > 0.206893)
			Z = pow(fZ, 3.0);
		else
			Z = (fZ - 16.0/116.0) / 7.787;

		X *= 0.950456;
		Y *= 1;
		Z *= 1.088754;
		
		RR =  (3.240479*X - 1.537150*Y - 0.498535*Z);
		GG =  (-0.969256*X + 1.875992*Y + 0.041556*Z);
		BB =  (0.055648*X - 0.204043*Y + 1.057311*Z);
	
		p0[n] =  ScaleDoubleToColor(RR);
		p1[n] =  ScaleDoubleToColor(GG);
		p2[n] =  ScaleDoubleToColor(BB);
	}

}

void CColorConvertorCGH::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ;
	double R, G, B;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		R = ScaleColorToDouble(p0[n]);
		G = ScaleColorToDouble(p1[n]);
		B = ScaleColorToDouble(p2[n]);

		X = (0.412453*R + 0.357580*G + 0.180423*B)/0.950456;
		Y = (0.212671*R + 0.715160*G + 0.072169*B);
	    Z = (0.019334*R + 0.119193*G + 0.950227*B)/1.088754;

		if (Y > 0.008856)
	    {
	       fY = pow(Y, 1.0/3.0);
		   L = 116.0*fY - 16.0;
		}
		else
		{
			fY = 7.787*Y + 16.0/116.0;
			L = 903.3*Y;
		}
		if (X > 0.008856)
			fX = pow(X, 1.0/3.0);
		else
			fX = 7.787*X + 16.0/116.0;
		if (Z > 0.008856)
			fZ = pow(Z, 1.0/3.0);
		else
			fZ = 7.787*Z + 16.0/116.0;

		a = 500.0*(fX - fY);
		b = 200.0*(fY - fZ);

		pLab[c++] = L;
		pLab[c++] = a;
		pLab[c++] = b;
	}
}


void CColorConvertorCGH::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(0, 0, 255);
		break;
	case 1:
		*pdwColor = RGB(0, 255, 0);
		break;
	case 2:
		*pdwColor = RGB(255, 0, 0);
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorCGH::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0);
		break;
	case 2:
		*pdwColorValues =  RGB(NotUsed, 0, 0);
		break;
	default:
		*pdwColorValues = 0;
	}
}

//delete operators
void FreeEmpty( void *ptr )
{}

void FreeReleaseUnknown( void *ptr )
{	IUnknown	*punk = (IUnknown	*)ptr;	if( punk )punk->Release();}

void FreeFree( void *ptr )
{
	if(ptr) free(ptr);
}
