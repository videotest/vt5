#include "stdafx.h"
#include "marker.h"
#include "resource.h"
#include "math.h"
#include "carioint.h"
#include "\vt5\common2\misc_calibr.h"
#include "units_const.h"
#include "docmiscint.h"
#include "PropBag.h"

#define _USEGDIPLUS
#if defined(_USEGDIPLUS)
#include <gdiplus.h>
using namespace Gdiplus;
#endif

#if defined(_USEGDIPLUS)
	static GdiplusStartupInput gdiplusStartupInput;
    static ULONG_PTR gdiplusToken;
#endif


IMPLEMENT_DYNCREATE(CMarker, CDataObjectBase)
IMPLEMENT_DYNCREATE(CArrow, CDataObjectBase)

// {7A7CBD31-27B1-4e8e-9726-B446A0E8F900}
GUARD_IMPLEMENT_OLECREATE(CMarker, "Editor.Marker", 
0x7a7cbd31, 0x27b1, 0x4e8e, 0x97, 0x26, 0xb4, 0x46, 0xa0, 0xe8, 0xf9, 0x0);
// {382C36D6-22C9-49ff-9664-F03548696A14}
GUARD_IMPLEMENT_OLECREATE(CArrow, "Editor.Arrow", 
0x382c36d6, 0x22c9, 0x49ff, 0x96, 0x64, 0xf0, 0x35, 0x48, 0x69, 0x6a, 0x14);
// {264C927F-E9B8-4fef-9EF6-1EA1222D3A97}
static const GUID clsidMarkerDataObject = 
{ 0x264c927f, 0xe9b8, 0x4fef, { 0x9e, 0xf6, 0x1e, 0xa1, 0x22, 0x2d, 0x3a, 0x97 } };
// {82E21FB9-422F-4e6a-8422-C2FFF3DF02AA}
static const GUID clsidArrowDataObject = 
{ 0x82e21fb9, 0x422f, 0x4e6a, { 0x84, 0x22, 0xc2, 0xff, 0xf3, 0xdf, 0x2, 0xaa } };


DATA_OBJECT_INFO_FULL(IDS_MARKER_TYPE, CMarker, szTypeMarker, szDrawingObjectList, clsidMarkerDataObject, IDI_MARKER );
DATA_OBJECT_INFO_FULL(IDS_ARROW_TYPE, CArrow, szTypeArrow, szDrawingObjectList, clsidArrowDataObject, IDI_ARROW );

BEGIN_INTERFACE_MAP(CMarker, CDataObjectBase)
	INTERFACE_PART(CMarker, IID_IDrawObject, Draw)
	INTERFACE_PART(CMarker, IID_IMarker, Mark)
	// vk
	INTERFACE_PART(CMarker, IID_IMarker2, Mark)
	// end 	
	INTERFACE_PART(CMarker, IID_IMarker3, Mark)
	INTERFACE_PART(CMarker, IID_ICalibr, Calibr)
	INTERFACE_PART(CMarker, IID_IRectObject, Rect)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CMarker, Mark)
IMPLEMENT_UNKNOWN(CMarker, Rect)
IMPLEMENT_UNKNOWN(CMarker, Calibr)

CMarker::CMarker()
{
	m_strText = "200";
	m_fLength = 100;
	m_x = 0;
	m_y = 0;
	// vk
	m_nPrecision = 1;
	m_fAdditionalWidth = 0;
	//m_nAdditionalWidth = 0;
	m_nFontHeight = 20;
	m_fZoom = 1;
	// end vk
	m_nPixelLength = 0;
	m_fCalibr = 1;
	m_guidC = GUID_NULL;
	m_xEnd=0;
	m_yEnd=0;
	#if defined(_USEGDIPLUS)
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	#endif
	m_crFillColor = RGB(255, 255, 255);
	m_bFillRect=0;
	IUnknownPtr punkDoc = GetAppUnknown();
	if(GetValueInt( punkDoc, "EditMarker", "FillRect", m_bFillRect )>0)
		m_bFillRect=1;
	//old_FillRect=m_bFillRect;
	if(GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor ))
		m_crFillColor=GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor );
	//old_FillColor=m_crFillColor;
	if(GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision )>=0)
	{
		m_countComma=GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision );
		m_nPrecision=m_countComma;
	}
	else
		m_countComma=-1;
	//добавим чтение кол-ва запятых после знака Accuracy. надо все пересмотреть с этими знаками.
	if(GetValueInt( punkDoc, "EditMarker", "Accuracy", m_nPrecision )>=0)
		m_nPrecision=GetValueInt( punkDoc, "EditMarker", "Accuracy", m_nPrecision );
	//old_Precision=m_nPrecision;
	//old_countComma=m_countComma;
	
	m_bScaleMarker=0;
	if(GetValueInt( punkDoc, "EditMarker", "ScaleMarker", m_bScaleMarker )>0)
		m_bScaleMarker=1;
	strScale="1:1";
	//old_ScaleMarker=m_bScaleMarker;

	if(GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" ))
		m_nameF = GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" );
	//old_nameF=m_nameF;
	m_nFontHeight=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight ) > 0 )
		m_nFontHeight = GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight );
	//old_FontHeight=m_nFontHeight;

	BYTE bIsFontItalic = 0;
	BYTE bIsFontUnderline = 0;
	m_resStyle=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Bold", m_resStyle ) )
		m_resStyle=m_resStyle|FontStyleBold;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Italic", bIsFontItalic) )
		m_resStyle=m_resStyle|FontStyleItalic;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Underline", bIsFontUnderline) )
		m_resStyle=m_resStyle|FontStyleUnderline;
	//old_resStyle=m_resStyle;

	m_crTextColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor ))
		m_crTextColor = GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor );
	//old_TextColor=m_crTextColor;
	m_crMarkerColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor ))
		m_crMarkerColor = GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor );
	//old_MarkerColor=m_crMarkerColor;
	m_initStrUnitName = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	m_strUnitName="";

	//m_bLoadDataMarker=false;
}

CMarker::~CMarker()
{
	#if defined(_USEGDIPLUS)
			GdiplusShutdown(gdiplusToken);
	#endif
}

// vk
const int cyMarker = 5;
//const int cyMarker = 10;
// end vk

void CMarker::VerifyDataMarker()
{
	
	/*m_crFillColor = RGB(255, 255, 255);
	m_bFillRect=0;
	IUnknownPtr punkDoc = GetAppUnknown();
	if(GetValueInt( punkDoc, "EditMarker", "FillRect", m_bFillRect )>0)
		m_bFillRect=1;
	if(m_bLoadDataMarker && old_FillRect!=m_bFillRect)
		m_bFillRect=old_FillRect;
	if(GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor ))
		m_crFillColor=GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor );
	old_FillColor=m_crFillColor;
	if(GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision )>=0)
	{
		m_countComma=GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision );
		m_nPrecision=m_countComma;
	}
	else
		m_countComma=-1;
	old_Precision=m_nPrecision;
	old_countComma=m_countComma;
	
	m_bScaleMarker=0;
	if(GetValueInt( punkDoc, "EditMarker", "ScaleMarker", m_bScaleMarker )>0)
		m_bScaleMarker=1;
	strScale="1:1";
	old_ScaleMarker=m_bScaleMarker;

	if(GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" ))
		m_nameF = GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" );
	old_nameF=m_nameF;
	m_nFontHeight=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight ) > 0 )
		m_nFontHeight = GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight );
	old_FontHeight=m_nFontHeight;

	BYTE bIsFontItalic = 0;
	BYTE bIsFontUnderline = 0;
	m_resStyle=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Bold", m_resStyle ) )
		m_resStyle=m_resStyle|FontStyleBold;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Italic", bIsFontItalic) )
		m_resStyle=m_resStyle|FontStyleItalic;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Underline", bIsFontUnderline) )
		m_resStyle=m_resStyle|FontStyleUnderline;
	old_resStyle=m_resStyle;

	m_crTextColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor ))
		m_crTextColor = GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor );
	old_TextColor=m_crTextColor;
	m_crMarkerColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor ))
		m_crMarkerColor = GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor );
	old_MarkerColor=m_crMarkerColor;*/

	m_crFillColor = RGB(255, 255, 255);
	m_bFillRect=0;
	IUnknownPtr punkDoc = GetAppUnknown();
	if(GetValueInt( punkDoc, "EditMarker", "FillRect", m_bFillRect )>0)
		m_bFillRect=1;
	
	if(GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor ))
		m_crFillColor=GetValueColor( punkDoc, "EditMarker", "FillRect_Color", m_crFillColor );

	if(GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision )>=0)
	{
		m_countComma=GetValueInt( punkDoc, "EditMarker", "CountComma", m_nPrecision );
		m_nPrecision=m_countComma;
	}
	else
		m_countComma=-1;
	//добавим чтение кол-ва запятых после знака Accuracy. надо все пересмотреть с этими знаками.
	if(GetValueInt( punkDoc, "EditMarker", "Accuracy", m_nPrecision )>=0)
		m_nPrecision=GetValueInt( punkDoc, "EditMarker", "Accuracy", m_nPrecision );
	//и сформируем новую надпись с новым кол-ом знаков
	CString ts;
	ts.Format("%d", m_nPrecision);  
	ts = "%0."+ ts + "f %s";
	if(m_strUnitName=="")
	{
		m_strText.Format( ts, m_fLength, (const char*)m_initStrUnitName );
	}
	else
		m_strText.Format( ts, m_fLength, (const char*)m_strUnitName );
	m_bScaleMarker=0;
	if(GetValueInt( punkDoc, "EditMarker", "ScaleMarker", m_bScaleMarker )>0)
		m_bScaleMarker=1;
	strScale="1:1";

	if(GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" ))
		m_nameF = GetValueString( punkDoc, "EditMarker", "TextFont_Name", "Arial" );
	
	m_nFontHeight=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight ) > 0 )
		m_nFontHeight = GetValueInt( punkDoc, "EditMarker", "TextFont_Height", m_nFontHeight );

	BYTE bIsFontItalic = 0;
	BYTE bIsFontUnderline = 0;
	m_resStyle=0;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Bold", m_resStyle ) )
		m_resStyle=m_resStyle|FontStyleBold;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Italic", bIsFontItalic) )
		m_resStyle=m_resStyle|FontStyleItalic;
	if ( GetValueInt( punkDoc, "EditMarker", "TextFont_Underline", bIsFontUnderline) )
		m_resStyle=m_resStyle|FontStyleUnderline;

	m_crTextColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor ))
		m_crTextColor = GetValueColor( punkDoc, "EditMarker", "TextFont_Color", m_crTextColor );
	
	m_crMarkerColor = RGB(0, 0, 0);
	if(GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor ))
		m_crMarkerColor = GetValueColor( punkDoc, "EditMarker", "Marker_Color", m_crMarkerColor );
}
DWORD CMarker::GetNamedObjectFlags()
{
	return nofHasData|nofStoreByParent;
}

