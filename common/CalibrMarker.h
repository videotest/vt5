#pragma once

#include "\vt5\common2\misc_calibr.h"
#include "units_const.h"
#include "PropBag.h"
#include <math.h>

#define PrtScale(x) int((x)*fPrintZoom+0.5)
#define PrtScaleSize(x) int((x)*fPrintZoom*fPrintMarkerScale+0.5)

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

static bool _DrawCalibr( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits,
				 bool bPrint, double fPrintZoom, bool bPrintPreview,
				 double fZoom, double fCalibr, CPoint ptScroll, CSize szClient, CWnd* pwnd, CPoint ptOffset )
{
	CRect	rect;
	pwnd->GetClientRect( rect );
	rect.OffsetRect( ptOffset );

	double fPrintMarkerScale = 1;
	if(bPrint) 
		fPrintMarkerScale = ::GetValueDouble(::GetAppUnknown(), "ImageView", "PrintMarkerScale", 1.0);

	rect.left = PrtScale(rect.left);
	rect.right = PrtScale(rect.right);
	rect.top = PrtScale(rect.top);
	rect.bottom = PrtScale(rect.bottom);

	//get the image rectangle
	CSize	size_cur = szClient;

	if( size_cur.cx<50 ) return false; // не рисуем на слишком мелких

	int nDPI = dc.GetDeviceCaps(LOGPIXELSX);
	//double fMeterPerPixelDC = 0.0254/max(nDPI,1);
	int nHorzSize = dc.GetDeviceCaps(HORZSIZE);
	int nHorzRes = dc.GetDeviceCaps(HORZRES);
	double fMeterPerPixelDC = nHorzSize*0.001/max(1,nHorzRes);

	int nPhysicalWidth = dc.GetDeviceCaps(PHYSICALWIDTH);
	int nLogPixelsX = dc.GetDeviceCaps(LOGPIXELSX);
	int nScalingFactorX = dc.GetDeviceCaps(SCALINGFACTORX);

	double fMeterPerPixel = fCalibr/fZoom;

	double fUnitPerMeter = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );

	CString strType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	double fPixelPerUnit = 1/fUnitPerMeter/fMeterPerPixel;

	//double fScreenSize = 15.5 * 2.54e-2; // диагональ экрана в метрах
	//int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	//int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
	// double fScreenMetersPerPixel = fScreenSize / _hypot(cxScreen,cyScreen);
	DisplayCalibration DispCal;
	double fScreenMetersPerPixel = 1/(DispCal.GetPixelPerMM()*1000);

	double fVisibleScale = fMeterPerPixel / fScreenMetersPerPixel;
	if(bPrint)
	{
		fVisibleScale = fMeterPerPixel/fMeterPerPixelDC;
		if(bPrintPreview) fVisibleScale *= fPrintZoom;
	}

	int bi_w34=pbmih==0?0:(pbmih->biWidth*3+3)/4*4;

	CRect	rectImage;

	rectImage.left = int(ceil( 0*fZoom-ptScroll.x));
	rectImage.right = int(floor((size_cur.cx)*fZoom-ptScroll.x + .5));
	rectImage.top = int(ceil( 0*fZoom-ptScroll.y));
	rectImage.bottom = int(floor((size_cur.cy)*fZoom-ptScroll.y + .5));

	rectImage.OffsetRect( ptOffset );

	if(bPrint) rect = rectImage;
	else rect &= rectImage;

	if( rect.Width()<100 ) return false; // не рисуем на слишком мелких

	double fCalUnits=1;
	int lim=20;	// ограничим циклы - чтобы не было бесконечного
	while(fPixelPerUnit*fCalUnits > rect.Width()/2 && lim-->0) fCalUnits /= 10;
	while(fPixelPerUnit*fCalUnits < rect.Width()/2 && lim-->0) fCalUnits *= 10;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2.5;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2;

	int x1 = rect.right - PrtScaleSize(10);
	int y1 = rect.bottom - PrtScaleSize(16);
	int w = int(fPixelPerUnit*fCalUnits);
		//rect.Width() / 4;
	double dw=w/5.0;
	int h = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "MarkerHeight", 8 );
	h = PrtScaleSize(h);

	if(h<4) return false; // не рисуем на слишком мелких

	int x0 = x1-w, y0 = y1-h;

	rect.top = max(rect.top, rect.bottom-PrtScaleSize(30));

	COLORREF clBack = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerBackground", RGB(255,255,255));
	int backR = GetRValue(clBack);
	int backG = GetGValue(clBack);
	int backB = GetBValue(clBack);

	if(pdibBits!=0)
	{
		CRect rectHighlight = rect & rectClip;
		for(int y=rectHighlight.top; y<rectHighlight.bottom; y++)
		{
			int yy=y-rectClip.top;
			yy=pbmih->biHeight-1-yy;
			byte *pb = pdibBits + bi_w34*yy;
			for(int x=rectHighlight.left; x<rectHighlight.right; x++)
			{
				int xx=x-rectClip.left;
				(*pb) = (*pb+backR)/2; pb++;
				(*pb) = (*pb+backG)/2; pb++;
				(*pb) = (*pb+backB)/2; pb++;
			}
		}
	}

	COLORREF color = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerColor", RGB(0,0,0));	
	COLORREF color2 = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerColor2", RGB(255,255,255));	
	CPen	pen(PS_SOLID, 0, color);
	CBrush	brush(color);
	CBrush	brush2(color2);


	//dc.SelectStockObject( BLACK_PEN );
	CPen	*pOldPen = dc.SelectObject( &pen );
	//dc.SelectStockObject( WHITE_BRUSH );
	CBrush* pOldBrush = dc.SelectObject( &brush2 );
	dc.Rectangle(x0,y0,x1,y1);
	dc.Rectangle(x0,y0-(h*2+2)/3,x0+1,y1+(h*2+2)/3);
	dc.Rectangle(x1-1,y0-(h*2+2)/3,x1,y1+(h*2+2)/3);
	dc.SelectObject( brush );
	for(int i=0; i<5; i+=2)
	{
		dc.Rectangle(int(x0+dw*i+0.5),y0,int(x0+dw*i+dw+0.5),y1);
	}

	dc.SetBkMode( TRANSPARENT );
	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );

	strcpy( lf.lfFaceName, "Arial" );
	lf.lfCharSet = 1;
	double nHeight = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "MarkerFontHeight", 11 );
	lf.lfHeight = -PrtScaleSize(nHeight);
	lf.lfWeight = FW_BOLD;

	CFont	font;
	font.CreateFontIndirect( &lf );

	CFont *pOldFont = dc.SelectObject( &font );
	char szbuf[256];
	format3(szbuf,fCalUnits);

	dc.SetTextColor(color);
	CString str = CString(szbuf) + " " + strType;
	CRect rect1 = CRect(x0, rect.top+PrtScaleSize(15), x1, rect.bottom);
	CRect rect2 = rect1;
	dc.DrawText( str, rect2, DT_CALCRECT );
	if(rect2.Width() <= rect1.Width())
	{
		rect1.left = (rect1.left + rect1.right)/2 - rect2.Width()/2;

		if(bPrint)
		{
			rect2.right = rect1.left + rect2.Width() + rect2.Height()/2;
			rect2.left = rect1.left - rect2.Height()/2;

			CBrush brush;
			brush.CreateSolidBrush(0xFFFFFF);
			CBrush* pOldBrush = dc.SelectObject(&brush);

			CPen pen(PS_NULL, 1, 0xFFFFFF);
			CPen* pOldPen = dc.SelectObject(&pen);

			dc.RoundRect( rect2, CPoint(1, 1) );

			dc.SelectObject(pOldBrush);
			dc.SelectObject(pOldPen);
		}

		dc.DrawText( str, rect1, 0);
	}

	rect1 = CRect(rect.left+PrtScaleSize(8), rect.top+PrtScaleSize(8), x0-PrtScaleSize(5), rect.bottom);
	rect2 = rect1;

	if(fVisibleScale>1)
	{
		format3(szbuf,fVisibleScale);
		str = CString("1 : ") + szbuf;
	}
	else
	{
		format3(szbuf,1/fVisibleScale);
		str = CString(szbuf) + "x";
	}
	dc.DrawText( str, rect2, DT_CALCRECT );
	if(rect2.Width() <= rect1.Width())
	{
		if(bPrint)
		{
			rect2.right = rect1.left + rect2.Width() + rect2.Height()/2;
			rect2.left = rect1.left - rect2.Height()/2;

			CBrush brush;
			brush.CreateSolidBrush(0xFFFFFF);
			CBrush* pOldBrush = dc.SelectObject(&brush);

			CPen pen(PS_NULL, 1, 0xFFFFFF);
			CPen* pOldPen = dc.SelectObject(&pen);

			dc.RoundRect( rect2, CPoint(1,1) );

			dc.SelectObject(pOldBrush);
			dc.SelectObject(pOldPen);
		}

		dc.DrawText( str, rect1, 0);
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldFont);

	return true;
}

static bool _IsCalibrVisible(IUnknown *pView=0)
{
	int lShowCalibr=-1000;
	INamedPropBagPtr bag(pView);
	if(bag!=0)
	{
		_variant_t	var;
		bag->GetProperty(_bstr_t("ShowCalibr"), &var);
		if(var.vt != VT_EMPTY) lShowCalibr=var.lVal;
	}

	if(lShowCalibr==-1000)
	{
		lShowCalibr = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "ShowMarker", 0 );
		if(bag!=0) bag->SetProperty(_bstr_t("ShowCalibr"),_variant_t(long(lShowCalibr)));
	}
	return lShowCalibr!=0;
}
