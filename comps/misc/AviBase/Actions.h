// Actions.h: interface for the CActions class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __avi_actions_h__
#define __avi_actions_h__

#include "resource.h"
#include "action_main.h"
#include "action_undo.h"
#include "action_interactive.h"
#include "impl_long.h"

#include "nameconsts.h"
#include "\vt5\ifaces\avi_int.h"

#include "mmtimer.h"

#include "misc_utils.h"

static inline HRESULT GetAviPropPage( IUnknown **ppunkPropPage )
{
	if( !ppunkPropPage )
		return E_INVALIDARG;

    IPropertySheet	*pSheet = 0;
	pSheet = ::FindPropertySheet();
	if( !pSheet )
		return E_POINTER;					 

	IOptionsPage	*pPage = 0;
	pPage = ::FindPage( pSheet, clsidAviPlayPage, 0 );
	pSheet->Release( ); pSheet = 0;
    if( !pPage )	    
		return E_POINTER;

	*ppunkPropPage = (IUnknown*)(pPage);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
class CActionInsertAviObject : public CAction,
						public CUndoneableActionImpl,
						public _dyncreate_t<CActionInsertAviObject>
{
	route_unknown();
public:	

	com_call DoInvoke();	
};


/////////////////////////////////////////////////////////////////////////////
class CActionInsertAviObjectInfo : public _ainfo_t<IDS_INSERT_AVI_OBJECT, _dyncreate_t<CActionInsertAviObject>::CreateObject, 0>,
							public _dyncreate_t<CActionInsertAviObjectInfo>
{
	route_unknown();
public:

	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};



/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveFirst : public CAction,						
						public _dyncreate_t<CActionAviMoveFirst>
{
public:
	route_unknown();
public:	
	com_call DoInvoke();
	com_call GetActionState( DWORD* pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveFirstInfo : public _ainfo_t<IDS_AVI_MOVE_FIRST, _dyncreate_t<CActionAviMoveFirst>::CreateObject, 0>,
							public _dyncreate_t<CActionAviMoveFirstInfo>
{
	route_unknown();
public:	
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveNext : public CAction,						
						public _dyncreate_t<CActionAviMoveNext>
{
public:
	route_unknown();
public:	
	com_call DoInvoke();
	com_call GetActionState( DWORD* pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveNextInfo : public _ainfo_t<IDS_AVI_MOVE_NEXT, _dyncreate_t<CActionAviMoveNext>::CreateObject, 0>,
							public _dyncreate_t<CActionAviMoveNextInfo>
{
	route_unknown();
public:	
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveLast : public CAction,						
						public _dyncreate_t<CActionAviMoveLast>
{
public:
	route_unknown();
public:	
	com_call DoInvoke();
	com_call GetActionState( DWORD* pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveLastInfo : public _ainfo_t<IDS_AVI_MOVE_LAST, _dyncreate_t<CActionAviMoveLast>::CreateObject, 0>,
							public _dyncreate_t<CActionAviMoveLastInfo>
{
	route_unknown();
public:	
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMovePrev : public CAction,						
						public _dyncreate_t<CActionAviMovePrev>
{
public:
	route_unknown();
public:	
	com_call DoInvoke();
	com_call GetActionState( DWORD* pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMovePrevInfo : public _ainfo_t<IDS_AVI_MOVE_PREV, _dyncreate_t<CActionAviMovePrev>::CreateObject, 0>,
							public _dyncreate_t<CActionAviMovePrevInfo>
{
	route_unknown();
public:	
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveTo : public CAction,
						public _dyncreate_t<CActionAviMoveTo>
{
public:
	route_unknown();
public:	
	com_call DoInvoke();
	com_call GetActionState( DWORD* pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviMoveToInfo : public _ainfo_t<IDS_AVI_MOVE_TO, _dyncreate_t<CActionAviMoveTo>::CreateObject, 0>,
							public _dyncreate_t<CActionAviMoveToInfo>
{
	route_unknown();
public:	
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};



/////////////////////////////////////////////////////////////////////////////
class CActionAviPlay : public CInteractiveAction,
						public IInteractiveAviPlayAction,
						public _dyncreate_t<CActionAviPlay>
{
public:
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );
public:	
	CActionAviPlay();
	virtual ~CActionAviPlay();


	com_call AttachTarget( IUnknown *punkTarget );
	com_call SetArgument( BSTR bstrArgName, VARIANT *pvarVal );

	virtual bool Initialize();
	virtual bool Finalize();
	virtual bool DoLButtonDown( _point point );

	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	bool IsExpired(long lFrame);
	bool IsMessageProcessing();

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	bool		Paint( HDC hdc, RECT rect );
	com_call	DoInvoke();
	com_call	GetActionState( DWORD* pdwState );

	void		InvalidateAvi();
	void		InvalidateView();

	bool		Init( bool bUpdatePropPage );
	void		Deinit( bool bUpdatePropPage );

	long		m_lCurFrame;
	long		m_lPrevKeyFrame;
	long		m_lPrevFrame;

	long		m_lFrameCount;
	BYTE*		m_pDibBits;
	long		m_lBufSize;
	bool		m_bCanPlay;
	long		m_lTimerPeriod;
	double		m_fFramesPerSecond;
	
	SIZE		m_sizePrev;
	double		m_fZoomPrev;
	POINT       m_pointPrev;
	
	DWORD		m_dwStartTime;
	long		m_lStartFrame;
	bool		m_bMessageProcessing;
	
	IAviDibImagePtr		m_avi;
	CMMTimer			m_timer;

	bool				m_bPlay;

	HDRAWDIB			m_hDrawDib;


	//IInteractiveAviPlayAction
	com_call Play();
	com_call Pause();
	com_call Stop();
	com_call SetCurrentFrame( long lFrame );
	com_call GetCurrentFrame( long* plFrame );
	com_call IsPlay( long* pbPlay );

	com_call GetAviImage( IUnknown** punkAvi );

	com_call SetZoom( double fZoom );

	IAviPlayPropPagePtr	m_ptrAviPropPage;

	bool	ShowPage( bool bshow );


//for timer
	bool	m_bTimerFired;
public:
	bool	OnTimerSetFrame( long lframe );
	long	GetCurFrame();
	HWND	GetHwnd();
	long	GetTimerPeriod() {return m_lTimerPeriod;}
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviPlayInfo : public _ainfo_t<IDS_AVI_PLAY, _dyncreate_t<CActionAviPlay>::CreateObject, 0>,
							public _dyncreate_t<CActionAviPlayInfo>
{
	route_unknown();
public:	
	//arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "view site";}
	//static arg	s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
class CActionAviStop : public CAction,
						public _dyncreate_t<CActionAviStop>
{
public:
	route_unknown();
public:	
	com_call	DoInvoke();
	com_call	GetActionState( DWORD* pdwState );

	IUnknown*	get_interactive();
};


/////////////////////////////////////////////////////////////////////////////
class CActionAviStopInfo : public _ainfo_t<IDS_AVI_STOP, _dyncreate_t<CActionAviStop>::CreateObject, 0>,
							public _dyncreate_t<CActionAviStopInfo>
{
	route_unknown();
public:	
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CShowAviGallery : public CAction,
						public _dyncreate_t<CShowAviGallery>
{
public:
	route_unknown();
public:
	CShowAviGallery();
	virtual ~CShowAviGallery();

	com_call DoInvoke();
};


/////////////////////////////////////////////////////////////////////////////
class CShowAviGalleryInfo : public _ainfo_t<IDS_SHOW_AVI_GALLERY, _dyncreate_t<CShowAviGallery>::CreateObject, 0>,
							public _dyncreate_t<CShowAviGalleryInfo>
{
	route_unknown();
public:
	CShowAviGalleryInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame;}	
};

/////////////////////////////////////////////////////////////////////////////
class CAviSignature : public CAction, public CLongOperationImpl,
	public _dyncreate_t<CAviSignature>
{
public:
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	CAviSignature();
	virtual ~CAviSignature();

	com_call DoInvoke();

	
};


/////////////////////////////////////////////////////////////////////////////
class CAviSignatureInfo : public _ainfo_t<IDS_AVI_SIGNATURE, _dyncreate_t<CAviSignature>::CreateObject, 0>,
							public _dyncreate_t<CAviSignatureInfo>
{
	route_unknown();
public:
	CAviSignatureInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return "anydoc";}	
};


#endif //__avi_actions_h__