bool CMarker::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if( !CDataObjectBase::SerializeObject( ar, pparams ) )
		return false;

	// vk
	//long	lVersion = 1;
	//long	lVersion = 2;
	// end vk
	long	lVersion = 3;
	if( ar.IsLoading() )
	{
		ar>>lVersion>>m_x>>m_y>>m_fLength>>m_nPixelLength>>m_strText;
		if( lVersion >= 1 )	ar>>m_guidC>>m_fCalibr;
		else	::GetDefaultCalibration( &m_fCalibr, &m_guidC );
		if ( lVersion >= 2 ) ar>>m_fAdditionalWidth>>m_nFontHeight>>m_nPrecision>>m_fZoom;
		//if ( lVersion >= 2 ) ar>>m_nAdditionalWidth>>m_nFontHeight>>m_nPrecision>>m_fZoom;
		if ( lVersion >= 3 )
		{
			ar>>m_xEnd>>m_yEnd>>m_bFillRect>>m_crFillColor>>m_countComma>>SelectRect>>m_widthImage>>m_heightImage
				>>m_nameF>>m_resStyle>>m_crTextColor>>m_crMarkerColor>>m_strUnitName;
			if(m_countComma>=0)
				m_nPrecision=m_countComma;
		}
		else
		{
			m_xEnd=m_x+m_nPixelLength;
			m_yEnd=m_y;
		}
		//m_bLoadDataMarker=true;
	}
	else
	{
		// vk
		if(m_strUnitName=="")
			m_strUnitName=m_initStrUnitName;
		//ar<<lVersion<<m_x<<m_y<<m_fLength<<m_nPixelLength<<m_strText<<m_guidC<<m_fCalibr;
		ar<<lVersion<<m_x<<m_y<<m_fLength<<m_nPixelLength<<m_strText<<m_guidC<<m_fCalibr<<m_fAdditionalWidth
			<<m_nFontHeight<<m_nPrecision<<m_fZoom<<m_xEnd<<m_yEnd<<m_bFillRect<<m_crFillColor<<m_countComma<<SelectRect
			<<m_widthImage<<m_heightImage<<m_nameF<<m_resStyle<<m_crTextColor<<m_crMarkerColor<<m_strUnitName;
		//ar<<lVersion<<m_x<<m_y<<m_fLength<<m_nPixelLength<<m_strText<<m_guidC<<m_fCalibr<<m_nAdditionalWidth
		//	<<m_nFontHeight<<m_nPrecision<<m_fZoom;
		// end vk
	}

	return true;
}

