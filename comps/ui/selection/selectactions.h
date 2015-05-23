#ifndef __selectactions_h__
#define __selectactions_h__


inline void __SmoothContour(CDC* pdc, Contour* pContour, long nPointsInSmooth)
{
	nPointsInSmooth += ((nPointsInSmooth & 1) == 1 ? 0 : 1);
	long nPointsCount = pContour->nContourSize;
	if(nPointsCount <= nPointsInSmooth)
		return;


	long fCurSumX = 0, fCurSumY = 0;
	Contour	*pNewContour = ::ContourCreate();

	int	nCount = 0;

	for(long i = -nPointsInSmooth; i <= nPointsInSmooth; i++)
	{
		int	nPos1 = i;
		while( nPos1 < 0 )
			nPos1 += nPointsCount;
		while( nPos1 > nPointsCount-1 )
			nPos1 -= nPointsCount;

		fCurSumX += pContour->ppoints[nPos1].x;
		fCurSumY += pContour->ppoints[nPos1].y;
		nCount++;
	}

	

	for(i = 0; i < nPointsCount; i++)
	{
		int nPos1 = i - nPointsInSmooth;
		while( nPos1 < 0 )
			nPos1 += nPointsCount;

		fCurSumX -= pContour->ppoints[nPos1].x;
		fCurSumY -= pContour->ppoints[nPos1].y;

		int nPos2 = i + nPointsInSmooth+1;
		while( nPos2 > nPointsCount-1 )
			nPos2 -= nPointsCount;

		fCurSumX += pContour->ppoints[nPos2].x;
		fCurSumY += pContour->ppoints[nPos2].y;

		::ContourAddPoint(pNewContour, 
			int(fCurSumX/(2*nPointsInSmooth+1)), int(fCurSumY/(2*nPointsInSmooth+1)), false);

	}
	
	pNewContour->lFlags = pContour->lFlags;

	if(pNewContour->nContourSize > nPointsInSmooth)
	{
		if( pContour->lFlags == cfInternal )
			pdc->SelectStockObject( BLACK_BRUSH );
		if( pContour->lFlags == cfExternal )
			pdc->SelectStockObject( WHITE_BRUSH );
		::ContourDraw( *pdc, pNewContour, 0, cdfFill|cdfClosed );
	}

	::ContourDelete( pNewContour );
}

class CActionSelectBase : public CInteractiveActionBase
{
	ENABLE_UNDO();
public:
	enum FrameDefMode
	{
		fdmNew,
		fdmAdd,
		fdmRemove
	};

	static FrameDefMode	s_defMode;
	static bool	        s_bMagick;	//magick stick action
	static bool	        s_bFreehand;//select area
protected:
	CActionSelectBase();
	virtual ~CActionSelectBase();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual bool Initialize();
	virtual bool DoSetCursor( CPoint point );
	virtual void Finalize();
	virtual bool IsCompatibleTarget();
	virtual void DoActivateObjects( IUnknown *punkObject )		{Terminate();}

	//virtual bool DoLButtonDown( CPoint pt );
	//virtual bool CanActivateObjectOnLbutton();


	void DrawPoint( CDC &dc, CPoint point );
protected:
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage ) = 0;

	CObjectListUndoRecord	m_changes;
	bool	m_bOldEnable;
	long	m_nOldSelectionType;
	CImageWrp	m_image, m_imageOld;
	CRect		m_rectImage;
	HCURSOR		m_hcurAdd, m_hcurSub;
	bool	m_bUseAddCursor;
};

class CActionSelectModeBase: public CActionBase
{
	ENABLE_SETTINGS();
public:
	CActionSelectModeBase();

	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual CActionSelectBase::FrameDefMode GetActionMode() = 0;
protected:
	bool	m_bSelect;
};
//[ag]1. classes

class CActionSelectAll : public CActionBase
{
	DECLARE_DYNCREATE(CActionSelectAll)
	GUARD_DECLARE_OLECREATE(CActionSelectAll)

	ENABLE_UNDO();

public:
	CActionSelectAll();
	virtual ~CActionSelectAll();

public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool IsAvaible();

	CObjectListUndoRecord	m_changes;
};

class CActionMakeVirtual : public CActionBase
{
	DECLARE_DYNCREATE(CActionMakeVirtual)
	GUARD_DECLARE_OLECREATE(CActionMakeVirtual)

	ENABLE_UNDO();

public:
	CActionMakeVirtual();
	virtual ~CActionMakeVirtual();

public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();

	CObjectListUndoRecord	m_changes;
};

class CActionSelectSmooth : public CActionBase
{
	DECLARE_DYNCREATE(CActionSelectSmooth)
	GUARD_DECLARE_OLECREATE(CActionSelectSmooth)

ENABLE_SETTINGS();
ENABLE_UNDO();
public:
	CActionSelectSmooth();
	virtual ~CActionSelectSmooth();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

	virtual bool IsAvaible();
protected:
	CObjectListUndoRecord	m_changes;
};

class CActionSelectInvert : public CActionBase
{
	DECLARE_DYNCREATE(CActionSelectInvert)
	GUARD_DECLARE_OLECREATE(CActionSelectInvert)

ENABLE_UNDO();
public:
	CActionSelectInvert();
	virtual ~CActionSelectInvert();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	
	virtual bool IsAvaible();
protected:
	CObjectListUndoRecord	m_changes;
};

class CActionSelectRemove : public CActionSelectModeBase
{
	DECLARE_DYNCREATE(CActionSelectRemove)
	GUARD_DECLARE_OLECREATE(CActionSelectRemove)
public:
	virtual CActionSelectBase::FrameDefMode GetActionMode()
	{	return CActionSelectBase::fdmRemove;	}
};

