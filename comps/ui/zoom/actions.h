#ifndef __actions_h__
#define __actions_h__

//[ag]1. classes

class CZoomUndoable
{
public:
	CZoomUndoable() {}
	~CZoomUndoable() {}
	bool IsAvaible( IUnknown *punkTarget );
};

class CActionFit2Image : public CActionBase
{
	DECLARE_DYNCREATE(CActionFit2Image)
	GUARD_DECLARE_OLECREATE(CActionFit2Image)

public:
	CActionFit2Image();
	virtual ~CActionFit2Image();

public:
	virtual bool Invoke();
	virtual bool IsAvaible()
	{ 
		CZoomUndoable zoom; 
		return zoom.IsAvaible( m_punkTarget ); 
	}
};

class CActionFitDoc2Screen : public CActionBase
{
	DECLARE_DYNCREATE(CActionFitDoc2Screen)
	GUARD_DECLARE_OLECREATE(CActionFitDoc2Screen)

public:
	CActionFitDoc2Screen();
	virtual ~CActionFitDoc2Screen();

public:
	virtual bool Invoke();
	virtual bool IsAvaible() 
	{ 
		IDocumentSitePtr sptrD = m_punkTarget;

		if( sptrD )
		{
			IUnknown *punkV = 0;
			sptrD->GetActiveView( &punkV );

			if( punkV )
			{
				punkV->Release();

				IWindowPtr ptrWnd = punkV;
				DWORD dwFlags = 0;
				ptrWnd->GetWindowFlags( &dwFlags );

				CZoomUndoable zoom; 
				return bool( zoom.IsAvaible( punkV ) || dwFlags & wfSupportFitDoc2Scr ); 
			}
		}
		return true;
	}
};

class CActionZoom : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionZoom)
	GUARD_DECLARE_OLECREATE(CActionZoom)
public:
	CActionZoom();
	virtual ~CActionZoom();
public:
	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );

	virtual bool Invoke();
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool DoSetCursor( CPoint point );
	virtual bool IsAvaible()
	{ 
		CZoomUndoable zoom; 
		return zoom.IsAvaible( m_punkTarget ); 
	}
protected:
	void _CorrectRect();

protected:
	CRect	m_rect;
	double	m_fZoom;
	double	m_fAspect;
	CPoint	m_ptScrollPos;

	HCURSOR	m_hcurZoomIn, m_hcurZoomOut;
	bool	m_bUseFrame;
};

class CActionViewZoom11 : public CActionBase
{
	DECLARE_DYNCREATE(CActionViewZoom11)
	GUARD_DECLARE_OLECREATE(CActionViewZoom11)

public:
	virtual bool Invoke();
	virtual double GetZoom(){return 1.;}
	virtual bool IsAvaible();
};

class CActionViewZoom41 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom41)
	GUARD_DECLARE_OLECREATE(CActionViewZoom41)

public:
	virtual double GetZoom(){return 4.;}
};

class CActionViewZoom31 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom31)
	GUARD_DECLARE_OLECREATE(CActionViewZoom31)

public:
	virtual double GetZoom(){return 3.;}
};

class CActionViewZoom21 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom21)
	GUARD_DECLARE_OLECREATE(CActionViewZoom21)

public:
	virtual double GetZoom(){return 2.;}
};

class CActionViewZoom12 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom12)
	GUARD_DECLARE_OLECREATE(CActionViewZoom12)

public:
	virtual double GetZoom(){return 1./2.;}
};

class CActionViewZoom13 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom13)
	GUARD_DECLARE_OLECREATE(CActionViewZoom13)

public:
	virtual double GetZoom(){return 1./3.;}
};

class CActionViewZoom14 : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoom14)
	GUARD_DECLARE_OLECREATE(CActionViewZoom14)

public:
	virtual double GetZoom(){return 1./4.;}
};

class CActionViewZoomOut : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoomOut)
	GUARD_DECLARE_OLECREATE(CActionViewZoomOut)

public:
	CActionViewZoomOut();
	virtual ~CActionViewZoomOut();

public:
	virtual double GetZoom();
};

class CActionViewZoomIn : public CActionViewZoom11
{
	DECLARE_DYNCREATE(CActionViewZoomIn)
	GUARD_DECLARE_OLECREATE(CActionViewZoomIn)

public:
	CActionViewZoomIn();
	virtual ~CActionViewZoomIn();

public:
	virtual double GetZoom();
};

class CActionViewZoomFit : public CActionBase
{
	DECLARE_DYNCREATE(CActionViewZoomFit)
	GUARD_DECLARE_OLECREATE(CActionViewZoomFit)

public:
	virtual bool Invoke();
	virtual double GetZoom(){return -1.;}
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


#endif //__actions_h__