void CMarker::DoDraw( CDC &dc )
{
	if(m_nPixelLength==0) 
		return; 
	#if defined(_USEGDIPLUS) 
	HDC hdc_=dc.GetSafeHdc(); 
	Graphics graphics(hdc_);
	
	dc.SetROP2(R2_COPYPEN); 

	bool bPrint=false;
	if( GetDeviceCaps( hdc_, TECHNOLOGY ) == DT_RASPRINTER )
	{
		graphics.SetPageUnit(UnitPixel);
		bPrint = true;
	}
	
	m_fZoom = ::GetZoom( m_punkDrawTarget );   
	
 	CPoint	pointScroll = ::GetScrollPos( m_punkDrawTarget ); 

	VerifyDataMarker();
	// vk
	int	cx = m_nPixelLength*m_fZoom;
	int	cy = cyMarker*m_fZoom;
	// end vk

	LPCTSTR	FontName = "Arial";
	if(m_nFontHeight==0)
		m_nFontHeight = 2*cyMarker; 
	
	CalcScale(dc, bPrint);

	IUnknownPtr punkDoc = GetAppUnknown();

	CString	strToDraw  = m_strText;
	bool	bUseGreek = false;

	const	char *p1 = strToDraw;
	const char *p = strstr( p1, "mu" );
	if( p )
	{
		bUseGreek = true;
		strToDraw = strToDraw.Left( (long)p-(long)p1 );
		strToDraw+="m";
	}

	// Initialize arguments.
	if(strlen(m_nameF)) 
		FontName=m_nameF;
	if( bUseGreek )
		FontName="Symbol";

	USES_CONVERSION;
	LPWSTR pwsText = T2W(FontName);
	LPWSTR string = T2W(strToDraw);
	LPWSTR stringScale = T2W(strScale);

	FontFamily fontFamily(pwsText);
	Unit unitFont=UnitPixel;  
	//if(bPrint) 
	//	unitFont=UnitPoint;
	
	Gdiplus::Font myFont(&fontFamily, (int)(m_nFontHeight*m_fZoom), m_resStyle, unitFont); 
	Gdiplus::Font blackFont(&fontFamily, (int)(m_nFontHeight*m_fZoom), m_resStyle, unitFont);

	//PointF origin(0, -m_nFontHeight*m_fZoom-2*cy);
	//PointF originScale(0, cy);

	int wx=cy;
	//Get width string
	SizeF layoutSize(20000, 20000);
	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	SizeF stringSize, stringSM;
	//Measure the string.
	graphics.MeasureString(string, wcslen(string), &blackFont, layoutSize, &format, &stringSize);
	if(m_bScaleMarker)
	{
		graphics.MeasureString(stringScale, wcslen(stringScale), &blackFont, layoutSize, &format, &stringSM);
		if(stringSM.Width>stringSize.Width)
			stringSize=stringSM;
	}
	RectF layoutRect;
 	if(stringSize.Width<cx)
	{
		if(m_bFillRect)
		{
			layoutRect.X=-cx/2-cy;
			layoutRect.Width=cx+2*cy;
		}
		else
		{
			layoutRect.X=-cx/2;
			layoutRect.Width=cx;
			wx=1;
		}
		layoutRect.Y=-3*cy-m_nFontHeight*m_fZoom;
		if(m_bScaleMarker)
			layoutRect.Height=2*(3*cy+m_nFontHeight*m_fZoom);
		else
			layoutRect.Height=5*cy+m_nFontHeight*m_fZoom;
	}
	else
	{
		layoutRect.X=-stringSize.Width/2;
		layoutRect.Y=-3*cy-m_nFontHeight*m_fZoom;
		layoutRect.Width=stringSize.Width;
		if(m_bScaleMarker)
			layoutRect.Height=2*(3*cy+m_nFontHeight*m_fZoom);
		else
			layoutRect.Height=5*cy+m_nFontHeight*m_fZoom;
		wx=(layoutRect.Width-cx)/2;
	}

	REAL size = myFont.GetSize(); 
	LOGFONTW logFont; 
	myFont.GetLogFontW(&graphics, &logFont); 
 
 	PointF origin(0, /*layoutRect.Y-2*cy*/layoutRect.Y-(layoutRect.Y-logFont.lfHeight-logFont.lfHeight/3)/2);   
	PointF originScale(0, cy); 

	if(m_xEnd<m_x)
	{
		int i=m_x;
		m_x=m_xEnd;
		m_xEnd=i;
		i=m_y;
		m_y=m_yEnd;
		m_yEnd=i;
	}

	CPoint pt;
	pt.x=m_xEnd;//*m_fZoom-pointScroll.x;
	pt.y=m_yEnd;//*m_fZoom-pointScroll.y;
	int	x = m_x;//*m_fZoom-pointScroll.x;
	int	y = m_y;//*m_fZoom-pointScroll.y;
	
	double sinA=(pt.y-y)/(sqrt(double((pt.x-x)*(pt.x-x)+(pt.y-y)*(pt.y-y))));
	double cosA=(pt.x-x)/(sqrt(double((pt.x-x)*(pt.x-x)+(pt.y-y)*(pt.y-y))));
	double angle=atan2(double(pt.x-x), double(-pt.y+y))*(180/3.14159265358979323846)+90;

 	CPoint findPoint[4],minPoint,maxPoint;
	
	findPoint[0].x=(int)(m_x-wx/m_fZoom*cosA-(-3*5-m_nFontHeight)*sinA);
	findPoint[0].y=(int)(m_y-wx/m_fZoom*sinA+(-3*5-m_nFontHeight)*cosA);
	if(m_bScaleMarker)
	{
		findPoint[1].x=(int)(m_x-wx/m_fZoom*cosA-(3*5+m_nFontHeight)*sinA); 
		findPoint[1].y=(int)(m_y-wx/m_fZoom*sinA+(3*5+m_nFontHeight)*cosA);
	}
	else
	{
		findPoint[1].x=(int)(m_x-wx/m_fZoom*cosA-(2*5)*sinA); 
		findPoint[1].y=(int)(m_y-wx/m_fZoom*sinA+(2*5)*cosA);
	}

	findPoint[2].x=(int)(m_x+(m_nPixelLength+wx/m_fZoom)*cosA-(-3*5-m_nFontHeight)*sinA); 
	findPoint[2].y=(int)(m_y+(m_nPixelLength+wx/m_fZoom)*sinA+(-3*5-m_nFontHeight)*cosA);
	if(m_bScaleMarker) 
	{
		findPoint[3].x=(int)(m_x+(m_nPixelLength+wx/m_fZoom)*cosA-(3*5+m_nFontHeight)*sinA); 
		findPoint[3].y=(int)(m_y+(m_nPixelLength+wx/m_fZoom)*sinA+(3*5+m_nFontHeight)*cosA); 
	}
	else
	{
		findPoint[3].x=(int)(m_x+(m_nPixelLength+wx/m_fZoom)*cosA-(2*5)*sinA); 
		findPoint[3].y=(int)(m_y+(m_nPixelLength+wx/m_fZoom)*sinA+(2*5)*cosA);
	}
  	
	minPoint=maxPoint=findPoint[0];
	for(int i=1;i<4;i++)
	{
		if(findPoint[i].x>maxPoint.x)
			maxPoint.x=findPoint[i].x;
		if(findPoint[i].x<minPoint.x)
			minPoint.x=findPoint[i].x;
		if(findPoint[i].y>maxPoint.y)
			maxPoint.y=findPoint[i].y;
		if(findPoint[i].y<minPoint.y)
			minPoint.y=findPoint[i].y;
	}
 	SelectRect.left=minPoint.x;    
	SelectRect.top=minPoint.y; 
	SelectRect.right=maxPoint.x;
	SelectRect.bottom=maxPoint.y;
	
//Проверим, чтобы маркер не вылезал за пределы текущей картинки
	CRect res=SelectRect;   
	int temp_x=m_x, temp_xEnd=m_xEnd, temp_y=m_y, temp_yEnd=m_yEnd; 
 	if(SelectRect.left<0) 
	{
		SelectRect.left=1;
		SelectRect.right=SelectRect.left+res.Width();
		temp_x+=SelectRect.left-res.left;
		temp_xEnd+=SelectRect.left-res.left;
	}
	else
		if(SelectRect.right>m_widthImage) 
		{
			SelectRect.right=m_widthImage-1;
			SelectRect.left=SelectRect.right-res.Width();
			temp_x+=SelectRect.left-res.left;
			temp_xEnd+=SelectRect.left-res.left;
		}
	if(SelectRect.top<0)
	{
		SelectRect.top=1; 
		SelectRect.bottom=SelectRect.top+res.Height();
		temp_y+=SelectRect.top-res.top;
		temp_yEnd+=SelectRect.top-res.top;
	}
	else
		if(SelectRect.bottom>m_heightImage)
		{
			SelectRect.bottom=m_heightImage-1;
			SelectRect.top=SelectRect.bottom-res.Height();
			temp_y+=SelectRect.top-res.top;
			temp_yEnd+=SelectRect.top-res.top;
		}
 	pt.x=temp_xEnd*m_fZoom-pointScroll.x;    
	pt.y=temp_yEnd*m_fZoom-pointScroll.y;
	x = temp_x*m_fZoom-pointScroll.x;
	y = temp_y*m_fZoom-pointScroll.y;

	format.SetAlignment(StringAlignmentCenter); 

	Color gdipColor(255, 255, 0, 0);
	gdipColor.SetFromCOLORREF(m_crTextColor);

	SolidBrush colorBrush(gdipColor);
	SolidBrush blackBrush(Color(0,0,0));
		
 	if(angle>=90 && angle<270)   
		graphics.RotateTransform(angle-180, MatrixOrderAppend);  
	else
		graphics.RotateTransform(angle, MatrixOrderAppend);  
	
	graphics.TranslateTransform(x+(pt.x-x)/2, y+(pt.y-y)/2, MatrixOrderAppend);
	
	// Draw two lines using each pen.
	graphics.SetSmoothingMode(SmoothingModeHighQuality); 

	// Draw layoutRect.
	if(m_bFillRect>0)
	{
		gdipColor.SetFromCOLORREF(m_crFillColor);
		SolidBrush rectBrush(gdipColor);
		graphics.FillRectangle(&rectBrush, layoutRect);
	}
	// Draw lines
	gdipColor.SetFromCOLORREF(m_crMarkerColor); 
	Pen blackPen(Color(255, 0, 0, 0), 2.5);  
	Pen whitePen(gdipColor, 1.5); 
	// Set the alignment of the green pen.
	blackPen.SetAlignment(PenAlignmentCenter);
 	graphics.DrawLine(&blackPen, -cx/2, -cy, -cx/2, cy);
	graphics.DrawLine(&blackPen, -cx/2, 0, cx/2, 0);
	graphics.DrawLine(&blackPen, cx/2, -cy, cx/2, cy);
	graphics.DrawLine(&whitePen, -cx/2, -cy, -cx/2, cy);
	graphics.DrawLine(&whitePen, -cx/2, 0, cx/2, 0);
	graphics.DrawLine(&whitePen, cx/2, -cy, cx/2, cy);

	// Draw string.
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias); 
	/*origin.X-=1;  
	origin.Y-=1;
	graphics.DrawString(string,	wcslen(string), &blackFont, origin, &format, &blackBrush);
	origin.X+=0;
	origin.Y+=2;
	graphics.DrawString(string,	wcslen(string), &blackFont, origin, &format, &blackBrush);
	origin.X+=2;
	origin.Y+=0;
	graphics.DrawString(string,	wcslen(string), &blackFont, origin, &format, &blackBrush);
	origin.X+=0;
	origin.Y-=2;
	graphics.DrawString(string,	wcslen(string), &blackFont, origin, &format, &blackBrush);
	origin.X-=1;
	origin.Y+=1;*/
	graphics.DrawString(string,	wcslen(string), &myFont, origin, &format, &colorBrush);

 	if(m_bScaleMarker)
	{
		/*originScale.X-=1;
		originScale.Y-=1;
		graphics.DrawString(stringScale, wcslen(stringScale), &blackFont, originScale, &format, &blackBrush);
		originScale.X+=0;
		originScale.Y+=2;
		graphics.DrawString(stringScale, wcslen(stringScale), &blackFont, originScale, &format, &blackBrush);
		originScale.X+=2;
		originScale.Y+=0;
		graphics.DrawString(stringScale, wcslen(stringScale), &blackFont, originScale, &format, &blackBrush);
		originScale.X+=0;
		originScale.Y-=2;
		graphics.DrawString(stringScale, wcslen(stringScale), &blackFont, originScale, &format, &blackBrush);
		originScale.X-=1;
		originScale.Y+=1;*/
		graphics.DrawString(stringScale, wcslen(stringScale), &myFont, originScale, &format, &colorBrush);
	}
