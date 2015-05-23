#ifndef __wndmisc5_h__
#define __wndmisc5_h__

#include "defs.h"

interface ITranslatorWnd : public IUnknown
{
	com_call TranslateAccelerator( MSG* pMsg, BOOL* pbTranslate ) = 0;	
};

interface IScrollZoomView : public IUnknown
{
	com_call SetZoom( double fZoom, BOOL* pbWasProcess, double* pReturnZoom ) = 0;
	com_call SetScrollPos( POINT ptPos,	BOOL* pbWasProcess, POINT* pptReturnPos ) = 0;
	com_call SetClientSize( SIZE sizeNew, BOOL* pbWasProcess, SIZE* pReturnsizeNew ) = 0;	
};

interface IHwndHook: public IUnknown
{
	com_call EnterHook( HWND hWnd ) = 0;
	com_call LeaveHook( ) = 0;
};


declare_interface(ITranslatorWnd, "6AD19998-FC70-4246-9B83-611181715603");
declare_interface(IScrollZoomView, "345B7AEB-F015-4beb-B244-11188E63A03A");
declare_interface(IHwndHook, "DD0C5AD1-F9D7-4a38-B4CA-B45C16908371");

#endif //__wndmisc5_h__