class CActionSelectAdd : public CActionSelectModeBase
{
	DECLARE_DYNCREATE(CActionSelectAdd)
	GUARD_DECLARE_OLECREATE(CActionSelectAdd)
public:
	virtual CActionSelectBase::FrameDefMode GetActionMode()
	{	return CActionSelectBase::fdmAdd;	}
};

class CActionSelectArea : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionSelectArea)
	GUARD_DECLARE_OLECREATE(CActionSelectArea)

public:
	CActionSelectArea();
	virtual ~CActionSelectArea();

	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );

	virtual bool Initialize();
	virtual void StoreCurrentExecuteParams();
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
protected:
	//implememtation
	CRect	CalcBoundsRect();
protected:
	CRect	m_rect;
	Contour	*m_pcontour;
	bool	m_bTracking;
};

class CRectCtrl;

class CActionSelectRect : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionSelectRect)
	GUARD_DECLARE_OLECREATE(CActionSelectRect)
public:
	CActionSelectRect();
	virtual ~CActionSelectRect();
public:
	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual bool Initialize();
	virtual void Finalize();

	virtual void StoreCurrentExecuteParams();
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
	virtual bool DoVirtKey( UINT nVirtKey );
	virtual bool DoSetCursor( CPoint pt );

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void CheckController();
protected:
	CPoint	m_pointStart;
	CRect	m_rect;
	CRectCtrl	*m_pctrl;
};

class CActionSelectEllipse : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionSelectEllipse)
	GUARD_DECLARE_OLECREATE(CActionSelectEllipse)
public:
	CActionSelectEllipse();
	virtual ~CActionSelectEllipse();

	virtual bool Initialize();

	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual void StoreCurrentExecuteParams();

	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
protected:
	CPoint	m_point1, m_point2;
	CPoint	m_pointCenter;
	long	m_nRadius;
};


class CActionMagickStick : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionMagickStick)
	GUARD_DECLARE_OLECREATE(CActionMagickStick)

	ENABLE_UNDO();

public:
	CActionMagickStick();
	virtual ~CActionMagickStick();

	virtual bool Initialize();

	virtual void DoDraw( CDC &dc );

	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoUpdateSettings();	
	IUnknown* GetResult( IUnknown* punkImage );
	void GetSettings();

protected:
	void CheckAddArea( int x, int y, int x1, int y1, int nNearX , int nNearY );
protected:
	CPoint	m_point;
	color	m_colorDelta;
	bool	m_bFillHoles;
//cache information
	int		m_cx, m_cy, m_colors;
	CPoint	*m_ppoints;
	byte	*m_pbyteMark;
	color	**m_prows;
	int		m_nPointCount;

	CRect	m_rect;

	bool	m_bDynamicBrightness;	
	
};

#define DECLARE_AVAIBLE() \
bool IsAvaible() \
{ \
	CChechAvailable avail; \
	return avail.IsAvaible( m_punkTarget ); \
} \

struct CChechAvailable
{
	bool IsAvaible( IUnknown *punkTarget );
};

class CActionFourierSelectCenter : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionFourierSelectCenter)
	GUARD_DECLARE_OLECREATE(CActionFourierSelectCenter)
public:
	CActionFourierSelectCenter();
	virtual ~CActionFourierSelectCenter();

public:
	virtual void DoDraw( CDC &dc );
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );

	virtual bool DoStartTracking( CPoint point );
	virtual bool DoFinishTracking( CPoint point );
	virtual bool DoTrack( CPoint point );

	DECLARE_AVAIBLE()

	CPoint	m_point;
};

class CActionFourierSelectFree : public CActionFourierSelectCenter
{
	DECLARE_DYNCREATE(CActionFourierSelectFree)
	GUARD_DECLARE_OLECREATE(CActionFourierSelectFree)

public:
	CActionFourierSelectFree();
	virtual ~CActionFourierSelectFree();

public:
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint point );
	virtual bool DoTrack( CPoint point );
	DECLARE_AVAIBLE()
protected:
	void _AddPoint( CPoint pt1 );
protected:
	Contour	*m_pcntr1;
	Contour	*m_pcntr2;
};

class CActionFourierSelectEllipse : public CActionFourierSelectCenter
{
	DECLARE_DYNCREATE(CActionFourierSelectEllipse)
	GUARD_DECLARE_OLECREATE(CActionFourierSelectEllipse)

public:
	CActionFourierSelectEllipse();
	virtual ~CActionFourierSelectEllipse();

public:
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint point );
	DECLARE_AVAIBLE()
protected:
	CPoint	m_pointCenter;
};

class CFixedSelectRectCtrl;
//fixed selection
class CActionSelectFixedRect : public CActionSelectBase
{
	DECLARE_DYNCREATE(CActionSelectFixedRect)
	GUARD_DECLARE_OLECREATE(CActionSelectFixedRect)

public:
	CActionSelectFixedRect();
	virtual ~CActionSelectFixedRect();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual void StoreCurrentExecuteParams();
	//virtual bool DoChangeSelects();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoUpdateSettings();
	void	UpdateCalibration();
	CRect	CalcPixelRect();
	CSize	CalcPixelSize(){return CalcPixelRect().Size();}
	virtual IUnknown *DoCreateSelectionImage( IUnknown *punkImage );
protected:
	double	m_left, m_top, m_width, m_height, m_calibr;
	bool	m_bCalibr;

	CFixedSelectRectCtrl	*m_pctrl;
};



#endif //__selectactions_h__