#endif
}


HRESULT CMarker::XMark::SetCoordsLen( int x, int y, int nLength )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	CString	strUnitName = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	double	fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );

	pThis->m_x = x;
	pThis->m_y = y;
	pThis->m_nPixelLength = max( nLength, 0 );
	pThis->m_fLength = pThis->m_nPixelLength*pThis->m_fCalibr*fMeterPerUnit;

	// vk	
	//pThis->m_strText.Format( "%0.2f %s", pThis->m_fLength, (const char*)strUnitName );
	CString ts;
	ts.Format("%d", pThis->m_nPrecision);  
	ts = "%0."+ ts + "f %s";
	pThis->m_strText.Format( ts, pThis->m_fLength, (const char*)strUnitName );
	// end vk

	return S_OK;
}

HRESULT CMarker::XMark::SetLength( double fLength )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	CString	strUnitName = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	double	fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );

	pThis->m_fLength = fLength;
	pThis->m_nPixelLength = pThis->m_fLength/pThis->m_fCalibr/fMeterPerUnit;
	if(pThis->m_nPixelLength <= 0)	
		pThis->m_nPixelLength=1;
	// vk	
	//pThis->m_strText.Format( "%0.2f %s", pThis->m_fLength, (const char*)strUnitName );
	CString ts;
	ts.Format("%d", pThis->m_nPrecision);  
	ts = "%0."+ ts + "f %s";
	pThis->m_strText.Format( ts, pThis->m_fLength, (const char*)strUnitName );
	// end vk

	return S_OK;
}

// vk
HRESULT CMarker::XMark::SetCoordsLenPrec( int x, int y, int nLength, int nPrecision )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	CString	strUnitName = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	double	fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );

	pThis->m_x = x;
	pThis->m_y = y;
	pThis->m_nPixelLength = max( nLength, 0 );
	pThis->m_fLength = pThis->m_nPixelLength*pThis->m_fCalibr*fMeterPerUnit;
	// vk
	if (nPrecision >= 0)
	{
		pThis->m_countComma = pThis->m_nPrecision = nPrecision;
	}
	// end vk

	// vk	
	CString ts;
	ts.Format("%d", pThis->m_nPrecision);  
	ts = "%0."+ ts + "f %s";
	pThis->m_strText.Format( ts, pThis->m_fLength, (const char*)strUnitName );
	// end vk

	return S_OK;
}

HRESULT CMarker::XMark::SetCoordsEnd( int x, int y, bool bInit )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	if(bInit)
		pThis->m_xEnd=x+pThis->m_nPixelLength;
	else
		pThis->m_xEnd=x;
	pThis->m_yEnd=y;
	return S_OK;
}


HRESULT CMarker::XMark::SetBorderMarker(int width, int height)
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	pThis->m_widthImage=width;
	pThis->m_heightImage=height;
	return S_OK;
}

HRESULT CMarker::XMark::GetMaxLength( int nWidth, double *pnLength )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	double	fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
	*pnLength = nWidth*pThis->m_fCalibr*fMeterPerUnit;

	return S_OK;
}

HRESULT CMarker::XMark::GetCaptionRect( CRect *prc )
{
	METHOD_PROLOGUE_EX(CMarker, Mark);

	prc->left = pThis->m_x-1-((floor(pThis->m_fAdditionalWidth)+3)/2);
	prc->right = pThis->m_x+pThis->m_nPixelLength+1+((floor(pThis->m_fAdditionalWidth)+3)/2);
	prc->top = pThis->m_y-1-pThis->m_nFontHeight;
	prc->bottom = pThis->m_y;

	return S_OK;
}

// end vk

HRESULT CMarker::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CMarker, Rect);

	//pThis->m_x = rc.left+1;
	//pThis->m_y = rc.top+1+cyMarker;

	pThis->SelectRect=rc;

	return S_OK;
}

HRESULT CMarker::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CMarker, Rect);

	prc->left = pThis->SelectRect.left;
	prc->right = pThis->SelectRect.right;
	prc->top=pThis->SelectRect.top;
	prc->bottom=pThis->SelectRect.bottom;


	return S_OK;
}

HRESULT CMarker::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CMarker, Rect);

	CRect	rect;
	GetRect( &rect );
	*plHitTest = rect.PtInRect( point );

	return S_OK;
}

HRESULT CMarker::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CMarker, Rect);

	CRect oldRect=pThis->SelectRect;
	pThis->SelectRect.top=point.y;
	pThis->SelectRect.bottom+=(pThis->SelectRect.top-oldRect.top);
	pThis->m_y+=(pThis->SelectRect.top-oldRect.top);
	pThis->m_yEnd+=(pThis->SelectRect.top-oldRect.top);

	pThis->SelectRect.left=point.x;
	pThis->SelectRect.right+=(pThis->SelectRect.left-oldRect.left);
	pThis->m_x+=(pThis->SelectRect.left-oldRect.left);
	pThis->m_xEnd+=(pThis->SelectRect.left-oldRect.left);

	return S_OK;
}



HRESULT CMarker::XCalibr::GetCalibration( double *pfPixelPerMeter, GUID *pguidC )
{
	METHOD_PROLOGUE_EX(CMarker, Calibr);

	*pfPixelPerMeter = pThis->m_fCalibr;
	*pguidC = pThis->m_guidC;

	return S_OK;
}
HRESULT CMarker::XCalibr::SetCalibration( double fPixelPerMeter, GUID *guidC )
{
	METHOD_PROLOGUE_EX(CMarker, Calibr);

	pThis->m_fCalibr = fPixelPerMeter;
	pThis->m_guidC = *guidC;

	return S_OK;
}



///////////////////////////////////////////Arrow class
CArrow::CArrow()
{
	m_pointStart = CPoint( 0, 0 );
	m_pointEnd = CPoint( 0, 0 );
	m_nArrowSize = 20;
	m_dwFlags = afFirstArrow|afLastArrow;
	m_cr = RGB( 0, 255, 255 );
	m_nWidth = 3;
}

CArrow::~CArrow()
{
}


BEGIN_INTERFACE_MAP(CArrow, CDataObjectBase)
	INTERFACE_PART(CArrow, IID_IDrawObject, Draw)
	INTERFACE_PART(CArrow, IID_IArrow, Arrow)
	INTERFACE_PART(CArrow, IID_IRectObject, Rect)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CArrow, Arrow)
IMPLEMENT_UNKNOWN(CArrow, Rect)


HRESULT CArrow::XArrow::SetCoords( POINT pointStart, POINT pointEnd )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	memcpy( &pThis->m_pointStart, &pointStart, sizeof( POINT ) );
	memcpy( &pThis->m_pointEnd, &pointEnd, sizeof( POINT ) );
	return S_OK;
}
HRESULT CArrow::XArrow::GetCoords( POINT *ppointStart, POINT *ppointEnd )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	memcpy( ppointStart, &pThis->m_pointStart, sizeof( POINT ) );
	memcpy( ppointEnd, &pThis->m_pointEnd, sizeof( POINT ) );
	return S_OK;
}
HRESULT CArrow::XArrow::SetDrawFlags( DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	pThis->m_dwFlags = dwFlags;
	return S_OK;
}
HRESULT CArrow::XArrow::GetDrawFlags( DWORD *pdwFlags )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	*pdwFlags = pThis->m_dwFlags;
	return S_OK;
}
HRESULT CArrow::XArrow::SetColor( COLORREF	cr )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	pThis->m_cr = cr;
	return S_OK;
}
HRESULT CArrow::XArrow::GetColor( COLORREF *pcr )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	*pcr = pThis->m_cr;
	return S_OK;
}
HRESULT CArrow::XArrow::SetSize( int nSize, int nWidth )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	pThis->m_nArrowSize = nSize;
	pThis->m_nWidth = nWidth;
	return S_OK;
}
HRESULT CArrow::XArrow::GetSize( int *pnSize, int *pnWidth )
{
	METHOD_PROLOGUE_EX(CArrow, Arrow);
	*pnSize = pThis->m_nArrowSize;
	*pnWidth = pThis->m_nWidth;
	return S_OK;
}

