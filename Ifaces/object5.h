#ifndef __object5_h__
#define __object5_h__

#include "defs.h"

interface IMouseDrawController : public IUnknown
{
	com_call DoDraw( HDC hDC, IUnknown *punkVS, POINT pointPos, BOOL bErase ) = 0;
};


interface IDrawObject : public IUnknown
{
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache ) = 0;
	com_call GetRect( RECT *prect ) = 0;
	com_call SetTargetWindow( IUnknown *punkTarget ) = 0;
	//com_call SetInvalidRect(RECT rect) = 0;
};

struct DRAWOPTIONS
{
	BYTE	byVersion;	// 0 for DRAWOPTIONS
	POINT	ptOffset;
	DWORD	dwFlags;	// For chromosomes ChromoAxisType in "Chromosome.h"
	POINT	ptScroll;	// if -1,-1 - use from ScrollZoomSite
	double	dZoom;		// if -1. - use from ScrollZoomSite
	double	dZoomDC;	// zoom of current device context
	DWORD	dwMask;		// mask	 
};

enum DrawOptionMasks
{
	domOffset	=	1 << 0,
    domFlags	=	1 << 1,
	domScroll	=	1 << 2,
	domZoom		=	1 << 3,
	domZoomDC	=	1 << 4,
};

interface IDrawObject2 : public IDrawObject
{
	com_call PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions) = 0;
};

interface IMoveObject : public IUnknown
{
	com_call GetOffset(POINT * ppt) = 0;
	com_call SetOffset(POINT pt) = 0;
	com_call SetTargetWindow(IUnknown * punkTarget) = 0;
};

interface IActiveObject : public IUnknown
{
	com_call Operation( POINT pt ) = 0;
	com_call SetCursor( POINT pt ) = 0;
};

/*interface INativeClipboardSupport : public IUnknown
{
	com_call CreateFromClipboard( IUnknwon *punkObject );
	com_call Can
};*/

interface IDataObjectList : public IUnknown
{
	com_call UpdateObject( IUnknown* punkObj ) = 0;
	com_call LockObjectUpdate( BOOL bLock ) = 0;
	com_call GetObjectUpdateLock( BOOL *pbLock ) = 0;
	com_call MoveObject(GUID keyFrom, BOOL bDirection) = 0; //TRUE - at side of head
};

interface IMouseController : public IUnknown
{
	com_call RegisterMouseCtrlTarget( IUnknown *punkView ) = 0;
	com_call UnRegisterMouseCtrlTarget( IUnknown *punkView ) = 0;
	com_call DispatchMouseToCtrl( IUnknown *punkView, POINT point ) = 0;
	com_call RepaintMouseCtrl( IUnknown *punkView, HDC hDC, BOOL bDraw ) = 0;
	com_call CheckMouseInWindow( IUnknown *punkView ) = 0;
	com_call InstallController( IUnknown *punkCtrl ) = 0;
	com_call UnInstallController( IUnknown *punkCtrl ) = 0;
};

interface IRectObject : public IUnknown
{
	com_call SetRect(RECT rc) = 0;
	com_call GetRect(RECT* prc) = 0;

	com_call HitTest( POINT	point, long *plHitTest ) = 0;
	com_call Move( POINT point ) = 0;
};

interface IClonableObject : public IUnknown
{
	com_call Clone(IUnknown** ppunkCloned) = 0;
};

interface INotifyObject : public IUnknown
{
	com_call NotifyCreate() = 0;
	com_call NotifyDestroy() = 0;
	com_call NotifyActivate( bool bActivate ) = 0;
	com_call NotifySelect( bool bSelect ) = 0;
};

interface IViewSubType : public IUnknown
{
	com_call GetViewSubType(unsigned long* pViewSubType)=0;
	com_call SetViewSubType(unsigned long ViewSubType)=0;
};

interface INotifyObject2 : public INotifyObject
{
	com_call NotifyDataListChanged(int nCode, IUnknown *punkHint) = 0;
};

interface ISmartSelector : public IUnknown
{
	com_call SmartSelect( bool bSelect, IUnknown *punkSelection ) = 0;
	// в переданном punkSelection (MultiFrame) селектит те объекты, которые нужны данному
};

declare_interface( IMoveObject, "02A4C874-16ED-41b4-B846-F125BDE6F9CE" );
declare_interface( IDrawObject, "E5971A02-77D1-11d3-A665-0090275995FE" );
declare_interface( IDrawObject2, "C0F03880-10B2-11D6-BA8D-0002B38ACE2C" );
declare_interface( IActiveObject, "9206A4A1-7A19-11d3-A666-0090275995FE" );
declare_interface( IDataObjectList, "AAAAFA41-9B79-11d3-A533-0000B493A187" );
declare_interface( IMouseDrawController, "AE77FDB1-A636-11d3-A6B3-0090275995FE" );
declare_interface( IMouseController, "1F22DD21-A7E8-11d3-A6B6-0090275995FE" );
declare_interface( IObjectBoundsRect, "0EF5EA81-C9CC-11d3-A087-0000B493A187" );
declare_interface( IRectObject, "43C95077-D613-4439-8A07-360D5B244115" );
declare_interface( IClonableObject, "B378A2F8-1021-4bf6-92A7-53D16B346A70" );
declare_interface( INotifyObject, "7AACD997-EFF4-4843-8760-39366D69DCF9" );
declare_interface( INotifyObject2, "0A6F8A4A-D08F-4266-82D5-2BC6AB95F8F9" );
declare_interface( ISmartSelector, "922BCA43-50D2-4247-99D6-21188D07643C" );
declare_interface( IViewSubType, "DCD75372-29DF-4c00-A0C6-3F59BA9D38AC" );


#endif //__object5_h__