#pragma once

#include "action_filter.h"
#include "\vt5\common2\action_interactive.h"
#include "\vt5\awin\misc_ptr.h"
#include "image5.h"
#include "resource.h"
#include "measure5.h"

/////////////////////////////////////////////////////////////////////////////
class CPolyEdit :
	public CInteractiveAction,
	public CUndoneableActionImpl,
	public _dyncreate_t<CPolyEdit>
{
public:
	CPolyEdit(void);
	~CPolyEdit(void);
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	virtual bool Initialize();
	virtual bool Finalize();

protected:
	IImage4Ptr m_ptrImage; // Image, по которому работаем
	INamedDataObject2Ptr m_ptrList; // ObjectList, если есть
	_gdi_t<HCURSOR> m_hCurActive;
	_gdi_t<HCURSOR> m_hCurStop;
	_point m_Points[20]; // вершины многоугольника - максимум 20 штук
	int m_nPoints; // текущее количество вершин
	int m_nMaxPoints; // максимальное количество вершин - по его достижении акция выполняется
	bool m_bRegularPoly;
	COLORREF m_clLineColor1; // цвет линии - основной
	COLORREF m_clLineColor2; // цвет линии - текущий отрезок
	int m_nLineWidth; // толщина линии
protected:
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonUp( _point point );
	virtual bool DoMouseMove( _point point );

	//virtual bool DoRButtonDown( _point point );
	//virtual bool DoLButtonDblClick( _point point );

	//virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual void EnsureVisible( _point point );

	virtual bool IsAvaible();

	IUnknownPtr get_document();

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call GetActionState( DWORD *pdwState );
};

/////////////////////////////////////////////////////////////////////////////
class CPolyEditInfo : public _ainfo_t<ID_ACTION_POLY_EDIT, _dyncreate_t<CPolyEdit>::CreateObject, 0>,
							public _dyncreate_t<CPolyEditInfo>
{
	route_unknown();
public:
	CPolyEditInfo() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};