HRESULT CArrow::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CArrow, Rect);

	if( pThis->m_pointStart.x < pThis->m_pointEnd.x )
	{
		pThis->m_pointStart.x = rc.left+pThis->m_nArrowSize+pThis->m_nWidth/2;
		pThis->m_pointEnd.x = rc.right-pThis->m_nArrowSize-pThis->m_nWidth/2;
	}
	else
	{
		pThis->m_pointEnd.x = rc.left+pThis->m_nArrowSize+pThis->m_nWidth/2;
		pThis->m_pointStart.x = rc.right-pThis->m_nArrowSize-pThis->m_nWidth/2;
	}

	if( pThis->m_pointStart.y < pThis->m_pointEnd.y )
	{
		pThis->m_pointStart.y = rc.top+pThis->m_nArrowSize+pThis->m_nWidth/2;
		pThis->m_pointEnd.y = rc.bottom-pThis->m_nArrowSize-pThis->m_nWidth/2;
	}
	else
	{
		pThis->m_pointEnd.y = rc.top+pThis->m_nArrowSize+pThis->m_nWidth/2;
		pThis->m_pointStart.y = rc.bottom-pThis->m_nArrowSize-pThis->m_nWidth/2;
	}
	
	return S_OK;
}
HRESULT CArrow::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CArrow, Rect);

	prc->left = min( pThis->m_pointStart.x, pThis->m_pointEnd.x )-pThis->m_nArrowSize-pThis->m_nWidth/2;
	prc->right = max( pThis->m_pointStart.x, pThis->m_pointEnd.x )+pThis->m_nArrowSize+pThis->m_nWidth/2;
	prc->top = min( pThis->m_pointStart.y, pThis->m_pointEnd.y )-pThis->m_nArrowSize-pThis->m_nWidth/2;
	prc->bottom = max( pThis->m_pointStart.y, pThis->m_pointEnd.y )+pThis->m_nArrowSize+pThis->m_nWidth/2;
	return S_OK;
}

HRESULT CArrow::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CArrow, Rect);

	CRect	rect;
	GetRect( &rect );
	*plHitTest = rect.PtInRect( point );

	return S_OK;
}

HRESULT CArrow::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CArrow, Rect);

	CRect	rect;
	GetRect( &rect );

	rect.right = rect.Width()+point.x;
	rect.bottom = rect.Height()+point.y;
	rect.left = point.x;
	rect.top = point.y;

	SetRect( rect );

	return S_OK;
}


void CArrow::DoDraw( CDC &dc )
{
	if( m_pointStart == m_pointEnd )return;

	CPen	pen( PS_SOLID, 0, m_cr );
	CPen	penLine( PS_SOLID, m_nWidth, m_cr );
	CBrush	brush( m_cr );

	dc.SetROP2( R2_COPYPEN );
	CPen	*pold = 
	dc.SelectObject( &pen );
	CBrush	*poldB = 
	dc.SelectObject( &brush );

	CPoint	ptStart = ::ConvertToWindow( m_punkDrawTarget, m_pointStart );
	CPoint	ptEnd = ::ConvertToWindow( m_punkDrawTarget, m_pointEnd );

	//dc.MoveTo( m_pointStart );
	//dc.LineTo( m_pointEnd );

	double	 fZoom = ::GetZoom( m_punkDrawTarget );
	CPoint	ptScroll = ::GetScrollPos( m_punkDrawTarget );


	if( m_dwFlags & afFirstArrow )
	{
		double	fAngle = ::atan2((double)m_pointStart.x-m_pointEnd.x,  (double)m_pointStart.y-m_pointEnd.y );
		double	fAngle1 = fAngle+15./180*3.14;
		double	fAngle2 = fAngle-15./180*3.14;

		POINT	points[4];
		points[0].x = int( fZoom*(m_pointStart.x )-ptScroll.x );
		points[0].y = int( fZoom*(m_pointStart.y )-ptScroll.y );
		points[1].x = int( fZoom*(m_pointStart.x-sin( fAngle1 )*m_nArrowSize )-ptScroll.x );
		points[1].y = int( fZoom*(m_pointStart.y-cos( fAngle1 )*m_nArrowSize )-ptScroll.y );
		points[2].x = int( fZoom*(m_pointStart.x-sin( fAngle )*m_nArrowSize/2 )-ptScroll.x );
		points[2].y = int( fZoom*(m_pointStart.y-cos( fAngle )*m_nArrowSize/2 )-ptScroll.y );
		points[3].x = int( fZoom*(m_pointStart.x-sin( fAngle2 )*m_nArrowSize )-ptScroll.x );
		points[3].y = int( fZoom*(m_pointStart.y-cos( fAngle2 )*m_nArrowSize )-ptScroll.y );

		ptStart = points[2];

		dc.SelectObject( &pen );
		dc.SelectObject( &brush );
		dc.Polygon( points, 4 );
	}

	if( m_dwFlags & afFirstCircle )
	{
		dc.SelectStockObject( NULL_BRUSH );
		dc.SelectObject( &penLine );
		dc.Ellipse( 
			int( fZoom * (m_pointStart.x - m_nArrowSize)-ptScroll.x ),
			int( fZoom * (m_pointStart.y - m_nArrowSize)-ptScroll.y ),
			int( fZoom * (m_pointStart.x + m_nArrowSize)-ptScroll.x ),
			int( fZoom * (m_pointStart.y + m_nArrowSize)-ptScroll.y ) );
		double	fAngle = ::atan2( double(m_pointStart.y-m_pointEnd.y), double(m_pointStart.x-m_pointEnd.x) );
		ptStart.x = int( fZoom*( m_pointStart.x-::cos( fAngle )*m_nArrowSize )-ptScroll.x );
		ptStart.y = int( fZoom*( m_pointStart.y-::sin( fAngle )*m_nArrowSize )-ptScroll.y );
	}	

	if( m_dwFlags & afLastArrow )
	{
		double	fAngle = ::atan2( double(m_pointEnd.x-m_pointStart.x), double(m_pointEnd.y-m_pointStart.y) );
		double	fAngle1 = fAngle+15./180*3.14;
		double	fAngle2 = fAngle-15./180*3.14;

		POINT	points[4];
		points[0].x = int( fZoom*(m_pointEnd.x )-ptScroll.x );
		points[0].y = int( fZoom*(m_pointEnd.y )-ptScroll.y );
		points[1].x = int( fZoom*(m_pointEnd.x-sin( fAngle1 )*m_nArrowSize )-ptScroll.x );
		points[1].y = int( fZoom*(m_pointEnd.y-cos( fAngle1 )*m_nArrowSize )-ptScroll.y );
		points[2].x = int( fZoom*(m_pointEnd.x-sin( fAngle )*m_nArrowSize/2 )-ptScroll.x );
		points[2].y = int( fZoom*(m_pointEnd.y-cos( fAngle )*m_nArrowSize/2 )-ptScroll.y );
		points[3].x = int( fZoom*(m_pointEnd.x-sin( fAngle2 )*m_nArrowSize )-ptScroll.x );
		points[3].y = int( fZoom*(m_pointEnd.y-cos( fAngle2 )*m_nArrowSize )-ptScroll.y );

		ptEnd = points[2];

		dc.SelectObject( &pen );
		dc.SelectObject( &brush );
		dc.Polygon( points, 4 );
	}
	if( m_dwFlags & afLastCircle )
	{
		dc.SelectStockObject( NULL_BRUSH );
		dc.SelectObject( &penLine );
		dc.Ellipse( 
			int( fZoom * (m_pointEnd.x - m_nArrowSize)-ptScroll.x ),
			int( fZoom * (m_pointEnd.y - m_nArrowSize)-ptScroll.y ),
			int( fZoom * (m_pointEnd.x + m_nArrowSize)-ptScroll.x ),
			int( fZoom * (m_pointEnd.y + m_nArrowSize)-ptScroll.y ) );
		double	fAngle = ::atan2( double(m_pointEnd.y-m_pointStart.y), double(m_pointEnd.x-m_pointStart.x) );
		ptEnd.x = int( fZoom*( m_pointEnd.x-::cos( fAngle )*m_nArrowSize )-ptScroll.x );
		ptEnd.y = int( fZoom*( m_pointEnd.y-::sin( fAngle )*m_nArrowSize )-ptScroll.y );
	}

	dc.SelectObject( &penLine );

	dc.MoveTo( ptStart );
	dc.LineTo( ptEnd );

	dc.SelectObject( pold );
	dc.SelectObject( poldB );
}

DWORD CArrow::GetNamedObjectFlags()
{return nofHasData|nofStoreByParent;}

bool CArrow::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if( !CDataObjectBase::SerializeObject( ar, pparams ) )
		return false;

	long	lVer = 1;
	if( ar.IsLoading() )
	{
		ar>>lVer;
		ar>>m_pointStart.x>>m_pointStart.y>>m_pointEnd.x>>m_pointEnd.y>>(long&)m_dwFlags>>m_nArrowSize>>(long&)m_cr>>(long&)m_nWidth;
	}
	else
	{
		ar<<lVer;
		ar<<m_pointStart.x<<m_pointStart.y<<m_pointEnd.x<<m_pointEnd.y<<(long)m_dwFlags<<m_nArrowSize<<(long)m_cr<<(long)m_nWidth;
	}
	return true;
}


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionEditCircleArrow, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditArrow, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditMarker, CCmdTargetEx);

