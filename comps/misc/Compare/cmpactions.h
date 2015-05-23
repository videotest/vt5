#ifndef __cmpactions_h__
#define __cmpactions_h__

#include "\vt5\awin\misc_list.h"

//[ag]1. classes
#define MAX_POINTS	3

class CFilterArgImpl : public CImplBase
{
public:
	~CFilterArgImpl();

	BEGIN_INTERFACE_PART(Filter, IFilterAction)
		com_call GetFirstArgumentPosition(POSITION *pnPos);
		com_call GetNextArgument(IUnknown **ppunkDataObject, POSITION *pnPosNext);
		com_call GetArgumentInfo(POSITION lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut);
		com_call SetArgument(POSITION lPos, IUnknown *punkDataObject, BOOL bDataChanged);
		com_call InitArguments();
		com_call LockDataChanging( BOOL bLock );
		com_call SetPreviewMode( BOOL bEnter );
		com_call CanDeleteArgument( IUnknown *punk, BOOL *pbCan );
	END_INTERFACE_PART(Filter)

	void AddArgumentToFilter( IUnknown *punk, bool bOut )
	{m_args.AddTail( new Arg( punk, bOut ) );}

	struct Arg
	{
		Arg( IUnknown *p, bool b )
		{punk = p; bOut = b;}

		IUnknown	*punk;
		bool		bOut;
	};
	CPtrList	m_args;
};


class CActionAphineByPoints : public CInteractiveActionBase, 
		public CLongOperationImpl, 
		public CFilterArgImpl
{
	friend class COtherViewHandler;
	DECLARE_DYNCREATE(CActionAphineByPoints)
	GUARD_DECLARE_OLECREATE(CActionAphineByPoints)

	ENABLE_UNDO();
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
public:
	CActionAphineByPoints();
	virtual ~CActionAphineByPoints();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool DoUndo(){m_changes.DoUndo(m_punkTarget);return true;}
	virtual bool DoRedo(){m_changes.DoRedo(m_punkTarget);return true;}
	virtual void DoActivateObjects( IUnknown *punkObject )	{		Terminate();	}
	virtual DWORD GetActionState()							{	return CInteractiveActionBase::GetActionState()|afLeaveModeOnChangeTarget;	}

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual void _Draw( CDC *pdc = 0 );

	void DrawPoints( HDC hDC, IUnknown *punkTarget, bool bOther );
	void AddPoint( CPoint point, bool bOther );
	void Redraw( bool bOther );
protected:
	CPoint	m_points[MAX_POINTS], m_pointsOther[MAX_POINTS];
	int		m_nPointsCount, m_nPointsCountOther;
	IUnknownPtr	m_ptrOtherView;
	IUnknownPtr	m_ptrOtherHandler;

	IImage2Ptr	m_image;
	IImage2Ptr	m_ptrOtherImage;

	CObjectListUndoRecord	m_changes;
};

void free_contour( void *pcontour )
{ContourDelete( (Contour*)pcontour );}

class CActionCompareImage : public CInteractiveActionBase,
		public CLongOperationImpl, 
		public CFilterArgImpl
{
	DECLARE_DYNCREATE(CActionCompareImage)
	GUARD_DECLARE_OLECREATE(CActionCompareImage)

	ENABLE_UNDO();
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
public:
	CActionCompareImage();
	virtual ~CActionCompareImage();

public:
	virtual bool Initialize();
	virtual void Finalize();
	void PaintDIB( BITMAPINFOHEADER *pbiSrc, byte *pbytes, const RECT &rectInvalid );
	virtual bool DoUpdateSettings();
	virtual void DoActivateObjects( IUnknown *punkObject );

	virtual bool Invoke();
	virtual bool IsAvaible();

	virtual bool DoUndo();
	virtual bool DoRedo();

	virtual bool DoLButtonDown( CPoint point );
	virtual bool DoSetCursor( CPoint pt );
	virtual bool DoStartTracking( CPoint point );
	virtual bool DoTrack( CPoint point );
	virtual bool DoFinishTracking( CPoint point );
	virtual bool DoChar( UINT nChar );
	virtual bool DoVirtKey( UINT nVirtKey );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual bool Notify( int nPos );
protected:
	void ResizeRotate();
	void MakeBitmap();
	void MakeContours( bool bUpdateFrame = true );
	void DeleteBitmap();
	void InitDIBCache();
	void FreeDIBCache();
	void Invalidate( CRect rect = NORECT );
	void InvalidateIntersect( bool bUpdate = false );
	void InitBackground();

	void _InitCache( CImageWrp &image, BITMAPINFOHEADER **ppbi, byte **ppbyte, bool bInitMask );

	void _InstallDrawing( bool bInstall );
	CImageWrp ConvertImage(CImageWrp& image,CString cc);

	CRect		m_rectDIB;
	CRect		m_rectOther;

	//long		m_k1, m_k2, m_k3;		//(image1*k1+image2*k2+k3)/65535
	int		m_nOverlayMode;

	CImageWrp	m_image; //неподвижный
	CImageWrp	m_imageOther; //подвижный

	BITMAPINFOHEADER	*m_pbiIntersect;
	byte				*m_pdibIntersect;	//raw intersect bytes
	BITMAPINFOHEADER	*m_pbi1;
	BITMAPINFOHEADER	*m_pbi2;
	byte				*m_pdib1;			//raw source1 bytes
	byte				*m_pdib2;			//raw source2 bytes
	byte				*m_pmask;
	BITMAPINFOHEADER	*m_pbiNoRotate;
	byte				*m_pdibNoRotate;
	byte				*m_pmaskNoRotate;


	long		m_nCurrentWidth;
	long		m_nCurrentHeight;
	long		m_nCurrentXPos;
	long		m_nCurrentYPos;
	double		m_nCurrentAngle;
	double		m_nAngleStep;
	long		m_nWidth;
	long		m_nHeight;
	long		m_nXPos;
	long		m_nYPos;
	double		m_nAngle;
	long		m_bEnableScroll;
	CPoint		m_pointStart;

	CObjectListUndoRecord	
				m_changes;
	bool		m_bSingleImage;
	COLORREF	m_crBack;
	byte		m_r, m_g, m_b;
	bool	m_bManualTerminated;
	CResizingFrame	m_frame;
	_list_t<Contour*, free_contour>	m_contours;
	CResizingFrame::PointLocation	m_location;
	CRect		m_rectAll;
};

class CActionRotateImage : public CActionCompareImage
{
	DECLARE_DYNCREATE(CActionRotateImage)
	GUARD_DECLARE_OLECREATE(CActionRotateImage)

	ENABLE_UNDO();
public:
	CActionRotateImage();
	virtual ~CActionRotateImage();

	virtual bool Initialize();
	virtual bool Invoke();

	virtual bool IsRequiredInvokeOnTerminate(){return !m_bManualTerminated;}
	virtual bool DoVirtKey( UINT nVirtKey );

};



#endif //__cmpactions_h__
