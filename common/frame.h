#ifndef __frame_h__
#define __frame_h__

#include "filebase.h"
#include "dragdrop.h"

/////////////////////////////////////////////////////////////////////////////////////////
//CResizingFrame
//resizing/rotating frame
class std_dll CResizingFrame
{  	
public:
	enum PointLocation
	{
		Left = 0, 
		TopLeft = 1,
		Top = 2,
		TopRight = 3,
		Right = 4,
		BottomRight = 5,
		Bottom = 6,
		BottomLeft = 7,
		None  = -1
	};
public:
	//construct the frame object
	CResizingFrame( CRect rect = NORECT, double fZoomX = 1, double fZoomY = 1, double fAngle = 0 );
	virtual ~CResizingFrame();				//deconstruct the frame object

	virtual void Init( IUnknown *punk, HWND hwnd );
public:
	//attributes
	double	GetAngle() const			{ return -m_fAngle;}
	double	GetZoomX() const			{ return m_fZoomX;}
	double	GetZoomY() const			{ return m_fZoomY;}
	CRect	GetRect() const				{ return m_rect;}
	DWORD	GetFlags() const			{ return m_dwFlags;}
	virtual void SetRect( CRect const rectSrc );
	virtual void SetAngle( double f )	{m_fAngle = f;m_bRotate = true;}
	virtual void SetZooms( double fx, double fy )	{m_fZoomX = fx; m_fZoomY = fy;}
public:
	//draw the frame in the specified device context
	virtual void	Draw( CDC& dc, CRect rectPaint = NORECT, BITMAPINFOHEADER *pbih = 0, byte *pdibBits = 0 );
	virtual void	Redraw( bool bOnlyPoints = false, bool bUpdate = false );
	virtual void	RecalcChanges( CPoint point, PointLocation location );
	virtual PointLocation	HitTest( CPoint point );
	
	virtual void	SetFlags( DWORD dwFlags );

	virtual CPoint	CalcPointLocation( PointLocation location, double &fAngle );
	virtual CRect	CalcBoundsRect();

	virtual void	ReLoadCursors();
	virtual bool	DoSetCursor( CPoint pt, PointLocation loc = None  );
protected:
	virtual void	_DrawPoint( CDC &dc, CPoint point, double a );	//draw the marker in the specified point
protected:
	CRect	m_rect;
	double	m_fZoomX, m_fZoomY;
	double	m_fAngle;	//in degree
	DWORD	m_dwFlags;
	CPoint	m_pointDelta;

	CRect	m_rectCurrent;
	bool	m_bRotate;
	HCURSOR			m_hcursors[8];
//view
	HWND		m_hwnd;
	IUnknown	*m_punkView;
public:
	bool	m_bDragDropActive;
};


class std_dll CViewBase;
class std_dll CFrame :	public CCmdTargetEx,
						public CMouseImpl,
						public CResizingFrame,
						public CSerializableObjectImpl
{
	friend class CDataSource;
	friend class CViewBase;
	DECLARE_DYNCREATE(CFrame);
	ENABLE_MULTYINTERFACE();
public:
	CFrame();
	~CFrame();

	CDataSource	m_dataSource;

public:
	virtual void SelectObject(IUnknown* punkObject, bool bAddToExist = true);
	virtual bool UnselectObject(IUnknown* punkObject);
	virtual int  GetObjectsCount();
	IUnknown*  GetObjectByIdx(int nIdx);
	virtual bool CheckObjectInSelection(IUnknown* punkObject);
	virtual bool CheckObjectInSelection( GuidKey lKey );
	
	virtual void	GetFrameOffset(CPoint& ptOffset);
	virtual void	SetFrameOffset(CPoint ptOffset);
	virtual void	GetObjectOffsetInFrame(IUnknown* punkObject, CPoint& ptOffset);
	virtual void	EmptyFrame();
	virtual CRect	GetBoundRect(){return m_rcBounds;};
	virtual void	SetBoundRect(CRect rcBound){m_rcBounds = rcBound;};
		

	virtual void Init(CViewBase* pView, bool bNotifyDeselect);
	virtual bool DoEnterMode(CPoint point);
	virtual bool CanStartTracking(CPoint pt);
	virtual IUnknown *GetTarget();

	virtual bool SerializeObject( CStreamEx& stream, SerializeParams *pparams );

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoSetCursor( CPoint pt );

	
	virtual void Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits );
	virtual void Redraw( bool bOnlyPoints = false, bool bUpdate = false );


	virtual void OnBeginDragDrop();
	virtual void OnEndDragDrop();

	bool	LockUpdate( bool bLock );

	virtual void UnselectObjectsWithSpecifiedParent( IUnknown *punkParent );	//remove all objects with specified parent



	///////////////////////////
	IUnknown	*GetActiveObject();
protected:
	BEGIN_INTERFACE_PART_EXPORT(Multi, IMultiSelection)
		com_call SelectObject(IUnknown* punkObj, BOOL bAddToExist);
		com_call UnselectObject(IUnknown* punkObj);
		com_call GetObjectsCount(DWORD* pnCount);
		com_call GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj);
		com_call GetFlags(DWORD* pnFlags);
		com_call SetFlags(DWORD nFlags);
		com_call GetFrameOffset(POINT* pptOffset);
		com_call SetFrameOffset(POINT ptOffset);
		com_call GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset);
		com_call RedrawFrame();
		com_call EmptyFrame();
		com_call SyncObjectsRect();
	END_INTERFACE_PART(Multi)
	

	DECLARE_INTERFACE_MAP();
protected:
	void _ClearSelectedObjectsArray();
	void _CalculateFrameBoundsRect();
	void _ManageObjectSelections(IUnknown* punk);
	void _select( IUnknown *punkObject );
	void _deselect( int idx );


	CTypedPtrArray<CPtrArray, IUnknown*> m_ptrSelectedObjects;
	CArray<GUID, GUID&>						m_ptrSelectedObjectsKeys;
	
	CDWordArray		m_arrObjectsOffsets; //regard from topleft coner of frame
	CRect			m_rcBounds;
	int				m_nID;
	CFrame::PointLocation	m_locationCurrent;
	bool			m_bLockUpdate;

	bool      m_bNotifyDeselect;

	CViewBase*		m_pView;
};

#endif //__frame_h__