// olecreate 

// {7BF979AC-C82E-4f56-B746-A6450EFEF7BF}
GUARD_IMPLEMENT_OLECREATE(CActionEditCircleArrow, "Editor.EditCircleArrow",
0x7bf979ac, 0xc82e, 0x4f56, 0xb7, 0x46, 0xa6, 0x45, 0xe, 0xfe, 0xf7, 0xbf);
// {FAE0172B-E93E-412b-B8CB-34F793337D86}
GUARD_IMPLEMENT_OLECREATE(CActionEditArrow, "Editor.EditArrow",
0xfae0172b, 0xe93e, 0x412b, 0xb8, 0xcb, 0x34, 0xf7, 0x93, 0x33, 0x7d, 0x86);
// {26AD36F5-E6B6-44fc-8485-4D7D51CB93CB}
GUARD_IMPLEMENT_OLECREATE(CActionEditMarker, "Editor.EditMarker",
0x26ad36f5, 0xe6b6, 0x44fc, 0x84, 0x85, 0x4d, 0x7d, 0x51, 0xcb, 0x93, 0xcb);
// guidinfo 

// {B447B840-2201-4836-AFDD-FA89C532A70A}
static const GUID guidEditCircleArrow =
{ 0xb447b840, 0x2201, 0x4836, { 0xaf, 0xdd, 0xfa, 0x89, 0xc5, 0x32, 0xa7, 0xa } };
// {FF2D1EEA-A79B-4ab3-8298-AAEDDFBF4DF8}
static const GUID guidEditArrow =
{ 0xff2d1eea, 0xa79b, 0x4ab3, { 0x82, 0x98, 0xaa, 0xed, 0xdf, 0xbf, 0x4d, 0xf8 } };
// {0B7E6FCF-5A91-44f0-B811-E4325E32DC20}
static const GUID guidEditMarker =
{ 0xb7e6fcf, 0x5a91, 0x44f0, { 0xb8, 0x11, 0xe4, 0x32, 0x5e, 0x32, 0xdc, 0x20 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionEditCircleArrow, ID_ACTION_EDIT_CIRCLEARROW, -1, -1, guidEditCircleArrow);
ACTION_INFO_FULL(CActionEditArrow, ID_ACTION_EDIT_ARROW, -1, -1, guidEditArrow);
ACTION_INFO_FULL(CActionEditMarker, ID_ACTION_EDIT_MERKER, -1, -1, guidEditMarker);
//[ag]7. targets

ACTION_TARGET(CActionEditCircleArrow, szTargetViewSite);
ACTION_TARGET(CActionEditArrow, szTargetViewSite);
ACTION_TARGET(CActionEditMarker, szTargetViewSite);

ACTION_ARG_LIST(CActionEditMarker)
	ARG_INT(_T("X"), 0)
	ARG_INT(_T("Y"), 0)
	ARG_DOUBLE(_T("Length"), 0)
	// vk
	ARG_INT(_T("Precision"), 1)
	// end vk
END_ACTION_ARG_LIST()

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionEditArrow implementation
BEGIN_INTERFACE_MAP(CActionEditArrow, CInteractiveActionBase)
	INTERFACE_PART(CActionEditArrow, IID_IObjectAction, DrAction)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActionEditArrow, DrAction)

HRESULT CActionEditArrow::XDrAction::GetFirstObjectPosition(LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditArrow, DrAction);
	(*plpos) = 1;
	return S_OK;
}
HRESULT CActionEditArrow::XDrAction::GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditArrow, DrAction);
	*ppunkObject = pThis->m_ptrArrow;
	if( *ppunkObject )(*ppunkObject)->AddRef();
	(*plpos) = 0;
	return S_OK;
}

HRESULT CActionEditArrow::XDrAction::AddObject( IUnknown *punkObject )
{
	METHOD_PROLOGUE_EX(CActionEditArrow, DrAction);
	pThis->m_ptrArrow = punkObject;
	pThis->m_ptrArrowDraw = punkObject;
	pThis->m_ptrArrowRect = punkObject;
	pThis->m_state = asReady;
	return S_OK;
}

CActionEditArrow::CActionEditArrow()
{
	m_dwArrowFlags = afLastArrow;
	m_nWidth = 3;
	m_nSize = 20;
}

CActionEditArrow::~CActionEditArrow()
{
}

bool CActionEditArrow::Invoke()
{
	if( m_ptrArrow == 0  )
		return false;
	
	IViewPtr	ptrV( m_punkTarget );
	IViewDataPtr ptr_view_data( ptrV );

	if( ptr_view_data )
	{
		IUnknown* punk_draw_list = 0;
		ptr_view_data->GetObject( &punk_draw_list, _bstr_t( szDrawingObjectList ) );
		if( !punk_draw_list )
			return false;

		INamedDataObject2Ptr	ptrN( (IUnknown*)m_ptrArrow );
		ptrN->SetParent( punk_draw_list, 0 );

		punk_draw_list->Release();	punk_draw_list = 0;

	}
	else
	{
		IUnknown	*punkDoc = 0;
		ptrV->GetDocument( &punkDoc );

		ASSERT( punkDoc );
		IUnknownPtr	ptrDoc = punkDoc;
		punkDoc->Release();


		IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szDrawingObjectList );
		if( !punk )
		{
			punk = ::CreateTypedObject( szDrawingObjectList );
			if( !punk  )
			{
				AfxMessageBox( IDS_ERR_CANTCREATEOBJECTLIST );
				return false;
			}
			IUnknown	*punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
			if( punkImage )
			{
				SetBaseObject( punk, punkImage, ptrDoc );
				punkImage->Release();
			}
			m_changes.SetToDocData( ptrDoc, punk );	

			//???activate
			_bstr_t	bstr( ::GetObjectKind( punk ) );
			IDataContextPtr	ptrC( m_punkTarget );
			ptrC->SetActiveObject( bstr, punk, 0 );

			

		}
		{
			INamedDataObject2Ptr	ptrN( (IUnknown*)m_ptrArrow );
			ptrN->SetParent( punk, 0 );
		}
		punk->Release();

		m_changes.SetToDocData( ptrDoc, m_ptrArrow );

		m_lTargetKey = ::GetObjectKey( ptrDoc );
	}

	return true;
}

//undo/redo
bool CActionEditArrow::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionEditArrow::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionEditArrow::Initialize()
{
	AddPropertyPageDescription( c_szCArrowPage );

	if( !CInteractiveActionBase::Initialize() )return false;

	DoUpdateSettings();

/*	int	x = GetArgumentInt( "X" );
	int	y = GetArgumentInt( "Y" );
	int	l = GetArgumentInt( "Length" );*/


	IUnknown	*punkDataObject = ::CreateTypedObject( szTypeArrow );
	if( !punkDataObject )
	{
		LeaveMode();
		return false;
	}
	m_ptrArrow = punkDataObject;
	m_ptrArrowDraw = punkDataObject;
	m_ptrArrowRect = punkDataObject;
	m_ptrArrow->SetDrawFlags( m_dwArrowFlags );
	m_ptrArrow->SetSize( m_nSize, m_nWidth );
	m_ptrArrow->SetColor( ::GetValueColor( GetAppUnknown(), "\\Editor", "Fore", RGB( 255, 0, 0 ) ) );
	punkDataObject->Release();

	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITARROW);

	/*if( l != 0 )
	{
		m_ptrMarker->SetCoordsLen( x, y, l, 0 );
		Finalize();
	}*/
	return true;
}

bool CActionEditArrow::DoStartTracking( CPoint pt )
{
	m_pointWhere = pt;
	m_ptrArrow->SetCoords( m_pointWhere, pt );
	m_ptrArrow->SetColor( ::GetValueColor( GetAppUnknown(), "\\Editor", "Fore", RGB( 255, 0, 0 ) ) );
	Invalidate();

	return true;
}

