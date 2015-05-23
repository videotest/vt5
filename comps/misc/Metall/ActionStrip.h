#pragma once
#include "action_filter.h"
#include "\vt5\common2\action_interactive.h"
#include "\vt5\awin\misc_ptr.h"
#include "image5.h"
#include "resource.h"
#include "measure5.h"

class CActionStrip :
	public CInteractiveAction,
	public CUndoneableActionImpl,
	public _dyncreate_t<CActionStrip>
{
public:
	CActionStrip(void);
	~CActionStrip(void);
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	virtual bool Initialize();
	virtual bool Finalize();

protected:
	_ptr_t2<double> m_StripTop;
	_ptr_t2<double> m_StripBottom;
	int m_nLeft;
	int m_nRight;
	int m_nStripSize;
	int m_nDrawingState; // 0 - не рисуем, 1 - рисуем Top, 2 - рисуем Bottom
	int m_nMinHeight;
	_point m_ptLast; // положение последней точки - для ведения мыши
	IImage4Ptr m_ptrImage; // Image, по которому работаем
	INamedDataObject2Ptr m_ptrList; // ObjectList, если есть
	IMeasureObjectPtr m_ptrObjectOld; // Объект, с которого брали данные на старте акции (в конце акции его грохаем, создавая новый)
	_gdi_t<HCURSOR> m_hCurActive;
protected:
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonUp( _point point );
	virtual bool DoMouseMove( _point point );

	virtual bool DoRButtonDown( _point point );
	virtual bool DoLButtonDblClick( _point point );

	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual void EnsureVisible( _point point );

	virtual bool IsAvaible();

	IUnknownPtr get_document();

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call GetActionState( DWORD *pdwState );

private:
	bool SetPoint(int x, double y);
	COLORREF m_clHilightColor;
	long m_nHilightPercent;
};

/////////////////////////////////////////////////////////////////////////////
class CActionStripInfo : public _ainfo_t<ID_ACTION_STRIP, _dyncreate_t<CActionStrip>::CreateObject, 0>,
							public _dyncreate_t<CActionStripInfo>
{
	route_unknown();
public:
	CActionStripInfo() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};
