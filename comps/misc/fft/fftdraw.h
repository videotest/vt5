#ifndef __fftdraw_h__
#define __fftdraw_h__

interface IFFTDrawController : public IUnknown
{
	com_call Init( IUnknown *punkDoc ) = 0;
	com_call DeInit() = 0;
};

class CFFTDrawController : public CCmdTargetEx,
									CDrawObjectImpl,
									CEventListenerImpl
{
	DECLARE_DYNCREATE(CFFTDrawController);
	GUARD_DECLARE_OLECREATE(CFFTDrawController);

	DECLARE_MESSAGE_MAP();
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
public:
	CFFTDrawController();
	~CFFTDrawController();

	virtual void OnFinalRelease();
public:

	BEGIN_INTERFACE_PART(FftDraw, IFFTDrawController)
		com_call Init( IUnknown *punkDoc );
		com_call DeInit();
	END_INTERFACE_PART(FftDraw);
	BEGIN_INTERFACE_PART(MsgList, IMsgListener)
		com_call OnMessage( MSG *pmsg, LRESULT *plReturn );
	END_INTERFACE_PART(MsgList)

	//{{AFX_MSG(CFFTDrawController)
	//}}AFX_MSG
protected:
	virtual IUnknown *GetTarget(){return 0;}


	virtual void DoDraw( CDC &dc );
	virtual bool DoMouseMove( CPoint pt, HWND hWnd );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

protected:
	void	_AddView( IUnknown *punkView );
	void	_RemoveView( IUnknown *punkView );
	void		_RedrawAll();
	IUnknown*	_GetViewFromUnknown( HWND hWnd );

	IUnknown	*m_punkDoc;
	CTypedPtrList<CPtrList, IUnknown *>	m_views;

	CPoint	m_pointFFTPos;
	CSize	m_sizeFFT;
	HWND	m_hwndLastSource;
};

declare_interface(IFFTDrawController, "6FEABCC5-6460-4fef-AF89-65FB1AF68126" );

#endif //__fftdraw_h__