bool CActionEditArrow::DoLButtonUp( CPoint pt )
{
	if( GetState() == asReady )
		return true;

	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szDrawingObjectList );
	if( !punk )return true;

	INamedDataObject2Ptr	ptrO( punk );
	punk->Release();

	POSITION	lpos = 0, lposActive = 0;
	ptrO->GetActiveChild( &lpos );
	lposActive = lpos;
	if( lpos )ptrO->GetNextChild( &lpos, 0 );
	else ptrO->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		POSITION	lposSave = lpos;
		IUnknown	*punk = 0;
		ptrO->GetNextChild( &lpos, &punk );
		if( !punk )continue;
		IRectObjectPtr	ptrRect( punk );
		punk->Release();

		if( ptrRect == 0 )continue;

		CRect	rect;
		ptrRect->GetRect( &rect );

		if( rect.PtInRect( pt ) )
		{
			ptrO->SetActiveChild( lposSave );
			return true;
		}
	}

	ptrO->GetFirstChildPosition( &lpos );
	while( lpos && lpos != lposActive )
	{
		POSITION	lposSave = lpos;
		IUnknown	*punk = 0;
		ptrO->GetNextChild( &lpos, &punk );
		if( !punk )continue;
		IRectObjectPtr	ptrRect( punk );
		punk->Release();

		if( ptrRect == 0 )continue;

		CRect	rect;
		ptrRect->GetRect( &rect );

		if( rect.PtInRect( pt ) )
		{
			ptrO->SetActiveChild( lposSave );
			return true;
		}
	}

	return true;
}

bool CActionEditArrow::DoTrack( CPoint pt )
{
	Invalidate();
	m_ptrArrow->SetCoords( m_pointWhere, pt );
	Invalidate();
	return true;
}

bool CActionEditArrow::DoFinishTracking( CPoint pt )
{
	Invalidate();
	m_ptrArrow->SetCoords( m_pointWhere, pt );
	Invalidate();
	Finalize();
	return true;
}

void CActionEditArrow::DoDraw( CDC &dc )
{
	if( m_ptrArrowRect == 0 )
		return;

	dc.SetROP2( R2_COPYPEN );
	m_ptrArrowDraw->Paint( dc, m_rcInvalid, m_punkDrawTarget, m_pbi, m_pdib );
}

void CActionEditArrow::Invalidate()
{
	if( m_ptrArrowRect == 0 )
		return;
	CRect	rect;
	m_ptrArrowRect->GetRect( &rect );
	rect = ::ConvertToWindow( m_punkTarget, rect );
	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
	if( pwnd->GetSafeHwnd() )pwnd->InvalidateRect( rect );
}

bool CActionEditArrow::DoUpdateSettings()
{

	m_dwArrowFlags = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Style", m_dwArrowFlags );
	m_nWidth = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Width", m_nWidth );
	m_nSize = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Size", m_nSize );


	IViewPtr	ptrV( m_punkTarget );
	IUnknown	*punkFM = 0;
	ptrV->GetMultiFrame( &punkFM, false );

	if( punkFM )
	{
		punkFM->Release();
		IMultiSelectionPtr	ptrFrame( punkFM );

		DWORD nCount = 0;
		ptrFrame->GetObjectsCount( &nCount );

		CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );

		for( int n = 0; n < nCount; n++ )
		{
			IUnknown	*punkObject = 0;
			ptrFrame->GetObjectByIdx( n, &punkObject );

			if( CheckInterface( punkObject, IID_IArrow ) )
			{
				CRect	rect1, rect2;
				IRectObjectPtr	ptrRect( punkObject );
				ptrRect->GetRect( &rect1 );

				IArrowPtr	ptrArrow( punkObject );
				ptrArrow->SetDrawFlags( m_dwArrowFlags );
				ptrArrow->SetSize( m_nSize, m_nWidth );
				ptrArrow->SetColor( ::GetValueColor( GetAppUnknown(), "\\Editor", "Fore", RGB( 255, 0, 0 ) ) );

				ptrRect->GetRect( &rect2 );

				rect1 = ::ConvertToWindow( m_punkTarget, rect1 );
				rect2 = ::ConvertToWindow( m_punkTarget, rect2 );
				if( pwnd->GetSafeHwnd() )pwnd->InvalidateRect( rect1 );
				if( pwnd->GetSafeHwnd() )pwnd->InvalidateRect( rect2 );
			}

			if( punkObject )
				punkObject->Release();
		}
	}

	if( m_ptrArrow != 0 )
	{
		m_ptrArrow->SetDrawFlags( m_dwArrowFlags );
		m_ptrArrow->SetSize( m_nSize, m_nWidth );
		Invalidate();
	}

	return true;
}

bool CActionEditArrow::IsAvaible()
{
	bool	bAvaible = false;
	if (CheckInterface( m_punkTarget, IID_IImageView ))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();	punk_img = 0;
			bAvaible = true;
		}
	}
	else if (CheckInterface( m_punkTarget, IID_ICarioView ))
		bAvaible = true;
	return bAvaible;
}





//////////////////////////////////////////////////////////////////////
//CActionEditMarker implementation
BEGIN_INTERFACE_MAP(CActionEditMarker, CInteractiveActionBase)
	INTERFACE_PART(CActionEditMarker, IID_IObjectAction, DrAction)
END_INTERFACE_MAP()
IMPLEMENT_UNKNOWN(CActionEditMarker, DrAction)

HRESULT CActionEditMarker::XDrAction::GetFirstObjectPosition(LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditMarker, DrAction);
	(*plpos) = 1;
	return S_OK;
}
HRESULT CActionEditMarker::XDrAction::GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditMarker, DrAction);
	*ppunkObject = pThis->m_ptrMarker;
	if( *ppunkObject )(*ppunkObject)->AddRef();
	(*plpos) = 0;
	return S_OK;
}

HRESULT CActionEditMarker::XDrAction::AddObject( IUnknown *punkObject )
{
	METHOD_PROLOGUE_EX(CActionEditMarker, DrAction);
	pThis->m_ptrMarker = punkObject;
	pThis->m_ptrMarkerDraw = punkObject;
	pThis->m_ptrMarkerRect = punkObject;
	pThis->m_state = asReady;
	return S_OK;
}

CActionEditMarker::CActionEditMarker()
{
	bool m_bStartEditMarker=false;
}

CActionEditMarker::~CActionEditMarker()
{
}

bool CActionEditMarker::Initialize()
{
	if( !CInteractiveActionBase::Initialize() )
		return false;

	int	x = GetArgumentInt( "X" );
	int	y = GetArgumentInt( "Y" );
	double l = GetArgumentDouble( "Length" );
	// vk
	int prec = GetArgumentInt( "Precision" );
	// end vk

	IUnknown	*punkDataObject = ::CreateTypedObject( szTypeMarker );
	if( !punkDataObject )
	{
		LeaveMode();
		return false;
	}
	m_ptrMarker = punkDataObject;
	m_ptrMarkerDraw = punkDataObject;
	m_ptrMarkerRect = punkDataObject;
	punkDataObject->Release();

	//init calibration
	{
		ICalibrPtr	ptrClbr = m_ptrMarkerRect;
		IUnknown	*punk = GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		// vk
		m_ptrImage = (IImagePtr)punk;
		// end vk
		double	f;
		GUID	g;
		if( ptrClbr != 0 && ::GetCalibration( punk, &f, &g ) )
			ptrClbr->SetCalibration( f, &g );

		if( punk )punk->Release();
	}

	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITMARK);

	// vk
	if (prec != 1)
	{
		m_ptrMarker->SetCoordsLenPrec( x, y, 0, prec );
	}
	// end vk
	
	// vk
	int w = 0;
	int h = 0;
	m_ptrImage->GetSize(&w, &h);
	m_ptrMarker->SetBorderMarker(w,h);
	double maxLength = 0;
	m_ptrMarker->GetMaxLength(w-x, &maxLength);
	m_nVertMargin = ::GetZoom( m_punkDrawTarget )*cyMarker*2;
	if  ( l > 0 )
	{
		if ( (l <= maxLength) && (y+m_nVertMargin < h) && (x > 0) && (y > 0) )
		{
			m_ptrMarker->SetCoordsLenPrec( x, y, 0, prec );
			m_ptrMarker->SetLength( l );
			m_ptrMarker->SetCoordsEnd(x,y,true);

			CRect	rect;
			m_ptrMarkerRect->GetRect( &rect );
			rect = ::ConvertToWindow( m_punkTarget, rect );
			CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
			pwnd->InvalidateRect( rect );

			Finalize();
		}
		else
		{
			AfxMessageBox( "The marker is too long to be displayed correctly.\nInteractive mode starts now.", MB_OK, 0 );
		}
		return true;
	}
	//if( l != 0 )
	//{
	//	// vk
	//	m_ptrMarker->SetCoordsLenPrec( x, y, 0, prec );
	//	//m_ptrMarker->SetCoordsLen( x, y, 0 );
	//	// end vk
	//	m_ptrMarker->SetLength( l );


	//	CRect	rect;

	//	m_ptrMarkerRect->GetRect( &rect );
	//	rect = ::ConvertToWindow( m_punkTarget, rect );
	//	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
	//	pwnd->InvalidateRect( rect );

	//	Finalize();
	//	return true;
	//}
	// end vk
	
	return true;
}

