#ifndef __marker_h__
#define __marker_h__

#include "editorint.h"
#include "calibrint.h"
#include "actions.h"


class CMarker : public CDataObjectBase,
				public CDrawObjectImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CMarker);
	GUARD_DECLARE_OLECREATE(CMarker);
	DECLARE_INTERFACE_MAP();

	ENABLE_MULTYINTERFACE();
public:
	CMarker();
	~CMarker();

	virtual void DoDraw( CDC &dc );
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual CString CalcScale(CDC &dc, bool bPrint);
	virtual void VerifyDataMarker();

	BEGIN_INTERFACE_PART(Mark, IMarker3)
		com_call SetCoordsLen( int x, int y, int nLength );
		com_call SetLength( double fLength );
		com_call SetCoordsLenPrec( int x, int y, int nLength, int nPrecision);
		com_call GetMaxLength( int nWidth, double *pnLength );
		com_call GetCaptionRect( CRect *prc );
		com_call SetCoordsEnd(int x, int y, bool bInit);
		com_call SetBorderMarker(int width, int height);
	END_INTERFACE_PART(Mark)
	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)
	BEGIN_INTERFACE_PART(Calibr, ICalibr)
		com_call GetCalibration( double *pfPixelPerMeter, GUID *pguidC );
		com_call SetCalibration( double fPixelPerMeter, GUID *pgudC );
	END_INTERFACE_PART(Calibr)

	int		m_x, m_y;
	int		m_xEnd, m_yEnd;
	double	m_fLength;
	int		m_nPixelLength;
	// vk
	int m_nPrecision;//, old_Precision;
	double m_fAdditionalWidth;
	//int m_nAdditionalWidth;
	int m_nFontHeight;//, old_FontHeight;
	double m_fZoom;
	// end vk
	CString	m_strText;
	double	m_fCalibr;
	GuidKey	m_guidC;

	int m_bFillRect;//, old_FillRect;
	COLORREF m_crFillColor;//, old_FillColor;
	COLORREF m_crTextColor;//, old_TextColor;
	COLORREF m_crMarkerColor;//, old_MarkerColor;
	
	CRect SelectRect;
	int m_countComma;//, old_countComma;
	CString strScale;
	int m_bScaleMarker;//, old_ScaleMarker;
	double m_fontZoom;
	bool m_bPreviewMode;
	int m_widthImage, m_heightImage;
	CString m_nameF;//, old_nameF;
	int m_resStyle;//, old_resStyle;
	CString m_strUnitName, m_initStrUnitName;
	//bool m_bLoadDataMarker;
};

class CArrow : public CDataObjectBase,
				public CDrawObjectImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CArrow);
	GUARD_DECLARE_OLECREATE(CArrow);
	DECLARE_INTERFACE_MAP();

	ENABLE_MULTYINTERFACE();
public:
	CArrow();
	~CArrow();

	virtual void DoDraw( CDC &dc );
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	BEGIN_INTERFACE_PART(Arrow, IArrow)
		com_call SetCoords( POINT pointStart, POINT pointEnd );
		com_call GetCoords( POINT *ppointStart, POINT *ppointEnd );
		com_call SetDrawFlags( DWORD dwFlags );
		com_call GetDrawFlags( DWORD *pdwFlags );
		com_call SetColor( COLORREF	cr );
		com_call GetColor( COLORREF *pcr );
		com_call SetSize( int nSize, int nWidth );
		com_call GetSize( int *pnSize, int *pnWidth );
	END_INTERFACE_PART(Arrow)
	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)


	CPoint	m_pointStart, m_pointEnd;
	DWORD	m_dwFlags;
	long	m_nArrowSize, m_nWidth;
	COLORREF	m_cr;
};

//[ag]1. classes

class CActionEditArrow : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionEditArrow)
	GUARD_DECLARE_OLECREATE(CActionEditArrow)

ENABLE_UNDO();
public:
	CActionEditArrow();
	virtual ~CActionEditArrow();

public:
	virtual bool Invoke();
	virtual bool Initialize();
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual void DoDraw( CDC &dc );
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool IsAvaible();

	virtual void Invalidate();
	virtual bool DoUpdateSettings();
protected:
	DWORD			m_dwArrowFlags;
	int				m_nWidth, m_nSize;

	IArrowPtr		m_ptrArrow;
	IDrawObjectPtr	m_ptrArrowDraw;
	IRectObjectPtr	m_ptrArrowRect;

protected:
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(DrAction, IObjectAction)
		com_call GetFirstObjectPosition(LONG_PTR *plpos);
		com_call GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos);
		com_call AddObject( IUnknown *punkObject );
	END_INTERFACE_PART(DrAction)
		
	CObjectListUndoRecord	m_changes;
	CPoint					m_pointWhere;
};

class CActionEditCircleArrow : public CActionEditArrow
{
	DECLARE_DYNCREATE(CActionEditCircleArrow)
	GUARD_DECLARE_OLECREATE(CActionEditCircleArrow)
public:
	CActionEditCircleArrow()
	{
		m_dwArrowFlags = afLastCircle;
	}
};

class CActionEditMarker : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionEditMarker)
	GUARD_DECLARE_OLECREATE(CActionEditMarker)

ENABLE_UNDO();
public:
	CActionEditMarker();
	virtual ~CActionEditMarker();

public:
	virtual bool Initialize();
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual void DoDraw( CDC &dc );
protected:
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(DrAction, IObjectAction)
		com_call GetFirstObjectPosition(LONG_PTR *plpos);
		com_call GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos);
		com_call AddObject( IUnknown *punkObject );
	END_INTERFACE_PART(DrAction)
protected:
	CObjectListUndoRecord	m_changes;
	CPoint					m_pointWhere;

	// vk
	IMarker3Ptr		m_ptrMarker;
	IImagePtr		m_ptrImage;
	int				m_nVertMargin;
	// end vk
	IDrawObjectPtr	m_ptrMarkerDraw;
	IRectObjectPtr	m_ptrMarkerRect;

	bool m_bStartEditMarker;
	CPoint m_prevPoint;
};


#endif //__marker_h__
