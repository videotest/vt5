#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "action_interactive.h"
#include "trajectory_int.h"
#include "Named2DArrays.h"

class CTailEdit :
	public CInteractiveAction,
	public CUndoneableActionImpl,
	public CNamed2DArrays,
	public _dyncreate_t<CTailEdit>
{
public:
	CTailEdit ();
	virtual ~CTailEdit();
	route_unknown();
public:
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual bool IsAvaible();
	virtual IUnknown *DoGetInterface( const IID &iid );
	_gdi_t<HCURSOR> m_hCurActive;
	_gdi_t<HCURSOR> m_hCurShiftActive;
	_gdi_t<HCURSOR> m_hCurStop;
protected:
	virtual void DoActivateObjects( IUnknown *punkObject ) {TerminateInteractive();}
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonUp( _point point );
	virtual bool DoRButtonDown( _point point );
	virtual bool DoRButtonUp( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoMouseMove( _point point );
	virtual bool DoChar( int nChar, bool bKeyDown );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call GetActionState( DWORD *pdwState );

	//IUndoneableAction
	com_call Undo();
	com_call Redo();

	com_call TerminateInteractive();

private:
	IImage3Ptr m_ptrImage;
	INamedDataObject2Ptr m_ptrList;
	INamedDataObject2Ptr m_ptrObject;
	int m_nObjNum;
	int m_nFrame;

	_byte_ptr<int> m_xTotal; //координаты
	_byte_ptr<int> m_yTotal;
	_byte_ptr<byte> m_exist; //существует ли на соответствующем кадре
	_byte_ptr<float> m_pLineWidth; // ширина
	_byte_ptr<int> m_pLineIntensity;

	int m_nObjectStep;
	int m_nFrameStep;
	int m_nFrames;

	void UndoRedo(); // обменивает сохраненные значения с текущими
	byte *m_pOldData;
	int m_nOldMaxFrameNo;
	int m_nOldMaxObjectNo;
	int m_nOldMaxItemSize;
	bool m_bActionExecuted;

	IUnknownPtr	get_document();
};

/////////////////////////////////////////////////////////////////////////////
class CTailEditInfo :
	public _ainfo_t<ID_ACTION_TAILEDIT, _dyncreate_t<CTailEdit>::CreateObject, 0>,
	public _dyncreate_t<CTailEditInfo>
{
	route_unknown();
public:
	CTailEditInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};