bool CActionEditMarker::Invoke()
{
	if( m_ptrMarker == 0  )
		return false;

	IViewPtr	ptrV( m_punkTarget );
	IUnknown	*punkDoc = 0;
	ptrV->GetDocument( &punkDoc );

	ASSERT( punkDoc );
	IUnknownPtr	ptrDoc = punkDoc;
	punkDoc->Release();


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szDrawingObjectList );
	if( !punk )
	{
		punk = ::CreateTypedObject( szDrawingObjectList );
		if( !punk  )
		{
			AfxMessageBox( IDS_ERR_CANTCREATEOBJECTLIST );
			return false;
		}

		IUnknown	*punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punkImage )
		{
			SetBaseObject( punk, punkImage, ptrDoc );
			punkImage->Release();
		}

		m_changes.SetToDocData( ptrDoc, punk );	

		//???activate
		_bstr_t	bstr( ::GetObjectKind( punk ) );
		IDataContextPtr	ptrC( m_punkTarget );
		ptrC->SetActiveObject( bstr, punk, 0 );
	}
	{
		INamedDataObject2Ptr	ptrN( (IUnknown*)m_ptrMarker );
		ptrN->SetParent( punk, 0 );
	}
	punk->Release();

	m_changes.SetToDocData( ptrDoc, m_ptrMarker );

	m_lTargetKey = ::GetObjectKey( ptrDoc );
	LeaveMode();

	return true;
}

//undo/redo
bool CActionEditMarker::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionEditMarker::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//extended UI
bool CActionEditMarker::IsAvaible()
{
	if( !CheckInterface( m_punkTarget, IID_IImageView ) )
		return false;
	IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
	if( punk_img )
	{
		punk_img->Release();	punk_img = 0;
		return true;
	}
	return false;
}

bool CActionEditMarker::IsChecked()
{
	return false;
}

bool CActionEditMarker::DoStartTracking( CPoint pt )
{
	// vk
	int w, h;
	m_ptrImage->GetSize( &w, &h );
	if ( pt.y+m_nVertMargin > h-1 ) return false;
	// end vk
	m_ptrMarker->SetCoordsLen( pt.x, pt.y, 0 );
	
	m_pointWhere = pt;

	m_bStartEditMarker=true;
	return true;
}

bool CActionEditMarker::DoTrack( CPoint pt )
{
	CRect	rectBefore, rectAfter;
	// vk
	int w, h;
	m_ptrImage->GetSize( &w, &h );
	::SetCursor( m_hcurActive );
	if ( pt.x > w )
	{
		::SetCursor( m_hcurStop );
		pt.x = w;
	}
	if ( pt.x < 0 )
	{
		::SetCursor( m_hcurStop );
		pt.x = 0;
	}
	if ( pt.y > h )
	{
		::SetCursor( m_hcurStop );
		pt.y = h;
	}
	if ( pt.y < 0 )
	{
		::SetCursor( m_hcurStop );
		pt.y = 0;
	}
	// end vk

	m_ptrMarkerRect->GetRect( &rectBefore ); 
	m_ptrMarker->SetCoordsLen( m_pointWhere.x, m_pointWhere.y, 
		sqrt(double(( pt.x-m_pointWhere.x )*( pt.x-m_pointWhere.x )+( pt.y-m_pointWhere.y )*( pt.y-m_pointWhere.y ))));
	m_ptrMarker->SetCoordsEnd(pt.x, pt.y, false);
	m_ptrMarkerRect->GetRect( &rectAfter ); 

	m_prevPoint=pt;
	if(m_bStartEditMarker)
		m_bStartEditMarker=false;

	CRect	rect;
	rect.UnionRect( rectBefore, rectAfter );
	//// vk
	//CRect	rcCaption;
	//m_ptrMarker->GetCaptionRect(&rcCaption);
	//rect.top = rcCaption.top;
	//rect.left = min( rect.left, rcCaption.left );
	//rect.right = max( rect.right, rcCaption.right );
	//// end vk
	rect = ::ConvertToWindow( m_punkTarget, rect );
	//// vk
	//rect.InflateRect(1,1,1,1);
	//// end vk
	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );

	// vk
	if ( pwnd->GetSafeHwnd() ) 
		pwnd->Invalidate();
	//if( pwnd->GetSafeHwnd() )pwnd->InvalidateRect( rect );
	// end vk

	return true;
}
bool CActionEditMarker::DoFinishTracking( CPoint pt )
{
	int w, h;
	m_ptrImage->GetSize( &w, &h );  
	if ( pt.x > w )
		pt.x = w;
	if ( pt.x < 0 )
		pt.x = 0;
	if ( pt.y > h )
		pt.y = h;
	if ( pt.y < 0 )
		pt.y = 0;
	CRect rectBefore;
	m_ptrMarkerRect->GetRect( &rectBefore ); 
	int res;
	if ( !m_bStartEditMarker)
	{
		if ( rectBefore.right > w )
		{
			res=rectBefore.right-w;
			pt.x =w-res-1;
		}
		if ( rectBefore.left < 0 )
		{
			res=rectBefore.left;
			pt.x = res+1;
		}
		if ( rectBefore.bottom > h )
		{
			res=rectBefore.bottom-h;
			pt.y =h-res-1;
		}
		if ( rectBefore.top < 0 )
		{
			res=rectBefore.top;
			pt.y = res+1;
		}
	}
	m_ptrMarker->SetCoordsLen( m_pointWhere.x, m_pointWhere.y, 
		sqrt(double(( pt.x-m_pointWhere.x )*( pt.x-m_pointWhere.x )+( pt.y-m_pointWhere.y )*( pt.y-m_pointWhere.y ))));
	m_ptrMarker->SetCoordsEnd(pt.x, pt.y, false);
	Finalize();
	return true;
}

void CActionEditMarker::DoDraw( CDC &dc )
{
	dc.SetROP2(R2_COPYPEN);
	m_ptrMarkerDraw->Paint( dc, m_rcInvalid, m_punkDrawTarget, m_pbi, m_pdib );
}

static void format3(char *szbuf, double val)
{
	if(val<0.01) sprintf(szbuf,"%1.3g",val);
	else if(val<0.1) sprintf(szbuf,"%1.4f",val);
	else if(val<1) sprintf(szbuf,"%1.3f",val);
	else if(val<10) sprintf(szbuf,"%1.2f",val);
	else if(val<100) sprintf(szbuf,"%1.1f",val);
	else if(val<1e7) sprintf(szbuf,"%3.0f",val);
	else sprintf(szbuf, "%1.3g",val);
	
	if( char *p = strchr(szbuf, '.') )  // нашли десятичную точку
	{
		do ++p; while (*p>='0' && *p<='9'); // нашли позицию за последней десятичной цифрой
		while(*--p == '0') strcpy(p, p+1); // сдвигаем хвост строки, чтобу убрать нуль - и так до упора
		if(*p == '.') strcpy(p, p+1); // если убили все цифры после точки - убираем и ее
	}
}

CString CMarker::CalcScale(CDC &dc, bool bPrint)
{
	double fVisibleScale, fPrintZoom;

	double fMeterPerPixel = m_fCalibr/m_fZoom;

	DisplayCalibration DispCal;
	double fScreenMetersPerPixel = 1/(DispCal.GetPixelPerMM()*1000);

	if(bPrint)
	{
		int nHorzSize = dc.GetDeviceCaps(HORZSIZE);
		int nHorzRes = dc.GetDeviceCaps(HORZRES);
		double fMeterPerPixelDC = nHorzSize*0.001/max(1,nHorzRes);
		fVisibleScale = fMeterPerPixel/fMeterPerPixelDC;
	}
	else
		fVisibleScale = fMeterPerPixel / fScreenMetersPerPixel;

	
	INamedPropBagPtr ptr_bag(m_punkDrawTarget);
	_variant_t var;
	if(ptr_bag)	
	{
		ptr_bag->GetProperty(_bstr_t("PrintPreview"),&var);
		if ( VT_I4 == var.vt && 1 == var.lVal )
		{
			_variant_t varZoom;
			ptr_bag->GetProperty(_bstr_t("PrintPreviewZoom"),&varZoom);
			if(VT_R8 == varZoom.vt && varZoom.dblVal>0)
				fVisibleScale *=(double)varZoom.dblVal;
		}
	}
	char szbuf[256];
	if(fVisibleScale>1)
	{
		format3(szbuf,fVisibleScale);
		strScale = CString("1 : ") + szbuf;
	}
	else
	{
		format3(szbuf,1/fVisibleScale);
		strScale = CString(szbuf) + "x";
	}

	return strScale;
}